#include "StdAfx.h"
#include "ioUserAccessory.h"
#include "ioAccessoryInfoManager.h"
#include "ioDateHelp.h"

ioHashString AccessorySlot::GetName()
{
	if( m_iCompoundCode == 0 )
		return m_Name;

	char szName[MAX_PATH] = "";
	AccessoryCompoundInfo cInfo = g_AccessoryInfoMgr.FindCompoundInfo( m_iCompoundCode );
	
	wsprintf_e( szName, "%s %s", cInfo.m_szName.c_str(), m_Name.c_str() );
	return szName;
}

ioHashString AccessorySlot::GetNameAdd()
{
	if( m_iCompoundCode == 0 )
		return ioHashString();

	char szName[MAX_PATH] = "";
	AccessoryCompoundInfo cInfo = g_AccessoryInfoMgr.FindCompoundInfo( m_iCompoundCode );
	
	wsprintf( szName, "%s ", cInfo.m_szName.c_str() );
	return szName;
}

ioUserAccessory::ioUserAccessory(void)
{
	m_AccessorySlotList.clear();	
}

ioUserAccessory::~ioUserAccessory(void)
{
	m_AccessorySlotList.clear();
}

void ioUserAccessory::ApplyAccessorySlotData( SP2Packet &rkPacket )
{
	m_AccessorySlotList.clear();

	int nSize;
	rkPacket >> nSize;

	for( int i=0; i<nSize; ++i )
	{
		int nEquipClassType = 0;
		AccessorySlot sAccessorySlot;

		rkPacket >> sAccessorySlot.m_iIndex>> sAccessorySlot.m_iAccessoryCode;
		rkPacket >> sAccessorySlot.m_PeriodType;
		rkPacket >> sAccessorySlot.m_iValue1 >> sAccessorySlot.m_iValue2;
		rkPacket >> nEquipClassType;
		rkPacket >> sAccessorySlot.m_iRandomNumber;
		rkPacket >> sAccessorySlot.m_iCompoundCode;
		rkPacket >> sAccessorySlot.m_iCompoundValue;

		const ioItem *pItem = g_ItemMaker.GetItemConst( sAccessorySlot.m_iAccessoryCode, __FUNCTION__ );
		if( pItem )
			sAccessorySlot.m_Name = pItem->GetName();
		sAccessorySlot.m_IconName = g_ItemMaker.GetItemLargeIcon( sAccessorySlot.m_iAccessoryCode, 0 );

		if ( sAccessorySlot.m_Name.IsEmpty() || sAccessorySlot.m_IconName.IsEmpty() )
			continue;

		if ( nEquipClassType > 0 )
		{
			sAccessorySlot.m_bEquip = true;

			int nCharArray = g_MyInfo.GetClassArray( nEquipClassType );
			int nSlot = sAccessorySlot.m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE - 1;
			g_MyInfo.EquipAccessoryInit( nCharArray, nSlot, sAccessorySlot );
		}

		m_AccessorySlotList.push_back( sAccessorySlot );
	}

	std::sort( m_AccessorySlotList.begin(), m_AccessorySlotList.end(), AccessorySlotSort() );
}

int ioUserAccessory::GetAccessorySlotCnt()
{
	return (int)m_AccessorySlotList.size();
}

bool ioUserAccessory::GetAccessorySlotArray( int iArray, AccessorySlot &sAccessorySlot )
{
	if ( m_AccessorySlotList.empty() )
		return false;

	if( COMPARE( iArray, 0, (int)m_AccessorySlotList.size() ) )
	{
		sAccessorySlot = m_AccessorySlotList[iArray];
		return true;
	}

	return false;
}

bool ioUserAccessory::GetAccessorySlot( int iSlotIndex, AccessorySlot &sAccessorySlot )
{
	if ( m_AccessorySlotList.empty() )
		return false;

	int nSize = (int)m_AccessorySlotList.size();
	for( int i=0; i < nSize; ++i )
	{
		if( m_AccessorySlotList[i].m_iIndex == iSlotIndex )
		{
			sAccessorySlot = m_AccessorySlotList[i];
			return true;
		}
	}

	return false;
}

