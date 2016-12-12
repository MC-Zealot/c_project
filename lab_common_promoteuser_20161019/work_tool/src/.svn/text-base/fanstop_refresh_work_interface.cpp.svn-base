#include "fanstop_refresh_work_interface.h"
#include <openssl/aes.h>
#include <string>

DYN_WORK(FansTopRefreshWorkInterface);

//json_object_get_int 64位
int FansTopRefreshWorkInterface::get_ip_code(const string& ip)
{
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
    if(NULL == p_global_db_company)                                                         
	{                                                                                       
	        LOG_DEBUG("p_global_db_company = NULL");                                        
	        return DEFAULT_IP_CODE;                                                         
	}                                                                                       
	GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("IP_CODE_DB");   
	if(NULL == global_db_interface)                                                         
	{                                                                                       
	        LOG_DEBUG("global_db_interface = NULL");                                        
	        return DEFAULT_IP_CODE;                                                         
	}                                                                                       
	return ((GlobalItemAdDbInterface*)global_db_interface)->get_ip_code(ip);

}

int FansTopRefreshWorkInterface::init_attri_by_req(json_object*& body_json,ACCESS_INFO*& access_info,VEC_CAND& fanstop_ad_vec)
{
	json_object* jo_exposure_id = json_object_object_get(body_json,"exposure_id");	
	if(jo_exposure_id != NULL)
		access_info->exposure_id = (string)json_object_get_string(jo_exposure_id);

	json_object* jo_access_info = json_object_object_get(body_json,"access_info");	
	if(jo_access_info != NULL){
        json_object* jo_come_uid = json_object_object_get(jo_access_info,"uid");
        if(jo_come_uid != NULL) access_info->come_uid = (uint64_t)json_object_get_int64(jo_come_uid);

        json_object* jo_age = json_object_object_get(jo_access_info,"age");
        if(jo_age != NULL) access_info->age = (uint16_t)json_object_get_int(jo_age);

        json_object* jo_gender = json_object_object_get(jo_access_info,"gender");
        if(jo_gender != NULL) access_info->gender = (uint16_t)json_object_get_int(jo_gender);

        json_object* jo_area = json_object_object_get(jo_access_info,"area_code");
        if(jo_area != NULL) access_info->area_code = (string)json_object_get_string(jo_area);

        json_object* jo_city = json_object_object_get(jo_access_info,"city_code");
        if(jo_city != NULL) access_info->city_code = (string)json_object_get_string(jo_city);

        json_object* jo_device_type = json_object_object_get(jo_access_info,"device_type");
        if(jo_device_type != NULL) access_info->device_type = (string)json_object_get_string(jo_device_type);
        
        json_object* jo_load_more = json_object_object_get(jo_access_info,"load_more");
        if(jo_load_more != NULL) access_info->load_more = (string)json_object_get_string(jo_load_more);
        
        json_object* jo_load_bhv = json_object_object_get(jo_access_info,"load_bhv");
        if(jo_load_bhv != NULL) access_info->load_bhv = (string)json_object_get_string(jo_load_bhv);

        json_object* jo_ip = json_object_object_get(jo_access_info,"ip");
        if(jo_ip != NULL) access_info->ip = (string)json_object_get_string(jo_ip);
    }

    json_object *jo_imp = json_object_object_get(body_json, "imp_info");
    if(jo_imp != NULL) {
        int len = json_object_array_length(jo_imp);
        for(int i = 0; i < len; ++i) {
            json_object *jo_one_imp = json_object_array_get_idx(jo_imp, i);
            if(jo_one_imp) {
                IMP_INFO imp_info;
                json_object *jo_pos = json_object_object_get(jo_one_imp, "pos");
                json_object *jo_impid    = json_object_object_get(jo_one_imp, "impid");

                if(jo_pos && jo_impid) {
                    int pos = json_object_get_int(jo_pos);
                    char tmp[32]; //enough for int32
                    sprintf(tmp, "%d", pos);
                    imp_info.pos = tmp;
                    imp_info.impid = json_object_get_string(jo_impid);
                    access_info->imp_vec.push_back(imp_info);
                }
            }
        }
    }

    /*
	json_object* jo_debug = json_object_object_get(body_json,"debug");
	if(jo_debug != NULL)
		access_info->debug = json_object_get_int(jo_debug);
    */


    //access_info->ip_code = get_ip_code(access_info->ip);
    
	json_object* jo_cands = json_object_object_get(body_json,"cand");
	if(jo_cands != NULL)
	{
		int array_len = json_object_array_length(jo_cands);
		for (int i = 0; i < array_len; i++)
		{
			json_object* jo_cand = json_object_array_get_idx(jo_cands,i);
			if(jo_cand != NULL)
			{
				FANSTOP_CAND_ADS* fanstop_cand_ad = new FANSTOP_CAND_ADS();
	
				json_object* jo_adid = json_object_object_get(jo_cand,"adid");
				if(jo_adid == NULL)
				{
                    LOG_ERROR("adid is NULL!");
					delete fanstop_cand_ad;
					continue;
				}else
					fanstop_cand_ad->adid = json_object_get_string(jo_adid);

				json_object* jo_mid = json_object_object_get(jo_cand,"mid");
				if(jo_mid == NULL)
				{
                    LOG_ERROR("mid is NULL!");
					delete fanstop_cand_ad;
					continue;
				}else
					fanstop_cand_ad->mid = json_object_get_string(jo_mid);

				json_object* jo_owner = json_object_object_get(jo_cand,"owner");
				if(jo_owner == NULL)
				{
                    LOG_ERROR("owner is NULL!");
					delete fanstop_cand_ad;
					continue;
				}else
					fanstop_cand_ad->owner = (uint64_t)json_object_get_int64(jo_owner);


				json_object* jo_custid = json_object_object_get(jo_cand,"custid");
				if(jo_custid == NULL)
				{
                    LOG_ERROR("custid is NULL!");
					delete fanstop_cand_ad;
					continue;
				}else
					fanstop_cand_ad->custid = json_object_get_int64(jo_custid);

				json_object* jo_createtime = json_object_object_get(jo_cand,"createtime");
				if(jo_createtime == NULL)
				{
                    LOG_ERROR("createtime is NULL!");
					delete fanstop_cand_ad;
					continue;
				}else
					fanstop_cand_ad->createtime = json_object_get_int(jo_createtime);

				json_object* jo_endtime = json_object_object_get(jo_cand,"endtime");
				if(jo_endtime == NULL)
				{
                    LOG_ERROR("endtime is NULL!");
					delete fanstop_cand_ad;
					continue;
				}else
					fanstop_cand_ad->endtime = json_object_get_int(jo_endtime);

				json_object* jo_tag_info = json_object_object_get(jo_cand,"tag_info");
				if(jo_tag_info == NULL)
				{

                   LOG_ERROR("tag_info is NULL!"); 
					delete fanstop_cand_ad;
					continue;
				}else
					fanstop_cand_ad->tag_info = (string)json_object_get_string(jo_tag_info);


				json_object* jo_paid_type = json_object_object_get(jo_cand,"paid_type");
				if(jo_paid_type == NULL)
				{
                    LOG_ERROR("paid_type is NULL!");
					delete fanstop_cand_ad;
					continue;
				}else
					fanstop_cand_ad->paid_type = json_object_get_int(jo_paid_type);

				json_object* jo_buynum = json_object_object_get(jo_cand,"buy_count");
				if(jo_buynum == NULL)
				{
                    LOG_ERROR("buy_count is NULL!");
					//delete fanstop_cand_ad;
                    fanstop_cand_ad->buynum = 0;
					//continue;
				}else
					fanstop_cand_ad->buynum = json_object_get_int(jo_buynum);

				fanstop_ad_vec.push_back(fanstop_cand_ad);	
                LOG_ERROR("init_ad_info is: %s_%s_%lu_%lu_%d_%d_%s_%d_%d ",fanstop_cand_ad->adid.c_str(),fanstop_cand_ad->mid.c_str(),
                        fanstop_cand_ad->owner,fanstop_cand_ad->custid,fanstop_cand_ad->createtime,fanstop_cand_ad->endtime,
                        fanstop_cand_ad->tag_info.c_str(),fanstop_cand_ad->paid_type,fanstop_cand_ad->buynum);
			}
		}	
	}

	LOG_ERROR("init_access_info is: %s_%lu_%d_%d_%s_%s_%s_%s",access_info->exposure_id.c_str(),access_info->come_uid,access_info->age, 
            access_info->gender,access_info->device_type.c_str(),access_info->load_more.c_str(),access_info->load_bhv.c_str(),access_info->ip.c_str());

	LOG_ERROR("fanstop_refresh_%d",fanstop_ad_vec.size());
	return 0;
}


