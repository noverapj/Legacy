

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioTargetDebuffSkill2.h"

#include "WeaponDefine.h"

ioTargetDebuffSkill2::ioTargetDebuffSkill2()
{
	ClearData();
}

ioTargetDebuffSkill2::ioTargetDebuffSkill2( const ioTargetDebuffSkill2 &rhs )
	: ioNormalSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
	m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_dwTargetMaxCnt( rhs.m_dwTargetMaxCnt ),
	m_szRedGuideMarker( rhs.m_szRedGuideMarker ),
	m_szBlueGuideMarker( rhs.m_szBlueGuideMarker ),
	m_szRedAimMarker( rhs.m_szRedAimMarker ),
	m_szBlueAimMarker( rhs.m_szBlueAimMarker ),
	m_szRedChargeMarker( rhs.m_szRedChargeMarker ),
	m_szBlueChargeMarker( rhs.m_szBlueChargeMarker ),
	m_szRedFireMarker( rhs.m_szRedFireMarker ),
	m_szBlueFireMarker( rhs.m_szBlueFireMarker ),
	m_fScalingRate( rhs.m_fScalingRate ),
	m_fCurScaleRate( rhs.m_fCurScaleRate ),
	m_fScalingSpeed( rhs.m_fScalingSpeed ),
	m_dwKeyDurationTime( rhs.m_dwKeyDurationTime ),
	m_AimAttribute( rhs.m_AimAttribute ),
	m_ZeroAimAttribute( rhs.m_ZeroAimAttribute ),
	m_TargetAimAttackBuffList( rhs.m_TargetAimAttackBuffList ),
	m_bSkillZeroAimAttackGauge( rhs.m_bSkillZeroAimAttackGauge ),
	m_bSkillAimAttackGauge( rhs.m_bSkillAimAttackGauge ),
	m_dwAimMarkerChargeEnableCnt( rhs.m_dwAimMarkerChargeEnableCnt ),
	m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime ),
	m_SkillMapEffect( rhs.m_SkillMapEffect ),
	m_fCharEffectOffset( rhs.m_fCharEffectOffset ),
	m_fTargetShowRange( rhs.m_fTargetShowRange ),
	m_dwLoopTime( rhs.m_dwLoopTime )
{
	ClearData();
}

ioTargetDebuffSkill2::~ioTargetDebuffSkill2()
{
}

void ioTargetDebuffSkill2::ClearData()
{
	m_TargetList.clear();

	m_bSetHandMesh = false;
	m_bSetMoveAni = false;
	m_bFireMaker = false;

	// 성장관련 요소
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	//maker
	m_dwGuidMaker = - 1;	
	m_dwMarkCurCnt = 0;

	m_dwCurScaleStartTime = 0;

	m_dwSkillAimMotionEnd = 0;

	m_dwFireStartTime = 0;

	m_dwSetLoopCnt = -1;
	m_dwCurAimMarkerChargeCnt = 0;

	m_dwStartSkillProtectedTime = 0;

	m_szTargetName.Clear();

	ResetSendList();

	m_dwCheckLoopTime = 0;
}

void ioTargetDebuffSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwKeyDurationTime = rkLoader.LoadInt_e( "skill_zero_aim_attack_duration", 0 );
	m_bSkillZeroAimAttackGauge = rkLoader.LoadBool_e( "skill_zero_aim_attack_reduce_gauge", 1 );

	wsprintf_e( szBuf, "skill_aim_attack");
	LoadAttackAttribute( szBuf, m_AimAttribute, rkLoader );

	wsprintf_e( szBuf, "skill_zero_aim_attack");
	LoadAttackAttribute( szBuf, m_ZeroAimAttribute, rkLoader );

	m_bSkillAimAttackGauge = rkLoader.LoadBool_e( "skill_aim_attack_reduce_gauge", 1 );

	m_dwAimMarkerChargeEnableCnt = rkLoader.LoadInt_e( "charge_marker_enable_cnt", 1000 );
	m_dwAimMarkerChargeEnableCnt = max( m_dwAimMarkerChargeEnableCnt, 1 );

	m_dwSkillProtectedTime = rkLoader.LoadFloat_e( "skill_protect_time", 0 );

	m_dwLoopTime = rkLoader.LoadFloat_e( "loop_time", 0 );

	// target search range
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetMaxRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_dwTargetMaxCnt = rkLoader.LoadInt_e( "attack_count", 0 );

	m_fTargetShowRange = rkLoader.LoadFloat_e( "target_show_range", 0.0f );

	//loop motion
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );	
	m_LoopDelayAnimation = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;

	// aim mark
	rkLoader.LoadString_e( "red_guide_marker", "", szBuf, MAX_PATH );
	m_szRedGuideMarker = szBuf;
	rkLoader.LoadString_e( "blue_guide_marker", "", szBuf, MAX_PATH );
	m_szBlueGuideMarker = szBuf;

	rkLoader.LoadString_e( "red_aim_marker", "", szBuf, MAX_PATH );
	m_szRedAimMarker = szBuf;
	rkLoader.LoadString_e( "blue_aim_marker", "", szBuf, MAX_PATH );
	m_szBlueAimMarker = szBuf;

	rkLoader.LoadString_e( "red_charge_marker", "", szBuf, MAX_PATH );
	m_szRedChargeMarker = szBuf;
	rkLoader.LoadString_e( "blue_charge_marker", "", szBuf, MAX_PATH );
	m_szBlueChargeMarker = szBuf;

	rkLoader.LoadString_e( "red_fire_marker", "", szBuf, MAX_PATH );
	m_szRedFireMarker = szBuf;	
	rkLoader.LoadString_e( "blue_fire_marker", "", szBuf, MAX_PATH );
	m_szBlueFireMarker = szBuf;	

	//scale info
	m_fScalingRate = rkLoader.LoadFloat_e( "charge_scaling_rate", 0.0f );
	m_fCurScaleRate = m_fScalingRate;
	m_fScalingSpeed = rkLoader.LoadFloat_e( "charge_scaling_speed", 0.0f );


	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffect = szBuf;

	m_fCharEffectOffset = rkLoader.LoadFloat_e( "skill_map_effect_offset", 0.0f );

	LoadBuffList( rkLoader );
}

void ioTargetDebuffSkill2::LoadBuffList( ioINILoader &rkLoader )
{
	int i, iCount;
	ioBuffInfo BuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_TargetAimAttackBuffList.clear();
	iCount = rkLoader.LoadInt_e( "skill_zero_aim_attack_buff_cnt", 0 );
	for( i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "skill_zero_aim_attack_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		BuffInfo.m_BuffName = szBuff;

		if( !BuffInfo.m_BuffName.IsEmpty() )
		{			
			BuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( BuffInfo.m_BuffName );			
			if( BuffInfo.m_iBuffType != BT_NONE )
			{	
				m_TargetAimAttackBuffList.push_back( BuffInfo );
			}
		}
	}
}

ioSkill* ioTargetDebuffSkill2::Clone()
{
	return new ioTargetDebuffSkill2( *this );
}

bool ioTargetDebuffSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ioNormalSkill::OnSkillStart( pChar );

	ClearData();
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_PRE;
	m_dwCheckTime = dwCurTime;
	m_dwStartSkillProtectedTime = dwCurTime;

	pChar->AttachEffect( m_SkillEffectName );

	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetCameraBuff( pChar );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillMoveEnable( false, false );

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );


			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
		}
	}

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetLoopState( pChar, pStage );
		CreateGuideMarker( pChar, pStage );
	}	

	m_dwCheckLoopTime = dwCurTime + m_dwLoopTime;

	return true;
}

void ioTargetDebuffSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetSkillMoveEnable( false, false );

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );		
	}

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	DestroyAllMarker( pChar, pStage );

	ClearData();
}

void ioTargetDebuffSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pChar );
				SetLoopState( pChar, pStage );
				CreateGuideMarker( pChar, pStage );				
			}
		}
		break;
	case SS_LOOP:
		{	
			if ( m_dwLoopTime > 0 && m_dwCheckLoopTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
				return;
			}

			CheckNormalMoveAni( pChar );
			UpdateGuideMarker( pChar, pStage );
			UpdateScaleInfo( pChar, pStage );

			ResetSendList();

			//첫번쨰 정 조준 타격 성공후에 연속적으로 타겟을 공격 할 경우 
			//지정된 조준->정조준 횟수 만큼 타겟팅 처리 후 타격 시도가 없으면 스킬 종료
			if( 0 < m_dwSetLoopCnt )
			{
				if(	m_dwAimMarkerChargeEnableCnt <= m_dwCurAimMarkerChargeCnt && !m_bFireMaker )
				{
					SetEndState( pChar, pStage );
					return;
				}
			}
			else
			{
				//첫번째 조준,정조준 타격 전까지만 유저를 찾는다.
				FindTarget( pChar, pStage );
			}

			UpdateTargetList( pChar, pStage );

			//동기화 처리
			if( pChar->IsNeedSendNetwork() )
				SendList( pChar, pStage );

			if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
			{
				if( !m_TargetList.empty() )
				{
					if( m_bFireMaker )
					{
						if( 0 < m_dwCurScaleStartTime && dwCurTime < m_dwCurScaleStartTime + m_dwKeyDurationTime )
							SetAimFireState( pChar, pStage, true );
						else
							SetAimFireState( pChar, pStage );
					}
					else
						SetAimFireState( pChar, pStage );
				}
				else
				{
					SetAimFireState( pChar, pStage );
				}
			}

			if( m_bFireMaker && m_dwCurScaleStartTime + m_dwKeyDurationTime < dwCurTime )
			{
				m_bFireMaker = false;
			}
		}
		break;
	case SS_ZERO_AIM_FIRE:
		{
			OnAimFireState( pChar, pStage, dwCurTime, true);

			if( 0 < m_dwSkillAimMotionEnd && m_dwSkillAimMotionEnd < dwCurTime )
			{				
				SetEndState( pChar, pStage );
			}
		}
		break;
	case SS_AIM_FIRE:
		{
			OnAimFireState( pChar, pStage, dwCurTime, false  );

			if( 0 < m_dwSkillAimMotionEnd && m_dwSkillAimMotionEnd < dwCurTime )
			{				
				SetEndState( pChar, pStage );
			}
		}
		break;
	case SS_END:
		{

		}
		break;
	}

	if( m_SkillState != SS_END )
	{
		//첫번째 타격 이후는 게이지 감소를 하지 않음
		if( m_dwSetLoopCnt <= 0 )
		{
			if( iTickGap >= iCurTicTime )
			{
				m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

				if( fCurTicGauge != 0.0f )
				{				
					if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
					{
						if( m_SkillState != SS_ZERO_AIM_FIRE && 
							m_SkillState != SS_AIM_FIRE &&
							m_SkillState != SS_END )
						{
							SetEndState( pChar, pStage );
						}
					}
				}
			}
		}
	}
}

void ioTargetDebuffSkill2::StartScaleInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurScaleStartTime == 0 )
	{		
		m_fCurScaleRate = m_fScalingRate;
		m_dwCurScaleStartTime = dwCurTime;
	}
}

void ioTargetDebuffSkill2::ResetScaleInfo( DWORD dwCurTime )
{
	m_fCurScaleRate = m_fScalingRate;
	m_dwCurScaleStartTime = dwCurTime;
}


void ioTargetDebuffSkill2::UpdateScaleInfo( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_bFireMaker )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	m_fCurScaleRate -= (fTimePerSec * m_fScalingSpeed);

	if( m_fCurScaleRate < FLOAT1 )
	{
		m_fCurScaleRate = m_fScalingRate;
		m_dwCurScaleStartTime = dwCurTime;

		m_bFireMaker = true;
		m_dwCurAimMarkerChargeCnt++;
	}	
}

void ioTargetDebuffSkill2::SetAimFireState( ioBaseChar *pChar, ioPlayStage *pStage, bool bZeroAim )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( bZeroAim )
	{
		m_SkillState = SS_ZERO_AIM_FIRE;
	}
	else
	{
		m_SkillState = SS_AIM_FIRE;
		DestroyAllMarker( pChar, pStage );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx(	bZeroAim == false ? m_AimAttribute.m_AttackAnimation : m_ZeroAimAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = (bZeroAim == false ? m_AimAttribute.m_fAttackAniRate : m_ZeroAimAttribute.m_fAttackAniRate) * m_fExtraAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwSkillAimMotionEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID );

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime == 0 )
	{
		m_dwFireStartTime = dwCurTime;
		m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	}

	pChar->SetSkillMoveEnable( false, false );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (bZeroAim ? SSC_ZERO_AIM_FIRE : SSC_AIM_FIRE);
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bSkillAimAttackGauge )
		pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem()->GetName() ) );
}

