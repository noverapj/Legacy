#pragma once
class ioWitchDoctoritem : public ioWeaponItem
{
private:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_CHARGE_ATTACK,
	};

	enum ChargeSyncType
	{
		CST_NORMAL_ATTACK,
		CST_CHARGE_ATTACK,
		CST_CREATE_DUMMY,
		CST_CALL_DUMMY,
		CST_DUMMY_ATTACK,
	};
private:
	ChargeState						m_ChargeState;
	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;
	DWORD							m_dwFireStartTime;
	
	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//Charge Attack
	AttackAttribute					m_ChargeAttack;
	float							m_fDummyCreateEnableRange;
	vDummyCharLoadInfoList			m_DummyCharList;
	float							m_fDummyTeleportOffset;
	float							m_fDummyTeleportSideOffset;

private:
	void ClearData();

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToChargeAttackState( ioBaseChar *pOwner );
	
	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessChargeAttack( ioBaseChar *pOwner );

	void CheckDummy( ioBaseChar *pOwner );
	void DestroyAllDummy( ioBaseChar *pOwner );

	ioDummyChar* CreateDummyChar( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashString szDummyCharName, 
									int iDummyIndex, D3DXVECTOR3 vPos, float fStartAngle = 0 );
public:
	void CallDummyChar( ioBaseChar *pOwner );
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
public:
	ioWitchDoctoritem(void);
	ioWitchDoctoritem( const ioWitchDoctoritem &rhs );
	~ioWitchDoctoritem(void);
};

inline ioWitchDoctoritem* ToWitchDoctoritem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_WITCH_DOCTOR )
		return NULL;

	return dynamic_cast< ioWitchDoctoritem* >( pItem );
}