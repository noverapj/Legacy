
#include "stdafx.h"

#include "ioDestroySkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioDestroySkill::ioDestroySkill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_dwCurSkillProtectTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwCheckChargeTime = 0;
	m_bSuccess = false;
	m_dwStartEffectID = -1;
	m_dwFindEffectID = -1;
}

ioDestroySkill::ioDestroySkill( const ioDestroySkill &rhs )
	: ioNormalSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_dwLoopDuration( rhs.m_dwLoopDuration ),
	  m_szLoopAni( rhs.m_szLoopAni ),
	  m_dwChargeTime( rhs.m_dwChargeTime ),
	  m_fColRange( rhs.m_fColRange ),
	  m_fColAngle( rhs.m_fColAngle ),
	  m_szStartEffect( rhs.m_szStartEffect ),
	  m_szSuccessEffect( rhs.m_szSuccessEffect ),
	  m_szFailEffect( rhs.m_szFailEffect ),
	  m_szFindEffect( rhs.m_szFindEffect ),
	  m_vEffectStartOffset( rhs.m_vEffectStartOffset ),
	  m_vEffectSuccessOffset( rhs.m_vEffectSuccessOffset ),
	  m_vEffectFailOffset( rhs.m_vEffectFailOffset ),
	  m_vEffectFindOffset( rhs.m_vEffectFindOffset ),
	  m_vPassDummyList( rhs.m_vPassDummyList )
{	
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_dwCurSkillProtectTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwCheckChargeTime = 0;
	m_bSuccess = false;
	m_dwStartEffectID = -1;
	m_dwFindEffectID = -1;
}

ioDestroySkill::~ioDestroySkill()
{
}

void ioDestroySkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	//무적시간 설정
	m_dwSkillProtectTime    = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//루프 설정
	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	//루프 시간 설정
	m_dwLoopDuration = rkLoader.LoadInt_e( "loop_duration", 0 );

	m_dwChargeTime = rkLoader.LoadInt_e( "charge_time", 0 );

	m_fColRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fColAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_szStartEffect = szBuf;
	rkLoader.LoadString_e( "success_effect", "", szBuf, MAX_PATH );
	m_szSuccessEffect = szBuf;
	rkLoader.LoadString_e( "fail_effect", "", szBuf, MAX_PATH );
	m_szFailEffect = szBuf;
	rkLoader.LoadString_e( "find_effect", "", szBuf, MAX_PATH );
	m_szFindEffect = szBuf;

	m_vEffectStartOffset.x = rkLoader.LoadFloat_e( "start_effect_offset_x", 0.0f );
	m_vEffectStartOffset.y = rkLoader.LoadFloat_e( "start_effect_offset_y", 0.0f );
	m_vEffectStartOffset.z = rkLoader.LoadFloat_e( "start_effect_offset_z", 0.0f );

	m_vEffectSuccessOffset.x = rkLoader.LoadFloat_e( "success_effect_offset_x", 0.0f );
	m_vEffectSuccessOffset.y = rkLoader.LoadFloat_e( "success_effect_offset_y", 0.0f );
	m_vEffectSuccessOffset.z = rkLoader.LoadFloat_e( "success_effect_offset_z", 0.0f );

	m_vEffectSuccessOffset.x = rkLoader.LoadFloat_e( "success_effect_offset_x", 0.0f );
	m_vEffectSuccessOffset.y = rkLoader.LoadFloat_e( "success_effect_offset_y", 0.0f );
	m_vEffectSuccessOffset.z = rkLoader.LoadFloat_e( "success_effect_offset_z", 0.0f );

	m_vEffectFailOffset.x = rkLoader.LoadFloat_e( "fail_effect_offset_x", 0.0f );
	m_vEffectFailOffset.y = rkLoader.LoadFloat_e( "fail_effect_offset_y", 0.0f );
	m_vEffectFailOffset.z = rkLoader.LoadFloat_e( "fail_effect_offset_z", 0.0f );

	m_vEffectFindOffset.x = rkLoader.LoadFloat_e( "find_effect_offset_x", 0.0f );
	m_vEffectFindOffset.y = rkLoader.LoadFloat_e( "find_effect_offset_y", 0.0f );
	m_vEffectFindOffset.z = rkLoader.LoadFloat_e( "find_effect_offset_z", 0.0f );

	int nPassDummyCnt = rkLoader.LoadInt_e( "pass_dummy_cnt", 0 );
	m_vPassDummyList.clear();
	m_vPassDummyList.reserve( nPassDummyCnt );

	for ( int i=0; i<nPassDummyCnt; i++)
	{
		wsprintf_e( szKey, "pass_dummy_name%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vPassDummyList.push_back( szBuf );
	}
}

ioSkill* ioDestroySkill::Clone()
{
	return new ioDestroySkill( *this );
}

