#pragma once

#include "ioModeItem.h"

class SP2Packet;
class ioMapEffect;
class ioPlayStage;

class ioHunterCoinModeItem : public ioModeItem
{
protected:
	enum ItemState
	{
		IS_WAIT,
		IS_DELAY,
		IS_MOVE,
		IS_PINGPONG,
	};

protected:
	ItemState m_State;

	ioHashString m_LoopEffectName;
	UniqueObjID m_dwLoopEffectID;

	D3DXVECTOR3 m_vMoveDirection;

	float m_fMoveSpeed;
	float m_fFloatingPower;

	float m_fCurMoveSpeed;
	float m_fCurFloatingPower;
	int m_iCurPingPongCnt;

	DWORD m_dwActivateTime;
	DWORD m_dwCollisionTime;
	DWORD m_dwCurCollisionTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioModeItem *Clone();
	virtual void OnCreate();
	virtual void OnDestroy();
	void SetItemValue( DWORD dwIndex, float fXPos, float fZPos, int iAngle, float fSpeed, float fPower, DWORD dwActivateTime );
	void SetItemValue( DWORD dwIndex, D3DXVECTOR3 vPos, int iAngle, float fSpeed, float fPower, DWORD dwActivateTime );
	
	inline void SetDirection( D3DXVECTOR3 vDir ) { m_vMoveDirection = vDir; }

public:
	virtual void Process();
	void ProcessWait();
	void ProcessMove();
	void ProcessPingpong();
	void ProcessDelay();

protected:
	void SetMoveState();
	void SetPingpongState();
	void SetDelayState();

public:
	virtual bool IsCollision( ioBaseChar *pChar );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	void SetWallCollision( const D3DXVECTOR3 &vFaceNrm );
	
public:
	ioHunterCoinModeItem();
	ioHunterCoinModeItem( const ioHunterCoinModeItem &rhs );
	virtual ~ioHunterCoinModeItem();
};

inline ioHunterCoinModeItem* ToHunterCoinModeItem( ioModeItem *pItem )
{
	if( !pItem || pItem->GetTypeID() != MIT_HUNTER_COIN )
		return NULL;

	return dynamic_cast< ioHunterCoinModeItem* >( pItem );
}