void ioTargetDebuffSkill2::OnAimFireState( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime, bool bZeroAim )
{	
	if( !pChar )
		return;

	if( !pStage )
		return;

	if( bZeroAim )
		UpdateAllMarker( pChar, pStage );

	int iWeaponCnt = 0;
	if( 0 < m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		if( !m_TargetList.empty() )
		{
			TargetList::iterator target_iter = m_TargetList.begin();
			m_CurFireIter = m_TargetList.begin();

			
			for( ; target_iter != m_TargetList.end(); ++target_iter )
			{				
				ioBaseChar* pTarget = pStage->GetBaseChar( (*target_iter).second.m_szTargetName );
				if( pTarget )
				{
					m_vAttackDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
					m_szTargetName = (*target_iter).second.m_szTargetName;

					WeaponInfoList::iterator iter;
					if( !bZeroAim )
					{
						iter = m_AimAttribute.m_vWeaponInfoList.begin();
						for( ; iter != m_AimAttribute.m_vWeaponInfoList.end(); ++iter )
						{
							pChar->SkillFireExplicit( (*iter),
								GetName(),
								m_dwWeaponBaseIndex+iWeaponCnt,
								pChar->GetWorldPosition() );

							iWeaponCnt++;
						}
					}
					else
					{
						iter = m_ZeroAimAttribute.m_vWeaponInfoList.begin();
						for( ; iter != m_ZeroAimAttribute.m_vWeaponInfoList.end(); ++iter )
						{
							pChar->SkillFireExplicit( (*iter),
								GetName(),
								m_dwWeaponBaseIndex+iWeaponCnt,
								pChar->GetWorldPosition() );

							iWeaponCnt++;
						}
					}
				}
			}
		}
		else
		{
			//int iWeaponCnt = 0;
			WeaponInfoList::iterator iter = m_AimAttribute.m_vWeaponInfoList.begin();
			m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			m_szTargetName.Clear();
			for( ; iter != m_AimAttribute.m_vWeaponInfoList.end(); ++iter )
			{
				pChar->SkillFireExplicit( (*iter),
					GetName(),
					m_dwWeaponBaseIndex+iWeaponCnt,
					pChar->GetWorldPosition() );

				iWeaponCnt++;
			}
		}

		if( bZeroAim )
			DestroyAllMarker( pChar, pStage );
		m_dwFireStartTime = 0;
	}
}

void ioTargetDebuffSkill2::CreateGuideMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;


	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	vPos.y = pChar->GetMidHeightByRate();
	vPos += ( vDir * m_fTargetShowRange );

	ioMapEffect *pMapEffect = NULL;

	if( pChar->IsNeedProcess() ) 
	{
		pMapEffect = pStage->CreateMapEffect( m_szBlueGuideMarker, vPos, vScale );
	}
	else
	{
		if( pChar->GetTeam() == TEAM_BLUE )
			pMapEffect = pStage->CreateMapEffect( m_szBlueGuideMarker, vPos, vScale );
		else
			pMapEffect = pStage->CreateMapEffect( m_szRedGuideMarker, vPos, vScale );
	}

	if( pMapEffect )
	{
		m_dwGuidMaker = pMapEffect->GetUniqueID();

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioTargetDebuffSkill2::UpdateGuideMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( !m_TargetList.empty() )
	{			
		DestroyGuideMarker( pChar );
		return;
	}

	if( m_dwGuidMaker == -1 && m_TargetList.empty() )
	{
		CreateGuideMarker( pChar, pStage );
		return;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vPos.y = pChar->GetMidHeightByRate();
	vPos += ( vDir * m_fTargetShowRange );

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwGuidMaker );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPos );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioTargetDebuffSkill2::DestroyGuideMarker( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwGuidMaker != -1 )
	{
		pChar->EndMapEffect( m_dwGuidMaker, false );
		m_dwGuidMaker = -1;
	}
}

void ioTargetDebuffSkill2::CreateFireMarker( ioBaseChar *pChar, ioPlayStage *pStage, EffectInfo& pInfo )
{
	if( !pChar ) return;
	if( !pStage ) return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( pInfo.m_dwFireEffectID );
	if( !pMapEffect )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		if( pChar->IsNeedProcess() ) 
		{
			pMapEffect = pStage->CreateMapEffect( m_szBlueFireMarker, vPos, vScale );
		}
		else
		{
			if( pChar->GetTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( m_szBlueFireMarker, vPos, vScale );
			else
				pMapEffect = pStage->CreateMapEffect( m_szRedFireMarker, vPos, vScale );
		}		
		if( pMapEffect )
		{
			pInfo.m_dwFireEffectID = pMapEffect->GetUniqueID();

			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
	}
}

void ioTargetDebuffSkill2::UpdateFireMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID )
{
	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	vTargetPos.y = pTarget->GetMidHeightByRate();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vTargetPos );
	}
}

void ioTargetDebuffSkill2::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	m_dwSetLoopCnt++;
	m_dwCurAimMarkerChargeCnt = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( true, true );

	m_bSetMoveAni = false;
}

