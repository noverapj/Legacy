#ifndef ___PROTOCOL_HELPER_H__
#define ___PROTOCOL_HELPER_H__


#include "Protocol.h"


#define GetStringOfCTPCKPacket(packetID, p) \
     switch( packetID ) \
     { \
     case CTPK_CONNECT: \
         (p) = "CTPK_CONNECT"; \
         break; \
     case CTPK_CHAR_CREATE: \
         (p) = "CTPK_CHAR_CREATE"; \
         break; \
     case CTPK_LOGOUT: \
         (p) = "CTPK_LOGOUT"; \
         break; \
     case CTPK_CHAR_DELETE: \
         (p) = "CTPK_CHAR_DELETE"; \
         break; \
     case CTPK_PLAZA_ROOM_LIST: \
         (p) = "CTPK_PLAZA_ROOM_LIST"; \
         break; \
     case CTPK_EXIT_ROOM: \
         (p) = "CTPK_EXIT_ROOM"; \
         break; \
     case CTPK_DROP_ITEM: \
         (p) = "CTPK_DROP_ITEM"; \
         break; \
     case CTPK_PICK_ITEM: \
         (p) = "CTPK_PICK_ITEM"; \
         break; \
     case CTPK_DROP_DIE: \
         (p) = "CTPK_DROP_DIE"; \
         break; \
     case CTPK_WEAPON_DIE: \
         (p) = "CTPK_WEAPON_DIE"; \
         break; \
     case CTPK_SYMBOL_DIE: \
         (p) = "CTPK_SYMBOL_DIE"; \
         break; \
     case CTPK_CREATE_PLAZA: \
         (p) = "CTPK_CREATE_PLAZA"; \
         break; \
     case CTPK_PRISONER_ESCAPE: \
         (p) = "CTPK_PRISONER_ESCAPE"; \
         break; \
     case CTPK_CHANGE_CHAR: \
         (p) = "CTPK_CHANGE_CHAR"; \
         break; \
     case CTPK_PRISONER_DROP: \
         (p) = "CTPK_PRISONER_DROP"; \
         break; \
     case CTPK_PUSHSTRUCT_DIE: \
         (p) = "CTPK_PUSHSTRUCT_DIE"; \
         break; \
     case CTPK_PUSHSTRUCT_CREATE: \
         (p) = "CTPK_PUSHSTRUCT_CREATE"; \
         break; \
     case CTPK_CURRENT_DAMAGELIST: \
         (p) = "CTPK_CURRENT_DAMAGELIST"; \
         break; \
     case CTPK_USE_ITEM: \
         (p) = "CTPK_USE_ITEM"; \
         break; \
     case CTPK_BUY_ITEM: \
         (p) = "CTPK_BUY_ITEM"; \
         break; \
     case CTPK_EQUIP_SLOT_ITEM: \
         (p) = "CTPK_EQUIP_SLOT_ITEM"; \
         break; \
     case CTPK_SELL_SLOT_ITEM: \
         (p) = "CTPK_SELL_SLOT_ITEM"; \
         break; \
     case CTPK_INC_STAT: \
         (p) = "CTPK_INC_STAT"; \
         break; \
     case CTPK_INIT_STAT: \
         (p) = "CTPK_INIT_STAT"; \
         break; \
     case CTPK_CREATE_OBJECTITEM: \
         (p) = "CTPK_CREATE_OBJECTITEM"; \
         break; \
     case CTPK_EVENT_SCENE_END: \
         (p) = "CTPK_EVENT_SCENE_END"; \
         break; \
     case CTPK_LEVELUP_ITEM: \
         (p) = "CTPK_LEVELUP_ITEM"; \
         break; \
     case CTPK_REPOSITION_FIELDITEM: \
         (p) = "CTPK_REPOSITION_FIELDITEM"; \
         break; \
     case CTPK_SELL_EQUIP_ITEM: \
         (p) = "CTPK_SELL_EQUIP_ITEM"; \
         break; \
     case CTPK_JOIN_BATTLEROOM_LIST: \
         (p) = "CTPK_JOIN_BATTLEROOM_LIST"; \
         break; \
     case CTPK_CREATE_BATTLEROOM: \
         (p) = "CTPK_CREATE_BATTLEROOM"; \
         break; \
     case CTPK_USER_BATTLEROOM_JOIN: \
         (p) = "CTPK_USER_BATTLEROOM_JOIN"; \
         break; \
     case CTPK_USER_BATTLEROOM_LEAVE: \
         (p) = "CTPK_USER_BATTLEROOM_LEAVE"; \
         break; \
     case CTPK_SYMBOL_DAMAMGED: \
         (p) = "CTPK_SYMBOL_DAMAMGED"; \
         break; \
     case CTPK_MACRO_COMMAND: \
         (p) = "CTPK_MACRO_COMMAND"; \
         break; \
     case CTPK_PRISONERMODE: \
         (p) = "CTPK_PRISONERMODE"; \
         break; \
     case CTPK_CHAR_SLOT_CHANGE: \
         (p) = "CTPK_CHAR_SLOT_CHANGE"; \
         break; \
     case CTPK_CREATE_VIRTUAL_CHAR: \
         (p) = "CTPK_CREATE_VIRTUAL_CHAR"; \
         break; \
     case CTPK_PASSAGE: \
         (p) = "CTPK_PASSAGE"; \
         break; \
     case CTPK_PLAZA_COMMAND: \
         (p) = "CTPK_PLAZA_COMMAND"; \
         break; \
     case CTPK_PLAZA_USER_INVITE: \
         (p) = "CTPK_PLAZA_USER_INVITE"; \
         break; \
     case CTPK_CATCH_CHAR: \
         (p) = "CTPK_CATCH_CHAR"; \
         break; \
     case CTPK_ESCAPE_CATCH_CHAR: \
         (p) = "CTPK_ESCAPE_CATCH_CHAR"; \
         break; \
     case CTPK_MOVE_SLOT_ITEM: \
         (p) = "CTPK_MOVE_SLOT_ITEM"; \
         break; \
     case CTPK_ABSORB_REQUEST: \
         (p) = "CTPK_ABSORB_REQUEST"; \
         break; \
     case CTPK_CHAT_MODE: \
         (p) = "CTPK_CHAT_MODE"; \
         break; \
     case CTPK_TUTORIAL_STEP: \
         (p) = "CTPK_TUTORIAL_STEP"; \
         break; \
     case CTPK_CHAR_LIMIT_CHECK: \
         (p) = "CTPK_CHAR_LIMIT_CHECK"; \
         break; \
     case CTPK_CHAR_DECORATION_BUY: \
         (p) = "CTPK_CHAR_DECORATION_BUY"; \
         break; \
     case CTPK_PLAYRECORD_INFO: \
         (p) = "CTPK_PLAYRECORD_INFO"; \
         break; \
     case CTPK_LAST_PLAYRECORD_INFO: \
         (p) = "CTPK_LAST_PLAYRECORD_INFO"; \
         break; \
     case CTPK_CHAR_EXTEND: \
         (p) = "CTPK_CHAR_EXTEND"; \
         break; \
     case CTPK_CHANGE_SINGLE_CHAR: \
         (p) = "CTPK_CHANGE_SINGLE_CHAR"; \
         break; \
     case CTPK_VOICE_INFO: \
         (p) = "CTPK_VOICE_INFO"; \
         break; \
     case CTPK_SEARCH_PLAZA_ROOM: \
         (p) = "CTPK_SEARCH_PLAZA_ROOM"; \
         break; \
     case CTPK_BATTLEROOM_INVITE: \
         (p) = "CTPK_BATTLEROOM_INVITE"; \
         break; \
     case CTPK_RESERVE_BATTLEROOM_DELETE: \
         (p) = "CTPK_RESERVE_BATTLEROOM_DELETE"; \
         break; \
     case CTPK_LADDER_TEAM_RANK_LIST: \
         (p) = "CTPK_LADDER_TEAM_RANK_LIST"; \
         break; \
     case CTPK_HACK_QUIZ: \
         (p) = "CTPK_HACK_QUIZ"; \
         break; \
     case CTPK_RELAY_CHAT: \
         (p) = "CTPK_RELAY_CHAT"; \
         break; \
     case CTPK_ABUSE_QUIZ_START: \
         (p) = "CTPK_ABUSE_QUIZ_START"; \
         break; \
     case CTPK_ABUSE_QUIZ: \
         (p) = "CTPK_ABUSE_QUIZ"; \
         break; \
     case CTPK_BATTLEROOM_JOIN_INFO: \
         (p) = "CTPK_BATTLEROOM_JOIN_INFO"; \
         break; \
     case CTPK_TRIAL: \
         (p) = "CTPK_TRIAL"; \
         break; \
     case CTPK_AWARDING_INFO: \
         (p) = "CTPK_AWARDING_INFO"; \
         break; \
     case CTPK_AWARDING_RESULT: \
         (p) = "CTPK_AWARDING_RESULT"; \
         break; \
     case CTPK_FOLLOW_USER: \
         (p) = "CTPK_FOLLOW_USER"; \
         break; \
     case CTPK_DEVELOPER_MACRO: \
         (p) = "CTPK_DEVELOPER_MACRO"; \
         break; \
     case CTPK_REQUEST_REVIVAL_TIME: \
         (p) = "CTPK_REQUEST_REVIVAL_TIME"; \
         break; \
     case CTPK_ETCITEM_BUY: \
         (p) = "CTPK_ETCITEM_BUY"; \
         break; \
     case CTPK_BATTLEROOM_COMMAND: \
         (p) = "CTPK_BATTLEROOM_COMMAND"; \
         break; \
     case CTPK_MOVING_SERVER: \
         (p) = "CTPK_MOVING_SERVER"; \
         break; \
     case CTPK_FRIEND_LIST_MSG: \
         (p) = "CTPK_FRIEND_LIST_MSG"; \
         break; \
     case CTPK_FRIEND_COMMAND: \
         (p) = "CTPK_FRIEND_COMMAND"; \
         break; \
     case CTPK_FRIEND_DELETE: \
         (p) = "CTPK_FRIEND_DELETE"; \
         break; \
     case CTPK_DELETE_FRIEND_BY_WEB: \
         (p) = "CTPK_DELETE_FRIEND_BY_WEB"; \
         break; \
     case CTPK_USER_LOGIN: \
         (p) = "CTPK_USER_LOGIN"; \
         break; \
     case CTPK_REGISTERED_USER: \
         (p) = "CTPK_REGISTERED_USER"; \
         break; \
     case CTPK_USER_POS_REFRESH: \
         (p) = "CTPK_USER_POS_REFRESH"; \
         break; \
     case CTPK_PLAZA_INVITE_LIST: \
         (p) = "CTPK_PLAZA_INVITE_LIST"; \
         break; \
     case CTPK_CHANNEL_INVITE: \
         (p) = "CTPK_CHANNEL_INVITE"; \
         break; \
     case CTPK_CHANNEL_LEAVE: \
         (p) = "CTPK_CHANNEL_LEAVE"; \
         break; \
     case CTPK_CHANNEL_CHAT: \
         (p) = "CTPK_CHANNEL_CHAT"; \
         break; \
     case CTPK_CHANNEL_CREATE: \
         (p) = "CTPK_CHANNEL_CREATE"; \
         break; \
     case CTPK_BATTLEROOM_INVITE_LIST: \
         (p) = "CTPK_BATTLEROOM_INVITE_LIST"; \
         break; \
     case CTPK_EXERCISE_CHAR_CREATE: \
         (p) = "CTPK_EXERCISE_CHAR_CREATE"; \
         break; \
     case CTPK_USER_INFO_REFRESH: \
         (p) = "CTPK_USER_INFO_REFRESH"; \
         break; \
     case CTPK_MEMO_SEND_MSG: \
         (p) = "CTPK_MEMO_SEND_MSG"; \
         break; \
     case CTPK_OFFLINE_MEMO_MSG: \
         (p) = "CTPK_OFFLINE_MEMO_MSG"; \
         break; \
     case CTPK_BANKRUPTCY_PESO: \
         (p) = "CTPK_BANKRUPTCY_PESO"; \
         break; \
     case CTPK_SERVICE_CHAR: \
         (p) = "CTPK_SERVICE_CHAR"; \
         break; \
     case CTPK_USER_POS_INDEX: \
         (p) = "CTPK_USER_POS_INDEX"; \
         break; \
     case CTPK_CHAR_CHARGE: \
         (p) = "CTPK_CHAR_CHARGE"; \
         break; \
     case CTPK_JOIN_HEADQUARTERS: \
         (p) = "CTPK_JOIN_HEADQUARTERS"; \
         break; \
     case CTPK_GUILD_RANK_LIST: \
         (p) = "CTPK_GUILD_RANK_LIST"; \
         break; \
     case CTPK_GUILD_INFO: \
         (p) = "CTPK_GUILD_INFO"; \
         break; \
     case CTPK_GUILD_JOINER_CHANGE: \
         (p) = "CTPK_GUILD_JOINER_CHANGE"; \
         break; \
     case CTPK_GUILD_ENTRY_APP: \
         (p) = "CTPK_GUILD_ENTRY_APP"; \
         break; \
     case CTPK_GUILD_ENTRY_CANCEL: \
         (p) = "CTPK_GUILD_ENTRY_CANCEL"; \
         break; \
     case CTPK_GUILD_ENTRY_DELAY_MEMBER: \
         (p) = "CTPK_GUILD_ENTRY_DELAY_MEMBER"; \
         break; \
     case CTPK_GUILD_ENTRY_AGREE: \
         (p) = "CTPK_GUILD_ENTRY_AGREE"; \
         break; \
     case CTPK_GUILD_ENTRY_REFUSE: \
         (p) = "CTPK_GUILD_ENTRY_REFUSE"; \
         break; \
     case CTPK_GUILD_INVITATION: \
         (p) = "CTPK_GUILD_INVITATION"; \
         break; \
     case CTPK_GUILD_LEAVE: \
         (p) = "CTPK_GUILD_LEAVE"; \
         break; \
     case CTPK_GUILD_TITLE_CHANGE: \
         (p) = "CTPK_GUILD_TITLE_CHANGE"; \
         break; \
     case CTPK_GUILD_MASTER_CHANGE: \
         (p) = "CTPK_GUILD_MASTER_CHANGE"; \
         break; \
     case CTPK_GUILD_POSITION_CHANGE: \
         (p) = "CTPK_GUILD_POSITION_CHANGE"; \
         break; \
     case CTPK_GUILD_KICK_OUT: \
         (p) = "CTPK_GUILD_KICK_OUT"; \
         break; \
     case CTPK_FRIEND_REQUEST_LIST: \
         (p) = "CTPK_FRIEND_REQUEST_LIST"; \
         break; \
     case CTPK_FRIEND_APPLICATION: \
         (p) = "CTPK_FRIEND_APPLICATION"; \
         break; \
     case CTPK_PLAZA_JOIN_INFO: \
         (p) = "CTPK_PLAZA_JOIN_INFO"; \
         break; \
     case CTPK_USER_ENTRY_REFRESH: \
         (p) = "CTPK_USER_ENTRY_REFRESH"; \
         break; \
     case CTPK_HEADQUARTERS_COMMAND: \
         (p) = "CTPK_HEADQUARTERS_COMMAND"; \
         break; \
     case CTPK_GUILD_CHAT: \
         (p) = "CTPK_GUILD_CHAT"; \
         break; \
     case CTPK_GUILD_EXIST: \
         (p) = "CTPK_GUILD_EXIST"; \
         break; \
     case CTPK_USER_INFO_EXIST: \
         (p) = "CTPK_USER_INFO_EXIST"; \
         break; \
     case CTPK_GUILD_MARK_KEY_VALUE: \
         (p) = "CTPK_GUILD_MARK_KEY_VALUE"; \
         break; \
     case CTPK_VOICE_INFO_USER: \
         (p) = "CTPK_VOICE_INFO_USER"; \
         break; \
     case CTPK_LADDER_TEAM_LIST: \
         (p) = "CTPK_LADDER_TEAM_LIST"; \
         break; \
     case CTPK_CREATE_LADDERTEAM: \
         (p) = "CTPK_CREATE_LADDERTEAM"; \
         break; \
     case CTPK_JOIN_LADDERTEAM: \
         (p) = "CTPK_JOIN_LADDERTEAM"; \
         break; \
     case CTPK_LADDERTEAM_MACRO: \
         (p) = "CTPK_LADDERTEAM_MACRO"; \
         break; \
     case CTPK_LADDERTEAM_JOIN_INFO: \
         (p) = "CTPK_LADDERTEAM_JOIN_INFO"; \
         break; \
     case CTPK_LADDERTEAM_LEAVE: \
         (p) = "CTPK_LADDERTEAM_LEAVE"; \
         break; \
     case CTPK_LADDERTEAM_INVITE_LIST: \
         (p) = "CTPK_LADDERTEAM_INVITE_LIST"; \
         break; \
     case CTPK_LADDERTEAM_INVITE: \
         (p) = "CTPK_LADDERTEAM_INVITE"; \
         break; \
     case CTPK_LADDER_TEAM_RANKING: \
         (p) = "CTPK_LADDER_TEAM_RANKING"; \
         break; \
     case CTPK_LADDER_BATTLE_RESTART: \
         (p) = "CTPK_LADDER_BATTLE_RESTART"; \
         break; \
     case CTPK_LADDER_OTHER_NAME_CHANGE: \
         (p) = "CTPK_LADDER_OTHER_NAME_CHANGE"; \
         break; \
     case CTPK_LADDER_BATTLE_REQUEST_AGREE: \
         (p) = "CTPK_LADDER_BATTLE_REQUEST_AGREE"; \
         break; \
     case CTPK_LADDER_USER_HQ_MOVE: \
         (p) = "CTPK_LADDER_USER_HQ_MOVE"; \
         break; \
     case CTPK_GUILD_MARK_KEY_VALUE_DELETE: \
         (p) = "CTPK_GUILD_MARK_KEY_VALUE_DELETE"; \
         break; \
     case CTPK_ETCITEM_USE: \
         (p) = "CTPK_ETCITEM_USE"; \
         break; \
     case CTPK_MOVIE_CONTROL: \
         (p) = "CTPK_MOVIE_CONTROL"; \
         break; \
     case CTPK_PRESENT_REQUEST: \
         (p) = "CTPK_PRESENT_REQUEST"; \
         break; \
     case CTPK_GUILD_TITLE_SYNC: \
         (p) = "CTPK_GUILD_TITLE_SYNC"; \
         break; \
     case CTPK_PROTECT_CHECK: \
         (p) = "CTPK_PROTECT_CHECK"; \
         break; \
     case CTPK_GET_CASH: \
         (p) = "CTPK_GET_CASH"; \
         break; \
     case CTPK_HOLE_SEND_COMPLETE: \
         (p) = "CTPK_HOLE_SEND_COMPLETE"; \
         break; \
     case CTPK_UDP_RECV_TIMEOUT: \
         (p) = "CTPK_UDP_RECV_TIMEOUT"; \
         break; \
     case CTPK_JOIN_SERVER_LOBBY_INFO: \
         (p) = "CTPK_JOIN_SERVER_LOBBY_INFO"; \
         break; \
     case CTPK_SERVER_LOBBY_CHAT: \
         (p) = "CTPK_SERVER_LOBBY_CHAT"; \
         break; \
     case CTPK_GROWTH_LEVEL_UP: \
         (p) = "CTPK_GROWTH_LEVEL_UP"; \
         break; \
     case CTPK_GROWTH_LEVEL_INIT: \
         (p) = "CTPK_GROWTH_LEVEL_INIT"; \
         break; \
     case CTPK_GUILD_USER_LIST: \
         (p) = "CTPK_GUILD_USER_LIST"; \
         break; \
     case CTPK_EVENT_DATA_UPDATE: \
         (p) = "CTPK_EVENT_DATA_UPDATE"; \
         break; \
     case CTPK_USER_KICK_VOTE: \
         (p) = "CTPK_USER_KICK_VOTE"; \
         break; \
     case CTPK_CHAR_CHANGE_PERIOD: \
         (p) = "CTPK_CHAR_CHANGE_PERIOD"; \
         break; \
     case CTPK_ETCITEM_ACTION: \
         (p) = "CTPK_ETCITEM_ACTION"; \
         break; \
     case CTPK_MYROOM_SERVER_CHANGE: \
         (p) = "CTPK_MYROOM_SERVER_CHANGE"; \
         break; \
     case CTPK_SERVER_LOBBY_INFO: \
         (p) = "CTPK_SERVER_LOBBY_INFO"; \
         break; \
     case CTPK_CAMP_DATA_SYNC: \
         (p) = "CTPK_CAMP_DATA_SYNC"; \
         break; \
     case CTPK_CAMP_CAHNGE_POS: \
         (p) = "CTPK_CAMP_CAHNGE_POS"; \
         break; \
     case CTPK_CAMP_BATTLE_END_LEAVE_TEAM: \
         (p) = "CTPK_CAMP_BATTLE_END_LEAVE_TEAM"; \
         break; \
     case CTPK_FISHING: \
         (p) = "CTPK_FISHING"; \
         break; \
     case CTPK_SERVER_ALARM_MSG: \
         (p) = "CTPK_SERVER_ALARM_MSG"; \
         break; \
     case CTPK_PRESENT_RECV: \
         (p) = "CTPK_PRESENT_RECV"; \
         break; \
     case CTPK_PRESENT_SELL: \
         (p) = "CTPK_PRESENT_SELL"; \
         break; \
     case CTPK_EXTRAITEM_BUY: \
         (p) = "CTPK_EXTRAITEM_BUY"; \
         break; \
     case CTPK_EXTRAITEM_SELL: \
         (p) = "CTPK_EXTRAITEM_SELL"; \
         break; \
     case CTPK_EXTRAITEM_CHANGE: \
         (p) = "CTPK_EXTRAITEM_CHANGE"; \
         break; \
     case CTPK_ETCITEM_SELL: \
         (p) = "CTPK_ETCITEM_SELL"; \
         break; \
     case CTPK_TIME_GROWTH_ADD: \
         (p) = "CTPK_TIME_GROWTH_ADD"; \
         break; \
     case CTPK_TIME_GROWTH_REMOVE: \
         (p) = "CTPK_TIME_GROWTH_REMOVE"; \
         break; \
     case CTPK_TIME_GROWTH_CHECK: \
         (p) = "CTPK_TIME_GROWTH_CHECK"; \
         break; \
     case CTPK_USE_MONSTER_COIN: \
         (p) = "CTPK_USE_MONSTER_COIN"; \
         break; \
     case CTPK_FIRST_CHANGE_ID: \
         (p) = "CTPK_FIRST_CHANGE_ID"; \
         break; \
     case CTPK_TURN_END_VIEW_STATE: \
         (p) = "CTPK_TURN_END_VIEW_STATE"; \
         break; \
     case CTPK_ITEM_MOVE_DROP: \
         (p) = "CTPK_ITEM_MOVE_DROP"; \
         break; \
     case CTPK_PRESENT_BUY: \
         (p) = "CTPK_PRESENT_BUY"; \
         break; \
     case CTPK_ALL_ITEM_DROP: \
         (p) = "CTPK_ALL_ITEM_DROP"; \
         break; \
     case CTPK_GASHAPON_LIST: \
         (p) = "CTPK_GASHAPON_LIST"; \
         break; \
     case CTPK_EXCAVATION_COMMAND: \
         (p) = "CTPK_EXCAVATION_COMMAND"; \
         break; \
     case CTPK_BALLSTRUCT_REPOSITION: \
         (p) = "CTPK_BALLSTRUCT_REPOSITION"; \
         break; \
     case CTPK_FOOTBALL_GOAL: \
         (p) = "CTPK_FOOTBALL_GOAL"; \
         break; \
     case CTPK_CONTROL_KEYS: \
         (p) = "CTPK_CONTROL_KEYS"; \
         break; \
     case CTPK_QUEST_OCCUR: \
         (p) = "CTPK_QUEST_OCCUR"; \
         break; \
     case CTPK_QUEST_ATTAIN: \
         (p) = "CTPK_QUEST_ATTAIN"; \
         break; \
     case CTPK_QUEST_ALARM: \
         (p) = "CTPK_QUEST_ALARM"; \
         break; \
     case CTPK_QUEST_REWARD: \
         (p) = "CTPK_QUEST_REWARD"; \
         break; \
     case CTPK_QUEST_ALL_DELETE: \
         (p) = "CTPK_QUEST_ALL_DELETE"; \
         break; \
     case CTPK_MEDALITEM_CHANGE: \
         (p) = "CTPK_MEDALITEM_CHANGE"; \
         break; \
     case CTPK_PRESENT_ALL_DELETE: \
         (p) = "CTPK_PRESENT_ALL_DELETE"; \
         break; \
     case CTPK_HERO_TOP100_DATA: \
         (p) = "CTPK_HERO_TOP100_DATA"; \
         break; \
     case CTPK_HERO_MATCH_OTHER_INFO: \
         (p) = "CTPK_HERO_MATCH_OTHER_INFO"; \
         break; \
     case CTPK_TRADE_CREATE: \
         (p) = "CTPK_TRADE_CREATE"; \
         break; \
     case CTPK_TRADE_LIST: \
         (p) = "CTPK_TRADE_LIST"; \
         break; \
     case CTPK_TRADE_ITEM: \
         (p) = "CTPK_TRADE_ITEM"; \
         break; \
     case CTPK_TRADE_CANCEL: \
         (p) = "CTPK_TRADE_CANCEL"; \
         break; \
     case CTPK_CHANGE_GANGSI: \
         (p) = "CTPK_CHANGE_GANGSI"; \
         break; \
     case CTPK_MACHINESTRUCT: \
         (p) = "CTPK_MACHINESTRUCT"; \
         break; \
     case CTPK_EVENT_SHOP_GOODS_LIST: \
         (p) = "CTPK_EVENT_SHOP_GOODS_LIST"; \
         break; \
     case CTPK_EVENT_SHOP_GOODS_BUY: \
         (p) = "CTPK_EVENT_SHOP_GOODS_BUY"; \
         break; \
     case CTPK_EVENT_SHOP_STATE: \
         (p) = "CTPK_EVENT_SHOP_STATE"; \
         break; \
     case CTPK_EVENT_SHOP_BUY_USER_CLEAR: \
         (p) = "CTPK_EVENT_SHOP_BUY_USER_CLEAR"; \
         break; \
     case CTPK_ETCITEM_SWITCH: \
         (p) = "CTPK_ETCITEM_SWITCH"; \
         break; \
     case CTPK_FIGHTCLUB_CHEER: \
         (p) = "CTPK_FIGHTCLUB_CHEER"; \
         break; \
     case CTPK_ROOM_STEALTH_ENTER: \
         (p) = "CTPK_ROOM_STEALTH_ENTER"; \
         break; \
     case CTPK_CAMP_SEASON_BONUS: \
         (p) = "CTPK_CAMP_SEASON_BONUS"; \
         break; \
     case CTPK_ETCITEM_MOTION_STATE: \
         (p) = "CTPK_ETCITEM_MOTION_STATE"; \
         break; \
     case CTPK_CUSTOM_ITEM_SKIN_UNIQUE_INDEX: \
         (p) = "CTPK_CUSTOM_ITEM_SKIN_UNIQUE_INDEX"; \
         break; \
     case CTPK_CUSTOM_ITEM_SKIN_DELETE: \
         (p) = "CTPK_CUSTOM_ITEM_SKIN_DELETE"; \
         break; \
     case CTPK_PUSHSTRUCT_OWNER_CLEAR: \
         (p) = "CTPK_PUSHSTRUCT_OWNER_CLEAR"; \
         break; \
     case CTPK_ETCITEM_MOTION_OPTION: \
         (p) = "CTPK_ETCITEM_MOTION_OPTION"; \
         break; \
     case CTPK_PRESENT_TEST_SEND: \
         (p) = "CTPK_PRESENT_TEST_SEND"; \
         break; \
     case CTPK_EXPERIENCE_MODE: \
         (p) = "CTPK_EXPERIENCE_MODE"; \
         break; \
     case CTPK_CHANGE_MY_LEADER: \
         (p) = "CTPK_CHANGE_MY_LEADER"; \
         break; \
     case CTPK_SET_MY_RENTAL: \
         (p) = "CTPK_SET_MY_RENTAL"; \
         break; \
     case CTPK_USER_CHAR_INFO_REFRESH: \
         (p) = "CTPK_USER_CHAR_INFO_REFRESH"; \
         break; \
     case CTPK_USER_CHAR_SUB_INFO_REFRESH: \
         (p) = "CTPK_USER_CHAR_SUB_INFO_REFRESH"; \
         break; \
     case CTPK_USER_CHAR_RENTAL_REQUEST: \
         (p) = "CTPK_USER_CHAR_RENTAL_REQUEST"; \
         break; \
     case CTPK_USER_CHAR_RENTAL_AGREE: \
         (p) = "CTPK_USER_CHAR_RENTAL_AGREE"; \
         break; \
     case CTPK_USER_CHAR_RENTAL_TIME_END: \
         (p) = "CTPK_USER_CHAR_RENTAL_TIME_END"; \
         break; \
     case CTPK_HEADQUARTERS_STATE_CHANGE: \
         (p) = "CTPK_HEADQUARTERS_STATE_CHANGE"; \
         break; \
     case CTPK_HEADQUARTERS_OPTION_CMD: \
         (p) = "CTPK_HEADQUARTERS_OPTION_CMD"; \
         break; \
     case CTPK_HEADQUARTERS_INFO: \
         (p) = "CTPK_HEADQUARTERS_INFO"; \
         break; \
     case CTPK_HEADQUARTERS_USER_INVITE: \
         (p) = "CTPK_HEADQUARTERS_USER_INVITE"; \
         break; \
     case CTPK_HEADQUARTERS_JOIN_AGREE: \
         (p) = "CTPK_HEADQUARTERS_JOIN_AGREE"; \
         break; \
     case CTPK_INSERT_BESTFRIEND: \
         (p) = "CTPK_INSERT_BESTFRIEND"; \
         break; \
     case CTPK_INSERT_BESTFRIEND_FAILED: \
         (p) = "CTPK_INSERT_BESTFRIEND_FAILED"; \
         break; \
     case CTPK_DISMISS_BESTFRIEND: \
         (p) = "CTPK_DISMISS_BESTFRIEND"; \
         break; \
     case CTPK_HEADQUARTERS_INVITE_LIST: \
         (p) = "CTPK_HEADQUARTERS_INVITE_LIST"; \
         break; \
     case CTPK_BESTFRIEND_EXCEPTION_LIST: \
         (p) = "CTPK_BESTFRIEND_EXCEPTION_LIST"; \
         break; \
     case CTPK_PICK_REWARD_ITEM: \
         (p) = "CTPK_PICK_REWARD_ITEM"; \
         break; \
     case CTPK_GET_MILEAGE: \
         (p) = "CTPK_GET_MILEAGE"; \
         break; \
     case CTPK_SIMPLE_USER_INFO_REFRESH: \
         (p) = "CTPK_SIMPLE_USER_INFO_REFRESH"; \
         break; \
     case CTPK_MACRO_NOTIFY: \
         (p) = "CTPK_MACRO_NOTIFY"; \
         break; \
     case CTPK_DISCONNECT_ALREADY_ID: \
         (p) = "CTPK_DISCONNECT_ALREADY_ID"; \
         break; \
     case CTPK_RUNNINGMAN_NAME_SYNC: \
         (p) = "CTPK_RUNNINGMAN_NAME_SYNC"; \
         break; \
     case CTPK_PCINFO: \
         (p) = "CTPK_PCINFO"; \
         break; \
     case CTPK_CLOSE_SESSION: \
         (p) = "CTPK_CLOSE_SESSION"; \
         break; \
     case CTPK_CHECK_BASE_VALUE: \
         (p) = "CTPK_CHECK_BASE_VALUE"; \
         break; \
     case CTPK_SHUTDOWN_DATE: \
         (p) = "CTPK_SHUTDOWN_DATE"; \
         break; \
     case CTPK_FIGHTCLUB_RESULT_INFO: \
         (p) = "CTPK_FIGHTCLUB_RESULT_INFO"; \
         break; \
     case CTPK_ALCHEMIC_FUNC: \
         (p) = "CTPK_ALCHEMIC_FUNC"; \
         break; \
     case CTPK_EXTRAITEM_DISASSEMBLE: \
         (p) = "CTPK_EXTRAITEM_DISASSEMBLE"; \
         break; \
     case CTPK_SOLDIER_DISASSEMBLE: \
         (p) = "CTPK_SOLDIER_DISASSEMBLE"; \
         break; \
     case CTPK_BUY_SELECT_EXTRA_GASHAPON: \
         (p) = "CTPK_BUY_SELECT_EXTRA_GASHAPON"; \
         break; \
     case CTPK_TOURNAMENT_MAIN_INFO: \
         (p) = "CTPK_TOURNAMENT_MAIN_INFO"; \
         break; \
     case CTPK_TOURNAMENT_LIST_REQUEST: \
         (p) = "CTPK_TOURNAMENT_LIST_REQUEST"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_CREATE: \
         (p) = "CTPK_TOURNAMENT_TEAM_CREATE"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_INFO: \
         (p) = "CTPK_TOURNAMENT_TEAM_INFO"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_USER_LIST: \
         (p) = "CTPK_TOURNAMENT_TEAM_USER_LIST"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_INVITATION: \
         (p) = "CTPK_TOURNAMENT_TEAM_INVITATION"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_ENTRY_MEMBER: \
         (p) = "CTPK_TOURNAMENT_TEAM_ENTRY_MEMBER"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_ENTRY_APP: \
         (p) = "CTPK_TOURNAMENT_TEAM_ENTRY_APP"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_ENTRY_REFUSE: \
         (p) = "CTPK_TOURNAMENT_TEAM_ENTRY_REFUSE"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_ENTRY_AGREE: \
         (p) = "CTPK_TOURNAMENT_TEAM_ENTRY_AGREE"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_LEAVE: \
         (p) = "CTPK_TOURNAMENT_TEAM_LEAVE"; \
         break; \
     case CTPK_TOURNAMENT_SCHEDULE_INFO: \
         (p) = "CTPK_TOURNAMENT_SCHEDULE_INFO"; \
         break; \
     case CTPK_TOURNAMENT_ROUND_TEAM_DATA: \
         (p) = "CTPK_TOURNAMENT_ROUND_TEAM_DATA"; \
         break; \
     case CTPK_TOURNAMENT_SUDDEN_DEATH: \
         (p) = "CTPK_TOURNAMENT_SUDDEN_DEATH"; \
         break; \
     case CTPK_TOURNAMENT_ROOM_LIST: \
         (p) = "CTPK_TOURNAMENT_ROOM_LIST"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_ALLOCATE_LIST: \
         (p) = "CTPK_TOURNAMENT_TEAM_ALLOCATE_LIST"; \
         break; \
     case CTPK_TOURNAMENT_TEAM_ALLOCATE_DATA: \
         (p) = "CTPK_TOURNAMENT_TEAM_ALLOCATE_DATA"; \
         break; \
     case CTPK_TOURNAMENT_JOIN_CONFIRM_CHECK: \
         (p) = "CTPK_TOURNAMENT_JOIN_CONFIRM_CHECK"; \
         break; \
     case CTPK_TOURNAMENT_JOIN_CONFIRM_REQUEST: \
         (p) = "CTPK_TOURNAMENT_JOIN_CONFIRM_REQUEST"; \
         break; \
     case CTPK_TOURNAMENT_JOIN_CONFIRM_COMMAND: \
         (p) = "CTPK_TOURNAMENT_JOIN_CONFIRM_COMMAND"; \
         break; \
     case CTPK_TOURNAMENT_ANNOUNCE_CHANGE: \
         (p) = "CTPK_TOURNAMENT_ANNOUNCE_CHANGE"; \
         break; \
     case CTPK_FIGHT_NPC: \
         (p) = "CTPK_FIGHT_NPC"; \
         break; \
     case CTPK_TOURNAMENT_TOTAL_TEAM_LIST: \
         (p) = "CTPK_TOURNAMENT_TOTAL_TEAM_LIST"; \
         break; \
     case CTPK_TOURNAMENT_CUSTOM_STATE_START: \
         (p) = "CTPK_TOURNAMENT_CUSTOM_STATE_START"; \
         break; \
     case CTPK_TOURNAMENT_CUSTOM_REWARD_LIST: \
         (p) = "CTPK_TOURNAMENT_CUSTOM_REWARD_LIST"; \
         break; \
     case CTPK_TOURNAMENT_CUSTOM_REWARD_BUY: \
         (p) = "CTPK_TOURNAMENT_CUSTOM_REWARD_BUY"; \
         break; \
     case CTPK_ROULETTE: \
         (p) = "CTPK_ROULETTE"; \
         break; \
     case CTPK_FILL_CASH_URL: \
         (p) = "CTPK_FILL_CASH_URL"; \
         break; \
     case CTPK_MEDALITEM_SELL: \
         (p) = "CTPK_MEDALITEM_SELL"; \
         break; \
     case CTPK_TOURNAMENT_CHEER_DECISION: \
         (p) = "CTPK_TOURNAMENT_CHEER_DECISION"; \
         break; \
     case CTPK_ROUND_END: \
         (p) = "CTPK_ROUND_END"; \
         break; \
     case CTPK_SUBSCRIPTION_BUY: \
         (p) = "CTPK_SUBSCRIPTION_BUY"; \
         break; \
     case CTPK_SUBSCRIPTION_RECV: \
         (p) = "CTPK_SUBSCRIPTION_RECV"; \
         break; \
     case CTPK_SUBSCRIPTION_RETR: \
         (p) = "CTPK_SUBSCRIPTION_RETR"; \
         break; \
     case CTPK_SUBSCRIPTION_RETR_CHECK: \
         (p) = "CTPK_SUBSCRIPTION_RETR_CHECK"; \
         break; \
     case CTPK_SUBSCRIPTION_REQUEST: \
         (p) = "CTPK_SUBSCRIPTION_REQUEST"; \
         break; \
     case CTPK_SHUFFLEROOM_JOIN: \
         (p) = "CTPK_SHUFFLEROOM_JOIN"; \
         break; \
     case CTPK_SHUFFLEROOM_JOIN_CANCEL: \
         (p) = "CTPK_SHUFFLEROOM_JOIN_CANCEL"; \
         break; \
     case CTPK_SHUFFLEROOM_USER_BLOW: \
         (p) = "CTPK_SHUFFLEROOM_USER_BLOW"; \
         break; \
     case CTPK_CREATE_MODE_ITEM: \
         (p) = "CTPK_CREATE_MODE_ITEM"; \
         break; \
     case CTPK_GET_MODE_ITEM: \
         (p) = "CTPK_GET_MODE_ITEM"; \
         break; \
     case CTPK_ROUND_END_CONTRIBUTE: \
         (p) = "CTPK_ROUND_END_CONTRIBUTE"; \
         break; \
     case CTPK_EXERCISE_PCROOM_CHAR_CREATE: \
         (p) = "CTPK_EXERCISE_PCROOM_CHAR_CREATE"; \
         break; \
     case CTPK_ATTENDANCE_CHECK: \
         (p) = "CTPK_ATTENDANCE_CHECK"; \
         break; \
     case CTPK_SHUFFLEROOM_DROPZONE: \
         (p) = "CTPK_SHUFFLEROOM_DROPZONE"; \
         break; \
     case CTPK_BUY_SELECT_GASHAPON: \
         (p) = "CTPK_BUY_SELECT_GASHAPON"; \
         break; \
     case CTPK_PET_CHANGE: \
         (p) = "CTPK_PET_CHANGE"; \
         break; \
     case CTPK_PET_SELL: \
         (p) = "CTPK_PET_SELL"; \
         break; \
     case CTPK_PET_NURTURE: \
         (p) = "CTPK_PET_NURTURE"; \
         break; \
     case CTPK_PET_COMPOUND: \
         (p) = "CTPK_PET_COMPOUND"; \
         break; \
     case CTPK_PET_EQUIP_INFO: \
         (p) = "CTPK_PET_EQUIP_INFO"; \
         break; \
     case CTPK_CHAR_AWAKE: \
         (p) = "CTPK_CHAR_AWAKE"; \
         break; \
     case CTPK_CHAR_AWAKE_EXTEND: \
         (p) = "CTPK_CHAR_AWAKE_EXTEND"; \
         break; \
     case CTPK_TOURNAMENT_REQUEST: \
         (p) = "CTPK_TOURNAMENT_REQUEST"; \
         break; \
     case CTPK_PLAYING_USERDATA_INFO: \
         (p) = "CTPK_PLAYING_USERDATA_INFO"; \
         break; \
     case CTPK_DOUBLE_CROWN_SYNC_REQUEST: \
         (p) = "CTPK_DOUBLE_CROWN_SYNC_REQUEST"; \
         break; \
     case CTPK_COSTUME_BUY: \
         (p) = "CTPK_COSTUME_BUY"; \
         break; \
     case CTPK_COSTUME_SELL: \
         (p) = "CTPK_COSTUME_SELL"; \
         break; \
     case CTPK_COSTUME_CHANGE: \
         (p) = "CTPK_COSTUME_CHANGE"; \
         break; \
     case CTPK_COSTUME_DISASSEMBLE: \
         (p) = "CTPK_COSTUME_DISASSEMBLE"; \
         break; \
     case CTPK_SPECIAL_SHOP_GOODS_LIST: \
         (p) = "CTPK_SPECIAL_SHOP_GOODS_LIST"; \
         break; \
     case CTPK_MISSION_INFO: \
         (p) = "CTPK_MISSION_INFO"; \
         break; \
     case CTPK_MISSION_COMPENSATION_RECV: \
         (p) = "CTPK_MISSION_COMPENSATION_RECV"; \
         break; \
     case CTPK_MISSION_TIME_CHECK: \
         (p) = "CTPK_MISSION_TIME_CHECK"; \
         break; \
     case CTPK_ROLLBOOK_RENEWAL: \
         (p) = "CTPK_ROLLBOOK_RENEWAL"; \
         break; \
     case CTPK_GUILD_ATTEND: \
         (p) = "CTPK_GUILD_ATTEND"; \
         break; \
     case CTPK_RECV_GUILDATTEND_REWARD: \
         (p) = "CTPK_RECV_GUILDATTEND_REWARD"; \
         break; \
     case CTPK_GUILD_MEMBER_ATTEND_RENEWAL: \
         (p) = "CTPK_GUILD_MEMBER_ATTEND_RENEWAL"; \
         break; \
     case CTPK_POPUP_ITEM_BUY: \
         (p) = "CTPK_POPUP_ITEM_BUY"; \
         break; \
     case CTPK_ENTER_GUILD_ROOM: \
         (p) = "CTPK_ENTER_GUILD_ROOM"; \
         break; \
     case CTPK_CONSTRUCT_MODE: \
         (p) = "CTPK_CONSTRUCT_MODE"; \
         break; \
     case CTPK_CONSTRUCT_BLOCK: \
         (p) = "CTPK_CONSTRUCT_BLOCK"; \
         break; \
     case CTPK_RETRIEVE_BLOCK: \
         (p) = "CTPK_RETRIEVE_BLOCK"; \
         break; \
     case CTPK_REQUEST_GUILD_INVEN: \
         (p) = "CTPK_REQUEST_GUILD_INVEN"; \
         break; \
     case CTPK_JOIN_PERSONAL_HQ: \
         (p) = "CTPK_JOIN_PERSONAL_HQ"; \
         break; \
     case CTPK_PERSONAL_HQ_INVITE_LIST: \
         (p) = "CTPK_PERSONAL_HQ_INVITE_LIST"; \
         break; \
     case CTPK_PERSONAL_HQ_JOIN_AGREE: \
         (p) = "CTPK_PERSONAL_HQ_JOIN_AGREE"; \
         break; \
     case CTPK_PERSONAL_HQ_COMMAND: \
         (p) = "CTPK_PERSONAL_HQ_COMMAND"; \
         break; \
     case CTPK_PERSONAL_HQ_INFO: \
         (p) = "CTPK_PERSONAL_HQ_INFO"; \
         break; \
     case CTPK_PERSONAL_HQ_INVITE: \
         (p) = "CTPK_PERSONAL_HQ_INVITE"; \
         break; \
     case CTPK_PERSONAL_HQ_INVEN_DATA: \
         (p) = "CTPK_PERSONAL_HQ_INVEN_DATA"; \
         break; \
     case CTPK_TITLE_CHANGE: \
         (p) = "CTPK_TITLE_CHANGE"; \
         break; \
     case CTPK_TITLE_SYNC_EQUIP_TIME: \
         (p) = "CTPK_TITLE_SYNC_EQUIP_TIME"; \
         break; \
     case CTPK_NEW_TITLE_CONFIRM: \
         (p) = "CTPK_NEW_TITLE_CONFIRM"; \
         break; \
     case CTPK_OAK_INFO_REQUEST: \
         (p) = "CTPK_OAK_INFO_REQUEST"; \
         break; \
     case CTPK_OAK_USE_SWORD: \
         (p) = "CTPK_OAK_USE_SWORD"; \
         break; \
     case CTPK_OAK_RESET: \
         (p) = "CTPK_OAK_RESET"; \
         break; \
     case CTPK_TRADE_LIST_REQ: \
         (p) = "CTPK_TRADE_LIST_REQ"; \
         break; \
     case CTPK_ACCESSORY_BUY: \
         (p) = "CTPK_ACCESSORY_BUY"; \
         break; \
     case CTPK_ACCESSORY_CHANGE: \
         (p) = "CTPK_ACCESSORY_CHANGE"; \
         break; \
     case CTPK_ACCESSORY_SELL: \
         (p) = "CTPK_ACCESSORY_SELL"; \
         break; \
     case CTPK_OAK_BARREL_GET_INFO: \
         (p) = "CTPK_OAK_BARREL_GET_INFO"; \
         break; \
     case CTPK_OAK_BARREL_USE_SWORD: \
         (p) = "CTPK_OAK_BARREL_USE_SWORD"; \
         break; \
     case CTPK_OAK_BARREL_GET_REWARD: \
         (p) = "CTPK_OAK_BARREL_GET_REWARD"; \
         break; \
     case CTPK_USER_MATCH_RANKING_DATA: \
         (p) = "CTPK_USER_MATCH_RANKING_DATA"; \
         break; \
     case CTPK_MATCH_TOP_RANKING_LIST: \
         (p) = "CTPK_MATCH_TOP_RANKING_LIST"; \
         break; \
     case CTPK_USER_MATCH_BATTLE_DATA: \
         (p) = "CTPK_USER_MATCH_BATTLE_DATA"; \
         break; \
     case CTPK_USER_MATCH_REQUEST: \
         (p) = "CTPK_USER_MATCH_REQUEST"; \
         break; \
     case CTPK_USER_MATCH_CANCEL: \
         (p) = "CTPK_USER_MATCH_CANCEL"; \
         break; \
     case CTPK_ACCESSORY_COMPOSE: \
         (p) = "CTPK_ACCESSORY_COMPOSE"; \
         break; \
     case CTPK_ACCESSORY_REINFORCE: \
         (p) = "CTPK_ACCESSORY_REINFORCE"; \
         break; \
	 case CTPK_EXTRAITEM_REINFORCE: \
         (p) = "CTPK_EXTRAITEM_REINFORCE"; \
         break; \
     case CTPK_SPIRIT_COMPOSE: \
         (p) = "CTPK_SPIRIT_COMPOSE"; \
         break; \
     case CTPK_SPIRIT_DECOMPOSE: \
         (p) = "CTPK_SPIRIT_DECOMPOSE"; \
         break; \
     case CTPK_SPIRIT_CONVERSION: \
         (p) = "CTPK_SPIRIT_CONVERSION"; \
         break; \
     case CTPK_SPIRIT_SELL: \
         (p) = "CTPK_SPIRIT_SELL"; \
         break; \
     case CTPK_USER_MEDALITEM_DATA: \
         (p) = "CTPK_USER_MEDALITEM_DATA"; \
         break; \
     case CTPK_CUSTOM_MEDALITEM_CHANGE: \
         (p) = "CTPK_CUSTOM_MEDALITEM_CHANGE"; \
         break; \
     case CTPK_CUSTOM_MEDALITEM_SELL: \
         (p) = "CTPK_CUSTOM_MEDALITEM_SELL"; \
         break; \
     case CTPK_CHANGE_CHAR_CHECK: \
         (p) = "CTPK_CHANGE_CHAR_CHECK"; \
         break; \
     case CTPK_ABSTRACT: \
         (p) = "CTPK_ABSTRACT"; \
         break; \
     case CUPK_CONNECT: \
         (p) = "CUPK_CONNECT"; \
         break; \
     case CUPK_SYNCTIME: \
         (p) = "CUPK_SYNCTIME"; \
         break; \
     case CUPK_CHAT: \
         (p) = "CUPK_CHAT"; \
         break; \
     case CUPK_RESERVE_ROOM_JOIN: \
         (p) = "CUPK_RESERVE_ROOM_JOIN"; \
         break; \
     case CUPK_CHECK_KING_PING: \
         (p) = "CUPK_CHECK_KING_PING"; \
         break; \
	 case CUPK_CHECK_FLAG_PING: \
	 (p) = "CUPK_CHECK_FLAG_PING";\
		break; \
     case CUPK_TEST: \
         (p) = "CUPK_TEST"; \
         break; \
	 case CTPK_CARD_MATCHING_GET_INFO: \
		 (p) = "CTPK_CARD_MATCHING_GET_INFO"; \
		 break; \
	 case CTPK_CARD_MATCHING_START_SET: \
		 (p) = "CTPK_CARD_MATCHING_START_SET"; \
	 	 break; \
	 case CTPK_CARD_MATCHING_CONFIRM_CARD: \
		 (p) = "CTPK_CARD_MATCHING_CONFIRM_CARD"; \
		 break; \
	 case CTPK_CARD_MATCHING_CONFIRM_SINGLE: \
		 (p) = "CTPK_CARD_MATCHING_CONFIRM_SINGLE"; \
		 break; \
	 case CTPK_CARD_MATCHING_END_REQUEST: \
		 (p) = "CTPK_CARD_MATCHING_END_REQUEST"; \
		 break; \
     default: \
         char buf[100]; \
         sprintf(buf, "0x%x", packetID); \
         (p) = buf; \
         break; \
     } \




