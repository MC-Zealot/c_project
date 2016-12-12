#include "p2p_work_interface.h"
DYN_WORK(P2PWorkInterface);

// 并行获取数据
// input: 被访问者B，need_relation表示是否需要按照关系排序
// output: 
// 1、B的粉丝相似度，按照粉丝相似度分值排序。若有需要，则先按照是否关注排序
// 2、B的亲密度数据，按照亲密度分值排序。若有需要，则先按照是否互粉排序
int P2PWorkInterface::get_recom_data(const uint64_t uid, const bool need_relation, 
		const __gnu_cxx::hash_set<uint64_t> filter_set,
		vector<RecoInfo>& fanssim_vec, vector<RecoInfo>& intimacy_vec){
	if(NULL == p_db_company_){
		LOG_ERROR("db_company is NULL!");
		return -1;
	}

	ReqResultMap req_result_map;
	// 取关注
	const int follow_db_id = 3;
	int result = push_request_result_map(follow_db_id, uid, 0, 0, NULL, true, req_result_map);
	if(result <= 0)
		LOG_ERROR("create follow db failed!");

	// 粉丝相似度
	const int fanssim_db_id = 18;		// 在配置文件中的db_num
	const int key_num = 17;				
	char fanssim_str[20];
	snprintf(fanssim_str, 20, "%d-%"PRIu64, key_num, uid);       // MC协议，构建key
	result = push_request_result_map(fanssim_db_id, uid, 0, 0, fanssim_str, false, req_result_map);
	if(result <= 0)
		LOG_ERROR("create fanssim db failed!");

	// 互粉数据
	const int friend_db_id = 6;
	result = push_request_result_map(friend_db_id, uid, 0, 0, NULL, true, req_result_map);
	if(result <= 0)
		LOG_ERROR("create hf db failed!");

	// 亲密度数据
	const int intimacy_db_id = 5;
	char intimacy_str[40];
	sprintf(intimacy_str, "{\"cmd\":\"query\",\"id\":%"PRIu64"}", uid);
	result = push_request_result_map(intimacy_db_id, uid, 0, 0, intimacy_str, false, req_result_map);
	if(result <= 0)
		LOG_ERROR("create qmd db failed!");

	// 并行获取上述数据
	if(p_db_company_->get_multi_db(req_result_map) > 0){
		char split_char, second_split_char;					// 分割符
		__gnu_cxx::hash_set<uint64_t> uid_set;				// 用户uid集合
		vector<PAIR_INT_FLOAT> uid_sco_vec;					// uid-score向量

		// 获取用户关注数据
		char* p_follow_result = NULL;
		get_data_from_req_result_map(follow_db_id, req_result_map,
				p_follow_result, split_char, second_split_char);
		get_data_set(p_follow_result, split_char, second_split_char, uid_set);

		// 获取用户粉丝相似度数据
		char* p_fanssim_result = NULL;
		get_data_from_req_result_map(fanssim_db_id, req_result_map,
				p_fanssim_result, split_char, second_split_char);
		if(p_fanssim_result != NULL){
			get_fanssim_data((const char* &)p_fanssim_result, split_char, second_split_char, uid_sco_vec);
		}else{
			LOG_ERROR("user %"PRIu64" fanssim data is NULL!", uid);
		}
		LOG_ERROR("get fanssim data finish! %d", uid_sco_vec.size());

		// 用户粉丝相似度数据与关注数据的交集
		// 输入：uid_sco_vec保存用户的粉丝相似度数据[(uid1-score1),(uid2-score2)]
		// 输入：uid_set保存用户的关注集合(uid1,uid2)
		// 输出：fanssim_vec 用户关注的粉丝相似度用户，按照粉丝相似度数值排名
		if(need_relation){
			recom_intersection(RELATION_FOLLOW, SOURCE_FANSSIM, filter_set, 
					uid_set, uid_sco_vec, fanssim_vec);
		}else{
			recom_intersection(RELATION_NO, SOURCE_FANSSIM, filter_set, uid_set, uid_sco_vec, fanssim_vec);
		}
		LOG_ERROR("get recom fanssim data finish! %d", fanssim_vec.size());

		// 清空再次使用，由于再次放入的数据可能有相同的规模，所以不用swap来节约内存
		uid_set.clear();
		uid_sco_vec.clear();
		
		// 获取用户互粉数据
		char* p_friend_result = NULL;
		get_data_from_req_result_map(friend_db_id, req_result_map,
				p_friend_result, split_char, second_split_char);
		get_data_set(p_friend_result, split_char, second_split_char, uid_set);
		LOG_ERROR("get hf data finish! %d", uid_set.size());
		
		// 获取用户亲密度数据
		char* p_intimacy_result = NULL;
		get_data_from_req_result_map(intimacy_db_id, req_result_map,
				p_intimacy_result, split_char, second_split_char);
		if(p_intimacy_result != NULL){
			get_intimacy_data((const char* &)p_intimacy_result, split_char, second_split_char, uid_sco_vec);
		}else{
			LOG_ERROR("user %"PRIu64" intimacy data is NULL!", uid);
		}
		LOG_ERROR("get intimacy data finish! %d", uid_sco_vec.size());

		// 互粉数据与亲密度数据的交集
		// 输入：uid_sco_vec保存用户的亲密度数据[(uid1-score1),(uid2-score2)]
		// 输入：uid_set保存用户的互粉数据集合(uid1,uid2)
		// 输出：friend_vec 用户带亲密度的互粉用户，按照亲密度数值排名
		if(need_relation){
			recom_intersection(RELATION_FRIEND, SOURCE_INTIMACY, filter_set, 
					uid_set, uid_sco_vec, intimacy_vec);
		}else{
			recom_intersection(RELATION_NO, SOURCE_INTIMACY, filter_set, uid_set, uid_sco_vec, intimacy_vec);
		}
		LOG_ERROR("get recom intimcay data finish! %d", intimacy_vec.size());

	}
	release_request_result_map(req_result_map);
	return 0;
}

