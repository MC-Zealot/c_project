#include "promotefans_algorithm_interface.h"

DYN_ALGORITHMS(PromoteFansAlgorithmInterface);

int PromoteFansAlgorithmInterface::algorithm_core(uint64_t req_id, const AccessStr& access_str,
                                                  VEC_CAND& vec_cand){
    
    LOG_ERROR("PromoteFansAlgorithmInterface::algorithm_core return 1\n");
    return 1;
}

bool dict_compare(pair<int, float> lhs, pair<int, float> rhs){
    return lhs.second > rhs.second;
}

int PromoteFansAlgorithmInterface::user_ad_history_ctr(const ACCESS_INFO* ai, const VEC_CAND& input_vec,VEC_CAND& output_vec, int num){
    //1. 取lushan数据
    LOG_ERROR("In history_ctr");
    DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
    if(NULL == p_insuff_order_interface){
        LOG_ERROR("lushan connect error!");
        return -1;
    }
    uint16_t key_size = input_vec.size() + 1;
    char** keystr = new char*[key_size];
    for(uint16_t i = 0; i < key_size; i++)
        keystr[i] = new char[256];
    int index = 0;
    float defaultctr = 0.0;
    //2. get ader
    
    for(VEC_CAND::const_iterator it = input_vec.begin();
        it!=input_vec.end(); it++){
        User_Order_Ad * ft = (User_Order_Ad*)(*it);
        sprintf(keystr[index++],"%u-%"PRIu64"", 3, ft->uid);//order_id info;
    }
    
    sprintf(keystr[index],"%u-1", 3);//默认ctr
    map<uint64_t, const char*> result;
    int count = 0;
    map<int, float> sort_ad;
    string token = ",";
    vector<string> user_profile;
    float score = 0.0;
    string def;
    int redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, key_size, result);
    if(redis_flag != 1){
        LOG_ERROR("redis_flag_error");
        //随机出广告
        for(VEC_CAND::const_iterator it = input_vec.begin();it!=input_vec.end(); it++){
            score = getRandomFloat();
            User_Order_Ad * ft = (User_Order_Ad*)(*it);
            ft->adscore = floor(score * 100000);
            sort_ad.insert(make_pair(count, score));
            LOG_ERROR("AD:%d %s ADER SCORE RANDOM:%lf", count,ft->order_id.c_str(), score);
            count++;
        }
        goto FREE_MODULE;
    }
    LOG_ERROR("EXPECT:%u GET: %u", key_size, result.size());
    for(map<uint64_t, const char*>::const_iterator iter = result.begin();iter != result.end(); iter++){
        LOG_ERROR("LUSHAN %lld:%s", iter->first, iter->second);
    }
    
    // 直接比拼ader的  ctr
    if (result.find(1) != result.end()){
        def = result[1];
        defaultctr = atof(def.c_str());
    }
    else defaultctr = 0.00043;
    
    for(VEC_CAND::const_iterator it = input_vec.begin();
        it!=input_vec.end(); it++){
        User_Order_Ad * ft = (User_Order_Ad*)(*it);
        if(result.find(ft->uid)!=result.end()){
            string str = result[ft->uid];
            score = atof(str.c_str());
        }else
            score = defaultctr;
        
        ft->adscore = floor(score * 100000);
        sort_ad.insert(make_pair(count, score));
        LOG_ERROR("AD:%d %s ADER SCORE:%lf", count,ft->order_id.c_str(), score);
        count++;
    }
    
    //释放内存
FREE_MODULE:
    for(uint16_t i=0; i < key_size; i++)
        delete[] keystr[i];
    if(keystr!=NULL)
        delete[] keystr;
    
    if (sort_ad.size() >= 1){
        vector<pair<int, float> > tmp(sort_ad.begin(), sort_ad.end());
        sort(tmp.begin(), tmp.end(), dict_compare);
        if (sort_ad.size()<3) num = sort_ad.size();
        for (int i = 0;i<num;i++){
            output_vec.push_back(input_vec[tmp[i].first]);
        }
        LOG_ERROR("BEST:%lf", tmp[0].second);
    }       
    return 1;
}


