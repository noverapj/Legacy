#pragma once


#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;

class ioZoneEffectWeapon : public ioWeapon
{
public:
	enum OwnerAttachType
	{
		OAT_NONE,
		OAT_NORMAL,
		OAT_SKILL,
		OAT_TARGET,
		OAT_ONCE,
	};

public:
	enum WoundItemType
	{
		WIT_NONE,
		WIT_MAID,
		WIT_DONQUIXOTE,
		WIT_DONQUIXOTE_LAND,
		WIT_VALKYRIE,
		WIT_CROW,
	};

protected:
	ioEffect		*m_pWeaponEffect;
	ioHashString	m_szWeaponEffect;
	DWORD			m_dwWeaponEffectID;

	TargetColType	m_TargetColType;
	TargetWoundType	m_TargetWoundType;
	CEncrypt<bool>	m_bDisableColGrap;

	OwnerAttachType m_OwnerAttachType;

	CEncrypt<bool>	m_bEnableOwnerScale;

	CEncrypt<float> m_fZoneRange;
	CEncrypt<float> m_fZoneMinRange;
	CEncrypt<float> m_fZoneMaxRange;
	CEncrypt<float>	m_fZoneInnerRange;
	CEncrypt<float>	m_fZoneAngle;
	float			m_fCurRange;

	CEncrypt<float>	m_fUpHeight;
	CEncrypt<float>	m_fUnderHeight;
	CEncrypt<float>	m_fWidth;
	CEncrypt<float>	m_fMaxWidth;
	float			m_fCurWidth;

	CEncrypt<float>	m_fOffSet;
	CEncrypt<float>	m_fHeightOffSet;

	CEncrypt<bool> m_bDisableZoneMapIgnore;
	CEncrypt<bool> m_bEnableDefenseSound;
	CEncrypt<bool> m_bEnableProtectCol;

	D3DXVECTOR3 m_vStartOffSet;
	D3DXVECTOR3 m_vCollisionDir;
	D3DXVECTOR3 m_vAttackDir;
	ioOrientBox m_LocalAttackBox;

	typedef std::list< ioPlayEntity* > BlockEntityList;
	BlockEntityList m_vBlockEntityList;

	//
	ioHashString m_ExtraZoneCheckBuff;
	CEncrypt<float> m_fExtraZoneAngle;

	CEncrypt<bool>	m_bAttachOnLand;
	CEncrypt<bool>  m_bOwnerCharRotApply;
	CEncrypt<bool>  m_bOnlyOneRotApply;
	CEncrypt<bool>	m_bSelfRotApply;
	bool			m_bCalcFirstWorldBox;

	ioHashString	m_BoomEffect;

	ioHashStringVec m_szCollsionIgnoreChar;

	ioHashString   m_CallAreaWeaponName;
	CEncrypt<DWORD> m_dwCallAreaWeaponTime;
	DWORD           m_dwCallAreaWeaponIndex;

	bool			m_bUseDelayCollisionTime;
	DWORD			m_dwDelayCollisionTime;

	D3DXQUATERNION	m_qtFirstRot;
	CEncrypt<int>   m_iAttackSide;

	bool			m_bUseMaxCollisionCnt;
	int				m_nMaxCollisionCnt;
	int				m_nNowCollisionCnt;

	WoundItemType	m_wondItemType;

	bool m_bSetCheckTarget;

	bool m_bTerrainHeightWeapon;
	float m_fTerrainHeightWeapon;

	//딱지 스킬때 추가
	ioHashString  m_stAttachTargetName;
	bool		  m_bNotChangeLiveTime;

	bool m_bTargetSelfControl;

	bool m_bUseTestmapCol_ColBox;

	//잠깐 충돌 함수를 사용하다 안하다 하기 위한 변수
	bool m_bNoCol;

	bool m_bOwnerAttachNotDestroy;
	bool m_bOnce;

	CEncrypt<float>	m_fMoveSpeed;


public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void SetNotUseCollision( bool bUse ){ m_bNoCol = bUse; }
	void SetZoneValue( bool bAttached, bool bSkill );
	// for skill
	void SetZoneValue( float fRange, float fAngle, float fUpHeight, float fUnderHeight, float fWidth, bool bAttached );
	void SetZoneValue( float fRange, bool bAttached );
	void SetZoneValue( D3DXVECTOR3 vPos, bool bTargetAttached );
	
	void SetZoneWidth( float fWidth );
	void SetZoneRange( float fRange );
	void SetCollisionDir( D3DXVECTOR3 &vDir );
	void SetCollisionWoundType( int iColType, int iWoundType );

	float GetZoneRange(){ return m_fZoneRange; }
	float GetZoneWidth(){ return m_fWidth; }

	bool CheckCollisionSphere( ioPlayEntity *pEntity );
	bool CheckCollisionCylinder( ioPlayEntity *pEntity );
	bool CheckCollisionBox( ioPlayEntity *pEntity );
	bool CheckCollisionDonut( ioPlayEntity *pEntity );
	bool CheckCollisionCone( ioPlayEntity *pEntity );

	bool IsAlreadyBlocked( ioPlayEntity *pEntity ) const;
	void AddBlockEntity( ioPlayEntity *pEntity );

	int GetCollisionType(){ return m_CollisionType; }

	inline int GetOwnerAttachType() const { return m_OwnerAttachType; }

	bool CheckDestroyOwnerAttach();

	void SetIgnoreCollsionChar( ioHashString& szName );

	void SetUseTestMapColBox( bool bUse ){ m_bUseTestmapCol_ColBox = bUse; }
	void SetZoneMapIgnore( bool bIgnore ){ m_bDisableZoneMapIgnore = bIgnore; }

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

	bool CreateWeaponEffect( const ioHashString &szFileName );
	void CallAreaWeapon();

	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

	void GetColOrientBox( ioOrientBox& kBuffBox);

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

	virtual void SetLiveTime( DWORD dwTime );

	virtual D3DXVECTOR3 GetMoveDir();

	int		GetWoundItemType();

	bool IsCheckTarget();

	void SetAttachTargetName( const ioHashString& stTargetName );

	virtual void SetMoveSpeed( float fSpeed );

public:
	virtual void SetWeaponDead();

public:
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	
	void SetAttackDir(  D3DXVECTOR3 &vDir );

public:
	ioZoneEffectWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioZoneEffectWeapon();
};

inline ioZoneEffectWeapon* ToZoneEffectWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ZONE_EFFECT )
		return NULL;

	return dynamic_cast< ioZoneEffectWeapon* >( pWeapon );
}

