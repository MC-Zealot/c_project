#ifndef _AD_USER_HEAHDER_
#define _AD_USER_HEAHDER_

#include "cand_user.h"
#include <vector>
#include <string>
#include <map>

typedef struct Access_Info: public AccessInfo {
        Access_Info():come_uid(0), os(0), area(0),
        feed_pos(0), ad_num(0), comefrom(0), cuid(0){}
        uint64_t come_uid;//当前来用户uid

	uint16_t  gender;
	uint16_t  os;
	uint16_t  area;
	uint16_t  feed_pos;
	uint32_t  ad_num;
	uint32_t  comefrom;
	uint64_t  cuid;

}ACCESS_INFO;

// 广告主的订单候选
typedef struct User_Order_Ad:public candidate_item_t {
        User_Order_Ad():order_id(""), create_time(0), expire_time(0), 
	target_num(0), adscore(0), adtype(0), source_type(0){}
	
	string order_id;                                         // 订单id
	uint64_t create_time;                           // 订单起始时间
	uint64_t expire_time;                           // 订单终止时间
	int target_num;                                         // 目标涨粉数
	uint64_t adscore;                               // 广告得分，20150609新增
	uint32_t adtype;                                        // 广告主的广告类型
	uint32_t source_type;                //标示用户的来源，是普通用户，蓝v用户，相似账号的用户
}USER_ORDER_AD;

typedef std::vector<USER_ORDER_AD*> USER_VEC_CAND;

#endif
