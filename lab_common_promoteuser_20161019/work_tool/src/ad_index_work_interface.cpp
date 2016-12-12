#include "ad_index_work_interface.h"

DYN_WORK(AdIndexWorkInterface);

GlobalAdDbInterface* AdIndexWorkInterface::get_ad_db_interface(const char* db_name){
	
	if(NULL == db_name){
		LOG_ERROR("db name is NULL!");
		return NULL;
	}

	GlobalAdDbInterface* ad_index_db_interface = NULL;

	if(NULL == p_db_company_){
		LOG_ERROR("db_company is NULL!");
		return NULL;
	}

	//get global db company
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	if(NULL == p_global_db_company){
		LOG_ERROR("global db_company is NULL!");
		return NULL;
	}
	
	ad_index_db_interface = 
		(GlobalAdDbInterface*)p_global_db_company->get_global_db_interface(db_name);
	if(NULL == ad_index_db_interface){
		LOG_ERROR("ad index db_interface is NULL!");
		return NULL;
	}
	
	return ad_index_db_interface;

}

int AdIndexWorkInterface::set(const char* db_name, json_object *req_json, char* &p_out_string, 
		int& n_out_len, uint64_t req_id){
	
	GlobalAdDbInterface* ad_index_db_interface = get_ad_db_interface(db_name);

	if(NULL == ad_index_db_interface){
		LOG_ERROR("get ad db is NULL!");
		return -1;
	}
			
	int result = ad_index_db_interface->set_ad_info(req_json);
	if(result < 0){
		return return_fail(408, "set ad info failed!", p_out_string, n_out_len);
	}
	else{

		char* p_temp = p_out_string;
		int len = sprintf(p_temp, "{\"return_code\":200, \"result\":1}");
		n_out_len = len;
	}

	return 1;
}

int AdIndexWorkInterface::get_indexes(const char* db_name, uint64_t ad_id, const char* item_keys[], 
     	size_t item_key_num, char* &p_out_string, int& n_out_len, uint64_t req_id){
 
	GlobalAdDbInterface* ad_index_db_interface = get_ad_db_interface(db_name);

	if(NULL == ad_index_db_interface){
		LOG_ERROR("get ad db is NULL!");
		return -1;
	}

	char buf[INDEX_BSON_LEN];
	size_t len = 0;
	size_t old_len = INDEX_BSON_LEN;

	int result = ad_index_db_interface->get_ad_index_str(ad_id, item_keys, item_key_num, 
			buf, old_len, len);

	if(result <0 )
		return return_fail(404, "can not found!", p_out_string, n_out_len);
	else{
		char* p_temp = p_out_string;
		len = sprintf(p_temp, "{\"return_code\":200, \"result\":%s}", buf);
		p_temp += len;
		n_out_len += len;
	}

	return 1;
}

int AdIndexWorkInterface::get_attrs(const char* db_name, uint64_t ad_id, const char* item_keys[], 
		size_t item_key_num, char* &p_out_string, int& n_out_len, uint64_t req_id){

	GlobalAdDbInterface* ad_index_db_interface = get_ad_db_interface(db_name);

	if(NULL == ad_index_db_interface){
		LOG_ERROR("get ad db is NULL!");
		return -1;
	}

	char buf[ATTR_BSON_LEN];
	size_t len = 0;
	size_t old_len = ATTR_BSON_LEN;

	int result = ad_index_db_interface->get_ad_attr_str(ad_id, item_keys, item_key_num,
			buf, old_len, len);

	if(result <0 )
		return return_fail(404, "can not found!", p_out_string, n_out_len);
	else{
		char* p_temp = p_out_string;
		len = sprintf(p_temp, "{\"return_code\":200, \"result\":%s}", buf);
		p_temp += len;
		n_out_len += len;
	}

	return 1;
}

