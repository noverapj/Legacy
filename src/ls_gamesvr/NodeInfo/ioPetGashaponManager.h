#ifndef _ioPetGashaponManager_h_
#define _ioPetGashaponManager_h_

#include "../Util/Singleton.h"
#include "../Util/IORandom.h"

class User;

class ioPetGashaponManager : public Singleton< ioPetGashaponManager >
{
public:
	enum PETRANK
	{
		RANKD = 1,
		RANKC = 2,
		RANKB = 3,
		RANKA = 4,
		RANKS = 5
	};

	struct PetRandomInfo
	{
		int iPresentType;
		int iPetCode;
		int iPetRandom;
		int iPetStartRank;

		PetRandomInfo()
		{
			iPresentType = 0;
			iPetCode = 0;
			iPetRandom =0;
		}
	};

	struct RankVectorTag
	{
		int iStartIndex;
		int iEndIndex;

		RankVectorTag()
		{
			iStartIndex = 0;
			iEndIndex = 0;
		}
	};

	struct BuyLimitPetRank
	{
		int iStartRank;
		int iEndRank;

		BuyLimitPetRank()
		{
			iStartRank = 0;
			iEndRank = 0;
		}
	};
		
	struct DiffPetCompoundRate
	{
		int iSuccessRate;
		int iFailRate;

		DiffPetCompoundRate()
		{
			iSuccessRate = 0;
			iFailRate = 0;
		}
	};

protected:
	std::vector< int > m_vPetRankRandom; //나올 등급랭크의 랜덤값 저장
	std::vector< PetRandomInfo > m_vAllRankPetRandomInfo;
	std::vector< RankVectorTag > m_vGradedTag; //m_vAllRankPetRandomInfo의 등급별 시작, 끝 index저장 벡터

	std::vector< int > m_vRandomTotalInRank;

	IORandom m_PetRankRandom;
	IORandom m_PetSearchRandom;

	BuyLimitPetRank m_rkPesoBuyLimitRank;
	BuyLimitPetRank m_rkGoldBuyLimitRank;

protected:
	void ClearAll();

public:
	void CheckNeedReload();
	void LoadINI();

public:
	bool PickBuyRandomPet( ioUserPet::PETSLOT& rkPetInfo, bool bCash );

	int GetRandomPetRank( bool bCash );
	void GetRandomTotalRateInRank( const int& iRank, int &iTotalRate );

	int GetRankWithRandom( const int iRandom );
	int GetRankWithRandom( const int iRandom, const int iStartRank, const int iEndRrank );

	int GetRankRandomTotal( const int iStartRank, const int iEndRank );

	bool GetPetInRank( const int& iRank, ioUserPet::PETSLOT& rkPetInfo );
	int GetPetCodeWithRandom(  const int iRank, const int iRandom );

	void SetGradeTag( const int iStartIndex, const int iEndIndex );

	int GetPetCodeInRank(  const int& iRank );

	bool IsRightPetRank( const int iRank, const int iPetCode );

private:
	//펫 교배 관련 변수
	DiffPetCompoundRate m_rkDiffPetCompoundRate;
	IORandom m_PetCompoundRandom;

public:
	bool IsMaxLevel( ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet );
	int PetCompound( User *pUser, ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet );  

	bool FinalRankCompoundResult( User *pUser, ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet );
	bool DiffPetCompoundResult( User *pUser, ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet );
	bool SamePetCompoundResult( User *pUser, ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet );

	bool CheckCompoundSuccess( );

public:
	static ioPetGashaponManager& GetSingleton();

public:
	ioPetGashaponManager();
	virtual ~ioPetGashaponManager();

};

#define g_PetGashaponMgr ioPetGashaponManager::GetSingleton()

#endif