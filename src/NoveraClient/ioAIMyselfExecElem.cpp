#include "stdafx.h"

#include <regex>

#include "ioAIBaseExecElem.h"
#include "ioAIMyselfExecElem.h"
#include "ioNpcChar.h"
#include "ioChampionAI.h"
#include "ioAISymbolTable.h"
#include "ioAIDatabase.h"
#include "stringext.h"
#include "ioPlayStage.h"
#include "ioAIDatabase.h"

/////////////////////////////////////////////////////////////////////////////////
ioAIExec_Myself_IsHP::ioAIExec_Myself_IsHP( AIExecOperatorType op, const std::string& args )
	: ioAIIsHP( AI_EXEC_MYSELF_IsHP, op, args )
{
}

void ioAIExec_Myself_IsHP::SetArgs( const std::string& args )
{
}

bool ioAIExec_Myself_IsHP::Execute(	ioBaseChar*& pTarget, ioChampionAI*	pCAI )
{
	if( pCAI )
	{
		if( pCAI->GetMyself() )	return CompareHP( pCAI->GetMyself() );
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_IsCurrentState : pCAI[%x] is null",
								pCAI );
	return false;
}

std::string ioAIExec_Myself_IsHP::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "AI_EXEC_TARGET_IsHP {m_fRate=%f,m_Compare=%d}", m_fRate, m_Compare );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_IsCurrentState::ioAIExec_Myself_IsCurrentState(AIExecOperatorType op, const std::string& args)
	: ioAIIsCurrentState( AI_EXEC_MYSELF_IsCurrentState, op, args )
{
}

void ioAIExec_Myself_IsCurrentState::SetArgs( const std::string& args )
{
}

bool ioAIExec_Myself_IsCurrentState::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	if( pCAI && pCAI->GetMyself() )
		if( pCAI->GetMyself() )	return CompareState( pCAI->GetMyself(), pCAI );
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_IsCurrentState : pCAI[%x] or Myself[%x] is null",
								pCAI, pCAI->GetMyself() );

	return false;
}

std::string ioAIExec_Myself_IsCurrentState::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_IsCurrentState {%d", m_vCharState.size()  );
	retStr = buff;

	for( std::vector<CharState>::iterator it = m_vCharState.begin(); it != m_vCharState.end(); it++ )
	{
		sprintf( buff, "%d ", *it );
		retStr += buff;
	}

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_StopAllActions::ioAIExec_Myself_StopAllActions(AIExecOperatorType op)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_StopAllActions, op )
{
}

bool ioAIExec_Myself_StopAllActions::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	if( pCAI )
	{
		pCAI->StopAllActions();
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_StopAllActions : pCAI[%x] is null",
								pCAI );
	m_bLastResult = true;

	return m_bLastResult;
}

std::string ioAIExec_Myself_StopAllActions::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_StopAllActions " );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_ShowChat::ioAIExec_Myself_ShowChat(AIExecOperatorType op, const std::string& args )
	: ioAIBaseExecElem( AI_EXEC_MYSELF_StopAllActions, op )
{
	SetArgs( args );
}
void	ioAIExec_Myself_ShowChat::SetArgs( const std::string& args )
{
	// 한글이 포함 될 수 있어서 find와 rfind의 조합으로 arguement 추출
	std::string	argstr = args;

	trim( argstr );
	int		firstindex = argstr.find( "'" );
	int		lastindex = argstr.rfind( "'" ) - firstindex;
	m_szChatMsg = argstr.substr( firstindex + 1, lastindex - 1 );

	// duration은 있을 수도 있고 없을 수도 있다
	std::regex	reg1( "[\\s]*,[\\s]*" + g_AISymbolTable.GetSymbolString("int") );
	std::smatch sm;
	std::string	a2 = argstr.substr( lastindex + 1 );

	if( std::regex_match( a2, sm, reg1 ) )
	{
		m_iDuration = atoi( std::string( sm[1] ).c_str() );
	}
	else
	{
		m_iDuration = 2000;
	}
}

