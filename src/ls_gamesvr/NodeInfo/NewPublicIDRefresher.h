#ifndef __NewPublicIDRefresher_h__
#define __NewPublicIDRefresher_h__

#include "../Util/Singleton.h"

class ioFriend;

typedef struct tagNewPublicIDInfo
{
	DWORD           m_dwUserIndex;
	ioHashString    m_szPublicID;
	ioHashString    m_szNewPublicID;
	ioHashStringVec m_vFriendNameVector;

	tagNewPublicIDInfo()
	{
		m_dwUserIndex = 0;
	}

}NewPublicIDInfo;

typedef std::vector<NewPublicIDInfo> vNewPublicIDInfoVector;

class NewPublicIDRefresher  : public Singleton< NewPublicIDRefresher >
{
protected:
	enum { MAX_INFO = 100, };

protected:
	static NewPublicIDRefresher *sg_Instance;

protected:
	vNewPublicIDInfoVector m_vNewPublicIdInfoVector;

public:
	void AddInfo( DWORD dwUserIndex, const ioHashString &rszPublicID, const ioHashString &rszNewPublicID, ioFriend *pFriend );
	void DeleteInfo( DWORD dwUserIndex );
	void SendFriendsAndDelete( DWORD dwUserIndex );

public:
	static NewPublicIDRefresher& GetSingleton();

public:
	NewPublicIDRefresher(void);
	virtual ~NewPublicIDRefresher(void);
};

#define g_NewPublicIDRefresher NewPublicIDRefresher::GetSingleton()

#endif // __NewPublicIDRefresher_h__


