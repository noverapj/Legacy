#pragma once

class ioNpcChar;
class ioNpcBigTower : public ioNpcChar
{

public:
	virtual NpcType GetNpcType() const;
	virtual int  DontMoveEntityLevel() const;

public:
	ioNpcBigTower( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
	virtual ~ioNpcBigTower(void);
};


class ioNpcTwGuard : public ioNpcBigTower
{

public:
 	ioNpcTwGuard( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
 	virtual ~ioNpcTwGuard(void);
	virtual NpcType GetNpcType() const;
	virtual int  DontMoveEntityLevel() const;

};



class ioNpcLair : public ioNpcBigTower
{

public:
	ioNpcLair( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
	virtual ~ioNpcLair(void);
	virtual NpcType GetNpcType() const;
	virtual int  DontMoveEntityLevel() const;

};



class ioNpcLairGuard : public ioNpcBigTower
{

public:
	ioNpcLairGuard( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
	virtual ~ioNpcLairGuard(void);
	virtual NpcType GetNpcType() const;
	virtual int  DontMoveEntityLevel() const;

};


class ioNpcNexus : public ioNpcBigTower
{

public:
	ioNpcNexus( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
	virtual ~ioNpcNexus(void);
	virtual NpcType GetNpcType() const;
	virtual int  DontMoveEntityLevel() const;

};



class ioNpcNexusGuard : public ioNpcBigTower
{

public:
	ioNpcNexusGuard( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
	virtual ~ioNpcNexusGuard(void);
	virtual NpcType GetNpcType() const;
	virtual int  DontMoveEntityLevel() const;

};