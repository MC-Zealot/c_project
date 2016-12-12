#ifndef _PARA_USERRECOM_WORK_INTERFACE_HEADER_
#define _PARA_USERRECOM_WORK_INTERFACE_HEADER_

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
#include "cand_user.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

class ParaUserRecomWorkInterface : public UserWorkInterface{
	public:
		ParaUserRecomWorkInterface(DbCompany*& p_db_company, int interface_id):
			UserWorkInterface(p_db_company, interface_id){
			
		}
		~ParaUserRecomWorkInterface(){
		}

	private:
		
		int generate_out_str(int num, const candidate_item_t& cand,
				uint32_t user_type, int& index, char*& p_out_temp, 
				int& n_len);
			
		//功能处理模块
		int get_reco_uids(const char* uid_str, uint32_t num, uint32_t rel_num, char* p_out_string, int& n_out_len);

	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);

};
#endif
