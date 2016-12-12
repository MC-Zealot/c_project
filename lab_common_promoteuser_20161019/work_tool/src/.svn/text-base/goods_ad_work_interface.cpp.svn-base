#include "goods_ad_work_interface.h"

DYN_WORK(GoodsAdWorkInterface);

int GoodsAdWorkInterface::get_ip_code(const string& ip)
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




int GoodsAdWorkInterface::init_attri_by_req(json_object* &body_json, ACCESS_INFO* access_info){

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
			 access_info->uid_ = json_object_get_int(jo_uid);

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
			 access_info->device_type = json_object_get_int(jo_device_type);
	}

	//LOG_DEBUG("uid is :%s;ip is :%s;gender is :%d;age is :%d;device_type :%d;", access_info->uid.c_str(), access_info->ip.c_str(), access_info->gender, access_info->age, access_info->device_type);
	
	json_object* jo_weibo_cate= json_object_object_get(body_json, "weibo_cate");
	if(NULL == jo_weibo_cate)
		access_info->weibo_cate = "0";
	else
		access_info->weibo_cate = string(json_object_get_string(jo_weibo_cate));
	//LOG_DEBUG("weibo_cate:%s", access_info->weibo_cate.c_str());

	json_object* jo_keywords = json_object_object_get(body_json, "weibo_keywords");
	if(NULL == jo_keywords){
			//access_info->keywords_vec.clear();
	}
	else{
		int array_len =  json_object_array_length(jo_keywords);
		for (int i = 0; i< array_len; i++){
			json_object* jo_one_keyword =  json_object_array_get_idx(jo_keywords, i);
			if(NULL == jo_one_keyword){
				continue;
			}
			else{
				KEYWORDS keywords;
				json_object* jo_keyword = json_object_object_get(jo_one_keyword, "keyword");
				if(NULL == jo_keyword)
					 continue;
				else
					keywords.keyword = json_object_get_string(jo_keyword);
				json_object* jo_weight = json_object_object_get(jo_one_keyword, "value");
				if(NULL == jo_weight)
					continue;
				else
					keywords.weight = json_object_get_int(jo_weight);
				json_object* jo_postagid = json_object_object_get(jo_one_keyword, "postagid");
				if(NULL == jo_postagid)
					continue;
				else
					keywords.postagid = json_object_get_int(jo_postagid);
				access_info->keywords_vec.push_back(keywords);
			}
		}	
				
	}
	return 0;
}
int GoodsAdWorkInterface::set_expo_freq(std::vector<uint64_t>& result_adid, ACCESS_INFO* access_info)
{
	int adid_num = result_adid.size();
	uint64_t* req_buff = new uint64_t[FREQ_MSG_HEAD_LEN + adid_num];
	if(req_buff == NULL){
		LOG_ERROR("new req_buff memory  in set freq failed!");
		return -1;
	}
	//uint64_t req_buff[3];

	req_buff[0] = strtoul(access_info->uid.c_str(), NULL, 10);	
	LOG_INFO("uid:%lu", req_buff[0]);
	
	uint64_t op = ((3&0xFF)<<24 )+ (0x81&0xFFFFFF);
	uint64_t op_num = (op<<32) | (1&0xFFFFFFFF);

	req_buff[1] = op_num;
	//req_buff[2] = adid;

	std::vector<uint64_t>::iterator iter;
	int i = FREQ_MSG_HEAD_LEN;
	for(iter = result_adid.begin(); iter != result_adid.end(); iter++){
		req_buff[i] = *iter;
		LOG_INFO("adid:%d", req_buff[i]);
		i++;
	}	

	string req;
	req=string((char*) &req_buff[0], (FREQ_MSG_HEAD_LEN + adid_num)*8);	

        string p_result;
        char split_char, second_split_char;
         //LOG_DEBUG("%s:%d:%d:%d", req.c_str(), req.length(),  req_buff[0], req_buff[2]);

        DbInterface* p_freq_db_interface = p_db_company_->get_db_interface("FREQ_CONTROL");
	NspioDbInterface* p_freq_db_interface_nspio = (NspioDbInterface*) p_freq_db_interface;
        if (NULL == p_freq_db_interface){
		delete [] req_buff;
                return -1;
	}

        int result =  p_freq_db_interface_nspio->s_get_str(0, req, p_result, split_char, second_split_char);

        if ( result < 0) {
		delete [] req_buff;
                LOG_ERROR("set freq result error");
                return -1;
        }
	LOG_INFO("set return str len:%d", p_result.size());
	delete [] req_buff;
	/*const uint32_t *res_int;
	res_int = reinterpret_cast<const uint32_t *>(p_result.c_str());
	uint32_t set_status = (uint32_t)res_int[0];
	LOG_DEBUG("set_status:%d", set_status);
	
	if (set_status == 0)
		return -1;*/
	return 0;
}
int GoodsAdWorkInterface::get_expo_freq(VEC_CAND& goods_ad_vec, ACCESS_INFO* access_info)
{
	uint32_t adid_num = goods_ad_vec.size();
	uint64_t* req_buff = new uint64_t[FREQ_MSG_HEAD_LEN + adid_num];
	if(req_buff == NULL){
		LOG_ERROR("new req_buff memory failed!");
		return -1;
	}

	req_buff[0] = strtoul(access_info->uid.c_str(), NULL, 10);
	LOG_INFO("uid:%lu;adid_num:%d", req_buff[0], adid_num);
	uint64_t op = ((1&0xFF)<<24 )+ (0x80&0xFFFFFF);
	uint64_t op_num = (op<<32) | (adid_num&0xFFFFFFFF);

	req_buff[1] = op_num;
	
	int i = FREQ_MSG_HEAD_LEN;	
	//std::map<uint64_t,uint32_t>::iterator iter_map;		
	//for(iter_map = adid2expoNum_map.begin(); iter_map != adid2expoNum_map.end(); iter_map++){
	for(VEC_CAND::iterator iter = goods_ad_vec.begin(); iter != goods_ad_vec.end(); iter++){
		GOODS_AD* goods_ad_ptr= (GOODS_AD*)(*iter);
		req_buff[i] = goods_ad_ptr->adid;
		LOG_INFO("adid:%lu", req_buff[i]);
		i++;
	}	
	//for(int i =0; i < 3; i++)
	//	req_buff[i] = 0x0000000000000000;
	string req;
	req=string((char*) req_buff, (FREQ_MSG_HEAD_LEN + adid_num)*8);	

        string p_result;
        char split_char, second_split_char;
         //LOG_DEBUG("%s:%d:%d:%d", req.c_str(), req.length(),  req_buff[0], adid_num);

        DbInterface* p_freq_db_interface = p_db_company_->get_db_interface("FREQ_CONTROL");
	NspioDbInterface* p_freq_db_interface_nspio = (NspioDbInterface*) p_freq_db_interface;
        if (NULL == p_freq_db_interface){
		delete [] req_buff;
                return -1;
	}

        int result =  p_freq_db_interface_nspio->s_get_str(0, req, p_result, split_char, second_split_char);

        if ( result < 0) {
                LOG_ERROR("get freq result error");
		delete [] req_buff;
                return -1;
        }
	uint16_t head_size = FREQ_MSG_HEAD_LEN*4;
	if(p_result.size() < head_size){
		LOG_ERROR("get expo: p_result size < 8 bytes");
		delete [] req_buff;
		return -1;
	}

	const uint32_t *res_int = NULL;
	res_int = reinterpret_cast<const uint32_t *>(p_result.c_str());
	if(NULL == res_int){
		LOG_ERROR("get expo: res_int ptr reinterpret_cast failed");
		delete [] req_buff;
		return -1;
	}
	uint32_t uid_num = (uint32_t)res_int[0];
	access_info->user_expo_num = uid_num;	
	uint32_t count = (uint32_t)res_int[1];

	uint32_t body_size = adid_num*4;
	if (count != adid_num){
		delete [] req_buff;
		LOG_ERROR("get expo: p_result count less then adid_num reqed");
		return -1;
	}
	if(p_result.size() < (head_size + body_size)){
		LOG_ERROR("get expo: p_result size less than head_size + body_size");
		delete [] req_buff;
		return -1;
	}
	int freq_num = 0;
	uint32_t vec_index = 0;
	for(uint32_t i=FREQ_MSG_HEAD_LEN; i<(adid_num + FREQ_MSG_HEAD_LEN); i++){
		 freq_num = (uint32_t)res_int[i];
		LOG_INFO("adid:%lu;freq_num:%d",req_buff[i],  freq_num);
		//adid2expoNum_map[req_buff[i]] = freq_num;
		if(vec_index < goods_ad_vec.size())
			((GOODS_AD*)goods_ad_vec[vec_index])->adid_expo_num = freq_num;	
		vec_index++;
		freq_num = 0;
	}

	LOG_INFO("user_expo_num:%d;adid_num:%d", uid_num, count);
	delete [] req_buff;

	return 0;
}
int GoodsAdWorkInterface::get_goodsAd_from_index_new(VEC_CAND& goods_ad_vec, ACCESS_INFO* access_info)
{
	//GOODS_AD* goods_ad = new GOODS_AD();
	//goods_ad->adid = 101; 
	//goods_ad_vec.push_back(goods_ad);
	if(NULL == p_db_company_){
		LOG_ERROR("db_company is NULL!");
		return -1;
	}

	WooDbInterface *ad_index_db_interface = 
		(WooDbInterface*)p_db_company_->get_db_interface("GOODS_AD_INDEX_DB");
	
	if(NULL == ad_index_db_interface){
		LOG_ERROR("ad index db_interface is NULL!");
		return -1;
	}


	//from wangli index attributes
	uint64_t uid = strtoul(access_info->uid.c_str(), NULL, 10);
	uint16_t age = 202;
	uint16_t location = 302;
	uint16_t gender = 401;
	uint16_t psid = 1304;
	uint16_t interests_array[4] = {101, 102, 103, 104};

//	age = access_info->age;
//	location = access_info->ip_code;
	

	char req_str[COMPRESS_LEN];
	char* p_req = req_str;
	int len = 0;
	len = sprintf(p_req, "{\"api\":\"adindex\", \"cmd\":\"find_indexes\", \"db\":\"GOODS_AD_INDEX_DB\", \"item_keys\":[\"keywords\", \"category\",\"price\",\"price_ceiling\",\"budget\"], ");
	p_req += len;

    len = sprintf(p_req, "\"age\":[%"PRIu16"],", age);
	p_req += len;
	
	len = sprintf(p_req, "\"location\":[%"PRIu16"],", location);
	p_req += len;

	len = sprintf(p_req, "\"gender\":[%"PRIu16"],", gender);
	p_req += len;

	len = sprintf(p_req, "\"psid\":[%"PRIu16"],", psid);
	p_req += len;

	len = sprintf(p_req, "\"interest\":[");
	p_req += len;

	for(size_t i = 0; i < 4; i++){
		if(i == 0)
			len = sprintf(p_req, "%"PRIu16, interests_array[i]);
		else
			len = sprintf(p_req, ",%"PRIu16, interests_array[i]);

		p_req += len;
	}

	len = sprintf(p_req, "]}");
	p_req += len;

	char* p_result = NULL;
	char split_char, second_split_char;

	int result = ad_index_db_interface->s_get(0, req_str, p_result, split_char, second_split_char, uid);
	if(result > 0 && p_result != NULL){
		LOG_INFO("%s", p_result);

		json_object* request_json = json_tokener_parse(p_result);
		if(request_json != NULL && !is_error(request_json)){
		
			json_object* results_json = json_object_object_get(request_json, "result");

			if(json_object_is_type(results_json, json_type_array)){
				int n_array_len = json_object_array_length(results_json);
				for(int i = 0; i < n_array_len; i ++){
					json_object* ad_object = json_object_array_get_idx(results_json, i);
				
					if(NULL == ad_object || is_error(ad_object))
						continue;

					json_object* ad_id_object = json_object_object_get(ad_object, "ad_id");
					if(NULL == ad_id_object || is_error(ad_id_object))
						continue;
					uint32_t ad_id = (uint32_t)json_object_get_int(ad_id_object);
					//LOG_ERROR("%"PRIu32, ad_id);

					json_object* ad_status_object = json_object_object_get(ad_object, "ad_status");
					if(NULL == ad_status_object || is_error(ad_status_object))
						continue;
					uint16_t ad_status = 0;
					ad_status = (uint16_t)json_object_get_int(ad_status_object);
					//LOG_ERROR("%"PRIu16, ad_status);
					
					json_object* ad_attr_object = json_object_object_get(ad_object, "attr");
					if(NULL == ad_attr_object || is_error(ad_attr_object))
						continue;

					json_object* ad_price_object = json_object_object_get(ad_attr_object, "price");
					if(NULL == ad_price_object || is_error(ad_price_object))
						continue;
					float price = (float)json_object_get_double(ad_price_object);
					//LOG_ERROR("%f", price);
					
					json_object* keywords_object = json_object_object_get(ad_attr_object, "keywords");
					if(NULL == keywords_object || is_error(keywords_object))
						continue;
					vector<KEYWORDS> keywords_vec;
					json_object_object_foreach(keywords_object, key_str, val){
						const char* keyword = json_object_get_string(val);

						//split keyword
						KEYWORDS keywords;
						string keyword_str(keyword);
						char* k_ptr = strtok((char*)keyword_str.c_str(), ":");
						if(k_ptr){
							//LOG_DEBUG("%s", k_ptr);
							keywords.keyword = string(k_ptr);
						}
						k_ptr = strtok(NULL, ":");
						if(k_ptr){
							//LOG_DEBUG("%s", k_ptr);
							keywords.weight = strtoul(k_ptr, NULL, 10);
						}
						k_ptr = strtok(NULL, ":");
						if(k_ptr){
							//LOG_DEBUG("%s", k_ptr);
							keywords.postagid = strtoul(k_ptr, NULL, 10);
						}
						k_ptr = strtok(NULL, ":");
						if(k_ptr){
							//LOG_DEBUG("%s", k_ptr);
							keywords.type = strtoul(k_ptr, NULL, 10);
						}
						keywords_vec.push_back(keywords);	
					//	LOG_ERROR("%s", keyword);
					}

					json_object* category_object = json_object_object_get(ad_attr_object, "category");
					if(NULL == category_object || is_error(category_object))
						continue;
					vector<CATEGORY> category_vec;
					json_object_object_foreach(category_object, category_key_str, category_val){
						const char* category = json_object_get_string(category_val);
						CATEGORY cate;	
						string cate_str(category);
						char* c_ptr = strtok((char*)cate_str.c_str(), ":");
						c_ptr = strtok(NULL, ":");
						if(c_ptr){
							//LOG_DEBUG("%s", c_ptr);
							cate.cate_code = strtoul(c_ptr, NULL, 10);
						}
						c_ptr = strtok(NULL, ":");
						if(c_ptr){
							cate.weight = (uint16_t)(strtod(c_ptr, NULL)*10000);
						}
						cate.level = 1;
						cate.src_type = 0;
						category_vec.push_back(cate);
						//split category
						//LOG_ERROR("%s", category);
					}
					GOODS_AD* goods_ad = new GOODS_AD();
					goods_ad->adid = ad_id;
					goods_ad->custid = 19281314;
					goods_ad->ad_type = 10;
					goods_ad->ad_cate = "weibomai";
					goods_ad->url = "http://weibo.com/p/100140Ee13380";
					goods_ad->url_md5 = "100140Ee13380";
					goods_ad->bid_type = 2;
					goods_ad->price = (uint32_t)(price*10000); 
					goods_ad->price_limited = 100000; 
					goods_ad->price_type = 1;
					goods_ad->keywords_vec = keywords_vec;
					goods_ad->category_vec = category_vec;
					goods_ad_vec.push_back(goods_ad);
					
				}
			}
			else{

				LOG_ERROR("result is not array!");
			}

		}else{
			LOG_ERROR("request is null!");
		}

		json_object_put(request_json);

	}else{
		LOG_ERROR("%d", result);
	}
	
	return 1;
}
/*string GoodsAdWorkInterface::get_target_info(ACCESS_INFO* &access_info, uint32_t cate_code){

	string target_info;
	char buff[20];
	int ret = 0;
	ret = snprintf(buff, 20, "%d", access_info->gender);
	//LOG_INFO("%d", ret);
	target_info = string(buff) + ";";
	ret = snprintf(buff, 20, "%d", access_info->age);
	target_info = target_info + string(buff) + ";";
	ret = snprintf(buff, 20, "%lu", access_info->ip_code);
	target_info = target_info + string(buff) + ";";
	target_info = target_info  + ";";
	target_info = target_info + ";";
	ret = snprintf(buff, 20, "%d", cate_code);
	target_info = target_info + string(buff) + ";";
	ret = snprintf(buff, 20, "%d", access_info->device_type);
	target_info = target_info + string(buff) + ";";
//	target_info = target_info + access_info->psid + ";";
	target_info = target_info + "|" + "|" + "|";
	LOG_DEBUG("%s", target_info.c_str());
	return target_info;
	
	
}*/
int GoodsAdWorkInterface::get_goodsAd_from_index(VEC_CAND& goods_ad_vec, ACCESS_INFO* access_info)
{

	uint64_t uid = strtoul(access_info->uid.c_str(), NULL, 10);
        access_info->ip_code = get_ip_code(access_info->ip);
        LOG_INFO("uid:%lu;gender:%d;age:%d;ip code:%d;psid:%s;ad_num:%d;ad_type:%d;device_type:%d", uid, 
		access_info->gender, access_info->age, access_info->ip_code,access_info->psid.c_str(), AD_NUM,
			AD_TYPE, access_info->device_type);
        goods_interface::AdEng2IdxSrvMsg req_msg;
        goods_interface::UserAttr* user_attr = req_msg.mutable_userattr();
        user_attr->set_gender(access_info->gender);
        user_attr->set_age(access_info->age);
        req_msg.set_psid(access_info->psid);
        req_msg.set_location(access_info->ip_code);
        req_msg.set_uid(uid);
        req_msg.set_adnum(AD_NUM);
        req_msg.set_adtype(AD_TYPE);
        req_msg.set_device(access_info->device_type);

	string req_str= "";
	req_msg.SerializeToString(&req_str);
	
	ms_dsad::MsidxRequest req_head;
	req_head.set_product_type(AD_TYPE);
	req_head.set_product_request_content(req_str);

	string req_pack = "";
	req_head.SerializeToString(&req_pack);
	//LOG_DEBUG("index req:%s", req_pack.c_str());
	//release
	//req_msg.release_userattr();
	//req_msg.release_psid();


	string p_result;	
	//char* p_result = NULL;
        char split_char, second_split_char;
        DbInterface* p_ad_index_db_interface = p_db_company_->get_db_interface("INDEX_SVR");
	SpioDbInterface* p_ad_index_db_interface_spio = (SpioDbInterface*) p_ad_index_db_interface;
        if (NULL == p_ad_index_db_interface){
                return -1;
	}
        if (NULL == p_ad_index_db_interface_spio){
                return -1;
	}

        int result =  p_ad_index_db_interface_spio->s_get_str(0, req_pack, p_result, split_char, 
		second_split_char);

        if ( result < 0) {
                LOG_ERROR("get spio result error");
                return -1;
        }
	//LOG_DEBUG("resp:%s, size:%d", p_result.c_str(), p_result.size());
	
	goods_interface::IdxSrv2AdEngMsg res_msg;
	int ret = res_msg.ParseFromString(p_result);
	if(ret == 0){
		LOG_ERROR("parse index msg error");
		return -1;
	}
	int status = res_msg.status();
	if(status != 0){
		LOG_ERROR("parse index status msg error:%d",status);
		return -1;
	}
	//LOG_DEBUG("parse index status:%d", status);
	goods_interface::FeedInfo feedinfo;
	for(int i=0; i < res_msg.feedlist_size(); i++){
	//	GOODS_AD goods_ad;
		GOODS_AD* goods_ad = new GOODS_AD();
		if(NULL == goods_ad){
			LOG_ERROR("new goods_ad memory failed!");
			continue;
		}
		feedinfo = res_msg.feedlist(i);
		if(feedinfo.has_custid())
			goods_ad->custid = feedinfo.custid();
		LOG_DEBUG("custid:%ld", goods_ad->custid);
		if(feedinfo.has_adid())
			goods_ad->adid = feedinfo.adid();
		LOG_DEBUG("adid:%ld", goods_ad->adid);
		if(feedinfo.has_price())
			goods_ad->price = feedinfo.price();
		if(feedinfo.has_bidtype()){
			if(feedinfo.bidtype() == 1)
				goods_ad->bid_type = BID_CPM;
			else if(feedinfo.bidtype() == 2)
				goods_ad->bid_type = BID_CPE;
			LOG_DEBUG("bid_type_index:%d", feedinfo.bidtype());
		}
		LOG_DEBUG("bid_type:%d", goods_ad->bid_type);
		if(feedinfo.has_pricetype())
			goods_ad->price_type = feedinfo.pricetype();
		LOG_DEBUG("price_type:%d", goods_ad->price_type);
		if(feedinfo.has_royalty_price())
			goods_ad->royalty_price = feedinfo.royalty_price();
		goods_interface::AdWordPair ad_word_pair;
		for(int j = 0; j< feedinfo.keywordlist_size(); j++){
			KEYWORDS keywords;
			 ad_word_pair = feedinfo.keywordlist(j);
			if(ad_word_pair.has_keywords()){
				keywords.keyword = ad_word_pair.keywords();	
				//LOG_DEBUG("keyword:%s", keywords.keyword.c_str());
			}
			if(ad_word_pair.has_weight())
				keywords.weight = ad_word_pair.weight();	
			if(ad_word_pair.has_postigid())
				keywords.postagid = ad_word_pair.postigid();	
			if(ad_word_pair.has_src_type())
				keywords.type = ad_word_pair.src_type();	
			goods_ad->keywords_vec.push_back(keywords);
			//ad_word_pair.release_keywords();
		}
			
		goods_interface::Category category;
		//CATEGORY category_local;
		uint32_t cate_code = 0;
		vector<uint32_t> cate_code_vec;
		string cate_code_str = "";
		char cate_code_buf[10];
		for(int j = 0; j< feedinfo.categorylist_size(); j++){
			CATEGORY category_local;
			category = feedinfo.categorylist(j);
			if(category.has_cate_code())
				category_local.cate_code = category.cate_code();	
			if(category.has_weight())
				category_local.weight = category.weight();	
			if(category.has_level())
				category_local.level = category.level();	
			if(category.has_src_type()){
				category_local.src_type = category.src_type();	
				if(category.src_type() == 0 && category.has_cate_code()){
					cate_code = category.cate_code();
					cate_code_vec.push_back(cate_code);
				}
			}
			goods_ad->category_vec.push_back(category_local);
		}
		uint16_t k_count = 0;
		for(uint16_t k = 0; k < cate_code_vec.size(); k++){
			snprintf(cate_code_buf, 10, "%d", cate_code);
			LOG_DEBUG("cate_code:%d", cate_code);
			k_count = k + 1;
			if(k_count < cate_code_vec.size())
				cate_code_str = cate_code_str + string(cate_code_buf) + ",";
			else
				cate_code_str = cate_code_str + string(cate_code_buf);
		}
			
		std::map<string, uint64_t> key_value_vec;
		goods_interface::KeyValue key_value;
		for(int j = 0; j< feedinfo.key_val_list_size(); j++){
			key_value = feedinfo.key_val_list(j);
			if(key_value.has_key() && key_value.has_value()){
				key_value_vec.insert(std::make_pair(key_value.key(), key_value.value()));
				LOG_DEBUG("key:%s; value:%d", key_value.key().c_str(), key_value.value());
			}
			//key_value.release_key();//release
		}
		//goods_ad->ad_target_info = get_target_info(access_info, cate_code);
		//memset(goods_ad->ad_target_info, '\0', 100);
        	snprintf(goods_ad->ad_target_info, AD_TARGET_INFO_LEN, "%d;%d;%lu;;;%s;%d;|||", 
			access_info->gender, access_info->age, access_info->ip_code, cate_code_str.c_str(), 
				access_info->device_type);
		LOG_DEBUG("%s",  goods_ad->ad_target_info);
		if(feedinfo.has_url())
			goods_ad->url = feedinfo.url();
		if(feedinfo.has_score())
			goods_ad->score = feedinfo.score();
		if(feedinfo.has_url_md5())
			goods_ad->url_md5 = feedinfo.url_md5();
		if(feedinfo.has_ad_cate())
			goods_ad->ad_cate = feedinfo.ad_cate();
		goods_ad->ad_type = AD_TYPE;
		goods_ad->bucketid = 0;
		if(feedinfo.has_price_limited())
			goods_ad->price_limited = feedinfo.price_limited();
		if(feedinfo.has_matchlevel())
			goods_ad->matchlevel = feedinfo.matchlevel();
		LOG_DEBUG("matchlevel:%d", goods_ad->matchlevel);
		goods_ad_vec.push_back(goods_ad);
		//release
		//feedinfo.release_url();
		//feedinfo.release_url_md5();
		//feedinfo.release_ad_cate();
	}
	return 0;
}
int GoodsAdWorkInterface::json_dump(json_object* & resp_json, VEC_CAND& goods_ad_result_vec, 
		string key,  ACCESS_INFO* access_info, int num_limit)
{
	json_object* ad_result_array = json_object_new_array();
	int count = 0;
	for(VEC_CAND::iterator iter = goods_ad_result_vec.begin(); iter != goods_ad_result_vec.end(); iter++)
	{
		GOODS_AD* goods_ad_ptr= (GOODS_AD*)(*iter);
		/*if(goods_ad_ptr->adid == 0)
			break;*/
		if(num_limit == 1){
			if(count >= access_info->num)
				break;
		}
		json_object* jo_ad = json_object_new_object();
		json_object_object_add(jo_ad, "adid", json_object_new_int(goods_ad_ptr->adid));
		json_object_object_add(jo_ad, "url", json_object_new_string((char*)goods_ad_ptr->url.c_str()));
		json_object_object_add(jo_ad, "url_md5", 
			json_object_new_string((char*)goods_ad_ptr->url_md5.c_str()));
		json_object_object_add(jo_ad, "ad_type", json_object_new_int(goods_ad_ptr->ad_type));
		json_object_object_add(jo_ad, "bid_type", json_object_new_int(goods_ad_ptr->bid_type));
		json_object_object_add(jo_ad, "price", json_object_new_int(goods_ad_ptr->price));
		json_object_object_add(jo_ad, "cost", json_object_new_int(goods_ad_ptr->cost));
		json_object_object_add(jo_ad, "score", json_object_new_int(goods_ad_ptr->score));
		json_object_object_add(jo_ad, "extra_info", 
			json_object_new_string((char*)goods_ad_ptr->extra_info.c_str()));
		json_object_object_add(jo_ad, "mark", json_object_new_string((char*)goods_ad_ptr->mark.c_str()));
		json_object_object_add(jo_ad, "mark_origin", 
			json_object_new_string((char*)goods_ad_ptr->mark_origin.c_str()));
		json_object_object_add(jo_ad, "ad_target_info",
			 json_object_new_string(goods_ad_ptr->ad_target_info));
		//json_object_object_add(jo_ad, "ad_target_info", json_object_new_string((char*)goods_ad_ptr->ad_target_info.c_str()));
		//json_object_object_add(jo_ad, "ad_target_info", json_object_new_string((char*)goods_ad_ptr->ad_target_info.c_str()));
		json_object_object_add(jo_ad, "algo_rank_attribute", 
			json_object_new_string((char*)goods_ad_ptr->algo_rank_attribute.c_str()));
		json_object_object_add(jo_ad, "loc", json_object_new_int(access_info->ip_code));
		json_object_object_add(jo_ad, "deliver_type", json_object_new_int(goods_ad_ptr->ad_type));
		json_object_object_add(jo_ad, "ad_cate", 
			json_object_new_string((char*)goods_ad_ptr->ad_cate.c_str()));
		json_object_object_add(jo_ad, "bucketid", json_object_new_int(goods_ad_ptr->bucketid));
		json_object_object_add(jo_ad, "price_limited", json_object_new_int(goods_ad_ptr->price_limited));

		if(access_info->debug == 1){
			json_object* jo_kw_list = json_object_new_array();
			for(uint16_t i = 0; i < goods_ad_ptr->keywords_vec.size(); i++){
				json_object* jo_kw = json_object_new_object();
				json_object_object_add(jo_kw, "keyword", json_object_new_string
					((char*)((goods_ad_ptr->keywords_vec[i]).keyword).c_str()));
				json_object_object_add(jo_kw, "weight", json_object_new_int(
					(goods_ad_ptr->keywords_vec[i]).weight));
				json_object_object_add(jo_kw, "postagid", json_object_new_int(
					(goods_ad_ptr->keywords_vec[i]).postagid));
				json_object_object_add(jo_kw, "src_type", json_object_new_int(
					(goods_ad_ptr->keywords_vec[i]).type));
				json_object_array_add(jo_kw_list, jo_kw);
			}
			json_object_object_add(jo_ad, "keywords", jo_kw_list);
			json_object* jo_cate_list = json_object_new_array();
			for(uint16_t i = 0; i < goods_ad_ptr->category_vec.size(); i++){
				json_object* jo_cate = json_object_new_object();
				json_object_object_add(jo_cate, "catecode", 
					json_object_new_int((goods_ad_ptr->category_vec[i]).cate_code));
				json_object_object_add(jo_cate, "weight", 
					json_object_new_int((goods_ad_ptr->category_vec[i]).weight));
				json_object_object_add(jo_cate, "level", 
					json_object_new_int((goods_ad_ptr->category_vec[i]).level));
				json_object_object_add(jo_cate, "src_type", 
					json_object_new_int((goods_ad_ptr->category_vec[i]).src_type));
				json_object_array_add(jo_cate_list, jo_cate);
			}
			json_object_object_add(jo_ad, "category", jo_cate_list);
			json_object_object_add(jo_ad, "adid_expo_num", json_object_new_int(
				goods_ad_ptr->adid_expo_num));
			

		}
		json_object_array_add(ad_result_array, jo_ad);

		count++;
		//json_object_put(jo_ad);
	}
	json_object_object_add(resp_json, key.c_str(), ad_result_array);

	return 0;
}
int GoodsAdWorkInterface::return_process(ACCESS_INFO*& access_info, VEC_CAND& goods_ad_vec,
	VEC_CAND& goods_ad_freqCtrl_vec,VEC_CAND& goods_ad_result_vec, 
		char* &p_out_temp, int& n_out_len, int ret_process)
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
		if(goods_ad_result_vec.size() > 0 &&  access_info->num != 0)
			return_code = 1;

		json_object* resp_json = json_object_new_object();

		json_object* jo_return_code = json_object_new_int(return_code);
		json_object* jo_host = json_object_new_string((char*)host.c_str());
		json_object_object_add(resp_json, "return_code", jo_return_code);
		json_object_object_add(resp_json, "host", jo_host);

		string key = "ad_result";
		json_dump(resp_json, goods_ad_result_vec, key, access_info, 1);
		int debug = access_info->debug;
		if(debug){
			json_object* debug_json = json_object_new_object();
			json_dump(debug_json, goods_ad_vec, "ad_index_vec", access_info, 0);
			json_dump(debug_json, goods_ad_freqCtrl_vec, "ad_freqCtrl_vec", access_info, 0);
			json_dump(debug_json, goods_ad_result_vec, "ad_result_vec", access_info, 0);
			
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
			json_object_object_add(debug_json, "userExpoNum", json_object_new_int(access_info->user_expo_num));

			json_object_object_add(resp_json, "debug", debug_json);
		}	
		
		string resp_str = json_object_to_json_string(resp_json);
		int len = sprintf(p_out_temp, "%s", resp_str.c_str());
		n_out_len = len;
		LOG_INFO("%s", p_out_temp);

		json_object_put(resp_json);

		return 0;
}
int  GoodsAdWorkInterface::release(VEC_CAND& vec_cand){
	for(VEC_CAND::iterator iter = vec_cand.begin(); iter != vec_cand.end(); iter++){
		/*GOODS_AD* goods_ad_ptr =(GOODS_AD*) ( *iter);
        	if(NULL != goods_ad_ptr){
            		delete goods_ad_ptr;
        	}*/
		delete (*iter);
	}
			
	return 0;
}
int GoodsAdWorkInterface::mark_func(ACCESS_INFO* access_info, GOODS_AD*& goods_ad_result_ptr){
	//GOODS_AD* goods_ad_result_ptr = (GOODS_AD*)goods_ad_result_vec[0];
	string mark;

	//mark = pos;
	const int BUFF_LEN = MARK_BUFF_SIZE;	
	char buff[BUFF_LEN];
	/*snprintf(buff, BUFF_LEN, "%lu", goods_ad_result_ptr->adid) ;
	//mark = string(buff);
	mark = mark + "_" + string(buff);
	mark = mark + "_" + goods_ad_result_ptr->url_md5;
	snprintf(buff, BUFF_LEN, "%d", goods_ad_result_ptr->cost) ;	
	mark = mark + "_" + string(buff);
	snprintf(buff, BUFF_LEN, "%d", goods_ad_result_ptr->bid_type) ;	
	mark = mark + "_" + string(buff);
	mark = mark + "_" + access_info->uid;*/
	const int pos_buff_len = 10;
	char pos_buff[pos_buff_len];
	snprintf(pos_buff, pos_buff_len , "%d", goods_ad_result_ptr->pos);
	string pos = string(pos_buff);

	//time_t  now;
	//now = time(NULL);

        struct timeval tv_temp;
        gettimeofday(&tv_temp, NULL);
        uint64_t now = tv_temp.tv_sec * 1000 + tv_temp.tv_usec / 1000;

	uint16_t check_code = 1;
	/*snprintf(buff, BUFF_LEN, "%ld", now); 
	mark = mark + "_" +string(buff);
	mark = mark + "_" +string( "1");
	snprintf(buff, BUFF_LEN, "%lu", goods_ad_result_ptr->custid) ;
	mark = mark + "_" + string(buff);
	mark = mark + "_" + access_info->exposure_id;
	snprintf(buff, BUFF_LEN, "%d", access_info->appid);
	mark = mark + "_" + string(buff);
	snprintf(buff, BUFF_LEN, "%d", goods_ad_result_ptr->ad_type) ;
	mark = mark + "_" + string(buff);*/
	snprintf(buff, BUFF_LEN, "%lu_%lu_%d_%d_%s_%lu_%d_%lu_%s_%d_%d",  
		goods_ad_result_ptr->adid, goods_ad_result_ptr->adid, goods_ad_result_ptr->cost, 
			goods_ad_result_ptr->bid_type, access_info->uid.c_str(), now, check_code, 
				goods_ad_result_ptr->custid, access_info->exposure_id.c_str(),  
					access_info->appid, goods_ad_result_ptr->ad_type);
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
	
	goods_ad_result_ptr->mark = pos + "_" +  string((char*)mark_enc);
	goods_ad_result_ptr->mark_origin =pos + "_" + mark;

	//unsigned char mark_buf[1024] = {'\0'};
	//uint32_t len_buf = 1024;
	//ret_aes = global_db_interface->dec_func(mark_enc, len_enc, (unsigned char*)mark_buf, &len_buf);
	//LOG_DEBUG("%s-%s-%d-%d", mark_enc, mark_buf, len_buf,ret_aes);
	return 0;	
	
}
int GoodsAdWorkInterface::query_process(ACCESS_INFO* & access_info ,VEC_CAND& goods_ad_vec, 
	VEC_CAND& goods_ad_freqCtrl_vec, VEC_CAND& goods_ad_result_vec, char* & p_out_temp, 
		int& n_out_len, uint64_t req_id)
{
	int result = 0;

	//VEC_CAND goods_ad_vec;

	//int ret_index = get_goodsAd_from_index_new(goods_ad_vec, access_info);
	int index_cost = -1;
	struct timeval index_beg;
	index_beg = calc_spend_time(index_beg, "goods index begin", index_cost, true);

	int ret_index = 0;
	ret_index = get_goodsAd_from_index(goods_ad_vec, access_info);

	calc_spend_time(index_beg, "goods index end", index_cost);
	LOG_INFO("goods index cost time:%d", index_cost);

	if(ret_index == -1){
		result = -1;
		LOG_ERROR("request index db error");
		//release(goods_ad_vec);
		return result;
	}
	if(goods_ad_vec.size() == 0){
		LOG_INFO("goods_ad_vec is null");
		return result;
	}
	//uint16_t user_expo_num = 0;
	//std::map<uint64_t,uint32_t> adid2expoNum_map;
	/*for(VEC_CAND::iterator iter = goods_ad_vec.begin(); iter != goods_ad_vec.end(); iter++){
		adid2expoNum_map.insert(std::make_pair(((GOODS_AD*)(*iter))->adid, 0));
		//LOG_DEBUG("adid:%d", ((GOODS_AD*)(*iter))->adid);
		//LOG_DEBUG("price:%d", ((GOODS_AD*)(*iter))->price);
		//LOG_DEBUG("price_type:%d", ((GOODS_AD*)(*iter))->price_type);
		//LOG_DEBUG("vecsize:%s", ((GOODS_AD*)(*iter))->keywords_vec[0].keyword.c_str());
	}*/
	int get_freq_cost = -1;
	struct timeval get_freq_beg;
	get_freq_beg = calc_spend_time(get_freq_beg, "goods get freq begin", get_freq_cost, true);

	int ret_expo_freq = get_expo_freq(goods_ad_vec, access_info);

	calc_spend_time(get_freq_beg, "goods get freq end", get_freq_cost);
	LOG_INFO("goods get freq cost time:%d", get_freq_cost);

	if (ret_expo_freq  == -1){
		LOG_ERROR("require freq_service error!");
	}

	/*VEC_CAND goods_ad_result_vec;
	GOODS_AD* goods_ad_result = new GOODS_AD();
	if(goods_ad_result == NULL)
		LOG_ERROR("new memory for goods_ad_result_vec failed!");
	goods_ad_result->adid = 0;
	goods_ad_result_vec.push_back(goods_ad_result);*/
	//int ret_process = 0;
	if(access_info->user_expo_num >= access_info->freq_uid){
	//if(user_expo_num > 15){
		//ret_process = return_process(goods_ad_result_vec, p_out_temp, n_out_len, access_info);
		//release(goods_ad_vec);
		//release(goods_ad_result_vec);
		LOG_INFO("user exposure limited");
		return result;
	}

	//VEC_CAND goods_ad_freqCtrl_vec;
	//std::map<uint64_t,uint32_t>::iterator iter_adid2expoNum_map;
	string adid_index = "";
	string adid_freq = "";
	char adid_buff[10];
	for(VEC_CAND::iterator iter = goods_ad_vec.begin(); iter != goods_ad_vec.end(); iter++){
		//iter_adid2expoNum_map = adid2expoNum_map.find(((GOODS_AD*)(*iter))->adid);
		//if(iter_adid2expoNum_map != adid2expoNum_map.end()){
			GOODS_AD* goods_ad_ptr= (GOODS_AD*)(*iter);
			snprintf(adid_buff, 10, "%lu", goods_ad_ptr->adid);
			adid_index = adid_index + "," + string(adid_buff);
			if(goods_ad_ptr->adid_expo_num < access_info->freq_adid){
			//if(iter_adid2expoNum_map->second < 1){
				goods_ad_freqCtrl_vec.push_back(goods_ad_ptr);
				adid_freq = adid_freq + "," + string(adid_buff);
			}
		//}
	}

	LOG_INFO("adid index: %s", adid_index.c_str());
	LOG_INFO("adid freq: %s", adid_freq.c_str());

	if(goods_ad_freqCtrl_vec.size() == 0){
		//ret_process = return_process(goods_ad_result_vec, p_out_temp, n_out_len, access_info);
		//release(goods_ad_vec);
		//release(goods_ad_result_vec);
		LOG_INFO("goods_ad_freqCtrl_vec is null");
		return result;
	}
			
	//uint64_t req_id = 0;
	//int req_alg = algorithm_core(req_id, access_info, "GOODSAD", goods_ad_vec, goods_ad_result_vec);//test
	//LOG_DEBUG("adid from freq:%d",  ((GOODS_AD*)goods_ad_freqCtrl_vec[0])->adid);

	int req_alg = 0;
	int alg_cost = -1;
	struct timeval alg_beg;
	alg_beg = calc_spend_time(alg_beg, "goods algorithm begin", alg_cost, true);
	req_alg = algorithm_core(req_id, access_info, "GOODSAD", goods_ad_freqCtrl_vec, goods_ad_result_vec);
	calc_spend_time(alg_beg, "goods algorithm end", alg_cost);
	LOG_INFO("goods algorithm cost time:%d", alg_cost);
	//LOG_DEBUG("adid from alg:%d",  ((GOODS_AD*)goods_ad_result_vec[0])->adid);

	//int ret_set_expo = set_expo_freq(((GOODS_AD*)goods_ad_freqCtrl_vec[0])->adid, access_info);//test

//	int ret_set_expo = 0;//test
//	if(goods_ad_vec.size() > 0)
//		ret_set_expo = set_expo_freq(((GOODS_AD*)goods_ad_vec[0])->adid, access_info);
	//int ret_set_expo = 0;
	//GOODS_AD* goods_ad_result_vec_ptr = 
	std::vector<uint64_t> result_adid;
	VEC_CAND::iterator goods_ad_result_vec_iter;
	int count = 0;
	for(goods_ad_result_vec_iter = goods_ad_result_vec.begin(); goods_ad_result_vec_iter != 
		goods_ad_result_vec.end(); goods_ad_result_vec_iter++){
		if(count >= access_info->num)
			break;
		GOODS_AD* goods_ad_result_vec_ptr = (GOODS_AD*)(*goods_ad_result_vec_iter);
		/*if(goods_ad_result_vec_ptr->adid == 0)
			break;*/
		result_adid.push_back(goods_ad_result_vec_ptr->adid);

		int ret_enc = 0;
//if(goods_ad_vec.size() > 0){	//just for test
//if(goods_ad_result_vec.size() > 0 && ((GOODS_AD*)goods_ad_result_vec[0])->adid != 0){
		ret_enc = mark_func(access_info, goods_ad_result_vec_ptr);
		count++;
	}
	if(result_adid.size() > 0 && access_info->debug == 0){

		int set_freq_cost = -1;
		struct timeval set_freq_beg;
		set_freq_beg = calc_spend_time(set_freq_beg, "goods set freq begin", set_freq_cost, true);

		int ret_set_expo = 0;
		ret_set_expo = set_expo_freq(result_adid, access_info);

		calc_spend_time(set_freq_beg, "goods set freq end", set_freq_cost);
		LOG_INFO("goods set freq cost time:%d", set_freq_cost);

		if (ret_set_expo  == -1){

			LOG_ERROR("set freq data error!");
		}
		LOG_INFO("goods_ad_result_vec is not null");
	}		
		
	/*if(goods_ad_result_vec.size() > 0 ){
		GOODS_AD* goods_ad_result_vec_ptr = (GOODS_AD*)goods_ad_result_vec[0];
		if(goods_ad_result_vec_ptr->adid != 0){
			int ret_set_expo = 0;
			ret_set_expo = set_expo_freq(goods_ad_result_vec_ptr->adid, access_info);
			if (ret_set_expo  == -1){

				LOG_ERROR("set freq data error!");
			}

			int ret_enc = 0;
	//if(goods_ad_vec.size() > 0){	//just for test
	//if(goods_ad_result_vec.size() > 0 && ((GOODS_AD*)goods_ad_result_vec[0])->adid != 0){
		 	ret_enc = mark_func(access_info, goods_ad_result_vec);
		}
	}*/
	//ret_process = return_process(goods_ad_vec, p_out_temp, n_out_len, access_info);//test
	//ret_process = return_process(goods_ad_result_vec, p_out_temp, n_out_len, access_info);
	//LOG_DEBUG("out_str:%s", p_out_temp);
	//release(goods_ad_vec);
	//release(goods_ad_result_vec);
	return 0;
}
int GoodsAdWorkInterface::work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id){
	int result = 0;
	//string ipp = "140.164.18.44";
       // int ip_code = get_ip_code(ipp);
        //LOG_DEBUG("ip_code:%d",ip_code);	
	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json){
		cmd_str = "query";
	}else{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	
	//业务请求
	if(strcasecmp(cmd_str, "query") == 0){

		LOG_INFO("access counter");

		ACCESS_INFO* access_info = new ACCESS_INFO();
		if(NULL == access_info){
			LOG_ERROR("allocate memory for access_info failed!");
			result = -1;
			return return_fail(406, "allocate for access_info memory failed!", p_out_string, n_out_len);
		}
		char* p_out_temp = p_out_string;
		json_object* body_json = json_object_object_get(req_json, "body");
		if(NULL == body_json){
			delete access_info;
			LOG_ERROR("require json body is null");
			result = -1;
			return return_fail(406, "require json body is null", p_out_string, n_out_len);
		}else{
			init_attri_by_req(body_json, access_info);
		}

		VEC_CAND goods_ad_vec;	

		VEC_CAND goods_ad_freqCtrl_vec;	//copy pointer from goods_ad_vec, need not release memory

		VEC_CAND goods_ad_result_vec;//copy pointer from goods_ad_vec, need not release memory
		/*GOODS_AD* goods_ad_result = new GOODS_AD();
		if(goods_ad_result == NULL)
			LOG_ERROR("new memory for goods_ad_result_vec failed!");
		goods_ad_result->adid = 0;
		goods_ad_result_vec.push_back(goods_ad_result);*/

		//std::map<uint64_t,uint32_t> adid2expoNum_map;
		//uint16_t user_expo_num = 0;
		int ret_process = 0;
		ret_process = query_process(access_info, goods_ad_vec, goods_ad_freqCtrl_vec, 
			goods_ad_result_vec, p_out_string, n_out_len, req_id); 


		return_process(access_info, goods_ad_vec, goods_ad_freqCtrl_vec, goods_ad_result_vec,
			p_out_temp, n_out_len, ret_process);

		/*else if(ret_process == -1){
			return_process(goods_ad_result_vec, p_out_temp, n_out_len, access_info, ret_process);
		}
		else if(ret_process == -2){
			return_process(goods_ad_result_vec, p_out_temp, n_out_len, access_info, ret_process);
		}
		else if(ret_process == -3){
			return_process(goods_ad_result_vec, p_out_temp, n_out_len, access_info, ret_process);
		}*/
			
			
			

		delete access_info;
		release(goods_ad_vec);
		//release(goods_ad_result_vec);
		//string host = get_host();
		//LOG_DEBUG("host:%s", host.c_str());

	       /*	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
		GlobalAesDbInterface* global_db_interface = (GlobalAesDbInterface*)p_global_db_company->get_global_db_interface("AES_DB");	
		int ret_aes = 1;
		//ret_aes = global_db_interface->is_exist(1);
		LOG_DEBUG("ret_aes:%d", ret_aes);	
		string mark = "movpid123";
		unsigned char mark_enc[1024] = {'\0'};
		uint32_t len_enc = 1024;
		ret_aes = global_db_interface->enc_func((unsigned char*)mark.c_str(), ((uint32_t)mark.length() + 1), (unsigned char*)mark_enc, &len_enc);
		LOG_DEBUG("%s-%s-%d", (char*)mark.c_str(), mark_enc, len_enc);
		unsigned char mark_buf[1024] = {'\0'};
		uint32_t len_buf = 1024;
		ret_aes = global_db_interface->dec_func(mark_enc, len_enc, (unsigned char*)mark_buf, &len_buf);
		LOG_DEBUG("%s-%s-%d", mark_enc, mark_buf, len_buf);*/
		 /*aes_t * aes_ptr = aes_create("/home/w/conf/aes.key");	
		if(aes_ptr == NULL)
			LOG_DEBUG("init aes fail");
		else
			LOG_DEBUG("init aes succ");
		u_char mark[] =  "movpi123";
		u_char buff[200] = {'\0'};
		uint32_t enc_len = 200;
		int ret_aes = aes_enc(aes_ptr, (u_char *)mark, strlen((char*)mark) + 1, (u_char *)buff, &enc_len);
		LOG_DEBUG("%s-%s-%d-%d", mark, buff, enc_len, ret_aes);*/
		

	}
	else{
	 	result = -1;
                return return_fail(406, "can not support cmd!", p_out_string, n_out_len);	
	}

	return result;
}

