#include "scence_ad_algorithm_interface.h"
#include "model.hpp"
#define MINCATELOGY 800
#define random(x) (rand()%x)
DYN_ALGORITHMS(ScenceAlgorithmInterface);

static int split(const std::string& str, const char* delimiter, std::vector<std::string> & vec, int save_space)
{
        std::string str_del = delimiter;
        int lastSpacePos = 0;
        for(int i=0;i<str.length();i++)
        {
                //if(str[i] == delimiter)
                 int flag = 0;
                 if(str_del.find(str[i]) != std::string::npos)
                 {
                         std::string subStr = str.substr(lastSpacePos,i-lastSpacePos);
                         if(strcmp(subStr.c_str(),"") != 0 || save_space > 0)
                                vec.push_back(subStr);
                         //else
                         //cout<<"split space"<<endl;
                         lastSpacePos = i + 1;
                         flag = 1;
                 }
                 if(i == str.length()-1 && flag == 0)
                 {
                         std::string subStr = str.substr(lastSpacePos,i+1-lastSpacePos);
                         if(subStr.length() > 0)
                                 vec.push_back(subStr);
                         //return vec;
                 }
        }
        return 0;
}

static int get_tday(string &tday){
	time_t t = time(0); 
	char tmp[2]; 
	strftime(tmp, sizeof(tmp), "%w",localtime(&t)); 
	tday = tmp;
	return 1; 
}
static int get_thour(string &thour){
	time_t t = time(0); 
	char tmp[3]; 
	strftime(tmp, sizeof(tmp), "%H",localtime(&t)); 
	thour = tmp;
	return 1; 
}
static int get_tmin(string &tmin){
	time_t t = time(0); 
	char tmp[3]; 
	strftime(tmp, sizeof(tmp), "%M",localtime(&t)); 
	tmin = tmp;
	return 1; 
}
static int get_tnow(string &tnow){
	time_t t = time(0); 
	char tmp[20]; 
	strftime(tmp, sizeof(tmp), "%Y%m%d%H",localtime(&t)); 
	tnow = tmp;
	return 1; 
}
static int trans_time(string &str, int &num){
	if(str.size()!=2)
		return -1;
	int i = str[1] - '0';
	int j = str[0] - '0';
	num = i + j * 10;
	return 0;
}
static int get_distance(string u_lat, string u_lon, string poi_lat, string poi_lon, int &dis)
{
	if(u_lat.size()==0 || u_lon.size()==0 || poi_lat.size()==0 || poi_lon.size()==0 ||
	   u_lat == "0" || u_lon == "0" || poi_lat == "0" || poi_lon == "0"){
		return -1;
	}
	double lat1 = (M_PI/180) * atof(u_lat.c_str());
	double lon1 = (M_PI/180) * atof(u_lon.c_str());
	double lat2 = (M_PI/180) * atof(poi_lat.c_str());
	double lon2 = (M_PI/180) * atof(poi_lon.c_str());
	/*
	double lat1 = (M_PI/180) * strtold(u_lat.c_str(),NULL);
	double lon1 = (M_PI/180) * strtold(u_lon.c_str(),NULL);
	double lat2 = (M_PI/180) * strtold(poi_lat.c_str(),NULL);
	double lon2 = (M_PI/180) * strtold(poi_lon.c_str(),NULL);
	*/
	double R = 6371; 
	double d = acos(sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon2-lon1))*R;
	dis = int(d * 1000); 
	return 1;
}
static string get_rec_dis(string u_lat, string u_lon, string poi_lat, string poi_lon)
{
	int dis;
	if(get_distance(u_lat, u_lon, poi_lat, poi_lon, dis) != 1){
    		LOG_DEBUG("get_distance fail");
		return "";
	}
	if(dis > 5000 || dis < 100){
    		LOG_DEBUG("distance:%d", dis);
		return "";
	}	
	char str[10] = {0};
	snprintf(str,sizeof(str),"%.1f", double(dis)/1000);
	return str;
}
static int trans_num(uint32_t num)
{	
	while(num > 999){
		num = num/10;
	}
	return num;
}

