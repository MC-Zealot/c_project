#include "test_so_work_interface.h"

DYN_WORK(TestSoWorkInterface);
int get_filter_api_set(char* p_filter_api_result, char sep_char, char second_sep_char, const char* first_str,
		__gnu_cxx::hash_set<uint64_t>& hashset_filter_uids, int num = -1){

	if(NULL == p_filter_api_result)
		return -1;

	char* p_temp = p_filter_api_result;

	char temp_arr[COMPRESS_LEN] = {0};
	int temp_num = 0;
	bool item_flag = false;
	uint64_t id = 0;
	int index = 0;
	while(*p_temp != '\0'){
		if(num != -1 && index >= num)
			break;

		if(*p_temp == sep_char){
			item_flag = false;

			if(id != 0){
				index ++;
				hashset_filter_uids.insert(id);
			}
			id = 0;
			temp_arr[0] = '\0';
			temp_num = 0;
		}else{
			if(!item_flag){
				temp_arr[temp_num] = *p_temp;
				temp_num ++;
				temp_arr[temp_num]= '\0';

				if(strcmp(temp_arr, first_str) == 0){
					item_flag = true;
				}
			}else{
				if(*p_temp >= '0' && *p_temp <= '9')
					id = id * 10 + *p_temp - '0';
			}
		}
			
		p_temp ++;
	}

	//lookis to add last one

	return 1;
}

int get_filter_set(char* p_filter_result, char sep_char, char second_sep_char, const char* first_str,
		__gnu_cxx::hash_set<uint64_t>& hashset_filter_uids, int num = -1){

	if(NULL == p_filter_result)
		return -1;

	char* p_temp = p_filter_result;

	uint8_t first_sep_index = 0;//0, 1, 2
	uint64_t id = 0;
	uint16_t type = 0;
	int index = 0;
	while(*p_temp != '\0'){
		if(num != -1 && index >= num)
			break;

		if(*p_temp == sep_char){
			first_sep_index = (first_sep_index % 2) + 1;

		}else if(*p_temp == second_sep_char){
			if(first_sep_index == 2){
				if(type == 24 || type == 19 || type == 35 || type == 34){
					index ++;

					hashset_filter_uids.insert(id);

					id = 0;
				}
				type = 0;
				first_sep_index = 0;
			}
		}else{
			if(first_sep_index == 1){
				if(*p_temp >= '0' && *p_temp <= '9')
					type = type * 10 + *p_temp - '0';
			}if(first_sep_index == 2){
				if(type == 24 || type == 19 || type == 35 || type == 34){
					if(*p_temp >= '0' && *p_temp <= '9')
						id = id * 10 + *p_temp - '0';
				}
			}
		}
		p_temp ++;
	}

	//lookis to add last one

	return 1;
}

int get_isread_id_set(char* p_isread_result, char sep_char, char second_sep_char, const char* first_str,
		__gnu_cxx::hash_set<uint64_t>& hashset_isread_ids, int num = -1){

	if(NULL == p_isread_result)
		return -1;

	char* p_temp = p_isread_result;

	uint8_t first_sep_index = 0;//0, 1, 2
	uint64_t id = 0;
	uint16_t type = 0;
	int index = 0;
	while(*p_temp != '\0'){
		if(num != -1 && index >= num)
			break;

		if(*p_temp == sep_char){
			first_sep_index = (first_sep_index % 2) + 1;

		}else if(*p_temp == second_sep_char){
			if(first_sep_index == 2){

				if(type == 1){
					index ++;

					hashset_isread_ids.insert(id);

					id = 0;
				}
				type = 0;
				first_sep_index = 0;
			}
		}else{
			if(first_sep_index == 1){
				if(*p_temp >= '0' && *p_temp <= '9')
					id = id * 10 + *p_temp - '0';
			}if(first_sep_index == 2){
				if(*p_temp >= '0' && *p_temp <= '9')
					type = type * 10 + *p_temp - '0';
			}
		}
		p_temp ++;
	}

	if(type == 1 && (index < num || num == -1)){
		hashset_isread_ids.insert(id);

		index ++;
	}

	//lookis to add last one

	return 1;
}