int FansTopRefreshWorkInterface::mark_func(ACCESS_INFO*& access_info,FANSTOP_CAND_ADS*& fans_tar_ad)
{
	string mark;
	char buff[MARK_BUFF_SIZE];
	const uint32_t cost = 0;
	const uint32_t settle_type = 4;
	const uint32_t check_code = 1;
    const uint32_t appid = 0;
	const uint32_t deliver_type = 11;
    
    LOG_ERROR("i am in mark_func");

    for(size_t i = 0; i < access_info->imp_vec.size(); ++i) {
        snprintf(buff,MARK_BUFF_SIZE,"%s_%s_%d_%d_%lu_%lu_%d_%lu_%s_%d_%d",fans_tar_ad->adid.c_str(),fans_tar_ad->mid.c_str(),cost,
                settle_type,access_info->come_uid,fans_tar_ad->createtime,check_code,fans_tar_ad->custid,
                access_info->imp_vec[i].impid.c_str(),appid,deliver_type);
        LOG_ERROR("buf:%s\n",buff);
        mark = string(buff);

        GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
        GlobalAesDbInterface* global_db_interface = 
            (GlobalAesDbInterface*)p_global_db_company->get_global_db_interface("AES_DB");	
        unsigned char mark_enc[MARK_BUFF_SIZE] = {'\0'};
        uint32_t len_enc = MARK_BUFF_SIZE;
        int ret_aes = 0;
        ret_aes = global_db_interface->enc_func((unsigned char*)mark.c_str(), ((uint32_t)mark.length() + 1), 
                (unsigned char*)mark_enc, &len_enc);
        LOG_INFO("%s-%s-%d-%d", (char*)mark.c_str(), mark_enc, len_enc, ret_aes);
        
        fans_tar_ad->mark = access_info->imp_vec[i].pos + "_" + string((char*)mark_enc);
        fans_tar_ad->mark_origin = access_info->imp_vec[i].pos + "_" + mark;
        //fans_tar_ad->mark_map[access_info->imp_vec[i].pos] = access_info->imp_vec[i].pos + "_" + string((char*)mark_enc);
        //fans_tar_ad->mark_origin_map[access_info->imp_vec[i].pos] = access_info->imp_vec[i].pos + "_" + mark;
    }

	return 0;				
}


