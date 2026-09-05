
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioTwoMagicCircleSkill.h"

ioTwoMagicCircleSkill::ioTwoMagicCircleSkill()
{
	ClearData();
}

ioTwoMagicCircleSkill::ioTwoMagicCircleSkill( const ioTwoMagicCircleSkill &rhs )
: ioNormalSkill( rhs ),
m_fFirstCircleRange( rhs.m_fFirstCircleRange ),
m_fFirstCircleMoveSpeed( rhs.m_fFirstCircleMoveSpeed ),
m_fFirstCircleHeightGap( rhs.m_fFirstCircleHeightGap ),
m_fFirstCircleOffSet( rhs.m_fFirstCircleOffSet ),
m_fSecondCircleMaxRange( rhs.m_fSecondCircleMaxRange ),
m_fSecondCircleMinRange( rhs.m_fSecondCircleMinRange ),
m_fSecondCircleMoveSpeed( rhs.m_fSecondCircleMoveSpeed ),
m_fSecondCircleHeightGap( rhs.m_fSecondCircleHeightGap ),
m_fSecondCircleOffSet( rhs.m_fSecondCircleOffSet ),
m_OnCircle( rhs.m_OnCircle ),
m_OffCircle( rhs.m_OffCircle ),
m_szStampCircleEffect( rhs.m_szStampCircleEffect ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_dwProtectTime( rhs.m_dwProtectTime ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_fCircleKeyOnHeightMin( rhs.m_fCircleKeyOnHeightMin ),
 m_fCircleKeyOnHeightMax( rhs.m_fCircleKeyOnHeightMax ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_SkillMotion( rhs.m_SkillMotion ),
 m_fSkillMototionRate(  rhs.m_fSkillMototionRate ),
 m_fSkillEndMototionRate( rhs.m_fSkillEndMototionRate ),
 m_fCircleToCircleGap( rhs.m_fCircleToCircleGap )

{
	ClearData();
}

ioTwoMagicCircleSkill::~ioTwoMagicCircleSkill()
{
}

void ioTwoMagicCircleSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fFirstCircleRange = rkLoader.LoadFloat_e( "first_magic_circle_range", 0.0f );
	m_fFirstCircleMoveSpeed = rkLoader.LoadFloat_e( "first_magic_circle_move_speed", 0.0f );
	m_fFirstCircleHeightGap = rkLoader.LoadFloat_e( "first_magic_circle_height_gap", 0.0f );
	m_fFirstCircleOffSet = rkLoader.LoadFloat_e( "first_magic_circle_offset", 0.0f );

	m_fSecondCircleMaxRange = rkLoader.LoadFloat_e( "second_magic_circle_max_range", 0.0f );
	m_fSecondCircleMinRange = rkLoader.LoadFloat_e( "second_magic_circle_min_range", 0.0f );
	m_fSecondCircleMoveSpeed = rkLoader.LoadFloat_e( "second_magic_circle_move_speed", 0.0f );
	m_fSecondCircleHeightGap = rkLoader.LoadFloat_e( "second_magic_circle_height_gap", 0.0f );
	m_fSecondCircleOffSet = rkLoader.LoadFloat_e( "second_magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "stamp_magic_circle", "", szBuf, MAX_PATH );
	m_szStampCircleEffect = szBuf;

	rkLoader.LoadString_e( "move_key_on_magic_circle", "", szBuf, MAX_PATH );
	m_OnCircle = szBuf;
	rkLoader.LoadString_e( "move_key_off_magic_circle", "", szBuf, MAX_PATH );
	m_OffCircle = szBuf;

	m_fCircleKeyOnHeightMin = rkLoader.LoadFloat_e( "second_magic_circle_key_on_height_min", 0.0f );
	m_fCircleKeyOnHeightMax = rkLoader.LoadFloat_e( "second_magic_circle_key_on_height_max", 0.0f );

	m_fCircleToCircleGap = rkLoader.LoadFloat_e( "circle_to_circle_offset", FLOAT100 );

	m_dwProtectTime = rkLoader.LoadInt_e( "loop_protect_time", 0 );

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	rkLoader.LoadString_e( "skill_motion", "", szBuf, MAX_PATH );
	m_SkillMotion = szBuf;	
	m_fSkillMototionRate =  rkLoader.LoadFloat_e( "animation_time_rate", FLOAT1 );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;	

	m_fSkillEndMototionRate =  rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );
}

ioSkill* ioTwoMagicCircleSkill::Clone()
{
	return new ioTwoMagicCircleSkill( *this );
}

void ioTwoMagicCircleSkill::ClearData()
{
	m_State = SS_NONE;
	m_CircleState = FIRST_CIRCLE;

	m_dwLoopDurationStart = 0;

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwEndMotionEndTime = 0;
	m_fFirstMapHeight = 0;

	m_vCirclePos = ioMath::VEC3_ZERO;
	m_vFirstCirclePos = ioMath::VEC3_ZERO;
	m_vSecondCirclePos =  ioMath::VEC3_ZERO; 

	m_bFirstOffCircle = false;
	m_bSecondOffCircle = false;

	m_CurCircleEffect.Clear();

	m_bDropzone = false;

	m_dwFirstStampCircleEffect = - 1;
	m_dwSecondStampCircleEffect= - 1;
	
	m_bSetHandMesh = false;
	m_bReduceGauge = false;

	m_bEnableSecnodCircle = false;

	m_dwCheckCallCircleCnt = 0;

	m_CurCircleEffect = m_OnCircle;

	m_SkillMotionEndTime = 0;
}

bool ioTwoMagicCircleSkill::OnSkillStart( ioBaseChar *pChar )
{	
	ioNormalSkill::OnSkillStart( pChar );

	ClearData();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );
	CheckCharMoveDir( pChar );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetLoopState( pChar );
	}	

	return true;
}

void ioTwoMagicCircleSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );
	
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->DestroyMagicCircle();

	if( m_dwFirstStampCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwFirstStampCircleEffect, false );
		m_dwFirstStampCircleEffect = -1;
	}

	if( m_dwSecondStampCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSecondStampCircleEffect, false );
		m_dwSecondStampCircleEffect = -1;
	}

	ClearData();
}

void ioTwoMagicCircleSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{	
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetLoopState( pChar );
			}
		}
		break;

	case SS_LOOP: 
		{	
			if( 0 < m_dwLoopDurationStart && dwCurTime < m_dwLoopDurationStart + m_dwLoopDuration )
			{
				CheckCircle( pChar, pStage );
			}

			if( 0 < m_dwLoopDurationStart && m_dwLoopDurationStart + m_dwLoopDuration < dwCurTime )
			{
				D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();
				m_vSecondCirclePos =  vPos;

				ChangeCircleDropZoneAndHeightGap( pChar, pStage );

				if( m_CurCircleEffect != m_OffCircle && m_CircleState != FIRST_CIRCLE )
				{
					if( m_CircleState != CIRCLE_COMPLETE )
						SetCircleEffect( pChar, pStage, vPos );

					SetActionState( pChar, pStage, CIRCLE_COMPLETE );
				}
				else
				{
					SetEndState(pChar, pStage);
				}
			}
				
		}
		break;
	case SS_ACTION:
		{
			if( 0 < m_SkillMotionEndTime && m_SkillMotionEndTime < dwCurTime )
			{
				if( m_CircleState == SECOND_CIRCLE )
				{
					SetLoopState( pChar );
					m_SkillMotionEndTime = 0;					
				}
				else if( m_CircleState == CIRCLE_COMPLETE )
				{
					SetEndState( pChar, pStage );
				}
			}
		}
		break;
	case SS_END:
		{
			pChar->DestroyMagicCircle();

			if( m_dwFirstStampCircleEffect != -1 )
			{
				pChar->EndMapEffect( m_dwFirstStampCircleEffect, false );
				m_dwFirstStampCircleEffect = -1;
			}

			if( m_dwSecondStampCircleEffect != -1 )
			{
				pChar->EndMapEffect( m_dwSecondStampCircleEffect, false );
				m_dwSecondStampCircleEffect = -1;
			}
		}
	}
}

