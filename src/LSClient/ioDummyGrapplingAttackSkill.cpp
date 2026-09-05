#include "StdAfx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioDummyGrapplingAttackSkill.h"
#include "ioTargetTrace8Dummy.h"


ioDummyGrapplingAttackSkill::ioDummyGrapplingAttackSkill(void)
{
	ClearData();
}

ioDummyGrapplingAttackSkill::ioDummyGrapplingAttackSkill( const ioDummyGrapplingAttackSkill &rhs )
	: ioAttackSkill( rhs ),
	m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	m_DummyCharList( rhs.m_DummyCharList ),
	m_szLoopMotion( rhs.m_szLoopMotion ),
	m_fLoopAnimationRate( rhs.m_fLoopAnimationRate ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_szActionDMotion( rhs.m_szActionDMotion ),
	m_fActionDAnimationRate( rhs.m_fActionDAnimationRate ),
	m_szActionSMotion( rhs.m_szActionSMotion ),
	m_fActionSAnimationRate( rhs.m_fActionSAnimationRate ),
	m_dwActionKeyEnableTime( rhs.m_dwActionKeyEnableTime ),
	m_DAttackAttribute( rhs.m_DAttackAttribute ),
	m_SAttackAttribute( rhs.m_SAttackAttribute )
{
	ClearData();
}


ioDummyGrapplingAttackSkill::~ioDummyGrapplingAttackSkill(void)
{
}

void ioDummyGrapplingAttackSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;
	m_dwLoopStartTime = 0;
	m_dwFireMotionStartTime = 0;
	m_SkillMotion = SM_D;
	m_iDummyIndexList.clear();
}

void ioDummyGrapplingAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopMotion = szBuf;
	m_fLoopAnimationRate = rkLoader.LoadFloat_e( "loop_animation_rate", FLOAT1 );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );

	m_dwActionKeyEnableTime = (DWORD)rkLoader.LoadInt_e( "action_key_enable_time", 0 );

	rkLoader.LoadString_e( "action_d_animation", "", szBuf, MAX_PATH );
	m_szActionDMotion = szBuf;
	m_fActionDAnimationRate = rkLoader.LoadFloat_e( "action_d_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "action_s_animation", "", szBuf, MAX_PATH );
	m_szActionSMotion = szBuf;
	m_fActionSAnimationRate = rkLoader.LoadFloat_e( "action_s_animation_rate", FLOAT1 );

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
	LoadDAttribute( rkLoader );
	LoadSAttribute( rkLoader );
}

void ioDummyGrapplingAttackSkill::LoadDAttribute( ioINILoader &rkLoader )
{
	m_DAttackAttribute.Init();
	m_DAttackAttribute.m_AttackAnimation = m_szActionDMotion;
	m_DAttackAttribute.m_fAttackAniRate  = m_fActionDAnimationRate;

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "d_skill_end_motion", "", szBuf, MAX_PATH );

	m_DAttackAttribute.m_AttackEndAnimation = szBuf;
	m_DAttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "d_skill_end_time_rate", FLOAT1 );

	m_DAttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "d_animation_pre_delay", 0 );
	m_DAttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "d_animation_end_delay", 0 );

	char szKey[MAX_PATH];

	m_DAttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "d_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_DAttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "d_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "d_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "d_attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_DAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "d_attack_type_count", 0 );
	m_DAttackAttribute.m_vWeaponInfoList.clear();
	m_DAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "d_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "d_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "d_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "d_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "d_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_DAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}


void ioDummyGrapplingAttackSkill::LoadSAttribute( ioINILoader &rkLoader )
{
	m_SAttackAttribute.Init();
	m_SAttackAttribute.m_AttackAnimation = m_szActionSMotion;
	m_SAttackAttribute.m_fAttackAniRate  = m_fActionSAnimationRate;

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "s_skill_end_motion", "", szBuf, MAX_PATH );

	m_SAttackAttribute.m_AttackEndAnimation = szBuf;
	m_SAttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "s_skill_end_time_rate", FLOAT1 );

	m_SAttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "s_animation_pre_delay", 0 );
	m_SAttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "s_animation_end_delay", 0 );

	char szKey[MAX_PATH];

	m_SAttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "s_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_SAttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "s_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "s_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "s_attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_SAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "s_attack_type_count", 0 );
	m_SAttackAttribute.m_vWeaponInfoList.clear();
	m_SAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "s_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "s_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "s_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "s_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "s_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_SAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

ioSkill* ioDummyGrapplingAttackSkill::Clone()
{
	return new ioDummyGrapplingAttackSkill( *this );
}

bool ioDummyGrapplingAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
	return true;
}

void ioDummyGrapplingAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetLoopState( pChar );
				CheckDummyChar( pChar, pStage );
				EndPreDelayEffect( pChar );
			}
		}
	case SS_LOOP:
		{
			if ( m_dwLoopStartTime > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				AttackSkillFire( pChar );
				m_SkillState = SS_ATTACK;
			}
		}
		break;
	case SS_ATTACK:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime + m_dwActionKeyEnableTime < dwCurTime )
			{
				m_SkillState = SS_END;
			}

			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				if ( pChar->IsAttackKey() && pChar->IsNeedProcess() )
				{
					m_SkillMotion = SM_D;
					SetDActionState( pChar );
					CheckDummyAttack( pChar );
				}
				else if ( pChar->IsDefenseKey() && pChar->IsNeedProcess() )
				{
					m_SkillMotion = SM_S;
					SetSActionState( pChar );
					CheckDummyAttack( pChar );
				}
			}
		}
		break;
	case SS_ACTION:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				m_SkillState = SS_END;
				//RemoveAllDummy( pChar );

			}
		}
		break;
	}
}

void ioDummyGrapplingAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	//RemoveAllDummy( pChar );
	ClearData();
}

void ioDummyGrapplingAttackSkill::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharList.empty() )
		return;

	//if( m_dwFireStartTime == 0 ) return;
	//if( m_dwFireStartTime >= FRAMEGETTIME() )
	//	return;

	//m_dwFireStartTime = 0;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	//
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
			pChar->GetCharName(),
			vTargetPos,
			fStartAngle,
			0, true );

		if( pDummy )
		{
			m_iDummyIndexList.push_back( iCurIndex );
			pDummy->SetControlSkillName( GetName() );
		}

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fStartAngle;
		kInfo.m_vTargetPos = vTargetPos;

		vCreateList.push_back( kInfo );
	}

	int iCreateCnt = vCreateList.size();
	if( pChar->IsNeedSendNetwork() && iCreateCnt > 0 )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_DUMMY;
		kPacket << pChar->GetTargetRot();

		kPacket << iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			kPacket << vCreateList[i].m_DummyCharName;
			kPacket << vCreateList[i].m_iDummyCharIdx;
			kPacket << vCreateList[i].m_fDummyCharStartAngle;
			kPacket << vCreateList[i].m_vTargetPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyGrapplingAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch ( iSyncState )
	{
	case SSS_DUMMY:
		{
			g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iCreateCnt;
			rkPacket >> iCreateCnt;

			for( int i=0; i < iCreateCnt; ++i )
			{
				int iCurIndex;
				ioHashString szDummyCharName;
				float fStartAngle;
				D3DXVECTOR3 vTargetPos;

				rkPacket >> szDummyCharName >> iCurIndex;
				rkPacket >> fStartAngle;
				rkPacket >> vTargetPos;

				//m_dwFireStartTime = 0;

				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
					pChar->GetCharName(),
					vTargetPos,
					fStartAngle,
					0, false );

				if( pDummy )
				{
					m_iDummyIndexList.push_back( iCurIndex );
					pDummy->SetControlSkillName( GetName() );
				}
			}
		}
		break;
	case SSS_ACTION:
		{
			int iSkillMotion;
			rkPacket >> iSkillMotion;
			m_SkillMotion = (SkillMotion)iSkillMotion;
			if ( m_SkillMotion == SM_D )
			{
				SetDActionState( pChar );
			}
			else if ( m_SkillMotion == SM_S )
			{
				SetSActionState( pChar );
			}

			CheckDummyAttack( pChar );
		}
		break;
	}
}

void ioDummyGrapplingAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioDummyGrapplingAttackSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_szLoopMotion );
	if( iAniID == -1 )
		return;
	float fTimeRate = m_fLoopAnimationRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwLoopStartTime = dwCurTime;
}

void ioDummyGrapplingAttackSkill::SetDActionState( ioBaseChar *pChar )
{
	if ( !pChar ) return;

	m_SkillState = SS_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion;
	szSkillMotion = m_DAttackAttribute.m_AttackAnimation;

	int iAniID = -1;
	iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )	
		return;

	float fTimeRate;
	fTimeRate = m_DAttackAttribute.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFullTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime +=  pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwFireMotionStartTime = dwCurTime;
	m_dwFireMotionStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_DAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		0 );

	pChar->SetReservedSliding( m_DAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_ACTION;
		kPacket << m_SkillMotion;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyGrapplingAttackSkill::SetSActionState( ioBaseChar *pChar )
{
	if ( !pChar ) return;

	m_SkillState = SS_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion;
	szSkillMotion = m_SAttackAttribute.m_AttackAnimation;

	int iAniID = -1;
	iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )	
		return;

	float fTimeRate;
	fTimeRate = m_SAttackAttribute.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFullTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime +=  pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwFireMotionStartTime = dwCurTime;
	m_dwFireMotionStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_SAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		0 );

	pChar->SetReservedSliding( m_SAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_ACTION;
		kPacket << m_SkillMotion;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioDummyGrapplingAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_ACTION:
	case SS_ATTACK:
		return false;
	case SS_END:
		return true;
	}

	return false;
}

void ioDummyGrapplingAttackSkill::RemoveAllDummy( ioBaseChar *pChar )
{
	if ( !pChar || m_iDummyIndexList.empty() )
		return;
	auto Iter = m_iDummyIndexList.begin();
	for ( ;Iter != m_iDummyIndexList.end(); ++Iter )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, (*Iter) );
		if( pDummy )	
		{
			pDummy->SetDieState( true );
		}
	}
}

void ioDummyGrapplingAttackSkill::CheckDummyAttack( ioBaseChar *pChar )
{
	if ( !pChar || m_iDummyIndexList.empty() )
		return;
	auto Iter = m_iDummyIndexList.begin();
	for ( ;Iter != m_iDummyIndexList.end(); ++Iter )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, (*Iter) );
		if( pDummy )	
		{
			ioTargetTrace8Dummy* pTrace8 = ToTargetTrace8Dummy( pDummy );
			if ( pTrace8 )
			{
				if ( m_SkillMotion == SM_D )
				{
					pTrace8->SetDAttack( true );
				}
				else if( m_SkillMotion == SM_S )
				{
					pTrace8->SetSAttack( true );
				}
			}
			
		}
	}
}