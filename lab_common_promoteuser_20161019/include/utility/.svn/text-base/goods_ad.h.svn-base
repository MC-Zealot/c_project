#ifndef __GOODSAD_H__
#define __GOODSAD_H__
#include "cand_user.h"
#include "algorithm_interface.h"
#include <vector>
#include <string>
#define AD_TARGET_INFO_LEN 100
typedef struct {
        //string keyword;//词
        uint16_t weight;//权重
        uint16_t postagid;//词性
        uint16_t type;//0,商品分词；1,广告主定向词;2，微博核心词；3，商业词；4，微博核心词&商业词
        string keyword;//词
}KEYWORDS;
typedef struct Access_Info :  public AccessInfo {
        Access_Info():appid(0), gender(0), age(0), num(0), debug(0), freq_uid(0), freq_adid(0),user_expo_num(0),
			device_type(0), ip_code(0), exposure_id(""), psid(""), uid(""), ip(""), weibo_cate(""){
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
        uint32_t device_type;//设备类型 
        uint64_t ip_code;//转码后ip
	//std::vector<KEYWORDS> keywords_vec;//微博分词结果
        string exposure_id;//id
        string psid;//位置
        string uid;
        string ip;
        string weibo_cate;//微博分类
	std::vector<KEYWORDS> keywords_vec;//微博分词结果
}ACCESS_INFO;
typedef struct {
        //uint32_t cate_code;//分类码                                                             
        uint16_t weight;//权重                                                                  
        uint16_t level;//类级别                                                                 
        uint16_t src_type;//系统分析商品分类：0，广告主定向：1                                  
        uint32_t cate_code;//分类码                                                             
}CATEGORY;
typedef struct Goods_Ad : public candidate_item_t{                                              
        //public:                                                                                 
        //uint64_t custid; //广告主id                                                             
        //uint64_t adid;//广告id                                                                  
        Goods_Ad():adid_expo_num(0), pos(0), price(0), bid_type(0), matchlevel(0), score(0), price_type(0), 
			price_limited(0), ad_type(0), cost(0), bucketid(0), custid(0), adid(0), royalty_price(0),
				 ad_cate(""), url(""), url_md5(""), creative(""), 
					extra_info(""), mark(""), mark_origin(""), algo_rank_attribute(""){
		memset(ad_target_info, '\0', AD_TARGET_INFO_LEN);
        }
	uint16_t adid_expo_num;//用户+商品曝光个数
	uint16_t pos;//广告位置
        uint32_t price;//广告主出价                                                             
        uint32_t bid_type;//4,CPM;8,CPE                                                         
        uint32_t matchlevel;//0 : fans;1 : non-fans;2 : all                                     
        uint32_t score;//质量分                                                                 
        uint32_t price_type;//出价方式,2,广告主出价；1，自动竞价                                
        uint32_t price_limited;//自动竞价时的价格上限                                           
        uint32_t ad_type;//广告类型，10                                                         
        uint32_t cost;//广告最终出价                                                            
        uint32_t bucketid;//算法路径                                                            
        uint64_t custid; //广告主id                                                             
        uint64_t adid;//广告id                                                                  
        float royalty_price;//分成比率                                                          
        char ad_target_info[AD_TARGET_INFO_LEN];//广告定向属性                                                    
        string ad_cate;//广告分类，微卖，微商铺                                                 
        string url;//商品url                                                                    
        string url_md5;//url md5                                                                
        string creative;//保留                                                                  
        string extra_info;//算法附加信息                                                        
        string mark;//加密后mark                                                                
        string mark_origin;//mark                                                                
        //string ad_target_info;//广告定向属性                                                    
        string algo_rank_attribute;//算法匹配模式                                               
        //char ad_target_info[AD_TARGET_INFO_LEN];//广告定向属性                                                    
        std::vector<KEYWORDS>  keywords_vec;//广告主定向词，商品分词结果                        
        std::vector<CATEGORY> category_vec;//广告主定向分类，商品分类                           
	/*~ Goods_Ad(){
    		keywords_vec.clear();
    		category_vec.clear();
        }*/
}GOODS_AD;
#endif
