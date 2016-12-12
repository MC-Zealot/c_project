#ifndef _FANSTOP_REFRESH_ALGORITHM_INTERFACE_HEADER_
#define _FANSTOP_REFRESH_ALGORITHM_INTERFACE_HEADER_

#include "algorithm_interface.h"
#include "fanstop_refresh.h"

typedef struct _lushan{
        int keysize;
        char** keystr;
        map<uint64_t,string> result;
        DbCompany* p_db_company;
}LUSHAN;


class FansTopRefreshAlgorithmInterface :public AlgorithmInterface{
	public: 
		FansTopRefreshAlgorithmInterface(DbCompany*& p_db_company, int interface_id):
			AlgorithmInterface(p_db_company, interface_id){
		}
		~FansTopRefreshAlgorithmInterface(){
		}
	public:
		int algorithm_core(uint64_t req_id, const AccessStr& access_str,
				VEC_CAND& vec_cand);
		int algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, 
				const VEC_CAND& input_vec, VEC_CAND& output_vec);
		int get_ad_user_history_ctr(const ACCESS_INFO* ai, const VEC_CAND& input_vec, VEC_CAND& output_vec,uint16_t type);
};

#endif
