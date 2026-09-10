
#pragma once

class ioBingo
{
public:
	enum BingoType
	{
		BT_RAND  = 0,
		BT_FIXED = 1,
	};

	enum
	{
		BINGO_LINE_LEFT_TOP,
		BINGO_LINE_HORZ1,
		BINGO_LINE_HORZ2,
		BINGO_LINE_HORZ3,
		BINGO_LINE_HORZ4,
		BINGO_LINE_HORZ5,
		BINGO_LINE_LEFT_BOTTOM,
		BINGO_LINE_VERT1,
		BINGO_LINE_VERT2,
		BINGO_LINE_VERT3,
		BINGO_LINE_VERT4,
		BINGO_LINE_VERT5,
	};

	enum
	{
		REWARD_MAX_COUNT = 12,
		BINGO_NUMBER_MAX_COUNT = 25,
	};

	enum
	{
		UI_MANUAL_START_BINGO    = 1,
		UI_MANUAL_ALL_BINGO      = 2,
		UI_MANUAL_GET_NUMBER     = 3,
		UI_MANUAL_SHUFFLE_NUMBER = 4,
		UI_MANUAL_SHUFFLE_REWARD = 5,
		UI_MANUAL_SPECIAL_NUMBER = 6,
		UI_MANUAL_FIXED_BINGO    = 7,
	};

	struct DummyInfo
	{
		ioUIRenderImage *m_pUnCheckIconImg;
		ioUIRenderImage *m_pCheckIconImg;
		ioUIRenderImage *m_pDummyIcon;
		ioUIRenderImage *m_pSubIcon;

		int m_iDummyItemCode;
		int m_iManual;

		DummyInfo()
		{
			m_pUnCheckIconImg = NULL;
			m_pCheckIconImg   = NULL;
			m_pDummyIcon	  = NULL;
			m_pSubIcon = NULL;

			m_iDummyItemCode  = 0;
			m_iManual		  = 0;
		}
	};
	typedef std::map<int, DummyInfo> DummyInfoMap;

	struct BingoInfo
	{
		int m_iIndex;
		bool m_bClear;
		ioHashString m_szIconName;
		int m_iDummyItemCode;

		BingoInfo()
		{
			m_iIndex         = 0;
			m_bClear         = false;
			m_iDummyItemCode = 0;
		}
	};
	typedef std::vector<BingoInfo> BingoInfoList;

	struct RewardInfo
	{
		int m_iIndex;
		ioHashString m_Name;
		ioHashString m_IconName;
		bool m_bHidden;
		bool m_bClear;

		int m_iType;
		int m_iValue1;
		int m_iValue2;

		int m_iManualIndex;

		RewardInfo()
		{
			m_iIndex  = 0;
			m_Name.Clear();
			m_IconName.Clear();
			m_bHidden = false;
			m_bClear = false;

			m_iType   = 0;
			m_iValue1 = 0;
			m_iValue2 = 0;

			m_iManualIndex = 0;
		}
		RewardInfo( int idx, const ioHashString &szRewardName, const ioHashString &szIconName, bool bHidden, bool bClear, int iType, int iValue1, int iValue2, int iManualIndex )
		{
			m_iIndex   = idx;
			m_Name     = szRewardName;
			m_IconName = szIconName;
			m_bHidden  = bHidden;
			m_bClear   = bClear;

			m_iType   = iType;
			m_iValue1 = iValue1;
			m_iValue2 = iValue2;

			m_iManualIndex = iManualIndex;
		}
		bool IsHidden()
		{
			return m_bHidden;
		}
	};
	typedef std::vector<RewardInfo> RewardInfoList;

	struct ColorText
	{
		int  m_iSpaceLine;
		DWORD m_dwColor;
		ioHashString m_szText;

		ColorText()
		{
			m_iSpaceLine = 0;
			m_dwColor = 0;
		}

		void Clear()
		{
			m_iSpaceLine = 0;
			m_dwColor = 0;

			m_szText.Clear();
		}
	};
	typedef std::vector< ColorText > vColorText;
	typedef std::vector< vColorText > vLineText;
	typedef std::vector< vLineText > vManualList;

protected:
	int m_iBingoType;

	int m_iMaxNumber;
	int m_iRewardCount;
	int m_iAllBingoRewardCount;

	bool m_aBingoLine[12];

	BingoInfoList  m_vNumberList;		// 12개의 숫자 리스트.
	RewardInfoList m_vBingoRewardList;	// 12개의 보상아이템 리스트