// 获取关注数据，获取互粉数据
// input: 
// output: (uid, uid, ...)
int P2PWorkInterface::get_data_set(char* p_result, char split_char, char second_split_char, 
		__gnu_cxx::hash_set<uint64_t>& hashset_uids){
	if(NULL == p_result){
		LOG_ERROR("get follow or friend data fail!");
		return -1;
	}

	uint32_t uid_num = 0;
	split_string_set_ids(hashset_uids, uid_num, (const char* &)p_result, split_char, 0);
	return 1;
}

// 获取粉丝相似度
// 输入：uid-score-isfollow%uid-score-isfollow
// 输出：[(uid, score), (uid, score)]
void P2PWorkInterface::get_fanssim_data(const char*& str_input, const char sep_char, 
		const char second_sep_char, vector<PAIR_INT_FLOAT>& vec_ids){
	const char* p_temp = str_input;
	int index = 0;
	while((*p_temp) != '\0'){								// 遍历字符串
		uint64_t uid = 0;
		while((*p_temp) != second_sep_char){				// 计算uid
			if ((*p_temp) >= '0' && (*p_temp) <= '9'){
				uid = uid * 10 + (*p_temp) - '0';
				p_temp ++;
			}
		}
		p_temp ++;											// 遇到分隔符，指针+1跳过

		float score = 0.0f;
		int score_index = 0;								// 有几位小数
		bool isfloat = false;								// 判断是否是小数点后的部分
		while((*p_temp) != second_sep_char){                // 计算得分
				if ((*p_temp) >= '0' && (*p_temp) <= '9'){
				score = score * 10 + (*p_temp) - '0';		// 分数累积成整数
				if(isfloat) score_index ++;
			}
			else if((*p_temp) == '.'){						// 遇到小数点，开始计数
				isfloat = true;
			}
			p_temp ++;
		}
		score /= float(pow(10, score_index));				// 累积的整数值除以小数位数
		vec_ids.push_back(make_pair(uid, score));
		p_temp ++;											// 遇到分隔符，跳过
		
		while((*p_temp) != sep_char){						// follow字段
			p_temp ++;
			if((*p_temp) == '\0') break;					// 若是最后一个元素，则退出
		}
		if((*p_temp) == sep_char) p_temp ++;				// 若不是最后一个元素，继续循环，否则循环会退出
		index ++;
	}
}

