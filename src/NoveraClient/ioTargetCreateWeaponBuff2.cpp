
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTargetCreateWeaponBuff2.h"

#include "WeaponDefine.h"

ioTargetCreateWeaponBuff2::ioTargetCreateWeaponBuff2()
{
	Init();
}

ioTargetCreateWeaponBuff2::ioTargetCreateWeaponBuff2( const ioTargetCreateWeaponBuff2 &rhs )
	: ioBuff( rhs ),
      m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_RemoveBuffList( rhs.m_RemoveBuffList ),
	  m_szReadyMarker( rhs.m_szReadyMarker ),
	  m_szAimMarker( rhs.m_szAimMarker ),
	  m_szPointMarker( rhs.m_szPointMarker ),
	  m_fReadyMarkerRange( rhs.m_fReadyMarkerRange ),
	  m_szAttackAni( rhs.m_szAttackAni ),
	  m_fAttackRate( rhs.m_fAttackRate ),
	  m_nAimWeapon( rhs.m_nAimWeapon ),
	  m_nPointWeapon( rhs.m_nPointWeapon ),
	  m_nTotalWeaponCnt( rhs.m_nTotalWeaponCnt ),
	  m_vCreateWeaponOffset( rhs.m_vCreateWeaponOffset ),
	  m_vArriveWeaponOffset( rhs.m_vArriveWeaponOffset ),
	  m_szWeaponRedEffect( rhs.m_szWeaponRedEffect ),
	  m_szWeaponBlueEffect( rhs.m_szWeaponBlueEffect ),
	  m_fTargetAngle( rhs.m_fTargetAngle ),
	  m_fTargetRange( rhs.m_fTargetRange ),
	  m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
	  m_fTargetUnderHeight( rhs.m_fTargetUnderHeight ),
	  m_nMaxTargetCnt( rhs.m_nMaxTargetCnt ),
	  m_fJumpPower( rhs.m_fJumpPower ),
	  m_szHideBuff( rhs.m_szHideBuff ),
	  m_bSetAttackBuff( rhs.m_bSetAttackBuff ),
	  m_dwCameraEvent( rhs.m_dwCameraEvent ),
	  m_szCameraBuff( rhs.m_szCameraBuff ),
	  m_EnableInputKeyState( rhs.m_EnableInputKeyState ),
	  m_BuffEndState( rhs.m_BuffEndState )
{
	Init();
}

ioTargetCreateWeaponBuff2::~ioTargetCreateWeaponBuff2()
{	
	m_AimMarkerList.clear();
	m_PointMarkerList.clear();
	m_EnableInputKeyState.clear();
	m_BuffEndState.clear();
}

void ioTargetCreateWeaponBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH] = "";
	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	char szRemoveBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szRemoveBuf, "remove_buff%d_name",i+1);
		rkLoader.LoadString( szRemoveBuf, "", szBuf, MAX_PATH );
		m_RemoveBuffList.push_back( szBuf );
	}

	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );

	rkLoader.LoadString( "hide_buff", "", szBuf, MAX_PATH );
	m_szHideBuff = szBuf;

	m_bSetAttackBuff = rkLoader.LoadBool_e( "set_attack_buff", false );

	m_dwCameraEvent = rkLoader.LoadInt_e( "camera_event", 0 );

	rkLoader.LoadString_e( "camera_buff", "", szBuf, MAX_PATH );
	m_szCameraBuff = szBuf;

	//추가 읽기
	LoadTargetProperty( rkLoader );
}

ioBuff* ioTargetCreateWeaponBuff2::Clone()
{
	return new ioTargetCreateWeaponBuff2( *this );
}

void ioTargetCreateWeaponBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	Init();

	m_iMaxActiveCount = m_nTotalWeaponCnt;
	m_iCurActiveCount = m_nTotalWeaponCnt;

	if ( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		CreateMarker( m_pOwner, m_szReadyMarker, vPos, false, m_dwReadyMakerID );
	}

	CreateMarkerList( pOwner, m_AimMarkerList, m_szAimMarker, m_nMaxTargetCnt );
	CreateMarkerList( pOwner, m_PointMarkerList, m_szPointMarker, m_nMaxTargetCnt );
	
	if ( pOwner->IsNeedProcess() )
	{
		SetCameraEvent( pOwner );
		SetCameraBuff( pOwner );
	}
}

bool ioTargetCreateWeaponBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	Init();

	m_iMaxActiveCount = m_nTotalWeaponCnt;
	m_iCurActiveCount = m_nTotalWeaponCnt;

	CreateMarkerList( pOwner, m_AimMarkerList, m_szAimMarker, m_nMaxTargetCnt );
	CreateMarkerList( pOwner, m_PointMarkerList, m_szPointMarker, m_nMaxTargetCnt );

	return true;
}

void ioTargetCreateWeaponBuff2::ProcessBuff( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( CheckWoundState( m_pOwner ) || CheckBuffEndState( m_pOwner ) || m_dwBuffEndTime < dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}

	switch ( m_BuffState )
	{
	case BS_FindTarget:
		{
			CheckFindTarget( m_pOwner );
			CheckInputKey( m_pOwner );
		}
		break;
	case BS_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_nCurWeaponCnt >= m_nTotalWeaponCnt )
					m_BuffState = BS_End;
				else
				{
					m_BuffState = BS_FindTarget;

					if ( m_bSetAttackBuff && !m_szHideBuff.IsEmpty() && m_pOwner  )
						//m_pOwner->AddNewBuff( m_szHideBuff, m_pOwner->GetCharName(), "", NULL );
						m_pOwner->ReserveAddNewBuff( m_szHideBuff, m_pOwner->GetCharName(), "", NULL );
				}
			}
			else
				CreateWeapon( m_pOwner );
		}
		break;
	case BS_End:
		{
			SetReserveEndBuff();
		}
		break;
	}

	UpdateReadyMarker( m_pOwner );
	UpdateTargetMarker( m_pOwner, m_AimMarkerList, m_AimTargetList );
	UpdateTargetMarker( m_pOwner, m_PointMarkerList, m_PointTargetList );
}

void ioTargetCreateWeaponBuff2::EndBuff()
{	
	ioBuff::EndBuff();
	
	//버프 제거
	for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
		m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
	m_RemoveBuffList.clear();	

	DestroyMarkerList( m_pOwner, m_AimMarkerList );
	DestroyMarkerList( m_pOwner, m_PointMarkerList );
	DestroyMarker( m_pOwner, m_dwReadyMakerID );

	if ( m_pOwner->IsNeedProcess() )
	{
		EndCameraBuff( m_pOwner );
		EndCameraEvent( m_pOwner );
	}

	//피격 상태가 아니면서
	if ( !m_bWound )
	{
		m_pOwner->SetCurMoveSpeed(0.0f);
		m_pOwner->SetJumpPower(0.0f);
		m_pOwner->SetGravityAmt( 0.0f );
		//공중에 있는 상태
		if ( m_pOwner->IsFloating() )
		{
			if ( m_fJumpPower > 0.0f )
				m_pOwner->SetSKillEndJumpState( m_fJumpPower, false, false );
		}
		else
			m_pOwner->SetState( CS_DELAY );
	}
}

void ioTargetCreateWeaponBuff2::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case BS_FindTarget:
		{
			m_PointTargetList.clear();

			int nPointCnt;
			rkPacket >> nPointCnt;

			for ( int i=0; i<nPointCnt; ++i )
			{
				ioHashString szPointTarget;
				rkPacket >> szPointTarget;

				m_PointTargetList.push_back( szPointTarget );
			}
		}
		break;
	case BS_Attack:
		{
			SetAttackState( pOwner, m_szAttackAni, m_fAttackRate );
		}
		break;
	}
}

