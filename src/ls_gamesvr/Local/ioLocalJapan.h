#ifndef __ioLocalJapan_h__
#define __ioLocalJapan_h__

#include "ioLocalParent.h"

#define JAPAN_TOKEN          " "
#define JAPAN_CRYPT_KEY      "tkxkd67ahwjd$@"

class ioLocalJapan : public ioLocalParent
{
protected:
	bool m_bRightTimeLoginKey;
	bool m_bDecryptLoginKey;

protected:
	void GetHexMD5( OUT char *szHexMD5, IN int iHexSize, IN const char *szSource );

public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetTextListFileName();

	virtual bool ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize );
	virtual bool DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize );
	virtual bool IsRightTimeLoginKey( DWORD dwTotalMins );
	virtual bool IsRightLoginKey( const char *szDBKey, const char *szDecryptKey );

	virtual void ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket );
	virtual bool SendLoginData( User *pUser );

	virtual void FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket );

	virtual void SendRefundCash( User *pUser, int iTransactionID, bool bRefund );
	virtual bool UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType );
	virtual void SendCancelCash( IN SP2Packet &rkPacket );

	virtual void SendUserInfo( User *pUser );

	virtual int GetFirstIDMaxSize();

	virtual const char *GetGuildMasterPostion();
	virtual const char *GetGuildSecondMasterPosition();
	virtual const char *GetGuildGeneralPosition();
	virtual void GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName );

	virtual bool IsCheckKorean();
	virtual bool IsChannelingID();

	virtual bool IsBillingTestID( const ioHashString &rsPublicID );

	virtual bool IsSamePCRoomUser();
	virtual bool IsBadPingKick( bool bLadder );

	virtual int GetLimitGradeLevel();

	virtual int GetLicenseDate();
	virtual bool IsFirstSoldierSelect(){ return true; }
	virtual bool IsMileage() { return true; }

public:
	ioLocalJapan(void);
	virtual ~ioLocalJapan(void);
};

#endif // __ioLocalJapan_h__