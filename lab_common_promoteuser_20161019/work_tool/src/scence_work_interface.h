#ifndef _SCENCE_WORK_INTERFACE_HEADER_
#define _SCENCE_WORK_INTERFACE_HEADER_

#include "work_interface.h"
#include "scence_ad.h"
#include <vector>

#define MARK_BUFF_SIZE 1024
#define DEFAULT_DEVICE_TYPE "iphone"
#define	DEFAULT_IP_CODE  30000//未知
#define DEFAULT_AGE  1030//age=30
#define	DEFAULT_GENDER  402//female
#define DEFAULT_FREQ_UID  15//uid exp freq ctrl
#define	DEFAULT_FREQ_ADID 3//uid+adid exp freq ctrl
#define AES_BUFFER_SIZE 16
#define AES_KEY_SIZE 32

class ScenceWorkInterface : public WorkInterface{

	private:
		//const static int DEFAULT_IP_CODE = 30000;//未知
		//const static uint16_t DEFAULT_AGE = 1030;//age=30
		//const static uint16_t DEFAULT_GENDER = 402;//female
		//const static uint16_t DEFAULT_FREQ_UID = 15;//uid exp freq ctrl
		//const static uint16_t DEFAULT_FREQ_ADID= 3;//uid+adid exp freq ctrl
		//const static string DEFAULT_DEVICE_TYPE = "iphone";//未知
			
	public:
		ScenceWorkInterface(DbCompany*& p_db_company, int interface_id):
			WorkInterface(p_db_company, interface_id){                                      

                }                                                                               
                                                                                                
                ~ScenceWorkInterface(){                                                        
                } 
		int init_attri_by_req(json_object* &body_json, ACCESS_INFO*& access_info, VEC_CAND& scence_ad_vec);
		//encrypt mark
		int mark_func(ACCESS_INFO*& access_info, SCENCE_AD*& scence_ad_ptr);
		//encrypt auction_price
		int aes_enc(SCENCE_AD*& scence_ad);
		//encrypt mark & auction_price
		int enc_ad_attri(ACCESS_INFO*& access_info, VEC_CAND& scence_result_vec);
		int json_dump(json_object* & resp_json, VEC_CAND& scence_ad_result_vec, 
			string key,  ACCESS_INFO*& access_info, int num_limit);
		int return_process(ACCESS_INFO*& access_info,
			VEC_CAND& scence_ad_vec,VEC_CAND& scence_ad_result_vec, 
				char* &p_out_temp, int& n_out_len);
		//release memory
		int release(VEC_CAND& scence_ad_vec);
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);	
};
#endif

