#pragma once


#include "IORandom.h"
#include "ioPlayEntity.h"
#include "ioUserKeyInput.h"
#include "ScreenBlindSetting.h"

#include "ioEnhanceRate.h"
#include "ioFormalAttackGrp.h"
#include "ioStatePriority.h"
#include "ioItemFeature.h"
#include "WeaponAttribute.h"
#include "ioTargetMarker.h"
#include "BuffTypeID.h"
#include "EncryptValue.h"
#include "CharacterInfo.h"
#include "ioWorldEventReceiver.h"

#include "ioTPtrGrowArray.h"

class ioINILoader;
class ioPlayStage;
class ioPlayMode;
class ioPushStruct;
class ioMachineStruct;

class ioEquipSlot;
class ioItem;
class ioAttackableItem;
class ioWeaponItem;
class ioArmorItem;
class ioHelmetItem;
class ioCloakItem;
class ioObjectItem;
class ioWearItem;
class ioFieldItem;
class ioFieldRewardItem;

class ioSkill;
class ioBuffSkill;
class ioBuff;
class ioEffect;
class ioMapEffect;
class SP2Packet;

class ioAniCollisionBoxGrp;
class ioWoundedCollisionBoxGrp;

class ioGaugeHelper;
class ioTeamCircle;
class ioMagicCircle;
class ioCharEmoticon;
class ioCharChatBubble;

class ioExtendDash;
class ioExtendJump;
class ioCounterAttack;
class ioUIRenderImage;
class ioCylinder;

class ioBall;
class ioEtcItemBlock;

class ioGrowthUpInfo;

class ioBaseCharParent : public ioPlayEntity, public ioWorldEventReceiver
{
protected: 
	//RoundEndMotion
	static ioHashString m_EndWinMotion;
	static ioHashString m_EndLoseMotion;
	static ioHashString m_EndDrawMotion;

	static ioHashString	m_AniHandDelay;
	static ioHashString m_AniHandJumpReady;
	static ioHashString m_AniHandJumpping;
	static ioHashString m_AniHandJumpEnd;
	static ioHashString m_AniHandDashRun;
	static ioHashString m_AniEmptyAttackRun;

	static ioHashString	m_AniPickItem;
	static ioHashString m_AniPickItemByRope;

	static ioHashString m_AniRopeDelay;
	static ioHashString m_AniRopeMove;

	static ioHashString m_AniRopeDamage;
	static ioHashString m_AniRopeGetUp;

	static ioHashString m_AniRopeJumpReady;
	static ioHashString m_AniRopeJumpping;
	static ioHashString m_AniRopeJumpLanding;
	static ioHashString m_AniFalling;
	static ioHashString m_AniRopeDown;
	static ioHashString m_AniRopeBlow;
	static ioHashString m_AniRopeBlowDown;
	
	static ioHashString m_AniDropAction;
	static ioHashString m_AniFallDownAction;
	static ioHashString m_AniFallBackDownAction;

	static ioHashString	m_AniKoAction;
	static ioHashString	m_AniBackKoAction;
	static ioHashString	m_AniDownKoAction;
	static ioHashString	m_AniBackdownKoAction;
	static ioHashString	m_AniJumpKoAction;
	static ioHashString m_AniBackJumpKoAction;
	static ioHashString m_AniStandKoAction;
	static ioHashString m_AniBackStandKoAction;
	static ioHashString m_AniStandKoDown;
	static ioHashString m_AniBackStandKoDown;
	static ioHashString m_AniStandKo2Action;
	static ioHashString m_AniBackStandKo2Action;
	static ioHashString m_AniStandKo2Down;
	static ioHashString m_AniBackStandKo2Down;
	static ioHashString m_AniBlowPush;
	static ioHashString m_AniBlowGetUp;
	static ioHashString m_AniBackBlowGetUp;
	static ioHashString m_AniCreateCharMotion;
	static ioHashString m_AniCreateCharEffect;
	static ioHashString m_AniGradeUpMotion;
	static ioHashString m_AniGradeUpEffect;
	static ioHashString m_AniStartMotion;
	static ioHashString m_AniDeveloperKMotion;

	static ioHashString m_AniDieAction;
	static ioHashString m_AniBackDieAction;

	static ioHashString m_AniBlowDieAction;
	static ioHashString m_AniBackBlowDieAction;
	static ioHashString m_AniDownBlowDieAction;
	static ioHashString m_AniBackdownBlowDieAction;

