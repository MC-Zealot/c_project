#ifndef _TFINDEX_WORK_INTERFACE_HEADER_
#define _TFINDEX_WORK_INTERFACE_HEADER_

#include "work_interface.h"
#include "tfindex.h"
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


class TfIndexWorkInterface : public WorkInterface{

	public:
		TfIndexWorkInterface(DbCompany*& p_db_company, int interface_id):
		WorkInterface(p_db_company, interface_id){
		}

		~TfIndexWorkInterface(){
		}

	private:
	public:
        int get(json_object* &req_json,  vector<TF_INFO>& index_result);
        int set(json_object* &req_json);
        int del(json_object* &req_json);
        int return_process(int return_code, vector<TF_INFO>& index_result, char* &p_out_string, int& n_out_len);
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
			
};
#endif
