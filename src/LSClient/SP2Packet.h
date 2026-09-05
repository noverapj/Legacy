#pragma once


#include "EncryptValue.h"
#include "ioCustomSoundDefine.h"
#include "ioFileWriteClient.h"
#ifdef XTRAP
#include "Xtrap/ioXtrap.h"
#endif 

#ifdef NPROTECT
#include "nProtect/ioNProtect.h"
#ifndef NPROTECT_CSAUTH3
#include "nProtect/NPGameLib.h"
#endif
#endif // NPROTECT 

#ifdef XIGNCODE
#include "XignCode/ioXignCode.h"
#endif 

#ifdef HACKSHIELD
#include "HackShield/ioHackShield.h"
#endif

struct CHARACTER;
struct ControlKeys;

class SP2Packet : public CPacket
{
public:
	// >> 쪽으로 current_pos 이동
	void  MovePointer( DWORD dwMoveBytes );
	DWORD StrToDwordIP( char *iip );
	void  DwordToStrIP( DWORD dwIP, char *iip );
	void  InitCurPos();

public:
	SP2Packet& operator = ( const SP2Packet &packet );

	bool Write( WORD nLength, BYTE* arg );

	bool Read( bool &arg );
	bool Read( BYTE &arg );
	bool Read( INT64 &arg );
	bool Read( int &arg );
	bool Read( long &arg );
	bool Read( float &arg );
	bool Read( WORD &arg );
	bool Read( DWORD &arg );
	bool Read( ioHashString &arg );
	bool Read( short &arg );
	bool Read( LPTSTR arg );

	bool Read(double &arg);
	bool Read( D3DXVECTOR2 &arg );
	bool Read( D3DXVECTOR3 &arg );
	bool Read( D3DXQUATERNION &arg );
	bool Read( CHARACTER &arg );
	bool Read(CEncrypt<BYTE> &arg);
	bool Read(CEncrypt<bool> &arg);
	bool Read(CEncrypt<int> &arg);
	bool Read(CEncrypt<long> &arg);
	bool Read(CEncrypt<WORD> &arg);
	bool Read(CEncrypt<DWORD> &arg);
	bool Read(CEncrypt<__int64> &arg);
	bool Read(CEncrypt<double> &arg);
	bool Read(CEncrypt<float> &arg);
	bool Read( FilePacket &arg );
	bool Read( ControlKeys &arg );

