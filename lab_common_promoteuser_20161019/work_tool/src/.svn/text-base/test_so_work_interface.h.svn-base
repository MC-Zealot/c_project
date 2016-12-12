#ifndef _TESTSO_WORK_INTERFACE_HEADER_
#define _TESTSO_WORK_INTERFACE_HEADER_

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

class TestSoWorkInterface : public UserWorkInterface{
	public:
		TestSoWorkInterface(DbCompany*& p_db_company, int interface_id):
			UserWorkInterface(p_db_company, interface_id){
			
		}
		~TestSoWorkInterface(){
		}

	private:
	private:
		
		int get_filter_uid_mid(uint64_t uid, __gnu_cxx::hash_set<uint64_t>& hashset_filter_uids,
				__gnu_cxx::hash_set<uint64_t>& hash_set_filter_mids, 
				const std::map<uint64_t, uint64_t>& reco_ids, int num = -1);

		//功能处理模块
		int get_test_so_uids(const char* uid_str, int num, int type, 
				const std::map<uint64_t, uint64_t>& reco_ids,
				char*& p_out_string, int& n_out_len);
			
	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
};
#endif
