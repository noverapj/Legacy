

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioStatAttribute.h"

#include "ioFlashTransformationAttackSkill.h"

ioFlashTransformationAttackSkill::ioFlashTransformationAttackSkill()
{
	InitSkillData();
}

ioFlashTransformationAttackSkill::ioFlashTransformationAttackSkill( const ioFlashTransformationAttackSkill &rhs )
:ioNormalSkill( rhs ),
m_dwOrgLoopDuration( rhs.m_dwOrgLoopDuration ),
m_szSkillLoopEffect( rhs.m_szSkillLoopEffect ),
m_fSkillForceMove( rhs.m_fSkillForceMove ),
m_fSkillForceFriction( rhs.m_fSkillForceFriction ),
m_fSkillForceTime( rhs.m_fSkillForceTime ),
m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime ),
m_dwKeyReserveTime( rhs.m_dwKeyReserveTime ),
m_dwKeyInputGapTime( rhs.m_dwKeyInputGapTime ), 
m_pWeaponList( rhs.m_pWeaponList )
{
	InitSkillData();
}

ioFlashTransformationAttackSkill::~ioFlashTransformationAttackSkill()
{
}

void ioFlashTransformationAttackSkill::InitSkillData()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireIndex = 0;
	m_dwLoopStartTime = 0;
	m_dwAttackKeyReserveTime = 0;
	m_dwSkillLoopEffect = -1;
	m_dwCurWeaponCount = 0;

	m_fSkillForceCurTime = 0;
	m_dwCurKeyInputGapTime = 0;	
}

void ioFlashTransformationAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_dwOrgLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	rkLoader.LoadString_e( "skill_loop_effect", "", szBuf, MAX_PATH );
	m_szSkillLoopEffect = szBuf;

	m_fSkillForceMove = rkLoader.LoadFloat_e( "skill_force_move", 0.0f );
	m_fSkillForceFriction = rkLoader.LoadFloat_e( "skill_force_friction", 0.0f );
	m_fSkillForceTime = rkLoader.LoadFloat_e( "skill_force_time", 300.0f );
	
	m_dwSkillProtectedTime = rkLoader.LoadFloat_e( "skill_loop_protect_time", 0);

	m_dwKeyReserveTime = rkLoader.LoadInt_e( "fire_reserve_time", 0 );
	
	m_dwKeyInputGapTime = rkLoader.LoadInt_e( "key_input_gaptime", 250);

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 0 );
	m_pWeaponList.clear();
	m_pWeaponList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		TimeLineWeapon pWeapon;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		pWeapon.m_pWeapon.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		pWeapon.m_pWeapon.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pWeapon.m_pWeapon.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		pWeapon.m_pWeapon.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		pWeapon.m_pWeapon.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "attack_type%d_time", i+1 );
		pWeapon.m_dwFireTime = rkLoader.LoadInt( szKey, 0 );

		m_pWeaponList.push_back( pWeapon );
	}
}

ioSkill* ioFlashTransformationAttackSkill::Clone()
{
	return new ioFlashTransformationAttackSkill( *this );
}

bool ioFlashTransformationAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	ioNormalSkill::OnSkillStart( pChar );
	
	InitSkillData();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{			
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{			
		SetLoopState(pChar , pStage);

	}

	return true;
}

void ioFlashTransformationAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );


	if( m_dwSkillLoopEffect != -1 )
	{		
		pChar->DestroyMapEffect( m_dwSkillLoopEffect );
		m_dwSkillLoopEffect = -1;
	}

	pChar->SetInvisibleState( false, true );
	if( pChar->IsInvisibleFace() )
		pChar->ShowFaceMesh( false );

	InitSkillData();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

}

void ioFlashTransformationAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE :
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetLoopState( pChar ,pStage );
			}
		}
		break;

	case SS_LOOP:
		{	
			if( m_dwSkillLoopEffect != -1 )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwSkillLoopEffect );
				if( pMapEffect )
				{
					pMapEffect->SetWorldPosition( pChar->GetMidPositionByRate() );
					pMapEffect->SetWorldOrientation( pChar->GetWorldOrientation() );
				}
			}

			CheckKeyInput( pChar, pStage );
			SetFrictionMove( pChar );
			SetFire( pChar, pStage );
			
			if( 0 < m_dwLoopTime &&  m_dwLoopTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
			}

			if( 0 < m_dwAttackKeyReserveTime && m_dwAttackKeyReserveTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
			}
		}
		break;
	case SS_END:
		{
			
		}
		break;
	}
}

void ioFlashTransformationAttackSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{	
	if( !pChar ) return;
	if( !pStage ) return;

	m_State = SS_LOOP;
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwLoopTime = dwCurTime;
	m_dwLoopTime += (DWORD)m_dwOrgLoopDuration;

	m_dwLoopStartTime = dwCurTime;

	m_dwAttackKeyReserveTime += dwCurTime;
	m_dwAttackKeyReserveTime += (DWORD)m_dwKeyReserveTime;				

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pChar->SetInvisibleState( true, true );
	
	D3DXVECTOR3 vScale = pChar->GetWorldScale();
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

	ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_szSkillLoopEffect, vPos, vScale );
	if( pMapEffect )
	{
		m_dwSkillLoopEffect = pMapEffect->GetUniqueID();
	}
}

