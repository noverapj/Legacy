#pragma once
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioKeyMarkAttackBuff :	public ioBuff
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

	ioHashString		m_szA_MarkTargetName;
	ioHashString		m_szS_MarkTargetName;
	ioHashString		m_szD_MarkTargetName;

	//Effect
	ioHashString		m_szA_MakrMapEffect;
	ioHashString		m_szS_MakrMapEffect;
	ioHashString		m_szD_MakrMapEffect;
	ioHashString		m_szMarkEffect;
	DWORD				m_dwA_MarkMapEffectID;
	DWORD				m_dwS_MarkMapEffectID;
	DWORD				m_dwD_MarkMapEffectID;

	//Loop
	bool				m_bFindTargetOnlyFirstLoop;
	ioHashString		m_szAttackTarget;
	bool				m_bInputKey;
	DWORD				m_dwAttackDelayTime;
	DWORD				m_dwKeyInputTime;
	D3DXVECTOR3			m_vTargetTeleportOffset;

	//Attack
	int					m_iAttackCount;
	int					m_iAttakMaxCount;
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
	ioKeyMarkAttackBuff(void);
	ioKeyMarkAttackBuff( const ioKeyMarkAttackBuff &rhs );
	virtual ~ioKeyMarkAttackBuff(void);
};

inline ioKeyMarkAttackBuff* ToKeyMarkAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_KEY_MARK_ATTACK )
		return NULL;

	return dynamic_cast< ioKeyMarkAttackBuff* >( pBuff );
}