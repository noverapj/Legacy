#pragma once
#include "ioSpecialStateBase.h"

typedef std::vector<float> LevelTimeRate;

class ioVulcanItem : public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_CHARGE_ATTACK_START,
		CS_CHARGE_ATTACK_LOOP,
		CS_CHARGE_ATTACK_END,
	};
	enum SyncType
	{
		ST_NONE,
		ST_NORMAL_ATTACK,
		ST_CHARGE_ATTACK_START,
		ST_CHARGE_ATTACK_LOOP,
		ST_CHARGE_ATTACK_END,
		ST_DIR,
		ST_RELOAD,
	};

protected:
	ChargeState						m_ChargeState;

	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;
	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	DWORD m_dwFireTime;
	DWORD m_dwFireGapTime;
	int m_iFireCount;

	DWORD m_dwFireGapMaxTime;
	DWORD m_dwFireGapMinTime;
	DWORD m_dwFireGapReduceTime;
	int m_iFireGapReduceCount;

	//Charge
	WeaponInfo						m_ChargeAttackInfo;
	D3DXVECTOR3						m_vChargeAttackOffset;
	ioHashString m_ChargeAttackSoundName;

	float							m_fVertUpLimitAngle;
	float							m_fVertDownLimitAngle;
	DWORD							m_dwVertRotateWeight;
	DWORD							m_dwHorzRotateWeight;
	D3DXVECTOR3						m_vAttackDir;
	float							m_fCurrMotionRate;
	ioUserKeyInput::DirKeyInput		m_CurDirKey;

	AttackAttribute	m_ChargeAttackStart;

	ioHashString	m_szChargeAttackCenterAni;
	ioHashString	m_szChargeAttackUpAni;
	ioHashString	m_szChargeAttackDownAni;

	ioHashString	m_szChargeAttackEndAni;
	float			m_fChargeAttackEndAniRate;

	//CEncrypt<int> m_iMaxBullet;
	//CEncrypt<int> m_iCurBullet;
	int m_iMaxBullet;
	int m_iCurBullet;
	CEncrypt<int> m_iNeedBullet;

	ioHashString m_ReloadAnimation;
	ioHashString m_ReloadMoveAnimation;
	ioHashString m_ReloadEffect;

	CEncrypt<float> m_fReloadTimeRate;
	float m_fReloadTimeModifyRate;
	bool m_bSetReloadMoveAni;

	LevelTimeRate m_LevelTimeRate;
	CEncrypt<DWORD> m_dwReloadStartTime;
	CEncrypt<DWORD> m_dwReloadEndTime;
	CEncrypt<DWORD> m_dwNextReloadTime;
	CEncrypt<DWORD> m_dwReloadGapTime;

	ioHashString m_szAimMoveBuffName;
	bool m_bFirstAimMoveMotion;
	bool m_bAimMotionSetted;
	bool m_bAimMove;
	float m_fAimMoveAniRate;
	float m_fAimMoveSpeedRate;
	ioUserKeyInput::DirKeyInput m_AimStartDirKey;

	ioHashString m_AimMoveFront;
	ioHashString m_AimMoveBack;
	ioHashString m_AimMoveLeft;
	ioHashString m_AimMoveRight;

protected:
	void ClearData();

	void ChangeToNormalAttack( ioBaseChar *pOwner );	
	void ChangeToChargeAttackLoop( ioBaseChar *pOwner );
	void ChangeToChargeAttackEnd( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessChargeAttackStart( ioBaseChar *pOwner );
	void ProcessChargeAttackLoop( ioBaseChar *pOwner );
	void ProcessChargeAttackEnd( ioBaseChar *pOwner );

	void ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, ioUserKeyInput::DirKeyInput eDir, bool bLeft );
	bool ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, bool bUp );
	void ProcessRotateAnimation( ioBaseChar *pOwner );
	void CheckAimMoveAniState( ioBaseChar *pOwner );
	void ProcessWeapon( ioBaseChar *pOwner );
	void FireWeapon( ioBaseChar *pOwner );

	bool CheckDirectionKey( ioBaseChar* pOwner );

	bool IsLimitAngle( float fLimitAngle, float fCurrAngle );
	bool IsLeftRotate( ioBaseChar* pOwner, ioUserKeyInput::DirKeyInput eNewDirKey );

public:
	void WasteBullet();
	void ChangeToChargeAttackStart( ioBaseChar *pOwner );

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }
	virtual void SetCurBullet( int iCurBullet );

	virtual bool CheckExtendDefenseEnable( ioBaseChar *pChar );
	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void OnEndExtendDefense( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );
	virtual float GetAttackSpeedRate() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	ioVulcanItem(void);
	ioVulcanItem( const ioVulcanItem &rhs );
	~ioVulcanItem(void);
};

inline ioVulcanItem * ToVulcanItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_VULCAN_ITEM )
		return NULL;

	return dynamic_cast< ioVulcanItem * >( pItem );
}