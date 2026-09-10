#pragma once

class ioWeapon;

class ioExcavationItem : public ioObjectItem
{
public:
	enum AniType
	{
		AT_NONE            = 0,
		AT_EQUIP           = 1,
		AT_RELEASE         = 2,
		AT_EXCAVATING_STRT = 3,
		AT_EXCAVATING      = 4,
		AT_RESULT_FAIL     = 5,
		AT_RESULT_SUCCESS  = 6,
		AT_RESULT_EQUIP    = 7,
	};

	enum
	{
		ANI_DELAY = 0,
		ANI_RUN,
		ANI_EQUIP,
		ANI_RELEASE,
		ANI_START,
		ANI_DIGGING,
		ANI_FAIL,
		ANI_SUCCESS,
		ANI_JUMP,
		ANI_JUMP_END,
		ANI_MAX,
	};

	struct ToolInfo
	{
		int min_lv;
		int max_lv;

		ioHashString equip_mesh_name;
		ioHashString mesh_name;
		ioHashString ani_name[ANI_MAX];
		ioHashString riding_dummy;
		ioHashString change_skill;

		ToolInfo()
		{
			min_lv = 0;
			max_lv = 100;
			for( int i=0; i<ANI_MAX; ++i )
				ani_name[i].Clear();
		}
		bool EnableTool( int iLevel )
		{
			return COMPARE( iLevel, min_lv, max_lv+1 ) ? true : false;
		}
	};
	typedef std::vector<ToolInfo> vToolInfo;

protected:
	vToolInfo m_vExcavatingTool;
	ToolInfo  m_CurToolInfo;
	DWORD     m_dwEquipTime;
	DWORD     m_dwAnimationEndTime;
	AniType   m_eAniType;
	bool      m_bExcavatingToolMesh;
	ioHashString m_szToolMesh;
	int       m_iRidingDummyCharIndex;
	int       m_iOwnerExcavationLevel;

protected:
	bool SetActionAni( ioBaseChar * pOwner, ioHashString &rszAnimation, float fAnimationRate, bool bLoopAni = false );

public:
	void SetExcavatingStartAni( ioBaseChar *pOwner );
	void SetExcavatingAni( ioBaseChar *pOwner );
	void SetResultFailAni( ioBaseChar *pOwner );
	void SetResultSuccessAni( ioBaseChar *pOwner );
	void SetResultEquipAni( ioBaseChar *pOwner );
	void SetReleaseAni( ioBaseChar *pOwner );

	void SetExcavatingToolMesh( ioBaseChar *pOwner, bool bSet );

	bool IsAnimationEndTime( ioBaseChar *pChar );
	void ClearAniInfo();
	
	ioExcavationItem::AniType GetAniType() const { return m_eAniType; }
	DWORD GetAnimationEndTime() const { return m_dwAnimationEndTime; }
	DWORD GetEquipTime() const { return m_dwEquipTime; }

public:	// overide ioItem
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual ioItem* Clone();
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );

public:
	virtual ObjectSubType GetObjectSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );

private:
	ioHashString GetMeshName( const ioHashString &szName );

public:
	void CheckMeshAndAni( ioBaseChar *pOwner );
	bool CheckChangeMesh();
	int  GetToolInfoIndex( int iExcavationLv );
	void SetRiding( bool bRide, ioBaseChar *pOwner );
	bool IsRiding();

public:
	ioExcavationItem(void);
	ioExcavationItem( const ioExcavationItem &rhs );
	virtual ~ioExcavationItem(void);
};

inline ioExcavationItem* ToExcavationItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_EXCAVATION )
		return NULL;

	return dynamic_cast< ioExcavationItem* >( pObject );
}