bool ioDestroySkill::OnSkillStart( ioBaseChar *pChar )
{	
	if ( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	m_SkillState = SS_NONE;
	m_dwCurSkillProtectTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwCheckChargeTime = 0;
	m_bSuccess = false;
	m_dwStartEffectID = -1;
	m_dwFindEffectID = -1;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	SetCameraBuff( pChar );

	//기본 게이지 감소 (마법진 게이지)
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;	
	
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

	return true;
}

void ioDestroySkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				//루프 상태로 전환
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			//키입력
			CheckInputKey( pChar );

			//루프 종료
			if ( m_dwCheckLoopTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
			}
		}
		break;
	case SS_CHARGE:
		{
			CheckChargeKey( pChar );
		}
		break;
	case SS_SUCCESS:
		{
			//모션 종료
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
			}
		}
		break;
	}
}

void ioDestroySkill::OnSkillEnd( ioBaseChar *pChar )
{
	if ( !m_bSuccess )
	{
		CreateActionEffect( pChar, m_szFailEffect, m_vEffectFailOffset );
	}

	DestroyEffect( pChar, m_dwStartEffectID );
	DestroyEffect( pChar, m_dwFindEffectID );
	
	ioNormalSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );
		
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = false;
	}
}

bool ioDestroySkill::IsProtected( int iDefenseBreakType ) const
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
	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;
	
	return false;
}

bool ioDestroySkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioDestroySkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioDestroySkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioDestroySkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioDestroySkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int nState;
	rkPacket >> nState;
	
	switch ( (SkillState)nState )
	{
	case SS_CHARGE:
		{
			CreateLoopEffect( pChar, m_szStartEffect, m_vEffectStartOffset, m_dwStartEffectID );
			m_SkillState = SS_CHARGE;
		}
		break;
	case SS_SUCCESS:
		{
			SetNormalSkill( pChar );
		
			FindTargetWeapon( pChar );
			FindTargetDummy( pChar );
			
			DestroyEffect( pChar, m_dwStartEffectID );
			DestroyEffect( pChar, m_dwFindEffectID );
			CreateActionEffect( pChar, m_szSuccessEffect, m_vEffectSuccessOffset );

			m_SkillState = SS_SUCCESS;
			m_bSuccess = true;
		}
		break;
	case SS_END:
		{
			m_SkillState = SS_END;
		}
		break;
	}
}

void ioDestroySkill::SetNormalSkill( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	
		return;

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//루프 상태 전환
void ioDestroySkill::SetLoopState( ioBaseChar *pChar )
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
	m_dwCheckLoopTime = dwCurTime;

	CreateLoopEffect( pChar, m_szFindEffect, m_vEffectFindOffset, m_dwFindEffectID );

	m_SkillState = SS_LOOP;	
}

//차지 상태로 전환
void ioDestroySkill::SetChargeState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckChargeTime = dwCurTime;

	CreateLoopEffect( pChar, m_szStartEffect, m_vEffectStartOffset, m_dwStartEffectID );

	m_SkillState = SS_CHARGE;
	
	//패킷 전송
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_CHARGE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//공격 상태로 전환
void ioDestroySkill::SetSuccessState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;
	
	//애니메이션
	SetNormalSkill( pChar );
	
	//타겟에 기능 적용
	FindTargetWeapon( pChar );
	FindTargetDummy( pChar );

	//이펙트
	DestroyEffect( pChar, m_dwStartEffectID );
	DestroyEffect( pChar, m_dwFindEffectID );
	CreateActionEffect( pChar, m_szSuccessEffect, m_vEffectSuccessOffset );

	m_SkillState = SS_SUCCESS;
	m_bSuccess = true;

	//패킷 전송
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_SUCCESS;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//종료 상태 설정
void ioDestroySkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_END;

	//패킷 전송
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//키입력 확인
void ioDestroySkill::CheckInputKey( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;	

	if( pChar->IsAttackKey() )
	{
		SetChargeState( pChar );
	}	
}

