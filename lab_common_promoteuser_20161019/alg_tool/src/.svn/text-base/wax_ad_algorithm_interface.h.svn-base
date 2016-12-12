#ifndef _GOODS_AD_ALGORITHM_HEADER_
#define _GOODS_AD_ALGORITHM_HEADER_

#include "algorithm_interface.h"
#include "wax_ad.h"

/*typedef struct {
        string keyword;//词
        uint16_t weight;//权重
        uint16_t postagid;//词性
        uint16_t type;//0,商品分词；1,广告主定向词;2，微博核心词；3，商业词；4，微博核心词&商业词
}KEYWORDS;
typedef struct Access_Info :public AccessInfo {
        string exposure_id;//id
        string psid;//位置
        uint16_t appid;//appid
        string uid;
        string ip;
        uint16_t gender;
        uint16_t age;
        uint16_t device_type;//设备类型 
        uint64_t ip_code;//转码后ip
        std::vector<KEYWORDS> keywords_vec;//微博分词结果
        string weibo_cate;//微博分类
}ACCESS_INFO;
typedef struct {
        uint16_t cate_code;//分类码
        uint16_t weight;//权重
        uint16_t level;//类级别
        uint16_t src_type;//系统分析商品分类：0，广告主定向：1                                  
}CATEGORY;
typedef struct Goods_Ad : public candidate_item_t{                                              
        public:                                                                                 
        uint64_t custid; //广告主id                                                             
        uint64_t adid;//广告id                                                                  
        uint32_t price;//广告主出价                                                             
        uint32_t bid_type;//1,CPF;2,CPE                                                         
        uint32_t matchlevel;//0 : fans;1 : non-fans;2 : all                                     
        uint32_t score;//质量分                                                                 
        uint32_t price_type;//出价方式,1,广告主出价；2，自动竞价                                
        uint32_t ad_cate;//广告分类，微卖，微商铺                                               
        uint32_t price_limited;//自动竞价时的价格上限                                           
        uint32_t ad_type;//广告类型，10                                                         
        uint32_t cost;//广告最终出价                                                            
        uint32_t bucketid;//算法路径                                                            
        float royalty_price;//分成比率                                                          
        string url;//商品url                                                                    
        string url_md5;//url md5                                                                
        string creative;//保留                                                                  
        string extra_info;//算法附加信息                                                        
        string mark;//加密后mark                                                                
        string ad_target_info;//广告定向属性                                                    
        string algo_rank_attribute;//算法匹配模式                                               
        std::vector<KEYWORDS>  keywords_vec;//广告主定向词，商品分词结果                        
        std::vector<CATEGORY> category_vec;//广告主定向分类，商品分类                           
}GOODS_AD;
*/

class WaxAlgorithmInterface : public AlgorithmInterface{
	public:
		WaxAlgorithmInterface(DbCompany*& p_db_company, int interface_id):
			AlgorithmInterface(p_db_company, interface_id){
		}
		~WaxAlgorithmInterface(){
		}

	public:
		// algorithm_core函数是纯虚函数，必须实现
		int algorithm_core(uint64_t req_id, const AccessStr& access_str, VEC_CAND& vec_cand);
		// algorithm_core_new函数是新的算法函数
		int algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, const VEC_CAND& input_vec, VEC_CAND& output_vec);
};
#endif
