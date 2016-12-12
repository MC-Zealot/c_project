#include "tfindex_work_interface.h"

DYN_WORK(TfIndexWorkInterface);

int TfIndexWorkInterface::set(json_object* &req_json){
    json_object* body_json = json_object_object_get(req_json, "body");
    if(NULL == body_json){
        LOG_ERROR("require json body is null");
        return -1;
    }
    else{
        TF_INFO ad_info;
        json_object* adid_json = json_object_object_get(body_json, "adid");
        if(adid_json == NULL){
            LOG_ERROR("adid error");
            return -1;
        } 
        ad_info.adid = json_object_get_string(adid_json);
        json_object* owner_json = json_object_object_get(body_json, "uid");
        if(owner_json == NULL){
            LOG_ERROR("owner error");
            return -1;
        }
        ad_info.owner = json_object_get_int64(owner_json);
        json_object* custid_json = json_object_object_get(body_json, "adsuid");
        if(custid_json == NULL){
            LOG_ERROR("custid error");
            return -1;
        }
        ad_info.custid = json_object_get_int64(custid_json);
        json_object* mid_json = json_object_object_get(body_json, "mid");
        if(mid_json == NULL){
            LOG_ERROR("mid error");
            return -1;
        }
        ad_info.mid = json_object_get_string(mid_json);
        json_object* createtime_json = json_object_object_get(body_json, "createtime");
        if(createtime_json == NULL){
            LOG_ERROR("createtime error");
            return -1;
        }
        ad_info.createtime = json_object_get_int64(createtime_json);
        json_object* endtime_json = json_object_object_get(body_json, "expiretime");
        if(endtime_json == NULL){ 
            LOG_ERROR("endtime error");
            return -1;
        }
        ad_info.endtime = json_object_get_int64(endtime_json);
        json_object* tag_info_json = json_object_object_get(body_json, "tag_info");
        if(tag_info_json == NULL){
            LOG_ERROR("tag_info error");
            return -1;
        }
        ad_info.tag_info = json_object_get_string(tag_info_json);
        json_object* paid_type_json = json_object_object_get(body_json, "paid");
        if(paid_type_json == NULL){
            LOG_ERROR("paid_type error");
            return -1;
        }
        ad_info.paid_type = (uint8_t)json_object_get_int(paid_type_json);
            

        GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();          
        if(NULL == p_global_db_company){                                                                                       
            LOG_DEBUG("p_global_db_company = NULL");                                        
        }                                                                                       
        GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("TFINDEX_DB");   
        if(NULL == global_db_interface){                                                                                       
            LOG_DEBUG("global_db_interface = NULL");                                        
        }                                                                                       
                        
        ((GlobalTFIndexDbInterface*)global_db_interface)->set(ad_info);
    }
    return 1;
}

int TfIndexWorkInterface::del(json_object* &req_json){
    json_object* body_json = json_object_object_get(req_json, "body");
    if(NULL == body_json){
        LOG_ERROR("require json body is null");
        return -1;
    }
    else{
        json_object* adid_json = json_object_object_get(body_json, "adid");
        if(adid_json == NULL){
            return -1;
        } 
        string adid = json_object_get_string(adid_json);
        json_object* owner_json = json_object_object_get(body_json, "uid");
        if(owner_json == NULL){
            return -1;
        }
        uint64_t owner = json_object_get_int64(owner_json); 

        GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();          
        if(NULL == p_global_db_company){                                                                                       
                LOG_DEBUG("p_global_db_company = NULL");                                        
        }                                                                                       
        GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("TFINDEX_DB");   
        if(NULL == global_db_interface){                                                                                       
                LOG_DEBUG("global_db_interface = NULL");                                        
        }                                                                                       
                    
        int del_ret = ((GlobalTFIndexDbInterface*)global_db_interface)->del(adid,owner);
        if(del_ret == -1)
            return -1;
        else if(del_ret == -2)
            return -2;
    }
    return 1;
}

int TfIndexWorkInterface::get(json_object* &req_json,  vector<TF_INFO>& index_result){
    vector<uint64_t> follow_list;
    json_object* body_json = json_object_object_get(req_json, "body");
    if(body_json == NULL){
            return -1;
    }
    else{
        json_object* follow_list_json = json_object_object_get(body_json, "follow_list"); 
        if(follow_list_json == NULL){
            return -1;
        }
        else{
            int len = json_object_array_length(follow_list_json);
            for(int i = 0; i < len; i++){
                json_object* uid_json = json_object_array_get_idx(follow_list_json, i);
                if(uid_json == NULL){
                    return -1;
                }
                else{
                    uint64_t uid = json_object_get_int64(uid_json);
                    follow_list.push_back(uid);
                }
            }
        }
    }
    
    //vector<TF_INFO> index_result;
    GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();          
    if(NULL == p_global_db_company){                                                                                       
        LOG_DEBUG("p_global_db_company = NULL");                                        
    }                                                                                       
    GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("TFINDEX_DB");   
    if(NULL == global_db_interface){                                                                                       
        LOG_DEBUG("global_db_interface = NULL");                                        
    }                                                                                       
                
    int ret_code = ((GlobalTFIndexDbInterface*)global_db_interface)->get(follow_list, index_result);
    
    for(size_t i = 0; i< index_result.size(); i++)
        LOG_ERROR("adid:%s", index_result[i].adid.c_str());
                            
    return ret_code;
}

