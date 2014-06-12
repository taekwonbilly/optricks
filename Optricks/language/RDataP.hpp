/*
 * RDataP.hpp
 *
 *  Created on: Mar 3, 2014
 *      Author: Billy
 */

#ifndef RDATAP_HPP_
#define RDATAP_HPP_

#include "RData.hpp"
#include "./class/AbstractClass.hpp"
#include "./data/VoidData.hpp"
#include "../operators/Deconstructor.hpp"

inline llvm::Function* RData::getExtern(String name, const AbstractClass* R, const std::vector<const AbstractClass*>& A, bool varArgs, String lib){
	llvm::SmallVector<llvm::Type*,0> args(A.size());
	for(unsigned i = 0; i<A.size(); i++){
		assert(A[i]);
		assert(A[i]->type);
		args[i] = A[i]->type;
	}
	assert(R);
	assert(R->type);
	llvm::FunctionType *FT = llvm::FunctionType::get(R->type, args, varArgs);
	return getExtern(name, FT, lib);
}
void RData::makeJump(String name, JumpType jump, const Data* val, PositionID id){
	if(name==""){
		if(jump==RETURN){
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump==FUNC){
					if(jumps[i]->returnType==nullptr){
						id.error("Cannot return from function that does not allow returns");
					} else if(jumps[i]->returnType->classType==CLASS_VOID){
						if(val->type!=R_VOID && val->getReturnType()->classType!=CLASS_VOID) id.error("Cannot return something in function requiring void");
						assert(jumps[i]->scope);
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
						builder.CreateRetVoid();
					}
					else {
						auto tmp = val->castToV(*this, jumps[i]->returnType, id);
						assert(jumps[i]->scope);
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
						builder.CreateRet(tmp);
					}
					return;
				} else {
					if(jumps[i]->scope){
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
					}
				}
				if(i <= 0){
					for(auto a: jumps)
						cerr << a->name << " and " << str(a->toJump) << endl << flush;
					id.compilerError("Error could not find returning block - func");
					exit(1);
				}
			}
		} else if(jump==YIELD){
			id.warning("Generator variable garbage collection / cleanup needs to be implemented");
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump==GENERATOR){
					llvm::BasicBlock* cur = builder.GetInsertBlock();
					if(jumps[i]->returnType->classType==CLASS_VOID){
						if(val->type!=R_VOID && val->getReturnType()->classType!=CLASS_VOID) id.error("Cannot return something in function requiring void");
						jumps[i]->endings.push_back(std::pair<llvm::BasicBlock*,const Data*>(cur, &VOID_DATA));
					}
					else jumps[i]->endings.push_back(std::pair<llvm::BasicBlock*,const Data*>(cur, val->castTo(*this, jumps[i]->returnType, id)));
					builder.CreateBr(jumps[i]->end);//TODO DECREMENT ALL COUNTS BEFORE HERE
					llvm::BasicBlock *RESUME = CreateBlock("postReturn",cur);
					jumps[i]->resumes.push_back(std::pair<llvm::BasicBlock*,llvm::BasicBlock*>(cur,RESUME));
					builder.SetInsertPoint(RESUME);
					return;
				} else {
					//TODO GENERATOR SCOPE GC
					/*
					if(jumps[i]->scope){
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
					}*/
				}
				if(i <= 0){
					id.compilerError("Error could not find returning block - gen");
					exit(1);
				}
			}
		} else{
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == LOOP){
					//jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
					builder.CreateBr((jump==BREAK)?(jumps[i]->end):(jumps[i]->start));
					return;
				} else {
					if(jumps[i]->scope){
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
					}
				}
				if(i <= 0){
					id.compilerError("Error could not find continue/break block");
					exit(1);
				}
			}
		}
	} else {
		id.compilerError("Named jumps not supported yet");
		exit(1);
	}
}

