#ifndef _GuildNode_h_
#define _GuildNode_h_
#include "../BoostPooler.h"

class SP2Packet;
typedef struct tagGuildRecord
{
	int m_iWin;
	int m_iLose;
	int m_iKill;
	int m_iDeath;

	bool m_bChange;
	tagGuildRecord()
	{
		Init();
	}

	void Init()
	{
		m_iWin = m_iLose = 0;
		m_iKill = m_iDeath = 0;
		m_bChange = false;
	}
}GUILDRECORD;

class GuildNode : public BoostPooler<GuildNode>
{
protected:
	// 기본 정보
	DWORD m_dwGuildIndex;            //길드 고유 인덱스
	DWORD m_dwGuildMark;             //길드 마크 업데이트 인덱스	
	DWORD m_dwGuildRegDate;          //길드 생성 일자
	DWORD m_dwGuildMaxEntry;         //최대 길드원
	DWORD m_dwGuildJoinUser;		 //현재 길드인원
	
	ioHashString m_szGuildName;      //길드 이름.
	ioHashString m_szGuildTitle;     //길드 소개.

	// 게임 서버에서 변경되며 DB에 업데이트 되어야할 값.
	DWORD m_dwGuildRank;             //길드 랭킹
	DWORD m_dwGuildPoint;            //길드 포인트 GP : 랭킹전까지 획득한 포인트 (음수가 될수없다)
	DWORD m_dwCurGuildPoint;         //시즌중 획득한 길드 포인트(음수가 될수없다)
	DWORD m_dwGuildLevel;            //길드 레벨

	// 전적 정보 
	GUILDRECORD m_Record;            //길드 전적

	// 길드 정보 저장 등록
	bool m_bSaveReg;
protected:
	void InitData();

public:
	void CreateGuild( SP2Packet &rkPacket, DWORD dwDefaultLevel );
	BOOL CreateGuild( CQueryResultData *pQueryData );

protected:
	DWORD GetLimitMaxEntry();

public:
	inline const DWORD GetGuildIndex() const { return m_dwGuildIndex; }
	inline DWORD GetGuildMark() { return m_dwGuildMark; }
	inline DWORD GetGuildRegDate() { return m_dwGuildRegDate; }
	inline DWORD GetGuildMaxEntry() { return m_dwGuildMaxEntry; }
	inline DWORD GetGuildJoinUser() { return m_dwGuildJoinUser; }

	inline ioHashString GetGuildName() { return m_szGuildName; }
	inline ioHashString GetGuildTitle() { return m_szGuildTitle; }

	inline DWORD GetGuildRank() { return m_dwGuildRank; }
	inline const DWORD GetGuildPoint() const { return m_dwGuildPoint; }
	inline const DWORD GetCurGuildPoint() const { return m_dwCurGuildPoint; }
	inline const DWORD GetGuildLevel() const { return m_dwGuildLevel; }
	inline GUILDRECORD GetGuildRecord() { return m_Record; }
	
	float GetGuildBonus();

public:
	inline void SetGuildRank( DWORD dwSetRank ) { m_dwGuildRank = dwSetRank; }
	inline void SetGuildTitle( const ioHashString &szGuildTitle ) { m_szGuildTitle = szGuildTitle; }
	inline void SetGuildMark( DWORD dwGuildMark ) { m_dwGuildMark = dwGuildMark; }
	void SetGuildMaxEntry( DWORD dwMaxEntry );
	void SetGuildJoinUser( DWORD dwJoinUserCount );
	void SetGuildLevel( DWORD dwGuildLevel, DWORD dwGuildMaxEntry );

public:
	void ChangeGuildName( const ioHashString &rkNewName );
	
public:
	inline void InitGuildCurPoint(){ m_dwCurGuildPoint = 0; }


public:
	void AddGuildRecord( int iWin, int iLose, int iKill, int iDeath );
	void AddGuildPoint( int iCurGP );

public:
	void Save();
	void SetSaveReg(){ m_bSaveReg = true; }
	bool IsSaveReg(){ return m_bSaveReg; }

public:
	GuildNode();
	virtual ~GuildNode();
};

#endif