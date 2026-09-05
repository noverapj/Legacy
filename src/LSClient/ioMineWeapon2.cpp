

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioMineWeapon2.h"

ActionStopType ioMineWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioMineWeapon2::m_DieActionStopType = AST_NONE;

ioMineWeapon2::ioMineWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MINE_EXPLOSION | WP_ATTACK_OWNER | WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pMine = NULL;

	m_dwSensingStartTime = 0;
	m_dwCurTraceDelayTime = 0;			// 대기시간 계산
	m_dwCurFlyTraceDealyTime = 0;

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
	
}

ioMineWeapon2::~ioMineWeapon2()
{
	if( m_pMine )
	{
		LOG.PrintTimeAndLog( 0, "ioMineWeapon::~ioMineWeapon - UnReleased Resource" );
	}
}

void ioMineWeapon2::LoadProperty( ioINILoader &rkLoader )
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
	m_dwFlyTraceDealyTime = (DWORD)rkLoader.LoadInt_e( "mine_fly_ready_delay", 300 );	
	m_fTraceReadySpeed = rkLoader.LoadFloat_e( "mine_fly_ready_speed", 0.0f );

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
}

void ioMineWeapon2::SetTeam( ioPlayStage *pStage, TeamType eType )
{
	if( !pStage ) return;

	ioWeapon::SetTeam( pStage, eType );

	DestroyResource();
	ioHashString szCurModel;

	if( m_bTeamModel )
	{
		ioBaseChar *pBaseOwner = pStage->GetOwnerChar();

		if( !pBaseOwner ) return;

		if( pBaseOwner->GetTeam() == eType )
		{
			if( !m_vTeamModelList.empty() )
			{
				int iRand = rand() % m_vTeamModelList.size();
				szCurModel = m_vTeamModelList[iRand];
			}
			else
			{
				szCurModel = m_BlueMineModel;
				m_CurTeamColor = TEAM_BLUE;
			}
		}
		else
		{
			if( !m_vEnemyModelList.empty() )
			{
				int iRand = rand() % m_vEnemyModelList.size();
				szCurModel = m_vEnemyModelList[iRand];
			}
			else
			{
				szCurModel = m_RedMineModel;
				m_CurTeamColor = TEAM_RED;
			}
		}
	}
	else
	{
		if( eType == TEAM_BLUE )
		{
			szCurModel = m_BlueMineModel;
			m_CurTeamColor = TEAM_BLUE;
		}
		else if( eType == TEAM_RED )
		{
			szCurModel = m_RedMineModel;
			m_CurTeamColor = TEAM_RED;
		}
		else //개인전
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			{
				szCurModel = m_BlueMineModel;
				m_CurTeamColor = TEAM_BLUE;
			}
			else
			{
				szCurModel = m_RedMineModel;
				m_CurTeamColor = TEAM_RED;
			}
		}
	}

	ioSceneNode *pSNode = GetParentSceneNode();
	m_pMine = pSNode->GetSceneMgr()->CreateEntityGrp();
	pSNode->AttachObject( m_pMine );
	m_pMine->LoadGroup( szCurModel.c_str() );

	pSNode->SetScale( D3DXVECTOR3(m_fMineSizeRate, m_fMineSizeRate, m_fMineSizeRate) );
	UpdateWorldCollisionBox();
}

void ioMineWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
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

void ioMineWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case MS_CHARGE:
		OnCharge( pStage );
		break;
	case MS_SENSING:
		OnSensing( pStage, fTimePerSec );
		break;
	case MS_WAIT_EXPLOSION:
		OnWaitExplosion( pStage );
		break;
	case MS_FLY_WAIT:
		{
			OnWaitFlyMine( pStage, fTimePerSec );

			ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwFireSprayEffect );
			if( pMapEffect )
			{
				D3DXVECTOR3 vPos = GetPosition();				
				D3DXQUATERNION quat;
				ioMath::CalcDirectionQuaternion( quat, m_vMoveDir );
			
				pMapEffect->SetWorldPosition( GetPosition() );
			}
		}

		break;
	case MS_FLY_TRACING:
		{		
			if( 0 < m_dwTraceEndTime && m_dwTraceEndTime < dwCurTime )
			{
				ExplosionMine( pStage, true );
			}
			else
			{
				OnFlyTracing( pStage, fTimePerSec );
			}

			ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwFireSprayEffect );
			if( pMapEffect )
			{
				pMapEffect->SetWorldPosition( GetPosition() );
				ioEffect* pEffect = pMapEffect->GetEffect();
				if( pEffect )
				{
					pEffect->GetParentSceneNode()->Rotate( m_vEffectDir, m_fGapAngle );
				}
			}
		}
		break;
	case MS_EXPLOSION:
		OnExplosion();
		break;
	case MS_NORMAL_DEAD:
		OnNormalDead();
		break;
	}

	UpdateWorldCollisionBox();
}

