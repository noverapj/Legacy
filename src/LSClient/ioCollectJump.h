#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCollectJump : public ioExtendJump
{
public:
	enum JumpAttackType
	{
		JAT_NORMAL,
		JAT_HIGH,
		JAT_LAND,
	};

protected:
	JumpAttackType m_JumpAttackType;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

	CEncrypt<float> m_fNormalEnableHeight;
	CEncrypt<float> m_fHighEnableHeight;
	CEncrypt<float> m_fLandEnableHeight;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	void GetEnableHeightInfo( OUT float &fNormal, OUT float &fHigh, OUT float &fLand );
	
	void SetJumpAttackType( JumpAttackType eType );
	inline const JumpAttackType GetJumpAttackType() const { return m_JumpAttackType; }
	
	bool SetCollectJumpAttackState( ioBaseChar *pOwner );

public:
	ioCollectJump();
	ioCollectJump( const ioCollectJump &rhs );
	virtual ~ioCollectJump();
};

inline ioCollectJump* ToCollectJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_COLLECT_JUMP )
		return NULL;

	return dynamic_cast< ioCollectJump* >( pJump );
}

