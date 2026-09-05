
#pragma once

#define WIN32_LEAN_AND_MEAN

#define __EX __declspec(dllimport)

#define _WIN32_WINNT   0x501              //WM_MOUSEWHEEL 상수를 사용하기 위해 define하였음. winuser.h에 보면 #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
#define _WIN32_WINDOWS 0x501              //이렇게 WM_MOUSEWHEEL사용을 제한하고 있다. 혹은 WM_MOUSEWHEEL은 0x020A이므로 바로 사용해도 상관없음.

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

#ifdef SHIPPING
#define _ENCRYPT_STRING

__forceinline unsigned int return1() { __asm mov eax, 1 };
__forceinline unsigned int return5() { __asm mov eax, 5 };
__forceinline unsigned int return8() { __asm mov eax, 8 };
__forceinline unsigned int return9() { __asm mov eax, 9 };
__forceinline unsigned int return10() { __asm mov eax, 10 };
__forceinline unsigned int return40() { __asm mov eax, 40 };
__forceinline unsigned int return62() { __asm mov eax, 62 };
__forceinline unsigned int return90() { __asm mov eax, 90 };
__forceinline unsigned int return100() { __asm mov eax, 100 };
__forceinline unsigned int return150() { __asm mov eax, 150 };
__forceinline unsigned int return344() { __asm mov eax, 344 };
__forceinline unsigned int return360() { __asm mov eax, 360 };
__forceinline unsigned int return500() { __asm mov eax, 500 };
__forceinline unsigned int return1000() { __asm mov eax, 1000 };
__forceinline unsigned int return1000000000() { __asm mov eax, 1000000000 };
__forceinline unsigned int return_PI() { __asm mov eax,   3141592654 };

#define FLOAT1		((float)return1())
#define FLOAT5		((float)return5())
#define FLOAT8		((float)return8())
#define FLOAT9		((float)return9())
#define FLOAT10		((float)return10())
#define FLOAT40		((float)return40())
#define FLOAT62		((float)return62())
#define FLOAT90		((float)return90())
#define FLOAT100	((float)return100())
#define FLOAT150	((float)return150())
#define FLOAT344	((float)return344())
#define FLOAT360	((float)return360())
#define FLOAT500	((float)return500())
#define FLOAT1000	((float)return1000())
#define FLOAT08		((float)(FLOAT8/FLOAT10))
#define FLOAT09		((float)(FLOAT9/FLOAT10))
#define FLOAT05		((float)(FLOAT5/FLOAT10))
#define FLOAT0001	((float)(FLOAT1/FLOAT1000))

#define FLOAT1000000000	((float)return1000000000())
#define FLOAT_PI	((float)(return_PI()/FLOAT1000000000))

#else

#define FLOAT1		1.f
#define FLOAT05		0.5f
#define FLOAT08		0.8f
#define FLOAT09		0.9f
#define FLOAT10		10.f
#define FLOAT40		40.f
#define FLOAT62		62.f
#define FLOAT90		90.f
#define FLOAT100	100.f
#define FLOAT150	150.f
#define FLOAT344	344.f
#define FLOAT360	360.f
#define FLOAT500	500.f
#define FLOAT1000	1000.f
#define FLOAT0001	0.001f
#define FLOAT_PI	3.141593f

#endif

// 국가 코드 페이지. 기본은 Korean 949
#if defined( SRC_INDONESIA )
#define COUNTRY_CODE_PAGE	1252
#elif defined( SRC_NA )
#define COUNTRY_CODE_PAGE	1252
#elif defined( SRC_TAIWAN )
#define COUNTRY_CODE_PAGE	950
#elif defined( SRC_THAILAND )
#define COUNTRY_CODE_PAGE	874
#elif defined( SRC_EU )
#define COUNTRY_CODE_PAGE	1252
#elif defined( SRC_LATIN )
#define COUNTRY_CODE_PAGE	1250
#else
#define COUNTRY_CODE_PAGE	949
#endif

#define SKILLCOOL_COUNT

#define ID_HOTKEY_PS				"Hot_Key_Ps"
#define ID_HOTKEY_Ctrl_PS			"Hot_Key_Ctrl_Ps"
#define ID_HOTKEY_Shift_PS			"Hot_Key_Shift_Ps"
#define ID_HOTKEY_Win_PS			"Hot_Key_Win_Ps"
#define ID_HOTKEY_Alt_PS			"Hot_Key_Alt_Ps"
#define ID_HOTKEY_Ctrl_Shift_PS		"Hot_Key_Ctrl_Shift_Ps"	
#define ID_HOTKEY_Ctrl_Win_PS		"Hot_Key_Ctrl_Win_Ps"


