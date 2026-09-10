#pragma once


#include "ioWeapon.h"
#include "ioUserKeyInput.h"

class ioEffect;

class ioFireWorksStep1Weapon : public ioWeapon
{
public:
	enum DoubleIceState
	{
		DIS_FLOATING,
		DIS_EXPLOSION,
		DIS_EXPLOSION_END,
	};

	enum StateSync
	{
		WSC_ORDER_EXPLOSION,
	};

	//movedir이 수직 방향이 아닌 대각선 방향일 경우 weapon이 찌그러지는 현상이 발생
	//이 현상을 막기위해 type 설정.
	enum Orientation_State
	{
		OS_NONE,
		OS_ORIENTATION,
	};

protected:
	DoubleIceState m_IceState;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;

	ioEffect *m_pIceCoreMesh;
	ioHashString m_BoomEffect;
	float m_fExplosionRange;

	bool m_bFirst;
	float m_fCurMoveSpeed;

	bool m_bCollision;
	bool m_bArleadyCallWeapon;
	bool m_bCallWeaponOnEnd;

	CEncrypt<float> m_fTotalMoveAmount;	
	float	m_fMaxMoveLenght;
	float   m_fAngleV;
	float	m_fAngleH;

	WeaponInfo	m_CallWeaponInfo;
	ChangeDirInfoList m_CallAttFireList;

	CEncrypt<bool>	m_bPlayWoundSound;
	CEncrypt<bool>	m_bDisableMapCollision;

	bool m_bSetCallAxisChange;

	Orientation_State m_OrientationState;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	bool IsFloating() const;
	bool IsExplosion() const;

	void Explosion( ioPlayStage *pStage );

	void SetCallAxisChange( bool bSetCallAxisChange );

protected:

	bool SetIceCoreMesh( const ioHashString &szFileName );
	void OnFloating( float fTimePerSec, ioPlayStage *pStage );

	float GetMoveSpeed();
	float GetMaxRange();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	void LoadCallAttribute( ioINILoader &rkLoader );
	void LoadCallAttributeFireDirectionList( ioINILoader &rkLoader );

	void CallAttribute();

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void PlayWoundedSound( ioPlayEntity *pEntity );
	virtual bool IsCanMineExplosion();

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

protected:

	bool CheckMoveRange( ioBaseChar* pOwner, ioPlayStage *pStage, float fTimePerSec );

public:

	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	virtual void SetLiveTime( DWORD dwTime );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioFireWorksStep1Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioFireWorksStep1Weapon();
};

inline ioFireWorksStep1Weapon* ToFireWorksStep1Waepon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_FIRE_WORKS_STEP1_WEAPON )
		return NULL;

	return dynamic_cast< ioFireWorksStep1Weapon* >( pWeapon );
}

