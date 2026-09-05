
#ifndef _ioClassExpert_h_
#define _ioClassExpert_h_

#include "ioDBDataController.h"

class ioClassExpert : public ioDBDataController
{
public:
	enum
	{
		MAX_SLOT   = 10,	
	};

protected:
	struct CLASSSLOT
	{
		int m_iType;       // 용병 타입 
		int m_iLevel;      // 용병 레벨
		int m_iExpert;     // 용병 경험치
		BYTE m_byReinforce; // 용병 강화도

		CLASSSLOT()
		{
			m_iType			= 0;
			m_iLevel		= 0;
			m_iExpert		= 0;
			m_byReinforce	= 0;
		}
	};

	struct CLASSDB
	{
		bool		m_bChange;
		DWORD		m_dwIndex;
		CLASSSLOT	m_Data[MAX_SLOT];
		CLASSDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
			memset( m_Data, 0, sizeof( m_Data ) );
		}
	};
	typedef std::vector< CLASSDB > vCLASSDB;
	vCLASSDB m_vClassList;
	vCLASSDB m_vCopyClassList;

protected:
	void InsertDBClass( CLASSDB &kClassDB );
	void CreateClass( int iClassType );  

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:	
	bool AddClassExp( int iClassType, int iExp, int &rRemainExp );

	int GetClassLevel( int iClassType );	
	int GetClassExp( int iClassType ); // 2018-01-15 by bckim, 탐사 확장 //탐사
	int GetTopLevelClassType();

public:
	bool SetClassReinforce( const int iClassType, const int iReinforce );
	int GetClassReinfoce( const int iClassType );
	bool IsExistExpertInfo( const int iClassType );
	void CreateClassExpertInfo( const int iClassType );
	int GetSameClass( int iClassType, int iLevel, BYTE iStat, int bb, int &aa, BYTE &a, int & aaa);

public:
	ioClassExpert();
	virtual ~ioClassExpert();
};

#endif