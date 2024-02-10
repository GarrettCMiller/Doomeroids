
#include "Globals.fx"
#include "Lighting.fx"

#include "Wavefunction.fx"

texture pPlanetTexture <>;
texture pPlanetTexture2 <>;
texture pAtmosTexture <>;

sampler sTexture = sampler_state
{
	Texture = <pPlanetTexture>;
	
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler sAtmos = sampler_state
{
	Texture = <pAtmosTexture>;
	
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler sTexture2 = sampler_state
{
	Texture = <pPlanetTexture2>;
	
	AddressU = WRAP;
	AddressV = WRAP;
};

///////////////////////////////////////////////////////////////

float gridSpacing = 25.0f;
int gridSize = 500;

float2 v2Zero = {0.0f, 0.0f};

float4 cPlanetColorCurrent;
float4 cPlanetColor[11];

float4 planetPos[11];
float planetRad[11];
float planetMass[11];

bool bColorGravityWell = true;
bool bUseNightTexture = false;

int bEnableGravity[11];

//////////////////////////////////////////////////////////////////

PSOutput PS_Planet(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	if (In.Color.r < 0.1f && bUseNightTexture)
	{
		Out.Color.rgb = tex2D(sTexture2, In.TexCoord);
	}
	else
	{
		Out.Color.rgb = tex2D(sTexture, In.TexCoord);
		Out.Color.rgb *= In.Color.rgb;
	}
	
	Out.Color.a = 1.0f;

	return Out;
}

PSOutput PS_Atmos(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	Out.Color.rgb = tex2D(sAtmos, In.TexCoord);
	//Out.Color.rgb *= In.Color.rgb;
	
	Out.Color.a = 1.0f;
	
	if (Out.Color.b - Out.Color.r > 0.5)
		Out.Color.a = 0.0f;

	return Out;
}

PSOutput PS_Star(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	Out.Color.a = 1.0f;
	Out.Color.rgb = tex2D(sTexture, In.TexCoord);
	
	return Out;
}

PSOutput PS_Orbit(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	Out.Color = In.Color;
	//Out.Color.rgb = tex2D(sTexture, In.TexCoord);
	
	return Out;
}

PSOutput PS_Grid(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	Out.Color = 1.0f;
	Out.Color.rgb = clamp(In.Color.rgb + 0.2f, 0.0f, 1.0f);
		
	return Out;
}

/////////////////////////////////////////////////////////////////

VSOutput VS_Planet(VSInput In)
{
	VSOutput Out = (VSOutput) 0;
	
	In.Pos = mul(In.Pos, matWorld);
	In.Normal.xyz = mul(In.Normal.xyz, matWorld);
	
	In.Color = 1.0f;	//cPlanetColorCurrent;
	
	In.Color.rgb = VS_Lighting(In);
	
	Out.Color.rgb = clamp(In.Color.rgb, 0.0f, 1.0f);
	Out.Color.a = 1.0f;
	
	Out.TexCoord = In.TexCoord;
	
	Out.Pos = mul(In.Pos, matViewProj);
	
	return Out;
}

VSOutput VS_Star(VSInput In)
{
	VSOutput Out = (VSOutput) 0;
	
	In.Pos = mul(In.Pos, matWorld);
	In.Normal = mul(In.Normal, matWorld);
	
	In.Normal *= -1;
	
	In.Color.rgb = cPlanetColorCurrent;
	
	In.Color.rgb = VS_Lighting(In);
	//In.Color.rg = 1.0f;
	
	Out.Color.a = 1.0f;
	Out.Color.rgb = clamp(In.Color.rgb, 0.0f, 1.0f);
	
	Out.TexCoord = In.TexCoord;
	
	Out.Pos = mul(In.Pos, matViewProj);
	
	return Out;
}

VSOutput VS_Orbit(VSInput In)
{
	VSOutput Out = (VSOutput) 0;
	
	Out.Color = In.Color;
	
	Out.TexCoord = In.TexCoord;
	
	Out.Pos = mul(In.Pos, matViewProj);
	
	return Out;
}

VSOutput VS_Grid(VSInput In)
{
	VSOutput Out = (VSOutput) 0;
	/*
	InitMainWF();
	
	WaveFunc w[11];
	
	w[0] = w[1] = w[2] = 
		w[3] = w[4] = w[5] =
			w[6] = w[7] = w[8] = 
				w[9] = w[10] = mainWF;
	
	Complex psi = (Complex) 0;
	float result = 0;
	
	Out.Color.a = 1;

	for (int p = 0; p < 11; p++)
	{
		float3 ppos = planetPos[p];
		
		if (bEnableGravity[p])
		{	
			if (p > 0)
			{
				w[p].x0 = ppos.x;
				w[p].z0 = ppos.z;
				w[p].a = w[p].c = (1 - planetMass[p] * 0.003f);
				w[p].cExp = 0.1f * (1 - (planetMass[p] / 1000.0f)) + 0.05f;
			}
		
			psi = WaveFunction(In, w[p]);
			
			float amp = SetAmp(psi);
			
			result += amp;
			
			if (bColorGravityWell && p > 0)
			{
				float3 color = cPlanetColor[p].rgb * (amp * amp * 0.05f);
				
				clamp(color, 0.0f, 1.0f);
				
				Out.Color.rgb += color;
			}
		}
	}
	
	In.Pos.y -= result;
	*/
	
	Out.Pos = mul(In.Pos, matViewProj);
	
	return Out;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

technique Planet
{
	pass P0
	{
		//FILLMODE = WIREFRAME;
		
		PixelShader		= compile ps_2_0 PS_Planet();
		VertexShader	= compile vs_2_0 VS_Planet();
	}
}

technique Atmosphere
{
	pass P0
	{
		//FILLMODE = WIREFRAME;
		CULLMODE = NONE;
		
		PixelShader		= compile ps_2_0 PS_Atmos();
		VertexShader	= compile vs_2_0 VS_Planet();
	}
}

technique Orbit
{
	pass P0
	{
		VertexShader	= compile vs_2_0 VS_Orbit();
		PixelShader		= compile ps_2_0 PS_Orbit();
	}
}

technique Star
{
	pass P0
	{
		//FILLMODE = WIREFRAME;
		
		//CULLMODE = NONE;
	
		PixelShader		= compile ps_2_0 PS_Star();
		VertexShader	= compile vs_2_0 VS_Star();
	}
}

technique Grid
{
	pass P0
	{
		FILLMODE = WIREFRAME;
		CULLMODE = NONE;
		
		PixelShader		= compile ps_2_0 PS_Grid();
		VertexShader	= compile vs_2_0 VS_Grid();		
	}
}