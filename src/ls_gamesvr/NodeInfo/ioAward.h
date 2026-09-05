
#ifndef _ioAward_h_
#define _ioAward_h_

#include "ioDBDataController.h"

class ioAward : public ioDBDataController
{
public:
	enum
	{
		MAX_SLOT   = 10,	
	};

protected:
	struct AWARDSLOT
	{
		int m_iCategory;       // 항목
		int m_iCount;          // 수상 횟수
		int m_iPoint;          // 수상 포인트
	};

	struct AWARDDB
	{
		bool     m_bChange;
		DWORD    m_dwIndex;
		AWARDSLOT m_Award[MAX_SLOT];		
		AWARDDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
			memset( m_Award, 0, sizeof( m_Award ) );
		}
	};
	typedef std::vector< AWARDDB > vAWARDDB;
	vAWARDDB m_vAwardList;
	int      m_iAwardLevel;
	int      m_iAwardExp;
	bool     m_bAwardChange;

protected:
	void InsertDBAward( AWARDDB &kAwardDB );

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	void SetAwardExpert( int iLevel, int iExp );
	void AddAward( int iCategory, int iPoint );
	bool AddAwardExp( int iExp );
	int GetAwardLevel();
	int GetAwardExp();

public:
	ioAward();
	virtual ~ioAward();
};

#endif