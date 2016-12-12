#include "recentlygz_work_interface.h"

DYN_WORK(RecentlyGZWorkInterface);

//获取关注列表
int RecentlyGZWorkInterface::get_follow_uids(const uint64_t uid, __gnu_cxx::hash_set<uint64_t>& hashset_follow_uids, 
		uint32_t& follow_uid_num){
	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("USER_GZ_DB");
	if (NULL == p_follow_db_interface){
		LOG_ERROR("follow db_interface is NULL!");
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
		LOG_ERROR("get gz from redis fail!");
		return -1;
	}

	follow_uid_num = 0;
	split_string_set_ids(hashset_follow_uids, follow_uid_num, (const char* &)p_result, split_char, 0);

	return 1;
}

// 与指定用户求交集，最后一个参数为true时，表示保留交集，为false时，表示保留差集
int RecentlyGZWorkInterface::get_intersection(const __gnu_cxx::hash_set<uint64_t>& src_set, 
		const char* db_name, bool cull, __gnu_cxx::hash_set<uint64_t>& des_set){	
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	GlobalDbInterface* global_db_interface = NULL;
	if(p_global_db_company != NULL){
		global_db_interface = p_global_db_company->get_global_db_interface(db_name);
	}
	else{
		global_db_interface = NULL;
	}

	if(global_db_interface == NULL){
		LOG_DEBUG("%s db_interface is NULL!", db_name);
		return -3;
	}

	for(__gnu_cxx::hash_set<uint64_t>::const_iterator it = src_set.begin();
			it != src_set.end(); it++){
		uint64_t uid = (*it);
		if (!cull){
			if(!(global_db_interface->is_exist(uid)))			// 不在集合中，保留
				des_set.insert(uid);
		}else{
			if(global_db_interface->is_exist(uid))				// 在集合中，保留
				des_set.insert(uid);
		}
	}
	return 1;
}

// 查询用户V类型
int RecentlyGZWorkInterface::get_v_type(const uint64_t uid, uint32_t& type){	
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	GlobalDbInterface* global_db_interface = NULL;
	if(p_global_db_company != NULL){							// 基类指针转为派生类指针
		global_db_interface = p_global_db_company->get_global_db_interface("V_USER_DB");
	}else{
		global_db_interface = NULL;
	}
	if(global_db_interface == NULL){
		LOG_DEBUG("VUSER db_interface is NULL!");
		return -1;
	}
	GlobalKVDbInterface* v_ptr = (GlobalKVDbInterface*)global_db_interface;
	if(v_ptr == NULL){
		LOG_ERROR("conversion from GlobalDbInterface* to GlobalKVDbInterface* failure!");
	}
	if (!(v_ptr->get_value(uid, type))) type = 0;			// 非V用户，type为0
	return 1;
}

//批量获取最近关注列表
int RecentlyGZWorkInterface::mget_follow_uids(const uint64_t uids[], const uint32_t get_uid_num, 
		MapMResult& map_m_fof_result, char& split_char, char& second_split_char){	
	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("GZ_RECENT_GZ_DB");
	if(NULL == p_follow_db_interface){
		LOG_ERROR("recent_gz db_interface is NULL!");
		return -3;
	}

	int result = p_follow_db_interface->mget(0, const_cast<uint64_t*>(uids), get_uid_num, 
			map_m_fof_result, split_char, second_split_char);
	if (result <= 0){
		LOG_ERROR("mget recent_gz from redis error!");
		return -2;
	}
	
	return 1;

}

// 将字符串按照分隔符解析成字符串数组
int RecentlyGZWorkInterface::split_string_to_array(const char* str_input, const char& sep, 
		char str_out[][MAX_GZ_LENGTH]){
	const char* p_temp = str_input;
	int num = 0, index = 0;
	while((*p_temp) != '\0'){
		if (*p_temp == sep){
			str_out[num][index] = '\0';
			num ++;
			index = 0;
		}else{
			str_out[num][index++] = *p_temp;
		}
		p_temp ++;
	}
	str_out[num][index] = '\0';
	num ++;
	return num;
}

