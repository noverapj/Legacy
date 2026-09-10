#pragma once


#include "ioPlayEntity.h"
#include "ioWorldEventReceiver.h"
#include "ioAttackAttribute.h"

class ioPlayStage;
class ioPlayMode;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class SP2Packet;
class ioBaseChar;
class ioAniCollisionBoxGrp;


struct CreatePushStructInfo
{
	int m_iNum;
	D3DXVECTOR3 m_vNewPos;
	StructCreateType m_CreateType;

	CreatePushStructInfo()
	{
		m_iNum = 0;
		m_vNewPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_CreateType = SCT_NORMAL;
	}
};
typedef std::vector< CreatePushStructInfo > CreatePushStructInfoList;

class ioPushStruct : public ioPlayEntity, public ioWorldEventReceiver
{
public:
	enum PushStructState
	{
		PS_DELAY,
		PS_MOVE,
		PS_DIE,
	};

	enum AttackType
	{
		PAT_NONE,
		PAT_NORMAL,
		PAT_SEARCH_AUTO,
	};

	enum SearchAutoState
	{
		SAS_SEARCHING,
		SAS_TRACING,
		SAS_FIRE,
	};

	enum BomberState
	{
		BBS_WAIT,
		BBS_SENCING,
		BBS_TRACE,
	};


protected:
	int m_iStructNum;
	int	m_iArrayIndex;
	DWORD m_dwCreateTime;
	DWORD m_dwCreateEtcItemCode;
	DWORD m_dwProcessTime;
	
	PushStructType m_iStructType;
	PushStructState m_State;
	ioGaugeHelper *m_pGauge;

	ArmorType   m_ArmorType;

	CEncrypt<float> m_fDefaultMaxHP;
	CEncrypt<float> m_fDefaultMinHP;
	float m_fPushRange;
	float m_fHeightGap;
	float m_fGravityAmt;

	ioHashString m_ExplosionSound;
	ioHashString m_BlockBuff;

	ioHashString m_AreaWeaponName;
	DWORD m_dwAreaWeaponTime;

	DWORD m_dwAreaWeaponIndex;
	DWORD m_dwAreaWeaponCreateTime;

	int m_iAnimationIndex;
	float m_fAnimationRate;

protected:
	ioBaseChar *m_pPushChar;
	ioHashString m_LastPushChar;
	ioHashString m_KillerChar;
	ioHashString m_CreateItem;

	DWORD m_dwSetPushTime;
	D3DXVECTOR3 m_vMoveDir;
	float m_fMoveSpeedRate;
	float m_fCurMoveSpeed;

	CEncrypt<bool> m_bMountEnable;

	ioHashString m_PushAnimation;
	ioHashString m_DestroyEffect;
	ioHashString m_MoveEffect;

	MountedEntityList m_vMountEntityList;
	MountedWeaponList m_vMountWeaponList;

protected:
	StringList m_ObjectItemList;
	CreatePushStructInfoList m_PushStructInfoList;

	DWORD m_dwTicTime;
	CEncrypt<float> m_fDecreaseHP;

	DWORD m_dwGapTime;

	ioHashString m_OwnerCharName;

	ioHashString m_RedTeamEffect;
	ioHashString m_BlueTeamEffect;

	bool m_bObjectCollision;
	bool m_bSemiMapCollision;
	bool m_bNotMoveStruct;
	bool m_bNoDropStruct;

	bool m_bEnableClick;
	bool m_bDisableHPVisible;
	bool m_bSendedDieMessage;
	bool m_bMoveEffectCreated;
	bool m_bMapCollisionPushed;
	bool m_bOwnerNameVisible;

protected:
	int   m_iMapTranslateContinuousCount;
	DWORD m_dwMapTranslateFrame;

protected:
	AttackType m_iAttackType;
	TargetWoundType m_TargetWoundType;
    SearchAutoState m_iSearchAutoState;

	WeaponInfo m_WeaponInfo;
	WeaponFireInfoList m_vWeaponFireInfoList;
	int m_iWeaponFireCnt;

	bool m_bOwnerRotate;
	DWORD m_dwWeaponCreateTime;
	DWORD m_dwFireDuration;

	DWORD m_dwBaseWeaponIndex;

	IntVec m_vCreateWeaponIndexList;

	float m_fWeaponHeightRate;
	float m_fWeaponOffSet;

	ioHashString m_WeaponFireEffect;
	ioHashString m_WeaponFireSound;

// Searching Auto
protected:
	float m_fSearchingRange;
	float m_fSearchingAngle;
	
	DWORD m_dwTracingSpeed;
	DWORD m_dwTracingStartTime;
	DWORD m_dwTracingDuration;
	
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireEndDelay;

	D3DXVECTOR3 m_vStartDir;

	ioHashString m_TargetName;
	ioHashString m_SearchingEffect;
	ioHashString m_AutoFireEffect;
	ioHashString m_SearchingSound;

