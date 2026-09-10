
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTargetCreateWeaponBuff.h"

ioTargetCreateWeaponBuff::ioTargetCreateWeaponBuff()
{
	Init();
}

ioTargetCreateWeaponBuff::ioTargetCreateWeaponBuff( const ioTargetCreateWeaponBuff &rhs )
	: ioBuff( rhs ),
	  m_dwReloadFireTime( rhs.m_dwReloadFireTime ),
	  m_nTotalWeaponCnt( rhs.m_nTotalWeaponCnt ),
	  m_RemoveBuffList( rhs.m_RemoveBuffList ),
	  m_szRedReadyMarker( rhs.m_szRedReadyMarker ),
	  m_szBlueReadyMarker( rhs.m_szBlueReadyMarker ),
	  m_szRedAimMarker( rhs.m_szRedAimMarker ),
	  m_szBlueAimMarker( rhs.m_szBlueAimMarker ),
	  m_szRedZeroMarker( rhs.m_szRedZeroMarker ),
	  m_szBlueZeroMarker( rhs.m_szBlueZeroMarker ),
	  m_fMakerShowRange( rhs.m_fMakerShowRange ),
	  m_fTargetAngle( rhs.m_fTargetAngle ),
	  m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	  m_fTargetMinRange( rhs.m_fTargetMinRange ),
	  m_bEnableDownAim( rhs.m_bEnableDownAim ),
	  m_dwAimTime( rhs.m_dwAimTime ),
	  m_dwZeroTime( rhs.m_dwZeroTime ),
	  m_nAimWeapon( rhs.m_nAimWeapon ),
	  m_nZeroWeapon( rhs.m_nZeroWeapon ),
	  m_szAimAttackAni( rhs.m_szAimAttackAni ),
	  m_fAimAttackRate( rhs.m_fAimAttackRate ),
	  m_szJumpAttackAni( rhs.m_szJumpAttackAni ),
	  m_fJumpAttackRate( rhs.m_fJumpAttackRate ),
	  m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
	  m_fGravityAmt( rhs.m_fGravityAmt ),
	  m_fJumpPower( rhs.m_fJumpPower ),
	  m_bSetAfterJump( rhs.m_bSetAfterJump )
{
	Init();
}

ioTargetCreateWeaponBuff::~ioTargetCreateWeaponBuff()
{
}

void ioTargetCreateWeaponBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	m_dwReloadFireTime = rkLoader.LoadInt_e( "reload_fire_time", 0 );

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

	//추가 읽기
	LoadTargetProperty( rkLoader );
}

ioBuff* ioTargetCreateWeaponBuff::Clone()
{
	return new ioTargetCreateWeaponBuff( *this );
}

void ioTargetCreateWeaponBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	Init();

	m_iMaxActiveCount = m_nTotalWeaponCnt;
	m_iCurActiveCount = m_nTotalWeaponCnt;

	if ( pOwner->IsNeedProcess() )
	{
		CreateReadyMarker( m_pOwner );
	}
}

bool ioTargetCreateWeaponBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	Init();

	m_iMaxActiveCount = m_nTotalWeaponCnt;
	m_iCurActiveCount = m_nTotalWeaponCnt;

	return true;
}

void ioTargetCreateWeaponBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckWoundState() ||
		( m_nCurWeaponCnt >= m_nTotalWeaponCnt && m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() ) )
	{
		SetReserveEndBuff();
		return;
	}

	CheckJumpAttackState( m_pOwner );
	CheckFindTarget( m_pOwner );
	CheckInputKey( m_pOwner );
	CheckAttack( m_pOwner );	

	UpdateReadyMarker( m_pOwner );
	UpdateAimMarker( m_pOwner );
	UpdateZeroMarker( m_pOwner );	
}

