

#ifndef _ioModelParticleSystem_h_
#define _ioModelParticleSystem_h_

#include "io3DCommon.h"
#include "ioStringInterface.h"

class ioRenderQueue;
class ioModelParticle;
class ioModelParticleAffector;
class ioModelParticleIterator;
class ioModelEmitter;
class ioEffect;
class ioCamera;
class ioNode;
class ioEntity;

class __EX ioModelParticleSystem : public ioStringInterface
{
public:
	class CmdQuota : public ioParamCommand
	{
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdCull : public ioParamCommand
	{
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdMeshName : public ioParamCommand
	{
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdEndCondition : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdLimitTime : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdIterationInterval : public ioParamCommand
	{
	public:
		bool IsSkipSave( const void *pTarget ) const;
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdStartCreateParticleCnt : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdFollowParentMode : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdLinkBipedName : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdLinkPosOffset : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdLinkRotOffset : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdRandomSyncIndex : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	static CmdQuota			m_QuotaCmd;
	static CmdCull			m_CullCmd;
	static CmdMeshName		m_MeshNameCmd;
	static CmdEndCondition  m_EndConditionCmd;
	static CmdLimitTime		m_LimitTimeCmd;
	static CmdIterationInterval m_IterationIntervalCmd;
	static CmdStartCreateParticleCnt m_StartCreateParticleCntCmd;
	static CmdFollowParentMode m_FollowParentModeCmd;
	static CmdLinkBipedName m_LinkBipedNameCmd;
	static CmdLinkPosOffset m_LinkPosOffsetCmd;
	static CmdLinkRotOffset m_LinkRotOffsetCmd;
	static CmdRandomSyncIndex m_RandomSyncIndexCmd;

	enum EndCondition
	{
		EC_NEVER_END,
		EC_TIME_LIMIT
	};
	
	enum MPSState
	{
		MPSS_LIVE,
		MPSS_GRACE_CLOSE,
		MPSS_END
	};
	
	enum FollowParentType
	{
		FPT_NONE,
		FPT_POS_ONLY,
		FPT_ROT_ONLY,
		FPT_ALL
	};

protected:
	static float m_sfDefaultIterationInterval;
	static bool  m_sbLimitFullParticleQuota;
	static float m_sfLimitQuotaRate;

protected:
	typedef std::list< ioModelParticle* > ActiveModelList;
	ActiveModelList m_ActiveModelList;

	typedef std::vector< ioModelParticleAffector* > ModelAffectorList;
	ModelAffectorList m_AffectorList;

	ioModelEmitter	*m_pEmitter;
	ioEffect		*m_pParent;

	int	m_iRandomSyncIndex;
	ioHashString m_LinkBipedName;
	D3DXVECTOR3  m_vLinkPosOffset;
	D3DXQUATERNION m_qtLinkRotOffset;

	ioAxisAlignBox  m_BoundBox;
	ioHashString m_MeshName;

	EndCondition m_EndCondition;
	float	m_fCurTime;
	float	m_fEndTime;

	FollowParentType m_FollowMode;
	D3DXMATRIX m_matParentXform;
	D3DXMATRIX m_matStartXform;
	D3DXQUATERNION m_qtStartRotation;

	int		m_iParticleQuota;
	MPSState m_eSystemState;

protected:
	int   m_iLimitQuotaRemain;
	int   m_iLastVisibleFrame;
	float m_fUpdateRemainTime;

	float m_fIterationInterval;
	int	  m_iStartCreateParticleCnt;

	float m_fExtendAlphaRate;

	bool m_bCullIndividual;
	bool m_bIterationIntervalSet;
	bool m_bStartParticleCreated;

	bool m_bEachCulled;

public:
	void SetIterationInterval( float fInterval );
	inline float GetIterationInterval() const { return m_fIterationInterval; }

	static void SetDefaultIterationInterval( float fInterval ) { m_sfDefaultIterationInterval = fInterval; }
	static float GetDefaultIterationInterval() { return m_sfDefaultIterationInterval; }

	static void SetLimitMaxParticleQuota( bool bLimit, float fLimitRate );

protected:
	void InitParameters();
	void ClearActiveParticles();
	ioModelParticle* AddModelParticle();

public:
	void Restart();
	void SetParent( ioEffect *pEffect );

public:
	ioModelEmitter* SetEmitter( const ioHashString &kType );
	void SetMeshName( const ioHashString &rkName );
	void SetCullIndividual( bool bIndividual ) { m_bCullIndividual = bIndividual; }
	void SetFollowParentMode( FollowParentType eType );

	void SetParticleQuota( int iQuota );
	void SetStartCreateParticleCnt( int iCount ) { m_iStartCreateParticleCnt = iCount; }
	void SetEndCondition( EndCondition eCondition ) { m_EndCondition = eCondition; }
	void SetEndTime( float fTime ) { m_fEndTime = fTime; }

	inline void SetState( MPSState eState ) { m_eSystemState = eState; }
	inline MPSState GetState() const { return m_eSystemState; }

public:
	void CreateStartParticles();
	void ScaleActualSize( float fScaleRate );
	void ChangeSystemDirection( const D3DXQUATERNION &qtRotate );

	int GetNumSimulatedMaxParticles();
	int GetNumSimulatedMaxFaceCount();

public:
	ioModelParticleAffector* AddAffector( const ioHashString &kType );
	void RemoveAffector( int iIdx );
	void RemoveAllAffector();

public:
	inline ioModelEmitter* GetEmitter() const { return m_pEmitter; }

	inline int GetNumAffector() const { return m_AffectorList.size(); }
	ioModelParticleAffector* GetAffector( int iIdx ) const;

	inline const ioHashString& GetMeshName() const { return m_MeshName; }
	ioModelParticleIterator GetIterator();

	inline int GetNumCurParticles() const { return m_ActiveModelList.size(); }
	inline int GetParticleQuota() const { return m_iParticleQuota; }
	inline FollowParentType GetFollowParentMode() const { return m_FollowMode; }
	inline int GetStartCreateParticleCnt() const { return m_iStartCreateParticleCnt; }

	inline EndCondition GetEndCondition() const { return m_EndCondition; }
	inline float GetEndTime() const { return m_fEndTime; }

	inline bool IsCullIndividual() const { return m_bCullIndividual; }

	inline void SetRandomSyncIndex( int iIndex ) { m_iRandomSyncIndex = iIndex; }
	inline int  GetRandomSyncIndex() const { return m_iRandomSyncIndex; }

	inline void SetEachCulled( bool bCulled ) { m_bEachCulled = bCulled; }
	inline bool IsEachCulled() const { return m_bEachCulled; }

public:
	inline bool IsEnable() const
	{
		if( m_eSystemState != MPSS_END )
			return true;

		return false;
	}

	bool IsEnableAtNextFrame( float fCurTimeGap );
	
	void SetRandomSeed( unsigned int uSeed );
	unsigned int GetRandomSeed() const;

	void SetExtendAlphaRate( float fRate );

protected:
	void UpdateParentTransform();
	void ExpireParticles( float fTime );
	void TriggerAffector( float fTime );
	void UpdateParticles( float fTime );
	void TriggerEmitter( float fTime );
	void UpdateSystemState( float fTime );

	void ApplyStartParticle( ioModelParticle *pParticle, float fTimePoint );
	int GetNewParticleCount( float fTime );

public:
	void Update( float fTime );
	void UpdateBounds( const D3DXMATRIX &rkInvParent );
	void NotifyCurrentCamera( ioCamera *pCamera );
	void UpdateRenderQueue( ioRenderQueue &queue );

public:
	inline void SetLinkBipedName( const ioHashString &rkName ) { m_LinkBipedName = rkName; }
	inline const ioHashString& GetLinkBipedName() const { return m_LinkBipedName; }

	inline void SetLinkPosOffset( const D3DXVECTOR3 &vPosOffset ) { m_vLinkPosOffset = vPosOffset; }
	inline const D3DXVECTOR3& GetLinkPosOffset() const { return m_vLinkPosOffset; }

	inline void SetLinkRotOffset( const D3DXQUATERNION &qtRotOffset ) { m_qtLinkRotOffset = qtRotOffset; }
	inline const D3DXQUATERNION& GetLinkRotOffset() const { return m_qtLinkRotOffset; }

	inline const ioAxisAlignBox& GetBoundBox() const { return m_BoundBox; }
	inline ioEffect* GetParent() const { return m_pParent; }

	RenderGroupID GetRenderQueueGroup() const;

public:
	inline const D3DXMATRIX* GetTransform() const { return &m_matParentXform; }
	D3DXVECTOR3 GetTrailBipedPosition( const ioHashString &rkName ) const;
	D3DXVECTOR3 GetEmitPosition() const;
	D3DXVECTOR3 GetParentScale() const;

	void TranslateModel( const D3DXVECTOR3 &vMove );

public:
	ioModelParticleSystem& operator=( const ioModelParticleSystem &rhs );
	bool operator==( const ioModelParticleSystem &rhs );
	bool operator!=( const ioModelParticleSystem &rhs );

public:
	ioModelParticleSystem();
	virtual ~ioModelParticleSystem();
};

typedef std::vector< ioModelParticleSystem* > ModelSystemList;

#endif

