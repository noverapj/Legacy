#pragma once

class ioWeapon;
class ioBaseChar;
class ioPlayEntity;
class SP2Packet;
class ioINILoader;
class ioPlayStage;
class ioPlayMode;
class ioBall;

class ioBallManager : public Singleton< ioBallManager >
{
protected:
	typedef std::vector< ioBall* > vBallList;
	vBallList m_vBallStructList;

	ioPlayStage *m_pPlayStage;

	int m_iLocalIndex;

public:
	ioBall* CreateBallStruct( ioHashString szStructName, ioPlayMode *pMode );

	void GetBallStructInfo( SP2Packet &rkPacket );
	void ApplyBallStructInfo( SP2Packet &rkPacket );

	void OnBallStructReposition( SP2Packet &rkPacket );

	void DestroyBallStructList();

public:
	ioBall* FindBallStruct( int iArray );	// 고유값으로 찾을때
	ioBall* GetBallStruct( int iIndex );	// list 순서대로 가져올때

public:
	inline int GetBallStructCnt() const { return m_vBallStructList.size(); }
	inline int GetCurLocalIndex() const { return m_iLocalIndex; }

public:
	static ioBallManager& GetSingleton();

public:
	ioBallManager( ioPlayStage *pStage );
	virtual ~ioBallManager();
};

#define g_BallStructMgr ioBallManager::GetSingleton()

