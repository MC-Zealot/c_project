#ifndef _PROMOTEFANS_ALGORITHM_INTERFACE_HEADER_
#define _PROMOTEFANS_ALGORITHM_INTERFACE_HEADER_

#include "algorithm_interface.h"
#include "user_ad.h"
const uint64_t FEATURE_HASH_SIZE_YIZHOU = 6000;
float getRandomFloat() {
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
	char** feature_index;
	char** weights;
	int w_key_num;
	int f_key_num;
}model_data;

template <typename TYPE, void (TYPE::*_RunThread)() >
void* _thread_t(void* param)//线程启动函数，声明为模板函数
{
	TYPE* This = (TYPE*)param;
	This->_RunThread();
	return NULL;
}

//声明全局变量 供模型使用
model_data *model_ptr,*model_copy_ptr;
pthread_t tid_fans_economy;
pthread_mutex_t p_lock_fans_economy;
int fans_economy_model_flg = 1;

class PromoteFansAlgorithmInterface :public AlgorithmInterface{
	public:
		map<int, float> weights;
		map<string, int> feature_index;
		int *model_flg;//用于标记本线程是否可以创建线程，以及创建后负责创建内存的释放
		model_data *model_read_ptr;
		PromoteFansAlgorithmInterface(DbCompany*& p_db_company, int interface_id): AlgorithmInterface(p_db_company, interface_id){
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

			//创建模型更新线程
			if( *model_flg == 1){
				//申请初始模型内存，以免释放内存错误
				model_ptr = new model_data();
				char** m_weights = new char*[FEATURE_HASH_SIZE];
				char** m_feature_index = new char*[FEATURE_HASH_SIZE];
				for (uint32_t i = 0; i < FEATURE_HASH_SIZE; i++){
					m_weights[i] = new char[256]();
					m_feature_index[i] = new char[256]();
				}
				model_ptr->weights = m_weights;
				model_ptr->feature_index = m_feature_index;
				//创建模型线程
				pthread_create(&tid_fans_economy,NULL,_thread_t<FansTargetAlgorithmInterface, &FansTargetAlgorithmInterface::_RunThread>, this);
				//LOG_ERROR("init FansTargetAlgorithmInterface.model_flg:%d, pthread_create %d",model_flg,tid_fans_economy);
			}
			LOG_ERROR("init promotefans_algorithm_interface_2.model_flg:%d, pthread_create %d",*model_flg,tid_fans_economy);
			LOG_ERROR("init success");
		}
		~PromoteFansAlgorithmInterface(){
			LOG_ERROR("destroy promotefans_algorithm_interface_2.");
			//该线程创建的模型，负责释放模型内存
			if (*model_flg == 1){
				pthread_mutex_destroy(&p_lock_fans_economy);
				if (model_ptr != NULL){
					if(model_ptr->weights != NULL){
						for(uint32_t i=0;i<FEATURE_HASH_SIZE;++i){
							delete[] model_ptr->weights[i];
							delete[] model_ptr->feature_index[i];
						}
						if (model_ptr->weights != NULL)
							delete model_ptr->weights;
						if (model_ptr->feature_index != NULL)
							delete model_ptr->feature_index;
					}
					delete model_ptr;
					if ( model_ptr != NULL ) model_ptr = NULL;
				}
			}
			delete model_flg;
		}
	public:
		int algorithm_core(uint64_t req_id, const AccessStr& access_str,
				VEC_CAND& vec_cand);
		int algorithm_core_new(uint64_t req_id, const AccessInfo* access_info,
				const VEC_CAND& input_vec, VEC_CAND& output_vec);
		int user_ad_history_ctr(const ACCESS_INFO* ai, const VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
		//模型更新函数
		void update_model();
		//定时任务函数
		void _RunThread();
		int search_feature_index(string &key);
		float search_weights(string& feature_name, string& value);
};

#endif