	UniqueObjID m_SearchSoundID;

// bomber move
protected:
	BomberState m_BomberState;

	ioHashString m_BomberCreateAni;
	ioHashString m_BomberSencingAni;
	ioHashString m_BomberTracingAni;

	float m_fBomberCreateAniRate;
	float m_fBomberSencingAniRate;
	float m_fBomberTracingAniRate;

	float m_fBomberMoveSpeed;

	DWORD m_dwBomberSensingEnableTime;
	DWORD m_dwBomberSensingDuration;
	CEncrypt<float> m_fBomberSensingRange;
	CEncrypt<float> m_fBomberSensingAngle;

	DWORD m_dwBomberSensingStartTime;
	float m_fBomberSencingMoveAngle;

	CEncrypt<float> m_fBomberTraceSpeed;
	CEncrypt<float> m_fBomberTraceRange;

	float m_fBomberRotateSpeed;
	DWORD m_dwBomberRotateSpeed;

	D3DXVECTOR3 m_vBomberMoveDir;
	ioHashString m_BomberTargetName;

	CEncrypt<DWORD> m_dwBomberTraceDuration;
	DWORD m_dwBomberTraceEndTime;

	float m_fBomberEnableMoveHeight;
	CEncrypt<float> m_fBomberBoomRange;
	CEncrypt<float> m_fBomberBoomAngle;

	bool m_bBomberOnlyCharBoom;

protected:
	ioAniCollisionBoxGrp *m_pSpecialBox;

	DWORD m_dwCollisionStartTime;
	DWORD m_dwCollisionEndTime;
	ioHashString m_szCollisionBox;

protected:
	bool m_bNoDropProcess;

	// (낚시터에서만 사용) 첫 생성 시 생성 자리에 용병이 있는지 확인하는 플래그, 이후 계속 true일때 체크하지 않음 
	bool m_bAreaCheckCharPos;

// OutLine
protected:
	OutLineRecordList m_OutLineChangeList;
	D3DCOLORVALUE m_OutLineColor;
	float m_fOutLineSize;
	bool  m_bOutLineModified;


public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );
	virtual void NeedUpdateBottomHeight();

	virtual void Translate( const D3DXVECTOR3 &vMove );
	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );
	virtual void TranslateByWorldEntity( const D3DXVECTOR3 &vMove );
	virtual void MapCollisionPushingTranslate( const D3DXVECTOR3 &vMoveAmt );
	virtual void DontMoveCollisionPushingTranslate( ioGameEntity *pPusher, const D3DXVECTOR3 &vMoveAmt );

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;

	virtual PlaySubType GetSubType() const;

	virtual ArmorType GetArmorType() const;

protected:
	virtual void UpdateBottomHeight() const;

public:
	virtual bool IsNeedCheckMapCollision() const;
	virtual bool IsMountingAvailable() const;
	virtual bool IsMountAvailable() const;
	virtual bool IsNowMountEnable() const;
	virtual bool IsCharCollisionSkipState( TeamType eTeamType, int iState=0 ) const;

public:
	virtual int  DontMoveEntityLevel() const;
	virtual void LoadProperty( ioINILoader &rkLoader );
	
	virtual void UpdateGauge();
	virtual void RenderGauge();

public:
	virtual ioWorldEventReceiver* GetWorldEventReceiver();

	virtual void OnTelepotation( const D3DXVECTOR3 &vTargetPos );
	virtual void OnFireZoneDamage( float fDamage, const ioHashString &szBuff );
	virtual void OnPoisonZoneDamage();

protected:
	void ProcessState( float fTimerPerSec );
	void ProcessBlockState( float fTimerPerSec );
	void ProcessFishingGroundState( float fTimerPerSec );

	void ProcessMoveState( float fTimePerSec );
	void ProcessAttack( float fTimePerSec );

	void ProcessNormalAttack();
	void ProcessSearchAutoAttack( float fTimePerSec );

	void CheckOwnerRotate();

	void TargetSearching( float fTimePerSec );
	void TargetTracing( float fTimePerSec );
	void TargetFire( float fTimePerSec );

	bool CheckTargetValid();

	void UpdateMountedEntityList();
	void UpdateMountedWeaponList();

	ioHashString GetObjectItem();
	void DropZoneCheck();
	void CheckLastPushChar();

	void CheckStructDieByHp();
	void CheckAreaWeapon();

	void CreateAreaWeapon( const D3DXVECTOR3 &vPos );

protected:
	bool IsCanSendNetwork();
	void SendDieMessageToServer( bool bEffect, bool bSendStructInfo );

public:
	void InitResource();
	void InitPushStructInfo( int iStructNum,
							 int iIndex,
							 DWORD dwCreateGapTime,
							 DWORD dwSeed,
							 DWORD dwEtcCode,
							 const D3DXVECTOR3 &vPos,
							 const D3DXVECTOR3 &vScale );

