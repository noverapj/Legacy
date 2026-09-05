#include "stdafx.h"
#include "ioPowerUpManager.h"
#include "User.h"
#include "ioCharacter.h"
#include "ioUserExtraItem.h"
#include "../Util/IORandom.h"
#include "ioExtraItemInfoManager.h"
#include "Room.h"
#include "ioClassExpert.h"
#include "HeadquartersMode.h"
#include "HeadquartersModeHelp.h"

template<> ioPowerUpManager *Singleton< ioPowerUpManager >::ms_Singleton = 0;

ioPowerUpManager::ioPowerUpManager()
{
	Init();
}

ioPowerUpManager::~ioPowerUpManager()
{
	Destroy();
}

void ioPowerUpManager::Init()
{
	m_mHeroNeedMaterialCnt.clear();
	m_mBasicItemNeedMaterialCnt.clear();
	m_mHeroDownNeedMaterialCnt.clear();
	m_mBasicItemDownNeedMaterialCnt.clear();
	m_mPowerUpCharList.clear();
	m_iMaterialCode	= 0;
}

void ioPowerUpManager::Destroy()
{
	m_mHeroNeedMaterialCnt.clear();
	m_mBasicItemNeedMaterialCnt.clear();
	m_mHeroDownNeedMaterialCnt.clear();
	m_mBasicItemDownNeedMaterialCnt.clear();
	m_mPowerUpCharList.clear();
	m_iMaterialCode	= 0;
}

ioPowerUpManager& ioPowerUpManager::GetSingleton()
{
	return Singleton< ioPowerUpManager >::GetSingleton();
}

void ioPowerUpManager::CheckNeedReload()
{
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_powerup_manager.ini" );
	if( kLoader.ReadBool( "common", "change", false ) )
	{
		LoadINI();
	}
}

