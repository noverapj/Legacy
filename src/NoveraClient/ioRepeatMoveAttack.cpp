
#include "stdafx.h"

#include "ioRepeatMoveAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioRepeatMoveAttack::ioRepeatMoveAttack()
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = false;
	m_nCurOffset = 0;
}

ioRepeatMoveAttack::ioRepeatMoveAttack( const ioRepeatMoveAttack &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_AttackInfo( rhs.m_AttackInfo ),
	  m_nMaxRepeatCnt( rhs.m_nMaxRepeatCnt ),
	  m_bNoDropState( rhs.m_bNoDropState ),
	  m_vMoveInfo( rhs.m_vMoveInfo ),
	  m_vOffsetList( rhs.m_vOffsetList ),
	  m_szWeaponRedEffect( rhs.m_szWeaponRedEffect ),
	  m_szWeaponBlueEffect( rhs.m_szWeaponBlueEffect ),
	  m_bChangeDir( rhs.m_bChangeDir )
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = false;
	m_nCurOffset = 0;

	m_DirKey = ioUserKeyInput::DKI_NONE;
}

ioRepeatMoveAttack::~ioRepeatMoveAttack()
{
	m_vMoveInfo.clear();
	m_vOffsetList.clear();
}

void ioRepeatMoveAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
		
	//스킬 무적 시간
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute( "Attack_Info", m_AttackInfo, rkLoader );

	char szBuf[MAX_PATH] = "";
	m_vMoveInfo.clear();
	int nMaxMoveList = 8;
	for ( int i=0; i<nMaxMoveList; i++ )
	{
		wsprintf( szBuf, "Move_Info%d", i+1 );

		AttackAttribute sMoveInfo;
		LoadAttackAttribute( szBuf, sMoveInfo, rkLoader );
		m_vMoveInfo.push_back( sMoveInfo );
	}

	m_nMaxRepeatCnt = rkLoader.LoadInt_e( "max_repeat_cnt", 0 );

	m_bNoDropState = rkLoader.LoadBool_e( "no_drop_state", false );
	m_bChangeDir = rkLoader.LoadBool_e( "change_dir", false );

	m_vOffsetList.clear();
	int nOffsetCnt = rkLoader.LoadInt_e( "attack_info_type_count", 0 );
	for ( int i=0; i< nOffsetCnt; ++i )
	{
		D3DXVECTOR3 vOffset;
		wsprintf_e( szBuf, "attack_info_type%d_offset_x", i+1 );
		vOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "attack_info_type%d_offset_y", i+1 );
		vOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "attack_info_type%d_offset_z", i+1 );
		vOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vOffsetList.push_back( vOffset );
	}

	rkLoader.LoadString_e( "weapon_red_effect", "", szBuf, MAX_PATH );
	m_szWeaponRedEffect = szBuf;
	rkLoader.LoadString_e( "weapon_blue_effect", "", szBuf, MAX_PATH );
	m_szWeaponBlueEffect = szBuf;
}

ioSkill* ioRepeatMoveAttack::Clone()
{
	return new ioRepeatMoveAttack( *this );
}

bool ioRepeatMoveAttack::OnSkillStart( ioBaseChar *pChar )
{	
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = true;
	m_nCurOffset = 0;
	m_DirKey = ioUserKeyInput::DKI_NONE;

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

void ioRepeatMoveAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				CreateWeaponEffect( pChar, pStage );

				if ( pChar->IsNeedProcess() )
					SetAttackState( pChar );
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

			if ( CheckKeyInput( pChar ) )
			{
				SetAttackState( pChar );
				return;
			}
		}
		break;
	case SS_ATTACK:
		{			
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_nCurRepeatCnt < m_nMaxRepeatCnt )
				{				
					SetRotateMoveState( pChar );
					return;
				}
				else
				{
					SetEndState( pChar );
					return;
				}
			}
		}
		break;
	}
}

void ioRepeatMoveAttack::OnSkillEnd( ioBaseChar *pChar )
{
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

bool ioRepeatMoveAttack::IsProtected( int iDefenseBreakType ) const
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

bool ioRepeatMoveAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
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

bool ioRepeatMoveAttack::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

void ioRepeatMoveAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if ( !m_vOffsetList.empty() && COMPARE( m_nCurOffset, 0, (int)m_vOffsetList.size() ) )
	{
		ioFireWorksStep1Weapon *pFireWorks = ToFireWorksStep1Waepon( pWeapon );
		if ( pFireWorks )
		{
			ioBaseChar *pOwner = pWeapon->GetOwner();
			if ( pOwner )
			{
				D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vOffsetList[m_nCurOffset];

				pFireWorks->SetCallAxisChange( true );
				pFireWorks->SetMoveDir( D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ) );
				pFireWorks->SetPosition( vNewPos );
				m_nCurOffset++;
			}
		}
	}
}

void ioRepeatMoveAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_ATTACK:
		SetAttackState( pChar );
		if( IsUseAutoTargetValue() )
			pChar->ApplyAutoTargetInfo( rkPacket );
		break;
	case SS_ROTATE_MOVE:
		{
			int nNewDir;
			int nNewDirKey;
			rkPacket >> nNewDir;
			rkPacket >> nNewDirKey;			

			m_DirKey = (ioUserKeyInput::DirKeyInput)nNewDirKey;
			SetMoveState( pChar, nNewDir );			
		}
		break;
	case SS_END:
		m_SkillState = SS_END;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRepeatMoveAttack::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() || m_nCurRepeatCnt >= m_nMaxRepeatCnt ) 
		return false;

	if ( m_dwAttackReserveTime < FRAMEGETTIME() &&
		 pChar->IsAttackKey() )
	{
		m_nCurRepeatCnt++;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRepeatMoveAttack::SetMoveState( ioBaseChar *pChar, int nDir )
{
	if ( !pChar )
		return;

	if( m_bChangeDir && m_DirKey != ioUserKeyInput::DKI_NONE )
	{
		pChar->SetTargetRotToDirKey( m_DirKey );
		nDir = 0;
	}

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

void ioRepeatMoveAttack::SetAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
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
	m_dwMotionEndTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;
	
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, nAniID, m_AttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_SkillState = SS_ATTACK;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ATTACK;

		if( IsUseAutoTargetValue() )
			pChar->FillAutoTargetInfo( kPacket );

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRepeatMoveAttack::SetRotateMoveState( ioBaseChar *pChar )
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

	m_DirKey = nDirKey;
	SetMoveState( pChar, nDir );	

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ROTATE_MOVE;
		kPacket << nDir;
		kPacket << (int)nDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRepeatMoveAttack::SetEndState( ioBaseChar *pChar )
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

bool ioRepeatMoveAttack::IsNoFallDropZone() const
{	
	return m_bNoDropState;
}

bool ioRepeatMoveAttack::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioRepeatMoveAttack::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nMaxRepeatCnt;
}

int ioRepeatMoveAttack::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nCurRepeatCnt;
}

void ioRepeatMoveAttack::CreateWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage || m_szWeaponRedEffect.IsEmpty() || m_szWeaponBlueEffect.IsEmpty() )
		return;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( fMapHeight > 0.0f )
		vPos.y = fMapHeight;

	if( pChar->GetTeam() == TEAM_RED )
		pStage->CreateMapEffect( m_szWeaponRedEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	else if( pChar->GetTeam() == TEAM_BLUE )
		pStage->CreateMapEffect( m_szWeaponBlueEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pStage->CreateMapEffect( m_szWeaponBlueEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		else
			pStage->CreateMapEffect( m_szWeaponRedEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	}
}