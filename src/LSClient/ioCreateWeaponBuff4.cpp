

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioCreateWeaponBuff4.h"

ioCreateWeaponBuff4::ioCreateWeaponBuff4()
{
	m_dwWeaponDeadIndex = 0;
}

ioCreateWeaponBuff4::ioCreateWeaponBuff4( const ioCreateWeaponBuff4 &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
	m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
	m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
	m_bEnableOutLine( rhs.m_bEnableOutLine ),
	m_fOutLineR( rhs.m_fOutLineR ),
	m_fOutLineG( rhs.m_fOutLineG ),
	m_fOutLineB( rhs.m_fOutLineB ),
	m_fOutLineAlpha( rhs.m_fOutLineAlpha ),
	m_fOutLineThickness( rhs.m_fOutLineThickness ),
	m_WeaponInfo( rhs.m_WeaponInfo ),
	m_fFireHeightOffSet( rhs.m_fFireHeightOffSet ),
	m_vFindBuffInfo( rhs.m_vFindBuffInfo ),
	m_dwFireTic( rhs.m_dwFireTic ),
	m_dwStartSearchTime( rhs.m_dwStartSearchTime ),
	m_fFindLength( rhs.m_fFindLength ),
	m_target_wound_type( rhs.m_target_wound_type )
{
	m_dwWeaponDeadIndex = 0;
}

ioCreateWeaponBuff4::~ioCreateWeaponBuff4()
{
}

void ioCreateWeaponBuff4::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState = rkLoader.LoadBool_e( "disable_drop_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bEscapeCatchState = rkLoader.LoadBool_e( "escape_catch_state", false );

	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );
	//////////////////////////////////////////////////////////////////////////
	m_fFireHeightOffSet = rkLoader.LoadFloat_e( "fire_offset_height", 0.f );

	m_fFindLength = rkLoader.LoadFloat( "find_enemy_length", 0.f );

	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_index", NO_WEAPON_ATTRIBUTE );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_index_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "attack_wounded", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_wounded_time", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_wounded_loop_ani", false );

	m_dwStartSearchTime = (DWORD)rkLoader.LoadInt( "start_serach_wait_time", 0);
	m_dwFireTic = (DWORD)rkLoader.LoadInt( "fire_tic_time", 10000);

	int nCnt = rkLoader.LoadInt( "search_rule_cnt", 0);
	for( int i = 0; i < nCnt; i++ )
	{
		FindBuffInfo info;
		wsprintf_e( szKey, "buff_search_cnt_%d", i+1 );
		info.nCnt = rkLoader.LoadInt( szKey, 100 );

		wsprintf_e( szKey, "after_fire_cnt_%d", i+1 );
		info.nFireCnt = rkLoader.LoadInt( szKey, 0 );

		m_vFindBuffInfo.push_back( info );
	}

	m_target_wound_type = rkLoader.LoadInt( "target_wound_type", TWT_ALL );
}

ioBuff* ioCreateWeaponBuff4::Clone()
{
	return new ioCreateWeaponBuff4( *this );
}

void ioCreateWeaponBuff4::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	/*if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}*/

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	m_BuffState = BS_NONE;
}

bool ioCreateWeaponBuff4::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	return true;
}

void ioCreateWeaponBuff4::StartFireCheckTime( ioBaseChar* pOwner )
{
	m_BuffState = BS_FIRE;
	m_dwFireStartTime = FRAMEGETTIME();

	m_nFireIndex = 0;
	m_dwNextFireTime = m_dwFireStartTime + m_dwFireTic;

	m_nMaxFireCnt = 0;

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	int nCheckCnt = 0;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;
		if( pTarget->GetTeam() != pOwner->GetTeam() ) 
			continue;
		
		if( pTarget->HasBuff( BT_CREATE_WEAPON_BUFF4, GetCreator()->GetCharName() ) )
			nCheckCnt++;
	}

	for( int i = 0; i < (int)m_vFindBuffInfo.size(); i++ )
	{
		const FindBuffInfo& info = m_vFindBuffInfo[i];
		if( info.nCnt >= nCheckCnt )
		{
			m_nMaxFireCnt = info.nFireCnt;
			return;
		}
	}

}