int TestSoWorkInterface::get_filter_uid_mid(uint64_t uid, __gnu_cxx::hash_set<uint64_t>& hashset_filter_uids,
				__gnu_cxx::hash_set<uint64_t>& hashset_filter_mids, 
				const std::map<uint64_t, uint64_t>& reco_ids, int num){
			
	if(NULL == p_db_company_){
		LOG_ERROR("db_company is NULL!");
		return -1;
	}

	hashset_filter_uids.insert(uid);

	ReqResultMap req_result_map;
	
	int block_db_id = 27; //
	char uid_req_str[WORD_LEN];
	sprintf(uid_req_str, "{\"api\":\"blockcache\",\"cmd\":\"query\",\"uid\":%"PRIu64"}", uid);

	int result = push_request_result_map(block_db_id, uid, 0, 0, uid_req_str, false, req_result_map);
	if(result <= 0)
		LOG_ERROR("get block uid is failed!");

	/*char uid_token_str[PATH_MAX];
	int result = get_token_uid(uid, uid_token_str);

	if(result <= 0)
		LOG_ERROR("get token is failed!");
	else{
		result = push_request_result_map(block_db_id, uid, 0, 0, uid_token_str, false, req_result_map);
		if(result <= 0)
			LOG_ERROR("get block uid is failed!");
	}*/

	int filter_api_db_id = 28; //
	char f_uid_req_str[WORD_LEN];
	sprintf(f_uid_req_str, "{\"api\":\"filtercache\",\"cmd\":\"query\",\"uid\":%"PRIu64"}", uid);
	
	result = push_request_result_map(filter_api_db_id, uid, 0, 0, f_uid_req_str, false, req_result_map);
	if(result <= 0)
		LOG_ERROR("get filter uid is failed!");

	/*if(result <= 0 )
		LOG_ERROR("get token is failed!");
	else{
		result = push_request_result_map(filter_api_db_id, uid, 0, 0, uid_token_str, false, req_result_map);
		if(result <= 0)
			LOG_ERROR("get filter uid is failed!");
	}*/
	
	int filter_db_id = 23; //
	result = push_request_result_map(filter_db_id, uid, 0, 0, NULL, true, req_result_map);
	if(result <= 0)
		LOG_ERROR("get filter mid is failed!");

	int isread_db_id = 24; // is read
	if(reco_ids.size() > 0){
		char isread_str[COMPRESS_LEN];

		char* str_temp = isread_str;
		int len = sprintf(str_temp, "%"PRIu64"&ids=", uid);
		str_temp = str_temp + len;

		int index = 0;
		for(std::map<uint64_t, uint64_t>::const_iterator it = reco_ids.begin();
				it != reco_ids.end(); it++){

			if(str_temp - isread_str > COMPRESS_LEN)
				break;

			uint64_t id = (*it).first;
			if(index == 0){
				len = sprintf(str_temp, "%"PRIu64, id);
			}else{
				len = sprintf(str_temp, ",%"PRIu64, id);
			}
			str_temp = str_temp + len;
			index ++;
		}
		result = push_request_result_map(isread_db_id, uid, 0, 0, isread_str, false, req_result_map);
		if(result < 0){
			LOG_ERROR("get isread id is failed!");
		}
	}

	int uninterest_db_id = 1;
	char uninterest_str[PATH_MAX];
	sprintf(uninterest_str, "%"PRIu64":wb", uid);
	result = push_request_result_map(uninterest_db_id, uid, 0, 0, uninterest_str, false, req_result_map);
	if(result <=0 )
		LOG_ERROR("get uninterest uid is failed!");

	if(p_db_company_->get_multi_db(req_result_map) > 0){
		char split_char, second_split_char;
				
		char* p_block_result = NULL;
		get_data_from_req_result_map(block_db_id, req_result_map,
						p_block_result, split_char, second_split_char);
		get_block_filter_set(p_block_result, split_char,
						second_split_char, hashset_filter_uids, num);
		LOG_TRACE("%d", hashset_filter_uids.size());

		/*char* p_filter_api_result = NULL;
		get_data_from_req_result_map(filter_api_db_id, req_result_map, 
						p_filter_api_result, split_char, second_split_char);
		get_filter_api_set(p_filter_api_result, split_char, second_split_char, 
				"\"id\":", hashset_filter_uids, num);

		LOG_TRACE("%d", hashset_filter_uids.size());
		*/

		char* p_filter_result = NULL;
		get_data_from_req_result_map(filter_db_id, req_result_map, 
						p_filter_result, split_char, second_split_char);
		get_filter_set(p_filter_result, split_char, second_split_char, "",
						hashset_filter_mids, num);

		LOG_TRACE("%d", hashset_filter_mids.size());

		char* p_uninterest_result = NULL;
		get_data_from_req_result_map(uninterest_db_id, req_result_map,
						p_uninterest_result, split_char, second_split_char);

		get_uninterest_filter_set(p_uninterest_result, split_char, second_split_char,
				        hashset_filter_mids, num);

		LOG_TRACE("%d", hashset_filter_mids.size());

		if(reco_ids.size() > 0){
			char* p_isread_result = NULL;
			get_data_from_req_result_map(isread_db_id, req_result_map,
					    p_isread_result, split_char, second_split_char);

			get_isread_id_set(p_isread_result, split_char, second_split_char, "",
						hashset_filter_mids, num);

			LOG_TRACE("%d", hashset_filter_mids.size());
		}
	}
	release_request_result_map(req_result_map);
	return 1;
}

