
#pragma once

#include "../Util/Singleton.h"

struct stRouletteRewardData
{
	int index;
	int type;			// 0 : no type,  1 : normal reward ( by HP Range ), 2 : Bonus reward ( 
	int value1;
	int value2;
	int minHP;
	int maxHP;
	char cMinUseCount;
	char cMaxUseCount;

	stRouletteRewardData() : index(0), type(0), value1(0), value2(0), minHP(0), maxHP(1000), cMinUseCount(0), cMaxUseCount(40){}
};

struct stRouletteSwordData
{
	int iType;
	int iSwordDamage1;
	int iSwordDamage2;
	int iSwordDamage3;
	int iSwordDamage1_rand;
	int iSwordDamage2_rand;
	int iSwordDamage3_rand;
};

class ioPirateRouletteManager : public Singleton< ioPirateRouletteManager >
{
public:
	ioPirateRouletteManager();
	virtual ~ioPirateRouletteManager();

	void Init();
	void Destroy();

public:
	static ioPirateRouletteManager& GetSingleton();


private:
//	int m_iBingoMaxNumber;
	int m_iMaxHP;
	int m_iMaxReward;
	int m_iMaxBonus;
	int m_iSwordTypeMax;
	int m_iNoticeSwordUseCount;

	int m_iState;
	int m_iPeriod;
	int m_iMent;
	
	vector< stRouletteRewardData > m_vecReward;			// ini 선물 by HP Range.
	vector< stRouletteRewardData > m_vecBonusReward;	// 보너스 by Sword Count Range
	vector< stRouletteSwordData > m_vecSword;

	IntVec m_SwordInfoVec;

public:
	BOOL LoadINIData( const ioHashString &rkFileName );
	void SetMaxHP ( const int iMaxHP ){ m_iMaxHP = iMaxHP; }
	void SetState( const int iAlarm ){ m_iState = iAlarm; }
	void SetPeriod( const int iPeriod ){ m_iPeriod = iPeriod; }
	void SetMent( const int iMent ){ m_iMent = iMent; }

	const int GetMaxHP(){return m_iMaxHP;}
	const int GetMaxReward() {return m_iMaxReward;}
	const int GetMaxBonus() {return m_iMaxBonus;}
	const int GetState(){ return m_iState; }
	const int GetPeriod(){ return m_iPeriod; }
	const int GetMent(){ return m_iMent; }
	const int GetNoticeSwordUseCount() { return m_iNoticeSwordUseCount; }

	void GetReward( vector< stRouletteRewardData >& rReward );	
	stRouletteRewardData& GetRouletteRewardInfoByHP( const int iHP );
	stRouletteRewardData& GetRouletteRewardInfo( const int index );
	stRouletteRewardData& GetBonusRewardPresentInfoByCount( const int count );
	stRouletteRewardData& GetBonusRewardPresentInfo( const int index );
	stRouletteSwordData& GetSwordInfo( const int swordType);

	// 선물 전체
	void GetRegisterRewardPresentInfo( vector< stRouletteRewardData >& rPresent );
	void GetRegisterBonusRewardPresentInfo( vector< stRouletteRewardData >& rPresent );

	//
	int GetSwordDummyCode( int iIndex );
	
};

#define g_PirateRouletteMgr ioPirateRouletteManager::GetSingleton()
