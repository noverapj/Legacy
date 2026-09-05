#pragma once


//HARDCODE: 이 헤더에는 정말 자주쓰이는, 특히 앞으로 "변하지 않을" define 만을 집어 넣는다.
//			이 헤더를 수정한다는 것은 전체 컴파일을 감수해야한다는것 명심

#define LENGTH_IP	16

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }
#define SAFERELEASE(x)		if(x != NULL) { x->Release(); x = NULL; }

#define PACKET_GUARD_VOID(x)	{ bool rtval = x; if( (rtval)==false ) { return; } }
#define PACKET_GUARD_INT(x)		{ bool rtval = x; if( (rtval)==false ) { return -1; } }
#define PACKET_GUARD_BOOL(x)	{ bool rtval = x; if( (rtval)==false ) { return FALSE; } }
#define PACKET_GUARD_bool(x)	{ bool rtval = x; if( (rtval)==false ) { return false; } }
#define PACKET_GUARD_BREAK(x)	{ bool rtval = x; if( (rtval)==false ) { break; } }
#define PACKET_GUARD_NULL(x)	{ bool rtval = x; if( (rtval)==false ) { return NULL; } }
#define PACKET_GUARD_FLOAT(x)	{ bool rtval = x; if( (rtval)==false ) { return 0.0f; } }

#define PACKET_GUARD_VOID_READ(p, v) { PACKET_GUARD_VOID(p.Read(v));}
#define PACKET_GUARD_INT_READ(p, v) { PACKET_GUARD_INT(p.Read(v));}
#define PACKET_GUARD_BOOL_READ(p, v) { PACKET_GUARD_BOOL(p.Read(v));}
#define PACKET_GUARD_bool_READ(p, v) { PACKET_GUARD_bool(p.Read(v));}
#define PACKET_GUARD_BREAK_READ(p, v) { PACKET_GUARD_BREAK(p.Read(v));}
#define PACKET_GUARD_NULL_READ(p, v) { PACKET_GUARD_NULL(p.Read(v));}
#define PACKET_GUARD_FLOAT_READ(p, v) { PACKET_GUARD_FLOAT(p.Read(v));}

template <typename type>					// RADtoDEG
type  RADtoDEG (type val) {return (type) (val * 57.2957795132);}
template <typename type>					// DEGtoRAD
type  DEGtoRAD (type val) {return (type) (val * 0.0174532925199);}

