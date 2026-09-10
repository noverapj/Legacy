
#include "stdafx.h"

#include "ioGlobalSoldierInfo.h"

template<> ioGlobalSoldierInfo* Singleton< ioGlobalSoldierInfo >::ms_Singleton = 0;

ioGlobalSoldierInfo::ioGlobalSoldierInfo()
{
	m_DefCharInfo.m_class_type = 0;
	m_DefCharInfo.m_kindred	   = 0;
	m_DefCharInfo.m_sex		   = 0;
	m_DefCharInfo.m_beard	   = 1;
	m_DefCharInfo.m_face	   = 1;
	m_DefCharInfo.m_hair	   = 1;
	m_DefCharInfo.m_skin_color = 1;
	m_DefCharInfo.m_hair_color = 1;
	m_DefCharInfo.m_accessories= 1;
	m_DefCharInfo.m_underwear  = 1;
	for (int i = 0; i < MAX_INVENTORY ; i++)
		 m_DefCharInfo.m_extra_item[i] = 0;
	m_DefCharInfo.m_iLimitSecond = 0;
	m_DefCharInfo.m_bActive      = true;
}

ioGlobalSoldierInfo::~ioGlobalSoldierInfo()
{	
	m_vSoldierMap.clear();
	m_vSoldierItemMap.clear();
	m_vAwardSoldierMap.clear();
}

ioGlobalSoldierInfo& ioGlobalSoldierInfo::GetSingleton()
{
	return Singleton< ioGlobalSoldierInfo >::GetSingleton();
}

CHARACTER ioGlobalSoldierInfo::GetSoldierData( const ioHashString &rkName, DWORD dwCharIndex, int iClassType, int iKindred, int iSex )
{
	CHARACTER kReturnInfo = m_DefCharInfo;

	// 기본이 되는 정보는 서버에서 받았음.
	kReturnInfo.m_class_type = iClassType;
	kReturnInfo.m_kindred    = iKindred;
	kReturnInfo.m_sex        = iSex;
	
	if( rkName == g_MyInfo.GetPublicID() )
	{
		int iCharArray = g_MyInfo.GetCharArray( dwCharIndex );
		if( iCharArray != -1 )
		{
			const CHARACTER &kCharInfo = g_MyInfo.GetCharacter( iCharArray );
			kReturnInfo = kCharInfo;

			// 시상식에 표시할 용병 정보( 자신의 용병도 출전한 용병만 허용 )
			SoldierDecMap::iterator iter = m_vAwardSoldierMap.find( dwCharIndex );
			if( iter != m_vAwardSoldierMap.end() )
			{
				if( kCharInfo.m_class_type == 0 )
				{
					m_vAwardSoldierMap.erase( iter );
				}
				else
				{
					CHARACTER &rkCharInfo = iter->second;
					rkCharInfo = kCharInfo;
				}
			}
			else if( kCharInfo.m_class_type != 0 )
			{
				m_vAwardSoldierMap.insert( SoldierDecMap::value_type( dwCharIndex, kCharInfo ) );
			}
		}
	}
	else
	{
		SoldierDecMap::iterator iter = m_vSoldierMap.find( dwCharIndex );
		if( iter != m_vSoldierMap.end() )
		{
			CHARACTER &kCharInfo = iter->second;
			if( kReturnInfo.m_class_type == kCharInfo.m_class_type )
			{
				kReturnInfo.m_beard	     = kCharInfo.m_beard;
				kReturnInfo.m_face	     = kCharInfo.m_face;
				kReturnInfo.m_hair	     = kCharInfo.m_hair;
				kReturnInfo.m_skin_color = kCharInfo.m_skin_color;
				kReturnInfo.m_hair_color = kCharInfo.m_hair_color;
				kReturnInfo.m_accessories= kCharInfo.m_accessories;
				kReturnInfo.m_underwear  = kCharInfo.m_underwear;

				for ( int i=0; i<MAX_INVENTORY; i++ )
					kReturnInfo.m_EquipCostume[i] = kCharInfo.m_EquipCostume[i];
			}
		}
	}
	return kReturnInfo;
}

