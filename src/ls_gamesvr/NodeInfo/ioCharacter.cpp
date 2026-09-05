

#include "stdafx.h"
#include "../EtcHelpFunc.h"

#include "ioExerciseCharIndexManager.h"
#include "ioDecorationPrice.h"
#include "ioCharacter.h"
#include "ioItemInfoManager.h"

#include "User.h"
#include "ioUserExtraItem.h"
#include "ioPowerUpManager.h"

ioCharacter::ioCharacter()
{
	Initialize();
}

ioCharacter::~ioCharacter()
{
}

void ioCharacter::Initialize()
{
	m_dwCharIndex = 0;
	
	m_CharInfo.Init();
	m_ExperienceCharInfo.Init();
	for( int i=0 ; i<MAX_CHAR_DBITEM_SLOT ; i++ )
	{
		m_DBItemData[i].Initialize();
	}

	m_EquipSlot.ClearSlot();

	m_bPlayJoined  = false;
	m_bCharDie     = false;
	m_bExtraItemChange = false;

	m_dwLimitTimer = 0;
	m_dwLimitSecond= 0;
	m_dwRentalCheckTimer = 0;
	BackUp();
}

void ioCharacter::CheckExtraItemEquip(User *pUser)
{
	if( !pUser )
		return;

	const int iDefItemCode = 100000;
	if( m_CharInfo.m_chExerciseStyle != EXERCISE_RENTAL )
	{
		for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
		{
			if( m_CharInfo.m_extra_item[i] > 0 )	// 장비 아이템
			{
				if( pUser->GetUserExtraItem() )
				{
					ioUserExtraItem::EXTRAITEMSLOT kSlot;
					if( pUser->GetUserExtraItem()->GetExtraItem(m_CharInfo.m_extra_item[i], kSlot) )
					{
						if( !kSlot.m_bEquip )
						{
							kSlot.m_bEquip = TRUE;
							pUser->GetUserExtraItem()->SetExtraItem( kSlot );
						}
						else
						{
							m_DBItemData[i].m_item_code = (iDefItemCode * i) + m_CharInfo.m_class_type;
							m_DBItemData[i].m_item_reinforce = 0;
							m_DBItemData[i].m_item_male_custom = 0;
							m_DBItemData[i].m_item_female_custom = 0;

							int iEquipedClass = pUser->GetEquipedClassWithExtraItem(m_CharInfo.m_extra_item[i]);
							LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][extraitem]Duplicated equipment : [%lu] [%d] [%d] [%d]", pUser->GetUserIndex(), m_CharInfo.m_extra_item[i], iEquipedClass, m_CharInfo.m_class_type );

							m_CharInfo.m_extra_item[i] = 0;
						}
					}
				}
			}
		}
	}
		
}

