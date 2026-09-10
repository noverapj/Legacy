#pragma once

#include "DataHeaders/LSC_SnakeLadders_Move.h"
#include "DataHeaders/LSC_SnakeLadders_Reward.h"

class ioDiceGame
{
public:
	enum
	{
		MAX_REWARD_INDEX			= 9,
		MAX_REWARD_INFO_PER_LINE	= 10,
		MAX_SLOT					= 87
	};

	struct MoveInfo
	{
		int		m_iStartPos;
		int		m_iArrivalPos;
		bool	m_bIsJump;
		int		m_JumpPos;
	};

private:
	typedef std::map<int, bool> TraceMap;
	TraceMap	m_TraceMap;

	LSC_SnakeLadders_Move_Manager  *m_pMoveManager;
	LSC_SnakeLadders_Reward_Manager *m_pRewardManager;

	int		m_iNormalRewardCount;
	int		m_iFinalRewardCount;

	BYTE	m_byBoardIndex;
	int		m_iCurrentDice;
	int		m_iCurrentPosition;
	int		m_iDestPosition;
	int		m_iFinalReward;
	int		m_iRewardIndex[MAX_REWARD_INDEX];

protected:
	void	LoadINI();
	void	LoadData();

public:
	void	Load();

	void	ApplyDiceGamePacket( SP2Packet &rkPacket );

	void	SendOpenGame();
	void	SendDiceRoll();
	void	SendRewardChange();
	void	SendBoardChange();
	void	SendGameEnd();
	void	SendRestartGame();

	SHORT	GetSnakeLadderEndPoint( SHORT sStartPoint );
	bool	IsSnakeLadderPoint( SHORT sPoint );

	int		GetDiceCount();
	int		GetBoardChangeCount();
	int		GetRewardChangeCount();

	LSC_SnakeLadders_Reward*	GetRandomReward( bool bFianlReward );
	LSC_SnakeLadders_Reward*	GetReward( int iIndex );
	bool	GetTraceInfo( int iIndex );
	bool	IsFinalRewardReceipt();

	inline	int		GetCurrentPosition()	{	return	m_iCurrentPosition;	}
	inline	BYTE	GetBoardIndex()			{	return	m_byBoardIndex;		}

	ioDiceGame(void);
	virtual ~ioDiceGame(void);
};

