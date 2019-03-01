#include "LocalizeGlobalVariableInSingleFunction.h"

using namespace llvm;

//#define ON_MAC
#define ON_WINDOWS

int main(int argc, char** argv)
{
	static LLVMContext context;
	SMDiagnostic err;

	std::unique_ptr<Module> M = parseIRFile("out.bc", err, context, true, "");

	if (!M) {
		errs() << err.getMessage();
		return 1;
	}

	LocalizeGlobalVariableInSingleFunction::RunOnModule(*M);

	// I couldn't use dump() on Mac because of reference error...
	// But I found there is another way to execute what dump() does!
 #ifdef ON_WINDOWS
 	M->dump();
 #endif
 #ifdef ON_MAC
 	M->print(llvm::errs(), nullptr);
 #endif

#ifdef ON_WINDOWS
	std::error_code ec;
	llvm::raw_fd_ostream os("result.bc", ec, llvm::sys::fs::OpenFlags::F_None); 
	WriteBitcodeToFile(*M, os);
#endif

	return 0;
}

