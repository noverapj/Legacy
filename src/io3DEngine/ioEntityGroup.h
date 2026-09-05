

#ifndef _ioEntityGroup_h_
#define _ioEntityGroup_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioMapIterator.h"

#include "ioSphere.h"
#include "ioAxisAlignBox.h"
#include "ioOrientBox.h"
#include "ioMovableObject.h"
#include "ioEntityParent.h"
#include "ioMath.h"

class ioEntity;
class ioRcAnimation;
class ioAnimation;
class ioAniController;
class ioSkeleton;
class ioBiped;
class ioOpcodeShape;
class ioEffect;
class ioDecal;
class ioMeshTrailer;
class ioEdgeRender;

class ioCamera;
class ioRenderQueue;
class ioAniEventHandler;

struct RopeSpringParam;

class __EX ioEntityEvent
{
public:
	virtual void AddEntityEvent( ioEntity *pEntity ){}

public:
	ioEntityEvent(){}
	virtual ~ioEntityEvent(){}
};

class __EX ioEntityGroup : public ioMovableObject, public ioEntityParent
{
public:
	typedef std::vector< ioEntity* > EntityList;
	typedef std::list< ioEntity* > NotBuiledList;

	typedef std::map< ioHashString, ioDecal* > DecalList;

protected:
	EntityList m_EntityList;
	NotBuiledList m_NotBuildEntityList;
	DecalList m_DecalList;

	typedef std::vector< ioRcAnimation* > RcAnimationList;
	RcAnimationList m_RcAnimationList;
	ioAniController *m_pAniController;
	ioSkeleton *m_pSkeleton;
	ioEdgeRender *m_pEdgeRender;

	typedef std::map< ioHashString, RopeSpringParam* > RopeSpringBackUp;
	RopeSpringBackUp m_RopeSpringBackUp;

	typedef std::list< ioEffect* > AttachedEffectList;
	AttachedEffectList m_AttachedEffectList;

	typedef std::list< ioMeshTrailer* > MeshTrailerList;
	MeshTrailerList m_MeshTrailerList;

	ioEntityEvent *m_pParentEvent;

	ioOpcodeShape *m_pOpcodeShape;
	ioOrientBox	  m_CollisionBox;
	ioSphere	  m_CollisionSphere;

	ioHashString m_FileName;
	ioAxisAlignBox m_BoundBox;

	float	m_fCameraDistSq;
	D3DCOLORVALUE m_MeshColor;

	LightReceiveType m_LightReceiveType;
	DWORD	m_dwLightApplyMask;

	DWORD	m_dwLastBipedCachedFrame;

	bool	m_bDestroyDeadEffect;
	bool	m_bSoftwareVertexBlend;
	bool	m_bEnableEachEntityCull;

	ioHashString m_szCustomSkeletonAniName;

	int m_iCurAlphaRate;

public:
	static ioHashString m_sMovableType;

public:
	bool LoadGroup( const char *szFileName, bool bThread = false );

public:
	void SetSkeletonName( const ioHashString &name );
	inline ioSkeleton* GetSkeleton() const { return m_pSkeleton; }

public:
	bool AddAnimation( const ioHashString &name );

	bool SetAniEventHandler( ioAniEventHandler *pHanlder );

public:
	void AddDecal( ioDecal *pDecal );
	void DestroyDecal( const ioHashString &rkName );
	void DestroyAllDecal();

	ioDecal* GetDecal( const ioHashString &rkName ) const;
	ioDecal* GetDecal( int iIndex ) const;
	int GetDecalCnt() const;

public:
	void AddEdgeRender( ioEdgeRender *pEdge );
	void EnableEdgeRender( bool bEnable );
	void RemoveEdgeRender();

	inline ioEdgeRender* GetEdgeRender() const { return m_pEdgeRender; }

public:
	bool AddRopeSpringCurve( const ioHashString &rkBiped, const RopeSpringParam *pParam );
	bool RemoveRopeSpringCurve( const ioHashString &rkBiped );
	void ClearAllRopeSpringCurve();

	void AddWorldForce( const D3DXVECTOR3 &vForce );

public:
	ioMeshTrailer* CreateTrailer();
	void SetGraceCloseAllTrailer();
	void DestroyTrailer( ioMeshTrailer *pTrail );
	void DestroyAllTrailer();

public:
	void SetEventParent( ioEntityEvent *pParentEvent ){ m_pParentEvent = pParentEvent; }
	void AddEntityEvent( ioEntity *pEntity );

public:
	void SetCustomSkeletonAniName( const ioHashString &rkAddAniName ){ m_szCustomSkeletonAniName = rkAddAniName; }
	ioHashString ConvertCustomAniName( const ioHashString &rkAniName );

public:
	ioEntity* AddEntity( const ioHashString &rkMeshName, bool bThread = false );
	ioEntity* GetEntity( const ioHashString &rkMeshName );
	ioEntity* GetEntity( int iIndex );

	inline int GetNumEntities() const { return m_EntityList.size(); }

	bool DestroyEntity( const ioHashString &rkMeshName );
	bool DestroyEntity( int iIndex );
	void DestroyAllEntity();

