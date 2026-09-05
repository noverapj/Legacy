#pragma once


#include "ioPlayEntity.h"

class ioPlayMode;
class ioBaseChar;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class SP2Packet;

class ioSymbolStruct : public ioPlayEntity
{
public:
	enum SymbolState
	{
		SS_DELAY,
		SS_WOUNDED,
	};

protected:
	int	m_iArrayIndex;
	SymbolState m_State;
	ioGaugeHelper *m_pGauge;

	ArmorType   m_ArmorType;

	CEncrypt<float> m_fDefaultMaxHP;
	CEncrypt<float> m_fExtraHP;
	CEncrypt<float> m_fExtraRate;

	CEncrypt<float> m_fTeamExtraDamageRate;

	CEncrypt<float> m_fRevivalRate;
	CEncrypt<float> m_fRevivalHP;

	TeamType m_OrgTeam;

	float m_fSymbolRange;
	CEncrypt<float> m_fCharHpRecovery;

	CEncrypt<float> m_fSkillGaugeRate;
	CEncrypt<float> m_fCharDamageRate;

	CEncrypt<float> m_fCurMaxHP;
	CEncrypt<float> m_fDecreaseRate;
	CEncrypt<float> m_fDecreaseCurRate;
	CEncrypt<float> m_fDecreaseMaxRate;

	CEncrypt<float> m_fRecovery;
	CEncrypt<float> m_fRecoveryRate;
	
	CEncrypt<DWORD> m_dwDecreaseTicTime;
	CEncrypt<DWORD> m_dwDecreaseCheckTime;

	ioHashString m_RedSymbolModel;
	ioHashString m_BlueSymbolModel;
	ioHashString m_NoneSymbolModel;
	ioHashString m_NoneActivitySymbolModel;
	ioHashString m_ExplosionSound;

	ioHashString m_BlockBuff;

	int m_iRevivalCnt;

	D3DXVECTOR3 m_vBackUpPos;
	D3DXVECTOR3 m_vDefaultPos;
	DWORD m_dwWoundedSetTime;
	bool m_bShakeLeft;
	
	int m_iSymbolPriority;
	bool m_bActivity;
	bool m_bDie;

	DWORD m_dwSymbolChangeTime;

public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );

	virtual void SetTeam( TeamType eType );
	virtual void Revival();

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );

	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

protected:
	virtual void UpdateBottomHeight() const;
	virtual void ProcessRecovery();

public:
	virtual bool IsMountAvailable() const;
	virtual bool IsMountingAvailable() const;
	virtual bool IsNowMountEnable() const;

	virtual int  DontMoveEntityLevel() const;
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void UpdateGauge();
	virtual void RenderGauge();

protected:
	void ProcessWoundedState();
	void ProcessHPDecrease();

public:
	void InitResource();
	void InitSymbolInfo( int iIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale );

	bool IsInSymbolRange( ioBaseChar *pChar );
	void SetSymbolPriority( int iPriority );
	void SetSymbolActivity( bool bActivity );

	inline float GetSymbolRange() const { return m_fSymbolRange; }
	inline int   GetSymbolPriority() { return m_iSymbolPriority; }
	inline bool  GetSymbolActivity() { return m_bActivity; }

	inline void SetRecoveryRate( float fRate ) { m_fRecoveryRate = fRate; }

public:
	void SetHP( float fCurHP, float fMaxHP, float fDecreaseCurRate );
	void UpdateMaxHP( float fMaxHP );
	void ReduceMaxHP( float fRate );	
	void SetWoundedState();
	void SetEndFocusEffect( const ioHashString &szEndFocusEffect );
	void SetOrgTeam( TeamType eTeam );
	void RestoreOrgTeam();
	void ResetTeam();            //광장에서 유저보다 상징물이 먼저 세팅되므로 유저가 들어오면 다시 세팅을해준다. 팀 변경은 없음.

public:
	inline TeamType GetOrgTeam() const { return m_OrgTeam; }

	inline float GetHPRecovery() const { return m_fCharHpRecovery; }
	inline float GetSkillGaugeRate() const { return m_fSkillGaugeRate; }
	inline float GetCharDamageRate() const { return m_fCharDamageRate; }

	inline float GetCurMaxHP() { return m_HP.m_fMaxValue; }
	inline float GetCurHP() { return m_HP.m_fCurValue; }
	inline float GetCurDecreaseRate() const { return m_fDecreaseCurRate; }

	float GetCurHPRate();

public:
	inline void SetRevivalCount( int iRevivalCnt ) { m_iRevivalCnt = iRevivalCnt; }
	inline int  GetRevivalCount() const { return m_iRevivalCnt; }
	inline int GetSymbolArrayIdx() const { return m_iArrayIndex; }

	void SetAddHP( int iCharCnt );
	void SetExtraRate( int iCharCnt, float fRate );
	void SetRevivalHP( float fHP, bool bInit = false );

	void SetChangeEvent();
	DWORD GetEventGapTime();
	D3DXVECTOR3 GetSpotPosition( float fPositionRate );

public:
	void OnSymbolDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker );

public:
	ioSymbolStruct( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioSymbolStruct();
};

inline ioSymbolStruct* ToSymbolStruct( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_SYMBOL )
		return NULL;

	return dynamic_cast< ioSymbolStruct* >( pPlay );
}

inline const ioSymbolStruct* ToSymbolStructConst( ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_SYMBOL )
		return NULL;

	return dynamic_cast< const ioSymbolStruct* >( pPlay );
}