void ioCharacter::SetCharInfo( DWORD dwIndex, const CHARACTER &rkInfo, User *pUser, bool bSetReinforce )
{
	m_dwCharIndex = dwIndex;
	m_CharInfo = rkInfo;
	ioClassExpert *pClassExpert = pUser->GetClassExpert();
	if( bSetReinforce && pClassExpert )
	{		

#ifdef ARENA_MODE_BY_BCKIM		// 2018-07-25 by bckim, 아레나 모드 추가	void ioCharacter::SetCharInfo
		if( pUser->GetModeType() == MT_ARENA ) 
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[ARENA_MODE] OnChangeCharCheck Clent[%s]", pUser->GetPublicID().c_str());
		}
		else		
#endif 	// ARENA_MODE_BY_BCKIM	// End. 2018-07-25 by bckim, 아레나 모드 추가	
			m_CharInfo.m_byReinforceGrade = pClassExpert->GetClassReinfoce(m_CharInfo.m_class_type);
	}

	m_ExperienceCharInfo = m_CharInfo;

	if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		// 대여 장비 사용
		if( pUser )
		{
			ioCharRentalData *pCharRentalData = pUser->GetCharRentalData();
			if( pCharRentalData )
			{
				const int iDefItemCode = 100000;
				for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
				{
					m_CharInfo.m_extra_item[i] = 0;

					ITEM_DATA kItemData;
					pCharRentalData->GetEquipItem( m_dwCharIndex, kItemData, i );
					if( kItemData.m_item_code > 0 )
					{
						m_DBItemData[i].m_item_code		     = kItemData.m_item_code;
						m_DBItemData[i].m_item_reinforce     = kItemData.m_item_reinforce;
						m_DBItemData[i].m_item_male_custom   = kItemData.m_item_male_custom;
						m_DBItemData[i].m_item_female_custom = kItemData.m_item_female_custom;
					}
					else	// 기본 아이템
					{
						if( m_CharInfo.m_byReinforceGrade > PUGT_NONE && m_CharInfo.m_byReinforceGrade <= PUGT_CHAR_GRADE5 )
						{
							SetDefaultPowerUpItem(i);
						}
						else
						{
							m_DBItemData[i].m_item_code          = (iDefItemCode * i) + m_CharInfo.m_class_type;
							m_DBItemData[i].m_item_reinforce     = 0;
							m_DBItemData[i].m_item_male_custom   = 0;
							m_DBItemData[i].m_item_female_custom = 0;
						}
					}
				}
			}
		}
	}
	else
	{
		// ITEM SET
		const int iDefItemCode = 100000;
		for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
		{
			if( m_CharInfo.m_extra_item[i] > 0 )	// 장비 아이템
			{
				if( pUser && pUser->GetUserExtraItem() )
				{
					ioUserExtraItem::EXTRAITEMSLOT kSlot;
					if( pUser->GetUserExtraItem()->GetExtraItem(m_CharInfo.m_extra_item[i], kSlot) )
					{
						m_DBItemData[i].m_item_code = kSlot.m_iItemCode;
						m_DBItemData[i].m_item_reinforce = kSlot.m_iReinforce;
						m_DBItemData[i].m_item_male_custom = kSlot.m_dwMaleCustom;
						m_DBItemData[i].m_item_female_custom = kSlot.m_dwFemaleCustom;
					}
					else
					{
						m_CharInfo.m_extra_item[i] = 0;

						if( m_CharInfo.m_byReinforceGrade > PUGT_NONE && m_CharInfo.m_byReinforceGrade <= PUGT_CHAR_GRADE5 )
						{
							SetDefaultPowerUpItem(i);
						}
						else
						{
							m_DBItemData[i].m_item_code = (iDefItemCode * i) + m_CharInfo.m_class_type;
							m_DBItemData[i].m_item_reinforce = 0;
							m_DBItemData[i].m_item_male_custom = 0;
							m_DBItemData[i].m_item_female_custom = 0;
						}
						m_bExtraItemChange = true;
					}
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioCharacter::SetCharInfo() - User Error: %d, %d", i, m_CharInfo.m_extra_item[i] );
					m_CharInfo.m_extra_item[i] = 0;

					if( m_CharInfo.m_byReinforceGrade > PUGT_NONE && m_CharInfo.m_byReinforceGrade <= PUGT_CHAR_GRADE5 )
					{
						SetDefaultPowerUpItem(i);
					}
					else
					{
						m_DBItemData[i].m_item_code = (iDefItemCode * i) + m_CharInfo.m_class_type;
						m_DBItemData[i].m_item_reinforce = 0;
						m_DBItemData[i].m_item_male_custom = 0;
						m_DBItemData[i].m_item_female_custom = 0;
					}
					m_bExtraItemChange = true;
				}
			}
			else	// 기본 아이템
			{
				if( m_CharInfo.m_byReinforceGrade > PUGT_NONE && m_CharInfo.m_byReinforceGrade <= PUGT_CHAR_GRADE5 )
				{
					SetDefaultPowerUpItem(i);
				}
				else
				{
					m_DBItemData[i].m_item_code = (iDefItemCode * i) + m_CharInfo.m_class_type;
					m_DBItemData[i].m_item_reinforce = 0;
					m_DBItemData[i].m_item_male_custom = 0;
					m_DBItemData[i].m_item_female_custom = 0;
				}
			}
		}
	}
	CheckRentalLimitTime();
}

void ioCharacter::ChangeDBExtraItem( User *pUser, int iSlotIndex )
{
	if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		// 대여 장비 사용
		if( pUser )
		{
			ioCharRentalData *pCharRentalData = pUser->GetCharRentalData();
			if( pCharRentalData )
			{
				const int iDefItemCode = 100000;
				for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
				{
					ITEM_DATA kItemData;
					pCharRentalData->GetEquipItem( m_dwCharIndex, kItemData, i );
					if( kItemData.m_item_code > 0 )
					{
						m_DBItemData[i].m_item_code		     = kItemData.m_item_code;
						m_DBItemData[i].m_item_reinforce     = kItemData.m_item_reinforce;
						m_DBItemData[i].m_item_male_custom   = kItemData.m_item_male_custom;
						m_DBItemData[i].m_item_female_custom = kItemData.m_item_female_custom;
					}
					else	// 기본 아이템
					{
						m_DBItemData[i].m_item_code          = (iDefItemCode * i) + m_CharInfo.m_class_type;
						m_DBItemData[i].m_item_reinforce     = 0;
						m_DBItemData[i].m_item_male_custom   = 0;
						m_DBItemData[i].m_item_female_custom = 0;
					}
				}
			}
		}
	}
	else
	{
		// ExtraItem값이 변경 되었으므로 다시 적용
		// ITEM SET
		const int iDefItemCode = 100000;
		for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
		{
			if( m_CharInfo.m_extra_item[i] == iSlotIndex )	// 장비 아이템
			{
				if( pUser && pUser->GetUserExtraItem() )
				{
					ioUserExtraItem::EXTRAITEMSLOT kSlot;
					if( pUser->GetUserExtraItem()->GetExtraItem(m_CharInfo.m_extra_item[i], kSlot) )
					{
						m_DBItemData[i].m_item_code = kSlot.m_iItemCode;
						m_DBItemData[i].m_item_reinforce = kSlot.m_iReinforce;
						m_DBItemData[i].m_item_male_custom = kSlot.m_dwMaleCustom;
						m_DBItemData[i].m_item_female_custom = kSlot.m_dwFemaleCustom;
					}
					else
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioCharacter::ChangeDBExtraItem() - Not Exist In ExtraInven: %d, %d", i, m_CharInfo.m_extra_item[i] );
						m_CharInfo.m_extra_item[i] = 0;
						m_DBItemData[i].m_item_code = (iDefItemCode * i) + m_CharInfo.m_class_type;
						m_DBItemData[i].m_item_reinforce = 0;
						m_DBItemData[i].m_item_male_custom = 0;
						m_DBItemData[i].m_item_female_custom = 0;
					}
				}
			}
		}
	}
}