public:
	void SetHP( float fCurHP, float fMaxHP );
	void SetMoveState( ioBaseChar *pPushChar );
	void SetDelayState();
	void SetOwner( ioHashString szOwnerName );
	void SetCreateItem( ioHashString szCreateItem );

	float GetPushRange() const;
	bool IsCanPushed( ioPlayEntity *pEntity );

	void SetStructDie( ioBaseChar *pChar, bool bEffect );
	void SetStructDieByOwnerClear( ioBaseChar *pChar, bool bEffect );
	void ClearMountEntity();

	void ChangeToSearchingState();
	void ChangeToTracingState( const ioHashString &szTargetName, const D3DXVECTOR3 &vDir );
	void ChangeToFireState();

public:
	inline bool IsObjectCollision() const { return m_bObjectCollision; }
	inline bool IsNoDropStruct() const { return m_bNoDropStruct; }
	inline bool IsEnableClick() const { return m_bEnableClick; }

	inline PushStructState GetState() const { return m_State; }
	inline int GetPushArrayIdx() const { return m_iArrayIndex; }
	inline float GetCurMoveSpeed() { return m_fCurMoveSpeed; }
	inline float GetHeightGap() { return m_fHeightGap;	}
	inline const ioHashString& GetOwnerName() { return m_OwnerCharName; }
	inline DWORD GetCreateTime() const { return m_dwCreateTime; }

	inline ioBaseChar* GetPushChar() const { return m_pPushChar; }
	inline const ioHashString& GetLastPushChar() { return m_LastPushChar; }

	inline void SetPushTime( DWORD dwPushTime ) { m_dwSetPushTime = dwPushTime; }
	inline DWORD GetPushTime() const { return m_dwSetPushTime; }

	inline void SetKillerChar( const ioHashString& szKiller ) { m_KillerChar = szKiller; }

	inline int GetStructNum() const { return m_iStructNum; }
	inline PushStructType GetStructType() const { return m_iStructType; }
	inline bool IsMapCollisionPushed() const { return m_bMapCollisionPushed; }
	inline DWORD GetCreateEtcCode() const { return m_dwCreateEtcItemCode; }

public:
	void StructDie( bool bEffect, bool bCreateStruct=true, bool bUsing = false );
	void SendPushStructInfo();
	void MountEntityMove( const D3DXVECTOR3 &vMoveAmt );

	void SetBaseWeaponIndex( int iWeaponIndex );
	void CreateWeapon( float fAngle );
	bool CheckCreateWeaponIndex( int iIndex );

	bool GetAreaCheckCharPos() { return m_bAreaCheckCharPos; }
	void CompleteAreaCheckCharPos() { m_bAreaCheckCharPos = true; }

public:
	void OnPushDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker );
	void OnPushStructInfo( SP2Packet &rkPacket );
	void OnPushStructBomber( SP2Packet &rkPacket );

public:
	bool GetCollisionRect( RECT *pRect ) const;

// bomber
public:
	void SetBomberSencingState();
	void SetBomberTraceState( ioPlayStage *pStage, bool bSendNetwork );

protected:
	void ProcessBomberState( float fTimePerSec );
	void ProcessBomberMoveState( float fTimePerSec );

	void BomberMoving( float fTimePerSec );
	void BomberSencing( float fTimePerSec );
	void BomberTracing( float fTimePerSec );
	void BomberProcessMoving( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec );

	bool IsEnableTargetState( ioBaseChar *pChar );

	void FindTraceTarget( ioPlayStage *pStage );
	bool CheckTraceTarget( ioPlayStage *pStage );

	void CheckBomberEnd();

// OutLine
public:
	void ApplyOutLineChange( ChangeOutLineType eType, const D3DCOLORVALUE &rkColor, float fSize );
	void RestoreOutLineChange( ChangeOutLineType eType );
	bool HasOutLineType( ChangeOutLineType eType );

protected:
	void ApplyModifiedOutLine();
	void RemoveChangeOutLineRecord( ChangeOutLineType eType );
	void ChangeOutLine( const D3DCOLORVALUE &rkOutLineColor, float fOutLineSize );
	void RestoreOutLine();

public:
	void CheckSpecialCollisionBox( const ioHashString &szFileName );
	virtual ioOrientBox GetAttackCollisionBox( AniCollisionType eType = ACT_NONE, bool bNoScale=false );

public:
	virtual void AddEntityEvent( ioEntity *pEntity );

public:
	ioPushStruct( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioPushStruct();
};

inline ioPushStruct* ToPushStruct( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_PUSHSTRUCT )
		return NULL;

	return dynamic_cast< ioPushStruct* >( pPlay );
}

inline const ioPushStruct* ToPushStructConst( const ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_PUSHSTRUCT )
		return NULL;

	return dynamic_cast< const ioPushStruct* >( pPlay );
}

