
#include "stdafx.h"

#include "ioCheckTargetBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioPassiveSkill.h"

ioCheckTargetBuff::ioCheckTargetBuff()
{
	m_SortList.clear();
}

ioCheckTargetBuff::ioCheckTargetBuff( const ioCheckTargetBuff &rhs )
	: ioBuff( rhs ),
	  m_fTargetColRange( rhs.m_fTargetColRange ),
	  m_fTargetColAngle( rhs.m_fTargetColAngle ),
	  m_fTargetColUpHeight( rhs.m_fTargetColUpHeight ),
	  m_fTargetColUnderHeight( rhs.m_fTargetColUnderHeight ),
	  m_TargetBuff( rhs.m_TargetBuff ),
	  m_fGaugeDec( rhs.m_fGaugeDec ),
	  m_nEquipSlot( rhs.m_nEquipSlot ),
	  m_bEnableTargetDown( rhs.m_bEnableTargetDown )
{
}

ioCheckTargetBuff::~ioCheckTargetBuff()
{
	m_SortList.clear();
}

void ioCheckTargetBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fTargetColRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetColAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetColUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetColUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );

	m_fGaugeDec = rkLoader.LoadFloat_e( "use_passive_gauge", 0.0f );
	m_nEquipSlot = rkLoader.LoadInt_e( "equip_slot", 0 );
	m_bEnableTargetDown = rkLoader.LoadBool_e( "enable_down_target", false );
		
	rkLoader.LoadString_e( "target_buff_name", "", szBuf, MAX_PATH );
	m_TargetBuff = szBuf;
}

ioBuff* ioCheckTargetBuff::Clone()
{
	return new ioCheckTargetBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckTargetBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_BuffState = BS_Find;
}

bool ioCheckTargetBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_BuffState = BS_Find;

	return true;
}

void ioCheckTargetBuff::ProcessBuff( float fTimePerSec )
{
	if ( !m_pOwner || !IsProcess( m_pOwner ) )
	{
		SetReserveEndBuff();
		return;
	}

	ioPlayStage *pStage = m_pOwner->GetCreator();
	ioHashString szCharName = m_pOwner->GetCharName();
	if( pStage && pStage->GetModeType() == MT_BATTLE && !g_BattleRoomMgr.CheckUserBattleFightFlag( szCharName ) )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	switch( m_BuffState )
	{
	case BS_Find:
		{
			if ( FindTarget( m_pOwner, m_fTargetColRange, m_fTargetColAngle, m_fTargetColUpHeight, m_fTargetColUnderHeight, true, false ) )
				SetAttackState( m_pOwner );
		}
		break;
	case BS_Attack:
		{
			FindTarget( m_pOwner, m_fTargetColRange, m_fTargetColAngle, m_fTargetColUpHeight, m_fTargetColUnderHeight, false, true );

			if ( m_pOwner->IsNeedProcess() && !IsCompareTarget( m_pOwner ) )
			{
				SetFindState( m_pOwner );
				return;
			}

			UpdatePassiveGauge( m_pOwner );
		}
		break;
	}
}

void ioCheckTargetBuff::EndBuff()
{
	ioBuff::EndBuff();

	if ( m_pOwner && !m_szCurTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = m_pOwner->GetBaseChar( m_szCurTarget );
		if ( pTarget )
			RemoveTargetBuff( m_pOwner, pTarget );
	}
}

void ioCheckTargetBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case BS_Attack:
		{
			rkPacket >> m_szCurTarget;
			SetAttackState( pOwner );
		}
		break;
	case BS_Find:
		{
			rkPacket >> m_szCurTarget;
			SetFindState( pOwner );
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioCheckTargetBuff::FindTarget( ioBaseChar *pOwner, float fColRange, float fColAngle, float fUpHeight, float fUnderHeight, bool bChangeTarget, bool bCheckBuff )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;
	
	//타겟 리스트
	BaseCharList vTargetList;

	//위치 설정
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	//타겟 리스트 얻기
	pOwner->GetTargetCharListByCylinder( vPos, fColRange, fColAngle, fUpHeight, fUnderHeight, vTargetList );
	
	//타겟 리스트 소팅
	SortTargetList( pOwner, vTargetList, bCheckBuff );
	
	if ( m_SortList.empty() )
		return false;

	std::sort( m_SortList.begin(), m_SortList.end() );

	if ( bChangeTarget )
		m_szCurTarget = m_SortList[0].szCharName;

	return true;
}