void ioPowerUpManager::LoadINI()
{
	// 일반 용병 진화
	{
		char szKey[MAX_PATH]="";
		ioINILoader kLoader( "config/sp2_powerup_manager.ini" );

		kLoader.SetTitle( "common" );
		int iEnableCharCnt = kLoader.LoadInt( "max_char", 0 );

		kLoader.SetTitle( "need_material" );
		m_iMaterialCode = kLoader.LoadInt( "material_code", 0 );
		int iCharGradeMax = kLoader.LoadInt( "char_grade_max", 0 );
		int iItemGradeMax = kLoader.LoadInt( "item_grade_max", 0 );

		//영웅, 아이템별 강화 필요 재료 수.
		for( int i = 0; i < iCharGradeMax; i++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "char_grade%d_cnt", i+1 );
			int iNeedCnt = kLoader.LoadInt( szKey, 0 );
			m_mHeroNeedMaterialCnt.insert( std::make_pair(i+1, iNeedCnt) );
		}

		for( int i = 0; i < iItemGradeMax; i++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "item_grade%d_cnt", i+1 );
			int iNeedCnt = kLoader.LoadInt( szKey, 0 );
			m_mBasicItemNeedMaterialCnt.insert( std::make_pair(i+1, iNeedCnt) );
		}
	
		//영웅, 아이템별 강화 필요 재료 수.
		for( int i = 0; i < iCharGradeMax; i++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "char_down_grade%d_cnt", i+1 );
			int iNeedCnt = kLoader.LoadInt( szKey, 0 );
			m_mHeroDownNeedMaterialCnt.insert( std::make_pair(i+1, iNeedCnt) );
		}

		for( int i = 0; i < iItemGradeMax; i++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "item_down_grade%d_cnt", i+1 );
			int iNeedCnt = kLoader.LoadInt( szKey, 0 );
			m_mBasicItemDownNeedMaterialCnt.insert( std::make_pair(i+1, iNeedCnt) );
		}

		for( int i=0; i<iEnableCharCnt; i++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "char%d", i+1 );
			kLoader.SetTitle( szKey );
			PowerUpCharInfo stPowerUpInfo;
			stPowerUpInfo.dwCharCode =  kLoader.LoadInt( "char_code", 0 );
			stPowerUpInfo.dwItemCode =  kLoader.LoadInt( "item_code", 0 );
			stPowerUpInfo.iMaxPowerUpGrade =  kLoader.LoadInt( "max_power_grade", 0 );
			for( int j=0; j<MAX_CHAR_DBITEM_SLOT; j++ )
			{
				static vItemGradeCodeInfo vCodeInfo;
				vCodeInfo.clear();
				char szTag[MAX_PATH]="";
			
				for(int k=0; k<stPowerUpInfo.iMaxPowerUpGrade; k++ )
				{
					switch(j)
					{
					case EQUIP_WEAPON:
						StringCbPrintf( szKey, sizeof( szKey ), "grade%d_weapon", k+1 );
						break;
					case EQUIP_ARMOR:
						StringCbPrintf( szKey, sizeof( szKey ), "grade%d_armor", k+1 );
						break;
					case EQUIP_HELM:
						StringCbPrintf( szKey, sizeof( szKey ), "grade%d_helmet", k+1 );
						break;
					case EQUIP_CLOAK:
						StringCbPrintf( szKey, sizeof( szKey ), "grade%d_cloak", k+1 );
						break;
					}
			
					int iItemCode = kLoader.LoadInt( szKey, 0 );
					vCodeInfo.push_back(iItemCode);
				}
				stPowerUpInfo.mPowerUpItemCodeInfo.insert( std::make_pair(j, vCodeInfo) );
			}
			m_mPowerUpCharList.insert( std::make_pair(stPowerUpInfo.dwCharCode, stPowerUpInfo) );
		}
	}
	//rare 장비 진화
	{
		char szKey[MAX_PATH]="";
		ioINILoader kLoader( "config/sp2_powerup_rare_manager.ini" );

		kLoader.SetTitle( "common" );

		int iMaxItem = kLoader.LoadInt( "max_item", 0 );
		m_vRareItemNeedMaterialCnt.push_back(0);

		for( int i = 0; i<10; i++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "grade%d_mtrl_cnt", i+1 );
			int iCount = kLoader.LoadInt( szKey, 0 );
			if( 0 == iCount )
				break;

			m_vRareItemNeedMaterialCnt.push_back(iCount);
		}

		for( int i = 0; i<10; i++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "down_grade%d_mtrl_cnt", i+1 );
			int iCount = kLoader.LoadInt( szKey, 0 );
			if( 0 == iCount )
				break;

			m_vRareItemDownNeedMaterialCnt.push_back(iCount);
		}

		for( int i = 0; i < iMaxItem; i++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "item%d", i+1 );
			kLoader.SetTitle( szKey );

			int iCode = kLoader.LoadInt( "item_code", 0 );
			
			int iGradeMax = kLoader.LoadInt( "grade_max", 0 );
			IntVec vCodeInfo;
			vCodeInfo.push_back(iCode);

			for( int j = 0; j < iGradeMax; j++ )
			{
				StringCbPrintf( szKey, sizeof( szKey ), "grade%d_code", j+1 );
				int iCode = kLoader.LoadInt( szKey, 0 );
				vCodeInfo.push_back(iCode);	
			}

			m_mPowerUpRareItemList.insert( std::make_pair(iCode, vCodeInfo) );
		}
	}
}

int ioPowerUpManager::GetBasicPowerUpItemCode(const int iItemCode, const int iClassType, const int iGrade)
{
	if( iGrade <= PUGT_NONE || iGrade > PUGT_CHAR_GRADE5 )
		return 0;

	//아이템 부위 추출
	int iEquipType = iItemCode / 100000;
	if( iEquipType < EQUIP_WEAPON || iEquipType > EQUIP_CLOAK )
		return 0;

	mPowerUpCharInfo::iterator iterChar = m_mPowerUpCharList.find(iClassType);
	if( iterChar == m_mPowerUpCharList.end() )
		return 0;

	PowerUpCharInfo stCharInfo = iterChar->second;
	mPowerUpItemCode::iterator iterItem = stCharInfo.mPowerUpItemCodeInfo.find(iEquipType);
	if( iterItem == stCharInfo.mPowerUpItemCodeInfo.end() )
		return 0;

	vItemGradeCodeInfo vItemCodeInfo = iterItem->second;
	if( (int)vItemCodeInfo.size() < iGrade )
		return 0;

	return vItemCodeInfo[iGrade-1];
}

