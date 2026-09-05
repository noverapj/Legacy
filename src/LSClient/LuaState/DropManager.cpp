

#include "stdafx.h"

#include "../io3DEngine/ioStream.h"
#include "../io3DEngine/ioResourceLoader.h"
#include "../GameEnumType.h"
#include "../ioBaseChar.h"
#include "DropManager.h"

template<> DropManager* Singleton< DropManager >::ms_Singleton = 0;

DropManager::DropManager()
{
	m_LuaState = NULL;
}

DropManager::~DropManager()
{
}

bool DropManager::Init( cLua *pContext )
{
	m_LuaState = pContext;
	
	std::string szPath = "ItemDrop.lua";

	if( g_ResourceLoader.IsLoadInPack() )
	{
		szPath.insert(0, "Scripts\\");

		ioBinaryStream kStream;
		if( !g_ResourceLoader.LoadStream( szPath.c_str(), &kStream ) )
		{
			LOG.PrintTimeAndLog( 0, "DropManager::Init() - RunScript is fail(%s)!", szPath.c_str());
			return false;
		}

		if( !m_LuaState->RunMemory( (char*)kStream.GetPtr(), kStream.GetSize()) )
		{
			LOG.PrintTimeAndLog( 0, "DropManager::Init() - RunMemory is fail" );
			return false;
		}
	}
	else
	{
		if( !m_LuaState->RunScript( szPath.c_str() ) )
		{
			LOG.PrintTimeAndLog( 0, "DropManager::Init() - RunScript is fail" );
			return false;
		}
	}

	return true;
}

void DropManager::DropItemCheck( ioBaseChar *pOwner,
								 bool bWeapon,
								 bool bArmor,
								 bool bHelmet,
								 bool bCloak,
								 bool bDropZone )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "ItemDropCheck" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushboolean( m_LuaState->GetScriptContext(), bWeapon );
	lua_pushboolean( m_LuaState->GetScriptContext(), bArmor );
	lua_pushboolean( m_LuaState->GetScriptContext(), bHelmet );
	lua_pushboolean( m_LuaState->GetScriptContext(), bCloak );
	lua_pushboolean( m_LuaState->GetScriptContext(), bDropZone );

	D3DXVECTOR3 vDropPos = pOwner->GetMidPositionByRate();

	lua_pushnumber( m_LuaState->GetScriptContext(), vDropPos.x );
	lua_pushnumber( m_LuaState->GetScriptContext(), vDropPos.z );

	lua_call( m_LuaState->GetScriptContext(), 7, 3 );

	vDropPos.z = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );
	vDropPos.x = (float)lua_tonumber( m_LuaState->GetScriptContext(), -2 );

	int iSlot = (int)lua_tonumber( m_LuaState->GetScriptContext(), -3 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	if( iSlot != -1 )
	{
		pOwner->ItemDrop( (EquipSlot)iSlot, vDropPos.x, vDropPos.z );
	}
}

int DropManager::DropItemCheck2( bool bWeapon, bool bArmor, bool bHelmet, bool bCloak )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "ItemDropCheck2" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );

	lua_pushboolean( m_LuaState->GetScriptContext(), bWeapon );
	lua_pushboolean( m_LuaState->GetScriptContext(), bArmor );
	lua_pushboolean( m_LuaState->GetScriptContext(), bHelmet );
	lua_pushboolean( m_LuaState->GetScriptContext(), bCloak );

	lua_call( m_LuaState->GetScriptContext(), 4, 1 );

	int iSlot = (int)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return iSlot;
}

////////////////////////////////////////////////////////////

