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
		if(flowControl(ft->order_uid, ft->begin_time, ft->finish_amount, theory_impression, 2, ft->source)){
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
			industry_ctr_estimate(ai,industry_ctr, output_vec, num);
			algorithm_log(output_vec, 13, num, 1);
			log_print(ai, output_vec, 13);
			so_consume_time(start_time);
			return 1;
		}else{	//对比陶总的ctr
			ctr_match_multi(ai,industry_ctr, output_vec, num);//需要读一下
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
int FansTargetAlgorithmInterface::parse_model_parameters(model_conf &mcf,int db_num,int key){
	//get lushan  model配置
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	//申请变量
	vector<string> tmp_vec;
	string token =";",info="";
	int idx = 0,debug =1,update_flg = 0;
	int redis_flag = 0,model_keysize = 1;
	map<uint64_t,const char*> lsresult;
	string conf_str;
	//参数初始化
	mcf.keys_vec.clear();
	mcf.mapkeys.clear();
	mcf.arraykey = -1;
	char** lskeys = new char*;
	lskeys[0] = new char[128]();
	sprintf(lskeys[0],"%d-%d",db_num,key);
	//obtain lushan data
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
	}
	else redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, lskeys, model_keysize, lsresult);
	if (redis_flag !=1){
		LOG_ERROR("lushan obtain data error!");
		goto FREE_MODEL;
	}
	if (lsresult.find(key) != lsresult.end() && lsresult[key] != NULL && strlen(lsresult[key])>2){
		conf_str = lsresult[key];
	}
	else {
		LOG_ERROR("lushan obtained data check is error!");
		goto FREE_MODEL;
	}
	if (debug ==1) LOG_ERROR("lushan model obtained data: db-key:%d-%d,val:%s",db_num,key,conf_str.c_str());
	info = conf_str.substr(1,conf_str.length()-2);
	SplitString(info,token,tmp_vec);
	for(size_t i=0;i< tmp_vec.size();i++){
		idx = tmp_vec[i].find(":");
		LOG_ERROR("lushan model obtain 1 parameters %s,%s",tmp_vec[i].substr(0,idx).c_str(),tmp_vec[i].substr(idx+1).c_str());
		//check 分割后 k-v 都不为空 
		if ( idx == -1 || idx ==0 || idx+2>tmp_vec[i].length()){
			continue;
			goto FREE_MODEL;
		}
		LOG_ERROR("lushan model obtain 2 parameters %s,%s",tmp_vec[i].substr(0,idx).c_str(),tmp_vec[i].substr(idx+1).c_str());
		if (strcmp(tmp_vec[i].substr(0,idx).c_str(),"db_key") == 0){
			token = ",";
			info = tmp_vec[i].substr(idx+2,tmp_vec[i].length()-idx-3);
			SplitString(info,token,mcf.keys_vec);
			
			
		}
		else if (strcmp(tmp_vec[i].substr(0,idx).c_str(),"mapkey") == 0){
			token = ",";
			info = tmp_vec[i].substr(idx+2,tmp_vec[i].length()-idx-3);
			SplitString(info,token,mcf.mapkeys);
		}
		else if (strcmp(tmp_vec[i].substr(0,idx).c_str(),"arraykey") == 0){
			//该字段可以不用，检查是否为空
			mcf.arraykey = atoi(tmp_vec[i].substr(idx+1,tmp_vec[i].length()-idx-1).c_str());
		}
		else if (strcmp(tmp_vec[i].substr(0,idx).c_str(),"indexkey") == 0){
			//该字段可以不用，检查是否为空
			mcf.indexkey = atoi(tmp_vec[i].substr(idx+1,tmp_vec[i].length()-idx-1).c_str());
		}
		else if (strcmp(tmp_vec[i].substr(0,idx).c_str(),"update") == 0){
			string flg = tmp_vec[i].substr(idx+1,tmp_vec[i].length()-idx-1);
			if ( strcmp(mcf.update_f.c_str(),flg.c_str()) !=0) {
			//	mcf.update_f = flg;
				update_flg = 1;
			}
			else{
				goto FREE_MODEL;
			}
		}
		//新增字段 自己调用使用
		else {
			LOG_ERROR("lushan model obtain 3 parameters %s,%s",tmp_vec[i].substr(0,idx).c_str(),tmp_vec[i].substr(idx+1).c_str());
			mcf.parameters.insert(make_pair(tmp_vec[i].substr(0,idx),atoi(tmp_vec[i].substr(idx+1).c_str()))); 
		}
		
	}
	//字段检查
	if(mcf.keys_vec.size()<1 || mcf.mapkeys.size()<1){
		update_flg = 0;
		LOG_ERROR("lushan model obtained error data: lushan keys num:%d mapkeys num :%d",mcf.keys_vec.size(),mcf.mapkeys.size());
	}
