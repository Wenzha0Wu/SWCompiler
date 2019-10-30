/***********************************************
#
#      Filename: src/pass/ParallelLabelingPass.h
#
#        Author: whl - lethewang@yeah.net
#   Description: ---
#        Create: 2019-09-18 15:40:59
# Last Modified: 2019-09-18 15:40:59
***********************************************/
#ifndef _PARALLEL_LABELING_PASS_H
#define _PARALLEL_LABELING_PASS_H

#include "common.h"
#include "OptimizePass.h"
#include "graphIR/OpNode.h"
#include "graphIR/TensorNode.h"
#include "parallel/TilingLabel.h"
#include "parallel/parallelGen.h"
#include "parallel/SearchSpace.h"
#include <random>
#include <algorithm>
#include <ctime>

namespace swc {
namespace pass {
class ParallelLabelingPass;
}

class swc::pass::ParallelLabelingPass: public swc::pass::OptimizePass {
    using OptimizePass::_graph;
    mutable std::mt19937_64 rng{std::random_device{}()};
public:
    ParallelLabelingPass(IRGraph *graph): OptimizePass(graph) {
        srand(time(NULL));
    };
    ~ParallelLabelingPass() {};
    void runLabeling(int p) {

        std::vector<TensorNode * > topoTensorNodes;
        std::vector<OpNode *> topoOpNodes;

        for (int i = 0; i < _graph->topologyNum(); i++) {
            for (int j = 0; j < _graph->getNumInTopoLevel(i); j++) {
                IRNode * irnode = _graph->getNodeInTopo(i, j);
                if(irnode->nodeType() == TENSOR_NODE) {
                    SWLOG_DEBUG(4) << "tensornode : (" << i
                        << "," << j << ") " << irnode->name() << std::endl;
                    topoTensorNodes.push_back(dynamic_cast<TensorNode *>(irnode));
                } else if(irnode->nodeType() == OP_NODE) {
                    SWLOG_DEBUG(4) << "opnode: (" << i
                        << "," << j << ") " << irnode->name() << std::endl;
                    topoOpNodes.push_back(dynamic_cast<OpNode*>(irnode));
                }

            }
        }

        //init tiling label
        for(unsigned long  i = 0; i < topoTensorNodes.size(); ++i) {
            TensorNode * originNode = topoTensorNodes[i];
            TilingLabel * tlabel =  new TilingLabel();
            originNode->setTilingLabel(tlabel);
        }

        std::ostringstream oss;
        std::ostream *os = &std::cout; 

        *os<<topoOpNodes.size()<<std::endl;

        std::vector<int> identity;

        for(unsigned long i =0; i<topoOpNodes.size(); ++i){
            OpNode * originNode = topoOpNodes[i];
            SWLOG_DEBUG(10) << i << " get Candidate strategies for "<<originNode->name()
                << " : " << originNode->getOp()->getOpName()<<"\n";

            if(originNode->getOp()->getEinOp()== 0){
                *os << originNode->name()<<" can not be parallelized (einOp=0)\n";
                continue;
            }

            std::vector<std::vector<int> > strategies = ParallelGen::generateStgy(originNode);
            int strategy_size = strategies.size();
            if(strategy_size == 0){
                *os << originNode->name()<<" get 0 strategies\n";
                continue;
            }

            for(auto sgy : strategies){
                for(auto dim: sgy)
                    *os<<dim<<" ";
                *os<<"\n";
            }

            std::vector<std::vector<int>> legal_strategies; // first available strategy

            // channel % degree may not be zero
            // finalstrategy = strategies[0];
            //int nInputs = originNode->getOp()->getnInput();
            //int nOutputs = originNode->getOp()->getnOutput();
            int nInputs = originNode->parentNum();
            int nOutputs = originNode->childNum();
            for(auto strategy : strategies) {
                bool legal = true;
                int idx = 0;
                for(auto tensor_dim: strategy) {
                    
                    Tensor* tensor;
                    if(idx < nInputs) {
                        tensor = ((TensorNode*)originNode->getParentNode(idx))->getTensor();
                    } else if(idx < (nInputs+nOutputs)) {
                        tensor = ((TensorNode*)originNode->getChildNode(idx-nInputs))->getTensor();
                    } else {
                        legal = false;
                        break;
                    }

                    if(tensor_dim >= 0) {
                        if(tensor->getDim(tensor_dim) % p) {
                            legal = false;
                            break;
                        }
                    }
                    idx++;
                } // for parallel dim in this strategy
                
                if(legal)
                    legal_strategies.push_back(strategy);
            }

            if(legal_strategies.size() > 0) {
                std::uniform_int_distribution<size_t> dist(0, legal_strategies.size()-1);
                int random_s_idx  = dist(rng);
                // int random_s_idx  = 0;
                auto best = legal_strategies[random_s_idx];

                if(_graph->getConfig().force_data_parallel) {
                    best = ParallelGen::generateDataParStgy(originNode); 
                    random_s_idx = 0;
                    for(auto &stgy :legal_strategies) {
                        if(stgy == best)
                            break;
                        random_s_idx++;
                    }
                }

                *os << "-----legal strategies------\n";
                for(auto sgy : legal_strategies){
                    for(auto s: sgy)
                        *os << s <<" ";
                    *os<<"\n";
                }

                *os << "-----selected strategy no." << random_s_idx << "------\n";
                identity.push_back(random_s_idx);
                for(auto s : best)
                    *os << s << " ";
                *os << "\n";

                StrategyLabel* slabel =  new StrategyLabel(best);
                originNode->setStrategyLabel(slabel);
            } else {
                *os << "-----no legal strategy------\n";
            }

        } // for topoOpNodes 

        for(auto i :identity)
         *os << i <<" "; 
        *os << "\n";

    }

