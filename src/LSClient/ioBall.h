#pragma once

#include "ioPlayEntity.h"
#include "ioWorldEventReceiver.h"
#include "ioAttackAttribute.h"
#include "DamageTable.h"

class ioPlayMode;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class ioBaseChar;
class ioOpcodeShape;

class SP2Packet;



class ioBall : public ioPlayEntity, public ioWorldEventReceiver
{
protected:
	struct MoveEffectInfo
	{
		ioHashString m_EffectName;
		float m_fCheckRate;

		MoveEffectInfo()
		{
			m_fCheckRate = 0.0f;
		}
	};

	typedef std::vector< MoveEffectInfo > MoveEffectInfoList;

	struct WoundSoundInfo
	{
		ioHashString m_SoundName;
		float m_fCheckRate;

		WoundSoundInfo()
		{
			m_fCheckRate = 0.0f;
		}
	};

	typedef std::vector< WoundSoundInfo > WoundSoundInfoInfoList;

public:
	enum BallStructState
	{
		BS_DELAY,
		BS_MOVE,
	};

protected:
	int m_iStructNum;
	int	m_iArrayIndex;

	ioHashString m_BallName;

	DWORD m_dwCreateTime;
	DWORD m_dwProcessTime;
	DWORD m_dwBallTouchTime;

	ArmorType m_ArmorType;

	ioHashString m_LastAttacker;
	ioHashString m_PreAttacker;

	DamageTable m_DamageTable;

// Move
protected:
	BallStructState m_State;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vRollAxis;

	CEncrypt<float> m_fMaxMoveSpeed;
	CEncrypt<float> m_fMaxBoundAmt;
	CEncrypt<float> m_fLimitBoundAmt;

	CEncrypt<float> m_fMoveSpeedAddRate;
	CEncrypt<float> m_fBoundAmtAddRate;

	CEncrypt<float> m_fPingPongPowerRate;
	CEncrypt<float> m_fCharPushRate;
	
	CEncrypt<float> m_fFrictionRate;
	CEncrypt<float> m_fAirFrictionRate;


	CEncrypt<float> m_fBoundPower;
	CEncrypt<float> m_fCurMoveSpeed;
	CEncrypt<float> m_fGravityAmt;

	CEncrypt<float> m_fGoalSpeedRate;
	CEncrypt<float> m_fGoalBoundRate;

	int   m_iMapTranslateContinuousCount;
	DWORD m_dwMapTranslateFrame;

// Etc
protected:
	int m_iAnimationIndex;

	bool m_bMapCollisionPushed;
	CEncrypt<bool> m_bSendReposition;
	CEncrypt<bool> m_bGoalState;

	ioHashString m_BallEffect;
	UniqueObjID m_dwBallEffect;

	ioHashString m_BallShadowEffect;
	UniqueObjID m_dwShadowEffect;

	ioHashString m_TimeOutEffect;
	ioHashString m_WoundedEffect;
	ioHashString m_MoveEffect;

	CEncrypt<float> m_fEffectRate;
	CEncrypt<float> m_fGoalAreaRange;

	ioHashString m_StartSound;
	WoundSoundInfoInfoList m_vSoundList;
	MoveEffectInfoList m_vMoveEffectList;

// OutLine
protected:
	OutLineRecordList m_OutLineChangeList;
	D3DCOLORVALUE m_OutLineColor;
	float m_fOutLineSize;
	bool  m_bOutLineModified;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

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

public:
	virtual bool IsNeedCheckMapCollision() const;
	virtual int  DontMoveEntityLevel() const;

public:
	virtual ioWorldEventReceiver* GetWorldEventReceiver();

	virtual void OnTelepotation( const D3DXVECTOR3 &vTargetPos );
	virtual void OnFireZoneDamage( float fDamage, const ioHashString &szBuff );
	virtual void OnPoisonZoneDamage();

protected:
	virtual void UpdateBottomHeight() const;

public:
	void InitStructInfo( int iBallNum, int iIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale );

	void SetMoveState();
	void SetDelayState();

	bool FillAllDamagedList( SP2Packet &rkPacket );

protected:
	void ProcessCurMoveSpeed( float fTimePerSec );
	void ProcessMoveState( float fTimePerSec );
	void ProcessState( float fTimerPerSec );

	void CheckGoalSaveTouch( ioBaseChar *pChar, const D3DXVECTOR3 &vDir );
	void CheckBallTouchTime();

	bool GoalCheck( float fTimePerSec );
	void DropZoneCheck();

	float GetCurFriction();

	void SetBallEffectPosition();
	void SetBallEffectOrientation();
	void UpdateBallEffect( float fTimePerSec );

	void CheckMoveEffect( float fPreRate, float fNewRate );
	int CheckWoundSound( float fRate );

public:
	inline BallStructState GetState() const { return m_State; }
	inline int GetArrayIndex() const { return m_iArrayIndex; }
	inline int GetStructNum() const { return m_iStructNum; }
	
	inline float GetCurMoveSpeed() const { return m_fCurMoveSpeed; }
	inline float GetCurBoundPower() const { return m_fBoundPower; }
	inline const D3DXVECTOR3& GetMoveDir() const { return m_vMoveDir; }

	inline bool IsMapCollisionPushed() const { return m_bMapCollisionPushed; }
	inline const ioHashString& GetLastAttackerName() const { return m_LastAttacker; }

public:
	bool IsFloating();
	void SendStructInfo();

	void OnStructInfo( SP2Packet &rkPacket );
	void OnStructReposition( SP2Packet &rkPacket );

	void GetBallStructInfo( SP2Packet &rkPacket );

	bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	void EntityCollision( ioPlayStage *pStage, const D3DXVECTOR3 &vMoveAmt );
	float CalcCollisionAmt( const D3DXVECTOR3 &vMoveAmt );

	void SetEndFocusEffect( const ioHashString &szEndFocusEffect );

	DWORD GetEventGapTime();

	void SetZeroHP();
	void ClearSendData();
	void SetGoalState( bool bGoal );

	inline const ioHashString& GetBallName() const { return m_BallName; }

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
	ioBall( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioBall();
};

inline ioBall* ToBallStruct( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_BALL )
		return NULL;

	return dynamic_cast< ioBall* >( pPlay );
}

inline const ioBall* ToBallConst( const ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_BALL )
		return NULL;

	return dynamic_cast< const ioBall* >( pPlay );
}



