#ifndef _ioINIManager_h_
#define _ioINIManager_h_

#include "../Util/Singleton.h"
class ioModeINIManager : public Singleton< ioModeINIManager >
{
protected:
	typedef std::map< ioHashString, ioINILoader* > INILoaderMap;
	INILoaderMap m_INILoaderMap;

protected:
	void InsertINI( const ioHashString &rkFileName );

public:
	void LoadINIData( const ioHashString &rkFileName );
	void ReloadINIData();

public:
	ioINILoader &GetINI( const ioHashString &rkFileName );

public:
	static ioModeINIManager& GetSingleton();

public:   
	ioModeINIManager();
	virtual ~ioModeINIManager();
};
#define g_ModeINIMgr ioModeINIManager::GetSingleton()
#endif