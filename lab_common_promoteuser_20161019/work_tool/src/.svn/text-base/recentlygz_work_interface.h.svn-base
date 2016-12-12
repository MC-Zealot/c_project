#ifndef _RECENTLYGZ_WORK_INTERFACE_HEADER_
#define _RECENTLYGZ_WORK_INTERFACE_HEADER_

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

const uint16_t INFO_PART = 4;					// 每条关注信息包含四部分
const uint16_t MAX_GZ_LENGTH = 50;				// 每条关注信息字符串长度
const uint16_t GZ_NUM = 10;						// 保留的关注信息数量
const int MAX_GZ_NUM = 3000;                    // 每个用户的最大关注数目
const uint64_t TIMESTAMP = (unsigned long long)(86400 * 7);	// 最多取七天前的关注

const uint16_t BRIDGE_NUM = 2;					// 仅返回桥梁数目>=2的用户
const int MAX_RESULT_NUM = 100;					// 返回结果的最大数目

class RecentlyGZWorkInterface : public WorkInterface{
	public:
		RecentlyGZWorkInterface(DbCompany*& p_db_company, int interface_id):
			WorkInterface(p_db_company, interface_id){ 
		}
		~RecentlyGZWorkInterface(){
		}
	
	private:
		//定义内部结构体，存储桥梁信息	
		struct BridgeStr{
			BridgeStr(uint64_t uid, uint64_t time){	
				num_ = 1;	
				time_ = time;
				bridge_uid_vec.push_back(uid);
			}
			uint16_t num_;                      // 桥梁数量
			uint64_t time_;						// 桥梁加关注的时间，保存最近的时间
			vector<uint64_t> bridge_uid_vec;	// 桥梁用户
		};

		// 定义字典，key表示推荐用户uid，value表示桥梁信息, 第二个参数是指向结构体的指针
		typedef __gnu_cxx::hash_map<uint64_t, BridgeStr*> HashMapGZResult;
		void release_hashmap_gz_result(HashMapGZResult& hashmap_gz_result){
			for(HashMapGZResult::iterator it = hashmap_gz_result.begin(); it != hashmap_gz_result.end(); it ++){
				if((*it).second != NULL){
					delete (*it).second;
					(*it).second = NULL;
				}
			}
			hashmap_gz_result.clear();
		}

		struct RecomInfo{										// 推荐用户信息
			RecomInfo(uint64_t reco_id, BridgeStr* bridgeInfo, uint32_t vtype){
				reco_id_ = reco_id;
				bridgeInfo_ = bridgeInfo;
				vtype_ = vtype;
			}
			uint64_t reco_id_;									// 推荐用户id
			BridgeStr* bridgeInfo_;								// 推荐用户桥梁信息
			uint32_t vtype_;									// 推荐用户类型
		};
		static bool cmp_RecomInfo(const RecomInfo& a, const RecomInfo& b){
			if(a.bridgeInfo_->num_ > b.bridgeInfo_->num_) return true;
			else if (a.bridgeInfo_->num_ < b.bridgeInfo_->num_) return false;
			else return a.bridgeInfo_->time_ > b.bridgeInfo_->time_;
		}

	private:
		//获取关注列表
		int get_follow_uids(const uint64_t uid, __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, 
				uint32_t& follow_uid_num);

		// 与指定用户求交集，最后一个参数为true时，表示保留交集，为false时，表示保留差集
		int get_intersection(const __gnu_cxx::hash_set<uint64_t>& src_set, const char* db_name, bool cull,
				__gnu_cxx::hash_set<uint64_t>& des_set);	

		// 查询用户V类型
		int get_v_type(const uint64_t uid, uint32_t& type);	

		//批量获取最近关注列表
		int mget_follow_uids(const uint64_t uids[], const uint32_t get_uid_num, MapMResult& map_m_fof_result,
				char& split_char, char& second_split_char);

		// 将字符串按照分隔符解析成字符串数组
		int split_string_to_array(const char* str_input, const char& sep, char str_out[][MAX_GZ_LENGTH]);

		//构建关系用户列表
		int construct_user_data(const MapMResult& map_m_fof_result, const char split_char, 
				const char second_split_char, HashMapGZResult& gz_dict, __gnu_cxx::hash_set<uint64_t>& recoid_set);

		//计算推荐结果
		int calc_recom_result(const __gnu_cxx::hash_set<uint64_t>& reco_ids, const HashMapGZResult& reco_dict,
				const __gnu_cxx::hash_set<uint64_t>& gz_ids, vector<RecomInfo>& result_vec);
		
		// 将vector转成字符串，形如 "recent_gz_follow":[id1,id2,...,idn]
		int vector_to_str(const vector<uint64_t>& vec, char*& out_str);

		// 将PAIR转成字符串，形如 [uid,{"recent_gz_num":num,"recent_gz_follow":[id1,id2,...,idn],"user_type":0}]
		int recomInfo_to_str(const RecomInfo& info, char*& tmp_str);

		//输出推荐结果
		void generate_output_str(const vector<RecomInfo>& res_vec, char*& out_str, int& n_len);

	private:
		//功能处理模块
		int get_recom_uids(const char* uid_str, char*& p_out_string, int& n_out_len);

	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);

};
#endif
