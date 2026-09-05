#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"
class ioINILoader;
class ioWeapon;
class ioBaseChar;

#define MAX_COMMAND_CNT 11

class ioGalfordItem : public ioWeaponItem
{
private:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,		
		CS_NORMAL_ATTACK,
		CS_ATTACK_FIRE,
	};

	enum SpecialState
	{
		SS_NONE,
		SS_REPLICA_ATTACK,
		SS_REPLICA_END_ATTACK,
		SS_SHADOW_COPY,
		SS_STRIKE_HEADS,
		SS_STRIKE_HEADS_LOOP,
		SS_STRIKE_HEADS_LAND_ATTACK,
		SS_PAPPY_ATTACK,
		SS_JUMP_KEY_COUNTER_ATTACK,
		SS_JUMP_KEY_COUNTER_LAND_ATTACK,
	};

	enum GalfordCmdState
	{
		GCS_NONE,
		GCS_SKILL,
		GCS_REPLICA_ATTACK,
		GCS_SHADOW_COPY,
		GCS_STRIKE_HEADS,

		//ÆÄÇÇ
		GCS_PAPPY,
	};

	enum GalfordSyncType
	{
		GST_NONE,
		GST_NORMAL,
		GST_ATTACK_FIRE,
		GST_REPLICA,
		GST_REPLICA_LAND,
		GST_SHADOW,
		GST_SHADOW_END,
		GST_STRIKE,
		GST_STRIKE_LOOP,
		GST_STRIKE_LAND,
		GST_PAPPY,

		GST_JUMP_ATTACK,
		GST_JUMP_LAND_ATTACK,
	};

	typedef struct _tagReplicaInfo
	{
		ioHashString				m_szReplicaLoopAni;
		float						m_fReplicaLoopAniRate;
		WeaponInfoList				m_ReplicaWeaponInfoList;
		AttackAttribute				m_ReplicaEndAttack;
		
		float						m_fReplicaTargetRange;
		float						m_fReplicaTargetAngle;
		float						m_fReplicaMaxMoveRange;
		float						m_fReplicaEndJumpPower;
		D3DXVECTOR3					m_vReplicaTargetOffset;
		D3DXVECTOR3					m_vReplicaNoneTargetOffset;
		_tagReplicaInfo() { Init(); }
		void Init()
		{
			m_fReplicaLoopAniRate		= 0.0f;
			m_fReplicaTargetRange		= 0.0f;
			m_fReplicaTargetAngle		= 0.0f;
			m_fReplicaMaxMoveRange		= 0.0f;
			m_vReplicaTargetOffset		= ioMath::VEC3_ZERO;
			m_vReplicaNoneTargetOffset	= ioMath::VEC3_ZERO;
		}

	}ReplicaInfo;
	typedef std::vector<ReplicaInfo> ReplicaInfoList;

	typedef struct _tagShadowCopyInfo
	{
		//Shadow Copy
		ioHashString				m_szShadowDummyName;
		D3DXVECTOR3					m_vShadowDummyOffset;
		ioHashString				m_szOwnerDummyName;
		D3DXVECTOR3					m_vOwnerDummyOffset;
		DWORD						m_dwShadowCopyDuration;
		DWORD						m_dwShadowCopyEnableKeyTime;
		float						m_fShadowCopyEndJumpPower;
		ioHashString				m_fShadowCopyStartEffect;
		ioHashStringVec				m_vShadowCopyBuffList;
		_tagShadowCopyInfo() { Init(); }
		void Init()
		{
			m_vShadowDummyOffset = ioMath::VEC3_ZERO;
			m_vOwnerDummyOffset = ioMath::VEC3_ZERO;
			m_dwShadowCopyDuration = 0;
			m_dwShadowCopyEnableKeyTime = 0;
			m_fShadowCopyEndJumpPower = 0.0f;
		}
	}ShadowCopyInfo;
	typedef std::vector<ShadowCopyInfo> ShadowCopyInfoList;
	
	typedef struct _TagStrikeHeads
	{
		AttackAttribute				m_StrikeHeadsAttack;
		ioHashString				m_GrapplingLoopMotion;
		float						m_fGrapplingLoopMotionRate;
		AttackAttribute				m_StrikeLandAttack;
		float						m_fJumpPower;

		GrapplingSkillWoundedInfo	m_TargetGrapplingInfo;

		float						m_fGrapplingOffsetGapRange;
		float						m_fGrapplingOffsetHeightRate;
		_TagStrikeHeads() { Init(); }
		void Init()
		{
			m_fGrapplingLoopMotionRate		= 0.0f;
			m_fJumpPower					= 0.0f;
			m_fGrapplingOffsetGapRange		= 0.0f;
			m_fGrapplingOffsetHeightRate	= 0.0f;
		}
	}StrikeHeadsInfo;

	struct GalfordCmdInfo
	{
		ioHashString		m_Cmd;
		GalfordCmdState		m_GalfordCmdState;
		int					m_iSkillSlot;
		ReplicaInfo			m_ReplicaInfo;
		ShadowCopyInfo		m_ShadowCopyInfo;
		StrikeHeadsInfo		m_StrikeHeadsInfo;

		int					m_iMaxAttackCount;
		AttackAttribute		m_Attack;
		float				m_fGravityRate;
		float				m_fDecreseGauge;

		GalfordCmdInfo() { Init(); }
		void Init()
		{
			m_Cmd.Clear();
			m_GalfordCmdState	= GCS_NONE;
			m_iSkillSlot		= 0;
			m_iMaxAttackCount	= 0;
			m_fGravityRate		= FLOAT1;
			m_fDecreseGauge = 0.0f;
		}
	};

	typedef std::vector<GalfordCmdInfo> GalfordCmdInfoList;
	

