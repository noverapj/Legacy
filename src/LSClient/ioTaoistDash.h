#pragma once
#include "ioextenddash.h"


class ioTaoistDash :	public ioExtendDash
{
protected:
	enum	TaoistDashState
	{
		TDS_NONE,
		TDS_CHARGE,
		TDS_NORMAL_ATTACK,
		TDS_COMBO_ATTACK,
		TDS_CHARGE_ATTACK
	};

protected:
	TaoistDashState	m_TaoistDashState;

	// normal attack
	int	m_iCurComboCnt;
	bool	m_bSetCombo;
	DWORD	m_dwActionStartTime;
	DWORD	m_dwActionEndTime;


	// charge
	DWORD	m_dwChargingStartTime;
	DWORD	m_dwChargingTime;
	ioHashStringVec	m_ChargingAniList;
	AttackAttribute	m_ChargingAttack;

	// cancel dash
	ioHashString	m_szInputEventAnimation;
	float	m_fInputEventAnimationRate;
	ioUserKeyInput::DirKeyInput	m_eSpecialDashCurrDirkey;
	DWORD	m_dwCancelCheckTime;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );
	virtual	DashType	GetType() const;
	virtual	ioExtendDash*	Clone();

	virtual	bool	StartDash( ioBaseChar *pOwner );
	virtual	void	ProcessDash( ioBaseChar *pOwner );
	virtual	void	ExtendDashEnd( ioBaseChar *pOwner );

	virtual	void	ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual	void	CheckKeyReserve( ioBaseChar *pOwner );

protected:
	void	ChangeToCharging( ioBaseChar* pOwner );
	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ChangeToComboAttack( ioBaseChar* pOwner );
	void	ChangeToChargingAttack( ioBaseChar* pOwner );

	void	ProcessCharging( ioBaseChar* pOwner );
	void	ProcessNormalAttack( ioBaseChar* pOwner );
	void	ProcessComboAttack( ioBaseChar* pOwner );
	void	ProcessChargingAttack( ioBaseChar* pOwner );

	bool	CheckAniJump_Input_Jump_S( ioBaseChar *pOwner );
	bool	CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner );
	void	CheckCancelReserveByConditionalSpecialDash( ioBaseChar *pOwner );

protected:
	void	CheckExtraReserve( ioBaseChar *pOwner );

protected:
	void	Init();

public:
	void	SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );

public:
	ioTaoistDash(void);
	ioTaoistDash( const ioTaoistDash &rhs );
	virtual ~ioTaoistDash(void);
};

inline ioTaoistDash* ToTaoistDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_TAOIST )
		return NULL;

	return dynamic_cast< ioTaoistDash* >( pDash );
}
