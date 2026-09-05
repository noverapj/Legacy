#include "StdAfx.h"
#include "ioCreateTicWeaponSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioCreateTicWeaponSkill::ioCreateTicWeaponSkill(void)
{
	ClearData();
}

ioCreateTicWeaponSkill::ioCreateTicWeaponSkill( const ioCreateTicWeaponSkill &rhs ):
ioAttackSkill( rhs ),
//Loop
m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
m_fLoopDelayAniRate( rhs.m_fLoopDelayAniRate ),
m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
m_fLoopMoveAniRate( rhs.m_fLoopMoveAniRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
//Effect
m_szWeaponEffectName( rhs.m_szWeaponEffectName ),
m_dwTicEffectCreateTicTime( rhs.m_dwTicEffectCreateTicTime ),
m_vStartEffectOffsetList( rhs.m_vStartEffectOffsetList ),
m_vTicEffectOffsetList( rhs.m_vTicEffectOffsetList ),
//Weapon
m_vWeaponInfoList( rhs.m_vWeaponInfoList ),
//Buff
m_OwnerBuffList( rhs.m_OwnerBuffList )
{
	ClearData();
}

ioCreateTicWeaponSkill::~ioCreateTicWeaponSkill(void)
{
}

ioSkill* ioCreateTicWeaponSkill::Clone()
{
	return new ioCreateTicWeaponSkill( *this );
}

void ioCreateTicWeaponSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	//Loop
	m_LoopDelayAnimation.Clear();
	rkLoader.LoadString_e( "loop_delay_animation", "", szBuf, MAX_PATH );
	m_LoopDelayAnimation = szBuf;
	m_fLoopDelayAniRate = rkLoader.LoadFloat_e("loop_delay_ani_rate", FLOAT1 );
	m_LoopMoveAnimation.Clear();
	rkLoader.LoadString_e( "loop_move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;
	m_fLoopMoveAniRate = rkLoader.LoadFloat_e("loop_move_ani_rate", FLOAT1 );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//Effcet
	rkLoader.LoadString_e( "weapon_effect_name", "", szBuf, MAX_PATH );
	m_szWeaponEffectName = szBuf;
	m_dwTicEffectCreateTicTime = (DWORD)rkLoader.LoadInt_e( "tic_effect_create_tic_time", 0 );
	
	int iMaxOffsetCount = rkLoader.LoadInt_e( "max_start_effect_offset_count", 0 );
	if ( iMaxOffsetCount > 0 )
	{
		m_vStartEffectOffsetList.clear();
		m_vStartEffectOffsetList.reserve( iMaxOffsetCount );
		for ( int i=0 ; i<iMaxOffsetCount ; ++i )
		{
			D3DXVECTOR3 vOffset;
			wsprintf_e( szKey, "start_effect_offset%d_x", i+1 );
			vOffset.x = rkLoader.LoadInt( szKey, 0 );
			wsprintf_e( szKey, "start_effect_offset%d_y", i+1 );
			vOffset.y = rkLoader.LoadInt( szKey, 0 );
			wsprintf_e( szKey, "start_effect_offset%d_z", i+1 );
			vOffset.z = rkLoader.LoadInt( szKey, 0 );

			m_vStartEffectOffsetList.push_back( vOffset );
		}
	}

	iMaxOffsetCount = rkLoader.LoadInt_e( "max_tic_effect_offset_count", 0 );
	if ( iMaxOffsetCount > 0 )
	{
		m_vTicEffectOffsetList.clear();
		m_vTicEffectOffsetList.reserve( iMaxOffsetCount );
		for ( int i=0 ; i<iMaxOffsetCount ; ++i )
		{
			D3DXVECTOR3 vOffset;
			wsprintf_e( szKey, "tic_effect_offset%d_x", i+1 );
			vOffset.x = rkLoader.LoadInt( szKey, 0 );
			wsprintf_e( szKey, "tic_effect_offset%d_y", i+1 );
			vOffset.y = rkLoader.LoadInt( szKey, 0 );
			wsprintf_e( szKey, "tic_effect_offset%d_z", i+1 );
			vOffset.z = rkLoader.LoadInt( szKey, 0 );

			m_vTicEffectOffsetList.push_back( vOffset );
		}
	}
	
	//Weapon
	int iAttackTypeCnt = rkLoader.LoadInt( "attack_type_count", 0 );
	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vWeaponInfoList.push_back( kInfo );
	}

	//Buff
	LoadBuffList( rkLoader );
	
	
}


void ioCreateTicWeaponSkill::LoadBuffList( ioINILoader &rkLoader )
{
	ioBuffInfo BuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 )
		return;
	m_OwnerBuffList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuff;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}


