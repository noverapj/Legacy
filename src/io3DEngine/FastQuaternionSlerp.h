

#ifndef _FastQuaternionSlerp_h_
#define _FastQuaternionSlerp_h_

static const float ISQRT_NEIGHBORHOOD = 0.959066f;
static const float ISQRT_SCALE = 1.000311f;
static const float ISQRT_ADDITIVE_CONSTANT = ISQRT_SCALE / (float)sqrt(ISQRT_NEIGHBORHOOD);
static const float ISQRT_FACTOR = ISQRT_SCALE * (-0.5f / (ISQRT_NEIGHBORHOOD * (float)sqrt(ISQRT_NEIGHBORHOOD)));

inline float ISqrt_approx_in_neighborhood(float s) 
{
    return ISQRT_ADDITIVE_CONSTANT + (s - ISQRT_NEIGHBORHOOD) * ISQRT_FACTOR;
}

//---------------------------------------------------------------------------

// Normalize a quaternion using the above approximation.

inline void FastNormalize( D3DXQUATERNION &rkQ ) 
{
    float s = rkQ.x*rkQ.x + rkQ.y*rkQ.y + rkQ.z*rkQ.z + rkQ.w*rkQ.w; // length^2
    float k = ISqrt_approx_in_neighborhood(s);

    if (s <= 0.91521198f) 
	{
        k *= ISqrt_approx_in_neighborhood(k * k * s);

        if (s <= 0.65211970f) 
		{
            k *= ISqrt_approx_in_neighborhood(k * k * s);
        }
    }

    rkQ.x *= k;
    rkQ.y *= k;
    rkQ.z *= k;
    rkQ.w *= k;
}

//---------------------------------------------------------------------------

inline float LerpFloat(float v0, float v1, float fPerc)
{
    return v0 + fPerc * (v1 - v0);
}

//---------------------------------------------------------------------------

// CounterWarp: A helper function used by Slerp.

inline float CounterWarp(float t, float fCos) 
{
    const float ATTENUATION = 0.82279687f;
    const float WORST_CASE_SLOPE = 0.58549219f;

    float fFactor = 1.0f - ATTENUATION * fCos;
    fFactor *= fFactor;

    float fK = WORST_CASE_SLOPE * fFactor;

    return t*(fK*t*(2.0f*t - 3.0f) + 1.0f + fK);
}


#endif