FREE_MODEL:
	if (lskeys != NULL) delete lskeys[0];
	if (lskeys != NULL ) delete lskeys;
	LOG_ERROR("lushan model obtain finish");
	return update_flg;
	
}

void FansTargetAlgorithmInterface::_RunThread(){
	vector<pair<int,int> > model_vec;
	vector<pair<int,int> >::iterator iter;
	//first is lushan db number; second is model conf
	model_vec.push_back(make_pair(21,14));
	model_vec.push_back(make_pair(21,134));
	if(model_vec.size() >0 ){
		model_conf* mcf;
		mcf = new model_conf[model_vec.size()];
		// 初始化配置参数
		for(iter = model_vec.begin(); iter < model_vec.end(); iter++){
			mcf[iter - model_vec.begin()].update_f = "-1"; 
		}
		while(1){
			//"{db_key:[21-131,21-132,21-133];mapkey:[133-0,131-1];arraykey:132,indexkey:133,update:60}";
			for(iter = model_vec.begin(); iter < model_vec.end(); iter++){
				int flg = parse_model_parameters(mcf[iter - model_vec.begin()],iter->first,iter->second);
				if(flg == 1){
					LOG_ERROR("_RunThread() update model dbnum:%d,conf_key:%d Update",iter->first,iter->second);
					this->update_model(mcf[iter - model_vec.begin()],&model_copy_ptr,&model_ptr[iter - model_vec.begin()],iter->first,iter->second);
				}
				else{
					LOG_ERROR("_RunThread() update model dbnum:%d,conf_key:%d  Not Need Update ",iter->first,iter->second);
				}

			}	
			sleep(60);//设置间隔时间 测试设置1分钟
		}
		delete[] mcf;
		if (mcf != NULL) mcf = NULL;
	}
}

