#ifndef _NSPIO_DB_INTERFACE_HEADER_
#define _NSPIO_DB_INTERFACE_HEADER_
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <assert.h>
#include "utility.h"
#include "base_define.h"
#include "json.h"
#include "db_interface.h"
#include "proxy.h"


using namespace std;

// nspio only support longconn
class NspioDbInterface : public DbInterface{
	public:
		NspioDbInterface(const Db_Info_Struct& db_info_struct):
			DbInterface(db_info_struct){
                // nspio long conn vec
				SvrIpPort svr_ip_port;
				get_ip_port(0, svr_ip_port);
				//char servers[MAX_BUFFER];
				string server = vec_str_ip_[svr_ip_port.ip_index_] + ":" + vec_str_port_[svr_ip_port.port_index_];
				//snprintf(servers, sizeof(servers), "%s:%"PRIu16, vec_str_ip_[svr_ip_port.ip_index_],
				//	vec_str_port_[svr_ip_port.port_index_]);
				LOG_INFO("nspio server is  %s", server.c_str());
				LOG_INFO("nspio group is  %s", db_info_struct_.group_);
                p_client_  = new Proxy(NSPIO);
				assert(p_client_->Init( db_info_struct_.group_, server, 500) == 0);

			}
		virtual ~NspioDbInterface(){
			if(p_client_ != NULL)
				delete p_client_;
		} 
	protected:

		int get_nspio_result(uint64_t n_key, const char* req, char* &resp_buf){
			SvrIpPort svr_ip_port;
			get_ip_port(n_key, svr_ip_port);
			return get_nspio_result(svr_ip_port, req, resp_buf);
		}
		int get_nspio_result_str(uint64_t n_key, string& req, string &resp_buf){
			SvrIpPort svr_ip_port;
			get_ip_port(n_key, svr_ip_port);
			return get_nspio_result_str(svr_ip_port, req, resp_buf);
		}

		int get_nspio_result(const SvrIpPort& svr_ip_port, const char* req,  char* &resp_buf){
			//uint16_t total_index = svr_ip_port.ip_index_ + svr_ip_port.port_index_ * ip_num_;
			//uint32_t log_id = 1234567;

			//uint32_t resp_buf_len = 0;
            /*
			char servers[MAX_BUFFER];
			snprintf(servers, sizeof(servers), "%s:%"PRIu16, vec_server_[total_index].ip_,
					vec_server_[total_index].port_);
			nspio::CNspioApi client;
    			assert(client.init( vec_server_[total_index].group_) == 0);
				LOG_ERROR("nspio_group:%s\n", vec_server_[total_index].group_);
    			assert(client.join_client(servers) == 0);
				LOG_ERROR("nspio_host:%s\n", servers);
            */
			
			ssize_t ret = 0;
			string resp;
			if(p_client_ == NULL){
				LOG_ERROR("nspio db:p_client_ is null");
				return -1;
			}
			if ((ret = p_client_->SendMsg(req)) == 0) {
				p_client_->RecvMsg(resp);
				int len = resp.length();
				if ( len ) {
					/*
					resp_buf = (char *)calloc(len, sizeof(char));
					resp.copy(resp_buf,len,0);
					*/
					memcpy(un_compress_[un_compress_index_], resp.c_str(), resp.length());
					resp_buf = un_compress_[un_compress_index_];
					un_compress_index_ ++;
				} else { 
					LOG_ERROR("client rev no data");
					ret = -1;
				}
			} else {
				LOG_ERROR("client send with errno %d\n", errno);
				ret = -1;
			}

			return ret;
		}
		int get_nspio_result_str(const SvrIpPort& svr_ip_port, string& req,  string &resp_buf){
			//uint16_t total_index = svr_ip_port.ip_index_ + svr_ip_port.port_index_ * ip_num_;
			//uint32_t log_id = 1234567;

			//uint32_t resp_buf_len = 0;
            /*
			char servers[MAX_BUFFER];
			snprintf(servers, sizeof(servers), "%s:%"PRIu16, vec_server_[total_index].ip_,
					vec_server_[total_index].port_);
			nspio::CNspioApi client;
    			assert(client.init( vec_server_[total_index].group_) == 0);
				LOG_ERROR("nspio_group:%s\n", vec_server_[total_index].group_);
    			assert(client.join_client(servers) == 0);
				LOG_ERROR("nspio_host:%s\n", servers);
            */
			
			ssize_t ret = 0;
			if(p_client_ == NULL){
				LOG_ERROR("nspio db:p_client_ is null");
				return -1;
			}
			string resp;
			if ((ret = p_client_->SendMsg(req)) == 0) {
				p_client_->RecvMsg(resp_buf);
				int len = resp_buf.length();
				if ( len ) {
					//resp_buf = (char *)calloc(len, sizeof(char));
					/* last char is \n
					if ( resp_buf[len] == '\n' ){
						resp.copy(resp_buf,len-1,0);
					} else {
						resp.copy(resp_buf,len,0);
					} */
					//resp.copy(resp_buf,len,0);
				} else { 
					LOG_ERROR("client rev no data");
					ret = -1;
				}
			} else {
				LOG_ERROR("client send with errno %d\n", errno);
				ret = -1;
			}

			return ret;
		}

