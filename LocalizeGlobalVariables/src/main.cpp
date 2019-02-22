#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Transforms/Scalar.h>

#include <llvm/Transforms/Scalar/DCE.h>
#include <llvm/Transforms/Scalar/ADCE.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm-c/Core.h>
#include <llvm-c/BitReader.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <fstream>
#include <iostream>

using namespace llvm; 

void myTest(Module &M);

int main(int argc, char** argv)
{
	llvm::legacy::PassManager PM;
	static LLVMContext context;
	SMDiagnostic err;

	std::unique_ptr<Module> M = parseIRFile("out.bc", err, context, true, "");

	if (!M) {
		errs() << err.getMessage();
		return 1;
	}

	myTest(*M);

	return 0;
}

void myTest(Module &M) {

	//find global variables
	for (auto &G : M.getGlobalList()) {
		
		errs() << "Global: " << G.getName() << "\n";
		errs() << "Used in following functions: " << "\n";

		for (auto U : G.users()) {
			if (auto I = dyn_cast<Instruction>(U)) {
				// FIXME!!: WE need to traverse until Function appers as Parent!
				if (auto F = dyn_cast<Function>(I->getParent()->getParent())) {
										
					for (auto A : F->getAttributes()) {
						if (!A.hasAttribute(Attribute::AlwaysInline)) {
							errs() << F->getName() << "\n";
						}
					}
				}
				
			}
		}
	}
}