#pragma once

#include "ioLocalManager.h"


class ioLocalParent
{
protected:
	ioHashString m_sErrorSolutionURL;
	ioHashString m_sGuildMaster;
	ioHashString m_sGuildSecondMaster;
	ioHashString m_sGuildGeneral;

public:
	virtual ioLocalManager::LocalType GetType() = 0;
	virtual const char *GetTextListFileName() = 0;
	virtual const char *GetMemTextList() = 0;

	virtual DWORD GetResourceIDTitleCashBmp()  = 0;
	virtual DWORD GetResourceIDTitleEntryBmp() = 0;
	virtual DWORD GetResourceIDTitleGuildBmp() = 0;
	virtual DWORD GetResourceIDTitleGuildMarkBmp() = 0;
	virtual DWORD GetResourceIDTitleHomeBmp() = 0;
	virtual DWORD GetResourceIDTitleBmp() = 0;

	virtual DWORD GetResourceIDErrorBack() = 0;
	virtual DWORD GetResourceIDErrorExitC() = 0;
	virtual DWORD GetResourceIDErrorExitN() = 0;
	virtual DWORD GetResourceIDErrorExitO() = 0;
	virtual DWORD GetResourceIDErrorSolutionC() = 0;
	virtual DWORD GetResourceIDErrorSolutionN() = 0;
	virtual DWORD GetResourceIDErrorSolutionO() = 0;

	virtual void SetLoginData( const ioHashString &szBillingUserKey ) = 0;
	//virtual void FillLoginData( OUT SP2Packet &rkPacket ) = 0;

	virtual void GetClassTypeForTutorialDropItem( int &riClassType ) = 0;

	virtual const char *GetGuildMasterPostion();
	virtual const char *GetGuildSecondMasterPosition();
	virtual const char *GetGuildGeneralPosition();
	virtual const char *GetErrorSolutionURL() = 0;
	virtual const char *GetBillingErrorMent() = 0;

	virtual int  GetFirstIDMaxSize() = 0;
	virtual bool IsShowFirstIDWnd() = 0;

	virtual void OpenFillCashPage() = 0;
	virtual bool IsRightNewID( const char *szID ) = 0;
	virtual void SetTimeZone() = 0;
	virtual bool IsSendGashaponList() = 0;
	virtual void GetChangedString( OUT char *szCheckString ) = 0;

	virtual bool IsExecuteLuncher() = 0;
	virtual bool IsKoreaGameLimit() = 0;
	virtual bool IsCheckKorean() = 0;
	virtual bool IsFirstIDCheckPass() { return false; }
	virtual bool IsMyHomePage() = 0;
	virtual bool IsShowExtraGoodsList() = 0;
	virtual bool IsShowRegulation() = 0;
	virtual bool IsStartFullScreen(){ return false; }
	virtual bool IsTooltipAutoLine(){ return false; }
	virtual bool IsMileage() { return false; }
	virtual bool IsGatotkacaPremium( int iClassType ) { return false; }

	virtual bool SetPresentRecvDisableMsg( int iPresentType ) = 0;

    virtual int GetLimitGradeLevel() = 0; // -1이면 limit가 없음.

	virtual bool IsEndFishingByMove() { return true; }

	virtual bool IsRightLicense();
	virtual int GetLicenseDate() { return 0; }
	virtual bool IsShowFirstSoldierSelectWnd() { return false; }

	virtual void ExitAction(){}
	virtual void ShowHackShieldErrorDlg( const ioHashString &rsPrivateID ){}

	virtual DWORD GetCashSendTime() { return 180000; } // ms
	virtual bool IsHeadquarterWebPage() { return false; }
	virtual bool IsFirstIDControlAltCtrl() { return true; }
	virtual bool IsPCRoomBonusSoldier() { return false; }

	virtual bool IsCheckValueServer() { return false; }
	virtual bool IsChangeServerHelp() { return false; }
	virtual bool IsSendCollisionPos() { return true; }
	virtual bool IsLimitStartCancel() { return false; }
	virtual bool IsCreateMiniDump() { return false; }
	virtual bool IsTickCheck() { return true; }
	virtual bool IsAnnounceShutDown() { return false; }
	virtual bool IsUseMedalLevelOpen() { return true; }
	virtual bool IsUsePresentMacroInShipping() { return true; }
	virtual bool IsSendChatLog() { return true; }
	virtual bool IsUseVoiceChat() { return true; }
	virtual bool IsMemoryLoadMapPac() { return false; }
	
	virtual int GetCashPageWidth() { return 0; }
	virtual int GetCashPageHeight() { return 0; }
	virtual void OnCashPageHide(){}
	virtual int GetProgramExitWaitSec() { return 60; } // 1분
	virtual void GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL ){}

	virtual void LoadINI();

	virtual void SetWebLoginKey( const ioHashString &rsWebLoginKey ) {}

	virtual int GetHighSpecWidth() { return 1024; }
	virtual int GetHighSpecHeight() { return 768; }

public:
	ioLocalParent(void);
	virtual ~ioLocalParent(void);
};

