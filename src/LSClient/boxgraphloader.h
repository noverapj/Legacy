#pragma once

bool LoadBoxSpaceFromFile( BoxSpace& space, const char* pFilename );
bool LoadBoxSpaceFromMem( BoxSpace& space, const void* pBuf, int size );