void ioCheckTargetBuff::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList, bool bCheckBuff )
{
	if( !pOwner )	
		return;

	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.clear();

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();

	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != pOwner )
		{
			//팀이 같거나 타겟이 될수 없으면 리스트에 저장 안함
			if ( pOwner->GetTeam() == pCurChar->GetTeam() )
				continue;

			if ( !IsEnableTarget( pOwner, pCurChar, bCheckBuff ) )
				continue;

			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			DistInfo sDist;
			sDist.szCharName = pCurChar->GetCharName();
			sDist.fDistSq = fCurDistSq;
			m_SortList.push_back( sDist );
		}
	}
}

bool ioCheckTargetBuff::IsEnableTarget( ioBaseChar *pOwner, ioBaseChar *pTarget, bool bCheckBuff )
{
	if ( !pOwner || !pTarget || pTarget->HasHideBuff() )
		return false;
	
	if ( bCheckBuff && !pTarget->HasBuff( pOwner->GetCharName(), m_TargetBuff ) )
		return false;

	if( pTarget->IsSystemState() )
		return false;
			
	CharState eTargetState = pTarget->GetState();
	switch( eTargetState )
	{
	case CS_BLOW_WOUNDED:
		{
			if ( m_bEnableTargetDown && !pTarget->IsFloatingState() )
				return true;
			return false;
		}
	case CS_BOUND_BLOW_WOUNDED:
		{
			if ( m_bEnableTargetDown && !pTarget->IsFloatingState() )
				return true;
			return false;
		}
	case CS_ETC_STATE:
		{
			if ( m_bEnableTargetDown && pTarget->IsApplyDownState(false) )
				return true;
			return false;
		}
	case CS_USING_SKILL:
		{
			if ( m_bEnableTargetDown && pTarget->IsApplyDownState(false) )
				return true;
			return false;
		}
	case CS_FROZEN:
		{
			if( m_bEnableTargetDown && pTarget->GetBlowFrozenState() )
				return true;
			return false;
		}
	case CS_STOP_MOTION:
		{
			if( m_bEnableTargetDown && pTarget->GetBlowStopMotionState() )
				return true;
			return false;
		}
	}

	return true;
}

bool ioCheckTargetBuff::IsCompareTarget( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( m_SortList.empty() || m_szCurTarget.IsEmpty() )
		return false;
	
	bool bFindTarget = false;
	for ( int i=0; i<(int)m_SortList.size(); i++ )
	{
		if ( !strcmp( m_szCurTarget.c_str(), m_SortList[i].szCharName.c_str() ) )
			bFindTarget = true;
	}

	if ( !bFindTarget )
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckTargetBuff::AddTargetBuff( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if ( !pOwner || !pTarget )
		return;

	if ( !m_TargetBuff.IsEmpty() )
		pTarget->AddNewBuff( m_TargetBuff, pOwner->GetCharName(), "", NULL );
}

void ioCheckTargetBuff::RemoveTargetBuff( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if ( !pOwner || !pTarget )
		return;

	if ( !m_TargetBuff.IsEmpty() )
	{
		ioBuff *pRemoveTargetBuff = pTarget->GetBuff( m_TargetBuff, pOwner->GetCharName() );
		if ( pRemoveTargetBuff )
			pRemoveTargetBuff->SetReserveEndBuff();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckTargetBuff::SetFindState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	if ( !m_szCurTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szCurTarget );
		if ( pTarget )
			RemoveTargetBuff( pOwner, pTarget );
	}

	m_BuffState = BS_Find;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Find;
		kPacket << m_szCurTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_szCurTarget.Clear();
}

void ioCheckTargetBuff::SetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;	

	if ( !m_szCurTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szCurTarget );
		if ( pTarget )
			AddTargetBuff( pOwner, pTarget );
	}

	m_BuffState = BS_Attack;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Attack;
		kPacket << m_szCurTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckTargetBuff::UpdatePassiveGauge( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pOwner->GetEquipedSkill( m_nEquipSlot ) );
	if( !pPassiveSkill || pPassiveSkill->GetName() != GetSkillName() )
		return;

	float fUseGauge = m_fGaugeDec * g_FrameTimer.GetSecPerFrame();
	pPassiveSkill->ReduceCurPassiveGauge( fUseGauge );
}

bool ioCheckTargetBuff::IsProcess( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pOwner->GetEquipedSkill( m_nEquipSlot ) );
	if( !pPassiveSkill || pPassiveSkill->GetName() != GetSkillName() )
		return false;

	if ( pPassiveSkill->GetCurPassiveGauge() <= 0.0f )
		return false;

	 return true;
}