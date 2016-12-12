#include "fanstarget_ad_algorithm_interface.h"
#include "fanstarget_ad_algorithm_high_quality.h"

DYN_ALGORITHMS(FansTargetAlgorithmInterface);
int FansTargetAlgorithmInterface::algorithm_core(uint64_t req_id,const AccessStr& access_str, VEC_CAND& vec_cand)
{
	return 1;	
}
bool ctr_ABtest(uint64_t uid){
	uint64_t mod = uid%100/10;
	if(mod < 5)
		return true;

	return false;
}


int FansTargetAlgorithmInterface::algorithm_core_new(uint64_t req_id, 
		const AccessInfo* access_info, const VEC_CAND& input_vec, VEC_CAND& output_vec)
{
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
	uint16_t strategy = 1;	//性别优先
	ACCESS_INFO* ai = (ACCESS_INFO*)access_info;
	uint16_t num = ai->num;

	//1. 分包，将集合分为 0:客户包，1:不能完成包, 2:精准定向包
	VEC_CAND fen_tiao;
	VEC_CAND insuff_impression;
	VEC_CAND industry_ctr;	//存储行业用于ctr订单
	VEC_CAND high_quality;
	VEC_CAND specify_uid;	//指定账户
	VEC_CAND high_quality_nonfans;
	VEC_CAND::const_iterator impression; //指向  buy_amout / max_package 最大的订单,或者高级定向包
	set<string> advanced;	//高级定向包实际adid个数
	double max_occupy_ratio = 0.0;  // 记录  buy_amount / max_package 最大值
	uint64_t need_butou = 0;        //需要补投总量
	for(VEC_CAND::const_iterator it = input_vec.begin(); it != input_vec.end(); ++it){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		//1、包的解析
		high_quality_flowAnalysis(ft);
		//2、流量控制（需要）
		time_t now;
		time(&now);
		double theory_impression = theory_impression_cal(ft, now);

		// 测试开始，统计所有的高级定向包的流量
		struct timeval start_time_now;
        	gettimeofday(&start_time_now,NULL);
		if(ft->source == 2){
			LOG_ERROR("UID:%"PRIu64" all flow source: %d time %d", ai->come_uid, ft->source, (start_time_now.tv_sec - ft->begin_time));
		}
		// 测试结束

		if(flowControl(ft->order_uid, ft->begin_time, ft->finish_amount, theory_impression, 2, ft->source)){
			// 测试开始，统计控制住的高级定向包
			//if (ft->source == 2){
			//	LOG_ERROR("UID:%"PRIu64" control flow source: %d time %d", ai->come_uid, ft->source, (start_time_now.tv_sec - ft->begin_time));
			//}
			// 测试结束
			continue;
                }
		//3、放到对应的vector中
		double ratio;
		switch (ft->source) {
			case 0:
			case 1:
				fen_tiao.push_back(*it);
				ratio = double(ft->buy_amount) / double(ft->max_package + 3.0);
				if((advanced.size()==0) && ratio > max_occupy_ratio){
					impression = it;
					max_occupy_ratio = ratio;
				}
				break;
			case 2:
				fen_tiao.push_back(*it);
				advanced.insert(ft->adid);
				impression = it;
				break;
			case 3:
				specify_uid.push_back(*it);
				break;
			case 10:
				need_butou += (ft->buy_amount - ft->finish_amount);
                        	if(ft->finish_amount < (ft->buy_amount * 0.8)){
                                	insuff_impression.push_back(*it);
				}
				break;
			case 2102:
				industry_ctr.push_back(*it);
				break;
			case 2100:
				high_quality.push_back(*it);
				break;
			case 2103:
				high_quality_nonfans.push_back(*it);
				break;
		}
		
	}
	uint32_t flag = advanced.size();	//高级定向包的个数 
	double threshold = need_butou * 5.0 / FLOW_AMOUNT;	//以前倍数是5，因为下面放大倍数，这里调小
	
	//LOG_ERROR("UID:%"PRIu64"specify_uid.size(): %d", ai->come_uid, specify_uid.size());	
	if(specify_uid.size() > 0){
		LOG_ERROR("UID:%"PRIu64" specify_uid.size(): %d", ai->come_uid, specify_uid.size());
		/*
		output_vec.push_back(specify_uid[0]);
		algorithm_log(output_vec, 9, num, 1);
		log_print(ai, output_vec, 9);
		so_consume_time(start_time);
		return 1;*/
		if (specify_uid.size() == 1){
			output_vec.push_back(specify_uid[0]);
			algorithm_log(output_vec, 15, num, 1);
			log_print(ai, output_vec, 15);
			so_consume_time(start_time);
			return 1;
		}else{
			output_vec.push_back(specify_uid[0]);
                        algorithm_log(output_vec, 16, num, 1);
                        log_print(ai, output_vec, 16);
                        so_consume_time(start_time);
                        return 1;
		}
		
		/*		
		if (ctr_ABtest(ai->come_uid)){
			output_vec.push_back(specify_uid[0]);
                        algorithm_log(output_vec, 16, num, 1);
                        log_print(ai, output_vec, 16);
                        so_consume_time(start_time);
                        return 1;
		*/
			/*ctr_match_multi(ai, specify_uid, output_vec, num);//需要读一下
			algorithm_log(output_vec, 10, num, 1);
                        log_print(ai, output_vec, 10);
                        so_consume_time(start_time);
                        return 1;
			*/
		/*
		}else{
			industry_ctr_match_level1(ai, specify_uid, output_vec, num);
			algorithm_log(output_vec, 17, num, 1);
                        log_print(ai, output_vec, 17);
                        so_consume_time(start_time);
                        return 1;
		}*/
	}
	// 精准流量判断
	
	LOG_ERROR("high_quality.size(): %d", high_quality.size());
	if( (high_quality.size() > 0) ){//精准定向
		output_vec.push_back(high_quality[0]);
		algorithm_log(output_vec, 7, num, 1);
		log_print(ai, output_vec, 7);
		so_consume_time(start_time);
		return 1;
	}
	if(industry_ctr.size() > 0){//优化的定向包
		//if(industry_ctr.size() == 1 || !ctr_ABtest(ai->come_uid)){
		if(industry_ctr.size() == 1){
			output_vec.push_back(industry_ctr[0]);
			//supply_order(num, falg, fen_tiao, output_vec);
			algorithm_log(output_vec, 7, num, 1);
			log_print(ai, output_vec, 7);
			so_consume_time(start_time);
			return 1;
		}
		//有多个定向包，根据CTR值返回
		if(ctr_ABtest(ai->come_uid)){
			//industry_ctr_match(ai,industry_ctr, output_vec, num);//需要读一下
			ctr_match_multi(ai,industry_ctr, output_vec, num);//需要读一下
			//supply_order(num,flag,fen_tiao,output_vec);	//如果有粉条，可以补足
			algorithm_log(output_vec, 13, num, 1);
			log_print(ai, output_vec, 13);
			so_consume_time(start_time);
			return 1;
		}else{	//对比陶总的ctr
			industry_ctr_match_level1(ai,industry_ctr, output_vec, num);
			algorithm_log(output_vec, 14, num, 1);
			log_print(ai, output_vec, 14);
			so_consume_time(start_time);
			return 1;
		}
	}
	if( (high_quality_nonfans.size() > 0) ){//亲密度非粉
		output_vec.push_back(high_quality_nonfans[0]);
		algorithm_log(output_vec, 7, num, 1);
		log_print(ai, output_vec, 7);
		so_consume_time(start_time);
		return 1;
	}

	//2. 粉条包为空，投放曝光不足
	if(fen_tiao.size() == 0){
		//2.1 低频次,投放最慢的
		if(ai->impression_counts < MIN_FREQUENCY){
			if(insuff_impression.size() == 0){
				// 2.11 曝光不足为空，直接返回
				LOG_ERROR("1 UID:%"PRIu64" counts:%"PRIu32" num:%"PRIu16" return 0", 
						ai->come_uid,ai->impression_counts,num);
				return 1;   
			}else{
				double chance = get_random();
				if(chance <= threshold){
					// 2.12 选取曝光不足最慢的，返回. 这里没有非粉订单，不补充
					slow_order(insuff_impression,output_vec);
					algorithm_log(output_vec,2,num,strategy);
					log_print(ai, output_vec,2);
					so_consume_time(start_time);
					return 1;
				}else{
					LOG_ERROR("20 UID:%"PRIu64" counts:%"PRIu32" num:%"PRIu16" return 0 probability reject",
							ai->come_uid,ai->impression_counts,num);
					return 1;
				}
			}

		}else{
			//2.2 非低频 ,不曝光
			LOG_ERROR("3 UID:%"PRIu64" counts:%"PRIu32" num:%"PRIu16" return 0", ai->come_uid,ai->impression_counts,num);
			return 1;   
		}
	}

	// 3. 投非粉包
	//3.1 高级定向包/最大包/分组feed
	if(flag > 0 || max_occupy_ratio >= BUY_OCCUPY_MAX_PACKAGE){
		output_vec.push_back(*impression);
		//有非粉，按照num补足非粉
		supply_order(num,flag,fen_tiao,output_vec);
		relation_replace(fen_tiao,output_vec,ai);
		algorithm_log(output_vec,4,num,strategy);
		log_print(ai, output_vec,4);
		so_consume_time(start_time);
		return 1;   
	}
	//3.2 没有高级定向包/最大包
	if(flag==0 && ai->impression_counts < MAX_FREQUENCY){
		slow_order(fen_tiao,output_vec);
		//有非粉，按照num补足非粉
		supply_order(num,flag,fen_tiao,output_vec);
		algorithm_log(output_vec,5,num,strategy);
		log_print(ai, output_vec,5);
		so_consume_time(start_time);
		return 1;
	}
	//3.3 不存在最大包，频次较高， 不曝光
	LOG_ERROR("6 UID:%"PRIu64" counts:%"PRIu32" num:%"PRIu16" return 0", ai->come_uid,ai->impression_counts,num);
	so_consume_time(start_time);
	return 1;
}

