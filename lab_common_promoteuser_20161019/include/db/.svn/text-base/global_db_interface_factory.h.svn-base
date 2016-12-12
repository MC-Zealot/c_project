#ifndef _GLOBAL_DB_INTERFACE_FACTORY_HEADER_
#define _GLOBAL_DB_INTERFACE_FACTORY_HEADER_

#include "global_set_db_interface.h"
#include "global_kv_db_interface.h"
#include "global_map_db_interface.h"
#include "global_fp_db_interface.h"
#include "global_fd_db_interface.h"
#include "global_fw_db_interface.h"
#include "global_ad_db_interface.h"
#include "global_aes_db_interface.h"
#include "global_itemad_db_interface.h"
#include "global_host_db_interface.h"
#include "global_adword_db_interface.h"
#include "global_string_kv_db_interface.h"
#include "global_tfindex_db_interface.h"
#include "global_ftexp_db_interface.h"
//#include "global_algo_db_interface.h"
class GlobalDbInterfaceFactory{

	public:
		GlobalDbInterfaceFactory(){}
		~GlobalDbInterfaceFactory(){}

	public:
		GlobalDbInterface* get_global_db_interface(const GlobalDbInfoStruct& db_info_struct){
			GlobalDbInterface* p_db_interface = NULL;
			switch(db_info_struct.db_type_){
				case GLOBAL_SET_DB_TYPE:
					p_db_interface = new GlobalSetDbInterface(db_info_struct);
					break;
				case GLOBAL_KV_DB_TYPE:
					p_db_interface = new GlobalKVDbInterface(db_info_struct);
					break;
				case GLOBAL_MAP_DB_TYPE:
					p_db_interface = new GlobalMapDbInterface(db_info_struct);
					break;
				case GLOBAL_FP_DB_TYPE:
					p_db_interface = new GlobalFpDbInterface(db_info_struct);
					break;
				case GLOBAL_FD_DB_TYPE:
					p_db_interface = new GlobalFeatureDataDbInterface(db_info_struct);
					break;
				case GLOBAL_FW_DB_TYPE:
					p_db_interface = new GlobalFeatureWeightDbInterface(db_info_struct);
					break;
				case GLOBAL_AD_DB_TYPE:
					p_db_interface = new GlobalAdDbInterface(db_info_struct);
					break;
				case GLOBAL_AES_DB_TYPE:
					p_db_interface = new GlobalAesDbInterface(db_info_struct);
					break;
				case GLOBAL_ITEMAD_DB_TYPE:
					p_db_interface = new GlobalItemAdDbInterface(db_info_struct);
					break;
				case GLOBAL_HOST_DB_TYPE:
					p_db_interface = new GlobalHostDbInterface(db_info_struct);
					break;
				case GLOBAL_ADWORD_DB_TYPE:
					p_db_interface = new GlobalAdWordDbInterface(db_info_struct); 
					break;
				case GLOBAL_STRING_KV_DB_TYPE:
					p_db_interface = new GlobalStringKVDbInterface(db_info_struct);
					break;
                case GLOBAL_TFINDEX_DB_TYPE:
                    p_db_interface = new GlobalTFIndexDbInterface(db_info_struct);
                    break;
                case GLOBAL_FTADID_DB_TYPE:
                    p_db_interface = new GlobalFTAdidDbInterface(db_info_struct);
                    break;
                /*case GLOBAL_ALGO_DB_TYPE:
                    p_db_interface = new GlobalAlgoDbInterface(db_info_struct);
                    break;*/
				default:
					break;	
			}
			return p_db_interface;
		}
};

#endif
