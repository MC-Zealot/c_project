#include <stdio.h>
#include <string.h>
#include "fanstarget_ad_algorithm_utility.h"
#include "fanstarget_ad_algorithm_interface.h"

//int high_quality_flowAnalysis(Fans_Tar_Ad *ft){
int high_quality_flowAnalysis(void *fttmp){
	Fans_Tar_Ad *ft = (Fans_Tar_Ad *)fttmp;

	//利用source标记ft->type和里面的细分
	int source = 0;
	if (ft->type == 0){//非粉+高级定向包+万能包
		//string 0开头非粉 1 开头万能包 其它高级定向包
		if (ft->field_id == "0"){//非粉
			source = 0;
		}else if (ft->field_id == "1"){//万能包
			source = 1;
		}else{
			source = 2;
		}
	}else if (ft->type == 1){//补投
		source = 10;
	}else{//精准流量
		char tmp[1024];
		strcpy(tmp, ft->alg_log.c_str());
		char *p = tmp;
		char *p2 = NULL;
		int index = 0;
		char *briuid = NULL;

		while((p2 = strchr(p, '|')) != NULL){
			index ++;
			if(index == 3){
				*p2 = 0;
				briuid = p;
			}
			if(index == 6){
				*p2 = 0;
				//LOG_ERROR("103: %s\n", p);
				if(strcmp(p, "103") == 0){//亲密度优化的非粉
					//LOG_ERROR("true 103:%s, %lld\n", briuid, strtoull(briuid, NULL, 10));
					ft->type = 0;
					ft->ad_type = 4;
					ft->bridge_uid = strtoull(briuid, NULL, 10);
					source = 2103;
				}else if(strcmp(p, "102") == 0){//优化的定向包
					source = 2102;
				}else{
					source = 2100;//精准定向
				}
				break;
			}
			p = p2 + 1;
		}
	}
	return source;
}
