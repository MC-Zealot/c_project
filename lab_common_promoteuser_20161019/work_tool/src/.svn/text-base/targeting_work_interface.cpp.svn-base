#include "targeting_work_interface.h"

DYN_WORK(TargetingWorkInterface);
void TargetingWorkInterface::release_map_vec_uid(MAP_VEC_UID& map_vec_uid){
	for(MAP_VEC_UID::iterator it = map_vec_uid.begin(); 
			it != map_vec_uid.end(); it++){
		(*it).second.clear();
	}
	map_vec_uid.clear();
}

// str_input：用户已关注
// reco_uids：推荐结果集合
// 将str_input的结果切分成uid，写入ids[]，并将reco_uids中在ids[]中的用户删除
void TargetingWorkInterface::split_string_ids(uint64_t ids[], uint32_t& id_num, 
		__gnu_cxx::hash_set<uint64_t>& hashset_reco_uids, const char* str_input,
		char sep_char, uint32_t limit){

	if(NULL == str_input)
		return;

	const char* p_temp = str_input;
	if(NULL == str_input)
		return;

	id_num = 0;
	uint64_t id = 0;
	while((*p_temp) != '\0'){
		/*if(limit != 0 && id_num >= limit){
			return;
		}*/ //因为我们要做过滤，过滤已经关注的，所以这个退出是不允许的。
		if(id_num >= MAX_FOLLOW_SIZE)
			break;

		if ((*p_temp) >= '0' && (*p_temp) <= '9'){
			id = id * 10 + (*p_temp) - '0';
		}else if((*p_temp) == sep_char){
			ids[id_num] = id;
			id_num ++;

			__gnu_cxx::hash_set<uint64_t>::iterator it = hashset_reco_uids.find(id);
			if(it != hashset_reco_uids.end()){
				LOG_ERROR("%"PRIu64, id);
				hashset_reco_uids.erase(it);
			}

			id = 0;
		}
		p_temp ++;
	}
	ids[id_num] = id;
	id_num ++;

	if(id_num > limit)
		id_num = limit; // 由于不会提前退出，因此需要在这里做一下limit的限制
}

// str_input：用户关注
// map_vec_uid：<推荐用户，桥梁集合>
void TargetingWorkInterface::split_string_ids_in_set(MAP_VEC_UID& map_vec_uid, uint64_t rel_uid, 
		const char* str_input, char sep_char, uint32_t limit){
	const char* p_temp = str_input;

	uint64_t id = 0;								// 关注的关注
	uint32_t id_num = 0;
	MAP_VEC_UID::iterator m_it;
	while((*p_temp) != '\0'){
		if(limit != 0 && id_num >= limit){
			return;
		}
		if ((*p_temp) >= '0' && (*p_temp) <= '9'){
			id = id * 10 + (*p_temp) - '0';
		}else if((*p_temp) == sep_char){	
			m_it = map_vec_uid.find(id);			// 若关注的关注在推荐结果集中
			if(m_it != map_vec_uid.end()){
				(*m_it).second.push_back(rel_uid);	// 桥梁加入桥梁集合
			}
			id_num ++;
			id = 0;
		}
		p_temp ++;
	}
	m_it = map_vec_uid.find(id);
	if(m_it != map_vec_uid.end()){
		(*m_it).second.push_back(rel_uid);
	}
	id_num ++;
}

//获取关注列表 from redis
int TargetingWorkInterface::get_follow_uids(uint64_t follow_uids[], uint32_t& follow_uid_num, uint32_t max, 
		__gnu_cxx::hash_set<uint64_t>& hashset_reco_uids, uint64_t uid){
	
	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("USER_GZ_DB");
	if (NULL == p_follow_db_interface)
		return -1;
	
	follow_uid_num = 0;
	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_follow_db_interface->get(0, uid, p_result, split_char, second_split_char);
	if (result <= 0){
		LOG_ERROR("get gz from redis error!");
		return result;
	}

	split_string_ids(follow_uids, follow_uid_num, hashset_reco_uids, 
			(const char* &)p_result, split_char, max);

	return 1;
}