bool ioCharacter::IsChange()
{
	if( m_bExtraItemChange )
	{
		m_bExtraItemChange = false;
		return true;
	}

	if( m_BackUP.m_dwCharIndex != m_dwCharIndex )
		return true;
	if( m_BackUP.m_CharInfo.m_class_type != m_CharInfo.m_class_type )
		return true;
	if( m_BackUP.m_CharInfo.m_kindred != m_CharInfo.m_kindred )
		return true;    
	if( m_BackUP.m_CharInfo.m_sex != m_CharInfo.m_sex )
		return true;
	if( m_BackUP.m_CharInfo.m_beard != m_CharInfo.m_beard )
		return true;
	if( m_BackUP.m_CharInfo.m_face != m_CharInfo.m_face )
		return true;
	if( m_BackUP.m_CharInfo.m_hair != m_CharInfo.m_hair )
		return true;
	if( m_BackUP.m_CharInfo.m_skin_color != m_CharInfo.m_skin_color )
		return true;
	if( m_BackUP.m_CharInfo.m_hair_color != m_CharInfo.m_hair_color )
		return true;
	if( m_BackUP.m_CharInfo.m_accessories != m_CharInfo.m_accessories )
		return true;
	if( m_BackUP.m_CharInfo.m_underwear != m_CharInfo.m_underwear )
		return true;
	if( m_BackUP.m_CharInfo.m_iLimitSecond != m_CharInfo.m_iLimitSecond )
		return true;
	if( m_BackUP.m_CharInfo.m_iSlotIndex != m_CharInfo.m_iSlotIndex )
		return true;
	if( m_BackUP.m_CharInfo.m_ePeriodType != m_CharInfo.m_ePeriodType )
		return true;
	if( m_BackUP.m_CharInfo.m_sLeaderType != m_CharInfo.m_sLeaderType )
		return true;
	if( m_BackUP.m_CharInfo.m_sRentalType != m_CharInfo.m_sRentalType )
		return true;
	if( m_BackUP.m_CharInfo.m_dwRentalMinute != m_CharInfo.m_dwRentalMinute )
		return true;
	if( m_BackUP.m_CharInfo.m_iAwakeLimitTime != m_CharInfo.m_iAwakeLimitTime )
		return true;
	if( m_BackUP.m_CharInfo.m_chAwakeType != m_CharInfo.m_chAwakeType )
		return true;

	for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
	{
		if( m_BackUP.m_CharInfo.m_extra_item[i] != m_CharInfo.m_extra_item[i] )
			return true;
	}



	return false;
}

void ioCharacter::BackUp()
{
	m_BackUP.m_dwCharIndex = m_dwCharIndex;
	m_BackUP.m_CharInfo    = m_CharInfo;	
}

void ioCharacter::InitDBItemList()
{
	memset( m_DBItemData, 0, sizeof(m_DBItemData) );
}

ioItem* ioCharacter::EquipItem( ioItem *pItem )
{
	return m_EquipSlot.EquipItem( pItem );
}

ioItem* ioCharacter::EquipItem( int iSlot, ioItem *pItem )
{
	return m_EquipSlot.EquipItem( iSlot, pItem );
}

ioItem* ioCharacter::ReleaseItem( int iSlot )
{
	return m_EquipSlot.ReleaseItem( iSlot );
}

ioItem* ioCharacter::ReleaseItem( int iGameIndex, int iItemCode )
{
	return m_EquipSlot.ReleaseItem( iGameIndex, iItemCode );
}

void ioCharacter::ClearEquipSlot()
{
	m_EquipSlot.ClearSlot();
}

bool ioCharacter::IsSlotEquiped( int iSlot ) const
{
	return m_EquipSlot.IsSlotEquiped( iSlot );
}

const ioItem* ioCharacter::GetItem( int iSlot ) const
{
	return m_EquipSlot.GetItem( iSlot );
}

void ioCharacter::ClearOwnerNameInEquipSlot( const ioHashString &rkName )
{
	m_EquipSlot.ClearOwnerName( rkName );
}

