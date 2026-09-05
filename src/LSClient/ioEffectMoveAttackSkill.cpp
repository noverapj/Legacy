
#include "stdafx.h"

#include "ioEffectMoveAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioEffectMoveAttackSkill::ioEffectMoveAttackSkill()
{
	Init();
}

ioEffectMoveAttackSkill::ioEffectMoveAttackSkill( const ioEffectMoveAttackSkill &rhs )
	: ioAttackSkill( rhs ),
    m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_dwLoopTime( rhs.m_dwLoopTime ),
	m_szLoopAni( rhs.m_szLoopAni ),
	m_fLoopAniRate( rhs.m_fLoopAniRate ),
	m_AttackInfo( rhs.m_AttackInfo ),
	m_nMaxAttackCnt( rhs.m_nMaxAttackCnt ),
	m_bSetUseCount( rhs.m_bSetUseCount ),
	m_szTargetMarker( rhs.m_szTargetMarker ),
	m_fTargetMarkerHeight( rhs.m_fTargetMarkerHeight ),
	m_fTargetMarkerDepth( rhs.m_fTargetMarkerDepth ),
	m_fTargetMarkerMoveSpeed( rhs.m_fTargetMarkerMoveSpeed ),
	m_dwEndCameraEvent( rhs.m_dwEndCameraEvent ),
	m_fHeightLimitRange( rhs.m_fHeightLimitRange ),
	m_fWidthLimitRange( rhs.m_fWidthLimitRange ),
	m_OwnerBuffList( rhs.m_OwnerBuffList ),
	m_szOtherShotEffect( rhs.m_szOtherShotEffect ),
	m_fOtherShotEffectOffsetX( rhs.m_fOtherShotEffectOffsetX ),
	m_fOtherShotEffectOffsetY( rhs.m_fOtherShotEffectOffsetY ),
	m_fOtherShotEffectOffsetZ( rhs.m_fOtherShotEffectOffsetZ )
{
	Init();
}

ioEffectMoveAttackSkill::~ioEffectMoveAttackSkill()
{
}

void ioEffectMoveAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_dwLoopTime = (DWORD)rkLoader.LoadInt_e( "loop_time", 0 );

	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat_e( "loop_ani_rate", FLOAT1 );
	
	LoadAttackAttribute( "Attack_Info", m_AttackInfo, rkLoader );
	
	m_nMaxAttackCnt = rkLoader.LoadInt_e( "max_attack_cnt", 0 );
	m_bSetUseCount = rkLoader.LoadBool_e( "set_use_count", false );

	rkLoader.LoadString_e( "target_marker", "", szBuf, MAX_PATH );
	m_szTargetMarker = szBuf;

	m_fTargetMarkerHeight = rkLoader.LoadFloat_e( "target_marker_height", 0.0f );
	m_fTargetMarkerDepth = rkLoader.LoadFloat_e( "target_marker_depth", 0.0f );
	m_fTargetMarkerMoveSpeed = rkLoader.LoadFloat_e( "target_marker_move_speed", 0.0f );

	m_dwEndCameraEvent = rkLoader.LoadInt_e( "end_camera_event", 0 );

	m_fHeightLimitRange = rkLoader.LoadFloat_e( "height_limit_range", 0.0f );
	m_fWidthLimitRange = rkLoader.LoadFloat_e( "width_limit_range", 0.0f );

	ioBuffInfo tBuffInfo;
	m_OwnerBuffList.clear();
	int iCount = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		tBuffInfo.m_BuffName = szBuf;
		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );

			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}

	rkLoader.LoadString_e( "other_shot_effect", "", szBuf, MAX_PATH );
	m_szOtherShotEffect = szBuf;

	m_fOtherShotEffectOffsetX = rkLoader.LoadFloat_e( "other_shot_effect_offset_x", 0.0f );
	m_fOtherShotEffectOffsetY = rkLoader.LoadFloat_e( "other_shot_effect_offset_y", 0.0f );
	m_fOtherShotEffectOffsetZ = rkLoader.LoadFloat_e( "other_shot_effect_offset_z", 0.0f );
}

ioSkill* ioEffectMoveAttackSkill::Clone()
{
	return new ioEffectMoveAttackSkill( *this );
}

