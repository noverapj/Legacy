#pragma once

#include "ioRangeSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioRangePositionInverseSkill : public ioRangeSkill
{
	enum SkillState
	{
		SS_LOOP,
		SS_CHANGE,
		SS_END,
	};

	enum CharWoundedType
	{
		CWT_NONE,
		CWT_AIR,
		CWT_DOWN,
		CWT_NORMAL
	};

	enum BuffState
	{
		BTS_TELEPORT,
		BTS_STATE_CHANGE,
	};

	enum SkillSync
	{
		SSC_CHANGE_ACTION,
		SSC_CHANGE_END,
	};

	struct BaseCharWoundedInfo
	{
		ioHashString m_szCharName;
		CharWoundedType m_WoundedType;

		BaseCharWoundedInfo()
		{
			m_szCharName.Clear();
			m_WoundedType = CWT_NONE;
		}
	};

protected:

	SkillState m_State;

	typedef std::vector<BaseCharWoundedInfo> BaseCharWoundedList;
	BaseCharWoundedList m_vCharList;
	
	D3DXVECTOR3 m_vOwnerSkillUsePos;

	float m_fTeleportAxisYOffset;
	float m_fTeleportRange;

	float m_fTeleportCurRange;
	float m_fRangeSpeed;

	float m_fRangeMinRadius;
	float m_fRangeMaxRadius;

	float m_fTeleprotTime;
	DWORD m_dwTeleportStartTime;
	
	BuffInfoList m_pEnemyBuff;
	BuffInfoList m_pEnemyAirBuff;
	BuffInfoList m_pEnemyDownBuff;

	BuffInfoList m_pChangeEnemyBuff;
	BuffInfoList m_pChangeEnemyAirBuff;
	BuffInfoList m_pChangeEnemyDownBuff;

	ioHashString m_szChangePositionEffect;
	ioHashString m_szChangePositionDownEffect;
	ioHashString m_szChangePositionAirEffect;

	UniqueObjID m_dwChangePositionEffect;
	UniqueObjID m_dwChangePositionDownEffect;
	UniqueObjID m_dwChangePositionAirEffect;

	float m_fEffectScaleRate;
	UniqueObjID m_dwRangeEffectID;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	virtual bool OnSkillStart( ioBaseChar *pChar );	
	virtual void OnSkillEnd( ioBaseChar *pChar );

protected:
	void SetChangeActionState( ioBaseChar *pChar );

	virtual void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void LoadWoundedTypeBuffList( ioINILoader &rkLoader, const char *szBuffType, BuffInfoList& pList );

#ifndef SHIPPING //UJ
	#define LoadWoundedTypeBuffList_e		LoadWoundedTypeBuffList
#else
	//UJ 130812, 스트링 상수 암호화 및 숨기기 위한 코드
	// VTable 순서에 영향이 있을 수 있으므로 무조건 맨 아래에 아래 내용을 유지할것
	//
	// 주의1: 최대 63글자(64bytes)까지만 지원
	// 주의2: debug 빌드에서는 string 보호가 되지 않는다
	// 주의3: 절대 문자열 상수 말고 char* szKey 등을 넘기지마라!! crash 가능!!
	//
public:
	template <int N>
	__forceinline void LoadWoundedTypeBuffList_e( ioINILoader &rkLoader, const char (&szBuffType)[N], BuffInfoList& pList )
	{
		_ENCSTR(szBuffType, szEncBuffType);
		return LoadWoundedTypeBuffList( rkLoader, szEncBuffType, pList );
	}
#endif // _DEBUG

	void SetTargetBuff( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetTargetPositionChange( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetChangeEffect( ioBaseChar* pChar, ioHashString szEffect, UniqueObjID& effectID, D3DXVECTOR3 vDirEffect, ioPlayStage* pStage );
	
	void SetTargetEndBuff(  ioBaseChar* pChar, ioPlayStage* pStage );

	void UddateRangeEffect( ioPlayStage* pStage, float fTimePerSec );
	void UpdateChangeEffect( ioBaseChar* pChar, UniqueObjID& effectID, ioPlayStage* pStage );
	void UpdateChangeAllEffect( ioPlayStage* pStage );


	bool SetTargetWoundedTypeParseBuffList( ioBaseChar *pOwner, ioBaseChar *pChar, BuffState State, CharWoundedType WoundedType );

	D3DXVECTOR3 CaculrateDirection( const D3DXVECTOR3 vView, const D3DXVECTOR3 vTarget, float fOffset );

public:
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void ClearData();
	void FindTarget( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	ioRangePositionInverseSkill();
	ioRangePositionInverseSkill( const ioRangePositionInverseSkill &rhs );
	virtual ~ioRangePositionInverseSkill();
};

inline ioRangePositionInverseSkill* ToRangePositionInverseSkill( ioSkill *pSkill )
{
	ioRangeSkill *pRange = ToRangeSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != RST_POSITION_INVERSE )
		return NULL;

	return dynamic_cast< ioRangePositionInverseSkill* >(pRange);
}