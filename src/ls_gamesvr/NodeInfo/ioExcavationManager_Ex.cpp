// 2018-01-15 by bckim, 탐사 확장 ioExcavationManager_Ex.cpp 추가 

#include <stdafx.h>

#include "ioExcavationManager_Ex.h"
#include "User.h"
#include "Room.h"

ioExcavationManager_Ex::ioExcavationManager_Ex()
{
	Init();
}

ioExcavationManager_Ex::~ioExcavationManager_Ex()
{
	Destroy();
}

void ioExcavationManager_Ex::Init()
{
	m_ioRand.Randomize();
	m_mMapExcavationPos.clear();
	m_mMapRewardInfo.clear();
	m_mMapTotalRewardRand.clear();
	m_vRewardGradeInfo.clear();
	m_mCriticalInfo.clear();
	m_mapExcavationEventGradeList.clear();

	m_iTotalRewardGradeRand	= 0;
	m_iRechargePermisionGap	= 0;
	m_iCheckRange			= 0;
	m_iReValuePeso			= 0;
	m_fRevalueAddRate		= 0.0f;
	m_iTimeOutSec			= 0;
	m_iMinimumCoolTime		= 0;
	m_iMaxCriticalValue		= 1;

	m_pExcavationRewardDat	= NULL;
	m_pExcavationGradeDat	= NULL;
}

void ioExcavationManager_Ex::Destroy()
{
	if( m_pExcavationRewardDat )
	{
		m_pExcavationRewardDat->Release();
		SAFEDELETE( m_pExcavationRewardDat );
	}

	if( m_pExcavationGradeDat )
	{
		m_pExcavationGradeDat->Release();
		SAFEDELETE( m_pExcavationGradeDat );
	}
}

