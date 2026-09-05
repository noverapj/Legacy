
#include "stdafx.h"

#include "ioActionController.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

ioActionController::ioActionController()
{
	m_dwCurrentTime = 0;
}

ioActionController::~ioActionController()
{
	ActionMap::iterator iCreate;
	for( iCreate = m_ActionMap.begin() ; iCreate != m_ActionMap.end() ; ++iCreate )
	{
		ActionList &kList = iCreate->second;
		kList.m_vMyState.clear();
		kList.m_vTargetState.clear();
		kList.m_vMyBuff.clear();
		kList.m_vTargetBuff.clear();
		kList.m_Action.clear();
		kList.m_RandomSkllList.clear();
	}
	m_CurAction.m_Action.clear();
}

void ioActionController::LoadController( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "ActionController" );
	int iMaxController = rkLoader.LoadInt_e( "MaxController", 0 );
	for(int i = 0;i < iMaxController;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "Action%d", i + 1 );
		rkLoader.SetTitle( szKey );

		ActionList kActionList;
		kActionList.m_fActionRangeMin  = rkLoader.LoadFloat_e( "ActionRangeMin", 150.0f );
		kActionList.m_fActionRangeMax  = rkLoader.LoadFloat_e( "ActionRangeMax", 150.0f );
		kActionList.m_fLimitHPRateMin  = rkLoader.LoadFloat_e( "LimitHPRateMin", FLOAT1 );
		kActionList.m_fLimitHPRateMax  = rkLoader.LoadFloat_e( "LimitHPRateMax", FLOAT1 );
		kActionList.m_fTargetAngle     = rkLoader.LoadFloat_e( "TargetAngle",FLOAT90 );
		kActionList.m_fTargetDirection = rkLoader.LoadFloat_e( "TargetDirection", 0.0f );
		kActionList.m_dwActiveRand     = rkLoader.LoadInt_e( "ActiveRand", 100 );
		kActionList.m_dwActiveDelayTime= rkLoader.LoadInt_e( "ActiveDelayTime", 100 );
		kActionList.m_dwActiveDelayTimeRand = rkLoader.LoadInt_e( "ActiveDelayTimeRand", 1 ); 
		kActionList.m_iActiveCount     = rkLoader.LoadInt_e( "ActiveCount", -1 );
		kActionList.CreateActiveCheckTime();
		
		char szBuf[MAX_PATH] = "";
		rkLoader.LoadString_e( "MyState", "", szBuf, MAX_PATH );
		StringVector vStringVector = ioStringConverter::Split( szBuf, "." );
		StringVector::iterator iter;
		for( iter = vStringVector.begin() ; iter!=vStringVector.end() ; ++iter )
		{
			int iValue = atoi( iter->c_str() );
			if( iValue == -1 )      // -1이면 모두 포함한다.
				break;
			kActionList.m_vMyState.push_back( iValue );
		}
		vStringVector.clear();

		rkLoader.LoadString_e( "TargetState", "", szBuf, MAX_PATH );
		vStringVector = ioStringConverter::Split( szBuf, "." );
		for( iter = vStringVector.begin() ; iter!=vStringVector.end() ; ++iter )
		{
			int iValue = atoi( iter->c_str() );
			if( iValue == -1 )      // -1이면 모두 포함한다.
				break;
			kActionList.m_vTargetState.push_back( iValue );
		}
		vStringVector.clear();

		rkLoader.LoadString_e( "MyBuff", "", szBuf, MAX_PATH );
		if( strlen( szBuf ) > 0 )
		{
			vStringVector = ioStringConverter::Split( szBuf, "." );
			for( iter = vStringVector.begin() ; iter!=vStringVector.end() ; ++iter )
			{
				kActionList.m_vMyBuff.push_back( iter->c_str() );
			}
			vStringVector.clear();
		}		

		rkLoader.LoadString_e( "TargetBuff", "", szBuf, MAX_PATH );
		if( strlen( szBuf ) > 0 )
		{
			vStringVector = ioStringConverter::Split( szBuf, "." );
			for( iter = vStringVector.begin() ; iter!=vStringVector.end() ; ++iter )
			{
				kActionList.m_vTargetBuff.push_back( iter->c_str() );
			}
			vStringVector.clear();
		}

		int j = 0;	// do not use now in ini file
		int iMaxRandSkill = rkLoader.LoadInt_e( "Skill_cnt", 0 );
		for(j = 0;j < iMaxRandSkill;j++)
		{
			ActionSkillData kSkill;

			sprintf_e( szKey, "Skill_%d_rate", j + 1 );
			kSkill.m_dwRandSeed = rkLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "Skill_%d", j + 1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kSkill.m_szSkillName = szBuf;

			kActionList.m_RandomSkilSeed += kSkill.m_dwRandSeed;
			kActionList.m_RandomSkllList.push_back( kSkill );
		}

		int iMaxAction = rkLoader.LoadInt_e( "MaxKey", 0 );
		for(int j = 0;j < iMaxAction;j++)
		{
			ActionData kData;
			
			sprintf_e( szKey, "Key%d_D", j + 1 );
			kData.m_wAttackKey  = rkLoader.LoadInt( szKey, 0 );
			sprintf_e( szKey, "Key%d_S", j + 1 );
			kData.m_wDefenseKey = rkLoader.LoadInt( szKey, 0 );
			sprintf_e( szKey, "Key%d_A", j + 1 );
			kData.m_wJumpKey    = rkLoader.LoadInt( szKey, 0 );
			sprintf_e( szKey, "Key%d_Delay", j + 1 );
			kData.m_dwDelayTime = rkLoader.LoadInt( szKey, 0 );
			sprintf_e( szKey, "Key%d_Chat", j + 1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kData.m_szChatMent = szBuf;

			kActionList.m_Action.push_back( kData );
		}
		m_ActionMap.insert( ActionMap::value_type( i + 1, kActionList ) );
	}
}

