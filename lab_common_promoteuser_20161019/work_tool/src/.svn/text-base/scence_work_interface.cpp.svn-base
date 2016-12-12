#include "scence_work_interface.h"
#include <openssl/aes.h>

DYN_WORK(ScenceWorkInterface);

int ScenceWorkInterface::init_attri_by_req(json_object* &body_json, ACCESS_INFO*& access_info, VEC_CAND& scence_ad_vec){

	json_object* jo_access_info = json_object_object_get(body_json, "access_info");	
	if(NULL == jo_access_info){
		access_info->has_wifi = false;
		access_info->time_slice = 1;
		LOG_DEBUG( "access null");
	} 	
	else{
		json_object* jo_wifi = json_object_object_get(jo_access_info, "wifi");
		if(NULL == jo_wifi){
			access_info->has_wifi = false;
		}
		else{
			int tmp = json_object_get_int(jo_wifi);
			if(tmp == 1){
				access_info->has_wifi = true;
			}
			else{
				access_info->has_wifi = false;
			}
			LOG_DEBUG( "wifi:%d", tmp);
		}
		
		json_object* jo_tm = json_object_object_get(jo_access_info, "timespan");
		if(NULL == jo_tm){
			access_info->time_slice = 1;
		}
		else{
			access_info->time_slice = json_object_get_int(jo_tm);
		}
		LOG_DEBUG("time_slice:%d", access_info->time_slice);
		
		json_object* jo_sc = json_object_object_get(jo_access_info, "scetype");
		if(NULL == jo_sc){
			access_info->scetype = 0;
		}
		else{
			access_info->scetype = json_object_get_int(jo_sc);
		}


		json_object* jo_uid = json_object_object_get(jo_access_info, "uid");
                if(NULL == jo_uid){
                        access_info->uid = "";
                }
                else{
                        access_info->uid = (char *)json_object_get_string(jo_uid);
                }
		
		json_object* jo_lat = json_object_object_get(jo_access_info, "latitude");
                if(NULL == jo_lat){
                        access_info->latitude = "";
                }
                else{
                        access_info->latitude = (char *)json_object_get_string(jo_lat);
                }
		
		json_object* jo_lon = json_object_object_get(jo_access_info, "longitude");
                if(NULL == jo_lon){
                        access_info->longitude = "";
                }
                else{
                        access_info->longitude = (char *)json_object_get_string(jo_lon);
                }


		json_object* jo_intst = json_object_object_get(jo_access_info, "interest");
                if(NULL == jo_intst){
                }
                else{
			int array_len =  json_object_array_length(jo_intst);
                	for (int i = 0; i< array_len; i++){
                        	json_object* jo_one =  json_object_array_get_idx(jo_intst, i);
                        	if(NULL == jo_one){
                                	continue;
                        	}
				access_info->interest.push_back(json_object_get_int(jo_one));
				LOG_DEBUG("interest:%d", json_object_get_int(jo_one));
			}
		}
		json_object* jo_pois = json_object_object_get(jo_access_info, "pois");
                if(NULL == jo_pois){
                }
                else{
			int array_len =  json_object_array_length(jo_pois);
                	for (int i = 0; i< array_len; i++){
				POI_INFO poi_info;
                        	json_object* jo_one =  json_object_array_get_idx(jo_pois, i);
                        	if(NULL == jo_one){
                                	continue;
                        	}
				json_object* jo_dis = json_object_object_get(jo_one, "distance");
                		if(NULL == jo_dis){
				}else{
                        		poi_info.distance = json_object_get_int(jo_dis);
				}
				json_object* jo_poiid = json_object_object_get(jo_one, "poiid");
                		if(NULL == jo_poiid){
				}else{
                        		poi_info.poiid = (char *)json_object_get_string(jo_poiid);
				}
				json_object* jo_cate = json_object_object_get(jo_one, "categorys");
                		if(NULL == jo_cate){
				}else{
                        		poi_info.categorys = (char *)json_object_get_string(jo_cate);
				}
				LOG_DEBUG("nearby-:%d, %s,%s", poi_info.distance,poi_info.poiid.c_str(),poi_info.categorys.c_str());
				access_info->nearby_pois.push_back(poi_info);
			}
		}
		json_object* jo_ctree = json_object_object_get(jo_access_info, "cate_tree");
		if(NULL != jo_ctree){
			json_object_object_foreach(jo_ctree, key_str, val){
				char* tmp = key_str;
				int n = atoi(tmp);
				int array_len =  json_object_array_length(val);
				for (int i = 0; i< array_len; i++){
					json_object* jo_one =  json_object_array_get_idx(val, i);
					if(NULL == jo_one){
						continue;
					}
					access_info->cate_tree[n].push_back(json_object_get_int(jo_one));
					LOG_DEBUG("class:%d, %d", n, json_object_get_int(jo_one));
				}
			}
		}
		json_object* jo_ctr = json_object_object_get(jo_access_info, "hisctr");
		if(NULL != jo_ctr){
                	json_object_object_foreach(jo_ctr, key_str1, val1){
                        	char* tmp = key_str1;
                        	int n = atoi(tmp);//timespan
	
				json_object_object_foreach(val1, key_str2, val2){
					char* tmp2 = key_str2;
					int n2 = atoi(tmp2);
					int array_len =  json_object_array_length(val2);
					for (int i = 0; i< array_len; i++){
						json_object* jo_one =  json_object_array_get_idx(val2, i);
						if(NULL == jo_one){
							continue;
						}
						switch(i){
						case 0:
							access_info->his_ctr[n][n2].click_num = json_object_get_int(jo_one);
							break;
						case 1:
                                                	access_info->his_ctr[n][n2].inter_num = json_object_get_int(jo_one);
                                                	break;
						case 2:
                                                	access_info->his_ctr[n][n2].expos_num = json_object_get_int(jo_one);
                                                	break;
						case 3:
                                                	access_info->his_ctr[n][n2].click_rate = json_object_get_double(jo_one);
                                                	break;
						case 4:
                                                	access_info->his_ctr[n][n2].inter_rate = json_object_get_double(jo_one);
                                                	break;
						}
						access_info->cate_tree[n].push_back(json_object_get_int(jo_one));
						LOG_DEBUG("class11:%d, %d, %d", n, n2, json_object_get_int(jo_one));
					}
				}
                	}
		}
	}
	
	SCENCE_AD* scence_ad = new SCENCE_AD();
	scence_ad_vec.push_back(scence_ad);
	
	return 0;
}


