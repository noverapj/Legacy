#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

#define MAX_COMMAND_CNT 11

enum CmdUseType
{
	CUT_NORMAL,
	CUT_SKILL,
	CUT_RERA_SHU,
	CUT_SHICHIKAPU_ETU,
	CUT_SHICHIKAPU_ARM,
};

struct ChargeCmdSubInfo
{
	AttackAttribute m_CmdAttack;
	AttackAttribute m_CmdAttack_D;
	AttackAttribute m_CmdAttack_S;

	bool m_bCmdAttack_D;
	bool m_bCmdAttack_S;

	ioHashString m_stGrappAttakerAni;
	float		 m_fGrappAttakerAniRate;
	ioHashString m_stGrappWounerAni;
	float		 m_fGrappWounerAniRate;

	float		 m_fGrappOffSetX;
	float		 m_fGrappOffSetY;

	int			 m_nID;
	int			 m_nMaxRepeatCnt;

	float		 m_fNeedGauge;

	ChargeCmdSubInfo()
	{
		Init();
	}

	void Init()
	{
		m_bCmdAttack_D = false;
		m_bCmdAttack_S = false;

		m_CmdAttack.Init();
		m_CmdAttack_D.Init();
		m_CmdAttack_S.Init();

		m_nID = -1;
		m_nMaxRepeatCnt = 100;
		m_fNeedGauge = 0.0f;
	}
};

typedef std::vector< ChargeCmdSubInfo > ChargeCmdSubInfoList;

struct ChargeCmdInfo
{
	ioHashString m_Cmd;

	CmdUseType m_CmdUseType;
	
	int m_iSkillSlot;

	ChargeCmdSubInfoList m_SubInfoList;
	
	ChargeCmdInfo()
	{
		Init();
	}

	void Init()
	{
		m_Cmd.Clear();
		m_SubInfoList.clear();

		m_iSkillSlot = 0;

		m_CmdUseType = CUT_NORMAL;
	}
};

typedef std::vector< ChargeCmdInfo > ChargeCmdInfoList;

class ioChargeCommandItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_ATTACK_FIRE,
	};

	enum SyncType
	{
		ST_ATTACK_STATE,
		ST_CMD_STATE,
		ST_CMD_STATE_D,
		ST_CMD_STATE_S,
		ST_EXTRA_ADD,
		ST_EXTRA_REMOVE,
		ST_FLY_STATE,
		ST_ROTATE,
		ST_JUMP_GRAP,
	};

	enum CmdInputType
	{
		CIT_ALL		= 0,
		CIT_DIR		= 1,
		CIT_ACTION	= 2,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	int m_iCurCombo;
	DWORD m_dwAttackStartTime;

	ChargeState m_ChargeState;
	
	CEncrypt<DWORD> m_dwInputSkillStartTime;

protected:
	CmdInputType m_iCmdInputType;
	ChargeCmdInfoList m_ChargeCmdInfoList;

	ioUserKeyInput::DirKeyInput m_PreDirKey;

	char m_CurCmdInfo[MAX_COMMAND_CNT];
	CEncrypt<int> m_iCurCmdIndex;

	CEncrypt<bool> m_bCmdInputD;
	CEncrypt<bool> m_bCmdInputS;

	CEncrypt<DWORD> m_dwInputMaxTime;
	CEncrypt<DWORD> m_dwInputStartGapTime;
	CEncrypt<DWORD> m_dwInputGapTime;

	CEncrypt<DWORD> m_dwInputStartTime;
	CEncrypt<DWORD> m_dwInputCheckTime;

	DWORD m_dwCmdAttackEndTime;

	CEncrypt<int> m_iCurCmdCombo;
	ChargeCmdInfo m_CurChargeCmdInfo;

protected:
	// extra gauge
	CEncrypt<bool> m_bUseExtraGauge;
	CEncrypt<bool> m_bSetExtraGauge;

	CEncrypt<float> m_fDecreaseExtraGauge;

	ioHashString m_ExtraGaugeEffect;
	ioHashStringVec m_ExtraGaugeBuffList;

//////////////////////////////////////////////////////////////////////////
	//∞‘¿Ã¡ˆ √ﬂ∞° by ¿±≈¬√∂
protected:
	CEncrypt<float> m_fIncreaseExtraGauge;
	CEncrypt<float> m_fNeedGauge;

	/*CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;*/

	//CEncrypt<float> m_fIncreaseGauge;
	//CEncrypt<float> m_fDecreaseGauge;

	//CEncrypt<float> m_fCurGauge;

	//Gauge
public:
	virtual int GetNeedBullet();

	/*virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );

	void CheckDecreaseGauge();

	bool IsEnableGauge();*/


//////////////////////////////////////////////////////////////////////////

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	void DecreaseGauge();

public:
	virtual WeaponSubType GetSubType() const;

protected:
	virtual	void LoadCmdInfoList( ioINILoader &rkLoader );

	virtual void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	
	bool ChangeToNewCmdSet( ioBaseChar *pOwner );
	bool ChangeToComboCmdSet( ioBaseChar *pOwner );
	bool ChangeToCmdSet( ioBaseChar *pOwner );

	virtual void SetCmdAttack( ioBaseChar *pOwner,
					   const AttackAttribute &rkAttr,
					   float fExtraAniRate=1.0f,
					   float fExtraForceRate=1.0f );

	void ApplyAttackState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void ApplyCmdSet( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );
	void ApplyCmdSetD( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );
	void ApplyCmdSetS( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );

	void ClearState();

	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

	bool CheckCmdInput( ioBaseChar *pOwner );

public:
	virtual void CheckCmdState( ioBaseChar *pOwner );
	void ClearCmdInfo();
	void ClearCmdTime();

	void SetCmdInputStart( int iType, ioBaseChar *pOwner );
	void SetCmdInputEnd( ioBaseChar *pOwner );
	bool CheckCmdInputState( ioBaseChar *pOwner );

	//
	void SetSkillInputStart();
	bool CheckSkillInputState( ioBaseChar *pOwner );

protected:
	bool CheckCmdInputTime();

protected:
	void SetExtraGaugeBuff( ioBaseChar *pOwner );
	void ReleaseExtraGaugeBuff( ioBaseChar *pOwner, bool bSend );
	void ProcessExtraGauge( ioBaseChar *pOwner );

protected:
	virtual bool CheckMamaHahaCmd( CmdUseType eUseType );
	virtual bool CheckEnableMamaHahaCmd( ioBaseChar *pOwner, CmdUseType eUseType );

public:
	ioChargeCommandItem();
	ioChargeCommandItem( const ioChargeCommandItem &rhs );
	virtual ~ioChargeCommandItem();
};

inline ioChargeCommandItem* ToChargeCommandItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_COMMAND )
		return NULL;

	return dynamic_cast< ioChargeCommandItem* >( pItem );
}

