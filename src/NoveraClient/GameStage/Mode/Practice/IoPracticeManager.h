#pragma once

#include "../../../DataHeaders/LSC_Practice.h"


class ioPracticeManager
{
public:
	enum PracticeType
	{
		E_PRACTICE_NONE = 0,
		E_PRACTICE_1 = 1, 		// 목적지 도달
		E_PRACTICE_2 = 2, 		// 포로 구출
		E_PRACTICE_3 = 3, 		// 스킬 사용
		E_PRACTICE_4 = 4, 		// 지뢰 제거
	};

	enum PracticeGrade
	{
		E_GRADE_NONE = 0,
		E_GRADE_C = 1,
		E_GRADE_B = 2,
		E_GRADE_A = 3,
	};

	typedef struct _tagPracticePresent
	{
		int iRewardType;
		int iRewardCode;
		int iRewardCount;
	}PracticePresent;

	typedef struct _tagPracticeReward
	{
		int iRewardStart;
		int iRewardEnd;
		std::vector<PracticePresent> kPresent;
		//여기에 타입추가
	}PracticeReward;

	typedef struct _tagPracticeListMyInfo
	{
		ioHashString szPracticeTime;
		int iPracticeGrade;
		int iPracticeCount;
		int iPracticeRank;
		
		_tagPracticeListMyInfo()
		{
			iPracticeRank = 0;
			iPracticeGrade = 0;
			szPracticeTime = "";
			iPracticeCount = 0;
		}
	}PracticeListMyInfo;

	typedef struct _tagRanker
	{
		ioHashString szName;
		ioHashString szTime;
		int iRank;
	}PracticeRankerInfo;

	typedef std::vector<PracticeReward> PracticeRewardVec;
	typedef struct _tagPracticePreRankingInfo
	{
		ioHashString strStartDate;
		ioHashString strEndDate;		
		int iPracticeIDX;
		int iPracticeRank;
		std::vector<PracticePresent> vecPresent; 
	}PracticePreRewardInfo;

	typedef std::map<DWORD, DWORD> PracticeRecordMap;
	
	typedef std::vector<PracticeListMyInfo> PracticeInfoVec;
	typedef std::vector<PracticeRankerInfo> RankerInfoVec;
	typedef std::vector<PracticeRankerInfo> PreRankingInfo;
	typedef std::vector<PracticePreRewardInfo> PracticePreRankingVec;	

protected:
	LSC_Practice_Manager* m_pPracticeMgr;
	PracticeRecordMap m_PracticeRecordMap;
	PracticeRewardVec m_vecPracticeReward;
	PracticeInfoVec m_vecPracticeMyInfo;
	RankerInfoVec m_vecRankerInfo;
	PracticeListMyInfo m_MyRankListInfo;
	PracticePreRankingVec m_preRankingReward;
	int m_iPrePracticeIndex; // UI 표시용으로 세팅한 인덱스
	bool m_bRefresh;

protected:
	void SetRankerInfo(SP2Packet& rkPacket);

public: // UI 표시용으로 세팅한 
	void SetPreIndex( int iPracticeIndex ) { m_iPrePracticeIndex = iPracticeIndex; }
	int GetPreIndex() { return m_iPrePracticeIndex; }

	void SetGrade( DWORD dwIndex, DWORD dwGrade );

	ioHashString GetPracticeLoadingText( int iIndex = 0 );
	DWORD GetPracticeBriefingTime( int iIndex = 0 );
	PracticeInfoVec GetPracticeInfo(){return m_vecPracticeMyInfo;}
	PracticeRankerInfo GetRankerInfo(int iIndex){return m_vecRankerInfo[iIndex];}
	PracticeListMyInfo GetMyRankInfo();//이거 없어도 될듯?
	PracticePreRankingVec GetMyPreRankInfo(){ return m_preRankingReward; }
public:
	void Init();
	void INIParsing(ioINILoader& kLoader);
	void SetServerINIList(SP2Packet& rkPacket);
	void SetServerMyInfoList(SP2Packet& rkPacket);
	void SetServerRankingList(SP2Packet& rkPacket);
	void SetModeLobbyInfo(SP2Packet& rkPacket);

	int GetPracticeCount();
	bool IsValidPractice( int iIndex );
	ioHashString GetListImage( int iIndex );
	ioHashString GetListOverImage( int iIndex );

	int GetRegularSoldierClass( int iIndex );
	BYTE GetRegularSoldierGender( int iIndex );
	int GetRegularSoldierUnderwear( int iIndex );
	int GetRegularSoldierHair( int iIndex );
	int GetRegularSoldierHairColor( int iIndex );
	int GetRegularSoldierFace( int iIndex );
	int GetRegularSoldierSkinColor( int iIndex );
	int GetPracticeMap( int iIndex );

	short GetPracticeRewardAType( int iIndex );
	short GetPracticeRewardBType( int iIndex );
	short GetPracticeRewardCType( int iIndex );

	int GetPracticeRewardValueA( int iIndex );
	int GetPracticeRewardValueB( int iIndex );
	int GetPracticeRewardValueC( int iIndex );

	//입장료
	int GetPracticeAdmission( int iIndex );
	int GetPracticeAdmissionMoney( int iIndex );
	int GetPracticeAdmissionType( int iIndex );

	const char* GetINIPath( int iIndex = 0 );
	DWORD GetLimitTime( int iIndex );
	ioHashString GetRegularSoldierName( int iIndex );

	char* GetPracticeBtnImage( int iIndex );
	ioHashString GetPracticeGradeDesc( int iIndex );
	DWORD GetPracticeGradeA( int iIndex );
	DWORD GetPracticeGradeB( int iIndex );
	DWORD GetPracticeGradeC( int iIndex );
	
	DWORD GetPracticeRewardA( int iIndex );
	DWORD GetPracticeRewardB( int iIndex );
	DWORD GetPracticeRewardC( int iIndex );

	PracticeType GetPracticeType( int iIndex );

	ioHashString GetPracticeTypeDesc( int iIndex = 0 );
	ioHashString GetPracticeTypeTitle( int iIndex = 0 );

	DWORD GetPracticeRecord( int iIndex );

	ioHashString GetPracticeGuide1( int iIndex );
	ioHashString GetPracticeGuideDesc1( int iIndex );
	ioHashString GetPracticeGuide2( int iIndex );
	ioHashString GetPracticeGuideDesc2( int iIndex );
	ioHashString GetPracticeGuide3( int iIndex );
	ioHashString GetPracticeGuideDesc3( int iIndex );

	bool IsCountDown( int iIndex );

	void SetRefreshPractice(bool bRefresh){m_bRefresh = bRefresh;}

public:
	void ApplyPracticeList( SP2Packet &rkPacket );
	void ApplyPracticeResult( SP2Packet &rkPacket );

public:
	int GetPracticeRemainCount();

public:
	static ioPracticeManager& GetInstance();
	static void ReleaseInstance();

public:
	ioPracticeManager();
	virtual ~ioPracticeManager();
};

#define g_PracticeMgr ioPracticeManager::GetInstance()

