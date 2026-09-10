#include "stdafx.h"

#include "ioRangePositionInverseSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

ioRangePositionInverseSkill::ioRangePositionInverseSkill()
{
	ClearData();
}

ioRangePositionInverseSkill::ioRangePositionInverseSkill( const ioRangePositionInverseSkill &rhs )
: ioRangeSkill( rhs ),
m_fTeleportAxisYOffset( rhs.m_fTeleportAxisYOffset ),
m_fTeleportRange( rhs.m_fTeleportRange ),
m_fRangeSpeed( rhs.m_fRangeSpeed ),
m_fTeleprotTime( rhs.m_fTeleprotTime ),
m_pEnemyBuff( rhs.m_pEnemyBuff ),
m_pEnemyAirBuff( rhs.m_pEnemyAirBuff ),
m_pEnemyDownBuff( rhs.m_pEnemyDownBuff ),
m_pChangeEnemyBuff( rhs.m_pChangeEnemyBuff ),
m_pChangeEnemyAirBuff( rhs.m_pChangeEnemyAirBuff ),
m_pChangeEnemyDownBuff( rhs.m_pChangeEnemyDownBuff ),
m_szChangePositionEffect( rhs.m_szChangePositionEffect ),
m_szChangePositionDownEffect( rhs.m_szChangePositionDownEffect ),
m_szChangePositionAirEffect( rhs.m_szChangePositionAirEffect ),
m_fRangeMinRadius( rhs.m_fRangeMinRadius ),
m_fRangeMaxRadius( rhs.m_fRangeMaxRadius )
{
	ClearData();
}

ioRangePositionInverseSkill::~ioRangePositionInverseSkill()
{
}

void ioRangePositionInverseSkill::ClearData()
{
	m_vCharList.clear();
	m_dwTeleportStartTime = 0;
	m_State = SS_LOOP;
		 
	m_fEffectScaleRate = FLOAT1;
	m_dwRangeEffectID = -1;

	m_bReduceGauge = false;
}

void ioRangePositionInverseSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioRangeSkill::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];

	m_fTeleportAxisYOffset = rkLoader.LoadFloat_e( "teleport_axis_y_offset", 200.0f );
	m_fTeleportRange = rkLoader.LoadFloat_e( "teleprot_ragne", 200.0f );
	m_fRangeSpeed = rkLoader.LoadFloat_e( "range_speed", 0.0f );
	
	m_fRangeMaxRadius = rkLoader.LoadFloat_e( "range_max_radius", 0.0f );
	m_fRangeMinRadius = rkLoader.LoadFloat_e( "range_min_radius", 0.0f );

	m_fTeleprotTime = rkLoader.LoadInt_e( "teleport_time", 100 );

	LoadWoundedTypeBuffList_e( rkLoader, "teleport_enemy_buff", m_pEnemyBuff );
	LoadWoundedTypeBuffList_e( rkLoader, "teleport_enemy_air_buff", m_pEnemyAirBuff );
	LoadWoundedTypeBuffList_e( rkLoader, "teleport_enemy_down_buff", m_pEnemyDownBuff );

	LoadWoundedTypeBuffList_e( rkLoader, "change_enemy_buff", m_pChangeEnemyBuff );
	LoadWoundedTypeBuffList_e( rkLoader, "change_enemy_air_buff", m_pChangeEnemyAirBuff );
	LoadWoundedTypeBuffList_e( rkLoader, "change_enemy_down_buff", m_pChangeEnemyDownBuff );

	rkLoader.LoadString_e( "change_position_effect", "", szBuf, MAX_PATH );
	m_szChangePositionEffect = szBuf;

	rkLoader.LoadString_e( "change_position_down_effect", "", szBuf, MAX_PATH );
	m_szChangePositionDownEffect = szBuf;

	rkLoader.LoadString_e( "change_position_Air_effect", "", szBuf, MAX_PATH );
	m_szChangePositionAirEffect = szBuf;
}

void ioRangePositionInverseSkill::LoadWoundedTypeBuffList( ioINILoader &rkLoader, const char *szBuffType, BuffInfoList& pList )
{
	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	rkLoader.LoadString( szBuffType, "", szBuf, MAX_PATH );
	
	wsprintf_e( szTitle, "%s_cnt", szBuffType );
	
	int iCnt = rkLoader.LoadInt( szTitle , 0 );
	pList.clear();

	if( iCnt <= 0 ) return;

	pList.reserve( iCnt );
	
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "%s%d", szBuffType, i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				pList.push_back( tBuffInfo );
			}
		}
	}
}