void ioMineWeapon2::OnCharge( ioPlayStage *pStage )
{
	if( m_dwSensingStartTime > FRAMEGETTIME() )
		return;

	m_State = MS_SENSING;
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

void ioMineWeapon2::OnSensing( ioPlayStage *pStage, float fTimePerSec )
{
	VisibleCheck( pStage );

	if( FindTarget( pStage, fTimePerSec ) )
	{	
		SetWaitFlyMine( pStage, true );
		return;			
	}

	// DropZone Check
	if( DropZoneCheck( pStage ) )
	{
		SetWaitExplosionMine( pStage, true );
	}

	// Mine Vs Mine Check
	if( pStage->CheckSensingMine2( this, m_fMineSensingRange ) )
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

bool ioMineWeapon2::FindTarget( ioPlayStage *pStage, float fTimePerSec )
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

void ioMineWeapon2::VisibleCheck( ioPlayStage *pStage )
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

bool ioMineWeapon2::DropZoneCheck( ioPlayStage *pStage )
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

void ioMineWeapon2::UpdateWorldCollisionBox()
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

void ioMineWeapon2::OnWaitExplosion( ioPlayStage *pStage )
{		
	if( m_dwCurTraceDelayTime + m_dwTraceDelayTime < FRAMEGETTIME() )
	{
		m_dwCurTraceDelayTime = 0;
		ExplosionMine( pStage, true );
	}
}

void ioMineWeapon2::SetWaitExplosionMine( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == MS_WAIT_EXPLOSION ||
		m_State == MS_EXPLOSION ||
		m_State == MS_NORMAL_DEAD )
		return;

	m_dwTraceDelayTime = FRAMEGETTIME();	// 대기 시간 셋팅
	m_State = MS_WAIT_EXPLOSION;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_MINE_WAIT_EXPLOSION );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMineWeapon2::SetWaitFlyMine( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == MS_WAIT_EXPLOSION ||
		m_State == MS_EXPLOSION ||
		m_State == MS_NORMAL_DEAD ||
		m_State == MS_FLY_WAIT )
		return;

	m_dwCurFlyTraceDealyTime = FRAMEGETTIME();	// 대기 시간 셋팅
	m_State = MS_FLY_WAIT;

	m_vEffectDir = m_vMoveDir;

	if( pStage )
	{
		pStage->CreateMapEffect( m_szTraceStartEffect, GetPosition(), GetScale() );		
	}

	if( pStage )
	{
		D3DXVECTOR3 vPos = GetPosition();

		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szFireSprayEffect, vPos, GetScale() );
		if( pMapEffect )
		{			
			m_dwFireSprayEffect = pMapEffect->GetUniqueID();
		}
	}

	ioBaseChar* pOwner = GetOwner();
	if( !m_szTraceStartSound.IsEmpty() && pOwner )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		m_dwTraceStartSound = g_SoundMgr.PlaySound( m_szTraceStartSound, pGrp->GetParentSceneNode() );			
	}

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_MINE_FLY_TRACE );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			kPacket << m_szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMineWeapon2::OnWaitFlyMine( ioPlayStage *pStage, float fTimePerSec )
{
	if( m_dwCurFlyTraceDealyTime + m_dwFlyTraceDealyTime < FRAMEGETTIME() )
	{
		m_dwCurFlyTraceDealyTime = 0;
		SetFlyTraceMine( pStage );
	}
	else
	{
		float fMoveDist = m_fTraceReadySpeed * fTimePerSec;
		D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
		GetParentSceneNode()->Translate( vDir * fMoveDist );
	}
}

void ioMineWeapon2::SetFlyTraceMine( ioPlayStage *pStage )
{
	m_State = MS_FLY_TRACING;
	m_dwTraceEndTime = FRAMEGETTIME() + m_dwTraceDuration;
}