	static ioHashString m_AniNpcDieAction;
	static ioHashString m_AniNpcBackDieAction;

	static ioHashString m_AniNpcBlowDieAction;
	static ioHashString m_AniNpcBackBlowDieAction;
	static ioHashString m_AniNpcDownBlowDieAction;
	static ioHashString m_AniNpcBackdownBlowDieAction;
	static ioHashString m_AniNpcFalling;
	static ioHashString m_AniNpcDropAction;
	static ioHashString m_AniNpcBlowGetUp;
	static ioHashString m_AniNpcBackBlowGetUp;

	static ioHashString m_AniSpecialMotion;

	// 임시 : 더미상태 모션
	static ioHashString m_AniDummyStateAction;
	static float m_fDummyStateAniRate;

	// 코인 부활 모션
	static ioHashString m_szAniCoinRebirthAction;
	static DWORD m_dwCoinRebirthCamera;

	// 강시 변환 모션
	static ioHashString m_szAniChangeGangsiAction;
	static DWORD m_dwChangeGangsiCamera;

	// 임시 : 처리 2009.03.24.
	static ioHashString	m_AniKoAction2;
	static ioHashString	m_AniBackKoAction2;
	static ioHashString	m_AniDownKoAction2;
	static ioHashString	m_AniBackdownKoAction2;

	// 임시 : 처리 2009.11.25.
	static ioHashString	m_AniKoAction3;
	static ioHashString	m_AniBackKoAction3;
	static ioHashString	m_AniDownKoAction3;
	static ioHashString	m_AniBackdownKoAction3;

	// BoundBlow
	static ioHashString m_AniBoundBlowAction;
	static ioHashString m_AniBoundBlowBackAction;
	static ioHashString m_AniBoundBlowAction2;
	static ioHashString m_AniBoundBlowBackAction2;
	static ioHashString m_AniBoundDownAction;
	static ioHashString m_AniBoundDownBackAction;

	static ioHashString m_AniBoundRopeBlow;
	static ioHashString m_AniBoundRopeBlow2;

	// BlowExtendWound
	static ioHashString m_AniBlowExtendWoundAction;
	static ioHashString m_AniBlowExtendWoundBackAction;


	static ioHashString m_CharChangeRedEffect;
	static ioHashString m_CharChangeBlueEffect;

	static ioHashString m_ProtectionEffect;

	// ExplosionExtraEffect
	static ioHashString m_CharWoundExtraEffect;

	static ioHashString m_DefaultDefenseEffect;
	static ioHashString m_AttackPushBuff;

	// 보스
	static ioHashString m_BossProtectEffect;
	static ioHashString m_BossProtectEndEffect;
	static float  m_fBossCurScaleRate;
	static ioHashString m_BossManSkeleton;
	static ioHashString m_BossWomanSkeleton;

	// Sound
	static ioHashString m_KillEffectSound;
	static ioHashString m_DeathEffectSound;
	static ioHashString m_CoinEventSound;

	// Take Machine
	static ioHashString m_szMachineTakeAni;
	static float m_fMachineTakeAniRate;

	// Shake Camera
public:
	static int m_iProtectShakeCamera;

	static int m_iDroppedShakeCamera;
	static int m_iOwnerDroppedShakeCamera;

	static DWORD m_dwOwnerSkillWoundedCameraEvent;
	static DWORD m_dwDefaultSpecialMotionCamera;

	static int m_iKillShakeCamera;
	static int m_iOwnerKilledShakeCamera;

	// For GrowthInfo
	static IntVec m_vGrowthInfoNum;

public:
	static int m_iOwnerIconOffsetX;
	static int m_iOwnerIconOffsetY;
	static int m_iOwnerIconOffsetExY;

protected:
	static float m_fDefaultMaxDamageRate;
	static float m_fDefaultMaxWoundedRate;

	static float m_fDefaultScaleMax;
	static float m_fDefaultMaxExtraSpeed;
	static float m_fDefaultMinExtraSpeed;

	static float m_fNPCDefaultMaxDamageRate;
	static float m_fNPCDefaultMaxWoundedRate;

	static float m_fNPCDefaultScaleMax;
	static float m_fNPCDefaultMaxExtraSpeed;
	static float m_fNPCDefaultMinExtraSpeed;

public:   // 이모티콘 표시 방식
	static bool m_bOnlyMyEmoticon;

public:
	ioBaseCharParent( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
	virtual ~ioBaseCharParent();
};