void FansTargetAlgorithmInterface::update_model(model_conf &mcf,model_data** copy_ptr, model_data** ptr,int lsdb_num,int conf_key){
	time_t now;
	time(&now);
	LOG_ERROR("MODEL UPDATE BEGIN PTHREAD:%d,dbnum:%d,conf_key:%d",tid_fans_economy,lsdb_num,conf_key);
	//申请模型内存  申请新模型空间struct
	model_data* m_ptr =  new model_data();
	if(mcf.parameters.size() != 0) m_ptr->mapNum = mcf.mapkeys.size()+1;
	else m_ptr->mapNum = mcf.mapkeys.size();
	LOG_ERROR("mapNum:%d",m_ptr->mapNum);
	char** m_weights = NULL;
	m_ptr->weightsNum = 0;
	Map** map_i = new Map*[m_ptr->mapNum]();
	for( uint16_t i=0;i<m_ptr->mapNum;i++){
		map_i[i] = new Map();
	}
	//准备获取lushan数据进行解析
	map<uint64_t, const char*> result;
	vector<string> tmp_vector;
	string token = ";";
	uint16_t model_keysize = 2;
	if (mcf.keys_vec.size() != 0) model_keysize = mcf.keys_vec.size();
	char** keystr = new char*[model_keysize];
	for (uint16_t i = 0;i<model_keysize;++i){
		keystr[i] = new char[256]();
		sprintf(keystr[i],"%s",mcf.keys_vec[i].c_str());//model key
	}
	//设置模型更新数据量的标记，以检查模型是否更新正确
	int feature_newdata_num = 0 ,weight_newdata_num = 0,debug = 1;
	//connect lushan
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	int redis_flag = 0;
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
	}
	else redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, model_keysize, result);
	if(debug == 1) LOG_ERROR("MODEL UPDATE LUSHAN DATA EXPECT :%u GET: %u", model_keysize, result.size());
	//解析数组型模型数据
	if( mcf.arraykey !=-1 && result.find(mcf.arraykey) != result.end()){
		tmp_vector.clear();
		string info = result[mcf.arraykey];
		SplitString(info, token,tmp_vector);
		//find max array number
		int max_num = -1;
		for(size_t i = 0;i<tmp_vector.size();i++){
			int idx = tmp_vector[i].find(':');
			int key = atoi(tmp_vector[i].substr(0,idx).c_str());
			if (key > max_num) max_num = key;
		}
		m_ptr->weightsNum = max_num+1;
		m_weights = new char*[m_ptr->weightsNum];
		for (int i = 0; i < m_ptr->weightsNum; i++){
			m_weights[i] = new char[256]();
		}
		//if(debug == 1) LOG_ERROR("MODEL UPDATE ARRAY DATA LUSHAN key:%d   value: %s",mcf.arraykey, info.c_str());
		if(debug == 1) LOG_ERROR("MODEL UPDATE ARRAY DATA LUSHAN key:%d item num:%d ",mcf.arraykey,tmp_vector.size());
		for(uint32_t i=0;i<tmp_vector.size();++i){
			int idx = tmp_vector[i].find(':');
			int key = atoi(tmp_vector[i].substr(0,idx).c_str());
			//考虑异常安全性，对写入key范围进行检查
			if( key < 0 || key >= m_ptr->weightsNum )
				LOG_ERROR("MODEL UPDATE ARRAR DATA ITERM ERROR key:%d value:%f", key,atof(tmp_vector[i].substr(idx+1).c_str()));
			else{
				sprintf(m_weights[key],"%f",atof(tmp_vector[i].substr(idx+1).c_str()));
				weight_newdata_num++;
			}
		}
	}
	//lushan数据错误，跳出
	if(redis_flag != 1){
		LOG_ERROR("redis_flag_error");
		goto FREE_MODULE;
	}
	//解析MAP型模型数据 特征离散数据
	for(int mk=0;mk < m_ptr->mapNum;mk++){
		if (debug ==1) LOG_ERROR("MODEL UPDATE MAP DATA Iter:%d,%d,%d,%d",mk,mcf.parameters.size(),mcf.mapkeys.size(),m_ptr->mapNum);
		if ( mcf.parameters.size() != 0 && mk == m_ptr->mapNum-1 ){
			Map::iterator iter;
			for(iter = mcf.parameters.begin(); iter != mcf.parameters.end();iter++){
				if (debug ==1) LOG_ERROR("MODEL UPDATE MAP DATA  parameters %s,%d",iter->first.c_str(),iter->second);
				(map_i[mk])->insert(make_pair(iter->first,iter->second));
			}
			break;
		}
		tmp_vector.clear();
		int idx = mcf.mapkeys[mk].find(":");
		int qkey = atoi(mcf.mapkeys[mk].substr(0,idx).c_str());
		int m_pos = atoi(mcf.mapkeys[mk].substr(idx+1).c_str());
		LOG_ERROR("MAP %d,%d %s",qkey,m_pos,mcf.mapkeys[mk].c_str());
		if(result.find(qkey) != result.end()){
			string info = result[qkey];
			SplitString(info, token,tmp_vector);
			if (debug ==1) LOG_ERROR("MODEL UPDATE MAP LUSHAN key:%d GET: %s", qkey,info.c_str());
		}
		if (debug ==1) LOG_ERROR("MODEL UPDATE MAP DATA key:%d item num:%d ",qkey, tmp_vector.size());
		for(uint32_t i=0;i<tmp_vector.size();++i){
			idx = tmp_vector[i].find(':');
			if (idx != -1 && idx >2 && idx+1 <= tmp_vector[i].size()){
				//if (debug ==1) LOG_ERROR("MODEL UPDATE MAP DATA ITERM CHECK: key:%s, value:%s",tmp_vector[i].substr(0,idx).c_str(),tmp_vector[i].substr(idx+1).c_str());
				(map_i[m_pos])->insert(make_pair(tmp_vector[i].substr(0,idx).c_str(),atoi(tmp_vector[i].substr(idx+1).c_str())));
				feature_newdata_num++;
			}
		}
		if(mcf.indexkey == qkey) m_ptr->indexkey = m_pos;
		if (debug ==1) LOG_ERROR("MODEL UPDATE MAP DATA key:%d map_pos:%d insert num:%d ",qkey,m_pos,feature_newdata_num);
	}
	//检查模型新数据量是正确,错误则goto free model
	if(feature_newdata_num < 100 || weight_newdata_num < 100 ){
		LOG_ERROR("MODEL UPDATE ERRO FREE MODEL PTHREAD:%d,dbnum:%d,conf_key:%d",tid_fans_economy,lsdb_num,conf_key);
		redis_flag =0;
		goto FREE_MODULE;
	}
	m_ptr->mapArray = map_i;
	m_ptr->weights = m_weights;
	//加锁 更新模型
	pthread_mutex_lock(&p_lock_fans_economy);
	LOG_ERROR("addlock update:%d",tid_fans_economy);
	*copy_ptr = *ptr;
	*ptr = m_ptr;
	//解锁
	pthread_mutex_unlock(&p_lock_fans_economy);
	LOG_ERROR("releaselock update:%d",tid_fans_economy);
	sleep(3);
	//释放原模型内存
	if((*copy_ptr)->weights != NULL){
		for(int i=0;i<(*copy_ptr)->weightsNum;++i){
			if ((*copy_ptr)->weights[i] != NULL)
				delete[] (*copy_ptr)->weights[i];
		}
		if ((*copy_ptr)->weights != NULL)
			delete (*copy_ptr)->weights;
	}
	if((*copy_ptr)->mapArray != NULL){
		for(uint16_t i=0;i<(*copy_ptr)->mapNum;i++){
			if ((*copy_ptr)->mapArray[i] != NULL)
			delete (*copy_ptr)->mapArray[i];
		}
		if ((*copy_ptr)->mapArray != NULL)
			delete (*copy_ptr)->mapArray;
	}
	//new 出来的要delete掉
	delete *copy_ptr;
	if(debug == 1) LOG_ERROR("MODEL UPDATE FINISHED UPDATE PTHREAD:%d,dbnum:%d,conf_key:%d",tid_fans_economy,lsdb_num,conf_key);
	// free memery
