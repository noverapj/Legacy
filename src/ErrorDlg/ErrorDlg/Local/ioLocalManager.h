#pragma once

#include "../io3DEngine/ioLocalManagerParent.h"

class ioLocalParent;

class ioLocalManager : public ioLocalManagerParent, public Singleton< ioLocalManager >
{
protected:
	typedef std::vector< ioLocalParent* > vLocalVector;
	vLocalVector m_vLocalVector;

protected:
	ioLocalParent *CreateLocal( LocalType eLocalType );
	void AddLocal( ioLocalParent *pLocal );
	const char *GetLocalName( LocalType eLocalType );

public:
	void Init();
	ioLocalParent *GetLocal( LocalType eLocalType );
	bool ParseCmd( const char *szCmd );

public:
	static ioLocalManager &GetSingleton();


public:
	ioLocalManager(void);
	virtual ~ioLocalManager(void);
};


#define g_LocalMgr ioLocalManager::GetSingleton()