void RData::recursiveFinalize(LazyLocation* ll, std::map<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >::iterator toFix){
	assert(toFix->second.first);
	llvm::PHINode* p = toFix->second.first;
	if(llvm::BasicBlock* prev = toFix->first->getUniquePredecessor()){
		//cerr << "has unique! "<<endl;fflush(stderr);
		auto found = ll->data.find(prev);
		if(found!=ll->data.end()){
			//cerr << "has value! "<<endl;fflush(stderr);
			llvm::Value* v = ll->getFastValue(*this,found);
			p->replaceAllUsesWith(v);
			p->eraseFromParent();
			auto repdata = ll->data.find(toFix->first);
			if(repdata->second==p) repdata->second = v;
			toFix->second.first = NULL;
		} else{
			//cerr << "no value "<<endl;fflush(stderr);
			assert(ll->phi.find(prev)==ll->phi.end());
			std::vector<llvm::BasicBlock*> cache(1,toFix->first);
			llvm::BasicBlock* tmp=prev;
			do{
				auto found3 = ll->data.find(tmp);
				if(found3!=ll->data.end() && llvm::dyn_cast_or_null<llvm::PHINode>(found3->second)==NULL){
					//cerr << "eventual unique value "<<endl;fflush(stderr);
					llvm::Value* v = ll->getFastValue(*this,found3);
					for(llvm::BasicBlock* bl:cache){
						std::map<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >::iterator fin = ll->phi.find(bl);
						if(fin!=ll->phi.end() && fin->second.first!=NULL){
							llvm::PHINode* f = fin->second.first;
							assert(v->getType()==f->getType());
							f->replaceAllUsesWith(v);
							f->eraseFromParent();
							fin->second.first=NULL;
							if(ll->data[bl]==f) ll->data[bl] = v;
						}
					}
					return;
				}
				cache.push_back(tmp);
				//cerr << "skipped " << tmp->getName().str() << endl << flush;
				prev = tmp;
				tmp = prev->getUniquePredecessor();
			}while(tmp!=NULL);
			cache.pop_back();
			builder.SetInsertPoint(prev);
			auto found3 = ll->data.find(prev);
			//cerr << "no unique! "<<endl;fflush(stderr);
			if(found3!=ll->data.end()){
				//cerr << "no unique, but value! "<<endl;fflush(stderr);
				builder.SetInsertPoint(prev);
				llvm::Value* v = ll->getFastValue(*this,found3);
				for(llvm::BasicBlock* bl: cache){
					auto fin = ll->phi.find(bl);
					if(fin!=ll->phi.end() && fin->second.first!=NULL){
						llvm::PHINode* f = fin->second.first;
						assert(v->getType()==f->getType());
						f->replaceAllUsesWith(v);
						f->eraseFromParent();
						fin->second.first=NULL;
						if(ll->data[bl]==f) ll->data[bl]=v;
					}
					//	cerr << "finished with " << blocks[i]->getName().str() << endl << flush;
				}
				auto fina = ll->phi.find(prev);
				if(fina!=ll->phi.end() && fina->second.first!=NULL){
					//cerr << "reviewing " << prev->getName().str() << endl << flush;
					recursiveFinalize(ll,fina);
					fina->second.first=NULL;
				}
				return;
			}

			//cerr << "no unique, no value!"<<endl;fflush(stderr);
			builder.SetInsertPoint(prev);
			llvm::PHINode* np = CreatePHI(p->getType(), 1/*,ll->name*/);
			ll->data[prev] = np;
			auto toRet = ll->phi.insert(std::pair<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >
			(prev, std::pair<llvm::PHINode*,PositionID>(np,toFix->second.second)));
			for(llvm::BasicBlock* bl:cache){
				auto fin = ll->phi.find(bl);
				if(fin!=ll->phi.end() && fin->second.first!=NULL){
					llvm::PHINode* f = fin->second.first;
					assert(np->getType()==f->getType());
					f->replaceAllUsesWith(np);
					f->eraseFromParent();
					fin->second.first=NULL;
					if(ll->data[bl]==f) ll->data[bl]=np;
				}
			}
			if(toRet.first->second.first!=NULL){
				recursiveFinalize(ll,toRet.first);
				toRet.first->second.first = NULL;
			}
		}
	} else{
		llvm::pred_iterator PI = pred_begin(toFix->first);
		llvm::pred_iterator E = pred_end(toFix->first);
		if(PI==E){
/*			toFix->first->getParent()->dump();
			cerr << endl << flush;
			toFix->first->dump();
			cerr << endl << flush;
			toFix->second.second.error("Variable was not defined here");
			*/
			llvm::Value* tmp = llvm::UndefValue::get(p->getType());
			p->replaceAllUsesWith(tmp);
			p->eraseFromParent();
			if(ll->data[toFix->first]==p) ll->data[toFix->first]=tmp;
			toFix->second.first=NULL;
			return;
		}
		toFix->second.first = NULL;
		do{
			llvm::BasicBlock* bb = *PI;
			auto found = ll->data.find(bb);
			if(found!=ll->data.end()){
				p->addIncoming(ll->getFastValue(*this,found),bb);
			} else{
				assert(ll->phi.find(bb)==ll->phi.end());
				builder.SetInsertPoint(bb);
				llvm::PHINode* np = CreatePHI(p->getType(), 1U/*,ll->name*/);
				auto toRet = ll->phi.insert(std::pair<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >
				(bb, std::pair<llvm::PHINode*,PositionID>(np,toFix->second.second)));
				ll->data[bb] = np;//todo speed up
				assert(np);
				assert(np->getType()==ll->type);
				p->addIncoming(np,bb);
				if(toRet.first->second.first!=NULL){
					recursiveFinalize(ll,toRet.first);
				}
			}
			++PI;
		}while(PI!=E);
	}
}
void RData::FinalizeFunction(llvm::Function* f){
	llvm::BasicBlock* Parent = builder.GetInsertBlock();
	for(LazyLocation*& ll: flocs.find(f)->second){
		//		ll->phi.
		for(std::map<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >::iterator it = ll->phi.begin(); it!=ll->phi.end(); ++it){
			if(it->second.first) recursiveFinalize(ll,it);
		}
		if(!ll->used){
			if(llvm::Instruction* u = llvm::dyn_cast<llvm::Instruction>(ll->position)) u->eraseFromParent();
		}
		delete ll;
	}
	if(Parent) builder.SetInsertPoint(Parent);
	//cerr << "start finalizing function" << endl << flush;
	if(debug){
		f->dump();
		cerr << endl << flush;
		cerr << "ENDPREV" << endl << flush;
		fflush(0);
	}
	fpm.run(*f);
	flocs.erase(f);
	pred.erase(f);
	if(debug){
		f->dump();
		cerr << endl << flush;
	}
	//f->dump();
	//cerr << endl << flush;
	//cerr << "done finalizing function" << endl << flush;
}
/*

		void RData::FinalizeFunction(Function* f,bool debug=false){
			BasicBlock* Parent = builder.GetInsertBlock();
			if(Parent) builder.SetInsertPoint(Parent);
			if(debug){
				f->dump();
				cerr << endl << flush;
			}



			fpm.run(*f);
			flocs.erase(f);
			pred.erase(f);
			if(debug){
				f->dump();
				cerr << endl << flush;
			}
		}
 */
#endif /* RDATAP_HPP_ */
