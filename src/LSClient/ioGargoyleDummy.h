#pragma once
class ioGargoyleDummy : public ioDummyChar
{
protected:
	enum GargoyleDummyState
	{
		GDS_START,
		GDS_MOVE,
		GDS_CHARGE,
		GDS_CHARGE_MOVE,
		GDS_ATTACK,
		GDS_CHARGE_ATTACK,
		GDS_JUMP,
	};
protected:
	GargoyleDummyState	m_GargoyleDummyState;

	ioHashString		m_szGargoyleMoveAni;
	float				m_fGargoyleMoveAniRate;

	ioHashString		m_szGargoyleChargeAni;
	float				m_fGargoyleChargeAniRate;

	ioHashString		m_szGargoyleChargeMoveAni;
	float				m_fGargoyleChargeMoveAniRate;

	ioHashString		m_szGargoyleAttackAni;
	float				m_fGargoyleAttackAniRate;
	D3DXVECTOR3			m_vGargoyleAttackOffset;

	ioHashString		m_szGargoyleChargeAttackAni;
	float				m_fGargoyleChargeAttackAniRate;
	D3DXVECTOR3			m_vGargoyleChargeAttackOffset;

	ioHashString		m_szGargoyleJumpAni;
	float				m_fGargoyleJumpAniRate;

	ioHashString		m_szTargetName;

	DWORD				m_dwFireStartTime;

	WeaponInfo			m_GargoyleAttackInfo;
	WeaponInfo			m_GargoyleChargeAttackInfo;
protected:
	void SetLoopAni( const ioHashString& rkszAni, const float& rkfAniRate);
	DWORD SetActionAni( const ioHashString& rkszAni, const float& rkfAniRate);

	void ProcessAttackState();
	void ProcessChargeAttackState();
public:
	void SetGargoyleMoveState( ioBaseChar *pOwner );
	void SetGargoyleChargeState( ioBaseChar *pOwner );
	void SetGargoyleChargeMoveState( ioBaseChar *pOwner );
	DWORD SetGargoyleAttackState( ioBaseChar *pOwner );
	DWORD SetGargoyleChargeAttackState( ioBaseChar *pOwner, const ioHashString& szTarget );
	DWORD SetGargoyleJumpState( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );
public:
	ioGargoyleDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioGargoyleDummy();
};

inline ioGargoyleDummy* ToGargoyleDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_GARGOYLE_DUMMY )
		return NULL;

	return dynamic_cast< ioGargoyleDummy* >( pDummy );
}