#pragma once

#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioDefenseJump : public ioExtendJump
{
protected:
	float	m_fDefenseEnableHeight;
	bool	m_bUsedDefense;

	float	m_fDefenseAniRate;
	DWORD	m_dwDefenseDuration;

	DWORD m_dwDefenseEnableTime;
	DWORD m_dwDefenseReleaseTime;

	ioHashString m_JumpDefenseAnimation;
	ioHashString m_JumpAniDefenseWounded;
	ioHashString m_JumpAniDefenseCriticalWounded;

	ioHashString m_DefenseLandAni;
	float m_fDefenseLandAniRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual ioHashString GetJumpAniDefenseWounded();
	virtual ioHashString GetJumpAniDefenseCriticalWounded();

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

public:
	inline const ioHashString& GetJumpDefenseLandAni() const { return m_DefenseLandAni; }
	inline float GetJumpDefenseLandAniRate() const { return m_fDefenseLandAniRate; }

protected:
	bool IsCanDefense( ioBaseChar *pOwner, float fHeightGap );

	void SetDefense( ioBaseChar *pOwner );
	bool CheckDefenseRelease( ioBaseChar *pOwner );
	void ReleaseDefense( ioBaseChar *pOwner );

public:
	ioDefenseJump();
	ioDefenseJump( const ioDefenseJump &rhs );
	virtual ~ioDefenseJump();
};

inline ioDefenseJump* ToDefenseJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_DEFENSE_ATTACK )
		return NULL;

	return dynamic_cast< ioDefenseJump* >( pJump );
}

