#include "fanstop_refresh_algorithm_interface.h"

DYN_ALGORITHMS(FansTopRefreshAlgorithmInterface);

bool dict_compare(pair<int, float> lhs, pair<int, float> rhs){
	return lhs.second > rhs.second;
}
int FansTopRefreshAlgorithmInterface::algorithm_core(uint64_t req_id, const AccessStr& access_str,
		VEC_CAND& vec_cand){

	//		LOG_ERROR("FansTopRefreshAlgorithmInterface::algorithm_core return 1\n");
	return 1;
}

int FansTopRefreshAlgorithmInterface::algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, 
		const VEC_CAND& input_vec, VEC_CAND& output_vec){
	LOG_ERROR("FANSTOP Enter algorith!candidate_Num:%d", input_vec.size());
	ACCESS_INFO* ai = (ACCESS_INFO*) access_info;
	if(ai->load_more == "1"){                          //后向粉条
		int size = input_vec.size();
		if((ai->come_uid/10)%10 > 2){
			if(std::atoi(ai->load_bhv.c_str()) < size && std::atoi(ai->load_bhv.c_str()) > 0){
				VEC_CAND::const_iterator it = input_vec.begin();
				FANSTOP_CAND_ADS *final = (FANSTOP_CAND_ADS*)(*it);
				time_t timestamp = final->createtime;
				uint16_t type = final->paid_type;
				for(;it != input_vec.end(); ++it){
					FANSTOP_CAND_ADS * ft = (FANSTOP_CAND_ADS*)(*it);
					if(ft->paid_type > type){
						type = ft->paid_type;
						timestamp = ft->createtime;
						final = ft;
					}else if(ft->paid_type == type){
						if(ft->createtime < timestamp){
							timestamp = ft->createtime;
							final = ft;
						}
					}
					//	LOG_ERROR("FansTopRefreshAlgorithmInterface,output_vec->time::%d\n",final->createtime);	
				}

				LOG_ERROR("back_refresh success! cand_num:%d,load_bhv:%s, type:%d\n",size,ai->load_bhv.c_str(), type);
				output_vec.push_back(final);
			}
		}else{
			VEC_CAND::const_iterator it = input_vec.begin();
			FANSTOP_CAND_ADS *final = (FANSTOP_CAND_ADS*)(*it);
			time_t timestamp = final->createtime;
			uint16_t type = final->paid_type;
			for(;it != input_vec.end(); ++it){
				FANSTOP_CAND_ADS * ft = (FANSTOP_CAND_ADS*)(*it);
				if(ft->paid_type > type){
					type = ft->paid_type;
					timestamp = ft->createtime;
					final = ft;
				}else if(ft->paid_type == type){
					if(ft->createtime < timestamp){
						timestamp = ft->createtime;
						final = ft;
					}
				}
			}
			LOG_ERROR("back_refresh success! uid is ods, type:%d\n", type);	
			output_vec.push_back(final);
		}

	}else if(ai->load_more == "0"){                  //前向粉条
		int size = input_vec.size();
		VEC_CAND::const_iterator it = input_vec.begin();
		VEC_CAND::const_iterator end = input_vec.end();
		FANSTOP_CAND_ADS *final = (FANSTOP_CAND_ADS*)(*it);
		time_t timestamp = final->createtime;
		uint16_t type = final->paid_type;
		map<uint16_t, uint16_t> type_cnt_map;
		map<uint16_t, uint16_t>::iterator itr_type_cnt_map;
		for(;it != end; ++it)
		{	
			FANSTOP_CAND_ADS * ft = (FANSTOP_CAND_ADS*)(*it);
			LOG_ERROR("front_refresh success! come_uid:%lu,owner:%lu, adid:%s\n",ai->come_uid,ft->owner, ft->adid.c_str());
			if(ft->paid_type > type)
			{
				type = ft->paid_type;
				timestamp = ft->createtime;
				final = ft;
			}
			else if(ft->paid_type == type)
			{
				if(ft->createtime < timestamp)
				{
					timestamp = ft->createtime;
					final = ft;
				}
			}
			if(type_cnt_map.find(ft->paid_type)==type_cnt_map.end())
				type_cnt_map.insert(make_pair(type,1));
			else
				type_cnt_map[ft->paid_type]+=1;
		}
		//modify temporary by YingyingLiu 20161011
		output_vec.push_back(final);
		/* 
		  if(type_cnt_map[type] < 2)
		   {
		   output_vec.push_back(final);
		   }
		   else
		   {

		   LOG_ERROR("front_refresh goto ctr prediction! come_uid:%lu, type:%d\n",ai->come_uid, type);
		   get_ad_user_history_ctr(ai, input_vec, output_vec, type);
		   int result_cnt = output_vec.size();
		   if(result_cnt < 1)
		   {
		   output_vec.push_back(final);
		   }

		   }
*/
		LOG_ERROR("front_refresh success! cand_num:%d\n",size);
	}
	return 1;
}