double FansTargetAlgorithmInterface::get_random(){
	struct timeval now;
	gettimeofday(&now,NULL);
	srand((unsigned)now.tv_usec);
	return rand()/(RAND_MAX+1.0);
}
bool dict_compare(pair<int, float> lhs, pair<int, float> rhs){
	return lhs.second > rhs.second;
}

//模型更新时间控制函数
void FansTargetAlgorithmInterface::_RunThread(){
	
    while(1){
		LOG_ERROR("init FansTargetAlgorithmInterface. void FansTargetAlgorithmInterface::_RunThread()");
		this->update_model();
		sleep(86400);//设置间隔时间 测试设置1分钟
	}
}

//模型更新函数
void FansTargetAlgorithmInterface::update_model(){
	time_t now;
	time(&now);
	LOG_ERROR("LOAD MODEL_TID BEGIN SLEEP PTHREAD:%d",tid_fans_economy);
	//申请模型内存
	char** m_weights = new char*[FEATURE_HASH_SIZE];
	char** m_feature_index = new char*[FEATURE_HASH_SIZE];
	for (uint32_t i = 0; i < FEATURE_HASH_SIZE; i++){
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
	sprintf(keystr[0],"%d-%d",21,1);//model key:1-10
	sprintf(keystr[1],"%d-%d",21,11);//model key:11-20
	//connect lushan
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	int redis_flag = 0;
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
		goto FREE_MODULE;
	}

	//get lushan data
	redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, model_keysize, result);
	if(redis_flag != 1){
		LOG_ERROR("redis_flag_error");
		goto FREE_MODULE;
	}
	LOG_ERROR("CTR_MATCH_MULTI EXPECT :%u GET: %u", model_keysize, result.size());
	//解析模型数据
	if(result.find(1) != result.end()){
		string info = result[1];
		SplitString(info, token,tmp_vector);
		//LOG_ERROR("CTR_MATCH_MULTI LUSHAN MODEL key:1   value: %s", info.c_str());
	}
	LOG_ERROR("CTR_MATCH_MULTI LUSHAN 1 item num:%d ",tmp_vector.size());
	for(uint32_t i=0;i<tmp_vector.size();++i){
		int idx = tmp_vector[i].find(',');
		int key = atoi(tmp_vector[i].substr(0,idx).c_str());
		//考虑异常安全性，对写入key范围进行检查
		if( key < 0 || key >= FEATURE_HASH_SIZE )
			LOG_ERROR("MODEL UPDATE ERRO key:%d value:%f", key,atof(tmp_vector[i].substr(idx+1).c_str()));
		else{
			sprintf(m_weights[key],"%f",atof(tmp_vector[i].substr(idx+1).c_str()));
			weight_newdata_num++;
		}
		//LOG_ERROR("CTR_MODEL WEIGHTS key:%d value: %f", key,atof(tmp_vector[i].substr(idx+1).c_str()));
	}
	//解析特征离散数据
	tmp_vector.clear();
	if(result.find(11) != result.end()){
		string info = result[11];
		SplitString(info, token,tmp_vector);
		//LOG_ERROR("CTR_MATCH_MULTI LUSHAN MODEL :11 GET: %s", info.c_str());
	}
	LOG_ERROR("CTR_MATCH_MULTI LUSHAN 11 item num:%d ",tmp_vector.size());
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
		if (strlen(m_feature_index[key]) > 1 || search_num >= 20)
			LOG_ERROR("MODEL UPDATE ERRO key:%d value:%s search_num:%d,k_h:%s", key,tmp_vector[i].c_str(),search_num,m_feature_index[key]);
		//考虑异常安全性，对写KEY范围做检查
		else if( key < 0 || key >= FEATURE_HASH_SIZE )
			LOG_ERROR("MODEL UPDATE ERRO: key:%d value:%s  search_num:%d,k_h:%s", key,tmp_vector[i].c_str(),search_num,m_feature_index[key]);
		else{
			sprintf(m_feature_index[key],"%s",tmp_vector[i].c_str());
			feature_newdata_num++;
		}
	}

	LOG_ERROR("LOAD MODEL BEGIN LOCK1 SLEEP PTHREAD:%d",tid_fans_economy);
	//检查模型新数据量是正确,错误则goto free model
	if(feature_newdata_num < 1000 || weight_newdata_num < 1000 ){
		redis_flag = 0;
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
		for(uint32_t i=0;i<FEATURE_HASH_SIZE;++i){
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
	if(redis_flag != 1){
		 for(uint32_t i=0;i<FEATURE_HASH_SIZE;++i){
			delete[] m_weights[i];
			delete[] m_feature_index[i];
		}
		if(m_weights != NULL) delete m_weights;
		if(m_feature_index != NULL) delete m_feature_index;
		delete m_ptr;
	}
}


int FansTargetAlgorithmInterface::search_feature_index(string& key){
	int k = GetFeatureHash(key.c_str());
	//LOG_ERROR("MODEL UPDATE CHECK:key-%s,value-%d",key.c_str(),k);
	int b_i = k,idx = 0,iter_limit = 0;
	string str="";
	while(iter_limit <20 && k<FEATURE_HASH_SIZE){
		str = model_read_ptr->feature_index[k];
		idx = str.find(',');
		if (strcmp(str.substr(0,idx).c_str(),key.c_str()) == 0){
			LOG_ERROR("MODEL_CHECK COMPARE :key_s-%s,key_f-%s,index_s:%d,search_num:%d",key.c_str(),str.c_str(),b_i,k);
			break;
		}
		k = k + 5;
		iter_limit++;
	}
	if( iter_limit >= 20 || k >= FEATURE_HASH_SIZE ) return -1;
	//LOG_ERROR("MODEL_CHECK FIND :key-%s,key_find:%s value%d",key.c_str(),str.c_str(),atoi(str.substr(idx+1).c_str()));
	//考虑异常安全性,对返回值做检查
	int target_v = atoi(str.substr(idx+1).c_str());
	if( target_v < 0 || target_v >= FEATURE_HASH_SIZE) return -1;
	return target_v;
}



float FansTargetAlgorithmInterface::search_weights(string& feature_name, string& value){
	pthread_mutex_lock(&p_lock_fans_economy);
        //LOG_ERROR("addlock search:%d",tid_fans_economy);
	model_read_ptr = model_ptr;
	//LOG_ERROR("releaselock search:%d",tid_fans_economy);
	//LOG_ERROR("LOAD MODEL BEGIN LOCK1 SLEEP PTHREAD:%d",tid_fans_economy);
        pthread_mutex_unlock(&p_lock_fans_economy);
	if (model_read_ptr == NULL || model_read_ptr->feature_index == NULL || model_read_ptr->weights == NULL ){
		LOG_ERROR("MODEL UPDATE CHECK: model_read_ptr is NULL");
		return 0.0;
	}
	string key = feature_name + "_"+ value;
	int k = search_feature_index(key);
	int back_i = 1;
	char val[128];
	sprintf(val,"%lf",atof(value.c_str()));
	while(-1 == k && back_i < 4 ){
		if(value.length()-back_i <0) break;
		val[value.length()-back_i] = 0;
		key = feature_name+"_" + val;
		k = search_feature_index(key);
		++back_i;
	}
	if(-1 != k) {
		LOG_ERROR("MODEL UPDATE CHECK:weights-%f",atof(model_read_ptr->weights[k]));
		return atof(model_read_ptr->weights[k]);
	}
	return 0.0;
}

/*
 * 历史 查找权重版本
float FansTargetAlgorithmInterface::search_weights(string& feature_name, string& value){
	string key = feature_name + value;
	if(feature_index.find(key) != feature_index.end()){
		int index = feature_index[key];
		if(weights.find(index) != weights.end()){
			return weights[index];
		}
	}
	// 找到不到,后退一位查找
	if (value.length()> 4){
		key = feature_name + value.substr(0,value.length()-1);
		if(feature_index.find(key) != feature_index.end()){
			int index = feature_index[key];
			if(weights.find(index) != weights.end()){
				return weights[index];
			}
		}
	}
	return 0.0;
}
*/

float FansTargetAlgorithmInterface::cross_feature(string& left, string& right, string& left_value, string& right_value){
	string name = left + "_" + right;
	string value = left_value + "_" + right_value;
	float score = search_weights(name, value);
	//LOG_ERROR("Fea:%s:%s SCORE:%lf", name.c_str(),value.c_str(),score);
	return score;

}


int FansTargetAlgorithmInterface::ctr_match_multi(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num){
	//1. 取lushan数据
	LOG_ERROR("In ctr_match_multi");

	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
		return -1;
	}
	uint16_t key_size = input_vec.size()+2;
	char** keystr = new char*[key_size];
	for(uint16_t i = 0; i < key_size; i++)
		keystr[i] = new char[256];
	sprintf(keystr[0],"%d-%"PRIu64"",22, ai->come_uid);
	//1. get user profile
	int index = 1;
	//2. get ader ctr
	for(VEC_CAND::const_iterator it = input_vec.begin();
		it!=input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		sprintf(keystr[index++],"%u-%"PRIu64"",29,ft->order_uid);//order_uid info;
		//sprintf(keystr[index++],"%u-%s",21,ft->mid.c_str());    //mid info
	}


	//3.  get ader defualt value
	sprintf(keystr[index++],"%u-%s",29,"1");

	//lushan query data result
	map<uint64_t, const char*> result;
	int count = 0;

	// ad score sorted
	map<int, float> sort_ad;
	string token = ",";
	//split user_profile
	vector<string> user_profile;
	vector<string> default_ctr;
	string user_gender ="1";
	string plat = "2";
	

	if(strcmp((ai->device_type).c_str(),"iOS")==0 || strcmp((ai->device_type).c_str(),"iphone")==0 || strcmp((ai->device_type).c_str(),"ipad")==0) 
		plat = "1";
	else if(strcmp((ai->device_type).c_str(),"android")==0)
		plat = "2";
	else if(strcmp((ai->device_type).c_str(),"pc")==0)
		plat = "3";
	

	//get lushan data
	int redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, key_size, result);
	if(redis_flag != 1){
		LOG_ERROR("redis_flag_error");
		goto FREE_MODULE;
	}
	LOG_ERROR("CTR_MATCH_MULTI EXPECT:%u GET: %u", key_size, result.size());

	//for(map<uint64_t, const char*>::const_iterator iter = result.begin();iter != result.end(); iter++){
	//	LOG_ERROR("CTR_MATCH_MULTI LUSHAN %lld:%s", iter->first, iter->second);
	//}
	

	//0 . user profile
	if(result.find(ai->come_uid) != result.end()){
		string info = result[ai->come_uid];
		SplitString(info, token, user_profile);
	}
	if (user_profile.size()<2){
		user_gender = ai->gender;
	}
	else{
		user_gender = user_profile[0];
	}
	//check valid
	if (strcmp(user_gender.c_str(),"0")<0 || strcmp(user_gender.c_str(),"2")>0){
		LOG_ERROR("USER GENDER DATA ERROR: %s", user_gender.c_str());
		goto FREE_MODULE;
	}

	//LOG_ERROR("USER INFO GENDER:%s,PLAT:%s", user_gender.c_str(), plat.c_str());
	LOG_ERROR("CTR_MATCH_MULTI USER INFO UID:%"PRIu64",GENDER:%s,PLAT:%s", ai->come_uid,user_gender.c_str(), plat.c_str());
	//1. default ctr   //0-1:feifen_ctr orientation_ctr 2-4:gender0-2 ctr 5-8:platform_ctr 0-3
	if(result.find(1) != result.end()){
		string info = result[1];
		string sel_info = info.substr(1,info.size()-2);
		//LOG_ERROR("CTR_MATCH_MULTI USER INFO DEFAULT CTR:%s", sel_info.c_str());
		SplitString(sel_info, token, default_ctr);
	}



	for(VEC_CAND::const_iterator it = input_vec.begin();it!=input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		//
		float score = 0.0;
		vector<string> ader_profile;
		vector<string> ader_ctr;

		//2 ader
		if(result.find(ft->order_uid)!=result.end()){
			//2.1
			string info = result[ft->order_uid];
			if(info.size() > 9){
				string ader_info = info.substr(1,info.size()-2);
				SplitString(ader_info, token, ader_ctr);
			}
			if( ader_ctr.size() == 9 && default_ctr.size() == 9){
				//industry ctr  0:feifen,1: 万能包
				if(ft->field_id =="" || ft->field_id == "0"){
					string query_name = "f";
					if(strcmp(ader_ctr[0].c_str() ,"0.0") == 0){
						score += search_weights(query_name,default_ctr[0]);
					}
					else{
						score += search_weights(query_name,ader_ctr[0]);
					}
				}
				else{
					string query_name = "o";
					if(strcmp(ader_ctr[1].c_str() ,"0.0") == 0){
						score += search_weights(query_name,default_ctr[1]);
					}
					else{
						score += search_weights(query_name,ader_ctr[1]);
					}
				}
				//gender ctr  user_profile[0]
				if(user_gender == ""){
					// male as default
					string query_name = "g1";
					score += search_weights(query_name,default_ctr[3]);
				}
				else if(strcmp(ader_ctr[2+atoi(user_gender.c_str())].c_str() ,"0.0") == 0){
					string query_name = "g"+user_gender;
					score += search_weights(query_name,default_ctr[2+atoi(user_gender.c_str())]);
				}
				else{
					string query_name = "g"+user_gender;
					score += search_weights(query_name,ader_ctr[2+atoi(user_gender.c_str())]);
				}
				//platform ctr
				if(plat == ""){
					//android as default
					string query_name = "p2";
					score += search_weights(query_name,default_ctr[7]);
				}
				else if(strcmp(ader_ctr[5+atoi((plat).c_str())].c_str() ,"0.0") == 0){
					string query_name = "p"+plat;
					score += search_weights(query_name,default_ctr[5+atoi((plat).c_str())]);
				}
				else{
					string query_name = "p"+plat;
					score += search_weights(query_name,ader_ctr[5+atoi((plat).c_str())]);
				}
			}
			else{
				//广告主ctr列数有问题,根据访问用户信息进行默认值的选择
				//0-1:feifen_ctr orientation_ctr 2-4:gender0-2 ctr 5-8:platform_ctr 0-3
				if(default_ctr.size() ==9 ){
					if(user_gender != ""){
						// male as default
						string query_name = "g"+user_gender;
						score += search_weights(query_name,default_ctr[2+atoi(user_gender.c_str())]);
					}
					else{
						string query_name = "g1";
						score += search_weights(query_name,default_ctr[3]);
					}
					if(plat != ""){
						string query_name = "p"+plat;
						score += search_weights(query_name,default_ctr[5+atoi((plat).c_str())]);
					}
					else{
						string query_name = "p2";
						score += search_weights(query_name,default_ctr[7]);
					}
					if(ft->field_id =="" || ft->field_id == "0"){
						string query_name = "f";
						score += search_weights(query_name,default_ctr[0]);
					}
					else{
						string query_name = "o";
						score += search_weights(query_name,default_ctr[1]);
					}
				}
				//默认值也出错，则设置固定值
				else{
					score += -0.151707;
				}
			}
		}
		else{
			//没广告主信息,根据访问用户信息进行默认值的选择
			if(default_ctr.size() ==9 ){
				if(user_gender != ""){
					// male as default
					string query_name = "g"+user_gender;
					score += search_weights(query_name,default_ctr[2+atoi(user_gender.c_str())]);
				}
				else{
					string query_name = "g1";
					score += search_weights(query_name,default_ctr[3]);
				}
				if(plat != ""){
					string query_name = "p"+plat;
					score += search_weights(query_name,default_ctr[5+atoi((plat).c_str())]);
				}
				else{
					string query_name = "p2";
					score += search_weights(query_name,default_ctr[7]);
				}
				if(ft->field_id =="" || ft->field_id == "0"){
					string query_name = "f";
					score += search_weights(query_name,default_ctr[0]);
				}
				else{
					string query_name = "o";
					score += search_weights(query_name,default_ctr[1]);
				}
			}
			//默认值也出错，则设置固定值
			else{
				score += -0.151707;
			}
		}
		sort_ad.insert(make_pair(count, score));
		LOG_ERROR("CTR_MATCH_MULTI AD:%d ADID:%s ADUID:%"PRIu64" FIELD:%s SCORE:%lf", count,ft->adid.c_str(),ft->order_uid,ft->field_id.c_str(), score);
		//LOG_ERROR("AD:%d %s ctr_match_multi SCORE:%lf", count,ft->adid.c_str(), score);
		count++;
	}
	//3. 释放内存
