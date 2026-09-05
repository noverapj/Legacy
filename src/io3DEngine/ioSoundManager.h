

#ifndef _ioSoundManager_h_
#define _ioSoundManager_h_

class ioSound;
class ioOggStreamSound;

class ioStream;
class ioBinaryStream;
class ioSceneNode;
class ioSceneManager;

#include "io3DCommon.h"
#include "ioDataProcessor.h"
#include "ioResourceManager.h"

class __EX ioSoundManager : public ioDataProcessor, public Singleton< ioSoundManager >
{
protected:
	char m_szStartDir[MAX_PATH];

	typedef std::map< ioHashString, ioSound* > SoundMap;
	SoundMap m_SoundMap;

	LPDIRECTSOUND8		m_pDS;
	LPDIRECTSOUNDBUFFER	m_pDSPrimary;
	ioOggStreamSound	*m_pOggStream;
	int m_iNumBufferPerSound;

protected:
	struct LoadRequest
	{
		ioSound *m_pSound;
		UniqueNodeID m_NodeID;

		int	  m_iVolume;
		bool  m_bLoop;
		UniqueObjID m_SoundID;
	};

	struct NotifiedRequest
	{
		ioBinaryStream *m_pStream;
		LoadRequest *m_pRequest;
		DWORD m_dwLoadState;
	};

	typedef std::vector< NotifiedRequest > NotifiedRequestList;
	NotifiedRequestList m_NotifiedRequestList;
	CRITICAL_SECTION m_NotifiedListSection;

protected:
	D3DXVECTOR3 m_vListenerPos;
	int m_iVolumeNormal;
	int m_iVolume3D;
	int m_iVolumeStream;
	int m_iVolumePiece;

	bool m_bCanPlayStream;
	bool m_bCanPlaySound;
	bool m_bCanPlayPiece;

protected:
	typedef std::map< ioHashString, DWORD > MemoryMap;
	MemoryMap m_MemoryMap;

	StaticSwitch m_StaticSwitch;

public:
	void AddMem( const ioHashString& KeyName, DWORD dwMem );
	void RemoveMem( const ioHashString& KeyName );
	void GetMemStatics( DWORD& dwCount, DWORD& dwSize );
	void SetSwitch( int eSwitch ){ m_StaticSwitch = static_cast<StaticSwitch>( eSwitch ); }

protected:
	static UniqueObjID m_sNextSoundID;
	static UniqueObjID GetNextUniqueID();
	
protected:
	void Init3DSound();

public:
	void SetStartDir( const char *szDir );
	void SetNumBufferPerSound( int iNumBuf );

public:
	bool InitDSound( HWND hWnd, int iSamples = 22050, int iBits = 16, int iChannels = 1 );

	void OpenOggStream( const char *szFileName );
	void UpdateStream();
	void PlayStream( bool bLoop = true );
	void StopStream();

public:
	void UpdateListener( const D3DXVECTOR3 &vPos );
	int  GetVolumeByDistance( const D3DXVECTOR3 &vPos ) const;

public:
	UniqueObjID PlaySound( const ioHashString &name,
					int iVolume = DSBVOLUME_MAX,
					DWORD dwOptMask = PSM_THREAD );

	UniqueObjID PlaySound( const ioHashString &name,
					ioSceneNode *pSNode,
					DWORD dwOptMask = PSM_THREAD );

	UniqueObjID PlayPieceSound( const ioHashString &szPathName,
		                 int iVolume = DSBVOLUME_MAX,
						 DWORD dwOptMask = PSM_THREAD );

public:
	void DeleteSound( const ioHashString &name );
	void StopSound( const ioHashString &name, UniqueObjID eSoundID );
	void AllSoundStop();
	bool IsPlayingSound( const ioHashString& szSoundName, UniqueObjID ID );

protected:
	ioSound* GetSoundByName( const ioHashString &name );
	ioSound* CreateSound( const ioHashString &name );

	bool LoadSound( const char *szPathName, ioSound *pSound, bool bPiece );

public:
	void DestoryNoneRefResources();

	void UpdateNotifiedResources( ioSceneManager *pSceneMgr );
	virtual void NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded );

public:
	void SetVolumeNormal(int iVolume);
	void SetVolume3D(int iVolume);
	void SetVolumeStream(int iVolume);
	void SetVolumePiece( int iVolume );
	void SetOggVolume(int iVolume);
	void SetSoundVolume( const ioHashString &name, int iVolume );

	inline int GetVolumeStream() const { return m_iVolumeStream; }

	void SetCanPlayStream(bool bCanPlayStream) { m_bCanPlayStream = bCanPlayStream; }
	void SetCanPlaySound(bool bCanPlaySound) { m_bCanPlaySound = bCanPlaySound; }
	void SetCanPlayPiece(bool CanPlayPiece) { m_bCanPlayPiece = CanPlayPiece; }

	inline bool IsCanPlayStream() const { return m_bCanPlayStream; }
	inline bool IsCanPlaySound() const { return m_bCanPlaySound; }
	inline bool IsCanPlayPiece() const { return m_bCanPlayPiece; }

	void ResetVolume();

public:
	static ioSoundManager& GetSingleton();

public:
	ioSoundManager();
	virtual ~ioSoundManager();
};

#define g_SoundMgr ioSoundManager::GetSingleton()

#endif
