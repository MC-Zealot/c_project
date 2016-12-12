#ifndef _GLOBAL_AES_DB_INTERFACE_HEADER_
#define _GLOBAL_AES_DB_INTERFACE_HEADER_
#include <stdlib.h> 
#include <vector>
#include <string>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"
#include "aes.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

using namespace std;

class GlobalAesDbInterface : public GlobalDbInterface{
	public:
		GlobalAesDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct), aes_ptr(NULL){
			//aes_ptr = aes_create("/home/w/conf/aes.key");
		}
		~GlobalAesDbInterface(){
			 if(NULL != aes_ptr)
			 	aes_destroy(aes_ptr);
			 else{
				LOG_ERROR("destroy aes_ptr failed");
			 }
		}
	private:
		 aes_t * aes_ptr;
	public:
		int enc_func(unsigned char* src, uint32_t src_len, unsigned char* enc, uint32_t * enc_len){
			int ret = 0;
			if(NULL == aes_ptr){
				LOG_ERROR("aes_ptr is null");
				return -1;
			}
			else
				ret = aes_enc(aes_ptr, src, src_len, enc, enc_len);
			return ret;
		}
		int dec_func(unsigned char* src, uint32_t src_len, unsigned char* enc, uint32_t *enc_len){
			int ret = 0;
			if(NULL == aes_ptr){
				LOG_ERROR("aes_ptr is null");
				return -1;
			}
			else
				ret = aes_dec(aes_ptr, src, src_len, enc, enc_len);
			return ret;
		}
		
		virtual bool is_exist(uint64_t id){
			return 0;
		}                                         
                virtual int load_db_config(){
			char* db_file_name = global_db_info_struct_.db_file_name_;              
                        if(NULL == db_file_name){
				LOG_ERROR("aes db_file_name ptr is null");
                                return -1;
			}
			aes_ptr = aes_create(db_file_name);
			if(NULL == aes_ptr){
				LOG_ERROR("create aes_ptr failed");
				return -1;
			}
			return 0;
		}
};
#endif