FREE_MODULE:
	for(uint16_t i=0; i < key_size; i++)
		delete[] keystr[i];
	if(keystr!=NULL)
		delete[] keystr;
	//4.  返回最优订单
	if (sort_ad.size() >= 2){
		vector<pair<int, float> > tmp(sort_ad.begin(), sort_ad.end());
		sort(tmp.begin(), tmp.end(), dict_compare);
		output_vec.push_back(input_vec[tmp[0].first]);
		LOG_ERROR("BEST:%lf", tmp[0].second);
	}
	//LOG_ERROR("END industry_ctr_match");
	return 1;
}



int FansTargetAlgorithmInterface::industry_ctr_match_level1(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num){
	//1. 取lushan数据
	LOG_ERROR("In industry_ctr_match");
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
		return -1;
	}
	uint16_t key_size = input_vec.size();
	char** keystr = new char*[key_size];
	for(uint16_t i = 0; i < key_size; i++)
		keystr[i] = new char[256];
	int index = 0;
	//2. get ader
	for(VEC_CAND::const_iterator it = input_vec.begin();
		it!=input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		sprintf(keystr[index++],"%u-%"PRIu64"",29,ft->order_uid);//order_uid info;
	}
	map<uint64_t, const char*> result;
	int count = 0;
	map<int, float> sort_ad;
	string token = ",";
	vector<string> user_profile;
	float score = 0.0;
	int redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, key_size, result);
	if(redis_flag != 1){
		LOG_ERROR("redis_flag_error");
		goto FREE_MODULE;
	}
	LOG_ERROR("EXPECT:%u GET: %u", key_size, result.size());
	for(map<uint64_t, const char*>::const_iterator iter = result.begin();iter != result.end(); iter++){
		LOG_ERROR("LUSHAN %lld:%s", iter->first, iter->second);
	}

	// 直接比拼ader的  ctr
	
	for(VEC_CAND::const_iterator it = input_vec.begin();
		it!=input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		if(result.find(ft->order_uid)!=result.end()){
			// [0.00427,0.00328]
			string str = result[ft->order_uid];
			int idx = str.find(",");
			if(idx != -1){
				string temp = str.substr(idx+1);
				//0.00328]
				string strscore = temp.substr(0, temp.length()-1);
				if(strscore == "0")
					score = 0.00328;
				else
					score = atof(strscore.c_str());
			}else
				score = 0.00328;
		}else
			score = 0.00328;

		sort_ad.insert(make_pair(count, score));
		LOG_ERROR("AD:%d %s ADER SCORE:%lf", count,ft->adid.c_str(), score);
		count++;
	}

	//释放内存