void ioTwoMagicCircleSkill::CheckCircle( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	switch( m_CircleState )
	{
	case FIRST_CIRCLE:
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll(	m_fFirstCircleMoveSpeed,
				m_fFirstCircleRange, 
				m_fFirstCircleHeightGap,
				false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );			
			ChangeCircleDropZoneAndHeightGap( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				//키입력 여부 검사
				bool bReserveSkill = false;
				int iSkillNum = pChar->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					bReserveSkill = true;
				}			

				if( pChar->IsAttackKey() && !bReserveSkill && !m_bDropzone )
				{						
					D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();

					SetCircleEffect( pChar, pStage, vPos );				
					m_fFirstMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
					m_vCirclePos = vPos;
					m_vFirstCirclePos = vPos;
					
					D3DXVECTOR3 vLength = m_vFirstCirclePos - pChar->GetWorldPosition();
					float fOffset = fabs(D3DXVec3Length( &vLength ));
					
					SetActionState( pChar, pStage, SECOND_CIRCLE );
					if( !m_bReduceGauge )
					{
						pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );						
					}

					//마법진 생성
					ioHashString szDummyCircle;
					pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, m_fSecondCircleOffSet+fOffset );
					
					if( pChar->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << NSS_CIRCLE_CHANGE;
						kPacket << m_vFirstCirclePos;
						kPacket << m_fFirstMapHeight;
						kPacket << fOffset;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
		break;
	case SECOND_CIRCLE:
		{	
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();
			
			// 12.6.11 요청 사항에 의해서 매직 서클간의 제한 기능 변경
			// 해당 위치 이동불가에서 소환불가 형태로 변경
			// MagicCircle 위치 갱신(현재 이동가능 높이제한 false)
			//pChar->UpdateMagicCircleToCirclePosAll(	m_fSecondCircleMoveSpeed,
			//	m_fSecondCircleMaxRange,
			//	m_fSecondCircleMinRange,
			//	m_fSecondCircleHeightGap,
			//	m_fCircleKeyOnHeightMin,
			//	m_fCircleKeyOnHeightMax,
			//	m_vCirclePos,
			//	false,
			//	false );

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll(	m_fSecondCircleMoveSpeed,
				m_fSecondCircleMaxRange, 
				m_fSecondCircleHeightGap,
			false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
			
			ChangeCircleDropZoneAndHeightGap( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				//키입력 여부 검사
				bool bReserveSkill = false;
				int iSkillNum = pChar->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					bReserveSkill = true;
				}
				
				if( pChar->IsAttackKey() && !bReserveSkill && !m_bDropzone && m_CurCircleEffect != m_OffCircle )
				{
					D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();
					float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
					float fMin = m_vFirstCirclePos.y + m_fCircleKeyOnHeightMin;
					float fMax = m_vFirstCirclePos.y + m_fCircleKeyOnHeightMax;

					if( COMPARE( fMapHeight, fMin, fMax ) )
					{
						D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();
						m_vSecondCirclePos =  vPos;
						SetCircleEffect( pChar, pStage, vPos );
						SetActionState( pChar, pStage, CIRCLE_COMPLETE );

						if( !m_bReduceGauge )
						{
							ioItem* pItem =  GetOwnerItem();
							if( pItem )
							{
								pChar->ReduceNeedGaugeBySkill( this, pItem->GetMaxSkillGauge( pChar->GetCharName() ) );
							}
							m_bReduceGauge = true;						
						}



						if( pChar->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
							kPacket << pChar->GetCharName();
							kPacket << GetName();
							kPacket << NSS_CREATE;
							kPacket << m_vSecondCirclePos;
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
					}					
				}		
			}
		}
		break;
	}
}

void ioTwoMagicCircleSkill::ChangeCircleDropZoneAndHeightGap( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( !pChar ) return;

	D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();

	if( m_CircleState == FIRST_CIRCLE )
	{
		if( pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) > 0.0f )
		{	
			if( m_bFirstOffCircle && m_CurCircleEffect == m_OffCircle )
			{
				ioHashString szDummyCircle;
				pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, 0 );
				pChar->SetMagicCirclePos( false, vPos );
				m_bFirstOffCircle = false;
				m_CurCircleEffect = m_OnCircle;
			}

			m_bDropzone = false;

		}
		else
		{
			if( !m_bFirstOffCircle && m_CurCircleEffect == m_OnCircle )
			{
				ioHashString szDummyCircle;
				pChar->CreateMagicCircle( m_OffCircle, m_OffCircle, szDummyCircle, 0 );
				pChar->SetMagicCirclePos( false, vPos );
				m_bFirstOffCircle = true;
				m_CurCircleEffect = m_OffCircle;
			}

			m_bDropzone = true;
		}
	}
	else if( m_CircleState == SECOND_CIRCLE )
	{
		if( !m_bDropzone )
		{
			float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
			float fMin = m_vFirstCirclePos.y + m_fCircleKeyOnHeightMin;
			float fMax = m_vFirstCirclePos.y + m_fCircleKeyOnHeightMax;
			
			D3DXVECTOR3 vDistace = m_vFirstCirclePos - pChar->GetMagicCirclePos();
			float fDistace = fabs( D3DXVec3Length( &vDistace ) );

			if( COMPARE( fMapHeight, fMin, fMax ) )
			{	
				if( m_fCircleToCircleGap < fDistace )
				{
					if( m_bSecondOffCircle && m_CurCircleEffect == m_OffCircle )
					{			
						ioHashString szDummyCircle;
						pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, 0 );
						pChar->SetMagicCirclePos( false, vPos );
						m_bSecondOffCircle = false;
						m_CurCircleEffect = m_OnCircle;
					}				
				}
				else
				{
					if( !m_bSecondOffCircle && m_CurCircleEffect == m_OnCircle )
					{
						ioHashString szDummyCircle;
						pChar->CreateMagicCircle( m_OffCircle, m_OffCircle, szDummyCircle, 0 );
						pChar->SetMagicCirclePos( false, vPos );
						m_bSecondOffCircle = true;
						m_CurCircleEffect = m_OffCircle;
					}
				}
			}
			else
			{
				if( !m_bSecondOffCircle && m_CurCircleEffect == m_OnCircle )
				{
					ioHashString szDummyCircle;
					pChar->CreateMagicCircle( m_OffCircle, m_OffCircle, szDummyCircle, 0 );
					pChar->SetMagicCirclePos( false, vPos );
					m_bSecondOffCircle = true;
					m_CurCircleEffect = m_OffCircle;
				}
			}
		}
		else
		{
			if( !m_bSecondOffCircle && m_CurCircleEffect == m_OnCircle )
			{
				ioHashString szDummyCircle;
				pChar->CreateMagicCircle( m_OffCircle, m_OffCircle, szDummyCircle, 0 );
				pChar->SetMagicCirclePos( false, vPos );
				m_bSecondOffCircle = true;
				m_CurCircleEffect = m_OffCircle;
			}
		}
	}
}

