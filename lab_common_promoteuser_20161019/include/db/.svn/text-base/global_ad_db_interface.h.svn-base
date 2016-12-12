#ifndef _GLOBAL_AD_DB_INTERFACE_HEADER_
#define _GLOBAL_AD_DB_INTERFACE_HEADER_
#include <stdlib.h> 
#include <stdint.h>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"
#include "xbson.h"
#include "xbsonjson.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif
#include "pthread_rw_locker.h"

using namespace std;

typedef enum{
	AD_STATUS_RUN = 1,
	AD_STATUS_STOP = 0,
	AD_STATUS_DELETE = -1
}AD_STATUS; // ad status

//
class XBSON{
	public:
		XBSON(uint16_t data_size):bson_data_(NULL){
			if(NULL == bson_data_){
				bson_data_ = new char[data_size];
				memset(bson_data_, 0, data_size);
			}

			if(bson_data_ != NULL)
				xbson_init_data(&bson_, bson_data_, data_size);
		}
		~XBSON(){
			if(bson_data_ != NULL){
				delete bson_data_;
				bson_data_ = NULL;
			}
		}
	private:
		XBSON(){
		}
		char* bson_data_;
		xbson bson_;

	public:
		int init(const char* data_str){
			if(NULL == data_str)
				return -1;
			
			int ret = xbson_from_json(&bson_, data_str);
			return ret;
		}

		int get_str(char buf[], size_t buf_size, size_t& len){

			if(NULL == bson_data_){
				LOG_ERROR("bson data is NULL!");
				return -1;
			}

			return xbson_to_json(&bson_, buf, &len, buf_size);
		}

		int get_sub_str(const char* name, char buf[], 
				size_t buf_size, size_t& len){
			if(NULL == bson_data_){
				LOG_ERROR("bson data is NULL!");
				return -1;
			}

			xbson sub_bson;

			int result = 1;
			if( XBSON_ERROR == xbson_find_subobject(&bson_, name, &sub_bson)){
				
				LOG_ERROR("can not find sub:%s", name);
				
				xbson_iterator it;
				xbson_type type = xbson_find(&it, &bson_, name);
				
				switch(type){
					case XBSON_INT:
						{
							len = snprintf(buf, buf_size, "{\"%s\":%d}", name, xbson_iterator_int(&it));
							result = 1;
						}
						break;
					case XBSON_LONG:
						{
							len = snprintf(buf, buf_size, "{\"%s\":%"PRIu64"}", name, xbson_iterator_long(&it));
							result = 1;
						}
						break;
					case XBSON_DOUBLE:
						{
							len = snprintf(buf, buf_size, "{\"%s\":%f}", name, xbson_iterator_double(&it));
							result = 1;
						}
						break;
					case XBSON_STRING:
						{
							len = snprintf(buf, buf_size, "{\"%s\":\"%s\"}", name, xbson_iterator_string(&it));
							result = 1;
						}
						break;
					case XBSON_BOOL:
						{
							len = snprintf(buf, buf_size, "{\"%s\":%d}", name, (int)xbson_iterator_bool(&it));
							result = 1;
						}
						break;
					default:{
							result = -1;
						}
						break;
				}

				return result;
			}else{

				return xbson_to_json(&sub_bson, buf, &len, buf_size);
			}
			
		}

