

#ifndef _IntersectionUtility_h_
#define _IntersectionUtility_h_

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3]);

bool Clip( float fDenom, float fNumer, float &fT0, float &fT1 );

#endif