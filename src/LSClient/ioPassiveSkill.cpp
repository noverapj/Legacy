

#include "stdafx.h"

#include "ioPassiveSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "BuffDefine.h"

ioPassiveSkill::ioPassiveSkill()
{
	m_fCurPassiveGauge = 0;

	m_dwCurStopPassiveSkillID = 1;
	m_vStopPassiveSkillID.clear();

	m_vKeepTimeBuff.clear();
}

ioPassiveSkill::ioPassiveSkill( const ioPassiveSkill &rhs )
: ioSkill( rhs ),
 m_ReduceIgnoreBuffList( rhs.m_ReduceIgnoreBuffList ),
 m_RecoveryIgnoreBuffList( rhs.m_RecoveryIgnoreBuffList ),
 m_OwnerBuffList( rhs.m_OwnerBuffList ),
 m_TargetBuffList( rhs.m_TargetBuffList ),
 m_bUsePassiveGauge( rhs.m_bUsePassiveGauge ),
 m_fMaxPassiveGauge( rhs.m_fMaxPassiveGauge ),
 m_fDefaultPassiveGauge( rhs.m_fDefaultPassiveGauge ),
 m_fRecoveryValue( rhs.m_fRecoveryValue ),
 m_dwRecoveryTick( rhs.m_dwRecoveryTick ),
 m_dwFCModeRecoveryTick( rhs.m_dwFCModeRecoveryTick ),
 m_dwRecoveryDelay( rhs.m_dwRecoveryDelay ),
 m_bReduceNeedGauge( rhs.m_bReduceNeedGauge ),
 m_OwnerGaugeBuffInfoList( rhs.m_OwnerGaugeBuffInfoList ),
 m_OwnerGaugeWoundBuffInfoList( rhs.m_OwnerGaugeWoundBuffInfoList ),
 m_fReducePassiveGaugeOnAttack( rhs.m_fReducePassiveGaugeOnAttack ),
 m_fReducePassiveGaugeOnWound( rhs.m_fReducePassiveGaugeOnWound ),
 m_fReducePassiveGaugeOnUse( rhs.m_fReducePassiveGaugeOnUse ),
 m_fReducePassiveGaugeOnJump( rhs.m_fReducePassiveGaugeOnJump ),
 m_fRecoveryPassiveGaugeRateOnDelayRun( rhs.m_fRecoveryPassiveGaugeRateOnDelayRun ),
 m_fRecoveryPassiveGaugeRateOnDash( rhs.m_fRecoveryPassiveGaugeRateOnDash ),
 m_fRecoveryPassiveGaugeRateOnJump( rhs.m_fRecoveryPassiveGaugeRateOnJump ),
 m_fRecoveryPassiveGaugeRateOnEtc( rhs.m_fRecoveryPassiveGaugeRateOnEtc ),
 m_fRecoveryPassiveGaugeRateOnJumpAttack( rhs.m_fRecoveryPassiveGaugeRateOnJumpAttack ),
 m_fRecoveryPassiveGaugeRateOnAttack( rhs.m_fRecoveryPassiveGaugeRateOnAttack ),
 m_bReduceIgnoreOnDropZone( rhs.m_bReduceIgnoreOnDropZone ), 
 m_bReduceIgnoreOnJumpState( rhs.m_bReduceIgnoreOnJumpState ),
 m_bSetUseActiveCnt( rhs.m_bSetUseActiveCnt ),
 m_bSetKeepGaugeBuff( rhs.m_bSetKeepGaugeBuff ),
 m_bClearWoundKeepTimeList( rhs.m_bClearWoundKeepTimeList ),
 m_bMaintainPassiveGauge( rhs.m_bMaintainPassiveGauge )
{
	m_fCurPassiveGauge = 0;

	m_dwCurStopPassiveSkillID = 1;
	m_vStopPassiveSkillID.clear();
	m_vKeepTimeBuff.clear();
}

ioPassiveSkill::~ioPassiveSkill()
{
}

ioSkill* ioPassiveSkill::Clone()
{
	return new ioPassiveSkill( *this );
}

void ioPassiveSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioSkill::LoadProperty( rkLoader );
	LoadBuffList( rkLoader );

	char szBuf[MAX_PATH] = "", szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "reduce_ignore_buff_cnt", 0 );
	m_ReduceIgnoreBuffList.clear();
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "reduce_ignore_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ReduceIgnoreBuffList.push_back( ioHashString(szBuf) );
	}

	m_RecoveryIgnoreBuffList.clear();
	iCnt = rkLoader.LoadInt_e( "recovery_ignore_buff_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "recovery_ignore_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_RecoveryIgnoreBuffList.push_back( ioHashString(szBuf) );
	}

	m_bUsePassiveGauge = rkLoader.LoadBool_e( "use_passive_gauge", false );
	m_bStopPassiveGaugeIOnSkipBuff = rkLoader.LoadBool_e( "stop_passive_gauge_on_skip_buff", false );
	m_fMaxPassiveGauge = rkLoader.LoadFloat_e( "max_passive_gauge", FLOAT100 );
	m_fDefaultPassiveGauge = rkLoader.LoadFloat_e( "default_passive_gauge", 0.0f );
	m_bMaintainPassiveGauge = rkLoader.LoadBool_e( "maintain_passive_gauge", false );

	m_fRecoveryValue = rkLoader.LoadFloat_e( "recovery_gauge_value", 0.0f );
	m_dwRecoveryTick = (DWORD)rkLoader.LoadInt_e( "recovery_gauge_tick", 0 );
	m_dwFCModeRecoveryTick = (DWORD)rkLoader.LoadInt_e( "FC_recovery_gauge_tick", 0 );

	m_dwRecoveryDelay = (DWORD)rkLoader.LoadInt_e( "recovery_delay_on_zero_gauge", 0 );
	m_bReduceNeedGauge = rkLoader.LoadBool_e( "reduce_need_gauge", false );

	m_fReducePassiveGaugeOnAttack = rkLoader.LoadFloat_e( "reduce_passive_gauge_on_attack", 0.0f );	
	m_fReducePassiveGaugeOnWound = rkLoader.LoadFloat_e( "reduce_passive_gauge_on_wound", 0.0f );	
	m_fReducePassiveGaugeOnUse = rkLoader.LoadFloat_e( "reduce_passive_gauge_on_use", 0.0f );
	m_fReducePassiveGaugeOnJump = rkLoader.LoadFloat_e( "reduce_passive_gauge_on_jump", 0.0f );
	m_bReduceIgnoreOnDropZone = rkLoader.LoadBool_e( "reduce_ignore_on_drop_zone", false );
	m_bReduceIgnoreOnJumpState = rkLoader.LoadBool_e( "reduce_ignore_on_jump_state", false );

	m_fRecoveryPassiveGaugeRateOnDelayRun = rkLoader.LoadFloat_e( "recovery_passive_gauge_rate_on_delay_run", FLOAT1 );
	m_fRecoveryPassiveGaugeRateOnDash    = rkLoader.LoadFloat_e( "recovery_passive_gauge_rate_on_dash", FLOAT1 );
	m_fRecoveryPassiveGaugeRateOnJump     = rkLoader.LoadFloat_e( "recovery_passive_gauge_rate_on_jump", FLOAT1 );
	m_fRecoveryPassiveGaugeRateOnEtc      = rkLoader.LoadFloat_e( "recovery_passive_gauge_rate_on_etc", FLOAT1 );
	m_fRecoveryPassiveGaugeRateOnJumpAttack = rkLoader.LoadFloat_e( "recovery_passive_gauge_rate_on_jump_attack", FLOAT1 );
	m_fRecoveryPassiveGaugeRateOnAttack = rkLoader.LoadFloat_e( "recovery_passive_gauge_rate_on_attack", FLOAT1 );
	m_bSetUseActiveCnt = rkLoader.LoadBool_e( "set_use_active_cnt", false );
	m_bSetKeepGaugeBuff = rkLoader.LoadBool_e( "set_keep_gauge_buff", false );

	m_bClearWoundKeepTimeList = rkLoader.LoadBool_e( "set_clear_keeptime_by_wound", false );
}

bool ioPassiveSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	return true;
}


bool ioPassiveSkill::HasBuff( int iBuffType ) const
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_OwnerBuffList[i].m_iBuffType == iBuffType )
			return true;
	}

	return false;
}

bool ioPassiveSkill::HasBuff( const ioHashString& szBuffName ) const
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_OwnerBuffList[i].m_BuffName == szBuffName )
			return true;
	}

	return false;
}

void ioPassiveSkill::CheckReducePassiveSkillGrade( ioBaseChar* pChar, ioBaseChar* pAttacker )
{
	if( !UsePassiveGauge() )
		return;
	
	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;
		
	GaugeBuffInfoList::iterator info_iter;
	for( info_iter =  m_OwnerGaugeBuffInfoList.begin(); info_iter != m_OwnerGaugeBuffInfoList.end(); ++info_iter )
	{
		GaugeBuffInfo gauge_info = (*info_iter);

		float fMaxRate = gauge_info.m_fMaxPassiveGauge;
		float fMinRate = gauge_info.m_fMinPassiveGauge;

		if( fRate <= fMaxRate && fRate > fMinRate )
		{
			BuffInfoList::iterator buff_iter;
			for( buff_iter = gauge_info.m_Buff.begin(); buff_iter != gauge_info.m_Buff.end(); ++buff_iter )
			{
				ioBuffInfo buff_info = (*buff_iter);
				ioBuff* pBuff = pChar->GetBuff( buff_info.m_BuffName );
				
				if( pBuff )
				{
					if( gauge_info.m_bCheckBuffActivated )
					{
						if( pBuff->GetPassiveActivated() && 0.0f < gauge_info.m_fGradeEndReduce )
							ReduceCurPassiveGauge( gauge_info.m_fGradeEndReduce );
					}
					else
					{
						if( 0.0f < gauge_info.m_fGradeEndReduce )
							ReduceCurPassiveGauge( gauge_info.m_fGradeEndReduce );
					}
				}
			}
		}
	}	
}