		int get_sub_strs(const char* names[], size_t name_len, 
				char buf[], size_t buf_size, size_t& len){
			if(NULL == bson_data_){
				LOG_ERROR("bson data is NULL!");
				return -1;
			}

			char* buf_temp = buf;
			len = 0;

			size_t len_temp = snprintf(buf_temp, buf_size, "{");
			len += len_temp;
			buf_temp += len_temp;

			int index = 0;
			for(size_t i = 0; i < name_len; i++){
			
				int result = 1;
				const char* name = names[i];
					
				xbson_iterator it;
				xbson_type type = xbson_find(&it, &bson_, name);
				
				switch(type){
					case XBSON_INT:
						{
							if(index == 0)
								len_temp = snprintf(buf_temp, buf_size, "\"%s\":%d", name, xbson_iterator_int(&it));
							else
								len_temp = snprintf(buf_temp, buf_size, ",\"%s\":%d", name, xbson_iterator_int(&it));
							buf_temp += len_temp;
							len += len_temp;
							result = 1;

							index ++;
						}
						break;
					case XBSON_LONG:
						{
							if(index == 0)
								len_temp = snprintf(buf_temp, buf_size, "\"%s\":%"PRIu64"", name, xbson_iterator_long(&it));
							else
								len_temp = snprintf(buf_temp, buf_size, ",\"%s\":%"PRIu64"", name, xbson_iterator_long(&it));
							buf_temp += len_temp;
							len += len_temp;
							result = 1;

							index ++;
						}
						break;
					case XBSON_DOUBLE:
						{
							if(index == 0)
								len_temp = snprintf(buf_temp, buf_size, "\"%s\":%f", name, xbson_iterator_double(&it));
							else
								len_temp = snprintf(buf_temp, buf_size, ",\"%s\":%f", name, xbson_iterator_double(&it));
							buf_temp += len_temp;
							len += len_temp;
							result = 1;

							index ++;
						}
						break;
					case XBSON_STRING:
						{
							if(index == 0)
								len_temp = snprintf(buf_temp, buf_size, "\"%s\":\"%s\"", name, xbson_iterator_string(&it));
							else
								len_temp = snprintf(buf_temp, buf_size, ",\"%s\":\"%s\"", name, xbson_iterator_string(&it));
							buf_temp += len_temp;
							len += len_temp;
							result = 1;

							index ++;
						}
						break;
					case XBSON_BOOL:
						{
							if(index == 0)
								len_temp = snprintf(buf_temp, buf_size, "\"%s\":%d", name, (int)xbson_iterator_bool(&it));
							else
								len_temp = snprintf(buf_temp, buf_size, ",\"%s\":%d", name, (int)xbson_iterator_bool(&it));
							buf_temp += len_temp;
							len += len_temp;
							result = 1;

							index ++;
						}
						break;
					case XBSON_ARRAY:
					case XBSON_OBJECT:
						{

							xbson sub_bson;
							if(XBSON_ERROR == xbson_find_subobject(&bson_, name, &sub_bson))
								break;

							char buf_array[COMPRESS_LEN];
							size_t buf_len = 0;
							result = xbson_to_json(&sub_bson, buf_array, &buf_len, COMPRESS_LEN);
							if(result == XBSON_ERROR){
								LOG_ERROR("xbson to json is error!");
								continue;
							}else{
								
								if( 0 == index)
									len_temp = snprintf(buf_temp, buf_size, "\"%s\":%s", name, buf_array);
								else
									len_temp = snprintf(buf_temp, buf_size, ",\"%s\":%s", name, buf_array);

								len += len_temp;
								buf_temp += len_temp;
							}

							index ++;

						}
						break;
					default:{
							index = index;
							result = -1;
						}
						break;
					}

			}

			len_temp = snprintf(buf_temp, buf_size, "}");
			buf_temp += len_temp;
			len += len_temp;
		
			return 1;
		}

};

const int ATTR_BSON_LEN = 5120;
const int INDEX_BSON_LEN = 5120;

class AdInfo{
	public:
		AdInfo():ad_bson_(NULL), ad_index_bson_(NULL){
			if(ad_bson_ == NULL){
				ad_bson_ = new XBSON(ATTR_BSON_LEN);
			}

			if(ad_index_bson_ == NULL){
				ad_index_bson_ = new  XBSON(INDEX_BSON_LEN);
			}
		}
		~AdInfo(){
			if(ad_bson_ != NULL){
				delete ad_bson_;
				ad_bson_ = NULL;
			}

			if(ad_index_bson_ != NULL){
				delete ad_index_bson_;
				ad_index_bson_ = NULL;
			}
		}

	private:

		uint64_t ad_id_;
		AD_STATUS ad_status_;
		XBSON* ad_bson_; // bson -- attr
		XBSON* ad_index_bson_; // bson -- index

		/*
		 * {
		 *  "index":{
		 *		"location":[], 
		 *		"age":[], 
		 *		"gender":[], 
		 *		"interest":[],
		 *		"device":[]
		 *		}
		 *  "attr":{}
		 * }
		 */

	public:
		uint64_t get_id(){
			return ad_id_;
		}
		
		AD_STATUS get_status(){
			return ad_status_;
		}
		
		int get_attr(char buf[], size_t buf_size, size_t& len){
			if(NULL == ad_bson_)
				return -1;

			return ad_bson_->get_str(buf, buf_size, len);
		}