    void runHandCraftLabeling() {
        std::vector<TensorNode * > topoTensorNodes;
        std::vector<OpNode *> topoOpNodes;

        for (int i = 0; i < _graph->topologyNum(); i++) {
            for (int j = 0; j < _graph->getNumInTopoLevel(i); j++) {
                IRNode * irnode = _graph->getNodeInTopo(i, j);
                if(irnode->nodeType() == TENSOR_NODE) {
                    topoTensorNodes.push_back(dynamic_cast<TensorNode *>(irnode));
                } else if(irnode->nodeType() == OP_NODE) {
                    topoOpNodes.push_back(dynamic_cast<OpNode*>(irnode));
                }

            }
        }

        for(unsigned long  i = 0; i < topoTensorNodes.size(); ++i) {
            TensorNode * originNode = topoTensorNodes[i];
            TilingLabel * tlabel =  new TilingLabel();
            originNode->setTilingLabel(tlabel);
        }

        std::ostringstream oss;
        std::ostream *os = &oss; 

        *os<<topoOpNodes.size()<<std::endl;

        StrategySearchSpace *sss = new StrategySearchSpace(_graph);

        for(unsigned long i =0; i<topoOpNodes.size(); ++i){
            OpNode * opNode = topoOpNodes[i];

            if(opNode->getOp()->getEinOp()== 0){
                *os << opNode->name()<<" can not be parallelized (einOp=0)\n";
                continue;
            }
            
            sss->addOpStrategyIfExist(opNode);
        }

        // op num in space may not be euqal to topoOpNodes.size() since we skip nodes whose einOp=0
        sss->printStrategySpace();

        std::vector<int> dp_seed(sss->getOpNum(), 0);

        /* for lenet, some different 
        mind that this will not get DP, because we cann not describe 
        do not parallelize SGD when search strategy now (possible fix is add this to strategy)
        c: channel of w
        */
        // auto lenet_conv0_c = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        // auto lenet_conv0_c_conv0Grad_c = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
        // auto lenet_conv1_c_conv1Grad_c = {0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0};
        

        //vgg b32_p8
        // auto vgg_b32_p8 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 2, 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        //vgg b32_p4
    //    auto vgg_b32_p4 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 2, 1, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 2, 1, 1, 2, 1, 0, 1, 1, 1, 2, 1, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        
        auto selected = dp_seed;
        assert(selected.size() == sss->getOpNum() && "illegal handcraft strategy, please check");
        sss->addStrategyToGraph(selected);
    }

    void runOptimizedLabeling() {

        std::vector<TensorNode * > topoTensorNodes;
        std::vector<OpNode *> topoOpNodes;

        for (int i = 0; i < _graph->topologyNum(); i++) {
            for (int j = 0; j < _graph->getNumInTopoLevel(i); j++) {
                IRNode * irnode = _graph->getNodeInTopo(i, j);
                if(irnode->nodeType() == TENSOR_NODE) {
                    SWLOG_DEBUG(4) << "tensornode : (" << i
                        << "," << j << ") " << irnode->name() << std::endl;
                    topoTensorNodes.push_back(dynamic_cast<TensorNode *>(irnode));
                } else if(irnode->nodeType() == OP_NODE) {
                    SWLOG_DEBUG(4) << "opnode: (" << i
                        << "," << j << ") " << irnode->name() << std::endl;
                    topoOpNodes.push_back(dynamic_cast<OpNode*>(irnode));
                }

            }
        }
        //init tiling label
        for(unsigned long  i = 0; i < topoTensorNodes.size(); ++i) {
            TensorNode * originNode = topoTensorNodes[i];
            TilingLabel * tlabel =  new TilingLabel();
            originNode->setTilingLabel(tlabel);
        }

        std::ostringstream oss;
        std::ostream *os = &oss; 

        *os<<topoOpNodes.size()<<std::endl;

        StrategySearchSpace *sss = new StrategySearchSpace(_graph);

        for(unsigned long i =0; i<topoOpNodes.size(); ++i){
            OpNode * opNode = topoOpNodes[i];

            if(opNode->getOp()->getEinOp()== 0){
                *os << opNode->name()<<" can not be parallelized (einOp=0)\n";
                continue;
            }
            
            sss->addOpStrategyIfExist(opNode);
        }

        sss->printStrategySpace();
        
        std::vector<std::vector<int>> udef;

        std::vector<int> dp_seed(sss->getOpNum(), 0);
        udef.push_back(dp_seed);


        // for lenet, some different 
        std::vector<int> lenet_init {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0};
        
        GeneticSearch ga(sss->getGeneSpace(),
            udef, /*specified identities*/
            500, /*populationSize*/
            0.5, /*crossOverRate*/
            0.1, /*mutationRate*/
            20, /*numberElites*/
            100, /*numGenerations*/
            sss /*StrategySearchSpace*/
            );
        
        ga.run();

        std::vector<int> best = ga.getBestIdentity();

        sss->addStrategyToGraph(best);
    }

    void run() {
        SWLOG_DEBUG(4) << "Start Paralleling Pass." << std::endl;

        auto config = _graph->getConfig();

        auto parallel_degree = config.mpi_size;
        assert(parallel_degree>1 && "error, degree of parallellism unset, please set config.mpi_size");

        if(config.geneticalgo_opt_parallel) {
            runOptimizedLabeling();
        }else if(config.handcraft_parallel) {
            runHandCraftLabeling();
        }else {
            runLabeling(parallel_degree);
        }

        //get strategy
        SWLOG_DEBUG(4) << "Finish Paralleling pass. " << std::endl;

    }



};

}
#endif