bool ioPassiveSkill::CheckReducePassiveSkillGaugeBuff( ioBaseChar* pChar, int iBuffType )
{
	GaugeBuffInfoList::iterator info_iter;
	BuffInfoList::iterator buff_iter;

	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;

	//패시브 스킬에서 생성된 버프가 게이지를 감소 시킬 수 있는지 체크
	for( info_iter = m_OwnerGaugeBuffInfoList.begin(); info_iter != m_OwnerGaugeBuffInfoList.end(); ++info_iter )
	{
		GaugeBuffInfo gauge_info = (*info_iter);

		float fMaxRate = gauge_info.m_fMaxPassiveGauge;
		float fMinRate = gauge_info.m_fMinPassiveGauge;
		
		if( fRate <= fMaxRate && fRate > fMinRate )
		{
			for( buff_iter = gauge_info.m_Buff.begin(); buff_iter != gauge_info.m_Buff.end(); ++buff_iter )
			{
				ioBuffInfo buff_info = (*buff_iter);
				if( buff_info.m_iBuffType == iBuffType )
				{
					ioBuff* pBuff = pChar->GetBuff( buff_info.m_BuffName );
					if( pBuff && pBuff->isPassiveSkillGauageReduce() )
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool ioPassiveSkill::CheckReducePassiveSkillGaugeBuff( ioBaseChar* pChar, int iBuffType, ioHashString& szBuffName )
{
	GaugeBuffInfoList::iterator info_iter;
	BuffInfoList::iterator buff_iter;

	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;

	//패시브 스킬에서 생성된 버프가 게이지를 감소 시킬 수 있는지 체크
	for( info_iter =  m_OwnerGaugeBuffInfoList.begin(); info_iter != m_OwnerGaugeBuffInfoList.end(); ++info_iter )
	{
		GaugeBuffInfo gauge_info = (*info_iter);

		float fMaxRate = gauge_info.m_fMaxPassiveGauge;
		float fMinRate = gauge_info.m_fMinPassiveGauge;

		if( fRate <= fMaxRate && fRate > fMinRate )
		{			
			for( buff_iter = gauge_info.m_Buff.begin(); buff_iter != gauge_info.m_Buff.end(); ++buff_iter )
			{
				ioBuffInfo buff_info = (*buff_iter);
				if( buff_info.m_iBuffType == iBuffType )
				{
					ioBuff* pBuff = pChar->GetBuff( buff_info.m_BuffName );
					if( pBuff && pBuff->isPassiveSkillGauageReduce() )
					{
						szBuffName = buff_info.m_BuffName;
						return true;
					}
				}
			}
		}
	}

	return false;
}

int ioPassiveSkill::GetTypeID() const
{
	return ST_PASSIVE;
}

void ioPassiveSkill::OnSkillEnd( ioBaseChar *pChar )
{
	pChar->EndEffect( m_SkillEffectName );

	m_bReduceGauge = false;
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	m_fCurPassiveGauge = m_fDefaultPassiveGauge;
}

bool ioPassiveSkill::OnSkillStart( ioBaseChar *pChar )
{
	//CheckSkillDesc( pChar );
	if( m_fCurPassiveGauge <= 0.0f)
		m_fCurPassiveGauge = m_fDefaultPassiveGauge;

	pChar->AttachEffect( m_SkillEffectName );
	AddOwnerBuff( pChar );

	m_dwCheckZeroGaugeTime = 0;

	return true;
}

void ioPassiveSkill::LoadBuffList( ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_OwnerBuffList.clear();
	m_TargetBuffList.clear();

	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
		ioHashString szBuffName = szBuff;

		if( !szBuffName.IsEmpty() )
		{
			int iType = g_BuffMaker.GetBuffType( szBuffName );
			if( iType != BT_NONE )
			{
				ioBuffInfo kBuffInfo( szBuffName, iType );
				m_OwnerBuffList.push_back( kBuffInfo );
			}
		}
	}

	iCnt = rkLoader.LoadInt_e( "target_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "target_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
		ioHashString szBuffName = szBuff;

		if( !szBuffName.IsEmpty() )
		{
			int iType = g_BuffMaker.GetBuffType( szBuffName );
			if( iType != BT_NONE )
			{
				ioBuffInfo kBuffInfo( szBuffName, iType );
				m_TargetBuffList.push_back( kBuffInfo );
			}
		}
	}

	iCnt = rkLoader.LoadInt_e( "owner_gauge_cnt", 0 );
	m_OwnerGaugeBuffInfoList.clear();
	m_OwnerGaugeBuffInfoList.reserve( iCnt );

	for( int i = 0; i < iCnt; ++i )
	{
		GaugeBuffInfo tGaugeBuffInfo;

		wsprintf_e( szTitle, "owner_max_gauge%d", i + 1 );
		tGaugeBuffInfo.m_fMaxPassiveGauge = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "owner_min_gauge%d", i + 1 );
		tGaugeBuffInfo.m_fMinPassiveGauge = rkLoader.LoadFloat( szTitle, 0.0f );		

		wsprintf_e( szTitle, "owner_gauge%d_grade_wound", i + 1 );
		tGaugeBuffInfo.m_fGradeEndReduce = rkLoader.LoadFloat( szTitle, 0.0f );		

		wsprintf_e( szTitle, "owner_gauge%d_buff_keep_time", i+1 );
		tGaugeBuffInfo.m_dwBuffKeepTime = (DWORD)rkLoader.LoadInt( szTitle, 0 );

		wsprintf_e( szTitle, "owner_gauge%d_buff_end_gauge_clear", i+1 );
		tGaugeBuffInfo.m_bBuffEndGuageClear = rkLoader.LoadBool( szTitle, 0 );

		wsprintf_e( szTitle, "owner_gauge%d_check_buff_activated", i+1 );
		tGaugeBuffInfo.m_bCheckBuffActivated = rkLoader.LoadBool( szTitle, 0 );

		wsprintf_e( szTitle, "owner_gauge%d_buff_cnt", i+1 );
		int iBuffCnt = rkLoader.LoadInt( szTitle, 0 );

		BuffInfoList vBuffList;
		vBuffList.clear();
		vBuffList.reserve( iBuffCnt );

		for( int j=0; j<iBuffCnt; ++j )
		{
			wsprintf_e( szTitle, "owner_gauge%d_buff%d", i+1, j+1 );
			rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
			ioHashString szBuffName = szBuff;

			if( !szBuffName.IsEmpty() )
			{
				int iType = g_BuffMaker.GetBuffType( szBuffName );
				if( iType != BT_NONE )
				{
					ioBuffInfo kBuffInfo( szBuffName, iType );
					vBuffList.push_back( kBuffInfo );
				}
			}
		}

		tGaugeBuffInfo.m_Buff = vBuffList;
		m_OwnerGaugeBuffInfoList.push_back( tGaugeBuffInfo );
	}

	int iWoundBuffCnt = rkLoader.LoadInt_e( "owner_gauge_wound_buff_cnt", 0 );
	m_OwnerGaugeWoundBuffInfoList.clear();
	m_OwnerGaugeWoundBuffInfoList.reserve( iWoundBuffCnt );

	for( int i=0; i < iWoundBuffCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_gauge%d_wound_buff_max", i+1 );
		float fMax = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf_e( szTitle, "owner_gauge%d_wound_buff_min", i+1 );
		float fMin = rkLoader.LoadFloat( szTitle, 0.0f );		

		wsprintf_e( szTitle, "owner_gauge%d_wound_buff_cnt", i+1 );
		int iBuffCnt = rkLoader.LoadInt( szTitle, 0 );

		BuffInfoList vBuffList;
		vBuffList.clear();
		vBuffList.reserve( iBuffCnt );

		for( int j=0; j<iBuffCnt; ++j )
		{
			wsprintf_e( szTitle, "owner_gauge%d_wound_buff%d", i+1, j+1 );
			rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
			ioHashString szBuffName = szBuff;

			if( !szBuffName.IsEmpty() )
			{
				int iType = g_BuffMaker.GetBuffType( szBuffName );
				if( iType != BT_NONE )
				{
					ioBuffInfo kBuffInfo( szBuffName, iType );
					vBuffList.push_back( kBuffInfo );
				}
			}
		}

		GaugeWoundBuffInfo kWoundBuffInfo( fMax, fMin, vBuffList );

		wsprintf_e( szTitle, "owner_gauge%d_wound_buff_duration", i+1 );
		kWoundBuffInfo.m_dwDuration = (int)rkLoader.LoadInt( szTitle, 0 );
		
		wsprintf_e( szTitle, "owner_gauge%d_wound_buff_duration_air", i+1 );
		kWoundBuffInfo.m_dwAirDuration = (int)rkLoader.LoadInt( szTitle, 0 );

		wsprintf_e( szTitle, "owner_gauge%d_wound_buff_duration_down", i+1 );
		kWoundBuffInfo.m_dwDownDuration = (int)rkLoader.LoadInt( szTitle, 0 );

		m_OwnerGaugeWoundBuffInfoList.push_back( kWoundBuffInfo );
	}
}

void ioPassiveSkill::RemoveSkillBuff( ioBaseChar *pChar )
{
	int iCnt = m_OwnerBuffList.size();
	for(int i=0; i < iCnt; ++i )
	{
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
	}

	iCnt = m_OwnerGaugeBuffInfoList.size();
	for(int i=0; i < iCnt; ++i )
	{
		int iBuffCnt = m_OwnerGaugeBuffInfoList[i].m_Buff.size();
		for( int j=0; j<iBuffCnt; ++j )
		{
			pChar->RemoveBuff( m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName );
		}
	}

	iCnt = m_OwnerGaugeWoundBuffInfoList.size();
	for(int i=0; i < iCnt; ++i )
	{
		int iBuffCnt = m_OwnerGaugeWoundBuffInfoList[i].m_BuffList.size();
		for( int j=0; j<iBuffCnt; ++j )
		{
			pChar->RemoveBuff( m_OwnerGaugeWoundBuffInfoList[i].m_BuffList[j].m_BuffName );
		}
	}
}

void ioPassiveSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( !m_OwnerBuffList[i].m_BuffName.IsEmpty() )
		{
			pChar->AddNewBuffWithAnimateTime( m_OwnerBuffList[i].m_BuffName,
											  pChar->GetCharName(),
											  szItemName,
											  this,
											  GetSkillMotionTime(),
											  true );
		}
	}
}

void ioPassiveSkill::AddTargetBuff( ioBaseChar *pChar )
{
	int iCnt = m_TargetBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( !m_TargetBuffList[i].m_BuffName.IsEmpty() )
		{
			pChar->AddNewBuff(m_TargetBuffList[i].m_BuffName, "", "", NULL );
		}
	}
}