void ioTargetCreateWeaponBuff::EndBuff()
{	
	ioBuff::EndBuff();
	
	//버프 제거
	for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
	{
		m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
	}
	m_RemoveBuffList.clear();	

	DestroyMarker( m_pOwner, m_dwReadyMakerID );
	DestroyMarker( m_pOwner, m_dwAimMakerID );
	DestroyMarker( m_pOwner, m_dwZeroMakerID );
	
	//피격 상태가 아니면서
	if ( !m_bWound )
	{
		//떨어지는 상태 (점프 공격 후 종료)
		if ( m_pOwner->GetState() == CS_FALL )
		{
			m_pOwner->SetSKillEndJumpState( m_fJumpPower, false, false, false, true );
			return;
		}
				
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
}

void ioTargetCreateWeaponBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	int nState;
	rkPacket >> nState;

	switch( (TargetBuffState)nState )
	{
	case TBS_FindTarget:
		{
			rkPacket >> m_AimedTarget;
			
			m_bAimFire = false;
			m_bZeroFire = false;

			DestroyMarker( pOwner, m_dwAimMakerID );
			DestroyMarker( pOwner, m_dwZeroMakerID );
			CreateAimMarker( pOwner, m_AimedTarget );
		}
		break;
	case TBS_CreateWeapon:
		{
			bool bJumpAttack;
			rkPacket >> m_bAimFire;
			rkPacket >> m_bZeroFire;
			rkPacket >> bJumpAttack;
			rkPacket >> m_bIsJumpAttack;
			rkPacket >> m_nCurWeaponCnt;
			rkPacket >> m_AttackTarget;
			
			if ( bJumpAttack )
			{
				SetAttackTime( pOwner, m_szJumpAttackAni, m_fJumpAttackRate );
			}			
			else if ( m_bAimFire )
			{
				SetAttackTime( pOwner, m_szAimAttackAni, m_fAimAttackRate );
			}
			else if ( m_bZeroFire )
			{
				SetAttackTime( pOwner, m_szAimAttackAni, m_fAimAttackRate );
			}
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//초기화
void ioTargetCreateWeaponBuff::Init()
{
	m_dwCheckFireTime = 0;
	m_dwCheckAimTime = 0;
	m_dwCheckZeroTime = 0;

	m_dwReadyMakerID = -1;	
	m_dwAimMakerID = -1;
	m_dwZeroMakerID = -1;

	m_AimedTarget.Clear();
	m_NewAimedTarget.Clear();
	m_AttackTarget.Clear();

	m_bAimFire = false;
	m_bZeroFire = false;

	m_bSetReadyMarkVisible = true;

	m_nCurWeaponCnt = 0;
	m_dwAttackEndTime = 0;
	m_bWound = false;
	m_bSetFire = false;
	m_bIsJumpAttack = false;

	m_BuffState = TBS_None;
}

//추가 읽기
void ioTargetCreateWeaponBuff::LoadTargetProperty( ioINILoader &rkLoader )
{
	m_nTotalWeaponCnt = rkLoader.LoadInt_e( "total_weapon_count", 0 );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "red_ready_marker", "", szBuf, MAX_PATH );
	m_szRedReadyMarker = szBuf;
	rkLoader.LoadString_e( "blue_ready_marker", "", szBuf, MAX_PATH );
	m_szBlueReadyMarker = szBuf;
	rkLoader.LoadString_e( "red_aim_marker", "", szBuf, MAX_PATH );
	m_szRedAimMarker = szBuf;
	rkLoader.LoadString_e( "blue_aim_marker", "", szBuf, MAX_PATH );
	m_szBlueAimMarker = szBuf;
	rkLoader.LoadString_e( "red_zero_marker", "", szBuf, MAX_PATH );
	m_szRedZeroMarker = szBuf;
	rkLoader.LoadString_e( "blue_zero_marker", "", szBuf, MAX_PATH );
	m_szBlueZeroMarker = szBuf;

	m_fMakerShowRange = rkLoader.LoadFloat_e( "maker_show_range", 0.0f );

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetMaxRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 0 );
	m_dwZeroTime = rkLoader.LoadInt_e( "zero_time", 0 );

	m_nAimWeapon = rkLoader.LoadInt_e( "aim_weapon_type", NO_WEAPON_ATTRIBUTE );
	m_nZeroWeapon = rkLoader.LoadInt_e( "zero_weapon_type", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "attack_animation", "", szBuf, MAX_PATH );
	m_szAimAttackAni = szBuf;
	m_fAimAttackRate = rkLoader.LoadFloat_e( "attack_animaion_rate", FLOAT1 );
	
	rkLoader.LoadString_e( "jump_attack_animation", "", szBuf, MAX_PATH );
	m_szJumpAttackAni = szBuf;
	m_fJumpAttackRate = rkLoader.LoadFloat_e( "jump_attack_animaion_rate", FLOAT1 );

	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	m_fGravityAmt = rkLoader.LoadFloat_e( "gravity_amt", 0.0f );

	m_bSetAfterJump = rkLoader.LoadBool_e( "set_after_jump", false );
}

//상태 확인
bool ioTargetCreateWeaponBuff::CheckWoundState()
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
		{
			return true;
		}
	}

	return false;
}

