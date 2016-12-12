#ifndef _GOODS_AD_WORK_INTERFACE_HEADER_
#define _GOODS_AD_WORK_INTERFACE_HEADER_

#include "work_interface.h"
#include "goods_ad.h"
#include "proto/goods_request.pb.h"
#include "proto/goods_response.pb.h"
#include "msidx/msidx_protocol.pb.h"
#include "ini_file.h"
#include "woo/log.h"
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>
#include "json.h"
#include <time.h>
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

#define MARK_BUFF_SIZE 1024 //存储mark加密值的buff长度
#define AD_TYPE 10//goods ad type:10
#define AD_NUM 100//发给索引的广告返回数目限制
#define FREQ_MSG_HEAD_LEN 2
#define BID_CPM  4
#define BID_CPE  8

class GoodsAdWorkInterface : public WorkInterface{

	public:
		GoodsAdWorkInterface(DbCompany*& p_db_company, int interface_id):
		WorkInterface(p_db_company, interface_id){
		}

		~GoodsAdWorkInterface(){
		}

	private:
		const static int DEFAULT_IP_CODE = 30000;//未知
		const static uint16_t DEFAULT_AGE = 1030;//age=30
		const static uint16_t DEFAULT_GENDER = 402;//female
		const static uint32_t DEFAULT_DEVICE_TYPE = 110200;//未知
		const static uint16_t DEFAULT_FREQ_UID = 15;//uid exp freq ctrl
		const static uint16_t DEFAULT_FREQ_ADID= 3;//uid+adid exp freq ctrl
		/*
			根据应用层请求信息初始化属性
			json_object* &body_json:请求json串
			ACCESS_INFO* access_info:访问者信息
		*/
		int init_attri_by_req(json_object* &body_json, ACCESS_INFO* access_info); 
		/*
			从索引服务里获取广告候选;
			VEC_CAND& goods_ad_vec:存储广告候选集
			ACCESS_INFO* access_info:获取访问信息和定向条件
		*/
		int get_goodsAd_from_index(VEC_CAND& goods_ad_vec, ACCESS_INFO* access_info);
		/*
			传鹏提供的索引
		*/
		int get_goodsAd_from_index_new(VEC_CAND& goods_ad_vec, ACCESS_INFO* access_info);
		/*
			从频次服务获取曝光数据；
			std::map<uint64_t,uint32_t>& adid2expoNum_map: adid->num;
			uint16_t& user_expo_num:当前访问用户曝光的次数；
			ACCESS_INFO* access_info: 访问者信息；
		*/
		int get_expo_freq(VEC_CAND& goods_ad_vec, ACCESS_INFO* access_info);
		/*
			向频次服务发送已经曝光的广告id
			std::vector<uint64_t>& result_adid:已经曝光的广告adid;
			ACCESS_INFO* access_info:访问者信息
		*/
		int set_expo_freq(std::vector<uint64_t>& result_adid, ACCESS_INFO* access_info);
		/*
			返回结果处理;
			ACCESS_INFO*& access_info:访问者信息;
			VEC_CAND& goods_ad_vec:从索引服务获取的广告候选集;
			VEC_CAND& goods_ad_freqCtrl_vec:经过频次过滤后的广告候选集;
			VEC_CAND& goods_ad_result_vec:算法竞价出的可以出的广告集合；
			std::map<uint64_t,uint32_t>& adid2expoNum_map：adid曝光个数;
			uint16_t& user_expo_num:uid曝光个数；
		*/
 		int return_process(ACCESS_INFO*& access_info, VEC_CAND& goods_ad_vec, VEC_CAND& 
			goods_ad_freqCtrl_vec, VEC_CAND& goods_ad_result_vec,  
				char* &p_out_temp, int& n_out_len, int ret_process);
		/*
			释放广告候选集申请的内存
		*/
		int release(VEC_CAND& vec_cand);
		/*
			对mark字段进行加密的函数
			ACCESS_INFO* access_info:访问者信息
			GOODS_AD* &goods_ad_result_ptr:需要进行加密的广告指针
		*/
		int mark_func(ACCESS_INFO* access_info, GOODS_AD* &goods_ad_result_ptr);
		/*
			根据ip获取ip_code
			const string& ip:ip串
			返回值：地域码
		*/
		int get_ip_code(const string& ip);
		/*
			query处理流程
			ACCESS_INFO*& access_info:访问者信息;
			VEC_CAND& goods_ad_vec:从索引服务获取的广告候选集;
			VEC_CAND& goods_ad_freqCtrl_vec:经过频次过滤后的广告候选集;
			VEC_CAND& goods_ad_result_vec:算法竞价出的可以出的广告集合；
			std::map<uint64_t,uint32_t>& adid2expoNum_map：adid曝光个数;
			uint16_t& user_expo_num:uid曝光个数；
		*/
		int query_process(ACCESS_INFO* & access_info, VEC_CAND& goods_ad_vec, VEC_CAND& 
			goods_ad_freqCtrl_vec, VEC_CAND& goods_ad_result_vec, char* & p_out_temp, 
				int& n_out_len, uint64_t req_id);
		/*
			将广告处理成json串
			json_object* & resp_json:json对象
			VEC_CAND& goods_ad_result_vec:算法竞价出的可以出的广告
			string key:json key
			ACCESS_INFO* access_info:访问者信息
			int num_limit:是否根据应用层传入的num值进行返回个数的限制，debug的返回值不限制个数
		*/
		int json_dump(json_object* & resp_json, VEC_CAND& goods_ad_result_vec, string key,  
			ACCESS_INFO* access_info, int num_limit);
		/*
			获取广告的ad_target_info信息
			ACCESS_INFO* &access_info:访问者信息
			uint32_t cate_code：商品分类码
		*/
		//string get_target_info(ACCESS_INFO* &access_info, uint32_t cate_code);
		//int find_ip(uint64_t ip_number,std::vector<uint64_t>*,std::vector<uint64_t>*);
	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
			
};
#endif
