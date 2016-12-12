#include "wax_ad_algorithm_interface.h"
#include "math.h"

#define PRICEFLOOR 500

DYN_ALGORITHMS(WaxAlgorithmInterface);

/*
typedef struct Wax_Ad : public candidate_item_t{
        
        uint16_t impid;//广告位置
        uint32_t bidid;
        uint32_t price;//广告主出价
        uint32_t cost;//真实扣费
        uint64_t custid; //广告主id                                                             
        uint64_t id;//广告id    
        uint64_t dspid;//广告id

        string psid;//广告位 比如:tips  
        string token;//dspToken
        string auction_price;//token加密过的价格,要回传给dsp的
        string extra_info;//算法附加信息                                                        
        string algo_rank_attribute;//算法匹配模式  
        string mark;//加密后mark                                                                
        string mark_origin;//mark                                                                

        CreativeData creative;//素材url
}WAX_AD;*/

int WaxAlgorithmInterface::algorithm_core(uint64_t req_id, const AccessStr& access_str,VEC_CAND& vec_cand){	
    LOG_DEBUG("test new algorithm interface alg_core!");
    return 1;
}

int WaxAlgorithmInterface::algorithm_core_new(uint64_t req_id, const AccessInfo* access_info, const VEC_CAND& input_vec, VEC_CAND& output_vec){
		//debug
		LOG_DEBUG("test new algorithm interface alg_core_new!");
		//LOG_DEBUG("access_info:%s", ((ACCESS_INFO*)access_info)->uid_str.c_str());
		LOG_DEBUG("int_vec:%d", input_vec.size());

		if(input_vec.size()<1){
				return 1;
		}

		map<double, int, greater<double> > price2sortid;
		int index = 0;
		srand((unsigned)time(NULL));
		int rad = 0;
		double radl = 0;
		//save
		map<uint32_t, int> valid_price;
		int sign = 0;
		for(VEC_CAND::const_iterator pos = input_vec.begin(); pos != input_vec.end(); pos++){

				WAX_AD* wa = (WAX_AD*)(*pos);

				LOG_DEBUG("price:%d\n", wa->price);
				if ( price2sortid.find(wa->price)!=price2sortid.end() ){//存在
						rad = rand()%100;
						valid_price[wa->price] = valid_price[wa->price]%98 + 1;
						sign = (rad%2)?1:-1;
						radl = ((double)rad)/100 * sign + ((double)valid_price[wa->price])/10000;
						radl =  wa->price + radl;
						price2sortid[ radl ] = index;
						LOG_DEBUG("radl:%lf\n", radl);
				}
				else{
						price2sortid[ wa->price ] = index;
						valid_price[wa->price] = 1;
				}
				index++;
		}

		//traversal
		map<string, int> hasPosition;
		char extra_log[1024]="";
		map<string, int> aduniq;
		//char tmp[ URLLEN*2 ];
		for(map<double, int>::const_iterator pos = price2sortid.begin(); pos != price2sortid.end(); pos++){
				int index = pos->second;
				WAX_AD* wa = (WAX_AD*)input_vec[ index ];
				//sprintf(tmp, "%ld_%s", wa->dspid, wa->creative.url);
				
				LOG_DEBUG("tmpUniq:%s", wa->uniq_id.c_str());
				if (aduniq.find(wa->uniq_id)==aduniq.end()){
						if(hasPosition.find( wa->impid) == hasPosition.end()){
								//cost 折扣 0.93-0.98
								radl = rand()%6;
								radl = radl/100;
								double costoff = (0.93 + radl);
                                if (wa->deal_id.length()>1){
								    costoff = 1;
								}
								LOG_DEBUG("radl:%f", radl);
								wa->cost = floor(wa->price * costoff) + 1;
								//wa->cost = wa->bidfloor;
								//不能低于底价
								if (wa->cost < wa->bidfloor ){
										wa->cost = wa->bidfloor;
								}
								//不能高于出价
								if (wa->cost > wa->price){
										wa->cost = wa->price;
								}
								output_vec.push_back(wa);
								hasPosition[ wa->impid ] = 1;

								LOG_DEBUG("ok adid:%s, dspid:%d, price:%d,%f, cost:%d, costoff:%f", wa->id.c_str(), wa->dspid, wa->price, pos->first, wa->cost, costoff);
								sprintf(extra_log, "ok adid:%s, dspid:%ld, price:%d,%f, cost:%d, costoff:%f", wa->id.c_str(), wa->dspid, wa->price, pos->first, wa->cost, costoff);
								wa->algo_rank_attribute = extra_log;

								aduniq[ wa->uniq_id ] = 1;
						}

				}
				else{
						LOG_DEBUG("notok adid:%s, dspid:%d, price:%d,%f, cost:%d", wa->id.c_str(), wa->dspid, wa->price, pos->first, wa->cost);
						sprintf(extra_log, "notok adid:%s, dspid:%ld, price:%d,%f, cost:%d", wa->id.c_str(), wa->dspid, wa->price, pos->first, wa->cost);
						wa->algo_rank_attribute = extra_log;
				}
		}

		return 1;
}

