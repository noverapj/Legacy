#pragma once
#include "ioextendjump.h"
#include "ioAttackAttribute.h"

class ioMagneticJump :	public ioExtendJump
{
public:
	enum ProcessJumpState
	{
		PJS_NONE,
		PJS_NORMAL_ATTACK,
		PJS_DASH,
		PJS_CHARGING_ATTACK,
		PJS_CHARGING_MOVE_ATTACK,
	};

	enum ComboType
	{
		CDT_GRAVITY		= 1,
		CDT_NO_GRAVITY	= 2,
	};

	enum SyncState
	{
		SSC_ATTACK,
		SSC_DASH,
		SSC_DASH_END,
		SSC_CHANGE_JUMP_ATTACK,		
		SSC_CHARGING,
		SSC_CHARGING_JUMP_ATTACK,
	};

protected:
	ProcessJumpState	m_JumpState;				// »óÅÂ

	AttributeList		m_ComboRedNAttackList;		// N±Ø(»¡°­) ±âº» °ø°Ý
	AttributeList		m_ComboBlueSAttackList;		// S±Ø(ÆÄ¶û) ±âº» °ø°Ý

	bool				m_bEnableJumpDash;	

	AttackAttribute		m_JumpRedNDash;				// N±Ø(»¡°­) ´ë½Ã 
	AttackAttribute		m_JumpBlueSDash;			// S±Ø(ÆÄ¶û) ´ë½Ã

	ioHashString		m_szChargeRedNAnimation;	// N±Ø(»¡°­) D~
	AttackAttribute		m_JumpChargeRedNAttack;		// D~ N±Ø(»¡°­) °ø°Ý
	AttackAttribute		m_ExtendLandRedNAttack;		// D~ N±Ø(»¡°­) land

	ioHashString		m_szChargeBlueSNAnimation;	// S±Ø(ÆÄ¶û) D~
	AttackAttribute		m_JumpChargeBlueSAttack;	// D~ S±Ø(ÆÄ¶û) °ø°Ý
	AttackAttribute		m_ExtendLandBlueSAttack;	// D~ S±Ø(ÆÄ¶û) land

	DWORD				m_dwJumpChargeTime;			// D~ ½Ã°£ 

	// D~ ÀÌÈÄ ÀÌµ¿ 
	float				m_fChargeBaseRange;
	float				m_fCurCharMoveSpeed;
	float				m_fChargeJumpPower;
	float				m_fChargeGravityRate;
	DWORD				m_dwFlightTime;
	D3DXVECTOR3			m_vCharMoveDir;
		
	DWORD				m_dwEnableComboTime_S;
	DWORD				m_dwEnableComboTime_E;
	DWORD				m_dwDashCheckEnableTime;	// Äµ½½ ´ë½Ã

	float				m_fJumpAttackSpeedRateFR;
	float				m_fJumpAttackSpeedRateSI;
	float				m_fJumpAttackSpeedRateBG;

	float				m_fJumpDashSpeedRateFR;
	float				m_fJumpDashSpeedRateSI;
	float				m_fJumpDashSpeedRateBG;

	// ´ë½Ã ÇÑ¹ø¸¸ °¡´É
	int					m_iUsedDash;
	int					m_iMaxDash;		

	int					m_iMaxJumpCnt;
	int					m_iCurJumpCnt;

	int					m_iCurComboCnt;
	bool				m_bReserveJumpAttack;

	// Áß·Â
	ComboType			m_ComboDashType;
	ComboType			m_ComboChargeType;

	DWORD				m_dwChargingStartTime;

	float				m_fJumpDashEndJumpPower;
	float				m_fJumpDashEndLandingRate;


	float				m_fComboDashGravityAmt;
	DWORD				m_dwComboDashStartTime;
	DWORD				m_dwComboDashEndTime;
	DWORD				m_dwDashAttackEndTime;

	bool				m_bUsedChargeAttack;

public:
	virtual	void	LoadProperty( ioINILoader& rkLoader );
	virtual	ioExtendJump*	Clone();
	virtual	JumpType	GetType() const;

public:
	virtual	void	SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void	ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge );
	virtual void	ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void	ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void	InitJump();

	virtual void	CheckComboJumpAttack( ioBaseChar *pOwner );
	virtual bool	IsCanJumpingSkill( ioBaseChar *pOwner );
	virtual void	SetLandingState( ioBaseChar *pOwner );
	virtual bool	CheckUseExtendJump( ioBaseChar *pOwner );
	virtual bool	IsEnableJumpDash( ioBaseChar *pOwner );
	virtual float	GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual bool	IsNoProcessMove();

public:
	void	ProcessComboDash( ioBaseChar *pOwner );
	float	GetCurGravityAmt();
	bool	IsJumpAttackState();
	void	ClearState( ioBaseChar* pOwner );
	bool	IsChargeAttackState();
	void	ClearState( ioBaseChar *pOwner, int eNewState );
	bool	IsChargeAttacked();

	// +D
protected:
	void	SetComboJumpAttack( ioBaseChar* pOwner );
	void	ProcessComboJumpAttack( ioBaseChar *pOwner );

	void	ChangeToJumpAttack( ioBaseChar* pOwner );

	bool	IsCanJumpDash( ioBaseChar* pOwner );
	void	SetJumpDash( ioBaseChar* pOwner );

	// D~
	void	ChangeToChargeState( ioBaseChar* pOwner );
	void	SetChargeJumpAttack( ioBaseChar* pOwner );
	void	PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage );
	float	GetFlightDuration( float fCurGravity );

public:
	const ioHashString& GetChargeLandAni(ioBaseChar *pOwner) const;
	float GetChargeLandAniRate(ioBaseChar *pOwner) const;
	const AttackAttribute& GetLandAttackAttribute(ioAttackableItem *pItem) const;

public:
	ioMagneticJump(void);
	ioMagneticJump( const ioMagneticJump& rhs );
	virtual ~ioMagneticJump(void);
};

inline ioMagneticJump* ToMagneticJump( ioExtendJump* pJump )
{
	if( !pJump || pJump->GetType() != JT_MAGNETIC_JUMP )
		return NULL;

	return dynamic_cast< ioMagneticJump* >( pJump );
}