#include "fanstargetidx_work_interface.h"
#include <openssl/aes.h>

DYN_WORK(FansTargetIdxWorkInterface);

//json_object_get_int 64位
int FansTargetIdxWorkInterface::get_ip_code(const string& ip)
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

int FansTargetIdxWorkInterface::init_attri_by_req(json_object*& body_json,ACCESS_INFO*& access_info,VEC_CAND& fans_tar_ad_vec)
{
	json_object* jo_exposure_id = json_object_object_get(body_json,"exposure_id");	
	if(jo_exposure_id != NULL)
		access_info->exposure_id = string(json_object_get_string(jo_exposure_id));

	json_object* jo_feed_position = json_object_object_get(body_json,"feed_position");	
	if(jo_feed_position != NULL)
		access_info->feed_position = string(json_object_get_string(jo_feed_position));

	json_object* jo_appid = json_object_object_get(body_json,"appid");
	if(jo_appid != NULL)
		access_info->appid = json_object_get_int(jo_appid);
	
	/*
	json_object* jo_impression_counts = json_object_object_get(body_json,"impression_counts");
	if(jo_impression_counts != NULL)
		access_info->impression_counts = json_object_get_int(jo_impression_counts);
	*/

    json_object *jo_imp = json_object_object_get(body_json, "imp");
    if(jo_imp != NULL) {
        int len = json_object_array_length(jo_imp);
        for(int i = 0; i < len; ++i) {
            json_object *jo_one_imp = json_object_array_get_idx(jo_imp, i);
            if(jo_one_imp) {
                IMP_INFO imp_info;
                json_object *jo_position = json_object_object_get(jo_one_imp, "position");
                json_object *jo_impid    = json_object_object_get(jo_one_imp, "impid");
                
                if(jo_position && jo_impid) {
                    int pos = json_object_get_int(jo_position);
                    char tmp[32]; //enough for int32
                    sprintf(tmp, "%d", pos);
                    imp_info.position = tmp;
                    imp_info.impid = json_object_get_string(jo_impid);
                    access_info->imp_vec.push_back(imp_info);
                }
            }
        }
    }

	json_object* jo_debug = json_object_object_get(body_json,"debug");
	if(jo_debug != NULL)
		access_info->debug = json_object_get_int(jo_debug);

	json_object* jo_access_info = json_object_object_get(body_json,"access_info");			
	if(jo_access_info != NULL)
	{
		json_object* jo_uid = json_object_object_get(jo_access_info,"uid");
		if(jo_uid != NULL)
			access_info->come_uid = json_object_get_int64(jo_uid);
		
		json_object* jo_ip = json_object_object_get(jo_access_info,"ip");
		if(jo_ip != NULL)
			access_info->ip = string(json_object_get_string(jo_ip));

		json_object* jo_gender = json_object_object_get(jo_access_info,"gender");
		if(jo_gender != NULL)
			access_info->gender = string(json_object_get_string(jo_gender));
		json_object* jo_age = json_object_object_get(jo_access_info,"age");
		if(jo_age != NULL)
			access_info->age = string(json_object_get_string(jo_age));
		json_object* jo_device_type = json_object_object_get(jo_access_info,"device_type");
		if(jo_device_type != NULL)
			access_info->device_type = string(json_object_get_string(jo_device_type));
		json_object* jo_area_code = json_object_object_get(jo_access_info,"area_code");
		if(jo_area_code != NULL)
			access_info->area_code = string(json_object_get_string(jo_area_code));
		json_object* jo_city_code = json_object_object_get(jo_access_info,"city_code");
		if(jo_city_code != NULL)
			access_info->city_code = string(json_object_get_string(jo_city_code));

		json_object* jo_impression_counts = json_object_object_get(jo_access_info,"impression_counts");
		if(jo_impression_counts != NULL)
			access_info->impression_counts = json_object_get_int(jo_impression_counts);

        json_object* jo_num = json_object_object_get(jo_access_info, "num");
        if(jo_num != NULL) {
            access_info->num = json_object_get_int(jo_num);
        } else {
            //default num = 1
            access_info->num = 1;
        }

        json_object* jo_group_id = json_object_object_get(jo_access_info, "group_id");
        if(jo_group_id != NULL) {
            access_info->group_id = string(json_object_get_string(jo_group_id));
        }

        json_object* jo_group_name = json_object_object_get(jo_access_info, "group_name");
        if(jo_group_name != NULL) {
            access_info->group_name = string(json_object_get_string(jo_group_name));
        }
	}

    access_info->ip_code = get_ip_code(access_info->ip);
    
	json_object* jo_ad_req = json_object_object_get(body_json,"ad_req");
	if(jo_ad_req != NULL)
	{
		int array_len = json_object_array_length(jo_ad_req);
		for (int i = 0; i < array_len; i++)
		{
			json_object* jo_one_ad = json_object_array_get_idx(jo_ad_req,i);
			if(jo_one_ad != NULL)
			{
				FANS_TAR_AD* fans_tar_ad = new FANS_TAR_AD();
	
				json_object* jo_adid = json_object_object_get(jo_one_ad,"adid");
				if(jo_adid == NULL)
				{
					//LOG_INFO("wangli_adid");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->adid = json_object_get_string(jo_adid);

                json_object* jo_field_id = json_object_object_get(jo_one_ad, "field_id");
                if(jo_field_id != NULL)
                {
                    fans_tar_ad->field_id = json_object_get_string(jo_field_id);
                }
	
				json_object* jo_custid = json_object_object_get(jo_one_ad,"custid");
				if(jo_custid == NULL)
				{
					//LOG_INFO("wangli_custid");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->order_uid = json_object_get_int64(jo_custid);


				json_object* jo_mid = json_object_object_get(jo_one_ad,"mid");
				if(jo_mid == NULL)
				{
					//LOG_INFO("wangli_mid");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->mid = json_object_get_string(jo_mid);

				json_object* jo_type = json_object_object_get(jo_one_ad,"type");
				if(jo_type == NULL)
				{
					//LOG_INFO("wangli_type");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->type = json_object_get_int(jo_type);

				json_object* jo_alglog = json_object_object_get(jo_one_ad,"alg_log");
				if(jo_alglog == NULL)
				{
					fans_tar_ad->alg_log = "";
				}else
					fans_tar_ad->alg_log = json_object_get_string(jo_alglog);

				

				/*
				json_object* jo_order_uid = json_object_object_get(jo_one_ad,"order_uid");
				if(jo_order_uid == NULL)
				{
					LOG_INFO("wangli_order_uid");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->order_uid = json_object_get_int(jo_order_uid);
				*/

				json_object* jo_bridge_uid = json_object_object_get(jo_one_ad,"bridgeid");
				if(jo_bridge_uid == NULL)
				{
					//LOG_INFO("wangli_bridgeid");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->bridge_uid = json_object_get_int64(jo_bridge_uid);
				LOG_ERROR("bridge_uid:%ld",fans_tar_ad->bridge_uid);
			
				json_object* jo_buy_amount = json_object_object_get(jo_one_ad,"buy_count");
				if(jo_buy_amount == NULL)
				{
					//LOG_INFO("wangli_buy_count");
					fans_tar_ad->buy_amount = 0;
				}else
					fans_tar_ad->buy_amount = json_object_get_int(jo_buy_amount);

				json_object* jo_max_package = json_object_object_get(jo_one_ad,"max_count");
				if(jo_max_package == NULL)
				{
					//LOG_INFO("wangli_max_count");
					fans_tar_ad->max_package = 0;
				}else
					fans_tar_ad->max_package = json_object_get_int(jo_max_package);


				json_object* jo_finish_amount = json_object_object_get(jo_one_ad,"fin_count");
				if(jo_finish_amount == NULL)
				{
					//LOG_INFO("wangli_fin_count");
					fans_tar_ad->finish_amount = 0;
				}else
					fans_tar_ad->finish_amount = json_object_get_int(jo_finish_amount);

				json_object* jo_begin_time = json_object_object_get(jo_one_ad,"timestamp");
				if(jo_begin_time == NULL)
				{
					//LOG_INFO("wangli_timestamp");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->begin_time = json_object_get_int(jo_begin_time);

				json_object* jo_ad_type = json_object_object_get(jo_one_ad,"ad_type");
				if(jo_ad_type == NULL)
				{
					//LOG_INFO("wangli_ad_type");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->ad_type = json_object_get_int(jo_ad_type);

				json_object* jo_brg_type = json_object_object_get(jo_one_ad,"brg_type");
				if(jo_brg_type == NULL)
				{
					//LOG_INFO("wangli_brg_type");
					delete fans_tar_ad;
					continue;
				}else
					fans_tar_ad->brg_type = json_object_get_int(jo_brg_type);


				fans_tar_ad_vec.push_back(fans_tar_ad);	
			}
		}	
	}	
	//LOG_ERROR("wangli_%d",fans_tar_ad_vec.size());
	return 0;
}


int FansTargetIdxWorkInterface::mark_func(ACCESS_INFO*& access_info,FANS_TAR_AD*& fans_tar_ad)
{
	string mark;
	char buff[MARK_BUFF_SIZE];
	const uint32_t cost = 0;
	const uint32_t settle_type = 4;
	const uint32_t check_code = 1;
	const uint32_t deliver_type = 11;
	//????time??
    for(size_t i = 0; i < access_info->imp_vec.size(); ++i) {
        snprintf(buff,MARK_BUFF_SIZE,"%s_%s_%d_%d_%lu_%lu_%d_%lu_%s_%s_%d",fans_tar_ad->adid.c_str(),fans_tar_ad->mid.c_str(),cost,
                 settle_type,access_info->come_uid,fans_tar_ad->begin_time,check_code,fans_tar_ad->order_uid,
                 access_info->imp_vec[i].impid.c_str(),fans_tar_ad->field_id.c_str(),deliver_type);
	//LOG_ERROR("buf:%s\n",buff);
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

        fans_tar_ad->mark_map[access_info->imp_vec[i].position] = access_info->imp_vec[i].position + "_" + string((char*)mark_enc);
        fans_tar_ad->mark_origin_map[access_info->imp_vec[i].position] = access_info->imp_vec[i].position + "_" + mark;
    }

	return 0;				
}


int FansTargetIdxWorkInterface::enc_ad_attri(ACCESS_INFO*& access_info,VEC_CAND& fans_tar_result_vec)
{
	VEC_CAND::iterator fans_tar_result_iter;
	for(fans_tar_result_iter = fans_tar_result_vec.begin();fans_tar_result_iter != fans_tar_result_vec.end();fans_tar_result_iter++)
	{
		FANS_TAR_AD* fans_tar_ad = (FANS_TAR_AD*)(*fans_tar_result_iter);
		mark_func(access_info,fans_tar_ad);
	}
	return 0;
}

int FansTargetIdxWorkInterface::json_dump(json_object*& resp_json, VEC_CAND& fans_tar_result_vec,
		string key, ACCESS_INFO*& access_info)
{
	json_object* ad_result_array = json_object_new_array();
	for(VEC_CAND::iterator iter = fans_tar_result_vec.begin();iter != fans_tar_result_vec.end(); iter++)
	{
		FANS_TAR_AD* fans_tar_ad = (FANS_TAR_AD*)(*iter);
		json_object* jo_ad = json_object_new_object();
		json_object_object_add(jo_ad,"adid",json_object_new_string((char*)fans_tar_ad->adid.c_str()));
        json_object_object_add(jo_ad,"field_id",json_object_new_string((char*)fans_tar_ad->field_id.c_str()));
		json_object_object_add(jo_ad,"mid",json_object_new_string((char*)fans_tar_ad->mid.c_str()));
		json_object_object_add(jo_ad,"custid",json_object_new_int64(fans_tar_ad->order_uid));
		json_object_object_add(jo_ad,"type",json_object_new_int(fans_tar_ad->type));
		json_object_object_add(jo_ad,"ad_type",json_object_new_int(fans_tar_ad->ad_type));
		json_object_object_add(jo_ad,"order_uid",json_object_new_int64(fans_tar_ad->order_uid));
		json_object_object_add(jo_ad,"bridgeid",json_object_new_int64(fans_tar_ad->bridge_uid));
		json_object_object_add(jo_ad,"buy_amount",json_object_new_int(fans_tar_ad->buy_amount));
		json_object_object_add(jo_ad,"max_package",json_object_new_int(fans_tar_ad->max_package));
		json_object_object_add(jo_ad,"finish_amount",json_object_new_int(fans_tar_ad->finish_amount));
		json_object_object_add(jo_ad,"begin_time",json_object_new_int(fans_tar_ad->begin_time));
		json_object_object_add(jo_ad,"brg_type",json_object_new_int(fans_tar_ad->brg_type));
        json_object_object_add(jo_ad,"alg_log", json_object_new_string((char*)fans_tar_ad->alg_log.c_str()));
		json_object_object_add(jo_ad,"algBucket_id",json_object_new_int(fans_tar_ad->algBucket_id));
		//json_object_object_add(jo_ad,"mark",
		//					json_object_new_string((char*)fans_tar_ad->mark.c_str()));
		//json_object_object_add(jo_ad,"mark_origin",
        //json_object_new_string((char*)fans_tar_ad->mark_origin.c_str()));

        json_object *jo_mark = json_object_new_object();
        json_object *jo_mark_origin = json_object_new_object();
        std::map<string, string>::const_iterator mark_iter = fans_tar_ad->mark_map.begin();
        std::map<string, string>::const_iterator mark_origin_iter = fans_tar_ad->mark_origin_map.begin();
        while(mark_iter != fans_tar_ad->mark_map.end() && mark_origin_iter != fans_tar_ad->mark_origin_map.end()) {
            json_object *mark = json_object_new_string(mark_iter->second.c_str());
            json_object *mark_origin = json_object_new_string(mark_origin_iter->second.c_str());

            json_object_object_add(jo_mark, mark_iter->first.c_str(), mark);
            json_object_object_add(jo_mark_origin, mark_origin_iter->first.c_str(), mark_origin);
                
            ++mark_iter;
            ++mark_origin_iter;
        }

        json_object_object_add(jo_ad, "mark", jo_mark);
        json_object_object_add(jo_ad, "mark_origin", jo_mark_origin);
        
		json_object_array_add(ad_result_array,jo_ad);
	}	
	json_object_object_add(resp_json,key.c_str(),ad_result_array);
	return 0;
}


int FansTargetIdxWorkInterface::return_process(ACCESS_INFO*& access_info,VEC_CAND& fans_tar_vec,
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
	json_object* jo_loc_code = json_object_new_int(access_info->ip_code);
	json_object_object_add(resp_json,"return_code",jo_return_code);
	json_object_object_add(resp_json,"host",jo_host);
	json_object_object_add(resp_json,"loc_code",jo_loc_code);
	string key = "result";
	json_dump(resp_json,fans_tar_result_vec,key,access_info);				

	int debug = access_info->debug;
	if(debug)
	{
		json_object* debug_json = json_object_new_object();
		json_dump(debug_json,fans_tar_vec,"fans_tar_vec",access_info);
		json_object_object_add(resp_json,"debug",debug_json);
	}
	string resp_str = json_object_to_json_string(resp_json);
	int len = sprintf(p_out_temp,"%s",resp_str.c_str());
	n_out_len = len;
	LOG_INFO("output:%s",p_out_temp);
	json_object_put(resp_json);
	return 0;
}

int FansTargetIdxWorkInterface::release(VEC_CAND& fans_tar_vec)
{
	for(size_t i = 0;i < fans_tar_vec.size(); i++)
	{
		delete fans_tar_vec[i];
	}
	return 0;
}

int FansTargetIdxWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id)
{
	//LOG_ERROR("I am in fanstarget");	
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
			//LOG_ERROR("allocate memory for access_info failed!");
			result = -1;
			return return_fail(406,"allocate for access_info memory failed!",p_out_string,n_out_len);
		}	
		VEC_CAND fans_tar_ad_vec;
		VEC_CAND fans_tar_ad_result_vec;
		json_object* body_json =  json_object_object_get(req_json,"body");
		int ret_init = 0;
		if(body_json == NULL)
		{
			delete access_info;
			//LOG_ERROR("require json body is null");
			result = -1;
			return return_fail(406,"require json body is null",p_out_string, n_out_len);
		}else
		{
			ret_init = init_attri_by_req(body_json,access_info,fans_tar_ad_vec); 	
			if(ret_init != 0)
			{
				delete access_info;
				result = -1;
				return return_fail(406,"init error",p_out_string,n_out_len);	
			}
		}
		//LOG_ERROR("I am after init\n");
		int req_alg = 0;
		req_alg = algorithm_core(req_id,access_info,"FANSTARAD",fans_tar_ad_vec,fans_tar_ad_result_vec);
		if(req_alg != 1)
		{
			delete access_info;
			result = -1;
			return return_fail(406,"alg error",p_out_string,n_out_len);
		}
		//LOG_ERROR("I am after algorithm\n");	
		int ret_enc = 0;
		ret_enc = enc_ad_attri(access_info,fans_tar_ad_result_vec);				
		if(ret_enc != 0)
		{
			delete access_info;
			result = -1;
			return return_fail(406,"encrypt error", p_out_string, n_out_len);
		}
		//LOG_ERROR("I am after enc_ad_attri\n");
		int ret = 0;
		ret = return_process(access_info,fans_tar_ad_vec,fans_tar_ad_result_vec,p_out_string,n_out_len);
		delete access_info;
		if(ret != 0)
		{
			result = -1;
			return return_fail(406,"return_process error",p_out_string,n_out_len);
		}
		release(fans_tar_ad_vec);
	}else
	{
		result = -1;
		return return_fail(406,"can not support cmd",p_out_string,n_out_len);
	}
	return result;
	
}



