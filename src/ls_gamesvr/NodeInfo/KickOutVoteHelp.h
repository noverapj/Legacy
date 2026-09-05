#ifndef _KickOutVoteHelp_h_
#define _KickOutVoteHelp_h_

class Mode;

class KickOutVoteHelp
{
protected:
	Mode *m_pCreator;

protected:
	struct KickVoteData
	{
		int m_iVoteType;
		TeamType m_eTeam;
		ioHashString m_szName;
		KickVoteData()
		{
			m_iVoteType = 0;
			m_eTeam     = TEAM_NONE;
		}
	};
	typedef std::vector< KickVoteData > KickVoteDataList;
	KickVoteDataList m_VoteUserList;
	
	ioHashStringVec  m_VoteProposalUserList;       // 1회만 가능함
	ioHashString     m_NowVoteProposalName;        // 강퇴 제안한 유저
	ioHashString     m_NowVoteTargetName;          // 강퇴 대상자	
	ioHashString	 m_NowVoteReason;			   // 강퇴 이유
	
	DWORD            m_dwCurVoteTime;              // 투표 시작 시간

	// 강퇴 INI
    DWORD            m_dwVoteProcessTime;          // 제한 시간
	int              m_iKickVoteUserPool;          // 투표를 하기 위한 인원
	int              m_iKickVoteRoundWin;          // 투표를 하기 위한 라운드 제한(승 라운드)
	DWORD            m_dwKickVoteRoundTime;        // 투표를 하기 위한 플레이 시간
	float            m_fVoteAdoptionPercent;       // 투표가 가결되기 위한 퍼센트

public:
	void InitVote();
	void LoadVoteInfo( Mode *pCreator, ioINILoader &rkLoader );

public:
	void ProcessVote();

public:
	const DWORD GetVoteProcessTime() { return m_dwVoteProcessTime; }
	const int GetKickVoteUserPool() { return m_iKickVoteUserPool; }
	const int GetKickVoteRoundWin() { return m_iKickVoteRoundWin; }
	const DWORD GetKickVoteRoundTime() { return m_dwKickVoteRoundTime; }
	const float GetVoteAdoptionPercent() { return m_fVoteAdoptionPercent; }

	const ioHashString &GetVoteProposalName() { return m_NowVoteProposalName; }
	const ioHashString &GetVoteTargetName() { return m_NowVoteTargetName; }
	const ioHashString &GetVoteReason() { return m_NowVoteReason; }
	TeamType GetVoteUserTeam( const ioHashString &rkName );

public:
	bool IsVoteProposalUser( const ioHashString &rkName );
	bool IsVoting();
	void StartVote( const ioHashString &rkProposalName, const ioHashString &rkTargetName, const ioHashString &rkReason );
	void EndVote();
	void InsertVoteUserList( const ioHashString &rkName, TeamType eTeam );
	void RemoveVoteUserList( const ioHashString &rkName );
	void SetKickVote( const ioHashString &rkName, int iVoteType );
	void CheckKickVoteResult();

public:
	KickOutVoteHelp();
	virtual ~KickOutVoteHelp();
};

#endif