	// 지정된 사이즈 만큼 읽어야 할때
	bool Read( WORD nLength, BYTE* arg );

public:
	SP2Packet& operator << (BYTE arg);
	SP2Packet& operator << (bool arg);
	SP2Packet& operator << (int arg);
	SP2Packet& operator << (LONG arg);
	SP2Packet& operator << (WORD arg);
	SP2Packet& operator << (DWORD arg);
	SP2Packet& operator << (__int64 arg);
	SP2Packet& operator << (LPTSTR arg);
	SP2Packet& operator << (double arg);
	SP2Packet& operator << (float arg);
	SP2Packet& operator << (short arg);
	SP2Packet& operator << ( const char *arg );
	SP2Packet& operator << ( const ioHashString &arg );
	SP2Packet& operator << ( const D3DXVECTOR2 &arg );
	SP2Packet& operator << ( const D3DXVECTOR3 &arg );
	SP2Packet& operator << ( const D3DXQUATERNION &arg );
	SP2Packet& operator << ( CHARACTER &arg );
	SP2Packet& operator << ( const VOICEDATA &arg );
	SP2Packet& operator << ( const CustomSoundPacket &arg);
	SP2Packet& operator << (CEncrypt<BYTE> &arg);
	SP2Packet& operator << (CEncrypt<bool> &arg);
	SP2Packet& operator << (CEncrypt<int> &arg);
	SP2Packet& operator << (CEncrypt<long> &arg);
	SP2Packet& operator << (CEncrypt<WORD> &arg);
	SP2Packet& operator << (CEncrypt<DWORD> &arg);
	SP2Packet& operator << (CEncrypt<__int64> &arg);
	SP2Packet& operator << (CEncrypt<double> &arg);
	SP2Packet& operator << (CEncrypt<float> &arg);
#ifdef XTRAP
	SP2Packet& operator << ( const XtrapPacket &arg );
#endif 
	SP2Packet& operator << ( const FilePacket &arg );
	SP2Packet& operator << ( const ControlKeys &arg );
#ifdef NPROTECT
#ifdef NPROTECT_CSAUTH3
	SP2Packet&  operator << ( const NProtectPacket &arg );
#else
	SP2Packet&  operator << ( const GG_AUTH_DATA &arg );
#endif 
#endif // NPROTECT
#ifdef XIGNCODE
	SP2Packet&  operator << ( const XignCodePacket &arg );
#endif
#ifdef HACKSHIELD
	SP2Packet&  operator << ( const HackShieldPacket &arg );
#endif

public:
	SP2Packet& operator >> (BYTE &arg);
	SP2Packet& operator >> (bool &arg);
	SP2Packet& operator >> (int &arg);
	SP2Packet& operator >> (LONG &arg);
	SP2Packet& operator >> (WORD &arg);
	SP2Packet& operator >> (DWORD &arg);
	SP2Packet& operator >> (__int64 &arg);
	SP2Packet& operator >> (LPTSTR arg);
	SP2Packet& operator >> (double &arg);
	SP2Packet& operator >> (float &arg);
	SP2Packet& operator >> (short &arg);
	SP2Packet& operator >> ( ioHashString &arg );
	SP2Packet& operator >> ( D3DXVECTOR2 &arg );
	SP2Packet& operator >> ( D3DXVECTOR3 &arg );
	SP2Packet& operator >> ( D3DXQUATERNION &arg );
	SP2Packet& operator >> ( CHARACTER &arg );
	SP2Packet& operator >> ( VOICEDATA &arg );
	SP2Packet& operator >> ( CustomSoundPacket &arg);
	SP2Packet& operator >> (CEncrypt<BYTE> &arg);
	SP2Packet& operator >> (CEncrypt<bool> &arg);
	SP2Packet& operator >> (CEncrypt<int> &arg);
	SP2Packet& operator >> (CEncrypt<long> &arg);
	SP2Packet& operator >> (CEncrypt<WORD> &arg);
	SP2Packet& operator >> (CEncrypt<DWORD> &arg);
	SP2Packet& operator >> (CEncrypt<__int64> &arg);
	SP2Packet& operator >> (CEncrypt<double> &arg);
	SP2Packet& operator >> (CEncrypt<float> &arg);
#ifdef XTRAP
	SP2Packet& operator >> ( XtrapPacket &arg );
#endif 
	SP2Packet& operator >> ( FilePacket &arg );
	SP2Packet& operator >> ( ControlKeys &arg );
#ifdef NPROTECT
#ifdef NPROTECT_CSAUTH3
	SP2Packet&  operator >> ( NProtectPacket &arg );
#else
	SP2Packet&  operator >> ( GG_AUTH_DATA &arg );
#endif
#endif // NPROTECT
#ifdef XIGNCODE
	SP2Packet&  operator >> ( XignCodePacket &arg );
#endif
#ifdef HACKSHIELD
	SP2Packet&  operator >> ( HackShieldPacket &arg );
#endif

public:
	SP2Packet();
	SP2Packet( const SP2Packet &rhs );
	SP2Packet( DWORD dwID );
	SP2Packet( char *szBuf, int iSize );
	SP2Packet( char *szIP, DWORD dwPort, DWORD dwUserIndex, int iIndex, const SP2Packet &rhs );
	SP2Packet( char *szIP, DWORD dwPort, DWORD dwUserIndex, int iIndex, DWORD dwNpcIndex, const SP2Packet &rhs );
	virtual ~SP2Packet();
};

