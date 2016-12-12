#ifndef _TFINDEX_HEADER_
#define _TFINDEX_HEADER_
typedef struct {
    string adid;
    uint64_t custid;
    uint64_t owner;
    string mid;
    uint64_t createtime;
    uint64_t endtime;
    string tag_info;
    uint8_t paid_type;  //0:运营  1:自助投放
}TF_INFO;

#endif
