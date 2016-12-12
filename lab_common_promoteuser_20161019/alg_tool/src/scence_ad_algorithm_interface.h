#ifndef _SCENCE_AD_ALGORITHM_HEADER_
#define _SCENCE_AD_ALGORITHM_HEADER_

#include "algorithm_interface.h"
#include "scence_ad.h"
typedef struct score_mark{
	int time_score;
	int interest_score;
	int wifi_score;	
	int hisctr_score;	
	double sel_prob; 
}SCOREMARK;
struct ItemHis{
	string	item_id;
	string	cate_id;
	int action_num;
	int expo_num;
	double yield;
};
struct ParaInRedis{
	int prob_for_sample;
	int prob_for_prof;
	int stat;
	int model_stat;
	map< string,vector<float> > model_para_map;
	ParaInRedis(){
		prob_for_sample = 30;
		prob_for_prof = 94;	
		stat = 0;
		model_stat = 0;
	}
};
ParaInRedis pir; 
class ScenceAlgorithmInterface : public AlgorithmInterface{
	public:
		ScenceAlgorithmInterface(DbCompany*& p_db_company, int interface_id):
			AlgorithmInterface(p_db_company, interface_id){
			if( 0){
				//model_para_init_stat = 1;
				FILE *fp = NULL;
				fp = fopen("../data/model.txt","r");	
				LOG_ERROR("model.txt fopen");
				if( NULL == fp) {
					LOG_ERROR("model.txt fopen error");
				}else{
					char buf[2048];
					while(fgets(buf,sizeof(buf),fp) != NULL){
						char *p = strstr(buf,":");
						if(p && p+1){
							string key,val;
							key.assign(buf,p-buf);
							val.assign(p+1);
							//pir.model_para_map[key] = val;
						}
					}
				
				}
				fclose(fp);
				fp = NULL;
			}
		}
		~ScenceAlgorithmInterface(){
		}

	public:
		// algorithm_core函数是纯虚函数，必须实现
		int algorithm_core(uint64_t req_id, const AccessStr& access_str, VEC_CAND& vec_cand);
		// algorithm_core_new函数是新的算法函数
		int algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, const VEC_CAND& input_vec, VEC_CAND& output_vec);
		int get_data_from_redis(const char *key, string &redis_val);
		int get_history_top_item(std::string &key_str, vector<ItemHis> &item_his_vec);
		int get_hasread_item(string &uid, map<string, int> &user_hasread_map);
		int get_rec_item_from_histop(vector<ItemHis> &item_his_vec, map<string, int> &user_has_read_map,map<uint32_t, int> &score_map,SCENCE_AD *sad);
		int get_kind_item(uint32_t cateid, vector<string> &item_vec);
		int get_kind_filt_item(uint32_t cateid, map<string, int> &item_map);
		int get_rec_item_from_hot(uint32_t cateid, map<string, int> &user_has_read_map, SCENCE_AD *sad);
		int get_rec_item_from_prof(uint32_t cateid, map<string, int> &user_has_read_map, SCENCE_AD *sad);
		int get_rec_item_from_sample(map<string, int> &user_has_read_map, map<uint32_t, int> &score_map, SCENCE_AD *sad);
		int set_data_to_redis(const  char *key, string &redis_val);
		int get_rec_title(uint32_t cateid, uint16_t time_slice, string &rec_title);
		int get_rec_reason(uint32_t cateid, string distance, uint16_t time_slice, string &rec_reason, string district);
		int get_rec_item_from_lbsad(uint32_t cateid, map<string, int> &user_has_read_map, SCENCE_AD *sad, string lat, string lon, string &uid, string &log_str);
		int get_ad_loc(string &mid, string &lon, string &lat, string &city, string &district);
		int get_black_users(map<string, bool> &black_users_map);
		int get_para_in_redis(ParaInRedis &pis);
		int get_model_para_in_redis(ParaInRedis &pis);
		int get_item_for_demo(vector<uint32_t> &cate_vec, map<string, int> &user_has_read_map, SCENCE_AD *sad, string lat, string lon);
		int get_cate_from_model(map<string,vector<float> > &mapvalues, vector<uint32_t> &cate_vec, string tday, bool has_wifi, string &pois, uint32_t &sel_cate);
		int get_user_city(string &uid, string &city_id);
		int get_ads_kind_item(uint32_t cateid, vector<string> &item_vec, vector<string> &city_vec);
};
#endif
