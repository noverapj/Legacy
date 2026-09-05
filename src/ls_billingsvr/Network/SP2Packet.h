#ifndef _SP2Packet_h_
#define _SP2Packet_h_

#include "MonitoringServerNode/MonitoringNode.h"

class CPacket;
class CQueryData;
class CQueryResultData;
struct CHARACTER;
struct NexonFirstInitialize;
struct NexonReInitialize;
struct NexonLogin;
struct OnNexonLogin;
struct OnNexonInitialize;
struct OnNexonTerminate;
struct OnNexonSynchronize;
struct OnNexonMessage;
struct NexonPolicyResult;
struct NexonAlive;
struct NexonSynchronize;
struct NexonSendSyncUser;
struct NexonLogOut;
struct OtherLogin;
struct NexonEUCheckCash;
struct NexonEUCheckCashResponse;
struct NexonEUPurchaseItem;
struct NexonEUPurchaseItemResponse;
struct NexonEUInitialize;
struct NexonEUInitializeResponse;
struct NexonEUSessionInitialize;
struct NexonEUSessionResponse;
struct NexonEUSessionCheck;
struct NexonEUSession4Request;
struct NexonEUSession4Reply;
struct NexonEUGetPurchaseAmount;
struct NexonEUGetPurchaseAmountResponse;
struct NexonEUCheckCashAmountResponse;
struct NexonProductInquiry;
struct NexonProductInquiryResponse;


class SP2Packet : public CPacket
{
	public:
	SP2Packet();
	SP2Packet( const SP2Packet &rhs );
	SP2Packet(DWORD ID);
	SP2Packet(char *buffer,int size);
	SP2Packet( DWORD dwUserIndex, SP2Packet &rhs );
	virtual ~SP2Packet();

	const char* GetData() const;
	int GetDataSize() const;
	const char* GetBuffer() const;	
	int   GetBufferSize() const;
	void SetDataAdd( char *buffer, int size, bool bCurPosReSet = false );
	void SetPosBegin();
	void SetBufferSizeMinusOne();
	void SetHashString(ioHashString& arg, unsigned char length );
	
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
	SP2Packet&  operator << ( const ioHashString &arg );
	SP2Packet&  operator << ( const Vector3 &arg );
	SP2Packet&  operator << ( const Quaternion &arg );
	SP2Packet&  operator << ( const MgameCashRequest &arg );
	SP2Packet&  operator << ( const MgameCashResult &arg );
	SP2Packet&  operator << ( const BOQPTS_GS_CONNECT &arg );
	SP2Packet&  operator << ( const BOQPTS_HEALTH_CHECK &arg );
	SP2Packet&  operator << ( const BOQPTS_CHECKPREMIUM2 &arg );
	SP2Packet&  operator << (CQueryData &arg);
	SP2Packet&  operator << (CQueryResultData &arg);
	SP2Packet&  operator << ( const MonitorStatusRequest &arg );
	SP2Packet&  operator << ( const MonitorStatusResult &arg );
	SP2Packet&  operator << ( const MonitorChangeRequest &arg );
	SP2Packet&  operator << ( const MonitorChangeResult &arg );
	SP2Packet&  operator << ( const PHEADER &arg );
	SP2Packet&  operator << ( const SC_ALIVE &arg );
	SP2Packet&  operator << ( const CS_BALANCE &arg );
	SP2Packet&  operator << ( const SC_BALANCE &arg );
	SP2Packet&  operator << ( const CS_PURCHASEITEM &arg );
	SP2Packet&  operator << ( const SC_PURCHASEITEM &arg );
	SP2Packet&  operator << ( const CS_GIFTITEM &arg );
	SP2Packet&  operator << ( const SC_GIFTITEM &arg );
	SP2Packet&  operator << ( const CS_CNLPURCHASE &arg );
	SP2Packet&  operator << ( const SC_CNLPURCHASE &arg );
	SP2Packet&  operator << ( const BILL_PACK_HEALTH_CHECK &arg );
	SP2Packet&  operator << ( const BILL_PACK_GETBALANCE &arg );
	SP2Packet&  operator << ( const BILL_PACK_PURCHASEITEM &arg );
	SP2Packet&  operator << ( const BILL_PACK_CNLPURCHASE &arg );
	SP2Packet&  operator << ( const CS_RETRACT_PAYBACK& arg );
	SP2Packet&  operator << ( const CS_RETRACT& arg );
	SP2Packet&  operator << ( const CS_RETRACT_CANCEL& arg );
	SP2Packet&  operator << ( char arg );
	SP2Packet&  operator << ( const unsigned int arg);
	SP2Packet&  operator << ( const uint64_t arg);
	//SP2Packet&  operator << ( unsigned short arg);
	
