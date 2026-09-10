#pragma once

#include "../io3DEngine/io3DCommon.h"
#include "../io3DEngine/ioPrerequisites.h"
#include "../io3DEngine/ioSingleton.h"
#include "ioKeyManager.h"

class ioBaseChar;
class ioPlayStage;

struct ChatIconInfo
{
	ioHashStringVec m_ChatIconStringList;
	ioHashStringVec m_ChatIconSoundList;
	ioHashString m_ChatIconName;
};
typedef std::vector< ChatIconInfo > ChatIconInfoList;

class ioChatMacro: public Singleton< ioChatMacro >
{
protected:
	ChatIconInfoList m_ChatIconList;
	ioHashString m_DefaultChatIcon;
	ioHashString m_DefaultChatSound;

	int m_iCurChatCnt;
	int m_iLimiteChatCnt;
	DWORD m_dwMacroChatCheckTime;
	DWORD m_dwMacroChatGapTime;
	DWORD m_dwDisableTime;
	DWORD m_dwDisableGapTime;

	int m_iCurNormalChatCnt;
	int m_iNormalChatLimiteCount;
	DWORD m_dwNormalChatCheckTime;
	DWORD m_dwNormalChatGapTime;

	int m_iCurSoundCnt;
	int m_iSoundLimiteCount;
	DWORD m_dwSoundCheckTime;
	DWORD m_dwSoundGapTime;

public:
	void LoadChatMacroInfo();
	
	void SetChatMacro( const ioBaseChar *pOwnerChar,
		               const CKeyInfo::MecroTextInfo *pInfo,
		               ModeType eModeType = MT_NONE,
					   const int iRoomIndex=-1,
					   ioPlayStage *pPlayStage = NULL );

	const ioHashString& GetChatIcon( ioHashString szChat );

	bool CheckMacroChatEnable();
	bool CheckNormalChatEnable();
	bool IsCheckStealthMode( ioPlayStage *pPlayStage );

protected:
	void FilterChar(const char *src, char *dst, const char *chars);

public:
	static ioChatMacro& GetSingleton();

public:
	ioChatMacro();
	~ioChatMacro();
};

#define g_ChatMacroInfo ioChatMacro::GetSingleton()

extern char *s_Macro[MAX_PATH]; 
extern char *s_Macro_us[MAX_PATH]; 

