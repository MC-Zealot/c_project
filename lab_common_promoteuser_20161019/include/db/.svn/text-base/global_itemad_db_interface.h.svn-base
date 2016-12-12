#ifndef _GLOBAL_ITEMAD_DB_INTERFACE_HEADER_
#define _GLOBAL_ITEMAD_DB_INTERFACE_HEADER_
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"

using namespace std;

typedef struct ip_num
{
	uint64_t start_num;
	uint64_t end_num;

	bool operator < (const ip_num &s) const
	{
		return end_num < s.start_num;	
	}
	
	bool operator == (const ip_num &s) const
	{
		return (s.start_num >= start_num && s.end_num <= end_num);
	}
}ip_num;

class GlobalItemAdDbInterface : public GlobalDbInterface
{
	public:
		GlobalItemAdDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct)
		{
		}

		~GlobalItemAdDbInterface()
		{
		}


		private:
			const static int DEFAULT_IP_CODE = 30000;
			std::map<struct ip_num,int> ip_code_dict;
		

		int find_ip(uint64_t ip_number)
		{
			struct ip_num local;
			local.start_num = ip_number;
			local.end_num = ip_number;
			std::map<struct ip_num,int>::iterator iter = ip_code_dict.find(local);
			if(iter != ip_code_dict.end())
				return iter->second;
			return DEFAULT_IP_CODE;
		}

		public:
		int get_ip_code(const string& ip)
		{
			const char *ip_str = ip.c_str();
			struct in_addr inp;
			if ( inet_aton( ip_str, &inp ) == 0 )
			{ 
				LOG_DEBUG("ip string to number wrong!");
				return DEFAULT_IP_CODE;
			}
			uint64_t ip_number = ntohl(inp.s_addr); 
			
			return find_ip(ip_number);
		}


		bool is_exist(uint64_t id)
		{
			return false;
		}		

		int load_db_config()
		{
			char* db_file_name = global_db_info_struct_.db_file_name_;
			if(NULL == db_file_name)
				return -1;

			FILE *fd = NULL;
			char buf[512];
			fd = fopen(db_file_name, "r");
			if(NULL == fd)
			{
				LOG_ERROR("%s read is error!", db_file_name);
				return 0;
			}
			while(!feof(fd))
			{
				fgets(buf,512,fd);
				
				uint64_t ids[3];
				uint32_t id_num = 0;
				split_string_ids(ids,id_num,(const char*)buf, global_db_info_struct_.kv_split_char_,0);
				
				if(id_num != 3)
					continue;
					
				struct ip_num temp;
				temp.start_num = ids[0];
				temp.end_num = ids[1];
				ip_code_dict[temp] = int(ids[2]);
			}

			
			fclose(fd);
			
			return 1;
                }
};

#endif
