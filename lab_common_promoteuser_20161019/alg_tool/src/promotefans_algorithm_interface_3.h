#ifndef _PROMOTEFANS_ALGORITHM_INTERFACE_HEADER_
#define _PROMOTEFANS_ALGORITHM_INTERFACE_HEADER_

#include "algorithm_interface.h"
#include "user_ad.h"
#include <pthread.h>
#include <unistd.h>
typedef std::map<string,int> Map;

const uint16_t MODEL_LR_DB_NO = 21;		//lushan
const uint16_t ADER_PROFILE_DB_NO = 3;		//lushan
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

float getRandomFloat()
{
	//srand((unsigned)time(NULL));
	float ans = 0.0;
	for(int i=0;i<2 ;i++){
		if (i == 0){
			ans += (rand()%10)/10000.0;
			if ( ans < 1e-7 ) ans = 1/10000.0;
		}
		else{
			ans += (rand()%10)/100000.0;
		}
	}
	return ans;
}
class Ad_Info{
public:
	string adid;//广告主id
	string ctr;//广告主ctr（天）
	string ios_ctr;
	string android_ctr;
	string other_ctr;
	string male_ctr;
	string female_ctr;
};
//声明全局变量 供模型使用
model_data **model_ptr,*model_copy_ptr;
pthread_t tid_fans_economy;
pthread_mutex_t p_lock_fans_economy;
int fans_economy_model_flg = 1;

class PromoteFansAlgorithmInterface :public AlgorithmInterface{
	public: 
		map<int, float> weights;
		map<string, int> feature_index;
		vector<string> feature_name_default;
		vector<string> feature_name_list;
		int *model_flg;//用于标记本线程是否可以创建线程，以及创建后负责创建内存的释放
		model_data *model_read_ptr;
		PromoteFansAlgorithmInterface(DbCompany*& p_db_company, int interface_id):
			AlgorithmInterface(p_db_company, interface_id){
				LOG_ERROR("init PromoteFansAlgorithmInterface.");
				time_t now;
				time(&now);
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
					pthread_create(&tid_fans_economy,NULL,_thread_t<PromoteFansAlgorithmInterface, &PromoteFansAlgorithmInterface::_RunThread>, this);
					LOG_ERROR("init PromoteFansAlgorithmInterface.model_flg:%d, pthread_create %d",model_flg,tid_fans_economy);
				}
				LOG_ERROR("init success");
		}
		~PromoteFansAlgorithmInterface(){
			LOG_ERROR("destroy PromoteFansAlgorithmInterface.");
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
		int algorithm_core(uint64_t req_id, const AccessStr& access_str, VEC_CAND& vec_cand);

		int algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, const VEC_CAND& input_vec, VEC_CAND& output_vec);
		int user_ad_history_ctr(const ACCESS_INFO* ai, const VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		//模型更新函数
		void update_model(model_conf &mcf,model_data** copy_ptr, model_data** ptr,int lsdb_num,int conf_key);
		//模型参数解析
		int parse_model_parameters(model_conf &mcf,int db_num,int key);
		//定时任务函数
		void _RunThread();
		int search_feature_index(string &key,model_data* read_ptr);
		float search_weights(string feature_name, string value,model_data *read_ptr,int value_type);
		int user_ad_ctr_estimate(const ACCESS_INFO* ai, const VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		map<string, Ad_Info> PromoteFansAlgorithmInterface::get_ad_infos(const VEC_CAND& input_vec);
		model_data* read_model();
		float getGenderScore(int gender, Ad_Info ai, model_data *model_read_ptr);
		float getPlatformScore(int platform, Ad_Info ai, model_data *model_read_ptr);
		float getAdScore(Ad_Info ai, model_data *model_read_ptr);
};

#endif
