#ifndef _GLOBAL_FTEXP_DB_INTERFACE_HEADER_
#define _GLOBAL_FTEXP_DB_INTERFACE_HEADER_
#include <stdlib.h>
#include <vector>
#include <string>
#include "global_db_interface.h"
//#include "fanstop_refresh.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif
#include "pthread_rw_locker.h"

using namespace std;
//粉条订单存放的hash_map
//typedef __gnu_cxx::hash_map<string, string> HASH_MAP_FTADID;
typedef map<string, string> HASH_MAP_FTADID;
#define ADID_CONF_PATH "../data/fanstop_profile.txt"

class GlobalFTAdidDbInterface : public GlobalDbInterface{
    public:
        GlobalFTAdidDbInterface(const GlobalDbInfoStruct& global_db_info_struct) :
            GlobalDbInterface(global_db_info_struct){
        }
        ~GlobalFTAdidDbInterface(){
        }
    private:
        HASH_MAP_FTADID hash_map_adid;
        PthreadRWLocker rw_tfindex_locker;
        PthreadRWLocker rw_file_locker;
    public:

        //增加get的接口
        bool get_adinfo(const string& adid,string& value){
            if (adid.empty()){
                LOG_ERROR("adid is ERROR");
                return false;
            }else
            {
                for(HASH_MAP_FTADID::iterator adid_iter = hash_map_adid.begin();
                        adid_iter != hash_map_adid.end();adid_iter++){
                    HASH_MAP_FTADID::iterator find_iter = hash_map_adid.find(adid);
                    if(find_iter != hash_map_adid.end()){
                        value = (*find_iter).second;
                        return true;
                    }
                    else{
                        LOG_ERROR("get adid %s is not exist!",adid.c_str());
                        return false;
                    }
                }
                return false;
            }
            return false;
        }
        //更新配置文件
        int update_adid_file(){
            HASH_MAP_FTADID new_map;
            rw_file_locker.wrlock();
            FILE *adid_file = NULL;
            char buf[256];
            adid_file = fopen(ADID_CONF_PATH,"r");
            if(NULL == adid_file){
                rw_file_locker.unlock();
                LOG_ERROR("read adid file is ERROR!");
                return -1;
            }
            while(fgets(buf,256,adid_file) != NULL){
                string temp(buf);
                string raw_data = temp.substr(0,temp.size()-1);
                vector<string> vec_data;
                //分隔符待定
                split_string(vec_data,raw_data,'\t');
                string adid_key = vec_data[0];
                string adid_value = vec_data[1];
                HASH_MAP_FTADID::iterator adid_iter = new_map.find(adid_key);
                if (adid_iter != new_map.end()){
                    LOG_ERROR("adid %s has aleady in hash_map",adid_key.c_str());
                } 
                else{
                    new_map.insert(make_pair(adid_key,adid_value));
                }
            }
            rw_file_locker.unlock();
            rw_tfindex_locker.wrlock();
            hash_map_adid.swap(new_map);
            //查询hash_map加载的对象
            show_hashmap_items();
            rw_tfindex_locker.unlock();
            new_map.clear();
            return 1;
        }

        virtual bool is_exist(uint64_t id) {
            return false;
        }

        virtual int load_db_config(){
            int res = update_adid_file();
            if ( res<1) {
                LOG_ERROR("init adid file is ERROR");
            }else
            {
                LOG_ERROR("init adid file OK!");
            }
            return 1;
        }

        void show_hashmap_items(){
            for(HASH_MAP_FTADID::iterator adid_iter = hash_map_adid.begin();
                    adid_iter != hash_map_adid.end(); adid_iter++){
                LOG_ERROR("adid file item key is %s,value is %s",(*adid_iter).first.c_str(),(*adid_iter).second.c_str());
            }
        }
};

#endif
