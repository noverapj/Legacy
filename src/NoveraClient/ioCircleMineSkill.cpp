

#include "stdafx.h"

#include "ioCircleMineSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioCircleMineSkill::ioCircleMineSkill()
{
	Init();
}

ioCircleMineSkill::ioCircleMineSkill( const ioCircleMineSkill &rhs )
	: ioAttackSkill( rhs ),
	m_nMaxMineCnt( rhs.m_nMaxMineCnt ),
	m_nUseMineCnt( rhs.m_nUseMineCnt ),
	m_eMineType( rhs.m_eMineType ),

	m_szLoopAnimation( rhs.m_szLoopAnimation ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_fLoopAnimationRate( rhs.m_fLoopAnimationRate ),
	
	m_fCircleRange( rhs.m_fCircleRange ),
	m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	m_fCircleOffSet( rhs.m_fCircleOffSet ),
	m_szOnCircle( rhs.m_szOnCircle ),
	m_szOffCircle( rhs.m_szOffCircle ),

	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),

	m_bUseFullAttack( rhs.m_bUseFullAttack ),
	m_A_Attiribute( rhs.m_A_Attiribute ),
	m_S_Attiribute( rhs.m_S_Attiribute )
{
	Init();
}

ioCircleMineSkill::~ioCircleMineSkill()
{
}

void ioCircleMineSkill::Init()
{
	m_SkillState			= SS_PRE;
		
	m_dwLoopStartTime		= 0;
	m_dwBuryEndTime			= 0;	
	m_nCurrUseMineCnt		= 0;

	m_vMinePos				= ioMath::VEC3_ZERO;
	m_vCircleStartPos		= ioMath::VEC3_ZERO;	

	m_szCurCircle.Clear();
}

void ioCircleMineSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_nMaxMineCnt	= rkLoader.LoadInt_e( "max_mine_cnt", 0 );
	m_nUseMineCnt	= rkLoader.LoadInt_e( "use_mine_cnt", 0 );
	m_eMineType		= rkLoader.LoadInt_e( "mine_type", 0 );

	//Loop
	m_dwLoopDuration = rkLoader.LoadInt_e( "loop_duration", 0 );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopAnimation = szBuf;
	m_fLoopAnimationRate = rkLoader.LoadFloat_e( "loop_animation_rate", FLOAT1 );

	//circle
	m_fCircleRange		= rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed	= rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap	= rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet		= rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "on_magic_circle", "", szBuf, MAX_PATH );
	m_szOnCircle		= szBuf;

	rkLoader.LoadString_e( "off_magic_circle", "", szBuf, MAX_PATH );
	m_szOffCircle		= szBuf;

	//무적
	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//////////////////////////////////////////////////////////////////////////
	m_bUseFullAttack = rkLoader.LoadBool( "use_full_attack", false );
	LoadAttackAttribute( "a_attack", m_A_Attiribute, rkLoader );
	LoadAttackAttribute( "s_attack", m_S_Attiribute, rkLoader );
}

ioSkill* ioCircleMineSkill::Clone()
{
	return new ioCircleMineSkill( *this );
}

bool ioCircleMineSkill::OnSkillStart( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillStart( pChar );

	Init();
	CheckWeaponItemMesh( pChar );

	m_dwSkillStartTime		= FRAMEGETTIME();
	m_dwPreDelayStartTime	= m_dwSkillStartTime;

	StartPreDelayAnimation( pChar );
	pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), GetFireNeedGauge( pChar->GetCharName() ) );
	m_bReduceGauge = true;

	return true;
}

void ioCircleMineSkill::CheckWeaponItemMesh( ioBaseChar* pChar )
{
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

int ioCircleMineSkill::SetLoopAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );

		return iAniID;
	}

	return -1;
}

void ioCircleMineSkill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pOwner, pStage );
		}
		break;
	case SS_LOOP:
		{
			CheckLoopState( pOwner, pStage );
		}
		break;
	case SS_ACTION:
		{
			CheckActionState( pOwner, pStage );
		}
		break;
	case SS_END:
		{
		}
		break;
	}
}

void ioCircleMineSkill::CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pOwner );
		SetLoopState( pOwner, pStage, true );
	}
}

