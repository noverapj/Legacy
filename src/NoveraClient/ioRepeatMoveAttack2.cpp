
#include "stdafx.h"

#include "ioRepeatMoveAttack2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioRepeatMoveAttack2::ioRepeatMoveAttack2()
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = false;
	m_dwMakerID = -1;
}

ioRepeatMoveAttack2::ioRepeatMoveAttack2( const ioRepeatMoveAttack2 &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_PreAttackInfo( rhs.m_PreAttackInfo ),
	  m_AttackInfo( rhs.m_AttackInfo ),
	  m_nMaxRepeatCnt( rhs.m_nMaxRepeatCnt ),
	  m_bNoDropState( rhs.m_bNoDropState ),
	  m_vMoveInfo( rhs.m_vMoveInfo ),
	  m_szAimedMarker( rhs.m_szAimedMarker ),
	  m_fAimRange( rhs.m_fAimRange ),
	  m_szCheckBuff( rhs.m_szCheckBuff ),
	  m_vBaseAimPos( rhs.m_vBaseAimPos ),
	  m_RemoveBuff( rhs.m_RemoveBuff ),
	  m_bSetUseCount( rhs.m_bSetUseCount )
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = false;
	m_dwMakerID = -1;
}

ioRepeatMoveAttack2::~ioRepeatMoveAttack2()
{
	m_vMoveInfo.clear();
}

void ioRepeatMoveAttack2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	//스킬 무적 시간
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute( "Pre_Attack_Info", m_PreAttackInfo, rkLoader );

	LoadAttackAttribute( "Attack_Info", m_AttackInfo, rkLoader );

	m_vMoveInfo.clear();
	int nMaxMoveList = 8;
	for ( int i=0; i<nMaxMoveList; i++ )
	{
		char szBuf[MAX_PATH] = "";
		wsprintf( szBuf, "Move_Info%d", i+1 );

		AttackAttribute sMoveInfo;
		LoadAttackAttribute( szBuf, sMoveInfo, rkLoader );
		m_vMoveInfo.push_back( sMoveInfo );
	}

	m_nMaxRepeatCnt = rkLoader.LoadInt_e( "max_repeat_cnt", 0 );

	m_bNoDropState = rkLoader.LoadBool_e( "no_drop_state", false );

	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker = szBuf;

	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", 0.0f );

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	m_vBaseAimPos.x = rkLoader.LoadFloat_e( "base_aim_offset_x", 0.0f );
	m_vBaseAimPos.y = rkLoader.LoadFloat_e( "base_aim_offset_y", 0.0f );
	m_vBaseAimPos.z = rkLoader.LoadFloat_e( "base_aim_offset_z", 0.0f );

	m_bSetUseCount = rkLoader.LoadBool_e( "set_use_count", false );

	LoadRemoveBuffList( rkLoader );
}

ioSkill* ioRepeatMoveAttack2::Clone()
{
	return new ioRepeatMoveAttack2( *this );
}

bool ioRepeatMoveAttack2::OnSkillStart( ioBaseChar *pChar )
{	
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = m_bSetUseCount;
	m_dwMakerID = -1;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
		
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	
	
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioRepeatMoveAttack2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				EndPreDelayEffect( pChar );

				SetPreAttackState( pChar );
			}
		}
		break;
	case SS_PRE_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetRotateMoveState( pChar );
				return;
			}
		}
		break;
	case SS_MOVE:
		{		
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			if ( CheckKeyInput( pChar, false ) )
				return;
		}
		break;
	case SS_FIND:
		{
			if ( GetTargetList( pChar, pStage ) )
			{
				SetTargetRotation( pChar );
				SetAttackState( pChar, pStage );
			}
			else
				SetEndState( pChar );
		}
		break;
	case SS_ATTACK:
		{			
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			if ( CheckKeyInput( pChar, true ) )
				return;
		}
		break;
	}

	if ( m_SkillState != SS_NONE && 
		 m_SkillState != SS_PRE_ATTACK &&
		 m_SkillState != SS_END )
		UpdateMaker( pChar, pStage );
}

