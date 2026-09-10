#ifndef __StringProtect_h__
#define __StringProtect_h__

class ioHashString;

namespace SP
{
	void EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize );
	void Decode( IN const char *szCipher, IN int iCipherSize, OUT char* szPlain, IN int iPlainSize  );
	const char *GetDecodeStr( const char *szCipher );
	const char *GetAESDecodeStr( IN const char *szCipher, IN DWORD dwKeyAndSize, OUT ioHashString &rsReturn );
	const char *GetRC5DecodeStr( IN const char *szCipher, IN DWORD dwEncryptSize, OUT ioHashString &rsReturn );
}

#ifndef SHIPPING //UJ
	//fall back to native sprintf() and wsprintf()
	#define sprintf_e sprintf
	#define sprintf_s_e sprintf_s
	#define wsprintf_e wsprintf
	#define StringCbPrintf_e StringCbPrintf
	#define lua_pushstring_e lua_pushstring
	#define _ENCSTR(s,e)	char e[]=s;
#else
//
//UJ 130809, compile-time에서 string literal을 암호화하는 코드
//
// 아래 코드를 사용해서 exe나 data segment에서 plain string을 숨길 수 있다
// 숨겨진 string은 4 bytes integer로 나뉘어 xor 연산후 mov 코드로 입력이 된다
// 물론 실제 사용되는 시점에는 stack에 char*로 암호화가 풀린채로 노출이 되지만 
// 사용이 끝나면 바로 해제되기 때문에 평범한 hack이나 cheat는 대부분 막을 수 있음
//
// Macro expansion 코드가 길고 난잡하지만 컴파일러에서 최적화되기에 cost는 준수함
// 다만 일부 너무 긴 파일(ioBaseChar.cpp, 1.6MB)은 컴파일러에 부담이라 코드를 최대한 짧게 짰다
//
// 주의1: 최대 48글자(49bytes)까지만 지원, 넘으면 컴파일 오류발생
// 주의2: debug 빌드에서는 string 보호가 되지 않는다
// 주의3: 문자열 상수 말고 char* szKey="123"이나 char szBuf[MAX_PATH]=..등을 넘기면 컴파일 오류발생함
// 주의4: 3글자(4bytes, 예를들어 max, red 따위) 처리도 되지 않는다, 주의3에서 포인터 오작동에 대비하느라..
//
__forceinline void _DECSTR(char* s, int l){int i=0,l4=(l-1)/4,*p=(int*)s;s[l-1]=0;for(;i<l4;++i)*(p++)^=0xEAEBECED;for(i=l4*4;i<l-1;++i)s[i]^=0xED-i%4;}

#define _ENCCHR(x,y)	x^(0xEA+y)
#define _ENCSTR(s,e)	char e[sizeof(s)];__ENCSTR(s,e)
#define __ENCSTR(s,e)	STATIC_ASSERT(sizeof(s)!=4&&sizeof(s)<=49);_ENC_00(s,e);_DECSTR(e,sizeof(s));

