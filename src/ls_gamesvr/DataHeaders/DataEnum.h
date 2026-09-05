#ifndef __DataEnum__H__
#define __DataEnum__H__


//--------------------------------------------------
// DATA_ITEM_TYPE
//--------------------------------------------------
enum DATA_ITEM_TYPE
{
    DATA_ITEM_TYPE_DEFAULT       = 0,
    DATA_ITEM_TYPE_SOLDIER       = 1,
    DATA_ITEM_TYPE_RANDOM_DECO   = 2,
    DATA_ITEM_TYPE_ETCITEM       = 3,
    DATA_ITEM_TYPE_PESO          = 4,
    DATA_ITEM_TYPE_EXTRAITEM     = 5,
    DATA_ITEM_TYPE_EXTRAITEMBOX  = 6,
    DATA_ITEM_TYPE_DECORATION    = 7,
    DATA_ITEM_TYPE_GRADE_EXP     = 8,
    DATA_ITEM_TYPE_MEDALITEM     = 9,
    DATA_ITEM_TYPE_ALCHEMIC_ITEM = 10,
    DATA_ITEM_TYPE_HERO_EXP      = 11,
    DATA_ITEM_TYPE_CASH          = 12,
    DATA_ITEM_TYPE_QQ_CASH       = 13,
    DATA_ITEM_TYPE_PIECE         = 14,
    DATA_ITEM_TYPE_MOTION        = 15,
    DATA_ITEM_TYPE_MAX           = 16,
    DATA_ITEM_TYPE_COSTUM        = 17
};


//--------------------------------------------------
// CN_TYPE
//--------------------------------------------------
enum CN_TYPE
{
    CN_TYPE_NONE                     = 0,
    CN_TYPE_PVE                      = 1,
    CN_TYPE_RENAME                   = 2,
    CN_TYPE_MOTION                   = 3,
    CN_TYPE_RANDOMBOX                = 4,
    CN_TYPE_RANDOMBOX_KEY            = 5,
    CN_TYPE_BONUS                    = 6,
    CN_TYPE_TIMEBOX                  = 7,
    CN_TYPE_CHANGESEX                = 8,
    CN_TYPE_FISHING_ROD              = 9,
    CN_TYPE_FISHING_BAIT             = 10,
    CN_TYPE_EVENTSHOP_TICKET         = 11,
    CN_TYPE_VIP                      = 12,
    CN_TYPE_GUILD_CREATE             = 13,
    CN_TYPE_SLOT_EXPAND              = 14,
    CN_TYPE_EXCAVATION_KIT           = 15,
    CN_TYPE_EXCAVATION_SHOVEL        = 16,
    CN_TYPE_RECORD_CLEAR             = 17,
    CN_TYPE_HERO_LEVEL_UP            = 18,
    CN_TYPE_BINGO_NUMBER             = 19,
    CN_TYPE_BINGO_SHUFFLE_NUMBER     = 20,
    CN_TYPE_BINGO_SHUFFLE_REWARD     = 21,
    CN_TYPE_BINGO_SPECIAL_NUMBER     = 22,
    CN_TYPE_DUMMY_EVENT              = 23,
    CN_TYPE_GUILD_MARK_CHANGE        = 24,
    CN_TYPE_DWARF_POTION             = 25,
    CN_TYPE_MYHOME_TICKET            = 26,
    CN_TYPE_MYHOME_BLOCK             = 27,
    CN_TYPE_MY_HOME_BLOCK_MAX_EXPAND = 28,
    CN_TYPE_MY_HOME_TILE_EXPAND      = 29,
    CN_TYPE_PLAZABEAR                = 30,
    CN_TYPE_PET_FOOD                 = 31,
    CN_TYPE_MAX                      = 32
};


//--------------------------------------------------
// USE_TYPE
//--------------------------------------------------
enum USE_TYPE
{
    USE_TYPE_NONE     = 0,
    USE_TYPE_COUNT    = 1,
    USE_TYPE_TIME     = 2,
    USE_TYPE_ONCE     = 3,
    USE_TYPE_ETERNITY = 4,
    USE_TYPE_DATE     = 5,
    USE_TYPE_RECHARGE = 6,
    USE_TYPE_MAX      = 7
};


