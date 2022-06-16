#ifndef PROTO_H__
#define PROTO_H__

#define RCVPORT  "1989"
#define NAMESIZE 11
#define MULGROUP "224.2.2.2"


struct msg_st
{
    uint8_t name[NAMESIZE]; // 采用type_t类型，而不使用int long short等类型
    uint32_t math;
    uint32_t chinese;
}__attribute__((packed));   // 不对齐


#endif