	IntVec         m_vBingoNumber;		// 빙고가 된 숫자들.
	int            m_aBingoNumber[5][5];

	RewardInfoList m_vRewardTable;			// 셋팅상의 모든 보상 아이템 리스트
	RewardInfoList m_vAllBingoRewardTable;	// 셋팅상의 모든 올빙고 보상 아이템 리스트
	int            m_iAllBingoRewardIndex;

	DummyInfoMap   m_DummyInfoMap;

	typedef std::map< int, ioUIRenderImage* > RewardIconMap;
	RewardIconMap m_RewardIconMap;			// 보상아이콘 리스트
	RewardIconMap m_AllBingoRewardIconMap;	// 올빙고 보상아이콘 리스트

	ioHashString m_HiddenIconName;

	bool m_bSettedData;

	vManualList m_vManualList;
	vManualList m_vUIManualList;
	
public:
	void Initialize();
	void CreateIconMap();
	void ClearIconMap();
	void ClearDummyInfo();

	void CheckBingoNumber( OUT int &rkiNumber, OUT bool &rkbClear );
	void CheckReward( OUT int &rkiIndex, OUT bool &rkbClear );

public:
	void LoadINI();
	void LoadCommon( ioINILoader_e& rkLoader );
	void LoadDummyInfo( ioINILoader_e& rkLoader );
	void LoadToolTipManual();
	void LoadUIManual();

	void ApplyAllData( SP2Packet &rkPacket, bool bOnlyNumberRoll );			// 로그인, 시작, 새로 시작
	void ApplyGetNumber( SP2Packet &rkPacket );			// 빙고 숫자뽑기, 당첨숫자뽑기
	void ApplyGetAllNumber( SP2Packet &rkPacket );		// 숫자초기화, 숫자섞기
	void ApplyShuffleReward( SP2Packet &rkPacket );		// 보상바꾸기
	
	void SendAllInit();
	void SendNumberInit();

	void GetAllRewardTable( OUT RewardInfoList &rkRewardTable );

	void GetBingoInfo( OUT BingoInfoList &rkNumberInfo );
	void GetRewardList( OUT RewardInfoList &rkRewardList );
	const ioHashString &GetRewardName( int iRewardIndex );

	RewardInfo GetRewardInfo( int iIndex );
	const RewardInfo GetAllBingoRewardInfo();
	const RewardInfo& GetAllBingoRewardInfo( int index );

	int GetAllBingoRewardIndex() { return m_iAllBingoRewardIndex; }
	const ioHashString &GetAllBingoRewardName( int iRewardIndex );

	const ioHashString &GetHiddenIconName() { return m_HiddenIconName; }

	ioUIRenderImage *GetRewardIcon( int iIndex );
	ioUIRenderImage *GetAllBingoRewardIcon( int iIndex = 0 );

	void CheckBingo( bool bEffect );
	bool CheckAllBingo();

	bool CheckBingoLine( int iNumber );
	void SetBingoLine( int iLineIndex );

	bool IsNumberClear( int iIndex );
	bool IsNumberBingo( int iIndex );
	bool IsRewardHidden( int iIndex );
	bool IsRewardClear( int iIndex );

	bool IsSettedData() { return m_bSettedData; }

	int GetMaxNumber() { return m_iMaxNumber; }
	int GetRewardCount() { return m_iRewardCount; }
	int GetAllBingoRewardListCount() { return m_iAllBingoRewardCount; }
	int GetBingoType(){ return m_iBingoType; }
	void SetBingoType( int iType ){ m_iBingoType = iType; }

public:
	int GetToolTipMaxLine( int iIndex );
	int GetToolTipGetSpaceLine( int iIndex, int iCurLine );
	void SetPrinter( int iIndex, int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter );
	void SetUIPrinter( int iType, int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter );
	void ParseString( char *szText );

public:
	ioUIRenderImage* GetBingoDummyCheckIcon( int iNumber );
	ioUIRenderImage* GetBingoDummyUnCheckIcon( int iNumber );
	ioUIRenderImage* GetBingoDummyIcon( int iNumber );
	ioUIRenderImage* GetBingoSubIcon( int iNumber );

	const ioHashString& GetBingoDummyName( int iNumber );
	int GetBingoDummyCode( int iNumber );
	int GetBingoDummyManualIndex( int iNumber );
	int GetBingoDummyNumberOfCurrent();
	bool HasBingoDummy( int iNumber );

public:
	ioBingo();
	virtual ~ioBingo();
};