	void SetEntityVisible( const ioHashString &szMeshName, bool bVisible );
	void HideMeshExceptionCheck( const ioHashString &szMeshName );

	// control animation
public:
	bool SetActionAni( int id,
		float fFadeIn,
		float fFadeOut,
		float fWeight = 1.0f,
		float fTimeFactor = 1.0f,
		float fWaitTime = 0.0f,
		float fPreDelay = 0.0f,
		bool bHoldLastFrame = false,
		bool bUseFX = true );

	bool SetActionAni( const ioHashString &rkName,
		float fFadeIn,
		float fFadeOut,
		float fWeight = 1.0f,
		float fTimeFactor = 1.0f,
		float fWaitTime = 0.0f,
		float fPreDelay = 0.0f,
		bool bHoldLastFrame = false,
		bool bUseFX = true );

	bool SetActionAniForTime( int id,
		float fFadeIn,
		float fFadeOut,
		float fPlayTime,
		float fWeight = 1.0f,
		float fWaitTime = 0.0f );

	bool SetActionAniForTime( const ioHashString &rkName,
		float fFadeIn,
		float fFadeOut,
		float fPlayTime,
		float fWeight = 1.0f,
		float fWaitTime = 0.0f );

	// 현재 플레이중인 액션애니메이션의 시간을 강제 설정
	bool SetPlayActionCurTime( int iId, float fCurTime );
	bool SetPlayActionCurTime( const ioHashString &rkName, float fCurTime );

	bool ClearActionAni( int id, float fFadeOut );
	void ClearAllActionAni( float fFadeOut, bool bWaitAlso = false );


public:
	bool SetLoopAni( int id,
		float fFadeIn,
		float fWeight = 1.0f,
		float fTimeFactor = 1.0f,
		float fWaitTime = 0.0f );

	bool SetLoopAni( const ioHashString &rkName,
		float fFadeIn,
		float fWeight = 1.0f,
		float fTimeFactor = 1.0f,
		float fWaitTime = 0.0f );

	bool SetLoopAniInMid( int id,
		float fStartTime,
		float fFadeIn,
		float fWeight = 1.0f,
		float fTimeFactor = 1.0f );

	bool SetLoopAniInMid( const ioHashString &rkName,
		float fStartTime,
		float fFadeIn,
		float fWeight = 1.0f,
		float fTimeFactor = 1.0f );

	bool ClearLoopAni( int id, float fFadeOut, bool bAlsoWait = false );
	void ClearAllLoopAni( float fFadeOut, bool bWaitAlso = false );
	bool ClearAllLoopAniExceptOne( int iExceptID, float fFadeOut, bool bWaitAlso = false );

	void StopAllPlayingAnimation();
	void ResumeAllPlayingAnimation();

	void ClearAniEvent();
	void ClearAniEffect();

public:
	ioAnimation* GetActionInPlay( const ioHashString &kName );
	ioAnimation* GetActionInPlay( int iIdx );	// 전체 리스트에서의 인덱스
	ioAnimation* GetActionInPlayByOrder( int iOrder );	// 현재플레이중인 GetCurPlayingActionCnt에서의 인덱스
	int GetCurPlayingActionCnt() const;

	ioAnimation* GetLoopInPlay( const ioHashString &rkName );
	ioAnimation* GetLoopInPlay( int iIdx );	// 전체 리스트에서의 인덱스
	ioAnimation* GetLoopInPlayByOrder( int iOrder );	// 현재플레이중인 GetCurPlayingLoopCnt에서의 인덱스
	int GetCurPlayingLoopCnt() const;

	int GetCurPlayingAniCnt() const;

public:
	ioHashString GetAnimationName( int id ) const;
	int GetAnimationIdx( const ioHashString &kName );
	inline int GetAnimationCnt() const { return m_RcAnimationList.size(); }

public:
	float GetAnimationFullTime( int id );

	float GetAnimationEventTime( int id, const ioHashString &szEventType );	
	int GetAnimationEventCnt( int id, const ioHashString &szEventType );

	int GetAnimationEventTimeList( int id,
		const ioHashString &szEventType,
		std::list<float> &vTimeList );

	int GetAnimationEventConstPtrList( int id,
		const ioHashString &szEventType,
		AniEventConstPtrList &vEventList );

	bool IsActionAniPlaying( int id );
	bool IsLoopAniPlaying( int id );

public:
	void SetCollisionBox( const ioOrientBox &rkOBB ) { m_CollisionBox = rkOBB; }
	void SetCollisionSphere( const ioSphere &rkSphere ) { m_CollisionSphere = rkSphere; }
	bool SetCollisionShape( const ioHashString &kFileName );

	const ioOrientBox& GetCollisionBox() const { return m_CollisionBox; }
	const ioSphere& GetCollisionSphere() const { return m_CollisionSphere; }
	ioOpcodeShape* GetOpcodeShape() const { return m_pOpcodeShape; }

public:
	void SetExtraRotate( const ioHashString &kBipedName,
		const D3DXQUATERNION &qtRot,
		bool bLocal );
	void ReleaseExtraRotate( const ioHashString &kBipedName );
	void ReleaseAllExtraRotate();

