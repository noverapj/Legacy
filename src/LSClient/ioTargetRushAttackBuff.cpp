#include "StdAfx.h"
#include "ioTargetRushAttackBuff.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "FindPredicateImpl.h"

ioTargetRushAttackBuff::ioTargetRushAttackBuff(void)
{
	Init();
}

ioTargetRushAttackBuff::ioTargetRushAttackBuff( const ioTargetRushAttackBuff &rhs ):
ioBuff( rhs ),

m_RemoveBuffList( rhs.m_RemoveBuffList ),

m_fMaxMoveDist( rhs.m_fMaxMoveDist ),
m_fRunSpeed( rhs.m_fRunSpeed ),

m_fMarkerMinRange( rhs.m_fMarkerMinRange ),
m_fMarkerMaxRange( rhs.m_fMarkerMaxRange ),
m_fMarkerAngle( rhs.m_fMarkerAngle ),
m_bEnableDownAim( rhs.m_bEnableDownAim ),
m_szBlueMarker( rhs.m_szBlueMarker ),
m_szRedMarker( rhs.m_szRedMarker ),

m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),

m_fJumpPower( rhs.m_fJumpPower ),

m_szMoveAttackMotion( rhs.m_szMoveAttackMotion ),
m_fMoveAttackMotionRate( rhs.m_fMoveAttackMotionRate ),

m_fReduceGauge( rhs.m_fReduceGauge ),
m_iAttackMaxCount( rhs.m_iAttackMaxCount ),

m_vWeaponInfoList( rhs.m_vWeaponInfoList ),

m_szMoveEndMotion( rhs.m_szMoveEndMotion ),
m_fMoveEndMotionRate( rhs.m_fMoveEndMotionRate ),
m_fMoveEndAttackMoveSpeed( rhs.m_fMoveEndAttackMoveSpeed ),
m_dwMoveEndAttackLiveTime( rhs.m_dwMoveEndAttackLiveTime ),

m_dwAttackReserveTime( rhs.m_dwAttackReserveTime ),
m_dwAttackCancelReserveTime( rhs.m_dwAttackCancelReserveTime ),
m_EnableInputKeyState( rhs.m_EnableInputKeyState ),
m_BuffEndState( rhs.m_BuffEndState )
{
	Init();
}


ioTargetRushAttackBuff::~ioTargetRushAttackBuff(void)
{
	m_EnableInputKeyState.clear();
	m_BuffEndState.clear();
}

ioBuff* ioTargetRushAttackBuff::Clone()
{
	return new ioTargetRushAttackBuff( *this );
}

void ioTargetRushAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	rkLoader.LoadString_e( "blue_marker", "", szBuf, MAX_PATH );
	m_szBlueMarker = szBuf;
	rkLoader.LoadString_e( "red_marker", "", szBuf, MAX_PATH );
	m_szRedMarker = szBuf;

	m_fMaxMoveDist = rkLoader.LoadFloat_e( "move_range", 0.0f );
	m_fRunSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_fMarkerMinRange = rkLoader.LoadFloat_e( "maker_min_range", 0.0f );
	m_fMarkerMaxRange = rkLoader.LoadFloat_e( "maker_max_range", 0.0f );
	m_fMarkerAngle = rkLoader.LoadFloat_e( "marker_angle", 0.0f );
	m_bEnableDownAim = rkLoader.LoadBool_e( "marker_enable_down_aim", false);

	//스킬 사용 가능 관련.
	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );

	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	char szRemoveBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szRemoveBuf, "remove_buff%d_name",i+1);
		rkLoader.LoadString( szRemoveBuf, "", szBuf, MAX_PATH );
		m_RemoveBuffList.push_back( szBuf );
	}

	rkLoader.LoadString_e( "move_attack_motion", "", szBuf, MAX_PATH );
	m_szMoveAttackMotion = szBuf;
	m_fMoveAttackMotionRate = rkLoader.LoadFloat_e( "move_attack_motion_rate", 0.0f );

	m_fReduceGauge = rkLoader.LoadFloat_e( "reduce_gauge", 0.0f );

	m_iAttackMaxCount = rkLoader.LoadInt_e( "attack_count", 0 );

	

	int iAttackTypeCnt = rkLoader.LoadInt_e( "move_attack_type_count", 0 );
	m_vWeaponInfoList.clear();
	m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "move_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "move_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "move_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "move_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "move_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vWeaponInfoList.push_back( kInfo );
	}

	rkLoader.LoadString_e( "move_end_motion", "", szBuf, MAX_PATH );
	m_szMoveEndMotion = szBuf;
	m_fMoveEndMotionRate = rkLoader.LoadFloat_e( "move_end_motion_rate", 0.0f );

	m_fMoveEndAttackMoveSpeed = rkLoader.LoadFloat_e( "move_end_attack_move_speed", 0.0f );
	m_dwMoveEndAttackLiveTime = (DWORD)rkLoader.LoadInt_e( "move_end_attack_live_time", 0 );

	m_dwAttackReserveTime = (DWORD)rkLoader.LoadInt_e( "attack_reserve_time", 0 ); 
	m_dwAttackCancelReserveTime  = (DWORD)rkLoader.LoadInt_e( "attack_cancel_reserve_time", 0 ); 

	m_EnableInputKeyState.clear();
	int nMaxState = rkLoader.LoadInt_e( "max_enable_input_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szKey, "enable_input_state%d", i+1 );
		int nState = rkLoader.LoadInt( szKey, -1 );
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

void ioTargetRushAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )	return;
	ioBuff::StartBuff( pOwner );
	Init();

	if ( pOwner->IsNeedProcess() )
		CreateMarker( pOwner );

	SetSerachState( pOwner );

	m_bStart = false;
}

