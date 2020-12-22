/*************************************************************************
	> File Name: basicOps.h
	> Author: cryinlaugh
	> Mail: cryinlaugh@gmail.com
	> Created Time: Tue 04 Dec 2018 12:05:58 PM UTC
 ************************************************************************/

#ifndef _BASICOPS_H
#define _BASICOPS_H

#include "op/Op.h"
namespace swc {
namespace op {

//=====================================================
// Definition of 2-D basic operations.
// Version v0.1: basic ops for simple-MLP-nobias-fw listed below
//--Math Operations:
//----MatrixMatrixMul
//----VectorMatrixMul
//----MatrixVectorMul
//----MatrixVectorAdd
//=====================================================

class MatrixMatrixMulOp : public Op {
  public:
    MatrixMatrixMulOp() : Op(BASIC_OP, 2, 1, std::string("MatrixMatrixMul")) {
        this->_inputNDims.push_back(2);
        this->_inputNDims.push_back(2);
        this->_outputNDims.push_back(2);
        this->_einOp = 1;
        this->_einRep.push_back("ij");
        this->_einRep.push_back("jk");
        this->_einRep.push_back("ik");
        this->_parallelDim.push_back(3);
        this->_parallelDim.push_back(3);
    }
    ~MatrixMatrixMulOp() {}
    void destroy(){};
};

class VectorMatrixMulOp : public Op {
  public:
    VectorMatrixMulOp() : Op(BASIC_OP, 2, 1, std::string("VectorMatrixMul")) {
        this->_inputNDims.push_back(1);
        this->_inputNDims.push_back(2);
        this->_outputNDims.push_back(1);

        this->_einOp = 1;
        this->_einRep.push_back("i");
        this->_einRep.push_back("ij");
        this->_einRep.push_back("j");
        this->_parallelDim.push_back(1);
        this->_parallelDim.push_back(3);
    }
    ~VectorMatrixMulOp() {}
    void destroy(){};
};

class MatrixVectorMulOp : public Op {
  public:
    MatrixVectorMulOp() : Op(BASIC_OP, 2, 1, std::string("MatrixVectorMul")) {
        this->_inputNDims.push_back(2);
        this->_inputNDims.push_back(1);
        this->_outputNDims.push_back(1);

        this->_einOp = 1;
        this->_einRep.push_back("ij");
        this->_einRep.push_back("j");
        this->_einRep.push_back("i");
        this->_parallelDim.push_back(3);
        this->_parallelDim.push_back(1);
    }
    ~MatrixVectorMulOp() {}
    void destroy(){};
};

class MatrixVectorAddOp : public Op {
  public:
    MatrixVectorAddOp() : Op(BASIC_OP, 2, 1, std::string("MatrixVectorAdd")) {
        this->_inputNDims.push_back(2);
        this->_inputNDims.push_back(1);
        this->_outputNDims.push_back(1);

        this->_einOp = 1;
        this->_einRep.push_back("ij");
        this->_einRep.push_back("j");
        this->_einRep.push_back("ij");
    }
    ~MatrixVectorAddOp() {}
    void destroy(){};
};

//=====================================================
// Definition of 1-D basic operations.
// Version v0.1: basic ops for simple-MLP listed below
//--Tensor operations:
//----Not implemented.
//--Math Operations:
//----VectorVectorInnerProduct
//=====================================================

class VectorVectorInnerProductOp : public Op {
  public:
    VectorVectorInnerProductOp()
        : Op(BASIC_OP, 2, 1, std::string("VectorVectorInnerProduct")) {
        this->_inputNDims.push_back(1);
        this->_inputNDims.push_back(1);
        this->_outputNDims.push_back(0);

        this->_einOp = 1;
        this->_einRep.push_back("i");
        this->_einRep.push_back("i");
        this->_einRep.push_back("");
        this->_parallelDim.push_back(1);
        this->_parallelDim.push_back(1);
    }
    ~VectorVectorInnerProductOp() {}
    void destroy(){};
};

//=====================================================
// Definition of 0-D basic operations.
// Version v0.1: basic ops for simple-MLP listed below
//--Tensor operations:
//----Not implemented.
//--Math Operations:
//----ScalarMul
//----ScalarAdd
//----ScalarExp
//----ScalarNeg
//----ScalarDiv
//----ScalarLog
//=====================================================

class ScalarMulOp : public Op {
  public:
    ScalarMulOp() : Op(BASIC_OP, 2, 1, std::string("ScalarMul")) {
        this->_inputNDims.push_back(0);
        this->_inputNDims.push_back(0);
        this->_outputNDims.push_back(0);

        this->_einOp = 1;
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_parallelDim.push_back(0);
        this->_parallelDim.push_back(0);
    }
    ~ScalarMulOp() {}
    void destroy(){};
};

class ScalarAddOp : public Op {
  public:
    ScalarAddOp() : Op(BASIC_OP, 2, 1, std::string("ScalarAdd")) {
        this->_inputNDims.push_back(0);
        this->_inputNDims.push_back(0);
        this->_outputNDims.push_back(0);

        this->_einOp = 1;
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_parallelDim.push_back(0);
        this->_parallelDim.push_back(0);
    }
    ~ScalarAddOp() {}
    void destroy(){};
};

class ScalarMaxOp : public Op {
  public:
    ScalarMaxOp() : Op(BASIC_OP, 2, 1, std::string("ScalarMax")) {
        this->_inputNDims.push_back(0);
        this->_inputNDims.push_back(0);
        this->_outputNDims.push_back(0);

        this->_einOp = 1;
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_parallelDim.push_back(0);
        this->_parallelDim.push_back(0);
    }
    ~ScalarMaxOp() {}
    void destroy(){};
};

class ScalarExpOp : public Op {
  public:
    ScalarExpOp() : Op(BASIC_OP, 1, 1, std::string("ScalarExp")) {
        this->_inputNDims.push_back(0);
        this->_outputNDims.push_back(0);

        this->_einOp = 1;
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_parallelDim.push_back(0);
    }
    ~ScalarExpOp() {}
    void destroy(){};
};

class ScalarNegOp : public Op {
  public:
    ScalarNegOp() : Op(BASIC_OP, 1, 1, std::string("ScalarNeg")) {
        this->_inputNDims.push_back(0);
        this->_outputNDims.push_back(0);

        this->_einOp = 1;
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_parallelDim.push_back(0);
    }
    ~ScalarNegOp() {}
    void destroy(){};
};

class ScalarDivOp : public Op {
  public:
    ScalarDivOp() : Op(BASIC_OP, 1, 1, std::string("ScalarDiv")) {
        this->_inputNDims.push_back(0);
        this->_outputNDims.push_back(0);

        this->_einOp = 1;
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_parallelDim.push_back(0);
    }
    ~ScalarDivOp() {}
    void destroy(){};
};

class ScalarLogOp : public Op {
  public:
    ScalarLogOp() : Op(BASIC_OP, 1, 1, std::string("ScalarLog")) {
        this->_inputNDims.push_back(0);
        this->_outputNDims.push_back(0);

        this->_einOp = 1;
        this->_einRep.push_back("");
        this->_einRep.push_back("");
        this->_parallelDim.push_back(0);
    }
    ~ScalarLogOp() {}
    void destroy(){};
};
} // namespace op
} // namespace swc

#endif
