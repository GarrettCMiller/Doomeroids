//////////////////////////////////////////////////////////////////////////

#include "Globals.fx"

//////////////////////////////////////////////////////////////////////////

float2 Delta;

float2 Z;
float2 C;

static const int nMaxIters = 2;

texture pTexture <>;

sampler sFractal = sampler_state
{
	Texture = <pTexture>;
	
	AddressU = WRAP;
	AddressV = WRAP;
};

//////////////////////////////////////////////////////////////////////////

PSOutput PS_Mandelbrot(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	C = In.TexCoord;
	
	Z = C;
	
	float2 z = Z;
	float orbit = 0.0f;
	int i = 0;
	
	for (i = 0; i < nMaxIters; i++)
	{
		z = Z;
		
		orbit = z.x * z.x + z.y * z.y;
		
		if (orbit > 4.0f)
			i = nMaxIters; //HACK, no break statements
		else
		{
			Z.x = z.x * z.x - z.y * z.y - (C.x / (C.x * C.x + C.y * C.y));
			Z.y = 2.0f * z.x * z.y - (-C.y / (C.x * C.x + C.y * C.y));
		}
	}
	
	float tCoord = i / float(nMaxIters);
	
	Out.Color.a = 1.0f;
	
	//Out.Color = tex1D(sFractal, tCoord);
	Out.Color.rgb = tCoord;
		
	return Out;
}

//////////////////////////////////////////////////////////////////////////

VSOutput VS_Mandelbrot(VSInput In)
{
	VSOutput Out = (VSOutput) 0;
	
	return Out;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

technique Mandelbrot
{
	pass P0
	{
		PixelShader = compile ps_2_0 PS_Mandelbrot();
	}
}