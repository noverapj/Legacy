#include "StdAfx.h"
#include "ioBindingFireAttackSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"

ioBindingFireAttackSkill::ioBindingFireAttackSkill(void)
{
}

ioBindingFireAttackSkill::ioBindingFireAttackSkill( const ioBindingFireAttackSkill &rhs ):
ioAttackSkill( rhs ),
m_dwProtectTime( rhs.m_dwProtectTime ),
m_PreAttackAttribute( rhs.m_PreAttackAttribute ),
m_BindingAttackAttribute( rhs.m_BindingAttackAttribute ),
m_BindingWeaponInfo( rhs.m_BindingWeaponInfo ),
m_AttackKeyFireAttackAttribute( rhs.m_AttackKeyFireAttackAttribute),
m_DefenseKeyFireAttackAttribute( rhs.m_DefenseKeyFireAttackAttribute ),
m_dwAttackKeyFireAttackWeaponLiveTime( rhs.m_dwAttackKeyFireAttackWeaponLiveTime ),
m_dwDefenseKeyFireAttackWeaponLiveTime( rhs.m_dwDefenseKeyFireAttackWeaponLiveTime ),
m_fAttackKeyFireAttackWeaponSpeed( rhs.m_fAttackKeyFireAttackWeaponSpeed),
m_fDefenseKeyFireAttackWeaponSpeed( rhs.m_fDefenseKeyFireAttackWeaponSpeed ),
m_fDefenseKeyFireWeaponAngle( rhs.m_fDefenseKeyFireWeaponAngle ),
m_fAttackKeyFireWeaponAngle( rhs.m_fAttackKeyFireWeaponAngle ),
m_vCheckBuffList( rhs.m_vCheckBuffList ),
m_vAttackKeyFireAttackBuffList( rhs.m_vAttackKeyFireAttackBuffList ),
m_vDefenseKeyFireAttackBuffList( rhs.m_vDefenseKeyFireAttackBuffList )
{

}

ioBindingFireAttackSkill::~ioBindingFireAttackSkill(void)
{
}

ioSkill* ioBindingFireAttackSkill::Clone()
{
	return new ioBindingFireAttackSkill( *this );
}

void ioBindingFireAttackSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_dwFireStartTime = 0;
	m_dwProtectEndTime = 0;
	m_dwFireWeaponIndex = 0;
	m_vFireAttackWeaponDir = ioMath::VEC3_ZERO;
	m_dwCurFireAttackWeaponLiveTime = 0;
	m_fCurFireAttackWeaponSpeed = 0;
}

void ioBindingFireAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH],szKey[MAX_PATH];

	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute_e( "pre_attack", m_PreAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "binding_attack", m_BindingAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "attack_key_fire_attack", m_AttackKeyFireAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "defense_key_fire_attack", m_DefenseKeyFireAttackAttribute, rkLoader );
	
	m_BindingWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "binding_weapon", NO_WEAPON_ATTRIBUTE );
	m_BindingWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "binding_weapon_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "binding_weapon_wounded", "", szBuf, MAX_PATH );
	m_BindingWeaponInfo.m_WoundedAnimation = szBuf;
	m_BindingWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "binding_weapon_wounded_time", 0 );
	m_BindingWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "binding_weapon_wounded_loop_ani", false );

	m_dwDefenseKeyFireAttackWeaponLiveTime = (DWORD)rkLoader.LoadInt_e( "defense_key_fire_attack_weapon_live_time", 0 );
	m_dwAttackKeyFireAttackWeaponLiveTime = (DWORD)rkLoader.LoadInt_e( "attack_key_fire_attack_weapon_live_time", 0 );
	m_fDefenseKeyFireAttackWeaponSpeed = rkLoader.LoadFloat_e( "defense_key_fire_attack_weapon_speed", 0.0f );
	m_fAttackKeyFireAttackWeaponSpeed = rkLoader.LoadFloat_e( "attack_key_fire_attack_weapon_speed", 0.0f );
	m_fDefenseKeyFireWeaponAngle = rkLoader.LoadFloat_e( "defense_key_weapon_angle", 0.0f );
	m_fAttackKeyFireWeaponAngle = rkLoader.LoadFloat_e( "attack_key_weapon_angle", 0.0f );

	//Buff
	int iBuffCount = rkLoader.LoadInt_e( "check_buff_count", 0 );
	if ( iBuffCount > 0 )
	{
		m_vCheckBuffList.clear();
		m_vCheckBuffList.reserve( iBuffCount );
		for ( int i=0; i<iBuffCount ; ++i )
		{
			ioHashString szBuffName;
			sprintf_e( szKey, "check_buff%d", i + 1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szBuffName = szBuf;
			m_vCheckBuffList.push_back( szBuffName );
			
		}
	}

	iBuffCount = rkLoader.LoadInt_e( "attack_key_fire_attack_buff_count", 0 );
	if ( iBuffCount > 0 )
	{
		m_vAttackKeyFireAttackBuffList.clear();
		m_vAttackKeyFireAttackBuffList.reserve( iBuffCount );
		for ( int i=0; i<iBuffCount ; ++i )
		{
			ioHashString szBuffName;
			sprintf_e( szKey, "attack_key_fire_attack_buff%d", i + 1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szBuffName = szBuf;
			m_vAttackKeyFireAttackBuffList.push_back( szBuffName );

		}
	}

	iBuffCount = rkLoader.LoadInt_e( "defense_key_fire_attack_buff_count", 0 );
	if ( iBuffCount > 0 )
	{
		m_vDefenseKeyFireAttackBuffList.clear();
		m_vDefenseKeyFireAttackBuffList.reserve( iBuffCount );
		for ( int i=0; i<iBuffCount ; ++i )
		{
			ioHashString szBuffName;
			sprintf_e( szKey, "defense_key_fire_attack_buff%d", i + 1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szBuffName = szBuf;
			m_vDefenseKeyFireAttackBuffList.push_back( szBuffName );

		}
	}
}

bool ioBindingFireAttackSkill::OnSkillStart( ioBaseChar *pOwner )
{
	if ( !ioAttackSkill::OnSkillStart( pOwner ) )
		return false;

	m_bSetHandMesh = false;
	SetCameraBuff( pOwner );
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}

	ClearData();

	return true;
}

void ioBindingFireAttackSkill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pOwner );
				SetPreAttackState( pOwner );
			}
		}
		break;
	case SS_PRE_ATTACK:
		{
			if ( pOwner->IsNeedProcess() && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetBindingAttackState( pOwner );
				return;
			}
		}
		break;
	case SS_BINDING:
		{
			if (  pOwner->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < FRAMEGETTIME() )
				{
					SetEndState( pOwner, true );
					return;
				}

				if ( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
				{
					m_dwFireStartTime = 0;
					FireBindingWeapon( pOwner );
				}

				if ( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < FRAMEGETTIME() )
				{
					if ( pOwner->IsAttackKey() )
					{
						SetFireState( pOwner, pStage, KS_ATTACK );
						return;
					}
					else if ( pOwner->IsDefenseKey() )
					{
						SetFireState( pOwner, pStage, KS_DEFENSE );
						return;
					}
				}
			}			
		}
		break;
	case SS_FIRE:
		{
			if ( pOwner->IsNeedProcess() && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetEndState( pOwner, false );
				return;
			}

			if ( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
			{
				m_dwFireStartTime = 0;
				ShootWeapon( pOwner );
			}
		}
		break;
	}
}

