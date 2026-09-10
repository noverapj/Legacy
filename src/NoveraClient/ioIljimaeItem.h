#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

typedef struct tagTargetInfo 
{
	ioHashString	m_TargetName;
	DWORD			m_dwEndTime;
	DWORD			m_dwMarkEffectID;
	float			m_fLengthSq;

	tagTargetInfo()
	{
		Init();
	}

	void Init()
	{
		m_TargetName.Clear();
		m_dwEndTime = -1;
		m_dwMarkEffectID = -1;
		m_fLengthSq = 0.0f;
	}
} TargetInfo;

class ioIljimaeItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
		CS_SEARCHING,
		CS_SEARCHING_END,
		CS_TELEPORT,
		CS_TELEPORT_END,
		CS_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	ioHashString m_GatheringEffect;
	UniqueObjID m_GatheringEffectID;

	DWORD m_dwGatheringStartTime;

	//
	CEncrypt<DWORD> m_dwMaxCoolTime;
	CEncrypt<float> m_fCurCoolTime;
	CEncrypt<float> m_fUseCoolTime;
	CEncrypt<float> m_fRecoveryTime;
	CEncrypt<float>	m_fRecoveryExTime;

protected:
	// Search
	DWORD			m_dwMotionStartTime;
	DWORD			m_dwMotionEndTime;

	ioHashString	m_SearchMotion;
	CEncrypt<float>	m_fSearchMotionRate;

	ioHashString	m_TeleportMotion;
	CEncrypt<float>	m_fTeleportMotionRate;

	ioHashString	m_TeleportEndMotion;
	CEncrypt<float>	m_fTeleportEndMotionRate;

	CEncrypt<float>	m_fDisableRange;

	CEncrypt<float>	m_fSearchRange;
	CEncrypt<float>	m_fSearchAngle;
	CEncrypt<int>	m_iSearchCount;
	CEncrypt<DWORD>	m_dwSearchDuration;

	CEncrypt<float>	m_fTeleportRange;
	CEncrypt<float>	m_fTeleportAngle;

	TargetWoundType	m_TargetWoundType;

	ioHashString	m_PreTargetName;

	ioHashString	m_TeleportMark;
	ioHashString	m_ReturnMark;
	CEncrypt<DWORD>	m_dwReturnMarkDuration;
	DWORD			m_dwReturnMarkID;
	DWORD			m_dwReturnMarkEndTime;

	D3DXVECTOR3		m_vReturnPosition;

	CEncrypt<float>	m_fMoveRange;
	CEncrypt<float>	m_fMoveHeightRange;
	
	std::vector<TargetInfo>	m_vTargetInfo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitCoolTime();
	void MaxCoolTime();
	void DecreaseCoolTime( int iNeedBullet );
	float GetUseBullet() { return m_fUseCoolTime; }

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

protected:
	void ChangeToSearching( ioBaseChar *pOwner );
	void OnSearching( ioBaseChar *pOwner );
	void ChangeToTeleport( ioBaseChar *pOwner );
	void OnTeleport( ioBaseChar *pOwner );
	void OnTeleportEnd( ioBaseChar *pOwner );

protected:
	void CheckTarget( ioBaseChar *pOwner );
	bool IsEnableTarget( ioBaseChar *pChar );
	void ClearAllTarget( ioBaseChar *pOwner );

public:
	void CheckTargetDuration( ioPlayStage *pStage );
	void CheckTargetRange( ioPlayStage *pStage );

protected:
	void _AttachTargetEffect( ioBaseChar *pOwner, TargetInfo& rkTargetInfo );
	void _DestroyTargetEffect( ioPlayStage *pStage, TargetInfo& rkTargetInfo );

public:
	bool IsNoFallState();
	const ioHashString& GetReturnMarkEffet() const;
	DWORD GetReturnMarkDuration();
	bool IsShowMarkEffect();
	void SetDisableReturnMarkEffect();
	void UpdateMarkEffect( ioPlayStage *pStage );
	
	const ioHashString& GetPreTarget() const { return m_PreTargetName; }
	void SetPreTarget( ioHashString vPreTargetName ) { m_PreTargetName = vPreTargetName; }
	int GetTargetCount() { return (int)m_vTargetInfo.size(); }
	
	const D3DXVECTOR3& GetReturnPosition() const { return m_vReturnPosition; }
	void SetReturnPosition( D3DXVECTOR3 vPreTargetName ) { m_vReturnPosition = vPreTargetName; }

	void SetReturnMarkEndTime( DWORD dwEndTime );
	void GetCurTargetList( ioHashStringVec &vTargetNameList );
	

public:
	ioIljimaeItem();
	ioIljimaeItem( const ioIljimaeItem &rhs );
	virtual ~ioIljimaeItem();
};

inline ioIljimaeItem* ToIljimaeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ILJIMAE )
		return NULL;

	return dynamic_cast< ioIljimaeItem* >( pItem );
}

class TargetDistSort : std::binary_function< const TargetInfo&, const TargetInfo&, bool >
{
public:
	bool operator()( const TargetInfo &lhs , const TargetInfo &rhs ) const
	{
		if( lhs.m_fLengthSq < rhs.m_fLengthSq )
		{
			return true;
		}

		return false;
	}
};