void ioMineWeapon2::OnFlyTracing( ioPlayStage *pStage, float fTimePerSec )
{	
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		if( pTarget->IsSystemState() || !pTarget->IsEnableTarget() )
		{
			m_szTargetName.Clear();
		}

		m_vPrePos = GetPosition();
		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - m_vPrePos;

		float fRangeSq = D3DXVec3LengthSq( & vDiff );
		if( fRangeSq < m_fTraceRange * m_fTraceRange )
		{
			float fTimeGap = fTimePerSec * FLOAT1000;
			float fCurLoopTimeGap;
			float fCurLoopTimePerSec;
			while( fTimeGap > 0.0f )
			{
				if( fTimeGap > FLOAT10 )
				{
					fCurLoopTimeGap = FLOAT10;
					fCurLoopTimePerSec = FLOAT10 / FLOAT1000;

					fTimeGap -= FLOAT10;
				}
				else
				{
					fCurLoopTimeGap = fTimeGap;
					fCurLoopTimePerSec = fTimeGap / FLOAT1000;

					fTimeGap = 0.0f;
				}

				float fPosRate = m_fTargetTracePosRate;
				if( CheckDownTargetState( pTarget ) )
					fPosRate = m_fDownTargetTracePosRate;

				vDiff = pTarget->GetMidPositionByRate(fPosRate) - GetPosition();
				D3DXVec3Normalize( &vDiff, &vDiff );

				D3DXVECTOR3 vPreDir = m_vMoveDir;
				D3DXVECTOR3 vCurForce = m_vMoveDir * m_fTraceSpeed * fCurLoopTimePerSec;
				m_vMoveDir = vCurForce + (vDiff * m_fTraceRotSpeed * fCurLoopTimePerSec);

				D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

				float fCos = D3DXVec3Dot( &vPreDir, &m_vMoveDir );
				// 0도 일때
				if( fCos >= FLOAT1 )
				{	
					// 처리 필요없음
				}
				else if( fCos <= -FLOAT1 ) // 180도 일때
				{
					D3DXVECTOR3 vAxis1, vAxis2;
					if( m_vMoveDir != ioMath::UNIT_Y )
					{
						D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_Y );
						D3DXVec3Normalize( &vAxis1, &-vAxis1 );
					}
					else
					{
						D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_X );
						D3DXVec3Normalize( &vAxis1, &-vAxis1 );
					}

					D3DXVec3Cross( &vAxis2, &m_vMoveDir, &vAxis1 );
					D3DXVec3Normalize( &vAxis2, &-vAxis2 );

					GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
					m_vEffectDir = vAxis2;
					m_fGapAngle = FLOAT1;						
				}
				else // 그외
				{
					float fGapAngle = RADtoDEG( acos( fCos ) );
					m_fGapAngle = fGapAngle;

					D3DXVECTOR3 vAxis;
					D3DXVec3Cross( &vAxis, &vDiff, &m_vMoveDir );
					D3DXVec3Normalize( &vAxis, &-vAxis );

					GetParentSceneNode()->Rotate( vAxis, fGapAngle );						
					m_vEffectDir = vAxis;
					m_fGapAngle = fGapAngle;						
				}


			}
		}

		if( m_dwTraceEndTime < dwCurTime )
		{
			m_szTargetName.Clear();
		}
		
		if( m_fTraceEndRange > 0.0f )
		{
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			D3DXVECTOR3 vLength = vPos - GetPosition();
			float fLength = D3DXVec3Length( &vLength );
			if( fLength <= m_fTraceEndRange )
			{
				m_szTargetName.Clear();
			}
		}
	}
	else
	{
		m_szTargetName.Clear();
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	UpdateWorldCollisionBox();
}

void ioMineWeapon2::SetNormalDeadMine( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == MS_WAIT_EXPLOSION ||
		m_State == MS_EXPLOSION ||
		m_State == MS_NORMAL_DEAD )
		return;

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
			SP2Packet kPacket( CUPK_TRACE_MINE_NORMAL_DEAD );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMineWeapon2::ExplosionMine( ioPlayStage *pStage, bool bWait, bool bSendNetwork )
{
	if( m_State == MS_EXPLOSION )
		return;

	if( !IsCanExplosion() )
		return;

	m_bWait = bWait;

	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		pStage->CreateMapEffect( m_BoomEffect,
			GetPosition(),
			GetScale() );

		ioWeapon::PlayExplosionSound();
	}

	if( pStage )
	{
		if( m_dwFireSprayEffect != -1 )
		{			
			pStage->DestroyMapEffect( m_dwFireSprayEffect );
			m_dwFireSprayEffect = -1;
		}
	}

	m_State = MS_EXPLOSION;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_MINE_EXPLOSION );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMineWeapon2::NormalDeadMine( ioPlayStage *pStage, bool bWait )
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

void ioMineWeapon2::SetTraceTarget( ioHashString szTarget )
{
	m_szTargetName.Clear();
	m_szTargetName = szTarget;
}

