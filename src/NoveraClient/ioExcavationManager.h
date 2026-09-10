#pragma once

#include "DataHeaders/LSC_Excavation_info.h"
#include "DataHeaders/LSC_Excavation_grade.h"

class ioPlayStage;
class ioExcavationManager : public Singleton< ioExcavationManager >
{
public:
	enum 
	{
		FAIL_ARTIFACT_START_TYPE   = 1000,
		EXTRAITEM_START_TYPE       = 10000,
	};

	enum
	{
		RESULT_ARTIFACT = 1,
		RESULT_ITEM = 2,
	};
	enum ExcavationState
	{
		ES_DISABLE,  // 비활성
		ES_CHARGING, // 게이지 회복중
		ES_STANDBY,  // 삽질 가능
		ES_DIGGING,  // 삽질중
	};

	enum ErrorMsg
	{
		ERR_COOL_TIME,
		ERR_NEED_KIT,
		ERR_NEED_SHOVEL,
		ERR_INVAILD_MODE,
		ERR_INVAILD_POS,
		ERR_NO_EQUIP_KIT,
		ERR_SHOVEL_MAX_COUNT,
		ERR_INVALID_TIME,
		ERR_SHOVEL_CHARGE_FAIL,
		ERR_EXCEPTION,
		ERR_MAX,
	};

	enum SystemMsg
	{
		MSG_SUCCESS = 0,
		MSG_MAX,
	};

	enum
	{
		PACKET_SEND_DELAY = 150
	};

	enum
	{
		EQUIPMENT_SHOVEL,
		EQUIPMENT_DRILL,
		EQUIPMENT_FORKCRANE
	};

private:
	struct ExcavationUserInfo
	{
		ioHashString name;
		bool is_success;
		DWORD result_show_time;
		int item_idx;
		bool is_end;
		ExcavationUserInfo();
		ExcavationUserInfo( const ioHashString &szName, bool IsSuccess, DWORD dwTime, int iItemIdx )
		{
			name             = szName;
			is_success       = IsSuccess;
			result_show_time = dwTime;
			item_idx         = iItemIdx;
			is_end           = false;
		}
	};
	typedef std::vector<ExcavationUserInfo> vExcavationUserInfo;

	struct RewardIcon
	{
		int index;
		ioUIRenderImage *icon;
		RewardIcon()
		{
			index = 0;
			icon = NULL;
		}
	};
	typedef std::vector<RewardIcon> vRewardIcon;

	struct UserExcavationLvInfo
	{
		ioHashString public_id;
		int excavation_lv;
		UserExcavationLvInfo();
		UserExcavationLvInfo( const ioHashString &szName, int iLevel )
		{
			public_id = szName;
			excavation_lv = iLevel;
		}
	};
	typedef std::vector<UserExcavationLvInfo> vUserExcavationLvInfo;

	struct IconInfo
	{
		int min_lv;
		int max_lv;
		ioHashString use_icon_name;
		bool EnableIcon( int iLevel )
		{
			return COMPARE( iLevel, min_lv, max_lv+1 )?true:false;
		}
	};
	typedef std::vector<IconInfo> vIconInfo;

	struct ResultInfo
	{
		int m_iAddExp;
		int m_iClassType;
		int m_iAddSoldierExp;
		int m_iAddGradeExp;
		bool m_bLevelUp;
		bool m_bGradeLevelUp;

		void Clear()
		{
			m_iAddExp        = 0;
			m_iClassType     = 0; 
			m_iAddSoldierExp = 0;
			m_iAddGradeExp	 = 0;
			m_bLevelUp       = false;
			m_bGradeLevelUp  = false;
		}

		bool IsEmpty()
		{
			if( m_iAddExp == 0 && m_iClassType == 0 && m_iAddSoldierExp == 0 && m_iAddGradeExp == 0 && m_bLevelUp == false && m_bGradeLevelUp == false )
				return true;

			return false;
		}

		ResultInfo()
		{
			Clear();
		}
	};

private:
	ioPlayStage *m_pStage;
	DWORD m_dwKitEtcItemCode;
	DWORD m_dwKitItemCode;
	DWORD m_dwResultTime;
	ioHashString m_szEmoticonExcavating;
	ioHashString m_szEmoticonDigging;
	ioHashString m_szEmoticonFail;

	int   m_iCoolTimeCheckLv;
	DWORD m_dwCoolTimeBonus;
	DWORD m_dwDefaultCoolTime;
	DWORD m_dwMinimumCoolTime;

	int   m_iSpeedCheckLv;
	float m_fSpeedBonus;
	float m_fDefaultSpeed;
	float m_fMinimumSpeed;

	DWORD m_dwRemainChargeTime;

	int m_iExcavationState;
	DWORD m_dwStateCheckTime;

	LSC_Excavation_info_Manager  *m_pInfoManager;
	LSC_Excavation_grade_Manager *m_pGradeManager;

	char m_ErrorMsg[ERR_MAX][MAX_PATH];
	char m_SysMsg[MSG_MAX][MAX_PATH];
	D3DXVECTOR3 m_ExcavationPoint;

