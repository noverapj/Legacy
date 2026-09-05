#pragma once

#include "ioLocalParent.h"
#define EU_TOKEN "|"

class ioLocalLatin : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetTextListFileName();

	virtual bool ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize );
	virtual bool DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize );
	virtual bool IsRightTimeLoginKey( DWORD dwTotalMins );
	virtual bool IsRightLoginKey( const char *szDBKey, const char *szDecryptKey );

	virtual void ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket );
	virtual bool ApplyLogin( IN User *pUser, IN SP2Packet &rkPacket );
	virtual void ApplyRequestLogin( IN SP2Packet &rkPacket, OUT ioHashString &rsValue1, OUT ioHashString &rsValue2 );
	virtual bool SendLoginData( User *pUser );
	virtual bool SendLogoutData( User *pUser );

	virtual void FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket );
	virtual void FillRequestOutputCash( IN User *pUser, IN SP2Packet &rkPacket, const char *szRecvPrivateID );
	virtual void FillRequestLogin( OUT SP2Packet &rkPacket, IN ioHashString &rsValue1, IN ioHashString &rsValue2 );
	virtual void SendRefundCash( User *pUser, int iTransactionID, bool bRefund );
	virtual void SendUserInfo( User *pUser );
	virtual void SendCancelCash( IN SP2Packet &rkPacket );

	virtual int  GetFirstIDMaxSize();

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

	virtual bool IsDecryptID() { return true; }

	virtual bool GetCCUCountSend();
public:
	ioLocalLatin(void);
	virtual ~ioLocalLatin(void);
};