void ioMineWeapon2::TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt )
{
	if( m_State == MS_SENSING || MS_CHARGE )
	{
		SetPosition( GetPosition() + vMoveAmt );
		m_iConveyerMoveFrameIdx = g_FrameTimer.GetFrameCounter();
	}
}

bool ioMineWeapon2::IsCanMineExplosion()
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

bool ioMineWeapon2::IsCanExplosion() const
{
	if( !IsLive() || m_State == MS_EXPLOSION )
		return false;

	if( m_State == MS_NORMAL_DEAD )
		return false;

	return true;
}

void ioMineWeapon2::OnExplosion()
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

	SetWeaponDead();
}

void ioMineWeapon2::OnNormalDead()
{
	if( m_bWait )
	{
		m_bWait = false;
		return;
	}

	SetWeaponDead();
}

void ioMineWeapon2::SetWeaponDead()
{
	ioPlayStage* pStage = g_WeaponMgr.GetCreator();
	if( pStage )
	{
		if( m_dwFireSprayEffect != -1 )
		{
			pStage->DestroyMapEffect( m_dwFireSprayEffect );
			m_dwFireSprayEffect = -1;
		}
	}
	
	ioWeapon::SetWeaponDead();
}

void ioMineWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	if( m_vMoveDir != ioMath::VEC3_ZERO )
		return;

	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioMineWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMine )
	{
		GetParentSceneNode()->GetSceneMgr()->DestroyEntityGrp( m_pMine );
		m_pMine = NULL;
	}
}

const ioOrientBox& ioMineWeapon2::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioMineWeapon2::GetType() const
{
	return WT_MINE2;
}

ActionStopType ioMineWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioMineWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioMineWeapon2::IsCanVisible( ioWeapon *pWeapon ) const
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

bool ioMineWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State == MS_SENSING || m_State == MS_FLY_WAIT )
	{
		if( ToSymbolStruct( pEntity ) )
			return false;
	}
	
	if( m_State == MS_EXPLOSION )
	{
		float fTargetDist = -FLOAT1;
		D3DXVECTOR3 vCenter = m_WorldCollisionBox.GetCenter();
		if( !pEntity->CheckSphereDistance( vCenter, m_fExplosionRange, fTargetDist ) )
			return false;

		if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
			return true;
	}
	else
	{
		if( ToBaseChar( pEntity ) == GetOwner() )
			return false;

		float fTargetDist = -FLOAT1;
		D3DXVECTOR3 vCenter = m_WorldCollisionBox.GetCenter();
		if( !pEntity->CheckSphereDistance( vCenter, m_fCollisionRange, fTargetDist ) )
		{
			return false;
		}

		if( COMPARE( fTargetDist, 0.0f, m_fCollisionRange ) )
		{	
			ioPlayStage * pStage = g_WeaponMgr.GetCreator();
			if( pStage )
			{
				ExplosionMine( pStage, true );
			}

			return true;
		}
	}

	return false;
}

bool ioMineWeapon2::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State != MS_FLY_TRACING )
		return true;

	int iFaceIndex = 0;
	if( !pShape->FindIntersection( pShapeWorldMat, m_WorldCollisionBox, &iFaceIndex ) )
		return false;

	ExplosionMine( pStage, true );
	return true;
}

DWORD ioMineWeapon2::GetSensingStartGapTime()
{
	if( m_dwSensingStartTime != 0 )
		return FRAMEGETTIME() - m_dwSensingStartTime;

	return 0;
}

const BuffInfoList& ioMineWeapon2::GetTargetAirBuffList() const
{
	if( !m_bUseTeamTargetBuff )
		return GetAttribute()->m_TargetAirBuffList;

	if( m_CurTeamColor == TEAM_BLUE )
		return GetAttribute()->m_TargetAirBuffList;

	return m_RedTargetAirBuffList;
}

const BuffInfoList& ioMineWeapon2::GetTargetBuffList() const
{
	if( !m_bUseTeamTargetBuff )
		return GetAttribute()->m_TargetBuffList;

	if( m_CurTeamColor == TEAM_BLUE )
		return GetAttribute()->m_TargetBuffList;

	return m_RedTargetBuffList;
}

const BuffInfoList& ioMineWeapon2::GetTargetDownBuffList() const
{
	if( !m_bUseTeamTargetBuff )
		return GetAttribute()->m_TargetDownBuffList;

	if( m_CurTeamColor == TEAM_BLUE )
		return GetAttribute()->m_TargetDownBuffList;

	return m_RedTargetDownBuffList;
}

bool ioMineWeapon2::CheckDownTargetState( ioBaseChar *pTarget )
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

bool ioMineWeapon2::CheckTargetWoundedState( ioBaseChar *pTarget )
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

