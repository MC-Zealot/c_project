#ifndef _FANSTARGET_AD_ALGORITHM_INTERFACE_HEADER_
#define _FANSTARGET_AD_ALGORITHM_INTERFACE_HEADER_

#include "algorithm_interface.h"
#include "fanstarget_ad.h"
#include <pthread.h>
#include <unistd.h>
typedef map<string,int> Map;
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
const int64_t FEATURE_HASH_SIZE = 30000;
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
int GetFeatureHash(const char* feature) {
  unsigned long long a, hash = 0;
  for (a = 0; a < strlen(feature); a++) hash = hash * 257 + feature[a];
  hash = hash % (FEATURE_HASH_SIZE - 101);
  return hash;
}


typedef struct _lushan{
	int keysize;
	char** keystr;
	map<uint64_t,string> result;
	DbCompany* p_db_company;
}LUSHAN;

typedef struct _model_data{
	char** weights;
	Map** mapArray;
	int indexkey;
	int mapNum,weightsNum;
}model_data;

typedef struct _model_conf{
	vector<string> mapkeys;
	vector<string> keys_vec;
	int arraykey;
	int indexkey;
	string update_f;
	map<string,int> parameters;
}model_conf;

template <typename TYPE, void (TYPE::*_RunThread)() >  
void* _thread_t(void* param)//线程启动函数，声明为模板函数  
{     
	TYPE* This = (TYPE*)param;     
	This->_RunThread();     
	return NULL;  
}


//声明全局变量 供模型使用
model_data **model_ptr,*model_copy_ptr;
pthread_t tid_fans_economy;
pthread_mutex_t p_lock_fans_economy;
int fans_economy_model_flg = 1;

class FansTargetAlgorithmInterface : public AlgorithmInterface
{
	public:
		FANS_TAR_AD *fansAdThread[3];
		VEC_CAND fans_ad_vec_thread;
		map<int, float> weights;
		map<string, int> feature_index;
		vector<string> feature_name_default;
		vector<string> feature_name_list;
		int *model_flg;//用于标记本线程是否可以创建线程，以及创建后负责创建内存的释放
		model_data *model_read_ptr;
		//model_data
		FansTargetAlgorithmInterface(DbCompany*& p_db_company, int interface_id):
			AlgorithmInterface(p_db_company, interface_id){
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
//			model_copy_ptr =(model_data *)malloc(sizeof(model_data));	
			//加载模型
			//
			model_flg = new int();
			//加锁 改变创建线程的全局变量  只创建一个线程 设置允许该线程创建线程 
			pthread_mutex_init(&p_lock_fans_economy,NULL);
			pthread_mutex_lock(&p_lock_fans_economy);
			if (fans_economy_model_flg ==1 ) {
				fans_economy_model_flg = 0;
				*model_flg = 1;
			}
			pthread_mutex_unlock(&p_lock_fans_economy);
			int model_num =2;
			//创建模型更新线程
			if( *model_flg == 1){
				//申请初始模型内存，以免释放内存错误
				model_ptr = new model_data*[model_num]();
				for (int i=0;i<model_num;i++){
					model_ptr[i] = new model_data();
					model_ptr[i]->mapNum = 2;
					model_ptr[i]->weightsNum=100;
					model_ptr[i]->mapArray = new Map*[model_ptr[i]->mapNum]();
					for(int j=0;j<model_ptr[i]->mapNum;j++){
						model_ptr[i]->mapArray[j] = new Map();
					}
					char** m_weights = new char*[model_ptr[i]->weightsNum];
					for (uint32_t j = 0; j < model_ptr[i]->weightsNum; j++){
						m_weights[j] = new char[256]();
					}
					model_ptr[i]->weights = m_weights;
				}
				//创建模型线程
				pthread_create(&tid_fans_economy,NULL,_thread_t<FansTargetAlgorithmInterface, &FansTargetAlgorithmInterface::_RunThread>, this);
				LOG_ERROR("init FansTargetAlgorithmInterface.model_flg:%d, pthread_create %d",model_flg,tid_fans_economy);
			}
			
		    	// 特征名字加载，为了交叉，以及不可能所有特征都从引擎带过来，因此存值
			string INDUSTRY_FEATURE = "f,o,g,p";
			string INDUSTRY_FEATURE_DEFAULT = "f,o,g1,p2";
			string token = ",";
			SplitString(INDUSTRY_FEATURE, token,feature_name_list);
			SplitString(INDUSTRY_FEATURE_DEFAULT, token, feature_name_default);
			LOG_ERROR("init success");
		}
		~FansTargetAlgorithmInterface() {
			LOG_ERROR("destroy FansTargetAlgorithmInterface.");
			for(int i = 0;i < 3; i ++){
				delete(fansAdThread[i]);
			}
			//该线程创建的模型，负责释放模型内存
			if (*model_flg == 1){
				pthread_mutex_destroy(&p_lock_fans_economy);
				for(int i=0;i<2;i++){
					if (model_ptr[i] != NULL){
						if(model_ptr[i]->weights != NULL){
							for(uint16_t j=0;j<model_ptr[i]->weightsNum;++j){
								delete[] model_ptr[i]->weights[j];
							}
							if (model_ptr[i]->weights != NULL)
								delete model_ptr[i]->weights;
						}
						if(model_ptr[i]->mapArray != NULL){
							for(uint16_t j=0;j<model_ptr[i]->mapNum;++j){
								delete model_ptr[i]->mapArray[j];
							}
							if (model_ptr[i]->mapArray != NULL)
								delete model_ptr[i]->mapArray;
						}
						delete model_ptr[i];
						if ( model_ptr[i] != NULL ) model_ptr[i] = NULL;
					}
				}
			}
			delete model_flg;
			
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
		float search_weights(string feature_name, string value,model_data *read_ptr,int value_type);
	//	void* get_lushan(void* data); //多线程取lushan 数据
		//float cross_feature(string& left, string& right, string& left_value, string& right_value);
		int industry_ctr_match(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		int ctr_match_multi(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		int industry_ctr_match_level1(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		//int match_order(uint64_t uid, VEC_CAND& input_vec, VEC_CAND::const_iterator& matcher);
		int high_quality_flowAnalysis(Fans_Tar_Ad *ft);
		//double theory_impression_cal(Fans_Tar_Ad * ft, int now_hour, double now_little);
		double theory_impression_cal(Fans_Tar_Ad * ft, time_t now);
		//int match_order(uint64_t uid, VEC_CAND& input_vec, VEC_CAND::const_iterator& matcher);
		//static void* update_model_thread(void *sl);
		//模型更新函数
		void update_model(model_conf &mcf,model_data** copy_ptr, model_data** ptr,int lsdb_num,int conf_key);
		//模型参数解析
		int parse_model_parameters(model_conf &mcf,int db_num,int key); 
		//定时任务函数
		void _RunThread();
		int search_feature_index(string &key,model_data* read_ptr);
		int search_industry_score(Fans_Tar_Ad* ft,string& score);
		int industry_ctr_estimate(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		int industry_score_threshold(Fans_Tar_Ad* ft,int base_score);
		float feature_cross_weight(int multi_flg,int fname_idx,string feature_val,int idx,vector<string> &default_info,vector<string> &target_info,int default_flg,model_data *read_ptr);
};
#endif
