#ifndef __ioPetInfoManager_h__
#define __ioPetInfoManager_h__

#include "../Util/Singleton.h"
#include "ioUserPet.h"

class User;
class ioPetInfoManager : public Singleton< ioPetInfoManager >
{
public:
	struct PetInfo
	{
		int iPetCode;
		//int iMaxRank;

		PetInfo()
		{
			iPetCode = 0;
			//iMaxRank = 0;
		}
	};

	struct PetRankInfo
	{
		int iRank;
		float iRankConst;
		int iRankMaxLevel;

		PetRankInfo()
		{
			iRank = 0;
			iRankConst = 0.0f;
			iRankMaxLevel = 0;
		}
	};

protected:
	void Clear();

protected:
	typedef std::vector< PetInfo > vPetInfoVec;
	typedef std::vector< PetRankInfo > vPetRankInfoVec;

protected:
	vPetInfoVec m_vPetInfoVec;
	vPetRankInfoVec m_vPetRankInfoVec;

protected:
	float m_fMaxExpConst; //최대 경험치 계산 상수
	int m_iPetSellPeso;	//펫 판메시 획득 페소
	int m_iMaxPetCount;	//최대 소지 가능한 펫 숫자
	int m_iMaxRank;

	int m_iPetEatAdditiveCount;
	int m_iPetEatAdditiveExp;

public:
	static ioPetInfoManager& GetSingleton();

public:
	void LoadINI();
	void CheckNeedReload();

	int GetSellPeso( );
	bool SetMaxExp( ioUserPet::PETSLOT& rkPetSlot );

	int GetMaxExp( int iCurLevel, int iPetRank );
	float GetRankConst( const int& iPetRank );

	int GetAddExp( const ioUserPet::PETSLOT& rkPetSlot, const int& iEtcItemCode );
	int GetPetMaxLevel( const int& iRank );

	inline int GetPetMaxRank() { return m_iMaxRank; }
	PetRankInfo* GetPetRankInfo( const int& iPetRank );
	PetInfo*	GetPetInfo( const int& iPetCode );

	inline int GetMaxPetCount() { return m_iMaxPetCount; }
	inline int GetPetEatAdditiveCount()	{ return m_iPetEatAdditiveCount; }

public:
	bool AddExp( User *pUser, ioUserPet::PETSLOT& rkPetSlot, const int& iEtcItemCode );

	void SetPetLevelUpInfo( ioUserPet::PETSLOT& rkPetSlot, int iAddExp, int iRankMaxLevel );
	int LevelUpCheck( ioUserPet::PETSLOT& rkPetSlot, int& iAddExp, const int& iRankMaxLevel );

	bool CheckRightPetCode( const int& iPetCode );

public:
	ioPetInfoManager();
	virtual ~ioPetInfoManager();
};

#define g_PetInfoMgr ioPetInfoManager::GetSingleton()

#endif