//构建关系用户列表
int RecentlyGZWorkInterface::construct_user_data(const MapMResult& map_m_fof_result, const char split_char, 
		const char second_split_char, HashMapGZResult& gz_dict, __gnu_cxx::hash_set<uint64_t>& recoid_set){
	// 输入：MapMResult是一个字典，{uid, 关注信息}
	// 输出：gz_dict是一个字典，{reco_id,{num,bridge_vec}}，保存MapMResult解析后的结果
	// 输出：recoid_set是一个集合，存储所有的推荐id，为了方便后续计算
	for(MapMResult::const_iterator it = map_m_fof_result.begin();	 
			it != map_m_fof_result.end(); it++ ){
		uint64_t bridge_id = it->first;								// 桥梁用户
		const char* res_str = it->second;							// 桥梁用户最近关注信息

		/*解析关注信息*/
		char tmp_str[GZ_NUM][MAX_GZ_LENGTH];						// 存储桥梁用户的关注信息
		// 将每天关注信息按照第一个分隔符解析
		int len = split_string_to_array(res_str, split_char, tmp_str);	
		for (int i = 0; i < len; i++){
			char out_str[INFO_PART][MAX_GZ_LENGTH];					// 将每条关注信息分割
			// 按照第二个分隔符解析
			int length = split_string_to_array(tmp_str[i], second_split_char, out_str);
			if (length != INFO_PART){								// 若信息有误，跳过
				LOG_DEBUG("recently gz data error!");
				continue;
			}
			
			uint64_t wr_time = strtoull(out_str[0], NULL, 10);		// 第一个数组存储时间戳	
			time_t cur_time;                                    	// 获取系统当前时间
			time(&cur_time);
			if (cur_time - wr_time > TIMESTAMP){             		// 若加关注时间在七天之前，跳过
				continue;
			}

			uint64_t gz_id = strtoull(out_str[3], NULL, 10);		// 第四个数组存储关注id

			HashMapGZResult::iterator map_it = gz_dict.find(gz_id);
			if(map_it != gz_dict.end()){							// 关注id在结果字典中
				BridgeStr* bridge_ptr = map_it->second;
				(bridge_ptr->num_) ++;								// 桥梁数目增加
				(bridge_ptr->bridge_uid_vec).push_back(bridge_id);	// 加入新的桥梁用户
				if (bridge_ptr->time_ < wr_time){					// 更新加关注时间
					bridge_ptr->time_ = wr_time;
				}
				if (bridge_ptr->num_ >= BRIDGE_NUM){				// 满足桥梁数目条件的推荐用户
					recoid_set.insert(gz_id);
				}
			}
			else{													// 关注id不在结果字典中
				BridgeStr* bridge_ptr = new BridgeStr(bridge_id, wr_time);
				gz_dict[gz_id] = bridge_ptr;						// 关注id添加到字典中
				//recoid_set.insert(gz_id);							// 仅在BRIDGE_NUM=1时需要
			}
		}	
	}
	return 1;
}


//计算推荐结果
int RecentlyGZWorkInterface::calc_recom_result(const __gnu_cxx::hash_set<uint64_t>& reco_ids, 
		const HashMapGZResult& reco_dict, const __gnu_cxx::hash_set<uint64_t>& gz_ids, 
		vector<RecomInfo>& result_vec){
	
	/*提取推荐用户集合中的优质用户，过滤黑名单用户*/
	__gnu_cxx::hash_set<uint64_t> good_ids;
	int result = get_intersection(reco_ids, "GOOD_USER_DB", true, good_ids);
	if (result < 0){											// 优质用户过滤失败则不推荐
		LOG_DEBUG("good user intersection error!");
		return -1;												
	}
	__gnu_cxx::hash_set<uint64_t> res_ids;
	result = get_intersection(good_ids, "BLACK_USER_DB", false, res_ids);
	if(result < 0){												// 黑名单过滤失败则不推荐
		LOG_DEBUG("black user intersection error!");
		return -1;
	}
	/*提取优质推荐用户结束*/

	/*过滤优质推荐用户中当前用户已关注的*/
	for (__gnu_cxx::hash_set<uint64_t>::const_iterator it = res_ids.begin(); it != res_ids.end(); it++){
		uint64_t reco_id = *it;
		if(gz_ids.find(reco_id) == gz_ids.end()){				// 推荐用户未被关注，保存到结果列表中
			HashMapGZResult::const_iterator gz_it = reco_dict.find(reco_id);
			if(gz_it != reco_dict.end()){
				//result_vec.push_back(make_pair(reco_id, gz_it->second));
				uint32_t v_type;								// 推荐用户类型
				int res = get_v_type(reco_id, v_type);	
				if(res < 0) v_type = 0;							// 获取失败默认为普户
				if(v_type != 3){								// 过滤企业蓝V
					if(v_type >= 2) v_type = 2;					// 蓝V全部标识为2
					// 构建RecomInfo类对象
					RecomInfo recomInfo(reco_id, gz_it->second, v_type);
					result_vec.push_back(recomInfo);
				}	
			}
		}
	}
	sort(result_vec.begin(), result_vec.end(), cmp_RecomInfo);	// 按照桥梁数目和关注时间排序

	return 1;
}

