#pragma once

class SP2Packet;
class ioAreaWeapon;
class ioBaseChar;
class ioINILoader;
class ioPlayStage;
class ioSkill;

class ioAreaWeaponManager : public Singleton< ioAreaWeaponManager >
{
protected:
	struct ReserveAreaWeaponInfo
	{
		ioHashString	szOwner;
		ioHashString	szTarget;
		ioHashString	szSkill;
		ioHashString	szAreaWeaponName;
		D3DXVECTOR3		vStartPos;
		D3DXQUATERNION	qtRot;
		int				eCreateType;

		DWORD			dwGenerateTime;

		ReserveAreaWeaponInfo()
		{
			vStartPos	= ioMath::VEC3_ZERO;
			qtRot		= ioMath::QUAT_IDENTITY;
			eCreateType	= 0;
		}
	};
	typedef std::vector<ReserveAreaWeaponInfo> ReserveAreaWeaponInfoList;
	typedef std::vector< ioAreaWeapon* > AreaWeaponList;
	
	class ReserveAreaWeaponInfoSort : public std::binary_function< const ReserveAreaWeaponInfo&, const ReserveAreaWeaponInfo&, bool >
	{
	public:
		bool operator()( const ReserveAreaWeaponInfo &lhs , const ReserveAreaWeaponInfo &rhs ) const
		{
			if( lhs.dwGenerateTime < rhs.dwGenerateTime )
			{
				return true;
			}

			return false;
		}
	};

protected:	
	ioPlayStage*				m_pPlayStage;

	AreaWeaponList				m_vAreaWeaponList;
	AreaWeaponList				m_vActiveAreaWeaponList;
	DWORD						m_dwCurAreaWeaponIndex;

	ReserveAreaWeaponInfoList	m_ReserveAreaWeaponInfoList;

public:
	void LoadAreaWeaponList();

	void  ReserveAreaWeapon( DWORD dwGenerateTime,
							 const ioHashString& szOwner,
							 const ioHashString& szTarget,
							 const ioHashString& szSkill,
							 const ioHashString& szAreaWeaponName,
							 const D3DXVECTOR3 &vStartPos,
							 const D3DXQUATERNION &qtRot,
							 int eCreateType );

	ioAreaWeapon* CreateAreaWeapon( const ioHashString& szOwner,
									const ioHashString& szTarget,
									const ioHashString& szSkill,
									const ioHashString& szAreaWeaponName,
									const D3DXVECTOR3 &vStartPos,
									const D3DXQUATERNION &qtRot,
									int eCreateType );
	void Update();

	void ClearDeadAreaWeaponList();
	void ClearMapAreaWeaponList();
	void ClearAreaWeaponListByOwnerChar( const ioHashString &rkOwner );
	void ClearAllAreaWeaponList();

	void DestroyAreaWeapon( DWORD dwIndex );

	void GetAreaWeaponInfo( SP2Packet &rkPacket );
	void ApplyAreaWeaponInfo( SP2Packet &rkPacket );

	ioAreaWeapon* FindAreaWeaponByIndex( DWORD dwAreaWeaponIndex );
	ioAreaWeapon* FindAttachedAreaWeapon( DWORD dwAreaWeaponIndex );
	ioAreaWeapon* FindAreaWeaponByMapIndex( DWORD dwAreaWeaponMapIndex );
	ioAreaWeapon* FindAreaWeaponByName( const ioHashString &szName );
	ioAreaWeapon* FindAreaWeaponByName( const ioHashString &szName, const ioHashString &rkOwner );
	ioAreaWeapon* FindAreaWeaponInRangeByName( const ioHashString &szName, const ioHashString &rkOwner, D3DXVECTOR3 vPos );
	void FindAllAreaWeaponByName( const ioHashString &szName, std::vector< ioAreaWeapon* >& vAreaWeaponList );

	void CheckCurAreaWeapon();

protected:
	void LoadAreaWeaponInfoList();
	void LoadAreaWeaponNPCInfoList();

public:
	ioPlayStage* GetPlayStage();

protected:
	ioAreaWeapon* FindAreaWeapon( const ioHashString &szName );

public:
	void ProcessCallAreaWeaponInfo();

public:
	static ioAreaWeaponManager& GetSingleton();

public:
	ioAreaWeaponManager( ioPlayStage *pStage );
	virtual ~ioAreaWeaponManager();
};

#define g_AreaWeaponMgr ioAreaWeaponManager::GetSingleton()
