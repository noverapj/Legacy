
#include "stdafx.h"

#include "ioFieldItemMgr.h"

template<> ioFieldItemMgr* Singleton< ioFieldItemMgr >::ms_Singleton = 0;

ioFieldItemMgr::ioFieldItemMgr()
{
}

ioFieldItemMgr::~ioFieldItemMgr()
{
}

ioFieldItemMgr& ioFieldItemMgr::GetSingleton()
{
	return Singleton< ioFieldItemMgr >::GetSingleton();
}

void ioFieldItemMgr::LoadINIData()
{
	char szBuf[MAX_PATH] = "";
	
	ioINILoader_e kLoader( "config/sp2_field_item.ini" );
	kLoader.SetTitle_e( "common" );

	kLoader.LoadString_e( "default_item_effect", "", szBuf, MAX_PATH );
	m_DefaultItemEffect = szBuf;
	kLoader.LoadString_e( "default_item_effect_red", "", szBuf, MAX_PATH );
	m_DefaultItemEffectRed = szBuf;
	kLoader.LoadString_e( "default_item_effect_blue", "", szBuf, MAX_PATH );
	m_DefaultItemEffectBlue = szBuf;
	kLoader.LoadString_e( "default_item_over_effect", "", szBuf, MAX_PATH );
	m_DefaultItemOverEffect = szBuf;
	kLoader.LoadString_e( "default_item_over_effect_red", "", szBuf, MAX_PATH );
	m_DefaultItemOverEffectRed = szBuf;
	kLoader.LoadString_e( "default_item_over_effect_blue", "", szBuf, MAX_PATH );
	m_DefaultItemOverEffectBlue = szBuf;

	m_fMoveDropJumpAmt = kLoader.LoadFloat_e( "move_drop_jump_amt", 0.0f );
	m_fMoveDropGravityAmt = kLoader.LoadFloat_e( "move_drop_gravity_amt", 0.0f );
}

const ioHashString& ioFieldItemMgr::GetItemEffect( TeamType eTeam ) const
{
	if( eTeam == TEAM_RED && !m_DefaultItemEffectRed.IsEmpty() )
		return m_DefaultItemEffectRed;
	else if( eTeam == TEAM_BLUE && !m_DefaultItemEffectBlue.IsEmpty() )
		return m_DefaultItemEffectBlue;

	return m_DefaultItemEffect; 
}

const ioHashString& ioFieldItemMgr::GetItemOverEffect( TeamType eTeam ) const
{ 
	if( eTeam == TEAM_RED && !m_DefaultItemOverEffectRed.IsEmpty() )
		return m_DefaultItemOverEffectRed;
	else if( eTeam == TEAM_BLUE && !m_DefaultItemOverEffectBlue.IsEmpty() )
		return m_DefaultItemOverEffectBlue;

	return m_DefaultItemOverEffect; 
}