bool ioTargetRushAttackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if ( !pOwner) return false;

	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	Init();

	return true;
}

void ioTargetRushAttackBuff::EndBuff()
{
	if ( !m_pOwner )	return;
	
	ioBuff::EndBuff();

	if( m_pOwner->IsNeedProcess() )
	{
		// set marker none...
		m_pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );

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

	//버프 제거
	for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
	{
		m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
	}
	m_RemoveBuffList.clear();	
}

void ioTargetRushAttackBuff::Init()
{
	m_fCurMoveDist = 0.0f;
	m_bWound = false;
	m_szTargetName.Clear();
	m_vTargetDir = ioMath::VEC3_ZERO;
	m_vAttackDir = ioMath::VEC3_ZERO;
	m_iAttackCurCount = 0;
	m_dwFireWeaponIndex = 0;
	m_dwWeaponBaseIndex = 0;
	m_BuffState = BS_NONE;
	m_dwFireTime = 0;
	m_bShootWeapon = false;
	m_dwEnableAttackTime = 0;
	m_dwEnableAttackCancelTime = 0;
	m_vTargetInfoList.clear();
	m_bChangeChar = false;
	m_bStart = true;
}

//마커 생성
void ioTargetRushAttackBuff::CreateMarker( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioHashString szMarker;
	szMarker.Clear();

	if( pOwner->GetTeam() == TEAM_RED )
		szMarker = m_szRedMarker;
	else if( pOwner->GetTeam() == TEAM_BLUE )
		szMarker = m_szBlueMarker;
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			szMarker = m_szBlueMarker;
		else
			szMarker = m_szRedMarker;
	}


	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
		szMarker,
		"",
		szMarker,
		"",
		"",
		TT_OTHER );
	pOwner->SetTargetMarkerRange( m_fMarkerMaxRange, TT_OTHER );
	m_szTargetName.Clear();
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
	// 빈타겟 설정...
	pOwner->SetTargetMarkerTarget( m_szTargetName, false, TT_OTHER );
}

