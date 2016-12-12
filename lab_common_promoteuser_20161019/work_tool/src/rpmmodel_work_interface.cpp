#include "rpmmodel_work_interface.h"

DYN_WORK(RpmModelWorkInterface);

int RpmModelWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){

	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	
	if(strcasecmp(cmd_str, "query") == 0){
		char* p_out_temp = p_out_string;
		json_object* body_json = json_object_object_get(req_json, "body");
		char* body_str = NULL;
		//char* body_str = "userfeature!";
		int len = 0;
		if(NULL == body_json){
			body_str = "hello word!";
			len = sprintf(p_out_temp, "%s", body_str);
		}else{
			body_str = (char*)json_object_get_string(body_json);
			uint64_t uid = strtoul(body_str,NULL,10);
			int result = 0;
			GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
			if(NULL == p_global_db_company)
				return -1;
			GlobalDbInterface* global_db_interface = 
				p_global_db_company->get_global_db_interface("USER_FD_DB");
			if(NULL == global_db_interface)
				return -1;
			//char return_value[1000]; 
			UserFeature * userfeature =  ((GlobalFeatureDataDbInterface*)global_db_interface)->getfeature(uid);
			len = sprintf(p_out_temp, "uid:%lu,corp_count:%d,school_count:%d", userfeature->uid,userfeature->corp_count,userfeature->school_count);
		}
		n_out_len = len;
	}
	return 0;
}