void ioExcavationManager_Ex::LoadData()
{
	m_iTotalRewardGradeRand	= 0;
	m_iRechargePermisionGap	= 0;
	m_iCheckRange			= 0;
	m_iReValuePeso			= 0;
	m_fRevalueAddRate		= 0.0f;
	m_iTimeOutSec			= 0;
	m_iMinimumCoolTime		= 0;
	m_iMaxCriticalValue		= 1;

	m_pExcavationRewardDat	= new LSC_Excavation_info_Manager;
	m_pExcavationGradeDat	= new LSC_Excavation_grade_Manager;

	if( !m_pExcavationRewardDat || !m_pExcavationGradeDat )
		return;


	if( !m_pExcavationRewardDat->LoadData(EXCAVATION_REWARD) )
		return;

	if( !m_pExcavationGradeDat->LoadData(EXCAVATION_GRADE) )
		return;

	//실제 데이타 겟
	int iTotal = m_pExcavationRewardDat->GetTotal();

	MAPREWARDINFO mMapExcavationInfo;
	mMapExcavationInfo.clear();

	MAPTOTALRAND mMapTotalRand;
	mMapTotalRand.clear();

	for( int i = 0; i < iTotal; i++ )
	{
		LSC_Excavation_info* pInfo	= m_pExcavationRewardDat->GetAt(i);

		if( pInfo )
		{
			int iMap		= 0;
			ExcavationReward stReward;			
			stReward.m_iRewardType	= pInfo->RewardType;			// bckim 보상 아이템 타입 정보 추가	int m_iRewardType;
			stReward.m_iIndex		= pInfo->ItemIndex;
			stReward.m_iType		= pInfo->ItemType;
			stReward.m_iRand		= pInfo->ItemRate;
			stReward.m_iPrice		= pInfo->ItemPrice;
			stReward.bRoomAlarm		= pInfo->AlarmRoom;
			stReward.bWholeAlarm	= pInfo->AlarmAll;

			iMap					= pInfo->AppointMap01;

			MAPREWARDINFO::iterator it = mMapExcavationInfo.find(iMap);
			if( it != mMapExcavationInfo.end() )
				(it->second).push_back(stReward);
			else
			{
				REWARDINFO vVac;
				vVac.push_back(stReward);
				mMapExcavationInfo.insert(std::make_pair(iMap, vVac));
			}

			MAPTOTALRAND::iterator it2 = mMapTotalRand.find(iMap);
			if( it2 != mMapTotalRand.end() )
				it2->second += stReward.m_iRand;
			else
				mMapTotalRand.insert(std::make_pair(iMap, stReward.m_iRand));
		}
	}

	m_vRewardGradeInfo.clear();
	iTotal	= m_pExcavationGradeDat->GetTotal();
	for( int i = 0; i < iTotal; i++ )
	{
		LSC_Excavation_grade* pInfo	= m_pExcavationGradeDat->GetAt(i);

		if( pInfo )
		{
			RewardGrade stGrade;

			stGrade.m_iIndex		= pInfo->GradeIndex;
			stGrade.m_iRand			= pInfo->GradeRate;
			stGrade.m_dGradeValue	= pInfo->GradeValue;

			m_iTotalRewardGradeRand += stGrade.m_iRand;

			m_vRewardGradeInfo.push_back(stGrade);
		}
	}

	IntVec vMapID;
	vMapID.clear();

	// x,y,z 좌표가 저장된 ini로드. 여기에만 광장 map id가 저장되어 있어서 이 정보를 가지고 map id별 분리 시작.
	char szKey[MAX_PATH] = "";

	ioINILoader kLoader;
	kLoader.ReloadFile("config/lsc_excavation_info.ini");

	kLoader.SetTitle("common");
	m_iCheckRange	= kLoader.LoadInt( "dig_able_check_space", 0 );
	m_iRechargePermisionGap	= kLoader.LoadInt( "permission_recharge_gap", 0 );
	m_iTimeOutSec	= kLoader.LoadInt( "timeout_sec", 0 );

	m_dwDefaultCoolTime	= kLoader.LoadInt( "default_cool_time", 0 );
	m_iDecreaseLevelGap	= kLoader.LoadInt( "decrease_level_gap", 0 );
	m_iDecreaseCoolTime	= kLoader.LoadInt( "decrease_cool_time", 0 );
	m_iMinimumCoolTime	= kLoader.LoadInt( "minimum_cool_time", 0 );

	kLoader.SetTitle("CriticalInfo");
	for( int i = 0; i < 500; i++ )
	{
		StringCbPrintf( szKey, sizeof( szKey ), "critical%d_multiple", i+1 );
		int iMultiple	= kLoader.LoadInt( szKey, 0 );

		if( 0 == iMultiple )
			break;

		if( m_iMaxCriticalValue < iMultiple )
			m_iMaxCriticalValue	= iMultiple;

		StringCbPrintf( szKey, sizeof( szKey ), "critical%d_rate", i+1 );
		int iRate		= kLoader.LoadInt( szKey, 0 );

		m_mCriticalInfo.insert( std::make_pair(iRate, iMultiple) );
	}

	m_mMapExcavationPos.clear();

	kLoader.SetTitle("ExcavatingRevalue");
	m_iReValuePeso	= kLoader.LoadInt( "revalue_first_price", 0 );
	m_fRevalueAddRate	= kLoader.LoadFloat( "revalue_price_add_rate", 0.0f );

	kLoader.SetTitle("ExcavationPos");
	for( int i = 0; i < 100; i++ )
	{
		CoordinateInfo stInfo;
		int iMapID	= 0;

		StringCbPrintf( szKey, sizeof( szKey ), "map%d_id", i+1 );
		iMapID	= kLoader.LoadInt( szKey, 0 );
		
		if( 0 == iMapID )
			break;

		for( int j = 0; j < 200; j++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "map%d_x%d", i+1, j+1 );
			stInfo.iX	= kLoader.LoadInt( szKey, 0 );

			StringCbPrintf( szKey, sizeof( szKey ), "map%d_y%d", i+1, j+1 );
			stInfo.iY	= kLoader.LoadInt( szKey, 0 );

			StringCbPrintf( szKey, sizeof( szKey ), "map%d_z%d", i+1, j+1 );
			stInfo.iZ	= kLoader.LoadInt( szKey, 0 );

			if( 0 == stInfo.iX && 0 == stInfo.iY && 0 == stInfo.iZ )
				break;

			MAPEXCAVATIONPOS::iterator it = m_mMapExcavationPos.find(iMapID);
			if( it != m_mMapExcavationPos.end() )
				(it->second).push_back(stInfo);
			else
			{
				COORDINATEINFO vVac;
				vVac.push_back(stInfo);
				m_mMapExcavationPos.insert(std::make_pair(iMapID, vVac));
			}
		}
		vMapID.push_back(iMapID);
	}

	//맵별 보상 정보 insert
	m_mMapRewardInfo.clear();
	m_mMapTotalRewardRand.clear();

	for( int i = 0; i < (int)vMapID.size(); i++ )
	{
		
		REWARDINFO vRewardVec;
		vRewardVec.clear();

		vRewardVec = mMapExcavationInfo[0];
			
		MAPREWARDINFO::iterator it = mMapExcavationInfo.find(vMapID[i]);
		if( it != mMapExcavationInfo.end() )
		{
			for( int j = 0; j < (int)(it->second).size(); j++ )
			{
				vRewardVec.push_back( (it->second)[j] );
			}
		}

		m_mMapRewardInfo.insert( std::make_pair(vMapID[i], vRewardVec) );

		int iRand = 0;
		iRand = mMapTotalRand[0] + mMapTotalRand[vMapID[i]];

		m_mMapTotalRewardRand.insert( std::make_pair(vMapID[i], iRand) );

	}
}

