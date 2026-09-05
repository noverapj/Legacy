

#ifndef _ioTextureEffectManager_h_
#define _ioTextureEffectManager_h_

#include "ioSingleton.h"

class ioTextureEffect;
class ioTextureUnitState;
struct TextureEffect;

class __EX ioTextureEffectManager : public Singleton< ioTextureEffectManager >
{
protected:
	typedef std::set< ioTextureEffect* > TextureEffectSet;
	TextureEffectSet m_EffectSet;
	DWORD m_dwLastUpdateFrame;

public:
	void UpdateAllEffect();

	void CreateEffect( ioTextureUnitState *pState, TextureEffect &rkEffect );

	void UnLoadEffect( ioTextureEffect *pEffect );
	void UnLoadAllEffect();

public:
	static ioTextureEffectManager& GetSingleton();

public:
	ioTextureEffectManager();
	virtual ~ioTextureEffectManager();
};

#define g_TextureEffectMgr ioTextureEffectManager::GetSingleton()

#endif