void ioCharacter::TakeOffExtraItem( int iEquipType )
{
	SetChangeExtraItem( iEquipType, 0 );
	SetDBItemData( iEquipType, 0, 0, 0, 0 );
}

void ioCharacter::SetDefaultPowerUpItem(int iSlot)
{
	int iCharGrade = m_CharInfo.m_byReinforceGrade;
	int iPowerUpItemCode = (100000 * iSlot) + g_PowerUpMgr.ConvertPowerUpTypeCharToItem(iCharGrade) + GetClassType();
	//존재하는 강화 아이템 코드 인지 체크
	const ItemInfo *pItemInfo = g_ItemInfoMgr.GetItemInfo( iPowerUpItemCode );
	if( pItemInfo )
		m_DBItemData[iSlot].m_item_code = (100000 * iSlot) + g_PowerUpMgr.ConvertPowerUpTypeCharToItem(iCharGrade) + GetClassType();
	else
		m_DBItemData[iSlot].m_item_code = (100000 * iSlot) + m_CharInfo.m_class_type;

	m_DBItemData[iSlot].m_item_reinforce = 0;
	m_DBItemData[iSlot].m_item_male_custom = 0;
	m_DBItemData[iSlot].m_item_female_custom = 0;
}

void ioCharacter::SetDBItemData( int iSlot, int iItemCode, int iReinforce, DWORD dwMaleCustom, DWORD dwFemaleCustom )
{
	if( COMPARE( iSlot, 0, MAX_CHAR_DBITEM_SLOT ) )
	{
		if( iItemCode == 0 )
		{
			if( (PUGT_CHAR_GRADE1 <= m_CharInfo.m_byReinforceGrade && m_CharInfo.m_byReinforceGrade <= PUGT_CHAR_GRADE5) )
			{
				SetDefaultPowerUpItem(iSlot);
			}
			else
			{
				m_DBItemData[iSlot].m_item_code = (100000 * iSlot) + m_CharInfo.m_class_type;
				m_DBItemData[iSlot].m_item_reinforce = 0;
				m_DBItemData[iSlot].m_item_male_custom = 0;
				m_DBItemData[iSlot].m_item_female_custom = 0;
			}
		}
		else
		{
			m_DBItemData[iSlot].m_item_code = iItemCode;
			m_DBItemData[iSlot].m_item_reinforce = iReinforce;
			m_DBItemData[iSlot].m_item_male_custom = dwMaleCustom;
			m_DBItemData[iSlot].m_item_female_custom = dwFemaleCustom;
		}
	}
}

const ITEM_DATA* ioCharacter::GetDBItemData( int iSlot ) const
{
	if( COMPARE( iSlot, 0, MAX_CHAR_DBITEM_SLOT ) )
		return &m_DBItemData[iSlot];

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioCharacter::GetDBItemData - %d Overflow", iSlot );
	return NULL;
}

const int ioCharacter::GetCurrentItemCode( int iSlot ) const
{
	const ioItem *pItem = m_EquipSlot.GetItem( iSlot );
	if( pItem == NULL ) return 0;

	return pItem->GetItemCode();
}

const int ioCharacter::GetCurrentItemReinforce( int iSlot ) const
{
	const ioItem *pItem = m_EquipSlot.GetItem( iSlot );
	if( pItem == NULL ) return 0;

	return pItem->GetItemReinforce();
}

const int ioCharacter::GetCurrentItemMaleCustom( int iSlot ) const
{
	const ioItem *pItem = m_EquipSlot.GetItem( iSlot );
	if( pItem == NULL ) return 0;

	return pItem->GetItemMaleCustom();
}

const int ioCharacter::GetCurrentItemFemaleCustom( int iSlot ) const
{
	const ioItem *pItem = m_EquipSlot.GetItem( iSlot );
	if( pItem == NULL ) return 0;

	return pItem->GetItemFemaleCustom();
}

const int ioCharacter::GetExtraItemIndex( int iSlot) const
{
	if(iSlot >= 0 && iSlot < MAX_CHAR_DBITEM_SLOT)
	{
		return m_CharInfo.m_extra_item[iSlot];
	}
	return 0;
}

void ioCharacter::SetChangeExtraItem( int iSlot, int iNewSlotIndex )
{
	m_CharInfo.m_extra_item[iSlot] = iNewSlotIndex;
}