ioSkill* ioRangePositionInverseSkill::Clone()
{
	return new ioRangePositionInverseSkill( *this );
}


bool ioRangePositionInverseSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioRangeSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();
	m_vEffectDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_fTeleportCurRange = m_fRangeMinRadius;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );						
	}

	return true;
}

void ioRangePositionInverseSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioRangeSkill::OnSkillEnd( pChar );

	ClearData();

	ioPlayStage* pStage;
	if( !pChar )
		pStage = pChar->GetCreator();

	if( m_dwRangeEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_dwRangeEffectID );
	}
	
}

void ioRangePositionInverseSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_RangeState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetLoopState( pChar, pStage );
		}
		break;
	case RS_LOOP:
		{		
			switch( m_State )
			{
			case SS_LOOP:
				{
					if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
					{
						m_dwLoopStartTime = 0;
						ioRangeSkill::SetEndState( pChar );
						pStage->DestroyMapEffect( m_dwRangeEffectID );
					}
					else if( m_dwLoopStartTime > 0 && ( m_dwLoopStartTime+m_dwDuration >= dwCurTime ) )
					{
						if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
						{
							m_vOwnerSkillUsePos = pChar->GetMidPositionByRate();	

							FindTarget( pChar, pStage );
							SetTargetBuff( pChar, pStage );
							
							m_dwTeleportStartTime = dwCurTime;
							SetChangeActionState( pChar );

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
								kPacket << SSC_CHANGE_ACTION;
								kPacket << pChar->GetMidPositionByRate();
								kPacket << (int)m_vCharList.size();
								
								BaseCharWoundedList::iterator iter;
								for( iter = m_vCharList.begin(); iter != m_vCharList.end(); ++iter )
								{
									kPacket << (*iter).m_szCharName;
									kPacket << (int)(*iter).m_WoundedType;
								}

								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
							return;
						}

						float fTimePerSec = g_FrameTimer.GetSecPerFrame();
						float fRate = (fTimePerSec*1000) / m_fRangeSpeed;
						float fAddRange = m_fRangeMaxRadius * fRate;

						if( COMPARE( m_fTeleportCurRange, m_fRangeMinRadius, m_fRangeMaxRadius ) )
						{
							m_fTeleportCurRange += fAddRange;
						}

						UddateRangeEffect( pStage, fTimePerSec );
					}
				}
				break;
			case SS_CHANGE:
				{			
					if( 0 < m_dwTeleportStartTime && m_dwTeleportStartTime + m_fTeleprotTime < dwCurTime )
					{	
						if( pChar->IsNeedProcess() )
						{
							SetTargetPositionChange( pChar, pStage );
							SetTargetEndBuff( pChar, pStage );
							m_State = SS_END;

							if( pChar->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
								kPacket << pChar->GetCharName();
								kPacket << GetName();
								kPacket << SSC_CHANGE_END;
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
						}
					}
					
					UpdateChangeAllEffect( pStage );
				}
				break;
			case SS_END:
				{
					ioRangeSkill::SetEndState( pChar );						
					pStage->DestroyMapEffect( m_dwRangeEffectID );					
				}
				break;
			}
		}
	}
}

void ioRangePositionInverseSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iAniID == -1 )
	{
		m_dwEnableReserveTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRangeBuffSkill::SetLoopState() - %s, Not Exist LoopAnimation",
			GetName().c_str() );
	}
	else
	{
		pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );
		m_dwEnableReserveTime = FRAMEGETTIME();
		m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + m_vEffectDir * m_fOffSet;
	ioMapEffect* pMapEffect = pChar->CreateMapEffectBySkill( m_RangeEffect, vPos );
	if(  pMapEffect )
	{
		m_dwRangeEffectID = pMapEffect->GetUniqueID();
	}


	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_RangeState = RS_LOOP;

	SetOwnerBuffList( pChar );

	m_fTeleportCurRange = m_fSkillRange;
}