void ioPassiveSkill::CheckAttackProcess( ioBaseChar *pOwner, ioBaseChar *pAttacker )
{
	if( m_fReducePassiveGaugeOnAttack > 0.0f )
	{
		ReduceCurPassiveGauge( m_fReducePassiveGaugeOnAttack );
	}
}

void ioPassiveSkill::CheckWoundProcess( ioBaseChar *pOwner, ioBaseChar *pAttacker )
{
	if( m_fReducePassiveGaugeOnWound > 0.0f )
	{
		CheckOwnerGaugeWoundBuff( pOwner );
		ReduceCurPassiveGauge( m_fReducePassiveGaugeOnWound );
	}
}

void ioPassiveSkill::CheckUseProcess( ioBaseChar *pOwner )
{
	if( m_fReducePassiveGaugeOnUse > 0.0f )
	{
		ReduceCurPassiveGauge( m_fReducePassiveGaugeOnUse );
	}
}

void ioPassiveSkill::CheckJumpProcess( ioBaseChar *pOwner )
{
	if( m_fReducePassiveGaugeOnJump > 0.0f )
	{
		ReduceCurPassiveGauge( m_fReducePassiveGaugeOnJump );
	}
}

bool ioPassiveSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	return true;
}

void ioPassiveSkill::UpdatePassiveGauge( ioBaseChar *pChar )
{
	if( !m_bUsePassiveGauge )	return;
	if( !pChar )	return;

	if( pChar->GetState() == CS_DIE ) return;
	if( pChar->GetState() == CS_VIEW ) return;
	if( pChar->GetState() == CS_OBSERVER ) return;
	if( pChar->GetState() == CS_LOADING ) return;
	if( pChar->GetState() == CS_READY ) return;
	if( pChar->GetState() == CS_DROP_ZONE_DOWN ) return;
	if( pChar->IsExcavating() )
	{
		RemoveAllPassiveGaugeBuff( pChar );
		m_fCurPassiveGauge = 0.0f;
		return;
	}

	if( pChar->HasBuff( BT_SEAL_SKILL ) )
	{
		RemoveAllPassiveGaugeBuff( pChar );
		return;
	}

	if( m_bClearWoundKeepTimeList )
		UpdatePassiveGaugeBuff2( pChar );
	else
		UpdatePassiveGaugeBuff( pChar );

	// UpdatePassiveGaugeBuff() 이후 높이체크
	// 이후 UpdateRecoveryPassiveGauge()가 와야 함.
	bool bDropZone = false;
	D3DXVECTOR3 vCurPos = pChar->GetWorldPosition();
	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		float fMapHeight = pStage->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f );
		if( fMapHeight <= 0.0f )
			bDropZone = true;
	}

	if( bDropZone )
	{
		if( m_bReduceIgnoreOnDropZone )
			return;
	}

	if( m_bReduceIgnoreOnJumpState && pChar->GetState() == CS_JUMP && pChar->GetJumpState() != JS_LANDING )
		return;

	UpdateRecoveryPassiveGauge( pChar );
	CheckKeepTimeAllBuff( pChar );
}


