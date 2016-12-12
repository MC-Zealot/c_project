#include "userrecom_work_interface.h"

DYN_WORK(UserRecomWorkInterface);
int UserRecomWorkInterface::get_user_mfeature(std::map<uint64_t, uint64_t>& map_return_value,
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
	GlobalDbInterface* FP_db_interface = 
		p_global_db_company->get_global_db_interface("USER_FP_DB");
	if(NULL == FP_db_interface){
		LOG_ERROR("USER_FP db_interface is NULL!");
		return -1;
	}
	bool flag = ((GlobalFpDbInterface*)FP_db_interface)->gets_value(
			map_return_value, uid, vec_item_key, "userfp");

	return flag;
}

int UserRecomWorkInterface::get_user_feature(uint64_t& return_value, uint64_t uid, uint64_t item_key){
	
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
	GlobalDbInterface* FP_db_interface = 
		p_global_db_company->get_global_db_interface("USER_FP_DB");
	if(NULL == FP_db_interface){
		LOG_ERROR("USER_FP db_interface is NULL!");
		return -1;
	}	
	// bool get_value(uint64_t& return_value, uint64_t id,
	// uint64_t item_id, const char* dbid)
	//
	bool flag = ((GlobalFpDbInterface*)FP_db_interface)->get_value(
			return_value, uid, item_key, "userfp");
	return flag;
}

