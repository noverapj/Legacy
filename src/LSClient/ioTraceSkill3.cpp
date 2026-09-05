#include "stdafx.h"

#include "ioTraceSkill3.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioTraceSkill3::ioTraceSkill3()
{
	ClearData();
}

ioTraceSkill3::ioTraceSkill3( const ioTraceSkill3 &rhs )
	: ioAttackSkill( rhs ),
	m_vTargetAngleList( rhs.m_vTargetAngleList ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_iTargetLimitCount( rhs.m_iTargetLimitCount ),
	m_TargetEffect( rhs.m_TargetEffect ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_dwWaitTimeForAttack( rhs.m_dwWaitTimeForAttack ),
	m_fTargetOffSetRate( rhs.m_fTargetOffSetRate ),
	m_fTargetYOffSet( rhs.m_fTargetYOffSet ),
	m_LandAttack( rhs.m_LandAttack ),
	m_dwProtectionTime( rhs. m_dwProtectionTime ),
	m_dwEnableKeyInputTime( rhs.m_dwEnableKeyInputTime )
{
	ClearData();
}

ioTraceSkill3::~ioTraceSkill3()
{
}

void ioTraceSkill3::ClearData()
{
	m_State = SS_PRE;

	m_dwFireStartTime = 0;
	m_bSetHandMesh = false;
	m_bAttackedWeapon = false;
	m_bReduceGauge = false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	m_dwAttackEndTime = 0;

	m_fCurChargeRate = 0.0f;
	m_dwLoopStartTime = 0;
	m_iWeaponWay = 0;
	m_vAngleList.clear();
	m_vTargetAngleInfoList.clear();
}


void ioTraceSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = ""; 

	int iCnt = rkLoader.LoadInt_e( "target_angle_cnt", 0 );

	for( int i=0; i < iCnt; ++i )
	{
		TargetAngleInfo Info;
		wsprintf_e( szBuf, "target_min_angle%d", i+1 );
		Info.m_fMinAngle = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "target_max_angle%d", i+1 );
		Info.m_fMaxAngle = rkLoader.LoadFloat( szBuf, 0.0f );
		m_vTargetAngleList.push_back( Info );
	}

	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetMinRange = min( m_fTargetMinRange, m_fTargetRange );
	m_iTargetLimitCount = rkLoader.LoadInt_e( "target_limit_count", 0 );

	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_TargetEffect = szBuf;

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopMotion = szBuf;

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	
	//공격후 기달리는 시간
	m_dwWaitTimeForAttack = (DWORD)rkLoader.LoadInt_e( "wait_time_for_attack", 0 );
	
	//오프셋
	m_fTargetOffSetRate = rkLoader.LoadFloat_e("Target_OffSetRate", 0 );
	m_fTargetYOffSet = rkLoader.LoadFloat_e("Target_Y_OffSet", 0 );

	//랜드 어택
	WeaponInfo weapon;
	rkLoader.LoadString_e( "skill_land_attack_ani", "", szBuf, MAX_PATH );
	m_LandAttack.m_AttackAnimation = szBuf;
	weapon.m_iWeaponIdx = rkLoader.LoadInt_e( "skill_land_attack_type", 0);
	m_LandAttack.m_vWeaponInfoList.push_back( weapon );

	m_dwProtectionTime = (DWORD)rkLoader.LoadInt_e( "protection_time", 0 );

	m_dwEnableKeyInputTime = (DWORD)rkLoader.LoadInt_e( "enable_key_input_time", 0 );
}

ioSkill* ioTraceSkill3::Clone()
{
	return new ioTraceSkill3( *this );
}

bool ioTraceSkill3::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	ClearData();

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillProtection();

	CheckSkillDesc( pChar );
	//InitAngleList();
	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		SetActionState( pChar );

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioTraceSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_PRE:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				FindTarget( pChar );
				SetActionState( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			if ( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				AttackSkillFire( pChar );
				SetLoopState( pChar );
			}
			break;
		}
	case SS_LOOP:
		{
			if( m_dwLoopStartTime > 0 && m_dwLoopStartTime + m_dwWaitTimeForAttack < dwCurTime )
			{
				if( m_bAttackedWeapon )
				{
					if ( pChar->IsAttackKey() && pChar->IsNeedProcess() )
					{
						SetExtraActionState( pChar );
					}

					if ( m_dwLoopStartTime + m_dwWaitTimeForAttack + m_dwEnableKeyInputTime < dwCurTime )
					{
						m_State = SS_END;
					}
				}
				else
				{
					m_dwMotionEndTime = dwCurTime;
					m_State = SS_EXTRA_ACTION;
				}
			}
		}
		break;
	case SS_EXTRA_ACTION:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			m_State = SS_END;

		break;
	}
}


