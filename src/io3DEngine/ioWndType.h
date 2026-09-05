
#ifndef _ioWndType_h_
#define _ioWndType_h_

// Window Style
#define IWS_START_HIDE			0x0001		// 윈도우 생성시 ShowWnd를 호출하지 않음
#define IWS_INACTIVE			0x0002		// 비활성 윈도우 
#define IWS_BACKMOST			0x0004		// 항상 뒤에 있는 윈도우 (GoToTop()을 실행하지 않는다.)
#define IWS_CANMOVE				0x0008		// 이동가능
#define IWS_MODAL				0x0010		// 모달윈도우
#define IWS_EXIT_ESC			0x0020		// ESC로 창이 닫히는 윈도우
#define	IWS_PROCESS_ALWAYS		0x0040		// Show든 Hide 상태에서든 언제나 OnProcess()를 호출한다
#define IWS_PROCESS				0x0080		// OnProcess()를 호출한다 매프레임 체크해야할것이 있다면 이것을 이용
#define IWS_DRAG_DROP			0x0100		// Drag and Drop 가능 윈도우
#define IWS_NO_MOUSE_RESPONSE	0x0200		// 마우스입력을 못받는 윈도우
#define IWS_CHILD_ONLY_RESPONSE 0x0400		// 자기 자신은 입력을 못받고 차일드만 입력을 받을수 있는 윈도우
#define IWS_TOPMOST				0x0800		// 항상위에 있는 윈도우 : TOPMOST끼리는 나중에 뜬 창이 위에 온다.
#define IWS_EXACTIVE            0x1000      // 마우스 입력을 EX로 리턴한다.
#define IWS_MODAL_TOOLTIP		0x2000		// 모달윈도우 용 툴팁
#define IWS_MODAL_EXTEND		0x4000		// 모달 윈도우 EX버젼(배경진하게..)
#define IWS_ISINRECT_CENTER		0x8000		// CENTER 기준 윈도우는 체크 방법을 바꿔줌

// Window Class Type
#define IWT_WND			0x0001
#define IWT_BUTTON		0x0002
#define IWT_RADIO_BTN	0x0003
#define IWT_SCROLL		0x0004
#define IWT_LIST		0x0005
#define IWT_EDIT		0x0006
#define IWT_SLIDE		0x0007
#define IWT_CHECK_BOX	0x0008
#define IWT_TAB_BTN		0x0009
#define IWT_TAB_CTRL	0x0010
#define IWT_MSGBOX		0x0011
#define IWT_CUSTOM_BTN	0x0012
#define IWT_PROGRESS	0x0013

// Child Window Notify Message
#define IOWN_MOVED		0x0001		// 윈도우가 움직였음
#define IOWN_HIDE		0x0002		// 윈도우가 임무를 마치고 숨어버렸음
#define IOWN_LEAVED		0x0003		// 오버된 마우스가 윈도우에서 떠났음
#define IOWN_OVERED		0x0004		// 윈도우에 마우스가 오버되었음
#define IOWN_BTNUP		0x0005		// 윈도우 업버튼

#define IOBN_BTNDOWN	0x0010		// 버튼 다운
#define IOBN_BTNUP		0x0011		// 버튼 업
#define IOBN_RBTNDOWN	0x0012		// 마우스 우클릭 되었다.
#define IOBN_RBTNUP		0x0013		// 마우스 우클릭
#define IOBN_LDBLCLK	0x0014		// 왼쪽마우스버튼 더블클릭

#define IOEX_BTNDOWN	0x0015		// IWS_EXACTIVE 상태일 때 버튼 다운
#define IOEX_BTNUP		0x0016		// IWS_EXACTIVE 상태일 때 버튼 업
#define IOEX_RBTNDOWN	0x0017		// IWS_EXACTIVE 상태일 때 마우스 우클릭 되었다.
#define IOEX_RBTNUP		0x0018		// IWS_EXACTIVE 상태일 때 마우스 우클릭
#define IOEX_LDBLCLK	0x0019		// IWS_EXACTIVE 상태일 때 왼쪽마우스버튼 더블클릭

#define IOSD_SCROLL		0x0020		// 슬라이드 무브
#define IOLS_SELECT		0x0021		// 리스트 셀렉트
#define IOED_INPUT		0x0022		// 에디트박스에 글이 입력되었음	, param (true: enter, false: tab)

#define IOED_EX_NEWLINECHAR	0x0023	// IWS_EX_MULTI_EDIT 타입의 에디트박스에 개행문자가 입력되거나 제거됨(parm 미사용)

//#define IOMS_MESSAGE	0x0011		// 메세지 박스에서 메세지 도착

#define IOCM_USER_DEFINED 0x1000	// 유저 정의 커멘드 메세지 iwm_command에 cmd로 올 메세지 

// Window Extend Style
#define IWS_EX_EDIT_PW		  0x0001	// 패스워드 에디트박스
#define IWS_EX_EDIT_RIGHT	  0x0002	// 우측정렬 에디트박스
#define IWS_EX_EDIT_CENTER	  0x0004	// 중앙정렬 에디트박스
#define IWS_EX_MULTI_EDIT	  0x0008	// 개행문자 입력가능한 멀티 에이트 박스
#define IWS_EX_NOTIFY_MOVED	  0x0010	// MoveWindow시 부모에게 Notify
#define IWS_EX_OVER_SCREEN	  0x0020	// Screen버튼이 아니지만 오버시 Screen 출력
#define IWS_EX_PUSH_SCREEN	  0x0040	// Screen버튼이 아니지만 클릭시 Screen 출력
#define IWS_EX_HSCROLL        0x0080    // HScroll
#define IWS_EX_VSCROLL        0x0100    // VScroll
#define IWS_EX_PUSH_DOUBLE	  0x0200	// Push상태에서 Push이미지 두번출력 ( 기존의 ioStretchButton효과 )
#define IWS_EX_EDIT_NUM		  0x0400	// 숫자 에디트박스
#define IWS_EX_EDIT_CHECK_FOCUS    0x0800     //마우스 클릭시 포커스를 체크한다. 예 : 바탕화면 클릭시 에디트박스 킬 포커스
#define IWS_EX_PUSHED_OVER_SCREEN  0x1000		// 눌려진상태에서 오버되는경우 스크린 출력
#define IWS_EX_SHOW_NO_TOP         0x2000       // Show할 때 GoToTop 실행 안함
#define IWS_EX_SHOW_HIDE_ACTION    0x4000       // Show&Hide될 때 애니메이션 발생
#define IWS_EX_NO_REND_EDIT_NUM	   0x8000       // 입력값을 직접 출력하지않는 숫자 에디트 박스

// IWS_EX_PUSH_SCREEN, IWS_EX_PUSH_DOUBLE, IWS_EX_PUSHED_OVER_SCREEN 는 두개이상 세팅되도 순서대로 하나만 작동한다. 

enum EXWINDOWPARAMETER
{
	EXWP_NONE		 = 0x0000,
	EXWP_SCALE_TOP   = 0x0001, //상단을 기점으로 스케일
	EXWP_SCALE_DOWN  = 0x0002, //하단을 기점으로 스케일
};

enum IWDropEffect
{
	IW_DROP_NONE,
	IW_DROP_ENABLE,
};

#define TR1BINDNA(x,y) std::tr1::bind(x,y) 
#define TR1BIND(x,y) std::tr1::bind(x,y,std::tr1::placeholders::_1)

#endif