	ioBiped* GetBiped( const ioHashString &kName );
	ioBiped* GetBiped( int iIndex );

public:
	ioEffect* AttachEffect( ioEffect *pEffect, ioEntityEvent *pEntityEventOwner,
		const D3DXQUATERNION &qtRot = ioMath::QUAT_IDENTITY,
		const D3DXVECTOR3 &vPos = ioMath::VEC3_ZERO,
		bool bDependence=true );

	ioEffect* AttachEffect( const ioHashString &rkFileName, ioEntityEvent *pEntityEventOwner,
		const D3DXQUATERNION &qtRot = ioMath::QUAT_IDENTITY,
		const D3DXVECTOR3 &vPos = ioMath::VEC3_ZERO,
		bool bDependence=true );

	int GetAttachedEffectCnt() const;
	ioEffect* GetAttachedEffect( int iIndex );
	ioEffect* GetAttachedEffectByObjID( UniqueObjID eObjID );
	ioEffect* GetAttachedEffectByFileName( const ioHashString &rkFileName );

	void EndEffect( const ioHashString &rkFileName, bool bGrace, bool bAllEnd = true );
	void EndEffect( int iIndex, bool bGrace );
	void EndEffect( UniqueObjID eObjID, bool bGrace );
	void DestroyAllEffect();

	void EnableDestroyDeadEffect( bool bDestroy );	// for tool
	void DetachEffect( ioEffect *pEff );

protected:
	void DestroyEffectSceneNode( ioEffect *pEffect );
	void DestroyDeadEffect();

protected:
	void UpdateApplyLightList();
	void CacheBipedMatrices();

public:
	inline void SetLightApplyMask( DWORD dwMask ) { m_dwLightApplyMask = dwMask;}
	inline DWORD GetLightApplyMask() const { return m_dwLightApplyMask; }

	void SetSoftwareVertexBlending( bool bSoftware );
	void ReBuildAnimateEntityMeshData();

public:	// ioEntityParent
	virtual void  GetEntityTransform( D3DXMATRIX *pMatWorld ) const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );

	virtual bool IsShadowRecvRender() const;
	virtual bool HasSkeleton() const;

public:	// ioMovableObject
	virtual void SetAlphaRate( int iAlphaRate );
	virtual void SetAttachEffectAlphaRate( int iAlphaRate, bool bMinZero = false );
	virtual void SetLightReceiveType( LightReceiveType eType );

	virtual void Update( float fTime );
	virtual void UpdateBounds( bool bForceUpdate = false );
	virtual void NotifyCurrentCamera( ioCamera *pCamera );
	virtual void UpdateRenderQueue( ioRenderQueue &queue );
	virtual void UpdateGenShadowMapToQueue( ioRenderQueue &queue );

	virtual const ioAxisAlignBox& GetBoundBox() const;
	virtual float GetBoundRadius() const;

	virtual void SetVisible( bool bVisible );
	void SetVisibleNow( bool bVisible, bool bVisibleChangePass );
	void SetVisibleAllMesh( bool bVisible );

public:
	void UpdateAttachedObjects( float fTime );

public:
	void SetFileName( const ioHashString &rkName );
	void SetDiffuseRate( const D3DCOLORVALUE &rkColor );
	void SetMeshColor( float fRed, float fGreen, float fBlue );
	void SetEachEntityCull( bool bEachCull );

protected:
	void CheckEachEntityCulled( ioCamera *pCamera );
	void CheckNotBuildEntity();
	void EraseNotBuildEntity( ioEntity *pEntity );

public:
	inline const D3DCOLORVALUE& GetMeshColor() const { return m_MeshColor; }
	inline const ioHashString& GetFileName() const { return m_FileName; }

public:
	static void GuaranteeAtLeastOneAnimation( bool bEnable );

public: 
	inline int GetAlphaRate() { return m_iCurAlphaRate; }

public:
	ioEntityGroup();
	virtual ~ioEntityGroup();

#ifndef SHIPPING //UJ
#define GetAnimationEventConstPtrList_e	GetAnimationEventConstPtrList
#define GetAnimationEventTimeList_e		GetAnimationEventTimeList
#define GetAnimationEventTime_e			GetAnimationEventTime
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
	__forceinline int GetAnimationEventTimeList_e( int id,
		const char (&szEventType)[N],
		std::list<float> &vTimeList )
	{
		_ENCSTR(szEventType, szEncEventType);
		return GetAnimationEventTimeList(id, szEncEventType, vTimeList);
	}

	template <int N>
	__forceinline float GetAnimationEventTime_e( int id, 
		const char (&szEventType)[N] )
	{
		_ENCSTR(szEventType, szEncEventType);
		return GetAnimationEventTime(id, szEncEventType);
	}

	template <int N>
	__forceinline int GetAnimationEventConstPtrList_e( int id,
		const char (&szEventType)[N],
		AniEventConstPtrList &vEventList )
	{
		_ENCSTR(szEventType, szEncEventType);
		return GetAnimationEventConstPtrList(id, szEncEventType, vEventList);
	}
#endif

};

#endif
