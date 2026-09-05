

#ifndef _ioOpcodeManager_h_
#define _ioOpcodeManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioOpcodeShape;
class ioResourceManager;

class __EX ioOpcodeManager : public ioResourceManager
{
public:
	virtual ioOpcodeShape* CreateShape( const ioHashString &name ) = 0;
	virtual ioOpcodeShape* GetShapeByName( const ioHashString &name ) = 0;

public:
	ioOpcodeManager(){};
	virtual ~ioOpcodeManager(){};
};

__EX ioOpcodeManager* CreateOpcodeManager();
__EX void DestroyOpcodeManager();

__EX ioOpcodeManager& GetOpcodeManager();

#define g_OpcodeMgr GetOpcodeManager()

#endif

