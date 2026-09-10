#pragma once
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioKeyTeleportAttackBuff :	public ioBuff
{
protected:
	enum BuffState
	{
		BS_LOOP,
		BS_ATTACK,
	};

	enum BuffSyncType
	{
		BST_CHANGE_TARGET,
		BST_ATTACK,
	};
protected:
	BuffState			m_BuffState;

	//Target 
	TargetWoundType		m_TargetWoundType;
	CEncrypt<float>		m_fTargetMinAngle;
	CEncrypt<float>		m_fTargetMaxAngle;
	CEncrypt<float>		m_fTargetMinRange;
	CEncrypt<float>		m_fTargetMaxRange;

	//ioHashString		m_szA_MarkTargetName;
	//ioHashString		m_szS_MarkTargetName;
	ioHashString		m_szD_MarkTargetName;

	//Effect
	//ioHashString		m_szA_MakrMapEffect;
	//ioHashString		m_szS_MakrMapEffect;
	ioHashString		m_szD_MakrMapEffect;
	ioHashString		m_szMarkEffect;
	//DWORD				m_dwA_MarkMapEffectID;
	//DWORD				m_dwS_MarkMapEffectID;
	DWORD				m_dwD_MarkMapEffectID;

	//Loop
	ioHashString		m_szAttackTarget;
	bool				m_bInputKey;
	DWORD				m_dwAttackDelayTime;			// 공격 키 딜레이 값
	DWORD				m_dwKeyInputTime;				// 키 입력 시간
	D3DXVECTOR3			m_vTargetTeleportOffset;		// 용병이 이동될 타겟 위치에 대한 오프셋

	//Attack
	int					m_iAttackCount;
	int					m_iAttakMaxCount;				// 최대 공격 횟수
	DWORD				m_dwMotionEndTime;
	AttackAttribute		m_NormalTargetAttackAttribute;
	AttackAttribute		m_AirTargetAttackAttribute;
	AttackAttribute		m_DownTargetAttackAttribute;
	float				m_fEndJumpPower;
	DWORD				m_dwFireMotionEndTime;
	
	//Option
	bool				m_bDeleteBuffByDownState;
	bool				m_bDeleteBuffByDropState;
	bool				m_bDeleteBuffByWoundedState;
	bool				m_bDisableOwnerDownAttack;
	bool				m_bReduceAllGauge;
	
	/////////////// 추가 변수
	ioHashString		m_szCheckTargetBuffName;		// 타겟에게 확인할 걸려있는 버프 이름

protected:
	void		ClearData();
	void		FindTarget( ioBaseChar* pChar, ioPlayStage *pStage );
	bool		IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

	void		UpdateMark( ioBaseChar* pChar, ioPlayStage *pStage );
	void		CreateMarker( ioBaseChar *pChar, ioPlayStage *pStage, const ioHashString& szTargetName, const ioHashString& szEffectName, DWORD& dwEffectID );
	void		DestroyMaker( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID );

	bool		CheckOwnerState();
	bool		CheckOperation( ioBaseChar* pChar, float fTimePerSec );
	bool		CheckInputKey( ioBaseChar* pChar, ioPlayStage *pStage );
	bool		CheckEnableAttackState( ioBaseChar* pChar, ioPlayStage *pStage );
	bool		CheckEnableTargetState( ioPlayStage *pStage );
	bool		CheckEnableOwnerState( ioBaseChar* pChar, ioPlayStage *pStage );

	void		SetActionState( ioBaseChar* pChar, ioPlayStage *pStage );
	void		SetTeleport( ioBaseChar *pChar, ioPlayStage *pStage );
	void		SetTargetRotation( ioBaseChar *pChar );
	void		SetActionAni( ioBaseChar* pChar, const AttackAttribute& Attri );
	void		SetActionEndState( ioBaseChar *pChar );

	const AttackAttribute& GetActionAttribute( ioBaseChar* pChar );
	bool		IsAirState( ioBaseChar* pTarget );
	bool		IsDownState( ioBaseChar* pTarget );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();
public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
	virtual bool IsProtected() const;
public:
	ioKeyTeleportAttackBuff(void);
	ioKeyTeleportAttackBuff( const ioKeyTeleportAttackBuff &rhs );
	virtual ~ioKeyTeleportAttackBuff(void);
};

inline ioKeyTeleportAttackBuff* ToKeyTeleportAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_KEY_MARK_ATTACK )
		return NULL;

	return dynamic_cast< ioKeyTeleportAttackBuff* >( pBuff );
}