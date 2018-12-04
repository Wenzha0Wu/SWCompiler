/*************************************************************************
	> File Name: tensor.h
	> Author: cryinlaugh
	> Mail: cryinlaugh@gmail.com
	> Created Time: Tues. 12/ 4 15:53:19 2018
 ************************************************************************/

#ifndef _TENSOR_H
#define _TENSOR_H

#include <memory>
#include <vector>
#include "../common.h"

namespace swc{

typedef enum TensorType{
    D2,
    D1,
    D0,
    UNKNOWN
};

class TensorShape{
private:
    int _ndim;
    std::shared_ptr<std::vector<unsigned long> > _shape;
public:
    TensorShape(unsigned ndim, std::shared_ptr<std::vector<unsigned> > shape);
    ~TensorShape(){};
    const int getNDim() const;
    const unsigned long getDim(int idx) const;
};


template <typename Dtype>
class Tensor{
private:
    TensorType _type;
    std::shared_ptr<TensorShape> _shape;
    std::shared_ptr<SWMem<Dtype> > _data;

public:
    Tensor();
    Tensor(TensorType t, std::shared_ptr<TensorShape> shape, std::shared_ptr<SWMem<Dtype> > tdata);
    ~Tensor(){}; 

    const int getNDim() const;
    const unsigned long getDim(int dim) const;
};

}

#endif
