#include "wax_work_interface.h"
#include <openssl/aes.h>

DYN_WORK(WaxWorkInterface);

int WaxWorkInterface::init_attri_by_req(json_object* &body_json, ACCESS_INFO*& access_info, VEC_CAND& wax_ad_vec){

	json_object* jo_exposure_id = json_object_object_get(body_json, "exposure_id");
	if(NULL == jo_exposure_id)
		access_info->exposure_id = "0";
	else
		access_info->exposure_id = string(json_object_get_string(jo_exposure_id));

	json_object* jo_psid = json_object_object_get(body_json, "psid");
	if(NULL == jo_psid)
		access_info->psid = "";
	else
		access_info->psid = string(json_object_get_string(jo_psid));

	json_object* jo_appid = json_object_object_get(body_json, "appid");
	if(NULL == jo_appid)
		access_info->appid = 0;
	else
		access_info->appid = json_object_get_int(jo_appid);
	json_object* jo_num = json_object_object_get(body_json, "num");
	if(NULL == jo_num)
		access_info->num = 1;
	else
		access_info->num = json_object_get_int(jo_num);
	json_object* jo_debug = json_object_object_get(body_json, "debug");
	if(NULL == jo_debug)
		access_info->debug = 0;
	else
		access_info->debug = json_object_get_int(jo_debug);

	json_object* jo_freq_uid = json_object_object_get(body_json, "freq_uid");
	if(NULL == jo_freq_uid)
		access_info->freq_uid = DEFAULT_FREQ_UID;
	else
		access_info->freq_uid = json_object_get_int(jo_freq_uid);

	json_object* jo_freq_adid = json_object_object_get(body_json, "freq_adid");
	if(NULL == jo_freq_adid)
		access_info->freq_adid = DEFAULT_FREQ_ADID;
	else
		access_info->freq_adid = json_object_get_int(jo_freq_adid);

	//LOG_DEBUG("exposure_id is :%s;psid is :%s;appid is :%d;freq_uid:%d,freq_adid:%d", access_info->exposure_id.c_str(), access_info->psid.c_str(), access_info->appid, access_info->freq_uid, access_info->freq_adid);

	json_object* jo_access_info = json_object_object_get(body_json, "access_info");	
	if(NULL == jo_access_info){
		access_info->uid_ = 0;
		access_info->ip = "127.0.0.1";
		access_info->gender = DEFAULT_GENDER;//female
		access_info->age = DEFAULT_AGE;//30
		access_info->device_type = DEFAULT_DEVICE_TYPE;
	} 	
	else{
		json_object* jo_uid = json_object_object_get(jo_access_info, "uid");
		if(NULL == jo_uid)
			access_info->uid_ = 0;
		else
			 access_info->uid_ = json_object_get_int64(jo_uid);

		json_object* jo_ip = json_object_object_get(jo_access_info, "ip");
		if(NULL == jo_ip)
			access_info->ip = "127.0.0.1";
		else
			 access_info->ip = string(json_object_get_string(jo_ip));

		json_object* jo_gender = json_object_object_get(jo_access_info, "gender");
		if(NULL == jo_gender)
			access_info->gender = DEFAULT_GENDER;
		else
			 access_info->gender = json_object_get_int(jo_gender);

		json_object* jo_age = json_object_object_get(jo_access_info, "age");
		if(NULL == jo_age)
			access_info->age = DEFAULT_AGE;
		else
			 access_info->age = json_object_get_int(jo_age);

		json_object* jo_device_type = json_object_object_get(jo_access_info, "device_type");
		if(NULL == jo_device_type)
			access_info->device_type = DEFAULT_DEVICE_TYPE;
		else
			 access_info->device_type = json_object_get_string(jo_device_type);

     }
	//LOG_DEBUG("uid is :%s;ip is :%s;gender is :%d;age is :%d;device_type :%d;", access_info->uid.c_str(), access_info->ip.c_str(), access_info->gender, access_info->age, access_info->device_type);
	
	//LOG_DEBUG("weibo_cate:%s", access_info->weibo_cate.c_str());

	json_object* jo_ad_req = json_object_object_get(body_json, "ad_req");
	if(NULL == jo_ad_req){
			//access_info->keywords_vec.clear();
	}
	else{
		int array_len =  json_object_array_length(jo_ad_req);
		for (int i = 0; i< array_len; i++){
			json_object* jo_one_ad =  json_object_array_get_idx(jo_ad_req, i);
			if(NULL == jo_one_ad){
				continue;
			}
			else{
				WAX_AD* wax_ad = new WAX_AD();
				json_object* jo_dspid = json_object_object_get(jo_one_ad, "dspid");
				if(NULL == jo_dspid){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->dspid = json_object_get_int(jo_dspid);
				json_object* jo_token = json_object_object_get(jo_one_ad, "token");
				if(NULL == jo_token){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->token = json_object_get_string(jo_token);
				json_object* jo_impid = json_object_object_get(jo_one_ad, "impid");
				if(NULL == jo_impid){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->impid = json_object_get_string(jo_impid);
				json_object* jo_uniqid = json_object_object_get(jo_one_ad, "uniq_id");
				if(NULL == jo_uniqid){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->uniq_id = json_object_get_string(jo_uniqid);
				json_object* jo_custid = json_object_object_get(jo_one_ad, "custid");
				if(NULL == jo_custid){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->custid = json_object_get_int(jo_custid);
				json_object* jo_price = json_object_object_get(jo_one_ad, "price");
				if(NULL == jo_price){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->price = json_object_get_int(jo_price);
				json_object* jo_bidfloor = json_object_object_get(jo_one_ad, "bid_floor");
				if(NULL == jo_bidfloor){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->bidfloor = json_object_get_int(jo_bidfloor);
//				json_object* jo_bidid = json_object_object_get(jo_one_ad, "bidid");
//				if(NULL == jo_bidid){
//					delete wax_ad;
//					continue;
//				}
//				else
//					wax_ad->bidid = json_object_get_string(jo_bidid);

				json_object* jo_deal_id = json_object_object_get(jo_one_ad, "deal_id");
				if(NULL == jo_deal_id){
                    wax_ad->deal_id = "";
				}
				else
					wax_ad->deal_id = json_object_get_string(jo_deal_id);

				json_object* jo_id = json_object_object_get(jo_one_ad, "id");
				if(NULL == jo_id){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->id = json_object_get_string(jo_id);
				json_object* jo_itemid = json_object_object_get(jo_one_ad, "itemid");
				if(NULL == jo_itemid){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->itemid = json_object_get_int(jo_itemid);
				json_object* jo_creative_sizew = json_object_object_get(jo_one_ad, "creative_sizew");
				if(NULL == jo_creative_sizew){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->creative.width = json_object_get_int(jo_creative_sizew);
				json_object* jo_creative_sizeh = json_object_object_get(jo_one_ad, "creative_sizeh");
				if(NULL == jo_creative_sizeh){
					delete wax_ad;
					continue;
				}
				else
					wax_ad->creative.height = json_object_get_int(jo_creative_sizeh);
				json_object* jo_creative_url = json_object_object_get(jo_one_ad, "creative_url");
				if(NULL == jo_creative_url){
					delete wax_ad;
					continue;
				}
				else
					snprintf(wax_ad->creative.url, 1000, "%s",  json_object_get_string(jo_creative_url));
				wax_ad_vec.push_back(wax_ad);
			}
		}	
				
	}
	return 0;
}
int WaxWorkInterface::mark_func(ACCESS_INFO*& access_info, WAX_AD*& wax_ad_ptr){
	//GOODS_AD* wax_ad_ptr = (GOODS_AD*)goods_ad_result_vec[0];
	string mark;

	//mark = pos;
	const int BUFF_LEN = MARK_BUFF_SIZE;	
	char buff[BUFF_LEN];
	/*snprintf(buff, BUFF_LEN, "%lu", wax_ad_ptr->adid) ;
	//mark = string(buff);
	mark = mark + "_" + string(buff);
	mark = mark + "_" + wax_ad_ptr->url_md5;
	snprintf(buff, BUFF_LEN, "%d", wax_ad_ptr->cost) ;	
	mark = mark + "_" + string(buff);
	snprintf(buff, BUFF_LEN, "%d", wax_ad_ptr->bid_type) ;	
	mark = mark + "_" + string(buff);
	mark = mark + "_" + access_info->uid;*/
	/*const int pos_buff_len = 10;
	char pos_buff[pos_buff_len];
	snprintf(pos_buff, pos_buff_len , "%d", wax_ad_ptr->pos);
	string pos = string(pos_buff);*/

	//time_t  now;
	//now = time(NULL);

        /*struct timeval tv_temp;
        gettimeofday(&tv_temp, NULL);
        uint64_t now = tv_temp.tv_sec * 1000 + tv_temp.tv_usec / 1000;*/

	uint16_t check_code = 1;
	/*snprintf(buff, BUFF_LEN, "%ld", now); 
	mark = mark + "_" +string(buff);
	mark = mark + "_" +string( "1");
	snprintf(buff, BUFF_LEN, "%lu", wax_ad_ptr->custid) ;
	mark = mark + "_" + string(buff);
	mark = mark + "_" + access_info->exposure_id;
	snprintf(buff, BUFF_LEN, "%d", access_info->appid);
	mark = mark + "_" + string(buff);
	snprintf(buff, BUFF_LEN, "%d", wax_ad_ptr->ad_type) ;
	mark = mark + "_" + string(buff);*/
	snprintf(buff, BUFF_LEN, "%lu_%lu_%lu_%dx%d_%d_%d_%d_%s",  
		wax_ad_ptr->dspid, wax_ad_ptr->custid,
			wax_ad_ptr->itemid, wax_ad_ptr->creative.width, wax_ad_ptr->creative.height, 
				wax_ad_ptr->price, wax_ad_ptr->cost, check_code, 
					wax_ad_ptr->impid.c_str());
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
	if(wax_ad_ptr->impid.length() > 0)
	    pos = wax_ad_ptr->impid.substr(wax_ad_ptr->impid.length() - 1, 1);	
	wax_ad_ptr->mark = pos + "_" + string((char*)mark_enc);
	wax_ad_ptr->mark_origin =pos + "_" + mark;

	//unsigned char mark_buf[1024] = {'\0'};
	//uint32_t len_buf = 1024;
	//ret_aes = global_db_interface->dec_func(mark_enc, len_enc, (unsigned char*)mark_buf, &len_buf);
	//LOG_DEBUG("%s-%s-%d-%d", mark_enc, mark_buf, len_buf,ret_aes);
	return 0;	
	
}
int WaxWorkInterface::aes_enc(WAX_AD*& wax_ad){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint64_t curr_time = tv.tv_sec * 1000 + tv.tv_usec/1000;

	char* auction_price = NULL;
	int nlen = 0;
	nlen = snprintf(auction_price, 0 , "%d_%lu", wax_ad->cost, curr_time);
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
	snprintf(auction_price, len,  "%d_%lu", wax_ad->cost, curr_time);

	AES_KEY aes;
	unsigned char key[AES_KEY_SIZE];
	unsigned char* encrypt_string = new unsigned char[len];
	unsigned char* decrypt_string = new unsigned char[len];
	unsigned char* input_string = (unsigned char*)auction_price;
	if(encrypt_string == NULL or decrypt_string == NULL){
		LOG_ERROR("new memory for enc dec string failed");
		delete [] auction_price;
		return -1;
	}
		
	//for(int i = 0; i< AES_KEY_SIZE; i++)
		//key[i] = 32 + i;
	if(strlen(wax_ad->token.c_str()) == AES_KEY_SIZE)
		memcpy(key, wax_ad->token.c_str(), AES_KEY_SIZE);
	else{
		LOG_ERROR("%lu token len error", wax_ad->dspid);
		delete [] auction_price;
		delete [] encrypt_string;
		delete [] decrypt_string;
		return -1;
	}

	if (AES_set_encrypt_key(key, AES_KEY_SIZE*8, &aes) < 0) {                                              
		LOG_ERROR("Unable to set encryption key in AES\n");                               
		delete [] auction_price;
		delete [] encrypt_string;
		delete [] decrypt_string;
		return -1;
	}

	for(int i = 0; i < len/AES_BUFFER_SIZE; i++)
		AES_ecb_encrypt(input_string + i*AES_BUFFER_SIZE, encrypt_string + i*AES_BUFFER_SIZE, &aes, AES_ENCRYPT);   	

	if (AES_set_decrypt_key(key, AES_KEY_SIZE*8, &aes) < 0) {                                              
		LOG_ERROR("Unable to set encryption key in AES\n");                               
		delete [] auction_price;
		delete [] encrypt_string;
		delete [] decrypt_string;
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
		delete [] auction_price;
		delete [] encrypt_string;
		delete [] decrypt_string;
		return -1;
	}
	for(int i = 0; i < len; i++){
		snprintf(buf, 3, "%x%x", (encrypt_string[i]>>4)&0x0f, encrypt_string[i]&0x0f);
		//LOG_DEBUG("%x%x", (encrypt_string[i]>>4)&0x0f, encrypt_string[i]&0x0f);
		//LOG_DEBUG("%s", buf);
		wax_ad->auction_price = wax_ad->auction_price + string(buf, 2);
	}
	//wax_ad->auction_price = string((char*)encrypt_string, len);	
	LOG_INFO("%s", wax_ad->auction_price.c_str());

	delete [] auction_price;
	delete [] encrypt_string;
	delete [] decrypt_string;
	delete [] buf;
	return 0;	
}
int WaxWorkInterface::enc_ad_attri(ACCESS_INFO*& access_info, VEC_CAND& wax_result_vec){
	VEC_CAND::iterator wax_result_iter;
	int aes_ret = 0;
	for(wax_result_iter = wax_result_vec.begin(); wax_result_iter != wax_result_vec.end(); wax_result_iter++){
		WAX_AD* wax_ad =(WAX_AD*)(*wax_result_iter);
		mark_func(access_info, wax_ad);
		aes_ret = aes_enc(wax_ad);
		if(aes_ret == -1){
			LOG_ERROR("aes_enc failed");
		}
	}
	return 0;
}
int WaxWorkInterface::json_dump(json_object* & resp_json, VEC_CAND& wax_ad_result_vec, 
		string key,  ACCESS_INFO*& access_info, int num_limit)
{
	json_object* ad_result_array = json_object_new_array();
	int count = 0;
	for(VEC_CAND::iterator iter = wax_ad_result_vec.begin(); iter != wax_ad_result_vec.end(); iter++)
	{
		WAX_AD* wax_ad_ptr= (WAX_AD*)(*iter);
		/*if(wax_ad_ptr->adid == 0)
			break;*/
		json_object* jo_ad = json_object_new_object();
		json_object_object_add(jo_ad, "impid", json_object_new_string((char*)wax_ad_ptr->impid.c_str()));
		json_object_object_add(jo_ad, "uniq_id", json_object_new_string((char*)wax_ad_ptr->uniq_id.c_str()));
		json_object_object_add(jo_ad, "dspid", json_object_new_int(wax_ad_ptr->dspid));
		json_object_object_add(jo_ad, "price", json_object_new_int(wax_ad_ptr->price));
		json_object_object_add(jo_ad, "cost", json_object_new_int(wax_ad_ptr->cost));
		json_object_object_add(jo_ad, "mark", json_object_new_string((char*)wax_ad_ptr->mark.c_str()));
		//json_object_object_add(jo_ad, "mark_ori", json_object_new_string((char*)wax_ad_ptr->mark_origin.c_str()));
		json_object_object_add(jo_ad, "auction_price", json_object_new_string((char*)wax_ad_ptr->auction_price.c_str()));
		json_object_object_add(jo_ad, "extra_info", json_object_new_string((char*)wax_ad_ptr->extra_info.c_str()));
		json_object_object_add(jo_ad, "mark_origin", 
			json_object_new_string((char*)wax_ad_ptr->mark_origin.c_str()));
		json_object_object_add(jo_ad, "algo_rank_attribute", 
			json_object_new_string((char*)wax_ad_ptr->algo_rank_attribute.c_str()));
		json_object_object_add(jo_ad, "creativeurl", 
			json_object_new_string(wax_ad_ptr->creative.url));

	/*	if(access_info->debug == 1){
			json_object* jo_kw_list = json_object_new_array();
			for(uint16_t i = 0; i < wax_ad_ptr->keywords_vec.size(); i++){
				json_object* jo_kw = json_object_new_object();
				json_object_object_add(jo_kw, "keyword", json_object_new_string
					((char*)((wax_ad_ptr->keywords_vec[i]).keyword).c_str()));
				json_object_object_add(jo_kw, "weight", json_object_new_int(
					(wax_ad_ptr->keywords_vec[i]).weight));
				json_object_object_add(jo_kw, "postagid", json_object_new_int(
					(wax_ad_ptr->keywords_vec[i]).postagid));
				json_object_object_add(jo_kw, "src_type", json_object_new_int(
					(wax_ad_ptr->keywords_vec[i]).type));
				json_object_array_add(jo_kw_list, jo_kw);
			}
			json_object_object_add(jo_ad, "keywords", jo_kw_list);
			json_object* jo_cate_list = json_object_new_array();
			for(uint16_t i = 0; i < wax_ad_ptr->category_vec.size(); i++){
				json_object* jo_cate = json_object_new_object();
				json_object_object_add(jo_cate, "catecode", 
					json_object_new_int((wax_ad_ptr->category_vec[i]).cate_code));
				json_object_object_add(jo_cate, "weight", 
					json_object_new_int((wax_ad_ptr->category_vec[i]).weight));
				json_object_object_add(jo_cate, "level", 
					json_object_new_int((wax_ad_ptr->category_vec[i]).level));
				json_object_object_add(jo_cate, "src_type", 
					json_object_new_int((wax_ad_ptr->category_vec[i]).src_type));
				json_object_array_add(jo_cate_list, jo_cate);
			}
			json_object_object_add(jo_ad, "category", jo_cate_list);
			json_object_object_add(jo_ad, "adid_expo_num", json_object_new_int(
				wax_ad_ptr->adid_expo_num));
			

		}*/
		json_object_array_add(ad_result_array, jo_ad);

		count++;
		//json_object_put(jo_ad);
	}
	json_object_object_add(resp_json, key.c_str(), ad_result_array);

	return 0;
}
int WaxWorkInterface::return_process(ACCESS_INFO*& access_info,
	VEC_CAND& wax_ad_vec,VEC_CAND& wax_ad_result_vec, char* &p_out_temp, int& n_out_len)
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
		if(wax_ad_result_vec.size() > 0)
			return_code = 1;

		json_object* resp_json = json_object_new_object();

		json_object* jo_return_code = json_object_new_int(return_code);
		json_object* jo_host = json_object_new_string((char*)host.c_str());
		json_object_object_add(resp_json, "return_code", jo_return_code);
		json_object_object_add(resp_json, "host", jo_host);

		string key = "result";
		json_dump(resp_json, wax_ad_result_vec, key, access_info, 1);
		int debug = access_info->debug;
		if(debug){
			json_object* debug_json = json_object_new_object();
			json_dump(debug_json, wax_ad_vec, "wax_ad_vec", access_info, 0);
			
			//json_object* freq_json = json_object_new_object();
			/*std::map<uint64_t,uint32_t>::iterator iter_adid2expoNum_map;
			for(iter_adid2expoNum_map = adid2expoNum_map.begin(); iter_adid2expoNum_map != 
				adid2expoNum_map.end(); iter_adid2expoNum_map++){
				char adid[50], freq_num[20];
				snprintf(adid, 50, "%lu", iter_adid2expoNum_map->first);
				snprintf(freq_num, 20, "%d", iter_adid2expoNum_map->second);
				json_object_object_add(freq_json, adid, json_object_new_string(freq_num));
			}*/
			//json_object_object_add(debug_json, "freq", freq_json);

			json_object_object_add(resp_json, "debug", debug_json);
		}	
		
		string resp_str = json_object_to_json_string(resp_json);
		int len = sprintf(p_out_temp, "%s", resp_str.c_str());
		n_out_len = len;
		LOG_INFO("%s", p_out_temp);

		json_object_put(resp_json);

		return 0;
}
int WaxWorkInterface::release(VEC_CAND& wax_ad_vec){
	for(size_t i = 0; i < wax_ad_vec.size(); i++){
		delete wax_ad_vec[i];
	}
	return 0;
}
int WaxWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
	
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

		VEC_CAND wax_ad_vec;
		VEC_CAND wax_ad_result_vec;
		//char* p_out_temp = p_out_string;
		json_object* body_json = json_object_object_get(req_json, "body");
		int ret_init = 0;
		if(NULL == body_json){
			delete access_info;
			LOG_ERROR("require json body is null");
			result = -1;
			return return_fail(406, "require json body is null", p_out_string, n_out_len);
		}else{
			ret_init = init_attri_by_req(body_json, access_info, wax_ad_vec);
			if(ret_init != 0){
				delete access_info;
				result = -1;
				return return_fail(406, "init error", p_out_string, n_out_len);
			}
				
		}

		int req_alg = 0;	
		req_alg = algorithm_core(req_id, access_info, "WAXAD", wax_ad_vec, wax_ad_result_vec);
		if(req_alg != 1){
			delete access_info;
			result = -1;
			return return_fail(406, "alg error", p_out_string, n_out_len);
		}

		int ret_enc = 0;
		ret_enc = enc_ad_attri(access_info, wax_ad_result_vec);		
		if(ret_enc != 0){
			delete access_info;
			result = -1;
			return return_fail(406, "encrypt error", p_out_string, n_out_len);
		}

		int ret = 0;
		ret = return_process(access_info, wax_ad_vec, wax_ad_result_vec, p_out_string, n_out_len);
		if(ret != 0){
			delete access_info;
			result = -1;
			return return_fail(406, "return_process error", p_out_string, n_out_len);
		}

		delete access_info;
		release(wax_ad_vec);	
	}
	else{
	 	result = -1;
        return return_fail(406, "can not support cmd!", p_out_string, n_out_len);	
	}

	return result;
}
