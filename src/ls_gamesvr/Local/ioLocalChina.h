#ifndef __ioLocalChina_h__
#define __ioLocalChina_h__

#include "ioLocalParent.h"

class ioLocalChina : public ioLocalParent
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

	virtual void SendRefundCash( User *pUser, int iTransactionID, bool bRefund );

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

	virtual bool IsDecryptID() { return false; }

public:
	ioLocalChina(void);
	virtual ~ioLocalChina(void);
};

#endif // __ioLocalChina_h__