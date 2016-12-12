#ifndef __SCENCEAD_H__
#define __SCENCEAD_H__
#include "algorithm_interface.h"
#include <vector>
#include <string>

typedef struct history_ctr{
	int click_num;	//点击数
	int inter_num;	//互动数
	int expos_num;	//展示数
	double click_rate;	//点击率
	double inter_rate;	//交互率

}HIS_CTR;

typedef struct poi_info{
	int distance;
	string poiid;
	string categorys;
}POI_INFO;

typedef struct Access_Info :  public AccessInfo {
        Access_Info():has_wifi(false){
        }

	bool has_wifi;
        vector<uint32_t> interest;
        uint16_t time_slice;
	map< uint32_t, vector<uint32_t> > cate_tree;
	string uid;
	string latitude;
	string longitude;
	int scetype;
	
	/*map< uint32_t, HIS_CTR > his_ctr_1;	//早上
	map< uint32_t, HIS_CTR > his_ctr_2;	//中午
	map< uint32_t, HIS_CTR > his_ctr_3;	//下午
	map< uint32_t, HIS_CTR > his_ctr_4;	//晚上
	map< uint32_t, HIS_CTR > his_ctr_5;	//夜里
	*/
	map<uint32_t, map< uint32_t, HIS_CTR > > his_ctr;
	vector<POI_INFO> nearby_pois;
}ACCESS_INFO;


typedef struct Scence_Ad : public candidate_item_t{                                              
	Scence_Ad(){
        }
	
	string item_id;
	uint32_t cate;
        string extra_data;                                                                
	string rec_title;
}SCENCE_AD;
#endif
