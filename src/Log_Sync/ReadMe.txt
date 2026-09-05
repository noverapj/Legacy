/********************************************************************
	created:	2003/01/22
	created:	22:1:2003   10:10
	filename: 	e:\■ now\workingdll\logdll\readme.txt
	file path:	e:\■ now\workingdll\logdll
	file base:	readme
	file ext:	txt
	author:		김인중
	
	purpose:	
*********************************************************************/

□	Dll 버전 확인을 위하여
	- 리소스에 버전 기록한다. 예) 2003.1.20 
	- 마지막 컴파일 날짜가 남는지 확인 (버전을 추가하면)
	- 히스토리를 남긴다. (될 수 있는대로 영어로 : 영어구사능력 향상을 위해서)
	 	최소 last write 날짜를 기록한다.
	- History 
		2003.1.20 add log function, CLog:DebugLOG, DebugMBox
				for debug infomation (__FILE__, __LINE__)
		2003.1.20 edit DEBUGLOG, DEBUGBOX definition for using this function.

□	DLL 직적접인 해더 파일은 반드시 LogDLL.h 라고 반드시 DLL부분을 끝부분에 
	기록함, 

□	프로젝트명은 LogDLL 로 정한다.

□	또한 간단한 경우는 별도의 메인 파일 필요 없이 LogDLL.cpp 안에 
	DllMain과 클래스 몸체 부분을 포함시킨다.

□	가능하다면 (DLL이 프로젝트에 추가 혹은 인식된다면 ) 다른 프로젝트에서 사용할때
    LogDLL 폴더에 LogDLL.dll, LogDLL.lib LogDLL.h를 넣고 사용