#define _ENC_00(s,e)	if(s[0]){e[0]=_ENCCHR(s[0],3);_ENC_01(s,e);}
#define _ENC_01(s,e)	if(s[1]){e[1]=_ENCCHR(s[1],2);_ENC_02(s,e);}
#define _ENC_02(s,e)	if(s[2]){e[2]=_ENCCHR(s[2],1);_ENC_03(s,e);}
#define _ENC_03(s,e)	if(s[3]){e[3]=_ENCCHR(s[3],0);_ENC_04(s,e);}
#define _ENC_04(s,e)	if(s[4]){e[4]=_ENCCHR(s[4],3);_ENC_05(s,e);}
#define _ENC_05(s,e)	if(s[5]){e[5]=_ENCCHR(s[5],2);_ENC_06(s,e);}
#define _ENC_06(s,e)	if(s[6]){e[6]=_ENCCHR(s[6],1);_ENC_07(s,e);}
#define _ENC_07(s,e)	if(s[7]){e[7]=_ENCCHR(s[7],0);_ENC_08(s,e);}
#define _ENC_08(s,e)	if(s[8]){e[8]=_ENCCHR(s[8],3);_ENC_09(s,e);}
#define _ENC_09(s,e)	if(s[9]){e[9]=_ENCCHR(s[9],2);_ENC_10(s,e);}
#define _ENC_10(s,e)	if(s[10]){e[10]=_ENCCHR(s[10],1);_ENC_11(s,e);}
#define _ENC_11(s,e)	if(s[11]){e[11]=_ENCCHR(s[11],0);_ENC_12(s,e);}
#define _ENC_12(s,e)	if(s[12]){e[12]=_ENCCHR(s[12],3);_ENC_13(s,e);}
#define _ENC_13(s,e)	if(s[13]){e[13]=_ENCCHR(s[13],2);_ENC_14(s,e);}
#define _ENC_14(s,e)	if(s[14]){e[14]=_ENCCHR(s[14],1);_ENC_15(s,e);}
#define _ENC_15(s,e)	if(s[15]){e[15]=_ENCCHR(s[15],0);_ENC_16(s,e);}
#define _ENC_16(s,e)	if(s[16]){e[16]=_ENCCHR(s[16],3);_ENC_17(s,e);}
#define _ENC_17(s,e)	if(s[17]){e[17]=_ENCCHR(s[17],2);_ENC_18(s,e);}
#define _ENC_18(s,e)	if(s[18]){e[18]=_ENCCHR(s[18],1);_ENC_19(s,e);}
#define _ENC_19(s,e)	if(s[19]){e[19]=_ENCCHR(s[19],0);_ENC_20(s,e);}
#define _ENC_20(s,e)	if(s[20]){e[20]=_ENCCHR(s[20],3);_ENC_21(s,e);}
#define _ENC_21(s,e)	if(s[21]){e[21]=_ENCCHR(s[21],2);_ENC_22(s,e);}
#define _ENC_22(s,e)	if(s[22]){e[22]=_ENCCHR(s[22],1);_ENC_23(s,e);}
#define _ENC_23(s,e)	if(s[23]){e[23]=_ENCCHR(s[23],0);_ENC_24(s,e);}
#define _ENC_24(s,e)	if(s[24]){e[24]=_ENCCHR(s[24],3);_ENC_25(s,e);}
#define _ENC_25(s,e)	if(s[25]){e[25]=_ENCCHR(s[25],2);_ENC_26(s,e);}
#define _ENC_26(s,e)	if(s[26]){e[26]=_ENCCHR(s[26],1);_ENC_27(s,e);}
#define _ENC_27(s,e)	if(s[27]){e[27]=_ENCCHR(s[27],0);_ENC_28(s,e);}
#define _ENC_28(s,e)	if(s[28]){e[28]=_ENCCHR(s[28],3);_ENC_29(s,e);}
#define _ENC_29(s,e)	if(s[29]){e[29]=_ENCCHR(s[29],2);_ENC_30(s,e);}
#define _ENC_30(s,e)	if(s[30]){e[30]=_ENCCHR(s[30],1);_ENC_31(s,e);}
#define _ENC_31(s,e)	if(s[31]){e[31]=_ENCCHR(s[31],0);_ENC_32(s,e);}
#define _ENC_32(s,e)	if(s[32]){e[32]=_ENCCHR(s[32],3);_ENC_33(s,e);}
#define _ENC_33(s,e)	if(s[33]){e[33]=_ENCCHR(s[33],2);_ENC_34(s,e);}
#define _ENC_34(s,e)	if(s[34]){e[34]=_ENCCHR(s[34],1);_ENC_35(s,e);}
#define _ENC_35(s,e)	if(s[35]){e[35]=_ENCCHR(s[35],0);_ENC_36(s,e);}
#define _ENC_36(s,e)	if(s[36]){e[36]=_ENCCHR(s[36],3);_ENC_37(s,e);}
#define _ENC_37(s,e)	if(s[37]){e[37]=_ENCCHR(s[37],2);_ENC_38(s,e);}
#define _ENC_38(s,e)	if(s[38]){e[38]=_ENCCHR(s[38],1);_ENC_39(s,e);}
#define _ENC_39(s,e)	if(s[39]){e[39]=_ENCCHR(s[39],0);_ENC_40(s,e);}
#define _ENC_40(s,e)	if(s[40]){e[40]=_ENCCHR(s[40],3);_ENC_41(s,e);}
#define _ENC_41(s,e)	if(s[41]){e[41]=_ENCCHR(s[41],2);_ENC_42(s,e);}
#define _ENC_42(s,e)	if(s[42]){e[42]=_ENCCHR(s[42],1);_ENC_43(s,e);}
#define _ENC_43(s,e)	if(s[43]){e[43]=_ENCCHR(s[43],0);_ENC_44(s,e);}
#define _ENC_44(s,e)	if(s[44]){e[44]=_ENCCHR(s[44],3);_ENC_45(s,e);}
#define _ENC_45(s,e)	if(s[45]){e[45]=_ENCCHR(s[45],2);_ENC_46(s,e);}
#define _ENC_46(s,e)	if(s[46]){e[46]=_ENCCHR(s[46],1);_ENC_47(s,e);}
#define _ENC_47(s,e)	if(s[47]){e[47]=_ENCCHR(s[47],0);}
//#define _ENC_47(s,e)	if(s[47]){e[47]=_ENCCHR(s[47],0);_ENC_48(s,e);}
//#define _ENC_48(s,e)	if(s[48]){e[48]=_ENCCHR(s[48],3);_ENC_49(s,e);}
//#define _ENC_49(s,e)	if(s[49]){e[49]=_ENCCHR(s[49],2);_ENC_50(s,e);}
//#define _ENC_50(s,e)	if(s[50]){e[50]=_ENCCHR(s[50],1);_ENC_51(s,e);}
//#define _ENC_51(s,e)	if(s[51]){e[51]=_ENCCHR(s[51],0);_ENC_52(s,e);}
//#define _ENC_52(s,e)	if(s[52]){e[52]=_ENCCHR(s[52],3);_ENC_53(s,e);}
//#define _ENC_53(s,e)	if(s[53]){e[53]=_ENCCHR(s[53],2);_ENC_54(s,e);}
//#define _ENC_54(s,e)	if(s[54]){e[54]=_ENCCHR(s[54],1);_ENC_55(s,e);}
//#define _ENC_55(s,e)	if(s[55]){e[55]=_ENCCHR(s[55],0);_ENC_56(s,e);}
//#define _ENC_56(s,e)	if(s[56]){e[56]=_ENCCHR(s[56],3);_ENC_57(s,e);}
//#define _ENC_57(s,e)	if(s[57]){e[57]=_ENCCHR(s[57],2);_ENC_58(s,e);}
//#define _ENC_58(s,e)	if(s[58]){e[58]=_ENCCHR(s[58],1);_ENC_59(s,e);}
//#define _ENC_59(s,e)	if(s[59]){e[59]=_ENCCHR(s[59],0);_ENC_60(s,e);}
//#define _ENC_60(s,e)	if(s[60]){e[60]=_ENCCHR(s[60],3);_ENC_61(s,e);}
//#define _ENC_61(s,e)	if(s[61]){e[61]=_ENCCHR(s[61],2);_ENC_62(s,e);}
//#define _ENC_62(s,e)	if(s[62]){e[62]=_ENCCHR(s[62],1);_ENC_63(s,e);}
//#define _ENC_63(s,e)	e[63]=0;