// 获取关注列表 from open API
int TargetingWorkInterface::get_follow_uids_bak(uint64_t follow_uids[], uint32_t& follow_uid_num, uint32_t max,
		__gnu_cxx::hash_set<uint64_t>& hashset_reco_uids, const char* uid_str){

	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("OPEN_API_FOLLOW_DB");
	if (NULL == p_follow_db_interface){
		LOG_ERROR("api db is error!");
		return -1;
	}

	char uid_token_str[PATH_MAX];
	char* token_str = NULL;
	snprintf(uid_token_str, PATH_MAX, "%s:%s", uid_str, token_str);
	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_follow_db_interface->s_get(0, uid_token_str, p_result, split_char, second_split_char);
	if (result <= 0 || p_result == NULL){
		LOG_ERROR("get gz from api error!");
		return result;
	}
	//LOG_ERROR("%s", p_result);
	json_object* req_json = json_tokener_parse(p_result);
	if ( is_error(req_json) || NULL == req_json ){
		LOG_ERROR("%s", p_result);
		return 0;
	}

	json_object* follow_ids_json = json_object_object_get(req_json, "ids");
	if (NULL == follow_ids_json){
		json_object_put(follow_ids_json);
		return -1;
	}else{
		const char* follow_ids_str = NULL;
		follow_ids_str = (const char*)json_object_get_string(follow_ids_json);
		follow_uid_num = 0;
		//LOG_ERROR("%s", follow_ids_str);
		split_string_ids(follow_uids, follow_uid_num, hashset_reco_uids,
				(const char* &)follow_ids_str, split_char, max);
	}
	json_object_put(follow_ids_json);

	return 1;
}

//批量获取关注列表
int TargetingWorkInterface::mget_follow_uids(uint64_t mget_uids[], uint32_t get_uid_num, MapMResult& map_m_fof_result,
		char& split_char, char& second_split_char){
	
	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("USER_GZ_DB");
	if(NULL == p_follow_db_interface){
		return -1;
	}	

	int result = p_follow_db_interface->mget(0, mget_uids, get_uid_num, map_m_fof_result, 
			split_char, second_split_char);
	if (result <= 0){
		LOG_ERROR("mget gz from redis error!");
		return result;
	}

	return 1;
}

int TargetingWorkInterface::generate_output_str(const MAP_VEC_UID& map_vec_uid, int num, 
		char*& p_out_string, int& n_len){	
	n_len = 0;
	char* p_out_temp = p_out_string;
	int len = sprintf(p_out_temp, "%s", "{\"return_code\":\"200\",\"result\":[");
	p_out_temp += len;
	n_len += len;
	
	int index = 0;

	for(MAP_VEC_UID::const_iterator it = map_vec_uid.begin(); 
			it != map_vec_uid.end(); it++){
		uint64_t reco_uid = (*it).first;				// 推荐用户
		const VEC_UID& vec_uid = (*it).second;			// 桥梁集合

		if(vec_uid.size() >= 1)							// 若桥梁存在
		{
			if(index == 0){
				len = sprintf(p_out_temp, "{\"target_id\":%"PRIu64",\"extra\":[%"PRIu64"]}", 
						reco_uid, vec_uid[0]);
				}
			else{
				len = sprintf(p_out_temp, ",{\"target_id\":%"PRIu64",\"extra\":[%"PRIu64"]}",
						reco_uid, vec_uid[0]);
			}
			p_out_temp += len;
			n_len += len;

			//index ++;
			//if(index >= num)
			//	break;
		}else{											// 若桥梁不存在
			if(index == 0){
				len = sprintf(p_out_temp, "{\"target_id\":%"PRIu64",\"extra\":[]}", reco_uid);
			}else{
				len = sprintf(p_out_temp, ",{\"target_id\":%"PRIu64",\"extra\":[]}",reco_uid);
			}
			p_out_temp += len;
			n_len += len;
		}
		index ++;
	}	
	
	len = sprintf(p_out_temp, "%s", "]}");
	p_out_temp += len;
	n_len += len;

	return 1;
}

