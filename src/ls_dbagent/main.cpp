// ls_dbagent.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ServiceLS.h"

int _tmain(int argc, _TCHAR* argv[])
{
	ServiceLS *service = new ServiceLS(argc, argv);
	service->ServiceMainProc();
	delete service;
	return 0;
}

