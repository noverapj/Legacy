#include "StdAfx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"



#include "ioRandomBuffSkill.h"


ioRandomBuffSkill::ioRandomBuffSkill(void)
{
	ClearData();
}

ioRandomBuffSkill::ioRandomBuffSkill( const ioRandomBuffSkill &rhs )
	:ioNormalSkill( rhs ),
m_iSectionCnt( rhs.m_iSectionCnt ),
m_iRandomLimitCnt( rhs.m_iRandomLimitCnt ),
m_SectionInfoList( rhs.m_SectionInfoList ),
m_MapEffectList( rhs.m_MapEffectList),
m_fEffectYGap( rhs.m_fEffectYGap ),
m_fEffectXGap( rhs.m_fEffectXGap ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_EndAnimation( rhs.m_EndAnimation ),
m_fEndAniRate( rhs.m_fEndAniRate )
{
	ClearData();
}

ioRandomBuffSkill::~ioRandomBuffSkill(void)
{
	ClearData();
}

void ioRandomBuffSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_iValue = 0;
	m_dwEndTime = 0;
	memset( m_MapEffectID, 0, sizeof( m_MapEffectID ) );
}

void ioRandomBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );
	
	m_iSectionCnt = rkLoader.LoadInt_e( "section_cnt", 0 );
	m_iRandomLimitCnt = rkLoader.LoadInt_e( "random_limit_cnt", 0 );

	m_SectionInfoList.clear();
	if ( m_iSectionCnt <= 0) return;

	m_SectionInfoList.reserve( m_iSectionCnt );
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	for( int i = 0; i < m_iSectionCnt; ++i )
	{
		SectionInfo kSectionInfo;
		wsprintf_e( szKey, "min%d", i+1 );
		kSectionInfo.m_iMin =  rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "max%d", i+1 );
		kSectionInfo.m_iMax =  rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "buff_target_type%d", i+1 );
		kSectionInfo.m_iBuffTargetType = (BuffTargetType)rkLoader.LoadInt( szKey, BUFFTT_NONE );
		wsprintf_e( szKey, "apply_range%d", i+1 );
		kSectionInfo.m_fSkillRange =  rkLoader.LoadFloat( szKey, 0.0f );

		BuffInfoList kList;
		wsprintf_e( szKey, "random%d_buff_cnt", i+1 );
		int iCnt = rkLoader.LoadInt( szKey, 0 );
		kList.reserve( iCnt );

		for( int j=0; j<iCnt; ++j )
		{
			wsprintf_e( szKey, "random%d_buff%d", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			ioBuffInfo tBuffInfo;
			tBuffInfo.m_BuffName = szBuf;

			if( !tBuffInfo.m_BuffName.IsEmpty() )
			{
				tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
				if( tBuffInfo.m_iBuffType != BT_NONE )
				{
					kList.push_back( tBuffInfo );
				}
			}
		}
		kSectionInfo.m_BuffList = kList;
		m_SectionInfoList.push_back( kSectionInfo );
	}

	int iCnt = rkLoader.LoadInt_e( "map_effect_cnt", 0 );
	m_MapEffectList.reserve( iCnt );
	for ( int i = 0; i < iCnt; ++i )
	{
		ioHashString szSkillMapEffect;
		wsprintf_e( szKey, "skill_map_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szSkillMapEffect = szBuf;

		m_MapEffectList.push_back( szSkillMapEffect );
	}

	m_fEffectYGap = rkLoader.LoadFloat_e( "number_effect_y_gap", 0.0f );
	m_fEffectXGap = rkLoader.LoadFloat_e( "number_effect_x_gap", 0.1f );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

}

ioSkill* ioRandomBuffSkill::Clone()
{
	return new ioRandomBuffSkill( *this );
}

bool ioRandomBuffSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioNormalSkill::OnSkillStart(pChar) )
		return false;

	ClearData();

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

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetActionState( pChar );
		if( pChar->IsNeedProcess() )
			SetRandomBuff( pChar, pStage, true );
	}	

	return true;
}

void ioRandomBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
			if( pChar->IsNeedProcess() )
				SetRandomBuff( pChar, pStage, true );
		}
		break;
	case SS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			if( m_dwAddBuffTime > 0 && m_dwAddBuffTime < FRAMEGETTIME() )
			{
				CreateMapEffect( pChar, pStage );
				SetCharBuffList( pChar, pStage, true );
				SetEndState( pChar, pStage );
				m_dwMotionStartTime = 0;
			}			
		}
		break;
	}
}