void ioCharacter::SetExperienceChar( CHARACTER &rkExperienceChar )
{
	m_ExperienceCharInfo = m_CharInfo;   // 원본 
	m_ExperienceCharInfo.m_kindred		= rkExperienceChar.m_kindred;
	m_ExperienceCharInfo.m_sex			= rkExperienceChar.m_sex;
	m_ExperienceCharInfo.m_beard		= rkExperienceChar.m_beard;
	m_ExperienceCharInfo.m_face			= rkExperienceChar.m_face;
	m_ExperienceCharInfo.m_hair			= rkExperienceChar.m_hair;
	m_ExperienceCharInfo.m_skin_color	= rkExperienceChar.m_skin_color;
	m_ExperienceCharInfo.m_hair_color	= rkExperienceChar.m_hair_color;
	m_ExperienceCharInfo.m_accessories	= rkExperienceChar.m_accessories;
	m_ExperienceCharInfo.m_underwear	= rkExperienceChar.m_underwear;
	
	for( int i = 0; i < MAX_CHAR_COSTUME_SLOT; i++ )
	{
		m_ExperienceCharInfo.m_costume_item[i].m_iCostumeCode = rkExperienceChar.m_costume_item[i].m_iCostumeCode;
	}

	//for( int i = 0; i < MAX_CHAR_ACCESSORY_SLOT; i++ )
	//{
	//	m_ExperienceCharInfo.m_accessory_item[i].m_iAccessoryCode = rkExperienceChar.m_accessory_item[i].m_iAccessoryCode;
	//}
}

void ioCharacter::FillEquipItemInfo( SP2Packet &rkPacket )
{
	m_EquipSlot.FillEquipItemInfo( rkPacket );
}

void ioCharacter::FillDBExtraItemInfo( SP2Packet &rkPacket, User *pUser, bool bAll )
{
	if( pUser == NULL || pUser->GetUserExtraItem() == NULL || pUser->GetCharRentalData() == NULL )
	{
		// Exception Error
		for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
		{
			if( bAll )
			{
				PACKET_GUARD_VOID( rkPacket.Write(0) );
				PACKET_GUARD_VOID( rkPacket.Write(0) );
				PACKET_GUARD_VOID( rkPacket.Write(0) );
				PACKET_GUARD_VOID( rkPacket.Write(0) );
			}
			else
			{
				PACKET_GUARD_VOID( rkPacket.Write(0) );
			}
		}
	}
	else
	{
		if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
		{
			// 대여 장비 사용
			const int iDefItemCode = 100000;
			for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
			{
				ITEM_DATA kItemData;
				pUser->GetCharRentalData()->GetEquipItem( m_dwCharIndex, kItemData, i );
				if( kItemData.m_item_code > 0 )
				{
					if( bAll )
					{
						PACKET_GUARD_VOID( rkPacket.Write(kItemData.m_item_code) ); 
						PACKET_GUARD_VOID( rkPacket.Write(kItemData.m_item_reinforce) ); 
						PACKET_GUARD_VOID( rkPacket.Write(kItemData.m_item_male_custom) ); 
						PACKET_GUARD_VOID( rkPacket.Write(kItemData.m_item_female_custom) );
					}
					else
					{
						PACKET_GUARD_VOID( rkPacket.Write(kItemData.m_item_code) );
					}
				}
				else
				{
					if( bAll )
					{
						PACKET_GUARD_VOID( rkPacket.Write(0) );
						PACKET_GUARD_VOID( rkPacket.Write(0) );
						PACKET_GUARD_VOID( rkPacket.Write(0) );
						PACKET_GUARD_VOID( rkPacket.Write(0) );
					}
					else
					{
						PACKET_GUARD_VOID( rkPacket.Write(0) );
					}
				}
			}
		}
		else
		{
			for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
			{
				if( m_CharInfo.m_extra_item[i] > 0 )	// 장비 아이템
				{
					ioUserExtraItem::EXTRAITEMSLOT kSlot;
					if( pUser->GetUserExtraItem()->GetExtraItem( m_CharInfo.m_extra_item[i], kSlot ) )
					{
						if( bAll )
						{
							PACKET_GUARD_VOID( rkPacket.Write(kSlot.m_iItemCode) ); 
							PACKET_GUARD_VOID( rkPacket.Write(kSlot.m_iReinforce) ); 
							PACKET_GUARD_VOID( rkPacket.Write(kSlot.m_dwMaleCustom) ); 
							PACKET_GUARD_VOID( rkPacket.Write(kSlot.m_dwFemaleCustom) );
						}
						else 
						{
							PACKET_GUARD_VOID( rkPacket.Write(kSlot.m_iItemCode) );
						}
					}
					else
					{
						if( bAll )
						{
							PACKET_GUARD_VOID( rkPacket.Write(0) );
							PACKET_GUARD_VOID( rkPacket.Write(0) );
							PACKET_GUARD_VOID( rkPacket.Write(0) );
							PACKET_GUARD_VOID( rkPacket.Write(0) );
						}
						else
						{
							PACKET_GUARD_VOID( rkPacket.Write(0) );
						}
					}
				}
				else
				{
					if( bAll )
					{
						PACKET_GUARD_VOID( rkPacket.Write(0) );
						PACKET_GUARD_VOID( rkPacket.Write(0) );
						PACKET_GUARD_VOID( rkPacket.Write(0) );
						PACKET_GUARD_VOID( rkPacket.Write(0) );
					}
					else
					{
						PACKET_GUARD_VOID( rkPacket.Write(0) );
					}
				}
			}
		}
	}
}

