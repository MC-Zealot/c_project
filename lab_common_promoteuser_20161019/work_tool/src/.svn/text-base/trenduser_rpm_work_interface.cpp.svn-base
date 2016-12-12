#include "trenduser_rpm_work_interface.h"

DYN_WORK(TrendUserRPMWorkInterface);
int TrendUserRPMWorkInterface::get_user_mfeature(std::map<uint64_t, uint64_t>& map_return_value,
		uint64_t uid, const std::vector<uint64_t>& vec_item_key){
	
	if(NULL == p_db_company_){
		LOG_ERROR("db_company is NULL!");
		return -1;
	}

	//get global db company
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	if(NULL == p_global_db_company){
		LOG_ERROR("global db_company is NULL!");
		return -1;
	}
	GlobalDbInterface* global_db_interface = 
		p_global_db_company->get_global_db_interface("USER_FP_DB");
	if(NULL == global_db_interface){
		LOG_ERROR("global db_interface is NULL!");
		return -1;
	}

	bool flag = ((GlobalFpDbInterface*)global_db_interface)->gets_value(
			map_return_value, uid, vec_item_key, "userfp");

	return flag;
}

int TrendUserRPMWorkInterface::get_user_feature(uint64_t& return_value, uint64_t uid, uint64_t item_key){
	
	if(NULL == p_db_company_){
		LOG_ERROR("db_company is NULL!");
		return -1;
	}

	//get global db company
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	if(NULL == p_global_db_company){
		LOG_ERROR("global db_company is NULL!");
		return -1;
	}
	GlobalDbInterface* global_db_interface = 
		p_global_db_company->get_global_db_interface("USER_FP_DB");
	if(NULL == global_db_interface){
		LOG_ERROR("global db_interface is NULL!");
		return -1;
	}
	// bool get_value(uint64_t& return_value, uint64_t id,
	// uint64_t item_id, const char* dbid)
	//
	bool flag = ((GlobalFpDbInterface*)global_db_interface)->get_value(
			return_value, uid, item_key, "userfp");
	return flag;
}

