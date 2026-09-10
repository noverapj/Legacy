#pragma once

class ioPetInfoManager : public Singleton< ioPetInfoManager >
{
protected:
	//랭크 정보 관련 (최대 레벨, 랭크 상수)
	PetRankInfoList			m_PetRankInfoList;
	//스탯 상승 관련
	PetAddStatInfoList		m_PetAddStatInfoList;
	//펫 기본 정보 관련
	PetBaseStatInfoList		m_PetBaseInfoList;

	//최대 경험치 상수
	float m_fMaxExpConst;
	//되팔기 페소값
	int m_nResellPeso;

	int m_iPetEatAdditiveCount;
	int m_iPetEatAdditiveExp;

public:
	static ioPetInfoManager& GetSingleton();

public:
	//데이터 비우기
	void ClearData();
	//데이터 읽기
	void LoadProperty();

protected:
	////랭크 정보
	void LoadPetRankInfo( ioINILoader &rkLoader );
	//스탯 정보
	void LoadPetAddStatInfo( ioINILoader &rkLoader );
	//개별 정보 읽기
	void LoadPetBaseInfo( ioINILoader &rkLoader );
	void LoadViewList( ioINILoader &rkLoader, PetViewInfoList& vList );
	void LoadBaseGrowthList( ioINILoader &rkLoader, IntVec& vList );
	void LoadAddStatList( ioINILoader &rkLoader, IntVec& vList );

public:
	//최대 경험치 얻기
	int GetMaxExp( int nCurLevel, PetRankType eRankType );
	//최대 레벨 얻기
	int GetMaxLevel( PetRankType eRankType );
	//기본 스탯 얻기
	int GetBaseGrowthStat( int nPetCode, int nIndex );
	//추가 스탯 얻기
	int GetAddStat( int nPetCode, int nIndex, int nCurLevel );
	//총 스탯 얻기
	int GetTotalStat( int nPetCode, int nIndex, int nCurLevel );
	//펫 설명 얻기
	int GetPetManual( int nPetCode );
	//펫 추천조각 얻기
	int GetPetNeedMaterial( int nPetCode );
	//펫 이름 얻기(UI표시)
	ioHashString GetPetName( int nPetCode, PetRankType eRankType );
	//펫 아이콘 얻기(UI표시)
	ioHashString GetPetIcon( int nPetCode, PetRankType eRankType );
	//펫 모델 얻기(UI표시)
	ioHashString GetPetModel( int nPetCode, PetRankType eRankType );
	//펫 모델 크기 얻기(UI표시)
	D3DXVECTOR3 GetPetModelScale( int nPetCode, PetRankType eRankType );
	//펫 뷰 애니 얻기(UI표시)
	ioHashString GetPetViewAni( int nPetCode, PetRankType eRankType );
	//펫 랭크 문자얻기
	ioHashString GetRankName( int nRank );
	//페소 값얻기
	int GetResellPeso();
	int GetPetEatAdditiveCount();

public:
	ioPetInfoManager();
	virtual ~ioPetInfoManager();
};

#define g_PetInfoMgr ioPetInfoManager::GetSingleton()