bool ioAIExec_Myself_ShowChat::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	if( pCAI && pCAI->GetMyself() )
	{
		if( pCAI->GetMyself()->GetCreator()		&&
			pCAI->GetMyself()->GetCreator()->GetPlayMode() )
		{
			switch( pCAI->GetMyself()->GetCreator()->GetPlayMode()->GetModeState() )
			{
			case ioPlayMode::MS_READY:	case ioPlayMode::MS_PLAY:
				if( pCAI->GetMyself()->IsApplyDownState( false ) )
				{
					std::string	msg;

					if( pCAI->GetMyself()->IsMale() )	
						msg = g_AIDB.GetRandomTextItem( "down_male" );
					else	
						msg = g_AIDB.GetRandomTextItem( "down_female" );

					pCAI->GetMyself()->SetChatBubble(	
										pCAI->GetMyself()->GetViewName().c_str(), 
										msg.c_str(), false,
										FRAMEGETTIME(), m_iDuration );
				}
				else if( pCAI->GetMyself()->IsCharacterWounded() )
				{
					std::string	msg;

					if( pCAI->GetMyself()->IsMale() )	
						msg = g_AIDB.GetRandomTextItem( "wounded_male" );
					else	
						msg = g_AIDB.GetRandomTextItem( "wounded_female" );

					pCAI->GetMyself()->SetChatBubble(	
										pCAI->GetMyself()->GetViewName().c_str(), 
										g_AIDB.GetRandomTextItem( "wounded" ).c_str(), false,
										FRAMEGETTIME(), m_iDuration );
				}
				else
				{
					pCAI->GetMyself()->SetChatBubble(	
										pCAI->GetMyself()->GetViewName().c_str(), 
										m_szChatMsg.c_str(), false,
										FRAMEGETTIME(), m_iDuration );
				}
				break;
			}
			m_bLastResult = true;
		}
		else
			LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_IsCurrentState : Creator[%x] or PlayMode[%x] or is null",
									pCAI->GetMyself()->GetCreator(),
									pCAI->GetMyself()->GetCreator()->GetPlayMode() );
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_ShowChat : pCAI[%x] or Myself[%x] is null",
								pCAI, pCAI->GetMyself() );

	return m_bLastResult;
}

std::string ioAIExec_Myself_ShowChat::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_ShowChat {m_szChatMsg=\"%s\"}", m_szChatMsg.c_str() );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_DoKeySeq::ioAIExec_Myself_DoKeySeq(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_DoKeySeq, op ), m_szKeySeqName()
{
	SetArgs( args );
}

void ioAIExec_Myself_DoKeySeq::SetArgs( const std::string& args )
{
	std::string	keySeqName = args;
	m_szKeySeqName = ltrim(rtrim(keySeqName));
}

bool ioAIExec_Myself_DoKeySeq::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	if( pCAI )
	{
		pCAI->CopyCurKeySeq( m_szKeySeqName );

		if( pCAI->IsDebugging( ioAIBrain::AIDEBUG_KEYSEQ ) )
			LOG.PrintTimeAndLog( 0, "Setting up : %s", m_szKeySeqName.c_str() );
		
		m_bLastResult = true;
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_DoKeySeq : pCAI[%x] is null",
								pCAI );
	return m_bLastResult;
}

std::string ioAIExec_Myself_DoKeySeq::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_DoKeySeq {m_szKeySeqName=%s}", m_szKeySeqName.c_str() );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_Move::ioAIExec_Myself_Move( AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_Move, op ), m_nDuration( 0 )
{
	SetArgs( args );
}

void ioAIExec_Myself_Move::SetArgs( const std::string& args )
{
	// find move type
	std::regex	reg( g_AISymbolTable.GetSymbolString( "move" ) );
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		m_DirType		= (MoveDirType)g_AISymbolTable.GetSymbolValue( std::string("MoveDirType"), trim(std::string(sm[1])) );
		m_szMoveSeqName	= sm[2];
	}
	else
	{
		m_DirType		= AI_MOVE_STRICT_CENTER;
		m_szMoveSeqName	= "STOP";
		LOG.PrintTimeAndLog( 0, "failed to parse the given arg in ioAIExec_Myself_Move with : %s", args.c_str() );
	}
}

