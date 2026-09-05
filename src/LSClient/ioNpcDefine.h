#pragma once

#define NPC_INDEX     0xffffffff

enum NpcType
{
	NT_DEFAULT		= 0,
	NT_DEVELOPER_K  = 1,  // 아래부터는 움직일 수 없는 npc
	NT_BIGTOWER		= 2,
	NT_TOWERGUARD	= 3,
	NT_LAIR			= 4,
	NT_LAIRGUARD	= 5,
	NT_NEXUS		= 6,
	NT_NEXUSGUARD	= 7,
	END_FIXING_NPC,   // 요기까지 안 움직이는 npc 들  TODO:나중에 공격하는 structure 로 대체 필요
};

enum AIType
{
	AI_NONE		= 0,
	AI_NORMAL	= 1,
	AI_CHAMPION	= 2,
};
