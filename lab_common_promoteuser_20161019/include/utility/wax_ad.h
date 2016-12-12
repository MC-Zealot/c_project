#ifndef __GOODSAD_H__
#define __GOODSAD_H__
#include "cand_user.h"
#include "algorithm_interface.h"
#include <vector>
#include <string>

#define URLLEN 1024

typedef struct Access_Info :  public AccessInfo {
        Access_Info():appid(0), gender(0), age(0), num(0), debug(0), freq_uid(0), freq_adid(0),user_expo_num(0),
			 ip_code(0), device_type(""), exposure_id(""), psid(""), ip(""){
        }

        uint16_t appid;//appid
        uint16_t gender;
        uint16_t age;
        //uint16_t device_type;//设备类型 
	uint16_t num;
	uint16_t debug;
	uint16_t freq_uid;
	uint16_t freq_adid;
	uint16_t user_expo_num;//用户曝光过的商品个数
        //uint32_t device_type;//设备类型 
        uint64_t ip_code;//转码后ip
        string device_type;//设备类型 
        string exposure_id;//id
        string psid;//位置
        //string uid_str;
        string ip;
}ACCESS_INFO;

struct CreativeData{
	uint32_t height;
	uint32_t width;
	uint64_t id;
	char url[URLLEN]; 
};

typedef struct Wax_Ad : public candidate_item_t{                                              
        //public:                                                                                 
        //uint64_t custid; //广告主id                                                             
        //uint64_t adid;//广告id                                                                  
        Wax_Ad(): price(0), cost(0), custid(0), dspid(0), itemid(0), bidid(""), id(""),
				 impid(""), psid(""), token(""), auction_price(""), extra_info(""), 
				algo_rank_attribute(""), mark(""), mark_origin(""), uniq_id(""),deal_id(""){
		memset(&creative, '\0', sizeof(creative));
        }


	uint32_t price;//广告主出价
	uint32_t cost;//真实扣费
	uint32_t bidfloor;//底价
	uint64_t custid; //广告主id                                                             
	uint64_t dspid;//广告id
	uint64_t itemid;//物料id

        string bidid;                                                
        string id;//广告id    
	string impid;//广告位置
	string psid;//广告位 比如:tips	
        string token;//dspToken
	string auction_price;//token加密过的价格,要回传给dsp的
        string extra_info;//算法附加信息                                                        
        string algo_rank_attribute;//算法匹配模式  
	string mark;//加密后mark                                                                
        string mark_origin;//mark                                                                
	string uniq_id;//唯一id索引

    string deal_id;
	CreativeData creative;//素材url
}WAX_AD;
#endif