void ioTargetDebuffSkill2::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{	
		if( !m_bSetMoveAni )
		{	
			iAniID = pGrp->GetAnimationIdx( m_LoopMoveAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );	
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}

void ioTargetDebuffSkill2::FindTarget( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;

		if( pTarget == pChar )
			continue;

		if( pTarget->GetTeam() == pChar->GetTeam() )
			continue;

		if( pTarget->IsSkipState() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;


		bool bEnableTarget = false;
		if( IsEnableTargetState( pTarget, true ) )
			bEnableTarget = true;

		DWORD dwCurTime = FRAMEGETTIME();

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fTargetMaxRange * m_fTargetMaxRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;

		//Already Target Find
		bool bFind = false;
		TargetList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
		if( ( m_TargetList.end() != iter ) )
			bFind = true;

		//Range Test
		bool bRangeAvailable = false;
		if( COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			bRangeAvailable = true;

		//Angle Test
		bool bAngleAvailable = false;
		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap <= m_fTargetAngle )
			bAngleAvailable = true;

		if( bRangeAvailable && bAngleAvailable )
		{	
			if( !bFind )
			{
				if( !bEnableTarget )
					continue;

				//타겟팅 되는 대상이 스킬 사용자에게 타겟팅 됬음을 알림
				if( m_dwMarkCurCnt < m_dwTargetMaxCnt )
				{
					AddTarget( pStage, pChar, pTarget, dwCurTime, m_bFireMaker );
					m_vAddTargetList.push_back( pTarget->GetCharName() );
				}
			}
			else
			{
				//2012. 6. 25 업데이트 부분 분리 => UpdateTargetList()
			}
		}
		else
		{
			//타겟팅에서 제외 되는 대상이 스킬 사용자에게 알림
			if( bFind )
			{				
				m_vRemoveTargetList.push_back( pTarget->GetCharName() );
			}
		}
	}

	if( 0 < m_TargetList.size() )
		StartScaleInfo();
	else
		ResetScaleInfo();
}

void ioTargetDebuffSkill2::AddTarget( ioPlayStage* pStage,
	ioBaseChar* pOwner,
	ioBaseChar* pTarget,
	DWORD dwCurTime,
	bool bMarker,
	bool CheckList )
{
	if( !pTarget )
		return;

	if( !pOwner )
		return;

	if( CheckList )
	{
		//리스트에 이미 존재하면 패스
		TargetList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
		if( ( m_TargetList.end() != iter ) )
			return;
	}

	//Create Effect
	EffectInfo pInfo;
	pInfo.m_dwCreateTime = dwCurTime;
	pInfo.m_szTargetName = pTarget->GetCharName();
	if( m_bFireMaker )
	{
		DestroyMarker( pOwner, pInfo.m_dwAimEffectID );
		DestroyMarker( pOwner, pInfo.m_dwChargeEffectID );
		CreateFireMarker( pOwner, pStage, pInfo );
	}
	else
	{
		DestroyMarker( pOwner, pInfo.m_dwFireEffectID );		
		CreateAimMarker( pOwner, pTarget, pStage, pInfo );				
		CreateChargeMarker( pOwner, pTarget, pStage, pInfo );
	}

	//Add Effect
	m_TargetList[pTarget->GetCharName()] = pInfo;
	m_dwMarkCurCnt++;
}

void ioTargetDebuffSkill2::RemoveTarget( ioBaseChar* pOwner, ioBaseChar* pTarget, bool CheckList )
{
	if( !pTarget )
		return;

	if( !pOwner )
		return;

	if( CheckList )
	{
		//리스트에 없으면 패스
		TargetList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
		if( ( m_TargetList.end() == iter ) )
			return;
	}

	//Destory Effect
	EffectInfo pInfo = m_TargetList[pTarget->GetCharName()];

	DestroyMarker( pOwner, pInfo.m_dwAimEffectID );
	DestroyMarker( pOwner, pInfo.m_dwChargeEffectID );
	DestroyMarker( pOwner, pInfo.m_dwFireEffectID );
	m_TargetList.erase(pTarget->GetCharName());

	m_dwMarkCurCnt--;
}

void ioTargetDebuffSkill2::RemoveTarget( ioBaseChar* pOwner, ioHashString szTargetName )
{
	//리스트에 없으면 패스
	TargetList::iterator iter = m_TargetList.find( szTargetName );
	if( ( m_TargetList.end() == iter ) )
		return;

	//Destory Effect
	EffectInfo pInfo = m_TargetList[szTargetName];

	DestroyMarker( pOwner, pInfo.m_dwAimEffectID );
	DestroyMarker( pOwner, pInfo.m_dwChargeEffectID );
	DestroyMarker( pOwner, pInfo.m_dwFireEffectID );

	m_TargetList.erase( szTargetName );
	m_dwMarkCurCnt--;
}

void ioTargetDebuffSkill2::UpdateTarget( ioBaseChar* pOwner,
	ioBaseChar* pTarget,
	ioPlayStage* pStage,
	bool bMarker )
{
	if( !pTarget )
		return;

	if( !pOwner )
		return;

	EffectInfo pInfo = m_TargetList[pTarget->GetCharName()];
	if( m_bFireMaker )
	{
		DestroyMarker( pOwner, pInfo.m_dwAimEffectID );
		DestroyMarker( pOwner, pInfo.m_dwChargeEffectID );

		//이미 존재한다면 함수 내에서 이펙트를 생성하지 않음
		CreateFireMarker( pOwner, pStage, pInfo );
		UpdateFireMarker( pTarget, pStage, pInfo.m_dwFireEffectID );

		m_TargetList[pTarget->GetCharName()] = pInfo;
	}
	else
	{
		DestroyMarker( pOwner, pInfo.m_dwFireEffectID );

		//이미 존재한다면 함수 내에서 이펙트를 생성하지 않음
		CreateAimMarker( pOwner, pTarget, pStage, pInfo );
		CreateChargeMarker( pOwner, pTarget, pStage, pInfo );

		UpdateAimMarker( pTarget, pStage, pInfo.m_dwAimEffectID );
		UpdateChargeMarker( pTarget, pStage, pInfo.m_dwChargeEffectID );	

		m_TargetList[pTarget->GetCharName()] = pInfo;
	}
}

void ioTargetDebuffSkill2::UpdateTargetList( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	TargetList::iterator target_iter;	
	for( target_iter = m_TargetList.begin(); target_iter != m_TargetList.end(); ++target_iter )
	{
		ioHashString szTargetName = (*target_iter).second.m_szTargetName;
		ioBaseChar* pTarget = pStage->GetBaseChar( szTargetName );

		//타겟 유효성 체크
		if( pTarget )
		{			
			if( IsEnableTargetState( pTarget, false ) )
			{
				UpdateTarget( pOwner, pTarget, pStage, m_bFireMaker );					
			}
			else
			{
				//타겟팅에서 제외 되는 대상이 스킬 사용자에게 알림
				m_vRemoveTargetList.push_back( szTargetName );
			}
		}
		else
		{
			m_vRemoveNonTargetList.push_back( szTargetName );
		}
	}

	//유저가 스테이지에 존재하지만 조건에 맞지 않는 경우
	int iSize = m_vRemoveTargetList.size();
	for( int i = 0; i < iSize; ++i )
	{
		ioHashString szTargetName = m_vRemoveTargetList[i];
		ioBaseChar* pTarget = pStage->GetBaseChar( szTargetName );

		if( pTarget )
		{
			RemoveTarget( pOwner, pTarget );
		}
	}

	//유저가 스테이지에 존재하지 않는 경우
	iSize = m_vRemoveNonTargetList.size();
	for( int i = 0; i < iSize; ++i )
	{		
		ioHashString szTargetName = m_vRemoveNonTargetList[i];
		RemoveTarget( pOwner, szTargetName );
	}
}

void ioTargetDebuffSkill2::ResetSendList()
{
	m_vAddTargetList.clear();
	m_vRemoveTargetList.clear();
	m_vRemoveNonTargetList.clear();
}

void ioTargetDebuffSkill2::SendList(  ioBaseChar* pOwner, ioPlayStage* pStage )
{
	int iAddUserSize = (int)m_vAddTargetList.size();
	int iRemoveUserSize = (int)m_vRemoveTargetList.size();
	int iRemoveNonUserSize = (int)m_vRemoveNonTargetList.size();

	if( iAddUserSize <= 0 && iRemoveUserSize <= 0 && iRemoveNonUserSize <= 0 )
		return;

	SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
	kPacket << pOwner->GetCharName();
	kPacket << GetName();
	kPacket << SSC_UPDATE_TARGETLIST;
	kPacket << iAddUserSize;

	if( 0 < iAddUserSize )
	{
		for( int i = 0 ; i < iAddUserSize; ++i )
			kPacket << m_vAddTargetList[i];
	}

	kPacket << iRemoveUserSize;
	if( 0 < iRemoveUserSize )
	{
		for( int i = 0 ; i < iRemoveUserSize; ++i )
			kPacket << m_vRemoveTargetList[i];
	}

	kPacket << iRemoveNonUserSize;
	if( 0 < iRemoveNonUserSize )
	{
		for( int i = 0 ; i < iRemoveNonUserSize; ++i )
			kPacket << m_vRemoveNonTargetList[i];
	}

	P2PNetwork::SendToAllPlayingUser( kPacket );

}

bool ioTargetDebuffSkill2::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bCheckProtected )
{
	if( !pTarget )
		return false;

	if( bCheckProtected )
	{	
		if( pTarget->IsProtectState() )
			return false;

		if( pTarget->IsSkillProtected() )
			return false;
	}

	if( 0 < m_dwSetLoopCnt )
	{
		if( !pTarget->HasBuff( BT_FLAME_STATUE ) )
			return false;
	}

	ioPlayStage *pStage = pTarget->GetCreator();
	if( pStage )
	{
		ioPlayMode *pMode = pStage->GetPlayMode();
		if( pMode && pMode->GetModeType() == MT_FOOTBALL )
			return false;
	}
	else{
		return false;
	}

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioTargetDebuffSkill2::CreateAimMarker( ioBaseChar* pChar, ioBaseChar *pTarget, ioPlayStage* pStage, EffectInfo& pInfo )
{		
	if( !pTarget ) return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( pInfo.m_dwAimEffectID );
	if( !pMapEffect )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vTargetPos.y = pTarget->GetMidHeightByRate();

		if( pChar->IsNeedProcess() ) 
		{
			pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vTargetPos, vScale );
		}
		else
		{
			if( pChar->GetTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vTargetPos, vScale );
			else
				pMapEffect = pStage->CreateMapEffect( m_szRedAimMarker, vTargetPos, vScale );
		}

		if( pMapEffect )
		{	
			pInfo.m_dwAimEffectID = pMapEffect->GetUniqueID();
		}
	}
}

