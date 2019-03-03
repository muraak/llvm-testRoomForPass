#include "LinkWithInline.h"

#include <memory>
#include <utility>

static ExitOnError ExitOnErr;


bool LinkWithInline::linkFilesWithInline(const char *argv0, LLVMContext &Context, Linker &L,
	const std::string srcFile, const std::list<std::string> &LinkFiles, unsigned Flags) {
	// Filter out flags that don't apply to the first file we load.
	unsigned ApplicableFlags = Flags & Linker::Flags::OverrideFromSrc;
	// Similar to some flags, internalization doesn't apply to the first file.
	bool InternalizeLinkedSymbols = false;

	// Merge srcFile and LinkFiles into a single list for iteration
	std::list<std::string> Files(LinkFiles);
	// Insert srcFile in front of Files
	Files.push_front(srcFile);
	// Flag for identify front element of Files
	bool isFront = true;

	for (const auto &File : Files) {

		std::unique_ptr<Module> M = loadFile(argv0, File, Context);
		if (!M.get()) {
			errs() << argv0 << ": ";
			WithColor::error() << " loading file '" << File << "'\n";
			return false;
		}

		// Note that when ODR merging types cannot verify input files in here When
		// doing that debug metadata in the src module might already be pointing to
		// the destination.
		if (verifyModule(*M, &errs())) {
			errs() << argv0 << ": " << File << ": ";
			WithColor::error() << "input module is broken!\n";
			return false;
		}

		// Make all Functions inside M to the inline function except srcFile's module
		if (isFront) {
			isFront = false;
		}
		else {
			for (auto &F : *M) {
				// Do not user addAttribute() and removeAttribute() becuse it does't work as we aimed. 
				F.removeFnAttr(Attribute::NoInline);
				F.removeFnAttr(Attribute::OptimizeNone);
				F.addFnAttr(Attribute::AlwaysInline);
				outs() << "add alwaysInline attribute to function " 
					<< F.getName() << "() in module " << M->getName() << "\n";
			}
		}


		// Link M in single module that you asigned at the constructor of Link Object "L"
		bool Err = false;
		if (InternalizeLinkedSymbols) {
			Err = L.linkInModule(
				std::move(M), ApplicableFlags, [](Module &M, const StringSet<> &GVS) {
				internalizeModule(M, [&GVS](const GlobalValue &GV) {
					return !GV.hasName() || (GVS.count(GV.getName()) == 0);
				});
			});
		}
		else {
			Err = L.linkInModule(std::move(M), ApplicableFlags);
		}

		if (Err)
			return false;

		// Internalization applies to linking of subsequent files.
		InternalizeLinkedSymbols = true;//Internalize;

		// All linker flags apply to linking of subsequent files.
		ApplicableFlags = Flags;
	}

	return true;
}

// Read the specified bitcode file in and return it. This routine searches the
// link path for the specified file to try to find it...
//
std::unique_ptr<Module> LinkWithInline::loadFile(const char *argv0,
	const std::string &FN,
	LLVMContext &Context,
	bool MaterializeMetadata) {
	SMDiagnostic Err;

	std::unique_ptr<Module> Result;

	Result = parseIRFile(FN, Err, Context);

	if (!Result) {
		Err.print(argv0, errs());
		return nullptr;
	}

	if (MaterializeMetadata) {
		ExitOnErr(Result->materializeMetadata());
		UpgradeDebugInfo(*Result);
	}

	return Result;
}