bool ioCreateTicWeaponSkill::OnSkillStart( ioBaseChar *pOwner )
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

	AddOwnerBuff( pOwner );

	ClearData();

	return true;
}

void ioCreateTicWeaponSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_bSetMoveAni = false;

	m_dwLoopStartTime = 0;
	m_dwLoopProtectEndTime = 0;

	m_vCurWeaponEffectList.clear();

	m_vNextTicEffectCreateTime = 0;
	m_iTicEffectCurCount = 0;
	m_bInputAttackKey = false;
}

void ioCreateTicWeaponSkill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
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
				EndPreDelayEffect( pOwner );
				SetLoopState( pOwner );
				CreateStartCallWeapon( pOwner );
			}
		}
		break;
	case SS_LOOP:
		{
			if ( pOwner->IsNeedProcess() && m_dwLoopDuration > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetFireState( pOwner );
				return;
			}

			CheckNormalMoveAni( pOwner );
			UpdateWeaponEffect( pOwner, pStage );

			if ( pOwner->IsNeedProcess() )
			{
				if ( m_bInputAttackKey )
				{
					if( pOwner->IsAttackKeyDown() )
					{
						if ( m_dwTicEffectCreateTicTime > 0 && m_vNextTicEffectCreateTime < dwCurTime )
						{
							m_vNextTicEffectCreateTime = dwCurTime + m_dwTicEffectCreateTicTime;
							CreateTicWeapon( pOwner );
						}
					}
					else
						SetFireState( pOwner );
				}
				else
				{
					if ( pOwner->IsAttackKey() )
						m_bInputAttackKey = true;
				}
			}
		}
		break;
	case SS_FIRE:
		{
			if ( m_dwMotionEndTime < FRAMEGETTIME() )
			{
				m_SkillState = SS_END;
				return;
			}
			if ( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
				FireEffectWeapon( pOwner, pStage );
			}
		}
		break;
	}
}

void ioCreateTicWeaponSkill::SetLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fLoopDelayAniRate );
	m_dwMotionEndTime = 0;

	pOwner->SetSkillMoveEnable( true, true );

	m_bSetMoveAni = false;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = m_vNextTicEffectCreateTime = m_dwLoopProtectEndTime = dwCurTime;
	m_dwLoopProtectEndTime += m_dwLoopProtectTime;
	m_vNextTicEffectCreateTime += m_dwTicEffectCreateTicTime;
	
}

void ioCreateTicWeaponSkill::CreateStartCallWeapon( ioBaseChar* pOwner )
{
	if ( !pOwner || m_vStartEffectOffsetList.empty() )
		return;

	if ( m_szWeaponEffectName.IsEmpty() )
		return;

	int iEffectSize = m_vStartEffectOffsetList.size();
	
	D3DXVECTOR3	vPos = pOwner->GetMidPositionByRate();
	D3DXQUATERNION	qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vScale = pOwner->GetWorldScale();

	for ( int i=0 ; i<iEffectSize ; ++i )
	{
		D3DXVECTOR3 vEffectPos = vPos + qtRot * m_vStartEffectOffsetList[i];
		ioMapEffect *pMapEffect = pOwner->CreateMapEffectBySkill( m_szWeaponEffectName, vEffectPos, vScale );
		if( pMapEffect )
		{
			WeaponEffect kInfo;
			kInfo.vOffset = m_vStartEffectOffsetList[i];
			kInfo.dwEffectID = pMapEffect->GetUniqueID();
			m_vCurWeaponEffectList.push_back( kInfo );
		}
	}
}

void ioCreateTicWeaponSkill::CheckNormalMoveAni( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsSettedDirection() )
	{	
		if( !m_bSetMoveAni )
		{	
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if ( !pGrp )
				return;
			int iAniID = pGrp->GetAnimationIdx( m_LoopMoveAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1/m_fLoopMoveAniRate );	
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if ( !pGrp )
				return;
			int iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1/m_fLoopDelayAniRate );
			m_bSetMoveAni = false;
		}
	}
}

void ioCreateTicWeaponSkill::CreateTicWeapon( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	if ( m_vTicEffectOffsetList.empty() )
		return;
	if ( m_szWeaponEffectName.IsEmpty() )
		return;
	if ( m_iTicEffectCurCount >= (int)m_vTicEffectOffsetList.size() )
		return;

	D3DXVECTOR3 vScale = pOwner->GetWorldScale();
	D3DXVECTOR3 vEffectPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * m_vTicEffectOffsetList[m_iTicEffectCurCount];
	ioMapEffect *pMapEffect = pOwner->CreateMapEffectBySkill( m_szWeaponEffectName, vEffectPos, vScale );
	if( !pMapEffect )
		return;

	WeaponEffect kInfo;
	kInfo.vOffset = m_vTicEffectOffsetList[m_iTicEffectCurCount];
	kInfo.dwEffectID = pMapEffect->GetUniqueID();
	m_vCurWeaponEffectList.push_back( kInfo );
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_CRATED_TIC_EFFECT;
		kPacket << m_iTicEffectCurCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iTicEffectCurCount++;
}