void ioTargetRushAttackBuff::ProcessBuff( float fTimePerSec )
{
	CheckCallingProcess();
	CheckCallingAreaWeapon();

	switch( m_BuffState )
	{
	case BS_SERACH:
		//타겟 변경
		if ( m_pOwner->IsNeedProcess() && CheckEnableKey( m_pOwner ) )
		{
			if ( m_dwEnableAttackTime < FRAMEGETTIME() && m_pOwner->IsAttackKey() )
			{
				SetMoveAttackState( m_pOwner );
				if ( m_pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
					kPacket << m_pOwner->GetCharName();
					kPacket << GetName();
					kPacket << BS_ACTION;
					kPacket << m_szTargetName;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				break;
			}
			else if ( m_pOwner->IsDefenseKey() )
				m_bChangeChar = true;
		}

		PorcessTargetFindState( m_pOwner );
		
		break;
	case BS_ACTION:
		{
			ProcessRushAttack( fTimePerSec);
		}
		break;
	case BS_MOVE_END:
		{
			if ( m_dwMoveEndMotionEndTime > 0 && m_dwMoveEndMotionEndTime < FRAMEGETTIME() )
			{
				if ( m_iAttackCurCount >= m_iAttackMaxCount )
					SetReserveEndBuff();
				else
					SetSerachState( m_pOwner );
			}
		}
		break;
	}

	//종료 조건
	if( CheckWoundState() || CheckBuffEndState( m_pOwner ) || ( m_dwBuffStartTime + GetBuffDuration() < FRAMEGETTIME() && m_BuffState == BS_SERACH ) )
	{
		SetReserveEndBuff();
		return;
	}
}

bool ioTargetRushAttackBuff::CheckWoundState()
{
	if( !m_pOwner ) 
		return false;

	CharState eState = m_pOwner->GetState();

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

//////////////////////////////////////////////////////////////////////////////////////////////
//serach state
void ioTargetRushAttackBuff::PorcessTargetFindState( ioBaseChar *pOwner )
{
	if( !pOwner ) 		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 		return;
	if ( m_BuffState != BS_SERACH )		return;

	if( pOwner->IsNeedProcess() )
	{
		if ( m_szTargetName.IsEmpty() || m_bChangeChar )
			CheckFindTarget(pOwner );

		if (!m_szTargetName.IsEmpty())
		{
			if( !CheckTargetValidate( pOwner, m_szTargetName ) )
			{
				m_szTargetName.Clear();
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY, TT_OTHER );
				// 빈타겟 설정...
				pOwner->SetTargetMarkerTarget( m_szTargetName, false, TT_OTHER );
			}
		}
	}
}

void ioTargetRushAttackBuff::CheckFindTarget( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	
	
	m_vTargetInfoList.clear();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget ) 
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )	
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )		
			continue;

		if ( !m_bEnableDownAim )
		{
			if ( pTarget->IsApplyDownState() )
				continue;
		}

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( ( m_fMarkerMaxRange * m_fMarkerMaxRange >= fDistSq) && ( m_fMarkerMinRange * m_fMarkerMinRange <= fDistSq) )
		{
			D3DXVECTOR3 vCharDir;
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			vCharDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fHalfCosine = cosf( DEGtoRAD( m_fMarkerAngle ) * FLOAT05 );
			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
			{
				tagBaseCharDistanceSq kInfo;
				kInfo.m_Name        = pTarget->GetCharName();
				kInfo.m_fDistanceSq = fDistSq;
				m_vTargetInfoList.push_back(kInfo);
			}
		}
	}
	std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), BaseCharDistanceSqSort2() );	
	
	if ( m_vTargetInfoList.empty() )
		return;
	
	if ( m_szTargetName.IsEmpty() )
		m_szTargetName = m_vTargetInfoList[0].m_Name;
	else if( m_bChangeChar )
	{
		ChangeTarget( pOwner );
		m_bChangeChar = false;
	}
	
	if ( !m_szTargetName.IsEmpty() )
	{
		pOwner->SetTargetMarkerTarget( m_szTargetName, false, TT_OTHER );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED, TT_OTHER );
	}
	
}

bool ioTargetRushAttackBuff::CheckTargetValidate( ioBaseChar *pOwner, ioHashString szTarget  )
{
	if ( !pOwner || szTarget.IsEmpty() )	return false;
	ioBaseChar *pTarget = pOwner->GetBaseChar( szTarget );
	if( !pTarget )		return false;

	if( pTarget->HasHideBuff() )		return false;

	if( pTarget->IsChatModeState( false ) )		return false;

	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();

	if( D3DXVec3LengthSq( &vDiff ) > m_fMarkerMaxRange * m_fMarkerMaxRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fMarkerMinRange * m_fMarkerMinRange )
		return false;

	vDiff.y = 0.0f;
	D3DXVec3Normalize( &vDiff, &vDiff );

	D3DXVECTOR3 vCharDir;
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	vCharDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fHalfCosine = cosf( DEGtoRAD( m_fMarkerAngle * FLOAT05 ) );
	if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
		return false;

	return true;
}

void ioTargetRushAttackBuff::SetRotation( ioBaseChar *pOwner )
{
	if ( !pOwner )		return;

	if ( !m_szTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
		if ( pTarget )
		{
			m_vTargetDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();

			D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );
			pOwner->SetTargetRotToDir( m_vTargetDir, false, false );
		}
	}
	else
	{
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
		D3DXVec3Normalize( &vDir, &vDir );
	}

	
}