int AdIndexWorkInterface::set_status(const char* db_name, uint64_t ad_id, int ad_status, 
		char* &p_out_string, int& n_out_len, uint64_t req_id){
	
	GlobalAdDbInterface* ad_index_db_interface = get_ad_db_interface(db_name);

	if(NULL == ad_index_db_interface){
		LOG_ERROR("get ad db is NULL!");
		return -1;
	}

	int result = ad_index_db_interface->set_ad_status(ad_id, (AD_STATUS)ad_status);

	if(result <0 )
		return return_fail(404, "set status is error!", p_out_string, n_out_len);
	else{
		char* p_temp = p_out_string;
		int len = sprintf(p_temp, "{\"return_code\":200, \"result\":{\"%"PRIu64"\":%d}}", ad_id, ad_status);
		n_out_len = len;
	}
	
	return 1;
}

int AdIndexWorkInterface::get_status(const char* db_name, uint64_t ad_id, 
		char* &p_out_string, int& n_out_len, uint64_t req_id){
	
	GlobalAdDbInterface* ad_index_db_interface = get_ad_db_interface(db_name);

	if(NULL == ad_index_db_interface){
		LOG_ERROR("get ad db is NULL!");
		return -1;
	}

	AD_STATUS ad_status = AD_STATUS_RUN;
	int result = ad_index_db_interface->get_ad_status(ad_id, ad_status);

	if(result <0 )
		return return_fail(404, "can not found!", p_out_string, n_out_len);
	else{
		char* p_temp = p_out_string;
		int len = sprintf(p_temp, "{\"return_code\":200, \"result\":{\"%"PRIu64"\":%d}}", ad_id, ad_status);
		n_out_len = len;
	}
	
	return 1;
}

int AdIndexWorkInterface::find_indexes(const char* db_name, json_object* req_json, 
		char* &p_out_string, int& n_out_len, uint64_t req_id){
	
	GlobalAdDbInterface* ad_index_db_interface = get_ad_db_interface(db_name);

	if(NULL == ad_index_db_interface){
		LOG_ERROR("get ad db is NULL!");
		return -1;
	}

	char* attr_name = ad_index_db_interface->get_attr_name();

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "find indexes is now", spend_msec, true);

	SET_AD set_ad;
	int result = ad_index_db_interface->find_indexes(set_ad, req_json);

	tv_temp = calc_spend_time(tv_temp, "find is end!", spend_msec);

	if(result <0 )
		return return_fail(404, "can not found!", p_out_string, n_out_len);
	else{

		//return attrs
		const char* item_keys[WORD_LEN];
		size_t item_key_num = 0;
		json_object* item_keys_json = json_object_object_get(req_json, "item_keys");
		if(NULL == item_keys_json || is_error(item_keys_json)){
			LOG_ERROR("item key is NULL!");
			item_key_num = 0;
		}else{
			if(json_object_is_type(item_keys_json, json_type_array)){
				int n_array_len = json_object_array_length(item_keys_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* item_key_object = json_object_array_get_idx(item_keys_json, i);
					const char* item_key = (const char*)json_object_get_string(item_key_object);
				
					item_keys[item_key_num] = item_key;
					item_key_num ++;
				}
			}
			else{

				LOG_ERROR("item key is NULL!");
			}
		}

		char* p_temp = p_out_string;
		int len = sprintf(p_temp, "{\"return_code\":200, \"result\":[");
		p_temp += len;
		n_out_len += len;

		int i = 0;
		for(SET_AD::iterator it = set_ad.begin(); it != set_ad.end(); it++){
			AdInfo* p_info =(*it);

			if(p_info == NULL)
				continue;
			
			uint64_t ad_id = p_info->get_id();
			AD_STATUS ad_status = p_info->get_status();
			if(ad_status != AD_STATUS_RUN)
				continue;

			char buf[ATTR_BSON_LEN];
			size_t buf_size = ATTR_BSON_LEN;
			size_t result_len = 0;
			
			if(0 == item_key_num)
				result = p_info->get_attr(buf, buf_size, result_len);
			else
				result = p_info->get_sub_attrs(item_keys, item_key_num, buf, buf_size, result_len);

			if(result < 0)
				continue;

			if(i == 0){
				len = sprintf(p_temp, "{\"ad_id\":%"PRIu64",\"ad_status\":%d,\"%s\":%s}", ad_id, ad_status, attr_name, buf);
			}else{
				len = sprintf(p_temp, ",{\"ad_id\":%"PRIu64",\"ad_status\":%d,\"%s\":%s}", ad_id, ad_status, attr_name, buf);
			}

			p_temp += len;
			n_out_len += len;

			i ++;

		}

		len = sprintf(p_temp, "]}");

		p_temp += len;
		n_out_len += len;

		tv_temp = calc_spend_time(tv_temp, "display is end!", spend_msec);
	}
	
	return 1;
}

