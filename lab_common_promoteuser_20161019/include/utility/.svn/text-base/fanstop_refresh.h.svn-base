#ifndef __FANSTARGETAD_H_
#define __FANSTARGETAD_H_

#include "cand_user.h"
#include<vector>
#include<string>
#include <map>

typedef struct Imp_Info {
	string pos;
	string impid;
} IMP_INFO;

typedef struct Access_Info: public AccessInfo {
	Access_Info():come_uid(0),ip("127.0.0.1"),exposure_id(""){}
	uint64_t come_uid;//当前来用户uid
	uint16_t gender;
	uint16_t age;
    string area_code;
    string city_code;
    string device_type;
    string load_more;
    string load_bhv;
	string ip;
	string exposure_id;//曝光id
	std::vector<IMP_INFO> imp_vec;
}ACCESS_INFO;


typedef struct FansTop_Cand_Ads:public candidate_item_t {
    FansTop_Cand_Ads():adid(""),mid(""),owner(0),custid(0),createtime(0),
    endtime(0),tag_info(""),alg_log(""),paid_type(0),mark(""),mark_origin(""){}
	string adid;//广告id
	string mid;//博文ID
    uint64_t owner;
    uint64_t custid;
	time_t createtime; //订单起始时间
	time_t endtime; //订单起始时间
    string tag_info;
    string alg_log;
    uint8_t paid_type;
	string mark;
	string mark_origin;
    int32_t  buynum;
    int32_t  expnum;
	//std::map<string, string> mark_map;
	//std::map<string, string> mark_origin_map;
}FANSTOP_CAND_ADS;

typedef std::vector<FANSTOP_CAND_ADS*> FANSTOP_VEC_CAND;

//粉条订单的配置文件
//#define ADID_CONF_PATH "../data/fanstop_profile.txt"

#endif