ioExcavationManager_Ex::ExcavationReward* ioExcavationManager_Ex::GetRewardType(int iMapID, User* pUser, int iMultiple)
{
	MAPTOTALRAND::iterator it = m_mMapTotalRewardRand.find(iMapID);

	if( it == m_mMapTotalRewardRand.end() )
		return NULL;

	MAPREWARDINFO::iterator it2 = m_mMapRewardInfo.find(iMapID);
	if( it2 == m_mMapRewardInfo.end() )
		return NULL;

	int iTotalRand = it->second;			// 맵에 따르는 확률  ?? 
	REWARDINFO& vInfo = it2->second;		// 맵 대응 보상

	DWORD dwRand		= m_ioRand.Random(iTotalRand);
	DWORD dwCurValue	= 0;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ioUserExcavation* pUser_EXCA = pUser->GetUserExcavation();
	pUser_EXCA->m_vExceptList.clear();
	pUser_EXCA->iReDecreaseRand = 0;

	for( int i = 0; i < (int)vInfo.size(); i++ )
	{
		ioExcavationManager_Ex::ExcavationReward* temp_info = &vInfo[i];
		
		if ( temp_info->m_iRewardType == PRESENT_ETC_ITEM )
		{
			ioEtcItem* pEtcItem =  g_EtcItemMgr.FindEtcItem( temp_info->m_iIndex ); 
			if( !pEtcItem )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::SetPresentEtcItem Fail - NULL - %s : %d",  pUser->GetPublicID().c_str(), temp_info->m_iIndex );
				continue;
			}

			///////////////////
			if( pUser != NULL
				&& COMPARE( temp_info->m_iIndex, ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_0001, ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_1000 + 1 )
				&& !pUser->IsGuild()
				&& !pUser->GetUserGuild()->IsActiveGuildRoom())
			{
				pUser_EXCA->m_vExceptList.push_back(temp_info->m_iIndex);
				pUser_EXCA->iReDecreaseRand = pUser_EXCA->iReDecreaseRand + temp_info->m_iRand;
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][excavation]GetRewardType accountIDX[%d]:지급제외1[%d],확률[%d]",pUser->GetUserIndex(),temp_info->m_iIndex,temp_info->m_iRand);
				continue;
			}
			///////////////////
			if( pUser != NULL
				&&	COMPARE(temp_info->m_iIndex, ioEtcItem::EIT_ETC_HOUSING_BLOCK_0001, ioEtcItem::EIT_ETC_HOUSING_BLOCK_1000 + 1 ) 
				&& !pUser->GetUserEtcItem()->HaveAThisItem(ioEtcItem::EIT_ETC_CREATE_HOME)	)		
			{	
				pUser_EXCA->m_vExceptList.push_back(temp_info->m_iIndex);
				pUser_EXCA->iReDecreaseRand = pUser_EXCA->iReDecreaseRand + temp_info->m_iRand;
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][excavation]GetRewardType accountIDX[%d]:지급제외2[%d],확률[%d]",pUser->GetUserIndex(),temp_info->m_iIndex,temp_info->m_iRand);
				continue;
			}
			///////////////////
			ioUserEtcItem::ETCITEMSLOT kEtcItemSlot; //가지고 있는 etcItem
			bool bExist = pUser->GetUserEtcItem()->GetEtcItem( temp_info->m_iIndex, kEtcItemSlot );

			bool bExistMortmain = false;
			if( bExist && g_EtcItemMgr.IsBlockEtcItem( temp_info->m_iIndex ) &&	kEtcItemSlot.m_iValue1 == 0 && kEtcItemSlot.m_iValue2 == 0 )
			{
				bExistMortmain = true;
			}

			int iMaxCheckResult = pUser->_OnEtcItemMaxCheck( pEtcItem, kEtcItemSlot, iMultiple, bExistMortmain ); 
			if( iMaxCheckResult != ETCITEM_BUY_OK )
			{
				pUser_EXCA->m_vExceptList.push_back(temp_info->m_iIndex);
				pUser_EXCA->iReDecreaseRand = pUser_EXCA->iReDecreaseRand + temp_info->m_iRand;
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][excavation]GetRewardType accountIDX[%d]:지급제외3[%d],확률[%d]",pUser->GetUserIndex(),temp_info->m_iIndex,temp_info->m_iRand);
				continue;
			}
		}
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][excavation]GetRewardType accountIDX[%d]:  지급대상현물아이템[%d],차감확률합산[%d]",pUser->GetUserIndex(),temp_info->m_iIndex,pUser_EXCA->iReDecreaseRand);
	}
	
	if ( iTotalRand != pUser_EXCA->iReDecreaseRand)
	{	
		dwRand	= m_ioRand.Random(iTotalRand - (pUser_EXCA->iReDecreaseRand));		// 제외되는 아이템의 확률값을 제외한 랜덤 확률 재계산 
	}

	for( int i = 0; i < (int)vInfo.size(); i++ )
	{	
		// 2018-01-31 by bckim, 보상 아이템 자격 확인  
		ioExcavationManager_Ex::ExcavationReward* temp = &vInfo[i];
		if( temp->m_iRewardType == PRESENT_ETC_ITEM)
		{	
			if( pUser != NULL && IsExceptIndex(pUser,temp->m_iIndex) )
			{
				continue;
			}	
		}
		// End. 2018-01-31 by bckim, 보상 아이템 자격 확인  

		if( COMPARE(dwRand, dwCurValue, dwCurValue+vInfo[i].m_iRand) )
		{
			return &vInfo[i];
		}

		dwCurValue += vInfo[i].m_iRand;
	}

	return NULL;
}

