#pragma once

#include "ioWeapon.h"
#include "ioUserKeyInput.h"

class ioEffect;

class ioSelectPositionMoveWeapon : public ioWeapon
{
public:
	enum WeaponState
	{
		WS_PRE,
		WS_FLOATING,
		WS_WAITING,
		WS_TRACING,
		WS_WAIT_END,
		WS_END,
	};

	enum WeaponStyle
	{
		WS_DEFALT,
		WS_SPACE_MASTER,
	};
	
protected:
	WeaponState		m_State;
	WeaponStyle		m_WeaponStyle;

	ioEffect*		m_pMesh;
	ioOrientBox		m_LocalAttackBox;	
	ioHashString	m_szBoomEffect;
	ioHashString	m_szMapCollisionEffect;

	D3DXVECTOR3		m_vStartPos;
	D3DXVECTOR3		m_vMoveDir;
	D3DXVECTOR3		m_vTracePos;

	CEncrypt<DWORD>	m_dwMapCollisionDelayTime;
	CEncrypt<DWORD>	m_dwCurMapCollisionDelayTime;

	CEncrypt<bool>	m_bDisableUpDown;
	CEncrypt<bool>	m_bPlayWoundSound;
	CEncrypt<bool>	m_bDisableMapCollision;
	CEncrypt<bool>	m_bDisableMove;
	ioHashString	m_bMoveControlSkillName;

	float			m_fMoveRangeBySkill;
	CEncrypt<float> m_fTraceCollisionRange;
	CEncrypt<float>	m_fCurMoveRange;
	CEncrypt<float> m_fTraceMoveSpeed;
	float			m_fCurMoveAmount;
	float			m_fTotalMoveAmount;

	CEncrypt<DWORD> m_dwWaitEndDuration;
	CEncrypt<DWORD> m_dwWaitEndEndTime;

	ioHashString	m_TargetBuff;
	ioHashStringVec	m_TargetNameList;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	inline int GetWeaponStyle(){ return m_WeaponStyle; }
	inline void SetDisableMove( bool bDisable ){ m_bDisableMove = bDisable; }
	inline void SetControlSkillName( const ioHashString& szName ){ m_bMoveControlSkillName = szName; }
	inline void SetCurrMoveRange( float fRange ){ m_fCurMoveRange = fRange; }
	inline void SetMoveRangeBySkill( float fMoveRange ){ m_fMoveRangeBySkill = fMoveRange; }

protected:
	bool SetMesh( const ioHashString &szFileName );
	void OnFloating( float fTimePerSec, DWORD dwCurTime, ioPlayStage *pStage );
	void OnWaiting( float fTimePerSec );
	void OnTracing( float fTimePerSec, DWORD dwCurTime, ioPlayStage *pStage );
	void OnWaitEnd( float fTimePerSec, DWORD dwCurTime );

	float GetMoveSpeed();
	float GetMoveRange();
	
	void CheckMoveKey( float fTimePerSec, DWORD dwCurTime );
	void CheckMoveDir();

	void SetWaitEndState( DWORD dwCurTime );
	void CreateMapCollisionEffect( ioPlayStage* pStage );

protected:
	void PorcessTargetMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void RemoveTargetBuff( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	void SetTracePosition( const D3DXVECTOR3 &vPos );

	bool isWeaponEnd();
public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void PlayWoundedSound( ioPlayEntity *pEntity );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	
public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual void SetWeaponDead();

public:
	inline void SetTargetList( const ioHashStringVec& TargetList ){ m_TargetNameList = TargetList; }

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioSelectPositionMoveWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioSelectPositionMoveWeapon();
};

inline ioSelectPositionMoveWeapon* ToSelectPositionMoveWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_SELECT_POSITION_MOVE )
		return NULL;

	return dynamic_cast< ioSelectPositionMoveWeapon* >( pWeapon );
}