int UserRecomWorkInterface::generate_out_str(int cand_num, const candidate_item_t& cand,
		uint32_t user_type, int& index, char*& p_out_temp, 
		int& n_len){

	if(index == -2){
		n_len = 0;
		int len = sprintf(p_out_temp, "%s", "{\"return_code\":\"200\",\"result\":[");
		p_out_temp = p_out_temp + len;
		n_len += len;
	}else if(index == -1){
		int len = sprintf(p_out_temp, "%s", "]}");
		p_out_temp = p_out_temp + len;
		n_len += len;

	}else{
		uint64_t uid = cand.uid;
		uint64_t score = cand.tscore;

		//0 is guanzhu of guanzhu; 3 is hufen of hufen
		//user_type : 0 is for common user 
		//
		int reason_index = 0;
		if(user_type == 0){
			if(cand.reason[3].bnum == 0){
				return 0;
			}else
				reason_index = 3;
		}else{
			if((cand.reason[0].bnum + cand.reason[1].bnum + cand.reason[2].bnum)== 0){
				return 0;
			}else
				reason_index = 0;
		}

		int len = 0;
		if(index == 0)
			len = sprintf(p_out_temp, "{\"tid\":%"PRIu64, uid);
		else
			len = sprintf(p_out_temp, ",{\"tid\":%"PRIu64, uid);

		index = index + 1;
		
		p_out_temp = p_out_temp + len;
		n_len += len;

		len = sprintf(p_out_temp, ",\"sco\":%"PRIu64, score);
		p_out_temp = p_out_temp + len;
		n_len += len;

		len = sprintf(p_out_temp, ",\"utype\":%"PRIu32, user_type);
		p_out_temp = p_out_temp + len;
		n_len += len;
		
		uint32_t bnum = cand.reason[3].bnum;

		if(bnum != 0){

			len = sprintf(p_out_temp, ",\"hoh_bnum\":%"PRIu32, bnum);
			p_out_temp = p_out_temp + len;
			n_len += len;

			len = sprintf(p_out_temp, ",\"hoh_ext\":[");
			p_out_temp = p_out_temp + len;
			n_len += len;

			uint32_t hf_b_num = (bnum > max_b_num) ? max_b_num : bnum;
			for(int i = 0; i < (int)hf_b_num; i++){
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

		const uint32_t max_gz_b_num = max_b_num * 3;
		uint64_t temp_bridge_uids[max_gz_b_num];
		uint32_t temp_uid_num = 0;

		uint32_t gz_b_num = (cand.reason[0].bnum > max_b_num) ? max_b_num : cand.reason[0].bnum;
		for(uint32_t i = 0; i < gz_b_num ; i++){
			temp_bridge_uids[i] = cand.reason[0].bs[i];
			temp_uid_num ++;
		}

		if(cand_num < CAND_NUM || temp_uid_num == 0){
			gz_b_num = (cand.reason[1].bnum > max_b_num) ? max_b_num : cand.reason[1].bnum;
			for(uint32_t i = 0; i < gz_b_num ; i++){
				temp_bridge_uids[temp_uid_num] = cand.reason[1].bs[i];
				temp_uid_num ++;
			}
			gz_b_num = (cand.reason[2].bnum > max_b_num) ? max_b_num : cand.reason[2].bnum;
			for(uint32_t i = 0; i < gz_b_num ; i++){
				temp_bridge_uids[temp_uid_num] = cand.reason[2].bs[i];
				temp_uid_num ++;
			}
		}

		if(temp_uid_num != 0){
			
			len = sprintf(p_out_temp, ",\"gog_bnum\":%"PRIu32, total_bnum);
			p_out_temp = p_out_temp + len;
			n_len += len;

			len = sprintf(p_out_temp, ",\"gog_ext\":[");
			p_out_temp = p_out_temp + len;
			n_len += len;

			uint32_t gz_b_num = (temp_uid_num > max_b_num) ? max_b_num : temp_uid_num;

			for(int i = 0; i < (int)gz_b_num; i++){
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
	
int UserRecomWorkInterface::get_offline_reco_uids(uint64_t uid, uint32_t num, 
		char* p_out_string, int& n_out_len){
	
	if(NULL == p_db_company_){
		LOG_ERROR("db company is null!");
		return -1;
	}
	
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "get offline reco uids start", spend_msec, true);

	DbInterface* p_offline_db_interface = p_db_company_->get_db_interface("OFFLINE_USER_DB");
	if(NULL == p_offline_db_interface){
		LOG_ERROR("offline db is fail!");
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
		LOG_ERROR("global vu db is failed");
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

	if(NULL == cands_){
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
		LOG_ERROR("parse failed!");
		return -1;
	}

	//tv_temp = calc_spend_time(tv_temp, "offline get", spend_msec);
	candidate_user_t* data = cands_->data();

	char* p_out_stream = p_out_string;
	int cand_num = data->num;
	if(cand_num <= num){
		LOG_ERROR("%"PRIu64" original offline result num %d less than req num %d", uid, cand_num, num);
		return -2;
	}

	if( data != NULL && cand_num > 0){
		
		//获取不能推荐列表	
		__gnu_cxx::hash_set<uint64_t> hashset_filter_uids;
		get_filter_uids(uid, hashset_filter_uids);

		//tv_temp = calc_spend_time(tv_temp, "filter get", spend_msec);

		candidate_item_t cand;
		int index = -2;
		generate_out_str(cand_num, cand, 0, index, p_out_stream, n_out_len);
		
		index = 0;

		for(int n = 0; n < cand_num; n++){
			uint64_t reco_uid = data->cands[n].uid;

			__gnu_cxx::hash_set<uint64_t>::const_iterator it = hashset_filter_uids.find(reco_uid);
			if(it == hashset_filter_uids.end()){
							
				uint32_t user_type = 0;
				
				if(global_vu_db_interface != NULL){

					bool flag = ((GlobalKVDbInterface*)global_vu_db_interface)->
						get_value(reco_uid, user_type);

					if(flag){
						if(user_type == 3) // 被王传鹏修改过滤掉蓝V企业用户
							continue;
						else if(user_type != 1)
							user_type = 2;
					}else{
						user_type = 0;
					}
				}

				generate_out_str(cand_num, data->cands[n], user_type, 
						index, p_out_stream, n_out_len);

				if(index >= (int)num)
					break;

				//封装用户
			}
		}
		//if( is_gray && (index < CONFIDENT_NUM)){	// 过滤后结果小于阈值
		if(index < CONFIDENT_NUM){
			LOG_ERROR("%"PRIu64" filtered offline result num: %d", uid, index);
			return -2;
		}
		
		index = -1;
		generate_out_str(cand_num, cand, 0, index, p_out_stream, n_out_len);
	}
	else{
		LOG_ERROR("data is NULL!");
		return -1;
	}
	tv_temp = calc_spend_time(tv_temp, "offline finish", spend_msec);
	return 1;
}

int UserRecomWorkInterface::get_reco_uids(const char* uid_str, uint32_t num, int type, int zip,
		char* p_out_string, int& n_out_len){

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

	if(type == 0){
		result = get_offline_reco_uids(uid, num, p_out_temp, n_out_temp);
	}else if(type == 1){
		result = get_online_reco_uids(uid, num, p_out_temp, n_out_temp);

	}else{
		result = get_offline_reco_uids(uid, num, p_out_temp, n_out_temp);
		if(result <= 0 ){
			if(result == -2) // CONFIDENT_NUM
				LOG_TRACE("off line data is not confidential!");

			//这里需要重新计算，因此归位p_out_temp
			if(zip == 1){
				p_out_temp = buf;
			}else
				p_out_temp = p_out_string;
			n_out_temp = 0;

			LOG_TRACE("online reco uid!");
			result = get_online_reco_uids(uid, num, p_out_temp, n_out_temp);
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

int UserRecomWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "user_recom work start", spend_msec, true);
	
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

		result = get_reco_uids(uid_str, num, type, zip, p_out_string, n_out_len);
		if(result <= 0){
			return return_fail(405, "compute is wrong!", p_out_string, n_out_len);
		}
	}
	tv_temp = calc_spend_time(tv_temp, "user_recom_work all finish", spend_msec);
	return result;
}
