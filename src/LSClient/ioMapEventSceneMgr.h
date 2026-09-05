#pragma once

class ioMapEventSceneMgr : public Singleton< ioMapEventSceneMgr >
{
protected:
	struct EventSceneData
	{
		Vector3Vec m_vLookAt;
		Vector3Vec m_vOffset;
		DWORD      m_dwEventDuration;
	};
	typedef std::map< DWORD, EventSceneData > EventSceneMap;
	EventSceneMap m_EventSceneMap;

protected:
	void ClearSceneData();

public:
	void LoadMapEventScene();

public:
	bool  GetMapEventScene( DWORD dwIndex, Vector3Vec &rLookAt, Vector3Vec &rOffset, DWORD &rEventDuration );
	DWORD GetMapEventSceneTime( DWORD dwIndex );
public:
	static ioMapEventSceneMgr& GetSingleton();

public:
	ioMapEventSceneMgr();
	virtual ~ioMapEventSceneMgr();
};

#define g_MapEventSceneMgr ioMapEventSceneMgr::GetSingleton()