void ioPassiveSkill::UpdatePassiveGaugeBuff2( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerGaugeBuffInfoList.empty() )	return;

	if( !m_vStopPassiveSkillID.empty() )
		return;

	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;

	bool bAddBuff = false;
	int iCnt = m_OwnerGaugeBuffInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		float fMaxRate = m_OwnerGaugeBuffInfoList[i].m_fMaxPassiveGauge;
		float fMinRate = m_OwnerGaugeBuffInfoList[i].m_fMinPassiveGauge;

		if( fRate <= fMaxRate && fRate > fMinRate )
		{
			int iBuffCnt = m_OwnerGaugeBuffInfoList[i].m_Buff.size();

			if( !m_OwnerGaugeBuffInfoList[i].m_bLoadedBuff )
			{
				for( int loop = 0; loop < (int)m_OwnerGaugeBuffInfoList.size(); loop++ )
				{
					m_OwnerGaugeBuffInfoList[loop].m_bLoadedBuff = false;
				}

				m_OwnerGaugeBuffInfoList[i].m_bLoadedBuff = true;

				for( int j=0; j<iBuffCnt; ++j )
				{
					if( !pChar->HasBuff(m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName) )
					{
						bAddBuff = true;
						ioBuff *pBuff = pChar->AddNewBuff( m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName,
							pChar->GetCharName(),
							GetOwnerItem()->GetName(),
							this );
					}
				}
			}
		}
		else
		{
			if ( m_bSetKeepGaugeBuff )
				return;

			DWORD dwCurTime = FRAMEGETTIME();		
			int iBuffCnt = m_OwnerGaugeBuffInfoList[i].m_Buff.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				if( 0 < m_OwnerGaugeBuffInfoList[i].m_dwBuffKeepTime )
				{
					if( !CheckKeepTimeBuff( pChar, m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName, i, j ) )
					{
						if( pChar->HasBuff(m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName) )
						{
							KeepTimeBuffInfo BuffInfo;
							BuffInfo.m_szBuffName	 = m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName;
							BuffInfo.m_dwBuffEndTime = dwCurTime + m_OwnerGaugeBuffInfoList[i].m_dwBuffKeepTime;
							BuffInfo.m_GaugeIdx		 = i;
							BuffInfo.m_BuffIdx		 = j;

							m_vKeepTimeBuff.push_back( BuffInfo );
						}
					}
				}
				else
				{
					if( !m_bClearWoundKeepTimeList )
					{
						if( pChar->HasBuff(m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName) )
						{
							pChar->RemoveBuff( m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName );
						}
					}
				}
			}
		}
	}

	if( bAddBuff && pChar->IsNeedSendNetwork() )
	{
		int iSlot = m_pOwnerItem->GetType() - 1;
		SP2Packet kPacket( CUPK_PASSIVE_GAUGE );
		kPacket << pChar->GetCharName();
		kPacket << iSlot;
		kPacket << m_fCurPassiveGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPassiveSkill::UpdatePassiveGaugeBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerGaugeBuffInfoList.empty() )	return;

	if( !m_vStopPassiveSkillID.empty() )
		return;

	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;

	bool bAddBuff = false;
	int iCnt = m_OwnerGaugeBuffInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		float fMaxRate = m_OwnerGaugeBuffInfoList[i].m_fMaxPassiveGauge;
		float fMinRate = m_OwnerGaugeBuffInfoList[i].m_fMinPassiveGauge;

		if( fRate <= fMaxRate && fRate > fMinRate )
		{
			int iBuffCnt = m_OwnerGaugeBuffInfoList[i].m_Buff.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				if( !pChar->HasBuff(m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName) )
				{
					bAddBuff = true;
					ioBuff *pBuff = pChar->AddNewBuff( m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName,
														pChar->GetCharName(),
														GetOwnerItem()->GetName(),
														this );
				}
			}
		}
		else
		{
			if ( m_bSetKeepGaugeBuff )
				return;

			DWORD dwCurTime = FRAMEGETTIME();		
			int iBuffCnt = m_OwnerGaugeBuffInfoList[i].m_Buff.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				if( 0 < m_OwnerGaugeBuffInfoList[i].m_dwBuffKeepTime )
				{
					if( !CheckKeepTimeBuff( pChar, m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName, i, j ) )
					{
						if( pChar->HasBuff(m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName) )
						{
							KeepTimeBuffInfo BuffInfo;
							BuffInfo.m_szBuffName	 = m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName;
							BuffInfo.m_dwBuffEndTime = dwCurTime + m_OwnerGaugeBuffInfoList[i].m_dwBuffKeepTime;
							BuffInfo.m_GaugeIdx		 = i;
							BuffInfo.m_BuffIdx		 = j;

							m_vKeepTimeBuff.push_back( BuffInfo );
						}
					}
				}
				else
				{
					if( pChar->HasBuff(m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName) )
					{
						pChar->RemoveBuff( m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName );
					}
				}
			}
		}
	}

	if( bAddBuff && pChar->IsNeedSendNetwork() )
	{
		int iSlot = m_pOwnerItem->GetType() - 1;
		SP2Packet kPacket( CUPK_PASSIVE_GAUGE );
		kPacket << pChar->GetCharName();
		kPacket << iSlot;
		kPacket << m_fCurPassiveGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPassiveSkill::RemoveAllPassiveGaugeBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerGaugeBuffInfoList.empty() )	return;

	int iCnt = m_OwnerGaugeBuffInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		int iBuffCnt = m_OwnerGaugeBuffInfoList[i].m_Buff.size();
		for( int j=0; j<iBuffCnt; ++j )
		{
			if( pChar->HasBuff(m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName) )
			{
				pChar->RemoveBuff( m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName );
			}
		}
	}

	iCnt = m_OwnerGaugeWoundBuffInfoList.size();
	for(int i=0; i < iCnt; ++i )
	{
		int iBuffCnt = m_OwnerGaugeWoundBuffInfoList[i].m_BuffList.size();
		for( int j=0; j<iBuffCnt; ++j )
		{
			pChar->RemoveBuff( m_OwnerGaugeWoundBuffInfoList[i].m_BuffList[j].m_BuffName );
		}
	}
}

