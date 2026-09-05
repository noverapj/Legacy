#include "../stdafx.h"
#include "../resource.h"
#include ".\iolocalkorea.h"
#include "../HelpFunc.h"
#include "../AutoUpgradeDlg.h"

ioLocalKorea::ioLocalKorea(void)
{
}

ioLocalKorea::~ioLocalKorea(void)
{
}

ioLocalManager::LocalType ioLocalKorea::GetType()
{
	return ioLocalManager::LCT_KOREA;
}

const char * ioLocalKorea::GetTextListFileName()
{
	return "text.txt";
}

const char * ioLocalKorea::GetMemTextList()
{
	return "|EXE_CAutoUpgradeApp::IsDuplication_1|\r\n\r\n      [로스트사가]가 이미 실행 중 입니다.\r\n\r\n      [로스트사가]를 종료후 실행해 주세요.\r\n\r\n\r\n      (오류번호:201)|"
		   "|EXE_CAutoUpgradeDlg::OnCopyData_1|게임서버에 접속 중입니다.|"
		   "|EXE_CAutoUpgradeDlg::OnCopyData_2|UDP 연결 중입니다.|"
		   "|EXE_CAutoUpgradeDlg::OnCopyData_3|게임서버 접속에 실패하였습니다.|"
		   "|EXE_CAutoUpgradeDlg::OnCopyData_4|접속할 수 없는 IP입니다.|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_1|게임을 실행 중입니다.|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_2|10분 동안 게임 실행에 실패하였습니다.\r\n(오류번호:210)|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_3|게임서버 접속에 실패하였습니다.|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_4|최적화 확인중.|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_5|최적화 확인중.|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_6|게임이 자동으로 시작됩니다.|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_7|게임이 자동으로 시작됩니다.|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_8|클라이언트 실행에 실패하였습니다.\r\n(오류번호:226)|"
		   "|EXE_CAutoUpgradeDlg::OnTimer_9|클라이언트 실행에 실패하였습니다.|"
		   "|EXE_CAutoUpgradeDlg::SetWantEndDialog_1|종료 중입니다. 잠시만 기다려주세요.|"
		   "|EXE_Help::CheckRightHDDSpace_1|로스트사가 업데이트를 위해서는 %1메가 이상의\r\n빈 하드디스크 공간이 필요합니다.\r\n(오류번호:207)\r\n\r\n전체크기:%2M, 남은크기:%3M, 로스트사가필요크기:%4M|"
		   "|EXE_Help::CheckDownloadedFile_1|무결성 확인을 위한 파일 오픈에 실패하였습니다.\r\n|"
		   "|EXE_Help::CheckDownloadedFile_2|다운로드 받은 파일이 에러 웹페이지 입니다.\r\n파일명:%1\r\n파일CRC:%2, 파일사이즈:%3,GetLastError:%4|"
		   "|EXE_HTTPManager::_Run_1|로컬 패치정보 읽는 중.|"
		   "|EXE_HTTPManager::_Run_2|설치된 DirectX의 버전이 낮습니다.\r\n최신버전의 DirectX를 설치해주세요.\r\n그래픽카드의 드라이버도 최신버전인지 확인해주세요.\r\n(오류번호:211)|"
		   "|EXE_HTTPManager::_Run_3|%1번째 iop 오픈이 실패하였고.\r\n파일삭제가 실패하였습니다.\r\n파일명:%2, GetLastError:%3\r\n프로그램을 다시 실행해 주세요.\r\n(오류번호:222)\r\n\r\n|"
		   "|EXE_HTTPManager::_Run_4|정보 파일 다운로드에 실패하였습니다.\r\n패치서버가 혼잡할 수 있습니다.\r\n잠시 후 프로그램을 다시 실행해 주세요.\r\n(오류번호:203)\r\n\r\n|"
		   "|EXE_HTTPManager::_Run_5|정보 파일이 손상되었습니다.\r\n패치서버가 혼잡할 수 있습니다.\r\n잠시 후 프로그램을 다시 실행해 주세요.\r\n(오류번호:215)\r\n\r\n|"
		   "|EXE_HTTPManager::_Run_6|정보 파일 팩을 풀지 못하였습니다.\r\n패치 파일 동기화에 문제가 없는지 확인하시고\r\n 프로그램을 다시 실행해 주세요.\r\n(오류번호:204)\r\n\r\n|"
		   "|EXE_HTTPManager::_Run_7|정보 파일 분석에 실패하였습니다.\r\n 프로그램을 다시 실행해 주세요.\r\n(오류번호:205)\r\n\r\n|"
		   "|EXE_HTTPManager::_Run_8|하드디스크 공간 확인 함수가 실패하였습니다.\r\n하디디스크 공간이 충분한 것으로\r\n간주하여 작동합니다.\r\n문제가 지속되면 하드 공간을 확보해 주세요.\r\n(오류번호:223)\r\n\r\nGetLastError:%1|"
		   "|EXE_HTTPManager::_Run_9|파일 다운로드 및 갱신에 실패하였습니다(100회 재시도 실패).\r\n프로그램을 다시 실행해 주십시오.\r\n문제가 지속되면 프로그램 메뉴항목에서\r\n로스트사가제거를 실행하시고바이러스 검사를 해주시기 바랍니다.\r\n또한 인터넷 회선 상태도 확인해 주시기 바랍니다.\r\n(오류번호:206)\r\n\r\n|"
		   "|EXE_HTTPManager::_Run_10|패치 적용 중.|"
		   "|EXE_HTTPManager::_Run_11|서버 연결 실패(오류번호 : ).|"
		   "|EXE_HTTPManager::DeleteGarbage_1| 패치 적용 중 [%1/%2].|"
		   "|EXE_HTTPManager::_RunAdmin_1|%1번 팩파일 푸는 중에 에러가 발생하였습니다.\r\n\r\n%2|"
		   "|EXE_HTTPManager::_RunAdmin_2|완료되었습니다.|"
		   "|EXE_HTTPManager::_RunAdmin_3|폴더 복사중 에러가 발생하였습니다.\r\n\r\n|"
		   "|EXE_HTTPManager::_RunOptionCopyFolder_1|에러 원폴더.|"
		   "|EXE_HTTPManager::_RunOptionCopyFolder_2|에러 목적폴더.|"
		   "|EXE_HTTPManager::_RunOptionCopyFolder_3|폴더 복사 중입니다.|"
		   "|EXE_HTTPManager::_RunOptionCopyFolder_4|완료되었습니다.|"
		   "|EXE_HTTPManager::AddPacFile_1|합치는 중:%1번째 파일.|"
		   "|EXE_HTTPManager::AddPacFile_2|에러 파일 합치기:%1.|"
		   "|EXE_HTTPManager::AddPacFile_3|파일합치기실패\r\n파일명:%1\r\n팩파일안에 파일명:%2\r\n팩파일 어레이:%3 , 파일 CRC:%4, GetLastError값:%5, Exception:%6, |"
		   "|EXE_HTTPManager::AddPacFile_4|파일합치는중 다운받은파일 삭제를 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2, |"
		   "|EXE_HTTPManager::CheckPac_1|에러 체크팩 NULL.|"
		   "|EXE_HTTPManager::CheckPac_2|체크에 사용할 팩변수가 NULL 입니다.\r\nGetLastError:%1\r\n|"
		   "|EXE_HTTPManager::CheckPac_3|파일 검사 중:%1/%2번째 파일[%3%%].|"
		   "|EXE_HTTPManager::CopyVersionupFiles_1|수동 패치 처리 중:[%1%%].|"
		   "|EXE_HTTPManager::CreateAllNewPacFile_1|%1번째 파일 최적화에 실패하였습니다.\r\n 프로그램을 다시 실행해 주세요.\r\n(오류번호:212)\r\n\r\n%2|"
		   "|EXE_HTTPManager::CreateNewPacFile_1|새로운 팩 파일 제작을 위한 이전 파일 삭제가 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::CreateNewPacFile_2|에러 저장 뉴팩.|"
		   "|EXE_HTTPManager::CreateNewPacFile_3|새로운 팩파일 생성 오픈에 실패하였습니다.\r\n|"
		   "|EXE_HTTPManager::CreateNewPacFile_4|에러 뉴팩 NULL.|"
		   "|EXE_HTTPManager::CreateNewPacFile_5|새로운 팩 파일 제작을 위한 iop파일 변수가 NULL 입니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::CreateNewPacFile_6|에러 오픈 뉴팩.|"
		   "|EXE_HTTPManager::CreateNewPacFile_7|새로운 팩 파일 제작을 위한 iop파일 오픈이 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2,Exception:%3|"
		   "|EXE_HTTPManager::CreateNewPacFile_8|에러 뉴팩 정보.|"
		   "|EXE_HTTPManager::CreateNewPacFile_9|새로운 팩 파일 제작을 위한 iop파일 정보 초기화 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::CreateNewPacFile_10|에러 뉴팩 NULL 2.|"
		   "|EXE_HTTPManager::CreateNewPacFile_11|새롭게 제작하는 iop 변수가 NULL 입니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::CreateNewPacFile_12|에러 오픈 뉴팩2.|"
		   "|EXE_HTTPManager::CreateNewPacFile_13|새롭게 제작하는 iop 파일 오픈이 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2, 파일CRC:%3, Exception:%4|"
		   "|EXE_HTTPManager::CreateNewPacFile_14|최적화 중:%1/%2번째 파일[%3%%].|"
		   "|EXE_HTTPManager::CreateNewPacFile_15|에러 뉴팩 파일 정보.|"
		   "|EXE_HTTPManager::CreateNewPacFile_16|새로운 팩 파일 제작을 위한 이전 iop 파일 정보 읽기 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2,어레이:%3,Exception:%4|"
		   "|EXE_HTTPManager::CreateNewPacFile_17|에러 뉴팩 파일 데이터.|"
		   "|EXE_HTTPManager::CreateNewPacFile_18|새로운 팩 파일 제작을 위한 이전 iop 파일 데이터 읽기 실패하였습니다.\r\n파일명:%1\r\n파일해쉬:%2,파일인덱스:%3,파일압축사이즈:%4,파일사이즈:%5,GetLastError:%6,어레이:%7,Exception:%8|"
		   "|EXE_HTTPManager::CreateNewPacFile_19|에러 뉴팩 파일 합치기.|"
		   "|EXE_HTTPManager::CreateNewPacFile_20|새로운 rs.iop에 파일 합치기 실패하였습니다.\r\n파일명:%1\r\n파일해쉬:%2,파일인덱스:%3,파일압축사이즈:%4,파일사이즈:%5,GetLastError:%6,어레이:%7,Exception:%8|"
		   "|EXE_HTTPManager::CreateNewPacFile_21|에러 뉴팩 사이즈.|"
		   "|EXE_HTTPManager::CreateNewPacFile_22|새로운 팩 파일 제작을 위한 이전 iop 파일 데이터 사이즈가 0입니다.\r\n파일명:%1\r\n파일해쉬:%2,파일인덱스:%3,파일압축사이즈:%4,파일사이즈:%5,GetLastError:%6,어레이:%7,Exception:%8|"
		   "|EXE_HTTPManager::CreateNewPacFile_23|에러 뉴팩 복사.|"
		   "|EXE_HTTPManager::CreateNewPacFile_24|새로운 iop 복사에 실패하였습니다.\r\n파일명:%1\r\n새로운파일명:%2\r\nGetLastError:%3|"
		   "|EXE_HTTPManager::CreateNewPacFile_25|에러 뉴팩 삭제.|"
		   "|EXE_HTTPManager::CreateNewPacFile_26|뉴파일 복사 완료후 복사전 뉴파일 삭제가 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::DownLoadProcess_1|다운로드 중:%1번째 파일.|"
		   "|EXE_HTTPManager::DownLoadProcess_2|에러 OpenUrl:%1.|"
		   "|EXE_HTTPManager::DownLoadProcess_3|OpenURL을 실패하였습니다.\r\n파일명:%1\r\n에러:%2|"
		   "|EXE_HTTPManager::DownLoadProcess_4|에러 OpenUrl NULL:%1.|"
		   "|EXE_HTTPManager::DownLoadProcess_5|CInternetFile일 값이 NULL입니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::DownLoadProcess_6|파일다운로드중 이전 파일 삭제를 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::DownLoadProcess_7|에러 파일 오픈:%1.|"
		   "|EXE_HTTPManager::DownLoadProcess_8|파일저장을 위한 파일오픈에 실패하였습니다.\r\n|"
		   "|EXE_HTTPManager::DownLoadProcess_9|%1 다운로드 중 [%2%%].|"
		   "|EXE_HTTPManager::DownLoadProcess_10|에러 파일 쓰기:%1.|"
		   "|EXE_HTTPManager::DownLoadProcess_11|파일저장을 위한 파일쓰기에 실패하였습니다.\r\n|"
		   "|EXE_HTTPManager::DownLoadProcess_12|에러 파일 받기:%1.|"
		   "|EXE_HTTPManager::DownLoadProcess_13|파일저장을 위한 파일받기에 실패하였습니다.\r\n|"
		   "|EXE_HTTPManager::DownLoadProcess_14|full.zip 이어받기 실패. HTTP서버에서 응답이 없습니다.|"
		   "|EXE_HTTPManager::FileCopy_1|에러 파일 복사:%1.|"
		   "|EXE_HTTPManager::FileCopy_2|파일 복사에 실패하였습니다.\r\n파일명:%1\r\n새로운파일명:%2\r\nGetLastError:%3, |"
		   "|EXE_HTTPManager::GetUpdateFile_1|다운로드 확인 중.|"
		   "|EXE_HTTPManager::GetUpdateFile_2|업데이트 확인 중[%1%%].|"
		   "|EXE_HTTPManager::InitPac_1|팩라이브러리 메모리 할당이 실패하였습니다.\r\n 프로그램을 다시 실행해 주세요.\r\n(오류번호:213)\r\n\r\n어레이:%1, GetLastError:%2|"
		   "|EXE_HTTPManager::OpenAllBigPacForGarbageSize_1|%1번째 iop 오픈이 실패하였습니다.\r\n파일명:%2, GetLastError:%3\r\n프로그램을 다시 실행해 주세요.\r\n(오류번호:225)\r\n\r\n|"
		   "|EXE_HTTPManager::ParseDeleteFileList_1|삭제 파일 분석 중.|"
		   "|EXE_HTTPManager::ParsePatchFile_1|파싱을 위한 파일 오픈에 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::ParsePatchFile_2|파싱후 이전 파일 삭제를 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::ProcessCheckPac_1|클라이언트 파일에서 문제가 발견되었습니다.\r\n\r\n[예]를 누르시면 전체 파일을 검사하고 \r\n\r\n에러가 발견된 경우 클라이언트를 다시 다운로드 합니다.|"
		   "|EXE_HTTPManager::ProcessCheckPac_2|클라이언트 파일에서 에러가 발견되었습니다.\r\n\r\n클라이언트를 다시 다운로드 합니다.|"
		   "|EXE_HTTPManager::ProcessCheckPac_3|%1번째 iop 오픈이 실패하였고.\r\n파일삭제가 실패하였습니다.\r\n파일명:%2, GetLastError:%3\r\n프로그램을 다시 실행해 주세요.\r\n(오류번호:224)\r\n\r\n|"
		   "|EXE_HTTPManager::ProcessNewPacFile_1|이미 최적화 되었습니다.|"
		   "|EXE_HTTPManager::ProcessNewPacFile_2|이미 최적화 되었습니다.|"
		   "|EXE_HTTPManager::Run_1|메인 루프에서 예외 오류가 발생하였습니다.\r\n 프로그램을 다시 실행해 주세요.\r\n(오류번호:209)\r\n\r\n|"
		   "|EXE_HTTPManager::SetTextWaitNewPac_1|최적화 확인중.[%1]|"
		   "|EXE_HTTPManager::SetupFull_1|%1번 팩파일 푸는 중에 에러가 발생하였습니다.\r\n\r\n%2|"
		   "|EXE_HTTPManager::SetupFull_2|설치버전 버전정보 삭제에 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::SetupFull_3|설치버전 버전정보 파일 오픈에 실패하였습니다.\r\n|"
		   "|EXE_HTTPManager::SetupFull_4|에러 팩파일 오픈.|"
		   "|EXE_HTTPManager::SetupFull_5|설치이후 %1번째 iop파일 오픈이 실패하였습니다.\r\n파일명:%2\r\nGetLastError:%3, 파일CRC:%4, Exception:%5, |"
		   "|EXE_HTTPManager::StartGame_1|게임을 실행 중입니다.|"
		   "|EXE_HTTPManager::StartGame_2|로그인키의 유효시간이 만료되었습니다.\r\n\r\n         안전한 로그인을 위하여\r\n\r\n            다시 로그인 해주세요.|"
		   "|EXE_HTTPManager::StartGame_3|게임이 자동으로 시작됩니다.[%1]|"
 		   "|EXE_HTTPManager::StartGame_4|게임을 시작하려면 시작 버튼을 눌러주십시오.|"
		   "|EXE_HTTPManager::StartGame_5|웹 브로커의 버전이 낮습니다. 게임을 다시 시작해주십시오.|"
		   "|EXE_HTTPManager::StartGame_6|Web Broker Error!!|"
		   "|EXE_HTTPManager::UnFull_1|에러 풀설치 NULL.|"
		   "|EXE_HTTPManager::UnFull_2|설치 버전이 사용할 팩변수가 NULL 입니다.\r\nGetLastError:%1, |"
		   "|EXE_HTTPManager::UnFull_3|에러 풀설치 오픈.|"
		   "|EXE_HTTPManager::UnFull_4|설치 버전 오픈이 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2, 파일CRC:%3, Exception:%4, |"
		   "|EXE_HTTPManager::UnFull_5|에러 풀설치 정보.|"
		   "|EXE_HTTPManager::UnFull_6|설치 정보 읽기가 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2, 파일CRC:%3, 파일어레이:%4, Exception:%5, |"
		   "|EXE_HTTPManager::UnFull_7|압축을 푸는 중:%1번째 파일[%2%%].|"
		   "|EXE_HTTPManager::UnFull_8|설치버전의 이전 파일 삭제가 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2, |"
		   "|EXE_HTTPManager::UnFull_9|에러 풀설치 풀기:%1.|"
		   "|EXE_HTTPManager::UnFull_10|설치버전 압축 풀기가 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2, 설치파일CRC:%3, 어레이:%4, Exception:%5, |"
		   "|EXE_HTTPManager::UnFull_11|설치버전 삭제를 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2, |"
		   "|EXE_HTTPManager::UnPac_1|에러 풀팩 NULL.|"
		   "|EXE_HTTPManager::UnPac_2|팩파일 풀기 위한 rs.iop파일 변수가 NULL 입니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::UnPac_3|에러 풀팩 오픈.|"
		   "|EXE_HTTPManager::UnPac_4|팩파일 풀기 위한 rs.iop파일 오픈이 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2,Exception:%3|"
		   "|EXE_HTTPManager::UnPac_5|에러 풀팩 INIT.|"
		   "|EXE_HTTPManager::UnPac_6|팩파일 풀기 위한 rs.iop파일 정보 초기화 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::UnPac_7|에러 풀팩 정보.|"
		   "|EXE_HTTPManager::UnPac_8|팩파일 풀기 위한 이전 rs.iop 파일 정보 읽기 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2,어레이:%3,Exception:%4|"
		   "|EXE_HTTPManager::UnPac_9|IOP를 푸는 중:%1/%2번째 파일[%3%%].|"
		   "|EXE_HTTPManager::UnPac_10|팩파일 풀기 위한 이전 파일 삭제 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::UnPac_11|에러 풀팩 풀기.|"
		   "|EXE_HTTPManager::UnPac_12|팩파일 풀기 위한 파일 풀기가 실패하였습니다.\r\n파일명:%1\r\n파일해쉬:%2,파일인덱스:%3,파일압축사이즈:%4,GetLastError:%5,어레이:%6,Exception:%7|"
		   "|EXE_HTTPManager::UnPacPieceFile_1|압축을 푸는 중:%1번째 파일.|"
		   "|EXE_HTTPManager::UnPacPieceFile_2|낱개 압축 풀기에서 이전 파일 삭제를 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::UnPacPieceFile_3|낱개 압축 풀기에서 압축풀 파일이 존재하지 않습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::UnPacPieceFile_4|에러 낱개언팩:%1.|"
		   "|EXE_HTTPManager::UnPacPieceFile_5|낱개 파일 압축 풀기에 실패하였습니다.\r\n파일명:%1\r\n에러리턴값:%2, 파일CRC값:%3, GetLastError값:%4, Exception:%5|"
		   "|EXE_HTTPManager::UnPacPieceFile_6|낱개 압축 풀기에서 다운받은파일 삭제를 실패하였습니다.\r\n파일명:%1\r\nGetLastError:%2|"
		   "|EXE_HTTPManager::UpdateFileLoop_1|다운로드 대상 파일 분석 중.|"
		   "|EXE_HTTPManager::UpdateFileLoop_2|에러 발생으로 재시도 중:%1/%2.|"
		   "|EXE_HTTPManager::UpdateOneFile_1|파일 다운로드에 에러가 발생하였습니다.\r\n자동으로 재시도를 시작합니다.(총100회)\r\n프로그램 종료를 원하시면\r\n자동업데이트의 X버튼을 클릭하십시오.\r\n(오류번호:218)\r\n\r\n|"
		   "|EXE_HTTPManager::UpdateOneFile_2|에러 다운로드파일 손상.:%1.|"
		   "|EXE_HTTPManager::UpdateOneFile_3|다운로드된 파일이 손상되었습니다.\r\n패치서버가 혼잡할 수 있습니다.\r\n자동으로 재시도를 시작합니다.(총100회)\r\n프로그램 종료를 원하시면\r\n자동업데이트의 X버튼을 클릭하십시오.\r\n(오류번호:216)\r\n\r\n|"
		   "|EXE_HTTPManager::UpdateOneFile_4|에러 다운로드파일 사이즈 문제.|"
		   "|EXE_HTTPManager::UpdateOneFile_5|파일사이즈가 다릅니다.\r\n파일명:%1\r\n서버파일사이즈:%2, 다운로드한 파일사이즈:%3, 서버파일CRC:%4, 다운로드한파일CRC:%5|"
		   "|EXE_HTTPManager::UpdateOneFile_6|다운로드 받은 파일에 사이즈가 변경되었습니다.\r\n패치정보 파일이 아직 갱신이 안되었거나\r\n바이러스에 감염되었을 가능성이 있습니다.\r\n문제가 지속되면 아래 방법대로 실행해 보세요.\r\n\r\n1. [로스트사가]가 설치된 폴더 제거.\r\n   인터넷 익스플로러 7은 도구->인터넷 옵션->일반 탭->삭제->모두 삭제->[추가 기능에서 저장한 파일 및 설정을 함께 삭제] 체크->예\r\n   인터넷 익스플로러 6은 도구->인터넷 옵션->일반 탭->쿠키 삭제->확인\r\n                                   도구->인터넷 옵션->일반 탭->파일 삭제->[오프라인 항목을 모두 삭제] 체크->|"
		   "|EXE_HTTPManager::UpdateOneFile_7|서버파일사이즈:%1, 다운로드한 파일사이즈:%2, 서버파일CRC:%3|"
		   "|EXE_HTTPManager::UpdateOneFile_8|파일 합치는데 에러가 발생하였습니다.\r\n자동으로 재시도를 시작합니다.(총100회)\r\n프로그램 종료를 원하시면\r\n자동업데이트의 X버튼을 클릭하십시오.\r\n(오류번호:219)\r\n\r\n|"
		   "|EXE_HTTPManager::UpdateOneFile_9|서버파일사이즈:%1, 다운로드한 파일사이즈:%2, 서버파일CRC:%3|"
		   "|EXE_HTTPManager::UpdateOneFile_10|낱개 파일 압축을 푸는데 에러가 발생하였습니다.\r\n자동으로 재시도를 시작합니다.(총100회)\r\n프로그램 종료를 원하시면\r\n자동업데이트의 X버튼을 클릭하십시오.\r\n(오류번호:220)\r\n\r\n|"
		   "|EXE_HTTPManager::UpdateOneFile_11|서버파일사이즈:%1, 다운로드한 파일사이즈:%2, 서버파일CRC:%3|"
		   "|EXE_HTTPManager::UpdateOneFile_12|파일복사중 에러가 발생하였습니다.\r\n자동으로 재시도를 시작합니다.(총100회)\r\n프로그램 종료를 원하시면\r\n자동업데이트의 X버튼을 클릭하십시오.\r\n(오류번호:221)\r\n\r\n|"
		   "|EXE_HTTPManager::UpdateOneFile_13|서버파일사이즈:%1, 다운로드한 파일사이즈:%2|"
		   "|EXE_HTTPManager::UpdateOneFile_14|풀버전 설치가 실패하였습니다.\r\n자동으로 재시도를 시작합니다.(총100회)\r\n프로그램 종료를 원하시면\r\n자동업데이트의 X버튼을 클릭하십시오.\r\n(오류번호:217)\r\n\r\n|"
		   "|EXE_ioErrorDlg::ShowErrorDlg_1|Ver : %1\r\n--- 시스템 사양 ---\r\n%2\r\n\r\n--- 에러 정보 ---\r\n%3|"
		   "|EXE_ioLoginDlg::FirstConnectServer_1|%1서버 네트워크 연결 중.|"
		   "|EXE_ioLoginDlg::OnTimer_1|%1서버 응답이 없어 %2서버 네트워크 연결 중.|"
		   "|EXE_ioLoginDlg::OnTimer_2|%1서버가 응답이 없어 연결 시도 종료, %2서버 네트워크 연결 중.|"
		   "|EXE_ioLoginDlg::SendServer_1|%1서버에 로그인 요청 메세지 전송 완료, 로그인 서버 응답 대기 중.|"
		   "|EXE_ioLoginDlg::SetTextNetworkConnected_1|%1서버 네트워크 연결 성공.|";
}

