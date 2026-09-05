

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioMineWeapon3.h"

ActionStopType ioMineWeapon3::m_ActionStopType = AST_NONE;
ActionStopType ioMineWeapon3::m_DieActionStopType = AST_NONE;

ioMineWeapon3::ioMineWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MINE_EXPLOSION | WP_ATTACK_OWNER | WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pMine = NULL;

	m_dwSensingStartTime = 0;
	m_dwCurTraceDelayTime = 0;			// 대기시간 계산
//	m_dwCurFlyTraceDealyTime = 0;

	m_State = MS_CHARGE;
	m_bMineVisible = false;

	m_iConveyerMoveFrameIdx = -1;
	m_bResistance = false;

	m_bWait = false;
	m_CurTeamColor = TEAM_BLUE;

	m_szTargetName.Clear();

	m_vMoveDir = ioMath::VEC3_ZERO;
	m_fGapAngle = 0.0f;

	m_dwTraceStartSound = 0;

	m_dwRemainDelayTime = 0;
	m_dwZoneEffectID = -1 ;

	m_nCurAttackCnt = 0;
	m_dwAttackChargeStartTime = 0;
}

ioMineWeapon3::~ioMineWeapon3()
{
	if( m_pMine )
	{
		LOG.PrintTimeAndLog( 0, "ioMineWeapon3::~ioMineWeapon3 - UnReleased Resource" );
	}
}

