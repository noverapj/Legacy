#pragma once

#include "../io3DEngine/ioMovingWnd.h"
#include "../ioBaseChar.h"

class MonsterCoinUseWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT					= 1,
		ID_EXIT_MODE            = 2,
		ID_SHOP_LINK            = 3,
		ID_REVIVAL              = 4,
		ID_CALCEL               = 5,
		ID_CONFIRM              = 6,
	};

	enum
	{
		USETYPE_START_COIN_LACK   = 0,
		USETYPE_REVIVAL_COIN      = 1,
		USETYPE_REVIVAL_COIN_LACK = 2,
		USETYPE_JOIN_COIN_LACK    = 3,
		USETYPE_PRESTART_COIN_LACK	= 4,
		USETYPE_PVE_JOIN_COIN_LACK, 
		USETYPE_PVE_START_COIN_LACK,
		USETYPE_PRESTART_RAID_COIN_LACK,
		USETYPE_RAID_START_COIN_LACK,
	};

protected:
	DWORD m_dwCoinUseType;
	ioHashString m_szMent;

protected: // 게임 시작 코인 사용 알림
	int   m_iStartUseCoinCnt;
	bool  m_bStartMonsterCoin;

protected:
	DWORD m_dwRevivalCoinUseTime;

protected:
	ioBaseChar *m_pOwnerChar;

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pExclamationMark;
	int              m_iExitModeSec;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	bool UpdateUseStartCoinLack( const ioHashString &rkMent, int iUseCoinCnt, DWORD dwCurStartCoinSec, DWORD dwMaxStartCoinSec );
	void UpdateUseRevivalCoin( ioBaseChar *pOwnerChar );

public:
	void ShowJoinCoinLack( const ioHashString &rkMent, int iUseCoinCnt, ModeType eMode );
	void ShowPreStartCoinLack( const ioHashString &rkMent, int iUseCoinCnt, ModeType eMode );

	DWORD GetCoinUseType() const { return m_dwCoinUseType; }

public:
	MonsterCoinUseWnd();
	virtual ~MonsterCoinUseWnd();
};

