#pragma once

class ioWnd;
class ioUIRenderImage;
class StringScrollListWnd;

struct QuickStartHelpData
{
	ioHashString m_szHelp;
	bool m_bModeTooltipHelpPass;     
	QuickStartHelpData()
	{
		m_bModeTooltipHelpPass = false;
	}
};
typedef std::vector< QuickStartHelpData > vQuickStartHelpData;

struct QuickStartHelp
{
	ioHashString m_szState;
	vQuickStartHelpData m_vHelpList;
};

class ioModeSelectInfo : public Singleton< ioModeSelectInfo >
{
protected:
	// 생성 UI
	// Mode / Map Image
	typedef std::map< ioHashString, ioUIRenderImage* > ModeAndMapUIResource;
	ModeAndMapUIResource m_ModeUIResource;
	ModeAndMapUIResource m_MapUIResource;

	// 리스트 UI
	ModeAndMapUIResource m_ListModeUIResource;
	ModeAndMapUIResource m_ListMapUIResource;

	// 순서 마크
	typedef std::map< DWORD, ioUIRenderImage* > OrderMarkUIResource;
	OrderMarkUIResource m_OrderMarkResource;

	// 모드 아이콘
	struct ModeIcon
	{
		ModeType m_eModeType;
		bool     m_bUserMode;

		ioUIRenderImage *m_pDefaultIcon;
		ioUIRenderImage *m_pBlueTeamIcon;
		ioUIRenderImage *m_pRedTeamIcon;
		ModeIcon()
		{
			m_eModeType = MT_NONE;
			m_bUserMode = false;

			m_pDefaultIcon = m_pBlueTeamIcon = m_pRedTeamIcon = NULL;
		}
	};
	typedef std::vector< ModeIcon* > vModeIcon;
	vModeIcon m_ModeIconList;

	
	// Quick Start List Type	
	typedef std::map< DWORD, QuickStartHelp > QuickStartHelpMap;
	QuickStartHelpMap m_QuickStartHelpMap;

	struct QuickStartList
	{
		int m_iIndex;
		ioHashString m_Title;
		ioHashString m_SubTitle;
		int  m_iHelpIndex;
		int  m_iManualIndex;
		int  m_iLimitGrade;
		int  m_iUseCoinCnt;
		int  m_iLimitMaxUser;
		int  m_iDefMinUser;
		int  m_iDefMaxUser;
		bool m_bUseSameUser;
		bool m_bDefSameUser;
		bool m_bAddCreateUI;
		int  m_iCreateModeOrder;
		int  m_iCreateMarkRes;
		int	 m_nQuickNewHot;
		QuickStartList()
		{
			m_iIndex = m_iLimitGrade = m_iUseCoinCnt = m_iHelpIndex = m_iManualIndex = 0;			
			m_iLimitMaxUser = m_iDefMinUser = m_iDefMaxUser = 0;
			m_bUseSameUser = m_bDefSameUser = false;
			m_bAddCreateUI = false;
			m_iCreateModeOrder = m_iCreateMarkRes = m_nQuickNewHot = 0;
		}
	};
	typedef std::vector< QuickStartList > vQuickStartList;
	vQuickStartList m_QuickStartList;
	// 전투방 생성 UI & 빠른 참가 리스트 정렬
	class QuickBattleInfoSort : public std::binary_function< const QuickStartList&, const QuickStartList&, bool >
	{
	public:
		bool operator()( const QuickStartList &lhs , const QuickStartList &rhs ) const
		{
			if( lhs.m_iCreateModeOrder > rhs.m_iCreateModeOrder )
			{
				return true;
			}
			return false;
		}
	};

	// Battle Room List Title
	struct BattleRoomListTitle
	{
		int m_iIndex;
		ioHashString m_Title;
		BattleRoomListTitle()
		{
			m_iIndex = 0;			
		}
	};
	typedef std::vector< BattleRoomListTitle > vBattleRoomListTitle;
	vBattleRoomListTitle m_BattleRoomListTitle;