void ioLocalKorea::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/  )
{
	if( iPasswordType == 0 )
	{
		// password 0: iosuccess#@
		char szEncPassWord[MAX_PASSWORD]={ -105, 112, 108, 127, 62, 66, 9, -43, 53, 4, 64, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password 1: XrFrI0%3BF%!0Dcx$30-
		char szEncPassWord[MAX_PASSWORD]={ -90, 109, 89, 120, 20, 17, 73, -107, 4, 97, 37, 6, 118, -30, 15, 89, 121, 57, 47, 50 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}

DWORD ioLocalKorea::GetResourceIDBackBmp()
{
	return IDB_BACK;
}

DWORD ioLocalKorea::GetResourceIDOptimizeBtnBmp()
{
	return IDB_OPTIMIZE_BTN;
}

DWORD ioLocalKorea::GetResourceIDStartBtnBmp()
{
	return IDB_START;
}

DWORD ioLocalKorea::GetResourceIDErrorBackBtnBmp()
{
	return IDB_ERROR_BACK;
}

DWORD ioLocalKorea::GetResourceIDErrorSolutionBtnBmp()
{
	return IDB_SOLUTIONBTN;
}

DWORD ioLocalKorea::GetResourceIDErrorExitBtnBmp()
{
	return IDB_EXITBTN;
}

const char * ioLocalKorea::GetRegKey()
{
	return "Software\\SP2Client\\";
}

const char * ioLocalKorea::GetFontName()
{
	return "돋움";
}

void ioLocalKorea::CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL )
{
	Help::CreateShortcutOnDeskTop( rszRootDir, rszShortcutName, rszLoginURL );
	Help::CreateMenuIcon( rszShortcutName );

	CString szClientPath = "lostsaga.exe";
	CString szFullPath;
	Help::SetFullPath( rszRootDir, szClientPath, szFullPath);

	char szVersionName[MAX_PATH*2]="";
	Help::GetFileVesrion((LPCTSTR)szFullPath, szVersionName, sizeof(szVersionName), false );
	Help::CreateUninstallRegInfo( rszRootDir, szVersionName );
}

const char * ioLocalKorea::GetErrorSolutionURL()
{
	return "http://www.lostsaga.com/common/rgfaq.asp?errnumber=%d";
}

bool ioLocalKorea::IsFine()
{
	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	if( pDlg && pDlg->IsGameServerID() )
		return true;
	else
		return false;
}