int TrendUserRPMWorkInterface::generate_out_str(int cand_num, candidate_item_t& cand,
		int& index, char*& p_out_temp, int& n_len){

	if(index == -2){				// 构建字符串起始
		n_len = 0;
		int len = sprintf(p_out_temp, "%s", "{\"return_code\":\"200\",\"result\":[");
		p_out_temp = p_out_temp + len;
		n_len += len;
	}else if(index == -1){			//  构建字符串末尾
		int len = sprintf(p_out_temp, "%s", "]}");
		p_out_temp = p_out_temp + len;
		n_len += len;

	}else{							// 填充字符串内容
		uint64_t uid = cand.uid;
		uint64_t tscore = cand.tscore;
		uint64_t rpmscore = cand.rpmscore;
		uint32_t user_type = cand.utype;
		//0 is guanzhu of guanzhu; 3 is hufen of hufen
		//user_type : 0 is for common user 
		//
		if(user_type == 0){
			if(cand.reason[3].bnum == 0)
				return 0;
		}else{
			if((cand.reason[0].bnum + cand.reason[1].bnum + cand.reason[2].bnum)== 0)
				return 0;
		}

		int len = 0;
		if(index == 0)
			len = sprintf(p_out_temp, "{\"tid\":%"PRIu64, uid);
		else
			len = sprintf(p_out_temp, ",{\"tid\":%"PRIu64, uid);
		index ++;							// 用户控制结果数量
		p_out_temp = p_out_temp + len;
		n_len += len;

		len = sprintf(p_out_temp, ",\"sco\":%"PRIu64",\"rpm_sco\":%"PRIu64",\"utype\":%"PRIu32, tscore, rpmscore, user_type);
		p_out_temp = p_out_temp + len;
		n_len += len;
		
		uint32_t bnum = cand.reason[3].bnum;

		if(bnum != 0){

			len = sprintf(p_out_temp, ",\"hoh_bnum\":%"PRIu32",\"hoh_ext\":[", bnum);
			p_out_temp = p_out_temp + len;
			n_len += len;

			uint32_t little_num = (bnum > max_b_num) ? max_b_num : bnum;
			for(int i = 0; i < (int)little_num; i++){
				if (i == 0){
					len = sprintf(p_out_temp, "%"PRIu64, cand.reason[3].bs[i]);
				}else
					len = sprintf(p_out_temp, ",%"PRIu64, cand.reason[3].bs[i]);

				p_out_temp = p_out_temp + len;
				n_len += len;
			}

			len = sprintf(p_out_temp, "]");
			p_out_temp = p_out_temp + len;
			n_len += len;

		}
	
		uint32_t total_bnum = cand.reason[0].bnum + 
			cand.reason[1].bnum + cand.reason[2].bnum;

		uint64_t temp_bridge_uids[max_b_num * 3];
		uint32_t temp_uid_num = 0;

		uint32_t bri_num = (cand.reason[0].bnum > max_b_num) ? max_b_num : cand.reason[0].bnum;
		for(uint32_t i = 0; i < bri_num ; i++){
			temp_bridge_uids[i] = cand.reason[0].bs[i];
			temp_uid_num ++;
		}

		if(cand_num < CAND_NUM || temp_uid_num == 0){
			for(int j = 1; j <= 2; j ++){
				bri_num = (cand.reason[j].bnum > max_b_num) ? max_b_num : cand.reason[j].bnum;
				for(uint32_t i = 0; i < bri_num ; i++){
					temp_bridge_uids[temp_uid_num] = cand.reason[j].bs[i];
					temp_uid_num ++;
				}
			}
		}

		if(temp_uid_num != 0){
			
			len = sprintf(p_out_temp, ",\"gog_bnum\":%"PRIu32",\"gog_ext\":[", total_bnum);
			p_out_temp = p_out_temp + len;
			n_len += len;

			uint32_t little_num = (temp_uid_num > max_b_num) ? max_b_num : temp_uid_num;

			for(int i = 0; i < (int)little_num; i++){
				if (i == 0){
					len = sprintf(p_out_temp, "%"PRIu64, temp_bridge_uids[i]);
				}else
					len = sprintf(p_out_temp, ",%"PRIu64, temp_bridge_uids[i]);

				p_out_temp = p_out_temp + len;
				n_len += len;
			}

			len = sprintf(p_out_temp, "]");
			p_out_temp = p_out_temp + len;
			n_len += len;
		}

		len = sprintf(p_out_temp, "%s", "}");
		p_out_temp = p_out_temp + len;
		n_len += len;

	}
	return 1;
}
	
