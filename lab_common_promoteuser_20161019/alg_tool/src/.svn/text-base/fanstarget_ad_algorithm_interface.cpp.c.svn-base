#include "fanstarget_ad_algorithm_interface.h"
#include "fanstarget_ad_algorithm_high_quality.h"

DYN_ALGORITHMS(FansTargetAlgorithmInterface);

int FansTargetAlgorithmInterface::algorithm_core(uint64_t req_id,const AccessStr& access_str, VEC_CAND& vec_cand)
{
	return 1;	
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

int FansTargetAlgorithmInterface::algorithm_core_new(uint64_t req_id, 
		const AccessInfo* access_info, const VEC_CAND& input_vec, VEC_CAND& output_vec)
{
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
	uint16_t strategy = 1;	//性别优先
	ACCESS_INFO* ai = (ACCESS_INFO*)access_info;
	uint16_t num = ai->num;

	//分组feed接入高级定向包,如果分组match到广告，则flag_group置为true
	string group_name=ai->group_name;
	string group_id=ai->group_id;
	bool flag_group=false;
	bool is_group=false;
	if(group_id.compare("") != 0)
		is_group=true;
	//yingying
	LOG_ERROR("group_name:%s, group_id:%s", group_name.c_str(), group_id.c_str());
	//1. 分包，将集合分为 0:客户包，1:不能完成包, 2:精准定向包
	VEC_CAND fen_tiao;
	VEC_CAND insuff_impression;
	VEC_CAND high_quality;
	VEC_CAND high_quality_nonfans;
	VEC_CAND group_input_vec;
	VEC_CAND::const_iterator impression; //指向  buy_amout / max_package 最大的订单,或者高级定向包
	set<string> advanced;	//高级定向包实际adid个数
	double max_occupy_ratio = 0.0;  // 记录  buy_amount / max_package 最大值
	uint64_t need_butou = 0;        //需要补投总量
	for(VEC_CAND::const_iterator it = input_vec.begin();
			it != input_vec.end(); ++it){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		if(ft->type == 0){
			//流量控制
			if(flowControl(ft->order_uid, ft->begin_time)){
				continue;
			}

			//高级定向包 0开头非粉 1 开头万能包 其它高级定向包
			if(ft->field_id>"1"){
				if(is_group)
				{
					group_input_vec.push_back(*it);
				}
				else
				{
					//分组情况下，只出有桥梁和匹配到的高级定向包
					fen_tiao.push_back(*it);
					advanced.insert(ft->adid);
					impression = it;
				}
				continue;
			}
			
			//最大包
			fen_tiao.push_back(*it);
			double ratio = double(ft->buy_amount) / double(ft->max_package + 3.0);
			if((advanced.size()==0) &&ratio > max_occupy_ratio){
				impression = it;
				max_occupy_ratio = ratio;
			}
		}
		else if(ft->type == 2){
			char tmp[1024];
			strcpy(tmp, ft->alg_log.c_str());
			char *p = tmp;
			char *p2 = NULL;
			int index = 0;
			char *briuid = NULL;
			
			int ifnonfans = 0;
			while((p2 = strchr(p, '|')) != NULL){
				index ++;
				if(index == 3){
					*p2 = 0;
					briuid = p;
				}
				if(index == 6){
					*p2 = 0;
					//LOG_ERROR("103: %s\n", p);
					if(strcmp(p, "103") == 0){
						//LOG_ERROR("true 103:%s, %lld\n", briuid, strtoull(briuid, NULL, 10));
						ft->type = 0;
						ft->ad_type = 4;
						ft->bridge_uid = strtoull(briuid, NULL, 10);
						ifnonfans = 1;
						/*
						//最大包
						fen_tiao.push_back(*it);
						double ratio = double(ft->buy_amount) / double(ft->max_package + 3.0);
						if((advanced.size()==0) &&ratio > max_occupy_ratio){
							impression = it;
							max_occupy_ratio = ratio;
						}*/
					}
				}
				p = p2 + 1;
			}

			//流量控制
			if(ifnonfans == 1 && flowControl(ft->order_uid, ft->begin_time)){
				continue;
			}
			else{
				high_quality_nonfans.push_back(*it);
				continue;
			}

			high_quality.push_back(*it);
		}
		else{
			need_butou += (ft->buy_amount - ft->finish_amount);
			if(ft->finish_amount < (ft->buy_amount * 0.8))
				insuff_impression.push_back(*it);
		}
	}
	uint32_t flag = advanced.size();	//高级定向包的个数 
	double threshold = need_butou * 5.0 / FLOW_AMOUNT;	//以前倍数是5，因为下面放大倍数，这里调小

	// 精准流量判断
	//if((advanced.size() == 0) && (high_quality.size() > 0) ){
	// 精准流量提权到定向包之前20160630
	if( (high_quality.size() > 0) ){
		output_vec.push_back(high_quality[0]);
		algorithm_log(output_vec, 7, num, 1);
		log_print(ai, output_vec, 7);
		so_consume_time(start_time);
		return 1;
	}
	if( (high_quality_nonfans.size() > 0) ){
		output_vec.push_back(high_quality_nonfans[0]);
		algorithm_log(output_vec, 7, num, 1);
		log_print(ai, output_vec, 7);
		so_consume_time(start_time);
		return 1;
	}
	

	//2. 粉条包为空，投放曝光不足
	if(fen_tiao.size() == 0 && !is_group){
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
	if(flag > 0 || max_occupy_ratio >= BUY_OCCUPY_MAX_PACKAGE || is_group){
		//add code by yingying
		//计算分组feed是否match到高级定向包的广告
		if(group_name.compare("")!=0)
			group_feed_match(num, group_name, group_input_vec, output_vec, flag_group);
		//add by yingying, 主feed时投放定向包
		if(!is_group)
		{
			output_vec.push_back(*impression);
		}

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

//使用匹配方法，选择广告,针对扩包方案
int FansTargetAlgorithmInterface::match_order(uint64_t uid, VEC_CAND& input_vec, VEC_CAND::const_iterator& matcher){
	LOG_ERROR("IN match_order");
	//1. 取lushan数据
	int amplifer = 0;
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!\n");
		return -1;
	}
	uint16_t key_size = input_vec.size()*2+1;// gender,industry + user
	char** keystr = new char*[key_size];	
	for(uint16_t i = 0; i < key_size; i++)
		keystr[i] = new char[128];
	int index = 1, dbno=0;
	if(uid%2==1)
		dbno = 3;
	else
		dbno = 2;

	sprintf(keystr[0],"%d-%"PRIu64"",dbno, uid);
	for(VEC_CAND::const_iterator it = input_vec.begin();
			it!=input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		sprintf(keystr[index++],"%u-%"PRIu64"",ADER_PROFILE_DB_NO,ft->order_uid);//uint64_t order_uid;
		sprintf(keystr[index++],"%u-%s",MID_INDUSTRY_DB_NO,ft->mid.c_str());	//string mid 
	}
	map<uint64_t, const char*> result;
	int redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, key_size, result);
	vector<string> user_profiles;
	vector<string> industry_profiles;
	if(redis_flag != 1){
		LOG_ERROR(" redis_flag error ");
		goto FREE_MODULE;
	}
	for(map<uint64_t, const char*>::iterator riter = result.begin(); riter != result.end(); riter++){
		LOG_ERROR("test! %lld %s", riter->first, riter->second);
	}
	//2. 匹配广告
	if(result.size() > 1 && result.find(uid)!= result.end()){
		string src = result[uid];
		string token = "_";
		SplitString(src,token,user_profiles);
		//	LOG_ERROR("uid info:%s %s",user_profiles[1].c_str(),user_profiles[2].c_str());//1_理财,婚庆_1_0 
		result.erase(uid);

		token = ",";
		SplitString(user_profiles[1],token,industry_profiles);
		//		for(vector<string>::const_iterator iter = industry_profiles.begin(); iter!=industry_profiles.end();iter++){
		//			LOG_ERROR("industry:%s", (*iter).c_str());
		//		}
	}
	if(user_profiles.size() < 3)
		goto FREE_MODULE;
	for(VEC_CAND::const_iterator it = input_vec.begin(); it != input_vec.end(); it++){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
		int tmp_score = 0;
		if(result.find(ft->order_uid) != result.end()){
			int ss = gender_match_score(user_profiles[2], result[ft->order_uid]);
			//		LOG_ERROR("gender score:%d", ss);
			tmp_score += ss;
		}
		uint64_t mid = strtoull(ft->mid.c_str(),NULL,10);
		if(result.find(mid) != result.end()){
			//tmp_score += industry_match_score(user_profiles[2],result[mid]);
			tmp_score += industry_match_score(industry_profiles,result[mid]);
		}
		if(tmp_score > amplifer){
			amplifer = tmp_score;
			matcher = it;
		}
	}
	//3. 释放内存
FREE_MODULE:
	for(uint16_t i=0; i < key_size; i++)
		delete[] keystr[i];
	if(keystr!=NULL)
		delete[] keystr;
	if(amplifer == 0){
		matcher = input_vec.begin();
		amplifer++;
	}
	LOG_ERROR("left matcher,amplifer: %d", amplifer);
	return amplifer;
}
//行业匹配得分,需要稍后修改匹配莹莹姐 id1_0.1_id2_0.2
int FansTargetAlgorithmInterface::industry_match_score(vector<string>& industrys, const char* info){
	LOG_ERROR("IN industry_match_score");
	string token = "_";
	string src = info;
	vector<string> user_profiles;
	SplitString(src,token, user_profiles);
	int score = 0;
	int value = 8;
	for(vector<string>::const_iterator iter = industrys.begin(); iter!=industrys.end(); iter++){
		if(find(user_profiles.begin(),user_profiles.end(),*iter) != user_profiles.end()){
			score += value;
			value -= 4;
		}
	}
	LOG_ERROR("industry %d", score);
	return score;
}

//行业匹配得分,玉清的分类匹配 id1
int FansTargetAlgorithmInterface::industry_match_score(string& uid_gender, const char* info){
	string src = info;
	int score = 0;
	if(uid_gender == "1" && find(male_like, male_like+5, src) != male_like+5)
		score = 8;
	if(uid_gender == "2" && find(female_like, female_like+5, src) != female_like+5)
		score = 8;
	LOG_ERROR("industry %d", score);
	return score;
}
// 性别匹配得分 gender1_0.00539_gender2_0.00725
int FansTargetAlgorithmInterface::gender_match_score(string& uid_gender, const char* info){
	LOG_ERROR("IN gender_match_score");
	string token = "_";
	string src = info;
	vector<string> user_profiles;
	SplitString(src,token,user_profiles);
	//1. 就一个ctr
	double ctr_threshold = 0.002;
	double ctr1 = atof(user_profiles[1].c_str());
	if(user_profiles.size()==2){
		if(user_profiles[0].find(uid_gender) != string::npos)
			if(ctr1 >= ctr_threshold)
				return 7;
			else
				return 2;
		else
			return 1;
	}
	//2. 两个ctr 都很大
	if(user_profiles.size() == 4){
		double ctr2 = atof(user_profiles[3].c_str());
		double ratio1 = ctr1 / (ctr2+0.00001);
		double ratio2 = ctr2 / (ctr1+0.00001);
		if(ctr1>=ctr_threshold && ctr2>=ctr_threshold)
			return 7;
		// 3. ctr1 大
		if(ctr1>=ctr_threshold){
			if(user_profiles[0].find(uid_gender) != string::npos)
				return 6;
			else
				if(ratio1 > 2.0)
					return 1;
				else
					return 2;
		}
		//4. ctr2 大
		if(ctr2 >= ctr_threshold){
			if(user_profiles[2].find(uid_gender) != string::npos)
				return 6;
			else
				if(ratio2 > 2.0)
					return 1;
				else
					return 2;
		}
		// 5 ctr1 ctr2 都小
		if(ratio1 >3.0 && user_profiles[0].find(uid_gender) != string::npos)
			return 3;
		if(ratio2 >3.0 && user_profiles[2].find(uid_gender) != string::npos)
			return 3;
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
	int now_hour = (now % 86400) / 3600;
	double now_little = ((now % 86400) % 3600) / 3600.0f; //12:30 ,30分钟的权重
	int iter_count = 0;	//
	for(VEC_CAND::iterator it = input_vec.begin();
			it != input_vec.end(); ++it){
		Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);

		int begin_hour = (ft->begin_time % 86400) / 3600;
		double begin_little = (3600 - ((ft->begin_time % 86400) % 3600)) / 3600.0f;    //8:20开始 ，40分钟的权重
		double weights = TIME_WEIGHT[begin_hour] * begin_little + TIME_WEIGHT[now_hour] * now_little;
		//2.1. 起始时间 8:00 , 现在 12:00  
		if(begin_hour < now_hour){
			for(int i = begin_hour + 1; i < now_hour; i++)
				weights += TIME_WEIGHT[i];
		}else{ // 2.2 起始时间 23:00， 现在时间 3:00
			if(begin_hour < 23)
				for(int i = begin_hour+1; i < 24; i++)
					weights += TIME_WEIGHT[i];

			for(int i = 0; i < now_hour; ++i)
				weights += TIME_WEIGHT[i];
		}
		// buy_amount=0 => theory_impression=0
		double theory_impression = (weights / 24) * (ft->buy_amount);
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
			if(flag > 1 && ft->field_id >"1"){
				output_vec.push_back(*it);
				adids.push_back(ft->adid);
			}
			if(flag <=1){
				output_vec.push_back(*it);
				adids.push_back(ft->adid);
			}
		}
		if(num == output_vec.size())
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
		if(return_type == 7){
			sprintf(str,"%u;%u;%u;%s;%u;%u;%s",num,return_type,ft->type,ft->field_id.c_str(), position,startegy, ft->alg_log.c_str());
		}
		else{
			sprintf(str,"%u;%u;%u;%s;%u;%u",num,return_type,ft->type,ft->field_id.c_str(), position,startegy);
		}		
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
		LOG_ERROR("%"PRIu16" UID:%"PRIu64" counts:%"PRIu32" order_uid:%"PRIu64" adid:%s mid:%s position:%"PRIu16" return %u field:%s",
				return_type,ai->come_uid, ai->impression_counts,result->order_uid, result->adid.c_str(),
				result->mid.c_str(),position,output_vec.size(),result->field_id.c_str());
	}
	return 1;
}

//永生优质流量
int FansTargetAlgorithmInterface::high_quality_user(uint64_t uid, VEC_CAND& input_vec, VEC_CAND& output_vec){
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("NONFANS_USER_PROFILE");
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!\n");
		return -1;
	}
	//LOG_ERROR("high_quality_user");
	uint16_t key_size = 2;
	//uid = 1952800693;
	char** keystr = new char*[key_size];
	for(uint16_t i = 0; i < key_size; i++){
		keystr[i] = new char[128];
	}

	sprintf(keystr[0],"%d-%"PRIu64"",22, uid);
	sprintf(keystr[1],"%d-%"PRIu64"",24, uid);
	map<uint64_t, const char*> result;
	//LOG_ERROR("getkey %s\n", keystr[0]);
	//-------to get
	int redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, key_size, result);
	map<uint64_t, const char*>::iterator riter;
	char res[4096];
	char *p = res;
	char *p2 = NULL;
	int ok = 0;

	//16031405019047179564_3951054678766992_2886425895_0.200000_3957493107113539

	int index = 0;
	for(riter = result.begin(); riter != result.end(); riter++){
		//LOG_ERROR("test1 output ;7; %u %s\n", riter->first, riter->second);
		strcpy(res, riter->second);

		//16041405019048181531_3965969032916828_3041157452_10110.0_3964477769153869;16041405019048181979_3965972577012661_1927743427_10020.0_3964485511289875

		//LOG_ERROR("mc ret:%s", res);

		p = res;
		while((p2 = strchr(p, '_')) != NULL){

			if(index > 2){break;}
			*p2 = 0;
			((Fans_Tar_Ad *)input_vec[index])->adid = string(p);

			p = p2 + 1;
			p2 = strchr(p, '_');
			if(p2 == NULL){goto BCK;} 
			*p2 = 0;
			((Fans_Tar_Ad *)input_vec[index])->mid = string(p);

			p = p2 + 1;
			p2 = strchr(p, '_');
			if(p2 == NULL){goto BCK;} 
			*p2 = 0;
			((Fans_Tar_Ad *)input_vec[index])->order_uid = atol(p);

			p = p2 + 1;
			p2 = strchr(p, '_');
			if(p2 == NULL){goto BCK;}
			*p2 = 0;
			((Fans_Tar_Ad *)input_vec[index])->alg_log = string(p);

			p = p2 + 1;
			p2 = strchr(p, ';');
			if(p2 == NULL){
				((Fans_Tar_Ad *)input_vec[index])->alg_log +=  "|" + string(p);
				output_vec.push_back(input_vec[index]);
				goto BCK;
			}
			*p2 = 0;
			((Fans_Tar_Ad *)input_vec[index])->alg_log +=  "|" + string(p);

			output_vec.push_back(input_vec[index]);
			index += 1;
			//p = p2 + 1;
			//p2 = strchr(p, ';');
			//if(p2 == NULL){goto BCK;}
			p = p2 + 1;
		}
	}

