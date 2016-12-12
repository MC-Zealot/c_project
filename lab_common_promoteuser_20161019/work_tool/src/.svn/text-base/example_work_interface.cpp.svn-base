#include "example_work_interface.h"

DYN_WORK(ExampleWorkInterface);

int ExampleWorkInterface::get_ad_index(uint64_t uid, uint32_t num, char*& p_out_string, int& n_out_len){
	
	if(NULL == p_db_company_){
		LOG_ERROR("db_company is NULL!");
		return -1;
	}

	WooDbInterface *ad_index_db_interface = 
		(WooDbInterface*)p_db_company_->get_db_interface("GOODS_AD_INDEX_DB");
	
	if(NULL == ad_index_db_interface){
		LOG_ERROR("ad index db_interface is NULL!");
		return -1;
	}


	//from wangli index attributes
	uint16_t age = 202;
	uint16_t location = 302;
	uint16_t gender = 401;
	uint16_t psid = 1304;
	uint16_t interests_array[4] = {101, 102, 103, 104};

	char req_str[COMPRESS_LEN];
	char* p_req = req_str;
	int len = 0;
	len = sprintf(p_req, "{\"api\":\"adindex\", \"cmd\":\"find_indexes\", \"db\":\"GOODS_AD_INDEX_DB\", \"item_keys\":[\"keywords\", \"category\",\"price\",\"price_ceiling\",\"budget\"], ");
	p_req += len;

    len = sprintf(p_req, "\"age\":[%"PRIu16"],", age);
	p_req += len;
	
	len = sprintf(p_req, "\"location\":[%"PRIu16"],", location);
	p_req += len;

	len = sprintf(p_req, "\"gender\":[%"PRIu16"],", gender);
	p_req += len;

	len = sprintf(p_req, "\"psid\":[%"PRIu16"],", psid);
	p_req += len;

	len = sprintf(p_req, "\"interest\":[");
	p_req += len;

	for(size_t i = 0; i < 4; i++){
		if(i == 0)
			len = sprintf(p_req, "%"PRIu16, interests_array[i]);
		else
			len = sprintf(p_req, ",%"PRIu16, interests_array[i]);

		p_req += len;
	}

	len = sprintf(p_req, "]}");
	p_req += len;

	char* p_result = NULL;
	char split_char, second_split_char;

	int result = ad_index_db_interface->s_get(0, req_str, p_result, split_char, second_split_char, uid);
	if(result > 0 && p_result != NULL){
		LOG_INFO("%s", p_result);

		json_object* request_json = json_tokener_parse(p_result);
		if(request_json != NULL && !is_error(request_json)){
		
			json_object* results_json = json_object_object_get(request_json, "result");

			if(json_object_is_type(results_json, json_type_array)){
				int n_array_len = json_object_array_length(results_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* ad_object = json_object_array_get_idx(results_json, i);
				
					if(NULL == ad_object || is_error(ad_object))
						continue;

					json_object* ad_id_object = json_object_object_get(ad_object, "ad_id");
					if(NULL == ad_id_object || is_error(ad_id_object))
						continue;
					uint32_t ad_id = (uint32_t)json_object_get_int(ad_id_object);
					LOG_ERROR("%"PRIu32, ad_id);

					json_object* ad_status_object = json_object_object_get(ad_object, "ad_status");
					if(NULL == ad_status_object || is_error(ad_status_object))
						continue;
					uint16_t ad_status = (uint16_t)json_object_get_int(ad_status_object);
					LOG_ERROR("%"PRIu16, ad_status);
					
					json_object* ad_attr_object = json_object_object_get(ad_object, "attr");
					if(NULL == ad_attr_object || is_error(ad_attr_object))
						continue;

					json_object* ad_price_object = json_object_object_get(ad_attr_object, "price");
					if(NULL == ad_price_object || is_error(ad_price_object))
						continue;
					float price = (float)json_object_get_double(ad_price_object);
					LOG_ERROR("%f", price);
					
					json_object* keywords_object = json_object_object_get(ad_attr_object, "keywords");
					if(NULL == keywords_object || is_error(keywords_object))
						continue;

					json_object_object_foreach(keywords_object, key_str, val){
						const char* keyword = json_object_get_string(val);

						//split keyword
						LOG_ERROR("%s", keyword);
					}

					json_object* category_object = json_object_object_get(ad_attr_object, "category");
					if(NULL == category_object || is_error(category_object))
						continue;
					
					json_object_object_foreach(category_object, category_key_str, category_val){
						const char* category = json_object_get_string(category_val);
					
						//split category
						LOG_ERROR("%s", category);
					}
				}
			}
			else{

				LOG_ERROR("result is not array!");
			}

		}else{
			LOG_ERROR("request is null!");
		}

		json_object_put(request_json);

	}else{
		LOG_ERROR("%d", result);
	}
	
	return 1;
}

int ExampleWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
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

	uint64_t uid = strtoull(uid_str, NULL, 10);
	
	uint32_t num = 600;
	json_object* num_json = json_object_object_get(req_json, "num");
	if(NULL == num_json){
		num = 600;
	}else{
		num = (uint32_t)json_object_get_int(num_json);
	}

	//及时用户列表查询
	if(strcasecmp(cmd_str, "query") == 0){
		char* p_out_temp = p_out_string;

		json_object* body_json = json_object_object_get(req_json, "body");
		char* body_str = NULL;
		if(NULL == body_json){
			body_str = "hello word!";
		}else{
			body_str = (char*)json_object_get_string(body_json);
		}

		int len = sprintf(p_out_temp, "%s", body_str);
		n_out_len = len;
	}else if(strcasecmp(cmd_str, "get_ad_index") == 0){
		result = this->get_ad_index(uid, num, p_out_string, n_out_len);

		if(result < 0)
			LOG_ERROR("get ad index is null!");
		else
			return return_fail(200, "get ad index is ok!", p_out_string, n_out_len);
	}
	return result;
}
