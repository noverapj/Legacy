
#pragma once

#include "../Util/Singleton.h"

struct stRewardData
{
	int index;
	int type;
	int value1;
	int value2;
	int percent;

	stRewardData() : index(0), type(0), value1(0), value2(0), percent(0){}

	bool operator==( int temp )
	{
		if( index == temp )
			return true;
		return false;
	}
};

struct stRewardDataSort
{
public:
	bool operator()( const stRewardData& lhs, const stRewardData& rhs ) const
	{
		return lhs.percent > rhs.percent;
	}
};

enum BingoType
{
	BT_RAND = 0,
	BT_SET  = 1,
};

class ioBingoManager : public Singleton< ioBingoManager >
{
public:
	ioBingoManager();
	virtual ~ioBingoManager();

	void Init();
	void Destroy();

public:
	static ioBingoManager& GetSingleton();


private:
	BingoType m_iBingoType;

	int m_iBingoMaxNumber;
	int m_iState;
	int m_iPeriod;
	int m_iMent;
	
	vector< stRewardData > m_vecReward;		// ini 선물 전체.
	vector< stRewardData > m_vecAllBingo;	// 올빙고 ini 선물

	IntVec m_DummyInfoVec;

public:
	BOOL LoadINIData( const ioHashString &rkFileName );

	void SetMaxNumber( const int iNumber ){ m_iBingoMaxNumber = iNumber; }
	void SetState( const int iAlarm ){ m_iState = iAlarm; }
	void SetPeriod( const int iPeriod ){ m_iPeriod = iPeriod; }
	void SetMent( const int iMent ){ m_iMent = iMent; }

	BingoType GetBingoType(){ return m_iBingoType; }
	const int GetBingoMaxNumber(){ return m_iBingoMaxNumber; }
	const int GetState(){ return m_iState; }
	const int GetPeriod(){ return m_iPeriod; }
	const int GetMent(){ return m_iMent; }
	void GetBingoReward( vector< stRewardData >& rReward );	
	stRewardData& GetBingoPresentInfo( const int index );
	stRewardData& GetAllBingoPresentInfo( const int index );

	// 선물 전체
	void GetRegisterRewardPresentInfo( vector< stRewardData >& rPresent );
	void GetRegisterAllBingoPresentInfo( vector< stRewardData >& rPresent );

	//
	int GetBingoDuumyCode( int iIndex );
	
};

#define g_BingoMgr ioBingoManager::GetSingleton()
