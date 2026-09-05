#pragma once
class ioMatchingGame
{
public:
	enum
	{
		CARD_NUMBER_1	= 0,
		CARD_NUMBER_2,
		CARD_NUMBER_3,
		CARD_NUMBER_4,
		CARD_NUMBER_5,
		CARD_NUMBER_6,
		CARD_NUMBER_7,
		CARD_NUMBER_8,
		CARD_NUMBER_9,
		CARD_NUMBER_10,
		CARD_NUMBER_11,
		CARD_NUMBER_12,
		CARD_NUMBER_13,
		CARD_NUMBER_14,
		CARD_NUMBER_15,
		CARD_LUCKY1,
		CARD_LUCKY2,
		CARD_NUMBER_NOT_OPENED = 999,
	};

	enum
	{
		MISSION_TYPE_NONE	= 0,
		MISSION_TYPE_3		= 3,
		MISSION_TYPE_4		= 4,
		MISSION_TYPE_ALL	= 5
	};

	enum
	{
		MISSION_COUNT_TYPE_3	= 3,
		MISSION_COUNT_TYPE_4	= 4,
		MISSION_COUNT_MAX		= 4
	};

	enum
	{
		MAX_NUMOF_CELL	= 30
	};

	enum
	{
		MAX_REWARD	= 3
	};

	enum Card_State
	{
		CARD_NONE	= 0,
		CARD_NOT_CHECKED,
		CARD_CONFIRMED,
		CARD_MATCHED_DONE
	};

	enum MatchingGameStatus
	{
		MGS_GAME_PREPARE,
		MGS_GAME_INIT,
		MGS_GAME_PLAYING,
		MGS_GAME_CLOSING
	};

	// 맞춰야할 그림판 정보
	struct CellInfo
	{
		bool m_bIsOpened;
		int	m_Status;
		int m_iCardNum;

		CellInfo()
		{
			m_bIsOpened = false;
			m_Status	= CARD_NONE;
			m_iCardNum = CARD_NUMBER_NOT_OPENED;
		}
	};
	typedef std::map<int, CellInfo> CellInfoMap;

	// 미션 정보
	struct MissionInfo
	{
		int m_iMissionType;
		int m_iLucky1Node;
		int m_iLucky2Node;
		int m_iLucky1CompletedNode;
		int m_iLucky2CompletedNode;
		int	m_iRewardItemType;
		DWORD	m_dwRewardItemValue;
		ioHashString m_szRewardName;
		ioHashString m_szRewardInfo;

		MissionInfo()
		{
			m_iMissionType	= MISSION_TYPE_NONE;
			m_iLucky1Node		= 0;
			m_iLucky2Node		= 0;
			m_iLucky1CompletedNode	= 0;
			m_iLucky2CompletedNode	= 0;
			m_iRewardItemType	= 0;
			m_dwRewardItemValue	= 0;
			m_szRewardName.Clear();
			m_szRewardInfo.Clear();
		}
	};
	typedef std::vector<MissionInfo> MissionVector;

protected:

	DWORD	m_dwCurrnetTime;
	DWORD	m_dwTotalTime;

	CellInfoMap		m_CellInfoMap;
	MissionVector	m_MissionVector;

	MissionInfo		m_CurrentMissionInfo;

	int	m_iRewardType[MAX_REWARD];
	int	m_iRewardValue[MAX_REWARD];

	MatchingGameStatus	m_Status;

	int	m_iMissionReward;

	int	m_iFirstSelect;
	int	m_iSecondSelect;

	int	m_iCardMatchCount;

	int	m_iTicket;

public:
	void	LoadProperty();

	void	SetCellInfo( int iCell, int iCardNum );
	void	OpenCard( int iCell );
	void	CloseCard();
	bool	SelectCell( int iCell );

	void	SetMissionInfo( int iType, int iLucky1Node, int iLucky2Node, int iLucky1CompleteNode, int iLucky2CompleteNode );
	void	UpdateMissionInfo( int iLucky1, int iLucky2 );
	MissionInfo*	GetMissionInfo()	{	return	&m_CurrentMissionInfo;	}

	bool	IsCellOpened( int iCell );
	int		GetCellStatus( int iCell );
	int		GetCellCardNum( int iCell );

	void	SetFirstSelect( int iSelect );
	void	SetSecondSelect( int iSelect );

	void	Reset();

public:
	void	ApplyMatchingGamePacket( SP2Packet &rkPacket );

	void	SendOpenGame();
	void	SendGameStart( bool bPremium = false );
	void	SendCellSelect( int iCell );
	void	SendEndGame();

	int		GetTicketCount( bool bPremium = false );
	DWORD	GetTotalTime();

	bool	IsMissionComplete();

	const	ioHashString	GetRewardIconName( int iReward );

inline	void	SetTicket( int iTicket )	{	m_iTicket	= iTicket;	}
inline	int		GetTicket()					{	return	m_iTicket;		}

inline	int		GetCardMatchCount()			{	return	m_iCardMatchCount;	}

inline	MatchingGameStatus	GetGameStatus()	{	return	m_Status;		}

public:
	ioMatchingGame(void);
	virtual ~ioMatchingGame(void);
};