void ioActionController::ClearCurrentData()
{
	m_dwCurrentTime = 0;
	m_CurAction.m_Action.clear();
}

void ioActionController::AttachActionData( ActionList &rkList, ioNpcChar *pOwner, ioBaseChar *pTarget )
{
	// Action Copy
	m_CurAction.m_Action.clear();
	m_CurAction.m_szCurrentSkill.Clear();
	for(int i = 0;i < (int)rkList.m_Action.size();i++)
	{
		ActionData &rkData = rkList.m_Action[i];
		m_CurAction.m_Action.push_back( rkData );
	}
	
	// Random Skill Select
	if( !rkList.m_RandomSkllList.empty() && rkList.m_RandomSkilSeed > 0 )
	{
		IORandom	ioRand;
		ioRand.Randomize();

		DWORD dwRandValue = ioRand.Random(rkList.m_RandomSkilSeed);
		DWORD dwCurValue = 0;
		for(i = 0;i < (int)rkList.m_RandomSkllList.size();i++)
		{
			ActionSkillData &rkSkill = rkList.m_RandomSkllList[i];
			if( COMPARE( dwRandValue, dwCurValue, rkSkill.m_dwRandSeed + dwCurValue ) )
			{
				rkList.m_szCurrentSkill = rkSkill.m_szSkillName;
				break;
			}
			dwCurValue += rkSkill.m_dwRandSeed;
		}
	}
	m_CurAction.m_szCurrentSkill = rkList.m_szCurrentSkill;

	m_dwCurrentTime = FRAMEGETTIME();

	OnAttackAction( pOwner, pTarget );	

	if( rkList.m_iActiveCount != -1 )
		rkList.m_iActiveCount = max( 0, rkList.m_iActiveCount - 1 );
}