int ScenceWorkInterface::mark_func(ACCESS_INFO*& access_info, SCENCE_AD*& scence_ad_ptr){
	//GOODS_AD* scence_ad_ptr = (GOODS_AD*)goods_ad_result_vec[0];
/*	string mark;

	//mark = pos;
	const int BUFF_LEN = MARK_BUFF_SIZE;	
	char buff[BUFF_LEN];

	uint16_t check_code = 1;
	snprintf(buff, BUFF_LEN, "%lu_%lu_%lu_%dx%d_%d_%d_%d_%s",  
		scence_ad_ptr->dspid, scence_ad_ptr->custid,
			scence_ad_ptr->itemid, scence_ad_ptr->creative.width, scence_ad_ptr->creative.height, 
				scence_ad_ptr->price, scence_ad_ptr->cost, check_code, 
					scence_ad_ptr->impid.c_str());
	mark = string(buff);
	//LOG_DEBUG("mark:%s", mark.c_str());

	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	GlobalAesDbInterface* global_db_interface = 
		(GlobalAesDbInterface*)p_global_db_company->get_global_db_interface("AES_DB");	
	unsigned char mark_enc[MARK_BUFF_SIZE] = {'\0'};
	uint32_t len_enc = MARK_BUFF_SIZE;
	int ret_aes = 0;
	ret_aes = global_db_interface->enc_func((unsigned char*)mark.c_str(), ((uint32_t)mark.length() + 1), 
		(unsigned char*)mark_enc, &len_enc);
	LOG_INFO("%s-%s-%d-%d", (char*)mark.c_str(), mark_enc, len_enc, ret_aes);
	string pos = "0";
	if(scence_ad_ptr->impid.length() > 0)
	    pos = scence_ad_ptr->impid.substr(scence_ad_ptr->impid.length() - 1, 1);	
	scence_ad_ptr->mark = pos + "_" + string((char*)mark_enc);
	scence_ad_ptr->mark_origin =pos + "_" + mark;

	//unsigned char mark_buf[1024] = {'\0'};
	//uint32_t len_buf = 1024;
	//ret_aes = global_db_interface->dec_func(mark_enc, len_enc, (unsigned char*)mark_buf, &len_buf);
	//LOG_DEBUG("%s-%s-%d-%d", mark_enc, mark_buf, len_buf,ret_aes);
	
*/
	return 0;	
	
}
int ScenceWorkInterface::aes_enc(SCENCE_AD*& scence_ad){
/*	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint64_t curr_time = tv.tv_sec * 1000 + tv.tv_usec/1000;

	char* auction_price = NULL;
	int nlen = 0;
	nlen = snprintf(auction_price, 0 , "%d_%lu", scence_ad->cost, curr_time);
	int len = 0;
	if((nlen + 1)%AES_BUFFER_SIZE == 0)
		len = nlen + 1;
	else
		len =((nlen + 1)/AES_BUFFER_SIZE + 1)*AES_BUFFER_SIZE;
	auction_price = new char[len];	
	memset(auction_price, '\0', len);
	LOG_DEBUG("len:%d", len);
	if(auction_price == NULL){
		LOG_ERROR("new memory for auction_price failed");
		return -1;
	}
	snprintf(auction_price, len,  "%d_%lu", scence_ad->cost, curr_time);

	AES_KEY aes;
	unsigned char key[AES_KEY_SIZE];
	unsigned char* encrypt_string = new unsigned char[len];
	unsigned char* decrypt_string = new unsigned char[len];
	unsigned char* input_string = (unsigned char*)auction_price;
	if(encrypt_string == NULL or decrypt_string == NULL){
		LOG_ERROR("new memory for enc dec string failed");
		return -1;
	}
		
	//for(int i = 0; i< AES_KEY_SIZE; i++)
		//key[i] = 32 + i;
	if(strlen(scence_ad->token.c_str()) == AES_KEY_SIZE)
		memcpy(key, scence_ad->token.c_str(), AES_KEY_SIZE);
	else{
		LOG_ERROR("%lu token len error", scence_ad->dspid);
		return -1;
	}

	if (AES_set_encrypt_key(key, AES_KEY_SIZE*8, &aes) < 0) {                                              
		LOG_ERROR("Unable to set encryption key in AES\n");                               
		return -1;
	}

	for(int i = 0; i < len/AES_BUFFER_SIZE; i++)
		AES_ecb_encrypt(input_string + i*AES_BUFFER_SIZE, encrypt_string + i*AES_BUFFER_SIZE, &aes, AES_ENCRYPT);   	

	if (AES_set_decrypt_key(key, AES_KEY_SIZE*8, &aes) < 0) {                                              
		LOG_ERROR("Unable to set encryption key in AES\n");                               
		return -1;
	}

	for(int i = 0; i < len/AES_BUFFER_SIZE; i++)
		AES_ecb_encrypt(encrypt_string + i*AES_BUFFER_SIZE, decrypt_string + i*AES_BUFFER_SIZE, &aes, AES_DECRYPT);   	

	LOG_INFO("%s", input_string);
	//LOG_INFO("%s", encrypt_string);
	LOG_INFO("%s", decrypt_string);
	LOG_INFO("%s", key);
	char* buf = new char[3];
	if(buf == NULL){
		LOG_ERROR("new memory for aes buf failed");
		return -1;
	}
	for(int i = 0; i < len; i++){
		snprintf(buf, 3, "%x%x", (encrypt_string[i]>>4)&0x0f, encrypt_string[i]&0x0f);
		//LOG_DEBUG("%x%x", (encrypt_string[i]>>4)&0x0f, encrypt_string[i]&0x0f);
		//LOG_DEBUG("%s", buf);
		scence_ad->auction_price = scence_ad->auction_price + string(buf, 2);
	}
	//scence_ad->auction_price = string((char*)encrypt_string, len);	
	LOG_INFO("%s", scence_ad->auction_price.c_str());
*/
	return 0;	
}
int ScenceWorkInterface::enc_ad_attri(ACCESS_INFO*& access_info, VEC_CAND& scence_result_vec){
	VEC_CAND::iterator scence_result_iter;
	int aes_ret = 0;
	for(scence_result_iter = scence_result_vec.begin(); scence_result_iter != scence_result_vec.end(); scence_result_iter++){
		SCENCE_AD* scence_ad =(SCENCE_AD*)(*scence_result_iter);
		mark_func(access_info, scence_ad);
		aes_ret = aes_enc(scence_ad);
		if(aes_ret == -1){
			LOG_ERROR("aes_enc failed");
		}
	}
	return 0;
}
int ScenceWorkInterface::json_dump(json_object* & resp_json, VEC_CAND& scence_ad_result_vec, 
		string key,  ACCESS_INFO*& access_info, int num_limit)
{
	json_object* ad_result_array = json_object_new_array();
	int count = 0;
	for(VEC_CAND::iterator iter = scence_ad_result_vec.begin(); iter != scence_ad_result_vec.end(); iter++)
	{
		SCENCE_AD* scence_ad_ptr= (SCENCE_AD*)(*iter);
		/*if(scence_ad_ptr->adid == 0)
			break;*/
		json_object* jo_ad = json_object_new_object();
		json_object_object_add(jo_ad, "extra_data", json_object_new_string((char*)scence_ad_ptr->extra_data.c_str()));
		json_object_object_add(jo_ad, "cate", json_object_new_int(scence_ad_ptr->cate));
		json_object_object_add(jo_ad, "item_id", json_object_new_string((char*)scence_ad_ptr->item_id.c_str()));
		json_object_object_add(jo_ad, "rec_title", json_object_new_string((char*)scence_ad_ptr->rec_title.c_str()));

	/*	if(access_info->debug == 1){
			json_object* jo_kw_list = json_object_new_array();
			for(uint16_t i = 0; i < scence_ad_ptr->keywords_vec.size(); i++){
				json_object* jo_kw = json_object_new_object();
				json_object_object_add(jo_kw, "keyword", json_object_new_string
					((char*)((scence_ad_ptr->keywords_vec[i]).keyword).c_str()));
				json_object_object_add(jo_kw, "weight", json_object_new_int(
					(scence_ad_ptr->keywords_vec[i]).weight));
				json_object_object_add(jo_kw, "postagid", json_object_new_int(
					(scence_ad_ptr->keywords_vec[i]).postagid));
				json_object_object_add(jo_kw, "src_type", json_object_new_int(
					(scence_ad_ptr->keywords_vec[i]).type));
				json_object_array_add(jo_kw_list, jo_kw);
			}
			json_object_object_add(jo_ad, "keywords", jo_kw_list);
			json_object* jo_cate_list = json_object_new_array();
			for(uint16_t i = 0; i < scence_ad_ptr->category_vec.size(); i++){
				json_object* jo_cate = json_object_new_object();
				json_object_object_add(jo_cate, "catecode", 
					json_object_new_int((scence_ad_ptr->category_vec[i]).cate_code));
				json_object_object_add(jo_cate, "weight", 
					json_object_new_int((scence_ad_ptr->category_vec[i]).weight));
				json_object_object_add(jo_cate, "level", 
					json_object_new_int((scence_ad_ptr->category_vec[i]).level));
				json_object_object_add(jo_cate, "src_type", 
					json_object_new_int((scence_ad_ptr->category_vec[i]).src_type));
				json_object_array_add(jo_cate_list, jo_cate);
			}
			json_object_object_add(jo_ad, "category", jo_cate_list);
			json_object_object_add(jo_ad, "adid_expo_num", json_object_new_int(
				scence_ad_ptr->adid_expo_num));
			

		}*/
		json_object_array_add(ad_result_array, jo_ad);

		count++;
		//json_object_put(jo_ad);
	}
	json_object_object_add(resp_json, key.c_str(), ad_result_array);

	return 0;
}
int ScenceWorkInterface::return_process(ACCESS_INFO*& access_info,
	VEC_CAND& scence_ad_vec,VEC_CAND& scence_ad_result_vec, char* &p_out_temp, int& n_out_len)
{
		int return_code = 0;

		GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
		GlobalHostDbInterface* global_db_interface = (GlobalHostDbInterface*)p_global_db_company->get_global_db_interface("HOST_DB");	
		string host;
		char* host_ptr = global_db_interface->get_local_host();
		host = string(host_ptr);	
		//LOG_DEBUG("%s", host.c_str());
		//string host;	
		//if(goods_ad_result_vec.size() > 0 && ((GOODS_AD*)(goods_ad_result_vec[0]))->adid != 0 && access_info->num != 0)
		if(scence_ad_result_vec.size() > 0)
			return_code = 1;

		json_object* resp_json = json_object_new_object();

		json_object* jo_return_code = json_object_new_int(return_code);
		json_object* jo_host = json_object_new_string((char*)host.c_str());
		json_object_object_add(resp_json, "return_code", jo_return_code);
		json_object_object_add(resp_json, "host", jo_host);

		string key = "result";
		json_dump(resp_json, scence_ad_result_vec, key, access_info, 1);
		/*int debug = access_info->debug;
		if(debug){
			json_object* debug_json = json_object_new_object();
			json_dump(debug_json, scence_ad_vec, "scence_ad_vec", access_info, 0);
			
			//json_object* freq_json = json_object_new_object();
			//json_object_object_add(debug_json, "freq", freq_json);

			json_object_object_add(resp_json, "debug", debug_json);
		}*/
		
		string resp_str = json_object_to_json_string(resp_json);
		int len = sprintf(p_out_temp, "%s", resp_str.c_str());
		n_out_len = len;
		LOG_DEBUG("%s", p_out_temp);

		json_object_put(resp_json);

		return 0;
}
int ScenceWorkInterface::release(VEC_CAND& scence_ad_vec){
	for(size_t i = 0; i < scence_ad_vec.size(); i++){
		delete scence_ad_vec[i];
	}
	return 0;
}
int ScenceWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){

	LOG_ERROR("come in");
	
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

		ACCESS_INFO* access_info = new ACCESS_INFO();
		if(NULL == access_info){
			LOG_ERROR("allocate memory for access_info failed!");
			result = -1;
			return return_fail(406, "allocate for access_info memory failed!", p_out_string, n_out_len);
		}

		VEC_CAND scence_ad_vec;
		VEC_CAND scence_ad_result_vec;
		//char* p_out_temp = p_out_string;
		json_object* body_json = json_object_object_get(req_json, "body");
		int ret_init = 0;
		if(NULL == body_json){
			delete access_info;
			LOG_ERROR("require json body is null");
			result = -1;
			return return_fail(406, "require json body is null", p_out_string, n_out_len);
		}else{
			ret_init = init_attri_by_req(body_json, access_info, scence_ad_vec);
			if(ret_init != 0){
				delete access_info;
				release(scence_ad_vec);	
				result = -1;
				return return_fail(406, "init error", p_out_string, n_out_len);
			}
				
		}

		int req_alg = 0;	
		req_alg = algorithm_core(req_id, access_info, "SCENCEAD", scence_ad_vec, scence_ad_result_vec);
		if(req_alg != 1){
			delete access_info;
			release(scence_ad_vec);	
			result = -1;
			return return_fail(406, "alg error", p_out_string, n_out_len);
		}

		/*int ret_enc = 0;
		ret_enc = enc_ad_attri(access_info, scence_ad_result_vec);		
		if(ret_enc != 0){
			result = -1;
			return return_fail(406, "encrypt error", p_out_string, n_out_len);
		}*/

		int ret = 0;
		ret = return_process(access_info, scence_ad_vec, scence_ad_result_vec, p_out_string, n_out_len);
		if(ret != 0){
			delete access_info;
			release(scence_ad_vec);	
			result = -1;
			return return_fail(406, "return_process error", p_out_string, n_out_len);
		}

		delete access_info;
		release(scence_ad_vec);	
	}
	else{
	 	result = -1;
        return return_fail(406, "can not support cmd!", p_out_string, n_out_len);	
	}

	return result;
}
