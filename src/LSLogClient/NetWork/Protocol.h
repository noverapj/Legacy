#ifndef ___PROTOCOL_H__
#define ___PROTOCOL_H__

//CLIENT SEND TCP PACKET
#define CTPK_CONNECT              0x0001
#define CTPK_PING                 0x0002

//SERVER SEND TCP PACKET
#define STPK_CONNECT              0x2001
#define CONNECT_OK                1
#define CONNECT_DUPLICATE         2
#define CONNECT_WRONG_CHECK_VALE  3

#define STPK_LOG                  0x2002
#define STPK_PING                 0x2003


//CLIENT SEND UCP PACKET
#define LUPK_LOG                  0x9000



#endif