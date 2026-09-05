#pragma once

#include "../io3DEngine/ioOrientBox.h"
#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"

class ioEntityGroup;
class ioPlayStage;

typedef struct tagTargetBulletInfo
{
	DWORD m_dwTargetTime;
	ioHashString m_szTargetName;

	tagTargetBulletInfo()
	{
		Init();
	}

	void Init()
	{
		m_dwTargetTime = 0;
		m_szTargetName.Clear();
	}

} TargetBulletInfo;
typedef std::vector< TargetBulletInfo > TargetBulletInfoList;

typedef struct tagGatheringInfo
{
	DWORD m_dwCreateTime;
	ioHashString m_GatheringEffect;
	ioHashString m_GatheringSound;

	tagGatheringInfo()
	{
		m_dwCreateTime = 0;
	}

} GatheringInfo;
typedef std::vector< GatheringInfo > GatheringInfoList;


class ioCrossBowItem : public ioWeaponItem
{
public:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_GATHERING,
		FS_FIRE,
		FS_NORMAL_ATTACK,
		FS_JGATHERING,
		FS_JFIRE,
	};

	enum SyncType
	{
		ST_CHARGE,
		ST_DIR,
	};

protected:
	// state
	FireState m_FireState;

	DWORD m_dwChargeStartTime;

	// Gathering
	int m_iCurCombo;
	ioHashString m_AttackReadyAni;
	GatheringInfoList m_GatheringInfoList;

	bool m_bTargetRot;
	CEncrypt<DWORD> m_dwRotateSpeed;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	ioHashString m_GatherAnimation;
	ioHashString m_GatherMoveAnimation;

	ioHashString m_GatherJumpAnimation;
	float m_fGatherJumpAniRate;
	
	bool m_bSetGatherMoveAni;

	UniqueObjID m_GatheringEffectID;
	DWORD m_dwGatheringStartTime;
	DWORD m_dwNextBulletAddTime;

	ioHashString m_GuidEffect;
	UniqueObjID m_GuidEffectID;

	// attack
	ioHashString m_AttackFireAni;
	float m_fAttackFireAniRate;

	AttackAttribute m_FireAttribute;
	DWORD m_dwBaseWeaponIndex;
	
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;

	// bullet
	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;
	ioHashString m_CurTargetName;
	TargetBulletInfoList m_TargetBulletInfoList;

	// target & marker
	TargetWoundType m_TargetWoundType;

	CEncrypt<float> m_fMarkerRange;

	CEncrypt<float> m_fCheckBoxX;
	CEncrypt<float> m_fCheckBoxY;
	CEncrypt<float> m_fCheckBoxZ;

	CEncrypt<DWORD> m_dwTargetSearchTime;
	CEncrypt<DWORD> m_dwTargetEndDuration;
	DWORD m_dwSearchCheckTime;

	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetMinRange;
	CEncrypt<float> m_fTargetAngle;

	ioHashString m_TargetEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual WeaponSubType GetSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void CheckAimJumpState( ioBaseChar *pChar );

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsMoveAttackState( ioBaseChar *pOwner );

	virtual float GetAttackSpeedRate() const;

	virtual bool CheckLanding( ioBaseChar *pChar );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool SetAimState( ioBaseChar *pChar );
	virtual bool CheckAimState(bool bAimMove = false);

public:
	void ClearTargetBulletInfo();

	void SetAimJumpState( ioBaseChar *pOwner, bool bAimed );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnFire( ioBaseChar *pOwner );

	void OnJGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );

protected:
	ioHashString GetCurGatheringEffect();
	ioHashString GetCurGatheringSound();

	bool CheckAddBulletTime();
	bool CheckNextAddBullet( ioBaseChar *pOwner, ioHashString &szAddTarget );

	bool CheckTargetState( ioBaseChar *pOwner, ioHashStringVec &vReleseList, ioHashStringVec &vFindList );
	bool CheckValidateTarget( ioBaseChar *pOwner, ioHashStringVec &vReleseList );
	bool CheckFindTarget( ioBaseChar *pOwner, ioHashStringVec &vFindList );

	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

	ioHashString AddBulletInfo( ioBaseChar *pOwner, ioPlayStage *pStage );
	void AddBulletInfo( const ioHashString &szName, ioPlayStage *pStage );
	void AddTarget( const ioHashString &szName, ioPlayStage *pStage );
	void ReleaseTarget( const ioHashString &szName, ioPlayStage *pStage );
	ioHashString FindTarget( ioBaseChar *pOwner );
	bool IsExistTarget( const ioHashString &szName );

	int GetCurTargetCnt();
	bool GetTargetList( ioHashStringVec &vList );

	void CheckMoveKey( ioBaseChar *pOwner );
	void CheckMoveDir( ioBaseChar *pOwner );
	void CheckMoveAni( ioBaseChar *pOwner );

public:
	ioCrossBowItem();
	ioCrossBowItem( const ioCrossBowItem &rhs );
	virtual ~ioCrossBowItem();
};

inline ioCrossBowItem* ToCrossBowItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CROSS_BOW )
		return NULL;

	return dynamic_cast< ioCrossBowItem* >( pWeapon );
}