FREE_MODULE:
	//释放临时空间
	for (uint32_t i = 0; i<model_keysize; ++i){
		if (keystr[i] != NULL)
		delete[] keystr[i];
	}
	if (keystr != NULL) delete keystr;
	//模型内存未使用，释放
	if(redis_flag != 1){
		if(debug == 1) LOG_ERROR("MODEL UPDATE ERRO FREE MODEL PTHREAD:%d,dbnum:%d,conf_key:%d",tid_fans_economy,lsdb_num,conf_key);
		if(m_weights != NULL){
			for(int i=0;i<m_ptr->weightsNum;++i){
				if (m_weights[i]!=NULL)
				delete[] m_weights[i];
			}
			if(m_weights != NULL) delete m_weights;
		}
		if(map_i != NULL){
			for(int i=0;i<m_ptr->mapNum;++i){
				if (map_i[i] != NULL)
				delete map_i[i];
			}
			if(map_i != NULL) delete map_i;
		}
		delete m_ptr;
	}
	if(debug == 1) LOG_ERROR("MODEL UPDATE TASK FINISHED  PTHREAD:%d,dbnum:%d,conf_key:%d",tid_fans_economy,lsdb_num,conf_key);
}

int FansTargetAlgorithmInterface::search_feature_index(string& key, model_data* read_ptr){
	int index = -1,debug = 0;
	if(read_ptr->mapArray[read_ptr->indexkey]->find(key.c_str()) != read_ptr->mapArray[0]->end()){
		index = (*(read_ptr->mapArray[0]))[key.c_str()];
		if (1 == debug) LOG_ERROR("MODEL UPDATE CHECK MAP COMPARE :key_s-%s,value:%d",key.c_str(),(*(read_ptr->mapArray[0]))[key.c_str()]);
	}
	return index;
}


