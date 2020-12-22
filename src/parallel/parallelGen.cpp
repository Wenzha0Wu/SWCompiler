/*************************************************************************
	> File Name: parallelGen.cpp
	> Author: cryinlaugh
	> Mail: cryinlaugh@gmail.com
	> Created Time: Tue Jul  9 07:20:55 2019
 ************************************************************************/

#include "parallelGen.h"
#include "graphIR/OpNode.h"
#include "graphIR/TensorNode.h"
#include "op/Op.h"
#include "op/basicOp/basicOps.h"
#include "op/dlOp/dlOp.h"
#include <cassert>
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace swc {

vector<vector<int>> ParallelGen::generateStgy(OpNode *node) {
    auto *testOp = node->getOp();

    assert((testOp->_einOp == 1) && "Not a parallelizable Op!");

    std::map<char, int> comDim;
    int nInput = testOp->_nInputTensor;
    int nOutput = testOp->_nOutputTensor;
    for (int iterT = 0; iterT < (nInput + nOutput); iterT++) {
        size_t dim_size = testOp->_einRep[iterT].size();
        if (dynamic_cast<SGDOp *>(testOp) || dynamic_cast<ReluOp *>(testOp) ||
            dynamic_cast<ReluGradOp *>(testOp)) {
            auto *tnode =
                iterT < nInput
                    ? (TensorNode *)node->getParentNode(iterT)
                    : (TensorNode *)node->getChildNode(iterT - nInput);
            dim_size = tnode->getNDim();
        }
        for (size_t dimIdx = 0; dimIdx < dim_size; dimIdx++) {
            char dim_rep = testOp->_einRep[iterT][dimIdx];
            if (dim_rep == '0' || dim_rep == '_')
                continue;
            if (comDim.find(testOp->_einRep[iterT][dimIdx]) == comDim.end()) {
                comDim[testOp->_einRep[iterT][dimIdx]] = 1;
            } else {
                comDim[testOp->_einRep[iterT][dimIdx]] =
                    comDim[testOp->_einRep[iterT][dimIdx]] + 1;
            }
        }
    }

    map<char, int>::iterator it;
    it = comDim.begin();
    vector<vector<int>> strategies;
    while (it != comDim.end()) {
        if (!(it->second > 1)) {
            it++;
            continue;
        }
        vector<int> stgy;
        for (int iterT = 0; iterT < (testOp->_nInputTensor); iterT++) {
            size_t dimIdx = 0;
            for (dimIdx = 0; dimIdx < testOp->_einRep[iterT].size(); dimIdx++) {
                if (testOp->_einRep[iterT][dimIdx] == it->first)
                    break;
            }
            if (dimIdx < testOp->_einRep[iterT].size())
                stgy.push_back(dimIdx);
            else
                stgy.push_back(-1);
        }
        for (int iterT = testOp->_nInputTensor;
             iterT < (testOp->_nInputTensor + testOp->_nOutputTensor);
             iterT++) {
            size_t dimIdx = 0;
            for (dimIdx = 0; dimIdx < testOp->_einRep[iterT].size(); dimIdx++) {
                if (testOp->_einRep[iterT][dimIdx] == it->first)
                    break;
            }
            if (dimIdx < testOp->_einRep[iterT].size())
                stgy.push_back(dimIdx);
            else
                stgy.push_back(-2);
        }
        strategies.push_back(stgy);

        it++;
    }
    SWLOG_DEBUG(4) << "generateStgy get " << strategies.size() << "\n";
    return strategies;
}

vector<int> swc::ParallelGen::generateDataParStgy(OpNode *opnode) {
    swc::op::Op *testOp = opnode->getOp();
    assert((testOp->_einOp == 1) && "Not a parallelizable Op!");

    std::map<char, int> comDim;
    for (int iterT = 0;
         iterT < (testOp->_nInputTensor + testOp->_nOutputTensor); iterT++) {
        for (size_t dimIdx = 0; dimIdx < testOp->_einRep[iterT].size();
             dimIdx++) {
            char dim_rep = testOp->_einRep[iterT][dimIdx];
            if (dim_rep == '0')
                continue;
            if (comDim.find(testOp->_einRep[iterT][dimIdx]) == comDim.end()) {
                comDim[testOp->_einRep[iterT][dimIdx]] = 1;
            } else {
                comDim[testOp->_einRep[iterT][dimIdx]] =
                    comDim[testOp->_einRep[iterT][dimIdx]] + 1;
            }
        }
    }

    size_t batch_dim = 0;
    if (dynamic_cast<op::MatrixMatrixMulOp *>(testOp)) {
        // x*w
        // dy*wT
        // xT*dy
        auto input = (TensorNode *)opnode->getParentNode(0);
        if (input->getMemLayout() == layout_cn)
            batch_dim = 1;
    }

    char batch_c = testOp->_einRep[0][batch_dim];

    map<char, int>::iterator it = comDim.find(batch_c);
    vector<int> stgy;
    if (it != comDim.end()) {
        for (int iterT = 0; iterT < (testOp->_nInputTensor); iterT++) {
            size_t dimIdx = 0;
            for (dimIdx = 0; dimIdx < testOp->_einRep[iterT].size(); dimIdx++) {
                if (testOp->_einRep[iterT][dimIdx] == it->first)
                    break;
            }
            if (dimIdx < testOp->_einRep[iterT].size())
                stgy.push_back(dimIdx);
            else
                stgy.push_back(-1);
        }
        for (int iterT = testOp->_nInputTensor;
             iterT < (testOp->_nInputTensor + testOp->_nOutputTensor);
             iterT++) {
            size_t dimIdx = 0;
            for (dimIdx = 0; dimIdx < testOp->_einRep[iterT].size(); dimIdx++) {
                if (testOp->_einRep[iterT][dimIdx] == it->first)
                    break;
            }
            if (dimIdx < testOp->_einRep[iterT].size())
                stgy.push_back(dimIdx);
            else
                stgy.push_back(-2);
        }
    } else {
        SWLOG_ERROR << opnode->name() << " has no batch-parallel strategy\n";
    }

    return stgy;
}

} // namespace swc
