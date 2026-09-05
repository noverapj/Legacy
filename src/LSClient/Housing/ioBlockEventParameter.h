#pragma once

class ioBlockEventParameter
{
public:
	ioBlockEventParameter(){};
	virtual ~ioBlockEventParameter(){};
};

class ioBlockEventCreate : public ioBlockEventParameter
{
public:
	int nCode;
	int nTileX;
	int nTileY;
	int nTileZ;

	ioBlockEventCreate()
	{
		nCode	= 0;
		nTileX	= 0;
		nTileY	= 0;
		nTileZ	= 0;
	}
	
	virtual ~ ioBlockEventCreate()
	{
	}
};