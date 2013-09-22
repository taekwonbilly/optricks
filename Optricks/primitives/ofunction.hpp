/*
 * ofunction.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OFUNCTION_HPP_
#define OFUNCTION_HPP_

#include "oobjectproto.hpp"
#include "../constructs/Declaration.hpp"
#include "../constructs/Module.hpp"

class ofunction:public oobject{
	public:
		Statement* self;
		Statement* returnV;
		FunctionProto* prototype;
		Function* myFunction;
		ofunction(PositionID id, Statement* s, Statement* r, std::vector<Declaration*> dec):oobject(id, functionClass),
				self(s),returnV(r){
			myFunction = NULL;
			prototype = new FunctionProto((self==NULL)?"unknown":(self->getMetadata()->name), dec, NULL);
			if(self!=NULL && self->getMetadata()!=NULL && self->getMetadata()->name!="" && self->getMetadata()->name[0]!='~'){
				self->getMetadata()->function = prototype;
				self->returnType = functionClass;
			}
		}

		ReferenceElement* getMetadata(){
			return new ReferenceElement(NULL,"lambda",NULL,NULL,prototype,NULL,NULL);
		}
		void registerFunctionArgs(RData& r) override{
			if(prototype==NULL) error("Function prototype should not be null");
			if(returnV!=NULL) prototype->returnType = returnV->getMetadata()->selfClass;
			for(Declaration* d: prototype->declarations){
				d->registerFunctionArgs(r);
			}
			//if(prototype->returnType==NULL) error("Function prototype-return should not be null");
		};
		void registerFunctionDefaultArgs() override{
			if(self!=NULL) self->registerFunctionDefaultArgs();
			for(Declaration* d: prototype->declarations){
				d->registerFunctionDefaultArgs();
			}
			if(returnV!=NULL) returnV->registerFunctionDefaultArgs();
		};
		void resolvePointers() override{
			if(self!=NULL) self->resolvePointers();
			for(Declaration* d: prototype->declarations){
				d->resolvePointers();
			}
			if(returnV!=NULL) returnV->resolvePointers();
		};
		ClassProto* checkTypes() override{
			for(auto& a:prototype->declarations){
				a->checkTypes();
			}
			if(self!=NULL) self->checkTypes();
			if(returnV!=NULL){
				returnV->checkTypes();
				prototype->returnType = returnV->getMetadata()->selfClass;
				if(prototype->returnType==NULL) error("Could not post-resolve return type "+returnV->returnType->name);
			} //else if(prototype->returnType==NULL) error("Could not p-resolve return type");
			return returnType;
		}
};
//TODO
class externFunction : public ofunction{
	public:
		externFunction(PositionID id, Statement* s, Statement* r, std::vector<Declaration*> dec):
			ofunction(id, s,r,dec){
		}
		void write(ostream& f, String b) const override{
			f << "extern ";
			f << returnV << " ";
			f << (prototype->name) ;
			f << "(" ;
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
		}
		void registerFunctionArgs(RData& a){
			ofunction::registerFunctionArgs(a);
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->getMetadata()->selfClass->getType(a);
				if(cl==NULL) error("Type argument "+b->classV->getMetadata()->name+" is null");
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getMetadata()->selfClass;
			if(cp==NULL) error("Cannot use void class proto in extern");
			Type* r = cp->getType(a);
			if(r==NULL) error("Type argument "+cp->name+" is null");
			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, prototype->name, a.lmod);//todo check this
			if(prototype->name=="printi") a.exec->addGlobalMapping(F, (void*)(&printi));
			if(prototype->name=="printd") a.exec->addGlobalMapping(F, (void*)(&printd));
			if(prototype->name=="printb") a.exec->addGlobalMapping(F, (void*)(&printb));
			if(prototype->name=="prints") a.exec->addGlobalMapping(F, (void*)(&prints));
			if(prototype->name=="printc") a.exec->addGlobalMapping(F, (void*)(&printc));
			if(F->getName().str()!=prototype->name) error("Cannot extern function due to name in use "+prototype->name +" was replaced with "+F->getName().str());
			myFunction = F;
			self->getMetadata()->setValue(F,a);
		}

		oobject* simplify() override final{
			return this;
		}
		Value* evaluate(RData& a) override{
			return self->getMetadata()->getValue(a);
		}
};
class lambdaFunction : public ofunction{
	public:
		Statement* ret;
		lambdaFunction(PositionID id, std::vector<Declaration*> dec, Statement* r):
			ofunction(id, NULL,NULL,dec){
			ret = r;
		}

		lambdaFunction* simplify() override final{

			std::vector<Declaration*> g;
			for(auto a:prototype->declarations) g.push_back(a->simplify());
			return new lambdaFunction(filePos,g,ret->simplify());
		}
		void write(ostream& f, String b) const override{
			f << "lambda ";
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,b+"  ");
			}
			f << ": ";
			f << ret;
		}
		void registerFunctionArgs(RData& r) override{
			ofunction::registerFunctionArgs(r);
			ret->registerFunctionArgs(r);
		};
		void registerFunctionDefaultArgs() override final{
			ofunction::registerFunctionDefaultArgs();
			ret->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			ofunction::resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes() override{
			ofunction::checkTypes();
			return prototype->returnType = ret->checkTypes();
		}
		Function* evaluate(RData& ar) override{
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->getMetadata()->selfClass->getType(ar);
				if(cl==NULL) error("Type argument "+b->classV->getMetadata()->name+" is null", true);
				args.push_back(cl);
			}
			Type* r = prototype->returnType->getType(ar);
			if(r==NULL){
				error("Error null return type for class " + prototype->returnType->name);
				r = VOIDTYPE;
			}
			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, "lambda", ar.lmod);//todo check this
			// Set names for all arguments.
			unsigned Idx = 0;
			for (Function::arg_iterator AI = F->arg_begin(); Idx != args.size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				prototype->declarations[Idx]->variable->getMetadata()->setValue(AI,ar);
			}

			//BasicBlock *Parent = ar.builder.GetInsertBlock();
			//	ar.builder.SetInsertPoint(Parent);

			BasicBlock *Parent = ar.builder.GetInsertBlock();
			BasicBlock *BB =
					//	ar.builder.GetInsertBlock();
					BasicBlock::Create(getGlobalContext(), "entry", F);
			ar.builder.SetInsertPoint(BB);
			Value* v = ret->evaluate(ar);
			if(r!=VOIDTYPE)
				ar.builder.CreateRet(v);
			else
				ar.builder.CreateRetVoid();
			//cout << "testing cos" << cos(3) << endl << flush;
			verifyFunction(*F);
			//cout << "verified" << endl << flush;
			ar.fpm->run(*F);

			ar.builder.SetInsertPoint( Parent );
			return F;
		}
};
class userFunction : public ofunction{
		//def int printr(int i){ int j = i+48; putchar(j); return i;}
	public:
		Statement* ret;
		userFunction(PositionID id, Statement* a, Statement* b, std::vector<Declaration*> dec, Statement* r):
			ofunction(id, a, b, dec){
			ret = r;
		}
		oobject* simplify() override final{
			return this;
		}
		void write(ostream& f, String b) const override{
			f << "def ";
			f << returnV << " ";
			f << (prototype->name) ;
			f << "(" ;
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
			if(ret!=NULL) ret->write(f, b);
		}
		void registerFunctionArgs(RData& ra) override{
			ofunction::registerFunctionArgs(ra);
			ret->registerFunctionArgs(ra);

			ret->registerFunctionArgs(ra);
			ret->registerFunctionDefaultArgs();
			ret->checkTypes();
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				b->registerFunctionArgs(ra);
				b->registerFunctionDefaultArgs();
				b->checkTypes();
				b->classV->checkTypes();
				Type* cl = b->classV->getMetadata()->selfClass->getType(ra);
				if(cl==NULL) error("Type argument "+b->classV->getMetadata()->name+" is null", true);
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getMetadata()->selfClass;
			if(cp==NULL) error("Unknown return type");
			if(cp==autoClass) error("Cannot support auto return for function");
			Type* r = cp->getType(ra);

			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, (self==NULL)?"afunc":(self->getMetadata()->name), ra.lmod);
			if(self!= NULL){
				if(self->getMetadata()->name.size()>0 && self->getMetadata()->name[0]=='~'){
					if(prototype->declarations.size()==1){
						error("User-defined unary operators not supported");
					} else if (prototype->declarations.size()==2){
						prototype->declarations[0]->classV->getMetadata()->selfClass->addBinop(
								self->getMetadata()->name.substr(1),
								prototype->declarations[1]->classV->getMetadata()->selfClass
						) = new obinopUser(F, cp);
					} else error("Cannot make operator with argument count of "+str<unsigned int>(prototype->declarations.size()));
				} else self->getMetadata()->setValue(F, ra);
			}
			//BasicBlock *Parent = ar.builder.GetInsertBlock();
			//	ar.builder.SetInsertPoint(Parent);

			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB =
					//	ar.builder.GetInsertBlock();
					BasicBlock::Create(getGlobalContext(), "entry", F);
			BasicBlock *MERGE =
					//	ar.builder.GetInsertBlock();
					BasicBlock::Create(getGlobalContext(), "funcMerge", F);
			Jumpable* j = new Jumpable("", FUNC, NULL, MERGE, prototype->returnType);
			ra.addJump(j);
			ra.builder.SetInsertPoint(BB);

			unsigned Idx = 0;

			//			IRBuilder<> TmpB(& F->getEntryBlock(),
			//				F->getEntryBlock().begin());
			for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				auto met = prototype->declarations[Idx]->variable->getMetadata();
				met->llvmLocation = ra.builder.CreateAlloca(prototype->declarations[Idx]->variable->returnType->getType(ra),
						0,prototype->declarations[Idx]->variable->pointer->name);
				met->setValue(AI,ra);
			}
			ra.guarenteedReturn = false;
			ret->evaluate(ra);
			if(ra.popJump()!=j) error("Did not receive same func jumpable created");
			if(! ra.guarenteedReturn){
				if(prototype->returnType==voidClass)	ra.builder.CreateBr(MERGE);
				else error("Could not find return statement");
			}
			ra.guarenteedReturn = false;

			F->getBasicBlockList().remove(MERGE);
			F->getBasicBlockList().push_back(MERGE);
			ra.builder.SetInsertPoint(MERGE);
			if(r!=VOIDTYPE){
				auto functionReturnType = prototype->returnType->getType(ra);
				PHINode* phi = ra.builder.CreatePHI(functionReturnType, j->endings.size(), "funcRet" );
				for(auto &a : j->endings){
					phi->addIncoming(a.second, a.first);
				}
				ra.builder.CreateRet(phi);
			}
			else
				ra.builder.CreateRetVoid();
			free(j);
			//cout << "testing cos" << cos(3) << endl << flush;
			verifyFunction(*F);
			//cout << "verified" << endl << flush;
			ra.fpm->run(*F);
			myFunction = F;
			if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
		};
		void registerFunctionDefaultArgs() override final{
			ofunction::registerFunctionDefaultArgs();
			ret->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			ofunction::resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes() override{
			ofunction::checkTypes();
			ret->checkTypes();
			return prototype->returnType;
		}
		Function* evaluate(RData& ra) override{
			return myFunction;
		}
};

#endif /* OFUNCTION_HPP_ */