int ioPowerUpManager::GetBasicPowerDownItemCode(const int iItemCode, const int iClassType, const int iGrade)
{
	if( iGrade < PUGT_NONE || iGrade > PUGT_CHAR_GRADE5 )
		return 0;

	//아이템 부위 추출
	int iEquipType = iItemCode / 100000;
	if( iEquipType < EQUIP_WEAPON || iEquipType > EQUIP_CLOAK )
		return 0;

	mPowerUpCharInfo::iterator iterChar = m_mPowerUpCharList.find(iClassType);
	if( iterChar == m_mPowerUpCharList.end() )
		return 0;

	PowerUpCharInfo stCharInfo = iterChar->second;
	mPowerUpItemCode::iterator iterItem = stCharInfo.mPowerUpItemCodeInfo.find(iEquipType);
	if( iterItem == stCharInfo.mPowerUpItemCodeInfo.end() )
		return 0;

	vItemGradeCodeInfo vItemCodeInfo = iterItem->second;
	if( (int)vItemCodeInfo.size() < iGrade )
		return 0;

	if(iGrade == 0)
	{
		return stCharInfo.dwItemCode;
	}

	return vItemCodeInfo[iGrade-1];
}


int ioPowerUpManager::ItemPowerUp(User *pUser, const int iTargetIndex, const int iCurMaterialCount, int &iNeedMaterialCount)
{
	//소유중인 아이템인지 체크
	ioUserExtraItem *pUserInven = pUser->GetUserExtraItem();
	if( !pUserInven )
		return POWER_UP_NO_TARGET;

	ioUserExtraItem::EXTRAITEMSLOT stExtraItem;
	pUserInven->GetExtraItem(iTargetIndex, stExtraItem);
	if( stExtraItem.m_iItemCode <= 0 )
		return POWER_UP_NO_TARGET;

	//영구아이템 체크
	if( stExtraItem.m_PeriodType != ioUserExtraItem::EPT_MORTMAIN )
		return POWER_UP_DISABLE_TARGET;

	//강화 불가능한 아이템인지 체크
	int iItemType = g_ExtraItemInfoMgr.GetExtraItemExtendType(stExtraItem.m_iItemCode);
	if( iItemType == EIET_DEFAULT ||  iItemType == EIET_DEFAULT_POWERUP )
		return POWER_UP_DISABLE_TARGET;
	//////////////////////////////////
	int iNextItemCode	= 0;

	if( EIET_RARE == iItemType || EIET_RARE_POWERUP == iItemType )
	{
		//레어 장비 진화.
		//기본 코드 변환
		int iDefaultCode = (stExtraItem.m_iItemCode / 10000) * 10000 + 1000 + (stExtraItem.m_iItemCode % 1000);

		mPowerUpRareItemInfo::iterator it = m_mPowerUpRareItemList.find(iDefaultCode);
		if( it == m_mPowerUpRareItemList.end() )
			return POWER_UP_DISABLE_TARGET;
	
		int iItemGrade = ConvertRareItemToRareItemGrade(stExtraItem.m_iItemCode);
		IntVec& vPowerUPCode = it->second;

		if( iItemGrade >= vPowerUPCode.size() - 1 || 0 > iItemGrade )
			return POWER_UP_DISABLE_TARGET;

		if( iItemGrade + 1 >= m_vRareItemNeedMaterialCnt.size() )
			return POWER_UP_DISABLE_TARGET;

		iNeedMaterialCount = m_vRareItemNeedMaterialCnt[iItemGrade + 1];
		if( iNeedMaterialCount <= 0 || iNeedMaterialCount > iCurMaterialCount )
			return POWER_UP_MATERIAL_SHORTAGE;

		iNextItemCode = vPowerUPCode[iItemGrade + 1];
		if( iNextItemCode <= 0 )
			return POWER_UP_DISABLE_TARGET;
	}
	else
	{
		//기본 장비 진화.
		int iClassType = stExtraItem.m_iItemCode % 1000;
		mPowerUpCharInfo::iterator iter = m_mPowerUpCharList.find(iClassType);
		if( iter == m_mPowerUpCharList.end() )
			return  POWER_UP_DISABLE_TARGET;

		//최대 아이템 등급체크
		int iItemGrade = stExtraItem.m_iItemCode % 100000 / PUGT_ITEM_GRADE_BASE * PUGT_ITEM_GRADE_BASE;
		int iTargetGrade = ConvertPowerUpTypeItemToChar(iItemGrade);
		PowerUpCharInfo stPowerupInfo = iter->second;
		if( stPowerupInfo.iMaxPowerUpGrade <= iTargetGrade )
			return POWER_UP_DISABLE_TARGET;


		//재료 부족 체크
		iNeedMaterialCount = GetBasicNeedMaterialCount(PUTT_ITEM, iTargetGrade+1);
		if( iNeedMaterialCount <= 0 || iNeedMaterialCount > iCurMaterialCount )
			return POWER_UP_MATERIAL_SHORTAGE;
		
		//강화 아이템 코드 Get
		iNextItemCode = GetBasicPowerUpItemCode(stExtraItem.m_iItemCode, iClassType, iTargetGrade+1);
		if( iNextItemCode <= 0 )
			return POWER_UP_DISABLE_TARGET;
	}
	
	//스킨이 존재할경우 삭제
	pUser->PowerUpItemSkinDelete(stExtraItem);
	pUserInven->SaveData();

	//봉인된, 가능 장비일 경우 해제
	if( stExtraItem.m_iTradeState != ioUserExtraItem::EET_DISABLE )
	{
		stExtraItem.m_iTradeState = ioUserExtraItem::EET_DISABLE;
	}

	//성공( 아이템 코드 변환 DB update는 etc아이템 쪽에서 )
	stExtraItem.m_iItemCode = iNextItemCode;
	pUserInven->SetExtraItem(stExtraItem);

	SP2Packet kReturn( STPK_POWER_UP_INFO );
	PACKET_GUARD_bool( kReturn.Write((BYTE)PUTT_ITEM) );
	PACKET_GUARD_bool( kReturn.Write(POWER_UP_SUCCESS) );
	PACKET_GUARD_bool( kReturn.Write(pUser->GetPublicID()));
	PACKET_GUARD_bool( kReturn.Write(iTargetIndex) );
	PACKET_GUARD_bool( kReturn.Write(stExtraItem.m_iItemCode) );
	PACKET_GUARD_bool( kReturn.Write((BYTE)PUGT_NONE) );		//등급(현재는 사용안하고있음)

	pUser->SendMessage( kReturn );

	return POWER_UP_SUCCESS;
}

