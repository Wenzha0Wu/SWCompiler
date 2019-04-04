/*
 * TensorNode.h
 * Copyright (C) 2018 Hongkun Yu <staryhk@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TENSORNODE_H_
#define TENSORNODE_H_

#include "IRNode.h"
#include "tensor/tensor.h"
#include "SWDSL.h"
#include <sstream>

namespace swc {

template <typename Dtype>
class TensorNode : public IRNode
{
  
  public:
    TensorNode() : _tensor(NULL) {};
    explicit TensorNode(const char name[], IRNode *parent = nullptr) : IRNode(TENSOR_NODE, name, parent) {};
    explicit TensorNode(const char name[], Tensor<Dtype> *tensor, IRNode *parent = nullptr) : IRNode(TENSOR_NODE, name, parent), _tensor(tensor) {};
    explicit TensorNode(const char name[], const std::initializer_list<int> &shape, IRNode *parent = nullptr) : IRNode(TENSOR_NODE, name, parent){    
        _tensor = new Tensor<Dtype>(shape);
    }
    ~TensorNode(){};

    void destroy(){
        printf("free TensorNode:%s\n", name().c_str());
    };

    void setTensor(Tensor<Dtype>* tensor) {
      _tensor = tensor; 
    }

    Tensor<Dtype>* getTensor() {
      return _tensor;
    }

    std::vector<unsigned long> getDims() { return _tensor->getDims(); }
    TensorNode *clone() const;
    std::string toString() const;

  private:
    Tensor<Dtype>* _tensor; 
};

/// share tensor, that _tensor point to
template <typename Dtype>
TensorNode<Dtype>* TensorNode<Dtype>::clone() const {
    TensorNode<Dtype> *tn = new TensorNode<Dtype>((name()+"_cp").c_str());
    tn->setTensor(_tensor);
    return tn;
}

template <typename Dtype>
std::string TensorNode<Dtype>::toString() const{
    std::stringstream os;
    os << "TensorNode: " << name() << "\n"
        << "  tensorDim: " << _tensor->getNDim() << "\n  ";
   for(int i=0; i<_tensor->getNDim(); i++)
        os <<  _tensor->getDim(i) << " ";

    return os.str();
}
} //namespace swc


#endif /* !TENSORNODE_H_ */