int ScenceAlgorithmInterface::get_para_in_redis(ParaInRedis &pis)
{
	string key_str = "para_in_redis";
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		json_object *pobj = NULL;
		pobj = json_tokener_parse(redis_val.c_str());
		if(NULL == pobj){
			LOG_ERROR("json format error:%s", redis_val.c_str());
			return -1;
		}
		int array_len = json_object_array_length(pobj);
		for(int i = 0; i< array_len;++i){
			json_object* jo_one = json_object_array_get_idx(pobj, i);
			if(NULL == jo_one){
				continue;
			}
			if(i == 0){
				pis.prob_for_sample = json_object_get_int(jo_one);
			}else if(i == 1){
				pis.prob_for_prof = json_object_get_int(jo_one);
			}else{
			}
		}
		json_object_put(pobj);
	}
	return 1;
}
int ScenceAlgorithmInterface::get_cate_from_model(map<string,vector<float> > &mapvalues, vector<uint32_t> &cate_vec, string tday, bool has_wifi, string &pois, uint32_t &sel_cate)
{
	if(cate_vec.size() < 1)
		return -1;
	if(mapvalues.size()==0 || pois.size()==0)
		return -1;
	string week = "0";
	string wifi = "1";
	if(tday == "0" || tday == "6")
		week = "1";
	if(!has_wifi)
		wifi = "0";
	string tmin,thour;
	get_tmin(tmin);
	get_thour(thour);
	int min, hour;
	int newtime = 660;
	if(trans_time(tmin, min)==0 && trans_time(thour, hour)==0){
		newtime = hour * 60 + min;
	}else{
		return -1;
	}
	map<string,double> res = computeTagProb(mapvalues,week,wifi,newtime,pois);
	LOG_DEBUG("computeTagProb:%s|%s|%s|%d", week.c_str(), wifi.c_str(), pois.c_str(), newtime);
	if(res.size() < 1)
		return -1;
	map<string,double>::iterator mapit;
	for(mapit = res.begin(); mapit != res.end(); mapit++){
		LOG_DEBUG("tagprob:%s %f", mapit->first.c_str(), mapit->second);
	}
	double max_prob = -1;
	for(int i=0;i<cate_vec.size();++i){
		uint32_t &cate = cate_vec[i];
		stringstream stream;
		string cate_str;
		stream << cate;
		stream >> cate_str;
		if(res[cate_str] > max_prob){
			sel_cate = cate;
			max_prob = res[cate_str];
		}
	}
	return 0;
}
int ScenceAlgorithmInterface::get_model_para_in_redis(ParaInRedis &pir)
{
	pir.model_para_map.clear();
	string redis_val;
	for(int i=1;i<100;++i){
		stringstream stream;
		string idx_str;
		stream << i;
		stream >> idx_str;
		string key_str = "model_para_";
		key_str += idx_str;
		LOG_ERROR("redis key:%s", key_str.c_str());
		redis_val.clear();
		if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
			json_object *pobj = NULL;
			pobj = json_tokener_parse(redis_val.c_str());
			if(NULL == pobj){
				LOG_ERROR("json format error:%s", redis_val.c_str());
				return -1;
			}
                	json_object_object_foreach(pobj, key, val){
				string p_k = key;
				string p_v = json_object_get_string(val);
				vector<string> vec;
				split(p_v, ",", vec, 0);
				vector<float> para_vec;
				for(int i=0;i<vec.size();++i){
					stringstream stream;
					float tmp;
					stream << vec[i];
					stream >> tmp;
					para_vec.push_back(tmp);
				}
				pir.model_para_map[p_k] = para_vec;
			}
			json_object_put(pobj);
		}else{
			break;
		}
	}
	return 1;
}
int ScenceAlgorithmInterface::get_rec_reason(uint32_t cateid, string distance, uint16_t time_slice, string &rec_reason, string district)
{
	string rec_title;
	get_rec_title(cateid, time_slice, rec_title);
	if(rec_title.size() == 0){
		uint32_t big_cate = trans_num(cateid);
		get_rec_title(big_cate, time_slice, rec_title);
	}
	if(rec_title.size() == 0){
		rec_title = "今日热点";
	}
	rec_reason = rec_title;
	if(cateid == 10901){
		rec_reason = district;
		rec_reason += "美食惠";
		if(distance.size() > 0){
			rec_reason += "          ";
			rec_reason += "距您";
			rec_reason += distance;
			rec_reason += "千米";
		}
	}
	if(cateid == 10902){
		rec_reason = "周末去哪儿";
	}
	if(cateid == 10903){
		rec_reason = "生活小帮手";
	}
	if(cateid == 10904){
		rec_reason = "世界那么大，我想去看看";
	}
	if(cateid == 10906){
		rec_reason = district;
		rec_reason += "精品酒店";
	}
	return 1;	

}
int ScenceAlgorithmInterface::get_rec_title(uint32_t cateid, uint16_t time_slice, string &rec_title)
{
	if(time_slice == 1){
		switch(cateid){
		    case 10101 :
		    rec_title = "热点早播报";
		    break;
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		    case 103 :
		    rec_title = "早间热议话题";
		    break;
		    case 10501 :
		    rec_title = "即将上映";
		    break;
		    case 10905 :
		    rec_title = "悦享早餐";
		    break;
		}
	}else if(time_slice == 2){		
		switch(cateid){
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		    case 103 :
		    rec_title = "午间热议话题";
		    break;
		    case 10602 :
		    rec_title = "热门金曲";
		    break;
		    case 10604 :
		    rec_title = "新歌速递";
		    break;
		    case 10107 :
		    rec_title = "热播综艺推荐";
		    break;
		    case 10113 :
		    rec_title = "热播电视剧推荐";
		    break;
		    case 103101 :
		    rec_title = "热播电视剧话题";
		    break;
		    case 103129 :
		    rec_title = "热播电视节目话题";
		    break;
		    case 10410011 :
		    rec_title = "午间轻松一刻";
		    break;
		    case 10501 :
		    rec_title = "即将上映";
		    break;
		    case 10502 :
		    rec_title = "热评电影推荐";
		    break;
		    case 10905 :
		    rec_title = "夏日慢生活";
		    break;
		}
	}else if(time_slice == 3){		
		switch(cateid){
		    case 108 :
		    rec_title = "午后资讯";
		    break;
		    case 101 :
		    rec_title = "午后资讯";
		    break;
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		    case 103101 :
		    rec_title = "热播电视剧话题";
		    break;
		    case 103129 :
		    rec_title = "热播电视节目话题";
		    break;
		    case 10602 :
		    rec_title = "音乐下午茶";
		    break;
		    case 10604 :
		    rec_title = "音乐下午茶";
		    break;
		    case 10410011 :
		    rec_title = "午后休闲时光";
		    break;
		    case 10501 :
		    rec_title = "即将上映";
		    break;
		    case 10502 :
		    rec_title = "热评电影推荐";
		    break;
		    case 10905 :
		    rec_title = "悦享下午茶";
		    break;
		}
	}else if(time_slice == 4){		
		switch(cateid){
		    case 101 :
		    rec_title = "晚间播报";
		    break;
		    case 108 :
		    rec_title = "晚间播报";
		    break;
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		    case 103129 :
		    rec_title = "热播电视节目话题";
		    break;
		    case 103101 :
		    rec_title = "热播电视剧话题";
		    break;
		    case 10604 :
		    rec_title = "新歌速递";
		    break;
		    case 10602 :
		    rec_title = "热门金曲";
		    break;
		    case 10410011 :
		    rec_title = "晚间轻松时刻";
		    break;
		    case 10501 :
		    rec_title = "即将上映";
		    break;
		    case 10502 :
		    rec_title = "热评电影推荐";
		    break;
		    case 10902 :
		    rec_title = "周末去哪儿";
		    break;
		    case 10904 :
		    rec_title = "世界那么大，我想去看看";
		    break;
		    case 10905 :
		    rec_title = "夏日慢生活";
		    break;
		}
	}else if(time_slice == 5){
		switch(cateid){
		    case 10101 :
		    rec_title = "午夜新闻";
		    break;
		    case 108 :
		    rec_title = "午夜新闻";
		    break;
		    case 102 :
		    rec_title = "24小时热点播报";
		    break;
		    case 103 :
		    rec_title = "24小时热议话题";
		    break;
		    case 10604 :
		    rec_title = "新歌速递";
		    break;
		    case 10602 :
		    rec_title = "热门金曲";
		    break;
		    case 10410011 :
		    rec_title = "轻松一刻";
		    break;
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		}
	}else if(time_slice == 6){
		switch(cateid){
		    case 10604 :
		    rec_title = "新歌速递";
		    break;
		    case 10501 :
		    rec_title = "即将上映";
		    break;
		    case 10902 :
		    rec_title = "周末去哪儿";
		    break;
		    case 10903 :
		    rec_title = "生活小帮手";
		    break;
		    case 10904 :
		    rec_title = "世界那么大，我想去看看";
		    break;
		    case 10410011 :
		    rec_title = "晚间轻松时刻";
		    break;
		    case 103129 :
		    rec_title = "热播电视节目话题";
		    break;
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		    case 10905 :
		    rec_title = "夏日慢生活";
		    break;
		}
	}else if(time_slice == 7){
		switch(cateid){
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		    case 10410011 :
		    rec_title = "晚间轻松时刻";
		    break;
		    case 10902 :
		    rec_title = "周末去哪儿";
		    break;
		    case 10904 :
		    rec_title = "世界那么大，我想去看看";
		    break;
		    case 10905 :
		    rec_title = "悦享早餐";
		    break;
		    case 103129 :
		    rec_title = "热播电视节目话题";
		    break;
		}
	}else if(time_slice == 8){
		switch(cateid){
		    case 103101 :
		    rec_title = "热播电视剧话题";
		    break;
		    case 10902 :
		    rec_title = "周末去哪儿";
		    break;
		    case 10903 :
		    rec_title = "生活小帮手";
		    break;
		    case 10410011 :
		    rec_title = "晚间轻松时刻";
		    break;
		    case 10501 :
		    rec_title = "即将上映";
		    break;
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		    case 10905 :
		    rec_title = "夏日慢生活";
		    break;
		}
	}else if(time_slice == 9){
		switch(cateid){
		    case 10602 :
		    rec_title = "热门金曲";
		    break;
		    case 108 :
		    rec_title = "午后资讯";
		    break;
		    case 10903 :
		    rec_title = "生活小帮手";
		    break;
		    case 10905 :
		    rec_title = "悦享下午茶";
		    break;
		    case 103129 :
		    rec_title = "热播电视节目话题";
		    break;
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		}
	}else if(time_slice == 10){
		switch(cateid){
		    case 10604 :
		    rec_title = "新歌速递";
		    break;
		    case 10902 :
		    rec_title = "周末去哪儿";
		    break;
		    case 102 :
		    rec_title = "24小时热点播报";
		    break;
		    case 10410011 :
		    rec_title = "轻松一刻";
		    break;
		    case 10502 :
		    rec_title = "热评电影推荐";
		    break;
		    case 107 :
		    rec_title = "微博新鲜事";
		    break;
		    case 10905 :
		    rec_title = "夏日慢生活";
		    break;
		}
	}else{
	}		
	return 1;
}
int ScenceAlgorithmInterface::set_data_to_redis(const  char *key, string &redis_val){	
	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("SCENCE_AD_DB");
	if (NULL == p_follow_db_interface){
		LOG_ERROR("redis db_interface error!");
		return -3;
	}
        int s_result = ((HiRedisDbInterface*)p_follow_db_interface)->s_set(0, key, redis_val.c_str(), 0);
        if(s_result <= 0){
                LOG_ERROR("set data is error!");
		return -2;
        } 
	return 1;
}
int ScenceAlgorithmInterface::get_data_from_redis(const  char *key, string &redis_val){	
	DbInterface* p_follow_db_interface = p_db_company_->get_db_interface("SCENCE_AD_DB");
	if (NULL == p_follow_db_interface){
		LOG_ERROR("redis db_interface error!");
		return -3;
	}

	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_follow_db_interface->s_get(0, key, p_result, split_char, second_split_char);
	if (result <= 0 ){
		LOG_ERROR("get data from redis error!");
		return -2;
	}
	if (NULL == p_result){
		LOG_ERROR("redis data is NULL!");
		return -1;
	}
	redis_val = p_result;

	return 1;
	
}
int ScenceAlgorithmInterface::algorithm_core(uint64_t req_id, const AccessStr& access_str,VEC_CAND& vec_cand){	
    LOG_DEBUG("test new algorithm interface alg_core!");
    return 1;
}
const static uint32_t conditionalWeight[][30] = { 
    {0},
    //早上
    {10501,107,103,109,0},
    //中午
    {107,10901,10604,103101,10410011,10501,10502,109,0},
    //{107,103,10602,10604,10410011,10501,10502,0},
    //下午
    {108,107,10905,10901,103129,10602,10410011,10501,109,0},
    //晚上
    {108,10901,10902,10904,107,103101,10602,10604,10410011,10501,109,0},
    //夜间
    {108,102,10602,10604,10410011,107,10901,10902,10903,10904,10906,0},
    //周五晚上
    {10604,10501,10901,10902,10903,10904,10906,10410011,103129,107,109,0},
    //周末上午
    {107,10410011,10902,10904,10905,103129,109,0},
    //周末中午
    {103101,10901,10902,10903,10410011,10501,107,109,0},
    //周末下午
    {10602,108,10901,10903,10905,103129,107,109,0},
    //周末晚上
    {10604,10901,10902,10906,102,10410011,10502,107,109,0}, 
    //wifi
    {104,106,0}
};
/*
const std::map<uint32_t, uint16_t>::value_type init_value[] =
{
	std::map<uint32_t, uint16_t>::value_type(101 , 0),
	std::map<uint32_t, uint16_t>::value_type(10107 , 0),
	std::map<uint32_t, uint16_t>::value_type(10113 , 0),
	std::map<uint32_t, uint16_t>::value_type(102 , 0),
	std::map<uint32_t, uint16_t>::value_type(103 , 0),
	std::map<uint32_t, uint16_t>::value_type(103101 , 0),
	std::map<uint32_t, uint16_t>::value_type(103129 , 0),
	std::map<uint32_t, uint16_t>::value_type(104 , 0),
	std::map<uint32_t, uint16_t>::value_type(10410011 , 0),
	std::map<uint32_t, uint16_t>::value_type(10501 , 0),
	std::map<uint32_t, uint16_t>::value_type(10502 , 0),
	std::map<uint32_t, uint16_t>::value_type(106 , 0),
	std::map<uint32_t, uint16_t>::value_type(10602 , 0),
	std::map<uint32_t, uint16_t>::value_type(10604 , 0),
	std::map<uint32_t, uint16_t>::value_type(107 , 0),
};
const static map<uint32_t, uint16_t> ad_cate_map(init_value, init_value+15);
*/
static bool exp_group(string &uid)
{
	string tail_num = uid.substr(uid.length()-1, 1);
	if(tail_num == "1" || tail_num == "3" || tail_num == "5"){
		return true;
	}
	return false;
}
//蒙特卡罗方法
/**
	arr: 需要按概率选择的数
	prob:每个数应出现的概率，转换为整形占比形式,比如概率3/7和4/7,转化为3和4
**/
static int monte_carlo_sel(vector<uint32_t> &arr, vector<int> &prob, uint32_t &feed_back_cate, double &sel_prob)
{
	if(arr.size() != prob.size())
		return -1;
	int sum = 0;
	for(int i=0;i< prob.size();++i){
		sum += prob[i];
	}
	if(sum < 1){
		return -1;
	}
	srand((int)time(0));
	int rand_num = random(sum);
	int tmp = 0;
	for(int i=0;i< prob.size();++i){
		tmp += prob[i];
		if(rand_num < tmp){
			feed_back_cate = arr[i];
			sel_prob = (double)prob[i]/sum;
			LOG_DEBUG("sum:%d %d %f",prob[i], sum, sel_prob);
			return 0;
		}			
	}	
				
	return -1;	
}
//按历史点击互动量来选
static int his_engage_sel(map< uint32_t, HIS_CTR > &his_ctr, map<uint32_t, int> &score_map, uint32_t &feed_back_cate, double &sel_prob)
{
	vector<uint32_t> arr;
	vector<int> prob;	
	map<uint32_t, int>::iterator it = score_map.begin();
	LOG_DEBUG("into his_engage_sel:%d",score_map.size());
	for(;it != score_map.end();++it){
		arr.push_back(it->first);
		if(his_ctr.find(it->first) == his_ctr.end()){
			prob.push_back(1);    		//新加类别也有机会出
			continue;
		}
		HIS_CTR &hc = his_ctr[it->first];
		int engage_num = hc.click_num + hc.inter_num;
		if(engage_num < 1){
			prob.push_back(1);    		//互动量过低类别做平滑 //平滑值需要根据数据出来再拍,基本上应该保证1/10的概率出
			continue;
		}
		int prob_int = engage_num;
		prob.push_back(prob_int);
		LOG_DEBUG("prob: %f %d %d",hc.click_rate,prob_int,it->first);
	}
		
	if( 0 != monte_carlo_sel(arr, prob, feed_back_cate, sel_prob) ){
		return -1;	
	}
	return 0;
}
/*
static int his_ctr_sel(map< uint32_t, HIS_CTR > &his_ctr, map<uint32_t, int> &score_map, uint32_t &feed_back_cate, double &sel_prob)
{
	//后期用ctr平滑预估代替规则
	vector<uint32_t> arr;
	vector<int> prob;	
	map<uint32_t, int>::iterator it = score_map.begin();
	LOG_DEBUG("into his_ctr_sel:%d",score_map.size());
	for(;it != score_map.end();++it){
		arr.push_back(it->first);
		if(his_ctr.find(it->first) == his_ctr.end()){
			prob.push_back(1);    		//新加类别也有机会出
			continue;
		}
		HIS_CTR &hc = his_ctr[it->first];
		if(hc.click_rate < 0.01){
			prob.push_back(1);    		//点击率过低类别做平滑 //平滑值需要根据数据出来再拍,基本上应该保证1/10的概率出
			continue;
		}
		if(hc.expos_num < 10){  //展示数多少才认为ctr可信，也需要看数据后再拍
			prob.push_back(1);
			continue;
		}
		int prob_int = int(hc.click_rate * 100 + hc.inter_rate * 100);
		prob.push_back(prob_int);
		LOG_DEBUG("prob: %f %d %d",hc.click_rate,prob_int,it->first);
	}
		
	if( 0 != monte_carlo_sel(arr, prob, feed_back_cate, sel_prob) ){
		return -1;	
	}
	return 0;
}
*/
static void print_feature(SCOREMARK &sm, string &fea_str)
{
	fea_str = "sp=";
	char str[10] = {0};
	snprintf(str,sizeof(str),"%.2f", sm.sel_prob);
	fea_str += str;
	fea_str+= ";";
}
static void mark_score(map<uint32_t, SCOREMARK> &score_mark, uint32_t cate, string desc, int score)
{
	if(score_mark.find(cate) == score_mark.end()){
		SCOREMARK sm;
		sm.time_score = 0;
		sm.interest_score = 0;
		sm.wifi_score = 0;
		sm.hisctr_score = 0;
		score_mark[cate] = sm;
	}
	SCOREMARK &sm = score_mark[cate];
	if(desc == "time_hit"){
		sm.time_score = score;
	}else if(desc == "interest"){
		sm.interest_score = score;
	}else if(desc == "wifi"){
		sm.wifi_score = score;
	}else if(desc == "his_ctr"){
		sm.hisctr_score = score;
	}else{
	}
}
static int add_score_cand_cate(map<uint32_t, int> &score_map, uint32_t cate, map< uint32_t, vector<uint32_t> > &cate_tree, int score, map<uint32_t, string> &score_log, string desc, map<uint32_t, SCOREMARK> &score_mark)
{
	stringstream stream;  //临时变量无碍，用clear不能清除缓存，会造成内存增长，需加上stream.str("");或直接用snprintf
	string score_str;
	stream << score;
	stream >> score_str;
	if(cate < 1000){
		if((cate_tree.find(cate) == cate_tree.end())){
			return 1;
		}
		if(cate_tree[cate].size()==0){
			score_map[cate] += score;
			mark_score(score_mark, cate, desc, score);
			score_log[cate] += desc;
			score_log[cate] += "=";
			score_log[cate] += score_str;
			score_log[cate] += "|";
			return 1;
		}else{
			for(int i=0;i<cate_tree[cate].size();++i){
				uint32_t child_cate = cate_tree[cate][i];
				score_map[child_cate] += score;
				mark_score(score_mark, child_cate, desc, score);
				score_log[child_cate] += desc;
				score_log[child_cate] += "=";
				score_log[child_cate] += score_str;
				score_log[child_cate] += "|";
			}
		}
	}else{
		if(score_map.find(cate) != score_map.end()){
                        return 1;
                }
		uint32_t big_cate = trans_num(cate);
		for(int i=0;i<cate_tree[big_cate].size();++i){
			if(cate_tree[big_cate][i] == cate){
				//要在分类树中存在的分类才能当做候选
				score_map[cate] += score;
				mark_score(score_mark, cate, desc, score);
				score_log[cate] += desc;
				score_log[cate] += "=";
				score_log[cate] += score_str;
				score_log[cate] += "|";
			}
		}
	}
	return 1;	
}
static int add_score(map<uint32_t, int> &score_map, uint32_t cate, map< uint32_t, vector<uint32_t> > &cate_tree, int score, map<uint32_t, string> &score_log, string desc, map<uint32_t, SCOREMARK> &score_mark)
{
	stringstream stream;
	string score_str;
	stream << score;
	stream >> score_str;
	//在候选集中才做加分的函数
	if(cate < 1000){
		if(cate_tree[cate].size()==0){
			if(score_map.find(cate) != score_map.end()){
				score_map[cate] += score;
				mark_score(score_mark, cate, desc, score);
				score_log[cate] += desc;
				score_log[cate] += "=";
				score_log[cate] += score_str;
				score_log[cate] += "|";
			}
			return 1;
		}else{
			for(int i=0;i<cate_tree[cate].size();++i){
				uint32_t child_cate = cate_tree[cate][i];
				if(score_map.find(child_cate) != score_map.end()){
					score_map[child_cate] += score;
					mark_score(score_mark, child_cate, desc, score);
					score_log[child_cate] += desc;
					score_log[child_cate] += "=";
					score_log[child_cate] += score_str;
					score_log[child_cate] += "|";
				}
			}
		}
	}else{
		if(score_map.find(cate) != score_map.end()){
			score_map[cate] += score;
			mark_score(score_mark, cate, desc, score);
			score_log[cate] += desc;
			score_log[cate] += "=";
			score_log[cate] += score_str;
			score_log[cate] += "|";
		}
	}
	return 1;	
}
static void print_score(map<uint32_t, int> &score_map, map<uint32_t, string> &score_log)
{
	string score_log_str;
	map<uint32_t, int>::iterator it;
	for(it = score_map.begin();it!=score_map.end();++it){
		stringstream stream;
		string cateid_str;
		stream << it->first;
		stream >> cateid_str;
		score_log_str += cateid_str;
		score_log_str += ":";
		score_log_str += score_log[it->first];
		score_log_str += ",";
	}
	LOG_DEBUG("score_log_str:%s",score_log_str.c_str());
}
int ScenceAlgorithmInterface::get_kind_item(uint32_t cateid, vector<string> &item_vec)
{
	stringstream stream;
	string cateid_str;
	stream << cateid;
	stream >> cateid_str;
	string key_str = "kind_";
	key_str += cateid_str;
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		json_object *pobj = NULL;
		pobj = json_tokener_parse(redis_val.c_str());
		if(NULL == pobj){
			LOG_ERROR("json format error:%s", redis_val.c_str());
			return -1;
		}
		int array_len = json_object_array_length(pobj);
		for(int i = 0; i< array_len;++i){
			json_object* jo_one = json_object_array_get_idx(pobj, i);
			if(NULL == jo_one){
				continue;
			}
			item_vec.push_back(json_object_get_string(jo_one));
			LOG_DEBUG("kind_%u : %s",cateid, item_vec.back().c_str());
		}
		json_object_put(pobj);
		return 1;
	}
	return -1;
}
int ScenceAlgorithmInterface::get_ads_kind_item(uint32_t cateid, vector<string> &item_vec, vector<string> &city_vec)
{
	stringstream stream;
	string cateid_str;
	stream << cateid;
	stream >> cateid_str;
	string key_str = "ads_kind_";
	key_str += cateid_str;
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		json_object *pobj = NULL;
		pobj = json_tokener_parse(redis_val.c_str());
		if(NULL == pobj){
			LOG_ERROR("json format error:%s", redis_val.c_str());
			return -1;
		}
		int array_len = json_object_array_length(pobj);
		for(int i = 0; i< array_len;++i){
			json_object* jo_one = json_object_array_get_idx(pobj, i);
			if(NULL == jo_one){
				continue;
			}
			int alen = json_object_array_length(jo_one);
			for(int j = 0;j < alen; ++j){
				json_object* jo_one_one = json_object_array_get_idx(jo_one, j);
				if(NULL == jo_one_one){
					continue;
				}
				switch(j){
				case 0:
					item_vec.push_back(json_object_get_string(jo_one_one));
					LOG_DEBUG("kind_%u : %s",cateid, item_vec.back().c_str());
					break;
				case 10:
					city_vec.push_back(json_object_get_string(jo_one_one));
					LOG_DEBUG("kind_%u : %s",cateid, city_vec.back().c_str());
					break;
					
				}
			}
		}
		json_object_put(pobj);
		return 1;
	}
	return -1;
}
int ScenceAlgorithmInterface::get_black_users(map<string, bool> &black_users_map)
{
	string key_str = "scence_black_users";
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		json_object *pobj = NULL;
		pobj = json_tokener_parse(redis_val.c_str());
		if(NULL == pobj){
			LOG_ERROR("json format error:%s", redis_val.c_str());
			return -1;
		}
		int array_len = json_object_array_length(pobj);
		for(int i = 0; i< array_len;++i){
			json_object* jo_one = json_object_array_get_idx(pobj, i);
			if(NULL == jo_one){
				continue;
			}
			black_users_map[ json_object_get_string(jo_one) ] = true;
		}
		json_object_put(pobj);
		return 1;
	}
	return -1;
}
int ScenceAlgorithmInterface::get_kind_filt_item(uint32_t cateid, map<string, int> &item_map)
{
	stringstream stream;
	string cateid_str;
	stream << cateid;
	stream >> cateid_str;
	string key_str = "kind_filt_";
	key_str += cateid_str;
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		json_object *pobj = NULL;
		pobj = json_tokener_parse(redis_val.c_str());
		if(NULL == pobj){
			LOG_ERROR("json format error:%s", redis_val.c_str());
			return -1;
		}
		int array_len = json_object_array_length(pobj);
		for(int i = 0; i< array_len;++i){
			json_object* jo_one = json_object_array_get_idx(pobj, i);
			if(NULL == jo_one){
				continue;
			}
			string itemid = json_object_get_string(jo_one);
			item_map[itemid] = 1;
			LOG_DEBUG("kind_filt_%u : %s",cateid, itemid.c_str());
		}
		json_object_put(pobj);
		return 1;
	}
	return -1;
}
int ScenceAlgorithmInterface::get_ad_loc(string &mid, string &lon, string &lat, string &city, string &district)
{
	string key_str = "ads_";
	key_str += mid;
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		json_object *pobj = NULL;
		pobj = json_tokener_parse(redis_val.c_str());
		if(NULL == pobj){
			LOG_ERROR("json format error:%s", redis_val.c_str());
			return -1;
		}
		int array_len = json_object_array_length(pobj);
		for(int i = 0; i< array_len;++i){
			json_object* jo_one = json_object_array_get_idx(pobj, i);
			if(NULL == jo_one){
				continue;
			}
			if(i==0){
				lat = json_object_get_string(jo_one);
			}else if(i==1){
				lon = json_object_get_string(jo_one);
			}else if(i==2){
				city = json_object_get_string(jo_one);
			}else if(i==3){
				district = json_object_get_string(jo_one);
			}else{
			}
			//LOG_DEBUG("loc %u : %s",cateid, item_vec.back().c_str());
		}
		json_object_put(pobj);
		return 1;
	}
	return -1;
}
int ScenceAlgorithmInterface::get_hasread_item(string &uid, map<string, int> &user_hasread_map)
{
	string key_str = uid;
	key_str += "_hasRead";
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		json_object *pobj = NULL;
		pobj = json_tokener_parse(redis_val.c_str());
		if(NULL == pobj){
			LOG_ERROR("json format error:%s", redis_val.c_str());
			return -1;
		}

                json_object_object_foreach(pobj, key, val){
			string item_id = key;
			int item_time = json_object_get_int(val);
			LOG_ERROR("hasread: %s %d", item_id.c_str(),item_time);
			user_hasread_map[item_id] = item_time;
		}
		json_object_put(pobj);
		return 1;
	}
	return -1;
}
int ScenceAlgorithmInterface::get_user_city(string &uid, string &city_id)
{
	string key_str = uid;
	key_str += "_city";
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		if(redis_val.size()<2){
			city_id = redis_val;
		}else{
			city_id = redis_val.substr(1, redis_val.size() - 2);
		}
		return 1;
	}
	return -1;
}
int ScenceAlgorithmInterface::get_history_top_item(std::string &key_str, vector<ItemHis> &item_his_vec)
{
	//string key_str = "history_top_item";
	LOG_ERROR("redis key:%s", key_str.c_str());
	string redis_val;
	if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
		json_object *pobj = NULL;
		pobj = json_tokener_parse(redis_val.c_str());
		if(NULL == pobj){
			LOG_ERROR("json format error:%s", redis_val.c_str());
			return -1;
		}
		int array_len = json_object_array_length(pobj);
		for(int i = 0; i< array_len;++i){
			json_object* jo_one = json_object_array_get_idx(pobj, i);
			if(NULL == jo_one){
				continue;
			}
			int alen = json_object_array_length(jo_one);
			ItemHis ih;
			for(int j = 0;j < alen; ++j){
				json_object* jo_one_one = json_object_array_get_idx(jo_one, j);
				if(NULL == jo_one_one){
					continue;
				}
				switch(j){
				case 0:
					ih.item_id = json_object_get_string(jo_one_one);	
					break;
				case 1:
					ih.cate_id = json_object_get_string(jo_one_one);	
					break;
				case 2:
					ih.action_num = json_object_get_int(jo_one_one);	
					break;
				case 3:
					ih.expo_num = json_object_get_int(jo_one_one);	
					break;
				case 4:
					ih.yield = json_object_get_double(jo_one_one);	
					break;
					
				}
			}
			item_his_vec.push_back(ih);
			//LOG_DEBUG("top item %d: %s, %s, %d, %d, %f", i+1 ,ih.item_id.c_str(),ih.cate_id.c_str(),ih.action_num, ih.expo_num, ih.yield);
		}
		json_object_put(pobj);
		return 1;
	}
	return -1;
}
int ScenceAlgorithmInterface::get_rec_item_from_hot(uint32_t cateid, map<string, int> &user_has_read_map, SCENCE_AD *sad)
{
	vector<string> item_vec;
	if(get_kind_item(cateid, item_vec) != 1){
		LOG_ERROR("get kind_%u item fail", cateid);
		return -1;
	}
	
	map<string, int> filt_item_map;
	get_kind_filt_item(cateid, filt_item_map);
	
	for(int i=0;i<item_vec.size();++i){
		if(filt_item_map.find(item_vec[i]) != filt_item_map.end()){
			continue;
		}
		if(user_has_read_map.find(item_vec[i]) == user_has_read_map.end()){
			sad->item_id = item_vec[i];
			sad->extra_data = "selway:fromhot|";
			sad->extra_data += "itemid:";
			sad->extra_data += item_vec[i];
			sad->extra_data += "|cateid:";
			stringstream stream;
			string cateid_str;
			stream << cateid;
			stream >> cateid_str;
			sad->extra_data += cateid_str;
			return 1;
		}
	}
	return -1;
	
}
int ScenceAlgorithmInterface::get_rec_item_from_prof(uint32_t cateid, map<string, int> &user_has_read_map, SCENCE_AD *sad)
{
	vector<string> item_vec;
	if(get_kind_item(cateid, item_vec) != 1){
		LOG_ERROR("get kind_%u item fail", cateid);
		return -1;
	}
	
	map<string, int> filt_item_map;
	get_kind_filt_item(cateid, filt_item_map);
	
	for(int i=0;i<item_vec.size();++i){
		if(filt_item_map.find(item_vec[i]) != filt_item_map.end()){
			continue;
		}
		if(user_has_read_map.find(item_vec[i]) == user_has_read_map.end()){
			sad->item_id = item_vec[i];
			sad->extra_data = "selwy:fromhot|";
			sad->extra_data += "itemid:";
			sad->extra_data += item_vec[i];
			sad->extra_data += "|cateid:";
			stringstream stream;
			string cateid_str;
			stream << cateid;
			stream >> cateid_str;
			sad->extra_data += cateid_str;
			return 1;
		}
	}
	return -1;
	
}
int ScenceAlgorithmInterface::get_rec_item_from_lbsad(uint32_t cateid, map<string, int> &user_has_read_map, SCENCE_AD *sad, string lat, string lon, string &uid, string &log_str)
{
	string city_id;
	log_str = "|uc=1";
	if(get_user_city(uid, city_id) != 1){
		LOG_ERROR("get user city fail");
		log_str = "|uc=0";
		//return -1;
	}else{
		LOG_ERROR("user city:%s", city_id.c_str());
	}
	log_str += "|ai=";
	stringstream stream;
	string ai_str;
	stream << cateid;
	stream >> ai_str;
	log_str += ai_str;
	
	vector<string> item_vec;
	vector<string> city_vec;
	if(get_ads_kind_item(cateid, item_vec, city_vec) == 1){
		LOG_DEBUG("get_ads_kind_item ok");
		
	}else if(get_kind_item(cateid, item_vec) == 1){
		LOG_DEBUG("get_kind_item ok");
	}else{
		LOG_ERROR("get kind_%u item fail", cateid);
		return -1;
	}
	
	int cand = 0;	
	int max_dis = 10000000;
	srand((int)time(0));
	int i = random( item_vec.size() );
	for(; i<item_vec.size();++i){
		if(!city_id.empty() && i< city_vec.size()  && city_id != city_vec[i]){
			continue;
		}
		if(user_has_read_map.find(item_vec[i]) == user_has_read_map.end()){
			if(cand == 0){
				sad->item_id = item_vec[i];
				sad->extra_data = "selway:lbsad|";
				sad->extra_data += "itemid:";
				sad->extra_data += item_vec[i];
				sad->extra_data += "|cateid:";
				stringstream stream;
				string cateid_str;
				stream << cateid;
				stream >> cateid_str;
				sad->extra_data += cateid_str;
				cand = 1;
				break;
			}
			/*
			string poi_lon,poi_lat,city,district;
			get_ad_loc(item_vec[i], poi_lon, poi_lat, city, district);
			int dis;
			int ret = get_distance(lat, lon, poi_lat, poi_lon, dis);
			if(ret == 1  && dis < max_dis){
				sad->item_id = item_vec[i];
				sad->extra_data = "selway:lbsad|";
				sad->extra_data += "itemid:";
				sad->extra_data += item_vec[i];
				sad->extra_data += "|cateid:";
				stringstream stream;
				string cateid_str;
				stream << cateid;
				stream >> cateid_str;
				sad->extra_data += cateid_str;
				max_dis = dis;
			}
			*/
		}
	}
	if(cand==1){
		return 1;
	}else{
		return -1;
	}
	
}
int ScenceAlgorithmInterface::get_item_for_demo(vector<uint32_t> &cate_vec, map<string, int> &user_has_read_map, SCENCE_AD *sad, string lat, string lon)
{
	string items;
	for(int i=0; i<cate_vec.size();++i){
		uint32_t cateid = cate_vec[i];	
		vector<string> item_vec;
		if(get_kind_item(cateid, item_vec) != 1){
			LOG_ERROR("get kind_%lu item fail", cateid);
			return -1;
		}
		for(;i<item_vec.size();++i){
			sad->item_id = item_vec[i];
			items += item_vec[i];
			items += ",";		
		}
	}
	sad->extra_data = "selway:lbsad|";
	sad->extra_data += "itemid:";
	sad->extra_data += items;
	return 1;	
	
}
int ScenceAlgorithmInterface::get_rec_item_from_sample(map<string, int> &user_has_read_map, map<uint32_t, int> &score_map, SCENCE_AD *sad)
{
	vector<uint32_t> weibo_tvec;
	vector<uint32_t> other_tvec;
	map<uint32_t, int>::iterator it = score_map.begin();
	for(; it!=score_map.end(); ++it){
		uint32_t big_cate = trans_num(it->first);
		if(big_cate !=109 && it->second > 0){
			if(big_cate == 107 || it->first == 10410011){	
				weibo_tvec.push_back(it->first);
			}else{
				other_tvec.push_back(it->first);
			}
		}
	}
	srand((int)time(0));
	uint32_t sel_cate = 0;
	int prob = random(10);
	int samp_num = 0;
	if(prob < 3  && weibo_tvec.size() > 0){
		int idx = random(weibo_tvec.size());
		sel_cate = weibo_tvec[idx];
		samp_num = 20;
	}else if(prob > 3   &&  other_tvec.size() > 0){
		int idx = random(other_tvec.size());
		sel_cate = other_tvec[idx];
		samp_num = 5;
	}else{
		return -1;
	}
	vector<string> item_vec;
	if(get_kind_item(sel_cate, item_vec) != 1){
		LOG_ERROR("get kind_%u item fail", sel_cate);
		return -1;
	}
	for(int i=0; i< samp_num && i<item_vec.size(); ++i){
		if(user_has_read_map.find(item_vec[i]) != user_has_read_map.end()){
			continue;
		}
		string key_str = "exp_num_";
		key_str += item_vec[i];	
		//key_str = "exp_num_test";
		LOG_ERROR("redis key:%s", key_str.c_str());
		string redis_val;
		int exp_num = 0;
		if(get_data_from_redis(key_str.c_str(), redis_val) == 1){
			stringstream stream;
			stream << redis_val;
			stream >> exp_num;
		}
		//if(true){
		if(exp_num < 400){
			string redis_val;
			++exp_num;
			stringstream stream;
			stream << exp_num;
			stream >> redis_val;
			
			if(set_data_to_redis(key_str.c_str(), redis_val) != 1)
				return -1;	
			sad->item_id = item_vec[i];
			sad->cate = sel_cate;
			sad->extra_data = "selway:fromsample|";
			sad->extra_data += "itemid:";
			sad->extra_data += item_vec[i];
			sad->extra_data += "|cateid:";
			stream.clear();
			stream.str("");	
			string cateid_str;
			stream << sel_cate;
			stream >> cateid_str;
			sad->extra_data += cateid_str;
			return 1;
		}
	}
	return -1;
	
}
int ScenceAlgorithmInterface::get_rec_item_from_histop(vector<ItemHis> &item_his_vec, map<string, int> &user_has_read_map,map<uint32_t, int> &score_map,SCENCE_AD *sad)
{
	if(item_his_vec.size() < 1)
		return -1;
	map<int,int> filt_map;
	vector<ItemHis> allow_his_vec;
	for(int i=0; i < item_his_vec.size(); ++i){
		ItemHis &ih = item_his_vec[i];	
		LOG_ERROR("hisvec:%s %s", ih.cate_id.c_str(), ih.item_id.c_str());
		stringstream stream;
		uint32_t cateid;
		stream << ih.cate_id;
		stream >> cateid;
		if(score_map.find(cateid) == score_map.end() ||  score_map[cateid] < 1){
			continue;
		}
		if(user_has_read_map.find(ih.item_id) != user_has_read_map.end())
			continue;
		allow_his_vec.push_back(ih);
	}
	if(allow_his_vec.size()<1)
		return -1;
	if(true){
		srand((int)time(0));
		int idx = random(allow_his_vec.size());
		ItemHis &ih = allow_his_vec[idx];	
		LOG_ERROR("allowhisvec:%s %s", ih.cate_id.c_str(), ih.item_id.c_str());
		sad->item_id = ih.item_id;
		stringstream stream;
		uint32_t cateid;
		stream << ih.cate_id;
		stream >> cateid;
		sad->cate = cateid;
		sad->extra_data = "selway:feedback|";
		sad->extra_data += "itemid:";
		sad->extra_data += ih.item_id;
		sad->extra_data += "|cateid:";
		sad->extra_data += ih.cate_id;
		return 1;
	}
	return -1;
}

