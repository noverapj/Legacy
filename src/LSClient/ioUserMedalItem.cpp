
#include "stdafx.h"

#include "ioUserMedalItem.h"

ioUserMedalItem::ioUserMedalItem()
{
	Initialize();
}

ioUserMedalItem::~ioUserMedalItem()
{
}

void ioUserMedalItem::Initialize()
{
	m_vMedalItemVec.clear();
}

void ioUserMedalItem::ApplyMedalItemData( SP2Packet &rkPacket )
{
	m_vMedalItemVec.clear();

	int iSize;
	rkPacket >> iSize;
	for( int i=0; i < iSize; ++i )
	{
		MEDALITEMSLOT kItem;

		rkPacket >> kItem.m_iItemType
			     >> kItem.m_iEquipClass
			     >> kItem.m_iPeriodType
			     >> kItem.m_iLimitDate
			     >> kItem.m_iLimitTime;

		m_vMedalItemVec.push_back( kItem );
	}
}

void ioUserMedalItem::ApplyCustomMedalItemData( SP2Packet &rkPacket )
{
	int iSize;
	rkPacket >> iSize;
	for( int i=0; i < iSize; ++i )
	{
		MEDALITEMSLOT kItem;

		rkPacket >> kItem.m_iItemType >> kItem.m_iCustomIndex >> kItem.m_iEquipClass
			     >> kItem.m_iPeriodType >> kItem.m_iLimitDate >> kItem.m_iLimitTime;

		for( int i = 0; i < 8; i++ )
			rkPacket >> kItem.m_iStat[i];

		m_vMedalItemVec.push_back( kItem );
	}
}

void ioUserMedalItem::AddMedalItemData( const MEDALITEMSLOT &rkNewSlot )
{
	MEDALITEMSLOT kItem;
	kItem = rkNewSlot;
	m_vMedalItemVec.push_back( kItem );
}

void ioUserMedalItem::AddCustomMedalItemData( SP2Packet &rkPacket )
{
	MEDALITEMSLOT kItem;
	rkPacket >> kItem.m_iItemType >> kItem.m_iCustomIndex >> kItem.m_iEquipClass
			    >> kItem.m_iPeriodType >> kItem.m_iLimitDate >> kItem.m_iLimitTime;

	for( int i = 0; i < 8; i++ )
		rkPacket >> kItem.m_iStat[i];

	m_vMedalItemVec.push_back( kItem );
}

bool ioUserMedalItem::DeleteMedalItemData( int iItemType, int iCustomIndex )
{
	int iSize = m_vMedalItemVec.size();

	for( int i=0; i < iSize; ++i )
	{
		MEDALITEMSLOT &rkItem = m_vMedalItemVec[i];
		if( rkItem.m_iItemType == iItemType && rkItem.m_iCustomIndex == iCustomIndex && rkItem.m_iEquipClass == 0 )
		{
			m_vMedalItemVec.erase( m_vMedalItemVec.begin() + i );
			return true;
		}
	}

	return false;
}

int ioUserMedalItem::GetMedalItemCnt()
{
	return (int)m_vMedalItemVec.size();
}

void ioUserMedalItem::SetMedalItem( const MEDALITEMSLOT &rkMedalItem, bool bFindNotEquipMedal, int nClassType )
{
	int iSize = m_vMedalItemVec.size();
	for( int i=0; i < iSize; ++i )
	{
		MEDALITEMSLOT &rkItem = m_vMedalItemVec[i];

		if( bFindNotEquipMedal )
		{
			if( rkItem.m_iItemType == rkMedalItem.m_iItemType && rkItem.m_iCustomIndex == rkMedalItem.m_iCustomIndex && rkItem.m_iEquipClass == 0 )
			{
				m_vMedalItemVec[i] = rkMedalItem;
				return;
			}
		}
		else
		{
			if( nClassType == 0 )
			{
				if( rkItem.m_iItemType == rkMedalItem.m_iItemType && rkItem.m_iCustomIndex == rkMedalItem.m_iCustomIndex )
				{
					m_vMedalItemVec[i] = rkMedalItem;
					return;
				}
			}
			else
			{
				if( rkItem.m_iItemType == rkMedalItem.m_iItemType && rkItem.m_iCustomIndex == rkMedalItem.m_iCustomIndex && rkItem.m_iEquipClass == nClassType )
				{
					m_vMedalItemVec[i] = rkMedalItem;
					return;
				}
			}
		}
	}
}

