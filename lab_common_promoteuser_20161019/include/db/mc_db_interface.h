#ifndef _MC_DB_INTERFACE_HEADER_
#define _MC_DB_INTERFACE_HEADER_
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "utility.h"
#include "base_define.h"
#include "json.h"
#include "db_interface.h"

#include <libmemcached/memcached.h>

using namespace std;

typedef struct _Mc_Struct{
	char ip_[IP_WORD_LEN];	
	uint32_t port_;
	memcached_st *memc_;
} Mc_Struct;

class McDbInterface : public DbInterface{
	public:
		McDbInterface(const Db_Info_Struct& db_info_struct):
			DbInterface(db_info_struct){
				// load configure
			for(vector<string>::iterator port_it = vec_str_port_.begin();
				port_it != vec_str_port_.end(); port_it++){
				uint32_t n_port = strtoul((*port_it).c_str(), NULL, 10);

				for(vector<string>::iterator ip_it = vec_str_ip_.begin(); 
					ip_it != vec_str_ip_.end(); ip_it++){

					Mc_Struct mc_struct;
					strncpy(mc_struct.ip_, (*ip_it).c_str(), IP_WORD_LEN);
					mc_struct.port_ = n_port;
					mc_struct.memc_ = NULL;
					vec_mc_server_.push_back(mc_struct);
				}
			}
		}

		virtual ~McDbInterface(){
			for(vector<Mc_Struct>::iterator it = vec_mc_server_.begin(); 
			it != vec_mc_server_.end(); it ++){
				if((*it).memc_ != NULL)
				memcached_free((*it).memc_);
			}	
			vec_mc_server_.clear();
		}