void ioDestroySkill::CheckChargeKey( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;	

	DWORD dwCurTime = FRAMEGETTIME();

	if ( pChar->IsAttackKeyDown() )
	{
		if ( m_dwCheckChargeTime + m_dwChargeTime <= dwCurTime )
			SetSuccessState( pChar );
	}
	else
	{
		if ( m_dwCheckChargeTime + m_dwChargeTime > dwCurTime )		
			SetEndState( pChar );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//무기 찾기
void ioDestroySkill::FindTargetWeapon( ioBaseChar *pChar )
{
	if ( !pChar )
		return;	

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	TargetWeaponList vTargetWeaponList;
		
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	//리스트 얻기
	pChar->GetTargetWeaponListBySphere( vPos, m_fColRange, m_fColAngle, vTargetWeaponList );

	if ( vTargetWeaponList.empty() )
		return;

	//폭발 설정
	SetMineExplosion( pChar, pStage, vTargetWeaponList );
}

//마인 폭발
void ioDestroySkill::SetMineExplosion( ioBaseChar *pChar, ioPlayStage *pStage, TargetWeaponList &vWeaponList )
{
	if ( !pChar || !pStage || vWeaponList.empty() )
		return;

	TargetWeaponList::iterator iter = vWeaponList.begin();
	for ( ; iter != vWeaponList.end() ; iter++ )
	{
		ioWeapon *pWeapon = *iter;

		ioBaseChar *pWeaponOwner = pWeapon->GetOwner();
		if ( !pWeaponOwner )
			continue;

		//같은 팀이면 패스
		if ( pChar->GetTeam() == pWeaponOwner->GetTeam() )
			continue;

		ioMineWeapon *pMine = ToMineWeapon( pWeapon );
		if ( pMine )
		{
			if( !pMine || !pMine->IsCanExplosion() ) continue;
			if( pMine->IsExperienceOwner() ) continue;

			pMine->SetWaitExplosionMine( pStage, true );	
		}
		
		ioMineWeapon2 *pMine2 = ToMineWeapon2( pWeapon );
		if ( pMine2 )
		{
			if( !pMine2 || !pMine2->IsCanExplosion() ) continue;
			if( pMine2->IsExperienceOwner() ) continue;

			pMine2->SetWaitExplosionMine( pStage, true );	
		}
		
		ioMineWeapon3 *pMine3 = ToMineWeapon3( pWeapon );
		if ( pMine3 )
		{
			if( !pMine3 || !pMine3->IsCanExplosion() ) continue;
			if( pMine3->IsExperienceOwner() ) continue;

			pMine3->SetWaitExplosionMine( pStage, true );	
		}
		
		ioMineWeapon4 *pMine4 = ToMineWeapon4( pWeapon );
		if ( pMine4 )
		{
			if( !pMine4 || !pMine4->IsCanExplosion() ) continue;
			if( pMine4->IsExperienceOwner() ) continue;

			pMine4->SetWaitExplosionMine( pStage, true );	
		}
	}
}

//더미 찾기
void ioDestroySkill::FindTargetDummy( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	TargetDummyList vTargetDummyList;
		
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	//리스트 얻기
	pChar->GetTargetDummyListBySphere( vPos, m_fColRange, m_fColAngle, vTargetDummyList );

	if ( vTargetDummyList.empty() )
		return;

	//더미 삭제
	SetDummyDestroy( pChar, vTargetDummyList );
}

//더미 삭제
void ioDestroySkill::SetDummyDestroy( ioBaseChar *pChar, TargetDummyList &vDummyList )
{
	if ( !pChar || vDummyList.empty() )
		return;

	TargetDummyList::iterator iter = vDummyList.begin();
	for ( ; iter != vDummyList.end() ; iter++ )
	{
		ioDummyChar *pDummy = *iter;

		ioBaseChar *pDummyOwner = pDummy->GetOwner();
		
		if ( !pDummyOwner )
			continue;

		//같은 팀이면 패스
		if ( pChar->GetTeam() == pDummyOwner->GetTeam() )
			continue;
				
		if ( !IsPassDummy( pDummy ) )
		{
			pDummy->SetDieState( true );
		}
	}
}

bool ioDestroySkill::IsPassDummy( ioDummyChar *pDummy )
{
	//패스 더미 리스트에서 검색
	int nPassDummyCnt = (int)m_vPassDummyList.size();
	for ( int i=0; i<nPassDummyCnt; i++ )
	{
		if ( m_vPassDummyList[i] == pDummy->GetDummyCharName() )
			return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//이펙트 생성
void ioDestroySkill::CreateActionEffect( ioBaseChar *pChar, ioHashString szEffect, D3DXVECTOR3 vOffset )
{
	if ( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;
	
	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + pChar->GetWorldOrientation() * vOffset;
	pStage->CreateMapEffect( szEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ));
}

void ioDestroySkill::CreateLoopEffect( ioBaseChar *pChar, ioHashString szEffect, D3DXVECTOR3 vOffset, DWORD &dwID )
{
	if ( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + pChar->GetWorldOrientation() * vOffset;

	ioMapEffect *pEffect = pStage->CreateMapEffect( szEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ));
	if ( pEffect )
	{
		dwID = pEffect->GetUniqueID();
	}
}

void ioDestroySkill::DestroyEffect( ioBaseChar *pChar, DWORD &dwID )
{
	if( !pChar )
		return;

	if( dwID != -1 )
	{
		pChar->EndMapEffect( dwID, false );
		dwID = -1;
	}
}