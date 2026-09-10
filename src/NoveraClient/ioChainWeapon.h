#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioChainWeapon : public ioWeapon
{
public:
	enum ChainState
	{
		CWS_NONE,
		CWS_MOVE,
		CWS_COLLISION,
		CWS_WAIT,
	};

	struct ChainInfo
	{
		float m_fSpeed;
		float m_fRange;

		ChainInfo()
		{
			m_fRange = 0.0f;
			m_fSpeed = 0.0f;
		}
	};

protected:
	ioEffect *m_pFireBall;

	ChainState m_ChainState;
	float	m_fCurMoveSpeed;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;

	CEncrypt<int> m_iMaxChainCnt;
	CEncrypt<int> m_iCurChainCnt;
	CEncrypt<float> m_fCurChainRange;
	CEncrypt<float> m_fChainAngle;
	CEncrypt<bool> m_bDisableDownTarget;
	
	IntVec	m_iIgnoreCountList;
	CEncrypt<float>	m_fLimitLengthToOwner;
	CEncrypt<bool>	m_bWeaponInvisible;

	float m_fCurMoveDistance;
	DWORD m_dwCollisionTime;

	typedef std::vector<ChainInfo> ChainInfoList;
	ChainInfoList m_vChainInfoList;

	bool m_bFirst;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void ChangeChainWeapon( int iCurCnt, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vMoveDir );

	float GetNextChainRange();

	void SetWaitState();

	inline int GetCurChainCnt() const { return m_iCurChainCnt; }
	inline int GetMaxChainCnt() const { return m_iMaxChainCnt; }

	inline float GetChainAngle() const { return m_fChainAngle; }

	inline bool IsDisableDownTarget() const { return m_bDisableDownTarget; }

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	float GetMoveSpeed();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual D3DXVECTOR3 GetMoveDir();

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioChainWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioChainWeapon();
};

inline ioChainWeapon* ToChainWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_CHAIN )
		return NULL;

	return dynamic_cast< ioChainWeapon* >( pWeapon );
}