void ioCreateTicWeaponSkill::UpdateWeaponEffect( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;

	for ( int i=0; i<(int)m_vCurWeaponEffectList.size() ; ++i )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vCurWeaponEffectList[i].dwEffectID );
		if ( !pMapEffect )
			continue;;
		D3DXVECTOR3 vEffectPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * m_vCurWeaponEffectList[i].vOffset;
		pMapEffect->SetWorldPosition( vEffectPos );
		pMapEffect->SetWorldOrientation( pOwner->GetTargetRot() );
		
	}
}

void ioCreateTicWeaponSkill::SetFireState( ioBaseChar *pOwner )
{
	m_bRefreshFireTimeList = false;
	AttackSkillFire( pOwner );
	m_SkillState = SS_FIRE;
	pOwner->SetSkillMoveEnable( false, false );
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SST_FIRE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateTicWeaponSkill::AttackSkillFire( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )
		return;

	pOwner->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			GetAttribute().m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	//
	SetExtraAniJump( pOwner, iAniID, fTimeRate );

	//
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
	//

	// Col Skip
	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

void ioCreateTicWeaponSkill::FireEffectWeapon( ioBaseChar* pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;

	if ( m_vCurWeaponEffectList.empty() )
		return;

	if ( m_vWeaponInfoList.empty() )
		return;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	int iWeaponIndex=0;
	int iMaxWeaponIndex = m_vWeaponInfoList.size();
	for ( int i=0; i<(int)m_vCurWeaponEffectList.size() ; ++i )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vCurWeaponEffectList[i].dwEffectID );
		if ( !pMapEffect )
			continue;

		if ( !COMPARE( iWeaponIndex, 0, iMaxWeaponIndex ) )
			iWeaponIndex = 0;

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx = m_vWeaponInfoList[iWeaponIndex].m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_vWeaponInfoList[iWeaponIndex].m_iResistanceIdx;
		kFireTime.szWoundedAni  = m_vWeaponInfoList[iWeaponIndex].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = m_vWeaponInfoList[iWeaponIndex].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_vWeaponInfoList[iWeaponIndex].m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = ++m_dwWeaponBaseIndex;
		kFireTime.eFireTimeType = FTT_SKILL_ATTACK;
		D3DXVECTOR3 vPos = pMapEffect->GetWorldPosition();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		ioHashString szItemName;
		if ( GetOwnerItem() )
			szItemName = GetOwnerItem()->GetName();
		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, szItemName );

		pMapEffect->EndEffectGrace();
		iWeaponIndex++;
	}

	m_vCurWeaponEffectList.clear();
}

void ioCreateTicWeaponSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillEnd( pOwner );

	FireEffectWeapon( pOwner, pOwner->GetCreator() );

	EraseOwnerBuff( pOwner );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	m_bSetHandMesh = false;

	pOwner->SetSkillMoveEnable( false, false );

	EndCameraBuff( pOwner );

	ClearData();
}

bool ioCreateTicWeaponSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioCreateTicWeaponSkill::IsProtected( int iDefenseBreakType ) const
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

		if ( m_dwLoopProtectEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

void ioCreateTicWeaponSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pOwner || !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SST_CRATED_TIC_EFFECT:
		{
			rkPacket >> m_iTicEffectCurCount;
			CreateTicWeapon( pOwner );
		}
		break;
	case SST_FIRE:
		SetFireState( pOwner );
		break;

	}

}

void ioCreateTicWeaponSkill::AddOwnerBuff( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	ioHashString szItemName;
	if ( GetOwnerItem() )
		szItemName = GetOwnerItem()->GetName();

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
		pOwner->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pOwner->GetCharName(), szItemName, this );
}

void ioCreateTicWeaponSkill::EraseOwnerBuff( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage && pOwner )
	{
		int iCnt = m_OwnerBuffList.size();
		for( int i=0; i<iCnt; ++i )
		{
			ioBuff *pBuff = pOwner->GetBuff( m_OwnerBuffList[i].m_BuffName );
			if( pBuff )
				pBuff->SetReserveEndBuff();
		}
	}
}
