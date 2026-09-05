#ifndef __ioXtrap_h__
#define __ioXtrap_h__

#ifdef SHIPPING
	#pragma comment(lib, "urlmon.lib" )
	#pragma comment(lib, "wininet.lib" )
	#pragma comment(lib, "Xtrap/XTrap4Launcher_mt.lib" ) // mt multi thread
#endif

class ioXtrap
{
protected:
	enum 
	{
		PATCH_WAIT_TIME = 60, // 패치 응답 대기 시간 초단위. 해외에서는 좀더 길게 설정.
	};

protected:
	static ioXtrap *sg_Instance;
#ifdef SRC_NA
	static bool m_bQA;
#endif

public:
	void RunPatch();

public:
	static ioXtrap &GetInstance();
	static void ReleaseInstance();

private: // Singleton Class
	ioXtrap(void);
	virtual ~ioXtrap(void);
};

#define g_ioXtrap ioXtrap::GetInstance()

#endif // __ioXtrap_h__