	//SP2Packet&  operator >> ( unsigned short& arg);
	SP2Packet&  operator >> ( uint64_t& arg);
	SP2Packet&  operator >> (unsigned int& arg);
	SP2Packet&  operator >> (char &arg);
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
	SP2Packet&  operator >> ( ioHashString &arg );
	SP2Packet&  operator >> ( Vector3 &arg );
	SP2Packet&  operator >> ( Quaternion &arg );
	SP2Packet&  operator >> ( MgameCashRequest &arg );
	SP2Packet&  operator >> ( MgameCashResult &arg );
	SP2Packet&  operator >> ( BOQPTS_GS_CONNECT &arg );
	SP2Packet&  operator >> ( BOQPTS_HEALTH_CHECK &arg );
	SP2Packet&  operator >> ( BOQPTS_CHECKPREMIUM2 &arg );
	SP2Packet&  operator >> (CQueryData &arg);
	SP2Packet&  operator >> (CQueryResultData &arg);
	SP2Packet&  operator >> ( MonitorStatusRequest &arg );
	SP2Packet&  operator >> ( MonitorStatusResult &arg );
	SP2Packet&  operator >> ( MonitorChangeRequest &arg );
	SP2Packet&  operator >> ( MonitorChangeResult &arg );
	SP2Packet&  operator >> ( PHEADER &arg );
	SP2Packet&  operator >> ( SC_ALIVE &arg );
	SP2Packet&  operator >> ( CS_BALANCE &arg );
	SP2Packet&  operator >> ( SC_BALANCE &arg );
	SP2Packet&  operator >> ( CS_PURCHASEITEM &arg );
	SP2Packet&  operator >> ( SC_PURCHASEITEM &arg );
	SP2Packet&  operator >> ( CS_GIFTITEM &arg );
	SP2Packet&  operator >> ( SC_GIFTITEM &arg );
	SP2Packet&  operator >> ( CS_CNLPURCHASE &arg );
	SP2Packet&  operator >> ( SC_CNLPURCHASE &arg );
	SP2Packet&  operator >> ( BILL_PACK_HEALTH_CHECK &arg );
	SP2Packet&  operator >> ( BILL_PACK_GETBALANCE &arg );
	SP2Packet&  operator >> ( BILL_PACK_PURCHASEITEM &arg );
	SP2Packet&  operator >> ( BILL_PACK_CNLPURCHASE &arg );
	SP2Packet&  operator >> ( CS_RETRACT_PAYBACK& arg );
	SP2Packet&  operator >> ( SC_RETRACT_PAYBACK& arg );
	SP2Packet&  operator >> ( CS_RETRACT& arg );
	SP2Packet&  operator >> ( SC_RETRACT& arg );
	SP2Packet&  operator >> ( CS_RETRACT_CANCEL& arg );
	SP2Packet&  operator >> ( SC_RETRACT_CANCEL& arg );
	
	template <typename T>
	bool Write(T& data)
	{
		if( !CheckLeftPacketSize( sizeof(data) ) ) return false;
		memcpy(&m_pBuffer[m_currentPos],&data,sizeof(data));
		m_currentPos += sizeof(data);
		*m_packet_header.m_Size = m_currentPos;
		return true;
	}

	bool Write(NexonFirstInitialize& data);
	bool Write(NexonReInitialize& data);
	bool Write(NexonLogin& data);
	bool Write(NexonAlive& data);
	bool Write(ioHashString& arg);//넥슨일때만 좀 특이한경우
	bool Write(NexonSynchronize& arg);
	bool Write(NexonLogOut& arg);
	bool Write(OtherLogin& arg);
	bool Write(NexonEUInitialize& arg);
	bool Write(NexonEUCheckCash& arg);
	bool Write(NexonEUPurchaseItem& arg);
	bool Write(NexonEUSessionInitialize& arg);
	bool Write(NexonEUSessionCheck& arg);
	bool Write(NexonEUSession4Request& arg);
	bool Write(NexonEUGetPurchaseAmount& arg);
	bool Write(NexonProductInquiry& arg);
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

	template <typename T>
	bool Read(T& data)
	{ 
		if( !CheckRightPacketSize( sizeof(data) ) ) return false;
		memcpy(&data,&m_pBuffer[m_currentPos],sizeof(data));
		m_currentPos += sizeof(data);
		return true;
	}
	bool Read(OnNexonInitialize& data);
	bool Read(OnNexonLogin& data);
	bool Read(OnNexonTerminate& data);
	bool Read(OnNexonMessage& data);
	bool Read(OnNexonSynchronize& data);
	bool Read(NexonEUCheckCashResponse& data);
	bool Read(NexonEUCheckCashAmountResponse& data);
	bool Read(NexonEUInitializeResponse& data);
	bool Read(NexonEUPurchaseItemResponse& data);
	bool Read(NexonEUSessionResponse& arg);
	bool Read(NexonEUSessionCheck& arg);
	bool Read(NexonEUSession4Reply& arg);
	bool Read(NexonEUGetPurchaseAmountResponse& arg);
	bool Read(NexonProductInquiryResponse& arg);
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
};
#endif