	public :
		int s_get(uint16_t type_id, const char* p_str_key, char* &p_result, 
				char& split_char, char& second_split_char, uint64_t n_key = 0){ 
			return get_i(type_id, p_str_key, 0, p_result, split_char, second_split_char);
			//return get_i(type_id, n_key, 0, p_result, split_char, second_split_char, p_str_key, true);
		}
		int s_get_str(uint16_t type_id, string& p_str_key, string& p_result, char& split_char, char& second_split_char, uint64_t n_key = 0){ 
			return get_i_str(type_id, p_str_key, 0, p_result, split_char, second_split_char);
			//return get_i(type_id, n_key, 0, p_result, split_char, second_split_char, p_str_key, true);
		}

		int get(uint16_t type_id, uint64_t n_key, char* &p_result, char& split_char, char& second_split_char){
			return get_i(type_id, n_key, 0, p_result, split_char, second_split_char);			
		}

		// deal with item_key by char*
		int get_i(uint16_t type_id, const char* &p_str_key, uint32_t n_item_key, char* &p_result,
				char& split_char, char& second_split_char){

			initialize();
			get_nspio_result(0, p_str_key, p_result);

			split_char = db_info_struct_.value_split_first_char_;
			second_split_char = db_info_struct_.value_split_second_char_;

			return 1;
		}

		int get_i_str(uint16_t type_id, string &p_str_key, uint32_t n_item_key, string &p_result,
				char& split_char, char& second_split_char){

			initialize();
			get_nspio_result_str(0, p_str_key, p_result);

			split_char = db_info_struct_.value_split_first_char_;
			second_split_char = db_info_struct_.value_split_second_char_;

			return 1;
		}

		int get_i(uint16_t type_id, uint64_t n_key, uint32_t n_item_key, char* &p_result,
				char& split_char, char& second_split_char, const char* other_key = NULL, bool other_flag = false) {
			initialize();
		
			char req[WORD_LEN];
			snprintf(req, WORD_LEN, "%"PRIu64, n_key);
			get_nspio_result(n_key, req, p_result);

			split_char = db_info_struct_.value_split_first_char_;
			second_split_char = db_info_struct_.value_split_second_char_;

			return 1;
		}

		int mget(uint16_t type_id, uint64_t n_keys[], uint32_t key_num, MapMResult& map_result,
				char& split_char, char& second_split_char){

			uint32_t n_item_keys[1];
			return mget_i(type_id, n_keys, key_num, n_item_keys, 0, map_result, 
					split_char, second_split_char);
		}

		int mget_i(uint16_t type_id, uint64_t n_keys[], uint32_t key_num, 
				uint32_t n_item_keys[], uint32_t item_key_num,
				MapMResult& map_result, char& split_char, char& second_split_char){
			initialize();

			MapSplitIpPort map_ip_port;
			gets_ip_port(n_keys, key_num, map_ip_port);
			for(MapSplitIpPort::iterator it = map_ip_port.begin(); it != map_ip_port.end(); it++){
				SvrIpPort svr_ip_port = (*it).first;
				ReqStruct& req_struct = (*it).second;

				for(uint32_t i = 0; i < req_struct.num_; i ++){
					char req[WORD_LEN];
					snprintf(req, WORD_LEN, "%"PRIu64, req_struct.n_requsts_[i]);
					char* p_result = NULL;
					get_nspio_result(svr_ip_port, req, p_result);

					map_result.insert(MapMResult::value_type(req_struct.n_requsts_[i], p_result));
				}
			}

			split_char = db_info_struct_.value_split_first_char_;
			second_split_char = db_info_struct_.value_split_second_char_;
			return 1;
		}
		/*
		int s_set(uint16_t type_id, const char* p_str_key, const char* p_value, 
				char split_char, char second_split_char, uint64_t n_key=0)
		{
			return 1;
		}

		int set(uint16_t type_id, uint64_t n_key, const char* p_value, char split_char, char second_split_char){
			return 1;
		}

		int set_i(uint16_t type_id, uint64_t n_key, uint32_t n_item_key, const char* p_value, char split_char, char second_split_char, const char* other_key = NULL, bool other_flag = false){
			return 1;
		}
		*/

	private:
    protected:
        Proxy * p_client_;

};

#endif