protected:
	ChargeState					m_ChargeState;
	SpecialState				m_SpecialState;
	int							m_iCurCombo;
	DWORD						m_dwAttackStartTime;

	ioHashStringVec				m_AttackReadyAniList;
	AttributeList				m_vExtendAttributeList;

	DWORD						m_dwMotionStartTime;
	DWORD						m_dwMotionEndTime;
	
	//cmd
private:
	int							m_iCurCmdCombo;
	int							m_iCurCmdMaxCombo;
	DWORD						m_dwInputStartTime;
	DWORD						m_dwInputCheckTime;

	CEncrypt<DWORD>				m_dwInputMaxTime;
	CEncrypt<DWORD>				m_dwInputStartGapTime;
	CEncrypt<DWORD>				m_dwInputGapTime;

	char						m_CurCmdInfo[MAX_COMMAND_CNT];
	int							m_iCurCmdIndex;
	ioUserKeyInput::DirKeyInput m_PreDirKey;
	
	//CS_GALFORD
	GalfordCmdInfoList			m_GalfordCmdInfoList;
	GalfordCmdInfo				m_CurGalfordCmdInfo;
	int							m_iGalfordCmdIndex;

	//Shadow Copy
	DWORD						m_dwShadowCopyEndTime;
	DWORD						m_dwShadowCopyEnableKeyStartTime;
	DWORD						m_dwShadowCopyStartEffectID;

	//Dummy
	ioHashString				m_EquipDummyCharName;
	int							m_iEquipDummyCharIndex;

	//Strike Heads
	ioHashString				m_GrapplingTargetName;


	//Counter Attack
	AttackAttribute				m_JumpKeyCounterAttack;
	ioHashString				m_szJumpKeyCounterAttackEffect;
	DWORD						m_dwJumpKeyCounterAttackEffectID;
	D3DXVECTOR3					m_vJumpKeyCANoneTargetTeleportOffset;
	D3DXVECTOR3					m_vJumpKeyCATargetTeleportOffset;
	float						m_fJumpKeyCAMaxMoveRange;
	float						m_fJumpKeyCAEndJumpPower;
	float						m_fJumpKeyCATargetRange;
	float						m_fJumpKeyCATargetAngle;
	float						m_fJumpKeyCABeforeHeight;
	float						m_fJumpKeyCACurMoveRange;
	AttackAttribute				m_JumpKeyCALandingAttack;
	float						m_fJumpKeyCAGravityRate;

	//Extra Ani Jump
	DWORD						m_dwExtraAniJumpTime;
	float						m_fExtraJumpPower;
	float						m_fExtraJumpAngle;

	//Pappy end Jump
	float						m_fPappyAttackEndJumpPower;
	float						m_fPappyAttackEndLandingRate;

	//Replica Attack
	float						m_fReplicaCurMoveRange;
	float						m_fReplicaBeforeHeight;

	//Gauge
	float						m_fMaxBullet;
	float						m_fCurBullet;
	float						m_fIncreaseGaugeTic;
	float						m_fNeedGauge;
	float						m_fJumpKeyCADecreaseBullet;

