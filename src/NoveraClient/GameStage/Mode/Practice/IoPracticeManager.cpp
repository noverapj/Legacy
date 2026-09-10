#include <stdafx.h>

#include "ioPracticeManager.h"
#include "../../../../io3DEngine/ioHashString.h"
#include "../../../GUI/LobbyMainWnd.h"
#include "../../../GUI/PracticemainWnd.h"

static ioPracticeManager *s_pInstance = NULL;

ioPracticeManager::ioPracticeManager()
{
	m_pPracticeMgr = NULL;
	m_PracticeRecordMap.clear();

	m_iPrePracticeIndex = 0;
	m_vecRankerInfo.reserve(3);//3위까지 보여줌
	for(int i = 0; i < 3; ++i)
	{
		PracticeRankerInfo _Dummy;
		_Dummy.szTime="00:00.00";
		m_vecRankerInfo.push_back(_Dummy);
	}

	m_bRefresh = false;
}

ioPracticeManager::~ioPracticeManager()
{
	if( m_pPracticeMgr )
	{
		m_pPracticeMgr->Release();
		SAFEDELETE( m_pPracticeMgr );
	}

	m_PracticeRecordMap.clear();
	g_PracticeMgr.SetPreIndex( 0 );
}

ioPracticeManager& ioPracticeManager::GetInstance()
{
	if( !s_pInstance )
	{
		s_pInstance = new ioPracticeManager;
	}

	return *s_pInstance;
}

void ioPracticeManager::ReleaseInstance()
{
	SAFEDELETE( s_pInstance );
}

void ioPracticeManager::Init()
{
	if( m_pPracticeMgr )
	{
		m_pPracticeMgr->Release();
		SAFEDELETE( m_pPracticeMgr );
	}

	m_pPracticeMgr = new LSC_Practice_Manager;
	m_pPracticeMgr->LoadData( PRACTICE_TABLE );

	ioINILoader kLoader( "config/sp2_practiceinfo.ini" );
	INIParsing(kLoader);

	int iPracticeCount = GetPracticeCount();
	iPracticeCount = iPracticeCount %2 == 0 ? iPracticeCount:iPracticeCount+1;
	
	m_vecPracticeMyInfo.reserve(iPracticeCount);
	int iCount = GetPracticeCount();
	for( int i = 0 ; i < iPracticeCount; ++i)
	{
		PracticeListMyInfo _Dummy;
		_Dummy.szPracticeTime="00:00.00";
		_Dummy.iPracticeCount = 3;
		if( i == iPracticeCount-1 && iCount != iPracticeCount)
			_Dummy.iPracticeCount = 0;
		_Dummy.iPracticeGrade = 0;
		m_vecPracticeMyInfo.push_back(_Dummy);
	}

}

void ioPracticeManager::INIParsing( ioINILoader& kLoader )
{
	if( m_pPracticeMgr )
	{
		for(int i = 0; i < GetPracticeCount(); ++i)//maxmode 수치 받아서 변경
		{
			LSC_Practice *pData = m_pPracticeMgr->GetData( i+1 );
			if(pData)
			{
				kLoader.SetTitle_e("mode_info");
				kLoader.LoadString(pData->TypeTitle, "", pData->TypeTitle, MAX_PATH);
				kLoader.LoadString(pData->TypeDesc, "", pData->TypeDesc, MAX_PATH);
				kLoader.LoadString(pData->Guide_1_Desc, "", pData->Guide_1_Desc, MAX_PATH);
				kLoader.LoadString(pData->UseName, "", pData->UseName, MAX_PATH);
			}
		}
	}
}

int ioPracticeManager::GetPracticeCount()
{
	if( m_pPracticeMgr )
		return m_pPracticeMgr->GetTotal();

	return 0;
}

bool ioPracticeManager::IsValidPractice( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );
		
		if( pData ) 
			return true;
	}

	return false;
}

ioHashString ioPracticeManager::GetListImage( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->ListImage;
	}

	return ioHashString();
}

ioHashString ioPracticeManager::GetListOverImage( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->ListImage_Over;
	}

	return ioHashString();
}

