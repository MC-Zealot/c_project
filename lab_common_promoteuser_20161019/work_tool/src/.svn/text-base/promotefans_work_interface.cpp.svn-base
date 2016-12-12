#include "promotefans_work_interface.h"

DYN_WORK(PromoteFansWorkInterface);

int PromoteFansWorkInterface::array_to_str(bridge_item_t bri_item, char*& out_str)
{
	int len = sprintf(out_str, "%s", "\"gog_ext\":[");
	out_str += len;
	int len_total = len;
	uint32_t i = 0;

	uint32_t bri_num = ((bri_item.bnum)  < MAX_BRI_NUM ? (bri_item.bnum) : MAX_BRI_NUM);


	for(i=0;i<bri_num;i++)
	{
		if( i==0 )
		{
			len = sprintf(out_str, "%"PRIu64, bri_item.bs[i]);
		}
		else
		{
			len = sprintf(out_str, ",%"PRIu64, bri_item.bs[i]);
		}

		out_str += len;
		len_total += len;
	}

	len = sprintf(out_str, "%s", "]");
	out_str += len;
	len_total += len;
	return len_total;
}

int PromoteFansWorkInterface::result_dict_to_str(USER_ORDER_AD* info, char*& out_str)
{
	USER_ORDER_AD *order_info = (USER_ORDER_AD *)info;

	string ad_id = order_info->order_id;

	uint32_t ad_flag = order_info->adtype;
	uint32_t bri_num = order_info->reason[0].bnum;
	uint32_t ad_utype = order_info->utype;
	uint32_t source_type = order_info->source_type;
	uint64_t score = order_info->adscore;
	uint64_t tid = order_info->uid;

	int len = sprintf(out_str, "{\"ad_flag\":%"PRIu16, ad_flag);                // 写入推荐广告用户ad_flag
	out_str += len;
	int len_total = len;

	len = sprintf(out_str, ",\"utype\":%"PRIu16",",ad_utype );                  // 写入推荐广告用户的类型
	out_str += len;
	len_total += len;
	len_total += array_to_str(order_info->reason[0], out_str);            // 写入桥梁的bri_uids

	len = sprintf(out_str, ", \"gog_bnum\":%"PRIu32, bri_num);                 // 写入用户桥梁的数目
	out_str += len;
	len_total += len;

	len = sprintf(out_str, ", \"adid\":\"%s\"", ad_id.c_str());                       //写入广告订单id
	out_str += len;
	len_total += len;

	len = sprintf(out_str, ", \"source_type\":%"PRIu32, source_type);                       //写入广告订单id
	out_str += len;
	len_total += len;

	len = sprintf(out_str, ", \"sco\":%"PRIu64,score);                          //写入广告主得分
	out_str += len;
	len_total += len;

	len = sprintf(out_str, ", \"tid\":%"PRIu64,tid);                             //写入广告主的id
	out_str += len;
	len_total += len;

	len = sprintf(out_str, "%s", "}");
	out_str += len;
	len_total += len;

	return len_total;
}

void PromoteFansWorkInterface::generate_output_str(VEC_CAND& vec_cand,
		char*& out_str, int& n_len)
{
	n_len = 0;
	char* p_out_tmp = out_str;
	if(vec_cand.size()<=0)
	{
		LOG_ERROR("result vector size is 0");
		int len = sprintf(out_str, "%s", "{\"result_code\":\"200\",\"result\":{}}");
		n_len += len;
		return;
	}
	int len = sprintf(p_out_tmp, "%s", "{\"result_code\":\"200\",\"result\":{\"ad_list\":[");
	p_out_tmp +=  len;
	n_len += len;
	int i = 0;
	for(VEC_CAND::iterator it = vec_cand.begin();
			it != vec_cand.end(); it++)
	{
		string adid = ((USER_ORDER_AD *)(*it))->order_id;
		if (i ==0)
		{
			n_len += result_dict_to_str((USER_ORDER_AD *)(*it), p_out_tmp);
		}
		else
		{
			len = sprintf(p_out_tmp, "%s", ",");
			p_out_tmp += len;
			n_len += len;
			n_len += result_dict_to_str((USER_ORDER_AD *)(*it), p_out_tmp);
		}
		i++;
	}

	len = sprintf(p_out_tmp, "%s", "]}}");
	p_out_tmp += len;
	n_len += len;
}