void ioTargetDebuffSkill2::UpdateAimMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID )
{
	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	vTargetPos.y = pTarget->GetMidHeightByRate();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vTargetPos );
	}

}

void ioTargetDebuffSkill2::DestroyMarker( ioBaseChar *pChar, DWORD& dwMarker )
{	
	if( !pChar )
		return;

	if( dwMarker != -1 )
	{
		pChar->EndMapEffect( dwMarker, false );
		dwMarker = -1;
	}
}

void ioTargetDebuffSkill2::CreateChargeMarker(ioBaseChar *pChar, ioBaseChar *pTarget, ioPlayStage* pStage, EffectInfo& pInfo )
{		
	if( !pTarget ) return;
	if( !pStage ) return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( pInfo.m_dwChargeEffectID );
	if( !pMapEffect )
	{
		D3DXVECTOR3 vScale( m_fCurScaleRate, m_fCurScaleRate, m_fCurScaleRate );
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vTargetPos.y = pTarget->GetMidHeightByRate();

		if( pChar->IsNeedProcess() )
		{
			pMapEffect = pStage->CreateMapEffect( m_szBlueChargeMarker, vTargetPos, vScale );
		}
		else
		{
			if( pChar->GetTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( m_szBlueChargeMarker, vTargetPos, vScale );
			else
				pMapEffect = pStage->CreateMapEffect( m_szRedChargeMarker, vTargetPos, vScale );
		}
		if( pMapEffect )
		{
			pInfo.m_dwChargeEffectID = pMapEffect->GetUniqueID();		
		}
	}
}

void ioTargetDebuffSkill2::UpdateChargeMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID )
{
	if( !pTarget ) return;
	if( !pStage ) return;

	float fScaleRate = m_fCurScaleRate;

	D3DXVECTOR3 vScale( fScaleRate, fScaleRate, fScaleRate );
	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	vTargetPos.y = pTarget->GetMidHeightByRate();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vTargetPos );
		ioEffect* pEffect = pMapEffect->GetEffect();
		if( pEffect )
		{
			pEffect->GetParentNode()->SetScale( vScale );
		}
	}
}

