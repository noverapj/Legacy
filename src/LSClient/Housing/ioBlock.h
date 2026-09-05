#pragma once

#include "../ioPlayEntity.h"

#include "BlockDefine.h"
#include "ioBlockProperty.h"

class ioPlayMode;
class ioINILoader;
class ioBlockManager;
class ioOpcodeShape;
class ioBlockWorld;

typedef std::vector<ioOrientBox> ioOrientBoxList;

class ioBlock : public ioPlayEntity
{
friend class ioBlockManager;

public:
	enum BlockType
	{
		BT_DIVIDE	= 1000000,
		BT_DEFAULT	= 1,
		BT_TILE		= 2,
		BT_FISHING	= 3,
	};

	enum ColorSetState
	{
		CSS_NONE,
		CSS_DISABLE,
		CSS_SELECT,
	};

	enum FadeState
	{
		FS_NONE,
		FS_IN,
		FS_OUT,
	};

	struct ColorSet
	{
		D3DCOLORVALUE kAmbientColor;
		D3DCOLORVALUE kDiffuseColor;
		D3DCOLORVALUE kSpecularColor;
		D3DCOLORVALUE kEmissiveColor;
	};

protected:
	__int64				m_nUniqueIndex;

protected:
	ioBlockProperty*	m_Property;	

protected:
	TilePos				m_Pos;

	ioSphere			m_CollSphere;	
	ioSphere			m_CollBlockSphere;
	
	ioOrientBoxList		m_TileCollBoxList;
	TilePosList			m_CurrPosList;
	D3DXQUATERNION		m_CollRot;
	float				m_fRotate;

protected:
	ColorSetState		m_ColorSetState;
	ColorSet			m_BackUpColor;
	FadeState			m_FadeState;
	float				m_fCurrFadeRate;
	DWORD				m_dwRateTime;
	
protected:
	bool				m_bCollChecked;
	D3DXMATRIX			m_FullTransform;
	
protected:
	ioOrientBoxList		m_InterPlayBoxList;
	ioHashString		m_InterPlayCharName;
	bool				m_bDisableInterPlay;

protected:
	IntVec				m_LiveAreaWeaponList;
	bool				m_bDisableAreaWeapon;

public:
	virtual ioBlock::BlockType GetBlockType() const { return ioBlock::BT_DEFAULT; }
	virtual ioBlockProperty* GetProperty(){ return m_Property; }
	virtual float GetRotate(){ return m_fRotate; }

public:
	void UpdateBound();
	const D3DXQUATERNION& GetCollRotate(){ return m_CollRot; }

public:
	void SetTitleCoord( int XPos, int YPos, int ZPos, bool bCallUdateBound = true );
	void SetCollBoxRotaion( D3DXQUATERNION qtRot );
	
	int GetCellPos() const;
	int GetCellLevel() const;

public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );

	virtual bool IsMountAvailable() const;
	virtual bool IsMountingAvailable() const;
	virtual bool IsNowMountEnable() const;

	virtual void Translate( const D3DXVECTOR3 &vMove );

protected:
	virtual void UpdateBottomHeight() const;

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );

	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

public:
	virtual int  DontMoveEntityLevel() const;	
	
public:
	virtual ioOpcodeShape* GetCollShape();
	virtual const ioSphere& GetCollSphere();
	virtual const ioSphere& GetCollBlockSphere();

public:
	ioBlock::ColorSetState IsColorState();

	void ChangeDisable();
	void ChangeSelect();
	void ChangeNone();
		
	void ChangeColor( IN const ColorSet& kColor );
	void BackUpColor( OUT ColorSet& kBackUpColor );

	void FadeIn();
	void FadeOut();
	void UpdateFade();

public:
	void SetShadowCastEnable( bool bCast );
	void SetShadowRecvEnable( bool bRecv );
	void AddShadowRecvPass();

	void EnableMapEdge( bool bEnable );
	void StopAllTextureEffect( bool bStop );

	void SetRenderQueueGroup( RenderGroupID eGroupID );

public:
	inline __int64 GetUniqueIndex(){ return m_nUniqueIndex; }
	const TilePos& GetTilePos() const { return m_Pos; }

public:
	virtual void Rotate( float fRotate );
	virtual void RotateCoord( IN float fRotate, IN TilePosList& List ) const;

public:
	virtual void UpdateCollBox();
	virtual void UpdateCollBox( IN TilePosList& CollList, OUT ioOrientBoxList& CollBoxList, OUT ioSphere& CollSphere, OUT ioSphere& CollHalfSphere ) const;
	virtual void GetCollBoxList( ioOrientBoxList& CollList );
	virtual void GetCollInterPlayBoxList( ioOrientBoxList& CollList );
	virtual void GetPosList( TilePosList& PosList ) const;
		
	virtual bool TestIntersection( IN const ioRay& kYRay, OUT D3DXVECTOR3* pCol =NULL ) const;
	virtual bool TestIntersection( IN const ioOrientBox& kBox ) const;
	virtual bool TestIntersection( IN const TilePos& Pos ) const;
	virtual bool TestIntersection( IN const TilePosList& List ) const;
	virtual bool TestPreIntersection( IN const ioBlock* pBlock, IN const TilePos& NextPos, IN float fNextRotate ) const;
	bool ioBlock::TestInterPlayIntersection( IN ioPlayEntity* pEntity, OUT int& nCollIndex, OUT D3DXVECTOR3& vCollPos ) const;

	virtual bool IsSetUp() const;

public:
	void ToCollWorldPos( TilePos& Pos );

public:
	void DisableInterPlay();
	void CreateInterPlayZone();
	void SetInterPlayChar( const ioHashString& CharName){ m_InterPlayCharName = CharName; }
	const ioHashString& GetInterPlayChar(){ return m_InterPlayCharName; }

public:
	void DisableAreaWeapon();
	void DestroyAreaWeapon();
	void CreateAreaWeapon();

public:
	void RenderBlockAABB( CollRenderType eType, ioPlayStage* pStage, ioCamera* pCamera );
	void TestBlock( IN const TilePosList& SrcPosList );

public:
	const D3DXMATRIX* GetTransform(){ return &m_FullTransform; }

public:
	inline bool IsCollChecked(){ return m_bCollChecked; }
	inline void SetCollChecked( bool bColl ){ m_bCollChecked = bColl; }

public:
	ioBlock( ioEntityGroup *pGrp, ioPlayMode *pMode, ioBlockProperty* pProperty, __int64 nUniqueIndex );
	virtual ~ioBlock();
};

inline ioBlock* ToBlock( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_HOUSING_BLOCK )
		return NULL;

	return dynamic_cast< ioBlock* >( pPlay );
}

inline const ioBlock* ToBlockConst( const ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_HOUSING_BLOCK )
		return NULL;

	return dynamic_cast< const ioBlock* >( pPlay );
}

typedef std::vector< ioBlock* > ioBlockList;

class ioBlockListSort : public std::binary_function< const ioBlock* , const ioBlock*, bool >
{
public:
	bool operator() ( const ioBlock* plhs, const ioBlock* prhs ) const
	{
		if( plhs && prhs )
		{
			if( plhs->GetCellLevel() < prhs->GetCellLevel() )
				return false;

			if( plhs->GetCellPos() < prhs->GetCellPos() )
				return false;

			return true;
		}

		return false;
	}
};