bool ioUserMedalItem::GetMedalItem( IN int iItemType, int iCustomIndex, OUT MEDALITEMSLOT &rkMedalItem, bool FindNotEquipMedal, int nClassType )
{
	// 초기화
	rkMedalItem.Init();
	int iSize = m_vMedalItemVec.size();
	for( int i=0; i < iSize; ++i )
	{
		MEDALITEMSLOT &rkItem = m_vMedalItemVec[i];
		if( FindNotEquipMedal )
		{
			if( rkItem.m_iItemType == iItemType && rkItem.m_iCustomIndex == iCustomIndex && rkItem.m_iEquipClass == 0 )
			{
				rkMedalItem = m_vMedalItemVec[i];
				return true;
			}
		}
		else
		{
			if( nClassType == 0 )
			{
				if( rkItem.m_iItemType == iItemType && rkItem.m_iCustomIndex == iCustomIndex )
				{
					rkMedalItem = m_vMedalItemVec[i];
					return true;
				}
			}
			else
			{
				if( rkItem.m_iItemType == iItemType && rkItem.m_iCustomIndex == iCustomIndex && rkItem.m_iEquipClass ==  nClassType )
				{
					rkMedalItem = m_vMedalItemVec[i];
					return true;
				}
			}
		}		
	}
	return false;
}

bool ioUserMedalItem::GetMedalItemArray( IN int iArray, OUT MEDALITEMSLOT &rkMedalItem )
{
	// 초기화
	rkMedalItem.Init();

	if( COMPARE( iArray, 0, (int)m_vMedalItemVec.size() ) )
	{
		rkMedalItem = m_vMedalItemVec[iArray];
		return true;
	}

	return false;
}

bool ioUserMedalItem::GetMedalItemTypeVec( OUT MedalSyncInfoVec &rvItemTypeVec, IN int iClassType )
{
	rvItemTypeVec.clear();
	bool bExist = false;
	int iSize = m_vMedalItemVec.size();
	for( int i=0; i < iSize; ++i )
	{
		MEDALITEMSLOT &rkItem = m_vMedalItemVec[i];
		if( rkItem.m_iEquipClass == iClassType )
		{
			MedalSyncInfo cInfo;
			cInfo.m_iItemType = rkItem.m_iItemType;
			cInfo.m_iCustomIndex = rkItem.m_iCustomIndex;
			for( int j = 0; j < 8; j++ )
				cInfo.m_iStat[j] = rkItem.m_iStat[j];

			rvItemTypeVec.push_back( cInfo );		
			bExist = true;
		}
	}
	return bExist;
}

void ioUserMedalItem::GetUseInfo( IN int iItemType, int iCustomIndex, OUT ioHashString &rsInfo )
{
	MEDALITEMSLOT kSlot;
	if( !GetMedalItem( iItemType, iCustomIndex, kSlot ) )
		return;

	char szText[MAX_PATH]="";
	if( kSlot.m_iPeriodType == PT_MORTMAIN )
	{
		SafeSprintf( szText, sizeof( szText ), STR(1) );
	}
	else 
	{
		Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szText, sizeof( szText ) );
	}

	rsInfo = szText;
}

void ioUserMedalItem::ReleaseEquipMedal( int iClassType )
{
	int iSize = m_vMedalItemVec.size();
	for( int i=0; i < iSize; ++i )
	{
		MEDALITEMSLOT &rkItem = m_vMedalItemVec[i];
		if( rkItem.m_iEquipClass == iClassType )
		{
			rkItem.m_iEquipClass = 0;
		}
	}
}

int ioUserMedalItem::GetExtraItemCnt()
{
	int iSize = m_vMedalItemVec.size();
	return iSize;
}

bool ioUserMedalItem::IsSlotFull()
{
	if( GetExtraItemCnt() > MAX_SLOT_CNT )
		return true;
	return false;
}