		int get_index(char buf[], size_t buf_size, size_t& len){
			if(NULL == ad_index_bson_)
				return -1;

			return ad_index_bson_->get_str(buf, buf_size, len);
		}

		int get_sub_attr(const char* name, char buf[], size_t buf_size, size_t& len){
			if(NULL == ad_bson_)
				return -1;

			return ad_bson_->get_sub_str(name, buf, buf_size, len);
		}

		int get_sub_attrs(const char* names[], size_t name_len, 
				char buf[], size_t buf_size, size_t& len){
			if(NULL == ad_bson_)
				return -1;

			return ad_bson_->get_sub_strs(names, name_len, buf, buf_size, len);
		}


		int get_sub_index(const char* name, char buf[], size_t buf_size, size_t& len){
			if(NULL == ad_index_bson_)
				return -1;

			return ad_index_bson_->get_sub_str(name, buf, buf_size, len);
		}
		
		int get_sub_indexes(const char* names[], size_t name_len,
				char buf[], size_t buf_size, size_t& len){
			if(NULL == ad_index_bson_)
				return -1;

			return ad_index_bson_->get_sub_strs(names, name_len, buf, buf_size, len);
		}

		// set data:
		// 1:init data
		// 2:set status
		int set(uint64_t ad_id,
				AD_STATUS ad_status,
				const char* ad_bson_str,
				const char* ad_bson_index_str){

			ad_id_ = ad_id;
			ad_status_ = ad_status;

			if(NULL == ad_bson_str){
				LOG_ERROR("bson is NULL!");
				return -1;
			}

			if(NULL == ad_bson_){

				LOG_ERROR("ad bson is NULL!");
				ad_bson_ = new XBSON(ATTR_BSON_LEN);
			}

			if(NULL == ad_bson_){
				LOG_ERROR("ad_bson_ is NULL!");
				return -1;
			}

			int result = ad_bson_->init(ad_bson_str);

			if(result < 0)
				return result;

			if(NULL == ad_bson_index_str){
				LOG_ERROR("index bson is NULL!");
				return -1;
			}

			if(NULL == ad_index_bson_){
				LOG_ERROR("ad index bson is NULL!");
				ad_index_bson_ = new XBSON(INDEX_BSON_LEN);
			}

			if(NULL == ad_index_bson_){
				LOG_ERROR("ad_index_bson_ is NULL!");
				return -1;
			}

			result = ad_index_bson_->init(ad_bson_index_str);

			return result;
		}

		int set_status(AD_STATUS ad_status){
			ad_status_ = ad_status;
	
			return 1;
		}
};

typedef __gnu_cxx::hash_map<uint64_t, AdInfo*> HASH_MAP_AD;

typedef std::set<AdInfo*> SET_AD;

typedef __gnu_cxx::hash_map<uint64_t, SET_AD> HASH_MAP_INDEX;

typedef std::set<uint64_t> SET_TARGET;
typedef std::vector<SET_TARGET*> VEC_SET_TARGET;

typedef struct _INFO{
	uint32_t db_size_;
	uint32_t index_size_;
	uint64_t access_time_;
	//......more to be continue;
}INFO;

typedef struct{
	uint8_t type_;
	uint8_t bit_pos_;
	uint16_t default_key_;
}INDEX_ATTR;