int TrendUserRPMWorkInterface::get_offline_reco_uids(uint64_t uid, uint32_t num, uint64_t req_id,
		char* p_out_string, int& n_out_len){
	
	if(NULL == p_db_company_){
		LOG_ERROR("db_company is NULL!");
		return -1;
	}
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get offline rpm uids start", spend_msec, true);

	DbInterface* p_offline_db_interface = p_db_company_->get_db_interface("OFFLINE_USER_DB");
	if(NULL == p_offline_db_interface){
		LOG_ERROR("offline db_interface is NULL!");
		return -1;
	}
	
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	if(NULL == p_global_db_company){
		LOG_ERROR("global db_company is NULL!");	
		return -1;
	}
	GlobalDbInterface* global_vu_db_interface = 
		p_global_db_company->get_global_db_interface("V_USER_DB");
	if(NULL == global_vu_db_interface){
		LOG_ERROR("global vu_db_interface is NULL!");
	}

	int db_num = 13 + uid/10%24%4;
	char skey[64];
	snprintf(skey, 64, "%d-%"PRIu64, db_num, uid);

	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_offline_db_interface->s_get(0, skey, p_result, split_char, 
		second_split_char, uid);

	if(result <= 0 || NULL == p_result){
		LOG_ERROR("offline result failed! %"PRIu64, uid);
		return -1;
	}

	if(NULL == cands_){			// cands_复用，使用num控制
		LOG_ERROR("cands is NULL!");
		cands_ = new CandUser(true);
		if(NULL == cands_){
			LOG_ERROR("new cands is failed!");
			return -1;
		}
	}

	int limit_num = OFF_LIMIT_LEN;
	result = cands_->parse(p_result, COMPRESS_LEN, limit_num);
	if(result < 0){
		LOG_ERROR("offline result parse is failed!");
		return -1;
	}

	candidate_user_t* data = cands_->data();			// 推荐用户结构体指针
	char* p_out_stream = p_out_string;
	int cand_num = data->num;
	if(cand_num <= (int)num){			// 若离线结果不够，直接退出，使用在线计算
		LOG_ERROR("%"PRIu64" original offline result num: %d", uid, cand_num);
		return -2;
	}

	if( data != NULL && cand_num > 0){
		
		//获取不能推荐列表	
		__gnu_cxx::hash_set<uint64_t> hashset_filter_uids;
		get_filter_uids(uid, hashset_filter_uids);

		VEC_CAND cand_rpm;			// 需计算RPM的用户集合，类型为vector<candidate_item_t*>，见cand_user.h
		for(int n = 0; n < cand_num; n++){
			uint64_t reco_uid = data->cands[n].uid;		// 获取推荐用户uid
			__gnu_cxx::hash_set<uint64_t>::const_iterator it = hashset_filter_uids.find(reco_uid);
			if(it == hashset_filter_uids.end()){		// 若用户可被推荐		
				uint32_t user_type = 0;
				if(global_vu_db_interface != NULL){
					bool flag = ((GlobalKVDbInterface*)global_vu_db_interface)->get_value(reco_uid, user_type);
					if(flag){
						if(user_type == 3) 				// 企业蓝V过滤
							continue;
						else if(user_type != 1)
							user_type = 2;
					}else{
						user_type = 0;
					}		
				}
				data->cands[n].utype = user_type;
				if(user_type == 0){						// 若是普户，且不存在好友的好友，pass
					if(data->cands[n].reason[3].bnum == 0)
						continue;
				}else{									// 对V用户，不存在好友的好友之外的桥梁，pass
					if((data->cands[n].reason[0].bnum + data->cands[n].reason[1].bnum + 
							data->cands[n].reason[2].bnum) == 0)
						continue;
				}
				cand_rpm.push_back(&(data->cands[n]));	// 存放地址
			}
		}

		cand_num = cand_rpm.size();
		if(cand_num <= CONFIDENT_NUM){					// 过滤后结果低于阈值，退出，使用在线计算模块
			LOG_ERROR("%"PRIu64" filtered offline result num: %d", uid, cand_num);
			return -2;
		}
		
		/*======print for debug=====*/
		/*LOG_ERROR("before rpm algorithm: %d", cand_num);
		char tmp[102400];
		char* debug_str = tmp;
		candidate_item_t debug_cand;
		int debug_len = 0;
		int debug_index = -2;
		generate_out_str(cand_num, debug_cand, debug_index, debug_str, debug_len);
		debug_index = 0;
		for(VEC_CAND::iterator it = cand_rpm.begin(); it != cand_rpm.end(); it ++){
			generate_out_str(cand_num, **it, debug_index, debug_str, debug_len);
			if (debug_index >= int(num)) break;
		}
		debug_index = -1;
		generate_out_str(cand_num, debug_cand, debug_index, debug_str, debug_len);
		LOG_ERROR("debug_len:%d", debug_len);
		tmp[debug_len] = '\0';
		debug_str = tmp;
		LOG_ERROR("before rpm algorithm: %s", tmp);*/
		/*=====print for debug end=====*/

		//struct timeval tv_rpm;
		//tv_rpm = calc_spend_time(tv_rpm, "rpm model start", spend_msec, true);
		// 使用RPM模型计算得分
		AccessStr access_str;
		access_str.uid_ = uid;
		int res = algorithm_core(req_id, access_str, cand_rpm);		// 输入输出均使用cand_rpm，引用传递
		if(res < 0) LOG_ERROR("%"PRIu64" rpm algorithm core fail!", uid);
		//tv_rpm = calc_spend_time(tv_rpm, "rmp model finish", spend_msec);
		candidate_item_t cand;
		int index = -2;
		generate_out_str(cand_num, cand, index, p_out_stream, n_out_len);
		index = 0;
		for(VEC_CAND::iterator it = cand_rpm.begin(); it != cand_rpm.end(); it ++){
			// 迭代器是指针，容器中每个元素是指针，所以使用指针的指针
			generate_out_str(cand_num, **it, index, p_out_stream, n_out_len);
			if (index >= int(num)) break;
		}
		if(index < CONFIDENT_NUM){					// 生成后结果低于阈值，退出，使用在线计算模块
			LOG_ERROR("%"PRIu64" final offline result num: %d", uid, index);
			return -2;
		}
		index = -1;
		generate_out_str(cand_num, cand, index, p_out_stream, n_out_len);
		
		/*======print for debug=====*/
		/*cand_num = cand_rpm.size();
		LOG_ERROR("after rpm algorithm: %d", cand_num);
		debug_len = 0;
		debug_index = -2;
		generate_out_str(cand_num, debug_cand, debug_index, debug_str, debug_len);
		debug_index = 0;
		for(VEC_CAND::iterator it = cand_rpm.begin(); it != cand_rpm.end(); it ++){
			generate_out_str(cand_num, **it, debug_index, debug_str, debug_len);
			if (debug_index >= int(num)) break;
		}
		debug_index = -1;
		generate_out_str(cand_num, debug_cand, debug_index, debug_str, debug_len);
		tmp[debug_len] = '\0';
		LOG_ERROR("after rpm algorithm: %s", tmp);*/
		/*=====print for debug end=====*/

	}
	else{
		LOG_ERROR("data is NULL!");
		return -1;
	}
	tv_temp = calc_spend_time(tv_temp, "offline rpm finish", spend_msec);
	return 1;
}

