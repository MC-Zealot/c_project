#ifndef _TARGETING_WORK_INTERFACE_HEADER_
#define _TARGETING_WORK_INTERFACE_HEADER_

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

class TargetingWorkInterface : public WorkInterface{
	public:
		TargetingWorkInterface(DbCompany*& p_db_company, int interface_id):
		WorkInterface(p_db_company, interface_id){
		}
		~TargetingWorkInterface(){
		}

	typedef std::vector<uint64_t> VEC_UID;
	typedef std::map<uint64_t, VEC_UID> MAP_VEC_UID;

	void release_map_vec_uid(MAP_VEC_UID& map_vec_uid);

#define MAX_FOLLOW_SIZE 3000
#define MAX_LIMIT 200
	private:
		void split_string_ids(uint64_t ids[], uint32_t& id_num, 
				__gnu_cxx::hash_set<uint64_t>& hashset_reco_uids, const char* str_input,
				char sep_char, uint32_t limit = MAX_LIMIT);
					
		void split_string_ids_in_set(MAP_VEC_UID& map_vec_uid, uint64_t rel_uid, const char* str_input, 
				char sep_char, uint32_t limit = MAX_LIMIT);
					
		//获取关注列表 from redis
		int get_follow_uids(uint64_t follow_uids[], uint32_t& follow_uid_num, uint32_t max, 
				__gnu_cxx::hash_set<uint64_t>& hashset_reco_uids, uint64_t uid);
	
		// 获取关注列表 from open API
		int get_follow_uids_bak(uint64_t follow_uids[], uint32_t& follow_uid_num, uint32_t max,
				__gnu_cxx::hash_set<uint64_t>& hashset_reco_uids, const char* uid_str);

		//批量获取关注列表
		int mget_follow_uids(uint64_t mget_uids[], uint32_t get_uid_num, MapMResult& map_m_fof_result,
				char& split_char, char& second_split_char);
		
		int generate_output_str(const MAP_VEC_UID& map_vec_uid, int num, 
				char*& p_out_string, int& n_len);
		
	private:

		//处理即时数据
		int get_targeting_uids(const char* uid_str, __gnu_cxx::hash_set<uint64_t>& reco_uids, 
				uint64_t group_uids[], uint16_t group_uid_num,
				uint32_t max, uint16_t num, char*& p_out_string, int& n_out_len);
			
	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
			
};
#endif
