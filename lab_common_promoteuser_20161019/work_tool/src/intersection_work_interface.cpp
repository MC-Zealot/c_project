#include "intersection_work_interface.h"

DYN_WORK(IntersectionWorkInterface);

//获取关注列表 from redis
int IntersectionWorkInterface::get_follow_uids(const uint64_t uid, __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, 
		uint32_t& follow_uid_num){
	
	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("USER_GZ_DB");
	if (NULL == p_follow_db_interface){
		LOG_ERROR("gz redis db_interface error!");
		return -3;
	}

	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_follow_db_interface->get(0, uid, p_result, split_char, second_split_char);
	if (result <= 0 ){
		LOG_ERROR("get gz from redis error!");
		return -2;
	}
	if (NULL == p_result){
		LOG_ERROR("redis gz data is NULL!");
		return -1;
	}

	follow_uid_num = 0;
	split_string_set_ids(hashset_follow_uids, follow_uid_num, (const char* &)p_result, split_char, 0);

	return 1;
}

// 获取关注列表 from open API
int IntersectionWorkInterface::get_follow_uids_bak(const char* uid_str, 
		__gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, uint32_t& follow_uid_num){

	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("OPEN_API_FOLLOW_DB");
	if (NULL == p_follow_db_interface){
		LOG_ERROR("gz api db_interface error!");
		return -3;
	}

	char uid_token_str[PATH_MAX];
	char* token_str = NULL;
	snprintf(uid_token_str, PATH_MAX, "%s:%s", uid_str, token_str);
	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_follow_db_interface->s_get(0, uid_token_str, p_result, split_char, second_split_char);
	if (result <= 0 ){
		LOG_ERROR("get gz from api error!");
		return -2;
	}
	if (NULL == p_result){
		LOG_ERROR("api gz data is NULL!");
		return -1;
	}

	// json解析平台取到的关注数据
	json_object* req_json = json_tokener_parse(p_result);		// 结果解析成json对象
	if ( is_error(req_json) || NULL == req_json ){
		LOG_ERROR("json parse error! result: %s", p_result);
		json_object_put(req_json);
		return -1;
	}

	json_object* follow_ids_json = json_object_object_get(req_json, "ids");		// 获取id字段
	if (NULL == follow_ids_json){
		LOG_ERROR("json parse error!");
		json_object_put(req_json);
		return -1;
	}else{
		const char* follow_ids_str = NULL;
		follow_ids_str = (const char*)json_object_get_string(follow_ids_json);
		follow_uid_num = 0;
		split_string_set_ids(hashset_follow_uids, follow_uid_num, (const char* &)follow_ids_str, split_char, 0);
	}
	
	json_object_put(req_json);
	return 1;
}

// set集合与db求交，保存到vec中
int IntersectionWorkInterface::intersection_uids(const __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, 
		const char* db_name, vector<uint64_t>& vec_intersection){

	// 连接全局数据库
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	GlobalDbInterface* global_db_interface = NULL;
	if(p_global_db_company != NULL){
		global_db_interface = p_global_db_company->get_global_db_interface(db_name);
	}
	else{
		global_db_interface = NULL;
	}
	if(global_db_interface == NULL){
		LOG_DEBUG("global db_interface error!");
		return -3;
	}

	for(__gnu_cxx::hash_set<uint64_t>::const_iterator it = hashset_follow_uids.begin();
			it != hashset_follow_uids.end(); it++){
		uint64_t uid = (*it);
		if(global_db_interface->is_exist(uid)){
			vec_intersection.push_back(uid);
		}
	}

	return 1;
}

// set转成vec
void IntersectionWorkInterface::hashset_to_vector(const __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, 
		vector<uint64_t>& vec_follows){
	for(__gnu_cxx::hash_set<uint64_t>::const_iterator it = hashset_follow_uids.begin();
			it != hashset_follow_uids.end(); it++){
		vec_follows.push_back(*it);
	}
}