//处理即时数据
int TargetingWorkInterface::get_targeting_uids(const char* uid_str, __gnu_cxx::hash_set<uint64_t>& reco_uids, 
		uint64_t group_uids[], uint16_t group_uid_num,
		uint32_t max, uint16_t num, char*& p_out_string, int& n_out_len){
	if( NULL == uid_str || NULL == p_db_company_ )
		return -1;

	//int spend_msec = -1;
	//struct timeval tv_temp;
	//tv_temp = calc_spend_time(tv_temp, "start", spend_msec, true);
	
	uint64_t uid = strtoull(uid_str, NULL, 10);
	int result = 0;

	/*获取用户关注列表*/
	uint64_t follow_uids[MAX_FOLLOW_SIZE];
	uint32_t follow_uid_num = 0;
	result = get_follow_uids(follow_uids, follow_uid_num, max, reco_uids, uid);
	if(result < 0){
		result = get_follow_uids_bak(follow_uids, follow_uid_num, max, reco_uids, uid_str);
	}
	//tv_temp = calc_spend_time(tv_temp, "get follows", spend_msec);
	if(result < 0){
		return result;
	}
	/*获取关注列表结束*/

	/* 从推荐集中移除自身,构建MAP_VEC_UID */
	MAP_VEC_UID map_vec_uid;	// 以推荐用户为key，桥梁集合为value
	for(__gnu_cxx::hash_set<uint64_t>::iterator reco_it = reco_uids.begin();
			reco_it != reco_uids.end(); reco_it++){

		if((*reco_it) == uid){ // 删除自己
			continue;
		}
		VEC_UID vec_uid;
		map_vec_uid.insert(MAP_VEC_UID::value_type((*reco_it), vec_uid));
	}

	if(map_vec_uid.size() == 0){
		result = generate_output_str(map_vec_uid, num, p_out_string, n_out_len);
		//tv_temp = calc_spend_time(tv_temp, "all finish", spend_msec);

		return result;
	}

	/*批量获取关注的关注*/
	MapMResult map_m_fof_result;
	char split_char = 0;
	char second_split_char = 0;
	//新增需求
	if(group_uid_num > 0)		// 从分组uid中找桥梁，取分组uid的关注
		result = mget_follow_uids(group_uids, group_uid_num, map_m_fof_result,
					split_char, second_split_char);
	else						// 从关注uid中找桥梁，取关注uid的关注
		result = mget_follow_uids(follow_uids, follow_uid_num, map_m_fof_result,
					split_char, second_split_char);
	//map_m_fof_result.clear();
	//tv_temp = calc_spend_time(tv_temp, "get follows of follows", spend_msec);
	/*获取关注的关注结束*/	
	
	vector<uint64_t> vec_result;

	// 遍历每一个关注人的关注结果
	for(MapMResult::iterator o_it = map_m_fof_result.begin(); 
			o_it != map_m_fof_result.end(); o_it ++){
		const char* p_result = (*o_it).second;
		if(NULL == p_result)
			continue;
		split_string_ids_in_set(map_vec_uid, (*o_it).first, p_result, ',', max);
	}

	//tv_temp = calc_spend_time(tv_temp, "intersection finish", spend_msec);

	/*输出结果*/
	result = generate_output_str(map_vec_uid, num, p_out_string, n_out_len);

	map_m_fof_result.clear();
	release_map_vec_uid(map_vec_uid);
	//tv_temp = calc_spend_time(tv_temp, "all finish", spend_msec);

	return 1;

}

int TargetingWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "target_work start", spend_msec, true);

	int result = 0;

	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	
	//及时用户列表查询
	if(strcasecmp(cmd_str, "query") == 0){

		json_object* uid_json = json_object_object_get(req_json, "uid");
		char* uid_str = NULL;
		if(NULL == uid_json || is_error(uid_json)){
			return return_fail(403, "uid is empty", p_out_string, n_out_len);

		}else{
			uid_str = (char*)json_object_get_string(uid_json);
		}

		__gnu_cxx::hash_set<uint64_t> hash_reco_uid;
		json_object* uids_json = json_object_object_get(req_json, "reco_uids");
		if(NULL == uids_json || is_error(uids_json)){
			return return_fail(403, "reco_uids is empty", p_out_string, n_out_len);
		}else{
			if(json_object_is_type(uids_json, json_type_array)){
				int n_array_len = json_object_array_length(uids_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* uid_object = json_object_array_get_idx(uids_json, i);
					uint64_t reco_uid = json_object_get_int64(uid_object);

					hash_reco_uid.insert(reco_uid);
				}
			}
			else{
				return return_fail(403, "uids should be array", p_out_string, n_out_len);
			}
		}

		/*增加输入组列表的参数*/
		uint64_t group_uids[128];
		uint16_t group_uid_num = 0;
		json_object* group_uids_json = json_object_object_get(req_json, "group_uids");
		if(NULL == group_uids_json || is_error(group_uids_json)){
			LOG_DEBUG("group uids is empty!");
		}else{
			if(json_object_is_type(group_uids_json, json_type_array)){
				int n_array_len = json_object_array_length(group_uids_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* group_uid_object = json_object_array_get_idx(group_uids_json, i);
					if(NULL == group_uid_object || is_error(group_uid_object))
						continue;

					uint64_t group_uid = json_object_get_int64(group_uid_object);

					group_uids[group_uid_num] = group_uid;
					group_uid_num ++;
				}
			}
			else{
				LOG_DEBUG("group uids is empty and not array!");
			}
		}

		/*end*/

		uint32_t max = MAX_LIMIT;
		json_object* max_json = json_object_object_get(req_json, "max");
		if(NULL == max_json || is_error(max_json)){
			max = MAX_LIMIT;
		}else{
			max = (uint32_t)json_object_get_int(max_json);
		}

		uint16_t num = 200;
		json_object* num_json = json_object_object_get(req_json, "num");
		if(NULL == num_json || is_error(num_json)){
			num = 200;
		}else{
			num = (uint16_t)json_object_get_int(num_json);
		}

		result = get_targeting_uids(uid_str, hash_reco_uid, group_uids, group_uid_num,
				max, num, p_out_string, n_out_len);

	}
	tv_temp = calc_spend_time(tv_temp, "target_work all finish", spend_msec);
	return result;
}
