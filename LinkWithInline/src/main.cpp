
#include "LinkWithInline.h"

#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;

#define ON_MAC
//#define ON_WINDOWS

int main(int argc, char** argv)
{
	LLVMContext Context;

	auto Composite = make_unique<Module>("link-with-inline", Context);
	Linker L(*Composite);

	auto inputFileNames = new std::list<std::string>();

	for(int i=2; i < argc; i++) {
		inputFileNames->push_back(argv[i]);
	}

	if (!LinkWithInline::linkFilesWithInline("llvm-link", Context, L, argv[1] ,*inputFileNames, 0))
		return 1;

	std::error_code ec;
	llvm::raw_fd_ostream os("link-with-inline.bc", ec, llvm::sys::fs::OpenFlags::F_None);
	WriteBitcodeToFile(*Composite, os);

	return 0;
}
