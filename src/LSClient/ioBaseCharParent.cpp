
#include "stdafx.h"

#include "ioBaseCharParent.h"

ioHashString ioBaseCharParent::m_EndWinMotion;
ioHashString ioBaseCharParent::m_EndLoseMotion;
ioHashString ioBaseCharParent::m_EndDrawMotion;

ioHashString ioBaseCharParent::m_AniHandDelay;
ioHashString ioBaseCharParent::m_AniHandJumpReady;
ioHashString ioBaseCharParent::m_AniHandJumpping;
ioHashString ioBaseCharParent::m_AniHandJumpEnd;
ioHashString ioBaseCharParent::m_AniHandDashRun;
ioHashString ioBaseCharParent::m_AniEmptyAttackRun;

ioHashString ioBaseCharParent::m_AniPickItem;
ioHashString ioBaseCharParent::m_AniPickItemByRope;

ioHashString ioBaseCharParent::m_AniRopeDelay;
ioHashString ioBaseCharParent::m_AniRopeMove;

ioHashString ioBaseCharParent::m_AniRopeDamage;
ioHashString ioBaseCharParent::m_AniRopeGetUp;

ioHashString ioBaseCharParent::m_AniRopeJumpReady;
ioHashString ioBaseCharParent::m_AniRopeJumpping;
ioHashString ioBaseCharParent::m_AniRopeJumpLanding;
ioHashString ioBaseCharParent::m_AniFalling;

ioHashString ioBaseCharParent::m_AniRopeDown;
ioHashString ioBaseCharParent::m_AniRopeBlow;
ioHashString ioBaseCharParent::m_AniRopeBlowDown;

ioHashString ioBaseCharParent::m_AniDropAction;
ioHashString ioBaseCharParent::m_AniFallDownAction;
ioHashString ioBaseCharParent::m_AniFallBackDownAction;

ioHashString ioBaseCharParent::m_AniKoAction;
ioHashString ioBaseCharParent::m_AniBackKoAction;
ioHashString ioBaseCharParent::m_AniDownKoAction;
ioHashString ioBaseCharParent::m_AniBackdownKoAction;
ioHashString ioBaseCharParent::m_AniJumpKoAction;
ioHashString ioBaseCharParent::m_AniBackJumpKoAction;
ioHashString ioBaseCharParent::m_AniStandKoAction;
ioHashString ioBaseCharParent::m_AniBackStandKoAction;
ioHashString ioBaseCharParent::m_AniStandKoDown;
ioHashString ioBaseCharParent::m_AniBackStandKoDown;
ioHashString ioBaseCharParent::m_AniStandKo2Action;
ioHashString ioBaseCharParent::m_AniBackStandKo2Action;
ioHashString ioBaseCharParent::m_AniStandKo2Down;
ioHashString ioBaseCharParent::m_AniBackStandKo2Down;
ioHashString ioBaseCharParent::m_AniBlowPush;
ioHashString ioBaseCharParent::m_AniBlowGetUp;
ioHashString ioBaseCharParent::m_AniBackBlowGetUp;
ioHashString ioBaseCharParent::m_AniCreateCharMotion;
ioHashString ioBaseCharParent::m_AniCreateCharEffect;
ioHashString ioBaseCharParent::m_AniGradeUpMotion;
ioHashString ioBaseCharParent::m_AniGradeUpEffect;
ioHashString ioBaseCharParent::m_AniStartMotion;
ioHashString ioBaseCharParent::m_AniDeveloperKMotion;

ioHashString ioBaseCharParent::m_AniDieAction;
ioHashString ioBaseCharParent::m_AniBackDieAction;

ioHashString ioBaseCharParent::m_AniBlowDieAction;
ioHashString ioBaseCharParent::m_AniBackBlowDieAction;
ioHashString ioBaseCharParent::m_AniDownBlowDieAction;
ioHashString ioBaseCharParent::m_AniBackdownBlowDieAction;

ioHashString ioBaseCharParent::m_AniNpcDieAction;
ioHashString ioBaseCharParent::m_AniNpcBackDieAction;

ioHashString ioBaseCharParent::m_AniNpcBlowDieAction;
ioHashString ioBaseCharParent::m_AniNpcBackBlowDieAction;
ioHashString ioBaseCharParent::m_AniNpcDownBlowDieAction;
ioHashString ioBaseCharParent::m_AniNpcBackdownBlowDieAction;
ioHashString ioBaseCharParent::m_AniNpcFalling;
ioHashString ioBaseCharParent::m_AniNpcDropAction;
ioHashString ioBaseCharParent::m_AniNpcBlowGetUp;
ioHashString ioBaseCharParent::m_AniNpcBackBlowGetUp;

ioHashString ioBaseCharParent::m_AniSpecialMotion;

// 임시 : 더미상태 모션
ioHashString ioBaseCharParent::m_AniDummyStateAction;
float ioBaseCharParent::m_fDummyStateAniRate;