//功能处理模块
int TestSoWorkInterface::get_test_so_uids(const char* uid_str, int num, int type, 
		const std::map<uint64_t, uint64_t>& reco_ids, //mid-uid
		char*& p_out_string, int& n_out_len){
	if( NULL == uid_str || NULL == p_db_company_ )
		return -1;

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "start", spend_msec, true);

	int result = 0;
	uint64_t uid = strtoull(uid_str, NULL, 10);

	__gnu_cxx::hash_set<uint64_t> hashset_filter_uids;
	__gnu_cxx::hash_set<uint64_t> hashset_filter_mids;
	result = get_filter_uid_mid(uid, hashset_filter_uids, hashset_filter_mids, reco_ids);
	if(result <= 0){
		return return_fail(405, "get filter uids is failed!", p_out_string, n_out_len); 
	}

	int n_len = 0;
	char* p_out_temp = p_out_string;
	int len = 0;

	if(type == 0){
		len = sprintf(p_out_temp, "%s", "{\"return_code\":\"200\",\"result\":[");
		p_out_temp = p_out_temp + len;
		n_len += len;

		int index = 0;
		for(std::map<uint64_t, uint64_t>::const_iterator it = reco_ids.begin();
				it != reco_ids.end(); it ++){
			uint64_t reco_id = (*it).first;
			uint64_t uid = (*it).second;

			__gnu_cxx::hash_set<uint64_t>::iterator m_it = hashset_filter_mids.find(reco_id);
			if(m_it == hashset_filter_mids.end()){
				__gnu_cxx::hash_set<uint64_t>::iterator u_it = hashset_filter_uids.find(uid);

				if(u_it == hashset_filter_uids.end()){
					if(index == 0){
						len = sprintf(p_out_temp, "\"%"PRIu64"\"", reco_id);
					}else{
						len = sprintf(p_out_temp, ",\"%"PRIu64"\"", reco_id);
					}
					p_out_temp = p_out_temp + len;
					n_len += len;

					index ++;
				}
			}
		}

		len = sprintf(p_out_temp, "%s", "]}");
		p_out_temp = p_out_temp + len;
		n_len += len;
	}else if(type == 1){
		len = sprintf(p_out_temp, "%s", "{\"return_code\":\"200\",\"result\":{\"filter_mids\":[");
		p_out_temp = p_out_temp + len;
		n_len += len;

		int index = 0;
		for(__gnu_cxx::hash_set<uint64_t>::iterator it = hashset_filter_mids.begin();
				it != hashset_filter_mids.end(); it++){
			uint64_t id = (*it);
			if(index == 0){
				len = sprintf(p_out_temp, "\"%"PRIu64"\"", id);
			}else{
				len = sprintf(p_out_temp, ",\"%"PRIu64"\"", id);
			}
			p_out_temp = p_out_temp + len;
			n_len += len;

			index ++;
		}

		len = sprintf(p_out_temp, "%s", "],\"filter_uids\":[");
		p_out_temp = p_out_temp + len;
		n_len += len;

		index = 0;
		for(__gnu_cxx::hash_set<uint64_t>::iterator it = hashset_filter_uids.begin();
				it != hashset_filter_uids.end(); it++){
			uint64_t id = (*it);
			if(index == 0){
				len = sprintf(p_out_temp, "%"PRIu64, id);
			}else{
				len = sprintf(p_out_temp, ",%"PRIu64, id);
			}
			p_out_temp = p_out_temp + len;
			n_len += len;
			index ++;
		}

		len = sprintf(p_out_temp, "%s", "]");
		p_out_temp = p_out_temp + len;
		n_len += len;
		
		len = sprintf(p_out_temp, "%s", "}}");
		p_out_temp = p_out_temp + len;
		n_len += len;
	}

	n_out_len = n_len;

	tv_temp = calc_spend_time(tv_temp, "all finish", spend_msec);

	return result;
}

