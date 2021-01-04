/*
 * TensorNode.h
 * Copyright (C) 2018 Hongkun Yu <staryhk@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TENSORNODE_H_
#define TENSORNODE_H_

#include "IRNode.h"

#include <sstream>
#include <unordered_map>

#include "SWLOG.h"
#include "tensor/tensor.h"

namespace swc {

// Forward declaration
class TilingLabel;

class TensorNode : public IRNode {
  public:
    bool require_grad{true};
    TensorNode() : tensor_(NULL){};
    explicit TensorNode(std::string name, IRNode *parent = nullptr)
        : IRNode(TENSOR_NODE, name, parent) {}
    explicit TensorNode(std::string name, Tensor *tensor,
                        IRNode *parent = nullptr)
        : IRNode(TENSOR_NODE, name, parent), tensor_(tensor){};
    explicit TensorNode(std::string name,
                        const std::initializer_list<size_t> &shape,
                        IRNode *parent = nullptr,
                        DataType dtype = DataType::Float_t,
                        mem_layout_t layout = layout_default)
        : IRNode(TENSOR_NODE, name, parent) {
        tensor_ = new Tensor(shape, dtype, layout);
    }

    ~TensorNode() { destroy(); }

    void destroy();

    void setTensor(Tensor *tensor) { tensor_ = tensor; }
    Tensor *getTensor() { return tensor_; }
    void reset(std::initializer_list<size_t> shape, DataType dtype = DataType::Float_t, mem_layout_t layout = layout_default) {
      tensor_->reset(shape, dtype, layout);
    }

    void setTraining(int train) { tensor_->setTraining(train); }
    int getTraining() const { return tensor_->getTraining(); }

    DataType getDataType() { return tensor_->getDataType(); }
    std::vector<unsigned long> getDims() { return tensor_->getDims(); }
    size_t getNDim() { return tensor_->getNDim(); }
    TensorNode *clone() const;
    TensorNode *deepClone() const;
    std::string toString() const;

    // AutoDiff implementation in tensorNodes
    void autoDiff(IRGraph *graph,
                  std::unordered_map<IRNode *, IRNode *> &gradNodeMap,
                  void *methodParams, pass::METHOD_TYPE methodType);

    void checkValid();

    void setTilingLabel(TilingLabel *tilinglabel) {
        _tilingLabel = tilinglabel;
    }
    TilingLabel *getTilingLabel() { return _tilingLabel; }

    void setMemLayout(mem_layout_t layout) { tensor_->setMemLayout(layout); }
    mem_layout_t getMemLayout() const { return tensor_->getMemLayout(); }

  private:
    Tensor *tensor_{nullptr};

    TilingLabel *_tilingLabel{nullptr};
};

} // namespace swc
#endif /* !TENSORNODE_H_ */
