

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioCountLightSkill.h"

ioCountLightSkill::ioCountLightSkill()
{
	m_iCurUseCount = 0;
	m_dwMotionEndTime = 0;
	m_dwCheckLoopTime = 0;
	m_SkillState = SS_None;
	m_bIsDropZone = false;
	m_dwEffectID = -1;
	m_nGrade = -1;
	m_dwLoopTime = 0;
	m_fCircleMoveSpeed = 0.0f;
	m_fCircleRange = 0.0f;
}

ioCountLightSkill::ioCountLightSkill( const ioCountLightSkill &rhs )
	: ioCountSkill( rhs ),
	  m_InfoList( rhs.m_InfoList ),
	  m_fCircleRange( rhs.m_fCircleRange ),
	  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	  m_fCircleOffSet( rhs.m_fCircleOffSet ),
	  m_ReadyCircle( rhs.m_ReadyCircle ),
	  m_szLoopAni( rhs.m_szLoopAni ),
	  m_dwLoopTime( rhs.m_dwLoopTime ),
	  m_DropZoneOffset( rhs.m_DropZoneOffset ),
	  m_vCreateBuffList( rhs.m_vCreateBuffList ),
	  m_vRemoveBuffList( rhs.m_vRemoveBuffList ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_iCurUseCount = 0;
	m_dwMotionEndTime = 0;
	m_dwCheckLoopTime = 0;
	m_SkillState = SS_None;
	m_bIsDropZone = false;
	m_dwEffectID = -1;
	m_nGrade = -1;
	m_dwLoopTime = 0;
	m_fCircleMoveSpeed = 0.0f;
	m_fCircleRange = 0.0f;
}

ioCountLightSkill::~ioCountLightSkill()
{
}

void ioCountLightSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	//버프 읽기
	LoadBuffInfo( rkLoader );

	//정보 읽기
	LoadEnableInfo( rkLoader );
	
	char szBuf[MAX_PATH];

	//마법진 설정
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
		
	//루프 애니 설정
	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	
	//드랍존 오프셋 설정
	m_DropZoneOffset = rkLoader.LoadFloat_e( "drop_zone_offset", 0.0f );

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioCountLightSkill::Clone()
{
	return new ioCountLightSkill( *this );
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountLightSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountLightSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioCountSkill::OnSkillStart(pChar) )
		return false;
	
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwCheckLoopTime = 0;
	m_SkillState = SS_None;
	m_bIsDropZone = false;
	m_dwCurSkillProtectTime = 0;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	SetSkillInfo( m_nGrade );

	//게이지, 등급 초기화
	m_iCurUseCount = 0;
	m_nGrade = -1;
	
	//이펙트 해제
	if ( m_dwEffectID != -1)
	{
		pChar->EndEffect( m_dwEffectID	);
		m_dwEffectID = -1;
	}

	return true;
}

void ioCountLightSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				SetActionState( pChar );
			}
		}
		break;
	case SS_Action:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar );
			}
		}
		break;
	case SS_Loop:
		{
			if ( m_dwCheckLoopTime + m_dwLoopTime < dwCurTime )
			{
				SetTeleportState( pChar );
				return;
			}

			UpdateMagicCircle( pChar );
			CheckRotate( pChar );
			CheckInputKey( pChar );
		}
		break;
	case SS_Teleport:
		{
			SetEndState( pChar );
		}
		break;
	}
}

void ioCountLightSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioCountSkill::OnSkillEnd( pChar );

	pChar->SetSpecialHide( false );

	//마법진 제거
	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	//버프 제거
	RemoveBuff( pChar );

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	//이펙트 해제
	if ( m_dwEffectID != -1)
	{
		pChar->EndEffect( m_dwEffectID	);
		m_dwEffectID = -1;
	}

	m_iCurUseCount = 0;
}

void ioCountLightSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int nState;
	rkPacket >> nState;

	switch ( (SkillState)nState )
	{
	case SS_Teleport:
		{
			D3DXVECTOR3 vTeleportPos;
			rkPacket >> vTeleportPos;
			
			pChar->SetWorldPosition( vTeleportPos );

			rkPacket >> m_bIsDropZone;

			m_SkillState = SS_Teleport;
		}
		break;
	}
}

void ioCountLightSkill::ApplyEquipSlotSkillExtraInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int nState;
	rkPacket >> nState;

	switch ( (SkillState)nState )
	{
	case SS_Effect:
		{
			rkPacket >> m_nGrade;

			if ( m_InfoList.empty() || m_nGrade == -1 || m_nGrade >= (int)m_InfoList.size() )
				return;

			const EnableInfo &sInfo = m_InfoList[m_nGrade];
			SetEffect( pChar, sInfo.szEffect );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountLightSkill::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_Action:
	case SS_Loop:
		return true;
	}
	return false;
}

bool ioCountLightSkill::IsProtected( int iDefenseBreakType ) const
{
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

bool ioCountLightSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_End )
	{
		return true;
	}
	return false;
}