int ioCharacter::IncreaseStat( int iStat )
{
	return 0;
}

void ioCharacter::InitStat()
{
}

void ioCharacter::SetCharDie( bool bCharDie )
{
	m_bCharDie = bCharDie;
}

bool ioCharacter::StartLimitTimer( DWORD dwCheckSecond )
{
	if( !IsActive() ) return false;
	if( !HasExerciseStyle( EXERCISE_NONE ) ) return false;
	if( IsMortmain() ) return false;

	m_dwLimitTimer = TIMEGETTIME();
	m_dwLimitSecond= dwCheckSecond;
	return true;
}

bool ioCharacter::UpdateLimitTimer()
{
	if( m_dwLimitTimer  == 0 ) return false;
	if( m_dwLimitSecond == 0 ) return false;
	if( IsMortmain() ) return false;
	if( !IsActive() ) return false;

	int iGapSecond = (float)( TIMEGETTIME() - m_dwLimitTimer ) / m_dwLimitSecond;

	m_CharInfo.m_iLimitSecond -= iGapSecond;

	// 기간 만료
	if( m_CharInfo.m_iLimitSecond <= 0 )
	{
		m_CharInfo.m_iLimitSecond = 0;
		SetActive( false );
	}
	m_dwLimitTimer = 0;
	m_dwLimitSecond= 0;
	return true;
}

int ioCharacter::GetGapLimitTime()
{
	if( m_dwLimitTimer  == 0 ) return 0;
	if( m_dwLimitSecond == 0 ) return 0;
	if( IsMortmain() ) return 0;
	if( !IsActive() ) return 0;

	int iGapSecond = (float)( TIMEGETTIME() - m_dwLimitTimer ) / m_dwLimitSecond;
	return iGapSecond;
}

void ioCharacter::SetCharLimitExtend( int iLimitDate )
{
	m_CharInfo.m_iLimitSecond += iLimitDate;
}

void ioCharacter::SetCharLimitExtendDate( int iLimitDate )
{
	CTime cCharTime = Help::ConvertNumberToCTime(m_CharInfo.m_iLimitSecond);
	CTime kCurTime = CTime::GetCurrentTime();

	CTime kCharTime;
	CTimeSpan kAddTime(0, 0, iLimitDate, 0);

	if(cCharTime > kCurTime)
		kCharTime = cCharTime + kAddTime;
	else
		kCharTime = kCurTime + kAddTime;

	SYSTEMTIME sysCharTime;
	kCharTime.GetAsSystemTime( sysCharTime );

	// 17년은 유지한다는 가정하에 2010 을 뺀다.
	m_CharInfo.m_iLimitSecond = Help::ConvertYYMMDDHHMMToDate(sysCharTime.wYear, sysCharTime.wMonth, sysCharTime.wDay,
		sysCharTime.wHour, sysCharTime.wMinute);

	LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "%s Set %d", __FUNCTION__, m_CharInfo.m_iLimitSecond);
}

bool ioCharacter::SetCharDecoration( int iType, int iCode )
{
	int iClassType = iType / 100000;
	int iSexType   = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

	if( iDecoType == UID_KINDRED ) //종족 변경
	{
		switch( iDecoCode )
		{
		case RDT_HUMAN_MAN:
			m_CharInfo.m_kindred = 1;
			m_CharInfo.m_sex = 1;
			break;
		case RDT_HUMAN_WOMAN:
			m_CharInfo.m_kindred = 1;
			m_CharInfo.m_sex = 2;
			break;
		case RDT_ELF_MAN:
			m_CharInfo.m_kindred = 2;
			m_CharInfo.m_sex = 1;
			break;
		case RDT_ELF_WOMAN:
			m_CharInfo.m_kindred = 2;
			m_CharInfo.m_sex = 2;
			break;
		case RDT_DWARF_MAN:
			m_CharInfo.m_kindred = 3;
			m_CharInfo.m_sex = 1;
			break;
		case RDT_DWARF_WOMAN:
			m_CharInfo.m_kindred = 3;
			m_CharInfo.m_sex = 2;
			break;
		default:
			return false;
		}
		return true;
	}
	else if( COMPARE( iDecoType , UID_FACE, UID_HAIR_COLOR + 1 ) || iDecoType == UID_UNDERWEAR )                        // 치장
	{
		switch( iDecoType )
		{
		case UID_FACE:
			m_CharInfo.m_face = iDecoCode;
			break;
		case UID_HAIR:
			m_CharInfo.m_hair = iDecoCode;
			break;
		case UID_SKIN_COLOR:
			m_CharInfo.m_skin_color = iDecoCode;
			break;
		case UID_HAIR_COLOR:
			m_CharInfo.m_hair_color = iDecoCode;
			break;
		case UID_UNDERWEAR:
			m_CharInfo.m_underwear = iDecoCode;
			break;
		}

		return true;
	}

	return false;
}

void ioCharacter::SetCharAllDecoration( ioInventory &rkInventory )
{
	rkInventory.GetEquipItemCode( m_CharInfo );
}

