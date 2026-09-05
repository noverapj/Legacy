

#ifndef _ioParticleSystem_h_
#define _ioParticleSystem_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioStringInterface.h"
#include "ioAxisAlignBox.h"

class ioParamCommand;

class ioParticle;
class ioParticleIterator;
class ioParticleEmitter;
class ioParticleAffector;

class ioCamera;
class ioEffect;
class ioMaterial;

#include "ioTPtrArray.h"

class __EX ioParticleSystem : public ioStringInterface
{
public:
	class CmdQuota : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdCull : public ioParamCommand
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
	class CmdDefaultWidth : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdDefaultHeight : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdRenderType : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdTextureName : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdBillboardType : public ioParamCommand
	{
	public:
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
	class CmdCommonDir : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	class CmdCommonDirNoneBillboard : public ioParamCommand
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
	class CmdDepthCheck : public ioParamCommand
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
	class CmdModulate2X : public ioParamCommand
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
	class CmdTexFiltering : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	static CmdQuota			m_QuotaCmd;
	static CmdCull			m_CullCmd;
	static CmdFollowParentMode m_FollowParentModeCmd;
	static CmdDefaultWidth  m_DefaultWidthCmd;
	static CmdDefaultHeight m_DefaultHeightCmd;
	static CmdRenderType    m_RenderTypeCmd;
	static CmdTextureName	m_TextureNameCmd;
	static CmdBillboardType m_BillboardTypeCmd;
	static CmdEndCondition  m_EndConditionCmd;
	static CmdLimitTime		m_LimitTimeCmd;
	static CmdCommonDir		m_CommonDirCmd;
	static CmdCommonDirNoneBillboard m_CommonDirNoneBillboardCmd;
	static CmdIterationInterval m_IterationIntervalCmd;
	static CmdStartCreateParticleCnt m_StartCreateParticleCnt;
	static CmdDepthCheck	m_DepthCheckCmd;
	static CmdLinkBipedName m_LinkBipedNameCmd;
	static CmdLinkPosOffset m_LinkPosOffsetCmd;
	static CmdLinkRotOffset m_LinkRotOffsetCmd;
	static CmdModulate2X    m_Modulate2XCmd;
	static CmdRandomSyncIndex m_RandomSyncIndexCmd;
	static CmdTexFiltering  m_TexFilteringCmd;
	
	enum BillBoardType
	{
		// Standard Billboard
		BBT_POINT,
		// Billboard ar oriented around a shared direction vector ( used as Y axis )
		BBT_ORIENTED_COMMON,
		// Billboard ar oriented around their move direction vector ( m_vDir )
		BBT_ORIENTED_DIRECTION,
		/// Billboards are oriented around their own direction vector (their own Y axis) and only rotate around this to face the camera
        BBT_ORIENTED_SELF
	};

	enum EndCondition
	{
		EC_NEVER_END,
		EC_TIME_LIMIT
	};

	enum ParticleSystemState
	{
		PSS_LIVE,
		PSS_GRACE_CLOSE,
		PSS_END
	};
	
	enum FollowParentType
	{
		FPT_NONE,
		FPT_POS_ONLY,
		FPT_ALL
	};

protected:
	static float m_sfDefaultIterationInterval;
	
	static bool  m_sbLimitFullParticleQuota;
	static float m_sfLimitQuotaRate;

protected:
	ioEffect	*m_pParent;
	ioParticleEmitter  *m_pEmitter;

	typedef std::list<ioParticle*>	 ActiveParticleList;
	ActiveParticleList m_ActiveParticle;

	typedef std::vector< ioParticleAffector* > ParticleAffectorList;
	ParticleAffectorList m_AffectorList;

	ioTPtrArray<ioParticle> m_VisibleArray;

	ioMaterial	  *m_pMtl;
	ioHashString  m_TextureName;
	BillBoardType m_BillboardType;
	FXRenderType  m_RenderType;
	TextureFilterOption m_TexFiltering;
	D3DXVECTOR3	  m_vCommonDir;

	D3DXVECTOR3  m_vCameraDir;

	ParticleSystemState m_eSystemState;
	int		m_iParticleQuota;

	ioAxisAlignBox	m_BoundBox;
	float m_fBillMinWidth, m_fBillMaxWidth;
	float m_fBillMinHeight, m_fBillMaxHeight;

	EndCondition m_EndCondition;
	float	m_fCurTime;
	float	m_fEndTime;

	D3DXMATRIX  m_matViewInv;
	D3DXVECTOR3 m_vCameraX;
	D3DXVECTOR3 m_vCameraY;
	float m_fCameraSqDist;

