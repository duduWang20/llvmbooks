#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

namespace {
  struct ReadMetadataPass : public FunctionPass {
    static char ID;
    ReadMetadataPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      LLVMContext& C = F.getContext();
      SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
      F.getAllMetadata(MDs);
      for (auto &MD : MDs) {
        if (MDNode *N = MD.second) {
          Constant* val = dyn_cast<ConstantAsMetadata>(dyn_cast<MDNode>(N->getOperand(0))->getOperand(0))->getValue();
          errs() << "Total instructions in function " << F.getName() << " - " << cast<ConstantInt>(val)->getSExtValue() << "\n";
        }
      }
      for(auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        //showing different way of accessing metadata
        if (MDNode* N = (*I).getMetadata("stats.instNumber")) {
          errs() << cast<MDString>(N->getOperand(0))->getString() << "\n";
        }
      } 
      return false;
    }
  };
}

char ReadMetadataPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerReadMetadataPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new ReadMetadataPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerReadMetadataPass);

static RegisterPass<ReadMetadataPass> X("read-metadata", "Read Metadata Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