// vec转成str，由type决定字符串样式
int IntersectionWorkInterface::vector_to_str(const vector<uint64_t>& vec, const int type, char*& out_str){
	// 根据type值将vector转成字符串，形如 "all_fol":[id1,id2,...,idn]
	int len = 0;
	if(type == 1){					// 用户关注的所有用户
		len = sprintf(out_str, "%s", "\"all_fol\":[");
	}else if(type == 2){			// 用户关注的广告主
		len = sprintf(out_str, "%s", "\"result\":[");
	}else if(type == 4){            // 用户关注的所有活跃用户
		len = sprintf(out_str, "%s", "\"follow\":[");
	}
	out_str += len;

	int len_total = len;
	int index = 0;					// 用户控制格式
	for(vector<uint64_t>::const_iterator it = vec.begin(); it != vec.end(); it++){
		if(index == 0){				// 第一个uid前不加逗号
			len = sprintf(out_str, "%"PRIu64, (*it));
		}else{
			len = sprintf(out_str, ",%"PRIu64, (*it));
		}
		index ++;
		out_str += len;
		len_total += len;
	}

	len = sprintf(out_str, "%s", "]");
	out_str += len;
	len_total += len;
	
	return len_total;
}

// 生成结果字符串，由type决定哪个集合
void IntersectionWorkInterface::generate_output_str(const vector<uint64_t>& vec_result, const int type, 
		char*& p_out_string, int& n_len){	
	// 返回一个结果，由type值确定
	n_len = 0;
	char* p_out_temp = p_out_string;
	int len = sprintf(p_out_temp, "%s", "{\"return_code\":\"ok\",");
	p_out_temp += len;
	n_len += len;

	n_len += vector_to_str(vec_result, type, p_out_temp);
	
	len = sprintf(p_out_temp, "%s", "}");
	p_out_temp += len;
	n_len += len;
}

void IntersectionWorkInterface::generate_output_str(const vector<uint64_t>& vec1, const vector<uint64_t>& vec2, 
		const int type, char*& p_out_string, int& n_len){
	// 返回两个结果
	n_len = 0;
	char* p_out_temp = p_out_string;
	int len = sprintf(p_out_temp, "%s", "{\"return_code\":\"ok\",");
	p_out_temp += len;
	n_len += len;
	
	if(type == 3){
		n_len += vector_to_str(vec1, 1, p_out_temp);	// 生成用户的所有关注
		len = sprintf(p_out_temp, "%s", ",");
		p_out_temp += len;
		n_len += len;
		n_len += vector_to_str(vec2, 2, p_out_temp);	// 生成用户关注的所有广告主
	}else if(type == 5){
		n_len += vector_to_str(vec1, 1, p_out_temp);
		len = sprintf(p_out_temp, "%s", ",");
		p_out_temp += len;
		n_len += len;
		n_len += vector_to_str(vec2, 4, p_out_temp);	// 生成用户关注的所有活跃用户
	}else if(type == 6){
		n_len += vector_to_str(vec1, 2, p_out_temp);
		len = sprintf(p_out_temp, "%s", ",");
		p_out_temp += len;
		n_len += len;
		n_len += vector_to_str(vec2, 4, p_out_temp);
	}
	
	len = sprintf(p_out_temp, "%s", "}");
	p_out_temp += len;
	n_len += len;
}

void IntersectionWorkInterface::generate_output_str(const vector<uint64_t>& vec_allfol, 
		const vector<uint64_t>& vec_result, const vector<uint64_t>& vec_good, 
		const int type, char*& p_out_string, int& n_len){	
	// 返回三个结果
	n_len = 0;
	char* p_out_temp = p_out_string;
	int len = sprintf(p_out_temp, "%s", "{\"return_code\":\"ok\",");
	p_out_temp += len;
	n_len += len;
	
	n_len += vector_to_str(vec_allfol, 1, p_out_temp);
	len = sprintf(p_out_temp, "%s", ",");
	p_out_temp += len;
	n_len += len;
	n_len += vector_to_str(vec_result, 2, p_out_temp);
	len = sprintf(p_out_temp, "%s", ",");
	p_out_temp = p_out_temp + len;
	n_len += len;
	n_len += vector_to_str(vec_good, 4, p_out_temp);
	
	len = sprintf(p_out_temp, "%s", "}");
	p_out_temp += len;
	n_len += len;
}

