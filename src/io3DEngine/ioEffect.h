

#ifndef _ioEffect_h_
#define _ioEffect_h_

#include "io3DCommon.h"
#include "ioOrientBox.h"
#include "ioStringInterface.h"
#include "ioMovableObject.h"

#include "ioParticleSystem.h"
#include "ioModelParticleSystem.h"

class ioNode;
class ioSphere;
class ioAxisAlignBox;
class ioCamera;
class ioRenderQueue;
class ioEntityGroup;
class ioEntityEvent;
class ioSkeleton;
class ioAnimation;
class ioParamCommand;
class ioEffectFactory;

struct ioLightSystem;

class __EX ioEffect : public ioStringInterface, public ioMovableObject
{
public:
	class CmdCollisionBox : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class CmdSkeletonName : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdAnimationName : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdEffectDirection : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdCullEachSystem : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdUpdateSystemAlways : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

public:
	static CmdCollisionBox		 m_CollisionBoxCmd;
	static CmdSkeletonName		 m_SkeletonCmd;
	static CmdAnimationName		 m_AnimationCmd;
	static CmdEffectDirection	 m_EffectDirectionCmd;
	static CmdCullEachSystem	 m_CullEachSystemCmd;
	static CmdUpdateSystemAlways m_UpdateSystemAlwaysCmd;

protected:
	ParticleSystemList m_ParticleGrp;
	ModelSystemList m_ModelGrp;

	ioHashString   m_FileName;
	D3DXVECTOR3	   m_vEffectDirection;

	ioEntityGroup  *m_pAttachedGrp;
	ioSkeleton	   *m_pSkeleton;
	ioAnimation	   *m_pAnimation;
	bool m_bAnimationLoop;
	bool m_bCullEachSystem;
	bool m_bAlwaysUpdateSystem;

	ioOrientBox		m_CollisionBox;
	ioAxisAlignBox	m_BoundBox;

	float m_fEffectStopWaitTime;
	float m_fEffectStopDuration;
	float m_fEffectStopLooseRate;

	float m_fSquaredViewDepth;

	ioLightSystem *m_pLightSys;
	DWORD m_dwLightCheckTime;

	float		m_fWaitTime;
	float		m_fRemainWaitTime;
	bool		m_bLive;
	bool		m_bTryedLightCreation;

	ioEntityEvent *m_pEntityEventOwner;

public:
	static ioHashString m_sMovableType;
	static bool m_sbEnableEffectLightCreation;


protected:
	void InitParameters();
	void InitializeRandomSeed();

public:
	void InitResource();
	void Restart();

	void AddParticleSys( ioParticleSystem *pSys );
	void DestroyParticleSys( int iIdx );

	void AddModelSystem( ioModelParticleSystem *pSys );
	void DestroyModelSystem( int iIdx );

	void DestroyAllSystem();

	void ChangeParticleSysIndex( int iSrc, int iDest );
	void ChangeModelSysIndex( int iSrc, int iDest );

	int GetNumSimulatedMaxFaces();

	void AddEntityEvent( ioEntity *pEntity );

public:
	void SetWaitTime( float fWaitTime );
	void SetExtendColorRate( float fRate, bool bMinZero = false );

	void ReserveEffectStop( float fStopWaitTime, float fStopDuration, float fLooseRate );

	void EndEffectForce();
	void EndEffectGrace();

	void ScaleActualSize( float fScaleRate );
	void ChangeEffectDirection( const D3DXVECTOR3 &vDirection, bool bTypeChange );

	void SetWorldEffectEventOwner( ioEntityEvent *pEntityEventOwner ){ m_pEntityEventOwner = pEntityEventOwner; }
	ioEntityEvent *GetWorldEffectEventOwner(){ return m_pEntityEventOwner; }

public:
	inline ioLightSystem* GetLightSys() { return m_pLightSys; }
	ioLightSystem* CreateLightSystem();
	void DestroyLightSystem();
	void DestroyCreatedLight();

	static void EnableEffectLightCreation( bool bEnable );

protected:
	void  CheckCullEachSystem( ioCamera *pCamera );
	float CheckEffectStop( float fTime );
	void  UpdateSkeleton( float fTime );

	void CheckCreateLightState( float fTime );
	void MakeLight();

public:
	ioParticleSystem* GetParticleSys( int iIdx );
	inline int GetParticleSystemCnt() const { return m_ParticleGrp.size(); }

	ioModelParticleSystem* GetModelSys( int iIdx );
	inline int GetModelSystemCnt() const { return m_ModelGrp.size(); }

public:
	bool HasTimeLimit() const;
	bool IsLiveAtNextFrame( float fCurTimeGap );

	inline bool IsLive() const { return m_bLive; }

public:
	void SetFileName( const ioHashString &kFileName );
	void SetCollisionBox( const ioOrientBox &rkBox );
	void SetEffectDirection( const D3DXVECTOR3 &vDirection );
	void SetCullEachSystem( bool bCullEach );
	void SetAlwaysUpdateSystem( bool bAlways );

	inline const ioHashString& GetFileName() const { return m_FileName; }
	inline const ioOrientBox& GetCollisionBox() const { return m_CollisionBox; }
	inline const D3DXVECTOR3& GetEffectDirection() const { return m_vEffectDirection; }
	inline bool IsCullEachSystem() const { return m_bCullEachSystem; }
	inline bool IsAlwaysUpdateSystem() const { return m_bAlwaysUpdateSystem; }

public:
	void SetSkeleton( const ioHashString &rkName );
	void SetAnimation( const ioHashString &rkName, bool bLoop );

	void ClearSkeletonAndAnimation();
	void ClearAnimation();

	ioSkeleton* GetSkeleton() const;
	ioHashString GetSkeletonName() const;
	ioHashString GetAnimationName() const;
	inline bool IsAnimationLoop() const { return m_bAnimationLoop; }

	bool HasLinkedSystem() const;
	bool GetFirstLinkOffset( ioHashString &szBiped,
						 	 D3DXVECTOR3 &vPosOffset,
							 D3DXQUATERNION &qtRotOffset );

public:
	void SetAttachedEntityGrp( ioEntityGroup *pGrp ) { m_pAttachedGrp = pGrp; }

	D3DXVECTOR3 GetDerivedBipedScale( const ioHashString &rkBiped ) const;
	D3DXVECTOR3 GetDerivedBipedPos( const ioHashString &rkBiped, const D3DXVECTOR3 &vPosOffset ) const;
	D3DXQUATERNION GetDerivedBipedOrientation( const ioHashString &rkBiped, const D3DXQUATERNION &qtRotOffset ) const;

	bool GetDerivedBipedTransform( const ioHashString &rkBiped,
								   const D3DXVECTOR3 &vPosOffset,
								   const D3DXQUATERNION &qtRotOffset,
								   D3DXMATRIX *pMat ) const;

	void TranslateParticle( const D3DXVECTOR3 &vMove );

public:
	ioEffect& operator=( const ioEffect &rhs );
	bool operator==( const ioEffect &rhs ) const;
	bool operator!=( const ioEffect &rhs ) const;

public:	// overide ioMovableObject
	virtual void Update( float fTime );
	virtual void UpdateBounds( bool bForceUpdate = false );
	virtual void NotifyCurrentCamera( ioCamera *pCamera );
	virtual void UpdateRenderQueue( ioRenderQueue &queue );

	virtual void NotifyAttached( ioNode *pParent );

	virtual const ioAxisAlignBox& GetBoundBox() const;
	virtual float GetBoundRadius() const;

private:
	friend class ioEffectFactory;

private:
	ioEffect();
	virtual ~ioEffect();
};

#endif
