#ifndef __ioChannelingNodeManager_h__
#define __ioChannelingNodeManager_h__

#include "../StringManager/ioSingleton.h"

class ioChannelingNodeParent;

class ioChannelingNodeManager  : public Singleton< ioChannelingNodeManager >
{
protected:
	ChannelingType m_eChannelingType;

public:
	ChannelingType GetChannelingType() { return m_eChannelingType; }
	void           SetChannelingType( ChannelingType eChannelingType ) { m_eChannelingType = eChannelingType; }

protected:
	typedef std::vector< ioChannelingNodeParent* > vChannelingNodeVector;
	vChannelingNodeVector m_vChannelingNodeVector;

protected:
	ioChannelingNodeParent *CreateNode( ChannelingType eChannelingType );
	void AddNode( ioChannelingNodeParent *pNode );

public:
	void Init();
	ioChannelingNodeParent *GetNode( ChannelingType eChannelingType );

public:
	static ioChannelingNodeManager &GetSingleton();

public:
	ioChannelingNodeManager(void);
	virtual ~ioChannelingNodeManager(void);
};

#define g_ChannelingMgr ioChannelingNodeManager::GetSingleton()

#endif // __ioChannelingNodeManager_h__