void ioTraceSkill3::OnSkillEnd( ioBaseChar *pChar )
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

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	ClearData();
}

bool ioTraceSkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTraceSkill3::SetActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_State = SS_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = -1;
	float fTimeRate  = FLOAT1;
	iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	fTimeRate = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime +=  pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	m_dwFireStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
}

void ioTraceSkill3::AttackSkillFire( ioBaseChar *pChar )
{
	int iTargetCnt = m_vTargetList.size();
	for(int i=0; i < iTargetCnt; ++i )
	{
		m_CurTargetName = m_vTargetList[i];

		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
			GetName(),
			m_dwWeaponBaseIndex + i,
			vPos);

	}

	int iAngleCnt = m_vTargetAngleInfoList.size();
	for( int i = 0; i < iAngleCnt; ++i )
	{
		m_CurTargetName.Clear();
		float Angle = ( m_vTargetAngleInfoList[i].m_fMaxAngle + m_vTargetAngleInfoList[i].m_fMinAngle ) * FLOAT05;
		m_vAngleList.push_back( Angle );
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

		pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
			GetName(),
			m_dwWeaponBaseIndex + i + 10 ,
			vPos);
	}

	m_dwFireStartTime = 0;
}

void ioTraceSkill3::SetLoopState( ioBaseChar* pChar )
{
	if( !pChar )
		return;

	m_State = SS_LOOP;
	m_dwLoopStartTime = FRAMEGETTIME();
}

D3DXVECTOR3 ioTraceSkill3::CalcReturnUroborusPosition( ioWeapon *pWeapon )
{
	m_bAttackedWeapon = true;

	ioBaseChar *pChar = pWeapon->GetOwner();

	D3DXVECTOR3 vOffSet = pChar->GetWorldPosition() - pWeapon->GetPosition();
	vOffSet.y = 0;
	D3DXVec3Normalize( &vOffSet, &vOffSet );
	vOffSet = vOffSet * m_fTargetOffSetRate;

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vReturnPos;
	vReturnPos = pChar->GetWorldPosition() + D3DXVECTOR3(0, m_fTargetYOffSet, 0 ) - vOffSet;

	return vReturnPos;
}

bool ioTraceSkill3::IsProtected( int iDefenseBreakType ) const
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
	case SS_PRE:
		break;
	case SS_ACTION:
	case SS_LOOP:
	case SS_EXTRA_ACTION:
		if( m_dwSkillStartTime + m_dwProtectionTime > dwCurTime )
			return true;
		break;
	}

	return false;
}

bool ioTraceSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_PRE:
	case SS_ACTION:
	case SS_LOOP:
	case SS_EXTRA_ACTION:
		return false;
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
			return true;
		break;
	}

	return false;
}

void ioTraceSkill3::SetWarpTargetList( const ioHashString &szTargetName )
{
	if( szTargetName.IsEmpty() )	return;

	ioHashStringVec::iterator iter = std::find( m_vWarpTargetList.begin(),
		m_vWarpTargetList.end(),
		szTargetName );

	if( iter == m_vWarpTargetList.end() )
		m_vWarpTargetList.push_back( szTargetName );
}