//타겟 찾기
void ioTargetCreateWeaponBuff::CheckFindTarget( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || m_bSetFire )
		return;

	m_NewAimedTarget.Clear();
	ioHashString szTarget;	
	if( pOwner->CheckAimedTargetByBow( m_fTargetAngle, m_fTargetMinRange, m_fTargetMaxRange, szTarget, m_bEnableDownAim ) )
	{
		m_NewAimedTarget = szTarget;

		//Ball 확인
		if( CompareAimedTarget( pOwner ) )
		{
			m_NewAimedTarget.Clear();
		}
	}
	else
	{
		m_NewAimedTarget.Clear();
	}

	if ( m_AimedTarget != m_NewAimedTarget )
	{
		m_AimedTarget = m_NewAimedTarget;

		m_bAimFire = false;
		m_bZeroFire = false;

		DestroyMarker( pOwner, m_dwAimMakerID );
		DestroyMarker( pOwner, m_dwZeroMakerID );
		CreateAimMarker( pOwner, m_AimedTarget );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << TBS_FindTarget;
			kPacket << m_AimedTarget;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//공인지 확인
bool ioTargetCreateWeaponBuff::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) 
		return false;
	
	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

	float fHalfCosine = cosf( DEGtoRAD( m_fTargetAngle ) * FLOAT05 );

	float fLimitDistSq = m_fTargetMaxRange * m_fTargetMaxRange;
	float fMinDistSq = m_fTargetMinRange * m_fTargetMinRange;
	
	// Ball
	if( pBall )
	{
		vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				return true;
		}
	}
	return false;
}