	ioHashString m_szCustomModeTitle;
	
	// Map Info
	struct MapInfo
	{
		int  m_MapIndex;
		ioHashString m_MapTitle;
		int  m_GradeLimitCheck;
		int  m_LimitMaxPlayer;
		int  m_DefaultMaxPlayer;
		int  m_iUseCoinCnt;
		int  m_iMapOrder;
		int  m_iMarkResBig;
		int  m_iMarkResSmall;
		bool m_bStartSetLimitPlayer;
	};
	typedef std::vector< MapInfo > vMapInfo;
	// 맵 리스트 정렬
	class MapInfoSort : public std::binary_function< const MapInfo&, const MapInfo&, bool >
	{
	public:
		bool operator()( const MapInfo &lhs , const MapInfo &rhs ) const
		{
			if( lhs.m_iMapOrder > rhs.m_iMapOrder )
			{
				return true;
			}
			return false;
		}
	};
	struct MapList
	{
		vMapInfo m_MapInfo;
	};
	typedef std::vector< MapList > vMapList;
	vMapList m_MapList;

	// Plaza Mode Info
	struct PlazaModeList
	{
		int m_ModeIndex;
		ioHashString m_ModeTitle;
		int m_PlazaType;
		int m_MapListIndex;
	};
	typedef std::vector< PlazaModeList > vPlazaModeList;
	vPlazaModeList m_PlazaModeList;

	// Headquarters Mode Info
	struct HeadquartersModeList
	{
		int m_ModeIndex;
		ioHashString m_ModeTitle;
		int m_MapListIndex;
	};
	typedef std::vector< HeadquartersModeList > vHeadquartersModeList;
	vHeadquartersModeList m_HeadquartersModeList;

	// Headquarters Mode Info
	struct HouseModeList
	{
		int m_ModeIndex;
		ioHashString m_ModeTitle;
		int m_MapListIndex;
	};
	typedef std::vector< HouseModeList > vHouseModeList;
	vHouseModeList m_HouseModeList;

	// Battle Mode Info
	struct BattleModeList
	{
		int m_ModeIndex;
		ioHashString m_ModeTitle;
		int  m_ModeHelpIndex;
		int  m_ModeManualIndex;
		bool m_bSafetyMode;
		bool m_bFreeForAll;
		int  m_StartMinUser;
		bool m_bStartEnterX;
		bool m_bCoinVisible;
		int  m_MapListIndex;
		int  m_iModeOrder;
		int  m_iMarkResBig;
		int  m_iMarkResSmall;
		bool m_bAIMode;
		bool m_bAutoStart;
	};
	typedef std::vector< BattleModeList > vBattleModeList;
	// 전투방 모드 리스트 정렬
	class BattleModeInfoSort : public std::binary_function< const BattleModeList&, const BattleModeList&, bool >
	{
	public:
		bool operator()( const BattleModeList &lhs , const BattleModeList &rhs ) const
		{
			if( lhs.m_iModeOrder > rhs.m_iModeOrder )
			{
				return true;
			}
			return false;
		}
	};
	vBattleModeList m_BattleModeList;
	vBattleModeList m_BroadcastBattleModeList;

	// Ladder Team Mode Info
	struct LadderTeamModeList
	{
		int  m_ModeIndex;
		ioHashString m_ModeTitle;
		int  m_ModeHelpIndex;
		int  m_ModeManualIndex;
		int  m_MapListIndex;
		bool m_bHeroMatch;
		int  m_iMinimumPlayer;
		int  m_iDefaultPlayer;
		int  m_iModeOrder;
		int  m_iMarkResBig;
		int  m_iMarkResSmall;
	};
	typedef std::vector< LadderTeamModeList > vLadderTeamModeList;
	// 진영팀 모드 리스트 정렬
	class LadderTeamModeInfoSort : public std::binary_function< const LadderTeamModeList&, const LadderTeamModeList&, bool >
	{
	public:
		bool operator()( const LadderTeamModeList &lhs , const LadderTeamModeList &rhs ) const
		{
			if( lhs.m_iModeOrder > rhs.m_iModeOrder )
			{
				return true;
			}
			return false;
		}
	};
	vLadderTeamModeList m_LadderTeamModeList;