bool ioExcavationManager_Ex::IsExceptIndex( User* pUser, int iIndex)
{
	ioUserExcavation* pUser_EXCA = pUser->GetUserExcavation();
	for( int i=0;i < (int)(pUser_EXCA->m_vExceptList.size()); i ++  )
	{		
		if (pUser_EXCA->m_vExceptList[i] == iIndex )		
		{			
			return true;
		}
	}	
	return false;
}

ioExcavationManager_Ex::RewardGrade* ioExcavationManager_Ex::GetRewardGrade()
{
	DWORD dwRand		= m_ioRand.Random(m_iTotalRewardGradeRand);
	DWORD dwCurValue	= 0;

	for( int i = 0; i < (int)m_vRewardGradeInfo.size(); i++ )
	{
		if( COMPARE(dwRand, dwCurValue, dwCurValue+m_vRewardGradeInfo[i].m_iRand) )
			return &m_vRewardGradeInfo[i];

		dwCurValue += m_vRewardGradeInfo[i].m_iRand;
	}

	return NULL;
}

int ioExcavationManager_Ex::GetRewardGradeIndex(User* pUser)
{
	if( !pUser )
		return -1;

	int iGrade = -1;
	RewardGrade* pGrade = GetRewardGrade();

	if( pGrade )
		iGrade = pGrade->m_iIndex;

	return iGrade;
}