	float m_fLimitAngle;
	float m_fHeightOffset;
	float m_fDistanceHeightOffset;
	float m_fSight;
	ioHashString m_szGuideEffectName;
	ioHashString m_szPointEffectName;

	DWORD m_dwShowResultTime;

	vExcavationUserInfo m_vExcavationUser;
	int m_iShovelCount;
	int m_iMaxShovelCount;
	DWORD m_dwPacketSendTime;

	bool m_bSendRechargeProcess;

	int m_iReserveItemType;
	int m_iReserveItemIndex;

	vRewardIcon m_vRewardIcon;

	int m_iManualIndex;

	int m_iEffectGrade;

	vUserExcavationLvInfo m_vUserExcavationLv;

	vIconInfo m_vIconInfo;

	ResultInfo m_kResultInfo;

public:
	static ioExcavationManager &GetSingleton();

public:
	void Load();

private:
	void LoadINI();
	void LoadData();

public:
	void SetDisableState();
	void SetChargingState();
	void SetStandByState();
	void SetDiggingState();

	void SetDiggingMotion( const ioHashString &szName );

public:
	void CheckResultMotion();

public:
	void ProcessExcavtion();
	void ProcessRecharge();
	void SetExcavation( const ioHashString &szName, bool bUse );
	void CheckInputKey( DWORD dwActionKey, bool bExcavating );

public:
	void SetPlayStage( ioPlayStage *pStage ) { m_pStage = pStage; }
	int GetExcavationState()                 { return m_iExcavationState; }
	float GetSight()                         { return m_fSight; }
	int   GetMaxShovelCount()                { return m_iMaxShovelCount; }
	DWORD GetPacketSendTime()				 { return m_dwPacketSendTime; }
	const D3DXVECTOR3 &GetExcavationPoint()  { return m_ExcavationPoint; }

	const ioHashString &GetExcavatingEmoticon() { return m_szEmoticonExcavating; }
	const ioHashString &GetFailEmoticon()       { return m_szEmoticonFail; }

	float GetChargingRate();
	DWORD GetCooltime();
	float GetSpeedClass( const ioHashString &szName );

	void GetResultIconName( int iType, int iItemIndex, ioHashString &szIconName );
	void GetResultItemName( LSC_Excavation_info *pInfo, ioHashString &szItemName );
	void GetResultItemValue( LSC_Excavation_info *pInfo, ioHashString &szItemName, int iCount );

	int GetArtifactCount();
	LSC_Excavation_info *GetArtifactInfoByArray( int iArray );
	LSC_Excavation_info *GetArtifactInfo( int iKey );
	LSC_Excavation_grade *GetGradeInfo( int iKey );

	void SetRemainChargingTime( DWORD dwNextCharginfTime );
	DWORD GetRemainChargeTime( bool bRemain = true );

	bool IsSendExcavationPacket();

public:
	bool IsExcavating();
	//bool HasEtcItem();

public:
	bool OnExcavationCommand( SP2Packet &rkPacket );
	void SendEquipKit();
	void SendReleaseKit();
	void SendStartDigging();
	void SendIdentify();
	void SendIdentifyComplete();
	void SendTimeRechargeShovel(); // 자동 충전
	void SendRechargeShovel();        // 아이템 소모하여 충전

	float GetLimitAngle() { return m_fLimitAngle; }
	float GetHeightOffset() { return m_fHeightOffset; }
	float GetDistanceHeightOffset() { return m_fDistanceHeightOffset; }
	const ioHashString &GetGuideEffectName() { return m_szGuideEffectName; }
	const ioHashString &GetPointEffectName() { return m_szPointEffectName; }

	void SetExcavatingSuccessAlarm( ioHashString &szSendID, int iRewardType, int iIndex, int iType, int iGrade, int iPrice, int iMultiple, bool bAllAlarm );

	void OnExcavationBtnUp( ioBaseChar *pOwner ); 

	void OnExcavationUDPPacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	void SetShovelCount( int iCnt );
	int  GetShovelCount() { return m_iShovelCount; }
	void UpdateShovelCount();
	ioUIRenderImage *GetRewardIcon( int iIndex );
	int GetManualIndex() { return m_iManualIndex; }
	int GetEffectGrade() { return m_iEffectGrade; }

public:
	UserExcavationLvInfo *FindUser( const ioHashString &szName );
	void CheckUserExcavationLv( const ioHashString &szName, int iLevel );
	void EraseUserExcavationLv( const ioHashString &szName );
	void ClearExcavationUserInfo();
	bool EnableShowResult();
	int  GetUserExcavationLv( const ioHashString &szName );
	bool GetUseIconName( ioHashString &szIconName );
	bool GetUseIconName( int iLevel, ioHashString &szIconName );
	bool IsChangeKitLevel( int iLevel );
	bool IsForkcraneLevel( int iLevel );

	void NoticeLevelAndExp();

private:
	void ChangeUseIcon();

public:
	void CheckExceptionError( int iCommand, int iResult, bool bChangeDelayState = false );

public:
	ioExcavationManager(void);
	virtual ~ioExcavationManager(void);
};

#define g_ExcavationMgr ioExcavationManager::GetSingleton()