int PromoteFansAlgorithmInterface::algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, 
                                                      const VEC_CAND& input_vec, VEC_CAND& output_vec){
    ACCESS_INFO* ai = (ACCESS_INFO*) access_info;
    
    int size = input_vec.size();
    LOG_ERROR("PromoteFansAlgorithmInterface inputvec.size():%d\n", size);
    if(size <= 0){
        return 1;
    }
    
    user_ad_history_ctr(ai, input_vec, output_vec, 3);
    return 1;
}

//模型更新时间控制函数
void PromoteFansAlgorithmInterface::_RunThread(){

    while(1){
		LOG_ERROR("init PromoteFansAlgorithmInterface. void PromoteFansAlgorithmInterface::_RunThread()");
		this->update_model();
//		sleep(86400);//设置间隔时间 一天
		sleep(60);//设置间隔时间 一分钟
	}
}

//模型更新函数
void PromoteFansAlgorithmInterface::update_model(){
	time_t now;
	time(&now);
	LOG_ERROR("load model_tid begin sleep pthread:%d",tid_fans_economy);
	//申请模型内存
	char** m_weights = new char*[FEATURE_HASH_SIZE_YIZHOU];
	char** m_feature_index = new char*[FEATURE_HASH_SIZE_YIZHOU];
	for (uint32_t i = 0; i < FEATURE_HASH_SIZE_YIZHOU; i++){
		m_weights[i] = new char[256]();
		m_feature_index[i] = new char[256]();
	}
	//设置模型更新数据量的标记，以检查模型是否更新正确
	int feature_newdata_num = 0 ,weight_newdata_num = 0;
	//准备获取lushan数据进行解析
	map<uint64_t, const char*> result;
	vector<string> tmp_vector;
	string token = ";";
	//申请新模型空间struct
	model_data* m_ptr =  new model_data();
	int model_keysize = 2;
	char** keystr = new char*[model_keysize];
	for (uint16_t i = 0;i<model_keysize;++i){
		keystr[i] = new char[256];
	}
	//lushan key: feature:11-20,model:1-10
	int dbno = 21;
	int weight_index = 21;
	int feature_mapping = 22;
	sprintf(keystr[0], "%d-%d", dbno, weight_index); //model key:权重
	sprintf(keystr[1], "%d-%d", dbno, feature_mapping); //model key:特征离散映射关系
	//connect lushan
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	int result_flag = 0;
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
		goto FREE_MODULE;
	}

	//get lushan data
	result_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, model_keysize, result);
	if(result_flag != 1){
		LOG_ERROR("result_flag_error");
		goto FREE_MODULE;
	}
	LOG_ERROR("ctr_match_multi expect :%u get: %u", model_keysize, result.size());
	//解析模型数据
	if(result.find(weight_index) != result.end()){
		string info = result[weight_index];
		SplitString(info, token,tmp_vector);
		//LOG_ERROR("CTR_MATCH_MULTI LUSHAN MODEL key:1   value: %s", info.c_str());
	}
	LOG_ERROR("ctr_match_multi lushan 21 item num:%d ",tmp_vector.size());
	for(uint32_t i=0;i<tmp_vector.size();++i){
		int idx = tmp_vector[i].find(',');
		int key = atoi(tmp_vector[i].substr(0,idx).c_str());
		//考虑异常安全性，对写入key范围进行检查
		if( key < 0 || key >= FEATURE_HASH_SIZE_YIZHOU ){
			LOG_ERROR("model update erro key:%d value:%f", key,atof(tmp_vector[i].substr(idx+1).c_str()));
		}
		else{
			sprintf(m_weights[key],"%f",atof(tmp_vector[i].substr(idx+1).c_str()));
			weight_newdata_num++;
		}
		//LOG_ERROR("CTR_MODEL WEIGHTS key:%d value: %f", key,atof(tmp_vector[i].substr(idx+1).c_str()));
	}
	//解析特征离散数据
	tmp_vector.clear();
	if(result.find(feature_mapping) != result.end()){
		string info = result[feature_mapping];
		SplitString(info, token,tmp_vector);
		//LOG_ERROR("CTR_MATCH_MULTI LUSHAN MODEL :11 GET: %s", info.c_str());
	}
	LOG_ERROR("ctr_match_multi lushan 22 item num:%d ",tmp_vector.size());
	for(uint32_t i=0;i<tmp_vector.size();++i){
		int idx = tmp_vector[i].find(',');
		int key = GetFeatureHash(tmp_vector[i].substr(0,idx).c_str());
		//LOG_ERROR("CTR_MODEL FEATURE HASH key:%d value: %s", key,tmp_vector[i].c_str());
		int search_num = 0;//最大查空位次数
		while( strlen(m_feature_index[key]) > 1 && search_num < 20) {
			search_num++;
			key = key + 5 ; //该位置已被使用寻找下一个位置
		}
		//若超过最大重复数仍没有找到，则输出日志
		if (strlen(m_feature_index[key]) > 1 || search_num >= 20){
			LOG_ERROR("model update erro key:%d value:%s search_num:%d,k_h:%s", key,tmp_vector[i].c_str(),search_num,m_feature_index[key]);
		}else if( key < 0 || key >= FEATURE_HASH_SIZE_YIZHOU ){//考虑异常安全性，对写KEY范围做检查
			LOG_ERROR("model update erro: key:%d value:%s  search_num:%d,k_h:%s", key,tmp_vector[i].c_str(),search_num,m_feature_index[key]);
		}else{
			sprintf(m_feature_index[key],"%s",tmp_vector[i].c_str());
			feature_newdata_num++;
		}
	}

	LOG_ERROR("load model begin lock1 sleep pthread:%d",tid_fans_economy);
	//检查模型新数据量是正确,错误则goto free model
	if(feature_newdata_num < 1000 || weight_newdata_num < 1000 ){
		result_flag = 0;
		goto FREE_MODULE;
	}
	m_ptr->weights = m_weights;
	m_ptr->feature_index = m_feature_index;
	//加锁 更新模型
	pthread_mutex_lock(&p_lock_fans_economy);
	LOG_ERROR("addlock update:%d",tid_fans_economy);
	model_copy_ptr = model_ptr;
	model_ptr = m_ptr;
	LOG_ERROR("releaselock update:%d",tid_fans_economy);
	//解锁
	pthread_mutex_unlock(&p_lock_fans_economy);
	sleep(3);
	//释放原模型内存
	if(model_copy_ptr->weights != NULL){
		for(uint32_t i=0;i<FEATURE_HASH_SIZE_YIZHOU;++i){
			delete[] model_copy_ptr->weights[i];
			delete[] model_copy_ptr->feature_index[i];
		}
		if (model_copy_ptr->weights != NULL)
			delete model_copy_ptr->weights;
		if (model_copy_ptr->feature_index != NULL)
			delete model_copy_ptr->feature_index;
	}
	//new 出来的要delete掉
	delete model_copy_ptr;
	LOG_ERROR("LOAD MODEL BEGIN LOCK2 SLEEP PTHREAD:%d",tid_fans_economy);
	LOG_ERROR("LOAD MODEL FINISHED SLEEP PTHREAD:%d",tid_fans_economy);
	// free memery
FREE_MODULE:
	//释放临时空间
	for (uint32_t i = 0; i<model_keysize; ++i)
		delete[] keystr[i];
	if (keystr != NULL) delete keystr;
	//模型内存未使用，释放
	if(result_flag != 1){
		 for(uint32_t i=0;i<FEATURE_HASH_SIZE_YIZHOU;++i){
			delete[] m_weights[i];
			delete[] m_feature_index[i];
		}
		if(m_weights != NULL) delete m_weights;
		if(m_feature_index != NULL) delete m_feature_index;
		delete m_ptr;
	}
}
