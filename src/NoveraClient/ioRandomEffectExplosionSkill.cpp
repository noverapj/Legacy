#include "StdAfx.h"
#include "ioRandomEffectExplosionSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioRandomEffectExplosionSkill::ioRandomEffectExplosionSkill(void)
{
	ClearData();
}

ioRandomEffectExplosionSkill::ioRandomEffectExplosionSkill( const ioRandomEffectExplosionSkill &rhs )
	: ioAttackSkill( rhs ),
	m_szLoopAni( rhs.m_szLoopAni ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_dwKeyAddDuration( rhs.m_dwKeyAddDuration ),
	m_dwKeyEnableTime( rhs.m_dwKeyEnableTime ),
	m_szSkillWeaponEffect( rhs.m_szSkillWeaponEffect ),
	m_fCollisionRange( rhs.m_fCollisionRange ),
	m_WeaponInfo( rhs.m_WeaponInfo ),
	m_AreaWeaponName( rhs.m_AreaWeaponName )
{
	ClearData();
}


ioRandomEffectExplosionSkill::~ioRandomEffectExplosionSkill(void)
{
}

void ioRandomEffectExplosionSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );

	m_dwKeyAddDuration	= (DWORD)rkLoader.LoadInt_e( "key_add_duration", 0 );
	m_dwKeyEnableTime	= (DWORD)rkLoader.LoadInt_e( "key_enable_time", 0 );

	rkLoader.LoadString_e( "skill_weapon_effect", "", szBuf, MAX_PATH );
	m_szSkillWeaponEffect = szBuf;

	m_fCollisionRange = rkLoader.LoadFloat_e( "collision_range", 0.0f );
		
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_type", NO_WEAPON_ATTRIBUTE );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "attack_type_wounded", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = (DWORD)rkLoader.LoadInt_e( "attack_type_wounded_time", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_type_loop_ani", false );

	// areaweapon
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

}

ioSkill* ioRandomEffectExplosionSkill::Clone()
{
	return new ioRandomEffectExplosionSkill( *this );
}

void ioRandomEffectExplosionSkill::ClearData()
{
	m_SkillState = SS_PRE;
	m_dwStartLoopTime = 0;
	m_dwKeyCheckStartTime = 0;

	m_bAddKey = false;
	m_vWeaponEffectIDList.clear();
	m_iCurCombo = 0;
	m_vEffectPosList.clear();
	m_dwFireStartTime = 0;
	m_bEnd = false;
}

bool ioRandomEffectExplosionSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	SetCameraBuff( pChar );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );	
	
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}
	//무기 변경
	if( m_bUseItemMesh && !m_bSetHandMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	m_iCurActiveCount = m_iMaxActiveCount;
	
	return true;
}

void ioRandomEffectExplosionSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				CreateAreaWeapon( pChar );
				//루프 상태로 전환
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwStartLoopTime > 0 && m_dwStartLoopTime + m_dwLoopDuration < dwCurTime )
			{
				SetFinishState( pChar );
			}

			if ( m_iCurActiveCount == 0 )
			{
				SetFinishState( pChar );
			}
			
			if( pChar->IsNeedProcess() )
			{
				DWORD dwCheck1 = m_dwKeyCheckStartTime + m_dwKeyEnableTime;
				DWORD dwCheck2 = m_dwKeyCheckStartTime + m_dwKeyAddDuration;

				if ( !m_bAddKey && dwCheck1 < dwCheck2 )
				{
					if( m_dwKeyCheckStartTime > 0 && COMPARE( dwCurTime, dwCheck1, dwCheck2 ) )
					{
						if( pChar->IsAttackKey() )
						{
							m_bAddKey = true;
						}
					}
				}

				if( dwCheck2 < dwCurTime )
				{
					if( m_bAddKey )
					{
						m_bAddKey = false;
						m_dwKeyCheckStartTime = dwCurTime;
						RandomPosCreateEffect( pChar );
					}
					else
					{
						SetFinishState( pChar );
						if( pChar->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
							kPacket << pChar->GetCharName();
							kPacket << GetName();
							kPacket << SS_FINISH;
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
					}
				}
			}
		}
		break;
	case SS_FINISH:
		{
			if ( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				EffectExplosion( pChar );
				m_dwFireStartTime = 0;
			}

			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				m_SkillState = SS_END;
			}
		}
		break;
	case SS_END:
		{
			m_bEnd = true;
		}
		break;
	}	
}

void ioRandomEffectExplosionSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	EffectExplosion( pChar );
	ClearData();
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	if( m_dwAreaWeaponIndex > 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
		m_dwAreaWeaponIndex = 0;
	}
}

void ioRandomEffectExplosionSkill::SetLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( iAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwStartLoopTime = dwCurTime;
	// 연타 체크용.
	m_dwKeyCheckStartTime = dwCurTime;

	m_SkillState = SS_LOOP;	
}

void ioRandomEffectExplosionSkill::CreateEffect( ioBaseChar *pChar, D3DXVECTOR3 vPos )
{
	if ( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect *pEffect = pStage->CreateMapEffect( m_szSkillWeaponEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ));
	if ( pEffect )
	{
		m_vWeaponEffectIDList.push_back( pEffect->GetUniqueID() );
	}
}

void ioRandomEffectExplosionSkill::DestroyEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	auto Iter = m_vWeaponEffectIDList.begin();
	for ( ; Iter != m_vWeaponEffectIDList.end(); ++Iter )
	{
		UniqueObjID dwID = (*Iter);
		pChar->EndMapEffect( dwID, false );
	}
}

void ioRandomEffectExplosionSkill::RandomPosCreateEffect( ioBaseChar *pChar )
{
	if ( !pChar ) return;
	
	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	auto RandomSeed = pChar->GetRandomSeed();
	IORandom random;
	random.SetRandomSeed( RandomSeed + m_iCurCombo );

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

	int iValue = m_fCollisionRange * 2;
	int iWidth1 = random.Random( iValue );
	int iWidth2 = random.Random( iValue );
	int iWidth3 = random.Random( iValue );
	
	iWidth1 -= static_cast<int>( m_fCollisionRange );
	iWidth2 -= static_cast<int>( m_fCollisionRange );
	iWidth3 -= static_cast<int>( m_fCollisionRange );

	vPos.x += iWidth1;
	vPos.y += iWidth2;
	vPos.z += iWidth3;

	if ( vPos.y < pStage->GetMapHeight( vPos.x, vPos.z ) )
		vPos.y = pStage->GetMapHeight( vPos.x, vPos.z );

	CreateEffect( pChar, vPos );
	m_vEffectPosList.push_back( vPos );

	m_iCurCombo++;
	m_iCurActiveCount--;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_LOOP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioRandomEffectExplosionSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	//if( m_SkillState == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
	//	return true;

	return false;
}

void ioRandomEffectExplosionSkill::EffectExplosion( ioBaseChar* pChar )
{
	if( !pChar )	return;
	
	DestroyEffect( pChar );
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	auto Iter = m_vEffectPosList.begin();
	for ( ;Iter != m_vEffectPosList.end(); ++Iter )
	{
		pChar->SkillFireExplicit( m_WeaponInfo,	GetName(),
			m_dwWeaponBaseIndex + m_iCurCombo, (*Iter) );
	}
	m_vEffectPosList.clear();
}

void ioRandomEffectExplosionSkill::SetFinishState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_FINISH;	

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwFireStartTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetSkillMotionRate();
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, 0, true );

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
}

bool ioRandomEffectExplosionSkill::IsEndState() const
{
	if( m_SkillState == SS_END && m_bEnd )
		return true;

	return false;
}

bool ioRandomEffectExplosionSkill::IsAttackEndState() const
{
	return IsEndState();
}


void ioRandomEffectExplosionSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioRandomEffectExplosionSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SS_LOOP:
		{
			RandomPosCreateEffect( pChar );
		}
		break;
	case SS_FINISH:
		{
			SetFinishState( pChar );
		}
		break;
	}
}

void ioRandomEffectExplosionSkill::CreateAreaWeapon( ioBaseChar *pChar )
{
	if( m_AreaWeaponName.IsEmpty() ) return;
	if( !pChar ) return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
		pChar->GetCharName(),
		GetName(),
		m_AreaWeaponName,
		vPos,
		pChar->GetTargetRot(),
		ioAreaWeapon::CT_ATTACHED );

	if( pArea )
	{
		m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
	}
}