void ioTwoMagicCircleSkill::SetCircleEffect( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vPos )
{
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );	
	
	switch( m_CircleState )
	{
	case FIRST_CIRCLE:
		{
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szStampCircleEffect, vPos, vScale );

			if( pMapEffect )
				m_dwFirstStampCircleEffect = pMapEffect->GetUniqueID();				
		}
		break;
	case SECOND_CIRCLE:
		{
			ioBaseChar* pTarget = pStage->GetCameraTargetChar();
			if( pChar && pTarget  )
			{
				if( pChar->GetTeam() == pTarget->GetTeam() )
				{
					ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szStampCircleEffect, vPos, vScale );

					if( pMapEffect )
						m_dwSecondStampCircleEffect = pMapEffect->GetUniqueID();							
				}
			}
		}

		break;
	}
	
}

void ioTwoMagicCircleSkill::CreateAreaWeapon( ioBaseChar *pChar, ioPlayStage* pStage )
{
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
															"",
															GetName(),
															m_AreaWeaponName,
															m_vFirstCirclePos,
															qtRot,
															ioAreaWeapon::CT_NORMAL );

	if( pArea )
	{
		pArea->SetPointToPointInfo( pStage,
			m_vFirstCirclePos,
			m_vSecondCirclePos,
			m_szStampCircleEffect,
			m_szStampCircleEffect );
	}
}

