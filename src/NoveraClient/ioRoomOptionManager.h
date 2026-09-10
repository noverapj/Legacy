#pragma once


#include "EncryptValue.h"

struct RoomOptionInfo
{
	CEncrypt<float> m_fValue;
	ioHashString m_Desc;
	ioHashString m_UIDesc;

	RoomOptionInfo()
	{
		m_fValue = FLOAT1;
	}
};
typedef std::vector< RoomOptionInfo* > RoomOptionInfoList;

struct PreSetInfo
{
	ioHashString m_Desc;

	int m_iTeamAttackType;
	int m_iChangeCharType;
	int m_iCoolTimeType;
	CEncrypt<int> m_iRedHPType;
	CEncrypt<int> m_iBlueHPType;
	int m_iDropDamageType;
	int m_iGravityType;
	int m_iGetUpType;
	int m_iKOType;
	int m_iKOEffectType;
	int m_iRedBlowType;
	int m_iBlueBlowType;
	int m_iRedMoveSpeedType;
	int m_iBlueMoveSpeedType;
	int m_iRedEquipType;
	int m_iBlueEquipType;

	int m_iCatchModeRoundType;
	int m_iCatchModeRoundTimeType;

	int m_iGrowthUseType;
	int m_iExtraItemUseType;

	PreSetInfo()
	{
		Init();
	}

	void Init()
	{
		m_Desc.Clear();

		m_iTeamAttackType = 0;
		m_iChangeCharType = 0;
		m_iCoolTimeType = 0;
		m_iRedHPType = 0;
		m_iBlueHPType = 0;
		m_iDropDamageType = 0;
		m_iGravityType = 0;
		m_iGetUpType = 0;
		m_iKOType = 0;
		m_iKOEffectType = 0;
		m_iRedBlowType = 0;
		m_iBlueBlowType = 0;
		m_iRedMoveSpeedType = 0;
		m_iBlueMoveSpeedType = 0;
		m_iRedEquipType = 0;
		m_iBlueEquipType = 0;

		m_iCatchModeRoundType = 0;
		m_iCatchModeRoundTimeType = 0;

		m_iGrowthUseType = 0;
		m_iExtraItemUseType = 0;
	}
};
typedef std::vector< PreSetInfo* > PreSetInfoList;

class ioRoomOptionManager : public Singleton< ioRoomOptionManager >
{
protected:
	IntVec m_EnableOptionModeList;
	IntVec m_EnableRoundOptionModeList;
	IntVec m_NoChallengerModeList;

	RoomOptionInfoList m_RoomChangeCharList;
	RoomOptionInfoList m_RoomRedHPList;
	RoomOptionInfoList m_RoomBlueHPList;
	RoomOptionInfoList m_RoomRedCurHPList;
	RoomOptionInfoList m_RoomBlueCurHPList;
	RoomOptionInfoList m_RoomCoolTimeList;
	RoomOptionInfoList m_RoomDropDamageList;
	RoomOptionInfoList m_RoomGravityList;
	RoomOptionInfoList m_RoomTeamAttackList;
	RoomOptionInfoList m_RoomGetUpList;
	RoomOptionInfoList m_RoomKOList;
	RoomOptionInfoList m_RoomKOEffectList;
	RoomOptionInfoList m_RoomRedBlowList;
	RoomOptionInfoList m_RoomBlueBlowList;
	RoomOptionInfoList m_RoomRedMoveSpeedList;
	RoomOptionInfoList m_RoomBlueMoveSpeedList;
	RoomOptionInfoList m_RoomRedEquipList;
	RoomOptionInfoList m_RoomBlueEquipList;

	RoomOptionInfoList m_RoomRoundTypeList;
	RoomOptionInfoList m_RoomRoundTimeTypeList;

	RoomOptionInfoList m_RoomGrowthUseList;
	RoomOptionInfoList m_RoomExtraItemUseList;

	PreSetInfoList m_PreSetList;
	PreSetInfo m_DefaultInfo;

	int m_iEnableLevel;
	int m_iNoChallengerGrade;		// 도전자 받기 계급

protected:
	void ClearAllInfo();

public:
	void LoadRoomOptionInfo();

	bool CheckChangeCharType( int iIndex );
	bool CheckEnableTeamAttack( int iIndex );
	bool CheckEnableTeamDamage( int iIndex );
	bool CheckEnableGrowthUse( int iIndex );
	bool CheckEnableExtraItemUse( int iIndex );

	float GetRedHPRate( int iIndex );
	float GetBlueHPRate( int iIndex );
	float GetCoolTimeRate( int iIndex );
	float GetDropDamageRate( int iIndex );
	float GetGravityRate( int iIndex );
	float GetGetUpRate( int iIndex );
	float GetKORate( int iIndex );
	float GetKOEffectRate( int iIndex );
	float GetRedBlowRate( int iIndex );
	float GetBlueBlowRate( int iIndex );
	float GetRedMoveSpeedRate( int iIndex );
	float GetBlueMoveSpeedRate( int iIndex );
	float GetRedEquipRate( int iIndex );
	float GetBlueEquipRate( int iIndex );


	ioHashString GetChangeCharDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetRedHPDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetBlueHPDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetCoolTimeDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetDropDamageDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetGravityDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetTeamAttackDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetGetUpDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetKODesc( int iIndex, bool bUiDesc = false );
	ioHashString GetKOEffectDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetRedBlowDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetBlueBlowDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetRedMoveSpeedDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetBlueMoveSpeedDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetRedEquipDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetBlueEquipDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetRoundTypeDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetRoundTimeTypeDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetGrowthUseDesc( int iIndex, bool bUiDesc = false );
	ioHashString GetExtraItemUseDesc( int iIndex, bool bUiDesc = false );

	bool IsEnableOptionMode( int iModeIndex );
	bool IsEnableRoundOptionMode( int iModeIndex );
	bool IsNoChallengerMode( int iModeIndex );

	int GetOptionTypeCnt( int iOptionType );

	ioHashString GetPreSetDesc( int iIndex );
	bool GetPreSetInfo( IN int iPreSetNum, OUT PreSetInfo &rkPreSetInfo );
	bool GetDefaultInfo( OUT PreSetInfo &rkPreSetInfo );

	inline int GetEnableLevel() const { return m_iEnableLevel; }
	inline int GetNoChallengerGrade() const { return m_iNoChallengerGrade; }

public:
	static ioRoomOptionManager& GetSingleton();

public:
	ioRoomOptionManager();
	virtual ~ioRoomOptionManager();
};

#define g_RoomOptionMgr ioRoomOptionManager::GetSingleton()

