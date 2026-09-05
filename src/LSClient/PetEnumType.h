#pragma once

enum PetRankType
{
	PRT_NONE = 0,
	PRT_D,
	PRT_C,
	PRT_B,
	PRT_A,
	PRT_S,
	PRT_MAX,
};

enum PetMainType
{
	PMT_NONE = 0,
	PMT_DEFAULT,
	PMT_MAX,
};

enum DefaultPetSubType
{
	DPST_NONE = 0,
	DPST_DEFAULT,
	DPST_MAX,
};

//펫 랭크 별 정보
typedef struct tagPetRankInfo
{
	//랭크 타입
	PetRankType m_RankType;
	//최대 레벨
	int m_nMaxLevel;
	//랭크 상수값
	float m_fRankValue;

	tagPetRankInfo()
	{
		m_RankType = PRT_NONE;
		m_nMaxLevel = 0;
		m_fRankValue = 0.0f;
	}
}PetRankInfo;

//펫 랭크 정보 리스트
typedef std::vector<PetRankInfo> PetRankInfoList;

//스탯 상승 정보
typedef struct tagPetAddStatInfo
{
	//몇 레벨당
	int m_nPerLevel;
	//몇 스탯
	int m_nAddStat;

	tagPetAddStatInfo()
	{
		//현재 레벨 * 1/m_nPerLevel 
		m_nPerLevel = 1;
		m_nAddStat = 0;
	}
}PetAddStatInfo;

//스탯 상승 정보 리스트
typedef std::map<int, PetAddStatInfo> PetAddStatInfoList;

//펫 타입 정보
typedef struct tagPetTypeInfo
{
	//펫 메인 타입
	PetMainType m_PetMainType;
	//서브 타입
	int m_nPetSubType;

	tagPetTypeInfo()
	{
		m_PetMainType = PMT_NONE;
		m_nPetSubType = 0;
	}
}PetTypeInfo;

typedef struct tagPetViewInfo
{
	PetRankType m_PetRank;
	ioHashString m_szPetName;
	ioHashString m_szPetIcon;
	ioHashString m_szPetModel;
	ioHashString m_szPetAni;
	D3DXVECTOR3 m_vScale;

	tagPetViewInfo()
	{
		m_PetRank = PRT_NONE;
		m_szPetName.Clear();
		m_szPetIcon.Clear();
		m_szPetModel.Clear();
		m_szPetAni.Clear();
	}
}PetViewInfo;

typedef std::vector<PetViewInfo> PetViewInfoList;

//펫 스탯 기본 정보
typedef struct tagPetStatBaseInfo
{
	//기본 스탯
	IntVec m_vBaseGrowthStat;
	//추가 스탯 타입 리스트
	IntVec m_vAddStatTypeList;
	//펫 아이콘
	PetViewInfoList m_PetViewInfoList;
	//펫 설명
	int m_nManualCode;
	//펫 추천조각
	int m_nNeedMaterial;
	
	tagPetStatBaseInfo()
	{
		m_vBaseGrowthStat.clear();
		m_vAddStatTypeList.clear();
		m_PetViewInfoList.clear();
		m_nManualCode = 0;
		m_nNeedMaterial = 0;
	}
}PetBaseStatInfo;

//펫 스탯 기본 정보 리스트
typedef std::map< int, PetBaseStatInfo > PetBaseStatInfoList;

//현재 착용 펫 정보
typedef struct tagCurPetInfo
{	
	//펫 코드->코드로 기본정보를 찾음
	int m_nPetCode;
	//현재 랭크->모델 정보를 불러옴
	PetRankType m_CurRankType;
	//현재 레벨->스탯 적용
	int m_nCurLevel;

	tagCurPetInfo()
	{
		m_nPetCode = 0;
		m_CurRankType = PRT_NONE;
		m_nCurLevel	= 0;
	}
}CurPetInfo;

typedef std::map<ioHashString, CurPetInfo> CurPetInfoList;

//펫 모델 정보
typedef struct tagPetModelInfo
{
	//랭크 타입
	PetRankType m_PetRankType;
	//모델 파일명
	ioHashString m_PetModel;
	//스케일 값
	D3DXVECTOR3 m_vScale;

	tagPetModelInfo()
	{
		m_PetRankType = PRT_NONE;
		m_vScale = ioMath::VEC3_ZERO;
	}
}PetModelInfo;