FREE_MODULE:
	for(uint16_t i=0; i < key_size; i++)
		delete[] keystr[i];
	if(keystr!=NULL)
		delete[] keystr;

	if (sort_ad.size() >= 2){
		vector<pair<int, float> > tmp(sort_ad.begin(), sort_ad.end());
		sort(tmp.begin(), tmp.end(), dict_compare);
		output_vec.push_back(input_vec[tmp[0].first]);
		LOG_ERROR("BEST:%lf", tmp[0].second);
	}
	return 1;
}


int FansTargetAlgorithmInterface::industry_ctr_match(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num){
	//1. 取lushan数据
	LOG_ERROR("In industry_ctr_match");
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
		return -1;
	}
	uint16_t key_size = input_vec.size()*2+1;
	char** keystr = new char*[key_size];
	for(uint16_t i = 0; i < key_size; i++)
		keystr[i] = new char[256];
	sprintf(keystr[0],"%d-%"PRIu64"",22, ai->come_uid);
	//1. get user
	int index = 1;
	//2. get ader
	for(VEC_CAND::const_iterator it = input_vec.begin();
		it!=input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		sprintf(keystr[index++],"%u-%"PRIu64"",21,ft->order_uid);//order_uid info;
		sprintf(keystr[index++],"%u-%s",21,ft->mid.c_str());    //mid info
	}
	map<uint64_t, const char*> result;
	int count = 0;
	map<int, float> sort_ad;
	string token = ",";
	vector<string> user_profile;
	int redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, key_size, result);
	if(redis_flag != 1){
		LOG_ERROR("redis_flag_error");
		goto FREE_MODULE;
	}
	LOG_ERROR("EXPECT:%u GET: %u", key_size, result.size());
	for(map<uint64_t, const char*>::const_iterator iter = result.begin();iter != result.end(); iter++){
		LOG_ERROR("LUSHAN %lld:%s", iter->first, iter->second);
	}

	//0 . user profile
	if(result.find(ai->come_uid) != result.end()){
		string info = result[ai->come_uid];
		SplitString(info, token, user_profile);
	}

	for(VEC_CAND::const_iterator it = input_vec.begin();
		it!=input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		//所有广告,时间,user 权重一样，可以省略	,所以需要交叉特征
		float score = 0.0;
		vector<string> ader_profile;
		//1. ad_industry_id
		string ad_industry_id = "ad_industry_id";
		score += search_weights(ad_industry_id, ft->field_id);
		//LOG_ERROR("Fea industry: %s, %lf", ft->field_id.c_str(), score);

		//2 ader
		if(result.find(ft->order_uid)!=result.end()){
			//2.1 广告主信息, 暂时不加
			string info = result[ft->order_uid];
			int idx1 = info.find(";");
			if(idx1 != -1){
				string ader = info.substr(idx1+1);
				SplitString(ader, token, ader_profile);
				if(ader_profile.size() == ader_names.size()){
					uint32_t i = 0;
					for(; i < ader_names.size() - 2; i++){
						score += search_weights(ader_names[i], ader_profile[i]);
						//LOG_ERROR("Feature:%s SCORE:%lf",ader_names[i].c_str(), score);
					}
					if(user_profile.size() > 0 && user_profile[0] == "2")
						i++;
					score += search_weights(ader_names[i], ader_profile[i]);					
				}
				//如果有user信息，交叉特征
				if(user_profile.size() > 4 && ader_profile.size() > 4){
					score += cross_feature(user_names[0], ader_names[0], user_profile[0], ader_profile[0]);
					score += cross_feature(user_names[2], ader_names[2], user_profile[2], ader_profile[2]);
					score += cross_feature(user_names[4], ader_names[4], user_profile[4], ader_profile[4]);
				}
			}else{
				//没有广告主信息，借用 user=ader 时候的信息
				SplitString(info, token, ader_profile);
				//
				if(ader_names.size() >= 6 && ader_profile.size() >= 6)
					for(uint32_t i = 0; i < 6; i++){
						score += search_weights(ader_names[i], ader_profile[i]);
						//LOG_ERROR("Feature:%s %s SCORE:%lf",ader_names[i].c_str(),ader_profile[i].c_str(), score);
					}
				//gender  均值, 写死了，后续改
				score += -0.151707;

				//如果有user信息，交叉特征
				if(user_profile.size() > 4 && ader_profile.size() > 4){
					score += cross_feature(user_names[0], ader_names[0], user_profile[0], ader_profile[0]);
					score += cross_feature(user_names[2], ader_names[2], user_profile[2], ader_profile[2]);
					score += cross_feature(user_names[4], ader_names[4], user_profile[4], ader_profile[4]);
				}
			}
		}else{
			//没广告主信息 全部取默认均值
			string value = "default";
			for(uint32_t i = 0; i < ader_names.size() - 2; i++){
				score += search_weights(ader_names[i], value);
			}
			//gender 默认值
			score += -0.151707;
		}

		//3, mid
		ader_profile.clear();
		uint64_t mid = strtoull(ft->mid.c_str(),NULL,10);
		if(result.find(mid) != result.end()){
			string info = result[mid];
			string token = ",";
			SplitString(info, token, ader_profile);
			if(ader_profile.size() == mid_names.size())
				for(uint32_t i = 0; i < ader_profile.size(); i++)
					score += search_weights(mid_names[i], ader_profile[i]);
		}else{
			string value = "default";
			for(uint32_t i = 0 ; i < mid_names.size(); i++)
				score += search_weights(ader_names[i], value);
		}

		sort_ad.insert(make_pair(count, score));
		LOG_ERROR("AD:%d %s SCORE:%lf", count,ft->adid.c_str(), score);
		count++;
	}
	//3. 释放内存