void ioExcavationManager_Ex::GetRewardInfo(int iMapID, UserExcavationRewardInfo& stReward, User* pUser)
{
	if( !pUser )
		return;

	int iMultiple = GetCriticalMultiple();
	ExcavationReward* pRewardInfo = GetRewardType(iMapID,pUser,iMultiple);
	if( !pRewardInfo )
		return;

	// 이벤트 보상
	float fPrice = (float)pRewardInfo->m_iPrice;		// + ((float)pRewardInfo->m_iPrice * (fSellPesoBonus/100));

	int iGrade			= -1;
	float fGradeValue	= 0.0f;
	
	RewardGrade* pGrade = GetRewardGrade();
	if( pGrade )
	{
		iGrade = pGrade->m_iIndex;
		fGradeValue	= pGrade->m_dGradeValue;
	}

	if( pRewardInfo->m_iRewardType != PRESENT_ETC_ITEM && fGradeValue > 0.0f )		// 2018-02-23 by bckim, 탐사 확장 아이텝 지급 수량 개선    // 현물 아이템일때 price는 지급 개수 
		fPrice = fPrice * fGradeValue;

	stReward.m_iRewardType	= pRewardInfo->m_iRewardType;		// 2018-01-15 by bckim, 탐사 확장
	stReward.m_iIndex		= pRewardInfo->m_iIndex;
	stReward.m_iType		= pRewardInfo->m_iType;
	stReward.m_iGrade		= iGrade;
	stReward.m_iPrice		= (int)fPrice;
	stReward.bRoomAlarm		= pRewardInfo->bRoomAlarm;
	stReward.bWholeAlarm	= pRewardInfo->bWholeAlarm;
	stReward.m_iMultiple	= iMultiple; // GetCriticalMultiple();
}

void ioExcavationManager_Ex::GetExcavationPos(int iMapID, CoordinateInfo& stReardInfo)
{
	MAPEXCAVATIONPOS::iterator it	= m_mMapExcavationPos.find(iMapID);
	if( it == m_mMapExcavationPos.end() )
		return;
	
	COORDINATEINFO& vCoordinateVec = it->second;

	int iIndex	= m_ioRand.Random(vCoordinateVec.size());

	if( iIndex < 0 || iIndex >= (int)vCoordinateVec.size() )
		return;

	stReardInfo = vCoordinateVec[iIndex];
}

float ioExcavationManager_Ex::GetGradeValue(int iIndex)
{
	for( int i = 0; i < (int)m_vRewardGradeInfo.size(); i++ )
	{
		if( m_vRewardGradeInfo[i].m_iIndex == iIndex )
			return m_vRewardGradeInfo[i].m_dGradeValue;
	}

	return 0.0f;
}