BCK:
	for(uint16_t i = 0; i < key_size; i++){
		if(keystr[i]!=NULL){
			//delete(keystr[i]);
			delete[] keystr[i];
		}
	}
	if(keystr != NULL){
		//delete(keystr);
		delete[] keystr;
	}
	return 1;
}

	int FansTargetAlgorithmInterface::relation_replace(VEC_CAND& input_vec,VEC_CAND& output_vec,ACCESS_INFO* ai){
		if (input_vec.size() == output_vec.size())
			return 1;
		for(VEC_CAND::const_iterator it = output_vec.begin();it != output_vec.end(); ++it){
			Fans_Tar_Ad * ft = (Fans_Tar_Ad*)(*it);
			if(ft->field_id <= "1")//非粉过滤
				continue;
			for(VEC_CAND::const_iterator iner = input_vec.begin();iner != input_vec.end(); ++iner){
				Fans_Tar_Ad * feifen = (Fans_Tar_Ad*)(*iner);
				if(feifen->adid == ft->adid && feifen->field_id=="1"){
					ft->bridge_uid = feifen->bridge_uid;
					LOG_ERROR("REPLACE adid %s user %"PRIu64" bridge %"PRIu64" field %s",
							ft->adid.c_str(),ai->come_uid,feifen->bridge_uid,ft->field_id.c_str());
				}
			}
		}
		return 1;
	}
