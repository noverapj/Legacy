

#include "stdafx.h"

#include "ioTeleportAttackSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "FindPredicateImpl.h"

#include "WeaponDefine.h"

ioTeleportAttackSkill::ioTeleportAttackSkill()
{
	m_dwFireStartTime = 0;
	m_dwAttackEndTime = 0;
	m_iCurAttackCount = 0;
	m_vTargetList.clear();
}

ioTeleportAttackSkill::ioTeleportAttackSkill( const ioTeleportAttackSkill &rhs )
: ioNormalSkill( rhs ),
  m_iAttackCount( rhs.m_iAttackCount ),
  m_vAttackAttributeList( rhs.m_vAttackAttributeList ),
  m_fTeleportRangeList( rhs.m_fTeleportRangeList ),
  m_fBackRange( rhs.m_fBackRange ),
  m_fHeightRange( rhs.m_fHeightRange ),
  m_OwnerBuffList( rhs.m_OwnerBuffList ),
  m_SkillWoundType( rhs.m_SkillWoundType ),
  m_bBackMoveDirRand( rhs.m_bBackMoveDirRand ),
  m_bBackMoveAutoTarget( rhs.m_bBackMoveAutoTarget ),
  m_iMaxWeaponWay( rhs.m_iMaxWeaponWay ),
  m_BulletInfoList( rhs.m_BulletInfoList ),
  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_dwFireStartTime	= 0;
	m_dwAttackEndTime	= 0;
	m_iCurAttackCount	= 0;
	m_iWeaponWay		= 0;

	m_vTargetList.clear();
}

ioTeleportAttackSkill::~ioTeleportAttackSkill()
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
		if( pOwner )
		{
			int iCnt = m_OwnerBuffList.size();
			for( int i=0; i<iCnt; ++i )
			{
				ioBuff *pBuff = pOwner->GetBuff( m_OwnerBuffList[i].m_BuffName );
				if( pBuff )
					pBuff->EndBuff();
			}
		}
	}
}

void ioTeleportAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH];

	m_iAttackCount = rkLoader.LoadInt_e( "attack_count", 0 );
	m_vAttackAttributeList.clear();
	for( int i=0; i<m_iAttackCount; ++i )
	{
		AttackAttribute kInfo;
		kInfo.Init();
		wsprintf_e( szKey, "attack%d", i+1 );
		LoadAttackAttribute( szKey, kInfo, rkLoader );	
		m_vAttackAttributeList.push_back( kInfo );

		wsprintf_e( szKey, "teleport_range%d", i+1 );
		float fRange = rkLoader.LoadFloat( szKey, 0.0f );
		m_fTeleportRangeList.push_back( fRange );
	}
	
	m_fBackRange			= rkLoader.LoadFloat_e( "back_move_range", 0.0f );
	m_fHeightRange			= rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );
	m_bBackMoveDirRand		= rkLoader.LoadBool_e( "back_move_dir_rand", true );
	m_bBackMoveAutoTarget	= rkLoader.LoadBool_e( "back_move_auto_target", true );
	m_dwSkillProtectTime	= rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadBuffList( rkLoader );
	m_SkillWoundType = (TargetWoundType)rkLoader.LoadInt_e( "skill_wound_type", 0 );

	LoadBullet( rkLoader );
}

void ioTeleportAttackSkill::LoadBuffList( ioINILoader &rkLoader )
{
	ioBuffInfo tBuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_OwnerBuffList.clear();

	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

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

void ioTeleportAttackSkill::LoadBullet( ioINILoader &rkLoader )
{
	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_bullet_cnt", 1 );
	m_iMaxWeaponWay = max( 1, m_iMaxWeaponWay );

	char szKey[MAX_PATH];
	m_BulletInfoList.clear();
	for( int i=0; i < m_iMaxWeaponWay; ++i )
	{
		BulletInfo kInfo;

		wsprintf_e( szKey, "bullet_angle_h%d", i+1 );
		kInfo.m_fYawAngle = rkLoader.LoadFloat( szKey, 0.0f );
		
		wsprintf_e( szKey, "bullet_angle_v%d", i+1 );
		kInfo.m_fPitchAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "bullet_range_offset%d", i+1 );
		kInfo.m_fRangeOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "bullet_height_offset%d", i+1 );
		kInfo.m_fHeightOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		m_BulletInfoList.push_back( kInfo );
	}
}

void ioTeleportAttackSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( !m_OwnerBuffList[i].m_BuffName.IsEmpty() )
		{
			pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, "", "", this );
		}
	}
}

ioSkill* ioTeleportAttackSkill::Clone()
{
	return new ioTeleportAttackSkill( *this );
}

bool ioTeleportAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	m_OwnerName = pChar->GetCharName();

	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_AttackState = AS_NONE;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	// hand mesh를 predelay 시점에 하기에...
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	pChar->AttachEffect( m_SkillEffectName );

	m_dwFireStartTime	= 0;
	m_dwAttackEndTime	= 0;
	m_iCurAttackCount	= 0;
	m_iWeaponWay		= 0;

	m_vTargetList.clear();

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	AddOwnerBuff( pChar );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
	return true;
}

void ioTeleportAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetTeleport( pStage, pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_iCurAttackCount >= m_iAttackCount )
			{
				if( m_dwMotionEndTime <= dwCurTime )
					SetEndState( pChar );
			}
			else if( m_dwMotionEndTime <= dwCurTime )
			{
				SetTeleport( pStage, pChar );
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioTeleportAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	m_dwFireStartTime = 0;

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	pChar->EndEffect( m_SkillEffectName, false );

	ioNormalSkill::OnSkillEnd( pChar );
}

void ioTeleportAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		pZoneEffect->SetZoneValue( true, true );
	}

	ioMissileWeapon* pMissile = ToMissileWeapon( pWeapon );	
	if( pMissile )
	{
		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurYawAngle		= 0.0f;
		float fCurPitchAngle	= 0.0f;
		
		fCurYawAngle	= m_BulletInfoList[m_iWeaponWay].m_fYawAngle;
		fCurPitchAngle	= m_BulletInfoList[m_iWeaponWay].m_fPitchAngle;

		fCurYawAngle	= ioMath::ArrangeHead( fCurYawAngle );
		fCurPitchAngle	= ioMath::ArrangeHead( fCurPitchAngle );

		D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
		D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		// vertical
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fCurYawAngle) );
		vMoveDir = qtRot * vMoveDir;

		// horizon
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fCurPitchAngle) );
		vMoveDir = qtRot * vMoveDir;	
		pWeapon->SetMoveDir( vMoveDir );

		m_iWeaponWay++;
	}
}

void ioTeleportAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	D3DXVECTOR3 vTargetPos;
	D3DXQUATERNION qtTargetOrientation;

	rkPacket >> vTargetPos >> qtTargetOrientation;

	if( pOwner )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		pStage->CreateMapEffect( m_SkillMapEffectName, vPos, vScale );

		pOwner->SetWorldPosition( vTargetPos );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetOrientation );
	}

	rkPacket >> m_iCurAttackCount;
	int iAttackState;
	rkPacket >> iAttackState;
	m_AttackState = (AttackState)iAttackState;

	SetNormalFire( pOwner );
}

bool ioTeleportAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
		return false;
	case SS_ACTION:
	case SS_END:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			return true;
		return false;
	}

	return true;
}

bool ioTeleportAttackSkill::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		if( m_AttackState == AS_FLOAT )
			return true;
		else
			return false;
	}

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioTeleportAttackSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_ACTION:
	case SS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioTeleportAttackSkill::IsNoDropState() const
{
	if( m_AttackState == AS_FLOAT )
		return true;

	return false;
}

void ioTeleportAttackSkill::SetNormalFire( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_ACTION;

	if( !COMPARE( m_iCurAttackCount, 0, (int)m_vAttackAttributeList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();
	
	AttackAttribute vAttackAttribute;
	vAttackAttribute.Init();
	vAttackAttribute = m_vAttackAttributeList[m_iCurAttackCount];

	int iCurSkillAniID = pGrp->GetAnimationIdx( vAttackAttribute.m_AttackAnimation );
	if( iCurSkillAniID == -1 )
		return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = vAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = vAttackAttribute.m_dwPreDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex + m_iCurAttackCount, 
										iCurSkillAniID,
										vAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwFireMotionEndTime = dwCurTime + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iCurSkillAniID, fTimeRate, dwPreDelay );

	pChar->SetReservedSliding( vAttackAttribute.m_vForceInfoList, iCurSkillAniID, fTimeRate, dwPreDelay );

	pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem()->GetName() ) );

	m_iCurAttackCount++;
	m_iCurAttackCount = min( m_iCurAttackCount, m_iAttackCount );
}

void ioTeleportAttackSkill::SetEndState( ioBaseChar *pChar )
{
	//SetSKillEndJumpState() 를 사용하기 위해 약간의 여유시간은 둔다.
	m_dwMotionEndTime = FRAMEGETTIME() + 100;
	m_SkillState = SS_END;
}

