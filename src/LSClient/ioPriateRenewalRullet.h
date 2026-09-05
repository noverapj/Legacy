#pragma once

#include "ioPriateRullet.h"

// 오크통 예외 처리 사항
enum OakBarrelException
{
	OAK_BARREL_OK					= 0,
	OAK_BARREL_ERR_HOLE_INDEX		= 1,	// 구멍 인덱스 범위 초과된 값
	OAK_BARREL_ERR_HOLE_DUPLICATED	= 2,	// 구멍 인덱스 중복
	OAK_BARREL_ERR_REWARD_STEP		= 3,	// 보상 단계가 맞지 않음
	OAK_BARREL_ERR_LIMIT_SWORD		= 4,	// 일일 한도 수량을 넘어섬
};

struct PriateRenewalPresentData
{
	int				m_eType;
	int				m_iVaule1;
	int				m_iVaule2;
	ioHashString	m_Icon;
	ioHashString	m_SubIcon;
	ioHashString	m_Name;

	PriateRenewalPresentData()
	{
		m_eType		= PRESENT_DEFAULT;
		m_iVaule1	= 0;
		m_iVaule2	= 0;
	}
};

typedef std::vector<PriateRenewalPresentData> PresentDataList;

struct PriateRenewalRewardInfo
{
	int			    m_iMax;
	PresentDataList m_PresentDataList;
};
typedef std::vector<PriateRenewalRewardInfo> PriateRenewalRewardInfoList;

class ioPriateRenewalRullet
{
public:
	enum
	{
		MAX_RENEWAL_RULLET_COUNT	= 12,
	};

protected:
	int							m_RoulletTable[MAX_RENEWAL_RULLET_COUNT];
	int							m_iCurrEnableRewardTurnNumber;
	int							m_iMaxEnableRewardTurnNumber;
	int							m_iSwordCode;

	PriateRenewalRewardInfoList	m_RewardInfoList;
	DevelopKFaceTable			m_DevelopKFaceTable;

	bool						m_bUsingOakPriate;

public:
	void ClearRullet();
	void Initialize();
	void LoadReward( ioINILoader& rkLoader );
	void LoadDevelopKFace( ioINILoader& rkLoader );

public:
	PriateRenewalRewardInfo* GetReward( int Index );
	PriateRenewalRewardInfo* GetCurrReward();

public:
	int GetRulletStickedCount();
	int GetSwordCount();
	int GetRulletStickedValue( int Index );

	int GetFaceIndex() const;
	int GetFaceIndex( int iEnableRewardTurnNumber ) const;

	DWORD GetCurrFaceTime() const;
	void GetCurrFaceName( ioHashStringVec& szFaceList ) const;

	void GetCurrBubble( ioHashStringVec& BubbleList, int& nBubbleTime );

	const int& GetCurEnableRewardTurnNumber() const { return m_iCurrEnableRewardTurnNumber; }
	const int& GetMaxEnableRewardTurnNumber() const { return m_iMaxEnableRewardTurnNumber; }
	const int& GetSwordCode() const { return m_iSwordCode; }
	
	//Packet
public:
	void ApplyRenewalOakPacket( SP2Packet &rkPacket );
	void RenewalOakInfo( SP2Packet &rkPacket );
	void RenewalOakSwordUse( SP2Packet &rkPacket );
	void RenewalOakGetReward( SP2Packet &rkPacket );

	bool SendRenewalRulletUse( int Index );
	bool SendRewardRequest();

public:
	//사용 여부
	inline void SetUsingOakPriate( bool bUse ) { m_bUsingOakPriate = bUse; }
	inline const bool& UsingOakPriate() const { return m_bUsingOakPriate; }

public:
	void SetValue( int i ) { m_iCurrEnableRewardTurnNumber = i;}

public:
	ioPriateRenewalRullet();
	~ioPriateRenewalRullet();
};
