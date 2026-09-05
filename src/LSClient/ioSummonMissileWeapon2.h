#pragma once


#include "ioWeapon.h"
#include "IORandom.h"

class ioEffect;

class ioSummonMissileWeapon2 : public ioWeapon
{
public:
	enum IceState
	{
		IS_MOVE,
		IS_EXPLOSION_WAIT,
		IS_EXPLOSION,
	};

	enum StateSync
	{
		SSC_EXPLOER,
		SSC_CHANGE_DIR,
	};

protected:
	IceState m_IceState;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;

	ioEffect *m_pIceMesh;

	ioHashString m_RedTeamEffect;
	ioHashString m_BlueTeamEffect;

	ioHashString m_BoomEffect;

	bool m_bFirst;
	bool m_bEndExplosion;

	ChangeDirInfoList m_ChangeDirInfoList;

	D3DXVECTOR3 m_vPrePos;
	float m_fDefaultGapHeight;
	float m_fEnableMoveGap;

	OwnerRotType m_OwnerRotate;
	float m_fMinMoveSpeed;
	float m_fMaxMoveSpeed;

	DWORD m_dwMinRotateTime;
	DWORD m_dwMaxRotateTime;

	float m_fCurMoveSpeed;
	float m_fCurMoveRange;
	DWORD m_dwCurRotateTime;

	float m_fRotateStartRange;
	float m_fRotateEndRange;

	IORandom m_Random;

	ioUserKeyInput::DirKeyInput m_eDirKey;
	int m_nMaxChangeDirCnt;
	int m_nCurChangeDirCnt;
	int m_OwnerState;

	ioHashString m_stReflectEffect;
	ioHashString m_szReserveSound;

	float		 m_fRotateDirAddRange;
	bool		m_bCheckOwnerState;
	bool m_bUseItemGauge;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetIceMesh( const ioHashString &szFileName );
	void OnFloating( float fTimePerSec, ioPlayStage *pStage );

	bool CheckCollisionLine( ioPlayStage *pStage );
	void CheckMoveKey( ioBaseChar* pOwner );

	void CreateMapEffect( ioBaseChar* pOwner, const D3DXVECTOR3& vPos, const D3DXQUATERNION& qt );

public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();
	DWORD GetRotateSpeed();

	bool IsFloating() const;
	bool IsExplosion() const;

	void SetExplosionWait();
	void SetExplosion( ioPlayStage *pStage, bool bSendNet );
	void Explosion( ioPlayStage *pStage );

	void CheckOwnerState( ioBaseChar *pOnwer );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual D3DXVECTOR3 GetMoveDir();

	virtual void SetTeam( ioPlayStage *pStage, TeamType eType );
	virtual void SetWeaponIndex( DWORD dwIndex );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual void SetOwnerChargeScaleRate( float fRate );
	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioSummonMissileWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioSummonMissileWeapon2();
};

inline ioSummonMissileWeapon2* ToSummonMissileWeapon2( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_SUMMON_MISSILE2 )
		return NULL;

	return dynamic_cast< ioSummonMissileWeapon2* >( pWeapon );
}