#include "../io3DEngine/ioPrerequisites.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <dsound.h>
#include <tchar.h>
#include <strsafe.h>
#include <atlbase.h>	
#include <atlcom.h>		
#include <atlhost.h>
#include <GdiPlus.h>
#include <Wininet.h>
#include <atltime.h>
#include "typeinfo.h"

#define CLASSTYPEQUALS( ClassName ) typeid( ClassName )

#include "ConsoleTextColorDefine.h"
#include "../LSLog/include/LSLog.h"

#include "../io3DEngine/io3DCommon.h"
#include "../io3DEngine/ioVertexFormat.h"
#include "../io3DEngine/ioFrameTimer.h"
#include "../io3DEngine/ioTimer.h"

#include "../TownPortal/include/TownPortal.h"
#include "ioVoiceChat/VoiceChat.h"
#include "LuaState/cLua.h"

#pragma
extern "C" {
#include "LuaState/lua.h"
#include "LuaState/lualib.h"
#include "LuaState/lauxlib.h"
}

#ifdef _DEBUG
	#pragma comment(lib, "../../lib/LSLogd.lib" )
	#pragma comment(lib, "../../lib/io3DEngined.lib")
	#pragma comment(lib, "../../lib/TownPortald.lib")
	#pragma comment(lib, "ioVoiceChat/ioVoiceChatd.lib")
	#pragma comment(lib, "LuaState/LuaStated.lib")
	#pragma comment(lib, "../../lib/ioPacd.lib" )
	#pragma comment(lib, "gdiplus" )
#else 
	#ifdef SHIPPING
		#pragma comment(lib, "../../lib/LSLog.lib" )
		#pragma comment(lib, "../../lib/lib_Shipping/io3DEngine.Lib")
		#pragma comment(lib, "../../lib/TownPortal.lib")
		#pragma comment(lib, "../../lib/CrashFind.lib")
		#pragma comment(lib, "ioVoiceChat/ioVoiceChat.lib")
		#pragma comment(lib, "LuaState/LuaState.lib")
		#pragma comment(lib, "../../lib/ioPac.lib" )
		#pragma comment(lib, "gdiplus" )
	#else
		#pragma comment(lib, "../../lib/LSLog.lib" )
		#pragma comment(lib, "../../lib/io3DEngine.Lib")
		#pragma comment(lib, "../../lib/TownPortal.lib")
		#pragma comment(lib, "../../lib/CrashFind.lib")
		#pragma comment(lib, "ioVoiceChat/ioVoiceChat.lib")
		#pragma comment(lib, "LuaState/LuaState.lib")
		#pragma comment(lib, "../../lib/ioPac.lib" )
		#pragma comment(lib, "gdiplus" )
	#endif
#endif

#include "VeryOftenUseDefine.h"
#include "GameEnumType.h"
#include "StringProtect.h"
#include "DataHeaders/DataEnum.h"
typedef std::vector< DWORD > DWORDVec;

extern "C" {
	typedef struct 
	{
		const char *name;
		int (*func)(lua_State *);
	}luaDef;
}

#define USE_LIST_ARQUMENTS_TEXT_FUNCTION // 해외버전은 가변인자 사용하면 안되므로 주석 처리해서 테스트



