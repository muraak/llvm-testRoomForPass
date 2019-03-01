#pragma once

#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;

static class LocalizeGlobalVariableInSingleFunction {
public:
	static void RunOnModule(Module &M);

private:
	static bool IsInlineFunction(Function * F);
	static bool IsThereUserInFunction(Function *F, GlobalValue *G);
};