//키입력 확인
void ioTargetCreateWeaponBuff::CheckInputKey(  ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() || m_AimedTarget.IsEmpty() )
		return;

	bool bSendPacket = false;
	bool bJumpAttack = false;

	if ( pOwner->GetState() == CS_JUMP )
	{
		bJumpAttack = true;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_nCurWeaponCnt < m_nTotalWeaponCnt && 
		m_dwAttackEndTime < dwCurTime &&
		m_dwCheckFireTime + m_dwReloadFireTime < dwCurTime && 
		pOwner->IsAttackKey() )
	{		
		if ( m_bAimFire )
		{
			if ( bJumpAttack )
			{
				SetAttackTime( pOwner, m_szJumpAttackAni, m_fJumpAttackRate );
				pOwner->SetGravityAmt( m_fGravityAmt );
				m_bIsJumpAttack = true;
			}
			else
				SetAttackTime( pOwner, m_szAimAttackAni, m_fAimAttackRate );
		}
		else if ( m_bZeroFire )
		{
			if ( bJumpAttack )
			{
				SetAttackTime( pOwner, m_szJumpAttackAni, m_fJumpAttackRate );
				pOwner->SetGravityAmt( m_fGravityAmt );
				m_bIsJumpAttack = true;
			}
			else
				SetAttackTime( pOwner, m_szAimAttackAni, m_fAimAttackRate );
		}
		bSendPacket = true;
	}

	if ( bSendPacket && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << TBS_CreateWeapon;
		kPacket << m_bAimFire;
		kPacket << m_bZeroFire;
		kPacket << bJumpAttack;
		kPacket << m_bIsJumpAttack;
		kPacket << m_nCurWeaponCnt;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//애니메이션 종료 시간 얻기
void ioTargetCreateWeaponBuff::SetAttackTime( ioBaseChar *pOwner, ioHashString szName, float fRate )
{
	if ( !pOwner )
		return;

	pOwner->SetEtcState( szName, fRate );

	ioEntityGroup *pGroup = pOwner->GetGroup();
	int iAniID = pGroup->GetAnimationIdx( szName );
	if( iAniID == -1 ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackEndTime = dwCurTime;
	m_dwAttackEndTime += pGroup->GetAnimationFullTime( iAniID ) * fRate;

	m_dwCheckFireTime = dwCurTime;
	m_dwFireTime = dwCurTime;
	m_dwFireTime += pGroup->GetAnimationEventTime_e( iAniID, "fire_s" ) * fRate;

	m_bSetFire = true;

	m_AttackTarget = m_AimedTarget;
}

//무기 생성
void ioTargetCreateWeaponBuff::CreateWeapon( ioBaseChar *pOwner, int nWeaponIndex, ioHashString TargetName )
{
	if( !pOwner || TargetName.IsEmpty() )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( TargetName );
	if ( pTarget )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;

		DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx = nWeaponIndex;
		kFireTime.iResistanceIdx = 0;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = false;
		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
		//kFireTime.eFireTimeType = FTT_BUFF_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );	

		m_nCurWeaponCnt++;
		m_iCurActiveCount = m_nTotalWeaponCnt - m_nCurWeaponCnt;
	}
}

//무기 선택 생성
void ioTargetCreateWeaponBuff::CheckAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !m_bSetFire )
		return;

	if ( m_dwFireTime < FRAMEGETTIME() )
	{
		if ( m_bAimFire )
		{
			CreateWeapon( pOwner, m_nAimWeapon, m_AttackTarget );
			m_bSetFire = false;	
		}
		else if ( m_bZeroFire )
		{
			CreateWeapon( pOwner, m_nZeroWeapon, m_AttackTarget );
			m_bSetFire = false;	
		}
	}
}

//////////////마크////////////////////////////////////////////////////////////////////////
//준비 마크 생성
void ioTargetCreateWeaponBuff::CreateReadyMarker( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	vPos.y = pOwner->GetMidHeightByRate();
	vPos += ( vDir * m_fMakerShowRange );

	ioMapEffect *pMapEffect = NULL;
	if( pOwner->IsNeedProcess() ) 
	{
		pMapEffect = pStage->CreateMapEffect( m_szBlueReadyMarker, vPos, vScale );		
	}
	else
	{
		if( pOwner->GetTeam() == TEAM_BLUE )
			pMapEffect = pStage->CreateMapEffect( m_szBlueReadyMarker, vPos, vScale );
		else
			pMapEffect = pStage->CreateMapEffect( m_szRedReadyMarker, vPos, vScale );
	}

	if( pMapEffect )
	{
		m_dwReadyMakerID = pMapEffect->GetUniqueID();

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

//준비 마크 업데이트
void ioTargetCreateWeaponBuff::UpdateReadyMarker( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() || m_dwReadyMakerID == -1 ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	
	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwReadyMakerID );
	if( pMapEffect )
	{
		if ( m_bSetReadyMarkVisible && !m_AimedTarget.IsEmpty() )
		{
			pMapEffect->SetVisible( false );
			m_bSetReadyMarkVisible = false;
		}
		else if ( m_AimedTarget.IsEmpty() )
		{
			if ( !m_bSetReadyMarkVisible )
			{
				pMapEffect->SetVisible( true );
				m_bSetReadyMarkVisible = true;
			}
			else 
			{
				D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				vPos.y = pOwner->GetMidHeightByRate();
				vPos += ( vDir * m_fMakerShowRange );

				pMapEffect->SetWorldPosition( vPos );

				D3DXQUATERNION qtCharRot;
				ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
				pMapEffect->SetWorldOrientation( qtCharRot );
			}
		}
	}
}

//조준 마크 생성
void ioTargetCreateWeaponBuff::CreateAimMarker( ioBaseChar *pOwner, ioHashString TargetName )
{
	if( !pOwner || TargetName.IsEmpty() ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( TargetName );
	if ( pTarget )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pTarget->GetWorldPosition();

		vPos.y = pTarget->GetMidHeightByRate();

		ioMapEffect *pMapEffect = NULL;
		if( pOwner->GetTeam() == TEAM_BLUE )
			pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vPos, vScale );
		else
			pMapEffect = pStage->CreateMapEffect( m_szRedAimMarker, vPos, vScale );
	
		if( pMapEffect )
		{
			m_dwAimMakerID = pMapEffect->GetUniqueID();
		}

		m_dwCheckAimTime = FRAMEGETTIME();
				
		m_bAimFire = true;
		m_bZeroFire = false;
	}
}

