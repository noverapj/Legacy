#include "StdAfx.h"

#include "ioDummyComboFireSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "DummyCharDefine.h"

ioDummyComboFireSkill::ioDummyComboFireSkill(void)
{
	ClearData();
}

ioDummyComboFireSkill::ioDummyComboFireSkill( const ioDummyComboFireSkill &rhs )
	:ioNormalSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_DummyCharName( rhs.m_DummyCharName ),
	m_fDummyCharOffset( rhs.m_fDummyCharOffset ),
	m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	m_dwDummyCreateGap( rhs.m_dwDummyCreateGap ),
	m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
	m_fLoopDelayAnimationRate( rhs.m_fLoopDelayAnimationRate ),
	m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
	m_fLoopMoveAnimationRate( rhs.m_fLoopMoveAnimationRate ),
	m_EndAnimation( rhs.m_EndAnimation ),
	m_fEndAnimationRate( rhs.m_fEndAnimationRate ),
	m_iMaxDummyCnt( rhs.m_iMaxDummyCnt ),
	m_OwnerBuffList( rhs.m_OwnerBuffList ),
	m_dwLifeTime( rhs.m_dwLifeTime )
{
	ClearData();
}


ioDummyComboFireSkill::~ioDummyComboFireSkill(void)
{
}

void ioDummyComboFireSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );
	LoadOwnerBuffList( rkLoader );
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_bDisableGrowth= rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic	= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime		= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_dwDummyCreateGap = (DWORD)rkLoader.LoadInt_e( "dummy_create_gap", 0);

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;
	m_fDummyCharOffset	= rkLoader.LoadFloat_e( "dummy_char_offset", 0.0f );
	
	//loop motion
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );	
	m_LoopDelayAnimation = szBuf;
	m_fLoopDelayAnimationRate = rkLoader.LoadFloat_e( "delay_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;
	m_fLoopMoveAnimationRate = rkLoader.LoadFloat_e( "move_animation_rate", FLOAT1 );

	int iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	m_vDestroyDummyCharList.clear();
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "destroy_dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vDestroyDummyCharList.push_back( szBuf );
	}

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );	
	m_EndAnimation = szBuf;
	m_fEndAnimationRate = rkLoader.LoadFloat_e( "end_animation_rate", FLOAT1 );

	m_iMaxDummyCnt = rkLoader.LoadInt_e( "max_dummy_cnt", 1 );
	m_dwLifeTime = (DWORD)rkLoader.LoadInt_e( "life_time", 0 );
}

void ioDummyComboFireSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;
	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
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
}
ioSkill* ioDummyComboFireSkill::Clone()
{
	return new ioDummyComboFireSkill( *this );
}

void ioDummyComboFireSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_iDummyIndexList.clear();

	m_dwLoopStartTime = 0;
	m_bSetMoveAni = false;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;

	// 성장관련 요소
	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;
	m_iDummyActiveCnt = 0;
	m_iRandomCnt = 0;
}

bool ioDummyComboFireSkill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
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
	pChar->SetSkillMoveEnable( false, false );
	m_dwCheckTime = m_dwSkillStartTime = FRAMEGETTIME();
	if ( UseActiveCount( pChar ) )
		m_iCurActiveCount = m_iMaxDummyCnt;

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

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetLoopState( pChar );
	}	

	return true;
}

void ioDummyComboFireSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float	fCurTicGauge	= m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int		iTickGap		= (int)( dwCurTime - m_dwCheckTime );
	int		iCurTicTime		= m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetLoopState( pChar );
				AddOwnerBuff( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckNormalMoveAni( pChar );
			
			if( m_dwSkillStartTime > 0 && m_dwSkillStartTime + m_dwLifeTime < dwCurTime )
			{
				SetEndState( pChar );
			}

			if( m_dwLoopStartTime > 0 && m_dwLoopStartTime + m_dwDummyCreateGap < dwCurTime )
			{
				if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
				{
					SetActionState( pChar );
				}
			}

			if( pChar->IsNeedProcess() && pChar->IsDefenseKey() )
			{
				RemoveAllDummy( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				CreateDummyChar( pChar, pStage );
				if ( m_iDummyActiveCnt == -1 )
				{
					if( pChar->IsNeedProcess() )
						RemoveDummy( pChar );
				}
				SetLoopState( pChar );
			}
		}
		break;
	case SS_END:
		{
			if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime < dwCurTime )
			{
				if( pChar->IsNeedProcess() )
					RemoveAllDummy( pChar );
			}
		}
		break;
	}

	if( m_SkillState != SS_END )
	{
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{
				// 정조준 발사중에는 종료되지 않도록 예외를 둠.
				if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					if( m_SkillState != SS_END && m_SkillState != SS_ACTION )
					{
						SetEndState( pChar );
						return;
					}
				}
			}
		}
	}
}

void ioDummyComboFireSkill::OnSkillEnd( ioBaseChar *pChar )
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

	RemoveOwnerBuff( pChar );
	EndCameraBuff( pChar );

	pChar->SetSkillMoveEnable( false, false );

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_SkillSoundName, 0 );

	}
	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );
	pChar->ReduceNeedGaugeBySkill( this, GetOwnerItem()->GetMaxSkillGauge( pChar->GetCharName() ) );
	ClearData();
}

