int gp : SasGlobal
<
  int3 SasVersion = {3,0,0};
>;

//////////////////////////////////////////////////////////////////////////

float4x4 matWorldViewProj;
float4x4 matViewProj;
float4x4 matWorld;

float	g_Time = 0.0f;

//////////////////////////////////////////////////////////////////////////

sampler Sprite : register(s0);

//////////////////////////////////////////////////////////////////////////

struct Complex
{
	float	real;
	float	imag;
};

//////////////////////////////////////////////////////////////////////////

static const float PI = 3.1415926535897932384626433832795;

static const float TWOPI = 6.283185307179586476925286766559;

//////////////////////////////////////////////////////////////////////////

float Sine(float freq, float base, float amp, float phase)
{
	return amp * sin(phase * freq * TWOPI) + base;
}

float Cosine(float freq, float base, float amp, float phase)
{
	return amp * cos(phase * freq * TWOPI) + base;
}

float Tangent(float freq, float base, float amp, float phase)
{
	return amp * tan(phase * freq * TWOPI) + base;
}

////////////////////////////////////////////////

struct PSInput
{
	float4	Color		: COLOR;
	float2	TexCoord	: TEXCOORD0;
};

struct PSOutput
{
    float4 Color		: COLOR;
};

///////////////////////////////////////////////////////////////

struct VSInput
{
	float4	Color		: COLOR;
	float4	Pos			: POSITION;
	float4	Normal		: NORMAL0;
	float2	TexCoord	: TEXCOORD0;
};

struct VSOutput
{
	float4	Color		: COLOR;
	float4	Pos			: POSITION;
	float2	TexCoord	: TEXCOORD0;
};