// 获取亲密度数据
// 输入：uid-score%uid-score%
// 输出：[(uid,score),(uid-score)]
void P2PWorkInterface::get_intimacy_data(const char*& str_input, const char sep_char, 
		const char second_sep_char, vector<PAIR_INT_FLOAT>& vec_ids){
	const char* p_temp = str_input;
	uint64_t id = 0;

	// 从头往尾遍历
	while((*p_temp) != '\0'){
		if ((*p_temp) >= '0' && (*p_temp) <= '9'){		// 前面的是uid
			id = id * 10 + (*p_temp) - '0';
		}
		else if((*p_temp) == second_sep_char){			// 遇到内部分隔符，开始计算得分
			p_temp ++;
			float score = 0.0f;							// 得分
			int score_index = 0;						// 有几位小数
			bool isfloat = false;						// 判断是否是小数点后的部分

			while(((*p_temp) != sep_char) && ((*p_temp) != '\0')){	// 遇到外部分隔符终止
				if ((*p_temp) >= '0' && (*p_temp) <= '9'){
					score = score * 10 + (*p_temp) - '0';			// 分数累积成整数
					if(isfloat) score_index ++;
				}
				else if((*p_temp) == '.'){				// 遇到小数点，开始计数
					isfloat = true;
				}
				p_temp ++;
			}
			score /= float(pow(10, score_index));		// 累积的整数值除以小数位数
			vec_ids.push_back(make_pair(id, score));
			id = 0;
		}
		if((*p_temp) != '\0') p_temp ++;
	}
}


// 互粉好友按照亲密度排序
// 输入：relation表示加入关系，0表示不考虑关系，1表示关注，2表示互粉
// 输入：source表示数据来源，0表示无来源，1表示搜索共现，2表示粉丝相似度，3表示亲密度
// 输入：过滤uid集合，关系uid集合，(uid,score)向量，
// 输出：对uid求交，得到(uid,score,relation)向量，并按照score排序
int P2PWorkInterface::recom_intersection(const int relation, const int source,
		const __gnu_cxx::hash_set<uint64_t>& filter_set,
		const __gnu_cxx::hash_set<uint64_t>& uid_set,
		const vector<PAIR_INT_FLOAT>& uid_sco_vec, vector<RecoInfo>& inter_vec){

	/*if(uid_set.size() <= 0){								// uid集合为空
		LOG_ERROR("uid set is NULL!");
		return -1;
	}*/
	
	if(uid_sco_vec.size() <= 0){							// 键值对列表为空
		LOG_ERROR("uid-score vector is NULL!");
		return -1;
	}
	
	// 将带分值的推荐结果与id集合求交
	for(vector<PAIR_INT_FLOAT>::const_iterator it = uid_sco_vec.begin(); it != uid_sco_vec.end(); it++){
		uint64_t uid = it->first;

		__gnu_cxx::hash_set<uint64_t>::iterator sit = find(filter_set.begin(), filter_set.end(), uid);
		if(sit != filter_set.end()){
			continue;				// 若推荐元素在过滤集中，跳过
		}
		if(!IsGoodUser(uid)){		// 若推荐元素不是优质用户，跳过
			continue;
		}

		RecoInfo reco_ele;			// 推荐元素
		reco_ele.reco_id = uid;
		reco_ele.score = it->second;
		reco_ele.source = source;
		reco_ele.user_type = get_v_type(uid);

		sit = find(uid_set.begin(), uid_set.end(), uid);
		if(sit != uid_set.end()){	// 若推荐结果在集合中，则表示存在关系
			reco_ele.relation = relation;
		}else{						// 否则，不存在关系
			reco_ele.relation = 0;			
		}
		inter_vec.push_back(reco_ele);
	}
	
	if(relation > 0){	// 若传入的relation>0，表示需要考虑关系，先按关系排序，再按分值排序
		sort(inter_vec.begin(), inter_vec.end(), cmp_all);          // 先关系后分值排序
	}else{				// 若传入的relation<=0，表示不考虑关系，仅按分值排序
		sort(inter_vec.begin(), inter_vec.end(), cmp_score);        // 按照分值排序
	}

	return 1;
}

bool P2PWorkInterface::IsGoodUser(const uint64_t uid){
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	GlobalDbInterface* global_db_interface = NULL;
	if(p_global_db_company != NULL){
		global_db_interface = p_global_db_company->get_global_db_interface("GOOD_USER_DB");
	}else{
		global_db_interface = NULL;
	}

	if(global_db_interface == NULL){
		LOG_DEBUG("global db interface is NULL!");
		return -1;
	}
	return global_db_interface->is_exist(uid);
}

