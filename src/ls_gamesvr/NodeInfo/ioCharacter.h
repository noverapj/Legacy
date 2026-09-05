

#ifndef _ioCharacter_h_
#define _ioCharacter_h_

#include "NodeHelpStructDefine.h"
#include "ioEquipSlot.h"

class SP2Packet;
class User;
class ioInventory;

#define VIRTUAL_CHAR_INDEX	0xffffffff

class ioCharacter
{
private:
	friend class User;
	friend class ioCharacter;

private:
	// 체험용병 index는 4284967295 ~ 4294967294 까지 천만개를 사용.
	// 실제용병 index가 4284967295 가까워지면 확장 고려할 것
	DWORD m_dwCharIndex;

	CHARACTER m_CharInfo;
	CHARACTER m_ExperienceCharInfo;        // 체험시 치장 정보 - 이 정보는 1회용이므로 저장을하면 안됨
	ITEM_DATA m_DBItemData[MAX_CHAR_DBITEM_SLOT];

	struct BackData
	{
		DWORD m_dwCharIndex;
		CHARACTER m_CharInfo;
	};
	BackData m_BackUP;
    
	ioEquipSlot m_EquipSlot;

	bool m_bExtraItemChange;
	bool m_bPlayJoined;
	bool m_bCharDie;

	DWORD m_dwLimitTimer;
	DWORD m_dwLimitSecond;
	DWORD m_dwRentalCheckTimer;
	
public:
	void Initialize();
	void SetCharInfo( DWORD dwIndex, const CHARACTER &rkInfo, User *pUser, bool bSetReinforce = true );
	void ChangeDBExtraItem( User *pUser, int iSlotIndex );
	void CheckExtraItemEquip(User *pUser);

public:
	void InitDBItemList();

public:
	ioItem* EquipItem( ioItem *pItem );
	ioItem* EquipItem( int iSlot, ioItem *pItem );
	ioItem* ReleaseItem( int iSlot );
	ioItem* ReleaseItem( int iGameIndex, int iItemCode );

	void ClearEquipSlot();
	void ClearOwnerNameInEquipSlot( const ioHashString &rkName );
	
	bool IsSlotEquiped( int iSlot ) const;
	const ioItem* GetItem( int iSlot ) const;

public:
	void SetDBItemData( int iSlot, int iItemCode, int iReinforce, DWORD dwMaleCustom, DWORD dwFemaleCustom );
	void SetDefaultPowerUpItem(int iSlot);

	const ITEM_DATA* GetDBItemData( int iSlot ) const;
	const int        GetCurrentItemCode( int iSlot ) const;
	const int        GetCurrentItemReinforce( int iSlot ) const;
	const int        GetCurrentItemMaleCustom( int iSlot ) const;
	const int        GetCurrentItemFemaleCustom( int iSlot ) const;
	const int		 GetExtraItemIndex( int iSlot) const;
	const CHARACTER& GetCharInfo() const { return m_CharInfo; }
	const CHARACTER& GetExperienceCharInfo() const { return m_ExperienceCharInfo; }

	bool SetCharDecoration( int iType, int iCode );
	void SetCharAllDecoration( ioInventory &rkInventory );
	void SetChangeKindred( const CHARACTER& rkCharInfo, DWORD dwRandSeed );
	void SetChangeExtraItem( int iSlot, int iNewSlotIndex );
	void SetExperienceChar( CHARACTER &rkExperienceChar );

	int IncreaseStat( int iStat );
	void InitStat();

	void FillEquipItemInfo( SP2Packet &rkPacket );
	void FillDBExtraItemInfo( SP2Packet &rkPacket, User *pUser, bool bAll );
	void FillEquipCostumeInfo( SP2Packet &rkPacket );
	void FillEquipAccessoryInfo( SP2Packet &rkPacket, User *pUser );

	void SetCharDie( bool bCharDie );
	inline bool IsCharDie() const { return m_bCharDie; }

	// 기본장착장비의 item code을 반환한다. 
	int GetItemCodeOfDefaultEquippedItem( int iSlot );

public:
	inline DWORD GetCharIndex() const { return m_dwCharIndex; }
	inline void  SetCharIndex( DWORD dwIdx ) { m_dwCharIndex = dwIdx; }

