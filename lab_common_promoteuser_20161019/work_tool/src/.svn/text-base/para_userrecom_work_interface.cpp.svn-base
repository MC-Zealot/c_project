#include "para_userrecom_work_interface.h"

DYN_WORK(ParaUserRecomWorkInterface);

int ParaUserRecomWorkInterface::get_reco_uids(const char* uid_str, uint32_t num, 
		uint32_t rel_num, char* p_out_string, int& n_out_len){

	if( NULL == uid_str)
		return -1;

	int spend_msec = -1;
	struct timeval tv_temp;
	tv_temp = calc_spend_time(tv_temp, "start", spend_msec, true);
	
	uint64_t uid = strtoull(uid_str, NULL, 10);

	int result = 0;

	if(NULL == p_db_company_){
		LOG_ERROR("db company is null!");
		return -1;
	}
	
	DbInterface* p_gz_db_interface = p_db_company_->get_db_interface("USER_GZ_DB");
	if(NULL == p_gz_db_interface){
		LOG_ERROR("gz db is fail!");
		return -1;
	}

	char* p_fri_result = NULL;
	char split_char, second_split_char;
	result = p_gz_db_interface->get(0, uid, p_fri_result, split_char, 
		second_split_char);

	if(result <= 0 || NULL == p_fri_result){
		LOG_ERROR("gz result failed!");
		return -1;
	}

	uint64_t fri_uids[MAX_FOL_NUM];
	uint32_t fri_id_num = 0;
	if(p_fri_result != NULL){
		split_string_ids(fri_uids, fri_id_num, p_fri_result, split_char, 0);
	}

	if(fri_id_num > rel_num)
		fri_id_num = rel_num;
	
	WooDbInterface* p_node_db_interface = (WooDbInterface*)p_db_company_->get_db_interface("PARA_DB");
	if(NULL == p_node_db_interface){
		LOG_ERROR("node db is fail!");
		return -1;
	}

	//这一个很重要，保持和后台的Ip列表一致。
	uint32_t para_db_num = (uint32_t)p_node_db_interface->get_ip_num();
	
	char* para_req_str[para_db_num];
	char* p_req_temp[para_db_num];

	for(uint32_t index = 0; index < para_db_num; index ++){
		para_req_str[index] = new char[COMPRESS_LEN];
		if(NULL ==  para_req_str[index]){
			LOG_ERROR("new is error!");
			para_req_str[index] = NULL;
			continue;
		}
		p_req_temp[index] = para_req_str[index];
	}

	int len = 0;
	for(uint32_t index = 0; index < fri_id_num; index ++){

		uint32_t pos = index % para_db_num;

		if(NULL == p_req_temp[pos])
			continue;

		if(index / para_db_num == 0){
			len = sprintf(p_req_temp[pos], "{\"cmd\":\"query\",\"api\":\"getnoderecouser\",\"num\":%"PRIu32",\"uid\":%s,\"node_uids\":[", num, uid_str);
			p_req_temp[pos] = p_req_temp[pos] + len;
			len = sprintf(p_req_temp[pos], "%"PRIu64, fri_uids[index]);
		}else{
			len = sprintf(p_req_temp[pos], ",%"PRIu64, fri_uids[index]);

		}
		p_req_temp[pos] = p_req_temp[pos] + len;
	}

	for(uint32_t index = 0; index < para_db_num; index ++){
		if(NULL == p_req_temp[index])
			continue;

		len = sprintf(p_req_temp[index], "]}");
	}

	tv_temp = calc_spend_time(tv_temp, "prepare", spend_msec);
	
	MapMResult map_result;
	result = (p_node_db_interface)->smget_i(0, para_req_str, para_db_num, 
			map_result, split_char, second_split_char);
	
	print_map_mresult(map_result);

	for(uint32_t index = 0; index < para_db_num; index ++){
		if(para_req_str[index] != NULL)
			delete [] para_req_str[index];
	}

	tv_temp = calc_spend_time(tv_temp, "all finish", spend_msec);

	return result;
}

int ParaUserRecomWorkInterface::work_core(json_object *req_json, char* &p_out_string, 
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
		
		uint32_t rel_num = 600;
		json_object* rel_num_json = json_object_object_get(req_json, "rel_num");
		if(NULL == rel_num_json){
			rel_num = 600;
		}else{
			rel_num = (uint32_t)json_object_get_int(rel_num_json);
		}

		result = get_reco_uids(uid_str, num, rel_num, p_out_string, n_out_len);
		if(result <= 0){
			return return_fail(405, "compute is wrong!", p_out_string, n_out_len);
		}
	}
	
	return result;
}