void ioPracticeManager::ApplyPracticeList( SP2Packet &rkPacket )
{
	int iCnt;
	DWORD dwIndex, dwGrade;

	PACKET_GUARD_VOID_READ( rkPacket, iCnt );

	for( int i = 0; i < iCnt; i++ )
	{
		PACKET_GUARD_VOID_READ( rkPacket, dwIndex );
		PACKET_GUARD_VOID_READ( rkPacket, dwGrade );

		PracticeRecordMap::iterator it = m_PracticeRecordMap.find( dwIndex );

		if( it == m_PracticeRecordMap.end() )
			m_PracticeRecordMap.insert( std::make_pair( dwIndex, dwGrade ) );
		else
			it->second = dwGrade;
	}
}

void ioPracticeManager::SetGrade( DWORD dwIndex, DWORD dwGrade )
{
	PracticeRecordMap::iterator it = m_PracticeRecordMap.find( dwIndex );

	if( it == m_PracticeRecordMap.end() )
	{
		m_PracticeRecordMap.insert( std::make_pair( dwIndex, dwGrade ) );
	}
	else
	{
		it->second = dwGrade;
	}
}

DWORD ioPracticeManager::GetPracticeRecord( int iIndex )
{
	PracticeRecordMap::iterator it = m_PracticeRecordMap.find( iIndex );

	if( it == m_PracticeRecordMap.end() )
		return 0;

	return it->second;
}

int ioPracticeManager::GetRegularSoldierClass( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseClass;
	}
	return -1;
}

BYTE ioPracticeManager::GetRegularSoldierGender( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseClass;
	}
	return 0;
}

int ioPracticeManager::GetRegularSoldierUnderwear( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseClass;
	}
	return -1;
}

int ioPracticeManager::GetRegularSoldierHair( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseClass;
	}
	return -1;
}

int ioPracticeManager::GetRegularSoldierHairColor( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseClass;
	}
	return -1;
}

int ioPracticeManager::GetRegularSoldierFace( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseClass;
	}
	return -1;
}

int ioPracticeManager::GetRegularSoldierSkinColor( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseClass;
	}
	return -1;
}

int ioPracticeManager::GetPracticeMap( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );
		
		if( pData ) 
			return pData->MapIndex;
	}
	return -1;
}

const char* ioPracticeManager::GetINIPath( int iIndex )
{
	if( m_pPracticeMgr )
	{
		if( iIndex == 0 )
			iIndex = GetPreIndex();
		//수련장 수정함
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->INIPath;
	}
	return "";
}

DWORD ioPracticeManager::GetLimitTime( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->TimeLimit;
	}
	return 0;
}

ioPracticeManager::PracticeType ioPracticeManager::GetPracticeType( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return PracticeType(pData->Type);
	}
	return E_PRACTICE_NONE;
}

ioHashString ioPracticeManager::GetRegularSoldierName( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseName;
	}
	return ioHashString();
}

ioHashString ioPracticeManager::GetPracticeGradeDesc( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return GET_STRING( pData->GradeDesc );
	}
	return ioHashString();
}

DWORD ioPracticeManager::GetPracticeGradeA( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->GradeA;
	}
	return 0;
}

DWORD ioPracticeManager::GetPracticeRewardA( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardA;
	}
	return 0;
}


DWORD ioPracticeManager::GetPracticeGradeB( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->GradeB;
	}
	return 0;
}

DWORD ioPracticeManager::GetPracticeRewardB( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardB;
	}
	return 0;
}

DWORD ioPracticeManager::GetPracticeGradeC( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->GradeC;
	}
	return 0;
}

DWORD ioPracticeManager::GetPracticeRewardC( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardC;
	}
	return 0;
}

ioHashString ioPracticeManager::GetPracticeLoadingText( int iIndex )
{
	if( m_pPracticeMgr )
	{
		if( iIndex == 0 )
			iIndex = GetPreIndex();

		LSC_Practice *pData = m_pPracticeMgr->GetData( GetPreIndex() );

		if( pData ) 
			return pData->LoadingText;
	}
	return ioHashString();
}