int ioPowerUpManager::ItemPowerDown(User *pUser, const int iTargetIndex, const int iCurMaterialCount, int &iNeedMaterialCount)
{
	//소유중인 아이템인지 체크
	ioUserExtraItem *pUserInven = pUser->GetUserExtraItem();
	if( !pUserInven )
		return POWER_DOWN_NO_TARGET;

	ioUserExtraItem::EXTRAITEMSLOT stExtraItem;
	pUserInven->GetExtraItem(iTargetIndex, stExtraItem);
	if( stExtraItem.m_iItemCode <= 0 )
		return POWER_DOWN_NO_TARGET;

	//영구아이템 체크
	if( stExtraItem.m_PeriodType != ioUserExtraItem::EPT_MORTMAIN )
		return POWER_DOWN_DISABLE_TARGET;

	//강화 불가능한 아이템인지 체크
	int iItemType = g_ExtraItemInfoMgr.GetExtraItemExtendType(stExtraItem.m_iItemCode);
	if( iItemType == EIET_DEFAULT ||  iItemType == EIET_DEFAULT_POWERUP )
		return POWER_DOWN_DISABLE_TARGET;
	//////////////////////////////////
	int iNextItemCode	= 0;

	if( EIET_RARE == iItemType || EIET_RARE_POWERUP == iItemType )
	{
		//레어 장비 진화.
		//기본 코드 변환
		int iDefaultCode = (stExtraItem.m_iItemCode / 10000) * 10000 + 1000 + (stExtraItem.m_iItemCode % 1000);

		mPowerUpRareItemInfo::iterator it = m_mPowerUpRareItemList.find(iDefaultCode);
		if( it == m_mPowerUpRareItemList.end() )
			return POWER_DOWN_DISABLE_TARGET;
	
		int iItemGrade = ConvertRareItemToRareItemGrade(stExtraItem.m_iItemCode);
		IntVec& vPowerUPCode = it->second;

		if( iItemGrade >= vPowerUPCode.size() || 0 > iItemGrade )
			return POWER_DOWN_DISABLE_TARGET;

		if( iItemGrade >= m_vRareItemDownNeedMaterialCnt.size() )
			return POWER_DOWN_DISABLE_TARGET;

		iNeedMaterialCount = m_vRareItemDownNeedMaterialCnt[iItemGrade];
		if( iNeedMaterialCount <= 0 || iNeedMaterialCount > iCurMaterialCount )
			return POWER_DOWN_MATERIAL_SHORTAGE;

		iNextItemCode = vPowerUPCode[iItemGrade - 1];
		if( iNextItemCode <= 0 )
			return POWER_DOWN_DISABLE_TARGET;
	}
	else
	{
		//기본 장비 진화.
		int iClassType = stExtraItem.m_iItemCode % 1000;
		mPowerUpCharInfo::iterator iter = m_mPowerUpCharList.find(iClassType);
		if( iter == m_mPowerUpCharList.end() )
			return  POWER_DOWN_DISABLE_TARGET;

		//최대 아이템 등급체크
		int iItemGrade = stExtraItem.m_iItemCode % 100000 / PUGT_ITEM_GRADE_BASE * PUGT_ITEM_GRADE_BASE;
		int iTargetGrade = ConvertPowerUpTypeItemToChar(iItemGrade);
		PowerUpCharInfo stPowerupInfo = iter->second;
		if( 0 >= iTargetGrade )
			return POWER_DOWN_DISABLE_TARGET;


		//재료 부족 체크
		iNeedMaterialCount = GetBasicNeedMaterialCount(PDTT_ITEM, iTargetGrade);
		if( iNeedMaterialCount <= 0 || iNeedMaterialCount > iCurMaterialCount )
			return POWER_DOWN_MATERIAL_SHORTAGE;
		
		//강화 아이템 코드 Get
		iNextItemCode = GetBasicPowerDownItemCode(stExtraItem.m_iItemCode, iClassType, iTargetGrade-1);
		if( iNextItemCode <= 0 )
			return POWER_DOWN_DISABLE_TARGET;
	}
	
	//스킨이 존재할경우 삭제
	pUser->PowerUpItemSkinDelete(stExtraItem);
	pUserInven->SaveData();

	//봉인된, 가능 장비일 경우 해제
	if( stExtraItem.m_iTradeState != ioUserExtraItem::EET_DISABLE )
	{
		stExtraItem.m_iTradeState = ioUserExtraItem::EET_DISABLE;
	}

	//성공( 아이템 코드 변환 DB update는 etc아이템 쪽에서 )
	stExtraItem.m_iItemCode = iNextItemCode;
	pUserInven->SetExtraItem(stExtraItem);

	SP2Packet kReturn( STPK_POWER_UP_INFO );
	PACKET_GUARD_bool( kReturn.Write((BYTE)PDTT_ITEM) );
	PACKET_GUARD_bool( kReturn.Write(POWER_UP_SUCCESS) );
	PACKET_GUARD_bool( kReturn.Write(pUser->GetPublicID()));
	PACKET_GUARD_bool( kReturn.Write(iTargetIndex) );
	PACKET_GUARD_bool( kReturn.Write(stExtraItem.m_iItemCode) );
	PACKET_GUARD_bool( kReturn.Write((BYTE)PUGT_NONE) );		//등급(현재는 사용안하고있음)

	pUser->SendMessage( kReturn );

	return POWER_UP_SUCCESS;
}


