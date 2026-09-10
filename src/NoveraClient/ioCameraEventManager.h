#pragma once

#include "../io3DEngine/io3DCommon.h"
#include "../io3DEngine/ioPrerequisites.h"
#include "../io3DEngine/ioSingleton.h"
#include "ioCameraEvent.h"

class ioINILoader;
class ioPlayStage;
class ioCameraEventManager: public Singleton< ioCameraEventManager >
{
private:
	typedef std::map< DWORD, ioCameraEvent* > CECodeMap;
	CECodeMap m_CECodeMap;

	ioCameraEvent* m_pCameraEvent;
	
	ioPlayStage *m_pPlayStage;
public:
	void LoadCameraEventList( const char *szFileName );

public:
	ioCameraEvent* CreateCE( DWORD dwCode, ioPlayStage *pPlayStage );
	void NotifyCEDestroyed();

private:
	ioCameraEvent* CreateCETemplete( ioCameraEvent::CameraEvnetType eType, ioINILoader &rkLoader );
	ioCameraEvent* CreateCloneCE( ioCameraEvent *pSrc );

	bool AddNewCE( ioCameraEvent *pCameraEvent );

public:
	inline ioCameraEvent* GetCurCameraEvent() const { return m_pCameraEvent; }
	
public:
	static ioCameraEventManager& GetSingleton();

public:
	ioCameraEventManager();
	~ioCameraEventManager();
};

#define g_CameraEventMgr ioCameraEventManager::GetSingleton()