//펫 모델 정보 리스트
typedef std::vector<PetModelInfo> PetModelInfoList;

//펫 기본엔티티 정보
typedef struct tagPetBaseInfo
{
	//파일 위치
	ioHashString m_PetPath;
	//타이틀
	ioHashString m_PetTitle;
	//펫 타입
	PetTypeInfo m_PetTypeInfo;
	//모델 정보
	PetModelInfoList m_vPetModelInfoList;

	tagPetBaseInfo()
	{
		m_PetPath.Clear();
		m_PetTitle.Clear();
		m_vPetModelInfoList.clear();
	}
}PetBaseInfo;

//펫 기본엔티티 정보 리스트
typedef std::map<int, PetBaseInfo> PetBaseInfoList;

//펫 슬롯
typedef struct tagPetSlot
{
	//펫 코드
	int m_nPetCode;
	//랭크
	int m_PetRankType;
	//인덱스
	int m_nIndex;
	//레벨, 경험치
	int m_nCurLevel;
	int m_CurExp; 
	bool m_bEquip;

	//이름, 아이콘
	ioHashString m_Name;
	ioHashString m_IconName;

	tagPetSlot()
	{
		m_nPetCode = 0;
		m_PetRankType = 0;
		m_nIndex = 0;
		m_nCurLevel = 0;
		m_CurExp = 0;
		m_bEquip = false;
		m_Name.Clear();
		m_IconName.Clear();
	}
}PetSlot;

//펫 슬롯 리스트
typedef std::vector<PetSlot> PetSlotList;

//펫 슬롯 정렬
class PetSlotSort : public std::binary_function< const PetSlot&, const PetSlot&, bool >
{
public:
	bool operator()( const PetSlot &lhs , const PetSlot &rhs ) const
	{
		if( lhs.m_PetRankType > rhs.m_PetRankType )
			return true;

		if( lhs.m_PetRankType == rhs.m_PetRankType )
		{
			if( lhs.m_nPetCode < rhs.m_nPetCode )
				return true;	

			if( lhs.m_nPetCode == rhs.m_nPetCode )
			{
				if( lhs.m_nCurLevel > rhs.m_nCurLevel )
					return true;	
				return false;
			}
			return false;			
		}
		return false;
	}
};

//펫 스테이트별 애니메이션 정보
typedef struct tagPetStateAniInfo
{
	int m_nState;
	ioHashString m_szAnimation;
	float m_fAniRate;

	tagPetStateAniInfo()
	{
		m_nState = 0;
		m_szAnimation.Clear();
		m_fAniRate = FLOAT1;
	}
}PetStateAniInfo;

typedef std::vector<PetStateAniInfo> PetStateAniInfoList;

typedef std::map<int, PetStateAniInfoList> PetRankAniInfoList;

//펫 랭크별 디폴트 애니메이션 정보
typedef struct tagPetDefaultAniInfo
{
	ioHashString m_szAnimation;
	float m_fAniRate;

	tagPetDefaultAniInfo()
	{
		m_szAnimation.Clear();
		m_fAniRate = FLOAT1;
	}
}PetDefaultAniInfo;

typedef std::map<int, PetDefaultAniInfo> PetDefaultAniInfoList;

typedef struct tagPetPullDownInfo
{
	int m_nPetCode;
	int m_nPetRank;
	int m_nSlotIndex;

	tagPetPullDownInfo()
	{
		m_nPetCode = 0;
		m_nPetRank = 0;
		m_nSlotIndex = 0;
	}
}PetPullDownInfo;

typedef std::vector<PetPullDownInfo> PetPullDownInfoList;

/////////////////////////////////////////////////////////////////////////////////////////////////////
class PetPullDownInfoSort : public std::binary_function< const PetPullDownInfo&, const PetPullDownInfo&, bool >
{
public:
	bool operator() ( const PetPullDownInfo &lhs, const PetPullDownInfo &rhs ) const
	{
		if( lhs.m_nPetRank > rhs.m_nPetRank )
			return true;

		if( lhs.m_nPetRank == rhs.m_nPetRank )
		{
			if( lhs.m_nPetCode < rhs.m_nPetCode )
				return true;	
		}
		return false;
	}
};