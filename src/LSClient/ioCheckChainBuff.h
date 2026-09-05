#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioCheckChainBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_CreateChain,
		BS_CreateWeapon,
		BS_AutoAttack,
		BS_START,
		BS_WAIT,
		BS_EXPLOSION
	};

	struct Chain
	{
		int nWeaponIndex;
		bool bSetVisible;
		ioHashString szTargetName;
	};

	typedef std::vector<Chain> ChainList;

	struct DistInfo
	{
		ioHashString szName;
		float fDistSq;
		D3DXVECTOR3 vMidPos;

		bool operator<( const DistInfo &rhs ) const
		{
			return fDistSq < rhs.fDistSq;
		}
	};

	typedef std::vector< DistInfo > SortList;

protected:
	ChainList m_ChainList;
	SortList m_SortList;

	BuffState m_BuffState;
	//충돌 범위
	float m_fColRange;
	//충돌 각도
	float m_fColAngle;
	//확인 버프
	ioHashString m_szCheckBuffName;
	//생성할 무기
	int m_nChainWeapon;
	int m_nAttackWeapon;
	//총 무기 개수
	int m_nTotalChainCnt;
	//무기 인덱스
	DWORD m_dwWeaponBaseIndex;
	//키입력 설정
	bool m_bSetEnableInputKey;
	//공격 애니메이션
	ioHashString m_szAttackAnimation;
	float m_fAttackTimeRate;
	//공격 시작 시간
	DWORD m_dwFireTime;
	//공격 확인
	bool m_bEnableAttack;
	//자동 공격 확인
	DWORD m_dwCheckAutoTime;
	DWORD m_dwAutoTime;
	
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
	//체인 생성
	void CreateChain( ioBaseChar *pOwner );
	//타겟 확인
	void CheckTarget( ioBaseChar *pOwner );
	//타겟 리스트 정렬
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList );
	//체인 설정
	void SetChain( ioBaseChar *pOwner );
	//타겟 가능한지 확인
	bool IsEnableTarget( ioBaseChar *pTarget );
	//자동 공격 확인
	void CheckAutoAttack( ioBaseChar *pOwner );
	//캐릭터 상태 확인
	void CheckCharState( ioBaseChar *pOwner );
	//공격 가능한 상태인지 확인
	bool IsEnableAttack( ioBaseChar *pOwner );
	//공격 애니메이션 설정
	void SetAttackTime( ioBaseChar *pOwner, ioHashString szName, float fRate );
	//무기 생성 확인
	void CheckCreateWeapon( ioBaseChar *pOwner );
	//체인 연결된 타겟만 버프 해제
	void RemoveCheckBuff( ioBaseChar *pOwner );
	//리스트 제거
	void ClearList( ioBaseChar *pOwner );

public:
	ioCheckChainBuff();
	ioCheckChainBuff( const ioCheckChainBuff &rhs );
	virtual ~ioCheckChainBuff();
};

inline ioCheckChainBuff* ToCheckChainBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHECK_CHAIN )
		return NULL;

	return dynamic_cast< ioCheckChainBuff* >( pBuff );
}