void ioTargetDebuffSkill2::UpdateAllMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	TargetList::iterator target_iter = m_TargetList.begin();
	for( ; target_iter != m_TargetList.end(); ++target_iter )
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( (*target_iter).second.m_szTargetName );
		if( pTarget )
		{
			UpdateFireMarker( pTarget, pStage, (*target_iter).second.m_dwFireEffectID );
			UpdateAimMarker( pTarget, pStage, (*target_iter).second.m_dwAimEffectID );
			UpdateChargeMarker(  pTarget, pStage, (*target_iter).second.m_dwChargeEffectID );
		}
	}
}

void ioTargetDebuffSkill2::DestroyAllMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DestroyGuideMarker( pChar );

	TargetList::iterator iter = m_TargetList.begin();
	for( ; iter != m_TargetList.end(); ++iter )
	{	
		EffectInfo pInfo = (*iter).second;

		if( pInfo.m_dwAimEffectID != -1 )
		{			
			DestroyMarker( pChar, pInfo.m_dwAimEffectID );
			pInfo.m_dwAimEffectID = -1;
		}

		if( pInfo.m_dwChargeEffectID != -1 )
		{
			DestroyMarker( pChar, pInfo.m_dwChargeEffectID );
			pInfo.m_dwChargeEffectID = -1;
		}

		if( pInfo.m_dwFireEffectID != -1 )
		{
			DestroyMarker( pChar, pInfo.m_dwFireEffectID );
			pInfo.m_dwFireEffectID = -1;
		}

		(*iter).second = pInfo;
	}
}