int FansTopRefreshWorkInterface::enc_ad_attri(ACCESS_INFO*& access_info,VEC_CAND& fans_tar_result_vec)
{
	VEC_CAND::iterator fans_tar_result_iter;
	for(fans_tar_result_iter = fans_tar_result_vec.begin();fans_tar_result_iter != fans_tar_result_vec.end();fans_tar_result_iter++)
	{
		FANSTOP_CAND_ADS* fans_tar_ad = (FANSTOP_CAND_ADS*)(*fans_tar_result_iter);
		mark_func(access_info,fans_tar_ad);
	}
	return 0;
}

int FansTopRefreshWorkInterface::json_dump(json_object*& resp_json, VEC_CAND& fans_tar_result_vec,
		string key, ACCESS_INFO*& access_info)
{
	json_object* ad_result_array = json_object_new_array();
	for(VEC_CAND::iterator iter = fans_tar_result_vec.begin();iter != fans_tar_result_vec.end(); iter++)
	{
		FANSTOP_CAND_ADS* fanstop_ad = (FANSTOP_CAND_ADS*)(*iter);
		json_object* jo_ad = json_object_new_object();
		json_object_object_add(jo_ad,"adid",json_object_new_string((char*)fanstop_ad->adid.c_str()));
        json_object_object_add(jo_ad,"owner",json_object_new_int64(fanstop_ad->owner));
		json_object_object_add(jo_ad,"mid",json_object_new_string((char*)fanstop_ad->mid.c_str()));
		json_object_object_add(jo_ad,"custid",json_object_new_int64(fanstop_ad->custid));
		json_object_object_add(jo_ad,"createtime",json_object_new_int(fanstop_ad->createtime));
		json_object_object_add(jo_ad,"endtime",json_object_new_int(fanstop_ad->endtime));
		json_object_object_add(jo_ad,"tag_info",json_object_new_string((char*)fanstop_ad->tag_info.c_str()));
		json_object_object_add(jo_ad,"alg_log",json_object_new_string((char*)fanstop_ad->alg_log.c_str()));
		json_object_object_add(jo_ad,"paid_type",json_object_new_int(fanstop_ad->paid_type));

		json_object_object_add(jo_ad,"mark",
							json_object_new_string((char*)fanstop_ad->mark.c_str()));
		json_object_object_add(jo_ad,"mark_origin",
        json_object_new_string((char*)fanstop_ad->mark_origin.c_str()));

        /*
        json_object *jo_mark = json_object_new_object();
        json_object *jo_mark_origin = json_object_new_object();
        std::map<string, string>::const_iterator mark_iter = fanstop_ad->mark_map.begin();
        std::map<string, string>::const_iterator mark_origin_iter = fanstop_ad->mark_origin_map.begin();
        while(mark_iter != fanstop_ad->mark_map.end() && mark_origin_iter != fanstop_ad->mark_origin_map.end()) {
            json_object *mark = json_object_new_string(mark_iter->second.c_str());
            json_object *mark_origin = json_object_new_string(mark_origin_iter->second.c_str());

            json_object_object_add(jo_mark, mark_iter->first.c_str(), mark);
            json_object_object_add(jo_mark_origin, mark_origin_iter->first.c_str(), mark_origin);
                
            ++mark_iter;
            ++mark_origin_iter;
        }

        json_object_object_add(jo_ad, "mark", jo_mark);
        json_object_object_add(jo_ad, "mark_origin", jo_mark_origin);
        */
        
		json_object_array_add(ad_result_array,jo_ad);
	}	
	json_object_object_add(resp_json,key.c_str(),ad_result_array);
	return 0;
}