	ioHashString m_LinkBipedName;
	D3DXVECTOR3	   m_vLinkPosOffset;
	D3DXQUATERNION m_qtLinkRotOffset;

	FollowParentType m_FollowParentMode;
	D3DXMATRIX m_matParentXform;

	int		m_iRandomSyncIndex;

	bool	m_bAutoExtendPool;
	bool	m_bCullIndividual;
	bool	m_bDepthCheck;
	bool	m_bModulate2X;
	bool	m_bCommonDirNoneBillboard;

protected:
	int   m_iLimitQuotaRemain;
	int   m_iLastVisibleFrame;
	float m_fUpdateRemainTime;

	float m_fIterationInterval;
	int	  m_iStartCreateParticleCnt;

	float m_fExtendColorRate;

	bool m_bIterationIntervalSet;
	bool m_bStartParticleCreated;
	bool m_bEachCulled;

public:
	void SetIterationInterval( float fInterval );
	inline float GetIterationInterval() const { return m_fIterationInterval; }

	static void SetDefaultIterationInterval( float fInterval ) { m_sfDefaultIterationInterval = fInterval; }
	static float GetDefaultIterationInterval() { return m_sfDefaultIterationInterval; }

	static void SetLimitMaxParticleQuota( bool bLimit, float fLimitRate );

public:
	void Restart();

	void SetParent( ioEffect *pParent );
	ioParticleEmitter* SetEmitter( const ioHashString &kType );
	ioParticleEmitter* GetEmitter() const { return m_pEmitter; }

	ioParticleAffector* AddAffector( const ioHashString &kType );
	void RemoveAffector( int iIdx );
	void RemoveAllAffector();

	ioParticleAffector* GetAffector( int iIdx ) const;
	inline int GetNumAffector() const { return m_AffectorList.size(); }

public:
	void CreateStartParticles();
	void ScaleActualSize( float fScaleRate );
	void ChangeSystemDirection( const D3DXQUATERNION &qtRotate, bool bTypeChange );

protected:
	void InitParameters();
	void ClearActiveParticles();

	void UpdateParentTransform();
	void ExpireParticles( float fTime );
	void TriggerAffector( float fTime );
	void UpdateParticles( float fTime );
	void TriggerEmitter( float fTime );
	void UpdateSystemState( float fTime );

	ioParticle* AddParticle();
	void ApplyStartParticle( ioParticle *pParticle, float fTimePoint );
	int GetNewParticleCount( float fTime );

public:
	void SetParticleQuota( int iQuota );
	inline int GetParticleQuota() const { return m_iParticleQuota; }

	inline void SetAutoExtend( bool bAuto ) { m_bAutoExtendPool = bAuto; }
	inline bool GetAutoExtend() const { return m_bAutoExtendPool; }

	inline void SetCullIndividual( bool bIndividual ) { m_bCullIndividual = bIndividual; }
	inline bool GetCullIndividual() const { return m_bCullIndividual; }

	void SetDepthCheck( bool bCheck );
	inline bool GetDepthCheck() const { return m_bDepthCheck; }

	void SetModulate2X( bool bModulat2X );
	inline bool GetModulate2X() const { return m_bModulate2X; }

	inline void SetFollowParentMode( FollowParentType eType ) { m_FollowParentMode = eType; }
	inline FollowParentType GetFollowParentMode() const { return m_FollowParentMode; }

	void SetBeginWidth( float fMin, float fMax );
	void SetBeginHeight( float fMin, float fMax );

	inline float GetBillMinWidth() const { return m_fBillMinWidth; }
	inline float GetBillMaxWidth() const { return m_fBillMaxWidth; }
	inline float GetBillMinHeight() const { return m_fBillMinHeight; }
	inline float GetBillMaxHeight() const { return m_fBillMaxHeight; }

	float GenBeginWidth() const;
	float GenBeginHeight() const;

	inline void SetCommonDirection( const D3DXVECTOR3 &vDir ) { m_vCommonDir = vDir; }
	inline const D3DXVECTOR3& GetCommonDirection() const { return m_vCommonDir; }

	inline void SetCommonDirNoneBillboard( bool bNone ) { m_bCommonDirNoneBillboard = bNone; }
	inline bool GetCommonDirNoneBillboard() const { return m_bCommonDirNoneBillboard; }

	inline void SetBillboardType( BillBoardType bbType ) { m_BillboardType = bbType; }
	inline BillBoardType GetBillboardType() const { return m_BillboardType; }

