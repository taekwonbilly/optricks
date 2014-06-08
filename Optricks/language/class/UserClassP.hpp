/*
 * UserClassP.hpp
 *
 *  Created on: Jan 19, 2014
 *      Author: Billy
 */

#ifndef USERCLASSP_HPP_
#define USERCLASSP_HPP_
#include "./UserClass.hpp"
#include "./AbstractClass.hpp"
#include "./builtin/IntClass.hpp"
#include "../data/ReferenceData.hpp"
#include "../data/DeclarationData.hpp"
UserClass::UserClass(const Scopable* sc, String nam, const AbstractClass* const supa, LayoutType t, bool fina,bool isObject)
	: AbstractClass(sc,nam,(!isObject && t==POINTER_LAYOUT && supa==nullptr)?(&objectClass):(supa),
				t,CLASS_USER, fina,
				(t==POINTER_LAYOUT)?(
						(llvm::Type*) llvm::PointerType::getUnqual(llvm::StructType::create(llvm::getGlobalContext(), llvm::StringRef(nam)))
		):(
						(llvm::Type*)llvm::StructType::create(llvm::getGlobalContext(), llvm::StringRef(nam))
				)
				),
						constructors(nam, nullptr),start(0),final(false)
		{
			if(t==PRIMITIVEPOINTER_LAYOUT) PositionID(0,0,"#class").warning("Garbage collection for primitivepointers is not implemented");
#ifdef NDEBUG
			if(superClass) assert(dynamic_cast<const UserClass*>(superClass));
#endif
			if(isObject){
				localVars.push_back(&intClass);
				final = true;
			}
		};

llvm::Value* UserClass::generateData(RData& r, PositionID id) const{
	if(!final) id.compilerError("Cannot generateData of non-finalized type");
	if(layout==PRIMITIVEPOINTER_LAYOUT || layout==PRIMITIVE_LAYOUT) return llvm::UndefValue::get(type);
	else {
		assert(type);
		assert(llvm::dyn_cast<llvm::PointerType>(type));
		auto tmp = ((llvm::PointerType*)type)->getArrayElementType();
		assert(tmp);
		assert(r.lmod);
		uint64_t s = llvm::DataLayout(r.lmod).getTypeAllocSize(tmp);
		llvm::IntegerType* ic = llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(size_t));
		auto v = llvm::CallInst::CreateMalloc(r.builder.GetInsertBlock(), ic,
				tmp, llvm::ConstantInt::get(ic, s));
		r.builder.Insert(v);
		return v;
	}
}
const Data* UserClass::getLocalData(RData& r, PositionID id, String s, const Data* instance) const {
	if(!final) id.compilerError("Cannot getLocalData() on unfinalized type");
	assert(instance);
	auto tmp=this;
			do{
				auto fd = tmp->localMap.find(s);
				if(fd!=tmp->localMap.end()){
					unsigned start = tmp->start+fd->second;
					if(instance->type==R_DEC)
						instance = ((const DeclarationData*)instance)->value->fastEvaluate(r);
					assert(instance->type==R_LOC || instance->type==R_CONST
							/*|| instance->type==R_REF*/);
					assert(instance->getReturnType()==this);
					if(instance->type==R_LOC){
						Location* ld;
						if(layout==PRIMITIVE_LAYOUT){
							ld = ((const LocationData*)instance)->value->getInner(r, id, 0, start);
							assert(ld);
						}
						else{
							ld = new StandardLocation(r.builder.CreateConstGEP2_32(
									((const LocationData*)instance)->value->getValue(r,id),0,start));
							assert(ld);
						}
						return new LocationData(ld, tmp->localVars[fd->second]);
					} /*else if(instance->type==R_REF){
						Location* ld;
						const LocationData* D  = ((const ReferenceData*)instance)->value;
						cerr << demangle(typeid(D).name()) << endl << flush;
						cerr << D->value->getName() << endl << flush;
						assert(D);
						if(layout==PRIMITIVE_LAYOUT){
							ld = D->value->getInner(r, id, 0, start);
							cerr << demangle(typeid(D->value).name()) << endl << flush;
							cerr << "LAZY: " << dynamic_cast<LazyLocation*>(D->value) << " STD: " << dynamic_cast<StandardLocation*>(D->value)<< endl << flush;
							assert(ld);
						}
						else{
							ld = new StandardLocation(r.builder.CreateConstGEP2_32(
									D->value->getValue(r,id),0,start));
							assert(ld);
						}
						return new LocationData(ld, tmp->localVars[fd->second]);
					} */else{
						assert(instance->type==R_CONST);
						llvm::Value* v = ((ConstantData*)instance)->value;
						if(layout==PRIMITIVE_LAYOUT)
							return new ConstantData(r.builder.CreateExtractValue(v,start),tmp->localVars[fd->second]);
						else{
							return new LocationData(new StandardLocation(r.builder.CreateConstGEP2_32(v, 0, start)), tmp->localVars[fd->second]);
						}
					}
				}
				tmp = (UserClass*)(tmp->superClass);
			}while(tmp);
		illegalLocal(id,s);
		exit(1);
}


#endif /* USERCLASSP_HPP_ */
