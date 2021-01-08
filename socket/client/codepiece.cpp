void runTuning(){
  for (const auto &I : F_->getInstrs()) {
    if (auto *MM = llvm::dyn_cast<MatMulInst>(&I)) {
      ShapeHW cdim(MM->getDest()->getType()->dims());
      ShapeHW adim(MM->getLHS()->getType()->dims());
      ShapeHW bdim(MM->getRHS()->getType()->dims());
      assert(adim.width==bdim.height && "error dims!");

      ATClient client;
      // defaultl ip = 127.0.0.1
      client.connect();

      char data[1024] = {0};
      char* buf = &data[0];
      std::string libPath = "/tmp";
      const int lenLibPath = strlen(libPath.c_str());
      const char* opName = "matmul_gpu";
      const int lenOpName = strlen(opName);

      auto elemTy = MM->getDest()->getElementType();
      if (elemTy == ElemKind::FloatTy) {
				llvm::outs()  << "Tuning MatMul for " << MM->getName() << "( " <<
								adim.height << "," << adim.width << ") x ("
								<< bdim.height << "," << bdim.width <<")\n";

        NetHeader* header = (NetHeader*)data;

        //set type to be NET_SEND
        header->type = NET_SEND;



        // head size = 8
        buf += 8;
        memcpy(buf, libPath.c_str(), lenLibPath+1);
        buf += lenLibPath + 1;
        memcpy(buf, opName, lenOpName+1);
        buf += lenOpName + 1;

        int* param = (int*)buf;
        //task count
        param[0] = 1;
        // task 1;
        param[1] = NET_FLOAT;
        param[2] = adim.height; //m
        param[3] = adim.width; //k
        param[4] = bdim.width; //n

        // Do not include the header size 8
        int size = lenLibPath + 1 + lenOpName + 1 + 4 + 4*4*param[0];
        header->size = size;

        client.send(data, size + 8);
        client.recv(data, 8);

        llvm::outs() << (int)(header->success) << "\n";
      //=========================================================================
        std::string funcName = getMatMulFuncName(MM);
        std::string libAbsolutePath = libPath + "/lib" + getMatMulFuncName(MM) + ".so";

        llvm::outs() << "\ndlopen " << libAbsolutePath << "\n";

        handle_ = dlopen(libAbsolutePath.c_str(), RTLD_LAZY);

        if(!handle_){
          GLOW_ASSERT(0 && "dlopen failed\n");
          // handle_ = dlopen("/tmp/libtuning.so", RTLD_LAZY);
        }

        matmulFuncTy func = reinterpret_cast<matmulFuncTy>(dlsym(handle_, funcName.c_str()));
        if(!func){
          GLOW_ASSERT(0 && "dlsym failed\n");
        }
        const char* name = MM->getName().data();
        matmulFuncs_[name] = func;
      }
      continue;
    }
  }
}