// 将vector转成字符串，形如 "recent_gz_follow":[id1,id2,...,idn]
int RecentlyGZWorkInterface::vector_to_str(const vector<uint64_t>& vec, char*& out_str){
	int len = sprintf(out_str, "%s", "\"recent_gz_follow\":[");
	out_str += len;	
	int len_total = len;
	int i = 0;
	for(vector<uint64_t>::const_iterator it = vec.begin(); it != vec.end(); it++){
		if(i == 0){
			len = sprintf(out_str, "%"PRIu64, (*it));
		}else{
			len = sprintf(out_str, ",%"PRIu64, (*it));
		}
		i++;
		out_str += len;
		len_total += len;
	}
	len = sprintf(out_str, "%s", "]");
	out_str += len;
	len_total += len;
	return len_total;
}

// 将RecomInfo转成字符串，形如 [uid,{"recent_gz_num":num,"recent_gz_follow":[id1,id2,...,idn],"user_type":0}]
int RecentlyGZWorkInterface::recomInfo_to_str(const RecomInfo& info, char*& out_str){
	uint64_t reco_id = info.reco_id_;
	BridgeStr* bridge_ptr = info.bridgeInfo_;
	uint32_t v_type = info.vtype_;
		
	int len = sprintf(out_str, "[%"PRIu64, reco_id);				// 写入推荐用户id
	out_str += len;
	int len_total = len;
	
	len = sprintf(out_str, ", {\"recent_gz_num\":%"PRIu16", ", bridge_ptr->num_);	// 写入桥梁数目
	out_str += len;
	len_total += len;
	
	len_total += vector_to_str(bridge_ptr->bridge_uid_vec, out_str);// 写入桥梁关系

	len = sprintf(out_str, ", \"user_type\":%"PRIu32, v_type);		// 写入用户V类型
	out_str += len;
	len_total += len;

	len = sprintf(out_str, "%s", "}]");
	out_str += len;
	len_total += len;

	return len_total;

}
//输出推荐结果
void RecentlyGZWorkInterface::generate_output_str(const vector<RecomInfo>& res_vec, char*& out_str, int& n_len){
	n_len = 0;
	char* p_out_tmp = out_str;
	int len = sprintf(p_out_tmp, "%s", "{\"return_code\":\"ok\",\"result\":[");
	p_out_tmp +=  len;
	n_len += len;

	int i = 0;	
	for(vector<RecomInfo>::const_iterator it = res_vec.begin(); it != res_vec.end(); it++){
		if (i ==0){
			n_len += recomInfo_to_str((*it), p_out_tmp);
		}else{
			len = sprintf(p_out_tmp, "%s", ",");
			p_out_tmp += len;
			n_len += len;
			n_len += recomInfo_to_str((*it), p_out_tmp);
		}
		i ++;
		if (i >= MAX_RESULT_NUM) break;						// 截断，仅返回MAX_RESULT_NUM个结果
	}

	len = sprintf(p_out_tmp, "%s", "]}");
	p_out_tmp += len;
	n_len += len;
}

	
//功能处理模块
int RecentlyGZWorkInterface::get_recom_uids(const char* uid_str, char*& p_out_string, int& n_out_len){
	if( NULL == uid_str || NULL == p_db_company_ ){
		return -3;
	}

	//int spend_msec = -1;
	//struct timeval tv_temp;
	//tv_temp = calc_spend_time(tv_temp, "start", spend_msec, true);
	uint64_t uid = strtoull(uid_str, NULL, 10);
	int result = 0;

	/*获取用户关注列表*/
	__gnu_cxx::hash_set<uint64_t> follow_set;
	uint32_t follow_uid_num = 0;
	result = get_follow_uids(uid, follow_set, follow_uid_num);
	//tv_temp = calc_spend_time(tv_temp, "get follows", spend_msec);
	if(result <= 0){						// 获取关注信息失败，直接返回
		follow_set.clear();
		return -2;
	}
	/*获取关注列表结束*/

	/*提取用户关注列表中的优质用户*/
	__gnu_cxx::hash_set<uint64_t> good_set;
	result = get_intersection(follow_set, "GOOD_USER_DB", true,  good_set);
	//tv_temp = calc_spend_time(tv_temp, "good user intersection finish", spend_msec);
	if(result <= 0 || good_set.empty()){	// 获取关注的优质用户信息失败，不做优质用户过滤
		LOG_ERROR("get gz of good user fail!");
		good_set = follow_set;
	}
	/*提取优质用户结束*/

	/*获取优质用户关注的最近关注数据*/
	uint64_t good_users[MAX_GZ_NUM];		// 将关注的优质用户转成数组
	int len = 0;
	for (__gnu_cxx::hash_set<uint64_t>::const_iterator it = good_set.begin(); 
			it != good_set.end(); it++ ){
		good_users[len++] = *it;
	}
	good_set.clear();

	MapMResult map_m_result;				// 结果保存到一个字典中，map<uint64_t, const char*>
	char split_char, second_split_char;
	result = mget_follow_uids(good_users, len, map_m_result, split_char, second_split_char);
	//tv_temp = calc_spend_time(tv_temp, "mget follows", spend_msec);
	if(result <= 0){
		follow_set.clear();
		release_map_m_result(map_m_result);
		return -2;
	}
	/*获取关注的最近关注结束*/
	
	/*对获取的数据进行处理，并构建字典和集合*/
	HashMapGZResult recom_dict;
	__gnu_cxx::hash_set<uint64_t> recom_set;
	result = construct_user_data(map_m_result, split_char, second_split_char, recom_dict, recom_set);
	//if (recom_set.erase(uid)){				// 删除用户自身，返回删除元素的数量
	//	LOG_DEBUG("removed user himself!");
	//}
	recom_set.erase(uid);
	/*构建字典和集合完成*/

	/*生成推荐用户*/
	vector<RecomInfo> result_vec;		
	result = calc_recom_result(recom_set, recom_dict, follow_set, result_vec);
	/*生成推荐用户结束*/
	//tv_temp = calc_spend_time(tv_temp, "calc gz finish", spend_msec);

	/*输出结果*/
	generate_output_str(result_vec, p_out_string, n_out_len);
	
	follow_set.clear();
	recom_set.clear();
	result_vec.clear();						// 在recom_dict之前清空
	release_map_m_result(map_m_result);
	release_hashmap_gz_result(recom_dict);	// 包含new出来的内存，需要释放内存后再清空
	
	//tv_temp = calc_spend_time(tv_temp, "all finish", spend_msec);

	return 1;
}

int RecentlyGZWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "recently_work start", spend_msec, true);
	
	int result = 0;
	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}

	if(strcasecmp(cmd_str, "query") == 0){

		json_object* uid_json = json_object_object_get(req_json, "uid");
		char* uid_str = NULL;
		if(NULL == uid_json){
			uid_str = "";
			return return_fail(403, "uid is empty", p_out_string, n_out_len);
		}else{
			uid_str = (char*)json_object_get_string(uid_json);
		}
		result = get_recom_uids(uid_str, p_out_string, n_out_len);
		uid_str = NULL;
	}
	tv_temp = calc_spend_time(tv_temp, "recently_work all finish", spend_msec);
	return result;
}
