#pragma once

#include "Service.h"

class ServiceLS : public Service
{
public:
	ServiceLS(int argc, TCHAR **argv);
	virtual ~ServiceLS();

public:
	virtual BOOL ExecuteProcess();									// 서비스 실행시에 호출되는 함수(사용자 정의)
	virtual BOOL ServiceStart();									// 서비스 실행(START)시에 호출됨
	virtual void ServiceStop();										// 서비스 중지(STOP)시에 호출됨

	virtual void Debug(const TCHAR *message);						// 디버깅용 메세지 출력

protected:
	void ConfigureSystem();
	void ConfigureTime();

	const TCHAR* GetLogFile()		{ return m_logFile.c_str(); }	// 서비스 로그가 남을 파일
	const TCHAR* GetINIFile()		{ return m_logFile.c_str(); }	// 설정 INI 파일
	const TCHAR* GetCurrentTime()	{ return m_time.c_str(); }		// 현재 시간

protected:
	tstring m_iniFile;
	tstring m_logFile;
	tstring m_time;
};