void ioBindingFireAttackSkill::SetPreAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	m_SkillState = SS_PRE_ATTACK;
	SetActionAni( m_PreAttackAttribute, pOwner, !m_PreAttackAttribute.m_vWeaponInfoList.empty() );
}

void ioBindingFireAttackSkill::SetBindingAttackState( ioBaseChar *pOwner )
{
	m_SkillState = SS_BINDING;
	if ( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
	}

	SetActionAni( m_BindingAttackAttribute, pOwner, !m_BindingAttackAttribute.m_vWeaponInfoList.empty() );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_BINDING;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBindingFireAttackSkill::SetActionAni( const AttackAttribute& rkAttri, ioBaseChar *pOwner, bool bRefreshFireTimeList )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( rkAttri.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = rkAttri.m_fAttackAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = rkAttri.m_dwPreDelay;

	if( bRefreshFireTimeList )
	{
		pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			rkAttri.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + rkAttri.m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = dwCurTime + dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	// Col Skip
	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime = pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;
	if ( m_dwFireMotionEndTime > 0 )
		m_dwFireMotionEndTime += dwCurTime + dwPreDelay;

	m_dwEnableReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwEnableReserveTime > 0 )
		m_dwEnableReserveTime += dwPreDelay + dwCurTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->SetReservedSliding( rkAttri.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

void ioBindingFireAttackSkill::SetEndState( ioBaseChar *pOwner, bool bDestroyWeapon )
{
	m_SkillState = SS_END;

	if ( bDestroyWeapon )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwFireWeaponIndex );
		if ( pWeapon )
			pWeapon->SetWeaponDead();

		m_dwFireWeaponIndex = 0;
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_END;
		kPacket << bDestroyWeapon;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioBindingFireAttackSkill::FireBindingWeapon( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	m_dwFireWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_BindingWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_BindingWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_BindingWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_BindingWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_BindingWeaponInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = m_dwFireWeaponIndex;
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;
	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if ( pZone )
		{
			ioBaseChar *pChar = pZone->GetOwner();
			if( pChar )
			{
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( false, false );
			}
		}
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_BINDING_FIRE;
		kPacket << vPos;
		kPacket << vDir;
		kPacket << m_dwFireWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBindingFireAttackSkill::SetFireState( ioBaseChar *pOwner, ioPlayStage *pStage, KeyState CurKeyState )
{
	if ( !pOwner || !pStage )
		return;
	
	m_SkillState = SS_FIRE;

	AttackAttribute& rkInfo = GetAttribute();
	
	if ( CurKeyState == KS_ATTACK )
		rkInfo = m_AttackKeyFireAttackAttribute;
	else if ( CurKeyState == KS_DEFENSE )
		rkInfo = m_DefenseKeyFireAttackAttribute;

	SetActionAni( rkInfo, pOwner, !rkInfo.m_vWeaponInfoList.empty() );

	SetFireDir( pOwner, CurKeyState );

	if ( !m_vCheckBuffList.empty() )
		AddFireBuff( pOwner, pStage, CurKeyState );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_FIRE;
		kPacket << CurKeyState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBindingFireAttackSkill::SetFireDir( ioBaseChar *pOwner, KeyState CurKeyState )
{
	float fAngle = 0.0f;
	if ( CurKeyState == KS_ATTACK )
	{
		fAngle = m_fAttackKeyFireWeaponAngle;
		m_fCurFireAttackWeaponSpeed = m_fAttackKeyFireAttackWeaponSpeed;
		m_dwCurFireAttackWeaponLiveTime = m_dwAttackKeyFireAttackWeaponLiveTime;
	}
	else if ( CurKeyState == KS_DEFENSE )
	{
		fAngle = m_fDefenseKeyFireWeaponAngle;
		m_fCurFireAttackWeaponSpeed = m_fDefenseKeyFireAttackWeaponSpeed;
		m_dwCurFireAttackWeaponLiveTime = m_dwDefenseKeyFireAttackWeaponLiveTime;
	}

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( fAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	m_vFireAttackWeaponDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &m_vFireAttackWeaponDir, &m_vFireAttackWeaponDir );
}

void ioBindingFireAttackSkill::AddFireBuff( ioBaseChar *pOwner, ioPlayStage *pStage, KeyState CurKeyState )
{
	if ( !pOwner || !pStage )
		return;

	ioHashStringVec vFireBuffList;
	if ( CurKeyState == KS_ATTACK )
		vFireBuffList = m_vAttackKeyFireAttackBuffList;
	else if ( CurKeyState == KS_DEFENSE )
		vFireBuffList = m_vDefenseKeyFireAttackBuffList;

	if ( vFireBuffList.empty() )
		return;


	ioHashString szItemName;
	if ( GetOwnerItem() )
		szItemName = GetOwnerItem()->GetName();
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

		bool bIsCheck=false;
		for ( int j=0; j<(int)m_vCheckBuffList.size() ;++j )
		{
			if ( pTarget->HasBuff( pOwner->GetCharName(), m_vCheckBuffList[j] ) )
				bIsCheck = true;
		}

		if ( bIsCheck )
		{
			for ( int j=0; j<(int)vFireBuffList.size() ;++j )
				pTarget->AddNewBuff( vFireBuffList[j], pOwner->GetCharName(), szItemName, this );
		}
	}
}

void ioBindingFireAttackSkill::ShootWeapon( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwFireWeaponIndex );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if ( pZone )
	{
		DWORD dwEndLiveTime = FRAMEGETTIME() - pZone->GetCreateTime();
		dwEndLiveTime += m_dwCurFireAttackWeaponLiveTime;
		D3DXVec3Normalize( &m_vFireAttackWeaponDir, &m_vFireAttackWeaponDir );
		pZone->SetCollisionDir( m_vFireAttackWeaponDir );
		pZone->SetLiveTime( dwEndLiveTime );
		pZone->SetZoneValue( ioMath::VEC3_ZERO, false );
		pZone->SetMoveSpeed( m_fCurFireAttackWeaponSpeed );
	}
}

bool ioBindingFireAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;

		if ( m_dwProtectEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioBindingFireAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END )
		return true;

	return false;
}

void ioBindingFireAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pOwner || !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SST_BINDING:
		{
			D3DXQUATERNION qRot;
			rkPacket >> qRot;
			pOwner->SetTargetRotToRotate( qRot, true, true );
			SetBindingAttackState( pOwner );
		}
		break;
	case SST_BINDING_FIRE:
		{
			D3DXVECTOR3 vPos,vDir;
			rkPacket >> vPos;
			rkPacket >> vDir;
			rkPacket >> m_dwFireWeaponIndex;
			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.iAttributeIdx = m_BindingWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = m_BindingWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni  = m_BindingWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = m_BindingWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = m_BindingWeaponInfo.m_bWoundedLoopAni;
			kFireTime.dwWeaponIdx = m_dwFireWeaponIndex;
			kFireTime.eFireTimeType = FTT_SKILL_ATTACK;
			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( pWeapon )
			{
				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
				if ( pZone )
				{
					ioBaseChar *pChar = pZone->GetOwner();
					if( pChar )
					{
						D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
						pZone->SetCollisionDir( vDir );
						pZone->SetZoneValue( false, false );
					}
				}
			}
		}
		break;
	case SST_FIRE:
		{
			int iKeyType;
			rkPacket >> iKeyType;
			SetFireState( pOwner, pStage, (KeyState)iKeyType );
		}
		break;
	case SST_END:
		{
			bool bDestroyWeapon;
			rkPacket >> bDestroyWeapon;
			SetEndState( pOwner, bDestroyWeapon );
		}
	}
}

void ioBindingFireAttackSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillEnd( pOwner );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	m_bSetHandMesh = false;

	EndCameraBuff( pOwner );

	ClearData();
}