bool ioTargetCreateWeaponBuff2::CheckEnableUseSkill( int iSlot )
{
	switch( iSlot )
	{
	case ES_WEAPON:
		if( m_bEnableWeaponSkill )
			return true;
		break;
	case ES_ARMOR:
		if( m_bEnableArmorSkill )
			return true;
		break;
	case ES_HELMET:
		if( m_bEnableHelmetSkill )
			return true;
		break;
	case ES_CLOAK:
		if( m_bEnableCloakSkill )
			return true;
		break;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////

void ioTargetCreateWeaponBuff2::Init()
{
	m_bReduceGauge = false;
	m_dwReadyMakerID = -1;	

	m_dwMotionEndTime = 0;
	m_dwWeaponFireTime = 0;
	m_nCurWeaponCnt = 0;

	m_bWound = false;

	m_AimMarkerList.clear();
	m_PointMarkerList.clear();

	m_BuffState = BS_FindTarget;
}

void ioTargetCreateWeaponBuff2::LoadTargetProperty( ioINILoader &rkLoader )
{
	m_nTotalWeaponCnt = rkLoader.LoadInt_e( "total_weapon_count", 0 );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "ready_marker", "", szBuf, MAX_PATH );
	m_szReadyMarker = szBuf;
	rkLoader.LoadString_e( "aim_marker", "", szBuf, MAX_PATH );
	m_szAimMarker = szBuf;
	rkLoader.LoadString_e( "point_marker", "", szBuf, MAX_PATH );
	m_szPointMarker = szBuf;

	m_fReadyMarkerRange = rkLoader.LoadFloat_e( "maker_show_range", 0.0f );

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );
	m_nMaxTargetCnt = rkLoader.LoadInt_e( "max_target_cnt", 0 );

	m_nAimWeapon = rkLoader.LoadInt_e( "aim_weapon_type", NO_WEAPON_ATTRIBUTE );
	m_nPointWeapon = rkLoader.LoadInt_e( "point_weapon_type", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "attack_animation", "", szBuf, MAX_PATH );
	m_szAttackAni = szBuf;
	m_fAttackRate = rkLoader.LoadFloat_e( "attack_animaion_rate", FLOAT1 );
	
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	m_vCreateWeaponOffset.x = rkLoader.LoadFloat_e( "create_weapon_offset_x", 0.0f );
	m_vCreateWeaponOffset.y = rkLoader.LoadFloat_e( "create_weapon_offset_y", 0.0f );
	m_vCreateWeaponOffset.z = rkLoader.LoadFloat_e( "create_weapon_offset_z", 0.0f );

	m_vArriveWeaponOffset.x = rkLoader.LoadFloat_e( "arrive_weapon_offset_x", 0.0f );
	m_vArriveWeaponOffset.y = rkLoader.LoadFloat_e( "arrive_weapon_offset_y", 0.0f );
	m_vArriveWeaponOffset.z = rkLoader.LoadFloat_e( "arrive_weapon_offset_z", 0.0f );

	rkLoader.LoadString_e( "aim_weapon_red_effect", "", szBuf, MAX_PATH );
	m_szWeaponRedEffect = szBuf;
	rkLoader.LoadString_e( "aim_weapon_blue_effect", "", szBuf, MAX_PATH );
	m_szWeaponBlueEffect = szBuf;

	m_EnableInputKeyState.clear();
	int nMaxState = rkLoader.LoadInt_e( "max_enable_input_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szBuf, "enable_input_state%d", i+1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_EnableInputKeyState.push_back( nState );
	}

	m_BuffEndState.clear();
	nMaxState = rkLoader.LoadInt_e( "max_buff_end_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szBuf, "buff_end_state%d", i+1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_BuffEndState.push_back( nState );
	}
}