//////////////////////////////////////////////////////////////////////////
// PCH 추가		
//////////////////////////////////////////////////////////////////////////
#include "../io3DEngine/ioGUIManager.h"
#include "../io3DEngine/ioUIFrameManager.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioEffect.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/ioEntityGroup.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/Safesprintf.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioCPU.h"
#include "../io3DEngine/ioRenderSystem.h"
#include "../io3DEngine/ioCamera.h"
#include "../io3DEngine/ioCameraController.h"
#include "../io3DEngine/ioLookAtCameraController.h"
#include "../io3DEngine/ioResourceManager.h"
#include "../io3DEngine/ioTexture.h"
#include "../io3DEngine/ioTextureManager.h"
#include "../io3DEngine/ioMeshManager.h"
#include "../io3DEngine/ioMaterialManager.h"
#include "../io3DEngine/ioSkeletonManager.h"
#include "../io3DEngine/ioAnimationManager.h"
#include "../io3DEngine/ioAnimationEventManager.h"
#include "../io3DEngine/ioShaderManager.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/ioTextureEffectManager.h"
#include "../io3DEngine/ioEffectFactory.h"
#include "../io3DEngine/ioVertexBufferManager.h"
#include "../io3DEngine/ioIndexBufferManager.h"
#include "../io3DEngine/ioOpcodeManager.h"
#include "../io3DEngine/ioAnimateFXManager.h"
#include "../io3DEngine/ioMeshTrailDataManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioSceneManager.h"
#include "../io3DEngine/ioEntityGroupBuilder.h"
#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/ioMeshTrailer.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioStream.h"
#include "../io3DEngine/ioResourceLoader.h"
#include "../io3DEngine/ErrorReport.h"
#include "../io3DEngine/ioThreadTaskManager.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioRenderQueue.h"
#include "../io3DEngine/ioOrientBox.h"
#include "../io3DEngine/ioSegMent.h"
#include "../io3DEngine/ioNode.h"
#include "../io3DEngine/ioEntity.h"
#include "../io3DEngine/ioSubEntity.h"
#include "../io3DEngine/ioTechnique.h"
#include "../io3DEngine/ioMaterial.h"
#include "../io3DEngine/ioPass.h"
#include "../io3DEngine/ioTextureUnitState.h"
#include "../io3DEngine/ioStringInterface.h"
#include "../io3DEngine/ioSceneNode.h"
#include "../io3DEngine/ioSkyDome.h"
#include "../io3DEngine/ioOpcodeShape.h"
#include "../io3DEngine/ioParticleSystem.h"
#include "../io3DEngine/ioUI3DEffectRender.h"
#include "../io3DEngine/ioRay.h"
#include "../io3DEngine/ioRenderable.h"
#include "../io3DEngine/ioMovableObject.h"
#include "../io3DEngine/ioMesh.h"
#include "../io3DEngine/ioAnimation.h"
#include "../io3DEngine/ioBiped.h"
#include "../io3DEngine/ioSkeleton.h"
#include "../io3DEngine/ioCylinder.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioINIParser.h"
#include "../io3DEngine/ioMovingWnd.h"
#include "../io3DEngine/ioUITitle.h"

#include "Local/ioLocalManager.h"
#include "Local/ioLocalParent.h"
#include "IoString/IoString.h"

//코스튬
#include "CostumeEnumType.h"
#include "ioCostumeInfoManager.h"
#include "ioCostumeShopInfoManager.h"

//칭호
#include "NamedTitleEnumType.h"
#include "ioNamedTitleInfoManager.h"

//액세서리
#include "AccessoryEnumType.h"

#include "NetworkWrappingFunc.h"
#include "TextColorTable.h"

#include "IORandom.h"
#include "ioPlayEntity.h"
#include "ioKeyManager.h"
#include "ioUserKeyInput.h"
#include "ScreenBlindSetting.h"
#include "WndID.h"
#include "ioEnhanceRate.h"
#include "ioFormalAttackGrp.h"
#include "ioStatePriority.h"
#include "ioItemFeature.h"
#include "ioTargetMarker.h"
#include "EncryptValue.h"
#include "CharacterInfo.h"
#include "ioWorldEventReceiver.h"
#include "ioTPtrGrowArray.h"
#include "ioMovieMaker.h"
#include "ioGameAnnMgr.h"
#include "Setting.h"
#include "ioCharChatBubble.h"
#include "ioWorldDecoration.h"
#include "ioMapEffect.h"
#include "ioGameSceneManager.h"
#include "ioP2PChecker.h"
#include "ioSP2TCPSocket.h"
#include "ioMyLevelMgr.h"
#include "ioModeSelectInfo.h"
#include "TradeInfoManager.h"
#include "ioFieldRewardItem.h"
#include "ioFieldRewardItemMgr.h"

//npc ai
#include "uniheap.h"
#include "priorityq.h"
#include "astar.h"
#include "boxgraph.h"
#include "boxgraphloader.h"

#include "ioClassPrice.h"
#include "ioEquipSlot.h"
#include "ioGlobalTimer.h"
#include "ioApplication.h"
#include "ioMyInfo.h"
#include "ioGuildInfo.h"
#include "ioBattleRoomMgr.h"
#include "ioRoomOptionManager.h"
#include "ioAbuseManager.h"
#include "DirectInput.h"
#include "ioPresentMgr.h"
#include "ioCampManager.h"
#include "ioQuestManager.h"
#include "ioQuestGuide.h"

#include "ioCameraEventManager.h"
#include "ioShakeCameraManager.h"

#include "ioPushStructListManager.h"
#include "ioBallManager.h"
#include "ioMachineStructMgr.h"
#include "EtcHelpFunc.h"
#include "ioPlayEntityHelp.h"

