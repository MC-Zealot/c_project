#include "block_cache_work_interface.h"

DYN_WORK(BlockCacheWorkInterface);

int BlockCacheWorkInterface::get_block(const char* uid_str, char* &p_out_string, 
		int& n_out_len, const char* from_str, int write_flag){

	int type = 1;

	if(from_str != NULL){
		if(strcasecmp(from_str, "cache") == 0)
			type = 0;
		else if(strcasecmp(from_str, "api") == 0)
			type = 1;
		else if(strcasecmp(from_str, "normal") == 0)
			type = 2;
		else
			type = 2;
	}else{
		type = 2;
	}

	int result = 1;
	int cache_flag = 1;

	char* p_result = NULL;
	switch(type){

		case 0:
			{
				result = get_block_from_db(uid_str, p_result, "BLOCK_API_CACHE_DB");
				break;
			}
		case 1:
			{
				result = get_block_from_db(uid_str, p_result, "USER_BLOCK_DB");
				break;
			}
		case 2:
		default:
			{
				result = get_block_from_db(uid_str, p_result, "BLOCK_API_CACHE_DB");
				if(result <= 0 || NULL == p_result){
					LOG_TRACE("api cache is NULL!");

					cache_flag = 0;
					result = get_block_from_db(uid_str, p_result, "USER_BLOCK_DB");
				}

				break;
			}
	}
	
	if(result <= 0 || NULL == p_result){
		LOG_ERROR("get block is error!");
		return result;
	}

	n_out_len = sprintf(p_out_string, "%s", p_result);

	if(type == 2){
		if(cache_flag == 0){
			result = set_block_cache(uid_str, p_result);
		}
	}else{
		if(write_flag == 1){
			result = set_block_cache(uid_str, p_result);
		}
	}

	return result;
}

int BlockCacheWorkInterface::set_block_cache(const char* uid_str, char* &p_result){
	int result = 1;
	
	if(NULL == uid_str){

		LOG_ERROR("uid is NULL!");
		return -1;
	}

	uint64_t uid = strtoull(uid_str, NULL, 10);

	if( NULL == p_db_company_){

		LOG_ERROR("db company is NULL!");
		return -1;
	}

	DbInterface* p_block_api_db_interface = p_db_company_->get_db_interface("BLOCK_API_CACHE_DB");
	if (NULL == p_block_api_db_interface){
	
		LOG_ERROR("block api interface is NULL!");
		return -1;
	}

	int s_result = ((HiRedisDbInterface*)p_block_api_db_interface)->s_set(0, uid_str, p_result, uid);
	if(s_result <= 0){
		LOG_ERROR("set data is error!");
	}

	return result;

}

int BlockCacheWorkInterface::get_block_from_db(const char* uid_str, char* &p_result, const char* db_name){
	//type = 0 is to write cache && 1 is not to
	
	int result = 0;
	
	if(NULL == uid_str){

		LOG_ERROR("uid is NULL!");
		return -1;
	}

	uint64_t uid = strtoull(uid_str, NULL, 10);
	
	if( NULL == p_db_company_){

		LOG_ERROR("db company is NULL!");
		return -1;
	}

	DbInterface* p_block_db_interface = p_db_company_->get_db_interface(db_name);
	if (NULL == p_block_db_interface){
	
		LOG_ERROR("block api interface is NULL!");
		return -1;
	}


	p_result = NULL;
	char split_char, second_split_char;

	if(strcasecmp(db_name, "USER_BLOCK_DB") == 0){
		char uid_token_str[PATH_MAX];
		result = get_token_uid(uid, uid_token_str);
		if(result <= 0){
			LOG_ERROR("get token is failed!");
			return result;
		}
		result = p_block_db_interface->s_get(0, uid_token_str, p_result, split_char, second_split_char, uid);
	}else{
		result = p_block_db_interface->s_get(0, uid_str, p_result, split_char, second_split_char, uid);
	}

	if(result <= 0){
		LOG_ERROR("get data is error:%s", db_name);
	}

	return result;
}

int BlockCacheWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
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
	
	int write_flag = 1;
	json_object* write_flag_json = json_object_object_get(req_json, "write_flag");
	if(NULL == write_flag_json){
		write_flag = 1;
	}else{
		write_flag = json_object_get_int(write_flag_json);
	}

	json_object* from_json = json_object_object_get(req_json, "from");
	char* from_str = NULL;
	if(NULL == from_json){
		from_str = "normal";
	}else{
		from_str = (char*)json_object_get_string(from_json);
	}

	result = get_block(uid_str, p_out_string, n_out_len, from_str, write_flag);
	if(result <= 0){
		return return_fail(405, "compute is wrong!", p_out_string, n_out_len);
	}

	return result;
}