int AdIndexWorkInterface::info(const char* db_name, char* &p_out_string, int& n_out_len, uint64_t req_id){
	
	GlobalAdDbInterface* ad_index_db_interface = get_ad_db_interface(db_name);

	if(NULL == ad_index_db_interface){
		LOG_ERROR("get ad db is NULL!");
		return -1;
	}

	INFO info;
	int result = ad_index_db_interface->info(info);

	char* p_temp = p_out_string;
	int len = sprintf(p_temp, "{\"return_code\":200, \"result\":{");
	p_temp += len;
	n_out_len += len;

	len = sprintf(p_temp, "\"db_size\":%"PRIu32, info.db_size_);
	p_temp += len;
	n_out_len += len;

	len = sprintf(p_temp, ",\"index_size\":%"PRIu32, info.index_size_);
	p_temp += len;
	n_out_len += len;

	len = sprintf(p_temp, "}}");
	p_temp += len;
	n_out_len += len;

	return result;
}

int AdIndexWorkInterface::update_dump_file(const char* db_name, const char* file_name_str, 
		char* &p_out_string, int& n_out_len, uint64_t req_id){
	
	
	GlobalAdDbInterface* ad_index_db_interface = get_ad_db_interface(db_name);

	if(NULL == ad_index_db_interface){
		LOG_ERROR("get ad db is NULL!");
		return -1;
	}

	int result = ad_index_db_interface->load_update_dump_file(file_name_str);

	char* p_temp = p_out_string;
	int len = sprintf(p_temp, "{\"return_code\":200, \"result\":%d}", result);
	p_temp += len;
	n_out_len += len;

	return result;

}