void ioMineWeapon3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "blue_mine_model", "", szBuf, MAX_PATH );
	m_BlueMineModel = szBuf;
	rkLoader.LoadString_e( "red_mine_model", "", szBuf, MAX_PATH );
	m_RedMineModel = szBuf;
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;
	rkLoader.LoadString_e( "normal_dead_effect", "", szBuf, MAX_PATH );
	m_NormalDeadEffect = szBuf;

	m_bTeamModel = rkLoader.LoadBool_e( "use_team_mine_model", false );

	m_vTeamModelList.clear();
	m_vEnemyModelList.clear();

	int iTeamCnt = rkLoader.LoadInt_e( "team_mine_model_cnt", 0 );
	int iEnemyCnt = rkLoader.LoadInt_e( "enemy_mine_model_cnt", 0 );

	if( iTeamCnt > 0 )
		m_vTeamModelList.reserve( iTeamCnt );

	if( iEnemyCnt > 0 )
		m_vEnemyModelList.reserve( iEnemyCnt );

	if( m_bTeamModel )
	{
		int i = 0;
		for( i=0; i < iTeamCnt; ++i )
		{
			wsprintf_e( szKey, "team_mine_model%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_vTeamModelList.push_back( ioHashString( szBuf ) );
		}

		for( i=0; i < iEnemyCnt; ++i )
		{
			wsprintf_e( szKey, "enemy_mine_model%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_vEnemyModelList.push_back( ioHashString( szBuf ) );
		}
	}

	m_fCollisionRange = rkLoader.LoadFloat_e( "collsion_range", 300.0f );
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 300.0f );
	m_fMineBoomDist = rkLoader.LoadFloat_e( "mine_boom_dist", 300.0f );
	m_dwTraceDelayTime = (DWORD)rkLoader.LoadInt_e( "mine_trace_delay", 300 );
	m_fMineSizeRate = rkLoader.LoadFloat_e( "mine_size_rate", FLOAT1 );
	m_fVisibleRange = rkLoader.LoadFloat_e( "mine_visible_range", 0.0f );
	m_fMineSensingRange = rkLoader.LoadFloat_e( "mine_sensing_range", 0.0f );

	//Fly Trace
	//m_dwFlyTraceDealyTime = rkLoader.LoadFloat_e( "mine_fly_ready_delay", 300 );	
	//m_fTraceReadySpeed = rkLoader.LoadFloat_e( "mine_fly_ready_speed", 0.0f );

	m_fTraceSpeed = rkLoader.LoadFloat_e( "trace_speed", 0.0f );
	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceRotSpeed = rkLoader.LoadFloat_e( "trace_rotate_speed", 0.0f );
	m_dwTraceDuration = (DWORD)rkLoader.LoadInt_e( "trace_duration", 0 );
	m_fTraceEndRange = rkLoader.LoadFloat_e( "trace_end_range", 0.0f );
	
	m_fTargetTracePosRate = rkLoader.LoadFloat_e( "target_trace_pos_rate", FLOAT05 );
	m_fDownTargetTracePosRate = rkLoader.LoadFloat_e( "down_target_trace_pos_rate", 0.0f );
	
	m_fPreUpHeight =rkLoader.LoadFloat_e("trace_pre_up_height", 50.0f );

	// Extend
	m_MineType = (MineType)rkLoader.LoadInt_e( "mine_type", MIT_MINE );
	m_bNoMineExplosion = rkLoader.LoadBool_e( "no_explosion_mine", false );
	m_bNoSencingMine = rkLoader.LoadBool_e( "no_sensing_mine", false );
	m_bNoMineUp = rkLoader.LoadBool_e( "no_mine_up", false );
	m_bUseTeamTargetBuff = rkLoader.LoadBool_e( "use_team_target_buff", false );
	m_bNormalDead = rkLoader.LoadBool_e( "use_normal_dead", false );
	
	rkLoader.LoadString_e( "trace_start_effect", "", szBuf, MAX_PATH );
	m_szTraceStartEffect = szBuf;

	rkLoader.LoadString_e( "trace_spray_effect", "", szBuf, MAX_PATH );
	m_szFireSprayEffect = szBuf;

	rkLoader.LoadString_e( "trace_start_sound", "", szBuf, MAX_PATH );
	m_szTraceStartSound = szBuf;

	//Buff List
	// Air
	int iAirCount = rkLoader.LoadInt_e( "red_target_air_buff_cnt", 0 );
	m_RedTargetAirBuffList.reserve( iAirCount );

	for( int i=0; i<iAirCount; i++ )
	{
		wsprintf_e( szKey, "red_target_air_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_RedTargetAirBuffList.push_back( tBuffInfo );
			}
		}
	}

	// Normal
	int iCount = rkLoader.LoadInt_e( "red_target_buff_cnt", 0 );
	m_RedTargetBuffList.reserve( iCount );

	for( int i=0; i<iCount; i++ )
	{
		wsprintf_e( szKey, "red_target_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_RedTargetBuffList.push_back( tBuffInfo );
			}
		}
	}

	// Down
	int iDownCount = rkLoader.LoadInt_e( "red_target_down_buff_cnt", 0 );
	m_RedTargetDownBuffList.reserve( iDownCount );

	for( int i=0; i<iDownCount; i++ )
	{
		wsprintf_e( szKey, "red_target_down_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_RedTargetDownBuffList.push_back( tBuffInfo );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	m_nMaxAttackCnt = rkLoader.LoadInt_e( "max_attack_cnt", 1 );
	m_dwMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "max_delay_time", 0 );
	m_dwMaxAttackChargeTime = (DWORD)rkLoader.LoadInt_e( "max_attack_charge_time", 0 );

	rkLoader.LoadString_e( "mine_effect_red", "", szBuf, MAX_PATH );
	m_stZoneEffect_Red = szBuf;
	rkLoader.LoadString_e( "mine_effect_blue", "", szBuf, MAX_PATH );
	m_stZoneEffect_Blue = szBuf;

	m_fOffSetY = rkLoader.LoadFloat_e( "effect_off_y", 0.f );

	m_CallWeaponInfoList.clear();
	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	m_CallWeaponInfoList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kCallAttribute );
	}

	m_dwMaxLifeTime = (DWORD)rkLoader.LoadInt_e( "max_life_time", 0 );
}

void ioMineWeapon3::SetTeam( ioPlayStage *pStage, TeamType eType )
{
	if( !pStage ) return;

	ioWeapon::SetTeam( pStage, eType );
}

