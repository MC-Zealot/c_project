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
	LOG_ERROR("In history_ctr1");
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
		return -1;
	}
	uint16_t key_size = input_vec.size() + 1;
	LOG_ERROR("key_size: %u", key_size);
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
	LOG_ERROR("redis_flag: %d", redis_flag);
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
