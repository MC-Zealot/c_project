#include <time.h>
#include "fanstarget_ad_algorithm_high_quality.h"
#include <sys/time.h>
#include <woo/log.h>
#include <stdlib.h>

// 广告主id倒数第二位为4的作为普通组
// 其他为灰度组
bool fc_ABtestCheck(uint64_t mod){
	//if(mod == 4){
	//全量放开
	if(true){
		return true;    
	}                       
	return false;           
}

double fc_get_random(){
        struct timeval now;
        gettimeofday(&now,NULL);
        srand((unsigned)now.tv_usec);
        return rand()/(RAND_MAX+1.0);
}

double fc_get_curveY(uint64_t mod, int difftime){
	/*if(mod != 3){
		//^3*尾号的广告主前1200秒,按照时间推移平滑投放,上线时间越久,投放几率越大,
		//1200秒时几率到达1
		
		return ((double)difftime/extenttime);
	}*/
	double score = (double)difftime/60;
	if(score <= 30){
		return score * score /4000;
	}
	else if(score <= 40){
		return 0.003875 * (score - 25) * (score - 25) + 0.128125;
	}
	else{
		return 1;
	}
	return 1;
}

//bool flowControl(uint64_t order_uid, uint32_t begin_time, int randomDecay = 1, int source = 0){
bool flowControl(uint64_t order_uid, uint32_t begin_time, uint32_t finish_amount, double theory_impression, int randomDecay = 1, uint32_t source = 0){
	
	//LOG_ERROR("inner flowControl source: %d", source);	
	uint64_t mod = order_uid % 100 / 10;

	struct timeval start_time;
	gettimeofday(&start_time,NULL);

	int difftime = (start_time.tv_sec - begin_time);
	if(difftime < 1){
		difftime = 1;
	}
	//if (source == 0 || source == 1 || source == 2 || source == 2103){
	if (source == 0 || source == 2103){
		if(difftime < 2400){
			double chance = fc_get_random();
			double curveY = fc_get_curveY(mod, difftime);
			if(chance > curveY ){
				return true;
			}
		}
	}
	if (source == 3 || source == 2100 || source == 10){//不控制指定账号
		return false;
	}
	if(fc_ABtestCheck(mod)){
		if (theory_impression * 8 < finish_amount){
			return true;
		}
	}
	return false;
}
