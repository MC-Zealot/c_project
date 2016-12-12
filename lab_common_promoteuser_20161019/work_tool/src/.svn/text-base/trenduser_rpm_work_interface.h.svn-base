#ifndef _TRENDUSER_RPM_WORK_INTERFACE_HEADER_
#define _TRENDUSER_RPM_WORK_INTERFACE_HEADER_

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

class TrendUserRPMWorkInterface : public UserWorkInterface{
	public:
		TrendUserRPMWorkInterface(DbCompany*& p_db_company, int interface_id):
			UserWorkInterface(p_db_company, interface_id), cands_(NULL){
			cands_ = new CandUser(true);
		}
		~TrendUserRPMWorkInterface(){
			if(cands_ != NULL){
				delete cands_;
				cands_ = NULL;
			}
		}

	#define OFF_LIMIT_LEN 2000
	#define CONFIDENT_NUM 50
	#define CAND_NUM 500
	static const uint32_t max_b_num = 2;
	static const int REASON_NUM = 4;
	static const int MAX_BRIDGE_NUM = 3;
	private:
		CandUser* cands_;			
	private:
		
		int get_user_mfeature(std::map<uint64_t, uint64_t>& map_return_value,
				uint64_t uid, const std::vector<uint64_t>& vec_item_key);
		
		int get_user_feature(uint64_t& return_value, uint64_t uid, uint64_t item_key);
		
		int generate_out_str(int num, candidate_item_t& cand, int& index, 
				char*& p_out_temp, int& n_len);
			
		int get_offline_reco_uids(uint64_t uid, uint32_t num, uint64_t req_id,
				char* p_out_string, int& n_out_len);

		int get_online_reco_result(uint64_t uid, uint32_t num, uint64_t req_id,
				char* p_out_string, int& n_out_len);
		//功能处理模块
		int get_reco_uids(const char* uid_str, uint32_t num, int type, int zip, uint64_t req_id,
				char* p_out_string, int& n_out_len);

	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);

};
#endif