int AdIndexWorkInterface::work_core(json_object *req_json, char* &p_out_string, 
		int& n_out_len, uint64_t req_id){
	int result = 0;
	
	json_object* db_json = json_object_object_get(req_json, "db");
	char* db_str = NULL;
	if(NULL == db_json){
		return return_fail(406, "db can not be null!", p_out_string, n_out_len);
	}else{
		db_str = (char*)json_object_get_string(db_json);
	}

	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}

	if(strcasecmp(cmd_str, "update_dump") == 0){
	
		json_object* file_name_json = json_object_object_get(req_json, "file_name");
		char* file_name_str = NULL;
		if(NULL == file_name_json || is_error(file_name_json)){
			return return_fail(406, "file name can not be null!", p_out_string, n_out_len);
		}else{
			file_name_str = (char*)json_object_get_string(file_name_json);
		}

		result = update_dump_file(db_str, file_name_str, p_out_string, n_out_len, req_id);
		if(result < 0)
			return return_fail(406, "update dump file is failed!", p_out_string, n_out_len);

	}else if(strcasecmp(cmd_str, "info") == 0){

		result = info(db_str, p_out_string, n_out_len, req_id);
		if(result < 0)
			return return_fail(406, "info failed!", p_out_string, n_out_len);

	}else if(strcasecmp(cmd_str, "set_status") == 0){
		uint64_t ad_id = 0;
		json_object* value_json = json_object_object_get(req_json, "ad_id");
		char* value_str = NULL;
		if(NULL == value_json){
			return return_fail(404, "ad id is NULL!", p_out_string, n_out_len);
		}else{
			value_str = (char*)json_object_get_string(value_json);
			if(value_str != NULL)
				ad_id = strtoull(value_str, NULL, 10);
			else
				return return_fail(406, "get failed!", p_out_string, n_out_len);
		}

		json_object* status_json = json_object_object_get(req_json, "status");
		
		int new_status = 0;
		if(NULL == status_json){
			return return_fail(404, "status is NULL!", p_out_string, n_out_len);
		}else{
			new_status = json_object_get_int(status_json);
		}
		
		result = set_status(db_str, ad_id, new_status, p_out_string, n_out_len, req_id);
		if(result < 0){
			return_fail(406, "status failed!", p_out_string, n_out_len);
		}

	}else if(strcasecmp(cmd_str, "set") == 0){
		result = set(db_str, req_json, p_out_string, n_out_len, req_id);	

		if(result < 0){
			return_fail(406, "set failed!", p_out_string, n_out_len);
		}

	}else if(strcasecmp(cmd_str, "get_indexes") == 0){
		
		uint64_t ad_id = 0;
		json_object* value_json = json_object_object_get(req_json, "ad_id");
		char* value_str = NULL;
		if(NULL == value_json){
			return return_fail(404, "ad id is NULL!", p_out_string, n_out_len);
		}else{
			value_str = (char*)json_object_get_string(value_json);
			if(value_str != NULL)
				ad_id = strtoull(value_str, NULL, 10);
			else
				return return_fail(406, "get failed!", p_out_string, n_out_len);
		}

		const char* item_keys[WORD_LEN];
		size_t item_key_num = 0;
		json_object* item_keys_json = json_object_object_get(req_json, "item_keys");
		if(NULL == item_keys_json || is_error(item_keys_json)){
			LOG_ERROR("item key is NULL!");
			item_key_num = 0;
		}else{
			if(json_object_is_type(item_keys_json, json_type_array)){
				int n_array_len = json_object_array_length(item_keys_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* item_key_object = json_object_array_get_idx(item_keys_json, i);
					const char* item_key = (const char*)json_object_get_string(item_key_object);
				
					item_keys[item_key_num] = item_key;
					item_key_num ++;
				}
			}
			else{

				LOG_ERROR("item key is NULL!");
			}
		}

		return get_indexes(db_str, ad_id, item_keys, item_key_num, p_out_string, n_out_len, req_id);

	}else if(strcasecmp(cmd_str, "get_attrs") == 0){

		uint64_t ad_id = 0;
		json_object* value_json = json_object_object_get(req_json, "ad_id");
		char* value_str = NULL;
		if(NULL == value_json){
			return return_fail(404, "ad id is NULL!", p_out_string, n_out_len);
		}else{
			value_str = (char*)json_object_get_string(value_json);
			if(value_str != NULL)
				ad_id = strtoull(value_str, NULL, 10);
			else
				return return_fail(406, "get failed!", p_out_string, n_out_len);
		}

		const char* item_keys[WORD_LEN];
		size_t item_key_num = 0;
		json_object* item_keys_json = json_object_object_get(req_json, "item_keys");
		if(NULL == item_keys_json || is_error(item_keys_json)){
			LOG_ERROR("item key is NULL!");
			item_key_num = 0;
		}else{
			if(json_object_is_type(item_keys_json, json_type_array)){
				int n_array_len = json_object_array_length(item_keys_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* item_key_object = json_object_array_get_idx(item_keys_json, i);
					const char* item_key = (const char*)json_object_get_string(item_key_object);
				
					item_keys[item_key_num] = item_key;
					item_key_num ++;
				}
			}
			else{

				LOG_ERROR("item key is NULL!");
			}
		}

		return get_attrs(db_str, ad_id, item_keys, item_key_num, p_out_string, n_out_len, req_id);
	}else if(strcasecmp(cmd_str, "get_status") == 0){
		uint64_t ad_id = 0;
		json_object* value_json = json_object_object_get(req_json, "ad_id");
		char* value_str = NULL;
		if(NULL == value_json){
			return return_fail(404, "ad id is NULL!", p_out_string, n_out_len);
		}else{
			value_str = (char*)json_object_get_string(value_json);
			if(value_str != NULL)
				ad_id = strtoull(value_str, NULL, 10);
			else
				return return_fail(406, "get failed!", p_out_string, n_out_len);
		}

		return get_status(db_str, ad_id, p_out_string, n_out_len, req_id);
	}else if(strcasecmp(cmd_str, "find_indexes") == 0){
	
		result = find_indexes(db_str, req_json, p_out_string, n_out_len, req_id);

		if(result < 0){
			return return_fail(405, "get indexes is error!", p_out_string, n_out_len);
		}

	}else{
		result = -1;
		return return_fail(406, "can not support cmd!", p_out_string, n_out_len);
	}
	return result;
}