bool ioAIExec_Myself_Move::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	if( pCAI )
	{
		int	iPrevDir = pCAI->GetPrevDirection();
		int iDirection = -1;
		switch( m_DirType )
		{
		case AI_MOVE_STRICT_CENTER:	
		case AI_MOVE_STRICT_DOWN:
		case AI_MOVE_STRICT_DN_LT:
		case AI_MOVE_STRICT_LEFT:
		case AI_MOVE_STRICT_UP_LT:
		case AI_MOVE_STRICT_UP:
		case AI_MOVE_STRICT_UP_RT:
		case AI_MOVE_STRICT_RIGHT:
		case AI_MOVE_STRICT_DN_RT:
			iDirection = (int)m_DirType;
			break;
	
		case AI_MOVE_TOWARD_TARGET:
			 iDirection = ( (int)pCAI->GetTargetAngle( pTarget ) % 360 ) / 45;
			 break;
		case AI_MOVE_TURNBACK_TARGET:
			 iDirection = ( (int)(pCAI->GetTargetAngle( pTarget ) + 180.0f ) % 360 ) / 45;
			 break;
		case AI_MOVE_TURNLEFT_TARGET:
			 iDirection = ( (int)(pCAI->GetTargetAngle( pTarget ) +FLOAT90 ) % 360 ) / 45;
			 break;
		case AI_MOVE_TURNRIGHT_TARGET:
			 iDirection = ( (int)(pCAI->GetTargetAngle( pTarget ) + 270.0f ) % 360 ) / 45;
			 break;
		case AI_MOVE_START_POS:
			iDirection = ( (int)(pCAI->GetTargetPosAngle( pCAI->GetMyself()->GetStartPos() )) % 360 ) / 45;
			break;
		}

		// move
		if( iDirection == -1 )
		{
			// 현재 움직임을 모두 없애고 (아마 없앨 일은 없겠지만) 멈춘다.
			pCAI->ClearCurKeySeq();
			pCAI->AppendCurKeySeq( "STOP_CUR_MOVE", iDirection );
		}
		else if( iPrevDir == iDirection )
		{
			// 진행 방향으로 움직임을 유지한다.
			pCAI->CopyCurKeySeq( "KEEP_CUR_MOVE", iDirection );	// overide input type with direction
		}
		else
		{
			// 현재 방향의 움직임을 멈추고, 이동 방향으로 원하는 유형으로 움직이기 시작한다
			pCAI->ClearCurKeySeq();
			pCAI->AppendCurKeySeq( "STOP_CUR_MOVE", iPrevDir );
			pCAI->AppendCurKeySeq( m_szMoveSeqName, iDirection );
			pCAI->AppendCurKeySeq( "KEEP_CUR_MOVE", iDirection );
		}

		pCAI->SetPrevDirection( iDirection );

		if( pCAI->IsDebugging( ioAIBrain::AIDEBUG_MOVESEQ ) )
			LOG.PrintTimeAndLog( 0, "Move seq : %d %s", iDirection, m_szMoveSeqName.c_str() );
		
		m_bLastResult = true;
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_IsCurrentState : pCAI[%x] or Myself[%x] or Target[%s] is null",
								pCAI, pCAI->GetMyself(), pTarget );

	return m_bLastResult;
}

std::string ioAIExec_Myself_Move::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_Move {m_DirType=%d,m_szMoveSeqName=%d}", m_DirType, m_szMoveSeqName );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_Turn::ioAIExec_Myself_Turn( AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_Turn, op )
{
	SetArgs( args );
}

void ioAIExec_Myself_Turn::SetArgs( const std::string& args )
{
	// find move type
	std::regex	reg(	g_AISymbolTable.GetSymbolString("turn"));
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		m_DirType	= 
			(ioAIExec_Myself_Move::MoveDirType)g_AISymbolTable.GetSymbolValue( 
							std::string("MoveDirType"), trim(std::string(sm[1])) );
	}
	else
	{
		m_DirType	= ioAIExec_Myself_Move::AI_MOVE_STRICT_CENTER;
		LOG.PrintTimeAndLog( 0, "failed to parse the given arg in ioAIExec_Myself_Turn with : %s", 
								args.c_str() );
	}
}