void ioRangePositionInverseSkill::SetChangeActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID == -1 )
	{
		m_dwMotionStartTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRangePositionInverseSkill::SetActionState() - %s, Not Exist ActionAnimation", GetName().c_str() );
	}
	else
	{
		float fAniRate = GetSkillMotionRate();

		float fTimeRate = fAniRate * m_fExtraAniRate;

		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		m_dwMotionStartTime = FRAMEGETTIME();
		m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	}

	m_State = SS_CHANGE;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioRangePositionInverseSkill::FindTarget( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner ) return;
	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT08 );
	vPos += m_vEffectDir * m_fOffSet;

	pOwner->GetTargetCharListByCylinder( vPos, m_fTeleportCurRange, m_fSkillAngle, m_fUpHeight, m_fUnderHeight, vTargetList );
	
	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( !pTarget || IsCollisioned( pTarget ) )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;
				
		if( pTarget->GetExperienceMode() == EMS_EXPERIENCE )
			continue;
		
		D3DXVECTOR3 vOwnerPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		if( m_bCheckCollisionLine && !pStage->CheckCollisionLine( vOwnerPos, vTargetPos ) )
			continue;
		
		BaseCharWoundedInfo pWoundedInfo;
		pWoundedInfo.m_szCharName = pTarget->GetCharName();
		if( pTarget->IsApplyDownState(false) )
		{
			pWoundedInfo.m_WoundedType = CWT_DOWN;
		}
		else if( pTarget->IsFloating() )		
		{
			pWoundedInfo.m_WoundedType = CWT_AIR;
		}
		else
		{
			pWoundedInfo.m_WoundedType = CWT_NORMAL;
		}

		m_vCharList.push_back( pWoundedInfo );
	}
}

void ioRangePositionInverseSkill::SetTargetBuff( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	BaseCharWoundedList::iterator iter;
	for( iter = m_vCharList.begin(); iter != m_vCharList.end(); )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( (*iter).m_szCharName );
		CharWoundedType WoundedType = (*iter).m_WoundedType;

		if( !pTarget )
			continue;

		bool bEnableTeamAttack = pTarget->IsEnableTeamAttack();
		if( pOwner->GetTeam() == pTarget->GetTeam() && SetTeamBuffList( pOwner, pTarget ) )
		{
			if( m_bEnableAttackCnt )
				pOwner->CheckTeamBuffAttackCnt( pTarget );
		}

		if( ( bEnableTeamAttack || pOwner->GetTeam() != pTarget->GetTeam() ) )
		{
			if( SetTargetWoundedTypeParseBuffList( pOwner, pTarget, BTS_TELEPORT, WoundedType ) )
			{
				if( m_bEnableAttackCnt )
				{				
					pOwner->CheckComboSkillHit();
					pOwner->CheckAttackCnt( pTarget );
				}
				
				D3DXVECTOR3 vDir;

				if( WoundedType == CWT_DOWN )
				{
					D3DXVECTOR3 vDir = CaculrateDirection( pOwner->GetMidPositionByRate(), 
						pTarget->GetMidPositionByRate(),
						m_fTeleportAxisYOffset );

					SetChangeEffect( pTarget, m_szChangePositionEffect, m_dwChangePositionDownEffect, vDir, pStage );
				}
				else if( WoundedType == CWT_AIR )
				{
					D3DXVECTOR3 vDir = CaculrateDirection( pOwner->GetMidPositionByRate(), 
														   pTarget->GetMidPositionByRate(),
														   m_fTeleportAxisYOffset );

					SetChangeEffect( pTarget, m_szChangePositionEffect, m_dwChangePositionAirEffect, vDir, pStage );
				}
				else if( WoundedType == CWT_NORMAL )
				{
					D3DXVECTOR3 vDir = CaculrateDirection( pOwner->GetMidPositionByRate(), 
														   pTarget->GetMidPositionByRate(),
														   0.0f );

					SetChangeEffect( pTarget, m_szChangePositionEffect, m_dwChangePositionAirEffect, vDir, pStage );
				}

				pTarget->SetTargetRotToTargetPos( m_vOwnerSkillUsePos, true, true );								
				iter++;
			}
			else
			{
				iter = m_vCharList.erase( iter );				
			}			
		}
	}
}

D3DXVECTOR3 ioRangePositionInverseSkill::CaculrateDirection( const D3DXVECTOR3 vView, 
															 const D3DXVECTOR3 vTarget,
															 float fOffset )
{
	D3DXVECTOR3 vAxisYPos = vView;
	vAxisYPos.y += fOffset;

	D3DXVECTOR3 vDir = vAxisYPos - vTarget;
	D3DXVec3Normalize( &vDir, &vDir );
	
	return vDir;
}


