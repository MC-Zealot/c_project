#ifndef _NONFANS_WORK_INTERFACE_HEADER_
#define _NONFANS_WORK_INTERFACE_HEADER_

#include "work_interface.h"

#define DEFAULT_IP_CODE  30000
#define MARK_BUFF_SIZE 1024

class NonFansWorkInterface : public WorkInterface{

	private:
			
	public:
		NonFansWorkInterface(DbCompany*& p_db_company, int interface_id):
			WorkInterface(p_db_company, interface_id){                                      

                }                                                                               
                                                                                                
                ~NonFansWorkInterface(){                                                        
                } 
		//encrypt mark
		int mark_func(const string& mark_enc_src, string& mark);
		int get_ip_code(const string& ip);
		int return_process(string& mark, uint64_t& loc_code, char* &p_out_temp, int& n_out_len);
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);	
};
#endif

