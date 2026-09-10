#pragma once

struct PriateRulletReward
{
	enum RewardType
	{
		RT_NONE,
		RT_REWARD,
		RT_BONUS_REWARD,
	};

	RewardType	m_eRewardType;

	//보상 내역
	int				m_eType;
	int				m_nVaule1;
	int				m_nVaule2;
	bool			m_bHidden;

	//보상 지급시 참조할 범위 내역
	int				m_nMax;
	int				m_nMin;

	ioHashString	m_Icon;
	ioHashString	m_SubIcon;
	ioHashString	m_Name;

	PriateRulletReward()
	{	
		m_eType		= PRESENT_DEFAULT;
		m_nVaule1	= 0;
		m_nVaule2	= 0;
		m_bHidden	= false;
	}
};
typedef std::vector<PriateRulletReward> PriateRulletRewardTable;

struct SwordDamage
{
	int	m_nDamage;
	int m_nRand;

	SwordDamage()
	{
		m_nDamage	= 0;
		m_nRand		= 0;
	}
};
typedef std::vector<SwordDamage> SwordDamageTable;

struct SwordInfo
{
	int					m_nSwordCode;
	SwordDamageTable	m_SwordDamageTable;

	SwordInfo()
	{
		m_nSwordCode = 0;
	}
};
typedef std::vector<SwordInfo> SwordInfoTable;

struct DevelopKFace
{
	ioHashStringVec	m_FaceName;

	int				m_nMax;
	int				m_nMin;
	int				m_nAnimationTime;
	int				m_nBubbleTime;
	ioHashStringVec	m_BubbleMentList;

	DevelopKFace()
	{		
		m_nMax				= 0;
		m_nMin				= 0;
		m_nAnimationTime	= 0;
	}
};
typedef std::vector<DevelopKFace> DevelopKFaceTable;


DWORD OakSwordTypeToOakCode( OakSwordType eType );
OakSwordType OakCodeToOakSwordType( DWORD dwCode );

class ioPriateRullet
{
public:
	enum
	{
		MAX_RULLET_COUNT	= 40,
		RULLET_BACK			= MAX_RULLET_COUNT / 2,
	};

protected:
	int							m_RoulletTable[MAX_RULLET_COUNT];
	int							m_nMaxHP;
	int							m_nCurrHP;
	int							m_nCurrRewardTurnNumber;

	PriateRulletRewardTable		m_RewardTable;
	PriateRulletRewardTable		m_BonusRewardTable;
	SwordInfoTable				m_SwordInfoTable;
	DevelopKFaceTable			m_DevelopKFaceTable;

public:
	void ClearRullet();
	void Initialize();
	void LoadReward( ioINILoader& rkLoader );
	void LoadBonusReward( ioINILoader& rkLoader );
	void LoadSword( ioINILoader& rkLoader );
	void LoadDevelopKFace( ioINILoader& rkLoader );

public:
	PriateRulletReward* GetReward( int Index );
	PriateRulletReward* GetCurrReward();
	int GetRewardTurnNumber( int nHP ) const;

	PriateRulletReward* GetBounsReward( int Index );
	PriateRulletReward* GetCurrBounsReward();

public:
	int GetRulletStickedCount();
	int GetSwordCount( int Index );
	int GetRulletStickedValue( int Index );
	int	GetCurrHP() const;

	int GetFaceIndex() const;
	int GetFaceIndex( int nHP ) const;

	DWORD GetCurrFaceTime() const;
	void GetCurrFaceName( ioHashStringVec& szFaceList ) const;

	void GetCurrBubble( ioHashStringVec& BubbleList, int& nBubbleTime );

public:
	void SendRequest();
	void ApplyRulletRequest( SP2Packet &rkPacket );

	bool SendRulletUse( int Index, DWORD dwCode );
	void ApplyRulletUse( SP2Packet &rkPacket );

	void SendRulletReset();
	void ApplyRulletReset( SP2Packet &rkPacket );

public:
	ioPriateRullet();
	~ioPriateRullet();
};