void ioDummyComboFireSkill::SetActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_ACTION;
	
	SetNormalSkill( pChar );
}

void ioDummyComboFireSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	pChar->SetSkillMoveEnable( false, false );

	if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << ST_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyComboFireSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_SkillState = SS_LOOP;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = FLOAT1;
	if( m_fLoopDelayAnimationRate > 0.0f )
		fAniRate = m_fLoopDelayAnimationRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	pChar->SetSkillMoveEnable( true, true );
	m_bSetMoveAni = false;
}

void ioDummyComboFireSkill::CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage || !pChar->IsNeedProcess() ) 
		return;

	if( m_DummyCharName.IsEmpty() ) 
		return;

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( m_fDummyCharOffset * vDir );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, iCurIndex,
													pChar->GetCharName(),
													vPos,
													0.0f,
													0, true );
	if ( pDummy )
	{
		m_iDummyIndexList.push_back( iCurIndex );
		ioTargetTrace7Dummy* pTTDummy = ToTargetTrace7Dummy( pDummy );
		if ( pTTDummy )
		{
			pTTDummy->SetRandomSeed( pChar->GetRandomSeed() + m_iRandomCnt );
			m_iRandomCnt++;
		}

		if ( m_iCurActiveCount > 0 )
		{
			m_iCurActiveCount--;
		}
		else
			m_iDummyActiveCnt = -1;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << ST_CREATE_DUMMY;
		kPacket << pChar->GetTargetRot();
		kPacket << vPos;
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyComboFireSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case ST_ACTION:
		{
			SetActionState( pChar );
		}
		break;
	case ST_CREATE_DUMMY:
		{
			if( m_DummyCharName.IsEmpty() ) 
				return;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			int iDummyIndex;
			rkPacket >> iDummyIndex;
			
			m_SkillState = SS_LOOP;

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, iDummyIndex,
															pChar->GetCharName(),
															vPos,
															0.0f,
															0, true );
			if ( pDummy )
			{
				m_iDummyIndexList.push_back( iDummyIndex );
				ioTargetTrace7Dummy* pTTDummy = ToTargetTrace7Dummy( pDummy );
				if ( pTTDummy )
				{
					pTTDummy->SetRandomSeed( pChar->GetRandomSeed() + m_iRandomCnt );
					m_iRandomCnt++;
				}
			}

		}
		break;
	case ST_REMOVE_DUMMY:
		{
			RemoveDummy( pChar );
		}
		break;
	case ST_REMOVE_ALL_DUMMY:
		{			
			RemoveAllDummy( pChar );
		}
		break;
	}
}

void ioDummyComboFireSkill::CheckNormalMoveAni( ioBaseChar* pChar )
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

void ioDummyComboFireSkill::RemoveAllDummy( ioBaseChar *pChar )
{
	if ( !pChar || m_iDummyIndexList.empty() )
		return;
	auto Iter = m_iDummyIndexList.begin();
	for ( ;Iter != m_iDummyIndexList.end(); ++Iter )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, (*Iter) );
		if( pDummy )	
		{
			pDummy->SetDieState( true );
		}
	}
	m_iCurActiveCount = m_iMaxDummyCnt;
	//패킷 전송
	if ( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << ST_REMOVE_ALL_DUMMY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyComboFireSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

bool ioDummyComboFireSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return ioSkill::IsProtected( iDefenseBreakType );
}

bool ioDummyComboFireSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ACTION:
	case SS_LOOP:
		return false;	
	case SS_END:
		if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime + dwActionStop < FRAMEGETTIME() )
			return true;

		return false;
	}

	return false;
}

void ioDummyComboFireSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )return;
	
	m_dwEndAniStartTime = FRAMEGETTIME();
	m_dwEndAniEndTime = m_dwEndAniStartTime;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = FLOAT1;
	if( m_fEndAnimationRate > 0.0f )
		fAniRate = m_fEndAnimationRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwEndAniEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fAniRate;

	pChar->SetSkillMoveEnable( false, false );

	m_SkillState = SS_END;
}

void ioDummyComboFireSkill::RemoveDummy( ioBaseChar *pChar )
{
	if ( !pChar || m_iDummyIndexList.empty() )
		return;
	
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndexList.front() );
	if( pDummy )
	{
		pDummy->SetDieState( true );
	}
	IntVec TempIndexList;

	auto Iter = m_iDummyIndexList.begin();
	m_iDummyIndexList.erase( Iter );
	TempIndexList.assign( Iter++, m_iDummyIndexList.end() );
	
	m_iDummyIndexList.clear();
	m_iDummyIndexList = TempIndexList;
	
	
	//패킷 전송
	if ( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << ST_REMOVE_DUMMY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyComboFireSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}

void ioDummyComboFireSkill::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;
	int iCnt = m_OwnerBuffList.size();

	for( int i=0; i<iCnt; ++i)
	{
		ioBuff* pBuff = pChar->GetBuff(m_OwnerBuffList[i].m_BuffName );
		if( pBuff )
		{
			pBuff->SetReserveEndBuff();
		}
	}
}

bool ioDummyComboFireSkill::HasOwnerBuff( const ioHashString &szBuffName )
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_OwnerBuffList[i].m_BuffName == szBuffName )
			return true;
	}

	return false;
}