int ioPowerUpManager::HeroPowerUp(User *pUser, const int iTargetIndex, const int iCurMaterialCount, int &iNeedMaterialCount)
{
	//소유중인 용병인지 체크
	int iCharArray = pUser->GetCharArray(iTargetIndex);
	if( iCharArray == -1 )
		return POWER_UP_NO_TARGET;

	ioCharacter* pCharInfo = pUser->GetCharacter( iCharArray );
	if( !pCharInfo )
		return POWER_UP_NO_TARGET;

	//영구 용병 체크
	if( !pCharInfo->IsMortmain() )
		return POWER_UP_DISABLE_TARGET;

	//대여중인이 체크
	if( pCharInfo->GetRentalType() == CRT_RENTAL )
		return POWER_UP_DISABLE_TARGET;

	//강화가능한 용병 체크
	int iClassType = pCharInfo->GetClassType();
	mPowerUpCharInfo::iterator iter = m_mPowerUpCharList.find(iClassType);
	if( iter == m_mPowerUpCharList.end() )
		return  POWER_UP_DISABLE_TARGET;

	//영웅 강화 성공 처리
	ioClassExpert *pClassExpert = pUser->GetClassExpert();
	if( !pClassExpert )
		return POWER_UP_EXCEPTION;

	if( !pClassExpert->IsExistExpertInfo(iClassType) )
	{
		//ClassExpertDB Insert
		pClassExpert->CreateClassExpertInfo( iClassType );
	}
	
	//용병의 강화가 최대치 인지 체크
	PowerUpCharInfo stPowerupInfo = iter->second;
	int iReinforceGrade = pClassExpert->GetClassReinfoce(iClassType);
	
	if( stPowerupInfo.iMaxPowerUpGrade <= iReinforceGrade )
		return POWER_UP_DISABLE_TARGET;

	//재료 부족 체크
	iNeedMaterialCount = GetBasicNeedMaterialCount(PUTT_CHAR, iReinforceGrade+1);
	if( iNeedMaterialCount <= 0 || iNeedMaterialCount > iCurMaterialCount )
		return POWER_UP_MATERIAL_SHORTAGE;

	if( !pClassExpert->SetClassReinforce(iClassType, iReinforceGrade+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][expert] no exist class type : [type:%d userID:%s]", iClassType, pUser->GetPublicID().c_str() );
		return POWER_UP_EXCEPTION;
	}

	pCharInfo->SetCharReinforceGrade(iReinforceGrade+1);

	SP2Packet kReturn( STPK_POWER_UP_INFO );
	PACKET_GUARD_bool( kReturn.Write((BYTE)PUTT_CHAR) );
	PACKET_GUARD_bool( kReturn.Write(POWER_UP_SUCCESS) );
	PACKET_GUARD_bool( kReturn.Write(pUser->GetPublicID()));
	PACKET_GUARD_bool( kReturn.Write(iTargetIndex) );
	PACKET_GUARD_bool( kReturn.Write(iClassType) );
	PACKET_GUARD_bool( kReturn.Write((BYTE)pCharInfo->GetCharReinforceGrade()) );

	Room* pRoom = pUser->GetMyRoom();

	if( pRoom )
	{
		if( pRoom->GetModeType() == MT_HEADQUARTERS )
		{
			HeadquartersMode* pMode = dynamic_cast<HeadquartersMode*>( pRoom->GetModeInfo() );
			if( pMode )
				pMode->SetCharacterReinforceInfo(iClassType, pCharInfo->GetCharReinforceGrade());
		}

		pRoom->RoomSendPacketTcp( kReturn );
	}
	else
		pUser->SendMessage( kReturn );


	//장착중인 무기 해제 후 강화 무기 default로 장착
	pUser->TakeOffCharExtraItem(pCharInfo, EQUIP_WEAPON);
	pUser->TakeOffCharExtraItem(pCharInfo, EQUIP_ARMOR);
	pUser->TakeOffCharExtraItem(pCharInfo, EQUIP_HELM);
	pUser->TakeOffCharExtraItem(pCharInfo, EQUIP_CLOAK);

	pClassExpert->SaveData();

	return POWER_UP_SUCCESS;
}