int TfIndexWorkInterface::return_process(int return_code, vector<TF_INFO>& index_result, char* &p_out_string, int& n_out_len){
    json_object* resp_json = json_object_new_object();
    json_object* return_json = json_object_new_int(return_code);
    json_object_object_add(resp_json, "return_code", return_json);
    GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
    GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("HOST_DB");
    string host;
    char* host_ptr = ((GlobalHostDbInterface*)global_db_interface)->get_local_host();
    host = string(host_ptr);
    json_object* host_json = json_object_new_string(host.c_str());
    json_object_object_add(resp_json, "host", host_json);
    json_object* result_json = json_object_new_array(); 
    for(size_t i = 0; i< index_result.size(); i++){
        json_object* ad_json = json_object_new_object();
        json_object* adid_json = json_object_new_string(index_result[i].adid.c_str());
        json_object* owner_json = json_object_new_int64(index_result[i].owner);
        json_object* mid_json = json_object_new_string(index_result[i].mid.c_str());
        json_object* custid_json = json_object_new_int64(index_result[i].custid);
        json_object* createtime_json = json_object_new_int64(index_result[i].createtime);
        json_object* endtime_json = json_object_new_int64(index_result[i].endtime);
        json_object* tag_info_json = json_object_new_string(index_result[i].tag_info.c_str());
        json_object* paid_type_json = json_object_new_int(index_result[i].paid_type);
        json_object_object_add(ad_json, "adid", adid_json);
        json_object_object_add(ad_json, "owner", owner_json);
        json_object_object_add(ad_json, "mid", mid_json);
        json_object_object_add(ad_json, "custid", custid_json);
        json_object_object_add(ad_json, "createtime", createtime_json);
        json_object_object_add(ad_json, "endtime", endtime_json);
        json_object_object_add(ad_json, "tag_info", tag_info_json);
        json_object_object_add(ad_json, "paid_type", paid_type_json);
        json_object_array_add(result_json, ad_json);
    }
    json_object_object_add(resp_json, "result", result_json);
    string resp_str = json_object_to_json_string(resp_json);
    int len = sprintf(p_out_string, "%s", resp_str.c_str());
    n_out_len = len;
    LOG_INFO("%s", p_out_string);
    json_object_put(resp_json);     //put?? 
    return 1;
}

int TfIndexWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
	int result = 0;
	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str =(char*) "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	
	//业务请求
	if(strcasecmp(cmd_str, "set") == 0){

        int set_ret = set(req_json);
        if(set_ret == -1){
            return return_fail(407, "parameter error!", p_out_string, n_out_len);	
        }
        else{
            return return_fail(201, "set ok!", p_out_string, n_out_len);	
        }

	}
    else if(strcasecmp(cmd_str, "del") == 0){
        int del_ret = del(req_json);
        if(del_ret == -1){
            return return_fail(407, "parameter error!", p_out_string, n_out_len);
        }
        else if(del_ret == -2){
            return return_fail(408, "can not find adid!", p_out_string, n_out_len);
        }
        else{
            return return_fail(201, "del ok!", p_out_string, n_out_len);	
        }
    }
    else if(strcasecmp(cmd_str, "dump") == 0){
        GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();          
        if(NULL == p_global_db_company){                                                                                       
            LOG_DEBUG("p_global_db_company = NULL");                                        
        }                                                                                       
        GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("TFINDEX_DB");   
        if(NULL == global_db_interface){                                                                                       
            LOG_DEBUG("global_db_interface = NULL");                                        
        }                                                                                       
                    
        int ret_dump = ((GlobalTFIndexDbInterface*)global_db_interface)->dump();
        if(ret_dump == 1){
            return return_fail(201, "dump ok!", p_out_string, n_out_len);
        }
        else{
            return return_fail(409, "dump failed!", p_out_string, n_out_len);
        }
    }    
    else if(strcasecmp(cmd_str, "get") == 0){
        vector<TF_INFO> index_result;

        int ret_get = get(req_json, index_result);
        if(ret_get == 1){ 
            return return_process(ret_get, index_result, p_out_string, n_out_len);
        }
        else{
            return return_fail(407,"parameter error!",p_out_string, n_out_len);
        }
    }
    else if(strcasecmp(cmd_str, "rebuild") == 0){
        GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
        if(NULL == p_global_db_company){
            LOG_DEBUG("p_global_db_company = NULL");
        }
        GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("TFINDEX_DB");
        if(NULL == global_db_interface){
            LOG_DEBUG("global_db_interface = NULL");
        }
        int ret_rebuild = ((GlobalTFIndexDbInterface*)global_db_interface)->rebuild();
        if(ret_rebuild==1){
            return return_fail(201, "rebuild ok!", p_out_string, n_out_len);
        }
        else{
            return return_fail(410, "rebuild failed!", p_out_string, n_out_len);
        }
    } 
	else{
        return return_fail(406, "can not support cmd!", p_out_string, n_out_len);	
	}

	return result;
}

