
#include "stdafx.h"

#include "ioItemCompoundManager.h"

#include "ioUserExtraItem.h"

template<> ioItemCompoundManager* Singleton< ioItemCompoundManager >::ms_Singleton = 0;

ioItemCompoundManager::ioItemCompoundManager()
{
	ClearAllInfo();
}

ioItemCompoundManager::~ioItemCompoundManager()
{
	ClearAllInfo();
}

void ioItemCompoundManager::LoadCompoundInfo()
{
	ClearAllInfo();

	ioINILoader_e kLoader( "config/sp2_item_compound_info.ini" );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	kLoader.SetTitle_e( "common_info" );

	m_iNeedLevel = (int)kLoader.LoadInt_e( "need_level", 0 );
	m_bEnableSamePart = (bool)kLoader.LoadBool_e( "enable_same_part", false );
	m_bEnableSameCode = (bool)kLoader.LoadBool_e( "enable_same_code", false );

	m_iMaxReinforce = (int)kLoader.LoadInt_e( "max_reinforce", 25 );
	m_iRoomAlarmLevel = (int)kLoader.LoadInt_e( "room_alarm_level", 0 );

	int iMaxCnt = kLoader.LoadInt_e( "max_info_cnt", 0 );
	if( iMaxCnt > 0 )
	{
		int iTemp = 1;
		while( 1 )
		{
			wsprintf_e( szKey, "compound_rate%d", iTemp );
			kLoader.SetTitle( szKey );

			DWORD dwCode = (DWORD)kLoader.LoadInt_e( "compound_item_code", 0 );
			if( dwCode == 0 )
				break;

			iTemp++;

			CompoundInfo kCompoundInfo;

			kCompoundInfo.m_iSmallUpReinforce = (int)kLoader.LoadInt_e( "small_up_reinforce", 0 );
			kCompoundInfo.m_iBigUpReinforce = (int)kLoader.LoadInt_e( "big_up_reinforce", 0 );
			kCompoundInfo.m_iDownReinforce = (int)kLoader.LoadInt_e( "down_reinforce", 0 );

			kCompoundInfo.m_vRateInfoList.reserve( iMaxCnt );
			for( int i=0; i < iMaxCnt; ++i )
			{
				CompoundRateInfo kRateInfo;

				wsprintf_e( szKey, "compound%d_level", i+1 );
				kRateInfo.m_iLevel = kLoader.LoadInt( szKey, 0 );

				wsprintf_e( szKey, "compound%d_same_rate", i+1 );
				kRateInfo.m_iSameItemRate = kLoader.LoadInt( szKey, 0 );

				wsprintf_e( szKey, "compound%d_same_special_rate", i+1 );
				kRateInfo.m_iSameItemRateS = kLoader.LoadInt( szKey, 0 );

				wsprintf_e( szKey, "compound%d_other_rate", i+1 );
				kRateInfo.m_iOtherItemRate = kLoader.LoadInt( szKey, 0 );

				wsprintf_e( szKey, "compound%d_other_special_rate", i+1 );
				kRateInfo.m_iOtherItemRateS = kLoader.LoadInt( szKey, 0 );

				kCompoundInfo.m_vRateInfoList.push_back( kRateInfo );
			}

			m_CompoundInfoMap.insert( CompoundInfoMap::value_type( dwCode, kCompoundInfo ) );
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	kLoader.SetTitle_e( "multiple_compound" );

	int iIconCnt = kLoader.LoadInt_e( "icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vCompoundItemIconList.reserve( iIconCnt );

		for( int i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			ioHashString szIconName = szBuf;

			m_vCompoundItemIconList.push_back( szIconName );
		}
	}
}

void ioItemCompoundManager::ClearAllInfo()
{
	ClearCompoundInfoMap();
}

void ioItemCompoundManager::ClearCompoundInfoMap()
{
	CompoundInfoMap::iterator iCreate;
	for(iCreate = m_CompoundInfoMap.begin();iCreate != m_CompoundInfoMap.end();iCreate++)
	{
		CompoundInfo &rkInfo = iCreate->second;
		rkInfo.m_vRateInfoList.clear();
	}

	m_CompoundInfoMap.clear();
}

ioItemCompoundManager& ioItemCompoundManager::GetSingleton()
{
	return Singleton< ioItemCompoundManager >::GetSingleton();
}

int ioItemCompoundManager::GetCompoundSuccessRate( int iTargetSlot, int iVictimSlot, CompoundType eType, DWORD dwCode )
{
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem ) return 0;

	EXTRAITEMSLOT kTargetSlot;
	if( !pExtraItem->GetExtraItem( iTargetSlot, kTargetSlot) )
		return 0;

	EXTRAITEMSLOT kVictimSlot;
	if( !pExtraItem->GetExtraItem( iVictimSlot, kVictimSlot) )
		return 0;

	int iTargetReinforce = kTargetSlot.m_iReinforce;

	bool bSame = false;
	if( kTargetSlot.m_iItemCode == kVictimSlot.m_iItemCode )
		bSame = true;

	int iCurRate = 0;
	int iCurSpecialRate = 0;

	CompoundInfoMap::iterator iter = m_CompoundInfoMap.find( dwCode );
	if( iter != m_CompoundInfoMap.end() )
	{
		CompoundInfo &rkInfo = iter->second;

		int iMaxCnt = rkInfo.m_vRateInfoList.size();
		for( int i=0; i < iMaxCnt; ++i )
		{
			if( rkInfo.m_vRateInfoList[i].m_iLevel == iTargetReinforce )
			{
				if( bSame )
				{
					iCurRate = rkInfo.m_vRateInfoList[i].m_iSameItemRate;
					iCurSpecialRate = rkInfo.m_vRateInfoList[i].m_iSameItemRateS;
				}
				else
				{
					iCurRate = rkInfo.m_vRateInfoList[i].m_iOtherItemRate;
					iCurSpecialRate = rkInfo.m_vRateInfoList[i].m_iOtherItemRateS;
				}
				break;
			}
		}
	}

	if( eType == CMT_SMALL )
		return iCurRate;
	else if( eType == CMT_BIG )
		return iCurSpecialRate;

	return 0;
}

int ioItemCompoundManager::GetCompoundReinforce( CompoundType eType, DWORD dwCode )
{
	int iSmallUpReinforce = 0;
	int iBigUpReinforce = 0;
	int iDownReinforce = 0;

	CompoundInfoMap::iterator iter = m_CompoundInfoMap.find( dwCode );
	if( iter != m_CompoundInfoMap.end() )
	{
		CompoundInfo &rkInfo = iter->second;

		iSmallUpReinforce = rkInfo.m_iSmallUpReinforce;
		iBigUpReinforce = rkInfo.m_iBigUpReinforce;
		iDownReinforce = rkInfo.m_iDownReinforce;
	}

	if( eType == CMT_SMALL )
		return iSmallUpReinforce;
	else if( eType == CMT_BIG )
		return iBigUpReinforce;

	return iDownReinforce;
}