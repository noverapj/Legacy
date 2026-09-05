#pragma once


#include "ioWeapon.h"

class ioEffect;
class ioUroborusGuideArrow;

class ioReturnMissileWeapon : public ioWeapon
{
public:
	enum WeaponState
	{
		WS_NONE,
		WS_NORMAL,
		WS_RETURN,
	};

	enum ReturnType
	{
		RT_NONE,
		RT_RETURN1,
	};

protected:
	ioEffect *m_pFireBall;

	WeaponState m_State;

	float m_fCurMoveSpeed;
	float m_fCurMoveRange;
	float m_fCurMaxMoveRange;
	float m_fCurMoveRate;

	D3DXVECTOR3 m_vStartOffSet;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;
	ChangeDirInfoList m_ChangeDirInfoList;

	bool	m_bSetGraceClose;
	bool	m_bFirst;
	bool m_bDisableOwnerScale;

	WeaponInfo	m_CallWeaponInfo;
	ChangeDirInfoList m_CallAttFireList;
	bool m_bArleadyCallWeapon;

	bool			m_bUseLengthMove;
	CEncrypt<float> m_fUseMaxLenthValue;
	bool			m_bNotUseBipeLink;
	bool			m_bUseEffectStartPosMode;

protected:
	CEncrypt<int> m_iReturnAttribute;
	const WeaponAttribute *m_pReturnAttr;

	ioUroborusGuideArrow *m_pGuideArrow;

	ioHashString m_ReturnEffect;
	CEncrypt<int> m_iReturnType;

	CEncrypt<float> m_fReturnMoveSpeed;

	BuffInfoList m_ReturnTargetAirBuffList;
	BuffInfoList m_ReturnTargetBuffList;
	BuffInfoList m_ReturnTargetDownBuffList;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();

	bool ChangeWeaponEffect( const ioHashString &szEffectName );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void SetGraceClose();

	void ProcessGuidArrow();

	void SetGuidArrowPosMode();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetMoveSpeed( float fSpeed );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual const BuffInfoList& GetTargetAirBuffList() const;
	virtual const BuffInfoList& GetTargetBuffList() const;
	virtual const BuffInfoList& GetTargetDownBuffList() const;

	virtual void SetWeaponDead();

public:
	void LoadCallAttribute( ioINILoader &rkLoader );
	void LoadCallAttributeFireDirectionList( ioINILoader &rkLoader );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual void SetSkillDependencyAction();

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

	void ProcessNormal( float fTimePerSec, ioPlayStage *pStage );
	void ProcessReturn( float fTimePerSec, ioPlayStage *pStage );

public:
	bool isCallAttribute();
	void CallAttribute( ioPlayEntity *pEntity );

	void ChangeToReturnState();

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioReturnMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioReturnMissileWeapon();
};

inline ioReturnMissileWeapon* ToReturnMissileWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_RETURN_MISSILE_WEAPON )
		return NULL;

	return dynamic_cast< ioReturnMissileWeapon* >( pWeapon );
}

