
#ifndef _DropManage_h_
#define _DropManage_h_

#include "../io3DEngine/io3DCommon.h"
#include "../io3DEngine/ioPrerequisites.h"
#include "../io3DEngine/ioSingleton.h"
#include "../PlayEntityHelpStruct.h"

class cLua;
class ioBaseChar;

class DropManager : public Singleton< DropManager >
{
private:
	cLua	*m_LuaState;

public:
	bool Init( cLua *pContext );
	void DropItemCheck( ioBaseChar *pOwner, bool bWeapon, bool bArmor, bool bHelmet, bool bCloak,
						bool bDropZone );

	int DropItemCheck2( bool bWeapon, bool bArmor, bool bHelmet, bool bCloak );

	cLua *GetLua() { return m_LuaState; }

public:
	DropManager();
	virtual ~DropManager();
};

#define g_DropManager DropManager::GetSingleton()

#endif
