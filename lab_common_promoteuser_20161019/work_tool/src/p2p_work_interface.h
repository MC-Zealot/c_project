#ifndef _P2P_WORK_INTERFACE_HEADER_
#define _P2P_WORK_INTERFACE_HEADER_

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

/* 根据被访问者推荐用户 */
struct RecoInfo{			// 推荐结果信息
	uint64_t reco_id;		// 推荐结果id
	float score;			// 推荐结果得分
	int relation;			// 推荐结果与被访问者关系
	int source;				// 数据来源
	int user_type;			// 用户V类型
};

/* 排序函数，先按关系排序，再按分值排序，可保证排在前面的都是带关系的，而且分值高的 */
static bool cmp_all(const RecoInfo& a, const RecoInfo& b){
	if(a.relation > b.relation) return true;			// 先按关系排序
	else if(a. relation < b.relation) return false;
	else return a.score > b.score;						// 再按分值排序
}

/* 排序函数，按分值排序 */
static bool cmp_score(const RecoInfo& a, const RecoInfo& b){
	return a.score > b.score;							// 按分值排序
}

// 表示关系，0表示无关系，1表示关注，2表示互粉
enum RELATION {RELATION_NO = 0, RELATION_FOLLOW = 1, RELATION_FRIEND = 2};

// 数据来源，0表示无来源，1表示搜索共现，2表示粉丝相似度，3表示亲密度
enum SOURCE {SOURCE_NOWHERE = 0, SOURCE_SEARCH = 1, SOURCE_FANSSIM = 2, SOURCE_INTIMACY = 3};

const int RESULT_NUM = 50;		// 默认结果数量

class P2PWorkInterface : public UserWorkInterface {
	public:
		P2PWorkInterface(DbCompany*& p_db_company, int interface_id):
			UserWorkInterface(p_db_company, interface_id){
			}
		~P2PWorkInterface(){
		}

	private:
		// 并行获取推荐结果，保存粉丝相似度结果，亲密度结果等
		int get_recom_data(const uint64_t uid, const bool need_relation, 
				const __gnu_cxx::hash_set<uint64_t> filter_set,
				vector<RecoInfo>& fanssim_vec, vector<RecoInfo>& friend_vec);

		// 获取数据，存至集合中。如关注数据，互粉数据等
		int get_data_set(char* p_result, char split_char, char second_split_char,
				__gnu_cxx::hash_set<uint64_t>& hashset_uids);

		// 分割粉丝相似度数据
		void get_fanssim_data(const char*& str_input, const char sep_char, 
				const char second_sep_char, vector<PAIR_INT_FLOAT>& vec_ids);

		// 分割亲密度数据
		void get_intimacy_data(const char*& str_input, const char sep_char,
				const char second_sep_char, vector<PAIR_INT_FLOAT>& vec_ids);

		// 构建初始推荐结果，并排序
		int recom_intersection(const int relation, const int source,
				const __gnu_cxx::hash_set<uint64_t>& filter_set, 
				const __gnu_cxx::hash_set<uint64_t>& uid_set,
				const vector<PAIR_INT_FLOAT>& uid_sco_vec, vector<RecoInfo>& inter_vec);

		// 判断用户是否是优质用户
		bool IsGoodUser(const uint64_t uid);

		// 获取用户的V类型
		int get_v_type(const uint64_t uid);

		// 获取橙V搜索共现
		int get_yellowV_recom(const uint64_t uid, const __gnu_cxx::hash_set<uint64_t> filter_set, 
				vector<RecoInfo>& yellow_V_data);

		// 生成字符串
		void generate_output_str(const vector<RecoInfo>& result_vec, const int num,
				const uint64_t tuid, char*& p_out_string, int& n_out_len);
	
		// 主要业务逻辑函数，生成推荐结果
		int p2p_recom(const char* uid_str, const char* tuid_str, const int num,
				const bool need_relation, char*& p_out_string, int& n_out_len);

	public:
		// 入口函数
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
};
#endif