//move attack state
void ioTargetRushAttackBuff::SetMoveAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )		return;

	m_iAttackCurCount++;

	m_BuffState = BS_ACTION;
	m_fCurMoveDist = 0.f;
	pOwner->SetState( CS_BUFF_ACTION );
	SetRotation( m_pOwner );

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_szMoveAttackMotion );
	if( iAniID == -1 )
		return;

	float fAniRate = m_fMoveAttackMotionRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_dwFireTime = FRAMEGETTIME();
	m_dwFireTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_dwEnableAttackCancelTime = FRAMEGETTIME();
	m_dwEnableAttackCancelTime += m_dwAttackCancelReserveTime;

	if( pOwner->IsNeedProcess())
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
		ioItem* pOwnerItem = GetOwnerItem();
		if ( pOwnerItem )
			pOwnerItem->ReduceSkillGauge( m_fReduceGauge );
	}
}

void ioTargetRushAttackBuff::ProcessRushAttack( float fTimePerSec )
{
	if ( m_BuffState != BS_ACTION || !m_pOwner )
		return;
	
	//캐릭터가 맵과 부딫쳤나 체크
	if ( !CheckMove( m_pOwner ) )
	{
		SetMoveEndState( m_pOwner );
		return;
	}
	//max 거리 체크
	if ( m_fCurMoveDist >= m_fMaxMoveDist )
	{
		SetMoveEndState( m_pOwner );
		return;
	}

	//ZoneEffectWeapon이 살아 있나 체크
	if ( CheckZoneEffectWeapon( m_pOwner ) )
	{
		SetMoveEndState( m_pOwner );
		return;
	}

	//weapon 발사( fire_s에 맞춰 )
	AttackWeaponFire( m_pOwner );

	if ( m_pOwner->IsNeedProcess() )
	{
		//a키 입력으로 어택 종료
		if ( m_dwEnableAttackCancelTime < FRAMEGETTIME() && m_pOwner->IsAttackKey() )
		{
			SetMoveEndState( m_pOwner );
			if ( m_pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
				kPacket << m_pOwner->GetCharName();
				kPacket << GetName();
				kPacket << BS_MOVE_END;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

bool ioTargetRushAttackBuff::CheckMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;

	D3DXVECTOR3 vDir;
	
	if ( !m_szTargetName.IsEmpty() )
	{
		// 점프상태에서 지상으로의 공격이라면
		if ( m_vTargetDir.y < 0.0f )
		{
			//현재 드랍존 혹은 맵 위라면
			if ( pOwner->GetWorldPosition().y == pOwner->GetBottomHeight() || pOwner->GetBottomHeight() <= 0.0f )
			{
				vDir = m_vTargetDir;
				vDir.y = 0.0f;
			}
			else
				vDir = m_vTargetDir;
		}
		else
			vDir = m_vTargetDir;
	}
	else 
		vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	
	D3DXVec3Normalize( &vDir, &vDir );
	m_vAttackDir = vDir;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fRunSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fRunSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = vDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}

		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
			vDiff = vTotalMove - vDiff;
			float fMoveDist = D3DXVec3Length( &vDiff );
			m_fCurMoveDist += fMoveDist;

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
						vCurPos.z,
						pOwner,
						false,
						pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
		if ( bCol )
			return false;
	}

	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();

	if( vCharPos.y < fPreHeight )
		vCharPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vCharPos );
		pOwner->SetWorldPosition( vCharPos );
	}	

	return true;
}

void ioTargetRushAttackBuff::AttackWeaponFire( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFireTime > 0 && m_dwFireTime < dwCurTime )
	{
		m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
		for ( int i= 0 ; i < (int)m_vWeaponInfoList.size() ; ++i )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

			FireTime kFireTime;
			kFireTime.dwStart = dwCurTime;
			kFireTime.iAttributeIdx = m_vWeaponInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = m_vWeaponInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni  = m_vWeaponInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = m_vWeaponInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = m_vWeaponInfoList[i].m_bWoundedLoopAni;
			kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );

			if ( pWeapon )
			{
				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
				if ( pZone )
				{
					ioBaseChar *pChar = pZone->GetOwner();
					if( pChar )
						pZone->SetZoneValue( true, false );
					m_dwFireWeaponIndex = m_dwWeaponBaseIndex;
				}
			}

			m_dwWeaponBaseIndex++;
		}

	}
	m_dwFireTime = 0;

}