float FansTargetAlgorithmInterface::search_weights(string feature_name, string value,model_data *read_ptr,int value_type){
	//value_type 0：float，1：int
	LOG_ERROR("MODEL UPDATE CHECK: search_weights");
	if (read_ptr == NULL || read_ptr->weights == NULL ){
		LOG_ERROR("MODEL UPDATE CHECK: read_ptr is NULL");
		return 0.0;
	}
	int val_length = value.length();
	float query_value = atof(value.c_str());
	char val[128];
	if (value_type) {
		sprintf(val,"%d",int(query_value));
	}
	else{
		sprintf(val,"%s",value.c_str());
	}
	string key = feature_name + "_"+ val;
	int k = search_feature_index(key,read_ptr);
	int back_i = 1;
	while(-1 == k && back_i < 4 ){
		query_value = query_value *(1- back_i *back_i / 50.0);
		if (value_type ==1 ) {
			sprintf(val,"%d",int(query_value));
		}
		else{
			sprintf(val,"%s",value.c_str());
			if(val_length - back_i >0)
			val[val_length - back_i] = 0;
		}
		key = feature_name+"_" + val;
		k = search_feature_index(key,read_ptr);
		++back_i;
	}
	if(-1 != k) {
		LOG_ERROR("MODEL UPDATE CHECK:weights-%f",atof(read_ptr->weights[k]));
		return atof(read_ptr->weights[k]);
	}
	return -10000;
}

int FansTargetAlgorithmInterface::search_industry_score(Fans_Tar_Ad* ft,string& score){
	size_t alg_log_num =5;
	vector<string> tmp;
	int result = 0;
	char stmp[128];
	string token = "|";
	SplitString(ft->alg_log,token,tmp);
	if (tmp.size()< alg_log_num || tmp[1].length() < 1  ){
		score = "";
		return 0;
	}
	score = tmp[1];
	int val = int(atof(score.c_str()));
	if (val > 150) result = industry_score_threshold(ft,150);
	else if (val >= 0) result = industry_score_threshold(ft,val);
	else {
		score = "";
		return 0;
	}
	sprintf(stmp,"%d",result);
	score =  stmp;
	LOG_ERROR("INDUSTRY_CTR_ESTIMATE SCORE:%s alg_log:%s", score.c_str(), ft->alg_log.c_str());
	return 1;
}
int FansTargetAlgorithmInterface::industry_score_threshold(Fans_Tar_Ad* ft,int base_score){
	time_t now;
	time(&now);
	int delay_h = 2;
	if (base_score<0 || base_score >1000000) return 70;
	double theory_impression = theory_impression_cal(ft,now);
	int hour = ((now - ft->begin_time ) % 86401) / 3600;
	double hard_ratio = (ft->finish_amount+1.0)/(theory_impression+1.0);
	double time_ratio = 24.0/(24+hour-delay_h);//延时2小时生效
	if (hour < delay_h) time_ratio = 1;
	if (hour < delay_h) hard_ratio = 1.0; //延时2小时生效
	else if (hard_ratio<0.5) hard_ratio = 0.5;
	//投放困难系数:hard_ratio取值范围：1--0.5 
	//时间衰减系数：24/(24+h)取值范围投放24小时:1-0.5,48小时:1-0.33,72小时投放:1-0.25 6小时后0.857
	base_score = int(time_ratio*hard_ratio*base_score);
	return base_score;
}

