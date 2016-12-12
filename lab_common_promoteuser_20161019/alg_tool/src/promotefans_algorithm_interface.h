#ifndef _PROMOTEFANS_ALGORITHM_INTERFACE_HEADER_
#define _PROMOTEFANS_ALGORITHM_INTERFACE_HEADER_

#include "algorithm_interface.h"
#include "user_ad.h"

float getRandomFloat()
{
	//srand((unsigned)time(NULL));
	float ans = 0.0;
	for(int i=0;i<2 ;i++){
		if (i == 0){
			ans += (rand()%10)/10000.0;
			if ( ans < 1e-7 ) ans = 1/10000.0;
		}
		else{
			ans += (rand()%10)/100000.0;
		}
	}
	return ans;
}
class PromoteFansAlgorithmInterface :public AlgorithmInterface{
	public: 
		PromoteFansAlgorithmInterface(DbCompany*& p_db_company, int interface_id):
			AlgorithmInterface(p_db_company, interface_id){
		}
		~PromoteFansAlgorithmInterface(){
		}
	public:
		int algorithm_core(uint64_t req_id, const AccessStr& access_str,
				VEC_CAND& vec_cand);
		int algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, 
				const VEC_CAND& input_vec, VEC_CAND& output_vec);
		int user_ad_history_ctr(const ACCESS_INFO* ai, const VEC_CAND& input_vec,VEC_CAND& output_vec, int num);
};

#endif