bool ioActionController::CheckState( DWORDVec &rkState, DWORD dwState )
{
	if( rkState.empty() ) 
		return true;

	DWORDVec::iterator iter = rkState.begin();
	for(;iter != rkState.end();iter++)
	{
		if( *iter == dwState )
			return true;
	}
	return false;
}

bool ioActionController::CheckBuff( ioHashStringVec &rkBuffList, ioBaseChar *pChar )
{
	if( rkBuffList.empty() ) return true;
	if( pChar == NULL ) return true;

	ioHashStringVec::iterator iter = rkBuffList.begin();
	for(;iter != rkBuffList.end();iter++)
	{
		if( pChar->HasBuff( *iter ) )
			return true;
	}
	return false;
}

ioActionController::ActionList &ioActionController::FindActionData( DWORD dwID )
{
	ActionMap::iterator iter = m_ActionMap.find( dwID );
	if( iter != m_ActionMap.end() )
	{
		return iter->second;
	}

	static ActionList kEmptyList;
	return kEmptyList;        //
}

bool ioActionController::Attach( ioNpcChar *pOwner, ioBaseChar *pTarget, float fDistSq, float fTargetAngle )
{
	if( pOwner == NULL ) return false;

	CharState eTargetState = CS_DELAY;
	bool      bBlowDownState = false;
	if( pTarget )        //타겟 상태 분석
	{
		eTargetState = pTarget->GetState();
	}
    
	DWORD dwRandSeed = 0;
	DWORDVec vActionID;
	ActionMap::iterator iCreate = m_ActionMap.begin();	
	for(;iCreate != m_ActionMap.end();++iCreate )
	{
		ActionList &rkList = iCreate->second;
		
		// 발동 딜레이 체크
		//if( rkList.m_dwCurActiveTimer != 0 && FRAMEGETTIME() - rkList.m_dwCurActiveTimer < rkList.m_dwActiveDelayTime ) continue;

		// 발동 딜레이 체크
		if( rkList.m_dwCurActiveTimer == 0 )
			rkList.CreateActiveCheckTime();
		if( FRAMEGETTIME() - rkList.m_dwCurActiveTimer < rkList.m_dwActiveDelayCheckTime ) continue;
		
		// 발동 횟수 제한
		if( rkList.m_iActiveCount == 0 ) continue;

		// 내 상태 체크
		if( !CheckState( rkList.m_vMyState,  pOwner->GetState() ) ) continue;

		// 타겟 상태 체크
		if( !CheckState( rkList.m_vTargetState, eTargetState ) ) continue;

		// 내 버프 체크
		if( !CheckBuff( rkList.m_vMyBuff, pOwner ) ) continue;

		// 타겟 버프 체크
		if( !CheckBuff( rkList.m_vTargetBuff, pTarget ) ) continue;

/*		// 몬스터 던전 버그 사항으로 인한 체크 구문 삭제.	JCLEE 140703		
		// 내 HP 체크
		float fMinHP = pOwner->GetHP().m_fMaxValue * rkList.m_fLimitHPRateMin;
		float fMaxHP = pOwner->GetHP().m_fMaxValue * rkList.m_fLimitHPRateMax;
		if( !COMPARE( pOwner->GetHP().m_fCurValue, fMinHP, fMaxHP ) ) continue;

		// 상대와 나의 각도
		float fHalfCosine = cosf( DEGtoRAD( rkList.m_fTargetAngle ) * FLOAT05 );
		if( fTargetAngle < fHalfCosine ) continue;
*/

		// 상대와 나의 거리 체크
		float fMinRange = rkList.m_fActionRangeMin * rkList.m_fActionRangeMin;
		float fMaxRange = rkList.m_fActionRangeMax * rkList.m_fActionRangeMax;
		if( !COMPARE( fDistSq, fMinRange, fMaxRange ) ) continue;

		// 내가 보는 방향과 상대가 보는 방향 : 180.0f면 서로 마주 보고 있음
		if( rkList.m_fTargetDirection != 0.0f )
		{
			if( pTarget )
			{
				// 타겟이 내 앞에 있는지 확인
				D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT08 ) + ( pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) );
				if( !pOwner->IsNpcTargetCharListByCylinder( vPos, rkList.m_fActionRangeMax * rkList.m_fActionRangeMax, 180.0f, FLOAT500, FLOAT500, pTarget ) ) continue;

				D3DXVECTOR3 vOwnerDir   = pOwner->GetMoveDir();
				D3DXVECTOR3 vCreatorDir = pTarget->GetMoveDir();
				D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );
				D3DXVec3Normalize( &vCreatorDir, &vCreatorDir );
				float fCos = D3DXVec3Dot( &vCreatorDir, &vOwnerDir );
				float fRadian = acos( fCos );
				float fDeg    = RADtoDEG(fRadian);
				if( fDeg < rkList.m_fTargetDirection ) continue;
			}
		}

		//
		dwRandSeed += rkList.m_dwActiveRand;
		vActionID.push_back( iCreate->first );
	}

	if( !vActionID.empty() )
	{
		if( dwRandSeed != 0 )
		{
			IORandom	ioRand;
			ioRand.Randomize();

			DWORD dwRandValue = ioRand.Random(dwRandSeed);
			DWORD dwCurValue = 0;
			for(int i = 0;i < (int)vActionID.size();i++)
			{
				ActionList &rkList = FindActionData( vActionID[i] );
				if( rkList.m_Action.empty() ) continue;

				if( COMPARE( dwRandValue, dwCurValue, dwCurValue + rkList.m_dwActiveRand ) )
				{
					rkList.CreateActiveCheckTime();
					AttachActionData( rkList, pOwner, pTarget );
					return true;
				}
				dwCurValue += rkList.m_dwActiveRand;
			}
		}
		else
		{
			// 완전 랜덤
			int r = rand()%vActionID.size();
			ActionList &rkList = FindActionData( vActionID[r] );
			if( !rkList.m_Action.empty() )
			{
				rkList.CreateActiveCheckTime();
				AttachActionData( rkList, pOwner, pTarget );
				return true;
			}
		}
	}
	return false;
}