private:
	//Dummy
	int CreateDummyChar( ioBaseChar *pOwner, const ioHashString& szDummyName, D3DXVECTOR3 vPos );
	void DestroyDummyChar( ioBaseChar *pOwner, const ioHashString& szDummyName );
	void CreateEquipDummyChar( ioBaseChar *pOwner );
	void DestroyEquipDummyChar( ioBaseChar *pOwner );
public:
	void Init();
	void ClearCmdInfo();
private:
	void LoadCommand( ioINILoader &rkLoader );
	void LoadCMDState( ioINILoader &rkLoader, GalfordCmdInfo& rkInfo, int CmdState, const char *szInfo );
	void LoadReplicaAttack( ioINILoader &rkLoader, ReplicaInfo& rkInfo, const char *szInfo );
	void LoadShadowCopy( ioINILoader &rkLoader, ShadowCopyInfo& rkInfo, const char *szInfo );
	void LoadStrikerHeads( ioINILoader &rkLoader, StrikeHeadsInfo& rkInfo, const char *szInfo );
	void LoadPappyAttack( ioINILoader &rkLoader, GalfordCmdInfo& rkInfo, const char *szInfo );
	void LoadJumpKeyCounterAttack( ioINILoader &rkLoader );

	void OnCharging( ioBaseChar *pOwner );
	void OnNormalAttack( ioBaseChar *pOwner );
	void OnAttackFire( ioBaseChar *pOwner );
	void OnReplicaAttack( ioBaseChar *pOwner );
	void OnReplicaEndAttack( ioBaseChar *pOwner );
	void OnShadowCopy( ioBaseChar *pOwner );
	void OnStrikeHeadsAttack( ioBaseChar *pOwner );
	void OnStrikeHeadsLoopAttack( ioBaseChar *pOwner );
	void OnStrikeHeadsLandAttack( ioBaseChar *pOwner );
	void OnPappyAttack( ioBaseChar *pOwner );
	void OnJumpKeyCounterAttack( ioBaseChar *pOwner );
	void OnJumpKeyCounterLandAttack( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	bool ChangeToCmdSet( ioBaseChar *pOwner );
	bool ChangeToNewCmdSet( ioBaseChar *pOwner );
	bool ChangeToComboCmdSet( ioBaseChar *pOwner );

	bool CheckCmdInputTime();
	bool CheckCmdInput( ioBaseChar *pOwner );
	bool CheckLanding( ioBaseChar *pChar );
	
	bool CheckGalfordCmdState( GalfordCmdState eUseType );
	bool CheckPappyCmd( GalfordCmdState eUseType );
	bool CheckEnablePappyCmd( ioBaseChar *pOwner, GalfordCmdState eUseType );

	void SetGalfordCmdAttack( ioBaseChar *pOwner, GalfordCmdInfo kinfo );
	void SetActionAni( ioBaseChar *pOwner, const AttackAttribute& rkAttri, bool bRefreshFireTimeList = true );
	void SetReplicaAttack( ioBaseChar *pOwner );
	void SetReplicaPosition( ioBaseChar *pOwner );
	void SetReplicaEndAttack( ioBaseChar *pOwner );
	void SetShadowCopy( ioBaseChar *pOwner );
	void AddShadowCopyBuff( ioBaseChar *pOwner );
	void DeleteShadowCopyBuff( ioBaseChar *pOwner );
	void SetShadowCopyEndState( ioBaseChar *pOwner );
	void SetStrikeHeadsAttack( ioBaseChar *pOwner );
	void SetStrikeHeadsLandAttack( ioBaseChar *pOwner );
	void SetPappyCmdAttack( ioBaseChar *pOwner );
	void SetJumpCountLandAttackState( ioBaseChar *pOwner );

	void CreateEffect( ioPlayStage *pStage, DWORD& dwEffectID, const ioHashString& szEffectName, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vScale, const D3DXQUATERNION qtRot );
	void DestroyEffect( ioPlayStage *pStage, DWORD& dwEffectID );

	ioHashString FindAttTarget( ioBaseChar *pOwner, float fAngle, float fRange );

		
private:
	void DecreaseGauge( float fNeedGauge );

	void CheckExtraAniJump( ioBaseChar *pOwner );

public:
	void ClearGalfordCmdSpecialStateInfo( ioBaseChar *pOwner );

	bool CheckCmdInputState( ioBaseChar *pOwner );

	void SetCmdInputStart( int iType, ioBaseChar *pOwner );
	void SetCmdInputEnd( ioBaseChar *pOwner );
	bool SetItemGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void SetJumpCountAttackState( ioBaseChar *pOwner );
	void SetJumpCounterAttackPosition( ioBaseChar *pOwner );

	bool IsEnableJumpCounterAttack();
	bool IsCollisionAvailableWeaponBySpecialState();
	bool IsEntityCollisionSkipStateBySpecialState();
	bool IsNoWoundedBySpecialState( ioBaseChar *pOwner );
	bool IsEnableBlowWounded();
	bool IsFloatingStateBySpecialState() const;

	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pOwner );
	void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
	virtual int GetNeedBullet();
