#pragma once


class ioAniCollisionBoxGrp;
class ioWoundedCollisionBoxGrp;

class ioCollisionBoxGrpManager : public Singleton< ioCollisionBoxGrpManager >
{
protected:
	typedef std::map< ioHashString, ioAniCollisionBoxGrp* > AniCollisionBoxGrpMap;
	typedef std::map< ioHashString, ioWoundedCollisionBoxGrp* > WoundedCollisionBoxGrpMap;

	AniCollisionBoxGrpMap m_AniCollisionBoxGrp;
	WoundedCollisionBoxGrpMap m_WoundedCollisionBoxGrp;

	char m_szStartPath[MAX_PATH];

public:
	void SetStartDir( const char *szStartPath );

public:
	ioAniCollisionBoxGrp* CreateAniCollisionGrp( const char *szFileName, bool bSpecial = false );
	ioWoundedCollisionBoxGrp* CreateWoundedCollisionGrp( const char *szFileName );

public:
	static ioCollisionBoxGrpManager& GetSingleton();

public:
	ioCollisionBoxGrpManager();
	virtual ~ioCollisionBoxGrpManager();
};

#define g_CollisionBoxGrpMgr ioCollisionBoxGrpManager::GetSingleton()

