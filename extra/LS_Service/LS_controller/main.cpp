#include "stdafx.h"
#include "cServiceLS.h"
#include <iostream>

using namespace std;


int _tmain(int argc, TCHAR **argv)
{
	// 한글 OS가 설치되지 않은 곳에서 실행되지 않으므로 삭제. 2013-01-14 신영욱
//#ifdef _UNICODE
//	wcin.imbue( locale("korean") );
//	wcout.imbue(locale("korean"));
//#endif

	cServiceLS *service = new cServiceLS(argc, argv);
	service->ServiceMainProc();
	delete service;
	return 0;
}