// 获取用户V类型数据
int P2PWorkInterface::get_v_type(const uint64_t uid){	
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	GlobalDbInterface* global_db_interface = NULL;
	if(p_global_db_company != NULL){
		global_db_interface = p_global_db_company->get_global_db_interface("V_USER_DB");
	}else{
		global_db_interface = NULL;
	}

	if(global_db_interface == NULL){
		LOG_DEBUG("global db interface is NULL!");
		return -1;
	}
	
	uint32_t vtype = 0;
	// 基类指针转为派生类指针
	GlobalKVDbInterface* v_ptr = (GlobalKVDbInterface*) global_db_interface;
	if (!(v_ptr->get_value(uid, vtype))){					// 未取到结果
		return 0;											// 返回0，表示是普户
	}else{
		int type = int(vtype);								// 类型转换
		if(type < 0) return 0;
		return type;
	}
	return 0;												// 默认返回普户
}


// 获取用户推荐的橙V数据
int P2PWorkInterface::get_yellowV_recom(const uint64_t uid, const __gnu_cxx::hash_set<uint64_t> filter_set, 
		vector<RecoInfo>& yellow_V_data){
	
	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("YELLOW_V_RECOM_DB");
	if (NULL == p_follow_db_interface)
		return -1;

	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_follow_db_interface->get(0, uid, p_result, split_char, second_split_char);
	if (result <= 0 ){
		LOG_ERROR("%s", "get_yellow_v_data fail!");
		return result;
	}
	return 1;
	//LOG_ERROR("user %"PRIu64" yellow V cooccurance: %s", uid, p_result);
	// [uid, uid, uid]
	LOG_ERROR("yellow v result: %s", p_result);
	json_object* result_json = json_tokener_parse(p_result);
	if(is_error(result_json)){
		LOG_ERROR("%s", p_result);
		json_object_put(result_json);
		return -1;
	}
	if(NULL == result_json){
		LOG_ERROR("get_yellow_v_data fail!!");
		//json_object_put(result_json);
		return -1;
	}else{
		if(json_object_is_type(result_json, json_type_array)){
			int n_array_len = json_object_array_length(result_json);	
			for(int i = 0; i < n_array_len; i ++){
				json_object* uid_object = json_object_array_get_idx(result_json, i);
				uint64_t reco_uid = json_object_get_int64(uid_object);

				__gnu_cxx::hash_set<uint64_t>::iterator sit;
				sit = find(filter_set.begin(), filter_set.end(), reco_uid);
				if(sit != filter_set.end()){
					continue;				// 若推荐元素在过滤集中，跳过
				}

				RecoInfo ele;
				ele.reco_id = reco_uid;
				ele.score = 0.0f;
				ele.relation = RELATION_NO;
				ele.source = SOURCE_SEARCH;
				ele.user_type = get_v_type(reco_uid);
				yellow_V_data.push_back(ele);
			}
		}
		json_object_put(result_json);
	}
	
	return 1;
}
/* 获取数据结束 */


void P2PWorkInterface::generate_output_str(const vector<RecoInfo>& result_vec, const int num, 
		const uint64_t tuid, char*& p_out_string, int& n_out_len){
	char* out_str = p_out_string;
	// 返回值头部
	int len = sprintf(out_str, "%s", "{\"return_code\":\"200\",\"result\":[");
	out_str += len;	
	n_out_len = len;
	
	int count = 0;
	for(vector<RecoInfo>::const_iterator it = result_vec.begin(); it != result_vec.end();){
		//int v_type = 0;
		//v_type = get_v_type(it->reco_id);
		len = sprintf(out_str, "{\"tid\":%"PRIu64",\"sco\":%f, \"source\":%d, \"utype\":%d, \"rel\":%d}", 
				it->reco_id, it->score, it->source, it->user_type, it->relation);
		out_str += len;
		n_out_len += len;
		count ++;
		it ++;

		if((count < num) && (it != result_vec.end())){		// 不是结果的最后一个，添加分隔符
			int len = sprintf(out_str, "%s", ", ");
			out_str += len;
			n_out_len += len;
		}else 
			break;
	}

	len = sprintf(out_str, "]}");
	out_str += len;
	n_out_len += len;
}