	inline const int  GetCharSlotIndex() const { return m_CharInfo.m_iSlotIndex; }
	inline void SetCharSlotIndex( int iSlotIndex ){ m_CharInfo.m_iSlotIndex = iSlotIndex; }

public:
	bool StartLimitTimer( DWORD dwCheckSecond );
	bool UpdateLimitTimer();
	void SetCharLimitExtend( int iLimitDate );
	int  GetCharLimitDate(){ return m_CharInfo.m_iLimitSecond; }
	void SetCharLimitDate( int iLimitDate );
	void SetCharLimitExtendDate( int iLimitDate );		// 기간제 기간 연장...

	int GetGapLimitTime();

	inline DWORD GetLimitCheckSecond(){ return m_dwLimitSecond; }

	bool IsActive(){ return m_CharInfo.m_bActive; }       //기간이 지났으면 false
	void SetActive( bool bActive ){ m_CharInfo.m_bActive = bActive; }

    bool HasExerciseStyle(BYTE chStyle );           // &연산이 아닌 비교다.
	void SetExerciseStyle(BYTE chStyle );

	bool IsMortmain() { return m_CharInfo.m_ePeriodType == CPT_MORTMAIN; }
	void SetPeriodType( CharPeriodType ePeriodType ) { m_CharInfo.m_ePeriodType = ePeriodType; }

	inline void  SetLeaderType( short sLeaderType ){ m_CharInfo.m_sLeaderType = sLeaderType; }
	inline short GetLeaderType() { return m_CharInfo.m_sLeaderType; }

	inline void  SetRentalType( short sRentalType ){ m_CharInfo.m_sRentalType = sRentalType; }
	inline const short GetRentalType() const { return m_CharInfo.m_sRentalType; }

	inline void SetRentalLimitTime( DWORD dwLimitTime ){ m_CharInfo.m_dwRentalMinute = dwLimitTime; }
	inline DWORD GetRentalLimitTime(){ return m_CharInfo.m_dwRentalMinute; }
	void CheckRentalLimitTime();

	inline int GetClassType() { return m_CharInfo.m_class_type; }

	void SetAwakeInit();
	inline int GetAwakeType() { return m_CharInfo.m_chAwakeType; }

	void SetAwakeInfo( int iAwakeType, int iEndDate );
	void SetAwakeTime( int iEndDate );

	inline int GetAwakeLimitTime() { return m_CharInfo.m_iAwakeLimitTime; }

	inline int GetCharReinforceGrade() { return  m_CharInfo.m_byReinforceGrade; }
	inline void SetCharReinforceGrade( int iGrade ) { m_CharInfo.m_byReinforceGrade = iGrade; }

	//costume
	void ChangeCostumeItem( int iSlot, DWORD dwIndex, DWORD dwCode );
	int GetCostumeIndex(int iSlot) { return m_CharInfo.m_costume_item[iSlot].m_iCostumeIndex; }
	int GetCostumeCode(int iSlot) { return m_CharInfo.m_costume_item[iSlot].m_iCostumeCode; }
	int GetCostumeSlot(int iIndex);

	//Accessory
	void ChangeAccessoryItem( int iSlot, DWORD dwIndex, DWORD dwCode, int iValue, DWORD dwComposeCode, int iComposeValue );
	int GetAccessoryIndex(int iSlot) { return m_CharInfo.m_accessory_item[iSlot].m_iAccessoryIndex; }
	int GetAccessoryCode(int iSlot) { return m_CharInfo.m_accessory_item[iSlot].m_iAccessoryCode; }
	int GetAccessoryValue(int iSlot) { return m_CharInfo.m_accessory_item[iSlot].m_iAccessoryValue; }
	int GetComposeCode(int iSlot) { return m_CharInfo.m_accessory_item[iSlot].m_iComposeCode; }
	int GetComposeValue(int iSlot) { return m_CharInfo.m_accessory_item[iSlot].m_iComposeValue; }
	int GetAccessorySlot(int iIndex);

public:
	bool IsCharEquipExtraItemPeriodCheck( User *pUser, int iPeriodType );

public:
	bool IsChange();
	void BackUp();
	
	bool IsChangeActive();

public:
	void CopyData( ioCharacter *pCharacter );

public:
	void TakeOffExtraItem( int iEquipType );

public:
	ioCharacter();
	~ioCharacter();
};

class CharInfoSort
{
public:
	bool operator()( const ioCharacter *lhs , const ioCharacter *rhs ) const
	{
		if( lhs && rhs )
		{
			if( lhs->GetRentalType() > rhs->GetRentalType() )
			{
				return true;
			}
			else if( lhs->GetRentalType() == rhs->GetRentalType() )
			{
                if( lhs->GetCharSlotIndex() < rhs->GetCharSlotIndex() )
					return true;
			}	
		}
		return false;
	}
};

#endif