FREE_MODULE:
	for(uint16_t i=0; i < key_size; i++)
		delete[] keystr[i];
	if(keystr!=NULL)
		delete[] keystr;
	//4. 按照num 返回订单
	if (sort_ad.size() >= 2){
		vector<pair<int, float> > tmp(sort_ad.begin(), sort_ad.end());
		sort(tmp.begin(), tmp.end(), dict_compare);
		output_vec.push_back(input_vec[tmp[0].first]);
		LOG_ERROR("BEST:%lf", tmp[0].second);
	}
	//LOG_ERROR("END industry_ctr_match");
	return 1;	
}
//返回订单, 针对非粉以及高级定向包
int FansTargetAlgorithmInterface::slow_order(VEC_CAND& input_vec, VEC_CAND& output){
	//1.  如果只有一个订单
	if(input_vec.size() == 1){
		output.push_back(*input_vec.begin());
		return 1;
	}

	double* probability = new double[input_vec.size()];
	time_t now;
	time(&now);
	int iter_count = 0;
	for(VEC_CAND::iterator it = input_vec.begin();
			it != input_vec.end(); ++it){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		double theory_impression = theory_impression_cal(ft, now);
		probability[iter_count++] =  theory_impression / (ft->finish_amount+1.0);
	}

	//轮盘赌，抽样
	for(uint32_t i = 1;i < input_vec.size(); i++){
		probability[i] += probability[i-1];
	}
	//这里有bug,存在越界问题
	double gamble = get_random() * probability[input_vec.size()-1];
	uint32_t sample = 0;
	for(;sample < input_vec.size(); sample ++){
		if(gamble <= (probability[sample]+0.000001)) 
			break;
	}
	if(sample >= input_vec.size()){
		sample = input_vec.size() - 1;
		LOG_ERROR("should not  happen");
	}
	output.push_back(input_vec[sample]);
	delete []probability;
	return 1;
}