bool ioCountLightSkill::IsEndState() const
{
	if ( m_SkillState == SS_End || m_bIsDropZone )
	{
		return true;
	}
	return false;
}

void ioCountLightSkill::RecoveryCount( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	ioCountSkill::RecoveryCount( pChar );

	int nInfoCnt = (int)m_InfoList.size();
	for (int i=0;i<nInfoCnt; i++)
	{
		const EnableInfo &sInfo = m_InfoList[i];
		if ( sInfo.nMinGauge <= m_iCurUseCount && sInfo.nMaxGuage > m_iCurUseCount )
		{			
			if ( m_nGrade != i)
			{
				m_nGrade = i;
				SetEffect( pChar, sInfo.szEffect );

				//패킷 전송
				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EQUIP_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SS_Effect;
					kPacket << m_nGrade;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;			
			}
		}
	}	
}

void ioCountLightSkill::OnSkillItemRelease( ioBaseChar *pChar )
{
	//이펙트 해제
	if ( m_dwEffectID != -1)
	{
		pChar->EndEffect( m_dwEffectID	);
		m_dwEffectID = -1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//스킬 사용 상태 얻기
bool ioCountLightSkill::IsCanDownSkill() const
{
	return FindUseEnable( UE_DOWN );
}

bool ioCountLightSkill::IsCanJumpingSkill() const
{
	return FindUseEnable( UE_JUMP );
}

bool ioCountLightSkill::IsCanJumpingSkillOnBlowWound() const
{
	return FindUseEnable( UE_JUMP_BLOW );
}

bool ioCountLightSkill::IsCanWoundSkill() const
{
	return FindUseEnable( UE_WOUND );
}

bool ioCountLightSkill::IsCanBlowWoundSkill() const
{
	return FindUseEnable( UE_BLOW );
}

//////////////////////////////////////////////////////////////////////////////////////
//버프 읽기
void ioCountLightSkill::LoadBuffInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int nCreateCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_vCreateBuffList.clear();
	for ( int i=0; i<nCreateCnt; i++ )
	{
		wsprintf_e( szKey, "owner_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vCreateBuffList.push_back( szBuf );
	}
	
	int nRemoveCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_vRemoveBuffList.clear();
	for ( int i=0; i<nRemoveCnt; i++ )
	{
		wsprintf_e( szKey, "remove_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vRemoveBuffList.push_back( szBuf );
	}
}

//정보 읽기
void ioCountLightSkill::LoadEnableInfo( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	int nInfoCnt = rkLoader.LoadInt_e( "enable_info_cnt", 0 );

	m_InfoList.clear();
	m_InfoList.reserve( nInfoCnt );

	for ( int i=0; i<nInfoCnt; i++ )
	{
		EnableInfo sEnableInfo;
		//최소
		wsprintf_e( szKey, "enable_info%d_min_gauge", i+1 );
		sEnableInfo.nMinGauge = rkLoader.LoadInt( szKey, 0 );
		//최대
		wsprintf_e( szKey, "enable_info%d_max_gauge", i+1 );
		sEnableInfo.nMaxGuage = rkLoader.LoadInt( szKey, 0 );
		//조건
		wsprintf_e( szKey, "enable_info%d_use_down", i+1 );
		sEnableInfo.bUseEnable[UE_DOWN] = rkLoader.LoadBool( szKey, false );
		wsprintf_e( szKey, "enable_info%d_use_jump", i+1 );
		sEnableInfo.bUseEnable[UE_JUMP] = rkLoader.LoadBool( szKey, false );		
		wsprintf_e( szKey, "enable_info%d_use_jump_blow", i+1 );
		sEnableInfo.bUseEnable[UE_JUMP_BLOW] = rkLoader.LoadBool( szKey, false );		
		wsprintf_e( szKey, "enable_info%d_use_wound", i+1 );
		sEnableInfo.bUseEnable[UE_WOUND] = rkLoader.LoadBool( szKey, false );		
		wsprintf_e( szKey, "enable_info%d_use_blow", i+1 );
		sEnableInfo.bUseEnable[UE_BLOW] = rkLoader.LoadBool( szKey, false );
		wsprintf( szKey, "enable_info%d_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sEnableInfo.szEffect = szBuf;
		
		wsprintf_e( szKey, "enable_info%d_duration", i+1 );
		sEnableInfo.fDuration = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "enable_info%d_magic_circle_range", i+1 );
		sEnableInfo.fRange = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "enable_info%d_magic_circle_move_speed", i+1 );
		sEnableInfo.fSpeed = rkLoader.LoadFloat( szKey, 0.0f );

		m_InfoList.push_back( sEnableInfo );
	}
}

//설정 정보 얻기
bool ioCountLightSkill::FindUseEnable( UseEnable eType ) const
{
	int nInfoCnt = (int)m_InfoList.size();
	for ( int i=0; i<nInfoCnt; i++ )
	{
		//게이지 조건 확인 최소 이상 최대 미만
		if ( m_iCurUseCount >= m_InfoList[i].nMinGauge && 
			 m_iCurUseCount < m_InfoList[i].nMaxGuage )
		{
			return m_InfoList[i].bUseEnable[eType];
		}
	}

	return false;
}

void ioCountLightSkill::SetSkillInfo( int nGrade )
{
	if ( m_InfoList.empty() || nGrade == -1 || nGrade >= (int)m_InfoList.size() )
		return;

	const EnableInfo &sInfo = m_InfoList[nGrade];
	m_dwLoopTime = sInfo.fDuration;
	m_fCircleRange = sInfo.fRange;
	m_fCircleMoveSpeed = sInfo.fSpeed;
}

//////////////////////////////////////////////////////////////////////////////////////
//액션 상태 설정
void ioCountLightSkill::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	ioHashString szAniName = GetSkillMotionName();
	float fTimeRate = GetSkillMotionRate();

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( szAniName );
	if ( iAniID != -1 )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	}

	m_SkillState = SS_Action;
}