void ioCharacter::SetChangeKindred( const CHARACTER& rkCharInfo, DWORD dwRandSeed )
{
	if( rkCharInfo.m_class_type != m_CharInfo.m_class_type ) return;

	m_CharInfo.m_face = rkCharInfo.m_face;
	if( m_CharInfo.m_face == -1)
		m_CharInfo.m_face = g_DecorationPrice.GetDefaultDecoCode( m_CharInfo.m_sex - 1, UID_FACE, dwRandSeed + m_CharInfo.m_class_type + UID_FACE, m_CharInfo.m_class_type );

	m_CharInfo.m_hair = rkCharInfo.m_hair;
	if( m_CharInfo.m_hair == -1 )
		m_CharInfo.m_hair = g_DecorationPrice.GetDefaultDecoCode( m_CharInfo.m_sex - 1, UID_HAIR, dwRandSeed + m_CharInfo.m_class_type + UID_HAIR, m_CharInfo.m_class_type );

	m_CharInfo.m_skin_color = rkCharInfo.m_skin_color;
	if( m_CharInfo.m_skin_color == -1 )
		m_CharInfo.m_skin_color = g_DecorationPrice.GetDefaultDecoCode( m_CharInfo.m_sex - 1, UID_SKIN_COLOR, dwRandSeed + m_CharInfo.m_class_type + UID_SKIN_COLOR, m_CharInfo.m_class_type );

	m_CharInfo.m_hair_color = rkCharInfo.m_hair_color;
	if( m_CharInfo.m_hair_color == -1 )
		m_CharInfo.m_hair_color = g_DecorationPrice.GetDefaultDecoCode( m_CharInfo.m_sex - 1, UID_HAIR_COLOR, dwRandSeed + m_CharInfo.m_class_type + UID_HAIR_COLOR, m_CharInfo.m_class_type );	

	m_CharInfo.m_underwear = rkCharInfo.m_underwear;
	if( m_CharInfo.m_underwear == -1 )
		m_CharInfo.m_underwear = g_DecorationPrice.GetDefaultDecoCode( m_CharInfo.m_sex - 1, UID_UNDERWEAR, dwRandSeed + m_CharInfo.m_class_type + UID_UNDERWEAR, m_CharInfo.m_class_type );	
}

void ioCharacter::CopyData( ioCharacter *pCharacter )
{
	// m_dwCharIndex, m_BackUP 제외 전부 복사한다.
	m_CharInfo = pCharacter->m_CharInfo;
	m_ExperienceCharInfo = pCharacter->m_ExperienceCharInfo;

	for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
		m_DBItemData[i] = pCharacter->m_DBItemData[i];
	m_EquipSlot		= pCharacter->m_EquipSlot;
	m_bPlayJoined	= pCharacter->m_bPlayJoined;
	m_bCharDie		= pCharacter->m_bCharDie;
}

bool ioCharacter::IsChangeActive()
{
	if( m_BackUP.m_CharInfo.m_bActive != m_CharInfo.m_bActive )
		return true;

	return false;
}

bool ioCharacter::HasExerciseStyle( BYTE chStyle )
{
	if( m_CharInfo.m_chExerciseStyle == chStyle )
		return true;
	return false;
}

void ioCharacter::SetExerciseStyle(BYTE chStyle )
{
	m_CharInfo.m_chExerciseStyle = chStyle;
}

void ioCharacter::SetCharLimitDate( int iLimitDate )
{
	m_CharInfo.m_iLimitSecond = iLimitDate;
}

void ioCharacter::CheckRentalLimitTime()
{
	if( m_CharInfo.m_dwRentalMinute == 0 ) return;

	if( m_dwRentalCheckTimer == 0 )
	{
		m_dwRentalCheckTimer = TIMEGETTIME();
	}
	else
	{
		DWORD dwGapMinute = (float)( TIMEGETTIME() - m_dwRentalCheckTimer ) / 60000;
		if( m_CharInfo.m_dwRentalMinute > dwGapMinute )
			m_CharInfo.m_dwRentalMinute -= dwGapMinute;
		else
		{
			// 대여 종료 - 시간 체크는 클라이언트에서 다시 보내주므로 클라이언트 
			// 기준으로 바꾸고 서버 시간은 해킹 방지용으로 사용
			m_CharInfo.m_dwRentalMinute = 0;
		}
		m_dwRentalCheckTimer = TIMEGETTIME();
	}
}