void ioRangePositionInverseSkill::SetTargetPositionChange( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	BaseCharWoundedList::iterator iter;
	for( iter = m_vCharList.begin(); iter != m_vCharList.end(); ++iter )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( (*iter).m_szCharName );
		CharWoundedType WoundedType = (*iter).m_WoundedType;

		if( !pTarget )
			continue;

		if( !pTarget->HasBuff( BT_FLOAT_BUFF ) )
			continue;

		if( WoundedType == CWT_DOWN )
		{
			D3DXVECTOR3 vAxisYPos = m_vOwnerSkillUsePos;
			vAxisYPos.y += m_fTeleportAxisYOffset;

			D3DXVECTOR3 vDir = vAxisYPos - pTarget->GetMidPositionByRate();
			D3DXVec3Normalize( &vDir, &vDir );
			vDir = vDir * m_fTeleportRange;

			D3DXVECTOR3 vTeleportPos = pTarget->GetMidPositionByRate() + vDir;

			D3DXVECTOR3 vColPos;
			if( pStage->CheckCollisionLine( pTarget->GetMidPositionByRate(), vTeleportPos, vColPos ) )
				pTarget->SetWorldPosition( vTeleportPos );
			else
				pTarget->SetWorldPosition( vColPos );				

		}
		else if( WoundedType == CWT_AIR )
		{
			D3DXVECTOR3 vAxisYPos = m_vOwnerSkillUsePos;
			vAxisYPos.y += m_fTeleportAxisYOffset;

			D3DXVECTOR3 vDir = vAxisYPos - pTarget->GetMidPositionByRate();
			D3DXVec3Normalize( &vDir, &vDir );
			vDir = vDir * m_fTeleportRange;

			D3DXVECTOR3 vTeleportPos = pTarget->GetMidPositionByRate() + vDir;

			D3DXVECTOR3 vColPos;
			if( pStage->CheckCollisionLine( pTarget->GetMidPositionByRate(), vTeleportPos, vColPos ) )			
				pTarget->SetWorldPosition( vTeleportPos );			
			else			
				pTarget->SetWorldPosition( vColPos );				
			
		}
		else if( WoundedType == CWT_NORMAL )
		{
			D3DXVECTOR3 vAxisYPos = m_vOwnerSkillUsePos;

			D3DXVECTOR3 vDir = vAxisYPos - pTarget->GetMidPositionByRate();
			D3DXVec3Normalize( &vDir, &vDir );
			vDir = vDir * m_fTeleportRange;

			D3DXVECTOR3 vTeleportPos = pTarget->GetMidPositionByRate() + vDir;

			D3DXVECTOR3 vColPos;
			if( pStage->CheckCollisionLine( pTarget->GetMidPositionByRate(), vTeleportPos, vColPos ) )
				pTarget->SetWorldPosition( vTeleportPos );
			else
				pTarget->SetWorldPosition( vColPos );
		}
	}
}

void ioRangePositionInverseSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState; 
	switch( iState )
	{
	case SSC_CHANGE_ACTION:
		{		
			m_vCharList.clear();

			rkPacket >> m_vOwnerSkillUsePos;

			int iCnt = 0;
			rkPacket >> iCnt;
	
			for( int i = 0; i < iCnt; ++i )
			{
				BaseCharWoundedInfo pInfo;		
				rkPacket >> pInfo.m_szCharName;

				int iWoundedType;
				rkPacket >> iWoundedType;
				pInfo.m_WoundedType = (CharWoundedType)iWoundedType;
				m_vCharList.push_back( pInfo );
			}

			SetTargetBuff( pOwner, pStage );			
			SetChangeActionState( pOwner );
		}
		break;
	case SSC_CHANGE_END:
		{
			SetTargetPositionChange( pOwner, pStage );
			SetTargetEndBuff( pOwner, pStage );
			m_State = SS_END;
		}
		break;
	}
}

bool ioRangePositionInverseSkill::SetTargetWoundedTypeParseBuffList( ioBaseChar *pOwner,
																	 ioBaseChar *pChar,
																	 BuffState State,
																	 CharWoundedType WoundedType )
{
	if( pChar->IsProtectState() )
	{
		bool bBreakProtcet = false;
		if( m_bIgnoreProtect )
		{
			if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
			{
				bBreakProtcet = true;
			}
		}
		if( !bBreakProtcet )
		{
			int iType = pChar->CheckBlockEmoticonType();
			if( iType >= 0 && !IsBlocked( pChar ) )
			{
				m_vBlockedList.push_back( pChar->GetCharName() );
				pChar->SetProtectWound( iType );
			}

			return false;
		}
	}
	
	BuffInfoList BuffList;
	switch( State )
	{
	case BTS_TELEPORT:
		if( WoundedType == CWT_DOWN )
		{
			BuffList = m_pEnemyDownBuff;
		}
		else if( WoundedType == CWT_AIR )
		{
			BuffList = m_pEnemyAirBuff;
		}
		else if( WoundedType == CWT_NORMAL )
		{
			BuffList = m_pEnemyBuff;
		}
		else
		{
			return false;
		}
		break;
	case BTS_STATE_CHANGE:
		if( WoundedType == CWT_DOWN )
		{
			BuffList = m_pChangeEnemyDownBuff;
		}
		else if( WoundedType == CWT_AIR )
		{
			BuffList = m_pChangeEnemyAirBuff;
		}
		else if( WoundedType == CWT_NORMAL )
		{
			BuffList = m_pChangeEnemyBuff;
		}
		else
		{
			return false;
		}
	}

	if( BuffList.empty() )
		return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpinfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpinfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpinfo->GetValue(pStage);
	}

	int iCnt = BuffList.size();

	for( int i=0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = BuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
		{
			if( !m_bIgnoreProtect )
			{
				continue;
			}
		}
		else if( pChar->IsCanNoWound() )
			continue;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff;
			if( State == BTS_TELEPORT )						
				pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pOwner->GetCharName(), szItemName, this, GetSkillMotionTime() );		
			else			
				pBuff = pChar->AddNewBuff( szBuffName, pOwner->GetCharName(), szItemName, NULL );				
			
			if( pBuff )
			{
				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( pOwner->GetCharName(), GetName() );
				}

				vBuffList.push_back( szBuffName );

				pBuff->SetExtraDurationByGrowth( fValue );				
				bResult = true;
			}
		}
	}

	return bResult;
}