// 获取即时查询业务，提供reco_ids
int IntersectionWorkInterface::get_intime_uids(const char* uid_str, const __gnu_cxx::hash_set<uint64_t>& reco_uids, 
		char*& p_out_string, int& n_out_len){
	if( NULL == uid_str || NULL == p_db_company_ ){
		return -3;
	}

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get_intime uid start", spend_msec, true);
	
	uint64_t uid = strtoull(uid_str, NULL, 10);
	int result = 0;

	/*获取用户关注列表*/
	 __gnu_cxx::hash_set<uint64_t> follow_uids;
	uint32_t follow_uid_num = 0;
	result = get_follow_uids(uid, follow_uids, follow_uid_num);
	if(result < 0){
		result = get_follow_uids_bak(uid_str, follow_uids, follow_uid_num);
	}
	//tv_temp = calc_spend_time(tv_temp, "get follows", spend_msec);
	if(result < 0){
		follow_uids.clear();
		return -2;
	}
	/*获取关注列表结束*/
	
	/*关注用户与活跃用户相交运算*/
	vector<uint64_t> vec_good_result;
	result = intersection_uids(follow_uids, "ACTIVE_USER_DB", vec_good_result);
	//tv_temp = calc_spend_time(tv_temp, "good user intersection finish", spend_msec);
	if(result < 0){
		follow_uids.clear();
		vec_good_result.clear();
		return -2;
	}

	/*活跃用户与指定用户相交运算*/
	vector<uint64_t> vec_result;
	for(__gnu_cxx::hash_set<uint64_t>::iterator it = follow_uids.begin(); it != follow_uids.end(); it++){
		uint64_t reco_uid = (*it);
		__gnu_cxx::hash_set<uint64_t>::const_iterator c_it = reco_uids.find(reco_uid);
		if(c_it != reco_uids.end()){
			vec_result.push_back(reco_uid);
		}
	}
	//tv_temp = calc_spend_time(tv_temp, "intersection finish", spend_msec);

	/*输出结果*/
	int type = 6;
	generate_output_str(vec_result, vec_good_result, type, p_out_string, n_out_len);
	tv_temp = calc_spend_time(tv_temp, "intime all finish", spend_msec);

	follow_uids.clear();
	vec_good_result.clear();
	vec_result.clear();
	return 1;

}