int ioExcavationManager_Ex::GetGradeRate(int iIndex)
{
	for( int i = 0; i < (int)m_vRewardGradeInfo.size(); i++ )
	{
		if( m_vRewardGradeInfo[i].m_iIndex == iIndex )
			return m_vRewardGradeInfo[i].m_iRand;
	}

	return 0;
}

BOOL ioExcavationManager_Ex::CheckTimeOut(DWORD dwStartTime)
{
	DWORD dwCurTime	= GetTickCount();

	if( dwCurTime >= dwStartTime + (GetTimeOutSec() * 1000 ) )
		return TRUE;

	return FALSE;
}

void ioExcavationManager_Ex::SetExcavationEventGrade( EventGradeMap& mapEventGrade )
{
	if( mapEventGrade.size() == 0 )
		return;

	m_mapExcavationEventGradeList.clear();

	int iBasicEventGrade = 0; // 이벤트가 걸려있느 등급들의 총 원래 확률
	int iTotalEventGrade = 0; // 이벤트가 걸려있는 등급들의 총 이벤트 확률
	EventGradeMap::iterator iter,iEnd;
	iEnd = mapEventGrade.end();
	for(iter = mapEventGrade.begin() ; iter != iEnd ; iter++)
	{
		EventGrade &kGrade = (*iter).second;
		iTotalEventGrade += kGrade.m_iGradeBuffValue;
		iBasicEventGrade += GetGradeRate(kGrade.m_iGradeIndex);
	}

	int iTotalNormalGrade = m_iTotalRewardGradeRand - iTotalEventGrade; // 이벤트 아닌 등급들의 이벤트 확률을 제외한 총 확률
	int iTotalBasicGrade = m_iTotalRewardGradeRand - iBasicEventGrade; // 이벤트 아닌 등급들의 원래 총 확률

	for( int i = 0; i < (int)m_vRewardGradeInfo.size(); i++ )
	{
		ExcavationEventGrade ExcavationGrade;
		ExcavationGrade.m_iGradeIndex = m_vRewardGradeInfo[i].m_iIndex;
		EventGradeMap::iterator iter = mapEventGrade.find( m_vRewardGradeInfo[i].m_iIndex );
		if( iter == mapEventGrade.end() )
		{
			ExcavationGrade.m_iGradeBuffValue = min( (m_vRewardGradeInfo[i].m_iRand * iTotalNormalGrade / iTotalBasicGrade), iTotalNormalGrade );
			m_mapExcavationEventGradeList.insert( make_pair(ExcavationGrade.m_iGradeIndex, ExcavationGrade) );
		}
		else
		{
			EventGrade& rGrade = (*iter).second;
			ExcavationGrade.m_iGradeBuffValue = rGrade.m_iGradeBuffValue;
			m_mapExcavationEventGradeList.insert( make_pair(ExcavationGrade.m_iGradeIndex, ExcavationGrade) );
		}
	}
}

int ioExcavationManager_Ex::GetCriticalMultiple()
{
	int iRand = m_ioRand.Random(100);

	CRITICALINFO::iterator it	= m_mCriticalInfo.begin();

	while( it != m_mCriticalInfo.end() )
	{
		if( iRand < it->first )
			return it->second;

		it++;
	}

	return 1;
}

int ioExcavationManager_Ex::GetCoolTime(const int iLevel)
{
	int iDefaultCoolTime	= GetDefaultCoolTime();
	int iDecreaseCoolTime	= GetDecreaseCoolTimeValue(iLevel);

	int iResult	= iDefaultCoolTime - iDecreaseCoolTime;
	if( iResult < GetMinimumCoolTime() )
		iResult = GetMinimumCoolTime();

	return iResult;
}

int	ioExcavationManager_Ex::GetDecreaseCoolTimeValue(const int iLevel)
{
	return ( iLevel / GetDecreaseLevelGap() ) * GetDecreaseCoolTime();
}
