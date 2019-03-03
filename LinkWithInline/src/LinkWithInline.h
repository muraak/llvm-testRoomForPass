#pragma once

#include "llvm/IR/AutoUpgrade.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Transforms/IPO/Internalize.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Linker/Linker.h"

using namespace llvm;

class LinkWithInline 
{
public:
	static bool linkFilesWithInline(const char *argv0, LLVMContext &Context, Linker &L, 
		const std::string srcFile, const std::list<std::string> &LinkFiles,unsigned Flags);

private:

	static std::unique_ptr<Module> loadFile(const char *argv0,
		const std::string &FN,
		LLVMContext &Context,
		bool MaterializeMetadata = true);
};
