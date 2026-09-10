#pragma once


#include "ioWeapon.h"

class ioContactWeapon : public ioWeapon
{
protected:
	ioOrientBox m_PreAttackBox;
	ioOrientBox m_MidAttackBox;

	bool m_bFirstCheck;
	bool m_bDeadWait;

	bool m_bDisableOwnerScale;
	// 스킬에서 생성한 웨폰
	bool m_bSkillWeapon;

	//충돌체 강제 할당
	ioAniCollisionBoxGrp *m_pSpecialBox;


	WeaponInfo	m_CallWeaponInfo;
	ChangeDirInfoList m_CallAttFireList;
	bool	m_bMoveWeapon;
	BOOL	m_bArleadyCallWeapon;

protected:
	enum UpdateState
	{
		CONTACT_CREATED,
		CONTACT_FIRST,
		CONTACT_NORMAL,
	};
	UpdateState m_UpdateState;

	ioHashStringVec m_AddBuffList;

	bool			m_bAddedBuffList;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual void SetStartCollisionBox();
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual void GetWorldAttackBoxList( std::vector< ioOrientBox > &vBoxList );

	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadCallAttribute( ioINILoader &rkLoader );
	void LoadCallAttributeFireDirectionList( ioINILoader &rkLoader );

	virtual const D3DXVECTOR3& GetPosition() const;

public:
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual WeaponType GetType() const;

	virtual void SetLiveTime( DWORD dwTime );
	
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

	void StartCollisionBoxByOwner();
	void StartCollisionBoxByStruct();
	void StartCollisionBoxByDummyChar();
	void StartCollisionBoxByMachine();

	void UpdateAttackBoxByOwner( float fTimePerSec, bool bSetNull );
	void UpdateAttackBoxByStruct( float fTimePerSec, bool bSetNull );
	void UpdateAttackBoxByDummyChar( float fTimePerSec, bool bSetNull );
	void UpdateAttackBoxByMachine( float fTimePerSec, bool bSetNull );
	
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

public:
	const D3DXVECTOR3& GetAttchBoxPosition();
	void CheckCollision( ioBaseChar *pChar, bool bApplyCol );
	void SetSkillWeapon( bool b ){ m_bSkillWeapon = b;}
	void CharCollision();
	void ApplyWeaponBuff( ioBaseChar *pBeatenChar );

protected:
	virtual void SetWeaponDead();
	void CallAttribute();

public:
	ioContactWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioContactWeapon();
};

inline ioContactWeapon* ToContactWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_CONTACT )
		return NULL;

	return dynamic_cast< ioContactWeapon* >( pWeapon );
}

