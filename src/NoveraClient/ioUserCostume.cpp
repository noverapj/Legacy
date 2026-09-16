
#include "stdafx.h"

#include "ioUserCostume.h"

ioUserCostume::ioUserCostume()
{
	m_CostumeSlotList.clear();
}

ioUserCostume::~ioUserCostume()
{
	m_CostumeSlotList.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//µ¿±âÈ­
void ioUserCostume::ApplyCostumeSlotData( SP2Packet &rkPacket )
{
	m_CostumeSlotList.clear();

	int nSize;
	rkPacket >> nSize;

	for( int i=0; i<nSize; ++i )
	{
		int nEquipClassType = 0;
		CostumeSlot sCostumeSlot;

		rkPacket >> sCostumeSlot.m_nIndex >> sCostumeSlot.m_nCostumeCode;
		rkPacket >> sCostumeSlot.m_PeriodType;
		rkPacket >> sCostumeSlot.m_iValue1 >> sCostumeSlot.m_iValue2 
				 >> sCostumeSlot.m_dwMaleCustom >> sCostumeSlot.m_dwFemaleCustom;

		rkPacket >> nEquipClassType;

		if ( nEquipClassType > 0 )
		{
			sCostumeSlot.m_bEquip = true;

			int nCharArray = g_MyInfo.GetClassArray( nEquipClassType );
			int nSlot = sCostumeSlot.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE;
			g_MyInfo.EquipCostumeInit( nCharArray, nSlot, sCostumeSlot );
		}
		
		sCostumeSlot.m_Name = g_CostumeInfoMgr.GetCostumeName( sCostumeSlot.m_nCostumeCode );
		sCostumeSlot.m_IconName = g_CostumeInfoMgr.GetCostumeIcon( sCostumeSlot.m_nCostumeCode );
		
		if ( sCostumeSlot.m_Name.IsEmpty() || sCostumeSlot.m_IconName.IsEmpty() )
			continue;

		m_CostumeSlotList.push_back( sCostumeSlot );
	}

	std::sort( m_CostumeSlotList.begin(), m_CostumeSlotList.end(), CostumeSlotSort() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//½½·Ô Ãß°¡
void ioUserCostume::AddCostumeSlotData( const CostumeSlot &sNewSlot )
{	
	if ( IsSlotFull() )
		return;

	CostumeSlot sCostumeSlot = sNewSlot;
	sCostumeSlot.m_Name = g_CostumeInfoMgr.GetCostumeName( sNewSlot.m_nCostumeCode );
	sCostumeSlot.m_IconName = g_CostumeInfoMgr.GetCostumeIcon( sNewSlot.m_nCostumeCode );

	m_CostumeSlotList.push_back( sCostumeSlot );
}

//½½·Ô »èÁ¦
bool ioUserCostume::DeleteCostumeSlotData( int nSlotIndex )
{
	CostumeSlotList::iterator iter = m_CostumeSlotList.begin();
	while( iter != m_CostumeSlotList.end() )
	{
		if( iter->m_nIndex == nSlotIndex )
		{
			iter = m_CostumeSlotList.erase( iter );
			return true;
		}
		else
			++iter;
	}

	return false;
}

//½½·Ô º¯°æ
void ioUserCostume::SetCostumeSlot( const CostumeSlot &sCostumeSlot )
{
	int nSize = (int)m_CostumeSlotList.size();
	for( int i=0; i < nSize; ++i )
	{
		if( m_CostumeSlotList[i].m_nIndex == sCostumeSlot.m_nIndex )
		{
			m_CostumeSlotList[i] = sCostumeSlot;
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//ÀåÂø ¼³Á¤
void ioUserCostume::SetCostumeEquip( int nSlotIndex, bool bEquip )
{
	int nSize = (int)m_CostumeSlotList.size();
	for( int i=0; i<nSize; ++i )
	{
		if( m_CostumeSlotList[i].m_nIndex == nSlotIndex )
			m_CostumeSlotList[i].m_bEquip = bEquip;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//½½·Ô ¾ò±â(½½·Ô ÀÎµ¦½º)
bool ioUserCostume::GetCostumeSlot( int nSlotIndex, CostumeSlot &sCostumeSlot )
{
	if ( m_CostumeSlotList.empty() )
		return false;

	int nSize = (int)m_CostumeSlotList.size();
	for( int i=0; i < nSize; ++i )
	{
		if( m_CostumeSlotList[i].m_nIndex == nSlotIndex )
		{
			sCostumeSlot = m_CostumeSlotList[i];
			return true;
		}
	}

	return false;
}

//½½·Ô ¾ò±â(½½·Ô ¹è¿­)
bool ioUserCostume::GetCostumeSlotArray( int nArray, CostumeSlot &sCostumeSlot )
{
	if ( m_CostumeSlotList.empty() )
		return false;

	if( COMPARE( nArray, 0, (int)m_CostumeSlotList.size() ) )
	{
		sCostumeSlot = m_CostumeSlotList[nArray];
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//½½·Ô °³¼ö ¾ò±â
int ioUserCostume::GetCostumeSlotCnt()
{	
	return (int)m_CostumeSlotList.size();
}

//½½·Ô »óÅÂ È®ÀÎ
bool ioUserCostume::IsSlotFull()
{
	if( GetCostumeSlotCnt() >= MAX_SLOT_CNT )
		return true;
	return false;
}

//Á¤·Ä
bool ioUserCostume::SlotSort()
{
	if ( m_CostumeSlotList.empty() )
		return false;

	std::sort( m_CostumeSlotList.begin(), m_CostumeSlotList.end(), CostumeSlotSort() );

	return true;
}

//ÄÚ½ºÆ¬ ÄÚµå ¾ò±â(½½·Ô ÀÎµ¦½º)
int ioUserCostume::GetCostumeCode( int nSlotIndex )
{
	if ( m_CostumeSlotList.empty() )
		return 0;

	int nSize = (int)m_CostumeSlotList.size();
	for( int i=0; i < nSize; ++i )
	{
		if( m_CostumeSlotList[i].m_nIndex == nSlotIndex )
			return  m_CostumeSlotList[i].m_nCostumeCode;
	}

	return 0;
}

//Á¤º¸ ¾ò±â
void ioUserCostume::GetUseInfo( int nSlotIndex, ioHashString &rsInfo )
{
	CostumeSlot kSlot;
	if( !GetCostumeSlot( nSlotIndex, kSlot ) )
		return;

	char szText[MAX_PATH]="";
	if( kSlot.m_PeriodType == PCPT_MORTMAIN )
		SafeSprintf( szText, sizeof( szText ), STR(1) );
	else 
		Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szText, sizeof( szText ) );

	rsInfo = szText;
}