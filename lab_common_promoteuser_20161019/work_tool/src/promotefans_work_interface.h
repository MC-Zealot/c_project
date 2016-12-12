#ifndef _FEATURE_WORK_INTERFACE_HEADER_
#define _FEATURE_WORK_INTERFACE_HEADER_

#include "work_interface.h"
#include "user_ad.h"

const int MAX_RESULT_NUM = 100;                 // 返回结果的最大数目
const int MAX_MGET_NUM = 500;                   // 限制MGET返回的最大数目
const uint16_t AD_FLAG = 1;                     // 广告主的广告类型默认写死为1
const uint16_t MAX_ADINFO_LEN = 50;             // 最大信息长度
const uint16_t INFO_PART = 4;                   //广告主信息包含四个部分
const uint32_t INIT_NUM = 1;                    //定义AdInfo结构体的初始桥梁数目
const uint32_t MAX_BRI_NUM = 2;                 //最大的桥梁数量
const uint32_t MAX_ADID_LEN = 100;              //adid的最大长度

class PromoteFansWorkInterface : public WorkInterface{
    public:
        PromoteFansWorkInterface(DbCompany*& p_db_company, int interface_id):
            WorkInterface(p_db_company, interface_id){ 
            }   
        ~PromoteFansWorkInterface(){
        } 

	int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
    private:
	int release(VEC_CAND& user_order_vec);

        //将从应用层获取的result输出为vector
        int split_result_to_vector(json_object* result_json, VEC_CAND& vec_reco_uids);
        
        // 将array转成字符串，形如 "bri_uids[i]":[bri_uid1,bri_uid2,...,bri_uidn]
        int array_to_str(bridge_item_t bri_item, char*& out_str);

        // 将dict中的项转成字符串
        int result_dict_to_str(USER_ORDER_AD* info, char*& out_str);

        //解析用户的特征
        int parse_user_feature(json_object *req_json, ACCESS_INFO*& access_str);

        //输出推荐结果
        void generate_output_str(VEC_CAND& vec_cand,char*& out_str, int& n_len);

};

#endif
