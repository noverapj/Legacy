#pragma once
#include "iobuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioSubWeaponAttackBuff :	public ioBuff
{
protected:
	struct SubWeaponInfo
	{
		ioHashString m_szWeaponEffect;
		float m_fWeaponAngle;
		D3DXVECTOR3 m_vWeaponPos;
		WeaponInfoList m_WeaponInfoList;
	};

	typedef std::vector<SubWeaponInfo> SubWeaponInfoList;

	struct DistInfo
	{
		ioBaseChar *pChar;
		float fDistSq;

		bool operator<( const DistInfo &rhs ) const
		{
			return fDistSq < rhs.fDistSq;
		}
	};

	typedef std::vector< DistInfo > SortList;

protected:
	float m_fColRange;
	float m_fColAngle;

	SortList m_SortList;
	ioHashString m_szTargetName;

	ioHashStringVec m_RemoveBuffList;

	float m_fStartJumpPower;
	float m_fEndJumpPower;

	std::vector<DWORD> m_SubWeaponEffectList;
	SubWeaponInfoList m_SubWeaponInfoList;

	IntVec m_DisableInputKeyState;

	int m_iCurAttackIdx;

	ioHashString m_szAttackSoundName;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	bool FindTarget( ioBaseChar *pOwner );
	//타겟 리스트 정렬
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList );
	//공격 가능한 상태인지 확인
	bool IsEnableTarget( ioBaseChar *pTarget );

protected:
	void CreateSubWeaponEffect( ioBaseChar *pOwner );
	void UpdateSubWeaponEffect( ioBaseChar *pOwner );
	DWORD CreateEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioBaseChar *pOwner, DWORD& dwEffectID );

protected:
	void CheckInputKey(  ioBaseChar *pOwner );
	bool CheckDisableKey( ioBaseChar *pOwner );

	void SetAttackState( ioBaseChar *pOwner );

public:
	ioSubWeaponAttackBuff(void);
	ioSubWeaponAttackBuff( const ioSubWeaponAttackBuff& rhs );
	virtual ~ioSubWeaponAttackBuff(void);
};

inline ioSubWeaponAttackBuff* ToSubWeaponAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SUB_WEAPON_ATTACK_BUFF )
		return NULL;

	return dynamic_cast< ioSubWeaponAttackBuff* >( pBuff );
}