#include "ioSetItemInfoManager.h"
#include "ioSetItemInfo.h"

#include "ioChatMacro.h"
#include "ChatMacroMgr.h"

//더미
#include "ioDummyChar.h"
#include "ioDummyCharManager.h"

//무기
#include "ioWeapon.h"
#include "ioWeaponManager.h"
#include "ioWeaponFeature.h"

#include "ioAreaWeapon.h"
#include "ioAreaWeaponManager.h"

//아이템
#include "ioItem.h"
#include "ioItemMaker.h"
#include "ioFieldItem.h"

#include "ioAttackableItem.h"
#include "ioWeaponItem.h"
#include "ioArmorItem.h"
#include "ioHelmetItem.h"
#include "ioCloakItem.h"
#include "ioWearItem.h"
#include "ioObjectItem.h"
#include "ioRingItem.h"
#include "ioNecklaceItem.h"
#include "ioBraceletItem.h"

#include "ioExtendJump.h"

//스킬
#include "ioSkill.h"
#include "ioSkillMaker.h"
#include "ioAttackSkill.h"
#include "ioBuffSkill.h"
#include "ioPassiveSkill.h"
#include "ioAbsorbSkill.h"
#include "ioCountSkill.h"
#include "ioRangeSkill.h"

//버프
#include "ioBuff.h"
#include "ioBuffMaker.h"

//펫
#include "PetEnumType.h"
#include "ioPetInfoManager.h"
#include "ioPet.h"
#include "ioPetManager.h"
#include "ioPetAniEventHandler.h"

#include "ioAnimate2DImageRender.h"
#include "ioDummyCharAniEventHandler.h"

#include "ioRequestCharManager.h"
#include "ioRequestLagManager.h"

#include "ioFootballMode.h"
#include "ioSymbolMode.h"
#include "ioUnderwearMode.h"
#include "ioCBTMode.h"
#include "ioCatchRunningManMode.h"
#include "ioHeroMatchMode.h"
#include "ioHiddenkingMode.h"
#include "ioTrainingMode.h"
#include "ioHeadquartersMode.h"
#include "ioSurvivalMode.h"
#include "ioFightClubMode.h"
#include "ioTeamSurvivalMode.h"
#include "ioDungeonAMode.h"
#include "ioGangsiMode.h"
#include "ioBossMode.h"
#include "ioMyRoomMode.h"

#include "ioTCPChecker.h"

#include "ioGlobalSoldierInfo.h"

#include "ioWallZone.h"
#include "ioSymbolStruct.h"
#include "ioWorldEntityFactory.h"
#include "ioBall.h"
#include "ioMachineStruct.h"
#include "ioPushStruct.h"

#include "SlangUtil.h"
#include "ChatMessageFunc.h"

#include "FindPredicate.h"

#include "Crc32Static.h"

#include "ioNpcMgr.h"
#include "ioAwardManager.h"
#include "ioVictoriesMgr.h"
#include "ioPlayBonusMgr.h"
#include "ioCustomSoundMgr.h"
#include "ioTalismanMaker.h"
#include "ioEtcItemManager.h"
#include "ioGrowthInfoManager.h"
#include "ioExtraItemInfoManager.h"
#include "ioShopItemExtraInfoManager.h"
#include "ioItemCompoundManager.h"
#include "ioItemCompoundMaterialManager.h"

#include "LuaState/MathManager.h"
#include "LuaState/DropManager.h"

#include "ioModeUIController.h"
#include "ioEventUserNode.h"
#include "ioMedalItemInfoManager.h"
#include "ioCustomTextureMgr.h"
#include "ioFlashDownLoadMgr.h"

#include "ioRoulette.h"
#include "ioRouletteManager.h"

#include "ioExpandMedalSlotManager.h"

#include "ioRecvPacketLogManager.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioUIImageRenderManager.h"
#include "GUI/ioSP2GUIManager.h"
#include "GUI/ioUI3DRender.h"
#include "GUI/ioAnimationState.h"
#include "GUI/GameChatInput.h"	
#include "GUI/ioModeHelpManager.h"
#include "GUI/ioUserInfoMgr.h"
#include "GUI/ioFriendManager.h"
#include "GUI/ioTutorialManager.h"
#include "GUI/ioObserverWndManager.h"
#include "GUI/ioBlackListManager.h"
#include "GUI/ioAlarmManager.h"
#include "GUI/ioWndEXEventHandlerEX.h"
#include "GUI/Tooltip/ToolTipManager.h"

#if defined ( USE_GA )
#include "ioHttpMng.h"
#endif