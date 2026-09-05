<% OPTION EXPLICIT %>

<%
Dim db
Dim dbcon
Dim sql
Dim rs
Dim gameServerID

Dim AUTOUPGRADE_EXE_CRC										'자동업그래이드 실행 파일 CRC 값
Dim AUTOUPGRADE_INFO_INI_CRC								'자동업그래이드 정보 파일 CRC 값
Dim HTTP_AUTOUPGRADE_EXE									'자동업그래이드 다운로드 URL
Dim FULLZIP_VERSION												'전체 클라이언트 설치 버전
Dim PATH_SETUP_FOLDER_AUTOUPGRADE_INI				'자동업그래이드 정보파일 기본 전체 설치 경로
Dim HTTP_AUTOUPGRADE_INI										'자동업그래이드 정보파일 다운로드 URL
Dim EXTEND_KEY														'자동업그래이드에서 사용하는 각종 수치

Dim PATH_SETUP_FOLDER_AUTOUPGRADE_EXE			'자동 업그래이드 실행 파일 기본 설치 경로
Dim PATH_SETUP_FOLDER_ROOT								'로스트사가 기본 설치 경로
Dim PATH_SETUP_FOLDER_INI									'자동 업그래이드 정보 파일 기본 설치 폴더
Dim PATH_SETUP_FOLDER_FULLZIP								'풀집 전체 경로
Dim PATH_BROKERFILENAME										'BROKER 파일 이름
Dim servID																	'접속 서버 아이디


Set db = Server.CreateObject("ADODB.Connection")
dbcon = "Provider=SQLOLEDB.1;Persist Security Info=True;Initial Catalog=LosaGame;Data Source=59.18.95.154;User ID=LSUSgame;Password=gkdrn!#ehtl38;"
db.Open dbcon

sql = "select count(*) from define_game_server where [status]=1"
Set rs = db.Execute(sql)

If rs(0) = 0 Then
	rs.close
	Set rs=Nothing
	db.close
	Set db=Nothing

	Response.write "0"
	Response.End
End If

sql = "select top 1 serverID from define_game_server where connNumber < 2000 and [status]=1 order by connNumber asc"
Set rs = db.Execute(sql)

If rs.eof and rs.bof Then
	rs.close
	Set rs=Nothing
	db.close
	Set db=Nothing

	Response.write "1"
	Response.End
Else
	gameServerID	= rs("serverID")
End If

rs.close
Set rs=Nothing
db.close
Set db=Nothing


AUTOUPGRADE_EXE_CRC									= "2426961129"
AUTOUPGRADE_INFO_INI_CRC							= "885771440"
HTTP_AUTOUPGRADE_EXE									= "http://cabinet.ogplanet.com/games/LostSaga/Live/AutoUpgrade/autoupgrade.exe"
FULLZIP_VERSION												= "2009,9,23,0?1"
PATH_SETUP_FOLDER_AUTOUPGRADE_INI			= "C:\\Program Files\\LostSaga\\Info\\autoupgrade_info.ini"
HTTP_AUTOUPGRADE_INI									= "http://cabinet.ogplanet.com/games/LostSaga/Live/AutoUpgrade/autoupgrade_info.ini"
EXTEND_KEY														= "EDEW3940FVDP4950,10,20,30,1,autoupgrade_info.ini,0,0,1,365849202"

PATH_SETUP_FOLDER_AUTOUPGRADE_EXE			= "C:\\Program Files\\LostSaga\\autoupgrade.exe"
PATH_SETUP_FOLDER_ROOT								= "C:\\Program Files\\LostSaga"
PATH_SETUP_FOLDER_INI									= "C:\\Program Files\\LostSaga\\Info"
PATH_SETUP_FOLDER_FULLZIP							= "C:\\Program Files\\LostSaga\\full.zip"
PATH_BROKERFILENAME										= "\\lswebbroker.exe"

; 아래 servID값을 DB에서 가져온 값으로 설정하지 마시고
; 아래 2개의 값중 랜덤하게 1개를 넣어주세요. 아주 단순하게 servID = "60169927248195"  이렇게만 하셔도 됩니다.
; 60169927248195   / 60169944025411
;
servID																= gameServerID

Response.write PATH_SETUP_FOLDER_AUTOUPGRADE_EXE & "?"
Response.write AUTOUPGRADE_EXE_CRC & "?"
Response.write PATH_SETUP_FOLDER_AUTOUPGRADE_INI & "?"
Response.write AUTOUPGRADE_INFO_INI_CRC & "?"
Response.write PATH_SETUP_FOLDER_ROOT & "?"
Response.write HTTP_AUTOUPGRADE_EXE & "?"
Response.write PATH_SETUP_FOLDER_INI & "?"
Response.write HTTP_AUTOUPGRADE_INI & "?"
Response.write PATH_SETUP_FOLDER_FULLZIP & "?"
Response.write EXTEND_KEY & "?"
'enKey, 
Response.write "0" & "?"
Response.write servID & "?"
Response.write FULLZIP_VERSION & "?"
Response.write PATH_BROKERFILENAME
%>

