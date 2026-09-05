#pragma once

class ioItem;

class ioAnnounce
{
protected:
	DWORD	m_dwStartTime;
	DWORD	m_dwEndTime;

public:
	void SetLiveTime( DWORD dwLiveTime );
	DWORD GetStartTime() const;

public:
	virtual bool IsLive() const;
	virtual int GetLevel() const = 0;
	virtual void Render() = 0;

public:
	ioAnnounce();
	virtual ~ioAnnounce();
};

class ioGameReadyAnnounce : public ioAnnounce
{
protected:
	TeamType m_eOwnerTeam;
	int m_iCurRound;
	int m_iHalfRound;

public:
	void SetAnnounce( TeamType eOwnerTeam, int iCurRound, int iHalfRound );

public:
	virtual int GetLevel() const;
	virtual void Render();

public:
	ioGameReadyAnnounce();
	virtual ~ioGameReadyAnnounce();
};

class ioNormalAnnounce : public ioAnnounce
{
protected:
	int m_iLevel;
	char m_szTitle[MAX_PATH];
	char m_szAnnounce[MAX_PATH];
	DWORD m_dwTitleColor;

public:
	void SetAnnounce( const char *szTitle,
					  const char *szAnnounce,
					  DWORD dwTitleColor );

	void SetLevel( int iLevel );

public:
	virtual int GetLevel() const;
	virtual void Render();

public:
	ioNormalAnnounce();
	virtual ~ioNormalAnnounce();
};

class ioLevelupAnnounce : public ioAnnounce
{
protected:
	int m_Stat[4];

public:
	void SetAnnounce( int iStr, int iDex, int iInt, int iVit );

public:
	virtual int GetLevel() const;
	virtual void Render();

public:
	ioLevelupAnnounce();
	virtual ~ioLevelupAnnounce();
};

class ioGetItemAnnounce : public ioAnnounce
{
protected:
	ioHashString m_ItemName;
	ioHashString m_SkillName;

public:
	void SetAnnounce( const ioItem *pItem );

public:
	virtual int GetLevel() const;
	virtual void Render();

public:
	ioGetItemAnnounce();
	virtual ~ioGetItemAnnounce();
};

class ioRevivalAnnounce : public ioAnnounce
{
public:
	virtual int GetLevel() const;
	virtual void Render();

public:
	ioRevivalAnnounce();
	virtual ~ioRevivalAnnounce();
};

class ioDefenseSymbolAnnounce : public ioAnnounce
{
protected:
	bool  m_bDefenseMode;

public:
	void SetAnnounce( bool bDefenseMode );

public:
	virtual int GetLevel() const;
	virtual void Render();

public:
	ioDefenseSymbolAnnounce();
	virtual ~ioDefenseSymbolAnnounce();
};

class ioExitPrisonerMode : public ioAnnounce
{
protected:
	bool  m_bExitPrisonerMode;

public:
	void SetAnnounce( bool bExitPrisonerMode );

public:
	virtual int GetLevel() const;
	virtual void Render();

public:
	ioExitPrisonerMode();
	virtual ~ioExitPrisonerMode();
};

class ioTimeLimitAnnounce : public ioAnnounce
{
public:
	virtual int GetLevel() const;
	virtual void Render();

public:
	ioTimeLimitAnnounce();
	virtual ~ioTimeLimitAnnounce();
};