public:
	void ProcessGalfordCmdSpecialState( ioBaseChar *pOwner );

	void ProcessReserveDummyCreate( ioBaseChar *pOwner, const ioPlayMode* const pMode );
public:
	virtual WeaponSubType GetSubType() const;
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	//Gauge
	virtual int GetMaxBullet() { return (int)m_fMaxBullet; }
	virtual int GetCurBullet() { return (int) m_fCurBullet; }
	virtual void SetCurBullet( int iCurBullet );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
public:
	ioGalfordItem(void);
	ioGalfordItem( const ioGalfordItem &rhs );
	~ioGalfordItem(void);
};

inline ioGalfordItem* ToGalfordItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GALFORD_ITEM )
		return NULL;

	return dynamic_cast< ioGalfordItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioGalfordCmdSpecialState: public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_GALFORD_CMD_STATE"; }
	virtual const CharState GetState(){ return CS_GALFORD_CMD_STATE; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity );
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;

	virtual bool IsCanChangeJumppingState( ioBaseChar* pOwner ) { return true; }
	bool IsNoDropState( ioBaseChar* pOwner );
	virtual bool IsCanNoWound( ioBaseChar* pOwner );
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

	virtual bool IsFloatingState( const ioBaseChar * const pOwner ) const ;
public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );

public:
	ioGalfordCmdSpecialState();
	virtual ~ioGalfordCmdSpecialState();
};

inline ioGalfordCmdSpecialState* ToGalfordCmdSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_GALFORD_CMD_STATE )
		return NULL;

	return dynamic_cast< ioGalfordCmdSpecialState* >( pState );
}