void ioMineWeapon3::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( !pStage )
	{
		SetPosition( vPos );
		return;
	}

	D3DXVECTOR3 vMinePos;
	vMinePos.x = vPos.x;
	vMinePos.y = pStage->GetMapHeight( vPos.x, vPos.z, NULL, true, vPos.y );
	vMinePos.z = vPos.z;

	SetPosition( vMinePos );

	m_vMoveDir = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
}

void ioMineWeapon3::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case MS_CHARGE:
		OnCharge( pStage );
		break;
	case MS_DELAY:
		OnDelay(pStage);
		break;
	case MS_SENSING:
		OnSensing( pStage, fTimePerSec );
		break;
	case MS_WAIT_EXPLOSION:
		OnWaitExplosion( pStage );
		break;
	case MS_EXPLOSION:
		OnExplosion( pStage );
		break;
	case MS_NORMAL_DEAD:
		OnNormalDead();
		break;
	}

	if( (m_dwCreatedTime + m_dwMaxLifeTime) < dwCurTime )
	{
		if(	GetOwner() && GetOwner()->IsNeedProcess() && (m_State == MS_DELAY || m_State == MS_SENSING) )
			SetNormalDeadMine( pStage, true );
	}

	UpdateWorldCollisionBox();
}

void ioMineWeapon3::OnCharge( ioPlayStage *pStage )
{
	if( m_dwSensingStartTime > FRAMEGETTIME() )
		return;

	//m_State = MS_SENSING;

	SetDelay(pStage, true, false);
	/*
	// 폭발, 생성 신호가 역전된 지뢰인지를 체크함, 역전되었으면 바로 폭발..~
	if( g_WeaponMgr.CheckMissingIngMineIndex( m_iWeaponIdx ) )
	{
		SetWaitExplosionMine( pStage, false );
	}
	else
	{		
		m_State = MS_SENSING;
	}
	*/
}


void ioMineWeapon3::OnDelay( ioPlayStage *pStage )
{
	DWORD dwCurtime = FRAMEGETTIME();
	
	if( m_dwDelayStartTime + m_dwMaxDelayTime <= dwCurtime )
	{
		m_dwRemainDelayTime = 0;
		m_State = MS_SENSING;
	}
	else
	{
		m_dwRemainDelayTime = (m_dwDelayStartTime + m_dwMaxDelayTime) - dwCurtime;
	}
}


void ioMineWeapon3::OnSensing( ioPlayStage *pStage, float fTimePerSec )
{
	VisibleCheck( pStage );

	if( FindTarget( pStage, fTimePerSec ) )
	{	
		//SetWaitFlyMine( pStage, true );
		SetWaitExplosionMine( pStage, true );
		return;			
	}

	// DropZone Check
	if( DropZoneCheck( pStage ) )
	{
		SetWaitExplosionMine( pStage, true );
	}

	// Mine Vs Mine Check
	if( pStage->CheckSensingMine3( this, m_fMineSensingRange ) )
	{
		SetWaitExplosionMine( pStage, true );
	}
	else
	{
		bool bCheckMoving = true;
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			bCheckMoving = IsGameOwnerWeapon();
		}

		if( bCheckMoving && m_iConveyerMoveFrameIdx > 0 )
		{
			if( m_iConveyerMoveFrameIdx + 1 < (int)g_FrameTimer.GetFrameCounter() )
			{
				SetWaitExplosionMine( pStage, true );
			}
		}
	}
}

bool ioMineWeapon3::FindTarget( ioPlayStage *pStage, float fTimePerSec )
{
	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetPosition();
	float fSensingRangeSq = m_fMineBoomDist * m_fMineBoomDist;

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	if( !IsExperienceOwner() )
	{
		for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
		{
			ioBaseChar *pChar = *iter;
			if( !pChar->IsNeedProcess() ) 
				continue;

			if( !CheckTargetWoundedState( pChar ) ) 
				continue;

			if( pChar->GetTeam() == GetTeam() ) 
				continue;

			if( pChar->HasBuff(BT_CHANGE_TEAM) )
				continue;

			if( pChar->IsSystemState() )
				continue;
				
			if( !pChar->IsEnableTarget() )
				continue;


			if( Help::CheckMineNoCollision( pChar->GetState() ) )
				continue;

			vDiff = vCurPos - pChar->GetWorldPosition();
			if( D3DXVec3LengthSq( &vDiff ) < fSensingRangeSq )
			{				
				m_szTargetName = pChar->GetCharName();
				return true;
			}
		}
	}

	return false;
}