void ioPassiveSkill::CheckKeepTimeAllBuff( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_vKeepTimeBuff.empty() ) return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	KeepTimeBuffInfoList::iterator iter;
	for( iter = m_vKeepTimeBuff.begin(); iter != m_vKeepTimeBuff.end(); )
	{
		KeepTimeBuffInfo Info = (*iter);

		if( m_bClearWoundKeepTimeList )
		{
			if( Info.m_dwBuffEndTime < dwCurTime )
			{
				pChar->RemoveBuff( Info.m_szBuffName );
				iter = m_vKeepTimeBuff.erase( iter );
			}
			else
			{
				iter++;
			}
		}
		else
		{
			if( Info.m_dwBuffEndTime < dwCurTime && pChar->HasBuff( Info.m_szBuffName ) )
			{
				pChar->RemoveBuff( Info.m_szBuffName );
				iter = m_vKeepTimeBuff.erase( iter );
			}
			else
			{
				iter++;
			}
		}
	}
}

bool ioPassiveSkill::CheckKeepTimeBuff( ioBaseChar *pChar, ioHashString szBuffName, DWORD dwGaugeIdx, DWORD dwBuffIdx )
{	
	if( m_vKeepTimeBuff.empty() ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	KeepTimeBuffInfoList::iterator iter;
	for( iter = m_vKeepTimeBuff.begin(); iter != m_vKeepTimeBuff.end(); iter++ )
	{
		KeepTimeBuffInfo Info = (*iter);
		if( Info.m_szBuffName == szBuffName &&
			Info.m_BuffIdx == dwBuffIdx &&
			Info.m_GaugeIdx == dwGaugeIdx )
		{
			return true;
		}
	}

	return false;
}

void ioPassiveSkill::UpdateRecoveryPassiveGauge( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !m_bUsePassiveGauge ) return;
	if( m_bStopPassiveGaugeIOnSkipBuff && pChar->HasBuff( BT_SKIP_STUN ) )
		return;
	if( m_dwCheckZeroGaugeTime > 0 && m_dwCheckZeroGaugeTime + m_dwRecoveryDelay >= FRAMEGETTIME() )
		return;
	if( !pChar->GetUseEnableSkill( GetEquipSlotByCode(GetOwnerItem()->GetItemCode() ), SUT_NORMAL, true ) )
		return;

	int iCnt = m_RecoveryIgnoreBuffList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBuff *pBuff = pChar->GetBuff( m_RecoveryIgnoreBuffList[i] );
		if( pBuff && pBuff->IsLive() )
		{
			return;
		}
	}

	iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioBuff *pBuff = pChar->GetBuff( m_OwnerBuffList[i].m_BuffName );
		if( pBuff && pBuff->OnReduceGauge() )
		{
			return;
		}
	}

	float fFrameGap = g_FrameTimer.GetCurFrameGap();

	float fRate = 0.0f;
	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage && pStage->GetModeType() == MT_FIGHT_CLUB && m_dwFCModeRecoveryTick > 0 )
	{
		fRate = fFrameGap / m_dwFCModeRecoveryTick;
	}
	else
	{
		fRate = fFrameGap / m_dwRecoveryTick;
	}

	float fValue = m_fRecoveryValue * fRate;
	
	// 상태에 따른 보정
	switch( pChar->GetState() )
	{
	case CS_RUN:
	case CS_DELAY:
	case CS_SPECIAL_MOTION:
	case CS_ETCITEM_MOTION:
		{
			fValue *= m_fRecoveryPassiveGaugeRateOnDelayRun;
		}
		break;
	case CS_JUMP:
		{
			switch( pChar->GetJumpState() )
			{
			case JS_AIM_JUMP:
			case JS_JUMP_ATTACK:
			case JS_AUTO_SHOT:
			case JS_AIM_JUMP2:
			case JS_COMBO_DASH:
				fValue *= m_fRecoveryPassiveGaugeRateOnJumpAttack;
				break;
			default:
				fValue *= m_fRecoveryPassiveGaugeRateOnJump;
				break;
			}			
		}
		break;
	case CS_DASH:
		{
			fValue *= m_fRecoveryPassiveGaugeRateOnDash;
		}
		break;
	case CS_ATTACK:
	case CS_DASH_ATTACK:
	case CS_USING_SKILL:
	case CS_COUNTER_ATTACK:
	case CS_NEW_JUMP_ATTACK:
	case CS_BUFF_AIR_ATTACK:
	case CS_DEFENSE_ATTACK:
	case CS_BLOW_DASH_ATTACK:
	case CS_CHARGE_FLY:
		{
			fValue *= m_fRecoveryPassiveGaugeRateOnAttack;
		}
		break;
	default:
		{
			if( pChar->IsSpecailAttackState() )
				fValue *= m_fRecoveryPassiveGaugeRateOnAttack;
			else
				fValue *= m_fRecoveryPassiveGaugeRateOnEtc;
		}
		break;
	}

	float fPreGauge = m_fCurPassiveGauge;

	m_fCurPassiveGauge += fValue;
	m_fCurPassiveGauge = min( m_fCurPassiveGauge, m_fMaxPassiveGauge );

	if( fPreGauge <= 0.0f && m_fCurPassiveGauge > 0.0f )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			int iSlot = GetOwnerItem()->GetType() - 1;
			SP2Packet kPacket( CUPK_PASSIVE_GAUGE );
			kPacket << pChar->GetCharName();
			kPacket << iSlot;
			kPacket << m_fCurPassiveGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