// 获取在线计算数据，并对其使用RPM模型
int TrendUserRPMWorkInterface::get_online_reco_result(uint64_t uid, uint32_t num, 
		uint64_t req_id, char* p_out_string, int& n_out_len){
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get online rpm uids start", spend_msec, true);

	// 获取在线计算结果，生成的是字符串
	char* online_out_str = p_out_string;	// 内存重复使用
	int online_out_len = 0;
	int result = get_online_reco_uids(uid, num, online_out_str, online_out_len);
	if (result <= 0){
		LOG_ERROR("get online result error!");
		return -2;
	}

	// 将在线结果保存在cands_中，以节省内存
	if(NULL == cands_){			// cands_复用，使用num控制
		LOG_ERROR("cands is NULL!");
		cands_ = new CandUser(true);
		if(NULL == cands_){
			LOG_ERROR("new cands is failed!");
			return -1;
		}
	}
	candidate_user_t* data = cands_->data();
	
	/* 以json格式解析字符串*/
	//{"return_code":"200","result":[{"tid":uid,"sco":score,"utype":type,
	// "hoh_bnum":hnum,"hoh_ext":[uid1,uid2],"gog_bnum":gnum,"gog_ext":[uid1,uid2]}]}
	json_object* res_json = json_tokener_parse(online_out_str);
	if (NULL == res_json){
		LOG_ERROR("parse online result error!");
		return -2;
	}
	if(is_error(res_json)){
		LOG_ERROR("parse online result error!");
		json_object_put(res_json);
		return -2;
	}

	// 解析return code字段
	json_object* code_json = json_object_object_get(res_json, "return_code");
	if(NULL == code_json){
		LOG_ERROR("online result is uncorrect");
		json_object_put(res_json);
		return -2;
	}
	char* code_str = NULL;
	code_str = (char*)json_object_get_string(code_json);
	if(strcasecmp(code_str, "200") != 0){
		LOG_ERROR("online result is uncorrect");
		json_object_put(res_json);
		return -2;
	}

	// 解析推荐结果字段
	json_object* result_json = json_object_object_get(res_json, "result");
	if(NULL == result_json || is_error(result_json)){
		LOG_ERROR("online result is uncorrect");
		json_object_put(res_json);
		return -2;
	}
	VEC_CAND cand_rpm;		// 候选集合
	if(json_object_is_type(result_json, json_type_array)){
		int array_len = json_object_array_length(result_json);
		if (array_len == 0) return -1;	
		data->num = array_len;

		for(int i = 0; i < array_len; i++){
			// like {"tid":uid,"sco":score,"utype":type,"hoh_bnum":hnum,"hoh_ext":[uid1,uid2],
			// "gog_bnum":gnum,"gog_ext":[uid1,uid2]}
			// 获取单条候选
			json_object* cand_json = json_object_array_get_idx(result_json, i);
			// 申请存储空间
			//candidate_item_t* cand_ptr = (candidate_item_t*) malloc(sizeof(candidate_item_t));
			//if(!cand_ptr) continue;
			candidate_item_t* cand_ptr = &(data->cands[i]);		// 避免申请内存，直接指向成员变量

			// 获取推荐id
			json_object* info_json = json_object_object_get(cand_json, "tid");
			cand_ptr->uid = (uint64_t)json_object_get_int64(info_json);
			
			// 获取得分
			info_json = json_object_object_get(cand_json, "sco");
			cand_ptr->tscore = (uint64_t)json_object_get_int64(info_json);
			cand_ptr->rpmscore = 0;
			
			// 获取用户类型
			info_json = json_object_object_get(cand_json, "utype");
			cand_ptr->utype = (uint32_t)json_object_get_int(info_json);
			
			// 获取桥梁
			for(int j = 0; j < REASON_NUM; j++){	// 初始化桥梁信息
				cand_ptr->reason[j].score = 0;
				cand_ptr->reason[j].bnum = 0;
				for(int k = 0; k < MAX_BRIDGE_NUM ; k++)
					cand_ptr->reason[j].bs[k] = 0;
			}
			// 存在关注的关注桥梁
			info_json = json_object_object_get(cand_json, "gog_bnum");
			if(NULL != info_json){	
				cand_ptr->reason[0].bnum = (uint32_t)json_object_get_int(info_json);
				info_json = json_object_object_get(cand_json, "gog_ext");
				int bri_len = json_object_array_length(info_json);
				bri_len =  bri_len < MAX_BRIDGE_NUM ? bri_len: MAX_BRIDGE_NUM;
				for(int j = 0; j< bri_len; j++){
					json_object* bid_json = json_object_array_get_idx(info_json, j);
					cand_ptr->reason[0].bs[j] = (uint64_t)json_object_get_int64(bid_json);
				}
			}
			// 好友的好友桥梁
			info_json = json_object_object_get(cand_json, "hoh_bnum");
			if(NULL != info_json){
				cand_ptr->reason[3].bnum = (uint32_t)json_object_get_int(info_json);
				info_json = json_object_object_get(cand_json, "hoh_ext");
				int bri_len = json_object_array_length(info_json);
				bri_len =  bri_len < MAX_BRIDGE_NUM ? bri_len: MAX_BRIDGE_NUM;
				for(int j = 0; j< bri_len; j++){
					json_object* bid_json = json_object_array_get_idx(info_json, j);
					cand_ptr->reason[3].bs[j] = (uint64_t)json_object_get_int64(bid_json);
				}
			}
			cand_rpm.push_back(cand_ptr);
		}
	}
	json_object_put(res_json);
	/*解析在线结果结束*/

	/* 使用RPM模型计算得分 */
	//struct timeval tv_rpm;
	//tv_rpm = calc_spend_time(tv_rpm, "rpm model online start", spend_msec, true);
	AccessStr access_str;
	access_str.uid_ = uid;
	int res = algorithm_core(req_id, access_str, cand_rpm);				// 输入输出均使用cand_rpm，引用传递
	if(res < 0) LOG_ERROR("%"PRIu64" rpm algorithm core fail!", uid);
	//tv_rpm = calc_spend_time(tv_rpm, "rmp model online finish", spend_msec);
	/* 使用RPM模型结束 */

	// 生成结果转成字符串
	char* p_out_stream = p_out_string;			// 内存重复使用
	int cand_num = cand_rpm.size();
	candidate_item_t cand;
	int index = -2;
	generate_out_str(cand_num, cand, index, p_out_stream, n_out_len);
	index = 0;
	for(VEC_CAND::iterator it = cand_rpm.begin(); it != cand_rpm.end(); it ++){
		// 迭代器是指针，容器中每个元素是指针，所以使用指针的指针
		generate_out_str(cand_num, **it, index, p_out_stream, n_out_len);
		if (index >= int(num)) break;
	}
	index = -1;
	generate_out_str(cand_num, cand, index, p_out_stream, n_out_len);

	// 释放内存
	/*for(VEC_CAND::iterator it = cand_rpm.begin(); it != cand_rpm.end(); it++){
		free(*it);
		*it = NULL;
	}*/
	tv_temp = calc_spend_time(tv_temp, "online rpm finish", spend_msec);
	cand_rpm.clear();
	return 1;
}

