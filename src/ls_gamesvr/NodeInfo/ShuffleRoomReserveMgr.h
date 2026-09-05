
#ifndef _ShuffleRoomReserveMgr_h_
#define _ShuffleRoomReserveMgr_h_

#include "ShuffleUserGroup.h"

class UserParent;
class User;

class ShuffleRoomNode;
class ShuffleRoomReserveMgr
{
public:
	enum
	{
		MAX_PARTY_CREATE_USER = 2,
	};

protected:
	static ShuffleRoomReserveMgr *sg_Instance;

public:
	static ShuffleRoomReserveMgr &GetInstance();
	static void ReleaseInstance();

public:
	void Initialize();

public:
	void AddShuffleQueue( User *pUser, int iGlobalSearchingTryCount );
	void DeleteShuffleQueue( User *pUser );

public:
	void GlobalSearchingShuffleUser( User *pUser, int iGlobalSearchingTryCount );

protected:
	ShuffleRoomNode* CreateShuffleRoom( const UserGroup& kGroup );

private:     	/* Singleton Class */
	ShuffleRoomReserveMgr();
	virtual ~ShuffleRoomReserveMgr();

};
#define g_ShuffleRoomReserveMgr    ShuffleRoomReserveMgr::GetInstance()

#endif