void ioTraceSkill3::FindTarget( ioBaseChar *pOwner )
{
	m_vWarpTargetList.clear();

	ioHashString szTarget;

	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	m_vTargetAngleInfoList = m_vTargetAngleList;
	// target list
	m_vTargetList.clear();
	
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;

		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar, true ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;

		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );

		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &vDiff );
		D3DXVec3Normalize( &vAxis, &vAxis );

		if ( vAxis.y >= 0.0f )
		{
			auto Iter = m_vTargetAngleInfoList.begin();
			while ( Iter != m_vTargetAngleInfoList.end() )
			{
				TargetAngleInfo AngleInfo = (*Iter);
				if ( COMPARE( fAngleGap, AngleInfo.m_fMinAngle, AngleInfo.m_fMaxAngle ) )
				{
					pCurChar->AttachEffect( m_TargetEffect );
					szTarget = pCurChar->GetCharName();
					m_vTargetList.push_back(szTarget);
					Iter = m_vTargetAngleInfoList.erase( Iter );
				}
				else
				{
					++Iter;
				}
			}
		}

		if ( vAxis.y < 0.0f )
		{
			fAngleGap = 360.0f - fAngleGap;
			auto Iter = m_vTargetAngleInfoList.begin();
			while ( Iter != m_vTargetAngleInfoList.end() )
			{
				TargetAngleInfo AngleInfo = (*Iter);
				if ( COMPARE( fAngleGap, AngleInfo.m_fMinAngle, AngleInfo.m_fMaxAngle ) )
				{
					pCurChar->AttachEffect( m_TargetEffect );
					szTarget = pCurChar->GetCharName();
					m_vTargetList.push_back(szTarget);
					Iter = m_vTargetAngleInfoList.erase( Iter );
				}
				else
				{
					++Iter;
				}
			}
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		if( !m_vTargetList.empty() )
		{
			int iSize = m_vTargetList.size();

			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_TARGET;
			kPacket << iSize;

			for( int i=0; i < iSize; ++i )
				kPacket << m_vTargetList[i];

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTraceSkill3::SetExtraActionState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	float fAniRate = m_LandAttack.m_fAttackAniRate;
	if ( fAniRate <= 0.0f )
		fAniRate = FLOAT1;	

	int iAniID = pGrp->GetAnimationIdx( m_LandAttack.m_AttackAnimation );

	if( iAniID != -1 )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	
	m_dwFireStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fAniRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	DWORD dwCurBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_LandAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fAniRate,
		0 );

	m_State = SS_EXTRA_ACTION;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_EXTRA_ACTION;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioTraceSkill3::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() )
		return false;

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioTraceSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	//
	ioUroborusWeapon3 *pUroborus3 = ToUroborusWeapon3( pWeapon );
	if( pUroborus3 )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			if( !m_CurTargetName.IsEmpty() )
			{
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
				D3DXVec3Normalize( &vDir, &vDir );

				pUroborus3->SetTraceTargetName( m_CurTargetName );
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_CurTargetName );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetMidPositionByRate() - pChar->GetMidPositionByRate(0.8);
					D3DXVec3Normalize( &vDir, &vDir );

					pUroborus3->SetOneTimeColCheck( true );
					pUroborus3->SetMoveDir( vDir );
					pUroborus3->SetSkillWeapon( true );
				}
			}
			else
			{
				float fCurAngle = 0.0f;
				int iMaxAngle = m_vAngleList.size();
				if ( iMaxAngle > m_iWeaponWay )
				{
					fCurAngle = m_vAngleList[m_iWeaponWay];

					fCurAngle = ioMath::ArrangeHead( fCurAngle );

					D3DXVECTOR3 vMoveDir;
					D3DXQUATERNION qtAngle;
					D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

					D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
					vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					vMoveDir = qtAngle * vMoveDir;
					D3DXVec3Normalize( &vMoveDir, &vMoveDir );

					pUroborus3->SetMoveDir( vMoveDir );
					pUroborus3->SetSkillWeapon( true );
					m_iWeaponWay++;
				}				
			}

		}
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( pZone->GetPosition(), true );
		}
	}
}

void ioTraceSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_TARGET:
		{
			int iSize;
			rkPacket >> iSize;

			m_vTargetList.clear();
			for( int i=0; i < iSize; ++i )
			{
				ioHashString szTargetName;
				rkPacket >> szTargetName;

				m_vTargetList.push_back( szTargetName );
			}
		}
		break;
	case SSC_EXTRA_ACTION:
		{
			SetExtraActionState( pChar );
		}
		break;
	}

}

bool ioTraceSkill3::IsAttackEndState() const
{
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime <= FRAMEGETTIME() )
		return true;

	return false;
}