void ioRandomBuffSkill::SetRandomBuff( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSend/*= false */ )
{
	if( !pOwner ) return;

	IORandom random;
	DWORD dwSeed = 0;
	dwSeed = pOwner->GetRandomSeed();
	random.SetRandomSeed( dwSeed );

	m_iValue = random.Random( m_iRandomLimitCnt );
	if ( m_iValue >= m_iRandomLimitCnt ) return;
	auto Iter = m_SectionInfoList.begin();
	for ( ; Iter != m_SectionInfoList.end(); ++Iter )
	{
		SectionInfo& Info = (*Iter);
		if ( !COMPARE( m_iValue, Info.m_iMin, Info.m_iMax ) )
			continue;

		m_SectionInfo = Info;
	}
	if ( bSend )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSS_CREATE_RANDOM;
			kPacket << pOwner->GetRandomSeed();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioRandomBuffSkill::SetCharBuffList( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSend/*= false */ )
{
	if( !pOwner ) return;
	//if( !pOwner->IsNeedProcess() )
	//	return;

	m_BuffCharList.clear();

	int iBuffTarget = m_SectionInfo.m_iBuffTargetType;
	float fPrevDistSq = m_SectionInfo.m_fSkillRange * m_SectionInfo.m_fSkillRange;

	float fLength = fPrevDistSq;
	ioBaseChar *pTarget = NULL;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;	
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = (*iter);

		if( pChar->IsSystemState() )
			continue;
		
		if( !pChar->IsEnableTarget() )
			continue;

		if( !pChar->CheckTargetWoundType( m_TargetWoundType ) )
			continue;
		
		switch ( iBuffTarget )
		{
		case BUFFTT_OWNER:
			if ( pOwner->GetCharName() == pChar->GetCharName() )
			{
				m_BuffCharList.push_back( pChar );
			}
			break;
		case BUFFTT_TEAM:
			if( pOwner->GetTeam() == pChar->GetTeam() && pOwner->GetCharName() != pChar->GetCharName() )	// Team
			{
				D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
				float fDistSq = D3DXVec3LengthSq( &vDiff );
				if ( fDistSq < fPrevDistSq )
				{
					m_BuffCharList.push_back( pChar );
				}
			}
			break;
		case BUFFTT_ENEMY_TEAM:
			if ( pOwner->GetTeam() != pChar->GetTeam() )
			{
				D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
				float fDistSq = D3DXVec3LengthSq( &vDiff );
				if ( fDistSq < fPrevDistSq )
				{
					m_BuffCharList.push_back( pChar );
				}
			}
			break;
		case BUFFTT_ENEMY_ONE:
			if ( pOwner->GetTeam() != pChar->GetTeam() )
			{
				D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
				float fDistSq = D3DXVec3LengthSq( &vDiff );
				if( fDistSq < fLength )
				{
					fLength = fDistSq;
					pTarget = pChar;
				}
			}
			break;
		}
	}

	if ( iBuffTarget == BUFFTT_ENEMY_ONE && pTarget )
	{
		m_BuffCharList.push_back( pTarget );
	}

	auto Chariter = m_BuffCharList.begin();
	for (; Chariter != m_BuffCharList.end(); ++Chariter )
	{
		AddTargetBuff( pOwner, (*Chariter) );
	}
}

void ioRandomBuffSkill::AddTargetBuff( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	if( !pChar ) return;
	
	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	auto Iter = m_SectionInfo.m_BuffList.begin();
	for ( ; Iter != m_SectionInfo.m_BuffList.end(); ++Iter )
	{
		ioHashString szBuffName = (*Iter).m_BuffName;
		if ( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuff( szBuffName, pOwner->GetCharName(), szItemName, this );
		}
	}

}
void ioRandomBuffSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	if( !m_bReduceGauge && pChar )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );	

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	DestroyMapEffect( pChar );
}

bool ioRandomBuffSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState == SS_END && FRAMEGETTIME() > m_dwEndAniEndTime )
	{
		return true;
	}
	return false;
}

void ioRandomBuffSkill::SetActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID == -1 )
	{
		m_dwMotionStartTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRandomBuffSkill::SetActionState() - %s, Not Exist ActionAnimation", GetName().c_str() );
	}
	else
	{
		float fAniRate = GetSkillMotionRate();

		float fTimeRate = fAniRate * m_fExtraAniRate;

		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		m_dwMotionStartTime = FRAMEGETTIME();
		m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate - FLOAT100;

		DWORD dwCreateTime = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
		m_dwAddBuffTime = m_dwMotionStartTime + dwCreateTime;
	}

	m_SkillState = SS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioRandomBuffSkill::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );

	m_dwEndAniStartTime = FRAMEGETTIME();		
	m_dwEndAniEndTime = m_dwEndAniStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*m_fEndAniRate;

	m_SkillState = SS_END;
}

void ioRandomBuffSkill::DestroyMapEffect( ioBaseChar* pChar )
{
	for( int i = 0; i < 2; ++i )
	{
		if( pChar )
			pChar->EndMapEffect( m_MapEffectID[i], false );
	}
}

void ioRandomBuffSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;
	auto pOwner = pStage->GetOwnerChar();

	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SSS_CREATE_RANDOM:
		{
			DWORD dwSeed = 0;
			rkPacket >> dwSeed;
			pChar->SetRandomSeed( dwSeed );
			SetRandomBuff( pChar, pStage, false );
		}
		break;
	}
}

void ioRandomBuffSkill::CreateMapEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if ( m_iValue > m_iRandomLimitCnt )
		return;
	if ( m_MapEffectList.empty() )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate(1.0f);
	vPos.y += m_fEffectYGap;
	vPos.x += m_fEffectXGap * FLOAT05;

	int iValue = m_iValue;
	int iOne = iValue % 10;

	ioHashString szOne = m_MapEffectList.at( iOne );
	ioMapEffect* pMapEffectOne = pChar->CreateMapEffectBySkill( szOne, vPos );
	if ( pMapEffectOne )
	{
		m_MapEffectID[0] = pMapEffectOne->GetUniqueID();
	}

	int iTen = iValue / 10;
	if ( iTen <= 0 )
		iTen = 0;

	vPos.x -= m_fEffectXGap;
	ioHashString szTen = m_MapEffectList.at( iTen );
	ioMapEffect* pMapEffectTen= pChar->CreateMapEffectBySkill( szTen, vPos );
	if ( !pMapEffectTen ) return;
	if ( pMapEffectTen )
	{
		m_MapEffectID[1] = pMapEffectTen->GetUniqueID();
	}
}