int TrendUserRPMWorkInterface::get_reco_uids(const char* uid_str, uint32_t num, int type, int zip,
		uint64_t req_id, char* p_out_string, int& n_out_len){

	if( NULL == uid_str)
		return -1;

	uint64_t uid = strtoull(uid_str, NULL, 10);
	int result = 0;
	char* p_out_temp = NULL;
	char buf[JSON_LEN] = {'\0'};
	if(zip == 1){
		p_out_temp = buf;
	}else
		p_out_temp = p_out_string;

	int n_out_temp = 0;
	
	// 获取离线推荐数据
	//result = get_offline_reco_uids(uid, num, req_id, p_out_temp, n_out_temp);
	if(type == 0){
		result = get_offline_reco_uids(uid, num, req_id, p_out_temp, n_out_temp);
	}else if(type == 1){
		result = get_online_reco_result(uid, num, req_id, p_out_temp, n_out_temp);
	}else{
		result = get_offline_reco_uids(uid, num, req_id, p_out_temp, n_out_temp);
		if(result <= 0 ){
			if(result == -2) // CONFIDENT_NUM
				LOG_TRACE("offline data is not confidential!");

			LOG_TRACE("online reco uid!");
			result = get_online_reco_result(uid, num, req_id, p_out_temp, n_out_temp);
		}
		else{
			LOG_TRACE("offline reco uid!");
		}
	}

	if(zip == 1){	
		if(p_out_temp != NULL){
			uLongf uLongf_out = JSON_LEN;
			result = compress((Bytef *)p_out_string, &uLongf_out, (Bytef *)p_out_temp, (uLong)n_out_temp);

			if(result != Z_OK){
				LOG_ERROR("zlib compress error is %d", result);
				result = -1;
			}else{
				LOG_TRACE("zlib compress success:%d", result);
				n_out_len = (int)uLongf_out;
				result = 1;
			}
		}
		else
			result = -1;
	}else
		n_out_len = n_out_temp;

	return result;
}

int TrendUserRPMWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "trend_rpm work start", spend_msec, true);
	
	int result = 0;

	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	//固定用户定时更新
	if(strcasecmp(cmd_str, "query") == 0){

		json_object* uid_json = json_object_object_get(req_json, "uid");
		char* uid_str = NULL;
		if(NULL == uid_json){
			uid_str = "";
			//return -1;
			return return_fail(403, "uid is empty", p_out_string, n_out_len);
		}else{
			uid_str = (char*)json_object_get_string(uid_json);
		}
		
		int64_t req_id = 0;
		json_object* reqid_json = json_object_object_get(req_json, "logid");
		if(NULL == reqid_json){
			req_id = 0;
		}else{
			req_id = (uint64_t)json_object_get_int64(reqid_json);
		}

		uint32_t num = 600;
		json_object* num_json = json_object_object_get(req_json, "num");
		if(NULL == num_json){
			num = 600;
		}else{
			num = (uint32_t)json_object_get_int(num_json);
		}
		
		int type = -1;
		json_object* type_json = json_object_object_get(req_json, "type");
		if(NULL == type_json){
			type = -1;
		}else{
			type = json_object_get_int(type_json);
		}
		
		int zip = 0;
		json_object* zip_json = json_object_object_get(req_json, "zip");
		if(NULL == zip_json){
			zip = 0;
		}else{
			zip = json_object_get_int(zip_json);
		}

		result = get_reco_uids(uid_str, num, type, zip, req_id, p_out_string, n_out_len);
		if(result <= 0){
			return return_fail(405, "compute is wrong!", p_out_string, n_out_len);
		}
	}
	tv_temp = calc_spend_time(tv_temp, "trend_rpm_work all finish", spend_msec);
	return result;
}