CHARACTER ioGlobalSoldierInfo::GetSoldierData( const ioHashString &rkName, DWORD dwCharIndex )
{
	CHARACTER kReturnInfo;
	if( rkName == g_MyInfo.GetPublicID() )
	{
		int iCharArray = g_MyInfo.GetCharArray( dwCharIndex );
		if( iCharArray != -1 )
		{
			const CHARACTER &kCharInfo = g_MyInfo.GetCharacter( iCharArray );
			kReturnInfo = kCharInfo;		
		}
	}
	else
	{
		SoldierDecMap::iterator iter = m_vSoldierMap.find( dwCharIndex );
		if( iter != m_vSoldierMap.end() )
		{
			const CHARACTER &kCharInfo = iter->second;
			kReturnInfo = kCharInfo;
		}
	}
	return kReturnInfo;
}

DWORD ioGlobalSoldierInfo::GetSoldierItemCode( const ioHashString &rkName, DWORD dwCharIndex, int iSlot )
{
	if( rkName == g_MyInfo.GetPublicID() )
	{
		int iCharArray = g_MyInfo.GetCharArray( dwCharIndex );
		if( iCharArray != -1 )
		{
			return g_MyInfo.GetCharItem( iCharArray, iSlot ).m_item_code;
		}
	}
	else
	{
		SoldierItemMap::iterator iter = m_vSoldierItemMap.find( dwCharIndex );
		if( iter != m_vSoldierItemMap.end() )
		{
			ItemCode &kItemCode = iter->second;
			if( COMPARE( iSlot, 0, MAX_INVENTORY ) )
				return kItemCode.m_dwItemCode[iSlot];
		}
	}
	return 0;
}

DWORD ioGlobalSoldierInfo::GetSoldierItemMaleCustom( const ioHashString &rkName, DWORD dwCharIndex, int iSlot )
{
	if( rkName == g_MyInfo.GetPublicID() )
	{
		int iCharArray = g_MyInfo.GetCharArray( dwCharIndex );
		if( iCharArray != -1 )
		{
			return g_MyInfo.GetCharItem( iCharArray, iSlot ).m_item_male_custom;
		}
	}
	else
	{
		SoldierItemMap::iterator iter = m_vSoldierItemMap.find( dwCharIndex );
		if( iter != m_vSoldierItemMap.end() )
		{
			ItemCode &kItemCode = iter->second;
			if( COMPARE( iSlot, 0, MAX_INVENTORY ) )
				return kItemCode.m_dwItemMaleCustom[iSlot];
		}
	}
	return 0;
}

DWORD ioGlobalSoldierInfo::GetSoldierItemFemaleCustom( const ioHashString &rkName, DWORD dwCharIndex, int iSlot )
{
	if( rkName == g_MyInfo.GetPublicID() )
	{
		int iCharArray = g_MyInfo.GetCharArray( dwCharIndex );
		if( iCharArray != -1 )
		{
			return g_MyInfo.GetCharItem( iCharArray, iSlot ).m_item_female_custom;
		}
	}
	else
	{
		SoldierItemMap::iterator iter = m_vSoldierItemMap.find( dwCharIndex );
		if( iter != m_vSoldierItemMap.end() )
		{
			ItemCode &kItemCode = iter->second;
			if( COMPARE( iSlot, 0, MAX_INVENTORY ) )
				return kItemCode.m_dwItemFemaleCustom[iSlot];
		}
	}
	return 0;
}

