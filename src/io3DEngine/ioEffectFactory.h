

#ifndef _ioEffectFactory_h_
#define _ioEffectFactory_h_

#include "io3DCommon.h"
#include "ioResourceManager.h"

class ioTextStream;

class ioParticleEmitter;
class ioParticleAffector;
class ioParticleSystem;
class ioModelParticleSystem;
class ioModelEmitter;
class ioModelParticleAffector;
class ioSceneManager;
class ioEntityEvent;
class ioStringInterface;
class ioEmitPointGenerator;
class ioEffectBufferManager;

class ioEffect;
struct ioLightSystem;

class __EX ioEffectFactory : public Singleton< ioEffectFactory >
{
protected:
	typedef std::map< UniqueObjID, ioEffect* > EffectMap;
	EffectMap m_EffectMap;

	typedef std::map< ioHashString, ioEffect* > EffectTempleteMap;
	EffectTempleteMap m_EffectTempleteMap;

	struct EffectTimeStamp
	{
		DWORD	dwTime;
		int		iRefCount;

		EffectTimeStamp()
		{
			dwTime		= 0;
			iRefCount	= 0;
		}
	};
	typedef std::map< ioHashString, EffectTimeStamp > EffectTempleteTimeMap;
	EffectTempleteTimeMap m_EffectTempleteTimeMap;

	ioEffectBufferManager *m_pEffectBuffer;
	
	char m_szStartDir[MAX_PATH];

protected:
	typedef std::map< ioHashString, DWORD > MemoryMap;
	MemoryMap m_MemoryMap;
	MemoryMap m_CloneMemoryMap;

	StaticSwitch m_StaticSwitch;

public:
	void AddMem( const ioHashString& KeyName, DWORD dwMem );
	void CloneMem( const ioHashString& KeyName );
	void RemoveMem( const ioHashString& KeyName );
	void RemoveCloneMem( const ioHashString& KeyName );
	void GetMemStatics( DWORD& dwTempleteCount, DWORD& dwTempleteSize, DWORD& dwCloneCount, DWORD& dwCloneSize );
	void SetSwitch( int eSwitch ){ m_StaticSwitch = static_cast<StaticSwitch>( eSwitch ); }

public:
	void InitMemoryPoolSize( int iParticle, int iModelParticle );

public:
	void SetStartDir( const char *szPath );
	void SetDefaultIterationInterval( float fInterval );
	void SetLimitMaxParticleQuota( bool bLimit, float fLimitRate );

public:
	int  GetCurEffectCount() const { return m_EffectMap.size(); }
	void PrintCurEffectList() const;

public:
	ioEffect* CreateEffect( const ioHashString &kFileName );
	ioEffect* GetEffect( UniqueObjID eObjID );
	void DestroyEffect( UniqueObjID eObjID );
	void DestroyEffect( ioEffect *pEffect );

	const ioEffect* GetScript( const ioHashString &kFileName );
	void ReLoadEffectScript( const ioHashString &kFileName );
	void DestroyTemplate();
	void DestroyTemplateEffect( const ioHashString& szFileName );
	void DestroyTimeTemplate( const ioHashString& szFileName );
	void ReleaseTimeTemplate( const ioHashString& szFileName );

protected:
	ioEffect* CloneEffect( ioEffect *pEffect );
	bool LoadEffect( const ioHashString &kFileName, ioEffect *pEffect );
	void ParseScript( ioEffect *pEffect, ioTextStream &rkStream );

public:
	ioParticleEmitter* CreateEmitter( const ioHashString &kType );
	ioParticleAffector* CreateAffector( const ioHashString &kType );

	ioModelEmitter* CreateModelEmitter( const ioHashString &kType );
	ioModelParticleAffector* CreateModelAffector( const ioHashString &kType );
	ioEmitPointGenerator* CreateEmitPointGenerator( const ioHashString &kType );

protected:
	void ParseEffectInfo( ioTextStream &rkStream, ioEffect *pEffect );

protected:
	void ParseNewParticle( ioTextStream &rkStream,
						   ioEffect *pEffect,
						   int iIndex );

	void ParseNewLight( ioTextStream &rkStream, ioEffect *pEffect );

	void ParseNewEmitter( const ioHashString &kType,
						  ioTextStream &rkStream,						  
						  ioParticleSystem *pSys );

	void ParseNewAffector( const ioHashString &kType,
						   ioTextStream &rkStream,
						   ioParticleSystem *pSys );

protected:
	void ParseNewModel( ioTextStream &rkStream, ioEffect *pEffect );

	void ParseNewModelEmitter( const ioHashString &kType,
							   ioTextStream &rkStream,
							   ioModelParticleSystem *pSys );

	void ParseNewModelAffector( const ioHashString &kType,
								ioTextStream &rkStream,
								ioModelParticleSystem *pSys );

	void ParseAttribute( const std::string &line, ioStringInterface *pInter );

public:
	void SaveEffect( const char *szStartPath, ioEffect *pEffect );

protected:
	void SaveEffectInfo( FILE *fp, ioEffect *pEffect );
	void SaveParticle( FILE *fp, ioParticleSystem *pSys );
	void SaveEmitter( FILE *fp, ioParticleEmitter *pEmit );
	void SaveAffector( FILE *fp, ioParticleAffector *pAff );

	void SaveModel( FILE *fp, ioModelParticleSystem *pSys );
	void SaveMdlEmitter( FILE *fp, ioModelEmitter *pEmit );
	void SaveMdlAffector( FILE *fp, ioModelParticleAffector *pAff );

	void SaveLight( FILE *fp, ioLightSystem *pSys );

public:
	void ClearWorldEffectEventOwner( ioEntityEvent *pEntityEventOwner );

public:
	void ProcerssTimeTempleteCheck();

public:
	static ioEffectFactory& GetSingleton();

public:
	ioEffectFactory();
	virtual ~ioEffectFactory();
};

#define g_EffectFactory ioEffectFactory::GetSingleton()

#endif