bool ioCharacter::IsCharEquipExtraItemPeriodCheck( User *pUser, int iPeriodType )
{
	for(int i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
	{
		if( m_CharInfo.m_extra_item[i] > 0 )	// 장비 아이템
		{
			if( pUser && pUser->GetUserExtraItem() )
			{
				ioUserExtraItem::EXTRAITEMSLOT kSlot;
				if( pUser->GetUserExtraItem()->GetExtraItem( m_CharInfo.m_extra_item[i], kSlot ) )
				{
					if( kSlot.m_PeriodType == iPeriodType )
						return true;
				}
			}
		}
	}
	return false;
}

void ioCharacter::SetAwakeInit()
{
	m_CharInfo.m_iAwakeLimitTime = 0;
	m_CharInfo.m_chAwakeType = AWAKE_NONE;
}

void ioCharacter::SetAwakeInfo( int iAwakeType, int iEndDate )
{
	m_CharInfo.m_iAwakeLimitTime = iEndDate;
	m_CharInfo.m_chAwakeType = iAwakeType;
}

void ioCharacter::SetAwakeTime( int iEndDate )
{
	m_CharInfo.m_iAwakeLimitTime = iEndDate;
}

void ioCharacter::ChangeCostumeItem( int iSlot, DWORD dwIndex, DWORD dwCode )
{
	m_CharInfo.m_costume_item[iSlot].m_iCostumeIndex = dwIndex;
	m_CharInfo.m_costume_item[iSlot].m_iCostumeCode = dwCode;
}

int ioCharacter::GetCostumeSlot(int iIndex)
{
	for( int i = 0; i < MAX_CHAR_COSTUME_SLOT; i++ )
	{
		if( m_CharInfo.m_costume_item[i].m_iCostumeIndex == iIndex )
			return i;
	}

	return -1;
}

void ioCharacter::FillEquipCostumeInfo( SP2Packet &rkPacket )
{
	for( int i = 0; i < MAX_CHAR_COSTUME_SLOT; i++ )
	{
		PACKET_GUARD_VOID(rkPacket.Write(m_CharInfo.m_costume_item[i].m_iCostumeIndex));
		PACKET_GUARD_VOID(rkPacket.Write(m_CharInfo.m_costume_item[i].m_iCostumeCode));
	}
}

void ioCharacter::ChangeAccessoryItem( int iSlot, DWORD dwIndex, DWORD dwCode, int iValue, DWORD dwComposeCode, int iComposeValue )
{
	m_CharInfo.m_accessory_item[iSlot].m_iAccessoryIndex = dwIndex;
	m_CharInfo.m_accessory_item[iSlot].m_iAccessoryCode = dwCode;
	m_CharInfo.m_accessory_item[iSlot].m_iAccessoryValue = iValue;
	m_CharInfo.m_accessory_item[iSlot].m_iComposeCode = dwComposeCode;
	m_CharInfo.m_accessory_item[iSlot].m_iComposeValue = iComposeValue;
}

int ioCharacter::GetAccessorySlot(int iIndex)
{
	for( int i = 0; i < MAX_CHAR_ACCESSORY_SLOT; i++ )
	{
		if( m_CharInfo.m_accessory_item[i].m_iAccessoryIndex == iIndex )
			return i;
	}

	return -1;
}

void ioCharacter::FillEquipAccessoryInfo( SP2Packet &rkPacket, User *pUser )
{
	if( pUser )
	{
		PACKET_GUARD_VOID( rkPacket.Write( pUser->GetPublicID()) );
	}
	else
	{
		PACKET_GUARD_VOID( rkPacket.Write( "" ) );
	}

	for( int i = 0; i < MAX_CHAR_ACCESSORY_SLOT; i++ )
	{
		PACKET_GUARD_VOID(rkPacket.Write(m_CharInfo.m_accessory_item[i].m_iAccessoryIndex));
		PACKET_GUARD_VOID(rkPacket.Write(m_CharInfo.m_accessory_item[i].m_iAccessoryCode));
		PACKET_GUARD_VOID(rkPacket.Write(m_CharInfo.m_accessory_item[i].m_iAccessoryValue));
		PACKET_GUARD_VOID(rkPacket.Write(m_CharInfo.m_accessory_item[i].m_iComposeCode));
		PACKET_GUARD_VOID(rkPacket.Write(m_CharInfo.m_accessory_item[i].m_iComposeValue));
	}
}

int ioCharacter::GetItemCodeOfDefaultEquippedItem( int iSlot )
{
	if( (PUGT_CHAR_GRADE1 <= m_CharInfo.m_byReinforceGrade && m_CharInfo.m_byReinforceGrade <= PUGT_CHAR_GRADE5) )
	{
		int iCharGrade = m_CharInfo.m_byReinforceGrade;
		int iPowerUpItemCode = (100000 * iSlot) + g_PowerUpMgr.ConvertPowerUpTypeCharToItem(iCharGrade) + GetClassType();
		//존재하는 강화 아이템 코드 인지 체크
		const ItemInfo *pItemInfo = g_ItemInfoMgr.GetItemInfo( iPowerUpItemCode );
		if( pItemInfo )
			return (100000 * iSlot) + g_PowerUpMgr.ConvertPowerUpTypeCharToItem(iCharGrade) + GetClassType();
		else
			return (100000 * iSlot) + m_CharInfo.m_class_type;
	}
	
	return (100000 * iSlot) + m_CharInfo.m_class_type;
}