
#ifndef _ioResource_h_
#define _ioResource_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class __EX ioResource
{
protected:
	ioHashString	m_Name;
	int				m_iRefCnt;
	
	ErasePolicy		m_ErasePolicy;
	ResourceLoadState m_LoadState;

public:
	inline int AddRef()
	{
		m_iRefCnt++;
		return m_iRefCnt;
	}

	inline int Release()
	{
		m_iRefCnt--;
		return m_iRefCnt;
	}

	inline int PeekRef() const { return m_iRefCnt; }

public:
	inline void SetErasePolicy( ErasePolicy ePolicy ) { m_ErasePolicy = ePolicy; }
	inline void SetLoadState( ResourceLoadState eState ) { m_LoadState = eState; }

public:
	inline ErasePolicy GetErasePolicy() const { return m_ErasePolicy; }
	inline ResourceLoadState GetLoadState() const { return m_LoadState; }
	
	inline bool IsLoaded() const { return (m_LoadState == RLS_LOADED); }	// 로딩성공
	inline bool IsLoadDone() const	// 성공이던 실패던 일단 완료는 되었다.
	{
		if( m_LoadState == RLS_LOADED || m_LoadState == RLS_FAILED )
			return true;

		return false;
	}

public:
	// 리소스의 이름을 반환, 리소스의 이름은 풀패스이다.
	inline const ioHashString& GetName() const { return m_Name; }

public:
	ioResource( const ioHashString &name);
	virtual ~ioResource();
};

#endif
