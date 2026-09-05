

#ifndef _ioMaterialModifier_h_
#define _ioMaterialModifier_h_

class ioMaterial;
class ioEntityGroup;

class __EX ioMaterialScriptModifier
{
public:
	virtual void ModifyMaterial( ioMaterial *pMaterial ) = 0;

public:
	ioMaterialScriptModifier() {}
	virtual ~ioMaterialScriptModifier() {}
};

class __EX ioEntityGrpMaterialModifier
{
public:
	virtual void ModifyMaterial( ioEntityGroup *pGrp ) = 0;

public:
	ioEntityGrpMaterialModifier(){}
	virtual ~ioEntityGrpMaterialModifier(){}
};

#endif