int FansTargetAlgorithmInterface::supply_order(uint16_t num,uint32_t flag,VEC_CAND& input_vec,VEC_CAND& output_vec){
	//根据num数目，补足返回订单数目
	if(num == output_vec.size())
		return 1;
	//Fans_Tar_Ad* best = (Fans_Tar_Ad*)(*output_vec.begin());
	//adids.push_back(best->adid);
	vector<string> adids; // 返回的广告 adid集合
	for(VEC_CAND::const_iterator it = output_vec.begin(); it != output_vec.end(); ++it)
	{
		Fans_Tar_Ad* it_temp = (Fans_Tar_Ad*)(*it);
		adids.push_back(it_temp->adid);
	}
	for(VEC_CAND::const_iterator it = input_vec.begin();
			it != input_vec.end(); ++it){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		//去重
		if(find(adids.begin(),adids.end(),ft->adid) == adids.end()){
		/*
			if(flag > 1 && ft->field_id >"1"){
				output_vec.push_back(*it);
				adids.push_back(ft->adid);
			}
			if(flag <=1){
				output_vec.push_back(*it);
				adids.push_back(ft->adid);
			}
		*/
			if ((flag <= 1) || (flag > 1 && ft->field_id > "1")){//小于等于一个高级定向包或者领域id为高级定向包
				output_vec.push_back(*it);
				adids.push_back(ft->adid);
			}
		}
		if(num == output_vec.size())//补全就退出
			return 1;
	}
	return 1;
}
//返回算法层日志 num;return_type;type,field_id
int FansTargetAlgorithmInterface::algorithm_log(VEC_CAND& output_vec,uint16_t return_type,uint16_t num,uint16_t startegy){
	uint16_t position = 0;
	for(VEC_CAND::const_iterator it = output_vec.begin();
			it != output_vec.end(); ++it){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		if(ft == NULL)
			LOG_ERROR("algorithm_log NULL POINTER ERROR");
		char str[512];
		position++;
		//if(return_type == 7){
			sprintf(str,"%u;%u;%u;%s;%u;%u;%s;%u;%u;%d",num,return_type,ft->type,ft->field_id.c_str(), position,startegy, ft->alg_log.c_str(), ft->finish_amount, ft->source, 38457);
		//}
		//else{
		//	sprintf(str,"%u;%u;%u;%s;%u;%u;%u;%u",num,return_type,ft->type,ft->field_id.c_str(), position,startegy, ft->finish_amount, ft->source);
		//}		
		ft->alg_log = string(str);
		LOG_ERROR("alg_log:%s", ft->alg_log.c_str());
		ft->algBucket_id = return_type;
	}
	return 1;
}

