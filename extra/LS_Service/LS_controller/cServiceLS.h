#pragma once

#include "cService.h"


class cServiceLS : public cService
{
public:
	cServiceLS(int argc, TCHAR **argv);
	virtual ~cServiceLS();

public:
	virtual BOOL ExecuteProcess();									// 서비스 실행시에 호출되는 함수(사용자 정의)
	virtual BOOL ServiceStart();									// 서비스 실행(START)시에 호출됨
	virtual void ServiceStop();										// 서비스 중지(STOP)시에 호출됨

	virtual void Debug(const TCHAR *message);						// 디버깅용 메세지 출력

protected:
	void ConfigureSystem();

	const TCHAR* GetLogFile() { return m_logFile.c_str(); }			// 서비스 로그가 남을 파일

protected:
	std::tstring m_luaFile;
	std::tstring m_logFile;
};