int PromoteFansWorkInterface::parse_user_feature(json_object* user_json_item,
		ACCESS_INFO*& access_info){

	json_object* comefrom_json = json_object_object_get(user_json_item, "comeFrom");
	if(NULL == comefrom_json){
		LOG_ERROR("comefrom_json is NULL");
		return -1;
	}
	int comefrom = json_object_get_int(comefrom_json);

	//解析gender字段，默认为2，其他
	json_object* gender_json = json_object_object_get(user_json_item, "gender");
	int gender = 2;
	if(NULL == gender_json){
		LOG_ERROR("gender_json is NULL");
	}
	else
		gender = json_object_get_int(gender_json);

	//解析os字段，默认为4，其他
	json_object* os_json = json_object_object_get(user_json_item, "os");
	int os = 4;
	if(NULL == os_json){
		LOG_ERROR("os_json is NULL");
	}else
		os = json_object_get_int(os_json);

	//解析area字段，默认为0
	json_object* area_json = json_object_object_get(user_json_item, "area");
	int area = 0;
	if(NULL == area_json){
		LOG_ERROR("area_json is NULL");
	}else
		area = json_object_get_int(area_json);

	//解析feed_pos字段，默认为0
	json_object* feed_pos_json = json_object_object_get(user_json_item, "feed_pos");
	int feed_pos = 0;
	if(NULL == feed_pos_json){
		LOG_ERROR("fead_pos_json is NULL");
	}else
		feed_pos = json_object_get_int(area_json);

	//解析ad_num字段,默认为0
	json_object* ad_num_json = json_object_object_get(user_json_item, "ad_num");
	int ad_num = 0;
	if(NULL == ad_num_json){
		LOG_ERROR("ad_num_json is NULL");
	}else
		ad_num = json_object_get_int(ad_num_json);

	//解析come_uid字段,默认为NULL
	json_object* come_uid_json = json_object_object_get(user_json_item, "come_uid");
	char* come_uid = NULL;
	if(NULL == come_uid_json){
		LOG_ERROR("come_uid_json is NULL");
		access_info->come_uid = 0;
	}else{
		come_uid = (char*)json_object_get_string(come_uid_json);
		access_info->come_uid = strtoull(come_uid, NULL, 10);
	}
	access_info->comefrom = comefrom;
	access_info->gender = gender;
	access_info->os = os;
	access_info->area = area;
	access_info->feed_pos = feed_pos;
	access_info->ad_num = ad_num;
	return 1;
}

