/*
 * lib.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: wmoses
 */

#ifndef LIB_HPP_
#define LIB_HPP_
#include "includes.hpp"

String getExecutablePath(){
	char resolved_path[PATH_MAX];
	uint32_t size = sizeof(resolved_path);
#if defined(WIN32) || defined(_WIN32)
	#define SEP '\\'
	size = GetModuleFileName(nullptr,resolved_path,sizeof(resolved_path));
#elif defined(__APPLE__)
	uint32_t size = sizeof(resolved_path);
	if (_NSGetExecutablePath(resolved_path, &size) ==-1)
		fprintf(stderr, "Executable path buffer too small; need size %u\n", size);
	#define SEP '/'
#else
	auto numBytes = readlink ("/proc/self/exe", resolved_path, sizeof(resolved_path));
	if(numBytes==-1){
		fprintf (stderr, "Cannot find path to executable: %s\n", strerror (errno));
		exit(1);
	}
	size = (uint32_t)numBytes;
	resolved_path[size]='\0';
	#define SEP '/'
#endif
	assert(resolved_path[size]=='\0');
	do{
		assert(size>0);
		size--;
	}while(resolved_path[size]!=SEP);

	/* size+1 as to include the final slash */
	return String(resolved_path, size+1);
}

template<typename T> constexpr T log2(T index){
	return (index>1)?(log2(index/2)+1):0;
}
bool isalpha(char c){
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
}
bool isalpha2(char c){
	return (c>='a' && c<='z');
}
bool isalnum(char c){
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9');
}
struct ComplexStruct{
		double real, complex;
};

void o_exit (int s) {
	mpfr_free_cache();
	exit(s);
}
void getDir(String pos, String& dir, String& file){
	size_t p = pos.find_last_of('/');
	if(p==pos.npos) p = pos.find_last_of('\\');
	else{
		size_t tmp = pos.find_last_of('\\');
		if(tmp!=pos.npos && tmp>p) p = tmp;
	}
	if(p==pos.npos){
		dir=".";
		file = pos;
	} else if(p>0 && pos[p-1]=='/'){
		cerr << "Do not accept // filepaths" << endl << flush;
		exit(1);
	} else{
		dir = pos.substr(0,p);
		file = pos.substr(p+1);
	}
}

class PositionID{
	public:
		unsigned int lineN;
		unsigned int charN;
		String fileName;
		//PositionID():lineN(0),charN(0),fileName("<c++ pair>"){};
		PositionID(String c, unsigned int a, unsigned int b){
			lineN = a;
			charN = b;
			fileName = c;
		}
		PositionID(unsigned int a, unsigned int b, String c){
			lineN = a;
			charN = b;
			fileName = c;
		}
		//	PositionID():PositionID(0,0,"<start>"){}
		void fatalError  (String s, bool end=true) const __attribute__ ((noreturn)){
			cerr << "Fatal Error: " << s << " at " << fileName << ":" << lineN << ":" << charN << endl << flush;
			#ifndef NDEBUG
				assert(0);
				exit(1);
			#else
				exit(1);
			#endif
		}
		bool compilerError(String s, bool end=true) const __attribute__ ((noreturn)){
			#ifndef NDEBUG
				cerr <<  "Compiler Error: " << s <<  " at " <<  fileName <<  ":" << lineN << ":" << charN;
				assert(0);
				exit(1);
			#else
				cerr << s << " at " << fileName << ":" << lineN << ":" << charN << endl << flush;
				exit(1);
			#endif
		}
		void error(String s, bool end=true) const{
			cerr << "Error: " << s << " at " << fileName << ":" << lineN << ":" << charN << endl << flush;
		}
		bool warning(String s) const{
			cerr << "Warning: " << s << " at " << fileName << ":" << lineN << ":" << charN << endl << flush;
			return false;
		}
		ostream& operator << (ostream& o) const{
			o << fileName;
			o << " line:";
			o << lineN;
			o << ", char: ";
			o << charN;
			return o;
		}
};

template<typename H, typename M> class Mapper{
private:
	M data;
	std::map<H,Mapper<H,M>> map;
public:
	inline M& get(const std::vector<H>& vals){
		const auto s = vals.size();
		if(s==0) return data;
		else{
			Mapper* t= &map[vals[0]];
			for(unsigned int i=1;i<s;i++)
				t = &(t->map[vals[i]]);
			return t->data;
		}
	}
	inline M& get() const{
		return data;
	}
	/*
	template<H... args> inline M& get() const{
		if(sizeof...(args)==0) return data;
		else{
		    H vals[] = {args...};
			Mapper* t=map[vals[0]];
			for(unsigned int i=1;i<sizeof...(args);i++)
				t = t->map[vals[i]];
			return t->data;
		}
	}*/
};

bool writeByte(FILE* f, byte a){
	return fputc(a, f)==EOF;
}

bool readByte(FILE* f, byte* a){
	*a = fgetc(f);
	return false;
}

bool peekByte(FILE* f, byte* a){
	*a = fgetc(f);
	return ungetc(*a,f)==EOF;
}

bool writeChar(FILE* f, int a){
	return fputc(a,f)==EOF;
}

bool readChar(FILE* f, int *a){
	*a = fgetc(f);
	return false;
}

bool returnChar(FILE* f, int a){
	return ungetc(a,f)==EOF;
}

bool peekChar(FILE* f, int* a){
	*a = fgetc(f);
	return ungetc(*a,f)==EOF;
}

bool returnString(FILE* f, String c){
	for(unsigned int i = c.length()-1; ; i--){
		if(returnChar(f, (char)c[i])) return true;
		if(i==0) break;
	}
	return false;
}

inline std::string demangle(const char* name){
	#if (__GNUC__ && __cplusplus && __GNUC__ >= 3)
    char*       realname;
    std::size_t len;
    int         stat;

    realname = abi::__cxa_demangle(name,NULL,&len,&stat);

    if (realname != NULL){
        std::string   out(realname);
        std::free(realname);
        return out;
    }
    return std::string("demangle :: error - unable to demangle specified symbol");
    #else
    return name;
    #endif
}

#endif /* LIB_HPP_ */
