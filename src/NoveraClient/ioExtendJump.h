#pragma once



class ioBaseChar;
class SP2Packet;
class ioINILoader;

class ioExtendJump
{
protected:
	float m_fJumpNeedGauge;
	float m_fJumpExtraGauge;

	CEncrypt<float> m_fJumpSpeedRate;

	bool m_bUseExtendJump;
	bool m_bEnableLandAfterState;
	bool m_bEnableLandAfterAttackByKeyDown;

	ioHashString m_LandAfterFailAni;
	float m_fLandAfterFailAniRate;
	ioHashString m_LandAfterSuccessAni;
	float m_fLandAfterSuccessAniRate;

	JumpGrapplingInfo m_JumpGrapplingInfo;
	JumpGrappingWoundInfo m_JumpGrapplingWoundInfo;

	//CCAI¨¡©¡¢Ò¡× AO¡Æ¢®
	bool m_bUseJumpDashAndLandDash;
	ioUserKeyInput::DirKeyInput m_JumpDashDownKey;

	bool m_bEnableDashEndJump;
	float m_fDashEndJumpPower;

	DWORD m_dwReserveEnableTime;

// For Air Jump
protected:
	int m_iCurAirJump;
	std::list<float> m_vAirJumpTimeList;

	float m_fAirJumpPower;
	float m_fAirForcePower;
	float m_fAirJumpPowerRate;

	//////////////////////////////////////////////////////////////////////////
	vAniRotateInfoList m_vAniRotateInfoList;

	int m_iCurAniRotate;
	DWORD m_dwAniRotateTime;
	float m_fAniRotateAngle;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone() = 0;
	virtual JumpType GetType() const = 0;

public:
	virtual void InitJump();
	virtual void SetJumpState();
	virtual void ProcessJumpReady( ioBaseChar *pOwner );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void SetJumpAttack( ioBaseChar *pOwner, int iAniIDm, float fTimeRate );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessLanding( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual bool ProcessAimJump( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );
	virtual ioHashString GetJumpAniDefenseWounded();
	virtual ioHashString GetJumpAniDefenseCriticalWounded();

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );
	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;

	virtual void SetLandingState( ioBaseChar *pOwner );

	virtual bool IsCanJumpAttack( ioBaseChar *pOwner );
	virtual bool IsEnableLandAfterState();
	virtual bool IsEnableJumpLandEndDash();
	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	virtual bool IsEnableLandAfterAttackByKeyDown();

	virtual const ioHashString& GetLandAfterAni( bool bSuccess );
	virtual float GetLandAfterAniRate( bool bSuccess );

	virtual void SetNextJumpItemState( ioBaseChar *pOwner, ioBaseChar *pWounder, bool bDefense, ChangeNextItemType type, ioWeapon* pWeapon, float fDamage );

	virtual void SetExtraJump( ioBaseChar *pOwner );
	virtual void ProcessExtraJump( ioBaseChar *pOwner );

	void InitAniRotate();
	void SetAniRotate( ioBaseChar *pChar, int iAniID, float fAniRate );
	void CheckAniRotate( ioBaseChar *pChar );

// air_jump
protected:
	virtual void ClearAirJump();
	virtual void SetAirJump( ioBaseChar *pOwner, int iAniID, float fTimeRate );
	virtual void CheckAirJump( ioBaseChar *pOwner );

public:	
	virtual bool CheckCanUseBuffAirDash();

	const JumpGrapplingInfo& GetJumpGrapplingInfo() const { return m_JumpGrapplingInfo; }
	const JumpGrappingWoundInfo& GetJumpGrapplingWoundInfo() const { return m_JumpGrapplingWoundInfo; }

	bool IsEnableJumpDashAndLandDash();
	void SetLandDashKey( ioUserKeyInput::DirKeyInput key ){ m_JumpDashDownKey = key; }
	const ioUserKeyInput::DirKeyInput& GetLandDashKey() const;

	virtual bool IsEnableDashEndJump()	{ return m_bEnableDashEndJump; }
	float GetDashEndJumpPower()			{ return m_fDashEndJumpPower; }

protected:
	virtual float CheckKeyReserveTime( ioBaseChar *pOwner, int iAniID, float fTimeRate, DWORD dwPredelay=0 );
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	virtual bool CheckUseExtendJump( ioBaseChar* pOwner );
	virtual bool IsBuffFlyChange( ioBaseChar* pOwner );

public:
	virtual bool IsNoProcessMove();

public:
	ioExtendJump();
	ioExtendJump( const ioExtendJump &rhs );
	virtual ~ioExtendJump();
};

