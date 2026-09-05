#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioPlayStage;
class ioINILoader;

class ioStateProtectionByPassiveBuff : public ioBuff
{
public:
	// 숫자가 높을수록 우선권을 가진다
	enum HPProtectType
	{
		HPT_ZERO_CANCEL		= 1,
		HPT_HP_NO_CANCEL	= 2,
	};

	struct ActiveCountInfo
	{
		float fMinRate;
		float fMaxRate;
		int nActiveCnt;
		ioHashString szEffect;

		ActiveCountInfo()
		{
			fMinRate = fMaxRate = 0.0f;
			nActiveCnt = 0;
			szEffect.Clear();
		}
	};

	typedef std::vector<ActiveCountInfo> ACList;

protected:
	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;
	bool m_bEnableOutLine;

	CEncrypt<float> m_fDamageRate;
	CEncrypt<bool> m_bNoBlockState;
	CEncrypt<bool> m_bNoDefenseBreakState;

	HPProtectType m_HPCancelType;

	AttackSide m_ProtectSide;

	bool m_bDisableDownState;

	ioHashString m_szPassiveSkillName;
	CEncrypt<float> m_fUsePassiveGauge;

	ACList m_ACList;

	ioHashString m_szWoundEffect;

	UniqueObjID m_dwEffectID;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void CheckActiveCount();

	float GetProtectionRate();
	inline const bool IsNoBlockState() const { return m_bNoBlockState; }
	inline const bool IsNoDefenseBreakState() const { return m_bNoDefenseBreakState; }
	inline AttackSide GetProtectSide() { return m_ProtectSide; }
	inline int GetHPCancelType() const { return m_HPCancelType; }

	bool IsProtectState() const;
	
protected:
	void CheckPassiveGauge( ioBaseChar *pOwner );
	void SetEffect( ioBaseChar *pOwner, ioHashString szEffectName );

public:
	ioStateProtectionByPassiveBuff();
	ioStateProtectionByPassiveBuff( const ioStateProtectionByPassiveBuff &rhs );
	virtual ~ioStateProtectionByPassiveBuff();
};

inline ioStateProtectionByPassiveBuff* ToStateProtectByPassiveBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_STATE_PROTECTION_BY_PASSIVE )
		return NULL;

	return dynamic_cast< ioStateProtectionByPassiveBuff* >( pBuff );
}