void ioMineWeapon3::VisibleCheck( ioPlayStage *pStage )
{
	if( !m_pMine )	return;

	ioBaseChar *pTarget = pStage->GetCameraTargetChar();
	if( pTarget )
	{
		if( GetTeam() == pTarget->GetTeam() || pTarget->HasBuff(BT_TRAP_EYE_BUFF) )
		{
			m_pMine->SetVisible( true );
		}
		else
		{
			float fVisibleRangeSq = m_fVisibleRange * m_fVisibleRange;

			D3DXVECTOR3 vDiff = GetPosition() - pTarget->GetWorldPosition();
			vDiff.y = 0.0f;

			if( D3DXVec3LengthSq( &vDiff ) < fVisibleRangeSq )
				m_bMineVisible = true;
			else
				m_bMineVisible = false;

			m_pMine->SetVisible( m_bMineVisible );
		}
	}
	else
	{
		m_pMine->SetVisible( false );
	}
}

bool ioMineWeapon3::DropZoneCheck( ioPlayStage *pStage )
{
	D3DXVECTOR3 vCurPos = GetPosition();
	float f_MapHeight = pStage->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z );

	if ( f_MapHeight > 0.0f )
	{
		return false;
	}

	f_MapHeight = pStage->GetMapHeightOnlyTerrain( vCurPos.x, vCurPos.z );
	if( fabs(f_MapHeight - vCurPos.y) >= FLOAT10 )
		return true;

	return false;
}

void ioMineWeapon3::UpdateWorldCollisionBox()
{
	if( m_pMine )
	{
		ioSceneNode *pSNode = GetParentSceneNode();
		if( pSNode )
		{
			m_WorldCollisionBox = m_pMine->GetCollisionBox();
			m_WorldCollisionBox.Transform( pSNode->GetDerivedPosition(),
				pSNode->GetDerivedScale(),
				pSNode->GetDerivedOrientation() );

		}
	}
}

void ioMineWeapon3::OnWaitExplosion( ioPlayStage *pStage )
{		
	if( m_dwCurTraceDelayTime + m_dwTraceDelayTime < FRAMEGETTIME() )
	{
		m_dwCurTraceDelayTime = 0;
		ExplosionMine( pStage, true );
	}
}