int ioPowerUpManager::HeroPowerDown(User *pUser, const int iTargetIndex, const int iCurMaterialCount, int &iNeedMaterialCount)
{
	//소유중인 용병인지 체크
	int iCharArray = pUser->GetCharArray(iTargetIndex);
	if( iCharArray == -1 )
		return POWER_DOWN_NO_TARGET;

	ioCharacter* pCharInfo = pUser->GetCharacter( iCharArray );
	if( !pCharInfo )
		return POWER_DOWN_NO_TARGET;

	//영구 용병 체크
	if( !pCharInfo->IsMortmain() )
		return POWER_DOWN_DISABLE_TARGET;

	//대여중인이 체크
	if( pCharInfo->GetRentalType() == CRT_RENTAL )
		return POWER_DOWN_DISABLE_TARGET;

	//강화가능한 용병 체크
	int iClassType = pCharInfo->GetClassType();
	mPowerUpCharInfo::iterator iter = m_mPowerUpCharList.find(iClassType);
	if( iter == m_mPowerUpCharList.end() )
		return  POWER_DOWN_DISABLE_TARGET;

	//영웅 강화 성공 처리
	ioClassExpert *pClassExpert = pUser->GetClassExpert();
	if( !pClassExpert )
		return POWER_DOWN_EXCEPTION;

	if( !pClassExpert->IsExistExpertInfo(iClassType) )
	{
		return POWER_DOWN_EXCEPTION;
	}
	
	//용병의 강화가 최하 기본형인지 체크 인지 체크
	int iReinforceGrade = pClassExpert->GetClassReinfoce(iClassType);
	if( 0 >= iReinforceGrade )
		return POWER_UP_DISABLE_TARGET;

	//재료 부족 체크
	iNeedMaterialCount = GetBasicNeedMaterialCount(PDTT_CHAR, iReinforceGrade);
	if( iNeedMaterialCount <= 0 || iNeedMaterialCount > iCurMaterialCount )
		return POWER_UP_MATERIAL_SHORTAGE;

	if( !pClassExpert->SetClassReinforce(iClassType, iReinforceGrade-1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][expert] no exist class type : [type:%d userID:%s]", iClassType, pUser->GetPublicID().c_str() );
		return POWER_UP_EXCEPTION;
	}

	pCharInfo->SetCharReinforceGrade(iReinforceGrade-1);

	SP2Packet kReturn( STPK_POWER_UP_INFO );
	PACKET_GUARD_bool( kReturn.Write((BYTE)PDTT_CHAR) );
	PACKET_GUARD_bool( kReturn.Write(POWER_UP_SUCCESS) );
	PACKET_GUARD_bool( kReturn.Write(pUser->GetPublicID()));
	PACKET_GUARD_bool( kReturn.Write(iTargetIndex) );
	PACKET_GUARD_bool( kReturn.Write(iClassType) );
	PACKET_GUARD_bool( kReturn.Write((BYTE)pCharInfo->GetCharReinforceGrade()) );

	Room* pRoom = pUser->GetMyRoom();

	if( pRoom )
	{
		if( pRoom->GetModeType() == MT_HEADQUARTERS )
		{
			HeadquartersMode* pMode = dynamic_cast<HeadquartersMode*>( pRoom->GetModeInfo() );
			if( pMode )
				pMode->SetCharacterReinforceInfo(iClassType, pCharInfo->GetCharReinforceGrade());
		}

		pRoom->RoomSendPacketTcp( kReturn );
	}
	else
		pUser->SendMessage( kReturn );


	//장착중인 무기 해제 후 강화 무기 default로 장착
	pUser->TakeOffCharExtraItem(pCharInfo, EQUIP_WEAPON);
	pUser->TakeOffCharExtraItem(pCharInfo, EQUIP_ARMOR);
	pUser->TakeOffCharExtraItem(pCharInfo, EQUIP_HELM);
	pUser->TakeOffCharExtraItem(pCharInfo, EQUIP_CLOAK);

	pClassExpert->SaveData();

	return POWER_UP_SUCCESS;
}


