#ifndef _GLOBAL_TFINDEX_DB_INTERFACE_HEADER_
#define _GLOBAL_TFINDEX_DB_INTERFACE_HEADER_
#include <stdlib.h>
#include <vector>
#include <string>
#include "global_db_interface.h"
#include "tfindex.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif
#include "pthread_rw_locker.h"

using namespace std;
/*typedef struct ad_info{
        string adid;
        uint64_t custid;
        uint64_t owner;
        string mid;
        uint64_t createtime;
        uint64_t endtime;
        string tag_info;
        uint8_t paid_type;
}AD_INFO;*/
typedef vector<TF_INFO> AD_VEC;
typedef __gnu_cxx::hash_map<uint64_t, AD_VEC> HASH_MAP_TFINDEX;
class GlobalTFIndexDbInterface : public GlobalDbInterface{
    public:
        GlobalTFIndexDbInterface(const GlobalDbInfoStruct& global_db_info_struct) :
            GlobalDbInterface(global_db_info_struct){
        }
        ~GlobalTFIndexDbInterface(){
        }
    private:
        HASH_MAP_TFINDEX hash_map_tfindex;
        PthreadRWLocker rw_tfindex_locker;
        PthreadRWLocker rw_file_locker;
    public:
        int set(TF_INFO ad_info){
            uint64_t owner = ad_info.owner;
            LOG_ERROR("set owner:%u;adid:%s", owner, ad_info.adid.c_str());
            rw_tfindex_locker.rdlock(); 
            HASH_MAP_TFINDEX::iterator tfindex_iter = hash_map_tfindex.find(owner);
            if (tfindex_iter != hash_map_tfindex.end()){
                AD_VEC& ad_vec = hash_map_tfindex[owner];
                AD_VEC::iterator vec_iter;
                for(vec_iter = ad_vec.begin(); vec_iter != ad_vec.end(); vec_iter++){
                    if((*vec_iter).adid == ad_info.adid)
                        break;
                }
                rw_tfindex_locker.unlock();
                rw_tfindex_locker.wrlock();
                if(vec_iter != ad_vec.end()){
                    ad_vec.erase(vec_iter);
                } 
                ad_vec.push_back(ad_info);
                rw_tfindex_locker.unlock();
            }
            else{
                rw_tfindex_locker.unlock();
                AD_VEC ad_vec;
                ad_vec.push_back(ad_info);
                rw_tfindex_locker.wrlock();
                hash_map_tfindex.insert(make_pair(owner, ad_vec)); 
                rw_tfindex_locker.unlock();
            } 
            return 1;
        }

        int get(vector<uint64_t>& follow_list, vector<TF_INFO>& index_result){
            HASH_MAP_TFINDEX::iterator tfindex_iter;
            rw_tfindex_locker.rdlock();
            for(size_t i = 0; i < follow_list.size(); i++){
                tfindex_iter = hash_map_tfindex.find(follow_list[i]);
                if(tfindex_iter != hash_map_tfindex.end()){
                    for( size_t i = 0; i < (*tfindex_iter).second.size(); i++){
                        index_result.push_back((*tfindex_iter).second[i]);
                    }
                }
            }
            rw_tfindex_locker.unlock();
            return 1;
        }
               
        int del(string adid,uint64_t owner){
            LOG_ERROR("del owner:%u;adid:%s", owner, adid.c_str());
            rw_tfindex_locker.rdlock();
            HASH_MAP_TFINDEX::iterator tfindex_iter = hash_map_tfindex.find(owner);
            if( tfindex_iter == hash_map_tfindex.end()){
                rw_tfindex_locker.unlock();
                return -2;
            }
            else{
                AD_VEC& ad_vec = hash_map_tfindex[owner];
                AD_VEC::iterator vec_iter;
                for(vec_iter = ad_vec.begin(); vec_iter != ad_vec.end(); vec_iter++){
                   if((*vec_iter).adid == adid)
                        break;
                }
                rw_tfindex_locker.unlock();
                rw_tfindex_locker.wrlock();
                if(vec_iter != ad_vec.end()){
                    ad_vec.erase(vec_iter);
                    if(ad_vec.size() == 0){
                        hash_map_tfindex.erase(tfindex_iter);
                    }
                }
                else{
                    if(ad_vec.size() == 0){
                        hash_map_tfindex.erase(tfindex_iter);
                    }
                    rw_tfindex_locker.unlock();
                    return -2;
                }
                rw_tfindex_locker.unlock();
            }
            return 1;
        }
 
