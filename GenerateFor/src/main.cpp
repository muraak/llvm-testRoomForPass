
// #include "LinkWithInline.h"

#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;

// #define ON_MAC
#define ON_WINDOWS

int main(int argc, char** argv)
{
	LLVMContext context;
	SMDiagnostic err;

	std::unique_ptr<Module> M = parseIRFile("in.bc", err, context, true, "");

	if (!M) {
		errs() << err.getMessage();
		return 1;
	}

	for(auto &F : *M) {
		IRBuilder<> Builder(&F.getEntryBlock());
		BasicBlock * tr = F.getBasicBlockList().getNextNode(F.getBasicBlockList().front());
		BasicBlock * fl = F.getBasicBlockList().getNextNode(*tr);
		Value * cond = ConstantInt::get(IntegerType::get(context, 1), 1);
		F.getEntryBlock().getInstList().pop_back();
		Builder.CreateCondBr(cond, tr, fl);
	}

	M->dump();

	 std::error_code ec;
	 llvm::raw_fd_ostream os("out.bc", ec, llvm::sys::fs::OpenFlags::F_None);
	 WriteBitcodeToFile(*M, os);

	return 0;
}