int ioPowerUpManager::TargetPowerUp(User *pUser, const int iTargetType, const int iTargetIndex, const int iCurMaterialCount, int &iNeedMaterialCount )
{
	switch(iTargetType)
	{
	case PDTT_CHAR:
		return HeroPowerDown(pUser, iTargetIndex, iCurMaterialCount, iNeedMaterialCount);
	case PDTT_ITEM:
		return ItemPowerDown(pUser, iTargetIndex, iCurMaterialCount, iNeedMaterialCount);
	case PUTT_CHAR:
		return HeroPowerUp(pUser, iTargetIndex, iCurMaterialCount, iNeedMaterialCount);
	case PUTT_ITEM:
		return ItemPowerUp(pUser, iTargetIndex, iCurMaterialCount, iNeedMaterialCount);
	}

	return POWER_UP_EXCEPTION;
}

int ioPowerUpManager::GetBasicNeedMaterialCount(const int iTargetType, const int iTargetGrade)
{
	mNeedMaterialCnt::iterator iter;

	if( iTargetType == PUTT_CHAR )
	{
		//용병 강화
		iter = m_mHeroNeedMaterialCnt.find(iTargetGrade);
		if( iter == m_mHeroNeedMaterialCnt.end() )
			return -1;

		return iter->second;
	}
	else if( iTargetType == PUTT_ITEM )
	{
		//아이템 강화
		iter = m_mBasicItemNeedMaterialCnt.find(iTargetGrade);
		if( iter == m_mBasicItemNeedMaterialCnt.end() )
			return -1;

		return iter->second;
	}
	else if( iTargetType == PDTT_CHAR )
	{
		//용병 강화
		iter = m_mHeroDownNeedMaterialCnt.find(iTargetGrade);
		if( iter == m_mHeroDownNeedMaterialCnt.end() )
			return -1;

		return iter->second;
	}
	else if( iTargetType == PDTT_ITEM )
	{
		//아이템 강화
		iter = m_mBasicItemDownNeedMaterialCnt.find(iTargetGrade);
		if( iter == m_mBasicItemDownNeedMaterialCnt.end() )
			return -1;

		return iter->second;
	}

	return -1;
}