void ioFlashTransformationAttackSkill::SetFrictionMove( ioBaseChar* pChar, float fExtraForceRate )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_fSkillForceCurTime == 0 )
	{
		m_fSkillForceCurTime = dwCurTime;
		m_fSkillForceCurTime += m_fSkillForceTime;
	}

	if( m_fSkillForceCurTime <= dwCurTime )
	{	
		float fForceAmt = 0.0f;
		float fForceFriction = FLOAT1;
		float fForceAngle = 0.0f;

		fForceAmt = m_fSkillForceMove;
		fForceAmt = g_StatAttribute.GetCalcStatAttribute( fForceAmt, SA_STR_FORCE_AMT_UP, pChar );
		fForceAmt *= fExtraForceRate;
		fForceFriction = m_fSkillForceFriction;
		m_fSkillForceCurTime = 0;
	
		pChar->AddReservedSliding( 0, fForceAmt, fForceFriction, fForceAngle, true );
	}
}

void ioFlashTransformationAttackSkill::CheckKeyInput( ioBaseChar* pChar, ioPlayStage *pStage )
{	
	if( !pChar ) return;
	if( !pStage ) return;
	
	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();

	if( pChar->IsNeedProcess() && m_CurDirKey != eNewDirKey )
	{
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_ROTATE;
			kPacket << (int)eNewDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	if( pChar->IsNeedProcess() )
	{
		//키입력 여부 검사
		bool bReserveSkill = false;
		int iSkillNum = pChar->GetSkillKeyInput();
		if( iSkillNum >= 0 )
		{
			bReserveSkill = true;
		}
		
		DWORD dwCurTime = FRAMEGETTIME();

		if( pChar->IsAttackKey() || pChar->IsDefenseKey() || pChar->IsJumpKey() && !bReserveSkill )
		{
			if( m_dwCurKeyInputGapTime < dwCurTime )
			{
				m_dwCurKeyInputGapTime = dwCurTime;
				m_dwCurKeyInputGapTime += m_dwKeyInputGapTime;

				m_dwAttackKeyReserveTime += (DWORD)m_dwKeyReserveTime;			
								
				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SS_ATTACK;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
}

void ioFlashTransformationAttackSkill::SetFire( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_pWeaponList.empty() ) return;
	if( m_pWeaponList.size() <= m_dwFireIndex ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	if( m_dwLoopStartTime + m_pWeaponList[m_dwFireIndex].m_dwFireTime < dwCurTime )
	{
		pChar->SkillFireExplicit( m_pWeaponList[m_dwFireIndex].m_pWeapon,
								  GetName(),
								  m_dwWeaponBaseIndex + m_dwCurWeaponCount,
								  pChar->GetMidPositionByRate() );
		
		m_dwFireIndex++;
		m_dwCurWeaponCount++;
	}
}

void ioFlashTransformationAttackSkill::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_END;

	if( m_dwSkillLoopEffect != -1 )
	{		
		pChar->DestroyMapEffect( m_dwSkillLoopEffect );
		m_dwSkillLoopEffect = -1;
	}

	pChar->SetInvisibleState( false, true );
}

void ioFlashTransformationAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncType;		
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SS_ROTATE :
		{
			int iDir;
			rkPacket >> iDir;

			ioUserKeyInput::DirKeyInput eNewDirKey = (ioUserKeyInput::DirKeyInput)iDir;			
			pChar->SetTargetRotToDirKey( eNewDirKey );
		}
		break;

	case SS_ATTACK:
		{			
			m_dwAttackKeyReserveTime += (DWORD)m_dwKeyReserveTime;
		}
		break;
	}

}

bool ioFlashTransformationAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
	case SS_LOOP:		
		return false;
		break;	
	case SS_END:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		break;
	}

	return false;
}

bool ioFlashTransformationAttackSkill::IsProtected( int iDefenseBreakType ) const
{	

	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{	
		if( m_dwPreDelayStartTime != 0 )
			return true;	
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_LOOP:
		if( 0 < m_dwSkillProtectedTime && dwCurTime <= m_dwLoopStartTime + m_dwSkillProtectedTime )
			return true;
	}

	return false;
}

bool ioFlashTransformationAttackSkill::IsAttackEndState() const
{	
	if( m_State == SS_END )
		return true;

	return false;
}


bool ioFlashTransformationAttackSkill::IsNoDropState() const
{	
	switch( m_State )
	{
	case SS_NONE:
		return true;
		break;

	case SS_LOOP:		
		return true;		
		break;
	}

	return false;
}

bool ioFlashTransformationAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}