int PromoteFansWorkInterface::split_result_to_vector(
		json_object* aditem_json, VEC_CAND & vec_reco_uids)
{
	int array_len = 0;
	if(NULL == aditem_json)
	{
		LOG_ERROR("aditem_json is uncorrect");
		return -1;
	}
	else
	{   
		if(json_object_is_type(aditem_json, json_type_array))
		{   
			array_len = json_object_array_length(aditem_json);
			if(array_len == 0)
			{   
				LOG_ERROR("aditem_json length is error");
				return -1;
			}

			for(int i=0;i<array_len;i++)
			{

				USER_ORDER_AD* ad_info = new USER_ORDER_AD;

				//获取第i个item
				json_object* cand_json = json_object_array_get_idx(aditem_json, i);
				if(NULL == cand_json)
				{
					LOG_ERROR("aditem_json length is error");
					continue; //这个地方不能用break或者是return;
				}

				//获取广告订单的开始时间createtime
				json_object* create_json = json_object_object_get(cand_json, "createtime");
				if( NULL == create_json)
				{
					LOG_ERROR("create_json length is error");
					continue;
				}
				ad_info->create_time = (uint64_t)json_object_get_int64(create_json);

				//获取广告订单的结束时间expiretime
				json_object* expire_json = json_object_object_get(cand_json, "expiretime");
				if( NULL == expire_json)
				{
					LOG_ERROR("expire_json length is error");
					continue;
				}
				ad_info->expire_time = (uint64_t)json_object_get_int64(expire_json);

				//获取广告订单的预期涨粉数量count_target
				json_object* count_json = json_object_object_get(cand_json, "count_target");
				if( NULL == count_json)
				{
					LOG_ERROR("count_json length is error");
					continue;
				}
				ad_info->target_num = (uint64_t)json_object_get_int64(count_json);

				//获取tid
				json_object* tid_json = json_object_object_get(cand_json, "tid");
				if( NULL == tid_json)
				{
					LOG_ERROR("tid_json length is error");
					continue;
				}
				ad_info->uid = (uint64_t)json_object_get_int64(tid_json);

				//获取ad_flag
				json_object* adflag_json = json_object_object_get(cand_json, "ad_flag");
				if( NULL == adflag_json)
				{
					LOG_ERROR("adflag_json length is error");
					continue;
				}
				ad_info->adtype = (uint32_t)json_object_get_int(adflag_json);

				//获取utype
				json_object* utype_json = json_object_object_get(cand_json, "utype");
				if( NULL == utype_json)
				{
					LOG_ERROR("utype_json length is error");
					continue;
				}
				ad_info->utype = (uint32_t)json_object_get_int(utype_json);

				//获取adid
				json_object* adid_json = json_object_object_get(cand_json, "adid");
				if( NULL == adid_json)
				{
					LOG_ERROR("adid_json length is error");
					continue;
				}
				ad_info->order_id= (char*)json_object_get_string(adid_json);

				//获取用户来源标志
				json_object* source_json = json_object_object_get(cand_json, "source_type");
				if( NULL == source_json)
				{
					LOG_ERROR("source_json length is error");
					//continue;
				}
				else
				{
					ad_info->source_type= (uint32_t)json_object_get_int(source_json);
				}
				//LOG_ERROR("ader->source_type is: %"PRIu32,ader->source_type);

				//获取得分
				json_object* sco_json = json_object_object_get(cand_json, "sco");
				if( NULL == sco_json)
				{
					LOG_ERROR("sco_json is error");
					//continue;
				}
				else
				{
					ad_info->adscore = (uint64_t)json_object_get_int64(sco_json);
				}

				//获取桥梁的个数
				json_object* brinum_json = json_object_object_get(cand_json, "gog_bnum");
				if( NULL == brinum_json)
				{
					LOG_ERROR("brinum_json is error");
					continue;
				}
				ad_info->reason[0].bnum = (uint32_t)json_object_get_int(brinum_json);
				//LOG_ERROR("bnum is %"PRIu32,ader->reason[0].bnum);

				//获取桥梁
				json_object* gogext_json = json_object_object_get(cand_json, "gog_ext");
				if( NULL == gogext_json)
				{
					LOG_ERROR("gogext_json is error");
					continue;
				}
				int bri_len = json_object_array_length(gogext_json);
				for(int j = 0; j< bri_len; j++)
				{
					json_object* bid_json = json_object_array_get_idx(gogext_json, j);
					ad_info->reason[0].bs[j%MAX_BRI_NUM] = (uint64_t)json_object_get_int64(bid_json);
				}
				vec_reco_uids.push_back((USER_ORDER_AD *)ad_info);
			}//for
		}//if
	}
	return 0;
}

int PromoteFansWorkInterface::release(VEC_CAND& user_order_vec)
{
	for(size_t i = 0;i < user_order_vec.size(); i++)
	{
		delete user_order_vec[i];
	}
	return 0;
}

int user_return_fail(int fail_code, const char* fail_string, char* &p_out_string, int& n_out_len){
	n_out_len = sprintf(p_out_string, "{\"result_code\":\"%d\", \"result\":{}, \"error_msg\":\"%s\"}",
			fail_code, fail_string);

	return -1;
}

