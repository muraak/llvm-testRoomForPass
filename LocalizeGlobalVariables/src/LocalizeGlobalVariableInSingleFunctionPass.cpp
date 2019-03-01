#include "LocalizeGlobalVariableInSingleFunctionPass.h"

char LocalizeGlobalVariableInSingleFunctionPass::ID = 0;

bool LocalizeGlobalVariableInSingleFunctionPass::runOnModule (Module &M) {

	bool modified = false;

	//find global variables
	for (auto &G : M.getGlobalList()) {

		bool can_localize = true;
		Function *F_refs_G = NULL;

		for (auto U : G.users()) {
			if (auto I = dyn_cast<Instruction>(U)) {

				if (Function *F = I->getFunction()) {

					if (IsInlineFunction(F) == false) {
						if (F_refs_G == NULL) {
							F_refs_G = F;
						}
						else {
							if (F->getGUID() != F_refs_G->getGUID()) {
								can_localize = false;
								errs() << "break!\n";
								break;
							}
						}
					}
				}
			}
		}

		if (can_localize && F_refs_G != NULL) {
			outs() << "Global Variable " << G.getName() << " can be localized into "
				<< F_refs_G->getName() << "()\n";

			for (auto &BB : *F_refs_G) {

				// insert substitute local value into the begining of belonging function
				IRBuilder<> Builder(&BB.getInstList().front()); // CHECK: Is that right way?
				auto ins = Builder.CreateAlloca(G.getValueType(), nullptr, "t");

				// replace G in F_refs_G to substitute local value
				// INFO: Do not use for(auto U :G->uses()) when you replace or remove then inside the loop
				//       because the iteration gonna be broken by changing list of iterator itself!
				while (IsThereUserInFunction(F_refs_G, &G)) {
					User* u = G.user_back();
					if (auto I = dyn_cast<Instruction>(u)) {
						if (I->getFunction()->getGUID() == F_refs_G->getGUID()) {
							u->replaceUsesOfWith(&G, ins);
							modified |= true;
						}
					}
				}

				if(modified) {
					outs() << "All uses of global variable '" << G.getName() << "' in function '"
						<< F_refs_G->getName() << "()' was replaced with local variable '"
						<< ins->getName() << "'!\n";
				}
			}
		}
		else {
			outs() << "Global Variable " << G.getName() << " can't be localized.\n";
		}
	}

	return modified;
}

bool LocalizeGlobalVariableInSingleFunctionPass::IsInlineFunction(Function * F) {
	for (auto A : F->getAttributes()) {
		if (A.hasAttribute(Attribute::AlwaysInline)) {
			return true;
		}
	}
	return false;
}

bool LocalizeGlobalVariableInSingleFunctionPass::IsThereUserInFunction(Function *F, GlobalValue *G) {

	for (auto &U : G->uses()) {
		if (auto I = dyn_cast<Instruction>(U.getUser())) {
			if (I->getFunction()->getGUID() == F->getGUID()) {
				return true;
			}
		}
	}

	return false;
}