int FansTargetAlgorithmInterface::so_consume_time(struct timeval& start){
	struct timeval end;
	gettimeofday(&end,NULL);
	struct timeval delta;
	uint64_t diff = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec - start.tv_usec;
	delta.tv_sec = diff/1000000;
	delta.tv_usec = diff%1000000;
	LOG_ERROR("FansTargetAlgorithmInterface consume time %d seconds %d useconds",delta.tv_sec,delta.tv_usec);
	return 1;
}

int FansTargetAlgorithmInterface::log_print(ACCESS_INFO* ai, VEC_CAND& output_vec, uint16_t return_type){
	uint16_t position = 0;
	for(VEC_CAND::const_iterator it = output_vec.begin();
			it != output_vec.end(); ++it){
		Fans_Tar_Ad * result = (Fans_Tar_Ad*)(*it);
		if(result == NULL)
			LOG_ERROR("log_print NULL POINTER ERROR");
		position++;
		//high_quality_flowAnalysis(result);
		LOG_ERROR("%"PRIu16" UID:%"PRIu64" counts:%"PRIu32" order_uid:%"PRIu64" adid:%s mid:%s position:%"PRIu16" return %u field:%s %d",
				return_type,ai->come_uid, ai->impression_counts,result->order_uid, result->adid.c_str(),
				result->mid.c_str(),position,output_vec.size(),result->field_id.c_str(), result->source);
	}
	return 1;
}


