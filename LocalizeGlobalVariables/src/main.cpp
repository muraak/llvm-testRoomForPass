#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;

#define ON_MAC

void localizeGlobalVariablesIfPossible(Module &M);
bool isInlineFunction(Function * F);

int main(int argc, char** argv)
{
	static LLVMContext context;
	SMDiagnostic err;

	std::unique_ptr<Module> M = parseIRFile("out.bc", err, context, true, "");

	if (!M) {
		errs() << err.getMessage();
		return 1;
	}

	localizeGlobalVariablesIfPossible(*M);

	// I couldn't use dunm() on Mac because of reference error...
	// But I found there is another way to execute what dump() does!
#ifdef ON_WINDOWS
	M->dump();
#endif
#ifdef ON_MAC
	M->print(llvm::errs(), nullptr);
#endif

	// I haven't try to below codes because of problems same as dump()
	// Is there any solution?
#ifdef ON_WINDOWS
	std::error_code ec;
	llvm::raw_fd_ostream os("result.bc", ec, llvm::sys::fs::OpenFlags::F_None); 
	WriteBitcodeToFile(*M, os, false, nullptr, false, nullptr);
#endif

	return 0;
}

void localizeGlobalVariablesIfPossible(Module &M) {

	//find global variables
	for (auto &G : M.getGlobalList()) {

		bool can_localize = true;
		Function *F_refs_G = NULL;

		for (auto U : G.users()) {
			if (auto I = dyn_cast<Instruction>(U)) {
				
				if (Function *F = I->getFunction()) {

					if(isInlineFunction(F) == false) {
						if(F_refs_G == NULL) {
							F_refs_G = F;
						}
						else {
							if(F->getGUID() != F_refs_G->getGUID()) {
								can_localize = false;
								errs() << "break!\n";
								break;
							}
						}
					}
				}
			}
		}

		if(can_localize && F_refs_G != NULL) {
			outs() << "Global Variable " << G.getName() << " can be localized into " 
				<< F_refs_G->getName() << "()\n";

			for(auto &BB : *F_refs_G) {
				// insert substitute local value into the begining of belonging function
				IRBuilder<> Builder(&BB.getInstList().front()); // CHECK: Is that right way?
				auto ins = Builder.CreateAlloca(G.getType(), 0, "t");
				// replace G to substitute local value
				G.replaceAllUsesWith(ins);
			}
		}
	}
}

bool isInlineFunction(Function * F) {
	for (auto A : F->getAttributes()) {
		if (A.hasAttribute(Attribute::AlwaysInline)) {
			return true;
		}
	}
	return false;
}