DWORD ioPracticeManager::GetPracticeBriefingTime( int iIndex )
{
	if( m_pPracticeMgr )
	{
		if( iIndex == 0 )
			iIndex = GetPreIndex();

		LSC_Practice *pData = m_pPracticeMgr->GetData( GetPreIndex() );

		if( pData ) 
			return pData->BriefingTime;
	}
	return 0;
}

ioHashString ioPracticeManager::GetPracticeTypeDesc( int iIndex )
{
	if( m_pPracticeMgr )
	{
		if( iIndex == 0 )
			iIndex = GetPreIndex();

		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return GET_STRING( pData->TypeDesc );
	}
	return ioHashString();
}

ioHashString ioPracticeManager::GetPracticeTypeTitle( int iIndex )
{
	if( m_pPracticeMgr )
	{
		if( iIndex == 0 )
			iIndex = GetPreIndex();

		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return GET_STRING( pData->TypeTitle );
	}
	return ioHashString();
}

ioHashString ioPracticeManager::GetPracticeGuide1( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->Guide_1;
	}
	return ioHashString();
}

ioHashString ioPracticeManager::GetPracticeGuideDesc1( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return GET_STRING( pData->Guide_1_Desc );
	}
	return ioHashString();
}

ioHashString ioPracticeManager::GetPracticeGuide2( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->Guide_2;
	}
	return ioHashString();
}

ioHashString ioPracticeManager::GetPracticeGuideDesc2( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return GET_STRING( pData->Guide_2_Desc );
	}
	return ioHashString();
}

ioHashString ioPracticeManager::GetPracticeGuide3( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->Guide_3;
	}
	return ioHashString();
}

ioHashString ioPracticeManager::GetPracticeGuideDesc3( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return GET_STRING( pData->Guide_3_Desc );
	}
	return ioHashString();
}

bool ioPracticeManager::IsCountDown( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->Countdown;
	}

	return false;
}

int ioPracticeManager::GetPracticeRemainCount()
{
	int iRemainCnt = g_PracticeMgr.GetPracticeCount() - (int)m_PracticeRecordMap.size();
	iRemainCnt = max( iRemainCnt, 0 );
	return iRemainCnt;
}

short ioPracticeManager::GetPracticeRewardAType( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardAType;
	}
	return 0;
}

short ioPracticeManager::GetPracticeRewardBType( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardBType;
	}
	return 0;
}

short ioPracticeManager::GetPracticeRewardCType( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardCType;
	}
	return 0;
}

int ioPracticeManager::GetPracticeRewardValueA( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardAValue;
	}
	return 0;
}

int ioPracticeManager::GetPracticeRewardValueB( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardBValue;
	}
	return 0;
}

int ioPracticeManager::GetPracticeRewardValueC( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardCValue;
	}
	return 0;
}

void ioPracticeManager::SetServerINIList(SP2Packet& rkPacket)
{
	//주간보상
	m_vecPracticeReward.clear();
	int iWeek, iHour, iMinute, iCount=0;
	rkPacket >> iWeek >> iHour >> iMinute >> iCount;

	PracticeReward Reward;

	for(int i = 0; i < iCount; ++i)
	{
		rkPacket >> Reward.iRewardStart >> Reward.iRewardEnd ;
		int iPresentCount = 0;
		rkPacket >> iPresentCount;

		for( int j = 0; j < iPresentCount; ++j )
		{
			PracticePresent kPresent;
			rkPacket >> kPresent.iRewardType >> kPresent.iRewardCode >> kPresent.iRewardCount;
			Reward.kPresent.push_back(kPresent);
		}
		m_vecPracticeReward.push_back(Reward);
	}
}

