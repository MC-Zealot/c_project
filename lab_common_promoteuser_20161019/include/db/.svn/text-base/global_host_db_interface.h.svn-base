#ifndef _GLOBAL_HOST_DB_INTERFACE_HEADER_
#define _GLOBAL_HOST_DB_INTERFACE_HEADER_
#include <stdlib.h> 
#include <vector>
#include <string>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

using namespace std;

class GlobalHostDbInterface : public GlobalDbInterface{
	public:
		GlobalHostDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct), host_(NULL){
		}
		~GlobalHostDbInterface(){
			if(NULL != host_){
				delete [] host_;
			}
		}
	private:
		 char* host_;
	public:
		char* get_local_host() const {
			return host_;
		}
		
		virtual bool is_exist(uint64_t id){
			return 0;
		}                                         
                virtual int load_db_config(){
			string host_ip = get_host();	
			uint16_t len = host_ip.length();
			host_ = new char[len + 1];
			if(NULL == host_){
				LOG_ERROR("allocate memory for host_ fail");
				return -1;
			}
			memset(host_, 0, len + 1);
			strncpy(host_, host_ip.c_str(), len);
			return 0;
		}
};
#endif
