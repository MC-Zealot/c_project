#include "node_userrecom_work_interface.h"

DYN_WORK(NodeUserRecomWorkInterface);

int NodeUserRecomWorkInterface::get_reco_uids(const char* uid_str, uint64_t node_uids[], 
		uint32_t node_uids_num, uint32_t num, char* p_out_string, int& n_out_len){

	if( NULL == uid_str)
		return -1;

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "start", spend_msec, true);
	
	int result = 1;

	MapMResult map_m_result;
    char split_char, second_split_char;

	LOG_INFO("node uid:%"PRIu32, node_uids_num);
	mget_rel_uids(node_uids, node_uids_num, map_m_result,
		split_char, second_split_char, 3);

	MAP_RECO_UID map_reco_uid;
	MAP_RECO_UID::iterator map_reco_it;
	for(MapMResult::iterator it = map_m_result.begin(); it != map_m_result.end(); it++){

		uint64_t bri_uid = (*it).first;
		const char* p_result = (*it).second;

		if(NULL == p_result){
			continue;
		}
		uint32_t reco_uid_num = 0;
		uint64_t reco_uids[MAX_SEC_NUM];
		split_string_ids(reco_uids, reco_uid_num, (const char* &)p_result, split_char, MAX_SEC_NUM);

		for(uint32_t index = 0; index < reco_uid_num; index ++){
			uint64_t reco_uid = reco_uids[index];

			if((map_reco_it = map_reco_uid.find(reco_uid)) != map_reco_uid.end()){
				RECO_UID* p_reco_uid = (*map_reco_it).second;
				if(NULL == p_reco_uid)
					continue;
				else{
					p_reco_uid->bri_uids_[p_reco_uid->bri_num_ % BRI_LEN] = bri_uid;
					p_reco_uid->bri_num_ ++;
				}
			}else{
				RECO_UID* p_reco_uid = new RECO_UID;
				if(NULL == p_reco_uid){
					LOG_ERROR("new reco uid is failed!");
					continue;
				}
				p_reco_uid->uid_ = reco_uid;
				p_reco_uid->bri_num_ = 0;
				p_reco_uid->bri_uids_[p_reco_uid->bri_num_ % BRI_LEN] = bri_uid;
				p_reco_uid->bri_num_ = 1;

				map_reco_uid.insert(MAP_RECO_UID::value_type(reco_uid, p_reco_uid));
			}
		}
	}

	LOG_INFO("%d", map_reco_uid.size());
	release_map_m_result(map_m_result);
	release_map_reco_uid(map_reco_uid);

	tv_temp = calc_spend_time(tv_temp, "all finish", spend_msec);

	return result;
}

int NodeUserRecomWorkInterface::work_core(json_object *req_json, char* &p_out_string, 
		int& n_out_len, uint64_t req_id){
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
	
		uint64_t node_uids[MAX_FOL_NUM];
		uint32_t node_uids_num = 0;
		json_object* uids_json = json_object_object_get(req_json, "node_uids");
		if(NULL == uids_json || is_error(uids_json)){
			return return_fail(403, "node_uids is empty", p_out_string, n_out_len);
		}else{
			if(json_object_is_type(uids_json, json_type_array)){
				int n_array_len = json_object_array_length(uids_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* uid_object = json_object_array_get_idx(uids_json, i);
					uint64_t node_uid = json_object_get_int64(uid_object);

					node_uids[node_uids_num] = node_uid;
					node_uids_num ++;
				}
			}
			else{
				return return_fail(403, "uids should be array", p_out_string, n_out_len);
			}
		}

		result = get_reco_uids(uid_str, node_uids, node_uids_num, num, p_out_string, n_out_len);
		if(result <= 0){
			return return_fail(405, "compute is wrong!", p_out_string, n_out_len);
		}else
			return return_fail(200, "success", p_out_string, n_out_len);
	}
	
	return result;
}