void ioTeleportAttackSkill::SetTeleport( ioPlayStage *pStage, ioBaseChar *pChar )
{
	if( !pStage )	return;
	if( !pChar )	return;

	if( !pChar->IsNeedProcess() )
		return;

	if( !COMPARE( m_iCurAttackCount, 0, (int)m_vAttackAttributeList.size() ) )
		return;

	m_vTargetInfoList.clear();

	float fTeleportRange = m_fTeleportRangeList[m_iCurAttackCount];
	float fPrevDistSq = fTeleportRange*fTeleportRange;
	ioBaseChar *pTargetChar = pChar;

	if( m_iCurAttackCount == 0 && m_bBackMoveAutoTarget )
	{
		FD_GrapplingAutoTarget kAutoPred( pChar, m_fAutoTargetMinAngle, m_fAutoTargetMaxAngle, m_fAutoTargetRange );

		ioBaseChar *pTarget = pStage->FindUser( kAutoPred );
		if( pTarget )
			pTargetChar = pTarget;
	}
	else
	{
		const BaseCharList &rkTargetList = pStage->GetBaseCharList();
		BaseCharList::const_iterator iter;
		for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
		{
			ioBaseChar *pTarget = (*iter);
			if( !pTarget )
				continue;

			if( !pStage->IsCollisionCheckByMode() )
				continue;

			if( pChar->GetCharName() == pTarget->GetCharName() )
				continue;

			if( pChar->GetTeam() == pTarget->GetTeam() )
				continue;
			if( pTarget->IsSystemState() )
				continue;
			if( !pTarget->IsEnableTarget() )
				continue;
			if( !IsEnableTarget( pTarget ) )
				continue;

			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
			float fDistSq = D3DXVec3LengthSq( &vDiff );
			TargetDistInfo	kInfo;
			kInfo.m_TargetName = pTarget->GetCharName();
			kInfo.m_fLenth = fDistSq;

			float fEnableDistSq = fTeleportRange*fTeleportRange;
			if( fDistSq > fEnableDistSq )
				continue;

			m_vTargetInfoList.push_back(kInfo);
		}

		if( !m_vTargetInfoList.empty() )
		{
			std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), DistSort() );

			int iInfoList = m_vTargetInfoList.size();
			for( int i=0; i<iInfoList; ++i )
			{
				bool bArleadyAttack = false;
				int iCnt = m_vTargetList.size();
				for( int j=0; j<iCnt; ++j )
				{
					if( m_vTargetInfoList[i].m_TargetName == m_vTargetList[j] )
					{
						bArleadyAttack = true;
					}
				}

				if( !bArleadyAttack )
				{
					pTargetChar = pStage->GetBaseChar( m_vTargetInfoList[i].m_TargetName );
					break;
				}
			}

			if( pTargetChar == pChar )
			{
				int iIdx = m_vTargetInfoList.size();
				pTargetChar = pStage->GetBaseChar( m_vTargetInfoList[iIdx-1].m_TargetName );
				m_vTargetList.clear();
			}
		}
	}

	if( pChar && pTargetChar )
	{
		IORandom random;
		DWORD dwSeed = 0;
		int iRand = 0;
		dwSeed = pChar->GetRandomSeed();
		random.SetRandomSeed( dwSeed + m_iCurAttackCount );

		int iDir = random.Random( 8 );
		float fYawD = 180.0f + 45.0f * iDir;

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		D3DXVECTOR3 vOffSet( 0.0f, m_fHeightRange, 0.0f );
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition() + vOffSet;

		D3DXQUATERNION qtTargetOrientation = ioMath::QUAT_IDENTITY;
		if( m_bBackMoveDirRand )
			qtTargetOrientation = qtRotate;
		else
			qtTargetOrientation = pTargetChar->GetWorldOrientation();

		D3DXVECTOR3 vNewPos = vTargetPos - ( ( qtTargetOrientation * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) ) * m_fBackRange );

		pChar->SetWorldPosition( vNewPos );
		pChar->SetTargetRotAndMoveDirChange( qtTargetOrientation );

		if( pStage )
			pStage->SetCameraToOwnerExplicit( pChar );

		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		pStage->CreateMapEffect( m_SkillMapEffectName, vPos, vScale );

		float fHeight = pChar->GetBottomHeight();
		float fHeightGap = vNewPos.y - fHeight;
		if( fHeight <= FLOAT10 )
			m_AttackState = AS_LAND;
		else
			m_AttackState = AS_FLOAT;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetTargetRot();
			kPacket << m_iCurAttackCount;
			kPacket << (int)m_AttackState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		m_vTargetList.push_back( pTargetChar->GetCharName() );

		SetNormalFire( pChar );
	}
}

bool ioTeleportAttackSkill::IsEnableTarget( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	// 쓰러짐 관련 체크
	if( m_SkillWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
	{
		if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
			return false;
	}
	else if( m_SkillWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
	{
		if( pChar->IsApplyDownState(false) )
			return false;
	}
	else if( m_SkillWoundType == TWT_ALL_WOUND_EXCEPT )
	{
		if( pChar->IsApplyDownState() ||
			( pChar->GetState() == CS_GRAPPLING_WOUNDED ||
			pChar->GetState() == CS_GRAPPLING_PUSHED ||
			pChar->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
			pChar->GetState() == CS_GRAPPLING_WOUNDED_SWING ) )
		{
			return false;
		}
	}
	else if( m_SkillWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
	{
		if( !pChar->IsFloating() )
			return false;
	}
	else if( m_SkillWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
	{	
		if( !pChar->IsApplyDownState() || pChar->IsFloating() )
		{	
			return false;
		}
	}

	return true;
}

bool ioTeleportAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	if( 0 < m_dwSkillProtectTime )
	{
		if(	FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
			return true;
		else
			return false;
	}

	return false;
}