#ifndef _GLOBAL_STRING_KV_DB_INTERFACE_HEADER_
#define _GLOBAL_STRING_KV_DB_INTERFACE_HEADER_
#include <stdlib.h> 
#include <vector>
#include <map>
#include <string>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

using namespace std;

class GlobalStringKVDbInterface : public GlobalDbInterface{
	public:
		GlobalStringKVDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct){
		}
		~GlobalStringKVDbInterface(){
			map_global_data_.clear();
		}
	private:
		 std::map<string, double> map_global_data_;
	public:
		bool is_exist(string id){
			std::map<string, double>::iterator it = 
				map_global_data_.find(id);
			if(it != map_global_data_.end())
				return true;
			else
				return false;
		}
		bool is_exist(uint64_t id){
			return false;
		}
		bool get_value(string id, double& value){
			std::map<string, double>::iterator it = 
				map_global_data_.find(id);
			if(it != map_global_data_.end()){
				value = (*it).second;
				return true;
			}
			else
				return false;
		}

		int load_db_config(){
			char* db_file_name = global_db_info_struct_.db_file_name_;
			if(NULL == db_file_name)
				return -1;

			FILE *fd = NULL;  
			char buf[512];  
			fd = fopen(db_file_name, "r");  
			if(NULL == fd){
				LOG_ERROR("%s read is error!", db_file_name);
				return 0;         
			}  
			//LOG_DEBUG("in alg configure load db");
			while(!feof(fd)){  
				//LOG_DEBUG("feof ret:%d", feof(fd));
				fgets(buf, 512, fd); 
				//LOG_DEBUG("feof ret:%d", feof(fd));
				if(feof(fd) == 1){
					break;
				}

				//uint64_t ids[4];
				//uint32_t id_num = 0;
				//split_string_ids(ids, id_num, (const char*)buf, global_db_info_struct_.kv_split_char_, 0);
				const char split_char = global_db_info_struct_.kv_split_char_;
			        string buf_str(buf);
				vector<string> key_val;
				split_string(key_val, buf_str, split_char);
				if(key_val.size() != 2){
				    LOG_ERROR("error alg configure");
				    continue;
				}
				//LOG_DEBUG("alg configure:%s", key_val[0].c_str());
				//LOG_DEBUG("alg configure:%s", key_val[1].c_str());
				double val =strtod(key_val[1].c_str(), NULL);
				map_global_data_.insert(std::make_pair(key_val[0], val));
				//char buf_temp[512];
				//strncpy(buf_temp, buf, 512);
				//LOG_DEBUG("alg configure:%s", buf);

				/*char* key = strtok(buf, &split_char);
				if(NULL == key){
					LOG_ERROR("read configure error");
					continue;
				}
				//LOG_DEBUG("alg configure key:%s", key);
				
				char* value = strtok(NULL, &split_char);
				if(NULL == value){
					LOG_ERROR("read configure error");
					continue;
				}
				//LOG_DEBUG("alg configure value:%s", value);
				double val = strtod(value, NULL);
				//LOG_DEBUG("alg configure value double: %f", val);
				map_global_data_.insert(std::make_pair(string(key), val));*/
				//if(id_num < 2)
				//	continue;

				//hashmap_global_data_.insert(__gnu_cxx::hash_map<uint64_t, uint32_t>::value_type(ids[0] , (uint32_t)ids[1]));
			}  
			fclose(fd);  

			return 1;
		}
};

#endif