void ioCircleMineSkill::SetLoopState( ioBaseChar* pOwner, ioPlayStage* pStage, bool bFirst )
{
	m_SkillState = SS_LOOP;
	SetLoopAni( pOwner, m_szLoopAnimation, m_fLoopAnimationRate );

	if( pOwner->GetMagicCircle() == NULL )
	{
		pOwner->CreateMagicCircle( m_szOnCircle, m_szOnCircle, GetMagicGuide( pOwner ), m_fCircleOffSet );
		m_szCurCircle	= m_szOnCircle;
		m_bCircleOn		= true;
	}
	
	if( bFirst )
	{
		m_dwLoopStartTime = FRAMEGETTIME();
	}
	else
	{
		pOwner->SetMagicCirclePos( false, m_vCircleStartPos );
	}
}

void ioCircleMineSkill::CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	CheckCircle( pOwner, pStage );

	// 뱡향 변화 체크
	pOwner->CheckChangeDirectionForMagicCircle();
	D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
	pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );
	pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleOffSet, m_fCircleHeightGap, false );

	if( pOwner->IsNeedProcess() )
	{
		if( m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
		{
			m_SkillState = SS_END;
		}
		else if( m_nCurrUseMineCnt >= m_nUseMineCnt )
		{	
			m_SkillState = SS_END;
		}
		else if( pOwner->IsAttackKey() )
		{
			SetActionState_D( pOwner, pStage );
		}
		else if( m_bUseFullAttack && pOwner->IsJumpKey() )
		{
			SetActionState_A( pOwner,  pStage );
		}
		else if( m_bUseFullAttack && pOwner->IsDefenseKey() )
		{
			SetActionState_S( pOwner, pStage );
		}
	}
}

void ioCircleMineSkill::SetActionState_A( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	m_SkillState		= SS_ACTION;
	m_vCircleStartPos	= pOwner->GetMagicCirclePos();

	//ioAttackSkill::AttackSkillFire( pOwner );
	AttackAttributeSkillFire( pOwner, m_A_Attiribute );
	m_vMinePos = pOwner->GetMagicCirclePos( 5.0f );
	pOwner->DestroyMagicCircle();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
		if( iAniID != -1 )
		{
			float fFireTime = pOwner->GetFirstFireTime( GetSkillMotionName() ) * m_A_Attiribute.m_fAttackAniRate;
			m_dwBuryEndTime = FRAMEGETTIME() + fFireTime;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (int)SSS_ATTACK_A;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_nCurrUseMineCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nCurrUseMineCnt++;

	if( m_nCurrUseMineCnt < m_nUseMineCnt )
	{
		pOwner->IncreaseWeaponIndexBase();
		m_dwWeaponBaseIndex = pOwner->GetWeaponIndexBase();
	}
}


void ioCircleMineSkill::SetActionState_S( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	m_SkillState		= SS_ACTION;
	m_vCircleStartPos	= pOwner->GetMagicCirclePos();

	//ioAttackSkill::AttackSkillFire( pOwner );
	AttackAttributeSkillFire( pOwner, m_S_Attiribute);
	m_vMinePos = pOwner->GetMagicCirclePos( 5.0f );
	pOwner->DestroyMagicCircle();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
		if( iAniID != -1 )
		{
			float fFireTime = pOwner->GetFirstFireTime( GetSkillMotionName() ) * m_S_Attiribute.m_fAttackAniRate;
			m_dwBuryEndTime = FRAMEGETTIME() + fFireTime;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (int)SSS_ATTACK_S;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_nCurrUseMineCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nCurrUseMineCnt++;

	if( m_nCurrUseMineCnt < m_nUseMineCnt )
	{
		pOwner->IncreaseWeaponIndexBase();
		m_dwWeaponBaseIndex = pOwner->GetWeaponIndexBase();
	}
}

void ioCircleMineSkill::SetActionState_D( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	m_SkillState		= SS_ACTION;
	m_vCircleStartPos	= pOwner->GetMagicCirclePos();

	//ioAttackSkill::AttackSkillFire( pOwner );
	AttackAttributeSkillFire( pOwner, GetAttribute());
	m_vMinePos = pOwner->GetMagicCirclePos( 5.0f );
	pOwner->DestroyMagicCircle();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
		if( iAniID != -1 )
		{
			float fFireTime = pOwner->GetFirstFireTime( GetSkillMotionName() ) * GetAttribute().m_fAttackAniRate;
			m_dwBuryEndTime = FRAMEGETTIME() + fFireTime;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (int)SSS_ATTACK_D;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_nCurrUseMineCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nCurrUseMineCnt++;

	if( m_nCurrUseMineCnt < m_nUseMineCnt )
	{
		pOwner->IncreaseWeaponIndexBase();
		m_dwWeaponBaseIndex = pOwner->GetWeaponIndexBase();
	}
}

void ioCircleMineSkill::CheckActionState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireMotionEndTime < dwCurTime )
	{
		SetLoopState( pOwner, pStage, false );
	}
}

void ioCircleMineSkill::CheckCircle( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( !pOwner )
		return;

	D3DXVECTOR3 vPos = pOwner->GetMagicCirclePos();

	if( pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) > 0.0f )
	{
		if( m_szCurCircle != m_szOnCircle )
		{
			pOwner->CreateMagicCircle( m_szOnCircle, m_szOnCircle, GetMagicGuide( pOwner ), m_fCircleOffSet );			
			pOwner->SetMagicCirclePos( false, vPos );
			m_szCurCircle	= m_szOnCircle;
			m_bCircleOn		= true;
		}
	}
	else
	{
		if( m_szCurCircle != m_szOffCircle )
		{
			pOwner->CreateMagicCircle( m_szOffCircle, m_szOffCircle, GetMagicGuide( pOwner ), m_fCircleOffSet );			
			pOwner->SetMagicCirclePos( false, vPos );
			m_szCurCircle	= m_szOffCircle;
			m_bCircleOn		= false;
		}
	}
}

