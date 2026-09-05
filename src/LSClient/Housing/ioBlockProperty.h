#pragma once
#include "BlockDefine.h"

class ioPlayMode;
class ioINILoader;
class ioBlockManager;

class ioBlockProperty
{
friend class ioBlockManager;
public:
	enum ShadowType
	{
		ST_NONE,	//그림자를 쓰지 않음
		ST_CAST,	//그림자를 생성
		ST_RECIVE,	//타 오브젝트의 그림자를 받음
	};

	enum ItemCategory
	{
		IC_NONE		= 0,		
		IC_BLOCK    = 1,
		IC_TILE     = 2,
		IC_DECO		= 3,
		IC_FUNCTION	= 4,

	};

	enum BlockAlphaType
	{
		BAT_OFF,
		BAT_ON,
	};

	enum InterPlayType
	{
		IPT_NONE,
		IPT_ACTION,
		IPT_LOOP,
	};

	//상호작용
	struct InterPlayInfo
	{
		InterPlayType	m_eInterPlayType;

		ioHashString	m_PreAnimation;
		ioHashString	m_ActionAnimation;
		ioHashString	m_LoopAnimation;
		ioHashString	m_EndAnimation;

		float			m_fPreAnimationRate;
		float			m_fActionAnimationRate;
		float			m_fLoopAnimationRate;
		float			m_fEndAnimationRate;
		float			m_fWidth;
		float			m_fUnderHeight;
		float			m_fUpHeight;
		float			m_fZoneRange;
		float			m_fCharRotate;

		D3DXVECTOR3		m_vZoneOffset;		

		ioHashString	m_EmoticonMent;
		DWORD			m_EmoticonMentColor;

		InterPlayInfo()
		{
			m_eInterPlayType		= IPT_NONE;

			m_fPreAnimationRate		= 0.0f;
			m_fActionAnimationRate	= 0.0f;
			m_fLoopAnimationRate	= 0.0f;
			m_fEndAnimationRate		= 0.0f;
			m_fWidth				= 0.0f;
			m_fUnderHeight			= 0.0f;
			m_fUpHeight				= 0.0f;
			m_fZoneRange			= 0.0f;
			m_fCharRotate			= 0.0f;
			m_vZoneOffset			= ioMath::VEC3_ZERO;
			m_EmoticonMentColor		= 0;
		}
	};
	typedef std::vector<InterPlayInfo> InterPlayInfoList;

	//AreaWeapon
	struct BlockAreaWeapon
	{
		ioHashString	m_AreaWeaponName;
		float			m_fCreateAngle;
		D3DXVECTOR3		m_vPos;

		BlockAreaWeapon()
		{
			m_fCreateAngle = 0.0f;
		}
	};
	typedef std::vector<BlockAreaWeapon> BlockAreaWeaponList;

protected:
	int					m_nCode;	
	int					m_nOrder;

	ShadowType			m_eShadowType;
	ItemCategory		m_eItemCategory;
	BlockAlphaType		m_eBlockAlphaType;

	int					m_nWidth;
	int					m_nDepth;
	int					m_nHeight;

	ioHashString		m_Name;
	ioHashString		m_MeshFileName;
	ioHashString		m_IconName;
		
	TilePosList			m_CollTilePosList;
	byte				m_Rotate;
	D3DCOLORVALUE		m_DisableColor;
	D3DCOLORVALUE		m_SelectColor;
	InterPlayInfoList	m_InterPlayInfoList;
	BlockAreaWeaponList	m_BlockAreaWeaponList;

public:
	virtual bool LoadBlockProperty( ioINILoader &rkLoader );

	void LoadInterPlay( ioINILoader &rkLoader );
	void LoadAreaWeapon( ioINILoader &rkLoader );

protected:	
	void SetCode( int nCode ){ m_nCode = nCode; }

public:
	int GetCode() const { return m_nCode; }
	int GetOrder() const { return m_nOrder; }

	ioBlockProperty::ShadowType GetShadowType() const { return m_eShadowType; }
	ioBlockProperty::ItemCategory GetItemCategory() const { return m_eItemCategory; }
	ioBlockProperty::BlockAlphaType GetBlockAlphaType() const { return m_eBlockAlphaType; }

	void GetCollTileList( OUT TilePosList& List );

	const ioHashString& GetName() const { return m_Name; }
	const ioHashString& GetMeshName() const { return m_MeshFileName; }
	const ioHashString& GetIconName() const { return m_IconName; }

	const D3DCOLORVALUE& GetDisableColor(){ return m_DisableColor; }
	const D3DCOLORVALUE& GetSelectColor(){ return m_SelectColor; }

public:
	int GetWidth() const { return m_nWidth; }
	int GetDepth() const { return m_nDepth; }
	int GetHeight() const { return m_nHeight; }

public:
	const ioBlockProperty::InterPlayInfo* GetInterPlayInfo( int nIndex ) const;
	const ioBlockProperty::InterPlayInfoList& GetInterPlayInfoList() const;

public:
	const ioBlockProperty::BlockAreaWeaponList& GetBlockAreaWeaponList() const;

protected:
	void UpdateCollTileSize();

public:
	virtual ioBlockProperty* Clone();

public:
	ioBlockProperty();
	ioBlockProperty( const ioBlockProperty& rhs );
	~ioBlockProperty();
};