bool ioUserAccessory::GetUseInfo( int iSlotIndex, ioHashString &rsInfo )
{
	bool bActive = true;
	AccessorySlot kSlot;
	if( !GetAccessorySlot( iSlotIndex, kSlot ) )
		return false;

	char szText[MAX_PATH]="";
	if( kSlot.m_PeriodType == ACCPT_MORTMAIN )
		SafeSprintf( szText, sizeof( szText ), "영구사용" );
	else 
		bActive = Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szText, sizeof( szText ), true );

	rsInfo = szText;
	return bActive;
}

bool ioUserAccessory::IsSlotFull()
{
	if( GetAccessorySlotCnt() >= MAX_SLOT_CNT )
		return true;
	return false;
}

int ioUserAccessory::GetAccessoryCode( int iSlotIndex )
{
	if ( m_AccessorySlotList.empty() )
		return 0;

	int nSize = (int)m_AccessorySlotList.size();
	for( int i=0; i < nSize; ++i )
	{
		if( m_AccessorySlotList[i].m_iIndex == iSlotIndex )
			return  m_AccessorySlotList[i].m_iAccessoryCode;
	}

	return 0;
}

void ioUserAccessory::SetAccessoryEquip( int iSlotIndex, bool bEquip )
{
	int nSize = (int)m_AccessorySlotList.size();
	for( int i=0; i<nSize; ++i )
	{
		if( m_AccessorySlotList[i].m_iIndex == iSlotIndex )
			m_AccessorySlotList[i].m_bEquip = bEquip;
	}
}

void ioUserAccessory::AddAccessorySlotData( const AccessorySlot &sNewSlot )
{
	if ( IsSlotFull() )
		return;

	AccessorySlot sAccessorySlot = sNewSlot;
	const ioItem *pItem = g_ItemMaker.GetItemConst( sNewSlot.m_iAccessoryCode, __FUNCTION__ );
	if( pItem )
		sAccessorySlot.m_Name = pItem->GetName();
	sAccessorySlot.m_IconName = g_ItemMaker.GetItemLargeIcon( sNewSlot.m_iAccessoryCode, 0 );

	m_AccessorySlotList.push_back( sAccessorySlot );
}

bool ioUserAccessory::DeleteAccessorySlotData( int iSlotIndex )
{
	AccessorySlotList::iterator iter = m_AccessorySlotList.begin();
	while( iter != m_AccessorySlotList.end() )
	{
		if( iter->m_iIndex == iSlotIndex )
		{
			iter = m_AccessorySlotList.erase( iter );
			return true;
		}
		else
			++iter;
	}

	return false;
}

void ioUserAccessory::SetAccessorySlot( const AccessorySlot &sAccessorySlot )
{
	int nSize = (int)m_AccessorySlotList.size();
	for( int i=0; i < nSize; ++i )
	{
		if( m_AccessorySlotList[i].m_iIndex == sAccessorySlot.m_iIndex )
		{
			m_AccessorySlotList[i] = sAccessorySlot;
			return;
		}
	}
}

void ioUserAccessory::SetAccessoryCompoundInfo( int iSlotIndex, int iCompoundCode, int iCompoundValue )
{
	int nSize = (int)m_AccessorySlotList.size();
	for( int i=0; i < nSize; ++i )
	{
		if( m_AccessorySlotList[i].m_iIndex == iSlotIndex )
		{
			m_AccessorySlotList[i].m_iCompoundCode = iCompoundCode;
			m_AccessorySlotList[i].m_iCompoundValue = iCompoundValue;
			return;
		}
	}
}

void ioUserAccessory::SetAccessoryReinforceInfo( int iAccessoryIndex, int iRandomValue, int iValue1, int iValue2 )
{
	int nSize = (int)m_AccessorySlotList.size();
	for( int i=0; i < nSize; ++i )
	{
		if( m_AccessorySlotList[i].m_iIndex == iAccessoryIndex )
		{
			m_AccessorySlotList[i].m_iRandomNumber = iRandomValue;
			m_AccessorySlotList[i].m_iValue1 = iValue1;
			m_AccessorySlotList[i].m_iValue2 = iValue2;
			return;
		}
	}
}