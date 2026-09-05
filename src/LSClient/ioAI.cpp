
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"
#include "ioAI.h"
#include "ioTowerDefMode.h"

#define PATH_FIND_TIME		200

void ioAI::TargetData::Init()
{
	m_TargetType = TARGET_NONE;
	m_dwCheckTargetType = (DWORD) TARGET_NONE;
	m_szTargetName.Clear();
	m_dwCheckTargetName = ioHashString::CalcHashCode("");
	m_iTargetLevel = 0;
	m_vTargetPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

void ioAI::TargetData::SetTargetType( TargetType eTargetType )
{
	if( m_dwCheckTargetType != (DWORD)m_TargetType )
	{
		// error
		if( REALGETTIME() - m_dwAbuseSendTime > MAX_ABUSE_SEND_TIME )
		{
			m_dwAbuseSendTime = REALGETTIME();

			SP2Packet kPacket( CTPK_ABUSE_QUIZ_START );
			kPacket << ioAbuseManager::AT_DAMAGE;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	m_TargetType = eTargetType;
	m_dwCheckTargetType = (DWORD) eTargetType;
}

void ioAI::TargetData::SetTargetName( const ioHashString &rsTargetName )
{
	if( m_dwCheckTargetName != ioHashString::CalcHashCode( m_szTargetName.c_str() ) )
	{
		// error
		if( REALGETTIME() - m_dwAbuseSendTime > MAX_ABUSE_SEND_TIME )
		{
			m_dwAbuseSendTime = REALGETTIME();

			SP2Packet kPacket( CTPK_ABUSE_QUIZ_START );
			kPacket << ioAbuseManager::AT_DAMAGE;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	m_szTargetName      = rsTargetName;
	m_dwCheckTargetName = ioHashString::CalcHashCode( rsTargetName.c_str() );
}

//////////////////////////////////////////////////////////////////////////////////////////
ioAI::ioAI( ioNpcChar *pCreator, ioPlayStage *pPlayStage ) 
	: m_pCreator( pCreator ), m_pPlayStage( pPlayStage )
{	
	m_fSearchTargetTime = FLOAT10;
	m_fSearchRangeMin   = FLOAT100;
	m_fSearchRangeMax   = 10000.f;
	m_fSearchRangeMax   = 1000000.0f;
	m_fSearchRangeCurrent = m_fSearchRangeMax;
	m_fSearchRangeUpByCylinder = m_fSearchRangeDownByCylinder = FLOAT500;
	m_fCurSearchTime    = 0.0f;
	m_fJumpCheckRange   = 150.0f;
	m_fTargetSearchAngle= SIGHT_360;
	m_fObjectSearchAngle= 45.0f;
	m_fEscapeRange      = 0.0f;
	m_bCheckDownState   = false;
	m_fCurTargetFormTime= 0.0f;
	m_bRoaming = true;
	m_fCurTargetFixingTime = 0.0f;
	m_fTargetFixingTime = 0.0f;
	m_bGlobalTarget = false;
	m_fGlobalTargetSearchTimeMin = 0.0f;
	m_fGlobalTargetSearchTimeMax = 0.0f;
	m_fGlobalTargetSearchTimeRand = 0.0f;
	m_fGlobalTargetFixingTimeMin = 0.0f;
	m_fGlobalTargetFixingTimeMax = 0.0f;
	m_bGlobalTargetRand = false;
	m_fCurGlobalTargetSearchTime = 0.0f;
	m_iOffensiveLevel = 0;
	m_dwSearchType = SEARCH_RANGE;
}

ioAI::~ioAI()
{
	m_TargetSearchData.clear();
	m_vTargetIgnoreState.clear();
	m_vTargetIgnoreBuff.clear();
	m_WayPointList.clear();
}

AIType ioAI::GetAIType() const
{
	return AI_NORMAL;
}

void ioAI::LoadAI( ioINILoader &rkLoader )
{
	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	rkLoader.SetTitle_e( "Info" );	
	m_bCheckDownState            = rkLoader.LoadBool_e( "CheckDownState", 1 );
	m_fSearchTargetTime          = rkLoader.LoadFloat_e( "SearchTargetTime", FLOAT10 );
	m_fSearchRangeMin            = rkLoader.LoadFloat_e( "SearchRangeMin", FLOAT100 );
	m_fSearchRangeMax            = rkLoader.LoadFloat_e( "SearchRangeMax", 1000000.0f );
	
	m_fSearchRangeUpByCylinder   = rkLoader.LoadFloat_e( "SearchRangeUpByCylinder", FLOAT500 );
	m_fSearchRangeDownByCylinder = rkLoader.LoadFloat_e( "SearchRangeDownByCylinder", FLOAT500 );

	m_fSearchRangeCurrent = rkLoader.LoadFloat_e( "SearchRangeCurrent", m_fSearchRangeMax );
	m_fJumpCheckRange   = rkLoader.LoadFloat_e( "JumpCheckRange", 150.0f );
	m_fTargetSearchAngle= rkLoader.LoadFloat_e( "TargetSearchAngle", SIGHT_360 );
	m_fObjectSearchAngle= rkLoader.LoadFloat_e( "ObjectSearchAngle", 45.0f );
	m_fEscapeRange      = rkLoader.LoadFloat_e( "EscapeRange", 0.0f );
	m_iOffensiveLevel   = rkLoader.LoadInt_e( "OffensiveLevel", 0 );
	m_bRoaming			= rkLoader.LoadBool_e( "Roaming", true );

	// 던전 모드로 AI 확장
	
	
	// 타겟 집중 기능
	m_vGlobalTargetBuff.clear();
	m_bGlobalTarget     = rkLoader.LoadBool_e( "global_target", false );
	m_fGlobalTargetSearchTimeMin = rkLoader.LoadFloat_e( "global_target_search_time_min", 0.0f );
	m_fGlobalTargetSearchTimeMax = rkLoader.LoadFloat_e( "global_target_search_time_max", 0.0f );
	m_fGlobalTargetFixingTimeMin = rkLoader.LoadFloat_e( "global_target_time_min", 0.0f );
	m_fGlobalTargetFixingTimeMax = rkLoader.LoadFloat_e( "global_target_time_max", 0.0f );
	m_bGlobalTargetRand = rkLoader.LoadBool_e( "global_target_rand", false );
	int iGlobalTargetBuffCnt = rkLoader.LoadInt_e( "global_target_buff_count", 0 );
	for(i = 0;i < iGlobalTargetBuffCnt;i++)	
	{		
		sprintf_e( szKey, "global_target_buff_%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vGlobalTargetBuff.push_back( szBuf );
	}

	IORandom	ioRand;
	ioRand.Randomize();

	m_fGlobalTargetSearchTimeRand = m_fGlobalTargetSearchTimeMin + ioRand.Random((int)GetGlobalTargetSearchMinMaxGapTime());
	m_fCurGlobalTargetSearchTime = FRAMEGETTIME();

	// 이동 경로
	m_WayPointList.clear();
	rkLoader.SetTitle_e( "WayPoint" );
	int iMaxPoint = rkLoader.LoadInt_e( "max_point", 0 );
	for(i = 0;i < iMaxPoint;i++)
	{
		D3DXVECTOR3 kPoint;
		sprintf_e( szKey, "point%d_x", i + 1 );
		kPoint.x = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "point%d_y", i + 1 );
		kPoint.y = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "point%d_z", i + 1 );
		kPoint.z = rkLoader.LoadFloat( szKey, 0.0f );
		m_WayPointList.push_back( kPoint );
	}

	// 타겟 제외 정보
	m_vTargetIgnoreState.clear();
	m_vTargetIgnoreBuff.clear();
	rkLoader.SetTitle_e( "Ignore" );
	
	// 상태
	rkLoader.LoadString_e( "ignore_state", "", szBuf, MAX_PATH );
	if( strlen( szBuf ) > 0 )
	{
		StringVector vStringVector = ioStringConverter::Split( szBuf, "." );
		StringVector::iterator iter;
		for( iter = vStringVector.begin() ; iter!=vStringVector.end() ; ++iter )
		{
			int iValue = atoi( iter->c_str() );
			if( iValue == -1 )      // -1이면 모두 포함한다.
				break;
			m_vTargetIgnoreState.push_back( iValue );
		}
	}

	// 버프
	int iMaxIgnoreBuff = rkLoader.LoadInt_e( "max_ignore_buff", 0 );
	for(i = 0;i < iMaxIgnoreBuff;i++)
	{
		sprintf_e( szKey, "ignore_buff_%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vTargetIgnoreBuff.push_back( szBuf );
	}

	// 타겟 검색 방식
	rkLoader.SetTitle_e( "TargetSearch" );
	int iMaxSearch = rkLoader.LoadInt_e( "max_search", 0 );
	for(i = 0;i < iMaxSearch;i++)
	{
		TargetSearchData kSearch;
		sprintf_e( szKey, "hp_rate_%d", i + 1 );
		kSearch.m_fHPRate = rkLoader.LoadFloat( szKey, 0.0f );

		sprintf_e( szKey, "search_type_%d", i + 1 );
		kSearch.m_dwSearchType = rkLoader.LoadInt( szKey, 0 );

		if( i == 0 )
		{
			m_dwSearchType = kSearch.m_dwSearchType;
		}
		m_TargetSearchData.push_back( kSearch );
	}

	// 몬스터 정보
	m_pCreator->LoadMonsterAIInfo( rkLoader );
	
	// 공격 관련 세팅
	m_ActionController.LoadController( rkLoader );

	// 이동 관련 세팅
	m_MoveController.LoadController( rkLoader );

	// 점프 관련 세팅
	m_JumpController.LoadController( rkLoader );

	// 방어 관련 세팅
	m_DefenseController.LoadController( rkLoader );
}

float ioAI::GetAngle( D3DXVECTOR3 vPos1, D3DXVECTOR3 vPos2 )
{
	D3DXVECTOR3 vSightDir = vPos1 - vPos2;
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	return fCurYaw;
}

float ioAI::GetTargetAngle( D3DXVECTOR3 vTargetPos )
{
	if( m_pCreator->CheckMagicCircle() )
		return GetAngle( vTargetPos, m_pCreator->GetMagicCirclePos() );
	else if( m_pCreator->CheckAreaWeaponControl() )
		return GetAngle( vTargetPos, m_pCreator->GetAreaWeaponPos() );

	return GetAngle( vTargetPos, m_pCreator->GetWorldPosition() );
}

float ioAI::GetTargetRange( D3DXVECTOR3 vPos )
{
	if( m_pCreator->CheckMagicCircle() )
	{
		D3DXVECTOR3 vDiff = vPos - m_pCreator->GetMagicCirclePos();
		return D3DXVec3LengthSq( &vDiff );		
	}
	else if( m_pCreator->CheckAreaWeaponControl() )
	{
		D3DXVECTOR3 vDiff = vPos - m_pCreator->GetAreaWeaponPos();
		return D3DXVec3LengthSq( &vDiff );		
	}

	D3DXVECTOR3 vDiff = vPos - m_pCreator->GetWorldPosition();
	return D3DXVec3LengthSq( &vDiff );		
}

float ioAI::GetTargetRange( D3DXVECTOR3 vPos1, D3DXVECTOR3 vPos2 )
{
	D3DXVECTOR3 vDiff = vPos1 - vPos2;
	return D3DXVec3LengthSq( &vDiff );		
}

D3DXVECTOR3 ioAI::GetTargetLeftPos( const D3DXQUATERNION &qtTargetRotate, const D3DXVECTOR3 &vTargetPos, float fMoveRange )
{
	// 좌측 이동 가능 검색
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3	axisY( 0.0f, -FLOAT1, 0.0f );
	D3DXQuaternionRotationAxis( &qtRotate, &axisY, FLOAT1 );		
	qtRotate = qtTargetRotate * qtRotate;
	D3DXVECTOR3 vDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );	
	D3DXVECTOR3 vReturnPos = vTargetPos + ( vDir * fMoveRange );
	return vReturnPos;
}

D3DXVECTOR3 ioAI::GetTargetRightPos( const D3DXQUATERNION &qtTargetRotate, const D3DXVECTOR3 &vTargetPos, float fMoveRange )
{
	// 우측 이동 가능 검색
	D3DXQUATERNION qtRotate;
	D3DXVECTOR3	axisY( 0.0f, FLOAT1, 0.0f );
	D3DXQuaternionRotationAxis( &qtRotate, &axisY, FLOAT1 );		
	qtRotate = qtTargetRotate * qtRotate;
	D3DXVECTOR3 vDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );	
	D3DXVECTOR3 vReturnPos = vTargetPos + ( vDir * fMoveRange );
	return vReturnPos;
}

D3DXVECTOR3 ioAI::GetRoamingPoint()
{
	int iXRange = 1100;
	int iZRange = 870;

	IORandom	ioRand;
	ioRand.Randomize();

	float fXPos = -(iXRange) + ioRand.Random(( iXRange * 2 ));

	ioRand.Randomize();
	float fZPos = -(iZRange) + ioRand.Random(( iZRange * 2 ));
	D3DXVECTOR3 vReturnPos( fXPos, 0.0f, fZPos );
	vReturnPos.y = m_pPlayStage->GetMapHeight( vReturnPos.x, vReturnPos.z );	
	return vReturnPos;
}

bool ioAI::CheckLeftRightPos( const D3DXQUATERNION &qtTargetRotate, const D3DXVECTOR3 &vTargetPos, float fMoveRange, 
							  D3DXVECTOR3 &vPrevTargetPos, D3DXVECTOR3 &vReturnPos )
{
	D3DXVECTOR3 vLeftPos  = GetTargetLeftPos( qtTargetRotate, vTargetPos, fMoveRange );
	D3DXVECTOR3 vRightPos = GetTargetRightPos( qtTargetRotate, vTargetPos, fMoveRange );
	float fLeftMapHeight  = m_pPlayStage->GetMapHeight( vLeftPos.x, vLeftPos.z );
	float fRightMapHeight = m_pPlayStage->GetMapHeight( vRightPos.x, vRightPos.z );
	
	if( fLeftMapHeight > 0.0f && fRightMapHeight > 0.0f )      //양 방향 모두 드랍존이 아니면
	{
		// 좌 - 우측중 가까운 곳으로 이동
		float fLeftRange = GetTargetRange( vPrevTargetPos, vLeftPos );
		float fRightRange= GetTargetRange( vPrevTargetPos, vRightPos );
		if( fLeftRange > fRightRange )
			vReturnPos = vRightPos;
		else
			vReturnPos = vLeftPos;
		return true;

	}
	else if( fLeftMapHeight > 0.0f )    //좌측만 이동가능
	{
		vReturnPos = vLeftPos;
		return true;
	}
	else if( fRightMapHeight > 0.0f )   //우측만 이동가능
	{
		vReturnPos = vRightPos;
		return true;
	}

	return false;
}

void ioAI::SetFixingTarget( const ioHashString &rkTargetName, float fFixingTime )
{
	m_szFixingName = rkTargetName;

	// 고정 시간이 0 이상이면 새로 시작하는 타겟 집중이다.
	if( fFixingTime > 0.0f )
	{
		m_fTargetFixingTime = fFixingTime;
		m_fCurTargetFixingTime = FRAMEGETTIME();

		// 버프 적용
		int iGlobalTargetBuffCnt = m_vGlobalTargetBuff.size();
		for(int i = 0;i < iGlobalTargetBuffCnt;i++)
		{
			if( m_vGlobalTargetBuff[i].IsEmpty() ) continue;

			m_pCreator->AddNewBuff( m_vGlobalTargetBuff[i], "", "", NULL );			
		}
	}
}

bool ioAI::IsTargetIgnoreState( ioBaseChar *pChar, bool bPassBuffCheck /* = true  */ )
{
	if( !pChar )
		return true;

	if( m_pCreator->GetCharName() == pChar->GetCharName() )
		return true;

	if( m_pCreator->GetTeam() == pChar->GetTeam() )
		return true;

	if( pChar->IsSystemState() )
		return true;
		
	if( !pChar->IsEnableTarget() ) 
		return true;

	if( pChar->IsPrisonerMode() )
		return true;

	if( !m_bCheckDownState && pChar->IsApplyDownState( false ) )
		return true;

	int i, iCnt;

	// 지정된 상태 패스
	iCnt = 	m_vTargetIgnoreState.size();
	for(i = 0;i < iCnt;i++)
	{
		if( pChar->GetState() == m_vTargetIgnoreState[i] )
			return true;
	}

	// 지정된 버프 패스
	if( bPassBuffCheck )
	{
		iCnt = m_vTargetIgnoreBuff.size();
		for(i = 0;i < iCnt;i++)
		{
			if( pChar->HasBuff( m_vTargetIgnoreBuff[i] ) )
				return true;
		}
	}
	return false;
}

void ioAI::CheckRandomTarget( ioHashString &rkReturnName )
{
	ioHashStringVec vRandTarget;
	const BaseCharList &rkTargetList = m_pPlayStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);
		if( IsTargetIgnoreState( pChar ) ) continue;

		vRandTarget.push_back( pChar->GetCharName() );
	}

	if( !vRandTarget.empty() )
	{
		int r = rand()%(int)vRandTarget.size();
		rkReturnName = vRandTarget[r];
		vRandTarget.clear();
	}
}

void ioAI::CheckReSearchFixingTarget()
{
	ioHashString szFixingName2 = m_szFixingName;
	// 타겟 집중 중인 유저가 공격 가능한지 확인하고 공격이 안되는 상태라면 타겟을 바꾼다.
	ioBaseChar *pFixingTarget = m_pCreator->GetBaseChar( szFixingName2 );
	if( IsTargetIgnoreState( pFixingTarget ) )
	{
		CheckRandomTarget( szFixingName2 );
		if( szFixingName2 != m_szFixingName )
		{
			// 유저들에게 전송
			SP2Packet kPacket( CUPK_MONSTER_GLOBAL_TARGET );
			kPacket << szFixingName2 << 0.0f;
			P2PNetwork::SendToAllPlayingUser( kPacket );

			// 광역 타겟 지정
			m_pPlayStage->SetMonsterTargetFixing( szFixingName2, 0.0f );
		}
	}
}

void ioAI::CheckGlobalTarget()
{
	if( !m_bGlobalTarget ) return;
	if( !m_szFixingName.IsEmpty() )	
	{
		CheckReSearchFixingTarget();
		return;
	}

	if( FRAMEGETTIME() - m_fCurGlobalTargetSearchTime < m_fGlobalTargetSearchTimeRand ) return;
	//
	// 데미지에 따른 우선 순위
	ioHashString szGlobalTarget;
	if( szGlobalTarget.IsEmpty() && !m_bGlobalTargetRand )
	{
		m_pCreator->GetDamagedTopChar( szGlobalTarget );
	}

	ioBaseChar *pGlobalTarget = m_pCreator->GetBaseChar( szGlobalTarget );
	if( IsTargetIgnoreState( pGlobalTarget ) )
	{
		CheckRandomTarget( szGlobalTarget );
	}		

	if( !szGlobalTarget.IsEmpty() && m_pPlayStage )
	{
		IORandom	ioRand;
		ioRand.Randomize();

		// 시간 초기화 -
		m_fCurGlobalTargetSearchTime  = FRAMEGETTIME();
		m_fGlobalTargetSearchTimeRand = m_fGlobalTargetSearchTimeMin + ioRand.Random((int)GetGlobalTargetSearchMinMaxGapTime());

		// 광역 타겟 전송
		ioRand.Randomize();
		float fGlobalTargetFixingTimeRand = m_fGlobalTargetFixingTimeMin + ioRand.Random((int)GetGlobalTargetFixingMinMaxGapTime());	
		SP2Packet kPacket( CUPK_MONSTER_GLOBAL_TARGET );
		kPacket << szGlobalTarget << fGlobalTargetFixingTimeRand;
		P2PNetwork::SendToAllPlayingUser( kPacket );

		// 광역 타겟 지정
		m_pPlayStage->SetMonsterTargetFixing( szGlobalTarget, fGlobalTargetFixingTimeRand );
	}
}

void ioAI::CheckFixingTarget()
{
	// 고정 타겟 
	if( m_szFixingName.IsEmpty() ) return;
	if( m_fCurTargetFixingTime == 0.0f ) return;
	if( FRAMEGETTIME() - m_fCurTargetFixingTime < m_fTargetFixingTime ) return;

	m_szFixingName.Clear();
	m_fCurTargetFixingTime = 0.0f;
}

void ioAI::CheckRandomAggroTarget()
{
	// 타겟 검색 방식 결정
	if( !m_szFixingName.IsEmpty() ) return;

	if( m_TargetSearchData.empty() )
	{
		if( FRAMEGETTIME() - m_fCurTargetFormTime > 5000 )
		{
			m_fCurTargetFormTime = FRAMEGETTIME();
			m_dwSearchType = rand()%MAX_SEARCH_TYPE;
		}
	}
	else
	{
		if( FRAMEGETTIME() - m_fCurTargetFormTime > 1000 )
		{
			m_fCurTargetFormTime = FRAMEGETTIME();
	
			vTargetSearchData::iterator iter = m_TargetSearchData.begin();
			for(;iter != m_TargetSearchData.end();iter++)
			{
				TargetSearchData &rkSearchData = *iter;
				if( m_pCreator->GetCurHPRate() >= rkSearchData.m_fHPRate )
				{
					if( rkSearchData.m_dwSearchType == SEARCH_RANDOM )
					{
						m_dwSearchType = rand()%MAX_SEARCH_TYPE;
					}
					else
					{
						m_dwSearchType = rkSearchData.m_dwSearchType;
					}
					break;
				}
			}
		}
	}
}

void ioAI::CheckWayPoint()
{
	if( m_WayPointList.empty() ) return;

	float fSearchRange = 0.0f;
	int iMaxPoint = (int)m_WayPointList.size();
	for(int i = 0;i < iMaxPoint;i++)
	{
		D3DXVECTOR3 kVector = m_WayPointList[i];
		kVector.y = m_pCreator->GetWorldPosition().y;
		float fDistSq = GetTargetRange( kVector );
		if( fSearchRange == 0.0f || fDistSq < fSearchRange )
		{
			fSearchRange = fDistSq;
			m_TargetData.SetTargetName( m_pCreator->GetCharName() );
			m_TargetData.SetTargetPos( kVector );
			m_TargetData.SetTargetType( TARGET_WAYPOINT );
		}
	}
}

bool ioAI::IsRoaming()
{
	if( m_TargetData.GetTargetType() != TARGET_ROAMING )
		return false;

	if( IsOffensive() )
	{
		// 타겟 가능한 유저가 있는지 확인한다.
		float fSearchRange = m_fSearchRangeCurrent * m_fSearchRangeCurrent;
		const BaseCharList &rkTargetList = m_pPlayStage->GetBaseCharList();
		BaseCharList::const_iterator iter;
		for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
		{
			ioBaseChar *pChar = (*iter);
			if( IsTargetIgnoreState( pChar ) ) continue;

			float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
			if( fDistSq < fSearchRange )	// Enemy
				return false;
		}	
	}

	// 타겟이 없으면 이동 위치를 바꿔야하는지 확인한다.
	{
		float fCheckRange = 10 * 10;
		float fDistSq = GetTargetRange( m_TargetData.GetTargetPos() );
		if( fDistSq < fCheckRange )
		{
			m_TargetData.SetTargetPos( GetRoamingPoint() );
		}
	}
	return true;
}

bool ioAI::IsOffensive()
{
	if( m_pCreator->GetLevel() >= m_iOffensiveLevel ) return true;
	if(	m_pCreator->GetHP().m_fCurValue < m_pCreator->GetHP().m_fMaxValue ) return true;

	return false;
}

void ioAI::CheckTargetChar()
{
	float fSearchTime = FRAMEGETTIME() - m_fSearchTargetTime;
	if( fSearchTime < m_fCurSearchTime )
	{
		return;
	}
    m_fCurSearchTime = FRAMEGETTIME();	
	
	m_TargetData.Init();

	CheckGlobalTarget();
	CheckFixingTarget();
	CheckRandomAggroTarget();
	CheckWayPoint(); 

	float fSearchRange = m_fSearchRangeCurrent * m_fSearchRangeCurrent;	
	if( m_TargetData.GetTargetType() != TARGET_WAYPOINT )
	{
		// 데미지에 따른 우선 순위
		ioHashString szFirstTarget = m_szFixingName;		
		if( szFirstTarget.IsEmpty() )
		{
			switch( m_dwSearchType )
			{
			case SEARCH_TOP_DAMAGE:
				m_pCreator->GetDamagedTopChar( szFirstTarget );
				break;
			case SEARCH_LOWEST_DAMAGE:
				m_pCreator->GetDamagedLowestChar( szFirstTarget );
				break;
			case SEARCH_MODE_CONDITION:
				m_pCreator->GetModeCondition( szFirstTarget );
				break;			
			}
		}

		ioBaseChar *pFirstTarget = m_pCreator->GetBaseChar( szFirstTarget );
		if( !IsTargetIgnoreState( pFirstTarget ) )
		{
			fSearchRange = GetTargetRange( pFirstTarget->GetWorldPosition() );
			m_TargetData.SetTargetName( pFirstTarget->GetCharName() );
			m_TargetData.SetTargetPos( pFirstTarget->GetWorldPosition() );
			m_TargetData.SetTargetLevel( pFirstTarget->GetLevel() );
			m_TargetData.SetTargetType( TARGET_BASECHAR );
		}
		else
		{
			BaseCharList vTargetList;
			BaseCharList::const_iterator iter;
			if( IsOffensive() )
			{				
				D3DXVECTOR3 vPos = m_pCreator->GetMidPositionByRate( FLOAT08 ) + ( m_pCreator->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) );
				m_pCreator->GetNpcTargetCharListByCylinder( vPos, m_fSearchRangeMax, m_fTargetSearchAngle, m_fSearchRangeUpByCylinder, m_fSearchRangeDownByCylinder, vTargetList );				
				for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
				{
					ioBaseChar *pChar = (*iter);

// 					ioNpcChar *pNpcChar = ToNpcChar(pChar);
// 					if( pNpcChar)
// 						continue;

					if( IsTargetIgnoreState( pChar ) ) continue;

					float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
					if( fDistSq < fSearchRange )	// Enemy
					{
						fSearchRange = fDistSq;
						m_TargetData.SetTargetName( pChar->GetCharName() );
						m_TargetData.SetTargetPos( pChar->GetWorldPosition() );
						m_TargetData.SetTargetLevel( pChar->GetLevel() );
						m_TargetData.SetTargetType( TARGET_BASECHAR );					
					}
				}
				vTargetList.clear();
			}

			// 타겟이 없으면 로밍
			if( m_TargetData.GetTargetType() == TARGET_NONE )
			{
				if( m_pCreator->IsDeathMonster() )
				{
					const BaseCharList &rkTargetList = m_pPlayStage->GetBaseCharList();
					for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
					{
						ioBaseChar *pChar = (*iter);
						if( IsTargetIgnoreState( pChar, false ) ) continue;

						float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
						if( fDistSq < fSearchRange )	// Enemy
						{
							fSearchRange = fDistSq;
							m_TargetData.SetTargetName( pChar->GetCharName() );
							m_TargetData.SetTargetPos( pChar->GetWorldPosition() );
							m_TargetData.SetTargetType( TARGET_BASECHAR );
						}
					}
				}
				else if( m_bRoaming )
				{
					fSearchRange = 200.0f * 200.0f;
					m_TargetData.SetTargetPos( GetRoamingPoint() );
					m_TargetData.SetTargetType( TARGET_ROAMING );
					m_TargetData.SetTargetName( m_pCreator->GetCharName() );
				}
			}
		}		
	}

	if( !m_TargetData.GetTargetName().IsEmpty() )
	{
		// 대상이 계급 높을 때 도망 
		if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
		{
			if( m_TargetData.GetTargetLevel() >= m_pCreator->GetLevel() )
			{
				float fDistSq = GetTargetRange( m_TargetData.GetTargetPos() );
				if( fDistSq < m_fEscapeRange * m_fEscapeRange )
				{
					float fAngle   = GetAngle( m_TargetData.GetTargetPos(), m_pCreator->GetWorldPosition() ) + 180.0f;
					fAngle = ioMath::ArrangeHead( fAngle );
					D3DXQUATERNION qtAngle;
					D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

					D3DXVECTOR3 kBackPos = m_TargetData.GetTargetPos();
					D3DXVECTOR3 vCurDir  = qtAngle * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					m_TargetData.SetTargetPos( m_pCreator->GetWorldPosition() + 200.0f * vCurDir );
					m_TargetData.SetTargetType( TARGET_ESCAPE );
					m_TargetData.SetTargetName( m_pCreator->GetCharName() );
					fSearchRange = 200.0f * 200.0f;
				}
				else if( m_fEscapeRange > 0.0f )
				{
					// 거리가 멀면 그냥 중지
					m_TargetData.Init();
				}
			}
		}
	}

	// 타겟 선택 후 처리
	if( !m_TargetData.GetTargetName().IsEmpty() )
	{
		// 웨이 포인트 사이에 오브젝트가 있는지 검색
		if( m_TargetData.GetTargetType() == TARGET_WAYPOINT )
		{
			const BaseCharList &rkTargetList = m_pPlayStage->GetBaseCharList();
			for( BaseCharList::const_iterator iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
			{				
				ioBaseChar *pChar = (*iter);
				if( IsTargetIgnoreState( pChar ) ) continue;

				float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
				if( fDistSq < fSearchRange )	// Enemy
				{
					float fAngle1   = GetAngle( m_TargetData.GetTargetPos(), m_pCreator->GetWorldPosition() );
					float fAngle2   = GetAngle( pChar->GetWorldPosition(), m_pCreator->GetWorldPosition() );
					float fGapAngle = fabs( ioMath::AngleGap( fAngle1, fAngle2 ) );
					if( fGapAngle <= m_fObjectSearchAngle )
					{
						fSearchRange = fDistSq;
						m_TargetData.SetTargetName( pChar->GetCharName() );
						m_TargetData.SetTargetPos( pChar->GetWorldPosition() );
						m_TargetData.SetTargetType( TARGET_BASECHAR );
					}					
				}
			}
		}

		// 타겟 사이에 오브젝트가 있는지 검색
		{
			const GameEntityList &rkObjectList = m_pPlayStage->GetGameEntityList();
			GameEntityList::const_iterator iter;
			for( iter = rkObjectList.begin() ; iter!=rkObjectList.end() ; ++iter )
			{
				ioPushStruct *pPush = ToPushStruct( *iter );
				if( !pPush ) continue;
				if( !pPush->GetGroup()->IsVisible() ) continue;		
				if( pPush->IsEnableClick() ) continue;

				float fDistSq = GetTargetRange( pPush->GetWorldPosition() );
				if( fDistSq < fSearchRange )	
				{
					float fAngle1   = GetAngle( m_TargetData.GetTargetPos(), m_pCreator->GetWorldPosition() );
					float fAngle2   = GetAngle( pPush->GetWorldPosition(), m_pCreator->GetWorldPosition() );
					float fGapAngle = fabs( ioMath::AngleGap( fAngle1, fAngle2 ) );
					if( fGapAngle <= m_fObjectSearchAngle )
					{
						m_TargetData.SetTargetPos( pPush->GetWorldPosition() );
						m_TargetData.SetTargetType( TARGET_OBJECT );
						break;
					}
				}
			}
		}

		// 타겟 사이에 몬스터가 있는지 검색
		if( fSearchRange > FLOAT1 ) 
		{
			const BaseCharList &rkTargetList = m_pPlayStage->GetBaseCharList();
			BaseCharList::const_iterator iter;
			for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
			{
				ioBaseChar *pChar = (*iter);
			
				if( !pChar ) continue;
				if( !ToNpcChar( pChar ) ) continue;
				if( m_pCreator->GetCharName() == pChar->GetCharName() ) continue;
				if( pChar->GetState() == CS_READY ||
					pChar->GetState() == CS_VIEW  ||
					pChar->GetState() == CS_DIE   ||
					pChar->GetState() == CS_OBSERVER ||
					pChar->GetState() == CS_LOADING ||
					pChar->GetState() == CS_DROP_ZONE_DOWN ||
					pChar->GetState() == CS_FALL || 
					pChar->GetState() == CS_BLOW_WOUNDED ||
					pChar->GetState() == CS_BOUND_BLOW_WOUNDED ||
					pChar->GetState() == CS_BLOW_EXTEND_WOUND )
				{
					continue;
				}

				float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
				if( fDistSq < fSearchRange )	
				{
					float fAngle1   = GetAngle( m_TargetData.GetTargetPos(), m_pCreator->GetWorldPosition() );
					float fAngle2   = GetAngle( pChar->GetWorldPosition(), m_pCreator->GetWorldPosition() );
					float fGapAngle = fabs( ioMath::AngleGap( fAngle1, fAngle2 ) );
					float fCheckAngle = m_fObjectSearchAngle * ( FLOAT1 - ( fDistSq / fSearchRange ) );
					if( fGapAngle <= fCheckAngle )
					{
						D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;
						if( !CheckLeftRightPos( pChar->GetWorldOrientation(), pChar->GetWorldPosition(), FLOAT100, m_TargetData.GetTargetPos(), vTargetPos ) )
							continue;

						m_TargetData.SetTargetPos( vTargetPos );
						m_TargetData.SetTargetType( TARGET_POSITION );
						break;
					}
				}
			}
		}
	}

	if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
		m_fSearchRangeCurrent = m_fSearchRangeMax;
}

bool ioAI::OnClearAction()
{
	m_TargetData.Init();
	m_pCreator->StopAutoTargetTracking();
	m_pCreator->SetDirectionKey( -1, KEY_NONE );
	m_pCreator->SetActionKey( KEY_NONE, KEY_NONE, KEY_NONE );

	m_ActionController.ClearCurrentData();
	m_MoveController.ClearCurrentData();	
	m_JumpController.ClearCurrentData();
	m_DefenseController.ClearCurrentData();
	return true;
}

bool ioAI::IsActionToMove()
{
	if( m_pCreator->GetState() == CS_USING_SKILL )
		return m_pCreator->IsAttackMoveEnable();
	if( m_pCreator->GetState() == CS_ATTACK )
		return m_pCreator->IsAttackMoveEnable();
	return false;
}

bool ioAI::OnMove()
{
	if( m_ActionController.IsAction() ) 
	{
		if( !IsActionToMove() )
			return false;
	}

	if( m_MoveController.IsMove() && IsTargetBaseChar() )
	{
		if( !IsActionToMove() )
		{
			// 이미 이동중이라면 정지를 탐색 
			float fRangeMin = m_fSearchRangeMin * m_fSearchRangeMin;
			if( GetTargetRange( m_TargetData.GetTargetPos() ) < fRangeMin )
			{
				return false;
			}
			return true;
		}
	}

	switch( m_pCreator->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
	case CS_JUMP:
		{
			// 방향 탐색
			float fDistSq = GetTargetRange( m_TargetData.GetTargetPos() );		
			float fHalfCosine = cosf( DEGtoRAD(FLOAT90 ) * FLOAT05 );
			float fTargetAngle = GetTargetAngle( m_TargetData.GetTargetPos() );
			if( fTargetAngle >= fHalfCosine )
			{
				// 거리 탐색
				float fRangeMin = m_fSearchRangeMin * m_fSearchRangeMin;
				if( fDistSq < fRangeMin )
				{
					return false;
				}
			}
			return m_MoveController.Attach( m_pCreator, NULL, fDistSq );
		}
		break;
	case CS_ATTACK:
	case CS_USING_SKILL:
		{
			if( m_MoveController.IsMoveRangeCheck( GetTargetRange( m_TargetData.GetTargetPos() ) ) )
				return false;

			// 방향 탐색
			float fDistSq = GetTargetRange( m_TargetData.GetTargetPos() );
			return m_MoveController.Attach( m_pCreator, NULL, fDistSq );
		}
		break;
	}
	return false;
}

bool ioAI::OnJump()
{
	if( m_JumpController.IsJump() ) return true;
	
	switch( m_pCreator->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		{
			D3DXVECTOR3 vDiff = m_TargetData.GetTargetPos() - m_pCreator->GetWorldPosition();
			vDiff.y = 0.0f;   // 높이는 계산하지 않는다.
			float fDistSq = D3DXVec3LengthSq( &vDiff );				

			D3DXVECTOR3 vNextPos = m_pCreator->GetWorldPosition() + ( m_pCreator->GetMoveDir() * ( m_fJumpCheckRange * FLOAT05 ) );
			float fMapHeight = m_pPlayStage->GetMapHeight( vNextPos.x, vNextPos.z );
			if( fMapHeight <= 0.0f )  //드랍존이면 점프
			{
				m_JumpController.Attach( m_pCreator, NULL, fDistSq );
				return true;
			}
			else if( fMapHeight > m_pCreator->GetWorldPosition().y + 50.0f ) // 현재 내 위치보다 높으면 점프
			{
				m_JumpController.Attach( m_pCreator, NULL, fDistSq );
				return true;				
			}
			else  // 타겟이 나보다 위에 있으면 점프
			{
				// 거리 탐색
				float fJumpRange = m_fJumpCheckRange * m_fJumpCheckRange;
				if( fDistSq < fJumpRange )
				{
					// 높이 탐색
					if( m_TargetData.GetTargetPos().y > m_pCreator->GetWorldPosition().y )
					{
						m_JumpController.Attach( m_pCreator, NULL, fDistSq );
						return true;
					}
				}				
			}
		}
		break;
	}
	return false;
}

bool ioAI::OnDefense()
{
	if( m_DefenseController.IsDefense() ) return true;

	switch( m_pCreator->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
	case CS_WOUNDED:
	case CS_WOUNDED_DEFENSE:
	case CS_BLOW_WOUNDED:
		{
			ioBaseChar *pTargetChar = NULL;
			if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
			{
				pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
			}
			return m_DefenseController.Attach( m_pCreator, pTargetChar, GetTargetRange( m_TargetData.GetTargetPos() ) );
		}
		break;
	}
	return false;
}

bool ioAI::OnAction()
{
	if( IsActionToMove() ) 
		return false;   // 스킬 사용중에는 다음 액션 블럭

	if( m_ActionController.IsAction() ) return true;
	if( m_TargetData.GetTargetType() != TARGET_BASECHAR && m_TargetData.GetTargetType() != TARGET_OBJECT ) 
		return false;

	switch( m_pCreator->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
	case CS_JUMP:
	case CS_ATTACK:
	case CS_WOUNDED:
	case CS_WOUNDED_DEFENSE:
	case CS_BLOW_WOUNDED:
		{				
			ioBaseChar *pTargetChar = NULL;
			if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
			{
				pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
			}
		
			if( m_ActionController.Attach( m_pCreator, pTargetChar, GetTargetRange( m_TargetData.GetTargetPos() ), GetTargetAngle( m_TargetData.GetTargetPos() ) ) )
			{
				//m_pCreator->SetDirectionKey( -1, KEY_NONE );
				m_MoveController.ClearCurrentData();
				return true;
			}
		}
		break;
	}
	return false;
}

void ioAI::ProcessAIforAOS()
{
	// 타겟 검색
	if( !IsRoaming() )
	{
		CheckTargetChar();
	}

	// 타겟이 없으면 모든 키입력 중단.
	if( m_TargetData.GetTargetName().IsEmpty() )
	{
		OnClearAction();
	}
	else
	{
		// 실시간 타겟 위치 변경
		ioBaseChar *pTargetChar = NULL;
		if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
		{
			pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
			if( pTargetChar )
			{
				m_TargetData.SetTargetPos( pTargetChar->GetWorldPosition() );
			}
		}

		m_ActionController.OnAttackAction( m_pCreator, pTargetChar );			

		{   // 타겟에 대한 행동 결정
			if( !OnAction() )
			{				
				if( !OnMove() && !m_DefenseController.IsDefense() )
				{
					OnClearAction();
				}
			}
		}
	}
}

void ioAI::ProcessAI()
{
	// 타겟 검색
	if( !IsRoaming() )
	{
		CheckTargetChar();
	}

	// 타겟이 없으면 모든 키입력 중단.
	if( m_TargetData.GetTargetName().IsEmpty() )
	{
		OnClearAction();
	}
	else
	{
		// 실시간 타겟 위치 변경
		ioBaseChar *pTargetChar = NULL;
		if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
		{
			pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
			if( pTargetChar )
			{
				m_TargetData.SetTargetPos( pTargetChar->GetWorldPosition() );
			}
		}

		{
			// 타겟에 대한 행동
			OnDefense();
			if( m_DefenseController.IsDefense() )
			{
				m_DefenseController.OnDefenseAction( m_pCreator, pTargetChar );
			}
			else
			{
				m_ActionController.OnAttackAction( m_pCreator, pTargetChar );			
				if( !m_ActionController.IsAction() )
				{
					m_MoveController.OnMoveAction( m_pCreator, pTargetChar, m_TargetData.GetTargetPos(), 
												   GetTargetAngle( m_TargetData.GetTargetPos() ), GetTargetRange( m_TargetData.GetTargetPos() ) );
					m_JumpController.OnJumpAction( m_pCreator, pTargetChar );				
				}
				else if( IsActionToMove() )
				{
					m_MoveController.OnMoveAction( m_pCreator, pTargetChar, m_TargetData.GetTargetPos(), 
												   GetTargetAngle( m_TargetData.GetTargetPos() ), GetTargetRange( m_TargetData.GetTargetPos() ) );
				}
			}
		}

		{   // 타겟에 대한 행동 결정
			if( !OnAction() )
			{				
				if( !OnMove() && !m_DefenseController.IsDefense() )
				{
					OnClearAction();
				}
				OnJump();				
			}
		}
	}
}


/*
	3   4   5
	2  NPC  6
	1	0	7          -1 = All Stop
	*/

float ioAIPathFind::GetDirection(const D3DXVECTOR3& vTarget)
{
	float fResult = 0.f;
	const float xGap = 120.f;
	D3DXVECTOR3 vOrigin = m_pCreator->GetWorldPosition();

	if( vTarget.x < vOrigin.x - xGap)
	{
		if( vTarget.z < vOrigin.z - xGap)
			fResult = 45.f;
		else if( vTarget.z > vOrigin.z + xGap )
			fResult = 135.f;
		else
			fResult = FLOAT90;
	}
	else if( vTarget.x > vOrigin.x + xGap)
	{
		if( vTarget.z < vOrigin.z - xGap)
			fResult = 315.f;
		else if( vTarget.z > vOrigin.z + xGap )
			fResult = 225.f;
		else
			fResult = 270.f;
	}
	else
	{
		if( vTarget.z < vOrigin.z - xGap)
			fResult = 0.f;
		else if( vTarget.z > vOrigin.z + xGap )
			fResult = 180.f;
		else
			fResult = m_fAngle;
	}
	m_fAngle = fResult;
	return fResult;
}

#if 1
void ioAIPathFind::ProcessAI()
{
	// 타겟 검색
	if( !IsRoaming() )
	{
		CheckTargetChar();
	}

	// 타겟이 없으면 모든 키입력 중단.
	if( m_TargetData.GetTargetName().IsEmpty() )
	{
		OnClearAction();
	}
	else
	{
		// 실시간 타겟 위치 변경
		ioBaseChar *pTargetChar = NULL;
		if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
		{
			pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
			if( pTargetChar )
			{
				m_TargetData.SetTargetPos( pTargetChar->GetWorldPosition() );
			}
		}

		{
			// 타겟에 대한 행동
			OnDefense();
			if( m_DefenseController.IsDefense() )
			{
				m_DefenseController.OnDefenseAction( m_pCreator, pTargetChar );
			}
			else
			{
				m_ActionController.OnAttackAction( m_pCreator, pTargetChar );			
				if( !m_ActionController.IsAction() )
				{
					int nResult = findBoxPath(m_pCreator->GetWorldPosition(), m_TargetData.GetTargetPos(), m_cBoxPath);

					if( nResult != -1)
					{
						char chPathType;

						if( getNextWaypoint(m_cBoxPath, m_pCreator->GetWorldPosition(), m_TargetData.GetTargetPos(), 
							m_vecWayPts, chPathType))
						{
							m_MoveController.OnMoveAIAction( m_pCreator, pTargetChar, m_vecWayPts, 
								GetDirection(m_vecWayPts), 
								/*GetAngle(m_vecWayPts, m_pCreator->GetWorldPosition()),*/	 
								/*GetTargetAngle( m_vecWayPts )*/
								GetTargetRange( m_TargetData.GetTargetPos() ) );

//							LOG.PrintTimeAndLog(0, "Ai Move Angle %f, %f %f %f %f", GetTargetAngle( m_vecWayPts ), m_vecWayPts.x, m_pCreator->GetWorldPosition().x, m_vecWayPts.z, m_pCreator->GetWorldPosition().z);
						}
						else
						{
							m_MoveController.OnMoveAction( m_pCreator, pTargetChar, m_TargetData.GetTargetPos(), 
								GetTargetAngle( m_TargetData.GetTargetPos() ), GetTargetRange( m_TargetData.GetTargetPos() ) );

//							LOG.PrintTimeAndLog(0, "Move Angle %f, %f %f ", GetTargetAngle( m_TargetData.GetTargetPos() ), m_TargetData.GetTargetPos().x, m_TargetData.GetTargetPos().z);
						}
					}
					else
					{
						m_MoveController.OnMoveAction( m_pCreator, pTargetChar, m_TargetData.GetTargetPos(), 
							GetTargetAngle( m_TargetData.GetTargetPos() ), GetTargetRange( m_TargetData.GetTargetPos() ) );

//						LOG.PrintTimeAndLog(0, "Move Angle %f, %f %f ", GetTargetAngle( m_TargetData.GetTargetPos() ), m_TargetData.GetTargetPos().x, m_TargetData.GetTargetPos().z);
					}
					m_JumpController.OnJumpAction( m_pCreator, pTargetChar );				
				}
				else if( IsActionToMove() )
				{
					m_MoveController.OnMoveAction( m_pCreator, pTargetChar, m_TargetData.GetTargetPos(), 
					GetTargetAngle( m_TargetData.GetTargetPos() ), GetTargetRange( m_TargetData.GetTargetPos() ) );
				}
			}
		}

		{   // 타겟에 대한 행동 결정
			if( !OnAction() )
			{				
				if( !OnMove() && !m_DefenseController.IsDefense() )
				{
					OnClearAction();
				}
				OnJump();				
			}
		}
	}
}

#else

void ioAIPathFind::ProcessAI()
{
// 	if(m_dwFindPathTime < FRAMEGETTIME())
// 	{
// 		m_dwFindPathTime = FRAMEGETTIME() + PATH_FIND_TIME;
// 	}
// 	else
// 	{
// 		return;
// 	}


	switch(m_eState)
	{
	case S_STAND:
//		if( m_pCreator->GetState() == CS_DELAY)
		{
			CheckTargetChar();
			if( m_TargetData.GetTargetType() != TARGET_NONE)
			{
				int nResult = findBoxPath(m_pCreator->GetWorldPosition(), m_TargetData.GetTargetPos(), m_cBoxPath);

				if( nResult > 1)
				{
					m_eState = S_NAV;

					char chPathType;

					if( getNextWaypoint(m_cBoxPath, m_pCreator->GetWorldPosition(), m_TargetData.GetTargetPos(), 
						m_vecWayPts, chPathType))
					{
// 						if( m_pCreator->GetCharName() == " -N33- ")
// 						{
// 							LOG.PrintTimeAndLog(0, "Player %f, %f   WayPoint %f, %f", 
// 								m_pCreator->GetWorldPosition().x, m_pCreator->GetWorldPosition().z,  m_vecWayPts.x, m_vecWayPts.z);
// 						}

						float fAngle = GetAngle(m_vecWayPts, m_pCreator->GetWorldPosition());

						D3DXVECTOR3 vWayDir   = m_vecWayPts - m_pCreator->GetWorldPosition();
						D3DXVECTOR3 vCreatorDir = m_pCreator->GetMoveDir();
						D3DXVec3Normalize( &vWayDir, &vWayDir );
						D3DXVec3Normalize( &vCreatorDir, &vCreatorDir );
						float fCos = D3DXVec3Dot( &vCreatorDir, &vWayDir );
						float fRadian = acos( fCos );
						float fDeg    = RADtoDEG(fRadian);

//						if( fDeg >= 45.f)
						m_fAngle = fAngle;

						int iDirection = ( (int)m_fAngle % 360 ) / 45;
						m_pCreator->SetDirectionKey( iDirection, KEY_REPEAT );
					}
					else
					{
						ioBaseChar *pTargetChar = NULL;
						if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
						{
							pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
							if( pTargetChar )
							{
								m_TargetData.SetTargetPos( pTargetChar->GetWorldPosition() );
							}
						}

						m_ActionController.OnAttackAction( m_pCreator, pTargetChar );
					}
				}
				else
				{
					ioBaseChar *pTargetChar = NULL;
					if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
					{
						pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
						if( pTargetChar )
						{
							m_TargetData.SetTargetPos( pTargetChar->GetWorldPosition() );
						}
					}

					m_ActionController.OnAttackAction( m_pCreator, pTargetChar );
				}
			}
			else
			{
				ioBaseChar *pTargetChar = NULL;
				if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
				{
					pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
					if( pTargetChar )
					{
						m_TargetData.SetTargetPos( pTargetChar->GetWorldPosition() );
					}
				}

				m_ActionController.OnAttackAction( m_pCreator, pTargetChar );
			}
		}
		break;

	case S_NAV:
			{
				
				char chPathType;

//				if( GetTargetRange(m_pCreator->GetWorldPosition(), m_vecWayPts) > 6.5f)
				{
					if( getNextWaypoint(m_cBoxPath, m_pCreator->GetWorldPosition(), m_TargetData.GetTargetPos(), 
						m_vecWayPts, chPathType))
					{
// 						if( m_pCreator->GetCharName() == " -N33- ")
// 						{
// 							LOG.PrintTimeAndLog(0, "Player %f, %f   WayPoint %f, %f", 
// 								m_pCreator->GetWorldPosition().x, m_pCreator->GetWorldPosition().z,  m_vecWayPts.x, m_vecWayPts.z);
// 						}

						float fAngle = GetAngle(m_vecWayPts, m_pCreator->GetWorldPosition());

						D3DXVECTOR3 vWayDir   = m_vecWayPts - m_pCreator->GetWorldPosition();
						D3DXVECTOR3 vCreatorDir = m_pCreator->GetMoveDir();
						D3DXVec3Normalize( &vWayDir, &vWayDir );
						D3DXVec3Normalize( &vCreatorDir, &vCreatorDir );
						float fCos = D3DXVec3Dot( &vCreatorDir, &vWayDir );
						float fRadian = acos( fCos );
						float fDeg    = RADtoDEG(fRadian);

						if( fDeg >= 45.f)
							m_fAngle = fAngle;

						int iDirection = ( (int)m_fAngle % 360 ) / 45;
						m_pCreator->SetDirectionKey( iDirection, KEY_REPEAT );
					}
					else
					{
						ioBaseChar *pTargetChar = NULL;
						if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
						{
							pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
							if( pTargetChar )
							{
								m_TargetData.SetTargetPos( pTargetChar->GetWorldPosition() );
							}
						}

						m_ActionController.OnAttackAction( m_pCreator, pTargetChar );

						if( !m_ActionController.IsAction() )
						{
							int iDirection = ( (int)m_fAngle % 360 ) / 45;
							m_pCreator->SetDirectionKey( iDirection, KEY_REPEAT );
							m_eState = S_STAND;
						}
					}
				}
// 				else
// 				{
// 					int iDirection = ( (int)m_fAngle % 360 ) / 45;
// 					m_pCreator->SetDirectionKey( iDirection, KEY_REPEAT );
// 				}
			}
		break;

	case S_ATTACK:
		if(!IsActionToMove() )
		{
			m_eState = S_STAND;
		}
		break;
	}
	
	if( !OnAction() )
	{				
		if( !OnMove() && !m_DefenseController.IsDefense() )
		{
			OnClearAction();
			m_eState = S_STAND;
		}
		OnJump();				
	}
	else
	{
		m_eState = S_NAV;
	}
}
#endif


ioAIPathFind::ioAIPathFind( ioNpcChar *pCreator, ioPlayStage *pPlayStage )
	: ioAI( pCreator, pPlayStage )
{	
	m_eState = S_STAND;
	m_dwFindPathTime = 0;
	m_fAngle = 0.f;
}

ioAIPathFind::~ioAIPathFind()
{

}


int ioAIPathFind::findBoxPath( const D3DXVECTOR3& posFrom, const D3DXVECTOR3& posTo, BoxPath& outPath )
{
	D3DXVECTOR3 p1( posFrom.x, posFrom.z, posFrom.y ), p2( posTo.x, posTo.z, posTo.y );

	ioTowerDefMode *pTowerDefMode = ToTowerDefMode( m_pPlayStage->GetPlayMode() );

	if(!pTowerDefMode)
		return false;

	return pTowerDefMode->GetNaviInfo()->findBoxPath( p1, p2, outPath );
}

bool ioAIPathFind::getNextWaypoint( BoxPath& path, const D3DXVECTOR3& posCur, const D3DXVECTOR3& posGoal, D3DXVECTOR3& outWaypoint, char& outType ) const
{
	D3DXVECTOR3 pc( posCur.x, posCur.z, posCur.y ), pg( posGoal.x, posGoal.z, posGoal.y ), pw( outWaypoint.x, outWaypoint.z, outWaypoint.y );

	ioTowerDefMode *pTowerDefMode = ToTowerDefMode( m_pPlayStage->GetPlayMode() );

	if(!pTowerDefMode)
		return false;

	bool bRes = pTowerDefMode->GetNaviInfo()->ls_getNextWaypoint( path, pc, pg, pw, outType );

//	bool bRes = pTowerDefMode->GetNaviInfo()->getNextWaypoint( path, pc, pg, pw, outType );

	if ( bRes ) 
		outWaypoint.x = pw.x, outWaypoint.y = pw.z, outWaypoint.z = pw.y;
	return bRes;
}


bool ioAIPathFind::OnMove()
{
	if( m_ActionController.IsAction() ) 
	{
		if( !IsActionToMove() )
			return false;
	}

	if( m_MoveController.IsMove() && IsTargetBaseChar() )
	{
		if( !IsActionToMove() )
		{
			// 이미 이동중이라면 정지를 탐색 
			float fRangeMin = m_fSearchRangeMin * m_fSearchRangeMin;
			if( GetTargetRange( m_TargetData.GetTargetPos() ) < fRangeMin )
			{
				return false;
			}
			return true;
		}
	}

	switch( m_pCreator->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
	case CS_JUMP:
		{
			// 방향 탐색
			float fDistSq = GetTargetRange( m_TargetData.GetTargetPos() );		
			float fHalfCosine = cosf( DEGtoRAD(FLOAT90 ) * FLOAT05 );
			float fTargetAngle = GetTargetAngle( m_TargetData.GetTargetPos() );
			if( fTargetAngle >= fHalfCosine )
			{
				// 거리 탐색
				float fRangeMin = m_fSearchRangeMin * m_fSearchRangeMin;
				if( fDistSq < fRangeMin )
				{
					return false;
				}
			}
			return m_MoveController.Attach( m_pCreator, NULL, fDistSq );
		}
		break;
	case CS_ATTACK:
	case CS_USING_SKILL:
		{
			if( m_MoveController.IsMoveRangeCheck( GetTargetRange( m_TargetData.GetTargetPos() ) ) )
				return false;

			// 방향 탐색
			float fDistSq = GetTargetRange( m_TargetData.GetTargetPos() );
			return m_MoveController.Attach( m_pCreator, NULL, fDistSq );
		}
		break;
	}
	return false;
}


void ioAIPathFind::CheckTargetChar()
{
	float fSearchTime = FRAMEGETTIME() - m_fSearchTargetTime;
	if( fSearchTime < m_fCurSearchTime )
	{
		return;
	}
	m_fCurSearchTime = FRAMEGETTIME();	

	m_TargetData.Init();

	CheckGlobalTarget();
	CheckFixingTarget();
	CheckRandomAggroTarget();
	CheckWayPoint(); 

	float fSearchRange = m_fSearchRangeCurrent * m_fSearchRangeCurrent;	
	if( m_TargetData.GetTargetType() != TARGET_WAYPOINT )
	{
		// 데미지에 따른 우선 순위
		ioHashString szFirstTarget = m_szFixingName;		
		if( szFirstTarget.IsEmpty() )
		{
			switch( m_dwSearchType )
			{
			case SEARCH_TOP_DAMAGE:
				m_pCreator->GetDamagedTopChar( szFirstTarget );
				break;
			case SEARCH_LOWEST_DAMAGE:
				m_pCreator->GetDamagedLowestChar( szFirstTarget );
				break;
			case SEARCH_MODE_CONDITION:
				m_pCreator->GetModeCondition( szFirstTarget );
				break;	
			}
		}

		ioBaseChar *pFirstTarget = m_pCreator->GetBaseChar( szFirstTarget );
		if( !IsTargetIgnoreState( pFirstTarget ) )
		{
			fSearchRange = GetTargetRange( pFirstTarget->GetWorldPosition() );
			m_TargetData.SetTargetName( pFirstTarget->GetCharName() );
			m_TargetData.SetTargetPos( pFirstTarget->GetWorldPosition() );
			m_TargetData.SetTargetLevel( pFirstTarget->GetLevel() );
			m_TargetData.SetTargetType( TARGET_BASECHAR );
		}
		else
		{
			BaseCharList vTargetList;
			BaseCharList::const_iterator iter;
			if( IsOffensive() )
			{				
				D3DXVECTOR3 vPos = m_pCreator->GetMidPositionByRate( FLOAT08 ) + ( m_pCreator->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) );
				m_pCreator->GetNpcTargetCharListByCylinder( vPos, m_fSearchRangeMax, m_fTargetSearchAngle, m_fSearchRangeUpByCylinder, m_fSearchRangeDownByCylinder, vTargetList );				
				for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
				{
					ioBaseChar *pChar = (*iter);

					// 					ioNpcChar *pNpcChar = ToNpcChar(pChar);
					// 					if( pNpcChar)
					// 						continue;

					if( IsTargetIgnoreState( pChar ) ) continue;

					float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
					if( fDistSq < fSearchRange )	// Enemy
					{
						fSearchRange = fDistSq;
						m_TargetData.SetTargetName( pChar->GetCharName() );
						m_TargetData.SetTargetPos( pChar->GetWorldPosition() );
						m_TargetData.SetTargetLevel( pChar->GetLevel() );
						m_TargetData.SetTargetType( TARGET_BASECHAR );					
					}
				}
				vTargetList.clear();
			}

			// 타겟이 없으면 로밍
			if( m_TargetData.GetTargetType() == TARGET_NONE )
			{
				if( m_pCreator->IsDeathMonster() )
				{
					const BaseCharList &rkTargetList = m_pPlayStage->GetBaseCharList();
					for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
					{
						ioBaseChar *pChar = (*iter);
						if( IsTargetIgnoreState( pChar, false ) ) continue;

						float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
						if( fDistSq < fSearchRange )	// Enemy
						{
							fSearchRange = fDistSq;
							m_TargetData.SetTargetName( pChar->GetCharName() );
							m_TargetData.SetTargetPos( pChar->GetWorldPosition() );
							m_TargetData.SetTargetType( TARGET_BASECHAR );
						}
					}
				}
				else if( m_bRoaming )
				{
					fSearchRange = 200.0f * 200.0f;
					m_TargetData.SetTargetPos( GetRoamingPoint() );
					m_TargetData.SetTargetType( TARGET_ROAMING );
					m_TargetData.SetTargetName( m_pCreator->GetCharName() );
				}
			}
		}		
	}

	if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
		m_fSearchRangeCurrent = m_fSearchRangeMax;
}