	//모드 타입으로 모드인덱스를 얻을 수 없어서 세팅으로 매칭되도록 하는 구조를 만듬
	struct ModeTypeInfo
	{
		int iModeIdex;
		int eModeType;

		ModeTypeInfo()
		{
			iModeIdex = -1;
			eModeType = MT_NONE;
		}
	};
	typedef std::vector<ModeTypeInfo> ModeTypeInfoList;
	ModeTypeInfoList m_ModeTypeInfoList;

public:
	void LoadModeSelectInfo();	
	void LoadQuickStartHelp();
	void ClearAllResource();

public: //리소스
	ioUIRenderImage *GetModeUIResource( const ioHashString &szTitle );
	ioUIRenderImage *GetMapUIResource( const ioHashString &szTitle );
	ioUIRenderImage *GetListModeUIResource( const ioHashString &szTitle );
	ioUIRenderImage *GetListMapUIResource( const ioHashString &szTitle );
	ioUIRenderImage *GetOrderMarkUIResource( DWORD dwMarkNum );

public: //빠른 시작
	int GetQuickStartModeIndexCheck( int iModeIndex );
	ioHashString GetSelectQuickStartModeTitle( int iModeIndex );
	ioHashString GetSelectQuickStartModeSubTitle( int iModeIndex );
	int  GetSelectQuickStartModeLimitGrade( int iModeIndex );
	int  GetSelectQuickStartModeUseCoinCnt( int iModeIndex );
	void GetQuickModeList( ioWnd *pParseWnd );
	int  GetQuickStartIndexToHelpIndex( int iModeIndex );
	int  GetQuickStartIndexToManualIndex( int iModeIndex );
	ioHashString GetQuickStartHelpState( int iHelpIndex );
	ioHashString GetQuickStartHelpMent( int iHelpIndex, int iLine );
	QuickStartHelp &GetQuickStartHelpData( int iHelpIndex );
	int  GetQuickStartMaxHelpMent( int iHelpIndex );
	int  GetQuickStartMaxPassHelpMent( int iHelpIndex );
	int  GetSelectQuickStartModeDefMaxPlayer( int iModeIndex );
	int  GetSelectQuickStartModeDefMinPlayer( int iModeIndex );
	bool GetSelectQuickStartModeDefSameUser( int iModeIndex );
	bool IsNoneQuickStartMode( int iModeIndex );
	bool IsQuickStartFreeForAllMode( int iModeIndex );

public: //광장
	int CheckPlazaModeIndex( int iPlazaType );
	int CheckPlazaMapIndex( int iPlazaType, int iMapIndex );
	int GetPlazaMapIndex( int iPlazaType );
	ioHashString GetPlazaModeTitle( int iPlazaType );
	ioHashString GetPlazaMapTitle( int iPlazaType, int iMapIndex );
	void GetPlazaModeUIList( ioWnd *pParseWnd );
	void GetPlazaMapUIList( ioWnd *pParseWnd, int iPlazaType );

public: //훈련소
	int GetHeadquartersModeIndex();	
	int GetHeadquartersMapIndex( int iModeIndex );	
	int CheckHeadquartersMapIndex( int iModeIndex, int iMapIndex );

public: //개인본부
	int GetHouseModeIndex();	
	int GetHouseMapIndex( int iModeIndex );	
	int CheckHouseMapIndex( int iModeIndex, int iMapIndex );

public: //전투방
	void GetBattleCreateModeList( ioWnd *pParseWnd );
	int CheckBattleModeIndex( int iModeIndex );
	int CheckBattleMapIndex( int iModeIndex, int iMapIndex );
	int CheckBattleFirstMapIndex( int iModeIndex );
	int GetBattleMapIndex( int iModeIndex );	
	int GetBattleModeHelpIndex( int iModeIndex );
	int GetBattleModeManualIndex( int iModeIndex );
	ioHashString GetBattleModeTitle( int iModeIndex );
	ioHashString GetBattleMapTitle( int iModeIndex, int iMapIndex );
	ioUIRenderImage *GetBattleModeOrderMarkSmall( int iModeIndex );
	ioUIRenderImage *GetBattleMapOrderMarkSmall( int iModeIndex, int iMapIndex );
	int GetBattleMapLimitGrade( int iModeIndex, int iMapIndex );
	int GetBattleMapLimitMaxPlayer( int iModeIndex, int iMapIndex );
	bool IsBattleStartSetMapLimitMaxPlayer( int iModeIndex, int iMapIndex );
	int GetBattleMapDefaultMaxPlayer( int iModeIndex, int iMapIndex );
	int GetBattleMapCoinUseCnt( int iModeIndex, int iMapIndex );	
	void GetBattleModeUIList( ioWnd *pParseWnd );
	void GetBattleMapUIList( ioWnd *pParseWnd, int iModeIndex );	
	void GetBattleAIUIList( ioWnd *pParseWnd );	
	int GetBattleModeIndex( int eModeType );
	int GetBattleMode( int nIndex );
	void GetBattleModeStringList( StringScrollListWnd *pParseWnd, int iModeIndex, DWORD WndID );