	void SetRenderType( FXRenderType eType );
	inline FXRenderType GetRenderType() const { return m_RenderType; }

	void SetTexFiltering( TextureFilterOption eOption );
	inline TextureFilterOption GetTexFiltering() const { return m_TexFiltering; }

	inline void SetEndCondition( EndCondition eCondition ) { m_EndCondition = eCondition; }
	inline EndCondition GetEndCondition() const { return m_EndCondition; }

	inline void SetEndTime( float fTime ) { m_fEndTime = fTime; }
	inline float GetEndTime() const { return m_fEndTime; }

	inline void SetStartCreateParticleCnt( int iCount ) { m_iStartCreateParticleCnt = iCount; }
	inline int  GetStartCreateParticleCnt() const { return m_iStartCreateParticleCnt; }

	inline void SetState( ParticleSystemState eState ) { m_eSystemState = eState; }
	inline ParticleSystemState GetState() const { return m_eSystemState; }

	void SetTexture( const ioHashString &name );
	inline const ioHashString& GetTextureName() const { return m_TextureName; }

	void SetLinkBipedName( const ioHashString &name );
	inline const ioHashString& GetLinkBipedName() const { return m_LinkBipedName; }

	inline void SetLinkPosOffset( const D3DXVECTOR3 &vPosOffset ) { m_vLinkPosOffset = vPosOffset; }
	inline const D3DXVECTOR3& GetLinkPosOffset() const { return m_vLinkPosOffset; }

	inline void SetLinkRotOffset( const D3DXQUATERNION &qtRotOffset ) { m_qtLinkRotOffset = qtRotOffset; }
	inline const D3DXQUATERNION& GetLinkRotOffset() const { return m_qtLinkRotOffset; }

	inline void SetRandomSyncIndex( int iIndex ) { m_iRandomSyncIndex = iIndex; }
	inline int  GetRandomSyncIndex() const { return m_iRandomSyncIndex; }

	inline int GetNumActiveParticles() const { return m_ActiveParticle.size(); }
	inline int GetNumVisibleParticles() const { return m_VisibleArray.GetCount(); }

	inline void SetEachCulled( bool bCulled ) { m_bEachCulled = bCulled; }
	inline bool IsEachCulled() const { return m_bEachCulled; }

public:
	inline bool IsEnable() const
	{
		if( m_eSystemState != PSS_END )
			return true;

		return false;
	}

	bool IsEnableAtNextFrame( float fCurTimeGap );
	
	void SetRandomSeed( unsigned int uSeed );
	unsigned int GetRandomSeed() const;

	void BuildMaterial();
	void BuildColorTable();
	void UpdateColorTable();

	void SetExtendColorRate( float fRate, bool bMinZero = false );
	int  GetNumSimulatedMaxParticles();

protected:
	bool IsVisibleParticle( ioCamera *pCamera, ioParticle *pParticle );

	void GenBillboardAxis( const D3DXVECTOR3 &vCamDir,
						   D3DXVECTOR3 *pX,
						   D3DXVECTOR3 *pY,
						   ioParticle *pParticle = NULL );

	void GenVertex( const ioParticle *pParticle, ioVtxColorTex *pVtx );
	void GenVertexParentXform( const ioParticle *pParticle, ioVtxColorTex *pVtx );

	void CreateBuffers();

public:
	void Update( float fTime );
	void UpdateBounds( const D3DXMATRIX &rkInvParent );
	
	void NotifyCurrentCamera( ioCamera *pCamera );
	int  FillParticleBuffer( ioVtxColorTex *pDest );

public:
	inline const ioAxisAlignBox& GetBoundBox() const { return m_BoundBox; }
	D3DXVECTOR3 GetTrailBipedPosition( const ioHashString &rkName ) const;
	D3DXVECTOR3 GetEmitPosition() const;

public:
	inline ioEffect* GetParent() const { return m_pParent; }
	inline ioMaterial* GetMaterial() const { return m_pMtl; }

	RenderGroupID GetRenderQueueGroup() const;
	ioParticleIterator GetIterator();

	bool HasSameMaterialProperty( ioParticleSystem *pRhs );

	void TranslateParticle( const D3DXVECTOR3 &vMove );

public:
	ioParticleSystem& operator=( const ioParticleSystem &rhs );
	bool operator==( const ioParticleSystem &rhs ) const;
	bool operator!=( const ioParticleSystem &rhs ) const;

public:
	ioParticleSystem();
	virtual ~ioParticleSystem();
};

typedef std::vector< ioParticleSystem* > ParticleSystemList;

#endif