CHARACTER ioGlobalSoldierInfo::GetAwardRandSoldier( int iSeed )
{
	CHARACTER kReturnInfo = m_DefCharInfo;
	
	// Default
	IntVec vClassType;
	vClassType.reserve(10);
	int iSize = g_ClassPrice.MaxClassPrice();
	for (int i = 0; i < iSize ; i++)
	{
		if( !g_ClassPrice.IsActiveClass( i ) )
			continue;
		vClassType.push_back( i + 1 );
	}
	int iRand = iSeed%vClassType.size();
	if( COMPARE( iRand , 0, (int) vClassType.size() ) )
		kReturnInfo.m_class_type = vClassType[iRand];
	vClassType.clear();

	kReturnInfo.m_kindred		= 1;
	kReturnInfo.m_sex			= 1;

	// 수집한 정보가 있으면 랜덤으로 선택
	if( !m_vAwardSoldierMap.empty() )
	{
		int iSize   = m_vAwardSoldierMap.size();
		int iRandom = iSeed%max( iSize, 1 );
		SoldierDecMap::iterator iter;
		for( iter = m_vAwardSoldierMap.begin() ; iter != m_vAwardSoldierMap.end() ; ++iter )
		{
			iRandom--;
			if( iRandom < 0 )
			{
				kReturnInfo = iter->second;
				break;
			}
		}
	}
	return kReturnInfo;
}

void ioGlobalSoldierInfo::SyncSoldierData( DWORD dwCharIndex, CHARACTER kCharInfo )
{
	if( dwCharIndex == 0 ) 
		return;

	{
		// 용병 정보 수집
		SoldierDecMap::iterator iter = m_vSoldierMap.find( dwCharIndex );
		if( iter != m_vSoldierMap.end() )
		{
			CHARACTER &rkCharInfo = iter->second;
			rkCharInfo = kCharInfo;
		}
		else
		{
			m_vSoldierMap.insert( SoldierDecMap::value_type( dwCharIndex, kCharInfo ) );
		}
	}

	{
		// 시상식에 표시할 용병 정보
		SoldierDecMap::iterator iter = m_vAwardSoldierMap.find( dwCharIndex );
		if( iter != m_vAwardSoldierMap.end() )
		{
			if( kCharInfo.m_class_type == 0 )
			{
				m_vAwardSoldierMap.erase( iter );
			}
			else
			{
				CHARACTER &rkCharInfo = iter->second;
				rkCharInfo = kCharInfo;
			}
		}
		else if( kCharInfo.m_class_type != 0 )
		{
			m_vAwardSoldierMap.insert( SoldierDecMap::value_type( dwCharIndex, kCharInfo ) );
		}
	}
}

void ioGlobalSoldierInfo::SyncSoldierItem( DWORD dwCharIndex, const ITEM_DATA* pItem )
{
	if( dwCharIndex == 0 ) 
		return;

	{
		// 용병 아이템 수집
		SoldierItemMap::iterator iter = m_vSoldierItemMap.find( dwCharIndex );
		if( iter != m_vSoldierItemMap.end() )
		{
			ItemCode &kItemCode = iter->second;
			for(int i = 0;i < MAX_INVENTORY;i++)
			{
				ITEM_DATA kItem = pItem[i];
				if( IsExceptionSyncItem( kItem.m_item_code ) ) continue;

				kItemCode.m_dwItemCode[i] = kItem.m_item_code;
				kItemCode.m_dwItemMaleCustom[i] = kItem.m_item_male_custom;
				kItemCode.m_dwItemFemaleCustom[i] = kItem.m_item_female_custom;
			}
		}
		else
		{
			ItemCode kItemCode;
			for(int i = 0;i < MAX_INVENTORY;i++)
			{
				ITEM_DATA kItem = pItem[i];
				if( IsExceptionSyncItem( kItem.m_item_code ) ) continue;

				kItemCode.m_dwItemCode[i] = kItem.m_item_code;
				kItemCode.m_dwItemMaleCustom[i] = kItem.m_item_male_custom;
				kItemCode.m_dwItemFemaleCustom[i] = kItem.m_item_female_custom;
			}
			m_vSoldierItemMap.insert( SoldierItemMap::value_type( dwCharIndex, kItemCode ) );
		}
	}
}

bool ioGlobalSoldierInfo::IsExceptionSyncItem( DWORD dwItemCode )
{
	// 강시 아이템은 동기화 시키지 않는다.
	switch( dwItemCode )
	{
	case 11011:
	case 111011:
	case 211011:
	case 311011:
	case 11012:
	case 111012:
	case 211012:
	case 311012:
		return true;
	}

	return false;
}

void ioGlobalSoldierInfo::ClearAwardSoldierMap()
{
	m_vAwardSoldierMap.clear();
}