void ioPracticeManager::SetServerMyInfoList( SP2Packet& rkPacket )
{
	int iCount = 0;
	int iTime = 0;
	int iIndex = 0;
	PracticeListMyInfo MyInfo;

	DWORD dwMinute = 0;
	DWORD dwSecond = 0;
	DWORD dwCentisecond = 0;

	rkPacket >> iCount;
	int iMax = g_PracticeMgr.GetPracticeCount();
	iCount = iCount > iMax ? iMax : iCount;

	for(int i = 0 ; i < iCount; ++i)
	{
		rkPacket >> iIndex;
		rkPacket >> MyInfo.iPracticeGrade >> MyInfo.iPracticeCount 
			>> iTime >> MyInfo.iPracticeRank;

		//LOG.PrintTimeAndLog( 0, "ioPracticeManager::SetServerMyInfoList %d/%d", i, MyInfo.iPracticeCount );

		if(iIndex < 1 || iIndex > iCount)
		{
			LOG.PrintTimeAndLog( 0, "ioPracticeManager::SetServerMyInfoList  continue, ");
			continue;
		}

		dwMinute = ( iTime / 1000 ) / 60;
		dwSecond = ( iTime / 1000 ) % 60;
		dwCentisecond = (iTime % 1000) / 10;

		CString szTime;

		szTime.Format(_T("%02d:%02d.%02d"), dwMinute,dwSecond,dwCentisecond);
		MyInfo.szPracticeTime = szTime;

		m_vecPracticeMyInfo[iIndex-1] = MyInfo;
	}
}

void ioPracticeManager::SetModeLobbyInfo( SP2Packet& rkPacket )
{
	int iIndex, iRank = 0, iTime = 0;
	ioHashString szName;
	//PracticeListMyInfo Ranker;

	CString szTime;

	DWORD dwMinute = 0;
	DWORD dwSecond = 0;
	DWORD dwCentisecond = 0;

	m_vecRankerInfo.clear();

	//자신
	rkPacket >> iIndex;
	rkPacket >> iRank >> iTime >> m_vecPracticeMyInfo[iIndex-1].iPracticeGrade
		>> m_vecPracticeMyInfo[iIndex-1].iPracticeCount;

	//LOG.PrintTimeAndLog( 0, "SetModeLobbyInfo iCount = %d",m_vecPracticeMyInfo[iIndex-1].iPracticeCount );
	iRank = iRank < 99999 ? iRank : 99999;
	m_vecPracticeMyInfo[iIndex-1].iPracticeRank = iRank;
	if(iTime != 0)  
		m_vecRankerInfo.clear();

	dwMinute = ( iTime / 1000 ) / 60;
	dwSecond = ( iTime / 1000 ) % 60;
	dwCentisecond = (iTime % 1000) / 10;

	if(dwMinute > 10 || iTime == 0)
		szTime = "00:00.00";
	else
		szTime.Format(_T("%02d:%02d.%02d"), dwMinute,dwSecond,dwCentisecond);
	m_MyRankListInfo.szPracticeTime = szTime;
	m_MyRankListInfo.iPracticeRank = m_vecPracticeMyInfo[iIndex-1].iPracticeRank;
	m_MyRankListInfo.iPracticeCount = m_vecPracticeMyInfo[iIndex-1].iPracticeCount;
	m_MyRankListInfo.iPracticeGrade = m_vecPracticeMyInfo[iIndex-1].iPracticeGrade;

	SetRankerInfo(rkPacket);
	SetRankerInfo(rkPacket);
	SetRankerInfo(rkPacket);

	int iMgrIndex = g_PracticeMgr.GetPreIndex();

	if(m_bRefresh && iMgrIndex == iIndex)
	{
		LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>(g_GUIMgr.FindWnd( LOBBY_MAIN_WND ));
		LobbyWnd *pLobbyWnd = dynamic_cast<LobbyWnd*>(pLobbyMainWnd->FindChildWnd(1)); // lobbywnd
		if(pLobbyWnd)
		{
			pLobbyWnd->SetTabID( LobbyWnd::ID_BATTLE_BTN );
			pLobbyWnd->SetTabID( LobbyWnd::ID_PRACTICE_BTN );
		}
		m_bRefresh = false;
	}

}

