#ifndef _INTERSECTION_WORK_INTERFACE_HEADER_
#define _INTERSECTION_WORK_INTERFACE_HEADER_

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

class IntersectionWorkInterface : public WorkInterface{
	public:
		IntersectionWorkInterface(DbCompany*& p_db_company, int interface_id):
		WorkInterface(p_db_company, interface_id){
		}
		~IntersectionWorkInterface(){
		}
	
	private:

		// 获取关注列表 from redis
		int get_follow_uids(const uint64_t uid, __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, 
				uint32_t& follow_uid_num);
			
		// 获取关注列表 from open API
		int get_follow_uids_bak(const char* uid_str, __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, 
				uint32_t& follow_uid_num);

		// set集合与db求交，保存到vec中
		int intersection_uids(const __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, const char* db_name, 
				vector<uint64_t>& vec_intersection);

		// set转成vec
		void hashset_to_vector(const __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, 
				vector<uint64_t>& vec_follows);
		
		// vec转成str，由type决定字符串样式	
		int vector_to_str(const vector<uint64_t>& vec, const int type, char*& out_str);

		// 生成结果字符串，由type决定哪个集合
		void generate_output_str(const vector<uint64_t>& vec_result, const int type, char*& p_out_string, int& n_len);
		
		// 生成结果字符串，由type决定哪两个集合
		void generate_output_str(const vector<uint64_t>& vec1, const vector<uint64_t>& vec2, const int type, 
				char*& p_out_string, int& n_len);
	
		// 生成结果字符串，三个集合内容，type参数无用
		void generate_output_str(const vector<uint64_t>& vec_allfol, const vector<uint64_t>& vec_result,
				const vector<uint64_t>& vec_good, const int type, char*& p_out_string, int& n_len);

	private:
		// 获取即时查询业务，提供reco_ids
		int get_intime_uids(const char* uid_str, const __gnu_cxx::hash_set<uint64_t>& reco_uids, 
				char*& p_out_string, int& n_out_len);
			
		// 相交业务，由type决定，返回所有关注，广告主，活跃用户等
		int get_intersection_uids(const char* uid_str, const int type, char*& p_out_string, int& n_out_len);
			
	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
};
#endif