//功能处理模块
int P2PWorkInterface::p2p_recom(const char* uid_str, const char* tuid_str, const int num,
		const bool need_relation, char*& p_out_string, int& n_out_len){
	
	if( (NULL == uid_str) || (NULL == tuid_str) ){
		LOG_ERROR("uid: %s, tuid: %s", uid_str, tuid_str);
		return -1;
	}
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "start", spend_msec, true);

	/* 获取uid和tuid以及请求的参数 */	
	uint64_t uid = strtoull(uid_str, NULL, 10);
	uint64_t tuid = strtoull(tuid_str, NULL, 10);
	/* 获取参数结束 */
	LOG_ERROR("uid:%"PRIu64", tuid:%"PRIu64, uid, tuid);
	
	/*获取用户过滤集合 */
	__gnu_cxx::hash_set<uint64_t> user_filter_set;				// 获取uid的过滤集合
	int result = get_filter_uids(uid, user_filter_set);
	if(result <= 0){
		LOG_ERROR("get user filter fail!");
		return -1;
	}
	user_filter_set.insert(uid);								// 将用户自身添加到过滤集中
	user_filter_set.insert(tuid);								// 将被访问者添加到过滤集中
	LOG_ERROR("filter num:%d", user_filter_set.size());
	/*获取用户过滤集合结束*/

	/*构建推荐结果*/
	vector<RecoInfo> recom_data;								// 存储推荐结果
	vector<RecoInfo> fanssim_data, intimacy_data;				// 存储粉丝相似度数据，亲密度数据
	get_recom_data(tuid, need_relation, user_filter_set, fanssim_data, intimacy_data);
	LOG_ERROR("fanssim num:%d", fanssim_data.size());
	LOG_ERROR("intimacy num:%d", intimacy_data.size());

	int tuid_v_flag = get_v_type(tuid);							// 被访问者的V类型
	if(tuid_v_flag > 0){
		// 获取橙V推荐
		vector<RecoInfo> yellow_recom;
		int result = get_yellowV_recom(tuid, user_filter_set, yellow_recom);	
		if(result > 0){
			recom_data.insert(recom_data.begin(), yellow_recom.begin(), yellow_recom.end());
		}
		LOG_ERROR("yellow v num:%d", yellow_recom.size());
		yellow_recom.clear();
	}
	
	if(fanssim_data.size() > 0){
		recom_data.insert(recom_data.end(), fanssim_data.begin(), fanssim_data.end());
	}else{										// 若粉丝相似度结果为空
		if(tuid_v_flag <= 0){					// 若用户为普户，则退出
			LOG_ERROR("user %"PRIu64" recom data is NULL!", uid);
		}else{									// 若为V用户，则补充亲密度	
			recom_data.insert(recom_data.end(), intimacy_data.begin(), intimacy_data.end());
		}
	}
	LOG_ERROR("final result num:%d", recom_data.size());
	/*构建推荐结果结束*/
	
	// 构建输出结果
	generate_output_str(recom_data, num, tuid, p_out_string, n_out_len);
	
	user_filter_set.clear();
	recom_data.clear();

	tv_temp = calc_spend_time(tv_temp, "all finish", spend_msec);
	return 1;
	
}

int P2PWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
	int result = 0;
	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}

	if(strcasecmp(cmd_str, "query") == 0){

		json_object* para_json = json_object_object_get(req_json, "uid");
		char* uid_str = NULL;
		if(NULL == para_json){
			return return_fail(403, "uid is empty", p_out_string, n_out_len);
		}else{
			uid_str = (char*)json_object_get_string(para_json);
		}

		para_json = json_object_object_get(req_json, "tuid");
		char* gz_uid = NULL;									// 保存uid
		if(NULL == para_json){									// tuid字段不存在
			return return_fail(403, "cannot get tuid!", p_out_string, n_out_len);
		}else{
			char* gz_id_str = (char*)json_object_get_string(para_json);
			if (gz_id_str == ""){								// tuid字段为空
					return return_fail(403, "cannot get tuid!", p_out_string, n_out_len);
			}else{
				gz_uid = gz_id_str;
			}
		}

		int num = RESULT_NUM;									// 请求结果的数量
		para_json = json_object_object_get(req_json, "num");
		if(NULL != para_json){
			num = (int)json_object_get_int(para_json);
			if (num <= 0){
				num = RESULT_NUM;
			}
		}

		bool need_relation = true;
		para_json = json_object_object_get(req_json, "appkey");
		if(NULL != para_json){
			int appkey = (int)json_object_get_int(para_json);
			if (appkey <= 0){
				need_relation = false;
			}
		}
		
		LOG_ERROR("ready to get recom: [uid:%s, gz_uid:%s, num:%d", uid_str, gz_uid, num);

		result = p2p_recom(uid_str, gz_uid, num, need_relation, p_out_string, n_out_len);
	}
	return result;
}