        int dump(){
            vector<string> adid_vec;
            HASH_MAP_TFINDEX::iterator tfindex_iter;
            rw_tfindex_locker.rdlock();
            for( tfindex_iter = hash_map_tfindex.begin(); tfindex_iter != hash_map_tfindex.end(); tfindex_iter++){
                for( size_t i = 0; i< (*tfindex_iter).second.size(); i++){
                    adid_vec.push_back((*tfindex_iter).second[i].adid);
                }    
            }
            rw_tfindex_locker.unlock();
            string dump_adid = "";
            for(size_t i =0; i < adid_vec.size(); i++){
                dump_adid = dump_adid + adid_vec[i] + "|";
            }
            if(dump_adid.size()!=0)
                dump_adid = dump_adid.substr(0,dump_adid.size()-1);
            rw_file_locker.wrlock();
            FILE *dump_file = NULL;
            dump_file = fopen("../log/index_dump.log","w");
            if(NULL == dump_file){
                rw_file_locker.unlock();
                return -1;
            } 
            char* result_data = (char*)dump_adid.c_str();
            LOG_ERROR("dump data is:%s",result_data);
            fwrite(result_data,strlen(result_data),1,dump_file);
            fclose(dump_file);
            rw_file_locker.unlock();            
            return 1;
        }

        int rebuild(){
            HASH_MAP_TFINDEX new_map;
            rw_file_locker.wrlock();
            FILE *mysql_file = NULL;
            char buf[256];
            mysql_file = fopen("../log/mysql_data.log","r");
            if(NULL == mysql_file){
                rw_file_locker.unlock();
                return -1;
            }
            while(!feof(mysql_file)){
                fgets(buf,256,mysql_file);
                string temp(buf);
                string raw_data = temp.substr(0,temp.size()-1);
                vector<string> vec_data;
                split_string(vec_data,raw_data,'#');
                TF_INFO ad_info;
                ad_info.adid = vec_data[0];
                ad_info.mid = vec_data[1];
                ad_info.owner = (uint64_t)atoll(vec_data[2].c_str());
                ad_info.custid = (uint64_t)atoll(vec_data[3].c_str());
                ad_info.createtime = (uint64_t)atoll(vec_data[4].c_str());
                ad_info.endtime = (uint64_t)atoll(vec_data[5].c_str());
                ad_info.tag_info = vec_data[6];
                ad_info.paid_type = (uint8_t)atoi(vec_data[7].c_str());
                HASH_MAP_TFINDEX::iterator tfindex_iter = new_map.find(ad_info.owner);
                if (tfindex_iter != new_map.end()){
                    AD_VEC& ad_vec = new_map[ad_info.owner];
                    ad_vec.push_back(ad_info);
                } 
                else{
                    AD_VEC ad_vec;
                    ad_vec.push_back(ad_info);
                    new_map.insert(make_pair(ad_info.owner,ad_vec));
                }
            }
            rw_file_locker.unlock();
            rw_tfindex_locker.wrlock();
            hash_map_tfindex.swap(new_map);
            rw_tfindex_locker.unlock();
            new_map.clear();
            return 1;
        }

        virtual bool is_exist(uint64_t id) {
            return false;
        }

        virtual int load_db_config(){
            return 1;
        }
};

#endif
