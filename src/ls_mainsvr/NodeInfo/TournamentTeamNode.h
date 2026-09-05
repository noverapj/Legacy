#ifndef _TournamentTeamNode_h_
#define _TournamentTeamNode_h_

class TournamentTeamNode
{
protected:
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;
	ioHashString m_szTeamName;
	DWORD m_dwOwnerIndex;
	SHORT m_Position;
	SHORT m_StartPosition;
	BYTE  m_MaxPlayer;
	int   m_iCheerPoint;
	BYTE  m_TourPos;
	int   m_iLadderPoint;
	BYTE  m_CampPos;

protected:
	DWORD	m_dwChangeTimer;
	DWORD   m_dwSaveTime;

protected:
	bool    m_bDropEntry;		// 첫 라운드 진출 후 예비 엔트리로 교체된 팀 - DB에는 저장하지 않는다.

protected:
	void InitDataChange();
	void SetDataChange();

public:
	bool IsDataSave();
	bool IsDataSaveTimeCheck();

public:
	const inline DWORD GetTourIndex() const{ return m_dwTourIndex; }
	const inline DWORD GetTeamIndex() const{ return m_dwTeamIndex; }
	const inline ioHashString &GetTeamName() const{ return m_szTeamName; }
	const inline DWORD GetOwnerIndex() const{ return m_dwOwnerIndex; }
	const inline SHORT GetPosition() const{ return m_Position; }
	const inline SHORT GetStartPosition() const { return m_StartPosition; }
	const inline BYTE  GetMaxPlayer() const{ return m_MaxPlayer; }
	const inline int   GetCheerPoint() const{ return m_iCheerPoint; }
	const inline BYTE  GetTourPos() const{ return m_TourPos; }
	const inline int   GetLadderPoint() const{ return m_iLadderPoint; }
	const inline BYTE  GetCampPos() const{ return m_CampPos; }
	const inline bool  IsDropEntry() const { return m_bDropEntry; }

public:
	void FillTeamInfo( SP2Packet &rkPacket );
	
public:
	void SetTourPos( BYTE TourPos );
	void SetPosition( SHORT Position );
	void SetStartPosition( SHORT StartPosition );
	void SetLadderPointAdd( int iLadderPoint );
	void SetDropEntry( bool bDrop ){ m_bDropEntry = bDrop; }

public:
	void IncreaseCheerPoint(){ m_iCheerPoint++; }	

public:
	void SaveData();

public:
	TournamentTeamNode( DWORD dwTourIndex, DWORD dwTeamIndex, ioHashString szTeamName,DWORD dwOwnerIndex,int iLadderPoint ,BYTE CampPos, BYTE MaxPlayer );
	TournamentTeamNode( DWORD dwTourIndex, DWORD dwTeamIndex, ioHashString szTeamName,DWORD dwOwnerIndex,int iLadderPoint ,BYTE CampPos, BYTE MaxPlayer, int iCheerPoint, SHORT Position, SHORT StartPosition, BYTE TourPos );
	virtual ~TournamentTeamNode();
};

// 래더 포인트로 팀 정렬
class TournamentTeamSort
{
public:
	bool operator()( const TournamentTeamNode *lhs , const TournamentTeamNode *rhs ) const
	{
		if( lhs->GetLadderPoint() > rhs->GetLadderPoint() ) 
			return true;
		else if( lhs->GetLadderPoint() == rhs->GetLadderPoint() )
		{
			if( lhs->GetTeamIndex() < rhs->GetTeamIndex() )
				return true;
		}
		return false;
	}
};

#endif