//--------------------------------------------------
// BANNER_LINK_TYPE
//--------------------------------------------------
enum BANNER_LINK_TYPE
{
    BANNER_LINK_TYPE_EVENTPOPUP = 0,
    BANNER_LINK_TYPE_MAX        = 1
};


//--------------------------------------------------
// ETCITEM_DROP_TYPE
//--------------------------------------------------
enum ETCITEM_DROP_TYPE
{
    ETCITEM_DROP_TYPE_CANDROP = 0,
    ETCITEM_DROP_TYPE_NOTDROP = 1,
    ETCITEM_DROP_TYPE_MAX     = 2
};


//--------------------------------------------------
// EXCAVATION_REWARD_TYPE
//--------------------------------------------------
enum EXCAVATION_REWARD_TYPE
{
    EXCAVATION_REWARD_NONE    = 0,
    EXCAVATION_REWARD_GOLD    = 1,
    EXCAVATION_REWARD_PRESENT = 2,
    EXCAVATION_REWARD_MAX     = 3
};


//--------------------------------------------------
// PIECECHANGETYPE
//--------------------------------------------------
enum PIECECHANGETYPE
{
    PIECECHANGE_ENABLE           = 0,
    PIECECHANGE_DISABLE_TARGET   = 1,
    PIECECHANGE_DISABLE_RESOURCE = 2,
    PIECECHANGE_DISABLE          = 3,
    PIECECHANGETYPE_MAX          = 4
};


//--------------------------------------------------
// ETCITEM_RECORD_CLEAR_TYPE
//--------------------------------------------------
enum ETCITEM_RECORD_CLEAR_TYPE
{
    ETCITEM_RECORD_CLEAR_TYPE_NONE    = 0,
    ETCITEM_RECORD_CLEAR_TYPE_BATTLE  = 1,
    ETCITEM_RECORD_CLEAR_TYPE_RANKING = 2,
    ETCITEM_RECORD_CLEAR_TYPE_MAX     = 3
};


//--------------------------------------------------
// GUILD_MARK_TYPE
//--------------------------------------------------
enum GUILD_MARK_TYPE
{
    GUILD_MARK_TYPE_ICON        = 0,
    GUILD_MARK_TYPE_FRAME       = 1,
    GUILD_MARK_TYPE_ICON_COLOR  = 2,
    GUILD_MARK_TYPE_FRAME_COLOR = 3,
    GUILD_MARK_TYPE_BACK_COLOR  = 4,
    GUILD_MARK_TYPE_MAX         = 5
};


//--------------------------------------------------
// QUEST_QUICK_START_TYPE
//--------------------------------------------------
enum QUEST_QUICK_START_TYPE
{
    QUEST_QUICK_START_TYPE_NONE          = 0,
    QUEST_QUICK_START_TYPE_BATTLE        = 1,
    QUEST_QUICK_START_TYPE_RANKING       = 2,
    QUEST_QUICK_START_TYPE_PLAZA         = 3,
    QUEST_QUICK_START_TYPE_PRACTICE      = 4,
    QUEST_QUICK_START_TYPE_HEADQUARTERS  = 5,
    QUEST_QUICK_START_TYPE_TUTORIAL      = 6,
    QUEST_QUICK_START_TYPE_INVEN_SOLDIER = 7,
    QUEST_QUICK_START_TYPE_INVEN_EXTRA   = 8,
    QUEST_QUICK_START_TYPE_REINFORCE     = 9,
    QUEST_QUICK_START_TYPE_COMPOSE_EXTRA = 10,
    QUEST_QUICK_START_TYPE_INVEN_ETC     = 11,
    QUEST_QUICK_START_TYPE_MAX           = 12
};


//--------------------------------------------------
// CHECK_REWARD_TIME_TYPE
//--------------------------------------------------
enum CHECK_REWARD_TIME_TYPE
{
    CHECK_REWARD_TIME_TYPE_NONE      = 0,
    CHECK_REWARD_TIME_TYPE_ONLINE    = 1,
    CHECK_REWARD_TIME_TYPE_ONOFFLINE = 2
};


#endif //__DataEnum__H__