int FansTargetAlgorithmInterface::industry_ctr_estimate(ACCESS_INFO* ai,VEC_CAND& input_vec,VEC_CAND& output_vec, int num){
	//1. 取lushan数据
	LOG_ERROR("In industry_ctr_estimate");
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
		return -1;
	}
	uint16_t key_size = 1;
	char** keystr = new char*[key_size];
	for(uint16_t i = 0; i < key_size; i++)
		keystr[i] = new char[256]();
	int index = 0,debug = 0;
	//2. get ader
	//get field_score_defualt ctr
	sprintf(keystr[index++],"%u-%"PRIu64"",21,131);
	map<uint64_t, const char*> result;
	int count = 0;
	float extend_ratio = 100000.0;
	map<int, float> sort_ad;
	string token = ",";
	vector<string> user_profile;
	float score = 0.0;
	string industry_score = "";
	vector<string> tmp_vec;
	map<string, float> default_ctr;
	float default_ctr_avg = 0;
	//3.ctr模型
	pthread_mutex_lock(&p_lock_fans_economy);
	LOG_ERROR("addlock search:%d",tid_fans_economy);
	model_read_ptr = model_ptr[1];
	LOG_ERROR("releaselock search:%d",tid_fans_economy);
	//LOG_ERROR("LOAD MODEL BEGIN LOCK1 SLEEP PTHREAD:%d",tid_fans_economy);
	pthread_mutex_unlock(&p_lock_fans_economy);
	
	for(Map::const_iterator iter = (model_read_ptr->mapArray[1])->begin();iter != (model_read_ptr->mapArray[1])->end(); iter++){
	//default ctr   //0-1:feifen_ctr orientation_ctr 2-4:gender0-2 ctr 5-8:platform_ctr 0-3
		if(iter->first.find("200") != 0) continue;
		float ctr_tmp = iter->second/extend_ratio;
		if(ctr_tmp <0 || ctr_tmp>1) continue;
		default_ctr.insert(make_pair(iter->first,ctr_tmp));
		default_ctr_avg +=ctr_tmp;
		if(debug == 1) LOG_ERROR("INDUSTRY_CTR_ESTIMATE DEFAULT CTR:%s,%f", iter->first.c_str(),ctr_tmp);
	}
	if(debug == 1) LOG_ERROR("INDUSTRY_CTR_ESTIMATE DEFAULT DEFAULT  CTR:%f,%d", default_ctr_avg,default_ctr.size());
	if (default_ctr.size() != 0) default_ctr_avg = default_ctr_avg/default_ctr.size();
	else default_ctr_avg = 0.0152;
	if(debug == 1) LOG_ERROR("INDUSTRY_CTR_ESTIMATE DEFAULT DEFAULT  CTR:%f", default_ctr_avg);

	// 直接比拼ader的  ctr
	for(VEC_CAND::const_iterator it = input_vec.begin();
		it!=input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		search_industry_score(ft,industry_score);
		if (ft->field_id.find("200") == 0){
			if ( industry_score == ""){
				if (default_ctr.find(ft->field_id)!= default_ctr.end())	{
					score = default_ctr[ft->field_id];
				}
				else{
					score = default_ctr_avg;
				}
			}
			else{
				score = search_weights(ft->field_id,industry_score,model_read_ptr,1);
				if (int(score) == -10000) {
					if (default_ctr.find(ft->field_id)!= default_ctr.end())
						score = default_ctr[ft->field_id];
					else	score = default_ctr_avg;
				}
				else	score = score/extend_ratio;
			}
		}
		else{
			//行业包 为空 则使用默认值
			score = default_ctr_avg;
		}
		sort_ad.insert(make_pair(count, score));
		LOG_ERROR("AD:%d %s SCORE:%lf", count,ft->adid.c_str(), score);
		count++;
	}

	//释放内存
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

