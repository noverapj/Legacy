#pragma once

#include "FindPredicate.h"
#include "ioWeapon.h"

class FD_CloseAliveEnemy : public FindPredicate
{
public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );

public:
	FD_CloseAliveEnemy( const ioBaseChar *pOwner );
	virtual ~FD_CloseAliveEnemy();
};

//-----------------------------------------------------------------------------------

class FD_AimTarget : public FindPredicate
{
protected:
	float m_fAimAngle;
	float m_fAimRange;
	float m_fMinRange;
	bool m_bOwnerTeam;
	bool m_bDownAim;

public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );

public:
	FD_AimTarget( const ioBaseChar *pOwner, float fAngle, float fMinRange, float fRange, bool bDownAim, bool bOwnerTeam = true );
	virtual ~FD_AimTarget();
};

//-----------------------------------------------------------------------------------

class FD_AimTarget_Staff : public FindPredicate
{
protected:
	float m_fAimAngle;
	float m_fAimRange;
	float m_fMinRange;
	bool  m_bOwnerTeam;

public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );

public:
	FD_AimTarget_Staff( const ioBaseChar *pOwner, float fAngle, float fMinRange, float fRange, bool bOwnerTeam = true );
	virtual ~FD_AimTarget_Staff();
};

//-----------------------------------------------------------------------------------

class FD_AimTarget_Bow : public FindPredicate
{
protected:
	float m_fAimAngle;
	float m_fAimRange;
	float m_fMinRange;
	bool m_bOwnerTeam;
	bool m_bDownAim;

public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );

public:
	FD_AimTarget_Bow( const ioBaseChar *pOwner, float fAngle, float fMinRange, float fRange, bool bDownAim, bool bOwnerTeam = true );
	virtual ~FD_AimTarget_Bow();
};

//-----------------------------------------------------------------------------------

class FD_SightTarget : public FindPredicate
{
protected:
	float m_fSightAngle;
	float m_fSightRange;

	class SightTargetSort : public std::binary_function< const CharDistanceSq&, const CharDistanceSq&, bool >
	{
	protected:
		TeamType m_OwnerTeam;

	public:
		bool operator()( const CharDistanceSq &lhs, const CharDistanceSq &rhs ) const;

	public:
		SightTargetSort( TeamType eOwnerTeam );
	};

public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );
	
public:
	bool CheckUser( ioBaseChar* pTarget );

public:
	FD_SightTarget( const ioBaseChar *pOwner, float fAngle, float fRange );
	virtual ~FD_SightTarget();
};

//-----------------------------------------------------------------------------------

class FD_AutoTarget : public FindPredicate
{
protected:
	float m_fMinAngle;
	float m_fMaxAngle;
	float m_fRangeSq;

public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );

public:
	FD_AutoTarget( const ioBaseChar *pOwner, float fMinAngle, float fMaxAngle, float fRange );
	virtual ~FD_AutoTarget();
};

//-----------------------------------------------------------------------------------

class FD_CloseChainTarget : public FindPredicate
{
protected:
	float m_fRangeSq;
	float m_fHalfAngle;
	ioWeapon* m_pWeapon;

public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );

public:
	FD_CloseChainTarget( const ioBaseChar *pOwner, ioWeapon *pWeapon, float fRange, float fAngle );
	virtual ~FD_CloseChainTarget();
};

//-----------------------------------------------------------------------------------

class FD_GrapplingAutoTarget : public FindPredicate
{
protected:
	float	m_fMinAngle;
	float	m_fMaxAngle;
	float	m_fRangeSq;
	bool	m_bDownTarget;

public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );

public:
	FD_GrapplingAutoTarget( const ioBaseChar *pOwner, float fMinAngle, float fMaxAngle, float fRange, bool bDownTarget = true );
	virtual ~FD_GrapplingAutoTarget();
};

//-----------------------------------------------------------------------------------

class FD_RangeTarget : public FindPredicate
{
protected:	
	float	m_fMaxRange;
	float	m_fMinRange;
	bool	m_bOwnerTeam;
	bool	m_bDownAim;


public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList );
	virtual bool CheckFindUser( ioBaseChar* pChar );

public:
	FD_RangeTarget( const ioBaseChar *pOwner, float fMaxRange, float fMinRange, bool bDownAim, bool bOwnerTeam = true );
	virtual ~FD_RangeTarget();
};

//-----------------------------------------------------------------------------------

class FD_BuffHasTarget : public FD_RangeTarget
{
protected:
	ioHashString m_szTargetBuff;

public:
	virtual bool CheckFindUser( ioBaseChar* pTarget );

public:
	FD_BuffHasTarget( const ioBaseChar *pOwner, const ioHashString& szTargetBuff, float fMaxRange, float fMinRange, bool bDownAim, bool bOwnerTeam = true );
	virtual ~FD_BuffHasTarget();
};

//-----------------------------------------------------------------------------------

class FD_RangeMultipleTarget : public FD_RangeTarget
{
protected:
	float	m_fMaxAngle;
	bool	m_bHideTargetFind;

public:	
	virtual void GetFindCharList( IN const BaseCharList &rkCandidateList, OUT BaseCharList& rkTargetedCharList, OUT BaseCharList& rkNotTargetedCharList, bool bTeleport = false, IN D3DXVECTOR3 vOwnerPos = D3DXVECTOR3() );
	virtual bool CheckFindUser( ioBaseChar* pChar );

public:
	FD_RangeMultipleTarget( const ioBaseChar *pOwner, float fMaxRange, float fMinRange, float fMaxAngle, bool bDownAim, bool bOwnerTeam = true, bool bHideTargetFind = true );
	virtual ~FD_RangeMultipleTarget();
};