void ioRepeatMoveAttack2::OnSkillEnd( ioBaseChar *pChar )
{
	ioPlayStage *pStage = pChar->GetCreator();
	if ( pStage )
		DestroyEffect( pChar, pStage, m_dwMakerID );

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget ) 
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )	
			continue;

		if ( !pTarget->HasBuff( pChar->GetCharName(), m_szCheckBuff ) )
			continue;
		
		int nBuffCnt = (int)m_RemoveBuff.size();
		for ( int i=0; i<nBuffCnt; ++i )
			pTarget->RemoveBuff( m_RemoveBuff[i], true );	
	}

	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
	m_bUseActiveCnt = false;
}

void ioRepeatMoveAttack2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );	

	if ( pWeapon )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{				
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
			D3DXVECTOR3 vFireDir = ioMath::VEC3_ZERO;
			if ( !m_szAimedTarget.IsEmpty() )
			{
				ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
				if ( pTarget )
					vFireDir = pTarget->GetMidPositionByRate() - pWeapon->GetPosition();
				else
					vFireDir = vPos + pChar->GetWorldOrientation() * m_vBaseAimPos - pWeapon->GetPosition();
			}
			else
				vFireDir = vPos + pChar->GetWorldOrientation() * m_vBaseAimPos - pWeapon->GetPosition();

			D3DXVec3Normalize( &vFireDir, &vFireDir );
			pWeapon->SetMoveDir( vFireDir );
		}
	}
}

void ioRepeatMoveAttack2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_FIND:
		{
			rkPacket >>	m_szAimedTarget;

			SetTargetRotation( pChar );
			SetAttackState( pChar, pStage );
		}
		break;
	case SS_ROTATE_MOVE:
		{
			int nNewDir;
			rkPacket >> nNewDir;

			SetMoveState( pChar, nNewDir );
		}
		break;
	case SS_END:
		m_SkillState = SS_END;
		break;
	}
}