#define GetStringOfSSTPKPacket(packetID, p) \
     switch( packetID ) \
     { \
     case SSTPK_CONNECT_INFO: \
         (p) = "SSTPK_CONNECT_INFO"; \
         break; \
     case SSTPK_PING: \
         (p) = "SSTPK_PING"; \
         break; \
     case SSTPK_ROOM_SYNC: \
         (p) = "SSTPK_ROOM_SYNC"; \
         break; \
     case SSTPK_MOVING_ROOM: \
         (p) = "SSTPK_MOVING_ROOM"; \
         break; \
     case SSTPK_MOVING_ROOM_RESULT: \
         (p) = "SSTPK_MOVING_ROOM_RESULT"; \
         break; \
     case SSTPK_USER_DATA_MOVE: \
         (p) = "SSTPK_USER_DATA_MOVE"; \
         break; \
     case SSTPK_USER_DATA_MOVE_RESULT: \
         (p) = "SSTPK_USER_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_USER_SYNC: \
         (p) = "SSTPK_USER_SYNC"; \
         break; \
     case SSTPK_USER_MOVE: \
         (p) = "SSTPK_USER_MOVE"; \
         break; \
     case SSTPK_USER_FRIEND_MOVE: \
         (p) = "SSTPK_USER_FRIEND_MOVE"; \
         break; \
     case SSTPK_USER_FRIEND_MOVE_RESULT: \
         (p) = "SSTPK_USER_FRIEND_MOVE_RESULT"; \
         break; \
     case SSTPK_FOLLOW_USER: \
         (p) = "SSTPK_FOLLOW_USER"; \
         break; \
     case SSTPK_CHANNEL_SYNC: \
         (p) = "SSTPK_CHANNEL_SYNC"; \
         break; \
     case SSTPK_CHANNEL_INVITE: \
         (p) = "SSTPK_CHANNEL_INVITE"; \
         break; \
     case SSTPK_CHANNEL_CHAT: \
         (p) = "SSTPK_CHANNEL_CHAT"; \
         break; \
     case SSTPK_BATTLEROOM_SYNC: \
         (p) = "SSTPK_BATTLEROOM_SYNC"; \
         break; \
     case SSTPK_BATTLEROOM_TRANSFER: \
         (p) = "SSTPK_BATTLEROOM_TRANSFER"; \
         break; \
     case SSTPK_BATTLEROOM_JOIN_RESULT: \
         (p) = "SSTPK_BATTLEROOM_JOIN_RESULT"; \
         break; \
     case SSTPK_BATTLEROOM_FOLLOW: \
         (p) = "SSTPK_BATTLEROOM_FOLLOW"; \
         break; \
     case SSTPK_USER_INFO_REFRESH: \
         (p) = "SSTPK_USER_INFO_REFRESH"; \
         break; \
     case SSTPK_BATTLEROOM_KICK_OUT: \
         (p) = "SSTPK_BATTLEROOM_KICK_OUT"; \
         break; \
     case SSTPK_OFFLINE_MEMO: \
         (p) = "SSTPK_OFFLINE_MEMO"; \
         break; \
     case SSTPK_CONNECT_SYNC: \
         (p) = "SSTPK_CONNECT_SYNC"; \
         break; \
     case SSTPK_USER_POS_INDEX: \
         (p) = "SSTPK_USER_POS_INDEX"; \
         break; \
     case SSTPK_RESERVE_CREATE_ROOM: \
         (p) = "SSTPK_RESERVE_CREATE_ROOM"; \
         break; \
     case SSTPK_RESERVE_CREATE_BATTLEROOM: \
         (p) = "SSTPK_RESERVE_CREATE_BATTLEROOM"; \
         break; \
     case SSTPK_RESERVE_CREATE_BATTLEROOM_RESULT: \
         (p) = "SSTPK_RESERVE_CREATE_BATTLEROOM_RESULT"; \
         break; \
     case SSTPK_EXCEPTION_BATTLEROOM_LEAVE: \
         (p) = "SSTPK_EXCEPTION_BATTLEROOM_LEAVE"; \
         break; \
     case SSTPK_CREATE_GUILD_RESULT: \
         (p) = "SSTPK_CREATE_GUILD_RESULT"; \
         break; \
     case SSTPK_CREATE_GUILD_COMPLETE: \
         (p) = "SSTPK_CREATE_GUILD_COMPLETE"; \
         break; \
     case SSTPK_GUILD_ENTRY_AGREE: \
         (p) = "SSTPK_GUILD_ENTRY_AGREE"; \
         break; \
     case SSTPK_GUILD_INVITATION: \
         (p) = "SSTPK_GUILD_INVITATION"; \
         break; \
     case SSTPK_GUILD_MASTER_CHANGE: \
         (p) = "SSTPK_GUILD_MASTER_CHANGE"; \
         break; \
     case SSTPK_GUILD_POSITION_CHANGE: \
         (p) = "SSTPK_GUILD_POSITION_CHANGE"; \
         break; \
     case SSTPK_GUILD_KICK_OUT: \
         (p) = "SSTPK_GUILD_KICK_OUT"; \
         break; \
     case SSTPK_FRIEND_DELETE: \
         (p) = "SSTPK_FRIEND_DELETE"; \
         break; \
     case SSTPK_PLAZAROOM_TRANSFER: \
         (p) = "SSTPK_PLAZAROOM_TRANSFER"; \
         break; \
     case SSTPK_GUILD_MARK_CHANGE: \
         (p) = "SSTPK_GUILD_MARK_CHANGE"; \
         break; \
     case SSTPK_GUILD_USER_DELETE: \
         (p) = "SSTPK_GUILD_USER_DELETE"; \
         break; \
     case SSTPK_LADDERTEAM_SYNC: \
         (p) = "SSTPK_LADDERTEAM_SYNC"; \
         break; \
     case SSTPK_LADDERTEAM_TRANSFER: \
         (p) = "SSTPK_LADDERTEAM_TRANSFER"; \
         break; \
     case SSTPK_EXCEPTION_LADDERTEAM_LEAVE: \
         (p) = "SSTPK_EXCEPTION_LADDERTEAM_LEAVE"; \
         break; \
     case SSTPK_RESERVE_CREATE_LADDERTEAM: \
         (p) = "SSTPK_RESERVE_CREATE_LADDERTEAM"; \
         break; \
     case SSTPK_RESERVE_CREATE_LADDERTEAM_RESULT: \
         (p) = "SSTPK_RESERVE_CREATE_LADDERTEAM_RESULT"; \
         break; \
     case SSTPK_LADDERTEAM_JOIN_RESULT: \
         (p) = "SSTPK_LADDERTEAM_JOIN_RESULT"; \
         break; \
     case SSTPK_LADDERTEAM_KICK_OUT: \
         (p) = "SSTPK_LADDERTEAM_KICK_OUT"; \
         break; \
     case SSTPK_LADDERTEAM_FOLLOW: \
         (p) = "SSTPK_LADDERTEAM_FOLLOW"; \
         break; \
     case SSTPK_LADDERTEAM_ENTER_ROOM_USER: \
         (p) = "SSTPK_LADDERTEAM_ENTER_ROOM_USER"; \
         break; \
     case SSTPK_GUILD_MEMBER_LIST_EX: \
         (p) = "SSTPK_GUILD_MEMBER_LIST_EX"; \
         break; \
     case SSTPK_WEB_EVENT: \
         (p) = "SSTPK_WEB_EVENT"; \
         break; \
     case SSTPK_GET_CASH_RESULT: \
         (p) = "SSTPK_GET_CASH_RESULT"; \
         break; \
     case SSTPK_OUTPUT_CASH_RESULT: \
         (p) = "SSTPK_OUTPUT_CASH_RESULT"; \
         break; \
     case SSTPK_WEB_REFRESH_BLOCK: \
         (p) = "SSTPK_WEB_REFRESH_BLOCK"; \
         break; \
     case SSTPK_WEB_GET_CASH: \
         (p) = "SSTPK_WEB_GET_CASH"; \
         break; \
     case SSTPK_WEB_REFRESH_USER_ENTRY: \
         (p) = "SSTPK_WEB_REFRESH_USER_ENTRY"; \
         break; \
     case SSTPK_WHOLE_CHAT: \
         (p) = "SSTPK_WHOLE_CHAT"; \
         break; \
     case SSTPK_UDP_RECV_TIMEOUT: \
         (p) = "SSTPK_UDP_RECV_TIMEOUT"; \
         break; \
     case SSTPK_CAMP_SEASON_BONUS: \
         (p) = "SSTPK_CAMP_SEASON_BONUS"; \
         break; \
     case SSTPK_SERVER_ALARM_MENT_UDP: \
         (p) = "SSTPK_SERVER_ALARM_MENT_UDP"; \
         break; \
     case SSTPK_GUILD_NAME_CHANGE: \
         (p) = "SSTPK_GUILD_NAME_CHANGE"; \
         break; \
     case SSTPK_GUILD_NAME_CHANGE_RESULT: \
         (p) = "SSTPK_GUILD_NAME_CHANGE_RESULT"; \
         break; \
     case SSTPK_BILLING_LOGIN_RESULT: \
         (p) = "SSTPK_BILLING_LOGIN_RESULT"; \
         break; \
     case SSTPK_BILLING_REFUND_CASH_RESULT: \
         (p) = "SSTPK_BILLING_REFUND_CASH_RESULT"; \
         break; \
     case SSTPK_BILLING_USER_INFO_RESULT: \
         (p) = "SSTPK_BILLING_USER_INFO_RESULT"; \
         break; \
     case SSTPK_PRESENT_SELECT: \
         (p) = "SSTPK_PRESENT_SELECT"; \
         break; \
     case SSTPK_USER_HERO_DATA: \
         (p) = "SSTPK_USER_HERO_DATA"; \
         break; \
     case SSTPK_HERO_MATCH_OTHER_INFO: \
         (p) = "SSTPK_HERO_MATCH_OTHER_INFO"; \
         break; \
     case SSTPK_TRADE_CREATE: \
         (p) = "SSTPK_TRADE_CREATE"; \
         break; \
     case SSTPK_TRADE_CREATE_COMPLETE: \
         (p) = "SSTPK_TRADE_CREATE_COMPLETE"; \
         break; \
     case SSTPK_TRADE_CREATE_FAIL: \
         (p) = "SSTPK_TRADE_CREATE_FAIL"; \
         break; \
     case SSTPK_TRADE_ITEM_COMPLETE: \
         (p) = "SSTPK_TRADE_ITEM_COMPLETE"; \
         break; \
     case SSTPK_TRADE_CANCEL: \
         (p) = "SSTPK_TRADE_CANCEL"; \
         break; \
     case SSTPK_TRADE_TIME_OUT: \
         (p) = "SSTPK_TRADE_TIME_OUT"; \
         break; \
     case SSTPK_BILLING_AUTOUPGRADE_LOGIN_RESULT: \
         (p) = "SSTPK_BILLING_AUTOUPGRADE_LOGIN_RESULT"; \
         break; \
     case SSTPK_INVENTORY_DATA_MOVE_RESULT: \
         (p) = "SSTPK_INVENTORY_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_EXTRAITEM_DATA_MOVE_RESULT: \
         (p) = "SSTPK_EXTRAITEM_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_QUEST_DATA_MOVE_RESULT: \
         (p) = "SSTPK_QUEST_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_EVENT_ITEM_INITIALIZE: \
         (p) = "SSTPK_EVENT_ITEM_INITIALIZE"; \
         break; \
     case SSTPK_USER_CHAR_INFO_REFRESH: \
         (p) = "SSTPK_USER_CHAR_INFO_REFRESH"; \
         break; \
     case SSTPK_USER_CHAR_SUB_INFO_REFRESH: \
         (p) = "SSTPK_USER_CHAR_SUB_INFO_REFRESH"; \
         break; \
     case SSTPK_USER_CHAR_RENTAL_AGREE: \
         (p) = "SSTPK_USER_CHAR_RENTAL_AGREE"; \
         break; \
     case SSTPK_JOIN_HEADQUARTERS_USER: \
         (p) = "SSTPK_JOIN_HEADQUARTERS_USER"; \
         break; \
     case SSTPK_HEADQUARTERS_INFO: \
         (p) = "SSTPK_HEADQUARTERS_INFO"; \
         break; \
     case SSTPK_HEADQUARTERS_ROOM_INFO: \
         (p) = "SSTPK_HEADQUARTERS_ROOM_INFO"; \
         break; \
     case SSTPK_HEADQUARTERS_JOIN_AGREE: \
         (p) = "SSTPK_HEADQUARTERS_JOIN_AGREE"; \
         break; \
     case SSTPK_BILLING_PCROOM_RESULT: \
         (p) = "SSTPK_BILLING_PCROOM_RESULT"; \
         break; \
     case SSTPK_LOGOUT_ROOM_ALARM: \
         (p) = "SSTPK_LOGOUT_ROOM_ALARM"; \
         break; \
     case SSTPK_BILLING_OTP_RESULT: \
         (p) = "SSTPK_BILLING_OTP_RESULT"; \
         break; \
     case SSTPK_BILLING_GET_MILEAGE_RESULT: \
         (p) = "SSTPK_BILLING_GET_MILEAGE_RESULT"; \
         break; \
     case SSTPK_BILLING_ADD_MILEAGE_RESULT: \
         (p) = "SSTPK_BILLING_ADD_MILEAGE_RESULT"; \
         break; \
     case SSTPK_SIMPLE_USER_INFO_REFRESH: \
         (p) = "SSTPK_SIMPLE_USER_INFO_REFRESH"; \
         break; \
     case SSTPK_DISCONNECT_ALREADY_ID: \
         (p) = "SSTPK_DISCONNECT_ALREADY_ID"; \
         break; \
     case SSTPK_BILLING_IPBONUS_RESULT: \
         (p) = "SSTPK_BILLING_IPBONUS_RESULT"; \
         break; \
     case SSTPK_CLOSE: \
         (p) = "SSTPK_CLOSE"; \
         break; \
     case SSTPK_ALCHEMIC_DATA_MOVE_RESULT: \
         (p) = "SSTPK_ALCHEMIC_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_TOURNAMENT_TEAM_CREATE: \
         (p) = "SSTPK_TOURNAMENT_TEAM_CREATE"; \
         break; \
     case SSTPK_TOURNAMENT_TEAM_ENTRY_AGREE_OK: \
         (p) = "SSTPK_TOURNAMENT_TEAM_ENTRY_AGREE_OK"; \
         break; \
     case SSTPK_TOURNAMENT_TEAM_JOIN: \
         (p) = "SSTPK_TOURNAMENT_TEAM_JOIN"; \
         break; \
     case SSTPK_TOURNAMENT_TEAM_LEAVE: \
         (p) = "SSTPK_TOURNAMENT_TEAM_LEAVE"; \
         break; \
     case SSTPK_PRESENT_INSERT: \
         (p) = "SSTPK_PRESENT_INSERT"; \
         break; \
     case SSTPK_BILLING_ADD_CASH_RESULT: \
         (p) = "SSTPK_BILLING_ADD_CASH_RESULT"; \
         break; \
     case SSTPK_BILLING_FILL_CASH_URL_RESULT: \
         (p) = "SSTPK_BILLING_FILL_CASH_URL_RESULT"; \
         break; \
     case SSTPK_ETC_ITEM_SEND_PRESENT: \
         (p) = "SSTPK_ETC_ITEM_SEND_PRESENT"; \
         break; \
     case SSTPK_SUBSCRIPTION_SELECT: \
         (p) = "SSTPK_SUBSCRIPTION_SELECT"; \
         break; \
     case SSTPK_BILLING_SUBSCRIPTION_RETRACT_CHECK_RESULT: \
         (p) = "SSTPK_BILLING_SUBSCRIPTION_RETRACT_CHECK_RESULT"; \
         break; \
     case SSTPK_BILLING_SUBSCRIPTION_RETRACT_RESULT: \
         (p) = "SSTPK_BILLING_SUBSCRIPTION_RETRACT_RESULT"; \
         break; \
     case SSTPK_SESSION_CONTROL_RESULT: \
         (p) = "SSTPK_SESSION_CONTROL_RESULT"; \
         break; \
     case SSTPK_SHUFFLEROOM_TRANSFER: \
         (p) = "SSTPK_SHUFFLEROOM_TRANSFER"; \
         break; \
     case SSTPK_SHUFFLEROOM_JOIN_RESULT: \
         (p) = "SSTPK_SHUFFLEROOM_JOIN_RESULT"; \
         break; \
     case SSTPK_SHUFFLEROOM_SYNC: \
         (p) = "SSTPK_SHUFFLEROOM_SYNC"; \
         break; \
     case SSTPK_SHUFFLEROOM_KICK_OUT: \
         (p) = "SSTPK_SHUFFLEROOM_KICK_OUT"; \
         break; \
     case SSTPK_EXCEPTION_SHUFFLEROOM_LEAVE: \
         (p) = "SSTPK_EXCEPTION_SHUFFLEROOM_LEAVE"; \
         break; \
     case SSTPK_SHUFFLEROOM_GLOBAL_CREATE: \
         (p) = "SSTPK_SHUFFLEROOM_GLOBAL_CREATE"; \
         break; \
     case SSTPK_PET_DATA_MOVE_RESULT: \
         (p) = "SSTPK_PET_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_CHAR_AWAKE_MOVE_RESULT: \
         (p) = "SSTPK_CHAR_AWAKE_MOVE_RESULT"; \
         break; \
     case SSTPK_TIMEOUT_BILLINGGUID: \
         (p) = "SSTPK_TIMEOUT_BILLINGGUID"; \
         break; \
     case SSTPK_NEW_PET_DATA_INFO: \
         (p) = "SSTPK_NEW_PET_DATA_INFO"; \
         break; \
     case SSTPK_SOLDIER_DATA_MOVE_RESULT: \
         (p) = "SSTPK_SOLDIER_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_ETCITEM_DATA_MOVE_RESULT: \
         (p) = "SSTPK_ETCITEM_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_MEDALITEM_DATA_MOVE_RESULT: \
         (p) = "SSTPK_MEDALITEM_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_COSTUME_DATA_MOVE_RESULT: \
         (p) = "SSTPK_COSTUME_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_COSTUME_ADD: \
         (p) = "SSTPK_COSTUME_ADD"; \
         break; \
     case SSTPK_MISSION_DATA_MOVE_RESULT: \
         (p) = "SSTPK_MISSION_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_ROOLBOOK_DATA_MOVE_RESULT: \
         (p) = "SSTPK_ROOLBOOK_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_RESERVE_CREATE_GUILD_ROOM: \
         (p) = "SSTPK_RESERVE_CREATE_GUILD_ROOM"; \
         break; \
     case SSTPK_ACTIVE_GUILD_ROOM: \
         (p) = "SSTPK_ACTIVE_GUILD_ROOM"; \
         break; \
     case SSTPK_JOIN_PERSONAL_HQ_USER: \
         (p) = "SSTPK_JOIN_PERSONAL_HQ_USER"; \
         break; \
     case SSTPK_PERSONAL_HQ_INFO: \
         (p) = "SSTPK_PERSONAL_HQ_INFO"; \
         break; \
     case SSTPK_PERSONAL_HQ_ROOM_INFO: \
         (p) = "SSTPK_PERSONAL_HQ_ROOM_INFO"; \
         break; \
     case SSTPK_PERSONAL_HQ_JOIN_AGREE: \
         (p) = "SSTPK_PERSONAL_HQ_JOIN_AGREE"; \
         break; \
     case SSTPK_PERSONAL_HQ_DATA_MOVE_RESULT: \
         (p) = "SSTPK_PERSONAL_HQ_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_PERSONAL_HQ_ADD_BLOCK: \
         (p) = "SSTPK_PERSONAL_HQ_ADD_BLOCK"; \
         break; \
     case SSTPK_TIMECASH_DATA_MOVE_RESULT: \
         (p) = "SSTPK_TIMECASH_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_REQUEST_TIME_CASH_RESULT: \
         (p) = "SSTPK_REQUEST_TIME_CASH_RESULT"; \
         break; \
     case SSTPK_UPDATE_TIME_CASH: \
         (p) = "SSTPK_UPDATE_TIME_CASH"; \
         break; \
     case SSTPK_TITLE_MOVE_RESULT: \
         (p) = "SSTPK_TITLE_MOVE_RESULT"; \
         break; \
     case SSTPK_TITLE_UPDATE: \
         (p) = "SSTPK_TITLE_UPDATE"; \
         break; \
     case SSTPK_BONUS_CASH_MOVE_RESULT: \
         (p) = "SSTPK_BONUS_CASH_MOVE_RESULT"; \
         break; \
     case SSTPK_BONUS_CAHSH_ADD: \
         (p) = "SSTPK_BONUS_CAHSH_ADD"; \
         break; \
     case SSTPK_BONUS_CASH_UPDATE: \
         (p) = "SSTPK_BONUS_CASH_UPDATE"; \
         break; \
     case SSTPK_ACCESSORY_DATA_MOVE_RESULT: \
         (p) = "SSTPK_ACCESSORY_DATA_MOVE_RESULT"; \
         break; \
     case SSTPK_ACCESSORY_ADD: \
         (p) = "SSTPK_ACCESSORY_ADD"; \
         break; \
     case SSTPK_OAK_BARREL_GET_INFO: \
         (p) = "SSTPK_OAK_BARREL_GET_INFO"; \
         break; \
     case SSTPK_OAK_BARREL_UPDATE_INFO: \
         (p) = "SSTPK_OAK_BARREL_UPDATE_INFO"; \
         break; \
     case SSTPK_OAK_BARREL_UPDATE_TIME: \
         (p) = "SSTPK_OAK_BARREL_UPDATE_TIME"; \
         break; \
     case SSTPK_OAK_BARREL_UPDATE_LIMIT_SWORD: \
         (p) = "SSTPK_OAK_BARREL_UPDATE_LIMIT_SWORD"; \
         break; \
     case SSTPK_USER_MATCH_UPDATE: \
         (p) = "SSTPK_USER_MATCH_UPDATE"; \
         break; \
     case SSTPK_MATCH_LOG: \
         (p) = "SSTPK_MATCH_LOG"; \
         break; \
     case SSTPK_LADDER_MATCH_TIME: \
         (p) = "SSTPK_LADDER_MATCH_TIME"; \
         break; \
	 case SSTPK_BATTLE_MODE_ORDER: \
		 (p) = "SSTPK_BATTLE_MODE_ORDER"; \
		 break; \
	 case SSTPK_DICE_GAME_GET_INFO: \
		 (p) = "SSTPK_DICE_GAME_GET_INFO"; \
	     break; \
     default: \
         char buf[100]; \
         sprintf(buf, "0x%x", packetID); \
         (p) = buf; \
         break; \
     } \






#endif
