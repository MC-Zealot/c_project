#ifndef _AD_INDEX_WORK_INTERFACE_HEADER_
#define _AD_INDEX_WORK_INTERFACE_HEADER_

#include "work_interface.h"
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

class AdIndexWorkInterface : public WorkInterface{
	public:
		AdIndexWorkInterface(DbCompany*& p_db_company, int interface_id):
		WorkInterface(p_db_company, interface_id){
		}
		~AdIndexWorkInterface(){
		}

	private:
		GlobalAdDbInterface* get_ad_db_interface(const char* db_name);

		int info(const char* db_name, char* &p_out_string, int& n_out_len, uint64_t req_id);

		int set(const char* db_name, json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);

		int get_attrs(const char* db_name, uint64_t ad_id, const char* item_keys[], size_t item_key_len,
				char* &p_out_string, int& n_out_len, uint64_t req_id);

		int set_status(const char* db_name, uint64_t ad_id, int status, char* &p_out_string, int& n_out_len, uint64_t req_id);

		int get_status(const char* db_name, uint64_t ad_id, char* &p_out_string, int& n_out_len, uint64_t req_id);

		int get_indexes(const char* db_name, uint64_t ad_id, const char* item_key[], size_t item_key_len,
				char* &p_out_string, int& n_out_len, uint64_t req_id);

		int find_indexes(const char* db_name, json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);

		int update_dump_file(const char* db_name, const char* update_dump_file, char* &p_out_string, int& n_out_len, uint64_t req_id);

	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
			
};
#endif