bool ioActionController::OnAttackAction( ioNpcChar *pOwner, ioBaseChar *pTarget )
{
	if( pOwner == NULL ) return false;
	if( m_CurAction.m_Action.empty() ) return false;

	ActionData &rkData = m_CurAction.m_Action[0];
	
	pOwner->SetActionKey( rkData.m_wDefenseKey, rkData.m_wJumpKey, rkData.m_wAttackKey );
	pOwner->ChangeItemSkill( ES_WEAPON, m_CurAction.m_szCurrentSkill );

	if( pTarget )
	{
		if( pOwner->GetState() == CS_DELAY || pOwner->GetState() == CS_RUN || pOwner->GetState() == CS_DASH ||
			pOwner->GetState() == CS_EXTEND_DASH || pOwner->GetState() == CS_JUMP )
		{
			pOwner->SetAutoTargetExplicit( pTarget->GetCharName(), pOwner->GetMoveDir(), 200, ATT_NORMAL, false, false );
		}
	}

	if( FRAMEGETTIME() - m_dwCurrentTime > rkData.m_dwDelayTime )
	{
		m_dwCurrentTime = FRAMEGETTIME();
		if( !rkData.m_szChatMent.IsEmpty() )
		{
			//g_ChatMgr.SetChatMsg( pOwner->GetTeam(), pOwner->GetViewName().c_str(), rkData.m_szChatMent.c_str(), false );
			pOwner->SetChatBubble( pOwner->GetViewName().c_str(), rkData.m_szChatMent.c_str(), false );
		}
		m_CurAction.m_Action.erase( m_CurAction.m_Action.begin() );
	}
	return true;
}

bool ioActionController::IsAction()
{
	if( m_CurAction.m_Action.empty() )
		return false;
	return true;
}