int ioPowerUpManager::ConvertPowerUpTypeCharToItem( int iCharGradeType )
{
	switch( iCharGradeType )
	{
		case PUGT_CHAR_GRADE1: return PUGT_ITEM_GRADE1;
		case PUGT_CHAR_GRADE2: return PUGT_ITEM_GRADE2;
		case PUGT_CHAR_GRADE3: return PUGT_ITEM_GRADE3;
		case PUGT_CHAR_GRADE4: return PUGT_ITEM_GRADE4;
		case PUGT_CHAR_GRADE5: return PUGT_ITEM_GRADE5;
	}
	return PUGT_NONE;
}

int ioPowerUpManager::ConvertPowerUpTypeItemToChar(int iItemGradeType)
{
	switch( iItemGradeType )
	{
		case PUGT_ITEM_GRADE1: return PUGT_CHAR_GRADE1;
		case PUGT_ITEM_GRADE2: return PUGT_CHAR_GRADE2;
		case PUGT_ITEM_GRADE3: return PUGT_CHAR_GRADE3;
		case PUGT_ITEM_GRADE4: return PUGT_CHAR_GRADE4;
		case PUGT_ITEM_GRADE5: return PUGT_CHAR_GRADE5;
	}
	return PUGT_NONE;
}

int ioPowerUpManager::ConvertRareItemToRareItemGrade( int iItemCode )
{
	int iType = iItemCode % 100000 % PUGT_ITEM_GRADE_BASE / PURIGT_RARE_ITEM_GRADE_BASE * PURIGT_RARE_ITEM_GRADE_BASE;
	switch( iType )
	{
		case PURIGT_RARE_ITEM_GRADE1: return 1;
		case PURIGT_RARE_ITEM_GRADE2: return 2;
		case PURIGT_RARE_ITEM_GRADE3: return 3;
		case PURIGT_RARE_ITEM_GRADE4: return 4;
		case PURIGT_RARE_ITEM_GRADE5: return 5;
	}

	return 0;
}

int ioPowerUpManager::GetPowerUpItemType(int iItemCode)
{
	return iItemCode % 100000 / PUGT_ITEM_GRADE_BASE * PUGT_ITEM_GRADE_BASE;
}

int ioPowerUpManager::GetPowerUpItemGrade(int iItemCode)
{
	int iPowerUpItemType = GetPowerUpItemType(iItemCode);
	return ConvertPowerUpTypeItemToChar(iPowerUpItemType);
}