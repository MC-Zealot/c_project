#ifndef _USER_FOLLOW_PROXY_WORK_INTERFACE_HEADER_
#define _USER_FOLLOW_PROXY_WORK_INTERFACE_HEADER_

#include "work_interface.h"
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

#define MAX_FOLLOW_NUM 3000

class UserFollowProxyWorkInterface : public WorkInterface{
	public:
		UserFollowProxyWorkInterface(DbCompany*& p_db_company, int interface_id):
		WorkInterface(p_db_company, interface_id){
		}
		~UserFollowProxyWorkInterface(){
		}

	private:
		int get_offline_follow_uids(uint64_t uid, char* &p_out_string, int& n_out_len);
		int get_online_follow_uids(uint64_t uid, char* &p_out_string, int& n_out_len);
		int get_api_follow_uids(uint64_t uid, char* &p_out_string, int& n_out_len);

		int get_follow_uids(uint64_t uid, int type, char* &p_out_string, int& n_out_len, uint64_t req_id);
	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, uint64_t req_id);
			
};
#endif
