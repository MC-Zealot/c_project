#ifndef _FANSTARGET_AD_ALGORITHM_INTERFACE_HEADER_
#define _FANSTARGET_AD_ALGORITHM_INTERFACE_HEADER_

#include "algorithm_interface.h"
#include "fanstarget_ad.h"

const uint32_t MAX_FREQUENCY = 15; //最大看广告数目
const uint32_t MIN_FREQUENCY = 1;  //最小看广告数目
const uint64_t FLOW_AMOUNT = 50000;	//5min 全局流量估计
const double BUY_OCCUPY_MAX_PACKAGE = 0.35f;  //最大包估计值
const uint16_t MODEL_LR_DB_NO = 21;		//lushan 
const uint16_t MID_INDUSTRY_DB_NO = 23;	//mà
const uint16_t GROUP_NAME_INDUSTRY_DB_NO = 26;
const double TIME_WEIGHT[] = { 0.71, 0.79, 1.11, 0.89, 1.16, 1.41, 1.31, 1.19,
	1.35, 1.12, 1.44, 1.24, 1.37, 1.52, 1.52, 1.75,
	1.68, 0.74, 0.41, 0.25, 0.2, 0.17, 0.24, 0.43 };
void SplitString(string& src, string& token, vector<string>& sp){
	string::size_type start, end;
	start = 0;
	end = src.find(token);
	while(string::npos != end){
		sp.push_back(src.substr(start,end - start));
		start = end + token.size();
		end = src.find(token, start);
	}
	if(start != src.length())
		sp.push_back(src.substr(start));
}

typedef struct _lushan{
	int keysize;
	char** keystr;
	map<uint64_t,string> result;
	DbCompany* p_db_company;
}LUSHAN;

class FansTargetAlgorithmInterface : public AlgorithmInterface
{
	public:
		FANS_TAR_AD *fansAdThread[3];
		VEC_CAND fans_ad_vec_thread;
		map<int, float> weights;
		map<string, int> feature_index;
		vector<string> ader_names;
		vector<string> user_names;
		vector<string> mid_names;
		//model data
		//struct/class {data,update_time}
		FansTargetAlgorithmInterface(DbCompany*& p_db_company, int interface_id):
			AlgorithmInterface(p_db_company, interface_id){
			//model data calloc
			LOG_ERROR("init FansTargetAlgorithmInterface.");
				time_t now;
				time(&now);
				for(int i = 0;i < 3; i ++){

					fansAdThread[i] = new FANS_TAR_AD();
					fansAdThread[i]->type = 1;
					fansAdThread[i]->field_id = "0";
					fansAdThread[i]->bridge_uid = 0;
					fansAdThread[i]->buy_amount = 1000;
					fansAdThread[i]->max_package = 2000;
					fansAdThread[i]->finish_amount = 500;
					fansAdThread[i]->begin_time = now;
					fansAdThread[i]->ad_type = 2;
					fansAdThread[i]->brg_type = 1;
					fansAdThread[i]->alg_log = "";
					fans_ad_vec_thread.push_back(fansAdThread[i]);
				}
			//加载模型
		    	string line;
		    	ifstream input("../conf/lr.model");
		    	//ifstream input("lr.model");
		    	if(input == NULL){
				LOG_ERROR("OPEN LR model ERROR");
				exit(1);
		    	}   
		    	while(getline(input, line)){
				int idx = line.find(',');
				int key =atoi(line.substr(0,idx).c_str());
				float value = atof(line.substr(idx+1).c_str());
				weights.insert(make_pair(key,value));
		    	}   
		    	input.close();	
		    	input.open("../conf/feature_map_file");
		    	//input.open("feature_map_file");
			if(input == NULL){
				LOG_ERROR("OPEN FEATURE_MAP ERROR");
				exit(1);
			}
			while(getline(input, line)){
				int idx1 = line.find(' ');
				string name = line.substr(0, idx1);
				int idx2 = line.find(' ', idx1+1);
				string value = line.substr(idx1+1, idx2-idx1-1);
				int index = atoi(line.substr(idx2+1).c_str());
				string key = name + value;
				feature_index.insert(make_pair(key, index));
			}
			input.close();
			// 特征名字加载，为了交叉，以及不可能所有特征都从引擎带过来，因此存值
			string ADER = "ader_gender,ader_age,ader_stage,ader_veritype,ader_fans,ader_atten,ctr,ctr";
			string USER = "user_gender,user_age,user_stage,user_veritype,user_fans,user_atten,actv_freq,active_days,pub_cnt,cmt_cnt,pv_cnt,interaction_cn";
			string MID = "image,video,url,music";
			string token = ",";
			SplitString(ADER, token, ader_names);
			SplitString(USER, token, user_names);
			SplitString(MID, token, mid_names);
			LOG_ERROR("init success");
		}
		~FansTargetAlgorithmInterface() {
			LOG_ERROR("destroy FansTargetAlgorithmInterface.");
			for(int i = 0;i < 3; i ++){
				delete(fansAdThread[i]);
			}
		}
	public:
		int algorithm_core(uint64_t req_id,const AccessStr& access_str, VEC_CAND& vec_cand);
		int algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, const VEC_CAND& input_vec, VEC_CAND& output_vec);
		int slow_order(VEC_CAND& cand_ids,VEC_CAND& output_vec);
		double get_random();
		int supply_order(uint16_t num,uint32_t flag,VEC_CAND& input_vec,VEC_CAND& output_vec);//
		int algorithm_log(VEC_CAND& output_vec,uint16_t return_type,uint16_t num,uint16_t startegy);
		int so_consume_time(struct timeval& start);	//Í
		int	log_print(ACCESS_INFO* ai, VEC_CAND& output_vec, uint16_t return_type); //Êä³ö´òÓ¡ÈÕÖ¾
		int high_quality_user(uint64_t uid, VEC_CAND& input_vec, VEC_CAND& output_vec);
		int relation_replace(VEC_CAND& input_vec,VEC_CAND& output_vec,ACCESS_INFO* ai);
		float search_weights(string& feature_name, string& value);
	//	void* get_lushan(void* data); //多线程取lushan 数据
		float cross_feature(string& left, string& right, string& left_value, string& right_value);
		int industry_ctr_match(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		int ctr_match_multi(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		int industry_ctr_match_level1(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		//int match_order(uint64_t uid, VEC_CAND& input_vec, VEC_CAND::const_iterator& matcher);
		int high_quality_flowAnalysis(Fans_Tar_Ad *ft);
		//double theory_impression_cal(Fans_Tar_Ad * ft, int now_hour, double now_little);
		double theory_impression_cal(Fans_Tar_Ad * ft, time_t now);
		//int match_order(uint64_t uid, VEC_CAND& input_vec, VEC_CAND::const_iterator& matcher);
};
#endif
