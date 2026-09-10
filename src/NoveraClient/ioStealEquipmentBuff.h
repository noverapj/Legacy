#pragma once

#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

typedef std::vector<bool> BoolVec;

class ioStealEquipmentBuff : public ioBuff
{
private:
	enum EndType 
	{
		ET_DEFAULT,
		ET_NODASH,
		ET_NOCHANGE,
	};

	enum EquipParts
	{
		EP_WEAPON,
		EP_ARMOR,
		EP_HELMET,
		EP_CLOAK,
		EP_MAX
	};

protected:
	bool m_bEnableTargetAim;
	DWORD m_BuffEffectID;
	EndType m_iEndType;

	// 게이지 표시할때 쓰임
	CEncrypt<float> m_fExtraRate;

	ioHashStringVec m_AddBuffList;

	bool			m_bAddedBuffList;

	// 훔쳐올 장비를 가지고 있는 용병 객체
	ioBaseChar*		m_pTargetChar;

	BoolVec			m_vEnableStealEquipParts;
	// 장비 훔치기 실행 플래그
	bool			m_bStealEquip;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	inline bool IsEnableTargetAim() const { return m_bEnableTargetAim; }

	const float GetExtraRate() const { return m_fExtraRate; }

	void SetStealTargetChar( ioBaseChar* pChar );

	void ExcuteStealEquipment() { m_bStealEquip = true; }

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

public:
	void ChangeEquipItem();
protected:
	void CheckingOwnerChar( ioBaseChar *pOwner );

public:
	ioStealEquipmentBuff();
	ioStealEquipmentBuff( const ioStealEquipmentBuff &rhs );
	virtual ~ioStealEquipmentBuff();
};

inline ioStealEquipmentBuff* ToStealEquipmentBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_STEAL_EQUIPMENT_BUFF )
		return NULL;

	return dynamic_cast< ioStealEquipmentBuff* >( pBuff );
}