float ioPassiveSkill::ReduceCurPassiveGauge( float fUseGauge )
{
	if( m_bClearWoundKeepTimeList )
	{
		for( int loop = 0; loop < (int)m_OwnerGaugeBuffInfoList.size(); loop++ )
		{
			m_OwnerGaugeBuffInfoList[loop].m_bLoadedBuff = false;
		}
	}

	if( !m_bUsePassiveGauge )
		return 0.0f;

	if( m_bReduceNeedGauge && m_fCurPassiveGauge < fUseGauge )
		return -FLOAT1;

	int iCnt = m_ReduceIgnoreBuffList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioItem *pItem = GetOwnerItem();
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		ioBaseChar *pOwner = NULL;
		if( pItem && pStage )
			pOwner = pStage->GetBaseChar( pItem->GetOwnerName() );

		if( pOwner )
		{
			ioBuff *pBuff = pOwner->GetBuff( m_ReduceIgnoreBuffList[i] );
			if( pBuff && pBuff->IsLive() )
			{
				return m_fCurPassiveGauge;
			}
		}
	}

	float fPreGauge = m_fCurPassiveGauge;
	m_fCurPassiveGauge -= fUseGauge;
	m_fCurPassiveGauge = max( m_fCurPassiveGauge, 0.0f );

	if( m_fCurPassiveGauge <= 0.0f )
		m_dwCheckZeroGaugeTime = FRAMEGETTIME();

	if( fPreGauge > 0.0f && m_fCurPassiveGauge <= 0.0f )
	{
		ioItem *pItem = GetOwnerItem();
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		ioBaseChar *pOwner = NULL;
		if( pItem && pStage )
			pOwner = pStage->GetBaseChar( pItem->GetOwnerName() );

		if( pOwner && pOwner->IsNeedSendNetwork() )
		{
			int iSlot = GetOwnerItem()->GetType() - 1;
			SP2Packet kPacket( CUPK_PASSIVE_GAUGE );
			kPacket << pOwner->GetCharName();
			kPacket << iSlot;
			kPacket << m_fCurPassiveGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return m_fCurPassiveGauge;
}

void ioPassiveSkill::SetCurPassiveGauge( float fValue )
{
	if( !m_bUsePassiveGauge )	return;
	
	m_fCurPassiveGauge = min( fValue, m_fMaxPassiveGauge );
}

bool ioPassiveSkill::UsePassiveGauge() const
{
	return m_bUsePassiveGauge;
}

bool ioPassiveSkill::IsBuffEndGaugeClear( const ioHashString& rkName ) const
{
	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;

	GaugeBuffInfoList::const_iterator iter = m_OwnerGaugeBuffInfoList.begin();
	for( ; iter != m_OwnerGaugeBuffInfoList.end(); ++iter )
	{
		const GaugeBuffInfo& rkGaugeBuff = *iter;
		if( !rkGaugeBuff.m_bBuffEndGuageClear )
			continue;

		BuffInfoList::const_iterator buff_iter = rkGaugeBuff.m_Buff.begin();
		for( ; buff_iter != rkGaugeBuff.m_Buff.end(); ++buff_iter )
		{
			const ioBuffInfo& rkBuffInfo = *buff_iter;
			if( rkBuffInfo.m_BuffName == rkName && fRate <= rkGaugeBuff.m_fMaxPassiveGauge && fRate > rkGaugeBuff.m_fMinPassiveGauge )
			{
				return true;
			}
		}
	}

	return false;
}

const ioHashStringVec ioPassiveSkill::GetOwnerGaugeBuffName( ioBaseChar *pOwner ) const
{
	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;

	ioHashStringVec vGaugeBuffNameList;
	vGaugeBuffNameList.clear();
	bool bAddBuff = false;
	int iCnt = m_OwnerGaugeBuffInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		float fMaxRate = m_OwnerGaugeBuffInfoList[i].m_fMaxPassiveGauge;
		float fMinRate = m_OwnerGaugeBuffInfoList[i].m_fMinPassiveGauge;

		if( fRate <= fMaxRate && fRate > fMinRate )
		{
			int iBuffCnt = m_OwnerGaugeBuffInfoList[i].m_Buff.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				ioBuff *pBuff = pOwner->GetBuff( m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName );
				if( ToCallBuffOnAttackBuff( pBuff ) )
				{
					vGaugeBuffNameList.push_back( ToCallBuffOnAttackBuff( pBuff )->GetCallBuffName() );
				}
			}
		}
	}
	return vGaugeBuffNameList;
}

void ioPassiveSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
}

DWORD ioPassiveSkill::StopPassiveSkill( ioBaseChar *pChar )
{
	RemoveSkillBuff( pChar );

	m_dwCurStopPassiveSkillID++;
	m_vStopPassiveSkillID.push_back( m_dwCurStopPassiveSkillID );

	return m_dwCurStopPassiveSkillID;
}

void ioPassiveSkill::RestorePassiveSkill( ioBaseChar *pChar, const DWORD &dwID )
{
	DWORDVec::iterator iter = m_vStopPassiveSkillID.begin();
	while( iter != m_vStopPassiveSkillID.end() )
	{
		if( (*iter) == dwID )
		{
			m_vStopPassiveSkillID.erase( iter );
			break;
		}

		++iter;
	}

	if( m_vStopPassiveSkillID.empty() )
		OnSkillStart( pChar );
}

void ioPassiveSkill::CheckPassiveSkillBuff( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( pChar->HasBuff(BT_STOP_PASSIVE) || pChar->HasBuff(BT_SEAL_SKILL) )
		return;

	if( pChar->GetState() == CS_DROP_ZONE_DOWN ||
		pChar->GetState() == CS_DIE ||
		pChar->GetState() == CS_VIEW ||
		pChar->GetState() == CS_OBSERVER ||
		pChar->GetState() == CS_LOADING ||
		pChar->GetState() == CS_READY )
		return;

	if( m_OwnerBuffList.empty() && m_OwnerGaugeBuffInfoList.empty() )
		return;

	bool bStartSkill = true;
	if( !m_OwnerBuffList.empty() )
	{
		int iBuffCnt = m_OwnerBuffList.size();
		for( int i=0; i<iBuffCnt; ++i )
		{
			if( pChar->HasBuff( m_OwnerBuffList[i].m_BuffName ) )
				bStartSkill = false;
		}
	}

	if( !m_OwnerGaugeBuffInfoList.empty() )
	{
		int iBuffCnt = m_OwnerGaugeBuffInfoList.size();
		for( int i=0; i<iBuffCnt; ++i )
		{
			int iCnt = m_OwnerGaugeBuffInfoList[i].m_Buff.size();
			for( int j=0; j<iCnt; ++j )
			{
				if( pChar->HasBuff( m_OwnerGaugeBuffInfoList[i].m_Buff[j].m_BuffName ) )
					bStartSkill = false;
			}
		}
	}

	if( bStartSkill )
	{
		OnSkillStart( pChar );
	}
}

void ioPassiveSkill::CheckOwnerGaugeWoundBuff( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	if( m_OwnerGaugeWoundBuffInfoList.empty() )	return;

	if( !m_vStopPassiveSkillID.empty() )
		return;

	float fRate = ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;

	bool bAddBuff = false;
	int iCnt = m_OwnerGaugeWoundBuffInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		GaugeWoundBuffInfo kWoundBuff = m_OwnerGaugeWoundBuffInfoList[i];
		float fMaxRate = kWoundBuff.m_fMaxGauge;
		float fMinRate = kWoundBuff.m_fMinGauge;

		if( fRate > fMinRate && fRate <= fMaxRate )
		{
			if( !bAddBuff )
				ReduceCurPassiveGauge( m_fReducePassiveGaugeOnWound );

			int iBuffCnt = kWoundBuff.m_BuffList.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				if( !pOwner->HasBuff(kWoundBuff.m_BuffList[j].m_BuffName) )
				{
					bAddBuff = true;
					ioBuff *pBuff = pOwner->AddNewBuff( kWoundBuff.m_BuffList[j].m_BuffName,
														pOwner->GetCharName(),
														GetOwnerItem()->GetName(),
														this );

					if( pBuff )
					{
						bool bAirTargetBuff = false;
						bool bTargetBuff = false;
						bool bDownTargetBuff = false;

						if( pOwner->IsFloatingState() || (pOwner->IsApplyDownState(true) && !pOwner->IsApplyDownState(false)) )
							bAirTargetBuff = true;
						else if( !pOwner->IsFloatingState() && !pOwner->IsApplyDownState(true) )
							bTargetBuff = true;
						else if( pOwner->IsApplyDownState(false) )
							bDownTargetBuff = true;

						if( bAirTargetBuff && kWoundBuff.m_dwAirDuration > 0 )
						{
							pBuff->SetBuffDurationAndEndTime( kWoundBuff.m_dwAirDuration );
						}
						else if( bTargetBuff && kWoundBuff.m_dwDuration > 0 )
						{
							pBuff->SetBuffDurationAndEndTime( kWoundBuff.m_dwDuration );
						}
						else if( bDownTargetBuff && kWoundBuff.m_dwDownDuration > 0 )
						{
							pBuff->SetBuffDurationAndEndTime( kWoundBuff.m_dwDownDuration );
						}
					}
				}
			}
		}
		else
		{
			int iBuffCnt = kWoundBuff.m_BuffList.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				if( pOwner->HasBuff( kWoundBuff.m_BuffList[j].m_BuffName ) )
				{
					pOwner->RemoveBuff( kWoundBuff.m_BuffList[j].m_BuffName );
				}
			}
		}
	}
}

//패시브 게이지 퍼센트 확인 jch
float ioPassiveSkill::GetCurPassiveGaugeRate()
{
	return ( m_fCurPassiveGauge / m_fMaxPassiveGauge ) * FLOAT100;
}

bool ioPassiveSkill::CheckUseActiveCount()
{
	return m_bSetUseActiveCnt;
}