bool ioTargetCreateWeaponBuff2::CheckWoundState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	CharState eState = pOwner->GetState();
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
			m_bWound = true;
		else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			m_bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			m_bWound = true;
		else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
			m_bWound = true;
		else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
			m_bWound = true;
		else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
			m_bWound = true;
	}

	if( m_bEnableWoundCancel )
	{
		switch( eState )
		{
		case CS_BLOW_WOUNDED:
		case CS_BOUND_BLOW_WOUNDED:
		case CS_WOUNDED:
		case CS_DROP_ZONE_DOWN:
			m_bWound = true;
			break;
		}

		if( m_bWound )
			return true;
	}

	return false;
}

void ioTargetCreateWeaponBuff2::CheckFindTarget( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	m_AimTargetList.clear();

	BaseCharList vTargetList;
	BaseCharDistanceSqList vNewCharList;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	pOwner->GetTargetCharListByCylinder( vPos, m_fTargetRange, m_fTargetAngle, m_fTargetUpHeight, m_fTargetUnderHeight, vTargetList );

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )	
			continue;

		if( pOwner == pTarget )	
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );

		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fDistanceSq;

		vNewCharList.push_back( kTarget );
	}

	if( !vNewCharList.empty() )
	{
		std::sort( vNewCharList.begin(), vNewCharList.end(), BaseCharDistanceSqSort2() );

		for ( int i=0; i<m_nMaxTargetCnt; ++i )
		{
			if ( COMPARE( i, 0, (int)vNewCharList.size() ) )
				m_AimTargetList.push_back( vNewCharList[i].m_Name );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void ioTargetCreateWeaponBuff2::CreateMarker( ioBaseChar *pOwner, ioHashString szMarker, D3DXVECTOR3 vPos, bool bVisible, DWORD& dwMarkID )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( szMarker, vPos, vScale );
	if( pMapEffect )
	{
		pMapEffect->SetVisible( bVisible );
		dwMarkID = pMapEffect->GetUniqueID();
	}
}

void ioTargetCreateWeaponBuff2::DestroyMarker( ioBaseChar *pOwner, DWORD& dwMarkID )
{
	if( !pOwner )
		return;

	if( dwMarkID != -1 )
	{
		pOwner->EndMapEffect( dwMarkID, false );
		dwMarkID = -1;
	}
}

void ioTargetCreateWeaponBuff2::CreateMarkerList( ioBaseChar *pOwner, DWORDVec& MakerList, ioHashString szMarker, int nCnt )
{
	if ( !pOwner )
		return;

	MakerList.clear();

	for ( int i=0; i<nCnt; ++i )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		DWORD dwAimID = -1;
		CreateMarker( pOwner, szMarker, vPos, false, dwAimID );

		if ( dwAimID != -1 )
			MakerList.push_back( dwAimID );
	}
}

void ioTargetCreateWeaponBuff2::DestroyMarkerList( ioBaseChar *pOwner, DWORDVec& MakerList )
{
	if ( !pOwner )
		return;

	if ( MakerList.empty() )
		return;

	for ( int i=0; i<(int)MakerList.size(); ++i )
		DestroyMarker( pOwner, MakerList[i] );

	MakerList.clear();
}