//功能处理模块
int IntersectionWorkInterface::get_intersection_uids(const char* uid_str, const int type, char*& p_out_string, 
		int& n_out_len){
	if( NULL == uid_str || NULL == p_db_company_ )
		return -3;

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get_intersection start", spend_msec, true);
	
	uint64_t uid = strtoull(uid_str, NULL, 10);
	int result = 0;

	/*获取用户关注列表*/
	 __gnu_cxx::hash_set<uint64_t> follow_uids;
	uint32_t follow_uid_num = 0;
	result = get_follow_uids(uid, follow_uids, follow_uid_num);
	if (result < 0){										// 若从redis取不到数据，从平台取
		result = get_follow_uids_bak(uid_str, follow_uids, follow_uid_num);
	}
	//tv_temp = calc_spend_time(tv_temp, "get follows", spend_msec);
	if(result < 0){
		follow_uids.clear();
		return -2;
	}
	vector<uint64_t> vec_allfollows;						// 用户的所有关注
	hashset_to_vector(follow_uids, vec_allfollows);			// 所有关注列表转成vector格式
	if(type == 1){											// 返回用户的所有关注
		generate_output_str(vec_allfollows, type, p_out_string, n_out_len);
		follow_uids.clear();
		vec_allfollows.clear();
		return 1;
	}
	/*获取关注列表结束*/

	/*进行优质用户相交运算*/
	vector<uint64_t> vec_good_result;						// 用户关注的活跃用户
	result = intersection_uids(follow_uids, "ACTIVE_USER_DB", vec_good_result);
	//tv_temp = calc_spend_time(tv_temp, "good user intersection finish", spend_msec);
	if(result < 0){
		follow_uids.clear();
		vec_allfollows.clear();
		vec_good_result.clear();
		return -2;
	}
	
	if(type == 4){			// 返回用户关注的所有活跃用户
		generate_output_str(vec_good_result, type, p_out_string, n_out_len);
		follow_uids.clear();
		vec_allfollows.clear();
		vec_good_result.clear();
		return 1;
	}else if(type == 5){	// 返回用户的所有关注 + 用户关注的所有活跃用户
		generate_output_str(vec_allfollows, vec_good_result, type, p_out_string, n_out_len);
		follow_uids.clear();
		vec_allfollows.clear();
		vec_good_result.clear();
		return 1;
	}

	/*进行广告主相交运算*/
	vector<uint64_t> vec_result;							// 用户关注的广告主
	result = intersection_uids(follow_uids, "INTERSECTION_USER_DB", vec_result);
	//tv_temp = calc_spend_time(tv_temp, "aders intersection finish", spend_msec);
	if(result < 0){
		follow_uids.clear();
		vec_allfollows.clear();
		vec_good_result.clear();
		vec_result.clear();
		return -2;
	}
	switch(type){
		case 2:				// 返回用户关注的广告主
			generate_output_str(vec_result, type, p_out_string, n_out_len);
			break;
		case 3:				// 返回用户所有关注 + 用户关注的广告主
			generate_output_str(vec_allfollows, vec_result, type, p_out_string, n_out_len);
			break;
		case 6:{			// 返回用户关注的广告主 + 用户关注的活跃用户
			vector<uint64_t> vec_follow(vec_result);		// 20140904修改，将二者结果合并到follow字段
			vec_follow.insert(vec_follow.end(), vec_good_result.begin(), vec_good_result.end());
			sort(vec_follow.begin(), vec_follow.end());
			vector<uint64_t>::iterator it_end = unique(vec_follow.begin(), vec_follow.end());
			vec_follow.erase(it_end, vec_follow.end());
			generate_output_str(vec_good_result, vec_follow, type, p_out_string, n_out_len);
			break;
		}case 7:			// 返回用户关注的所有用户 + 用户关注的广告主 + 用户关注的活跃用户
			generate_output_str(vec_allfollows, vec_result, vec_good_result, type, p_out_string, n_out_len);
			break;
		default:			// 默认情况返回用户关注的广告主 + 用户关注的活跃用户
			generate_output_str(vec_result, vec_good_result, type, p_out_string, n_out_len);
			break;
	}
	tv_temp = calc_spend_time(tv_temp, "intersection all finish", spend_msec);
	
	follow_uids.clear();
	vec_allfollows.clear();
	vec_good_result.clear();
	vec_result.clear();
	return 1;
}

int IntersectionWorkInterface::work_core(json_object *req_json, char* &p_out_string, 
		int& n_out_len, uint64_t req_id){
	int spend_msec = -1;
    struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "aders work start", spend_msec, true);
	int result = 0;

	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = "query";
	if(NULL != cmd_json){							// 默认值为query
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	
	// 广告主相交业务
	if(strcasecmp(cmd_str, "query") == 0){
		json_object* uid_json = json_object_object_get(req_json, "uid");
		char* uid_str = NULL;
		if(NULL == uid_json){
			uid_str = "";
			return return_fail(403, "uid is empty", p_out_string, n_out_len);
		}else{
			uid_str = (char*)json_object_get_string(uid_json);
		}

		json_object* type_json = json_object_object_get(req_json,"type");
		int type = 6;									// 参数中的type值默认为6
		if ( NULL != type_json ){
			type = json_object_get_int(type_json); 
			if( type < 1 || type > 7 ) type = 6;		// 对不合法输入，使用默认值
		}
		result = get_intersection_uids(uid_str, type, p_out_string, n_out_len);
	}
	// 即时用户列表查询
	else if(strcasecmp(cmd_str, "intime") == 0){
		json_object* uid_json = json_object_object_get(req_json, "uid");
		char* uid_str = NULL;
		if(NULL == uid_json){
			uid_str = "";
			return return_fail(403, "uid is empty", p_out_string, n_out_len);
		}else{
			uid_str = (char*)json_object_get_string(uid_json);
		}

		__gnu_cxx::hash_set<uint64_t> hash_reco_uid;
		json_object* uids_json = json_object_object_get(req_json, "reco_uids");
		if(NULL == uids_json){
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
		result = get_intime_uids(uid_str, hash_reco_uid, p_out_string, n_out_len);
	}
	tv_temp = calc_spend_time(tv_temp, "aders_work all finish", spend_msec);	
	return result;
}