//루프 상태 설정
void ioCountLightSkill::SetLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( iAniID != -1 )
		pGrp->SetLoopAni( iAniID, FLOAT100 );	

	CreateMagicCircle( pChar );
	CreateBuff( pChar );
	pChar->SetSpecialHide( true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckLoopTime = dwCurTime;
	m_SkillState = SS_Loop;
}

//텔레포트 상태 설정
void ioCountLightSkill::SetTeleportState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;
	
	GetMagicCirclePos( pChar );

	D3DXVECTOR3 TeleportPos = m_vCirclePos;

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( TeleportPos.x, TeleportPos.z );
	
	if ( fMapHeight <= 0.0f )
	{
		TeleportPos.y = m_vCirclePos.y + m_DropZoneOffset;
		m_bIsDropZone = true;
	}

	pChar->SetWorldPosition( TeleportPos );

	m_SkillState = SS_Teleport;

	//패킷 전송
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Teleport;
		kPacket << TeleportPos;
		kPacket << m_bIsDropZone;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//종료 상태 설정
void ioCountLightSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_End;
}

//////////////////////////////////////////////////////////////////////////////////////
//마법진 생성
void ioCountLightSkill::CreateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( !pChar->CheckMagicCircle() )
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_ReadyCircle, m_fCircleOffSet );
}

//마법진 업데이트
void ioCountLightSkill::UpdateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( pChar->CheckMagicCircle() )
	{
		//뱡향 체크
		pChar->CheckChangeDirectionForMagicCircle();
		//위치 갱신
		pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );
	}
}

//마법진 위치 얻기
void ioCountLightSkill::GetMagicCirclePos( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
}

//마법진 방향으로 회전
void ioCountLightSkill::CheckRotate( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	//마법진 방향으로 캐릭터 회전
	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
}

//키입력 확인
void ioCountLightSkill::CheckInputKey( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;	

	if( pChar->IsAttackKey() )
	{
		SetTeleportState( pChar );
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//버프 생성
void ioCountLightSkill::CreateBuff( ioBaseChar *pChar )
{
	if ( !pChar || m_vCreateBuffList.empty() )
		return;

	int nCreateCnt = (int)m_vCreateBuffList.size();
	for ( int i=0; i<nCreateCnt; i++ )
	{
		//해당 이름의 버프가 없을때
		if ( !pChar->HasBuff( m_vCreateBuffList[i] ) )
			pChar->AddNewBuff( m_vCreateBuffList[i], pChar->GetCharName(), "", NULL, false );
	}
}

//버프 제거
void ioCountLightSkill::RemoveBuff( ioBaseChar *pChar )
{
	if ( !pChar || m_vRemoveBuffList.empty() )
		return;

	int nRemoveCnt = (int)m_vRemoveBuffList.size();
	for ( int i=0; i<nRemoveCnt; i++ )
	{
		//해당 이름의 버프가 있을때
		if ( pChar->HasBuff( m_vRemoveBuffList[i] ) )
			pChar->RemoveBuff( m_vRemoveBuffList[i], true );
	}
}

//이펙트 설정
void ioCountLightSkill::SetEffect( ioBaseChar *pChar, ioHashString szEffectName )
{
	if ( !pChar )
		return;

	//이펙트 해제
	if ( m_dwEffectID != -1)
	{
		pChar->EndEffect( m_dwEffectID	);
		m_dwEffectID = -1;
	}

	if ( szEffectName.IsEmpty() )
		return;

	ioEffect *pEffect = pChar->AttachEffect( szEffectName );
	if ( pEffect )
		m_dwEffectID = pEffect->GetUniqueID();
}

int ioCountLightSkill::GetCurUseCount() 
{ 
	return m_iCurUseCount/10; 
}

int ioCountLightSkill::GetMaxUseCount()
{ 
	return m_iMaxUseCount/10;
}

int ioCountLightSkill::GetNeedCount()
{
	return m_iNeedCount/10; 
}