class GlobalAdDbInterface : public GlobalDbInterface{
	public:
		GlobalAdDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct){
		
			memset(attr_name_, '\0', sizeof(attr_name_));
			strcpy(attr_name_, "attr");

			memset(index_name_, '\0', sizeof(index_name_));
			strcpy(index_name_, "index");

			memset(main_file_name_, '\0', sizeof(main_file_name_));

		}
		~GlobalAdDbInterface(){
			for(HASH_MAP_AD::iterator it = hash_map_ad_.begin(); 
					it != hash_map_ad_.end(); it++){
				AdInfo* p_ad_info = (*it).second;

				if(p_ad_info != NULL){

					delete p_ad_info;
					p_ad_info = NULL;
				}
			}

			hash_map_ad_.clear();

			for(HASH_MAP_INDEX::iterator it = hash_map_ad_index_.begin();
					it != hash_map_ad_index_.end(); it++){
				(*it).second.clear();
			}

			hash_map_ad_index_.clear();
	
			map_index_sign_.clear();	
		}
	private:
		PthreadRWLocker rw_locker_;

		HASH_MAP_AD hash_map_ad_;

		PthreadRWLocker rw_index_locker_;

		HASH_MAP_INDEX hash_map_ad_index_;

		char attr_name_[WORD_LEN];
		char index_name_[WORD_LEN];
		char main_file_name_[PATH_MAX];
		std::map<std::string, INDEX_ATTR> map_index_sign_;
		
		int load_dump_file(const char* p_dump_file_name){
			if(NULL == p_dump_file_name){
				LOG_ERROR("load all is error!");
				return -1;
			}

			FILE *fd = NULL;

			int buf_len = 16 + INDEX_BSON_LEN + ATTR_BSON_LEN;
			char buf[buf_len]; 
			fd = fopen(p_dump_file_name, "r");  
			if(NULL == fd){
				LOG_ERROR("%s read is error!", p_dump_file_name);
				return 0;
			} 

			int i = 0;
			while(!feof(fd)){ 
				char* p_return = fgets(buf, buf_len, fd); 
				if(NULL == p_return)
					break;

				LOG_INFO("deal with line: %d", i);
				i ++;
				// deal with json
				json_object* req_json = json_tokener_parse(buf);
				if (NULL == req_json){
					LOG_ERROR("%s:json is error!", buf);
					continue;
				}

				if(is_error(req_json)){
					LOG_ERROR("%s:req json is error!", buf);
					json_object_put(req_json);
				
					continue;
				}

				this->set_ad_info(req_json);

				json_object_put(req_json);

			}  
			fclose(fd);  

			return 1;
		}

		int append_file(const char* main_file_name, const char* update_file_name){
			if(NULL == main_file_name || NULL == update_file_name){
				LOG_ERROR("file name is null!");
				return -1;
			}

			FILE* w_main_fd = fopen(main_file_name, "a+");
			
			if(NULL == main_file_name){
				LOG_ERROR("%s append is error!", main_file_name);
				return 0;
			}


			FILE* r_update_fd = fopen(update_file_name, "r");
			if(NULL == r_update_fd){
				LOG_ERROR("%s read is error!", update_file_name);
				
				fclose(w_main_fd);
				return 0;
			} 
			int buf_len = 16 + INDEX_BSON_LEN + ATTR_BSON_LEN;
			char buf[buf_len]; 
			int i = 0;
			while(!feof(r_update_fd)){
				char* p_return = fgets(buf, buf_len, r_update_fd);
			
				if(p_return != NULL){
					fputs(buf, w_main_fd);
					LOG_INFO("deal with line:%d", i);
					i ++;
				}
			}

			fclose(r_update_fd);
			fclose(w_main_fd);

			return 1;
		}


	public:

		char* get_attr_name(){
			return attr_name_;
		}

		char* get_index_name(){
			return index_name_;
		}

		bool is_exist(uint64_t id){
			return false;
		}

		bool get_value(uint64_t id, size_t& length, const char*& value){
			return false;

		}

		bool mget_value(uint64_t ids[], int ids_len, const char* value[], int& value_len){
			return false;
		}

		int load_db_config(){
		/*
		 * [INFO]
		 * ATTR_NAME=attr
		 * INDEX_LIST_NAME=INDEX_LOC,INDEX_AGE,INDEX_PSID,INDEX_FANS
		 * INDEX_LIST_SPLIT_CHAR=44
		 */
			char* db_file_name = global_db_info_struct_.db_file_name_;
			if(NULL == db_file_name)
				return -1;

			char split_char = (char)read_profile_int("INFO", "INDEX_LIST_SPLIT_CHAR",
					44, db_file_name);

			char indexes_name[PATH_MAX];
			if( !read_profile_string("INFO", "INDEX_LIST_NAME",
						 indexes_name, PATH_MAX, "", db_file_name))
			{
				LOG_ERROR("load index name error!");
				return -1;
			}
		
			if( !read_profile_string("INFO", "ATTR_NAME",
						 attr_name_, WORD_LEN, "", db_file_name))
			{
				LOG_ERROR("load attr name error!");
				return -1;
			}
			
			if( !read_profile_string("INFO", "INDEX_NAME",
						 index_name_, WORD_LEN, "", db_file_name))
			{
				LOG_ERROR("load index name error!");
				return -1;
			}
			
			if( !read_profile_string("INFO", "DUMP_FILE_NAME",
						 main_file_name_, PATH_MAX, "", db_file_name))
			{
				LOG_ERROR("load all file name error!");
			}

			std::vector<std::string> vec_index_name;
			split_string(vec_index_name, indexes_name, split_char);

			for(std::vector<std::string>::iterator it = 
					vec_index_name.begin(); it != vec_index_name.end(); it++){
			
				char index_name[WORD_LEN];
				if( !read_profile_string((*it).c_str(), "NAME",
						index_name, WORD_LEN, "", db_file_name)){
					LOG_ERROR("load name error!");
					continue;
				}

				int type = read_profile_int((*it).c_str(), "TYPE",
						0, db_file_name);
			
				int bit_pos = read_profile_int((*it).c_str(), "BIT_POS",
						0, db_file_name);

				int default_key = read_profile_int((*it).c_str(), "DEFAULT_KEY",
						0, db_file_name);

				INDEX_ATTR index_attr;
				index_attr.type_ = (uint8_t)type;
				index_attr.bit_pos_ = (uint8_t)bit_pos;
				index_attr.default_key_ = (uint16_t)default_key;

				map_index_sign_.insert(std::map<std::string, INDEX_ATTR>::value_type(index_name, index_attr));
			}

			this->load_dump_file(main_file_name_);

			return 1;
		}

		static int get_value_uint16(json_object *req_json, uint16_t& value, 
				const char* name, uint16_t default_value){
			if(NULL == req_json){
				LOG_ERROR("req json is NULL");

				return -1;
			}
			
			value = default_value;
			json_object* value_json = json_object_object_get(req_json, name);
			if(NULL == value_json){
				LOG_INFO("%s is null!", name);
				return -1;
			}else{
				value = (uint16_t)json_object_get_int(value_json);
			}

			return 1;
		}

		static int get_value_uint32(json_object *req_json, uint32_t& value, 
				const char* name, uint32_t default_value){
			if(NULL == req_json){
				LOG_ERROR("req json is NULL");

				return -1;
			}
			
			value = default_value;
			json_object* value_json = json_object_object_get(req_json, name);
			if(NULL == value_json){
				LOG_INFO("%s is null!", name);
				return -1;
			}else{
				value = (uint32_t)json_object_get_int(value_json);
			}

			return 1;
		}

		static int get_value_uint64(json_object *req_json, uint64_t& value, 
				const char* name, uint64_t default_value){
			if(NULL == req_json){
				LOG_ERROR("req json is NULL");

				return -1;
			}
			
			value = default_value;
			json_object* value_json = json_object_object_get(req_json, name);
			char* value_str = NULL;
			if(NULL == value_json){
				LOG_INFO("%s is null!", name);

				return -1;
			}else{
				value_str = (char*)json_object_get_string(value_json);
				if(value_str != NULL)
					value = strtoull(value_str, NULL, 10);
				else
					return -1;
			}

			return 1;
		}
		
		static int get_value_float(json_object *req_json, float& value, 
				const char* name, float default_value){
			if(NULL == req_json){
				LOG_ERROR("req json is NULL");

				return -1;
			}
			
			value = default_value;
			json_object* value_json = json_object_object_get(req_json, name);
			if(NULL == value_json){
				LOG_INFO("%s is null!", name);

				return -1;
			}else{
				value = (float)json_object_get_double(value_json);
			}

			return 1;
		}

		static int get_array_int(json_object* req_json, std::vector<uint64_t>& vec_value,
				const char* name){
			if(NULL == req_json){
				LOG_ERROR("req json is NULL");

				return -1;
			}

			json_object* values_json = json_object_object_get(req_json, name);
			if(NULL == values_json || is_error(values_json)){
				LOG_INFO("%s is null!", name);
				return -1;
			}else{
				LOG_ERROR("%s", name);
				if(json_object_is_type(values_json, json_type_array)){
					int n_array_len = json_object_array_length(values_json);
					for(int i = 0; i < n_array_len; i ++){
						json_object* value_object = json_object_array_get_idx(values_json, i);
						
						if(NULL == value_object || is_error(value_object)){
							LOG_ERROR("%d", i);
							continue;
						}

						char* value_str = (char*)json_object_get_string(value_object);
						if(NULL == value_str)
							continue;

						uint64_t value = strtoull(value_str, NULL, 10);
						vec_value.push_back(value);
					}
				}
				else{
					LOG_INFO("%s is not array!", name);
					return -1;
				}
			}

			return 1;

		}

		static int get_str(json_object* req_json, char* &value,
				const char* name){
			if(NULL == req_json){
				LOG_ERROR("req json is NULL");

				return -1;
			}

			json_object* values_json = json_object_object_get(req_json, name);
			if(NULL == values_json || is_error(values_json)){
				LOG_INFO("%s is null!", name);
				return -1;
			}else{	
				value = (char*)json_object_to_json_string(values_json);
				return 1;
			}
		}

		uint64_t change_key(const INDEX_ATTR& index_attr, uint64_t key){
			uint64_t result_key = key << index_attr.bit_pos_;

			return result_key;
		}

		int set_ad_info(json_object *ad_info_json){

			if(NULL == ad_info_json){
				LOG_ERROR("ad info json is NULL");

				return -1;
			}

			uint64_t ad_id = 0;

			int result = get_value_uint64(ad_info_json, ad_id, "ad_id", 0);
			if(result < 0)
				return result;

			uint16_t ad_status = 0;
			result = get_value_uint16(ad_info_json, ad_status, "ad_status", 0);

			char* attr_str = NULL;
			result = get_str(ad_info_json, attr_str, attr_name_);
			
			char* index_str = NULL;
			result = get_str(ad_info_json, index_str, index_name_);

			//above is attribute
			
			AdInfo* ad_info = new AdInfo();
			if(NULL == ad_info){
				LOG_ERROR("new ad info is NULL!");
				return -1;
			}

			result = ad_info->set(ad_id, (AD_STATUS)ad_status, attr_str, index_str);

			if(result < 0){

				delete ad_info;
				LOG_ERROR("set ad info is error!");
				return -1;
			}

			//above is targeting conditions
			AdInfo* old_ad_info = NULL;
			rw_locker_.rdlock();
			HASH_MAP_AD::iterator it = hash_map_ad_.find(ad_id);
			if(it != hash_map_ad_.end()){
				rw_locker_.unlock();

				rw_locker_.wrlock();
				old_ad_info = (*it).second;
				(*it).second = ad_info;

				//need to remove old index
				
				rw_locker_.unlock();

			}else{
				rw_locker_.unlock();

				rw_locker_.wrlock();
				hash_map_ad_.insert(HASH_MAP_AD::value_type(ad_id, ad_info));
				rw_locker_.unlock();

			}

			json_object* index_json = json_object_object_get(ad_info_json, this->index_name_);
			
			if(NULL == index_json || is_error(index_json)){
				LOG_ERROR("index json is NULL!");
			}else{

				for(std::map<std::string, INDEX_ATTR>::iterator it = 
						map_index_sign_.begin(); it != map_index_sign_.end(); it++){
					std::vector<uint64_t> target_keys;

					INDEX_ATTR& index_attr = (*it).second;
					result = get_array_int(index_json, target_keys, (*it).first.c_str());

					if(target_keys.size() == 0)
						target_keys.push_back((uint64_t)index_attr.default_key_);

					for(std::vector<uint64_t>::iterator other_it = 
							target_keys.begin(); other_it != target_keys.end(); other_it ++){
				
						uint64_t key = (*other_it);
						key = change_key(index_attr, key);

						rw_index_locker_.rdlock();
						HASH_MAP_INDEX::iterator index_it = hash_map_ad_index_.find(key);
						if(index_it != hash_map_ad_index_.end()){

							SET_AD& set_ad = (*index_it).second;
							rw_index_locker_.unlock();

							rw_index_locker_.wrlock();
							if(old_ad_info != NULL){

								SET_AD::iterator ad_it = set_ad.find(old_ad_info);
								if(ad_it != set_ad.end()){
									set_ad.erase(ad_it);
								}
							}

							set_ad.insert(ad_info);

							rw_index_locker_.unlock();

						}else{
							rw_index_locker_.unlock();

							SET_AD set_ad;
							set_ad.insert(ad_info);

							rw_index_locker_.wrlock();
							hash_map_ad_index_.insert(HASH_MAP_INDEX::value_type(key, set_ad));
							rw_index_locker_.unlock();
						}

					}
				}
			}
			if(old_ad_info != NULL){
				usleep(5000);
				delete old_ad_info;
				old_ad_info = NULL;
			}

			return 1;
		}

		int load_update_dump_file(const char* update_dump_file_name){
			if(NULL == update_dump_file_name){
				LOG_ERROR("update dump file name is NULL!");
				return -1;
			}

			int result = load_dump_file(update_dump_file_name);
			if(result <= 0){
				LOG_ERROR("load dump file is error:%s", update_dump_file_name);
				return -1;
			}

			result = append_file(this->main_file_name_, update_dump_file_name);
			if(result <= 0){
				LOG_ERROR("append file is error!");
			}

			return 1;
		}

		AdInfo* get_ad_info(uint64_t ad_id){
			AdInfo* p_info = NULL;

			rw_locker_.rdlock();
			HASH_MAP_AD::iterator it = hash_map_ad_.find(ad_id);
			
			if(it != hash_map_ad_.end()){
				p_info = (*it).second;

				rw_locker_.unlock();
			}else{
				rw_locker_.unlock();

				LOG_ERROR("not found:%"PRIu64, ad_id);
			}

			//p_info is also  dangerous
			return p_info;
		}
		
		int get_ad_index_str(uint64_t ad_id, const char* item_keys[], size_t item_key_num, 
				char buf[], size_t buf_size, size_t& len){
			AdInfo* p_info = NULL;

			int result = 1;

			rw_locker_.rdlock();
			HASH_MAP_AD::iterator it = hash_map_ad_.find(ad_id);
			if(it != hash_map_ad_.end()){
				p_info = (*it).second;

				rw_locker_.unlock();
				
				if(NULL == p_info){
					LOG_ERROR("p info is NULL!");
					result = -1;
				}
				else{
					if(0 == item_key_num)
						result = p_info->get_index(buf, buf_size, len);
					else
						result = p_info->get_sub_indexes(item_keys, item_key_num, buf, buf_size, len);
				}

			}else{
				rw_locker_.unlock();

				result = -1;

				LOG_ERROR("not found:%"PRIu64, ad_id);
			}

			//p_info is also  dangerous
			return result;
		}
		int get_ad_attr_str(uint64_t ad_id, const char* item_keys[], size_t item_key_num, 
				char buf[], size_t buf_size, size_t& len){
			AdInfo* p_info = NULL;

			int result = 1;

			rw_locker_.rdlock();
			HASH_MAP_AD::iterator it = hash_map_ad_.find(ad_id);
			
			if(it != hash_map_ad_.end()){
				p_info = (*it).second;

				rw_locker_.unlock();

				if(NULL == p_info){
					LOG_ERROR("p info is NULL!");
					result = -1;
				}
				else{
					if(0 == item_key_num)
						result = p_info->get_attr(buf, buf_size, len);
					else
						result = p_info->get_sub_attrs(item_keys, item_key_num, buf, buf_size, len);
				}

			}else{
				rw_locker_.unlock();

				result = -1;

				LOG_ERROR("not found:%"PRIu64, ad_id);
			}

			//p_info is also  dangerous
			return result;
		}

		int set_ad_status(uint64_t ad_id, AD_STATUS ad_status){
			AdInfo* p_info = NULL;
			
			rw_locker_.rdlock();
			HASH_MAP_AD::iterator it = hash_map_ad_.find(ad_id);

			if(it != hash_map_ad_.end()){
				p_info = (*it).second;

				rw_locker_.unlock();

				rw_locker_.wrlock();
				if(p_info != NULL){
					p_info->set_status(ad_status);

					rw_locker_.unlock();
						
					return 1;
				}
				else{

					rw_locker_.unlock();
					LOG_ERROR("ad info is NULL:%"PRIu64, ad_id);
					return -1;
				}
			}else{
				rw_locker_.unlock();

				LOG_ERROR("not found:%"PRIu64, ad_id);
				return -1;
			}
		}

		int get_ad_status(uint64_t ad_id, AD_STATUS& ad_status){
			AdInfo* p_info = NULL;

			rw_locker_.rdlock();
			HASH_MAP_AD::iterator it = hash_map_ad_.find(ad_id);
			
			if(it != hash_map_ad_.end()){
				p_info = (*it).second;
				rw_locker_.unlock();

				if(p_info != NULL){
					ad_status = p_info->get_status();
					return 1;
				}
				else{
					LOG_ERROR("ad info is NULL:%"PRIu64, ad_id);
					return -1;
				}
			}else{

				rw_locker_.unlock();
				LOG_ERROR("not found:%"PRIu64, ad_id);
				return -1;
			}
		}

		int find_indexes(SET_AD& set_result, json_object* req_json){
		
			if(NULL == req_json || is_error(req_json)){
				LOG_ERROR("req json is NULL!");
				return -1;
			}

			int i = 0;
			for(std::map<std::string, INDEX_ATTR>::iterator it = 
					map_index_sign_.begin(); it != map_index_sign_.end(); it++){
				std::vector<uint64_t> target_keys;

				int result = get_array_int(req_json, target_keys, (*it).first.c_str());

				if(result < 0)
					continue;

				INDEX_ATTR& index_attr = (*it).second;

				target_keys.push_back(uint64_t(index_attr.default_key_));
				SET_AD set_temp;
				
				for(std::vector<uint64_t>::iterator key_it = target_keys.begin();
						key_it != target_keys.end(); key_it ++){
					uint64_t key = (*key_it);

					key = change_key(index_attr, key);
			
					rw_index_locker_.rdlock();

					HASH_MAP_INDEX::iterator ad_index_it = hash_map_ad_index_.find(key);
					if(ad_index_it != hash_map_ad_index_.end()){
						SET_AD& set_ad = (*ad_index_it).second;
						
						rw_index_locker_.unlock();

						set_union(set_temp.begin(), set_temp.end(), set_ad.begin(), set_ad.end(),
								std::insert_iterator<SET_AD>(set_temp, set_temp.begin()));
							
					}else
						rw_index_locker_.unlock();

				}

				SET_AD set_temp_result;

				if(0 == i)
					copy(set_temp.begin(), set_temp.end(), 
							std::insert_iterator<SET_AD>(set_result, set_result.begin()));
				else{
					
					set_intersection(set_temp.begin(), set_temp.end(), set_result.begin(), set_result.end(),
							std::insert_iterator<SET_AD>(set_temp_result, set_temp_result.begin()));
				
					set_result.clear();

					copy(set_temp_result.begin(), set_temp_result.end(), 
							std::insert_iterator<SET_AD>(set_result, set_result.begin()));

				}

				i++;
			}
	
			return 1;
		}

		int find_index(SET_AD& set_result, const VEC_SET_TARGET& vec_set_target){
		
			int i = 0;

			for(VEC_SET_TARGET::const_iterator it = vec_set_target.begin();
					it !=  vec_set_target.end(); it++){

				SET_TARGET* set_target = (*it);
				if(NULL == set_target){
					LOG_ERROR("target is NULL!");
					continue;
				}

				SET_AD set_temp;

				for(SET_TARGET::iterator key_it = set_target->begin();
						key_it != set_target->end(); key_it ++){
				
					uint64_t key = (*key_it);

					rw_index_locker_.rdlock();

					HASH_MAP_INDEX::iterator ad_index_it = hash_map_ad_index_.find(key);
					if(ad_index_it != hash_map_ad_index_.end()){
						SET_AD& set_ad = (*ad_index_it).second;
						
						rw_index_locker_.unlock();

						set_union(set_temp.begin(), set_temp.end(), set_ad.begin(), set_ad.end(),
								std::insert_iterator<SET_AD>(set_temp, set_temp.begin()));
							
					}else
						rw_index_locker_.unlock();
				}

				SET_AD set_temp_result;

				if(0 == i)
					copy(set_temp.begin(), set_temp.end(), 
							std::insert_iterator<SET_AD>(set_result, set_result.begin()));
				else{
					
					set_intersection(set_temp.begin(), set_temp.end(), set_result.begin(), set_result.end(),
							std::insert_iterator<SET_AD>(set_temp_result, set_temp_result.begin()));
				
					set_result.clear();

					copy(set_temp_result.begin(), set_temp_result.end(), 
							std::insert_iterator<SET_AD>(set_result, set_result.begin()));

				}

				i ++;

			}

			return 1;
		}

		int info(INFO& info){

			//rw_locker_.rdlock();
			info.db_size_ = hash_map_ad_.size();
			//rw_locker_.unlock();

			//rw_index_locker_.rdlock();
			info.index_size_ = hash_map_ad_index_.size();
			//rw_index_locker_.unlock();

			return 1;
		}
};

#endif