// 코인 부활 모션
ioHashString ioBaseCharParent::m_szAniCoinRebirthAction;
DWORD ioBaseCharParent::m_dwCoinRebirthCamera;

// 강시 변환 모션
ioHashString ioBaseCharParent::m_szAniChangeGangsiAction;
DWORD ioBaseCharParent::m_dwChangeGangsiCamera;

// 임시 : 처리 2009.03.24.
ioHashString ioBaseCharParent::m_AniKoAction2;
ioHashString ioBaseCharParent::m_AniBackKoAction2;
ioHashString ioBaseCharParent::m_AniDownKoAction2;
ioHashString ioBaseCharParent::m_AniBackdownKoAction2;

// 임시 : 처리 2009.11.25.
ioHashString ioBaseCharParent::m_AniKoAction3;
ioHashString ioBaseCharParent::m_AniBackKoAction3;
ioHashString ioBaseCharParent::m_AniDownKoAction3;
ioHashString ioBaseCharParent::m_AniBackdownKoAction3;

// BoundBlow
ioHashString ioBaseCharParent::m_AniBoundBlowAction;
ioHashString ioBaseCharParent::m_AniBoundBlowBackAction;
ioHashString ioBaseCharParent::m_AniBoundBlowAction2;
ioHashString ioBaseCharParent::m_AniBoundBlowBackAction2;
ioHashString ioBaseCharParent::m_AniBoundDownAction;
ioHashString ioBaseCharParent::m_AniBoundDownBackAction;

ioHashString ioBaseCharParent::m_AniBoundRopeBlow;
ioHashString ioBaseCharParent::m_AniBoundRopeBlow2;

// BlowExtendWound
ioHashString ioBaseCharParent::m_AniBlowExtendWoundAction;
ioHashString ioBaseCharParent::m_AniBlowExtendWoundBackAction;

ioHashString ioBaseCharParent::m_CharChangeRedEffect;
ioHashString ioBaseCharParent::m_CharChangeBlueEffect;

ioHashString ioBaseCharParent::m_ProtectionEffect;

// ExplosionExtraEffect
ioHashString ioBaseCharParent::m_CharWoundExtraEffect;

ioHashString ioBaseCharParent::m_DefaultDefenseEffect;
ioHashString ioBaseCharParent::m_AttackPushBuff;

// 보스
ioHashString ioBaseCharParent::m_BossProtectEffect;
ioHashString ioBaseCharParent::m_BossProtectEndEffect;
float  ioBaseCharParent::m_fBossCurScaleRate;
ioHashString ioBaseCharParent::m_BossManSkeleton;
ioHashString ioBaseCharParent::m_BossWomanSkeleton;	

// Sound
ioHashString ioBaseCharParent::m_KillEffectSound;
ioHashString ioBaseCharParent::m_DeathEffectSound;
ioHashString ioBaseCharParent::m_CoinEventSound;

// Take Machine
ioHashString ioBaseCharParent::m_szMachineTakeAni;
float ioBaseCharParent::m_fMachineTakeAniRate;

// Shake Camera
int ioBaseCharParent::m_iProtectShakeCamera;

int ioBaseCharParent::m_iDroppedShakeCamera;
int ioBaseCharParent::m_iOwnerDroppedShakeCamera;

DWORD ioBaseCharParent::m_dwOwnerSkillWoundedCameraEvent;
DWORD ioBaseCharParent::m_dwDefaultSpecialMotionCamera;

int ioBaseCharParent::m_iKillShakeCamera;
int ioBaseCharParent::m_iOwnerKilledShakeCamera;

// For GrowthInfo
IntVec ioBaseCharParent::m_vGrowthInfoNum;

int ioBaseCharParent::m_iOwnerIconOffsetX;
int ioBaseCharParent::m_iOwnerIconOffsetY;
int ioBaseCharParent::m_iOwnerIconOffsetExY;

// 이모티콘 표시 방식
bool ioBaseCharParent::m_bOnlyMyEmoticon = false;

//
float ioBaseCharParent::m_fDefaultMaxDamageRate;
float ioBaseCharParent::m_fDefaultMaxWoundedRate;

float ioBaseCharParent::m_fDefaultScaleMax;
float ioBaseCharParent::m_fDefaultMaxExtraSpeed;
float ioBaseCharParent::m_fDefaultMinExtraSpeed;

float ioBaseCharParent::m_fNPCDefaultMaxDamageRate;
float ioBaseCharParent::m_fNPCDefaultMaxWoundedRate;

float ioBaseCharParent::m_fNPCDefaultScaleMax;
float ioBaseCharParent::m_fNPCDefaultMaxExtraSpeed;
float ioBaseCharParent::m_fNPCDefaultMinExtraSpeed;

ioBaseCharParent::ioBaseCharParent( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) : ioPlayEntity( pGrp, pMode )
{	
}

ioBaseCharParent::~ioBaseCharParent()
{
}