void ioRangePositionInverseSkill::UddateRangeEffect( ioPlayStage* pStage, float fTimePerSec )
{	
	if( !pStage ) return;
	
	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwRangeEffectID );
	if( pMapEffect )
	{
		float fCurScaleRate = FLOAT1;

		float fCurScale = m_fTeleportCurRange / FLOAT100;
		fCurScale += FLOAT1;	

		D3DXVECTOR3 vScale( fCurScale, fCurScale, fCurScale );		
		ioEffect* pEffect = pMapEffect->GetEffect();
		if( pEffect )
		{
			pEffect->GetParentNode()->SetScale( vScale );
		}		
	}
}

void ioRangePositionInverseSkill::SetChangeEffect( ioBaseChar* pChar, ioHashString szEffect, UniqueObjID& effectID, D3DXVECTOR3 vDirEffect, ioPlayStage* pStage )
{
	if( pStage )
	{
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( szEffect, pChar->GetWorldPosition(), pChar->GetWorldScale() );
		if( pMapEffect )
		{	
			effectID = pMapEffect->GetUniqueID();	

			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vDirEffect );
			pMapEffect->SetWorldOrientation( qtRot );
		}
	}
}

void ioRangePositionInverseSkill::UpdateChangeEffect( ioBaseChar* pChar,  UniqueObjID& effectID, ioPlayStage* pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	
	ioMapEffect *pMapEffect = pStage->FindMapEffect( effectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( pChar->GetWorldPosition() );
	}
}

void ioRangePositionInverseSkill::UpdateChangeAllEffect( ioPlayStage* pStage )
{
	BaseCharWoundedList::iterator iter;
	for( iter = m_vCharList.begin(); iter != m_vCharList.end(); ++iter )
	{						
		ioBaseChar *pTarget = pStage->GetBaseChar( (*iter).m_szCharName );
		CharWoundedType WoundedType = (*iter).m_WoundedType;

		if( !pTarget )
			continue;

		if( WoundedType == CWT_DOWN )
			UpdateChangeEffect( pTarget, m_dwChangePositionDownEffect, pStage );

		else if( WoundedType == CWT_AIR )

			UpdateChangeEffect( pTarget, m_dwChangePositionAirEffect, pStage );

		else if( WoundedType == CWT_NORMAL )
			UpdateChangeEffect( pTarget, m_dwChangePositionEffect, pStage );

	}
}

void ioRangePositionInverseSkill::SetTargetEndBuff( ioBaseChar* pChar, ioPlayStage* pStage )
{
	BaseCharWoundedList::iterator iter;
	for( iter = m_vCharList.begin(); iter != m_vCharList.end(); ++iter )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( (*iter).m_szCharName );
		CharWoundedType WoundedType = (*iter).m_WoundedType;

		if( !pTarget )
			continue;

		if( !pTarget->HasBuff( BT_FLOAT_BUFF ) )
			continue;

		if( WoundedType == CWT_DOWN )
			SetTargetWoundedTypeParseBuffList( pChar, pTarget, BTS_STATE_CHANGE, CWT_DOWN );

		else if( WoundedType == CWT_AIR )
			SetTargetWoundedTypeParseBuffList( pChar, pTarget, BTS_STATE_CHANGE, CWT_AIR );

		else if( WoundedType == CWT_NORMAL )							
			SetTargetWoundedTypeParseBuffList( pChar, pTarget, BTS_STATE_CHANGE, CWT_NORMAL );
	}
}