void ioTargetCreateWeaponBuff2::UpdateTargetMarker( ioBaseChar *pOwner, const DWORDVec& MakerList, const ioHashStringVec& TargetList )
{
	if ( !pOwner || MakerList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	for ( int i=0; i<(int)MakerList.size(); ++i )
	{
		if ( COMPARE( i, 0, (int)TargetList.size() ) )
		{
			ioMapEffect *pEffect = pStage->FindMapEffect( MakerList[i] );
			if ( pEffect )
			{
				ioBaseChar *pTarget = pOwner->GetBaseChar( TargetList[i] );
				if ( pTarget )
				{
					D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();

					pEffect->SetWorldPosition( vPos );
					pEffect->SetVisible( true );
				}
			}
		}
		else
		{
			ioMapEffect *pEffect = pStage->FindMapEffect( MakerList[i] );
			if ( pEffect )
				pEffect->SetVisible( false );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void ioTargetCreateWeaponBuff2::UpdateReadyMarker( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || m_dwReadyMakerID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect *pReadyEffect = pStage->FindMapEffect( m_dwReadyMakerID );
	if ( !pReadyEffect )
		return;

	if ( m_BuffState != BS_FindTarget )
		pReadyEffect->SetVisible( false );
	else
	{
		if ( m_AimTargetList.empty() )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, m_fReadyMarkerRange );
			pReadyEffect->SetWorldPosition( vPos );
			pReadyEffect->SetVisible( true );
		}
		else
			pReadyEffect->SetVisible( false );
	}
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioTargetCreateWeaponBuff2::CheckEnableKey( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_EnableInputKeyState.size(); ++i )
	{
		if ( nOwnerState == m_EnableInputKeyState[i] )
			return true;
	}
	
	return false;
}

bool ioTargetCreateWeaponBuff2::CheckBuffEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_BuffEndState.size(); ++i )
	{
		if ( nOwnerState == m_BuffEndState[i] )
		{
			m_bWound = true;
			return true;
		}
	}

	return false;
}

void ioTargetCreateWeaponBuff2::CheckInputKey(  ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() || !CheckEnableKey( pOwner ) )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime > dwCurTime )
		return;

	if ( CheckPointKey( pOwner ) )
		return;	

	if ( CheckAttackKey( pOwner ) )
		return;
}

bool ioTargetCreateWeaponBuff2::CheckAttackKey( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() || pOwner->GetState() == CS_JUMP )
		return false;

	if( m_nCurWeaponCnt < m_nTotalWeaponCnt && 
		pOwner->IsAttackKey() )
	{		
		SetAttackState( pOwner, m_szAttackAni, m_fAttackRate );
		return true;
	}

	return false;
}