void ioMineWeapon3::SetWaitExplosionMine( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == MS_WAIT_EXPLOSION ||
		m_State == MS_EXPLOSION ||
		m_State == MS_NORMAL_DEAD )
		return;

	m_nCurAttackCnt++;

	m_dwCurTraceDelayTime = FRAMEGETTIME();	// 대기 시간 셋팅
	m_State = MS_WAIT_EXPLOSION;

	//대기 사간동안 적이 이지 할 수 있는 이펙트를 뿌린다.
	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = GetPosition();
		vPos.y += m_fOffSetY;
		pStage->CreateMapEffect( m_BoomEffect,
			vPos,
			GetScale() );

		ioWeapon::PlayExplosionSound();
	}

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE2_MINE_WAIT_EXPLOSION );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioMineWeapon3::SetDelay( ioPlayStage *pStage, bool bFirst, bool bSendNetwork )
{
	if( bFirst && m_State == MS_SENSING )
		return;

	if( !bFirst && m_State == MS_DELAY )
		return;

	ioBaseChar *pBaseOwner = pStage->GetOwnerChar();

	if( !pBaseOwner )
		return;

	if( m_dwZoneEffectID == -1 )
	{
		D3DXVECTOR3 vPos = GetPosition();

		vPos.y += m_fOffSetY;

		ioMapEffect* pMapEffect = NULL;


		if( GetOwner() && pBaseOwner->GetTeam() == GetTeam() )
		{
			if( GetOwner()->GetTeam() == TEAM_RED )
				pMapEffect = pStage->CreateMapEffect( m_stZoneEffect_Red, vPos, GetScale() );
			else
				pMapEffect = pStage->CreateMapEffect( m_stZoneEffect_Blue, vPos, GetScale() );
		}

		if( pMapEffect )
			m_dwZoneEffectID = pMapEffect->GetUniqueID();
	}

	if( bFirst )
	{
		m_State = MS_SENSING;
		return;
	}

	m_dwDelayStartTime = FRAMEGETTIME();
	m_State = MS_DELAY;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE2_MINE_DELAY );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMineWeapon3::SetNormalDeadMine( ioPlayStage *pStage, bool bSendNetwork )
{
	/*if( m_State == MS_WAIT_EXPLOSION ||
		m_State == MS_EXPLOSION ||
		m_State == MS_NORMAL_DEAD )
		return;*/

	UpdateWorldAttackBox( 0.0f, true );
	if( !m_NormalDeadEffect.IsEmpty() )
	{
		pStage->CreateMapEffect( m_NormalDeadEffect,
			GetPosition(),
			GetScale() );

		ioWeapon::PlayExplosionSound();
	}

	m_State = MS_NORMAL_DEAD;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE2_MINE_NORMAL_DEAD );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMineWeapon3::ExplosionMine( ioPlayStage *pStage, bool bWait, bool bSendNetwork )
{
	if( m_State == MS_EXPLOSION )
		return;

	if( !IsCanExplosion() )
		return;

	m_bWait = bWait;

	m_State = MS_EXPLOSION;

	CallAttribute();

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE2_MINE_EXPLOSION );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMineWeapon3::NormalDeadMine( ioPlayStage *pStage, bool bWait )
{
	if( !IsCanExplosion() )	return;

	m_bWait = bWait;

	UpdateWorldAttackBox( 0.0f, true );

	if( !m_NormalDeadEffect.IsEmpty() )
	{
		pStage->CreateMapEffect( m_NormalDeadEffect,
			GetPosition(),
			GetScale() );

		ioWeapon::PlayExplosionSound();
	}

	m_State = MS_NORMAL_DEAD;
}

void ioMineWeapon3::SetTraceTarget( ioHashString szTarget )
{
	m_szTargetName.Clear();
	m_szTargetName = szTarget;
}

void ioMineWeapon3::TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt )
{
	if( m_State == MS_SENSING || MS_CHARGE )
	{
		SetPosition( GetPosition() + vMoveAmt );
		m_iConveyerMoveFrameIdx = g_FrameTimer.GetFrameCounter();
	}
}

bool ioMineWeapon3::IsCanMineExplosion()
{
	if( !IsLive() )	return false;
	if( m_bNoMineExplosion ) return false;
	if( !HasPeculiarity( WP_MINE_EXPLOSION ) )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( m_State != MS_EXPLOSION )
		return false;

	return true;
}

bool ioMineWeapon3::IsCanExplosion() const
{
	if( !IsLive() || m_State == MS_EXPLOSION )
		return false;

	if( m_State == MS_NORMAL_DEAD )
		return false;

	return true;
}

void ioMineWeapon3::OnExplosion( ioPlayStage *pStage )
{
	if( m_bWait )
	{
		m_bWait = false;
		return;
	}
	
	if( !m_szTraceStartSound.IsEmpty() )
	{			
		g_SoundMgr.StopSound( m_szTraceStartSound, m_dwTraceStartSound );	
	}

	if( m_nCurAttackCnt >= m_nMaxAttackCnt )
		SetWeaponDead();
	else
		SetDelay( pStage, false, true );
}

void ioMineWeapon3::OnNormalDead()
{
	if( m_bWait )
	{
		m_bWait = false;
		return;
	}

	SetWeaponDead();
}