void ioTwoMagicCircleSkill::SetActionState( ioBaseChar *pChar, ioPlayStage* pStage, CircleState cState )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();

	m_CircleState = cState;
	m_State = SS_ACTION;

	int iAniID = pGrp->GetAnimationIdx( m_SkillMotion );
	if( iAniID == -1 )	return;
	float fTimeRate = m_fSkillMototionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_SkillMotionEndTime = dwCurTime;
	m_SkillMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
}

void ioTwoMagicCircleSkill::SetEndState( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iAniID == -1 )	return;
	float fTimeRate = m_fSkillEndMototionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	if( m_dwEndMotionEndTime == 0 )	
	{
		m_dwEndMotionEndTime = dwCurTime;
		m_dwEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	}

	m_State = SS_END;

	if( m_CurCircleEffect != m_OffCircle && m_CircleState == CIRCLE_COMPLETE )
	{
		CreateAreaWeapon( pChar, pStage );
	}
}


void ioTwoMagicCircleSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
 	int iSyncType;		
 	rkPacket >> iSyncType;
 
 	switch( iSyncType )
 	{
	case NSS_CIRCLE_CHANGE:
		{			
			rkPacket >> m_vFirstCirclePos;
			rkPacket >> m_fFirstMapHeight;
			
			float fOffset;
			rkPacket >> fOffset;
			
			SetCircleEffect(pChar, pStage, m_vFirstCirclePos);
			SetActionState( pChar, pStage, SECOND_CIRCLE );

			//마법진 생성
			ioHashString szDummyCircle;
			pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, m_fSecondCircleOffSet+fOffset );
		}
		break;
 	case NSS_CREATE:
		{			
			rkPacket >> m_vSecondCirclePos;
			
			SetCircleEffect(pChar, pStage, m_vSecondCirclePos);
			SetActionState(pChar, pStage, CIRCLE_COMPLETE);
		}
		break;
 	}
}


bool ioTwoMagicCircleSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioTwoMagicCircleSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_dwLoopDurationStart == 0 )	
		m_dwLoopDurationStart = dwCurTime;
	
	//마법진 생성
	if( !pChar->CheckMagicCircle() )
	{		
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, m_fFirstCircleOffSet );
	}
}



bool ioTwoMagicCircleSkill::IsProtected( int iDefenseBreakType ) const
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

	DWORD dwTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_LOOP:
		if( m_dwLoopDurationStart + m_dwProtectTime < dwTime )
			return false;
		else
			return true;
		break;
	case SS_ACTION:
		return true;
	}

	return false;
}

bool ioTwoMagicCircleSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:		
	case SS_LOOP:
	case SS_ACTION:
		return false;

	case SS_END:
		if( 0 < m_dwEndMotionEndTime && m_dwEndMotionEndTime < dwCurTime )
			return true;
	}

	return false;
}

bool ioTwoMagicCircleSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTwoMagicCircleSkill::CheckCharMoveDir( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pChar->CheckCurDirKey();

		float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pChar->SetTargetRotToRotate( qtRotate, true, false );
	}
}