void ioPracticeManager::SetRankerInfo( SP2Packet& rkPacket )
{
	int iTime = 0;
	ioHashString szName;
	PracticeRankerInfo Ranker;

	DWORD dwMinute = 0;
	DWORD dwSecond = 0;
	DWORD dwCentisecond = 0;

	rkPacket >> Ranker.szName >> iTime >> Ranker.iRank;
	dwMinute = ( iTime / 1000 ) / 60;
	dwSecond = ( iTime / 1000 ) % 60;
	dwCentisecond = (iTime % 1000) / 10;
	CString szTime;

	if(dwMinute > 10 || iTime == 0)
		szTime = "00:00.00";
	else
		szTime.Format(_T("%02d:%02d.%02d"), dwMinute,dwSecond,dwCentisecond);

	Ranker.szTime = szTime;
	m_vecRankerInfo.push_back(Ranker);

}

int ioPracticeManager::GetPracticeAdmission( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->FreeAdmission;
	}
	return 0;
}

int ioPracticeManager::GetPracticeAdmissionMoney( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->AdmissionMoney;
	}
	return 0;
}

int ioPracticeManager::GetPracticeAdmissionType( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->AdmissionType;
	}
	return 0;
}

ioPracticeManager::PracticeListMyInfo ioPracticeManager::GetMyRankInfo()
{
	return m_MyRankListInfo;
}

void ioPracticeManager::SetServerRankingList( SP2Packet& rkPacket )
{
	m_preRankingReward.clear();
	int iCount = 0, iPresentCount=0;
	ioHashString strStart, strEnd;

	//이전 랭킹 정보

	int iFirstLogin = 0;
	rkPacket >> iFirstLogin >> iCount;

	if(iCount > g_PracticeMgr.GetPracticeCount())
		iCount = 0;

	m_preRankingReward.reserve(iCount);
	for(int i = 0 ; i < g_PracticeMgr.GetPracticeCount(); ++i)
	{
		PracticePreRewardInfo preInfo;
		PracticePresent kPresent;
		kPresent.iRewardCode = 0;
		kPresent.iRewardCount = 0;
		kPresent.iRewardType = 0;
		preInfo.iPracticeIDX = 0;
		preInfo.iPracticeRank = 0;
		preInfo.strStartDate = "";
		preInfo.strEndDate = "";
		preInfo.vecPresent.push_back(kPresent);
		m_preRankingReward.push_back(preInfo);
	}
	PracticePreRewardInfo preInfo;

	for(int i = 0 ; i < iCount; ++i)
	{
		rkPacket >> preInfo.strStartDate >> preInfo.strEndDate >> preInfo.iPracticeIDX >> preInfo.iPracticeRank >> iPresentCount;
		preInfo.vecPresent.clear();
		preInfo.vecPresent.reserve(iPresentCount);		
		PracticePresent kPresent;
		for( int j = 0; j < iPresentCount; ++j)
		{
			rkPacket >> kPresent.iRewardType >> kPresent.iRewardCode >> kPresent.iRewardCount;
			preInfo.vecPresent.push_back(kPresent);
		}
		m_preRankingReward[i] = preInfo;
	}
	
	if( iFirstLogin && iCount > 0 )
	{
		ioWnd *pLobbyMainWnd = g_GUIMgr.FindWnd( LOBBY_MAIN_WND );
		if(pLobbyMainWnd)
		{
			ioWnd *pPracticeLobby = pLobbyMainWnd->FindChildWnd(LobbyMainWnd::ID_PRACTICE_INFO_WND);
			if(pPracticeLobby->IsShow())
				return;

			ioWnd *pRankingWnd = g_GUIMgr.FindWnd(PRACTICE_RANKING_WND);
			if(pRankingWnd)
				pRankingWnd->ShowWnd();
			else
			{
				ioWnd *pPracTiceRank = ( g_GUIMgr.AddWnd( "XML/Practice/PracticeRankingWnd.xml" ) );
				pPracTiceRank->ShowWnd();
			}			
		}
	}


}
char* ioPracticeManager::GetPracticeBtnImage( int iIndex )
{
	LSC_Practice *pData = m_pPracticeMgr->GetData(iIndex);
	char* szTemp = pData->ListImage;
	return szTemp;
}

