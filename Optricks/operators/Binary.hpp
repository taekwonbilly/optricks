/*
 * Binary.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: Billy
 */

#ifndef BINARY_HPP_
#define BINARY_HPP_

#include "../language/class/AbstractClass.hpp"
#include "../language/class/builtin/IntClass.hpp"
#include "../language/class/builtin/VectorClass.hpp"
#include "../language/class/builtin/CStringClass.hpp"
#include "../language/evaluatable/CastEval.hpp"
#include "../language/data/literal/MathConstantLiteral.hpp"
#include "../language/data/ArrayData.hpp"
#include "../language/class/ClassLib.hpp"
#include "./Unary.hpp"
inline const AbstractClass* getBinopReturnType(PositionID filePos, const AbstractClass* cc, const AbstractClass* dd, const String operation){
	if(operation=="+"){
		if(cc->classType==CLASS_STR) return cc;
		if(dd->classType==CLASS_STR) return dd;

		if(cc->classType==CLASS_STRLITERAL || cc->classType==CLASS_CHAR
			|| dd->classType==CLASS_STRLITERAL || dd->classType==CLASS_CHAR){
			return &stringLiteralClass;
		}
		//if(cc->classType==CLASS_STRLITERAL || dd->classType==CLASS_STRLITERAL)
		//	return &stringClass;
	}
	switch(cc->classType){
	case CLASS_SCOPE:
		filePos.compilerError("Scope should never be instatiated");
		exit(1);
	case CLASS_CSTRING:{
		if(operation=="[]") return &charClass;
		else if(operation=="==" || operation=="!=") return &boolClass;
		else {
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			return &voidClass;
		}
	}
	case CLASS_ENUM:{
		if(operation=="==" || operation=="!=") return &boolClass;
		else {
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			return &voidClass;
		}
	}
	case CLASS_STRLITERAL:{
		if(operation=="[]") return &charClass;
		if(dd->classType==CLASS_CHAR || dd->classType==CLASS_STRLITERAL || dd->classType==CLASS_INTLITERAL
				|| dd->classType==CLASS_FLOATLITERAL || dd->classType==CLASS_MATHLITERAL)
			return cc;
		//else return &stringClass;
		filePos.compilerError("String binops not implemented");
		exit(1);
	}
	case CLASS_VECTOR:{
		//const VectorClass* vc = (const VectorClass*)cc;

		if(dd->classType!=CLASS_VECTOR){
			if(operation!="==" && operation!="!="){
			}
			filePos.error("Cannot have binary operation between vector and non-vector type");
		}
		if(operation=="==" || operation=="!=") return &boolClass;
		filePos.compilerError("Vector binops not implemented");
		exit(1);
		//return VectorClass::get(getBinopReturnType(filePos, vc->inner, operation), vc->classType);
	}
	case CLASS_INT:{
		switch(dd->classType){
		/*case CLASS_STR:
		case CLASS_STRLITERAL:{
			if(operation=="*") return &stringClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}*/
		case CLASS_INT:{
			const IntClass* max = (const IntClass*)cc;
			if(max->getWidth()<=((const IntClass*)dd)->getWidth()){
				max = (const IntClass*)dd;
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" || operation=="&" || operation=="|" || operation=="^"
							|| operation=="<<" || operation==">>" || operation==">>>") return max;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return &boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INTLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" || operation=="&" || operation=="|" || operation=="^"
							|| operation=="<<" || operation==">>" || operation==">>>") return cc;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
								|| operation=="==" || operation=="!=") return &boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOAT:{
			if(!cc->hasCast(dd)){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			} else return getBinopReturnType(filePos, dd, dd, operation);
		}
		case CLASS_COMPLEX:{
			return getBinopReturnType(filePos, ComplexClass::get((const IntClass*)cc), dd, operation);
		}
		case CLASS_FLOATLITERAL:{
			return dd;
		}
		case CLASS_MATHLITERAL:{
			filePos.error("Math literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_INTLITERAL:{
		switch(dd->classType){
		case CLASS_STR:
		case CLASS_STRLITERAL:{
			if(operation=="*") return dd;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_COMPLEX:
		case CLASS_FLOATLITERAL:
		case CLASS_FLOAT:
		case CLASS_INT:{
			assert(cc->hasCast(dd));
			return getBinopReturnType(filePos, dd, dd, operation);
		}
		case CLASS_INTLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" || operation=="&" || operation=="|" || operation=="^"
							|| operation=="<<" || operation==">>" || operation==">>>"){
				return &intLiteralClass;
			}
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return &boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_MATHLITERAL:{
			if(operation=="<" || operation==">" || operation=="==" || operation=="!=" || operation==">=" || operation=="<=") return &boolClass;
			else{
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_FLOAT:{
		switch(dd->classType){
		case CLASS_FLOAT:{
			const FloatClass* max = (const FloatClass*)cc;
			if(max->getWidth()<=((const FloatClass*)dd)->getWidth()){
				max = (const FloatClass*)dd;
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" ) return max;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return &boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INT:
		case CLASS_MATHLITERAL:
		case CLASS_INTLITERAL:
		case CLASS_FLOATLITERAL:{
			assert(dd->hasCast(cc));
			return getBinopReturnType(filePos, cc, cc, operation);
		}
		case CLASS_COMPLEX:{
			return getBinopReturnType(filePos, ComplexClass::get((const FloatClass*)cc), dd, operation);
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_FLOATLITERAL:{
		switch(dd->classType){
		case CLASS_FLOATLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" ) return dd;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return &boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOAT:{
			assert(cc->hasCast(dd));
			return getBinopReturnType(filePos, dd, dd, operation);
		}
		case CLASS_MATHLITERAL:
		case CLASS_INTLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" ) return cc;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return &boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_COMPLEX:{
			return getBinopReturnType(filePos, ComplexClass::get((const FloatLiteralClass*)cc), dd, operation);
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_MATHLITERAL:{
		switch(dd->classType){
		case CLASS_FLOAT:{
			assert(cc->hasCast(dd));
			return getBinopReturnType(filePos, dd, dd, operation);
		}
		case CLASS_FLOATLITERAL:
		case CLASS_INTLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%" || operation=="**" )
				return &floatLiteralClass;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<=" || operation=="==" || operation=="!=")
				return &boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_MATHLITERAL:{
			if(operation=="==" || operation=="!=") return &boolClass;
			if(dd==cc){
				if(operation=="-") return & intLiteralClass /* 0 */;
				else if(operation=="/") return &intLiteralClass /* 1 */;
			}
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(comp->innerClass->classType!=CLASS_FLOAT){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return comp;
			else if(operation=="==" || operation=="!=") return &boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_COMPLEX:{
		if(operation=="==" || operation=="!=") return &boolClass;
		return getMin(cc, dd,filePos);
	}
	case CLASS_BOOL:{
		if(operation=="||" || operation=="&&" || operation=="|" || operation=="&" || operation=="==" || operation=="!=")
			return cc;
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_STR:
	case CLASS_CHAR:{
		if(operation=="==" || operation=="!=") return &boolClass;
		if(operation=="+" || operation=="*") return &stringLiteralClass;
		//todo allow for string
		filePos.compilerError("todo -- string binops");
		exit(1);
	}
	case CLASS_CPOINTER:
	case CLASS_NULL:{
		if((dd->classType==CLASS_CSTRING || dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->classType==CLASS_NULL || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT)
			&& (operation=="==" || operation=="!=")) return &boolClass;
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_RATIONAL:{
		filePos.compilerError("Todo -- implement rationals");
		exit(1);
	}
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:{
		const TupleClass* t1 = (const TupleClass*)cc;
		const TupleClass* t2 = (const TupleClass*)dd;
		if(t1->innerTypes.size()!=t2->innerTypes.size() || (operation!="==" && operation!="!=")){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		for(unsigned i=0; i<t1->innerTypes.size(); i++){
			if(getBinopReturnType(filePos, t1->innerTypes[i], t2->innerTypes[i], operation)->classType!=CLASS_BOOL){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		if(t1->classType==CLASS_NAMED_TUPLE && t2->classType==CLASS_NAMED_TUPLE){
			const NamedTupleClass* nt1 = (const NamedTupleClass*)cc;
			const NamedTupleClass* nt2 = (const NamedTupleClass*)dd;

			for(unsigned i=0; i<t1->innerTypes.size(); i++){
				if(nt1->innerNames[i]!=nt2->innerNames[i]){
					filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
					exit(1);
				}
			}
		}
		return &boolClass;
	}
	case CLASS_CLASS:
	case CLASS_FUNC:{
		if(!(operation=="==" || operation=="!=") ||
				!(dd->classType==cc->classType|| dd->classType==CLASS_NULL || dd->classType==CLASS_CPOINTER)){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		return &boolClass;
	}
	case CLASS_ARRAY:{
		if(operation=="==" || operation=="!="){
			return &boolClass;
		}
		if(operation=="[]" && (dd->classType==CLASS_INT || dd->classType==CLASS_INTLITERAL)){
			auto T = ((const ArrayClass*)cc)->inner;
			if(T==nullptr){
				filePos.error("Array index out of bounds");
			}
			assert(T);
			return T;
		}
		if(operation=="[]="){
			auto T = ((const ArrayClass*)cc)->inner;
			if(T==nullptr){
				filePos.error("Must define inner class type before adding to empty array");
			}
			assert(T);
			return T;
		}
		filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
		exit(1);
	}
	case CLASS_GEN:
	case CLASS_SET:
	case CLASS_MAP:
	case CLASS_WRAPPER:
	case CLASS_VOID:{
		if(false) return cc;
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Cannot apply binary operations to reference or lazy classes");
		exit(1);
	}
	case CLASS_USER:{
		if(dd->classType==CLASS_NULL && (operation=="==" || operation=="!=")){
			return &boolClass;
		}
		const UserClass* uc = (const UserClass*)cc;
		return uc->getLocalFunction(filePos, ":"+operation, NO_TEMPLATE, std::vector<const AbstractClass*>({cc, dd}))->getSingleProto()->returnType;
	}
	}
}

inline const Data* getBinop(RData& r, PositionID filePos, const Data* value, const Evaluatable* ev, const String operation){
	const AbstractClass* cc = value->getReturnType();
	const AbstractClass* dd = ev->getReturnType();
	if(operation=="+"){
		if(cc->classType==CLASS_STR){
			filePos.error("Todo -- string binop 1");
		}
		if(dd->classType==CLASS_STR){
			filePos.error("Todo -- string binop 2");
		}
		if(cc->classType==CLASS_STRLITERAL || cc->classType==CLASS_CHAR
			|| dd->classType==CLASS_STRLITERAL || dd->classType==CLASS_CHAR){
			auto S = getPreop(r, filePos, ":str",value);
			auto T = getPreop(r, filePos, ":str",ev->evaluate(r));
			if(S->type==R_STR && T->type==R_STR){
				return new StringLiteral(((const StringLiteral*)S)->value+((const StringLiteral*)T)->value);
			}
		}
	}
	switch(cc->classType){
	case CLASS_SCOPE:
		filePos.compilerError("Scope should never be instantiated");
		exit(1);
		case CLASS_ENUM:{
			if(dd->classType!=CLASS_ENUM){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				return &VOID_DATA;
			}
			if(operation=="=="){
				return new ConstantData(r.builder.CreateICmpEQ(value->getValue(r, filePos), ev->evalV(r, filePos)), &boolClass);
			} else if(operation=="!="){
				return new ConstantData(r.builder.CreateICmpNE(value->getValue(r, filePos), ev->evalV(r, filePos)), &boolClass);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				return &VOID_DATA;
			}
		}
	case CLASS_STRLITERAL:{
		const StringLiteral* S = (const StringLiteral*)value;
		if(operation=="[]"){
			if(dd->classType!=CLASS_INT && dd->classType!=CLASS_INTLITERAL){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				return &VOID_DATA;
			}
			llvm::Value* V;
			if(dd->classType==CLASS_INT) V =
					r.builder.CreateSExtOrTrunc(ev->evalV(r, filePos),intClass.type);
			else{
				assert(dd->classType==CLASS_INTLITERAL);
				V = ev->evaluate(r)->castToV(r, &intClass, filePos);
			}
			StringLiteral* D = (StringLiteral*)value;
			if(D->value.size()==0){
				filePos.error("String index out of bounds");
				return &VOID_DATA;
			}
			if(auto C = llvm::dyn_cast<llvm::ConstantInt>(V)){
				auto tmp = C->getValue().getLimitedValue();
				if(tmp>= D->value.size()){
					filePos.error("String index out of bounds");
					return &VOID_DATA;
				}
				return new ConstantData(charClass.getValue(D->value[tmp]),&charClass);
			}
			llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
			llvm::BasicBlock* ErrorBB = r.CreateBlock("stringError",StartBB);
			llvm::BasicBlock* MergeBB = r.CreateBlock("stringMerge");
			auto Switch = r.builder.CreateSwitch(V, ErrorBB, D->value.size());
			r.builder.SetInsertPoint(MergeBB);
			auto PHI = r.builder.CreatePHI(CHARTYPE, D->value.size());
			for(unsigned i=0; i<D->value.size(); i++){
				llvm::BasicBlock* TmpBB = r.CreateBlock("stringPiece",StartBB);
				r.builder.SetInsertPoint(TmpBB);
				auto VAL = charClass.getValue(D->value[i]);
				PHI->addIncoming(VAL, TmpBB);
				r.builder.CreateBr(MergeBB);
				Switch->addCase(getInt32(i), TmpBB);
			}
			r.builder.SetInsertPoint(ErrorBB);
			llvm::SmallVector<llvm::Type*,1> t_args(1);
			t_args[0] = C_STRINGTYPE;
			auto CU = r.getExtern("printf", llvm::FunctionType::get(c_intClass.type, t_args,true));
			r.builder.CreateCall2(CU, r.getConstantCString("Illegal string index %d in "+str(D->value.size())), V);
			r.builder.SetInsertPoint(MergeBB);
			return new ConstantData(PHI, &charClass);
		} else if(operation=="+"){
			const auto T = getPreop(r, filePos, ":str",ev->evaluate(r));
			if(T->type==R_STR){
				return new StringLiteral(S->value+ ((const StringLiteral*)T)->value);
			}
			dd = T->getReturnType();
		} else if(operation=="*"){
			const Data* D = ev->evaluate(r);
			if(D->type==R_INT){
				const auto IL = (const IntLiteral*)D;
				if(mpz_sgn(IL->value)<0){
					filePos.error("Cannot multiply string by negative integer");
					exit(1);
				} else if(mpz_cmp_ui(IL->value, UINT_MAX)>0){
					filePos.error("Cannot multiply string by integer -- too large "+
							IL->toString());
					exit(1);
				}
				auto E = mpz_get_ui(IL->value)*S->value.size();
				String s(E,' ');
				for(unsigned i = 0; i<E; i++)
					s[i] = S->value[i % S->value.size()];
				return new StringLiteral(s);
			}
		}
		filePos.compilerError("TODO _STRING LITERAL");
		exit(0);
	}
	case CLASS_VECTOR:{
		//const VectorClass* vc = (const VectorClass*)cc;
		if(dd->classType!=CLASS_VECTOR){
			if(operation!="==" && operation!="!="){
			}
			filePos.error("Cannot have binary operation between vector and non-vector type");
		}
		//if(operation=="==" || operation=="!=") return boolClass;
		filePos.compilerError("Vector binops not implemented");
		exit(1);
		//return VectorClass::get(getBinopReturnType(filePos, vc->inner, operation), vc->classType);
	}
	case CLASS_INT:{
		switch(dd->classType){
		case CLASS_INT:{
			const IntClass* max = (const IntClass*)cc;
			if(max->getWidth()<=((const IntClass*)dd)->getWidth()){
				max = (const IntClass*)dd;
			}
			if(operation=="+") return new ConstantData(r.builder.CreateAdd(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="-") return new ConstantData(r.builder.CreateSub(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="*") return new ConstantData(r.builder.CreateMul(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="/") return new ConstantData(r.builder.CreateSDiv(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="%") return new ConstantData(r.builder.CreateSRem(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="&")	return new ConstantData(r.builder.CreateAnd(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="|")	return new ConstantData(r.builder.CreateOr(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="^")	return new ConstantData(r.builder.CreateXor(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="<<") return new ConstantData(r.builder.CreateShl(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation==">>") return new ConstantData(r.builder.CreateAShr(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation==">>>") return new ConstantData(r.builder.CreateLShr(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation==">") return new ConstantData(r.builder.CreateICmpSGT(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation==">=") return new ConstantData(r.builder.CreateICmpSGE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="<") return new ConstantData(r.builder.CreateICmpSLT(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="<=") return new ConstantData(r.builder.CreateICmpSLE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="!=") return new ConstantData(r.builder.CreateICmpNE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="**"){
				filePos.compilerError("Todo -- integer pow");
				exit(1);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INTLITERAL:{
			return getBinop(r, filePos, value, ev->evaluate(r)->castTo(r, cc, filePos), operation);
		}
		case CLASS_FLOAT:{
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(cc->hasCast(comp->innerClass)) {
				return getBinop(r, filePos, value->castTo(r, comp, filePos), ev, operation);
			}
			if(comp->innerClass->hasCast(cc)){
				comp = ComplexClass::get((const RealClass*)cc);
				return getBinop(r, filePos, value->castTo(r, comp, filePos), new CastEval(ev, comp, filePos), operation);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOATLITERAL:{
			filePos.error("Floating literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		case CLASS_MATHLITERAL:{
			filePos.error("Math literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_INTLITERAL:{
		const IntLiteral* il = (const IntLiteral*)value;
		switch(dd->classType){
		case CLASS_INT:{
			const IntClass* R = (const IntClass*)dd;
			if(!R->hasFit(il->value)) filePos.warning("Cannot fit integer literal "+il->toString()+" in integer type "+R->getName());
			return getBinop(r, filePos, value->castTo(r, R, filePos), ev, operation);
		}
		case CLASS_FLOAT:{
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_INTLITERAL:{
			assert(value->type==R_INT);
			auto tmp = ev->evaluate(r);
			assert(tmp->type==R_INT);
			const auto& tmp1 = ((const IntLiteral*) value)->value;
			const auto& tmp2 = ((const IntLiteral*) tmp)->value;
			if(operation=="=="){
				return new ConstantData(BoolClass::getValue(mpz_cmp(tmp1, tmp2)==0), &boolClass);
			} else if(operation=="!="){
				return new ConstantData(BoolClass::getValue(mpz_cmp(tmp1, tmp2)!=0), &boolClass);
			} else if(operation=="<="){
				return new ConstantData(BoolClass::getValue(mpz_cmp(tmp1, tmp2)<=0), &boolClass);
			} else if(operation=="<"){
				return new ConstantData(BoolClass::getValue(mpz_cmp(tmp1, tmp2)< 0), &boolClass);
			} else if(operation==">="){
				return new ConstantData(BoolClass::getValue(mpz_cmp(tmp1, tmp2)>=0), &boolClass);
			} else if(operation==">"){
				return new ConstantData(BoolClass::getValue(mpz_cmp(tmp1, tmp2)> 0), &boolClass);
			}
			if(operation=="+"){
				auto ret = new IntLiteral(0,0,0);
				mpz_add(ret->value, tmp1, tmp2);
				return ret;
			} else if(operation=="-"){
				auto ret = new IntLiteral(0,0,0);
				mpz_sub(ret->value, tmp1, tmp2);
				return ret;
			} else if(operation=="*"){
				auto ret = new IntLiteral(0,0,0);
				mpz_mul(ret->value, tmp1, tmp2);
				return ret;
			} else if(operation=="/"){
				//todo should round towards zero
				if(mpz_sgn(tmp2)==0)
					filePos.error("Divide by integer 0");
				auto ret = new IntLiteral(0,0,0);
				mpz_div(ret->value, tmp1, tmp2);
				return ret;
			} else if(operation=="%"){
				if(mpz_sgn(tmp2)==0)
					filePos.error("Divide by integer 0");
				auto ret = new IntLiteral(0,0,0);
				mpz_mod(ret->value, tmp1, tmp2);
				return ret;
				filePos.warning("Literal modolo gives different results from integer modulo");
			} else if(operation=="&"){
				auto ret = new IntLiteral(0,0,0);
				mpz_and(ret->value, tmp1, tmp2);
				return ret;
			} else if(operation=="|"){
				auto ret = new IntLiteral(0,0,0);
				mpz_ior(ret->value, tmp1, tmp2);
				return ret;
			} else if(operation=="^"){
				auto ret = new IntLiteral(0,0,0);
				mpz_xor(ret->value, tmp1, tmp2);
				return ret;
			} else if(operation=="<<" || operation==">>" || operation==">>>"){
				filePos.compilerError("TODO -- bitshifts");
			} else if(operation=="**"){
				auto s = mpz_sgn(tmp2);
				if(s<0){
					return & ZERO_LITERAL;
				}
				else if(s==0){
					return & ONE_LITERAL;
				}
				else if(mpz_cmp_ui(tmp1, 1)==0 || mpz_cmp_ui(tmp1,0)==0){
					return value;
				}
				else if(mpz_cmp_si(tmp1,-1)==0){
					if(mpz_even_p(tmp2)==0){
						//is odd
						return value;
					} else{
						// is even
						return & ONE_LITERAL;
					}
				}
				else {
					if(mpz_cmp_ui(tmp2, UINT_MAX)<=0){
						auto ret = new IntLiteral(0,0,0);
						mpz_pow_ui(ret->value, tmp1, mpz_get_ui(tmp2));
						return ret;
					} else{
						filePos.compilerError("Result of integer exponentiation cannot fit in memory (perhaps use floating-point instead)");
						exit(1);
					}
				}
			} else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_COMPLEX:{
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_FLOATLITERAL:{
			const auto& it1 = ((const IntLiteral*) value)->value;
			FloatLiteral fl(it1);
			auto tmp =  getBinop(r, filePos, &fl, ev, operation);
			return tmp;
		}
		case CLASS_MATHLITERAL:{
			const auto& it1 = ((const IntLiteral*) value)->value;
			FloatLiteral f1(it1);
			FloatLiteral f2(0,0,0);
			switch(((const MathConstantLiteral*) ev->evaluate(r))->mathType.mathType){
				case MATH_PI: mpfr_const_pi(f2.value, MPFR_RNDN); break;
				case MATH_E:{
					mpfr_t ONE;
					mpfr_init(ONE);
					mpfr_set_ui(ONE, 1,MPFR_RNDN);
					mpfr_exp(f2.value, ONE,MPFR_RNDN);
					break;
				}
				case MATH_EULER_MASC: mpfr_const_euler(f2.value, MPFR_RNDN); break;
				case MATH_LN2: mpfr_const_log2(f2.value, MPFR_RNDN); break;
				case MATH_CATALAN: mpfr_const_catalan(f2.value, MPFR_RNDN); break;
			}
			auto tmp = getBinop(r, filePos, &f1, &f2, operation);
			return tmp;
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_FLOAT:{
		switch(dd->classType){
		case CLASS_FLOAT:{
			const FloatClass* max = (const FloatClass*)cc;
			if(max->getWidth()<=((const FloatClass*)dd)->getWidth()){
				max = (const FloatClass*)dd;
			}
			if(operation=="+") return new ConstantData(r.builder.CreateFAdd(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="-") return new ConstantData(r.builder.CreateFSub(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="*") return new ConstantData(r.builder.CreateFMul(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="/") return new ConstantData(r.builder.CreateFDiv(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="%") return new ConstantData(r.builder.CreateFRem(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation==">") return new ConstantData(r.builder.CreateFCmpOGT(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation==">=") return new ConstantData(r.builder.CreateFCmpOGE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="<") return new ConstantData(r.builder.CreateFCmpOLT(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="<=") return new ConstantData(r.builder.CreateFCmpOLE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="==") return new ConstantData(r.builder.CreateFCmpOEQ(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="!=") return new ConstantData(r.builder.CreateFCmpONE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), &boolClass);
			else if(operation=="**"){
				auto INTR = llvm::Intrinsic::getDeclaration(r.lmod, llvm::Intrinsic::pow, llvm::SmallVector<llvm::Type*,1>(1,max->type));
				return new ConstantData(r.builder.CreateCall2(INTR, value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INT:
		case CLASS_INTLITERAL:{
			if(operation=="**" && dd->hasCast(&intClass)){
				auto INTR = llvm::Intrinsic::getDeclaration(r.lmod, llvm::Intrinsic::powi, llvm::SmallVector<llvm::Type*,1>(1,cc->type));
				return new ConstantData(r.builder.CreateCall2(INTR, value->getValue(r, filePos), ev->evaluate(r)->castToV(r, &intClass, filePos)), cc);
			} else return getBinop(r, filePos, value, new CastEval(ev, cc, filePos), operation);
		}
		case CLASS_MATHLITERAL:
		case CLASS_FLOATLITERAL:{
			CastEval CE(ev,cc,filePos);
			return getBinop(r, filePos, value, &CE, operation);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(cc->hasCast(comp->innerClass)) {
				return getBinop(r, filePos, value->castTo(r, comp, filePos), ev, operation);
			}
			if(comp->innerClass->hasCast(cc)){
				comp = ComplexClass::get((const RealClass*)cc);
				return getBinop(r, filePos, value->castTo(r, comp, filePos), new CastEval(ev, comp, filePos), operation);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_FLOATLITERAL:{
		switch(dd->classType){
		case CLASS_FLOATLITERAL:{
			assert(value->type==R_FLOAT);
			auto tmp = ev->evaluate(r);
			assert(tmp->type==R_FLOAT);
			const auto& tmp1 = ((const FloatLiteral*) value)->value;
			const auto& tmp2 = ((const FloatLiteral*) tmp)->value;
			if(operation=="=="){
				return new ConstantData(BoolClass::getValue(mpfr_equal_p(tmp1, tmp2)), &boolClass);
			} else if(operation=="!="){
				return new ConstantData(BoolClass::getValue(!mpfr_equal_p(tmp1, tmp2) && !mpfr_unordered_p(tmp1,tmp2)), &boolClass);
			} else if(operation=="<="){
				return new ConstantData(BoolClass::getValue(mpfr_lessequal_p(tmp1, tmp2)), &boolClass);
			} else if(operation=="<"){
				return new ConstantData(BoolClass::getValue(mpfr_less_p(tmp1, tmp2)), &boolClass);
			} else if(operation==">="){
				return new ConstantData(BoolClass::getValue(mpfr_greaterequal_p(tmp1, tmp2)), &boolClass);
			} else if(operation==">"){
				return new ConstantData(BoolClass::getValue(mpfr_greater_p(tmp1, tmp2)), &boolClass);
			}
			auto ret = new FloatLiteral(0,0,0);
			if(operation=="+"){
				mpfr_add(ret->value, tmp1, tmp2, MPFR_RNDN);
			} else if(operation=="-"){
				mpfr_sub(ret->value, tmp1, tmp2, MPFR_RNDN);
			} else if(operation=="*"){
				mpfr_mul(ret->value, tmp1, tmp2, MPFR_RNDN);
			} else if(operation=="/"){
				mpfr_div(ret->value, tmp1, tmp2, MPFR_RNDN);
			} else if(operation=="%"){
				mpfr_remainder(ret->value, tmp1, tmp2, MPFR_RNDN);
			} else if(operation=="**"){
				mpfr_pow(ret->value, tmp1, tmp2, MPFR_RNDN);
			}
			return ret;
		}
		case CLASS_FLOAT:{
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_INTLITERAL:{
			const auto& it1 = ((const IntLiteral*) ev->evaluate(r))->value;
			FloatLiteral fl(it1);
			auto tmp =  getBinop(r, filePos, value, &fl, operation);
			return tmp;
		}
		case CLASS_COMPLEX:{
			return getBinop(r, filePos, value->castTo(r, getMin(cc,dd,filePos), filePos), ev, operation);
		}
		case CLASS_MATHLITERAL:{
			FloatLiteral f2(0,0,0);
			switch(((const MathConstantLiteral*) ev->evaluate(r))->mathType.mathType){
				case MATH_PI: mpfr_const_pi(f2.value, MPFR_RNDN); break;
				case MATH_E:{
					mpfr_t ONE;
					mpfr_init(ONE);
					mpfr_set_ui(ONE, 1,MPFR_RNDN);
					mpfr_exp(f2.value, ONE,MPFR_RNDN);
					break;
				}
				case MATH_EULER_MASC: mpfr_const_euler(f2.value, MPFR_RNDN); break;
				case MATH_LN2: mpfr_const_log2(f2.value, MPFR_RNDN); break;
				case MATH_CATALAN: mpfr_const_catalan(f2.value, MPFR_RNDN); break;
			}
			auto tmp = getBinop(r, filePos, value, &f2, operation);
			return tmp;
		}
		case CLASS_INT:
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_MATHLITERAL:{
		switch(dd->classType){
		case CLASS_FLOAT:{
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_INTLITERAL:
		case CLASS_FLOATLITERAL:{
			FloatLiteral f2(0,0,0);
			switch(((const MathConstantLiteral*) value)->mathType.mathType){
				case MATH_PI: mpfr_const_pi(f2.value, MPFR_RNDN); break;
				case MATH_E:{
					mpfr_t ONE;
					mpfr_init(ONE);
					mpfr_set_ui(ONE, 1,MPFR_RNDN);
					mpfr_exp(f2.value, ONE,MPFR_RNDN);
					break;
				}
				case MATH_EULER_MASC: mpfr_const_euler(f2.value, MPFR_RNDN); break;
				case MATH_LN2: mpfr_const_log2(f2.value, MPFR_RNDN); break;
				case MATH_CATALAN: mpfr_const_catalan(f2.value, MPFR_RNDN); break;
			}
			auto tmp = getBinop(r, filePos, &f2, ev,operation);
			return tmp;
		}
		case CLASS_MATHLITERAL:{
			if(operation=="=="){
				return new ConstantData(BoolClass::getValue(cc==dd), &boolClass);
			} else if(operation=="!="){
				return new ConstantData(BoolClass::getValue(cc!=dd), &boolClass);
			}
			if(dd==cc){
				if(operation=="-") return & ZERO_LITERAL;
				else if(operation=="/") return & ONE_LITERAL;
			}
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(comp->innerClass->classType!=CLASS_FLOAT){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_COMPLEX:{
		const ComplexClass* comp = (const ComplexClass*)getMin(cc, dd, filePos);
		switch(comp->innerClass->classType){
		case CLASS_FLOATLITERAL:
		case CLASS_INTLITERAL:{
			const Data* L = value->castTo(r, comp, filePos);
			const Data* R = ev->evaluate(r)->castTo(r, comp, filePos);
			assert(L->type==R_IMAG);
			assert(R->type==R_IMAG);
			const ImaginaryLiteral* IL = (const ImaginaryLiteral*)L;
			const ImaginaryLiteral* IR = (const ImaginaryLiteral*)R;
			const Data* LR = (IL->real)?IL->real:(&ZERO_LITERAL);
			const Data* LI = IL->imag;
			const Data* RR = (IR->real)?IR->real:(&ZERO_LITERAL);
			const Data* RI = IR->imag;
			if(operation=="+"){
				return new ImaginaryLiteral(
						getBinop(r, filePos, LR, RR, "+"),
						getBinop(r, filePos, LI, RI, "+")
				);
			} else if(operation=="-"){
				return new ImaginaryLiteral(
						getBinop(r, filePos, LR, RR, "-"),
						getBinop(r, filePos, LI, RI, "-")
				);
			} else if(operation=="*"){
				return new ImaginaryLiteral(
						getBinop(r, filePos, getBinop(r, filePos, LR, RR,"*"), getBinop(r, filePos, LI, RI,"*"), "-"),
						getBinop(r, filePos, getBinop(r, filePos, LR, RI,"*"), getBinop(r, filePos, LI, RR,"*"), "+")
				);
			} else if(operation=="=="){
				auto re = getBinop(r, filePos, LR, RR, "==")->getValue(r, filePos);
				auto im = getBinop(r, filePos, LI, RI, "==")->getValue(r, filePos);
				return new ConstantData(r.builder.CreateAnd(re, im), &boolClass);
			} else if(operation=="!="){
				auto re = getBinop(r, filePos, LR, RR, "!=")->getValue(r, filePos);
				auto im = getBinop(r, filePos, LI, RI, "!=")->getValue(r, filePos);
				return new ConstantData(r.builder.CreateOr(re, im), &boolClass);
			} else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INT:
		case CLASS_FLOAT:{//todo
			llvm::Value* L = value->castToV(r, comp, filePos);
			llvm::Value* R = ev->evaluate(r)->castToV(r, comp, filePos);
			if(operation=="+"){
				if(comp->innerClass->classType==CLASS_INT)
					return new ConstantData(r.builder.CreateAdd(L,R), comp);
				else return new ConstantData(r.builder.CreateFAdd(L,R),comp);
			} else if(operation=="-"){
				if(comp->innerClass->classType==CLASS_INT)
					return new ConstantData(r.builder.CreateSub(L,R), comp);
				else return new ConstantData(r.builder.CreateFSub(L,R),comp);
			} else if(operation=="*"){
				llvm::Value* DOT =
					(comp->innerClass->classType==CLASS_INT)?
						r.builder.CreateMul(L, R):
						r.builder.CreateFMul(L,R);
				llvm::Value* NR =
						(comp->innerClass->classType==CLASS_INT)?
							r.builder.CreateSub(r.builder.CreateExtractElement(DOT, getInt32(0)), r.builder.CreateExtractElement(DOT, getInt32(1))):
							r.builder.CreateFSub(r.builder.CreateExtractElement(DOT, getInt32(0)), r.builder.CreateExtractElement(DOT, getInt32(1)));
				llvm::Value* LR = r.builder.CreateExtractElement(L, getInt32(0));
				llvm::Value* LI = r.builder.CreateExtractElement(L, getInt32(1));
				llvm::Value* RR = r.builder.CreateExtractElement(R, getInt32(0));
				llvm::Value* RI = r.builder.CreateExtractElement(R, getInt32(1));
				llvm::Value* I1 =
						(comp->innerClass->classType==CLASS_INT)?
							r.builder.CreateMul(LR, RI):
							r.builder.CreateFMul(LR,RI);
				llvm::Value* I2 =
						(comp->innerClass->classType==CLASS_INT)?
							r.builder.CreateMul(LI, RR):
							r.builder.CreateFMul(LI,RR);
				llvm::Value* NI =
						(comp->innerClass->classType==CLASS_INT)?
							r.builder.CreateAdd(I1, I2):
							r.builder.CreateFAdd(I1, I2);

				llvm::Value* V = llvm::UndefValue::get(comp->type);
				V = r.builder.CreateInsertElement(V, NR, getInt32(0));
				V = r.builder.CreateInsertElement(V, NI, getInt32(1));
				return new ConstantData(V,comp);
			} else if(operation=="=="){
				llvm::Value* LR = r.builder.CreateExtractElement(L, getInt32(0));
				llvm::Value* LI = r.builder.CreateExtractElement(L, getInt32(1));
				llvm::Value* RR = r.builder.CreateExtractElement(R, getInt32(0));
				llvm::Value* RI = r.builder.CreateExtractElement(R, getInt32(1));
				llvm::Value* re = (comp->innerClass->classType==CLASS_INT)?
						r.builder.CreateICmpEQ(LR, RR):r.builder.CreateFCmpOEQ(LR, RR);
				llvm::Value* im = (comp->innerClass->classType==CLASS_INT)?
						r.builder.CreateICmpEQ(LI, RI):r.builder.CreateFCmpOEQ(LI, RI);
				return new ConstantData(r.builder.CreateAnd(re, im), &boolClass);
			} else if(operation=="!="){
				llvm::Value* LR = r.builder.CreateExtractElement(L, getInt32(0));
				llvm::Value* LI = r.builder.CreateExtractElement(L, getInt32(1));
				llvm::Value* RR = r.builder.CreateExtractElement(R, getInt32(0));
				llvm::Value* RI = r.builder.CreateExtractElement(R, getInt32(1));
				llvm::Value* re = (comp->innerClass->classType==CLASS_INT)?
						r.builder.CreateICmpEQ(LR, RR):r.builder.CreateFCmpONE(LR, RR);
				llvm::Value* im = (comp->innerClass->classType==CLASS_INT)?
						r.builder.CreateICmpEQ(LI, RI):r.builder.CreateFCmpONE(LI, RI);
				return new ConstantData(r.builder.CreateOr(re, im), &boolClass);
			} else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		default:{
			filePos.compilerError("complex - this should never happen!");
			exit(1);
		}
		}
		break;
	}
	case CLASS_BOOL:{
		if(dd->classType!=CLASS_BOOL){
			filePos.compilerError("complex - this should never happen!");
			exit(1);
		}
		if(operation=="&") return new ConstantData(r.builder.CreateAnd(value->getValue(r, filePos), ev->evaluate(r)->getValue(r, filePos)), cc);
		else if(operation=="|") return new ConstantData(r.builder.CreateOr(value->getValue(r, filePos), ev->evaluate(r)->getValue(r, filePos)), cc);
		else if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(value->getValue(r, filePos), ev->evaluate(r)->getValue(r, filePos)), cc);
		else if(operation=="!=") return new ConstantData(r.builder.CreateICmpNE(value->getValue(r, filePos), ev->evaluate(r)->getValue(r, filePos)), cc);
		else if(operation=="||"){
			llvm::Value* Start = value->castToV(r, &boolClass, filePos);

			llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
			llvm::BasicBlock* ElseBB;
			llvm::BasicBlock* MergeBB;
			if(auto c = llvm::dyn_cast<llvm::ConstantInt>(Start)){
				if(!c->isOne()) return ev->evaluate(r)->castTo(r,&boolClass,filePos);
				else return ConstantData::getTrue();
			}
			else{
				ElseBB = r.CreateBlock("else",StartBB);
				MergeBB = r.CreateBlock("ifcont",StartBB);
				r.builder.CreateCondBr(Start, MergeBB, ElseBB);
			}
			StartBB = r.builder.GetInsertBlock();
			r.builder.SetInsertPoint(ElseBB);
			llvm::Value* fin = ev->evaluate(r)->castToV(r,&boolClass,filePos);
			r.builder.CreateBr(MergeBB);
			ElseBB = r.builder.GetInsertBlock();
			//a.addPred(MergeBB,ElseBB);
			r.builder.SetInsertPoint(MergeBB);
			llvm::PHINode *PN = r.builder.CreatePHI(BOOLTYPE, 2,"iftmp");
			PN->addIncoming(Start, StartBB);
			PN->addIncoming(fin, ElseBB);
			return new ConstantData(PN, &boolClass);
		}
		else if(operation=="&&"){
			llvm::Value* Start = value->castToV(r, &boolClass, filePos);
			llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
			llvm::BasicBlock *ElseBB;
			llvm::BasicBlock *MergeBB;
			if(auto c = llvm::dyn_cast<llvm::ConstantInt>(Start)){
				if(c->isOne()) return ev->evaluate(r)->castTo(r,&boolClass,filePos);
				else return ConstantData::getFalse();
			}
			else{
				ElseBB = r.CreateBlock("else",StartBB);
				MergeBB = r.CreateBlock("ifcont",StartBB);
				r.builder.CreateCondBr(Start, ElseBB, MergeBB);
			}
			r.builder.SetInsertPoint(ElseBB);
			llvm::Value* fin = ev->evaluate(r)->castToV(r,&boolClass,filePos);
			//TODO can allow check if right is constant
			r.builder.CreateBr(MergeBB);
			ElseBB = r.builder.GetInsertBlock();
			//a.addPred(MergeBB,ElseBB);
			r.builder.SetInsertPoint(MergeBB);
			llvm::PHINode *PN = r.builder.CreatePHI(BOOLTYPE, 2,"iftmp");
			PN->addIncoming(Start, StartBB);
			PN->addIncoming(fin, ElseBB);
			return new ConstantData(PN, &boolClass);
		}
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_STR:
	case CLASS_CHAR:{
		auto C = value->getValue(r,filePos);
		switch(dd->classType){
		case CLASS_CHAR:
			if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(C, ev->evalV(r,filePos)),&boolClass);
			else if(operation=="!=") return new ConstantData(r.builder.CreateICmpNE(C, ev->evalV(r,filePos)),&boolClass);
			else if(operation=="+"){
				auto D = ev->evalV(r, filePos);
				if(auto CC = llvm::dyn_cast<llvm::ConstantInt>(C)){
					if(auto DD = llvm::dyn_cast<llvm::ConstantInt>(D)){
						return new StringLiteral(String(1,(char)(CC->getLimitedValue()))+String(1,(char) DD->getLimitedValue()));
					}
				}
			}
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
			break;
		case CLASS_INT:
		case CLASS_INTLITERAL:
			if(operation=="*"){
				if(auto CC = llvm::dyn_cast<llvm::ConstantInt>(C)){
					if(auto DD = llvm::dyn_cast<llvm::ConstantInt>(ev->evaluate(r)->castToV(r,&longClass,filePos))){
						return new StringLiteral(String(DD->getLimitedValue(),(char) CC->getLimitedValue()));
					}
				}
			}
		default:
			if(operation=="+"){
				if(auto CC = llvm::dyn_cast<llvm::ConstantInt>(C)){
					auto S = getPreop(r, filePos,":str",ev->evaluate(r));
					if(S->type==R_STR){
						return new StringLiteral(String(1, (char) CC->getLimitedValue())+((const StringLiteral*)S)->value);
					}
				}
			}
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		break;
	}
	case CLASS_NULL:{
		if((dd->classType==CLASS_CSTRING || dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->classType==CLASS_NULL || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT)
			&& (operation=="==" || operation=="!=")){
			auto T = ev->evalV(r, filePos);
			assert(llvm::dyn_cast<llvm::PointerType>(T->getType()));
			if(operation=="==") return new ConstantData(
					r.builder.CreateIsNull(T), &boolClass);
			else return new ConstantData(r.builder.CreateIsNotNull(T), &boolClass);
		}
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_CPOINTER:{
		if(!( operation=="==" || operation=="!=")){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		auto T = value->getValue(r, filePos);
		llvm::Value* NU;
		if(dd->classType==CLASS_NULL){
			assert(llvm::dyn_cast<llvm::PointerType>(T->getType()));
			NU = llvm::ConstantPointerNull::get((llvm::PointerType*) T->getType());
		} else if(dd->classType==CLASS_CSTRING || dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT){
			NU = ev->evaluate(r)->castToV(r, cc, filePos);
		} else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(T,NU), &boolClass);
		else return new ConstantData(r.builder.CreateICmpNE(T,NU), &boolClass);
	}
	case CLASS_RATIONAL:{
		filePos.compilerError("Todo -- implement rationals");
		exit(1);
	}
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:{
		const TupleClass* t1 = (const TupleClass*)cc;
		const TupleClass* t2 = (const TupleClass*)dd;
		if(t1->innerTypes.size()!=t2->innerTypes.size() || (operation!="==" && operation!="!=")){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		const NamedTupleClass* nt1=(t1->classType==CLASS_NAMED_TUPLE)?((const NamedTupleClass*)cc):nullptr;
		const NamedTupleClass* nt2=(t2->classType==CLASS_NAMED_TUPLE)?((const NamedTupleClass*)dd):nullptr;
		if(nt1 && nt2){
			for(unsigned i=0; i<t1->innerTypes.size(); i++){
				if(nt1->innerNames[i]!=nt2->innerNames[i]){
					filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
					exit(1);
				}
			}
		}
		const Data* d = ev->evaluate(r);
		llvm::Value* V = llvm::ConstantInt::getTrue(llvm::getGlobalContext());
		for(unsigned i=0; i<t1->innerTypes.size(); i++){
			auto a1 = cc->getLocalData(r, filePos, (nt1)?(nt1->innerNames[i]):("_"+str(i)),value);
			auto a2 = dd->getLocalData(r, filePos, (nt2)?(nt2->innerNames[i]):("_"+str(i)),d);
			auto M = getBinop(r, filePos, a1, a2, operation);
			V = r.builder.CreateAnd(V,M->castToV(r, &boolClass, filePos));
		}
		return new ConstantData(V, &boolClass);
	}
	case CLASS_CLASS:
	case CLASS_FUNC:{
		if(!(operation=="==" || operation=="!=") ||
				!(dd->classType==cc->classType|| dd->classType==CLASS_NULL || dd->classType==CLASS_CPOINTER)){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		auto T = value->getValue(r, filePos);
		llvm::Value* NU;
		if(dd->classType==CLASS_NULL){
			assert(llvm::dyn_cast<llvm::PointerType>(T->getType()));
			NU = llvm::ConstantPointerNull::get((llvm::PointerType*) T->getType());
		} else if(dd->classType==cc->classType || dd->classType==CLASS_CPOINTER || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT){
			if(dd->classType==CLASS_FUNC && dd!=cc){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
			NU = ev->evaluate(r)->castToV(r, cc, filePos);
		} else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(T,NU), &boolClass);
		else return new ConstantData(r.builder.CreateICmpNE(T,NU), &boolClass);
	}
	case CLASS_CSTRING:{
		//TODO
		if(operation=="[]"){
			llvm::Value* V;
			if(dd->classType==CLASS_INT) V =
					r.builder.CreateSExtOrTrunc(ev->evalV(r, filePos),intClass.type);
			else{
				assert(dd->classType==CLASS_INTLITERAL);
				V = ev->evaluate(r)->castToV(r, &intClass, filePos);
			}
			llvm::Value* A = value->getValue(r,filePos);

			llvm::Value* I = r.builder.CreateGEP(A, V);
			return new LocationData(new StandardLocation(I), &charClass);
		}
		else if(operation=="==" || operation=="!="){
			ev->evaluate(r);
			if(dd->classType==CLASS_NULL){
				if(operation=="==")
					return new ConstantData(r.builder.CreateIsNull(value->getValue(r, filePos)),&boolClass);
				else
					return new ConstantData(r.builder.CreateIsNotNull(value->getValue(r, filePos)),&boolClass);
			}
			filePos.compilerError("TODO cstring"+operation);
			exit(1);
		}
		else {
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			return &voidClass;
		}
	}
	case CLASS_ARRAY:{

		if(operation=="==" || operation=="!="){
			if(!(dd->classType==cc->classType|| dd->classType==CLASS_NULL || dd->classType==CLASS_CPOINTER)){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
			auto T = value->getValue(r, filePos);
			llvm::Value* NU;
			if(dd->classType==CLASS_NULL){
				assert(llvm::dyn_cast<llvm::PointerType>(T->getType()));
				NU = llvm::ConstantPointerNull::get((llvm::PointerType*) T->getType());
			} else if(dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT){
				NU = ev->evaluate(r)->castToV(r, cc, filePos);
			} else{
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
			if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(T,NU), &boolClass);
			else return new ConstantData(r.builder.CreateICmpNE(T,NU), &boolClass);
		}
		if(operation=="[]"){
			llvm::Value* V;
			ArrayClass* AC = (ArrayClass*) cc;
			if(dd->classType==CLASS_INT) V =
					r.builder.CreateSExtOrTrunc(ev->evalV(r, filePos),intClass.type);
			else{
				assert(dd->classType==CLASS_INTLITERAL);
				V = ev->evaluate(r)->castToV(r, &intClass, filePos);
			}
			//todo
			if(value->type==R_ARRAY){
				ArrayData* D = (ArrayData*)value;
				if(D->inner.size()==0){
					filePos.error("Array index out of bounds");
					return &VOID_DATA;
				}
				if(auto C = llvm::dyn_cast<llvm::ConstantInt>(V)){
					auto tmp = C->getValue().getLimitedValue();
					if(tmp>= D->inner.size()){
						filePos.error("Array index out of bounds");
						return &VOID_DATA;
					}
					return D->inner[tmp];
				}
				const AbstractClass* RT = ((ArrayClass*)D->getReturnType())->inner;
				llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
				llvm::BasicBlock* ErrorBB = r.CreateBlock("arrayError",StartBB);
				llvm::BasicBlock* MergeBB = r.CreateBlock("arrayMerge");
				auto Switch = r.builder.CreateSwitch(V, ErrorBB, D->inner.size());
				r.builder.SetInsertPoint(MergeBB);
				auto PHI = r.builder.CreatePHI(RT->type, D->inner.size());
				for(unsigned i=0; i<D->inner.size(); i++){
					llvm::BasicBlock* TmpBB = r.CreateBlock("arrayPiece",StartBB);
					r.builder.SetInsertPoint(TmpBB);
					auto VAL = D->inner[i]->getValue(r, filePos);
					PHI->addIncoming(VAL, TmpBB);
					r.builder.CreateBr(MergeBB);
					Switch->addCase(getInt32(i), TmpBB);
				}
				r.builder.SetInsertPoint(ErrorBB);
				llvm::SmallVector<llvm::Type*,1> t_args(1);
				t_args[0] = C_STRINGTYPE;
				auto CU = r.getExtern("printf", llvm::FunctionType::get(c_intClass.type, t_args,true));
				r.builder.CreateCall2(CU, r.getConstantCString("Illegal array index %d in "+str(D->inner.size())), V);
				r.error("");
				r.builder.SetInsertPoint(MergeBB);
				return new ConstantData(PHI, RT);
			} else {
				llvm::Value* A = value->getValue(r,filePos);
				llvm::Value* I = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(A, 0,3));
				I = r.builder.CreateGEP(I, V);
				return new LocationData(new StandardLocation(I), AC->inner);
			}
		} else if(operation=="[]="){
			ArrayClass* AC = (ArrayClass*) cc;
			llvm::Value* INTO = ev->evaluate(r)->castToV(r, AC->inner, filePos);
			llvm::Value* V = value->getValue(r, filePos);
			assert(V);
			auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
			auto LENGTH = r.builder.CreateLoad(LENGTH_P);
			auto ALLOC_P = r.builder.CreateConstGEP2_32(V, 0, 2);
			auto ALLOC = r.builder.CreateLoad(ALLOC_P);
			auto DATA_P = r.builder.CreateConstGEP2_32(V, 0, 3);
			auto REALLOC = r.CreateBlockD("realloc", r.builder.GetInsertBlock()->getParent());
			auto ADD = r.CreateBlockD("add", r.builder.GetInsertBlock()->getParent());
			r.builder.CreateCondBr(r.builder.CreateICmpSGT(ALLOC,LENGTH),ADD,REALLOC);
			r.builder.SetInsertPoint(REALLOC);
			llvm::SmallVector<llvm::Type*,2> args(2);
			args[0] = C_POINTERTYPE;
			args[1] = C_SIZETTYPE;
			llvm::FunctionType *FT = llvm::FunctionType::get(C_POINTERTYPE, args, false);
			auto R_FUNC = r.getExtern("realloc",FT);
			llvm::Value* NEWLEN = r.builder.CreateMul(r.builder.CreateAdd(getInt32(1), LENGTH),getInt32(2));
			auto IP = r.builder.CreatePointerCast(r.builder.CreateLoad(DATA_P),C_POINTERTYPE);

			uint64_t s = llvm::DataLayout(r.lmod).getTypeAllocSize(AC->inner->type);
			auto CAL = r.builder.CreateCall2(R_FUNC,IP,r.builder.CreateMul(r.builder.CreateZExt(NEWLEN,C_SIZETTYPE),
					llvm::ConstantInt::get(C_SIZETTYPE, s)));
			auto NEW_P = r.builder.CreatePointerCast(CAL,llvm::PointerType::getUnqual(AC->inner->type));
			r.builder.CreateStore(NEW_P,DATA_P);
			r.builder.CreateStore(NEWLEN,ALLOC_P);
			r.builder.CreateBr(ADD);
			r.builder.SetInsertPoint(ADD);
			llvm::Value* I = r.builder.CreateLoad(DATA_P);
			I = r.builder.CreateGEP(I, LENGTH);
			r.builder.CreateStore(INTO, I);
			r.builder.CreateStore(r.builder.CreateAdd(LENGTH, getInt32(1)),LENGTH_P);
			return new ConstantData(INTO, AC->inner);
		}
		filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
		exit(1);
	}
	case CLASS_GEN:
	case CLASS_SET:
	case CLASS_MAP:
	case CLASS_WRAPPER:
	case CLASS_VOID:{
		if(false) return cc;
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Cannot apply binary operations to reference or lazy classes");
		exit(1);
	}
	case CLASS_USER:{

		if(dd->classType==CLASS_NULL && (cc->layout==POINTER_LAYOUT || cc->layout==PRIMITIVEPOINTER_LAYOUT)){
			if(operation=="=="){
				return new ConstantData(r.builder.CreateIsNull(value->getValue(r, filePos)), &boolClass);
			} else if (operation=="!="){
				return new ConstantData(r.builder.CreateIsNotNull(value->getValue(r, filePos)), &boolClass);
			}
		}
		const UserClass* uc = (const UserClass*)cc;
		return uc->getLocalFunction(filePos, ":"+operation, NO_TEMPLATE, {ev})->callFunction(r, filePos, {ev},value);
	}
	}
}


#endif /* BINARY_HPP_ */
