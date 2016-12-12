#ifndef _FANSTOPREFRESH_WORK_INTERFACE_HEADER_
#define _FANSTOPREFRESH_WORK_INTERFACE_HEADER_

#include "work_interface.h"
#include "global_ftexp_db_interface.h"
#include "fanstop_refresh.h"
#include <vector>

#define MARK_BUFF_SIZE 1024 //存储mark加密值的buff长度

class FansTopRefreshWorkInterface : public WorkInterface {
	private:
		const static int DEFAULT_IP_CODE = 30000;	
	public:
		FansTopRefreshWorkInterface(DbCompany*& p_db_company, int interface_id):
				WorkInterface(p_db_company,interface_id)
		{}
		
		~FansTopRefreshWorkInterface()
		{}
		
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);	
	private:
		int get_ip_code(const string& ip);
		int init_attri_by_req(json_object*& body_json,ACCESS_INFO*& access_info,VEC_CAND& fanstop_ad_vec);
		int mark_func(ACCESS_INFO*& access_info,FANSTOP_CAND_ADS*& fanstop_cand_ad);
		int enc_ad_attri(ACCESS_INFO*& access_info,VEC_CAND& fanstop_ad_result_vec);	
		int json_dump(json_object*& resp_json, VEC_CAND& fanstop_ad_vec,string key, ACCESS_INFO*& access_info);	
		int return_process(ACCESS_INFO*& access_info,
					VEC_CAND& fanstop_ad_result_vec,char*& p_out_temp, int& n_out_len);	
		int release(VEC_CAND& fanstop_ad_vec);	
}; 
#endif