bool ioAIExec_Myself_Turn::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	if( pCAI && pTarget )
	{
		int	iPrevDir = pCAI->GetPrevDirection();
		int iDirection = -1;
		switch( m_DirType )
		{
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_CENTER:	
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_DOWN:
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_DN_LT:
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_LEFT:
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_UP_LT:
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_UP:
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_UP_RT:
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_RIGHT:
		case ioAIExec_Myself_Move::AI_MOVE_STRICT_DN_RT:		
			iDirection = (int)m_DirType;		
			break;

		case ioAIExec_Myself_Move::AI_MOVE_TOWARD_TARGET:		 
			iDirection = ( (int)pCAI->GetTargetAngle( pTarget ) % 360 ) / 45;		 
			break;

		case ioAIExec_Myself_Move::AI_MOVE_TURNBACK_TARGET:		 
			iDirection = ( (int)(pCAI->GetTargetAngle( pTarget ) + 180.0f ) % 360 ) / 45;		 
			break;

		case ioAIExec_Myself_Move::AI_MOVE_TURNLEFT_TARGET:		 
			iDirection = ( (int)(pCAI->GetTargetAngle( pTarget ) +FLOAT90 ) % 360 ) / 45;		 
			break;

		case ioAIExec_Myself_Move::AI_MOVE_TURNRIGHT_TARGET:		 
			iDirection = ( (int)(pCAI->GetTargetAngle( pTarget ) + 270.0f ) % 360 ) / 45;		 
			break;
		}

		// turn
		if( !pCAI->GetMyself()->IsApplyDownState( false ) )
		{
			switch( pCAI->GetMyself()->GetState() )
			{
			default:
				if( iPrevDir != iDirection )
				{
					pCAI->GetMyself()->SetTargetRotToTargetPos( pTarget, false, false );
				}

				pCAI->SetPrevDirection( iDirection );
				m_bLastResult = true;
				break;

			case CS_BLOW_WOUNDED:	case CS_USING_SKILL:	case CS_FLOAT_STATE:
			case CS_WARP_STATE:		case CS_FROZEN:			case CS_ETC_STATE:
				m_bLastResult = false;
				break;
			}
		}
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_Turn : pCAI[%x] of Myself[%x] or pTarget[%x] is null",
								pCAI, pCAI->GetMyself(), pTarget );

	return m_bLastResult;
}

std::string ioAIExec_Myself_Turn::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_Move {m_DirType=%d}", m_DirType );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_Idle::ioAIExec_Myself_Idle(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_Idle, op )
{
	SetArgs( args );
}

void ioAIExec_Myself_Idle::SetArgs( const std::string& args )
{
	std::regex	reg( g_AISymbolTable.GetSymbolString("int") );
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )		m_nIdleTime	= ConvertToInt( sm[1] );
	else										
	{
		m_nIdleTime	= 0;
		LOG.PrintTimeAndLog( 0, "failed to parse the given arg in ioAIExec_Myself_Idle with : %s", args.c_str() );
	}
}

bool ioAIExec_Myself_Idle::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	// cancle all remaining actions
	m_bLastResult = false;
	if( pCAI && pCAI->GetIdleTimer() )
	{
		pCAI->StopCurrentAction();

		if( !pCAI->GetIdleTimer()->IsRunning() )
		{
			pCAI->GetIdleTimer()->StartTimer( m_nIdleTime );
		}

		if( pCAI->IsDebugging( ioAIBrain::AIDEBUG_IDLE ) )
			LOG.PrintTimeAndLog( 0, "------------------------<< IDLE [%d] >>-------------------------", m_nIdleTime );
		
		m_bLastResult = true;
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_Idle : pCAI[%x] of Idle timer[%x] is null",
								pCAI, pCAI->GetIdleTimer() );

	return m_bLastResult;
}

std::string ioAIExec_Myself_Idle::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_Idle {m_nIdleTime=%d}", m_nIdleTime );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_IsSkillAvailable::ioAIExec_Myself_IsSkillAvailable(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_IsSkillAvailable, op )
{
	SetArgs( args );
}

void ioAIExec_Myself_IsSkillAvailable::SetArgs( const std::string& args )
{
	// find move type
	_ENCSTR( "skillkey", szEncSkillKey );
	std::regex	reg( g_AISymbolTable.GetSymbolString( szEncSkillKey ) );
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		switch( g_AISymbolTable.GetKeyInputType( sm[1] ) )
		{
		case AI_BvT_KIT_CLOAK_AS:		m_EquipSlot = ES_CLOAK;		break;
		case AI_BvT_KIT_HELMET_AD:		m_EquipSlot = ES_HELMET;	break;
		case AI_BvT_KIT_WEAPON_ASD:		m_EquipSlot = ES_ARMOR;		break;
		case AI_BvT_KIT_ARMOR_SD:		m_EquipSlot = ES_WEAPON;	break;
		default:						m_EquipSlot = ES_UNKNOWN;	break;
		}
	}
	else
	{
		m_EquipSlot = ES_UNKNOWN;
		LOG.PrintTimeAndLog( 0, "failed to parse the given arg in ioAIExec_Myself_IsSkillAvailable with : %s", args.c_str() );
	}
}