void ioTargetDebuffSkill2::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_END;

	DestroyAllMarker( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetDebuffSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{	
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( !pStage )
		return;

	ioTraceWeaponType2	*pTrace2 = ToTraceWeaponType2( pWeapon );
	if( pTrace2 )
	{
		pTrace2->SetMoveDir( m_vAttackDir );
		pTrace2->SetTraceTarget( m_szTargetName );
	}
}

void ioTargetDebuffSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{

	DWORD dwCurTime = FRAMEGETTIME();
	int iType;
	rkPacket >> iType;


	switch( iType )
	{
	case SSC_UPDATE_TARGETLIST:
		{
			int iAddUserSize = 0;
			int iRemoveUserSize = 0;
			int iRemoveNonUserSize = 0;

			rkPacket >> iAddUserSize;
			if( 0 < iAddUserSize )
			{
				for( int i = 0; i <iAddUserSize; ++i )
				{
					ioHashString szCharName;
					rkPacket >> szCharName;

					ioBaseChar* pTarget = pStage->GetBaseChar( szCharName );
					if( pTarget )
						AddTarget( pStage, pChar, pTarget, dwCurTime, m_bFireMaker, true );
				}
			}

			rkPacket >> iRemoveUserSize;
			if( 0 < iRemoveUserSize )
			{
				for( int i = 0; i < iRemoveUserSize; ++i ) 
				{
					ioHashString szCharName;
					rkPacket >> szCharName;

					ioBaseChar* pTarget = pStage->GetBaseChar( szCharName );
					if( pTarget )
						RemoveTarget( pChar, pTarget, true );
				}
			}

			rkPacket >> iRemoveNonUserSize;
			if( 0 < iRemoveNonUserSize )
			{
				for( int i = 0; i < iRemoveNonUserSize; ++i ) 
				{
					ioHashString szCharName;
					rkPacket >> szCharName;					
					RemoveTarget( pChar, szCharName );
				}
			}
		}
		break;
	case SSC_ZERO_AIM_FIRE:
		{
			SetAimFireState(pChar, pStage, true );
		}
		break;
	case SSC_AIM_FIRE:
		{
			SetAimFireState( pChar, pStage );
		}
		break;
	case SSC_END:
		{
			SetEndState( pChar, pStage );
		}
		break;
	}
}

bool ioTargetDebuffSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetDebuffSkill2::IsProtected( int iDefenseBreakType ) const
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

	switch( m_SkillState )
	{
	case SS_LOOP:
		{
			if( 0 < m_dwStartSkillProtectedTime && 
				dwCurTime < m_dwStartSkillProtectedTime + m_dwSkillProtectedTime )
				return true;
		}
		break;			
	case SS_ZERO_AIM_FIRE:
	case SS_AIM_FIRE:				
		if( 0 < m_dwStartSkillProtectedTime && 
			dwCurTime < m_dwStartSkillProtectedTime + m_dwSkillProtectedTime )
			return true;
		else
			if( 0 < m_dwFireMotionEndTime && dwCurTime <= m_dwFireMotionEndTime )
				return true;
	}

	return false;
}

bool ioTargetDebuffSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_ZERO_AIM_FIRE:
	case SS_AIM_FIRE:
		return false;
	case SS_END:
		return true;
	}

	return true;
}