bool ioTargetCreateWeaponBuff2::CheckPointKey( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( pOwner->IsDefenseKey() )
	{
		SetPointTarget( pOwner );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

void ioTargetCreateWeaponBuff2::SetAttackState( ioBaseChar *pOwner, ioHashString szName, float fRate )
{
	if ( !pOwner )
		return;

	m_AimTargetList.clear();

	if ( m_bSetAttackBuff && !m_szHideBuff.IsEmpty() )
		pOwner->RemoveBuff( m_szHideBuff, true );

	pOwner->SetEtcState( szName, fRate );

	ioEntityGroup *pGroup = pOwner->GetGroup();
	int iAniID = pGroup->GetAnimationIdx( szName );
	if( iAniID == -1 ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGroup->GetAnimationFullTime( iAniID ) * fRate;

	m_dwWeaponFireTime = dwCurTime;
	m_dwWeaponFireTime += pGroup->GetAnimationEventTime_e( iAniID, "fire_s" ) * fRate;
	
	m_BuffState = BS_Attack;
	
	if ( !m_bReduceGauge )
	{
		ioItem *pOwnerItem = GetOwnerItem();
		if ( pOwnerItem )
			pOwnerItem->SetCurSkillGauge( 0.0f );
		m_bReduceGauge = true;
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_Attack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetCreateWeaponBuff2::SetPointTarget( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	m_PointTargetList.clear();

	if ( !m_AimTargetList.empty() )
	{
		for ( int i=0; i<(int)m_AimTargetList.size(); ++i )
			m_PointTargetList.push_back( m_AimTargetList[i] );
	}

	if ( pOwner->IsNeedSendNetwork() )
	{		
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_FindTarget;

		int nPointCnt = (int)m_PointTargetList.size();
		kPacket << nPointCnt;

		for ( int i=0; i<nPointCnt; ++i )
			kPacket << m_PointTargetList[i];

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void ioTargetCreateWeaponBuff2::CreateWeapon( ioBaseChar *pOwner )
{
	if( !pOwner || m_dwWeaponFireTime == 0 )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwWeaponFireTime > dwCurTime )
		return;

	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXQUATERNION vOwnerRot = pOwner->GetWorldOrientation();

	if ( !m_PointTargetList.empty() )
	{
		for ( int i=0; i<(int)m_PointTargetList.size(); ++i )
		{
			DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

			FireTime kFireTime;
			kFireTime.dwStart = dwCurTime;
			kFireTime.iAttributeIdx = m_nPointWeapon;
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;
			kFireTime.dwWeaponIdx = dwWeaponIndex;
			kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

			D3DXVECTOR3 vPos = vOwnerPos + vOwnerRot * m_vCreateWeaponOffset;
			D3DXVECTOR3 vDir = vOwnerRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( !m_PointTargetList[i].IsEmpty() )
			{
				ioTraceWeapon *pTraceWeapon = ToTraceWeapon( pWeapon );
				if ( pTraceWeapon )
					pTraceWeapon->SetTraceTarget( m_PointTargetList[i] );
			}
		}

		m_PointTargetList.clear();
	}
	else
	{
		DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;
		kFireTime.iAttributeIdx = m_nAimWeapon;
		kFireTime.iResistanceIdx = 0;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = false;
		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

		D3DXVECTOR3 vPos = vOwnerPos + vOwnerRot * m_vCreateWeaponOffset;
		D3DXVECTOR3 vArrivePos = vOwnerPos + vOwnerRot * m_vArriveWeaponOffset;
		D3DXVECTOR3 vDir = vArrivePos - vPos;
		D3DXVec3Normalize( &vDir, &vDir );

		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		if ( pWeapon )
		{
			pWeapon->SetPosition( vPos );
			pWeapon->SetMoveDir( vDir );
		}

		ioPlayStage *pStage = pOwner->GetCreator();
		if ( pStage )
		{
			float fMapHeight = pStage->GetMapHeightOnlyTerrain( vArrivePos.x, vArrivePos.z );
			if ( fMapHeight > 0.0f )
				vArrivePos.y = fMapHeight;
			
			if( pOwner->GetTeam() == TEAM_RED )
				pStage->CreateMapEffect( m_szWeaponRedEffect, vArrivePos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
			else if( pOwner->GetTeam() == TEAM_BLUE )
				pStage->CreateMapEffect( m_szWeaponBlueEffect, vArrivePos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
			else
			{
				if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
					pStage->CreateMapEffect( m_szWeaponBlueEffect, vArrivePos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				else
					pStage->CreateMapEffect( m_szWeaponRedEffect, vArrivePos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
			}
		}
	}

	m_nCurWeaponCnt++;
	m_iCurActiveCount = m_nTotalWeaponCnt - m_nCurWeaponCnt;
	m_dwWeaponFireTime = 0;
}

//////////////////////////////////////////////////////////////////////////////////
//카메라 설정
void ioTargetCreateWeaponBuff2::SetCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_szCameraBuff.IsEmpty() )
		//pOwner->AddNewBuff( m_szCameraBuff, "", "", NULL );
		pOwner->ReserveAddNewBuff( m_szCameraBuff, "", "", NULL );
}

void ioTargetCreateWeaponBuff2::EndCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->RemoveBuff( m_szCameraBuff );
}

void ioTargetCreateWeaponBuff2::SetCameraEvent( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_CameraEventMgr.CreateCE( m_dwCameraEvent, pStage );
}

void ioTargetCreateWeaponBuff2::EndCameraEvent( ioBaseChar *pOwner )
{
	ioCameraEvent* pCamera = g_CameraEventMgr.GetCurCameraEvent();
	if( pCamera && pCamera->GetCECode() == m_dwCameraEvent )
	{
		g_CameraEventMgr.NotifyCEDestroyed();
	}
}