int FansTopRefreshAlgorithmInterface::get_ad_user_history_ctr(const ACCESS_INFO* ai,const VEC_CAND& input_vec,VEC_CAND& output_vec, uint16_t type)
{
	int ret = -1;
	DbInterface* p_insuff_order_interface = p_db_company_->get_db_interface("FANSTOP_AD_CTR");
	if(NULL == p_insuff_order_interface){
		LOG_ERROR("lushan connect error!");
		return ret;
	}

	//取全局平均ctr
	uint64_t general_uid=1;
	uint16_t key_size = 1; //默认
	for(VEC_CAND::const_iterator it = input_vec.begin(); it!=input_vec.end(); it++)
	{
		FANSTOP_CAND_ADS * ft = (FANSTOP_CAND_ADS*)(*it);
		if(ft->paid_type == type)
			key_size +=1;
	}
	if(key_size > 501)
		key_size = 501;
	char** keystr = new char*[key_size];
	for(uint16_t i = 0; i < key_size; i++)
		keystr[i] = new char[256];
	int index = 0;

	//构造取lushan数据的信息
	for(VEC_CAND::const_iterator it = input_vec.begin(); (it!=input_vec.end()) && (index < 500); it++)
	{
		FANSTOP_CAND_ADS * ft = (FANSTOP_CAND_ADS*)(*it);
		if(ft->paid_type == type)
			sprintf(keystr[index++],"%u-%"PRIu64"",30,ft->owner);//粉丝头条博文的博主;
	}
	sprintf(keystr[index++],"%u-%"PRIu64"",30, general_uid);


	map<uint64_t, const char*> result;
	float score = 0.0;
	float avg_ctr=0;
	int count = 0;
	map<int, float> sort_ad;
	int redis_flag = ((McDbInterface *)p_insuff_order_interface)->mget(1, keystr, key_size, result);
	if(redis_flag != 1)
	{
		LOG_ERROR("redis_flag_error");
		goto FREE_MODULE;
	}
	LOG_ERROR("EXPECT:%u GET: %u", key_size, result.size());

	for(map<uint64_t, const char*>::const_iterator iter = result.begin();iter != result.end(); iter++)
	{
		LOG_ERROR("LUSHAN %lld:%s", iter->first, iter->second);
	}
	if(result.find(general_uid)!=result.end())
	{
		string str = result[general_uid];
		avg_ctr = atof(str.c_str());
	}
	for(VEC_CAND::const_iterator it = input_vec.begin(); it!=input_vec.end(); it++)
	{
		FANSTOP_CAND_ADS * ft = (FANSTOP_CAND_ADS*)(*it);
		if(ft->paid_type != type)
			continue;
		if(result.find(ft->owner)!=result.end())
		{

			string str = result[ft->owner];
			score = atof(str.c_str());
		}else
			score = avg_ctr;

		sort_ad.insert(make_pair(count, score));
		LOG_ERROR("AD:%d %s SCORE:%lf", count,ft->adid.c_str(), score);
		count++;
	}

FREE_MODULE:
	for(uint16_t i=0; i < key_size; i++)
		delete[] keystr[i];
	if(keystr!=NULL)
		delete[] keystr;

	if (sort_ad.size() >= 2) {
		vector<pair<int, float> > tmp(sort_ad.begin(), sort_ad.end());
		sort(tmp.begin(), tmp.end(), dict_compare);
		output_vec.push_back(input_vec[tmp[0].first]);
		VEC_CAND::const_iterator it=output_vec.begin();
		FANSTOP_CAND_ADS * ft = (FANSTOP_CAND_ADS*)(*it);
		LOG_ERROR("CTR BEST:AD:%s, owner:%lu, custid:%lu, score: %lf", ft->adid.c_str(), ft->owner, ft->custid, tmp[0].second);
		ret = sort_ad.size();
	}
	return ret;
}
