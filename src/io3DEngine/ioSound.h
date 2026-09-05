

#ifndef _ioSound_h_
#define _ioSound_h_

#include "ioResource.h"

class ioSceneNode;
class ioSound;
class ioStream;

class __EX ioSoundBuffer
{
private:
	IDirectSoundBuffer *m_pBuf;
	int m_iAttachRef;

protected:
	UniqueObjID m_SoundID;

public:
	void SetVolume( int iVolume );
	void ResetVolume( int iVolume );
	void Play( int iVolume, UniqueObjID eSoundID, bool bLoop );
	void Stop();

	bool IsPlaying() const;

public:
	inline void AddAttachRef() { ++m_iAttachRef; }
	inline void ReleaseAttach() { --m_iAttachRef;}
	inline int  PeekRef() const { return m_iAttachRef; }

	inline UniqueObjID GetSoundID() const { return m_SoundID; }

private:
	bool CheckRestore();

private:
	friend class ioSound;

private:
	ioSoundBuffer( IDirectSoundBuffer *pBuf );
	~ioSoundBuffer();
};

class __EX ioSound
{
protected:
	ioHashString m_Name;
	ResourceLoadState m_LoadState;

	typedef std::vector< ioSoundBuffer* > SoundBufferList;
	SoundBufferList m_BufferList;

	bool m_bDeleteReserved;

public:
	bool InitSound( IDirectSound8 *pDS, ioStream *pStream, int iBufCound );

public:
	void SetVolume( int iVolume );
	void ResetVolume( int iVolume );
	void SetLoadState( ResourceLoadState eState );

	void Play( int iVolume, UniqueObjID eSoundID, bool bLoop );
	void Play3D( int iVolume, UniqueObjID eSoundID, ioSceneNode *pSNode, bool bLoop );
	void Stop( UniqueObjID eSoundID, bool bAllStop = false );

protected:
	ioSoundBuffer* AddBuffer( IDirectSoundBuffer *pBuf );
	ioSoundBuffer* GetFreeBuffer();

public:
	inline void ReserveDelete() { m_bDeleteReserved = true; }
	inline bool IsDeleteReserved() const { return m_bDeleteReserved; }

	bool IsNotUsing() const;
	bool IsNotUsing( UniqueObjID eSoundID ) const;

public:
	inline const ioHashString& GetName() const { return m_Name; }

	inline bool IsLoaded() const { return (m_LoadState == RLS_LOADED); }	// 로딩성공
	inline bool IsLoadDone() const	// 성공이던 실패던 일단 완료는 되었다.
	{
		if( m_LoadState == RLS_LOADED || m_LoadState == RLS_FAILED )
			return true;

		return false;
	}

public:
	ioSound( const ioHashString &name );
	virtual ~ioSound();
};

#endif
