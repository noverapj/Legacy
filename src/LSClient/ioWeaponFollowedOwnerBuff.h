#pragma once
#include "ioBuff.h"
class ioWeaponFollowedOwnerBuff :	public ioBuff
{
private:
	typedef struct _tagFollowWeaponInfo
	{
		WeaponInfo  weaponinfo;
		float		fStartAngle;
		float		fStartRange;
		float		fMoveSpeed;
		float		fRemoveGapLength;

	}FollowWeaponInfo;
	typedef std::vector<FollowWeaponInfo> FollowWeaponInfoList;

	typedef struct _tagFollowWeaponControl
	{
		DWORD		dwIndex;
		float		fMoveSpeed;
		float		fRemoveGapLength;

	}FollowWeaponControl;
	typedef std::vector<FollowWeaponControl> FollowWeaponControlList;
	
	enum BuffSync
	{
		BS_CREATE_WEAPON,
		BS_DELETE_WEAPON,
	};
private:
	CEncrypt<bool> m_bEnableDropZoneCancel;
	bool			m_bAlreadyCallWeapon;

	FollowWeaponInfoList m_vFollowWeaponInfoList;
	FollowWeaponControlList	 m_dwFollowWeaponControlList;

private:
	void CreateFollowWeapon( ioBaseChar *pOwner, D3DXVECTOR3 vOwnerPos, D3DXQUATERNION qtRot, DWORD dwWeaponIndex );
	void FollowWeaponRotateMoveDir( D3DXVECTOR3& vOutMoveDir, D3DXVECTOR3 vCurMoveDir, float fAngle );
	bool CheckOwnerState();

	bool UpdateWeaponControl( float fTimePerSec );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
public:
	ioWeaponFollowedOwnerBuff(void);
	ioWeaponFollowedOwnerBuff( const ioWeaponFollowedOwnerBuff &rhs );
	~ioWeaponFollowedOwnerBuff(void);
};

inline ioWeaponFollowedOwnerBuff* ToWeaponFollowedOwnerBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WEAPON_FOLLOWED_OWNER )
		return NULL;

	return dynamic_cast< ioWeaponFollowedOwnerBuff* >( pBuff );
}
