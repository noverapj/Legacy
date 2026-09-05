#pragma once

class ioLight;
class ioEntityGroup;
class ioGameSceneManager;
class ioOpcodeShape;
class ioINILoader;
class ioDecal;
class ioOrientBox;

class ioWorldMesh
{
protected:
	ioGameSceneManager *m_pCreator;

	struct EntityGroupData
	{
		int m_iMapIndex;
		ioEntityGroup *m_pGroup;
	};
	typedef std::vector< EntityGroupData > EntityGroupDataList;
	EntityGroupDataList m_GroupList;

	ioHashStringVec m_LightDecalTextureList;
	
	typedef std::map< ioHashString, D3DXVECTOR3 > DecalLightMap;
	DecalLightMap m_DecalLightMap;
	
	ioAxisAlignBox m_WorldAABB;

public:
	bool LoadWorldMesh( ioINILoader &rkLoader );
	void LoadLightDecalTextureList( ioINILoader &rkLoader );

public:
	void ShadowMapEnable( bool bEnable );
	void MapEdgeEnable( bool bEnable );
	void StopAllTextureEffect( bool bStop );

public:
	void Update();

public:
	void AddOmniLightDecal( int iMapIndex, ioLight *pLight );
	void UpdateEffectLightDecal( int iMapIndex, ioLight *pLight );

protected:
	void BuildOmniLightDecal( int iMapIndex, ioLight *pLight );
	ioDecal* CreateDecal( int iMapIndex, const ioHashString &rkName, const ioOrientBox &rkBound, float fEpsilon = -FLOAT1 );

protected:
	ioEntityGroup *GetEntityGroup( int iMapIndex );

public:
	bool HasLightDecal( int iMapIndex, const ioHashString &rkName );
	void DestroyLightDecal( int iMapIndex, const ioHashString &rkName );
	void DestroyAllLightDecal( int iMapIndex );

public:
	ioOpcodeShape* GetCollisionShape();
	float GetHeight( float fXPos, float fZPos );
	float GetHeight( const D3DXVECTOR3 &vPos );

	inline const ioAxisAlignBox& GetWorldAABB() const { return m_WorldAABB; }

public:
	void SetMultiMapCurrentIndex( int iMapIndex );

public:
	ioWorldMesh( ioGameSceneManager *pSceneMgr );
	virtual ~ioWorldMesh();
};