bool ioRepeatMoveAttack2::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}
	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{		
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioRepeatMoveAttack2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ATTACK:
	case SS_MOVE:
		return false;
	case SS_END:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioRepeatMoveAttack2::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRepeatMoveAttack2::CheckKeyInput( ioBaseChar *pChar, bool bSetMove )
{
	if( !pChar || !pChar->IsNeedProcess() || m_nCurRepeatCnt >= m_nMaxRepeatCnt ) 
		return false;

	if ( m_dwAttackReserveTime < FRAMEGETTIME() )
	{
		if ( bSetMove && pChar->CheckCurDirKey() != ioUserKeyInput::DKI_NONE &&
			 pChar->IsAttackKey() )
		{
			SetRotateMoveState( pChar );
			return true;
		}

		if ( pChar->IsAttackKey() )
		{
			m_SkillState = SS_FIND;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRepeatMoveAttack2::SetPreAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	//단계의 애니메이션 값 읽기
	int nAniID = pGrp->GetAnimationIdx( m_PreAttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	//빠르기 설정, 딜레이 설정
	float fTimeRate  = m_PreAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_PreAttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	//모션 시간 설정
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, nAniID, m_PreAttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	m_SkillState = SS_PRE_ATTACK;
}

void ioRepeatMoveAttack2::SetMoveState( ioBaseChar *pChar, int nDir )
{
	if ( !pChar )
		return;
	
	if ( nDir < 0 || (int)m_vMoveInfo.size() <= nDir )
		return;

	const AttackAttribute &sMoveInfo = m_vMoveInfo[nDir];

	ioEntityGroup *pGrp = pChar->GetGroup();

	//단계의 애니메이션 값 읽기
	int nAniID = pGrp->GetAnimationIdx( sMoveInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	//빠르기 설정, 딜레이 설정
	float fTimeRate  = sMoveInfo.m_fAttackAniRate;
	DWORD dwPreDelay = sMoveInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	//모션 시작 시간 설정
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwAttackReserveTime = dwCurTime;
	m_dwAttackReserveTime += pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	//캐릭터 충돌 해제
	pChar->CheckCharColSkipSkillTime( nAniID, fTimeRate, dwPreDelay );
	//슬라이딩 설정
	pChar->SetReservedSliding( sMoveInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );
	
	m_SkillState = SS_MOVE;
}

void ioRepeatMoveAttack2::SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	//단계의 애니메이션 값 읽기
	int nAniID = pGrp->GetAnimationIdx( m_AttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	//빠르기 설정, 딜레이 설정
	float fTimeRate  = m_AttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	//모션 시간 설정
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwEnableReserveTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwAttackReserveTime = dwCurTime;
	m_dwAttackReserveTime += pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, nAniID, m_AttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	m_SkillState = SS_ATTACK;

	m_nCurRepeatCnt++;
}

void ioRepeatMoveAttack2::SetRotateMoveState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	int nDir = 0;
	ioUserKeyInput::DirKeyInput nDirKey = pChar->CheckCurDirKey();

	if ( nDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fTargetYaw = 180.0f + 45.0f * ( nDirKey - ioUserKeyInput::DKI_UP );
		fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

		float fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pChar->GetTargetRot() ) );
		fCurYaw = ioMath::ArrangeHead( fCurYaw );

		float fAngleGap = ioMath::AngleGap( (int)fCurYaw, (int)fTargetYaw );
		fAngleGap += 360.0f;
		int nSide = (int)fAngleGap % 360;

		nDir = nSide / 45;
	}

	SetMoveState( pChar, nDir );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ROTATE_MOVE;
		kPacket << nDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRepeatMoveAttack2::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_END;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRepeatMoveAttack2::IsNoFallDropZone() const
{	
	return m_bNoDropState;
}

bool ioRepeatMoveAttack2::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioRepeatMoveAttack2::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nMaxRepeatCnt;
}

int ioRepeatMoveAttack2::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nCurRepeatCnt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRepeatMoveAttack2::GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return false;

	m_szAimedTarget.Clear();
	m_vTargetInfoList.clear();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget ) 
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )	
			continue;

		if ( !pTarget->HasBuff( m_szCheckBuff ) )
			continue;

		if ( pTarget->IsApplyDownState() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		tagBaseCharDistanceSq kInfo;
		kInfo.m_Name        = pTarget->GetCharName();
		kInfo.m_fDistanceSq = fDistSq;

		float fEnableDistSq = m_fAimRange * m_fAimRange;
		if( fDistSq < fEnableDistSq )
			m_vTargetInfoList.push_back(kInfo);
	}

	if ( m_vTargetInfoList.empty() )
		return false;

	std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), BaseCharDistanceSqSort2() );	

	if( !m_vTargetInfoList.empty() )
		m_szAimedTarget = m_vTargetInfoList[0].m_Name;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_FIND;
		kPacket << m_szAimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioRepeatMoveAttack2::SetTargetRotation( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		pChar->SetTargetRotToDir( vAimedDir, true );		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioRepeatMoveAttack2::CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pChar || !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioRepeatMoveAttack2::DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pChar || !pStage || dwEffectID == -1 )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioRepeatMoveAttack2::UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + pChar->GetWorldOrientation() * m_vBaseAimPos;

	if ( !m_szAimedTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szAimedTarget );
		if ( pTarget )
			vPos = pTarget->GetMidPositionByRate();
	}

	if ( m_dwMakerID == -1 )
		m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );

	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwMakerID );
	if ( pEffect )		
		pEffect->SetWorldPosition( vPos );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRepeatMoveAttack2::LoadRemoveBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_RemoveBuff.clear();

	if( iCnt <= 0 ) 
		return;

	m_RemoveBuff.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "remove_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_RemoveBuff.push_back( szBuf );
	}
}