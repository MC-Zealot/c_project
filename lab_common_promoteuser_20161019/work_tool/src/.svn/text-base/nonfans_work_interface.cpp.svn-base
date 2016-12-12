#include "nonfans_work_interface.h"

DYN_WORK(NonFansWorkInterface);

int NonFansWorkInterface::mark_func(const string& mark_enc_src, string& mark){
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	GlobalAesDbInterface* global_db_interface = 
		(GlobalAesDbInterface*)p_global_db_company->get_global_db_interface("AES_DB");	
	unsigned char mark_enc[MARK_BUFF_SIZE] = {'\0'};
	uint32_t len_enc = MARK_BUFF_SIZE;
	int ret_aes = 0;
	ret_aes = global_db_interface->enc_func((unsigned char*)mark_enc_src.c_str(), ((uint32_t)mark_enc_src.length() + 1), (unsigned char*)mark_enc, &len_enc);
	LOG_ERROR("%s-%s-%d-%d", (char*)mark_enc_src.c_str(), mark_enc, len_enc, ret_aes);
	mark = string((char*)mark_enc);
	return 0;	
}
int NonFansWorkInterface::get_ip_code(const string& ip)
{
    GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
        if(NULL == p_global_db_company)
        {
                LOG_ERROR("p_global_db_company = NULL");
                return DEFAULT_IP_CODE;
        }
        GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("IP_CODE_DB");
        if(NULL == global_db_interface)
        {
                LOG_ERROR("global_db_interface = NULL");
                return DEFAULT_IP_CODE;
        }
        return ((GlobalItemAdDbInterface*)global_db_interface)->get_ip_code(ip);
}
int NonFansWorkInterface::return_process(string& mark, uint64_t& loc_code, char* &p_out_temp, int& n_out_len)
{
		int return_code = 1;

		GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
		GlobalHostDbInterface* global_db_interface = (GlobalHostDbInterface*)p_global_db_company->get_global_db_interface("HOST_DB");	
		string host;
		char* host_ptr = global_db_interface->get_local_host();
		host = string(host_ptr);	

		json_object* resp_json = json_object_new_object();

		json_object* jo_return_code = json_object_new_int(return_code);
		json_object* jo_host = json_object_new_string((char*)host.c_str());
		json_object_object_add(resp_json, "return_code", jo_return_code);
		json_object_object_add(resp_json, "host", jo_host);

		json_object* jo_result = json_object_new_object();

		json_object* jo_mark = json_object_new_string((char*)mark.c_str());
		json_object_object_add(jo_result, "mark", jo_mark);

		json_object* jo_loc_code = json_object_new_int(loc_code);
		json_object_object_add(jo_result, "loc_code", jo_loc_code);

		json_object_object_add(resp_json, "result", jo_result);

		string resp_str = json_object_to_json_string(resp_json);
		int len = sprintf(p_out_temp, "%s", resp_str.c_str());
		n_out_len = len;
		LOG_ERROR("%s", p_out_temp);

		json_object_put(resp_json);

		return 0;
}
int NonFansWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
	
	int result = 0;
	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	
	//业务请求
	if(strcasecmp(cmd_str, "query") == 0){

		json_object* body_json = json_object_object_get(req_json, "body");
		string ip;
		string mark_enc_src;
		if(NULL == body_json){
			result = -1;
			return return_fail(406, "require json body is null", p_out_string, n_out_len);
		}else{
			json_object* ip_json = json_object_object_get(body_json, "ip");
			if( NULL == ip_json){
				ip = "127.0.0.1";
			}
			else{
				ip = string(json_object_get_string(ip_json));
			}
			json_object* mark_json = json_object_object_get(body_json, "mark_enc_src");
			if(NULL == mark_json){
				return return_fail(406, "mark_enc_src is null", p_out_string, n_out_len);
			}
			else{
				mark_enc_src = string(json_object_get_string(mark_json));
			}
			
		}
		string mark = "";
		int ret_mark = 0;
		ret_mark = mark_func(mark_enc_src, mark);

		uint64_t loc_code = get_ip_code(ip);

		int ret = 0;
		ret = return_process(mark, loc_code, p_out_string, n_out_len);
		if(ret != 0){
			result = -1;
			return return_fail(406, "return_process error", p_out_string, n_out_len);
		}

	}
	else{
	 	result = -1;
        return return_fail(406, "can not support cmd!", p_out_string, n_out_len);	
	}

	return result;
}