//设置默认索引的 feature_name //第一位表示是否是单一特征值，第二位表示特征名位置，第三位表示该特征的特征值，第四位表示在特征值对的在数据中的位置，第五位默认备用数据
//
float FansTargetAlgorithmInterface::feature_cross_weight(int multi_flg,int fname_idx,string feature_val,int idx,vector<string> &default_info,vector<string> &target_info,int default_flg,model_data *read_ptr){
	float rs = 0.0;
	int erro_flg = -10000;
	float default_w = -0.151707; 
	if ( multi_flg==1 && feature_val == ""){
		//需要交叉的，但feature_val为空时，默认值代替
		rs =  search_weights(feature_name_default[fname_idx],default_info[idx],read_ptr,0);
		if (int(rs) == erro_flg) rs = default_w;
		return rs;
	}
	string query_name = feature_name_list[fname_idx]+feature_val;
	if (default_flg == 1 || strcmp(target_info[idx].c_str(),"0.0") == 0){
		rs =  search_weights(query_name,default_info[idx],read_ptr,0);
		if (int(rs) == erro_flg) rs = default_w;
	}
	else{
		rs =  search_weights(query_name,target_info[idx],read_ptr,0);
		if (int(rs) == erro_flg){
			rs =  search_weights(query_name,default_info[idx],read_ptr,0);
			if (int(rs) == erro_flg) rs = default_w;
		}
	}
	return rs;
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
		keystr[i] = new char[256]();
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

	for(map<uint64_t, const char*>::const_iterator iter = result.begin();iter != result.end(); iter++){
		LOG_ERROR("CTR_MATCH_MULTI LUSHAN %lld:%s", iter->first, iter->second);
	}
	

	//0 . user profile
	if(result.find(ai->come_uid) != result.end()){
		string info = result[ai->come_uid];
	//	string sel_info = info.substr(1,info.size()-2);
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
		LOG_ERROR("CTR_MATCH_MULTI USER INFO DEFAULT CTR:%s", sel_info.c_str());
		SplitString(sel_info, token, default_ctr);
	}

	LOG_ERROR("addlock search:%d",tid_fans_economy);
	pthread_mutex_lock(&p_lock_fans_economy);
	model_read_ptr = model_ptr[0];
	pthread_mutex_unlock(&p_lock_fans_economy);
	LOG_ERROR("releaselock search:%d",tid_fans_economy);

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
				//industry feature ctr
				//第一位表示是否是单一特征值，第二位表示特征名位置，第三位表示该特征的特征值，第四位表示在特征值对的在数据中的位置，第五位默认备用数据
				score += feature_cross_weight(0,1,"",1,default_ctr,ader_ctr,0,model_read_ptr);
				//gender feature ctr  user_profile[0]
				score += feature_cross_weight(1,2,user_gender,2+atoi(user_gender.c_str()),default_ctr,ader_ctr,0,model_read_ptr);
				//plat feature ctr
				score += feature_cross_weight(1,3,plat,5+atoi(plat.c_str()),default_ctr,ader_ctr,0,model_read_ptr);
			}
			else{
				//广告主ctr列数有问题,根据访问用户信息进行默认值的选择
				//0-1:feifen_ctr orientation_ctr 2-4:gender0-2 ctr 5-8:platform_ctr 0-3
				if(default_ctr.size() ==9 ){
					//industry feature ctr   
					score += feature_cross_weight(0,1,"",1,default_ctr,ader_ctr,1,model_read_ptr);
					//gender feature ctr  user_profile[0]
					score += feature_cross_weight(1,2,user_gender,2+atoi(user_gender.c_str()),default_ctr,ader_ctr,1,model_read_ptr);
					//plat feature ctr
					score += feature_cross_weight(1,3,plat,5+atoi(plat.c_str()),default_ctr,ader_ctr,1,model_read_ptr);
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
				score += feature_cross_weight(0,1,"",1,default_ctr,ader_ctr,1,model_read_ptr);
				//gender feature ctr  user_profile[0]
				score += feature_cross_weight(1,2,user_gender,2+atoi(user_gender.c_str()),default_ctr,ader_ctr,1,model_read_ptr);
				//plat feature ctr
				score += feature_cross_weight(1,3,plat,5+atoi(plat.c_str()),default_ctr,ader_ctr,1,model_read_ptr);
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
		sprintf(str,"%u;%u;%u;%s;%u;%u;%s;%u;%u;%d",num,return_type,ft->type,ft->field_id.c_str(), position,startegy, ft->alg_log.c_str(), ft->finish_amount, ft->source, 38457);
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