int TestSoWorkInterface::work_core(json_object *req_json, char* &p_out_string, 
		int& n_out_len, uint64_t req_id){
	int result = 0;

	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}

	json_object* uid_json = json_object_object_get(req_json, "uid");
	char* uid_str = NULL;
	if(NULL == uid_json){
		uid_str = "";
		//return -1;
		return return_fail(403, "uid is empty", p_out_string, n_out_len);
	}else{
		uid_str = (char*)json_object_get_string(uid_json);
	}
	
	uint32_t num = 600;
	json_object* num_json = json_object_object_get(req_json, "num");
	if(NULL == num_json){
		num = 600;
	}else{
		num = (uint32_t)json_object_get_int(num_json);
	}

	//固定用户定时更新
	if(strcasecmp(cmd_str, "query") == 0){

		std::map<uint64_t, uint64_t> reco_ids;

		json_object* ids_json = json_object_object_get(req_json, "reco_ids");
		if(NULL == ids_json || is_error(ids_json)){
			return return_fail(403, "reco_ids is error!", p_out_string, n_out_len);
		}else{
			json_object_object_foreach(ids_json, key_str, val){
				uint64_t id = 0;
				id = strtoull(key_str, NULL, 10);

				uint64_t uid = 0;
				uid = strtoull((char*)json_object_get_string(val), NULL, 10);

				reco_ids.insert(std::map<uint64_t, uint64_t>::value_type(id, uid));
			}
		}

		//type 0 is for query, 1 is for get
		result = get_test_so_uids(uid_str, num, 0, reco_ids, p_out_string, n_out_len);
		if(result <= 0){
			return return_fail(405, "computing is failed!", p_out_string, n_out_len);
		}
	}else if(strcasecmp(cmd_str, "get") == 0){

		std::map<uint64_t, uint64_t> reco_ids;
		result = get_test_so_uids(uid_str, num, 1, reco_ids, p_out_string, n_out_len);
		if(result <= 0){
			return return_fail(405, "computing is failed!", p_out_string, n_out_len);
		}
	}
	return result;
}