void ioTargetRushAttackBuff::SetSerachState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	m_BuffState = BS_SERACH;
	m_szTargetName.Clear();
	m_bShootWeapon = false;

	m_dwEnableAttackTime = FRAMEGETTIME();
	m_dwEnableAttackTime += m_dwAttackReserveTime;

	if ( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY, TT_OTHER );
		// 빈타겟 설정...
		pOwner->SetTargetMarkerTarget( m_szTargetName, false, TT_OTHER );

		//피격 상태가 아니면서
		if ( !m_bWound && !m_bStart)
		{
			pOwner->SetCurMoveSpeed(0.0f);
			pOwner->SetJumpPower(0.0f);
			pOwner->SetGravityAmt( 0.0f );
			//공중에 있는 상태
			if ( pOwner->IsFloating() )
			{
				if ( m_fJumpPower > 0.0f )
					pOwner->SetSKillEndJumpState( m_fJumpPower, false, false );
			}
			else
				pOwner->SetState( CS_DELAY );
		}
	}

	m_vTargetDir = ioMath::VEC3_ZERO;
	m_vAttackDir = ioMath::VEC3_ZERO;

	m_dwFireWeaponIndex = 0;
}

bool ioTargetRushAttackBuff::CheckEnableUseSkill( int iSlot )
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

void ioTargetRushAttackBuff::SetMoveEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_BuffState = BS_MOVE_END;

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_szMoveEndMotion );
	if( iAniID == -1 )
		return;

	float fAniRate = m_fMoveEndMotionRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_dwMoveEndMotionEndTime = FRAMEGETTIME();
	m_dwMoveEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, 0, true );

	if ( m_bShootWeapon)
		ShootWeapon( pOwner );
	else
		DestroyWeapon( pOwner );
}

void ioTargetRushAttackBuff::ShootWeapon( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwFireWeaponIndex );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if ( pZone )
	{
		DWORD dwEndLiveTime = FRAMEGETTIME() - pZone->GetCreateTime();
		dwEndLiveTime += m_dwMoveEndAttackLiveTime;
		D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
		pZone->SetCollisionDir( m_vAttackDir );
		pZone->SetLiveTime( dwEndLiveTime );
		pZone->SetZoneValue( ioMath::VEC3_ZERO, false );
		pZone->SetMoveSpeed( m_fMoveEndAttackMoveSpeed );
	}
}

//적 캐릭터와 weapon이 충돌 했을 경우 ( 데미지가 0 이상일 때 ) EndAttack 사용
void ioTargetRushAttackBuff::UseMoveEndAttack( ioBaseChar *pWounder )
{
	if ( !pWounder ) return;
	m_bShootWeapon = true;
	if ( pWounder->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_SHOOT;
		kPacket << m_bShootWeapon;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTargetRushAttackBuff::IsCharCollisionSkipState()
{
	switch( m_BuffState )
	{
	case BS_ACTION:
		return true;
	}

	return false;
}

void ioTargetRushAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;
	int iType;
	rkPacket >> iType;

	switch ( iType )
	{
		case BS_ACTION:
		{
			rkPacket >> m_szTargetName;
			SetMoveAttackState( pOwner );
		}
		break;
		case BS_MOVE_END:
		{
			SetMoveEndState( pOwner );
		}
		break;
		case BS_SHOOT:
		{
			rkPacket >> m_bShootWeapon;
		}
		break;
	}
}

void ioTargetRushAttackBuff::ChangeTarget( ioBaseChar *pOwner )
{
	if( m_vTargetInfoList.empty() || m_szTargetName.IsEmpty() )
		return;

	int nCurTarget = -1;
	int nCnt = m_vTargetInfoList.size();
	for( int i=0; i<nCnt; ++i )
	{
		if( m_szTargetName == m_vTargetInfoList[i].m_Name )
			nCurTarget = i;
	}

	if ( nCurTarget != -1 )
	{
		nCurTarget++;

		if( nCurTarget >= nCnt )
			nCurTarget = 0;

		if( m_szTargetName != m_vTargetInfoList[nCurTarget].m_Name )
		{
			m_szTargetName = m_vTargetInfoList[nCurTarget].m_Name;
			return;
		}
	}

	return;
}

bool ioTargetRushAttackBuff::CheckZoneEffectWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || m_dwFireWeaponIndex <= 0 )
		return false;
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwFireWeaponIndex );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if ( pZone && !pZone->IsLive() )
		return true;
	else if ( !pZone )
		return true;

	return false;
}

void ioTargetRushAttackBuff::DestroyWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return ;
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwFireWeaponIndex );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if ( pZone && pZone->IsLive() )
		pZone->SetWeaponDead();
}

bool ioTargetRushAttackBuff::CheckEnableKey( ioBaseChar *pOwner )
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

bool ioTargetRushAttackBuff::CheckBuffEndState( ioBaseChar *pOwner )
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