bool ioEffectMoveAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	Init();

	m_bUseActiveCnt = m_bSetUseCount;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
	
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
	//자신에게 생성하는 버프추가
	for (int i=0; i < (int)m_OwnerBuffList.size(); ++i)
	{
		ioBuffInfo pOwnerBuff = m_OwnerBuffList[i];
		pChar->AddNewBuff( pOwnerBuff.m_BuffName, pChar->GetCharName(), GetOwnerItem()->GetName(), this);					
	}
	return true;
}

void ioEffectMoveAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_None:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_Loop:
		{
			if ( m_dwCheckLoopTime > 0 && m_dwCheckLoopTime + m_dwLoopTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			CheckDirKey( pChar );
			UpdateTargetMarker( pChar, pStage );
			CheckInputKey( pChar, pStage );
		}
		break;
	case SS_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_nCurAttackCnt < m_nMaxAttackCnt )
					SetLoopState( pChar, pStage );
				else
					SetEndState( pChar );
				return;
			}

			CheckDirKey( pChar );
			UpdateTargetMarker( pChar, pStage );
		}
		break;
	}

}

void ioEffectMoveAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	//자신에게 생성하는 버프를 삭제
	for (int i=0; i < (int)m_OwnerBuffList.size(); ++i)
	{
		ioBuffInfo pOwnerBuff = m_OwnerBuffList[i];
		pChar->RemoveBuff( pOwnerBuff.m_BuffName, false);			
	}

	m_bUseActiveCnt = false;

	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	ioPlayStage *pStage = pChar->GetCreator();
	if ( pStage )
	{
		DestroyEffect( pStage, m_dwTargetMarkerID );

		if( pChar->IsNeedProcess() )
			g_CameraEventMgr.CreateCE( m_dwEndCameraEvent, pStage );
	}

	m_bSetHandMesh = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEffectMoveAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if ( pWeapon )
	{
		pWeapon->SetStartPosition( m_vEffectPos, pStage );
		pWeapon->SetPosition( m_vEffectPos );
		pWeapon->SetMoveDir( ioMath::UNIT_Z );
	}
}

void ioEffectMoveAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_Attack:
		{
			rkPacket >> m_nCurAttackCnt;
			rkPacket >> m_vEffectPos;

			SetAttackState( pChar, pStage );
		}
		break;
	case SS_End:
		m_SkillState = SS_End;
		break;
	}
}

bool ioEffectMoveAttackSkill::IsProtected( int iDefenseBreakType ) const
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

bool ioEffectMoveAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Loop:
	case SS_Attack:
		return false;
	case SS_End:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioEffectMoveAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_End )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioEffectMoveAttackSkill::CheckExceptionCamera()
{
	switch( m_SkillState )
	{
	case SS_None:
	case SS_End:
		return false;
	}

	if( !m_szTargetMarker.IsEmpty() )
		return true;

	return false;
}

D3DXVECTOR3 ioEffectMoveAttackSkill::GetExceptionCameraPos( ioBaseChar *pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return ioMath::VEC3_ZERO;

	if ( m_dwTargetMarkerID != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwTargetMarkerID );
		if( pMapEffect )
			return pMapEffect->GetWorldPosition();
	}

	return pChar->GetWorldPosition();
}

