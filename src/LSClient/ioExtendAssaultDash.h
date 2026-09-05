#pragma once


#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioExtendAssaultDash : public ioExtendDash
{
protected:
	enum SubType
	{
		ST_Attack,
		ST_Repeat,
	};

protected:
	AttackAttribute m_AttackAttribute;
	AttributeList m_ExtraAttackList;

	int m_iCurExtendCnt;

	DWORD m_dwActionEndTime;

	bool m_bSetExtendAssault;

	AttackAttribute m_RepeatDashInfo;
	int m_nMaxRepeatDashCnt;
	bool m_bSetEnableDash;

protected:
	// For Ani Rotate
	vAniRotateInfoList m_vAniRotateInfoList;

	int m_iCurAniRotate;
	DWORD m_dwAniRotateTime;
	float m_fAniRotateAngle;

	bool m_bEnableDashState;
	int m_nCurRepeatDashCnt;

	bool m_bEnableAttackAfterDash;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );

	void SetExtendAssaultDash( ioBaseChar *pOwner );
	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void CheckAniRotate( ioBaseChar *pChar );

public:
	void SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );

protected:
	virtual void CheckKeyReserve( ioBaseChar *pOwner );
	void CheckExtendReserve( ioBaseChar *pOwner );

protected:
	void SetRepeatDash( ioBaseChar *pOwner );

public:
	bool IsDashEnable( ioBaseChar *pOwner );

	AttackAttribute& GetDashAttackAttribute();

public:
	ioExtendAssaultDash();
	ioExtendAssaultDash( const ioExtendAssaultDash &rhs );
	virtual ~ioExtendAssaultDash();
};

inline ioExtendAssaultDash* ToExtendAssaultDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_EXTEND_ASSAULT )
		return NULL;

	return dynamic_cast< ioExtendAssaultDash* >( pDash );
}

