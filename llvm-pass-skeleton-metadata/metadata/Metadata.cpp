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
  struct MetadataPass : public FunctionPass {
    static char ID;
    MetadataPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      errs() << "I saw a function called " << F.getName() << "!\n";
      LLVMContext& C = F.getContext();
      int instructions = 0;
      if (!F.isDeclaration()) {
        for (auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {
          instructions++;
          MDNode* N = MDNode::get(C, MDString::get(C, std::to_string(instructions)));
          (*I).setMetadata("stats.instNumber", N);
        }
        MDNode* temp_N = MDNode::get(C, ConstantAsMetadata::get(ConstantInt::get(C, llvm::APInt(64, instructions, false))));
        MDNode* N = MDNode::get(C, temp_N);
        F.setMetadata("stats.totalInsts", N);
      }
      return true;
    }
  };
}

char MetadataPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerMetadataPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new MetadataPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerMetadataPass);

static RegisterPass<MetadataPass> X("metadata", "Metadata Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
