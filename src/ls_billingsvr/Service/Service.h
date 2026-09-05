#pragma once

#include <vector>
#include <string>

typedef std::basic_string<TCHAR> tstring;

class Service  
{
public:
	Service(int argc, TCHAR **argv);
	virtual ~Service();

public:
	void ServiceMainProc();											// 서비스 실행 메인 함수

	static Service* _serviceMainClass;
	static void WINAPI _ServiceMain(int argc, TCHAR **argv);			// 서비스 메인 함수(ServiceMain()함수를 호출)
	static void WINAPI _ServiceHandler(DWORD fdwControl);			// 서비스 제어 메인 함수(ServiceHandler()함수를 호출)

	void ServiceMain(int argc, TCHAR **argv);						// 서비스 메인의 구현부
	void ServiceHandler(DWORD fdwControl);							// 서비스 제어의 구현부

public:
	virtual BOOL ExecuteProcess() = 0;								// 서비스 실행시에 호출되는 함수
	virtual BOOL ServiceStart() = 0;								// 서비스 실행(START)시에 호출됨
	virtual void ServiceStop() = 0;									// 서비스 중지(STOP)시에 호출됨

	virtual void Debug(const TCHAR *message) = 0;					// 디버깅용 메세지 출력

protected:
	BOOL _ExecuteProcess();											// 서비스의 실행시 호출됨

	// Service Control
	BOOL Install(const TCHAR* serviceName, const TCHAR* displayName);	// 서비스 등록
	BOOL Uninstall(const TCHAR* serviceName);						// 서비스 제거
	BOOL RunService(const TCHAR* serviceName);						// 서비스 실행(START)
	BOOL KillService(const TCHAR* serviceName);						// 서비스 중지(STOP)

protected:
	std::vector<tstring> m_arguments;
	tstring m_serviceName;
	tstring m_scriptName;
	tstring m_displayName;

	SERVICE_STATUS_HANDLE   m_hServiceStatusHandle; 
	SERVICE_STATUS          m_serviceStatus; 
};
