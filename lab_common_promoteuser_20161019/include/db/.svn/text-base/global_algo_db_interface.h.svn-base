#ifndef _GLOBAL_ALGOINIT_DB_INTERFACE_HEADER_
#define _GLOBAL_ALGOINIT_DB_INTERFACE_HEADER_
#include <stdlib.h> 
#include <vector>
#include <string>
#include <set>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

#include "sim_ranking_plugin.h"

using namespace std;

class GlobalAlgoDbInterface : public GlobalDbInterface{
	public:
		GlobalAlgoDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct){
		}
		~GlobalAlgoDbInterface(){
			set_global_data_.clear();
		}
	private:
		SimRankingPlugin *rankingPlugin;
        set<std::string> set_global_data_;
	public:
		bool is_exist(uint64_t id)
		{
			return false;
		}

		/*bool is_exist(const std::string& id){
			set<std::string>::iterator it = set_global_data_.find(id);
			if(it != set_global_data_.end())
				return true;
			else
				return false;
		}*/

        SimRankingPlugin *ret_data(){
            return rankingPlugin;
        }

		int load_db_config(){
			char* db_file_name = global_db_info_struct_.db_file_name_;
			if(NULL == db_file_name)
				return -1;

            rankingPlugin= new SimRankingPlugin();
            rankingPlugin->initialize(db_file_name);
            //rankingPlugin->initialize("./conf/libsim_rank_l1.so.conf");
            std::string name;
            rankingPlugin->getPluginName(name);

		/*	FILE *fd = NULL;  
			char buf[128];  
			fd = fopen(db_file_name, "r");  
			if(NULL == fd){
				LOG_ERROR("%s read is error!", db_file_name);
				return 0;         
			}  
			while(!feof(fd)){  
				fgets(buf, 128, fd); 

				std::string id(buf,strlen(buf)-1);//去掉结尾的\n
				set_global_data_.insert(id);
			}  
			fclose(fd);  
        */
			return 1;
		}
};

#endif
