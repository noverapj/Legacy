#pragma once

#include "ioSizeItem.h"
#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioMagneticDash : public ioExtendDash
{
public:
	enum ChargeDashState
	{
		CDS_NONE,
		CDS_CHARGE,
		CDS_NORMAL_ATTACK,
		CDS_COMBO_ATTACK,
		CDS_CHARGING_ATTACK_FIRE,
	};

protected:
	ChargeDashState				m_DashState;				//

	ioHashStringVec				m_ChargingRedNAniList;		// N극(빨강)
	AttackAttribute				m_DashRedNAttack;			// 기본 공격
	AttributeList				m_ExtraDashRedNAttackList;	// 기본 공격 2타, 3타 콤보
	AttackAttribute				m_ExtendChargeRedNAttack;	// 차지 공격

	ioHashStringVec				m_ChargingBlueSAniList;		// S극(파랑)
	AttackAttribute				m_DashBlueSAttack;			// 기본 공격
	AttributeList				m_ExtraDashBlueSAttackList;	// 기본 공격 2타, 3타 콤보
	AttackAttribute				m_ExtendChargeBlueSAttack;

	bool						m_bNormalAttacked;
	bool						m_bSetCombo;
	int							m_iCurComboCnt;
	DWORD						m_dwActionStartTime;
	DWORD						m_dwActionEndTime;
	DWORD						m_dwChargingTime;
	DWORD						m_dwChargingStartTime;
	DWORD						m_dwCancelCheckTime;

	ioHashString				m_szInputEventAnimation;
	float						m_fInputEventAnimationRate;	
	ioUserKeyInput::DirKeyInput	m_eSpecialDashCurrDirkey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );
	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void ExtendDashEnd( ioBaseChar *pOwner );

protected:
	void ChangeToCharging( ioBaseChar *pOwner );
	void ChangeToExtendCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );

	// D~
	void ChangeToAttackFire( ioBaseChar *pOwner );


protected:
	void SetComboDash( ioBaseChar *pOwner, bool bCharge = false );
	void ProcessNormalComboDash( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );

	virtual void CheckKeyReserve( ioBaseChar *pOwner );
	virtual void ProcessKeyReserve( ioBaseChar *pOwner );

	void CheckExtraReserve( ioBaseChar *pOwner );
	void CheckExtendReserve( ioBaseChar *pOwner );
	bool CheckPreReserve( ioBaseChar *pOwner );

	bool CheckAniJump_Input_Jump_S( ioBaseChar *pOwner );
	bool CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner );

public:	// 캔슬 대시
	void CheckCancelReserveByConditionalSpecialDash( ioBaseChar *pOwner );

public:
	ioMagneticDash();
	ioMagneticDash( const ioMagneticDash &rhs );
	virtual ~ioMagneticDash();
};

inline ioMagneticDash* ToMagneticDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_MAGNETIC )
		return NULL;

	return dynamic_cast< ioMagneticDash* >( pDash );
}