void ioMineWeapon3::SetWeaponDead()
{
	ioPlayStage* pStage = g_WeaponMgr.GetCreator();
	if( pStage )
	{
		if( m_dwZoneEffectID != -1 )
		{
			pStage->DestroyMapEffect( m_dwZoneEffectID );
			m_dwZoneEffectID = -1;
		}
	}
	
	ioWeapon::SetWeaponDead();
}

void ioMineWeapon3::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	if( m_vMoveDir != ioMath::VEC3_ZERO )
		return;

	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioMineWeapon3::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMine )
	{
		GetParentSceneNode()->GetSceneMgr()->DestroyEntityGrp( m_pMine );
		m_pMine = NULL;
	}

	if( m_dwZoneEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_dwZoneEffectID );
		m_dwZoneEffectID = -1;
	}
}

const ioOrientBox& ioMineWeapon3::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioMineWeapon3::GetType() const
{
	return WT_MINE3;
}

ActionStopType ioMineWeapon3::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioMineWeapon3::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioMineWeapon3::IsCanVisible( ioWeapon *pWeapon ) const
{
	ioBaseChar *pChar = pWeapon->GetOwner();
	if( !pChar ) return false;

	D3DXVECTOR3 vDiff;
	vDiff = GetPosition() - pChar->GetWorldPosition();
	vDiff.y = 0.0f;

	float fVisibleRangeSq = m_fVisibleRange * m_fVisibleRange;

	if( pChar->IsEntityCollisionSkipState( NULL ) || pChar->IsFloating() )
		return false;

	if( pChar->GetTeam() == GetTeam() )
		return true;

	if( D3DXVec3LengthSq( &vDiff ) < fVisibleRangeSq )
		return true;

	return false;
}

bool ioMineWeapon3::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	return false;
}


DWORD ioMineWeapon3::GetSensingStartGapTime()
{
	if( m_dwSensingStartTime != 0 )
		return FRAMEGETTIME() - m_dwSensingStartTime;

	return 0;
}

const BuffInfoList& ioMineWeapon3::GetTargetAirBuffList() const
{
	if( !m_bUseTeamTargetBuff )
		return GetAttribute()->m_TargetAirBuffList;

	if( m_CurTeamColor == TEAM_BLUE )
		return GetAttribute()->m_TargetAirBuffList;

	return m_RedTargetAirBuffList;
}

const BuffInfoList& ioMineWeapon3::GetTargetBuffList() const
{
	if( !m_bUseTeamTargetBuff )
		return GetAttribute()->m_TargetBuffList;

	if( m_CurTeamColor == TEAM_BLUE )
		return GetAttribute()->m_TargetBuffList;

	return m_RedTargetBuffList;
}

const BuffInfoList& ioMineWeapon3::GetTargetDownBuffList() const
{
	if( !m_bUseTeamTargetBuff )
		return GetAttribute()->m_TargetDownBuffList;

	if( m_CurTeamColor == TEAM_BLUE )
		return GetAttribute()->m_TargetDownBuffList;

	return m_RedTargetDownBuffList;
}

bool ioMineWeapon3::CheckDownTargetState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() )
		return true;

	return false;
}

bool ioMineWeapon3::CheckTargetWoundedState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( (eState == CS_BLOW_WOUNDED && !pTarget->IsFloating()) ||
		(pTarget->GetProtectionMode() != PM_NONE &&
		 pTarget->GetProtectionMode() != PM_BOSS) )	
	{
		return false;
	}
	else if( (eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloating()) &&
			 (pTarget->GetProtectionMode() != PM_NONE &&
			  pTarget->GetProtectionMode() != PM_BOSS))
	{
		return false;
	}
	else if( pTarget->HasBuff( BT_SKIP_STUN ) )
	{
		return false;
	}
		
	return true;
}


void ioMineWeapon3::CallAttribute()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;	

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurGenerateTime = dwCurTime;

	WeaponInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(),
			pInfo,
			GetPosition(),
			D3DXVECTOR3( 0.f, FLOAT1, 0.f),
			dwCurGenerateTime + pInfo.m_dwCallTime,
			m_dwWeaponIdx );

	}
}