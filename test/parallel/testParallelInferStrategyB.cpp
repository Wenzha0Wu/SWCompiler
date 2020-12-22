#include <iostream>

#include "SWC.h"

using namespace swc;
using namespace swc::op;
using namespace swc::pass;
using namespace std;

int main() {
    //============================
    // Example of 2-layer
    // fully connected network:
    //
    //  T:data0   T:weight0
    //     \       /
    //      \     /
    //        O:fc0 -- T:bias0
    //         |
    //      T:data1
    //         |
    //      O:tanh0
    //         |
    //      T:data2
    //                  T:weight1
    //          \       /
    //           \     /
    //          O:fc1 -- T:bias1
    //              |
    //          T:data3
    //              |
    //          O: softmax
    //              |
    //          T:data4
    //              |
    //          O: argmax_o
    //              |
    //          T: top3_t
    //              |
    //          O: print_o
    //              |
    //          T: useless
    //=============================

    TENSOR(data0, 8, 784);
    TENSOR(weight0, 784, 512);
    TENSOR(bias0, 512);
    data0_Tensor->setTensorInit(TensorInitType::FILE,
                                "input/mnist_images_8.bin");
    weight0_Tensor->setTensorInit(TensorInitType::FILE,
                                  "input/mlp_weight0.bin");
    bias0_Tensor->setTensorInit(TensorInitType::FILE, "input/mlp_bias0.bin");

    OP(fc0, MatrixMatrixFCBiasOp);
    LINKUPPER(fc0, data0, weight0, bias0);
    TENSOR(data1, 8, 512);
    LINKUPPER(data1, fc0);

    OP(tanh0, MatrixTanhOp);
    LINKUPPER(tanh0, data1);

    TENSOR(data2, 8, 512);
    LINKUPPER(data2, tanh0);

    TENSOR(weight1, 512, 10);
    TENSOR(bias1, 10);
    weight1_Tensor->setTensorInit(TensorInitType::FILE,
                                  "input/mlp_weight1.bin");
    bias1_Tensor->setTensorInit(TensorInitType::FILE, "input/mlp_bias1.bin");

    OP(fc1, MatrixMatrixFCBiasOp);
    LINKUPPER(fc1, data2, weight1, bias1);

    TENSOR(data3, 8, 10);
    LINKUPPER(data3, fc1);

    // Tensor *labelt = new Tensor({8}, DataType::Int32_t);
    // TensorNode *labeln = new TensorNode("selected", labelt);

    OP(softmax, MatrixSoftmaxOp);
    LINKUPPER(softmax, data3);

    TENSOR(data4, 8, 10);
    LINKUPPER(data4, softmax);

    auto *argmax_o = new OpNode("argmax", new ArgMaxOp(3));
    argmax_o->exlinkUpperNode(data4);
    auto *top3_t =
        new TensorNode("top3", new Tensor({8, 3}, DataType::Int32_t), argmax_o);
    auto *print_o = new OpNode("print", new DebugOp());
    print_o->exlinkUpperNode(top3_t);

    TENSOR(placeholder, 0);
    LINKUPPER(placeholder, print_o);

    // define IR graph
    G(mlp);
    GpT(mlp, data0, weight0, bias0, data1, data2, weight1, bias1, data3, data4,
        top3_t, placeholder);
    GpO(mlp, fc0, tanh0, fc1, softmax, argmax_o, print_o);

    //====================================================
    mlp->findInOut();
    mlp->updateTopology();

    Config config;
    config.mpi = true;
    config.mpi_size = 2;
    mlp->setConfig(config);

    pass::Optimizer *opt = new pass::Optimizer(mlp);
    opt->runOptimizer();

    // manully label parallel strategy for opnodes
    // remind that lowered nodes cannot simply take
    // the same strategyLabel

    // fc0->setStrategyLabel(new StrategyLabel({0, -1, -1, 0}));
    auto *fc0_mm = (OpNode *)mlp->getNodeByName("fc0_mm");
    auto *fc0_add = (OpNode *)mlp->getNodeByName("fc0_add");
    if (fc0_mm == NULL || fc0_add == NULL) {
        SWLOG_ERROR << "cannot find lowered nodes for fcbias\n";
        exit(1);
    }
    fc0_mm->setStrategyLabel(new StrategyLabel({1, 0, -2}));
    // let fc0_add be parallel will add bias twice after reduce
    // uncommont this will get 0 0 4 1 9 2 1 3
    // fc0_add->setStrategyLabel(new StrategyLabel({-2, -1, -2}));

    // let tanh0 be nonparallel will add reduceOp
    // if tanh0 parallel -2, than we need transform -2->i
    // tanh0->setStrategyLabel(new StrategyLabel({-2, -2}));

    // fc1->setStrategyLabel(new StrategyLabel({0, -1, -1, 0}));
    auto *fc1_mm = (OpNode *)mlp->getNodeByName("fc1_mm");
    auto *fc1_add = (OpNode *)mlp->getNodeByName("fc1_add");
    if (fc1_mm == NULL || fc1_add == NULL) {
        SWLOG_ERROR << "cannot find lowered nodes for fcbias\n";
        exit(1);
    }
    fc1_mm->setStrategyLabel(new StrategyLabel({1, 0, -2}));
    fc1_add->setStrategyLabel(new StrategyLabel({-2, -1, -2}));

    /*
    softmax->setStrategyLabel(new StrategyLabel({1, 1}));
    argmax_o->setStrategyLabel(new StrategyLabel({0, 0}));
    print_o->setStrategyLabel(new StrategyLabel({-1}));
    */

    PassManager passManager;
    passManager.add(new ParallelLabelingPass(mlp));
    passManager.add(new ParallelLoweringPass(mlp));

    // !!! is a must for EliminationPass, or all nodes
    // will be eliminated.
    // SETOUT(mlp, data4);
    // SETOUT(mlp, top3_t);
    SETOUT(mlp, placeholder);
    passManager.add(new EliminationPass(mlp));
    passManager.add(new RenamingNodePass(mlp));
    passManager.run();

    //====================================================
    dotGen(mlp);

    //====================================================
    codegen::ParallelCodegen *cg = new codegen::ParallelCodegen(mlp, config);
    string code = cg->generate();
    cout << code;

    return 0;
}
