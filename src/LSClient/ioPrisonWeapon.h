#pragma once
#include "ioWeapon.h"

class ioEffect;

class ioPrisonWeapon : public ioWeapon
{
public:
	struct UpDownMoveInfo
	{
		float m_fMoveSpeed;
		float m_fMinRnage;
		float m_fMaxRnage;

		UpDownMoveInfo()
		{
			m_fMoveSpeed = 0.0f;
			m_fMinRnage  = 0.0f;
			m_fMaxRnage  = 0.0f;
		}
	};
	typedef std::vector<UpDownMoveInfo> UpDownMoveInfoVec;

	struct UpDownInfo
	{
		UpDownMoveInfoVec m_vUpDownMoveInfo;
		float m_fMoveUpDownHeight;

		UpDownInfo()
		{
			m_fMoveUpDownHeight = 0.0f;
		}
	};
	typedef std::vector<UpDownInfo> UpDownInfoVec;

protected:

	enum WeaponState
	{
		WS_INIT,
		WS_MOVE,
		WM_DELAY,
		WS_PRISON_CREATE,
		WS_PRISON_UPDOWN,		
		WS_END,
	};

	WeaponState m_State;

	float m_fMoveRange;

	D3DXVECTOR3 m_vCollisionDir;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vPrisionPos;

	D3DXVECTOR3 m_vStartUpDownPos;
	UINT m_iUpDownIndex;

	float m_fCurMoveRange;
	float m_fTotUpDownRange;
	float m_fCurUpDownRange;
	
	ioEffect *m_pFireMesh;
	ioOrientBox m_LocalAttackBox;

	ioHashString m_szRedDealyEffect;
	ioHashString m_szBlueDealyEffect;
	ioHashString m_szRedPrisonEffect;
	ioHashString m_szBluePrisonEffect;
	ioHashString m_szEndEffect;	

	UniqueObjID m_PrisonEffectID;
	UniqueObjID m_PrisonEffectID2;

protected:
	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;
	CEncrypt<int> m_iUpDownInRange;
	CEncrypt<int> m_iUpDownOutRange;

protected:
	UpDownInfoVec m_vUpDownInfo;

protected:
	DWORD m_dwDelayTime;
	DWORD m_dwCurDelayTime;

	DWORD m_dwUpDownDuration;
	DWORD m_dwCurUpDownDuration;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:	
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void LoadUpDownProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );	

public:
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

protected:
	bool SetFireMesh( const ioHashString &szFileName );

public:	
	void CreateGuide( ioINILoader &rkLoader );

public:
	void ProcessMove( float fTimePerSec, ioPlayStage *pStage );
	void ProcessUpDown( float fTimePerSec, ioPlayStage *pStage );

	void ProcessPrisionEffect( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual void SetWeaponDead();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );	
	virtual D3DXVECTOR3 GetMoveDir();

public:
	void SetMoveRange( float fMoveRange );
	void SetPrisionPos( const D3DXVECTOR3 &vPos );

public:
	void CreateDealyEffect( float fTimePerSec, ioPlayStage *pStage );
	void CreatePrisonEffect( float fTimePerSec, ioPlayStage *pStage );

	void CreateEndEffect( float fTimePerSec, ioPlayStage *pStage );
	void CheckTestCollisionBox();

public:
	virtual WeaponType GetType() const;

	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

public:
	void ClearData();

public:
	ioPrisonWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioPrisonWeapon();
};

inline ioPrisonWeapon* ToPrisonWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_PRISON_WEAPON )
		return NULL;

	return dynamic_cast< ioPrisonWeapon* >( pWeapon );
}