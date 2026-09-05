#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

struct SecondEffect
{
	ioHashString m_szSecondEffect;
	DWORD		 m_dwSecondEffectID;
	SecondEffect()
	{
		m_szSecondEffect.Clear();
		m_dwSecondEffectID = -1;
	}
};

class ioReturnPingpongWeapon : public ioWeapon
{
protected:
	ioEffect *m_pFireMesh;

	D3DXVECTOR3 m_vPrePos;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vAttackDir;
	D3DXVECTOR3 m_vRollAxis;
	ioOrientBox m_LocalAttackBox;

	bool m_bCheckCollision;
	int m_iCollisionCnt;

	CEncrypt<float>	m_fReturnHeightRate;

	CEncrypt<int>	m_iTotalPingpongCount;
	int				m_iCurPingpongcount;
	CEncrypt<DWORD>	m_dwEnablePingpongTime;

	const WeaponAttribute *m_pSecondAttr;
	SecondAttributeList m_vSecondAttrList;

	bool			m_bArleadyCallWeapon;
	WeaponInfoList	m_CallWeaponInfoList;

	CEncrypt<bool>	m_bDisableMaxCount;

	std::vector<SecondEffect>	m_SecontEffectList;

	bool	m_bTimeOver;

protected:
	ChangeDirInfoList m_ChangeDirInfoList;
	
	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;


protected:
	bool SetFireMesh( const ioHashString &szFileName );

public:
	bool SetWallCollision( const D3DXVECTOR3 &vFaceNrm, bool bMap, bool bOnlyAttack );
	D3DXVECTOR3 GetMoveDir();
	void ChangeBallWeapon( int iCurCnt, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vMoveDir, const D3DXVECTOR3 &vAttackDir );
	
	inline int GetCollisionCnt() const { return m_iCollisionCnt; }

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetLiveTime( DWORD dwTime );
	void SetSecondAttribute( SecondAttributeList vSecondList );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void SetWeaponDead();

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

protected:
	void OnMoveState( float fTimePerSec );
	void OnDownState( float fTimePerSec, ioPlayStage *pStage );
	void OnPingPongState( float fTimePerSec, ioPlayStage *pStage );

	void RollWeapon( float fTimePerSec );

protected:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

	float GetMoveSpeed();
	float GetMaxRange();

public:
	void SetReturn( const D3DXVECTOR3 &vPos );
	bool DisableMaxCount() const { return m_bDisableMaxCount; }
	bool EnablePingpong() const;

protected:
	void CallAttribute();
	void ChangeSecondEffect();
	void UpdateSecondEffect( ioPlayStage *pStage );
	void DestroySecondEffect();

public:
	ioReturnPingpongWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioReturnPingpongWeapon();
};

inline ioReturnPingpongWeapon* ToReturnPingpongWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_RETURN_PINGPONG )
		return NULL;

	return dynamic_cast< ioReturnPingpongWeapon* >( pWeapon );
}

