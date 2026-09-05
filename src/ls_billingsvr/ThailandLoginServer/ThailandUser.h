#ifndef __ThailandUser_h__
#define __ThailandUser_h__

#include "../Channeling\ioChannelingNodeParent.h"

class SP2Packet;
class ThailandUser : public SuperParent
{
protected:
	DWORD        m_dwCreateTime;
	ioHashString m_sPID;
	ioHashString m_sPrivateID;
	ioHashString m_sBillingGUID;
	ioHashString m_sThailandID;
	ioHashString m_sPW; 
	ioHashString m_sDomain;
	ioHashString m_sIP;
	DWORD        m_dwReturnMsgType;
	ioHashString m_sOTPID;
	ioHashString m_sOTPPW;
	int          m_iChannelingType;	
	DWORD        m_dwUserIndex;
	bool         m_bMouseBusy;
	int          m_iItemType;
	int          m_iItemValueList[MAX_ITEM_VALUE];
	int          m_iPayAmt;
	
	SOCKET		 m_socket;
	bool         m_bAlive;

	ioHashString m_sServerIP;
	int          m_iServerPort;
	
public:
	void SetData( DWORD dwCreateTime, const ioHashString &rsBillingGUID, const ioHashString &rsPrivateID, const ioHashString &rsEncodePW, const ioHashString &rsIP, DWORD dwReturnMsgType, const ioHashString &rsServerIP, const int iServerPort );
	void SetExtendData( int iChannelingType, DWORD dwUserIndex, bool bMouseBusy, int iItemType, int iItemValueList[MAX_ITEM_VALUE], int iPayAmt );
	void Clear();

	bool IsAlive() const { return m_bAlive; }
	void SetAlive( bool bAlive ) { m_bAlive = bAlive; }

public:
	DWORD GetCreateTime() const { return m_dwCreateTime; }
	const ioHashString &GetPID() const { return m_sPID; }
	const ioHashString &GetPrivateID() const { return m_sPrivateID; }
	const ioHashString &GetThailandID() const { return m_sThailandID; }
	const ioHashString &GetPW() const { return m_sPW; }
	const ioHashString &GetDomain() const { return m_sDomain; }
	const ioHashString &GetIP() const { return m_sIP; }
	const ioHashString &GetBillingGUID() const { return m_sBillingGUID; }
	const ioHashString &GetServerIP() const { return m_sServerIP; }
	const ioHashString &GetOTPID() const { return m_sOTPID; }
	const ioHashString &GetOTPPW() const { return m_sOTPPW; }
	int   GetServerPort() const { return m_iServerPort; }
	DWORD GetReturnMsgType() const { return m_dwReturnMsgType; }
	int   GetChannelingType() const { return m_iChannelingType; }
	DWORD GetUserIndex() const { return m_dwUserIndex; }
	bool  IsMouseBusy() const { return m_bMouseBusy; }
	int   GetItemType() const { return m_iItemType; }
	int   GetItemValue ( int iArray ) const;
	int   GetPayAmt() const { return m_iPayAmt; }

	void   SetPID( const ioHashString &rsPID) { m_sPID = rsPID; }

	SOCKET GetSocket() const { return m_socket; }
	void   SetSocket(SOCKET Socket) { m_socket = Socket; }

protected:
	void SetDomainAndID( const ioHashString &rsPrivateID );
	void SetPW( const ioHashString &rsEncodePW );
	void GetDecodeStr( IN const ioHashString &rsEncode, IN const ioHashString &rsPrivateID, OUT char *szDecode, IN int iDecodeSize );

public:
	ThailandUser(void);
	virtual ~ThailandUser(void);
};

typedef std::vector< ThailandUser* > vThailandUser;

#endif // __ThailandUser_h__