bool ioAIExec_Myself_IsSkillAvailable::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	if( pCAI &&	pCAI->GetMyself() )
	{
		SkillUseType	eType	= pCAI->GetMyself()->GetAvailableSkillUseType();
		ioSkill*		pSkill	= pCAI->GetMyself()->GetUseEnableSkill( m_EquipSlot, eType, false );
	
		m_bLastResult = (pSkill != (ioSkill*)0 );
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_IsSkillAvailable : pCAI[%x] of Myself[%x] is null",
								pCAI, pCAI->GetMyself() );

	return m_bLastResult;
}

std::string ioAIExec_Myself_IsSkillAvailable::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_IsSkillAvailable {m_EquipSlot=%d}", m_EquipSlot );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_IsCombo::ioAIExec_Myself_IsCombo(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_IsCombo, op )
{
	SetArgs( args );
}

void ioAIExec_Myself_IsCombo::SetArgs( const std::string& args )
{
	std::regex	reg( g_AISymbolTable.GetSymbolString("int") );
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )		m_iAttackCombo	= ConvertToInt( sm[1] );
	else										
	{
		LOG.PrintTimeAndLog( 0, "failed to parse the given arg in ioAIExec_Myself_IsCombo with : %s", args.c_str() );
		m_iAttackCombo	= 0;
	}
}

bool ioAIExec_Myself_IsCombo::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	if( pCAI && pCAI->GetMyself() )
	{
		m_bLastResult = ( m_iAttackCombo == pCAI->GetMyself()->GetCurAttackComboNum() );
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_IsCombo : pCAI[%x] of Myself[%x] is null",
								pCAI, pCAI->GetMyself() );

	return m_bLastResult;
}

std::string ioAIExec_Myself_IsCombo::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_IsCombo {m_iAttackCombo=%d}", m_iAttackCombo );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_CanDoCombo::ioAIExec_Myself_CanDoCombo(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_CanDoCombo, op )
{
}

bool ioAIExec_Myself_CanDoCombo::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	if( pCAI && pCAI->GetMyself() )
	{
		m_bLastResult = ( pCAI->GetMyself()->IsComboAttackPossible() );
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_CanDoCombo : pCAI[%x] of Myself[%x] is null",
								pCAI, pCAI->GetMyself() );

	return m_bLastResult;
}

std::string ioAIExec_Myself_CanDoCombo::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_CanDoCombo " );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Myself_WaitForCombo::ioAIExec_Myself_WaitForCombo(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_MYSELF_WaitForCombo, op )
{
}

void ioAIExec_Myself_WaitForCombo::SetArgs( const std::string& args )
{
	std::regex	reg(	std::string(
						g_AISymbolTable.GetSymbolString("int")		+ std::string("|") + 
						g_AISymbolTable.GetSymbolString("intRange") ) );
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		if( sm[1] != "" )	m_nFrontMargin	= (unsigned int)ConvertToInt( sm[1] );
		if( sm[2] != "" )	m_nTailMargin	= (unsigned int)ConvertToInt( sm[2] );
	}
	else
	{
		m_nFrontMargin	= 0;
		m_nTailMargin	= 0;

		LOG.PrintTimeAndLog( 0, "failed to parse the given arg in ioAIExec_Myself_WaitForCombo with : %s", args.c_str() );
	}
}

bool ioAIExec_Myself_WaitForCombo::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	if( pCAI )
	{
		pCAI->WaitForCombo( m_nFrontMargin, m_nTailMargin );

		m_bLastResult = true;
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_WaitForCombo : pCAI[%x] is null", pCAI );

	return m_bLastResult;
}

std::string ioAIExec_Myself_WaitForCombo::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Myself_WaitForCombo " );
	retStr = buff;

	return retStr;
}
