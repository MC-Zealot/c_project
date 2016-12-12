#ifndef _BLOCK_CACHE_WORK_INTERFACE_HEADER_
#define _BLOCK_CACHE_WORK_INTERFACE_HEADER_

#include "work_interface.h"
#include "user_work_interface.h"
#include "ini_file.h"
#include "woo/log.h"
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>
#include "json.h"
#include <time.h>
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

class BlockCacheWorkInterface : public UserWorkInterface{
	public:
		BlockCacheWorkInterface(DbCompany*& p_db_company, int interface_id):
		UserWorkInterface(p_db_company, interface_id){
		}
		~BlockCacheWorkInterface(){
		}

	private:
		int set_block_cache(const char* uid_str, char* &p_result);
		int get_block_from_db(const char* uid_str, char* &p_result, const char* db_name);
		int get_block(const char* uid_str, char* &p_out_string, int& n_out_len, 
				const char* from_str, int write_flag = 1);
	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
			
};
#endif
