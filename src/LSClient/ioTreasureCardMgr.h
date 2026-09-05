
#ifndef _ioTreasureCardMgr_h_
#define _ioTreasureCardMgr_h_

#include "IORandom.h"

class ioTreasureCardMgr  : public Singleton< ioTreasureCardMgr >
{
protected:
	struct TreasureCard
	{
		DWORD m_dwRandValue;
		short m_iPresentType;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		TreasureCard()
		{
			m_dwRandValue	= 0;
			m_iPresentType	= 0;
			m_iPresentValue1= m_iPresentValue2 = 0;
		}
	};
	typedef std::vector< TreasureCard > TreasureCardVec;
	
	struct TreasureRand
	{
		DWORD m_dwRandSeed;
		TreasureCardVec m_TreasureCardVec;

		TreasureRand()
		{
			m_dwRandSeed = 0;
		}
	};
	typedef std::map< DWORD, TreasureRand > TreasureRandTable;
	TreasureRandTable m_TreasureRandTable;
	IORandom          m_TreasureCardRandom;

public:
	void Initialize();

public:
	void TreasureCardRandSeed( DWORD dwSeed );
	void TreasureCardRandom( DWORD dwTable, short &rPresentType, int &rPresentValue1, int &rPresentValue2 );

public:
	static ioTreasureCardMgr& GetSingleton();

public:
	ioTreasureCardMgr();
	virtual ~ioTreasureCardMgr();
};

#define g_TreasureCardMgr ioTreasureCardMgr::GetSingleton()

#endif
