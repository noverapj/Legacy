

#ifndef _AxisXYZ_h_
#define _AxisXYZ_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class __EX AxisXYZ
{
private:
	ioVtxColor	m_Axis[6];

public:
	void Create( float fLineLen );
	void DrawAxis();

public:
	AxisXYZ();
	~AxisXYZ();
};

#endif