// 일괄 변경한 형태
// 1. sprintf( szBuf, "", ... )
// 2. wsprintf( szBuf, "", ... )
// 3. sprintf_s( szBuf, length, "", ... )
// 4. kLoader.SetTitle( "" );
// 5. var = kLoader.LoadInt( "", ... )
// 6. var = kLoader.LoadBool( "", ... )
// 7. ...
//
// do..while(0)은 semicolon 제거하기 위한 트릭, 컴파일러 최적화로 no extra code
#define sprintf_e(x, src, ...) \
do{_ENCSTR(src,__encStr);sprintf(x,__encStr,__VA_ARGS__);}while(0) 
#define sprintf_s_e(x, src, ...) \
do{_ENCSTR(src,__encStr);sprintf_s(x,__encStr,__VA_ARGS__);}while(0)
#define wsprintf_e(x, src, ...) \
do{_ENCSTR(src,__encStr);wsprintf(x,__encStr,__VA_ARGS__);}while(0)
#define StringCbPrintf_e(x, len, src, ...) \
do{_ENCSTR(src,__encStr);StringCbPrintf(x,len,__encStr,__VA_ARGS__);}while(0) 
#define lua_pushstring_e(ls, src) \
do{_ENCSTR(src,__encStr);lua_pushstring(ls,__encStr);}while(0)
#endif // _DEBUG

//UJ 130909, 문자열 상수대신 포인터를 집어넣으면 오작동하는 문제를 해결하기 위해 아예 컴파일러 에러를 내뱉도록함
// 다만, 그결과 3글자 문자열은 처리할 수 없게 되었다. 제대로 상수와 포인터 구분을 하려면 C++11이 필요함...
namespace interslice {
	template<bool b> struct _FAIL;
	template<>       struct _FAIL<true>{enum{v=1};};
	template<int x>  struct _TEST{};
}
#define PASTE(x,y)			x##y
#define STATIC_ASSERT(x)	typedef ::interslice::_TEST<sizeof(::interslice::_FAIL<(bool)(x)>)> PASTE(Failed_,__COUNTER__)

#endif // __StringProtect_h__