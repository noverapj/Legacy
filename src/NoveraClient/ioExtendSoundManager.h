#pragma once

class ExSound
{
public:
	enum ExSoundType
	{
		EST_NONE				= 0,	//사운드 없음
		EST_COMPANY				= 1,	//회사로고 사운드
		EST_LOGIN				= 2,	//최초 로비 입장시 사운드
		EST_LOGOUT				= 3,	//게임 종료 팝업 뜰떄 사운드
		EST_SHOP_ENTER			= 4,	//상점 입장시
		EST_SHOP_BUY_SUCCESS	= 5,	//상점에서 아이템 구매 성공시
		EST_PRESENT_POPUP		= 6,	//선물 윈도우가 떳을때
		EST_REINFORCE_SUCCESS	= 7,	//강화(차원/기존) 성공시
		EST_REINFORCE_FAIL		= 8,	//강화(차원/기존) 실패시
		EST_PET_MIX_SUCESS		= 9,	//펫 합성 성공시
		EST_GASHAPON_GET		= 10,	//가챠 획득시
		EST_QUEST_COMPLETE		= 11,	//퀘스트 완료 팝업이 떳을때
		EST_MISSION_COMPLETE	= 12,	//미션 완료 팝업이 떳을때
		EST_HQ_ENTER			= 13,	//훈련소 입장시
		EST_GUILD_HQ_ENTER		= 14,	//길드본부 입장시
		EST_HOME_ENTER			= 15,	//개인본부 입장시
		EST_WIN					= 16,	//승리
		EST_LOSE				= 17,	//패배
		EST_AWARD				= 18,	//시상식
		EST_PRESENT_BOX_RESULT	= 19,	//선물함 받기 후 결과창
	};

	ExSoundType		m_SoundType;
	ioHashString	m_SoundName;
	bool			m_bThread;
	bool			m_bIgnoreNextSound;	//재생 중일 때 다음 사운드가 재생 되지 못하도록 함(최 우선 순위)
	bool			m_bCutIgnore;		//재생 중일 때 아래 m_bCutPrevSound로 못 자르게 함(차 우선 순위)
	bool			m_bCutPrevSound;	//이전에 재생중인 사운드가 있다면 자르고 재생(촤 하위)
	

	ExSound()
	{
		m_SoundType			= EST_NONE; 
		m_bThread			= false;
		m_bIgnoreNextSound	= false;
		m_bCutIgnore		= false;
		m_bCutPrevSound		= false;
	}
	virtual ~ExSound(){};
};
typedef std::vector<ExSound> ExSoundList;

class ioExtendSoundManager : public Singleton< ioExtendSoundManager >
{
protected:
	ExSoundList		m_ExSoundList;
	UniqueObjID		m_CurrSoundID;
	ioHashString	m_CurrSoundName;
	bool			m_bIgnoreNextSound;
	bool			m_bCutIgnore;

public:
	static ioExtendSoundManager& GetSingleton();

public:
	virtual void Initialize();

	bool IsSound( ExSound::ExSoundType eSound );
	void PlaySound( ExSound::ExSoundType eSound );

public:
	ioExtendSoundManager();
	virtual ~ioExtendSoundManager();
};

#define g_ExSoundMgr ioExtendSoundManager::GetSingleton()