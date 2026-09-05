#ifndef __ioLocalKorea_h__
#define __ioLocalKorea_h__

#include "ioLocalParent.h"

class ioLocalKorea : public ioLocalParent
{
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
	virtual void SendCancelCash( IN SP2Packet &rkPacket );
	virtual bool UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType );

	virtual void SendRefundCash( User *pUser, int iTransactionID, bool bRefund );

	virtual void SendUserInfo( User *pUser );

	virtual int GetFirstIDMaxSize();

	virtual const char *GetGuildMasterPostion();
	virtual const char *GetGuildSecondMasterPosition();
	virtual const char *GetGuildBuilderPosition();
	virtual const char *GetGuildGeneralPosition();
	virtual void GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName );

	virtual bool IsCheckKorean();
	virtual bool IsChannelingID();

	virtual bool IsBillingTestID( const ioHashString &rsPublicID );

	virtual bool IsSamePCRoomUser();
	virtual bool IsBadPingKick( bool bLadder );
	virtual bool IsPCRoomCheck() { return false; } //kyg 130723_피시방혜텍 보내지 않음 

	virtual int GetLimitGradeLevel();
	virtual int GetLicenseDate();

	virtual bool IsRunUserShutDown() { return false; }
	virtual bool IsFriendRecommend() { return true; }

public:
	ioLocalKorea(void);
	virtual ~ioLocalKorea(void);
};

#endif // __ioLocalKorea_h__