	bool IsFreeForAllMode( int iModeIndex );
	bool IsStartEnterX( int iModeIndex );
	bool IsCoinVisible( int iModeIndex );
	int  GetBattleStartMinUser( int iModeIndex );
	ioHashString GetBattleRoomListTitle( int iModeIndex );

	inline const ioHashString& GetCustomModeTitle() const { return m_szCustomModeTitle; }

public: //방송용 전투방
	void GetBroadcastBattleModeUIList( ioWnd *pParseWnd );

public: //진영팀
	void GetLadderCreateModeList( ioWnd *pParseWnd );
	int  CheckLadderTeamModeIndex( int iModeIndex );
	int  CheckLadderTeamMapIndex( int iModeIndex, int iMapIndex );
	int  GetLadderTeamMapIndex( int iModeIndex );
	int  GetLadderTeamModeHelpIndex( int iModeIndex );
	int  GetLadderTeamModeManualIndex( int iModeIndex );
	int  GetLadderTeamMinimumPlayer( int iModeIndex );
	int  GetLadderTeamDefaultPlayer( int iModeIndex );
	int  GetLadderTeamHeroMatchIndex();
	bool IsLadderTeamHeroMatchMode( int iModeIndex );
	ioHashString GetLadderTeamModeTitle( int iModeIndex );
	ioHashString GetLadderTeamMapTitle( int iModeIndex, int iMapIndex );
	ioUIRenderImage *GetLadderTeamModeOrderMarkSmall( int iModeIndex );
	ioUIRenderImage *GetLadderTeamMapOrderMarkSmall( int iModeIndex, int iMapIndex );
	void GetLadderTeamModeUIList( ioWnd *pParseWnd, bool bHeroMatch );
	void GetLadderTeamMapUIList( ioWnd *pParseWnd, int iModeIndex );

public: // 모드 아이콘 : 중간현황/중간결과/최종결과에서 사용
	ioUIRenderImage *GetModeIcon( ModeType eModeType, TeamType eTeamType, bool bUserMode );
	bool IsAIMode( int iModeIndex );
	bool IsAutoStart( int iModeIndex );

public:
	static ioModeSelectInfo& GetSingleton();

public:
	ioModeSelectInfo();
	virtual ~ioModeSelectInfo();
};

#define g_ModeSelectInfo ioModeSelectInfo::GetSingleton()