//조준 마크 업데이트
void ioTargetCreateWeaponBuff::UpdateAimMarker( ioBaseChar *pOwner )
{
	if( !pOwner || m_dwAimMakerID == -1 ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( !m_bSetFire && m_dwCheckAimTime + m_dwAimTime < FRAMEGETTIME() )
	{
		DestroyMarker( pOwner, m_dwAimMakerID);
		CreateZeroMarker( pOwner, m_AimedTarget );
		return;
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if ( pTarget )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAimMakerID );
		if( pMapEffect )
		{
			if ( m_bSetFire )
			{
				pMapEffect->SetVisible( false );
				return;
			}

			D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
			vPos.y = pTarget->GetMidHeightByRate();
			pMapEffect->SetWorldPosition( vPos );
		}
	}
}

//영점 마크 생성
void ioTargetCreateWeaponBuff::CreateZeroMarker( ioBaseChar *pOwner, ioHashString TargetName )
{
	if( !pOwner || TargetName.IsEmpty() ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( TargetName );
	if ( pTarget )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pTarget->GetWorldPosition();

		vPos.y = pTarget->GetMidHeightByRate();

		ioMapEffect *pMapEffect = NULL;
		if( pOwner->GetTeam() == TEAM_BLUE )
			pMapEffect = pStage->CreateMapEffect( m_szBlueZeroMarker, vPos, vScale );
		else
			pMapEffect = pStage->CreateMapEffect( m_szRedZeroMarker, vPos, vScale );

		if( pMapEffect )
		{
			m_dwZeroMakerID = pMapEffect->GetUniqueID();
		}

		m_dwCheckZeroTime = FRAMEGETTIME();
				
		m_bAimFire = false;
		m_bZeroFire = true;
	}
}

//영점 마크 업데이트
void ioTargetCreateWeaponBuff::UpdateZeroMarker( ioBaseChar *pOwner )
{
	if( !pOwner || m_dwZeroMakerID == -1 ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( !m_bSetFire && m_dwCheckZeroTime + m_dwZeroTime < FRAMEGETTIME() )
	{
		DestroyMarker( pOwner, m_dwZeroMakerID);
		CreateAimMarker( pOwner, m_AimedTarget );
		return;
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if ( pTarget )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwZeroMakerID );
		if( pMapEffect )
		{
			if ( m_bSetFire )
			{
				pMapEffect->SetVisible( false );
				return;
			}

			D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
			vPos.y = pTarget->GetMidHeightByRate();
			pMapEffect->SetWorldPosition( vPos );
		}
	}
}

//마크 삭제
void ioTargetCreateWeaponBuff::DestroyMarker( ioBaseChar *pOwner, DWORD &dwMarkID )
{
	if( !pOwner )
		return;

	if( dwMarkID != -1 )
	{
		pOwner->EndMapEffect( dwMarkID, false );
		dwMarkID = -1;
	}
}

bool ioTargetCreateWeaponBuff::CheckEnableUseSkill( int iSlot )
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

//추가 점프
void ioTargetCreateWeaponBuff::CheckJumpAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !m_bSetAfterJump)
		return;

	if ( m_bIsJumpAttack && m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		pOwner->SetSKillEndJumpState( m_fJumpPower, false, false, false, true );
		m_bIsJumpAttack = false;
	}
}