int FansTargetAlgorithmInterface::relation_replace(VEC_CAND& input_vec,VEC_CAND& output_vec,ACCESS_INFO* ai){
	if (input_vec.size() == output_vec.size())
		return 1;
	for(VEC_CAND::const_iterator it = output_vec.begin();it != output_vec.end(); ++it){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		if(ft->field_id <= "1")//非粉过滤，排除非粉和万能包
			continue;
		for(VEC_CAND::const_iterator iner = input_vec.begin();iner != input_vec.end(); ++iner){
			Fans_Tar_Ad * feifen = (Fans_Tar_Ad*)(*iner);
			if(feifen->adid == ft->adid && feifen->field_id=="1"){
				ft->bridge_uid = feifen->bridge_uid;//重置万能包的桥梁
				LOG_ERROR("REPLACE adid %s user %"PRIu64" bridge %"PRIu64" field %s",
						ft->adid.c_str(),ai->come_uid,feifen->bridge_uid,ft->field_id.c_str());
			}
		}
	}
	return 1;
}


int FansTargetAlgorithmInterface::high_quality_flowAnalysis(Fans_Tar_Ad *ft){//统计不同的来源
	//利用source标记ft->type和里面的细分
	uint32_t source = 0;
	if (ft->type == 0){//非粉+高级定向包+万能包
		//string 0开头非粉 1 开头万能包 其它高级定向包
		if (ft->field_id == "0"){//非粉
			source = 0;
		}else if (ft->field_id == "1"){//万能包
			source = 1;
		}else if (ft->field_id.find("u") != string::npos){
			source = 3;
		}else{
			source = 2;
		}
	}else if (ft->type == 1){//补投
		source = 10;
	}else{//精准流量
		char tmp[1024];
		strcpy(tmp, ft->alg_log.c_str());
		char *p = tmp;
		char *p2 = NULL;
		int index = 0;
		char *briuid = NULL;

		while((p2 = strchr(p, '|')) != NULL){
			index ++;
			if(index == 3){
				*p2 = 0;
				briuid = p;
			}
			if(index == 6){
				*p2 = 0;
				//LOG_ERROR("103: %s\n", p);
				if(strcmp(p, "103") == 0){//亲密度优化的非粉
					//LOG_ERROR("true 103:%s, %lld\n", briuid, strtoull(briuid, NULL, 10));
					ft->type = 0;
					ft->ad_type = 4;
					ft->bridge_uid = strtoull(briuid, NULL, 10);
					source = 2103;
				}else if(strcmp(p, "102") == 0){//优化的定向包
					source = 2102;
				}else{
					source = 2100;//精准定向
				}
				break;
			}
			p = p2 + 1;
		}
	}
	ft->source = source;
	return 1;
}

double FansTargetAlgorithmInterface::theory_impression_cal(Fans_Tar_Ad * ft, time_t now){
	double weights = 0.0;
	if (ft->begin_time != now){
		int begin_hour = (ft->begin_time % 86400) / 3600;
		double begin_minute = (ft->begin_time % 86400) % 3600 / 3600.0f;
		
		int now_hour = (now % 86400) / 3600;
		double now_minute = (now % 86400) % 3600 / 3600.0f;

		//hour
		int delivery_hour = (((now - (now % 86400) % 3600) - (ft->begin_time - (ft->begin_time % 86400) % 3600)) % 86401) / 3600;
		//limit order time
		if (((now - (now % 86400) % 3600) - (ft->begin_time - (ft->begin_time % 86400) % 3600)) > 86400) delivery_hour = 24;
		int m = 0;
		// judge the delivery_hour
		if ((begin_hour + delivery_hour) >= 24){
			for (int i = begin_hour; i < 24; i++){
				m ++;
				weights += TIME_WEIGHT[i];
			}
			if (m < delivery_hour){
				int left = delivery_hour - m;
				for (int i = 0; i < left; i++){
					weights += TIME_WEIGHT[i];
				}
			}
		}else{
			for (int i = 0; i < delivery_hour; i ++){
				int index = i + begin_hour;
				weights += TIME_WEIGHT[index];
			}
		}

		//minute
		weights = weights + TIME_WEIGHT[now_hour] * now_minute - TIME_WEIGHT[begin_hour] * begin_minute;
		
		
	}

	double theory_impression = (weights / 24) * (ft->buy_amount);
	return theory_impression;
}

