#ifndef _SP2Packet_h_
#define _SP2Packet_h_

#ifdef XTRAP
#include "../Xtrap/ioXtrap.h"
#endif

#ifdef NPROTECT
#include "../nProtect/ioNProtect.h"
#ifndef NPROTECT_CSAUTH3
#include "../nProtect/ggsrv25.h"
#endif 
#endif // NPROTECT

#ifdef XIGNCODE
#include "XignCode/ioXignCode.h"
#endif 

#ifdef HACKSHIELD
#include "HackShield/AntiCpXSvr.h"
#endif

#ifdef SRC_LATIN
#include "Apex/ioApex.h"
#endif

#include "MonitoringServerNode/MonitoringNode.h"

class CPacket;
class CQueryData;
class CQueryResultData;

struct CHARACTER;
struct GAMESERVERINFO;

class SP2Packet : public CPacket
{
public:
	SP2Packet();
	SP2Packet( const SP2Packet &rhs );
	SP2Packet(DWORD ID);
	SP2Packet(char *buffer,int size);
	SP2Packet( DWORD dwUserIndex, SP2Packet &rhs );
#ifdef ANTIHACK
	SP2Packet(DWORD ID, bool bClear, PacketFlowTypes pkflowType, int iSize);
	SP2Packet(bool bClear, PacketFlowTypes pkflowType, int iSize);
#endif
	virtual ~SP2Packet();

	const char* GetData() const;
	int GetDataSize() const;
	const char* GetBuffer() const;	
	int   GetBufferSize() const;
	void  SetDataAdd( char *buffer, int size, bool bCurPosReSet = false );
	void  SetDataAddCreateUDP( DWORD dwIP, DWORD dwPort, char *buffer, int size, bool bCurPosReSet = false );
	// >> 쪽으로 current_pos 이동
	void  MovePointer( DWORD dwMoveBytes );
	void  MoveBufferPointer(int size); //For UDP Packet
	void  GetSockAddress(sockaddr_in& arg);//For UDP Packet
	void  SetPosBegin();
	//operator
public:
	SP2Packet&  operator =  ( const SP2Packet& packet );
	SP2Packet&  operator << (BYTE arg);
	SP2Packet&  operator << (bool arg);
	SP2Packet&  operator << (int arg);
	SP2Packet&  operator << (LONG arg);
	SP2Packet&  operator << (WORD arg);
	SP2Packet&  operator << (DWORD arg);
	SP2Packet&  operator << (__int64 arg);
	SP2Packet&  operator << (LPTSTR arg);
	SP2Packet&  operator << (double arg);	
	SP2Packet&  operator << (float arg);
	SP2Packet&  operator << (short arg);
	SP2Packet&  operator << ( CHARACTER &arg );
	SP2Packet&  operator << ( const ioHashString &arg );
	SP2Packet&  operator << ( const Vector3 &arg );
	SP2Packet&  operator << ( const Quaternion &arg );
	SP2Packet&  operator << (CQueryData &arg);
	SP2Packet&  operator << (CQueryResultData &arg);
#ifdef XTRAP
	SP2Packet&  operator << ( const XtrapPacket &arg );
#endif
	SP2Packet&  operator << ( const ControlKeys &arg );
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
	SP2Packet&  operator << ( const AHNHS_TRANS_BUFFER &arg );
#endif
#ifdef SRC_LATIN
	SP2Packet&  operator << ( const ApexPacket &arg );
#endif
	SP2Packet&  operator << ( const MonitorStatusRequest &arg );
	SP2Packet&  operator << ( const MonitorStatusResult &arg );
	SP2Packet&  operator << ( const MonitorChangeRequest &arg );
	SP2Packet&  operator << ( const MonitorChangeResult &arg );
	SP2Packet&	operator << ( const GAMESERVERINFO& arg );
	SP2Packet&  operator << ( const sockaddr_in& arg);//For UDP Packet