int ScenceAlgorithmInterface::algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, 
        const VEC_CAND& input_vec, VEC_CAND& output_vec)
{
    	//debug
	LOG_DEBUG("scencealgorithminterface alg_core_new!");
	LOG_DEBUG("lat:%s lon:%s",((ACCESS_INFO*)access_info)->latitude.c_str(), ((ACCESS_INFO*)access_info)->longitude.c_str());
	time_t rawtime;
	time(&rawtime);
	char timestamp[100]={0};
	snprintf(timestamp,sizeof(timestamp),"%lu",rawtime);
	//LOG_DEBUG("access_info:%s", ((ACCESS_INFO*)access_info)->uid_str.c_str());
	LOG_DEBUG("int_vec:%d", input_vec.size());
	map<uint32_t, int> score_map;
	map<uint32_t, string> score_log;
	map<uint32_t, SCOREMARK> score_mark;
	map<uint32_t, int> interest_map;
	string &uid =  ((ACCESS_INFO*)access_info)->uid;
	map< uint32_t, vector<uint32_t> > &cate_tree =  ((ACCESS_INFO*)access_info)->cate_tree;
	vector<uint32_t> &interest = ((ACCESS_INFO*)access_info)->interest;
	uint16_t &time_slice = ((ACCESS_INFO*)access_info)->time_slice;
	int &scetype = ((ACCESS_INFO*)access_info)->scetype;
	bool has_wifi =  ((ACCESS_INFO*)access_info)->has_wifi;
	vector<POI_INFO> &nearby_pois = ((ACCESS_INFO*)access_info)->nearby_pois;
	string &lat = ((ACCESS_INFO*)access_info)->latitude;
	string &lon = ((ACCESS_INFO*)access_info)->longitude;
	string pois;
	for(int i=0;i<nearby_pois.size();++i){
		stringstream stream;
		string dis_str;
		stream << nearby_pois[i].distance;
		stream >> dis_str;
		pois += nearby_pois[i].poiid;
		pois += ",";
		pois += nearby_pois[i].categorys;
		pois += ",";
		pois += dis_str;
		pois += ";";
	}
	
	SCENCE_AD *sad = (SCENCE_AD*)input_vec[0];

	//test code
	/*
	sad->item_id="3859816521318128";
	sad->cate = 10901;
	sad->extra_data = "test for ad";
	sad->rec_title = "春园海鲜广场附近美食惠       距您3.2km";	
	output_vec.push_back(sad);
	return 1;
	*/
	//test code end
	

	map<string, int> user_has_read_map;
	
	get_hasread_item(uid, user_has_read_map);

	if(uid == "5577109009"){
		user_has_read_map.clear();	
		scetype = 0;
	}
	//时间段决定了候选分类
	string tday;
	get_tday(tday);
	if(tday == "5"){
		if(time_slice == 4)
			time_slice == 6;
	}
	if(tday == "0" || tday == "6"){
		if(time_slice == 1){
			time_slice = 7;
		}else if(time_slice == 2){
			time_slice = 8;
		}else if(time_slice == 3){
			time_slice = 9;
		}else if(time_slice == 4){
			time_slice = 10;
		}else{
		}
	}
	if(time_slice < 1 || time_slice > 10){
		return 2;
	}else{
		const uint32_t *arr = conditionalWeight[time_slice];
		for(int i=0;arr[i]!=0;++i){
			add_score_cand_cate(score_map, arr[i], cate_tree, 1, score_log, "time_hit", score_mark);
		}
	}
	
	if(!has_wifi){
		const uint32_t *arr = conditionalWeight[11];
		for(int i=0;arr[i]!=0;++i){
			add_score(score_map, arr[i], cate_tree, -10, score_log, "wifi", score_mark);
		}
			
	}

	for(int i=0;i<interest.size();++i){
		//用户有兴趣但没有物料的分类就不做加分了,在add_score_cand_cate已经保证了，不必在此处再判断
		//if(ad_cate_map.find(interest[i]) != ad_cate_map.end()){
		add_score(score_map, interest[i], cate_tree, 1, score_log,"interest", score_mark);
		interest_map[interest[i]] = 1;
		//}
	}
	char str[10]={0};
	string score_mark_log = "|";
	score_mark_log += "ts=";
	snprintf(str,sizeof(str),"%d",time_slice);
	score_mark_log += str;
	score_mark_log += "|";
	score_mark_log += "wi=";
	if(has_wifi){	
		score_mark_log += "1";
	}else{
		score_mark_log += "0";
	}
	score_mark_log += "|";
	score_mark_log += "st=";
	if(scetype == 1){	
		score_mark_log += "1";
		score_mark_log += "|pois=";
		score_mark_log += pois;
		score_mark_log += "|lat=";
		score_mark_log += lat;
		score_mark_log += "|lon=";
		score_mark_log += lon;
		
	}else{
		score_mark_log += "0";
	}
	string tnow;
	get_tnow(tnow);
	score_mark_log += "|tn=";
	score_mark_log += tnow;


	string tmin;
	get_tmin(tmin);

	//ParaInRedis t_pir;
	//get_para_in_redis(t_pir);
	//LOG_DEBUG("t_pir:%d %d",t_pir.prob_for_sample, t_pir.prob_for_prof);

	
	if( tmin.size()==2 && tmin[1]=='3' && pir.stat == 0){
		pir.stat = 1;
		ParaInRedis t_pir  = pir;
		get_para_in_redis(pir);
		if(pir.prob_for_sample < 0 || pir.prob_for_sample > 100 || 
		   pir.prob_for_prof < 0 || pir.prob_for_prof > 100 ){
			pir = t_pir;
		}
	}
	LOG_DEBUG("model_size: %d",pir.model_para_map.size());
	if( tmin.size()==2 && tmin[1]=='6' && tmin[0]=='4' && pir.model_stat == 0){
		pir.model_stat = 1;
		ParaInRedis t_pir  = pir;
		get_model_para_in_redis(pir);
		if(pir.model_para_map.size() < 1){
			pir = t_pir;
		}
		if(1){
			map< string,vector<float> >::iterator it = pir.model_para_map.begin();
			int i = 0;
			for(;it!=pir.model_para_map.end();++it){
				vector<float> &para_vec = it->second;
				//if(++i==2)
				//	break;
				for(int j=0;j<para_vec.size();++j)
					LOG_DEBUG("model_para_key:%s  val:%f", it->first.c_str(), para_vec[j]);
			}
		}
		pir.model_stat == 2;
	}
	if(tmin.size()==2 && tmin[1]=='8'){  //必须大于3
		pir.stat = 0;
		if( pir.model_stat == 2)
			pir.model_stat = 0;
	}
	
	//for ads
	if(scetype == 1 || scetype == 5){
	//if(true){     //redis有数据即可投广告
		map<uint32_t, int>::iterator it = score_map.begin();
		vector<uint32_t> cate_vec;
		vector<uint32_t> demo_cate_vec;
		int max_score = 1 ;
		for(;it!=score_map.end();++it){
			uint32_t big_cate = trans_num(it->first);
			if(big_cate == 109){
				if(it->second > 0){
					demo_cate_vec.push_back(it->first);
					cate_vec.push_back(it->first);
				}
				/*
				if(it->second > max_score){
					cate_vec.clear();
					cate_vec.push_back(it->first);
					max_score = it->second;
				}else if(it->second == max_score){
					cate_vec.push_back(it->first);
				}else{
				}
				*/
			}
		}
		if(cate_vec.size() > 0){
			if(scetype == 5){
				get_item_for_demo(demo_cate_vec, user_has_read_map, sad, lat, lon);
				sad->cate = 10907;
				sad->rec_title = "for demo";
				output_vec.push_back(sad);
				return 1;
			}
			srand((int)time(0));
			uint32_t sel_cate;
			string as = "0";
			if(get_cate_from_model(pir.model_para_map, cate_vec, tday, has_wifi, pois, sel_cate) == 0){
				as = "1";
			}else{
				sel_cate = cate_vec[random(cate_vec.size())];
			}
			score_mark_log +="|as:";
			score_mark_log += as;
			string log_str;
			if(get_rec_item_from_lbsad(sel_cate, user_has_read_map, sad , lat, lon, uid, log_str) == 1){
				sad->cate = sel_cate;
				sad->extra_data += "|uid:";
                		sad->extra_data += uid;
				sad->extra_data += "|tst:";
                		sad->extra_data += timestamp;
				sad->extra_data += score_mark_log;
				sad->extra_data += log_str;
				string rec_reason;
				string poi_lon,poi_lat,city,district;
				get_ad_loc(sad->item_id, poi_lon, poi_lat, city, district);
				string pos;
				if(district.size()>0){
					pos = district;
					pos += "附近";
				}else{
					pos = city;
				}
				string dis = "";
				//LOG_DEBUG("poi_dis: %s %s %s %s", poi_lat.c_str(),poi_lon.c_str(), lat.c_str(), lon.c_str());
				dis = get_rec_dis(poi_lat, poi_lon, lat, lon);
				get_rec_reason(sad->cate, dis, time_slice, rec_reason, pos);
				sad->rec_title = rec_reason;
				output_vec.push_back(sad);
				LOG_INFO("get rec item from lbsad success: %s", sad->extra_data.c_str());
				return 1;
			}else{
				score_mark_log += log_str;
				LOG_INFO("get rec item from lbsad fail");
			}
		}
		
	}
	//
	if(scetype == 5)
		return 2;
	
	//按概率选择投递方法
	srand((int)time(0));
	int idx = random(100);
	map<string, bool> black_users_map;
	get_black_users(black_users_map);	
	LOG_DEBUG("pir:%d %d",pir.prob_for_sample, pir.prob_for_prof);
	if(idx > pir.prob_for_prof || uid == "5577109009"){
		//map<string, bool>::iterator it = black_users_map.begin();
		//for(;it!=black_users_map.end();++it){
		//	LOG_DEBUG("black uid:%s", it->first.c_str());
		//}
		if(black_users_map.find(uid) == black_users_map.end()){
			//user_has_read_map.clear();
			if(get_rec_item_from_prof(19010, user_has_read_map, sad) == 1){
				sad->extra_data += "|uid:";
                		sad->extra_data += uid;
				sad->extra_data += "|tst:";
                		sad->extra_data += timestamp;
				sad->extra_data += score_mark_log;
				sad->rec_title = "微博新鲜事";
				output_vec.push_back(sad);
				LOG_INFO("get rec item from prof success: %s", sad->extra_data.c_str());
				return 1;
			}else{
				LOG_INFO("get rec item from prof fail: %s", sad->extra_data.c_str());
			}
		}
	}else if(idx < pir.prob_for_sample){
		int try_num = 0;
		while(++try_num < 2){ //random函数在毫秒级别时差里取得数一样...
			if(get_rec_item_from_sample(user_has_read_map, score_map, sad) == 1){
				sad->extra_data += "|uid:";
                        	sad->extra_data += uid;
				sad->extra_data += "|tst:";
                        	sad->extra_data += timestamp;
				sad->extra_data += "|";
                        	sad->extra_data += score_mark_log;
				string rec_reason;
				get_rec_reason(sad->cate, "", time_slice, rec_reason, "");
				sad->rec_title = rec_reason;
		
				output_vec.push_back(sad);
				LOG_INFO("get rec item from sample success: %s", sad->extra_data.c_str());
				return 1;
		
			}		
		}
		LOG_DEBUG("get rec item from sample fail");
		
	}else{
	}
	if(true){
		vector<ItemHis> item_his_vec;
		string key_str = "history_top_item";
		get_history_top_item(key_str,item_his_vec);
		if(black_users_map.find(uid) == black_users_map.end() && get_rec_item_from_histop(item_his_vec,user_has_read_map,score_map,sad) == 1){
			sad->extra_data += "|uid:";
                        sad->extra_data += uid;
			sad->extra_data += "|tst:";
                        sad->extra_data += timestamp;
			sad->extra_data += "|";
                        sad->extra_data += score_mark_log;
			string rec_reason;
			get_rec_reason(sad->cate, "", time_slice, rec_reason, "");
			sad->rec_title = rec_reason;
			output_vec.push_back(sad);
			LOG_INFO("get rec item from history top success: %s", sad->extra_data.c_str());
			return 1;
		}
		LOG_DEBUG("get rec item from history top fail");
	}

	
	map<uint32_t, int>::iterator it = score_map.begin();
	map<uint32_t, int> big_cate_map;
	vector<uint32_t> big_cate_vec;
	for(;it!=score_map.end();++it){
		//如果全部得分为0或得分为负就不推荐了
		if(it->second > 0){
			uint32_t big_cate = trans_num(it->first);
			if( big_cate != 109 && big_cate_map.find(big_cate) == big_cate_map.end()){
				big_cate_vec.push_back(big_cate);
				big_cate_map[big_cate] = 1;
			}
		}
	}
	srand((int)time(0));
	if(big_cate_vec.size()<1){
		//无可选分类
		LOG_DEBUG("no big cate sel");
		print_score(score_map,score_log);
		return 1;
	}
	uint32_t sel_big_cate = big_cate_vec[random(big_cate_vec.size())];
	vector<uint32_t> sel_cate_vec;
	map<uint32_t, int> sel_cate_score_map;
	int max_score = 1;
	string score_log_str;
	for(it = score_map.begin();it!=score_map.end();++it){
		stringstream stream;
		string cateid_str;
		stream << it->first;
		stream >> cateid_str;
		score_log_str += cateid_str;
		score_log_str += ":";
		score_log_str += score_log[it->first];
		score_log_str += ",";
		if(trans_num(it->first) == sel_big_cate){
			//LOG_DEBUG("score:%d %d", it->first, it->second);
			
			sel_cate_score_map[it->first] = it->second; //大类随机后走反馈的候选集
			if(it->second > max_score){
				sel_cate_vec.clear();
				sel_cate_vec.push_back(it->first);
				max_score = it->second;
			}else if(it->second == max_score){
				sel_cate_vec.push_back(it->first);
			}else{
			}
		}
	}
	//历史点击反馈
	uint32_t feed_back_cate;
	double sel_prob;
	//abtest
	uint32_t sel_cate = 0;
	int sel_cate_score = 0;
	if(false ){
	}else{
		if(sel_cate_vec.size()==0){
			LOG_DEBUG("no cate sel");
			return 1;
		}
		srand((int)time(0));
		sel_cate = sel_cate_vec[random(sel_cate_vec.size())];
	}
	/*LOG_DEBUG("sel_cate_vec.size:%d", sel_cate_vec.size());
	for(int i=0;i<sel_cate_vec.size();++i){
		LOG_DEBUG("sel_cate:%d", sel_cate_vec[i]);
	}
	*/

	sad->cate = sel_cate;
	SCOREMARK &sm = score_mark[sel_cate];
	
	//"ext":"datatype:10102|feature:31",
	score_mark_log += "|";
	snprintf(str,sizeof(str),"%d",sm.interest_score);
	score_mark_log += "is=";
	score_mark_log += str;
	
	if(get_rec_item_from_hot(sel_cate, user_has_read_map, sad) == 1){
		sad->extra_data += "|uid:";
                sad->extra_data += uid;
		sad->extra_data += "|tst:";
                sad->extra_data += timestamp;
		sad->extra_data += score_mark_log;
		string rec_reason;
		get_rec_reason(sad->cate, "", time_slice, rec_reason, "");
		sad->rec_title = rec_reason;
		output_vec.push_back(sad);
		LOG_INFO("get rec item from hot success: %s", sad->extra_data.c_str());
		return 1;
	}else{
		LOG_DEBUG("no item sel");
	}
	
	//sad->extra_data = score_log_str;
	//sad->extra_data += score_mark_log;
	
	return 1;
}