	protected:
		memcached_st* get_mc_raw(SvrIpPort& svr_ip_port){

			uint16_t total_index = svr_ip_port.ip_index_ + svr_ip_port.port_index_ * ip_num_;
			if( NULL == vec_mc_server_[total_index].memc_){
				memcached_st *memc = NULL;
				memcached_return rc;
				memcached_server_st *servers;
				memc = memcached_create(NULL);
				if(NULL == memc){
					LOG_ERROR("mc allocate is error!");
					return NULL;
				}else{
					//LOG_ERROR("%s:%d\n", (char*)vec_mc_server_[total_index].ip_, vec_mc_server_[total_index].port_);
					servers = memcached_server_list_append(NULL, (char*)vec_mc_server_[total_index].ip_, 
							vec_mc_server_[total_index].port_, &rc);
					rc = memcached_server_push(memc, servers);
					memcached_server_free(servers);
					memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, db_info_struct_.timeout_);
					memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, db_info_struct_.timeout_);
					memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 20) ;
					memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 5);  
					memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS, true);

					vec_mc_server_[total_index].memc_ = memc;
					if(NULL == memc){
						LOG_ERROR("connect mc %s:%"PRIu32" error!", vec_mc_server_[total_index].ip_, 
								vec_mc_server_[total_index].port_);
					}
				}
			}
			return vec_mc_server_[total_index].memc_;

		}

	public :
		int s_get(uint16_t type_id, const char* p_str_key, char* &p_result, 
				char& split_char, char& second_split_char, uint64_t n_key){ 
			return get_i(type_id, n_key, 0, p_result, split_char, second_split_char, p_str_key, true);
		}

		// deal with item_key by char*
		int get_i(uint16_t type_id, const char* &p_str_key, uint32_t n_item_key, char* &p_result,
				char& split_char, char& second_split_char){

			initialize();

			split_char = db_info_struct_.value_split_first_char_;
			second_split_char = db_info_struct_.value_split_second_char_;

			return 1;

		}

		int get(uint16_t type_id, uint64_t n_key, char* &p_result, char& split_char, char& second_split_char){
			return get_i(type_id, n_key, 0, p_result, split_char, second_split_char);			
		}

		int get_i(uint16_t type_id, uint64_t n_key, uint32_t n_item_key, char* &p_result,
				char& split_char, char& second_split_char, const char* other_key = NULL, bool other_flag = false) {
			initialize();

			size_t result_str_length = 0;
			SvrIpPort svr_ip_port;
			get_ip_port(n_key, svr_ip_port);
			memcached_st* memc = get_mc_raw(svr_ip_port);
			if(NULL == memc){
				return -1;
			}else{
				char str_key[WORD_LEN];
				size_t key_len = 0;
				if(!other_flag)
					key_len = snprintf(str_key, WORD_LEN, "%"PRIu64, n_key);
				else
					key_len = snprintf(str_key, WORD_LEN, "%s", other_key);
				
				uint32_t flags;
				memcached_return rc;
				char* result_str = memcached_get(memc, str_key, key_len, &result_str_length, &flags, &rc);
				if (rc == MEMCACHED_SUCCESS){
					memcpy(un_compress_[un_compress_index_], result_str, result_str_length);
					p_result = un_compress_[un_compress_index_];
					un_compress_index_ ++;
					free(result_str);
					result_str = NULL;
				}else{
					return 0;
				}
			}

			split_char = db_info_struct_.value_split_first_char_;
			second_split_char = db_info_struct_.value_split_second_char_;

			return (int)result_str_length;
		}

		int mget(uint16_t type_id, const char* str_keys[], uint32_t keys_num, char* results_array[],
				uint32_t& results_num, char& split_char, char& second_split_char){
			LOG_ERROR("mget ideal QYQ");
			initialize();

			SvrIpPort svr_ip_port;
			get_ip_port(0, svr_ip_port);
			memcached_st* memc = get_mc_raw(svr_ip_port);
			if(NULL == memc){
				LOG_ERROR("get mc is error!");
				return 0;
			}
				
			size_t key_len_array[keys_num];

			for(uint32_t i = 0; i < keys_num; i++){
				key_len_array[i] = strlen(str_keys[i]);
			}
			
			memcached_return_t rc = memcached_mget(memc, str_keys, key_len_array, keys_num);

			char return_key[MEMCACHED_MAX_KEY];
			char *return_value = NULL; 
			size_t return_key_length = 0;
			size_t return_value_length = 0;  
			uint32_t flags = 0;
			results_num = 0;
			while((return_value = memcached_fetch(memc, return_key, 
							&return_key_length, &return_value_length, &flags, &rc))){
				if(rc == MEMCACHED_SUCCESS){

					//strncpy(un_compress_[un_compress_index_], return_value, return_value_length);
					memcpy(un_compress_[un_compress_index_], return_value, return_value_length);
					results_array[results_num] = un_compress_[un_compress_index_];
					if(return_value != NULL)
						free(return_value);
					un_compress_index_ ++;
					results_num ++;
				}else{
					LOG_DEBUG("mget data is error!");
				}
			}

			return 0;
		}

		int mget(uint16_t type_id, uint64_t n_keys[], uint32_t key_num, MapMResult& map_result,
				char& split_char, char& second_split_char){
			LOG_ERROR("mget 2 ");
			uint32_t n_item_keys[32];
			return mget_i(type_id, n_keys, key_num, n_item_keys, 0, map_result, 
					split_char, second_split_char);
		}

		int mget_i(uint16_t type_id, uint64_t n_keys[], uint32_t key_num, 
				uint32_t n_item_keys[], uint32_t results_num,
				MapMResult& map_result, char& split_char, char& second_split_char){

			initialize();
			SvrIpPort svr_ip_port;
			get_ip_port(0, svr_ip_port);
			memcached_st* memc = get_mc_raw(svr_ip_port);
			if(NULL == memc){
				LOG_ERROR("get mc is error!");
				return 0;
			}
			
			char *str_keys[key_num];
			size_t key_len_array[key_num];

			for(uint32_t i = 0; i < key_num && i < COMPRESS_NUM; i++){
				str_keys[i] = (char *)malloc(WORD_LEN * sizeof(char));
				key_len_array[i] = sprintf(str_keys[i], "%d-%ld", type_id, n_keys[i]);
				LOG_DEBUG("key:%s\n", str_keys[i]);
			}
			
			memcached_return_t rc = memcached_mget(memc, str_keys, key_len_array, key_num);

			char return_key[MEMCACHED_MAX_KEY];
			char *return_value = NULL; 
			size_t return_key_length = 0;
			size_t return_value_length = 0;  
			uint32_t flags = 0;
			results_num = 0;
			//char *results_array[key_num];
			while((return_value = memcached_fetch(memc, return_key, 
							&return_key_length, &return_value_length, &flags, &rc))){
				if(rc == MEMCACHED_SUCCESS){
					//results_array[results_num] = (char *)malloc(1024 * sizeof(char));
					return_key[return_key_length] = 0;
					memcpy(un_compress_[un_compress_index_], return_value, return_value_length);
					//results_array[results_num] = un_compress_[un_compress_index_];
					LOG_DEBUG("return_key:%s, return_value:%s\n", return_key, return_value);
					//strcpy(results_array[results_num], return_value);
					//map_result[n_keys[results_num]] = results_array[results_num];
					map_result[n_keys[results_num]] = un_compress_[un_compress_index_];
					if(return_value != NULL)
						free(return_value);
					un_compress_index_ ++;
					results_num ++;
				}else{
					LOG_DEBUG("mget data is error!");
				}
			}

			for(uint32_t i = 0; i < key_num; i++){
				if(str_keys[i] != NULL){
					free(str_keys[i]);
				}
			}
			
			return 1;
		}

		int mget(uint16_t type_id, char* str_keys[], uint32_t key_num, 
				MapMResult& map_result){

			
			initialize();
			SvrIpPort svr_ip_port;
			
			struct timeval start_time;
			gettimeofday(&start_time,NULL);

			//LOG_DEBUG("comming!6!");

			if(key_num < 1){return 0;}

			uint64_t modUid = 0;
			if(type_id == 1){
				//get_ip_port(0, svr_ip_port);
				char *p = strchr((char *)str_keys[0], '-');
				if((p == NULL) || ((p + 1) == NULL)){
						return 0;
				}
				
				modUid = atol(p + 1);
				get_ip_port(modUid, svr_ip_port);
				
			}
			else{
				get_ip_port(0, svr_ip_port);
			}

//			get_ip_port(0, svr_ip_port);
			memcached_st* memc = get_mc_raw(svr_ip_port);
			if(NULL == memc){
				LOG_ERROR("get mc is error!");
				return 0;
			}
			
			//uint64_t n_keys[key_num];
			size_t key_len_array[key_num];

			//LOG_DEBUG("key:123\n");

			for(uint32_t i = 0; i < key_num && i < COMPRESS_NUM; i++){
				key_len_array[i] = strlen(str_keys[i]);
				//LOG_DEBUG("mckey:%s\n", str_keys[i]);
			}
/*
			for(uint32_t i = 0; i < key_num && i < COMPRESS_NUM; i++){
				str_keys[i] = (char *)malloc(WORD_LEN * sizeof(char));
				key_len_array[i] = sprintf(str_keys[i], "%d-%ld", type_id, n_keys[i]);
				LOG_DEBUG("key:%s\n", str_keys[i]);
			}
*/
			memcached_return_t rc = memcached_mget(memc, str_keys, key_len_array, key_num);

			//LOG_DEBUG("rc:%d\n", rc);

			char return_key[MEMCACHED_MAX_KEY];
			char *return_value = NULL; 
			size_t return_key_length = 0;
			size_t return_value_length = 0;  
			uint32_t flags = 0;
			//char *results_array[key_num];
			while((return_value = memcached_fetch(memc, return_key, 
							&return_key_length, &return_value_length, &flags, &rc))){
				if(rc == MEMCACHED_SUCCESS){
					return_key[return_key_length] = 0;
					memcpy(un_compress_[un_compress_index_], return_value, return_value_length);
					un_compress_[un_compress_index_][return_value_length] = 0;
					//LOG_DEBUG("return_key:%s, return_value:%s\n", return_key, un_compress_[un_compress_index_]);
					uint64_t n_key;
					if(type_id == 1){
						char *p = strchr((char *)return_key, '-');
						if((p == NULL) || ((p + 1) == NULL)){
							continue;
						}
						n_key =  atoll(p + 1);
						//LOG_DEBUG("return_key:%lld, return_value:%s\n", n_key, p+1);
					}
					else{
						n_key =  atol(return_key);
					}
					//LOG_DEBUG("n_key:%lld, uc:%s\n", n_key, un_compress_[un_compress_index_]);
					map_result[n_key] = un_compress_[un_compress_index_];
					if(return_value != NULL)
						free(return_value);
					un_compress_index_ ++;
					if(un_compress_index_ >= COMPRESS_NUM){
						break;
					}
				}else{
					LOG_DEBUG("mget data is error!");
				}
			}

			struct timeval end_time;
                        gettimeofday(&end_time,NULL);
			uint16_t total_index = svr_ip_port.ip_index_ + svr_ip_port.port_index_ * ip_num_;
			char *ip = (char*)vec_mc_server_[total_index].ip_;
			LOG_ERROR("MC %s consume time %lf mseconds", ip, (end_time.tv_sec-start_time.tv_sec) * 1000 + (double)(end_time.tv_usec - start_time.tv_usec)/1000);
			return 1;
		}


    protected:
		vector<Mc_Struct> vec_mc_server_;
};

#endif