int PromoteFansWorkInterface::work_core(json_object *req_json, char* &p_out_string, 
		int& n_out_len, uint64_t req_id)
{
	int result = 0;
	json_object* cmd_json = json_object_object_get(req_json, "cmd");
	char* cmd_str = NULL;
	if(NULL == cmd_json)
	{
		cmd_str = "query";
	}
	else
	{
		cmd_str = (char*)json_object_get_string(cmd_json);
	}
	if(strcasecmp(cmd_str, "query") == 0)
	{
		json_object* uid_json = json_object_object_get(req_json, "uid");
		char* uid_str = NULL;
		if(NULL == uid_json)
		{
			return user_return_fail(401, "no uid", p_out_string, n_out_len);
		}
		else
		{
			uid_str = (char*)json_object_get_string(uid_json);
		}

		//获取json字符串中的type
		json_object* type_json = json_object_object_get(req_json, "type");
		int type_int = -1;
		if(NULL == type_json)
		{
			//如果不传type的值，目前认为是错误的
			return user_return_fail(401, "no type_json", p_out_string, n_out_len);
		}
		else
		{
			//如果type=1,则代表不走计算层，那么需要将应用层算出的数据以json的格式传到计算层
			//计算层对结果进行解析，成为一个vector，然后传给算法排序部分
			type_int = json_object_get_int(type_json);
			if(type_int == 1)
			{

				//先解析user字段 
				json_object* user_json = json_object_object_get(req_json, "user");
				if(NULL == user_json){
					return user_return_fail(401, "user_json is NULL", p_out_string, n_out_len);
				}
				char* user_json_dict = (char*)json_object_get_string(user_json);

				json_object* user_json_item = json_tokener_parse(user_json_dict);
				if (NULL == user_json_item){
					return user_return_fail(401, "user_json_item is NULL", p_out_string, n_out_len);
				}

				ACCESS_INFO* access_info = new ACCESS_INFO();
				if(access_info == NULL)
				{
					return user_return_fail(401,"allocate for access_info memory failed!",p_out_string,n_out_len);
				}
				VEC_CAND user_order_ad_vec;
				VEC_CAND user_order_ad_result_vec;

				int result;
				result = parse_user_feature(user_json_item, access_info);
				//LOG_DEBUG("access_info gender is %d,os is %d,area is %d,feed_pos is %d,ad_num is %d,comefrom is%d,come_uid is %s",
				//		access_info.gender,access_info.os,access_info.area,access_info.feed_pos,access_info.ad_num,access_info.comefrom,access_info.come_uid);
				if(result <= 0)
				{
					delete(access_info);
					return user_return_fail(401,"parse_user_feature is wrong",p_out_string,n_out_len);
				}

				//解析应用层计算结果的函数
				json_object* result_json = json_object_object_get(req_json, "result_str");

				if(NULL == result_json)
				{
					//如果type=1，不走计算层，但是应用层又没有传result_str过来,所以返回401
					delete(access_info);
					return user_return_fail(401,"result_json is NULL",p_out_string,n_out_len);
				}
				else
				{
					//然后对result_str函数进行解析，组成相应的vector中的项，然后直接传给计算层
					json_object* adlist_json = json_object_object_get(result_json, "ad_list");
					char* adlist = NULL;
					if(NULL == adlist_json)
					{
						delete(access_info);
						return user_return_fail(401,"adlist_json is uncorrect",p_out_string,n_out_len);
					}
					adlist = (char*)json_object_get_string(adlist_json);
					if(adlist == NULL)
					{
						delete(access_info);
						return user_return_fail(401,"adlist is NULL",p_out_string,n_out_len);
					}
					json_object* aditem_json = json_tokener_parse(adlist);
					result = split_result_to_vector(aditem_json, user_order_ad_vec);
					if(result<0)
					{
						delete(access_info);
                                                return user_return_fail(401,"aditem_json is NULL",p_out_string,n_out_len);
					}

					//将uid_str变为uid
					uint64_t uid = strtoull(uid_str, NULL, 10);
					access_info->uid_ = uid;
					//LOG_DEBUG("access_info gender is %d,os is %d,area is %d,feed_pos is %d,ad_num is %d,comefrom is%d,come_uid is %s,access_info.uid is %"PRIu64,
					//		access_info.gender,access_info.os,access_info.area,access_info.feed_pos,
					//		access_info.ad_num,access_info.comefrom,access_info.come_uid,access_info.uid_);
					
					LOG_DEBUG("size %d", user_order_ad_vec.size());
					int res = algorithm_core(uid, access_info, "PROMOTEFANS_ALG", user_order_ad_vec, user_order_ad_result_vec);
					if(res<0)
					{
						LOG_ERROR("%"PRIu64" feature algorithm core fail!", uid);
						generate_output_str(user_order_ad_vec, p_out_string, n_out_len);
					}
					else
					{
						generate_output_str(user_order_ad_result_vec, p_out_string, n_out_len);
					}
					result = 1;
					json_object_put(aditem_json);
					json_object_put(user_json_item);
					delete(access_info);
					release(user_order_ad_vec);
				}
			}
		}//else
	}
	return result;
}//int

