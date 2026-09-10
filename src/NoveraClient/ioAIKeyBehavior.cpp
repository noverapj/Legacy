#include "stdafx.h"

#include "ioAIKeyBehavior.h"
#include "ioAISymbolTable.h"
#include "ioNpcChar.h"
#include "ioChampionAI.h"

ioAIKeyBehavior::ioAIKeyBehavior( ioXMLElement* node )
{
	if( node != (ioXMLElement*)0 && !node->IsEmpty() )
	{
		std::string state = node->GetStringAttribute_e( "action" );
		m_inputType	= g_AISymbolTable.GetKeyInputType( node->GetStringAttribute_e("type") );
		m_stateType	= g_AISymbolTable.GetKeyStateType( state );
		m_duration	= node->GetIntAttribute_e( "duration" );
	}
}

ioAIKeyBehavior::~ioAIKeyBehavior()
{
}

bool	ioAIKeyBehavior::DoKeyAction( ioBaseChar* pCurTarget, ioNpcChar* pMyself, ioChampionAI* pCAI )
{
	WORD	wDefenseKey, wJumpKey, wAttackKey;

	if( m_inputType >= -1 && m_inputType <= 7 )
	{
		pMyself->SetDirectionKey( m_inputType, m_stateType );
	}
	else
	{
		// general key
		wDefenseKey = wJumpKey = wAttackKey = KEY_NONE;
		switch( m_inputType )
		{
		case AI_BvT_KIT_JUMP_A:		wJumpKey	= m_stateType;	break;
		case AI_BvT_KIT_DEFENSE_S:	wDefenseKey	= m_stateType;	break;
		case AI_BvT_KIT_ATTACK_D:	wAttackKey	= m_stateType;	break;
		case AI_BvT_KIT_CLOAK_AS:	wJumpKey = wDefenseKey = m_stateType;	break;
		case AI_BvT_KIT_HELMET_AD:	wJumpKey = wAttackKey = m_stateType;	break;
		case AI_BvT_KIT_ARMOR_SD:	wDefenseKey = wAttackKey = m_stateType;	break;
		case AI_BvT_KIT_WEAPON_ASD:	wJumpKey = wDefenseKey = wAttackKey = m_stateType;	break;
		}
		pMyself->SetActionKey( wDefenseKey, wJumpKey, wAttackKey );

		//LOG.PrintTimeAndLog( 0, "KEY : Def=%d Jump=%d Attack=%d Duration=%u Time=%u", 
		//							wDefenseKey, wJumpKey, wAttackKey, m_duration, FRAMEGETTIME() );
	}

	return true;
}