void ioCreateWeaponBuff4::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pCreator = GetCreator();

	if( !pCreator )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	/*if( pCreator->GetState() == CS_DIE )
	{
		SetReserveEndBuff();
		return;
	}*/

	//////////////////////////////////////////////////////////////////////////
	CheckCallingProcess();

	/*if( GetBuffDuration() == 0 )
	{		
	return;
	}*/

	if( m_dwBuffEndTime <= FRAMEGETTIME() )
	{		
		SetReserveEndBuff();
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BuffState )
	{
	case BS_NONE:
		if( dwCurTime > m_dwBuffStartTime + m_dwStartSearchTime )
		{
			StartFireCheckTime( m_pOwner );
			m_BuffState = BS_FIRE;
		}
		break;
	case BS_FIRE:
		if( m_pOwner->IsNeedProcess() )
		{
			if( m_nFireIndex >= m_nMaxFireCnt )
			{
				SetReserveEndBuff();
				m_BuffState = BS_END;

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
					kPacket << m_pOwner->GetCharName();
					kPacket << GetName();
					kPacket << BS_END;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				return;
			}

			if( m_dwNextFireTime <= dwCurTime )
			{
				//Search루틴
				if( !FindEnemy( m_pOwner) )
					break;

				m_dwNextFireTime = dwCurTime + m_dwFireTic;
			}
		}
		break;
	case BS_WAIT:
		break;
	case BS_END:
		SetReserveEndBuff();
		break;
	default: SetReserveEndBuff(); break;
	}

}


bool ioCreateWeaponBuff4::IsEnableTargetState( ioBaseChar* pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( (TargetWoundType)m_target_wound_type ) )
		return false;

	return true;
}


bool ioCreateWeaponBuff4::FindEnemy( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fTargetRange = m_fFindLength;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == pOwner->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRange = D3DXVec3Length( &vDiff );

		

		// 거리 체크
		if( fDiffRange < fTargetRange )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRange;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRange < fCurGap )
			{
				fCurGap = fDiffRange;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			D3DXVECTOR3 vSrcPos = pOwner->GetWorldPosition() + D3DXVECTOR3( 0.f, m_fFireHeightOffSet, 0.f );
			D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - vSrcPos;
			D3DXVec3Normalize( &vDir, &vDir );
			CreateWeapon( vSrcPos, vDir );
			return true;
		}
	}

	return false;
}


void ioCreateWeaponBuff4::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case BS_FIRE:
		{
			D3DXVECTOR3 vPos;
			D3DXVECTOR3 vDir;

			rkPacket >> vPos;
			rkPacket >> vDir;

			CreateWeapon( vPos, vDir );
		}
		break;
	case BS_END:
		SetReserveEndBuff();
		break;
	}
}


void ioCreateWeaponBuff4::CreateWeapon(const D3DXVECTOR3& pos, const D3DXVECTOR3& dir)
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = GetCreator();
	if( !pCreator )
		return;

	/*D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	vPos.y += m_fFireHeightOffSet;
	DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();
	ioWeapon* pWeapon = pCreator->DummyExplicit( m_WeaponInfo,
	vPos,
	dwWeaponIndex,
	false,
	pOwner->GetCharName(),
	m_Name,
	m_bUseCallAttWeaponCol );

	if( pWeapon )
	{
	pWeapon->SetPosition( vPos );
	pWeapon->SetMoveDir( D3DXVECTOR3(0.f, -FLOAT1, 0.f) );
	}*/

	DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();
	ioWeapon* pWeapon = pCreator->DummyExplicit( m_WeaponInfo,
		pos,
		dwWeaponIndex,
		false,
		pOwner->GetCharName(),
		m_Name,
		m_bUseCallAttWeaponCol );

	if( pWeapon )
	{
		pWeapon->SetPosition( pos );
		pWeapon->SetMoveDir( dir );
	}

	m_nFireIndex++;
	//SendPacket

	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_FIRE;
		kPacket << pos;
		kPacket << dir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCreateWeaponBuff4::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	if( m_bEnableOutLine )
	{
		m_pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );
	}

	ioBuff::EndBuff();

	/*if( 0 < m_dwWeaponDeadIndex )
	{
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner )
			return;

		ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
		if( !pCreator )
			return;

		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwWeaponDeadIndex );
		if( pWeapon )	
			pWeapon->SetWeaponDead();
	}*/
}