bool ioEffectMoveAttackSkill::IsFixedExceptionCamera()
{
	switch( m_SkillState )
	{
	case SS_None:
	case SS_End:
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEffectMoveAttackSkill::Init()
{
	m_dwCurSkillProtectTime = 0;

	m_bSetHandMesh = false;

	m_SkillState = SS_None;	
	m_nCurAttackCnt = 0;
	m_bUseActiveCnt = false;

	m_dwCheckLoopTime = 0;
	m_dwTargetMarkerID = -1;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioEffectMoveAttackSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( nAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/m_fLoopAniRate );

	if( pChar->IsNeedProcess() && !m_szTargetMarker.IsEmpty() && m_dwTargetMarkerID == -1 )
	{
		g_CameraEventMgr.NotifyCEDestroyed();
		pStage->ClearCameraModeFixed( CAMERA_STYLE_MOVIE );

		m_dwCheckLoopTime = FRAMEGETTIME();
		SetCameraBuff( pChar );

		D3DXVECTOR3 vCharPos = pChar->GetMidPositionByRate();
		vCharPos.z = 0.0f;

		D3DXVECTOR3 vPos = vCharPos + ( D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * m_fTargetMarkerDepth );
		vPos.y = vCharPos.y + m_fTargetMarkerHeight;

		pStage->CheckFlyMapLimit( &vPos );
		pStage->CheckAxisLimit( &vPos );

		m_dwTargetMarkerID = CreateEffect( pStage, m_szTargetMarker, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		m_vEffectPos = vPos;
	}

	m_SkillState = SS_Loop;
}

void ioEffectMoveAttackSkill::SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_AttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_AttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->RefreshFireTimeList( nAniID, m_AttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_SkillState = SS_Attack;

	if( !m_szOtherShotEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = m_vEffectPos;
		vPos.x += m_fOtherShotEffectOffsetX;
		vPos.y += m_fOtherShotEffectOffsetY;
		vPos.z += m_fOtherShotEffectOffsetZ;
		CreateEffect( pStage, m_szOtherShotEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	}

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Attack;
		kPacket << m_nCurAttackCnt;
		kPacket << m_vEffectPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nCurAttackCnt++;
}

void ioEffectMoveAttackSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_End;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEffectMoveAttackSkill::CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	if ( pChar->IsAttackKey() )
		SetAttackState( pChar, pStage );
}

void ioEffectMoveAttackSkill::CheckDirKey( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
		m_CurDirKey = eNewDirKey;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioEffectMoveAttackSkill::CreateEffect( ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioEffectMoveAttackSkill::DestroyEffect( ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pStage || dwEffectID == -1 )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioEffectMoveAttackSkill::UpdateTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage || m_dwTargetMarkerID == -1 )
		return;
	
	bool bMove = false;
	D3DXVECTOR3 vCurDir = ioMath::VEC3_ZERO;
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		switch( m_CurDirKey )
		{
		case ioUserKeyInput::DKI_UP:
			vCurDir	+= ioMath::UNIT_Y;
			break;
		case ioUserKeyInput::DKI_DOWN:
			vCurDir	-= ioMath::UNIT_Y;
			break;
		case ioUserKeyInput::DKI_LEFT:
			vCurDir	-= ioMath::UNIT_X;
			break;
		case ioUserKeyInput::DKI_RIGHT:
			vCurDir	+= ioMath::UNIT_X;
			break;
		case ioUserKeyInput::DKI_LEFTDOWN:
			vCurDir	-= ioMath::UNIT_X;	vCurDir	-= ioMath::UNIT_Y;
			break;
		case ioUserKeyInput::DKI_LEFTUP:
			vCurDir	-= ioMath::UNIT_X;	vCurDir	+= ioMath::UNIT_Y;
			break;
		case ioUserKeyInput::DKI_RIGHTDOWN:
			vCurDir	+= ioMath::UNIT_X;	vCurDir	-= ioMath::UNIT_Y;
			break;
		case ioUserKeyInput::DKI_RIGHTUP:
			vCurDir	+= ioMath::UNIT_X;	vCurDir	+= ioMath::UNIT_Y;
			break;
		};
		bMove = true;
	}

	if( !bMove )
		return;

	D3DXVec3Normalize( &vCurDir, &vCurDir );

	float fMoveSpeed = m_fTargetMarkerMoveSpeed * g_FrameTimer.GetSecPerFrame();
	D3DXVECTOR3 vTotalMove = vCurDir * fMoveSpeed;	
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();	
	m_vEffectPos += vTotalMove;
	
	if ( m_vEffectPos.x <= vCharPos.x - m_fWidthLimitRange )
		m_vEffectPos.x = vCharPos.x - m_fWidthLimitRange;
	else if ( m_vEffectPos.x >= vCharPos.x + m_fWidthLimitRange )
		m_vEffectPos.x = vCharPos.x + m_fWidthLimitRange;

	if ( m_vEffectPos.y <= vCharPos.y - m_fHeightLimitRange )
		m_vEffectPos.y = vCharPos.y - m_fHeightLimitRange;
	else if ( m_vEffectPos.y >= vCharPos.y + m_fHeightLimitRange )
		m_vEffectPos.y = vCharPos.y + m_fHeightLimitRange;

	pStage->CheckFlyMapLimit( &m_vEffectPos );
	pStage->CheckAxisLimit( &m_vEffectPos );

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwTargetMarkerID );
	if( pMapEffect )
		pMapEffect->SetWorldPosition( m_vEffectPos );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioEffectMoveAttackSkill::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioEffectMoveAttackSkill::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nMaxAttackCnt;
}

int ioEffectMoveAttackSkill::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nCurAttackCnt;
}