const ioHashString& ioCircleMineSkill::GetMagicGuide( ioBaseChar *pOwner )
{
	// 팀 체크
	if( pOwner->GetTeam() == TEAM_RED )
	{
		return m_szRedGuideCircle;
	}
	else if( pOwner->GetTeam() == TEAM_BLUE )
	{
		return m_szBlueGuideCircle;
	}
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		{
			return m_szBlueGuideCircle;
		}
	}

	return m_szRedGuideCircle;
}

void ioCircleMineSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}
	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	pChar->ClearReserveKeyInput();
	pChar->DestroyMagicCircle();

	Init();
}

bool ioCircleMineSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_ACTION:
		return false;
	}

	return true;
}

bool ioCircleMineSkill::IsAttackEndState() const
{
	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_ACTION:
		return false;
	}

	return true;
}

bool ioCircleMineSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioCircleMineSkill::IsUseActiveCnt() const
{
	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_ACTION:
		return true;
	}
	return false;
}

int ioCircleMineSkill::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nUseMineCnt;
}

int ioCircleMineSkill::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nCurrUseMineCnt;
}

void ioCircleMineSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioMineWeapon *pMine = ToMineWeapon( pWeapon );
	ioMineWeapon6 *pMine6 = ToMineWeapon6( pWeapon );


	if( pMine )
	{
		pMine->SetSensingStartTime( m_dwBuryEndTime );
		pMine->SetPosition( m_vMinePos );
		g_WeaponMgr.CheckMaxWeaponCnt( pWeapon, m_nMaxMineCnt );
	}
	else if( pMine6 )
	{
		pMine6->SetSensingStartTime( m_dwBuryEndTime );
		pMine6->SetPosition( m_vMinePos );
		g_WeaponMgr.CheckMaxWeaponCnt( pWeapon, m_nMaxMineCnt );
	}
}

void ioCircleMineSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int eState;
	rkPacket >> eState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, true );
	pOwner->SetWorldPosition( vPos );

	if( eState == SSS_ATTACK_D )
	{
		rkPacket >> m_nCurrUseMineCnt;
		SetActionState_D( pOwner, pStage );
	}
	else if( eState == SSS_ATTACK_A )
	{
		rkPacket >> m_nCurrUseMineCnt;
		SetActionState_A( pOwner, pStage );
	}
	else if( eState == SSS_ATTACK_S )
	{
		rkPacket >> m_nCurrUseMineCnt;
		SetActionState_S( pOwner, pStage );
	}
}

bool ioCircleMineSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	if( m_dwSkillStartTime + m_dwSkillProtectTime < FRAMEGETTIME() )
		return false;

	return true;
}