	SP2Packet&  operator >> (BYTE &arg);
	SP2Packet&  operator >> (bool &arg);
	SP2Packet&  operator >> (int &arg);
	SP2Packet&  operator >> (LONG &arg);
	SP2Packet&  operator >> (WORD &arg);
	SP2Packet&  operator >> (DWORD &arg);
	SP2Packet&  operator >> (__int64 &arg);
	SP2Packet&  operator >> (LPTSTR arg);
	SP2Packet&  operator >> (double &arg);	
	SP2Packet&  operator >> (float &arg);	
	SP2Packet&  operator >> (short &arg);
	SP2Packet&  operator >> (CHARACTER &arg);
	SP2Packet&  operator >> ( ioHashString &arg );
	SP2Packet&  operator >> ( Vector3 &arg );
	SP2Packet&  operator >> ( Quaternion &arg );
	SP2Packet&  operator >> (CQueryData &arg);
	SP2Packet&  operator >> (CQueryResultData &arg);
#ifdef XTRAP
	SP2Packet&  operator >> ( XtrapPacket &arg );
#endif
	SP2Packet&  operator >> ( ControlKeys &arg );
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
	SP2Packet&  operator >> ( AHNHS_TRANS_BUFFER &arg );
#endif
#ifdef SRC_LATIN
	SP2Packet&  operator >> ( ApexPacket &arg );
#endif
	SP2Packet&  operator >> ( MonitorStatusRequest &arg );
	SP2Packet&  operator >> ( MonitorStatusResult &arg );
	SP2Packet&  operator >> ( MonitorChangeRequest &arg );
	SP2Packet&  operator >> ( MonitorChangeResult &arg );
	SP2Packet&	operator >> ( GAMESERVERINFO& arg );
	SP2Packet& operator  >> ( sockaddr_in& arg);//For UDP Packet

	SP2Packet& operator >> ( InsertData& arg);
	SP2Packet& operator << ( InsertData& arg);

	SP2Packet& operator >> ( SendRelayInsertData& arg);
	SP2Packet& operator << ( SendRelayInsertData& arg);

	SP2Packet& operator >> ( RemoveData& arg);
	SP2Packet& operator << ( RemoveData& arg);

	SP2Packet& operator >> ( SendRelayInfo_& arg);
	SP2Packet& operator << ( SendRelayInfo_& arg);

public:
	//Write

	bool Write(bool arg)						{ return CPacket::Write(arg); }
	bool Write(int arg)							{ return CPacket::Write(arg); }
	bool Write(LONG arg)						{ return CPacket::Write(arg); }
	bool Write(DWORD arg)						{ return CPacket::Write(arg); }
	bool Write(__int64 arg)						{ return CPacket::Write(arg); } 
	bool Write(LPTSTR arg)						{ return CPacket::Write(arg); } 
	bool Write(double arg)						{ return CPacket::Write(arg); } 
	bool Write(float arg)						{ return CPacket::Write(arg); }
	bool Write(BYTE arg);
	bool Write(short arg);
	bool Write(WORD arg);;
	bool Write( const ioHashString &arg );
	bool Write( const Vector3 &arg );
	bool Write( const ControlKeys &arg );
	bool Write( CQueryData &arg );
	bool Write( CHARACTER& arg );
	bool Write( CQueryResultData &arg );
	bool Write( const Quaternion &arg );

	//Read
	bool Read(bool& arg)						{ return CPacket::Read(arg); }
	bool Read(int& arg)							{ return CPacket::Read(arg); }
	bool Read(LONG& arg)						{ return CPacket::Read(arg); }
	bool Read(DWORD& arg)						{ return CPacket::Read(arg); }
	bool Read(__int64& arg)						{ return CPacket::Read(arg); }
	bool Read(const int nLength, LPTSTR arg)	{ return CPacket::Read(nLength,arg); }
	bool Read(double& arg)						{ return CPacket::Read(arg); }
	bool Read(float& arg)						{ return CPacket::Read(arg); }
	bool Read(BYTE& arg);
	bool Read(short& arg);
	bool Read(WORD& arg);;
	bool Read( ioHashString &arg );
	bool Read( ControlKeys& arg );
	bool Read( CHARACTER& arg );
	bool Read( CQueryResultData &arg );
	bool Read( Vector3 &arg );
	bool Read( Quaternion &arg );

	void GetStringOfStream(std::string & str);
	void GetStringOfStreamBody(std::string & str);
};
#endif