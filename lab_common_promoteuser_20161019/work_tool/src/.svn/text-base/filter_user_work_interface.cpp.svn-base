#include "filter_user_work_interface.h"

DYN_WORK(FilterUserWorkInterface);
int FilterUserWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "filter_work start", spend_msec, true);

	int result = 0;
	
	json_object* uid_json = json_object_object_get(req_json, "uid");
	char* uid_str = NULL;
	if(NULL == uid_json){
		uid_str = "";
		//return -1;
		return return_fail(403, "uid is empty", p_out_string, n_out_len);
	}else{
		uid_str = (char*)json_object_get_string(uid_json);
	}

	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		LOG_ERROR("error");
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	//固定用户定时更新
	if(strcasecmp(cmd_str, "query") == 0){
		std::vector<uint64_t> vec_reco_uids;
		json_object* uids_json = json_object_object_get(req_json, "reco_uids");
		if(NULL == uids_json || is_error(uids_json)){
			return return_fail(403, "reco_uids is empty", p_out_string, n_out_len);
		}else{
			if(json_object_is_type(uids_json, json_type_array)){
				int n_array_len = json_object_array_length(uids_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* uid_object = json_object_array_get_idx(uids_json, i);
					uint64_t reco_uid = json_object_get_int64(uid_object);

					vec_reco_uids.push_back(reco_uid);
				}
			}
			else{
				return return_fail(403, "uids should be array", p_out_string, n_out_len);
			}
		}
		std::vector<uint64_t> diff_uids;
		result = filter_users(uid_str, vec_reco_uids, diff_uids);
		if(result <= 0){
			return return_fail(405, "operator is failed!", p_out_string, n_out_len);
		}
		generate_output_str(diff_uids, 100, p_out_string, n_out_len);
	}else if(strcasecmp(cmd_str, "get") == 0){ // get filter
		
		json_object* num_json = json_object_object_get(req_json, "num");
		int num = -1;
		if(NULL == num_json){
			num = -1;
		}else{
			num = json_object_get_int(num_json);
		}

		uint64_t uid = strtoull(uid_str, NULL, 10);
		__gnu_cxx::hash_set<uint64_t> hashset_filter_uids;
		get_filter_uids(uid, hashset_filter_uids, num);
		
		generate_output_str(hashset_filter_uids, 100, p_out_string, n_out_len);	
	}else{
		return return_fail(403, "The cmd is not recognized!", p_out_string, n_out_len);
	}
	tv_temp = calc_spend_time(tv_temp, "filter_work all finish", spend_msec);
	return result;
}