int FansTopRefreshWorkInterface::return_process(ACCESS_INFO*& access_info,
		VEC_CAND& fans_tar_result_vec,char*& p_out_temp, int& n_out_len)
{
	int return_code = 0;
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	GlobalHostDbInterface* global_db_interface = (GlobalHostDbInterface*)p_global_db_company->get_global_db_interface("HOST_DB");	
	string host;
	char* host_ptr = global_db_interface->get_local_host();
	host = string(host_ptr);	
	if(fans_tar_result_vec.size() > 0)
		return_code = 1;
	
	json_object* resp_json = json_object_new_object();
	
	json_object* jo_return_code = json_object_new_int(return_code);
	json_object* jo_host = json_object_new_string((char*)host.c_str());
	json_object_object_add(resp_json,"return_code",jo_return_code);
	json_object_object_add(resp_json,"host",jo_host);

	string key = "result";
	json_dump(resp_json,fans_tar_result_vec,key,access_info);				

	string resp_str = json_object_to_json_string(resp_json);
	int len = sprintf(p_out_temp,"%s",resp_str.c_str());
	n_out_len = len;
	LOG_INFO("output:%s",p_out_temp);
	json_object_put(resp_json);
	return 0;
}

int FansTopRefreshWorkInterface::release(VEC_CAND& fans_tar_vec)
{
	for(size_t i = 0;i < fans_tar_vec.size(); i++)
	{
		delete fans_tar_vec[i];
	}
	return 0;
}

int FansTopRefreshWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id)
{
    int spend_msec = -1;
    struct timeval tv_temp;
    tv_temp = calc_spend_time(tv_temp, "tfmatch_core start", spend_msec, true);
	int result = 0;
	json_object* cmd_json = json_object_object_get(req_json,"cmd");
	char* cmd_str = NULL;
	if(cmd_json == NULL)
		cmd_str = "query";
	else
		cmd_str = (char*)json_object_get_string(cmd_json);	
	if(strcasecmp(cmd_str,"query") == 0)
	{
		ACCESS_INFO* access_info = new ACCESS_INFO();
		if(access_info == NULL)
		{
			LOG_ERROR("allocate memory for access_info failed!");
			result = -1;
			return return_fail(406,"allocate for access_info memory failed!",p_out_string,n_out_len);
		}	
		VEC_CAND fanstop_ad_vec;
		VEC_CAND fanstop_ad_result_vec;
		json_object* body_json =  json_object_object_get(req_json,"body");
		int ret_init = 0;
		if(body_json == NULL)
		{
			delete access_info;
			LOG_ERROR("require json body is null");
			result = -1;
			return return_fail(406,"require json body is null",p_out_string, n_out_len);
		}else
		{
			ret_init = init_attri_by_req(body_json,access_info,fanstop_ad_vec); 	
			if(ret_init != 0)
			{
				delete access_info;
				result = -1;
				return return_fail(406,"init error",p_out_string,n_out_len);	
			}
		}
		LOG_ERROR("I am after init\n");
		//int req_alg = 0;
		int req_alg = 1;
		LOG_ERROR("req_alg init:%d\n",req_alg);
        //string alg_name  = "TFMATCH_ALG";
		LOG_ERROR("req_alg init:%d\n",access_info);
        string algo_name = "TFMATCH_ALG";
        req_alg = algorithm_core(req_id,(AccessInfo*)access_info,algo_name,fanstop_ad_vec,fanstop_ad_result_vec);
		LOG_ERROR("req_alg return:%d\n",req_alg);
		if(req_alg != 1)
		{
			delete access_info;
			result = -1;
			return return_fail(406,"alg error",p_out_string,n_out_len);
		}
		LOG_ERROR("I am after algorithm\n");	
		int ret_enc = 0;
		//ret_enc = enc_ad_attri(access_info,fanstop_ad_result_vec);				
		ret_enc = enc_ad_attri(access_info,fanstop_ad_vec);				
		if(ret_enc != 0)
		{
			delete access_info;
			result = -1;
			return return_fail(406,"encrypt error", p_out_string, n_out_len);
		}
		LOG_ERROR("I am after enc_ad_attri\n");
		int ret = 0;
		ret = return_process(access_info,fanstop_ad_result_vec,p_out_string,n_out_len);
		//ret = return_process(access_info,fanstop_ad_result_vec,p_out_string,n_out_len);
		delete access_info;
		if(ret != 0)
		{
			result = -1;
			return return_fail(406,"return_process error",p_out_string,n_out_len);
		}
		release(fanstop_ad_vec);
        tv_temp = calc_spend_time(tv_temp, "tfmatch_core get finish", spend_msec);
	}else if(strcasecmp(cmd_str,"update") == 0)
    {
        //当发送update命令时，需要对订单文件进行更新
        GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
        if(NULL == p_global_db_company){
            LOG_ERROR("p_global_db_company = NULL");
            return return_fail(500,"system error!",p_out_string, n_out_len);
        }
        GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("FT_ADID_DB");
        if(NULL == global_db_interface){
            LOG_ERROR("global_db_interface = NULL");
            return return_fail(500,"system error!",p_out_string, n_out_len);
        }
        int update_ret = ((GlobalFTAdidDbInterface*)global_db_interface)->update_adid_file();
        if(update_ret == 1){
            tv_temp = calc_spend_time(tv_temp, "tfmatch_core upate adid file finish", spend_msec);
            return return_fail(201, "update ok!", p_out_string, n_out_len);
        }
        else{
            return return_fail(201, "update ok!", p_out_string, n_out_len);
        }

    }else if (strcasecmp(cmd_str,"getadinfo") == 0){

        json_object* adid_json = json_object_object_get(req_json,"adid");
        if(adid_json == NULL){
            LOG_ERROR("get adid ERROR");
            return return_fail(500,"system error!",p_out_string, n_out_len);
        }
        string adid = (string)json_object_get_string(adid_json);

        GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
        if(NULL == p_global_db_company){
            LOG_ERROR("p_global_db_company = NULL");
            return return_fail(500,"system error!",p_out_string, n_out_len);
        }
        GlobalDbInterface* global_db_interface = p_global_db_company->get_global_db_interface("FT_ADID_DB");
        if(NULL == global_db_interface){
            LOG_ERROR("global_db_interface = NULL");
            return return_fail(500,"system error!",p_out_string, n_out_len);
        }
        string adid_value;
        bool update_ret = ((GlobalFTAdidDbInterface*)global_db_interface)->get_adinfo(adid,adid_value);
        if(update_ret){
            tv_temp = calc_spend_time(tv_temp, "tfmatch_core upate adid file finish", spend_msec);
            LOG_ERROR("adid %s info is %s ",adid.c_str(),adid_value.c_str());
            return return_fail(201, "get adid info ok!", p_out_string, n_out_len);
        }
    }
    else
	{
		result = -1;
		return return_fail(406,"can not support cmd",p_out_string,n_out_len);
	}
	return result;
	
}



