#include <iostream>

#include "SWC.h"

#define Dtype float

using namespace swc;
using namespace std;

int main()
{
    //============================
    // Example of 1 FC layer:
    //  T:data_0   T:weight_0
    //     \       /
    //      \     /
    //        O:FC_0
    //         |
    //      T:data_1    
    //         |
    //      O:Tanh_0
    //         |
    //      T:data_2
    //=============================

  TENSOR(Data_0, 1000 , 1000, 1000);
  TENSOR(Weight_0, 1000, 1000, 1000);

  OP(FC_0, MatrixMatrixFCOp);
  LINKUPPER(FC_0, Data_0, Weight_0);

  TENSOR(Data_1, 1000 , 1000);
  LINKUPPER(Data_1, FC_0);
  
  OP(Tanh_0, MatrixTanhOp);
  LINKUPPER(Tanh_0, Data_1);
  
  TENSOR(Data_2, 1000 , 1000);
  LINKUPPER(Data_2, Tanh_0);

  //define IR graph
  G(MLPLayer);
  GpT(MLPLayer, Data_0, Data_1, Data_2, Weight_0);
  GpO(MLPLayer, FC_0, Tanh_0);

  CHECKT(Data_0);
  CHECKT(Weight_0);
  CHECKO(FC_0);
  CHECKT(Data_1);
  CHECKO(Tanh_0);
  CHECKT(Data_2);
  CHECKG(MLPLayer);

  MLPLayer->findInOut();
  MLPLayer->updateTopology();
  MLPLayer->updateTopoNodeList();

  OpNode<Dtype>* FC = FC_0->clone();
  cout << FC->toString() << endl;
  TensorNode<Dtype>* Data = Data_0->clone();
  cout << Data->toString() << endl;

  // Optimizer<Dtype>* opt = new Optimizer<Dtype>(MLPLayer);
  // opt->runOptimizer();
  IRGraph<Dtype> * graph = MLPLayer->clone();
  graph->findInOut();
  graph->updateTopology();
  graph->updateTopoNodeList();
  dotGen(graph);

  // dotGen(MLPLayer);



  SWLOG_INFO << "this is LOG"<<endl;
  return 0;
}