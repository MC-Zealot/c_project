#include "user_follow_proxy_work_interface.h"

DYN_WORK(UserFollowProxyWorkInterface);

int UserFollowProxyWorkInterface::get_online_follow_uids(uint64_t uid, 
		char* &p_out_string, int& n_out_len){	
	if(NULL == p_db_company_){
		LOG_ERROR("db company is null!");
		return -1;
	}
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get online follow uids start", spend_msec, true);

	DbInterface* p_online_db_interface = p_db_company_->get_db_interface("ONLINE_USER_GZ_DB");
	if(NULL == p_online_db_interface){
		LOG_ERROR("online db is fail!");
		return -1;
	}
	
	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_online_db_interface->get(0, uid, p_result, split_char, second_split_char);
	if (result <= 0 ){
		LOG_ERROR("get gz from redis error!:%d", result);
		return -2;
	}
	if (NULL == p_result){
		LOG_ERROR("get gz from redis fail!");
		return -1;
	}

	char* p_temp = p_out_string;
	int len = 0;
	len = sprintf(p_temp, "{\"ids\":[%s", p_result);
	p_temp = p_temp + len;
	n_out_len += len;

	len = sprintf(p_temp, "]}");
	n_out_len += len;

	tv_temp = calc_spend_time(tv_temp, "online finish", spend_msec);

	return 1;
}

unsigned long long htonll(unsigned long long val){
   if (__BYTE_ORDER == __LITTLE_ENDIAN){
		return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
   }else if (__BYTE_ORDER == __BIG_ENDIAN){
	   return val;
   }
}

unsigned long long ntohll(unsigned long long val){
	if (__BYTE_ORDER == __LITTLE_ENDIAN){
		return (((unsigned long long )ntohl((int)((val << 32) >> 32))) << 32) | (unsigned int)ntohl((int)(val >> 32));
	}else if (__BYTE_ORDER == __BIG_ENDIAN){
		return val;
	}
}

int UserFollowProxyWorkInterface::get_offline_follow_uids(uint64_t uid, 
	char* &p_out_string, int& n_out_len){	
	
	if(NULL == p_db_company_){
		LOG_ERROR("db company is null!");
		return -1;
	}
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get offline follow uids start", spend_msec, true);

	DbInterface* p_offline_db_interface = p_db_company_->get_db_interface("OFF_USER_DB_2");
	if(NULL == p_offline_db_interface){
		LOG_ERROR("offline db is fail!");
		return -1;
	}
	
	int db_num = 23 + uid%24%4;
	char skey[64];
	snprintf(skey, 64, "%d-%"PRIu64, db_num, uid);

	LOG_ERROR("%s", skey);

	char* p_result = NULL;
	char split_char, second_split_char;
	int result_len = p_offline_db_interface->s_get(0, skey, p_result, split_char, 
		second_split_char, uid);

	if(result_len <= 0 || NULL == p_result){
		LOG_ERROR("%d:offline result failed! %"PRIu64, result_len, uid);
		return -1;
	}

	char* p_temp = p_out_string;
	int len = 0;
	len = sprintf(p_temp, "{\"ids\":[");
	p_temp = p_temp + len;
	n_out_len += len;

	int step = sizeof(uint64_t);
	int total_num = result_len / step;
	for(int i = 0; i < total_num ; i++){
		uint64_t uid = 0;
		memcpy(&uid, p_result, step);
		uid = ntohll(uid);
		p_result = p_result + step;

		if(i == 0)
			len = sprintf(p_temp, "%"PRIu64, uid);
		else
			len = sprintf(p_temp, ",%"PRIu64, uid);

		p_temp = p_temp + len;
		n_out_len += len;

	}
	len = sprintf(p_temp, "],\"total_number\":%d}", total_num);
	n_out_len += len;

	tv_temp = calc_spend_time(tv_temp, "offline finish", spend_msec);

	return 1;
}

int UserFollowProxyWorkInterface::get_api_follow_uids(uint64_t uid, 
		char* &p_out_string, int& n_out_len){
	
	if(NULL == p_db_company_){
		LOG_ERROR("db company is null!");
		return -1;
	}
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get api follow uids start", spend_msec, true);


	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("OPEN_API_FOLLOW_DB");
	if (NULL == p_follow_db_interface){
		LOG_ERROR("api db is error!");
		return -1;
	}

	char uid_token_str[PATH_MAX];
	char* token_str = NULL;
	snprintf(uid_token_str, PATH_MAX, "%"PRIu64":%s", uid, token_str);
	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_follow_db_interface->s_get(0, uid_token_str, p_result, split_char, second_split_char);
	if (result <= 0 || p_result == NULL){
		LOG_ERROR("get gz from api error!");
		return result;
	}

	char* p_temp = p_out_string;
	n_out_len = sprintf(p_temp, "%s", p_result);

	tv_temp = calc_spend_time(tv_temp, "offline finish", spend_msec);
	return 1;
}
	
int UserFollowProxyWorkInterface::get_follow_uids(uint64_t uid, int type, 
		char* &p_out_string, int& n_out_len, uint64_t req_id){

	int result = 0;
	switch(type){
		case 0:{ // 0 is for all
				result = get_online_follow_uids(uid, p_out_string, n_out_len);
				if(result <=0 ){
					result = get_offline_follow_uids(uid, p_out_string, n_out_len);
					if(result <=0 ){
						result= get_api_follow_uids(uid, p_out_string, n_out_len);
					}
				}
			}
			break;
		case 1:{ // 1 is for redis
				result = get_online_follow_uids(uid, p_out_string, n_out_len);
			} 
			break;
		case 2:{ // 2 is for lushan
				result = get_offline_follow_uids(uid, p_out_string, n_out_len);
			}
			break;
		case 3:{ // 3 is for api
				result = get_api_follow_uids(uid, p_out_string, n_out_len);
			}
			break;
		default:
			break;
	}

	return result;
}

int UserFollowProxyWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get follow work start", spend_msec, true);

	int result = 0;

	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "get";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	
	if(strcasecmp(cmd_str, "get") == 0){

		json_object* uid_json = json_object_object_get(req_json, "uid");
		char* uid_str = NULL;
		if(NULL == uid_json){
			uid_str = "";
			//return -1;
			return return_fail(403, "uid is empty", p_out_string, n_out_len);
		}else{
			uid_str = (char*)json_object_get_string(uid_json);
		}
		
		int type = 0;
		json_object* type_json = json_object_object_get(req_json, "type");
		if(NULL == type_json){
			type = 0;
		}else{
			type = json_object_get_int(type_json);
		}

		uint64_t uid = strtoull(uid_str, NULL, 10);
		result = get_follow_uids(uid, type, p_out_string, n_out_len, req_id);
		if(result <= 0){
			return return_fail(405, "compute is wrong!", p_out_string, n_out_len);
		}

	}
	tv_temp = calc_spend_time(tv_temp, "get follow work all finish", spend_msec);
	return result;
}