//add code by yingying
unsigned int hash_code(string group_name)
{
	unsigned int seed=131;
	unsigned int hash=0;
	char * str = const_cast<char *>(group_name.c_str());
	while(*str)
	{
		hash=hash*seed+(*str++);
	}
	return hash & 0x7FFFFFFF;
}


//add code by yingying
int FansTargetAlgorithmInterface::group_feed_match(uint16_t num, string &group_name, const VEC_CAND& input_vec, VEC_CAND& output_vec, bool &flag_group)
{
	//从lushan上面取group_name对应的垂直定向包id
	//如果对应的垂直定向包id和广告的industry_id是一样的，则放入output_vec，flag_group置为true
	//否则，直接返回
	//

	LOG_ERROR("enter in group_feed_match!");
	DbInterface* p_groupname_industry_interface = p_db_company_->get_db_interface("GROUP_MATCH");
	if(p_groupname_industry_interface == NULL)
	{
		LOG_ERROR("connect lushan user_nofans_groupname_industry_match failed!");
		return -1;
	}
	char **keystr=new char*[1];
	int key_size = 1;
	keystr[0]=new char[128];
	unsigned int group_name_code=hash_code(group_name);
	sprintf(keystr[0], "%u-%u", GROUP_NAME_INDUSTRY_DB_NO, group_name_code);

	map<uint64_t, const char*> result;
	int redis_flag = ((McDbInterface *)p_groupname_industry_interface)->mget(1, keystr, key_size, result);

	//取到数据，释放keystr
	delete[] keystr[0];
	delete[] keystr;
	//如果没有对应的垂直定向包id，直接返回
	if(result.size()<1)
	{
		return 0;
	}
	string industry_id=result[group_name_code];
	LOG_ERROR("group_name:%s, group_name_code:%u, industry_id:%s!", group_name.c_str(), group_name_code, industry_id.c_str());

	//将分组feed match到的广告放入output中，最多num个
	set<string> result_set; //广告排重
	set<string>::iterator itr_result;
	VEC_CAND::const_iterator end = input_vec.end();
	for(VEC_CAND::const_iterator itr = input_vec.begin(); itr != end; ++itr)
	{
		Fans_Tar_Ad *itr_temp= (Fans_Tar_Ad*)(*itr);
		if(itr_temp->field_id == industry_id)
		{
			itr_result=result_set.find(itr_temp->adid);
			if(itr_result != result_set.end())
				continue;
			LOG_ERROR("matched group_name:%s, group_name_code:%u, industry_id:%s!", group_name.c_str(), group_name_code, industry_id.c_str());
			output_vec.push_back(*itr);
			result_set.insert(itr_temp->adid);
			flag_group=true;
		}
		if(num == output_vec.size())
		{
			break;
		}
	}
	return 0;
}

