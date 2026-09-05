#pragma once

#include "ioPlayEntity.h"
#include "ioWorldEventReceiver.h"
#include "ioAttackAttribute.h"

class ioPlayMode;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class SP2Packet;
class ioBaseChar;
class ioAniCollisionBoxGrp;

class ioMachineStruct : public ioPlayEntity, public ioWorldEventReceiver
{
protected:
	int m_iStructNum;
	int	m_iArrayIndex;
	ioHashString m_MachineName;

	ArmorType m_ArmorType;

	CEncrypt<float> m_fDefaultMaxHP;
	CEncrypt<float> m_fDefaultMinHP;

	ioHashString m_BlockBuff;
	ioHashString m_KillerChar;

	ioHashString m_DestroyEffect;
	ioHashString m_DestroySound;

	ioHashString m_TakeCharName;

	ioHashString m_TakeCharMotion;
	float m_fTakeCharMotionRate;

	bool m_bLive;

protected:
	int   m_iMapTranslateContinuousCount;
	DWORD m_dwMapTranslateFrame;

	bool m_bSendedDieMessage;
	bool m_bMapCollisionPushed;

	float m_fTakeEnableRange;

	bool m_bEnableTakeEffect;
	ioHashString m_EnableTakeEffect;

	ioHashString m_WeaponFireEffect;
	ioHashString m_WeaponFireSound;

	ioHashString m_CameraBuff;

protected:
	ioAniCollisionBoxGrp *m_pSpecialBox;

	DWORD m_dwCollisionStartTime;
	DWORD m_dwCollisionEndTime;
	ioHashString m_szCollisionBox;

// charging
protected:
	CEncrypt<DWORD> m_dwChargingStartTime;
	CEncrypt<DWORD> m_dwChargingMaxTime;

	CEncrypt<float> m_fCurChargeRate;

	CEncrypt<float> m_fMaxDamageRate;
	CEncrypt<float> m_fMaxForceRate;
	CEncrypt<float> m_fMaxBlowRate;
	CEncrypt<float> m_fMaxSpeedRate;
	CEncrypt<float> m_fMaxFloatRate;

	ioHashString m_ChargingEffect;

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
	virtual bool IsLive();

	virtual void CheckChargeInfo( ioWeapon *pWeapon );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon );

	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;

	virtual MachineStructType GetType() const;
	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

protected:
	virtual void UpdateBottomHeight() const;

public:
	virtual bool IsNeedCheckMapCollision() const;
	virtual bool IsMountAvailable() const;
	virtual bool IsNowMountEnable() const;

public:
	virtual int  DontMoveEntityLevel() const;
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual ioWorldEventReceiver* GetWorldEventReceiver();

	virtual void OnTelepotation( const D3DXVECTOR3 &vTargetPos );
	virtual void OnFireZoneDamage( float fDamage, const ioHashString &szBuff );
	virtual void OnPoisonZoneDamage();

public:
	virtual void InitMachineStructInfo( int iStructNum, int iArrayNum, DWORD dwCreateGapTime, DWORD dwSeed, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale );

	virtual void SetHP( float fCurHP, float fMaxHP );
	virtual void SetTakeChar( ioHashString szTakeCharName );
	virtual void SetTakeState( const ioHashString &szTakeCharName );
	virtual void ReleaseState();

	virtual void ApplyMachineStructExtraInfo( SP2Packet &rkPacket );

protected:
	virtual void ProcessState( float fTimerPerSec );

protected:
	virtual bool IsCanSendNetwork();
	virtual void SendDieMessageToServer( bool bEffect );

public:
	virtual void OnMachineStructDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker );
	virtual void OnMachineStructInfo( SP2Packet &rkPacket );

	virtual void SetStructDie( ioBaseChar *pChar, bool bEffect );
	virtual void StructDie( bool bEffect=true );

	virtual void SendMachineStructInfo();

	virtual bool IsTakeEnableState();
	
public:
	inline void SetKillerChar( const ioHashString& szKiller ) { m_KillerChar = szKiller; }

	inline int GetMachineStructNum() const { return m_iStructNum; }
	inline int GetMachineArrayIdx() const { return m_iArrayIndex; }

	inline const ioHashString& GetTakeCharName() { return m_TakeCharName; }
	inline bool IsMapCollisionPushed() const { return m_bMapCollisionPushed; }
	inline float GetTakeEnableRange() const { return m_fTakeEnableRange; }

	inline const ioHashString& GetTakeMotion() const { return m_TakeCharMotion; }
	inline float GetTakeMotionRate() const { return m_fTakeCharMotionRate; }

public:
	void CheckSpecialCollisionBox( const ioHashString &szFileName );
	virtual ioOrientBox GetAttackCollisionBox( AniCollisionType eType = ACT_NONE, bool bNoScale=false );

	void SetEnableTakeEffect( bool bSet );
	inline bool IsEnableTakeEffect() const { return m_bEnableTakeEffect; }

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
	ioMachineStruct( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioMachineStruct();
};

inline ioMachineStruct* ToMachineStruct( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_MACHINE_STRUCT )
		return NULL;

	return dynamic_cast< ioMachineStruct* >( pPlay );
}

inline const ioMachineStruct* ToMachineStructConst( const ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_MACHINE_STRUCT )
		return NULL;

	return dynamic_cast< const ioMachineStruct* >( pPlay );
}

