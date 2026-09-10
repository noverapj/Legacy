#pragma once
#include "ioBuff.h"

class ioCountGatherAttackBuff :	public ioBuff
{
protected:
	enum BuffSyncState
	{
		BSS_INCREASE_HIT_COUNT,
		BSS_CREAT_HIT_ATTACK_WEAPON,
		BSS_END_ATTACK,
		BSS_END,
	};
protected:
	typedef struct _tagCountEndAttack
	{
		int				  m_iCount;
		WeaponInfoList	  m_EndAttackList;

	}CountEndAttack;
	
	typedef std::vector<CountEndAttack> CountEndAttackList;

	class CountEndAttackSort : public std::binary_function< const CountEndAttack& , const CountEndAttack&, bool >
	{
	public:
		bool operator() ( const CountEndAttack &lhs, const CountEndAttack &rhs ) const
		{
			if( lhs.m_iCount < rhs.m_iCount )
				return true;

			return false;
		}
	};

protected:
	bool				m_bDisableDownState;
	bool				m_bDisableDropState;
	DWORD				m_dwWeaponBaseIndex;

	//Hit Attack
	WeaponFireInfoList	m_HitAttackWeaponList;
	Vector3Vec			m_vHitAttackOffsetList;
	ioHashString		m_szHitAttackBuff;

	//EndAttack
	CountEndAttackList	m_CountEndAttackList;
	int					m_iMinEndAttackNumber;
	bool				m_bAlreadyCreate;

	int					m_iItemSlotIndex;
	DWORD				m_dwSwitchTime;
	DWORD				m_dwEnableSwitchTime;

	bool				m_bReserveEndAttackFire;
	bool				m_bSendedEndAttack;

protected:
	void ClearData();
	void LoadHitWeaponList( ioINILoader &rkLoader );
	void LoadEndWeaponList( ioINILoader &rkLoader );
	
	bool CheckOwnerStateCheck();
	void CheckEnableKeepBuff( ioBaseChar* pOwner );

	void SetReserveEndAttack();
	inline bool IsReserveEndattack() { return m_bReserveEndAttackFire; }

	bool EnableHitAttack( ioBaseChar* pAttacker, ioBaseChar* pWounder );
	void CreateEndWeapon( ioBaseChar* pOwner );

	
public:
	void CheckHitCount( ioBaseChar* pAttacker, ioBaseChar* pWounder );
	void CreateHitAttackWeapon( ioBaseChar* pAttacker, ioBaseChar* pWounder, ioPlayStage *pStage );
	void SendIncreaseHitCount( ioBaseChar* pAttacker, ioBaseChar* pWounder, ioPlayStage *pStage );
	void IncreaseHitCount();
public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();
public:
	ioCountGatherAttackBuff(void);
	ioCountGatherAttackBuff( const ioCountGatherAttackBuff &rhs );
	virtual ~ioCountGatherAttackBuff(void);
};

inline ioCountGatherAttackBuff* ToCountGatherAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_COUNT_GATHER_ATTACK )
		return NULL;

	return dynamic_cast< ioCountGatherAttackBuff* >( pBuff );
}