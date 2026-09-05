#pragma once

// 이벤트만 리턴한다.
class ChatMacro
{
public:
	enum
	{
		// 타입은 array 사용되므로 값을 변경하지 말것.
		NONE_MACRO = -1,
		MACRO_HELP = 0,						//    메크로 입력이 잘못 되었음.
		MACRO_QUEST_GUIDE,					//    퀘스트가이드
		MACRO_QUEST_OCCUR,					//    퀘스트발생
		MACRO_QUEST_ATTAIN,					//    퀘스트달성
		MACRO_QUEST_DELETE,					//    퀘스트초기화
		MACRO_EXIT_LADDER,					//    나가기(진영팀)
		MACRO_PRESENT_DELETE,				//    선물함 클리어
		MACRO_CHANNEL_INFO,					//    채널정보(훈련소&광장에서만 가능하며 유저 리스트 출력) /채널정보 페이지
		MACRO_CONTRIBUTE,					//    개발자 전용 : 기여도 로그
		MACRO_MEMO,							//    쪽지 보내기(미사용)
		MACRO_INVITE_CHANNEL,				//    채널 초대(미사용)
		MACRO_ENTER_PLAZA,					//    용병광장 들어가기(미사용)
		MACRO_NONE_UI,						//    UI출력안함
		MACRO_USER_INFO,					//    유저정보윈도우
		MACRO_CHAT_MODE,					//	   용병광장 대화모드(미사용)
		MACRO_INIT_UI_POS,					//    UI위치 초기화(미사용)
		MACRO_AWARD,						//    개발자 전용 : 시상식
		MACRO_INFO,							//    개발자 전용 : 정보
		MACRO_ROOM_ENTER,					//    개발자 전용 : 훈련
		MACRO_UDP,							//    개발자 전용 : UDP 패킷 
		MACRO_MOVE_PACKET,					//    개발자 전용 : 이동패킷 로그(미사용)
		MACRO_GUILD_INFO,					//    길드정보윈도우
		MACRO_TIME,							//    시간 안내
		MACRO_SHOW_ANNOUNCEMENT,			//    공지윈도우(AnnouncementWnd) show
		MACRO_BLOCK_GULID_MARK,				//    개발자 전용 : 길드 마크 변경
		MACRO_MOVIE_MODE,					//    영화모드
		MACRO_USER_RECORD,					//    개발자 전용 : 전적 로그
		MACRO_TEAM_INFO,					//    개발자 전용 : 팀 정보
		MACRO_CHANNEL_MOVE,					//    채널 이동(본부에서만 가능)	
		MACRO_CAMP_INFO,					//    개발자 전용 : 진영유저
		MACRO_CAMP_USER,  					//    개발자 전용 : 대회포인트
		MACRO_GUILDNAME_CHANGE,				//    개발자 전용 : 길드명 변경 테스트
		MACRO_EVENT_TEST,					//    개발자 전용 : 이벤트 테스트
		MACRO_WHOLE_CHAT_OFF,				//	   외침끄기
		MACRO_WHOLE_CHAT_ON,				//	   외침켜기
		MACRO_CAMERA_TEST,					//    개발자 전용 : 맵 카메라 이벤트 테스트
		MACRO_FONT_TEXTURE,					//    폰트텍스쳐
		MACRO_SHOW_WND_ID,					//    Show 윈도우 확인
		MACRO_CHAT_INFO_OFF,				//    채팅 알림 메세지 off
		MACRO_CHAT_INFO_ON,					//    채팅 알림 메세지 on
		MACRO_CONTROL_SYNC_EX_ON,			//    Control Packet의 인덱스 체크를 다른 패킷에도 확대
		MACRO_CONTROL_SYNC_EX_OFF,			//    Control Packet의 인덱스 체크를 Control Packet에만 적용
		MACRO_EVENT_SHOP_GOODS_LIST,		//    개발자 전용 : 이벤트 상품 목록 가져온다.	/10
		MACRO_EVENT_SHOP_GOODS_BUY,			//    개발자 전용 : 이벤트 상품 구매.			/11
		MACRO_EVENT_SHOP_STATE,				//    개발자 전용 : 이벤트 상점 On / Off.		/ShopClose = 0(Open), 1(Close)
		MACRO_EVENT_SHOP_BUY_USER_CLEAR,	//    개발자 전용 : 이벤트 상점 구매자 목록 삭제/BuyListClear
		MACRO_CUSTOM_DDS_FROM_JPG,			//    개발자 전용 : DDS 파일을 JPG로 변경
		MACRO_CUSTOM_JPG_MIX_DDS_FROM_DDS,  //    개발자 전용 : JPG와 원본 DDS 파일 합쳐서 DDS로 변경
		MACRO_CUSTOM_DDS_FROM_LSC,			//    개발자 전용 : DDS 파일을 LSC로 변경
		MACRO_CUSTOM_LSC_FROM_DDS,			//    개발자 전용 : LSC 파일을 DDS로 변경
		MACRO_CUSTOM_ENTITY_TEXTURE,        //    개발자 전용 : 해당 부위의 커스텀 가능한 텍스쳐에 색칠함.
		MACRO_SHOW_WND,						//    개발자 전용 : 윈도우 쇼
		MACRO_PRESENT,                      //    사내 admin 전용 : 선물 보내기
		MACRO_BEST_FRIEND_APPLICATION,      //    사내 admin 전용 : 절친 신청
		MACRO_BEST_FRIEND_AGREE,            //    사내 admin 전용 : 절친 수락
		MACRO_BEST_FRIEND_REFUSE,           //    사내 admin 전용 : 절친 거절
		MACRO_BEST_FRIEND_CLEAR,            //    사내 admin 전용 : 절친 해제
		MACRO_EXPERIENCE_STATE,             //    사내 admin 전용 : 체험상태
		MACRO_MULTI_MAP_CHANGE,             //    사내 admin 전용 : 멀티맵 인덱스 변경 
		MACRO_MACRO_NOTIFY,                 //    매크로 유저 신고
		MACRO_EXTRAITEM_GROWTH_CATALYST,    //    장비 성장 아이템 : /장비성장 
		MACRO_GROWTH_LEVELUP,				//    사내 admin 전용 : 타워디펜스 모드 시 150레벨 업 매크로
		MACRO_MONSTER_CLOSE,				//    광장에 몬스터 생성되지 않는 매크로  /MonsterClose = 0, 1    0이면 안 나온다. 1이면 나오고
		MACRO_BINGO_PRESENT,				//	  빙고( sp2_bingo.ini )에 설정된 선물 모두다 받기.
		MACRO_MONSTER_SPAWN,				//    광장에 곰소환하기 /곰소환 출근, 퇴근 
		MACRO_SUPERGASHAPON_GET_ALL,		//	  개발자 전용 : 지정된 EtcItemCode의 아이템 패키지를 전부 가져옴
		MACRO_SUPERGASHAPON_GET,			//	  개발자 전용 : 지정된 EtcItemCode의 특정 아이템 패키지를 가져옴
		MACRO_SUPERGASHAPON_LIMIT_INFO_GET,	//	  개발자 전용 : 지정된 EtcItemCode의 특정 아이템 Limit 정보을 가져옴
		MACRO_SUPERGASHAPON_LIMIT_RESET,	//    개발자 전용 : 지정된 EtcItemCode의 특정 아이템 Limit 정보을 초기화 시킴
		MACRO_SEND_RECIVE_CHECK_ON,			//	  개발자V 전용 : 패킷 전송, 받기 체크 시작
		MACRO_SEND_RECIVE_CHECK_OFF,		//	  개발자V 전용 : 패킷 전송, 받기 체크 종료
		MACRO_ATTENDANCE_WND_SHOW,			//	  개발자 전용 : 출석체크 윈도우 Show
		MACRO_ATTENDANCE_ADD_DAY,			//	  개발자 전용 : 출석일자 추가
		MACRO_ATTENDANCE_ADD_PREV_MONTH,	//	  개발자 전용 : 이전달 출석데이터 추가
		MACRO_ATTENDANCE_RESET,				//	  개발자 전용 : 출석부 초기화
		MACRO_ATTENDANCE_SET_DATE,			//	  개발자 전용 : 출석부 날짜 수정
		MACRO_AWAKE_SET_DATE,				//	  개발자 전용 : 종료시간
		MACRO_CHAR_POWER_UP_ADD,			//    테스트 전용 : 용병 강화 추가
		MACRO_ITEM_SLOT_INDEX_FIND,			//	  테스트 전용 : 아이템 슬롯 엔덱스 찾기
		MACRO_ITEM_POWER_UP_ADD,			//    테스트 전용 : 아이템 진화 추가
		MACRO_CANNOT_OPEN_DDS_FILE,			//    테스트 전용 : 불가능한 DDS 파일 Log로 출력
		MACRO_CHAR_INFO_WND_SHOW,			//    테스트 전용 : 캐릭터 아이템 정보 및 버프 정보 출력
		MACRO_SET_MISSION,					//    테스트 전용 : 입력 타입의 ( 현재 미션 날짜 + 입력 일 ) 이후 미션 세팅
		MACRO_SET_DATE_MISSION,				//    테스트 전용 : 입력 타입의 다음날짜 미션 세팅
		MACRO_SET_MISSION_VALUE,			//	  테스트 전용 : 입력 받은 만큼 미션 갱신.
		MACRO_ROLLBOOK_WND_SHOW,			//	  테스트 전용 : 누적 출석부 보기
		MACRO_ROLLBOOK_PROGRESS,			//	  테스트 전용 : /누적출석 횟수   해당 횟수만큼 출석이 진행됩니다.( 현재출석 + 입력 횟수(<8) )
		MACRO_GASHAPON_GET_ALL,				//	  개발자 전용, 알파서버 전용 : 지정된 EtcItemCode의 가챠형태 아이템 패키지를 전부 가져옴
		MACRO_FPS_LOWER,					//	  프레임 출력 매크로(소문자)
		MACRO_FPS_UPPER,					//	  프레임 출력 매크로(대문자)
		MACRO_TIMER_CHANGE,					//	  프레임 시간API 변경
		MACRO_FPS_LAG_CHECK,				//	  프레임 체크 매크로 (렉권)
		MACRO_ALL_LSC_FROM_DDS,				//    개발자 전용 : LSC 파일을 DDS로 변경		
		MACRO_HOUSING_TEST,					//    하우징 시스템 검증 매크로
		MACRO_HOUSING_CMD,					//    하우징 시스템 커맨드 매크로
		MACRO_HOUSING_SAVE,					//    하우징 블럭 설치 정보 INI로 저장
		MACRO_COMFORTABLE_LIFE,				//    편하게 살자 윈도우(내부 개발용) 매크로
		MACRO_TOURNAMENT_NEXT_STATE,		//    대회용 매크로 : 다음 스텝 강제 진행
		MACRO_TOURNAMENT_RESULT,			//    대회용 매크로 : 현재 대회의 보상 쿼리를 강제 호출
		MACRO_SHOW_POPUP,					//	  팝업 오픈
		MACRO_PRACTICE_COMPLETE,			//    현재 들어간 수련장 완료 시키기
		MAX_MACRO,
	};

protected:
	static char m_szMacro[MAX_MACRO][MAX_PATH];

public:
	static void SetMacroText();
	static char *GetMacroText( int iMacroType );

public:
	int IsMacro(char *szChat,int &key_word);

public:
	ChatMacro();
	virtual ~ChatMacro();
};
