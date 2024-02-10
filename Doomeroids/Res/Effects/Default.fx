#include "Globals.fx"

///////////////////////////////////////////////////////////////
static const int kMaxLights = 4;

int		NumActiveLights = kMaxLights;

float	time = 0.0;

float	fUmbra0 = 150.0f;
float	fUmbra1 = 150.0f;
float	fUmbra2 = 150.0f;
float	fUmbra3 = 150.0f;

float	fPenumbra0 = 2.0f;
float	fPenumbra1 = 25.0f;
float	fPenumbra2 = 25.0f;
float	fPenumbra3 = 25.0f;	

float2	vLightVec0 = float2(512,	450);		//Center
float2	vLightVec1 = float2(512,	128);		//Top
float2	vLightVec2 = float2(896,	384);		//Right
float2	vLightVec3 = float2(512,	640);		//Bottom

float4	vLightColor0 = float4(0.5f,	0.5f,	0.5f,	1.0f);
float4	vLightColor1 = float4(1.0f,	1.0f,	1.0f,	1.0f);
float4	vLightColor2 = float4(1.0f,	1.0f,	1.0f,	1.0f);
float4	vLightColor3 = float4(1.0f,	1.0f,	1.0f,	1.0f);

float4	vAmbientLight = {0.05f, 0.05f, 0.05f, 1.0f};

texture Texture <>;
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////

float DoLight0(float2 Pos3D)
{
	float dist = abs((vLightVec0.x - Pos3D.x) * (vLightVec0.x - Pos3D.x) + (vLightVec0.y - Pos3D.y) * (vLightVec0.y - Pos3D.y));
	float amt = 0.0f;
	
	//Are we outside the light's range?
	if (dist < fUmbra0 * fUmbra0)
	{
 		if (dist > fPenumbra0 * fPenumbra0)
 		{
 			amt = sqrt(dist) - fPenumbra0;
 			amt /= fUmbra0 - fPenumbra0;
 		}
	 		
		amt = 1.0f - amt;
	}
	
	return clamp(amt, 0.0, 1.0);
}

float DoLight1(float2 Pos3D)
{
	float dist = abs((vLightVec1.x - Pos3D.x) * (vLightVec1.x - Pos3D.x) + (vLightVec1.y - Pos3D.y) * (vLightVec1.y - Pos3D.y));
	float amt = 0.0f;
	
	//Are we outside the light's range?
	if (dist < fUmbra1 * fUmbra1)
	{
 		if (dist > fPenumbra1 * fPenumbra1)
 		{
 			amt = sqrt(dist) - fPenumbra1;
 			amt /= fUmbra1 - fPenumbra1;
 		}
	 		
		amt = 1.0f - amt;
	}
	
	return clamp(amt, 0.0, 1.0);
}

float DoLight2(float2 Pos3D)
{
	float dist = abs((vLightVec2.x - Pos3D.x) * (vLightVec2.x - Pos3D.x) + (vLightVec2.y - Pos3D.y) * (vLightVec2.y - Pos3D.y));
	float amt = 0.0f;
	
	//Are we outside the light's range?
	if (dist < fUmbra2 * fUmbra2)
	{
 		if (dist > fPenumbra2 * fPenumbra2)
 		{
 			amt = sqrt(dist) - fPenumbra2;
 			amt /= fUmbra2 - fPenumbra2;
 		}
	 		
		amt = 1.0f - amt;
	}
	
	return clamp(amt, 0.0, 1.0);
}

float DoLight3(float2 Pos3D)
{
	float dist = abs((vLightVec3.x - Pos3D.x) * (vLightVec3.x - Pos3D.x) + (vLightVec3.y - Pos3D.y) * (vLightVec3.y - Pos3D.y));
	float amt = 0.0f;
	
	//Are we outside the light's range?
	if (dist < fUmbra3 * fUmbra3)
	{
 		if (dist > fPenumbra3 * fPenumbra3)
 		{
 			amt = sqrt(dist) - fPenumbra3;
 			amt /= fUmbra3 - fPenumbra3;
 		}
	 		
		amt = 1.0f - amt;
	}
	
	return clamp(amt, 0.0, 1.0);
}

float DiffLighting(int lightindex, float2 Pos3D)
{
	if (lightindex == 0)
		return DoLight0(Pos3D);
	else if (lightindex == 1)
		return DoLight1(Pos3D);
	else if (lightindex == 2)
		return DoLight2(Pos3D);
	else if (lightindex == 3)
		return DoLight3(Pos3D);
	else
		return 0.0f;
}

float3 GetLightColor(int lightindex)
{
	if (lightindex == 0)
		return vLightColor0.rgb;
	else if (lightindex == 1)
		return vLightColor1.rgb;
	else if (lightindex == 2)
		return vLightColor2.rgb;
	else if (lightindex == 3)
		return vLightColor3.rgb;
	else
		return float3(1.0f, 0.0f, 1.0f);
}

float3 PPLighting(float2 Pos2D)
{
	float3 color = vAmbientLight.rgb;
	float3 lightColor = float3(1.0f, 1.0f, 1.0f);
	
	if (NumActiveLights > 0)
	{
		lightColor = GetLightColor(0);
	  	color += DiffLighting(0, Pos2D) * lightColor;
	  	
	  	if (NumActiveLights > 1)
	  	{
  			lightColor = GetLightColor(1);
  			color += DiffLighting(1, Pos2D) * lightColor;
  			
  			if (NumActiveLights > 2)
  			{
  				lightColor = GetLightColor(2);
  				color += DiffLighting(2, Pos2D) * lightColor;
  				
  				if (NumActiveLights > 3)
  				{
  					lightColor = GetLightColor(3);
  					color += DiffLighting(3, Pos2D) * lightColor;
  				}
  			}
	  	}
	}
 	
 	color = clamp(color, 0.0, 1.0);
	
	return color;
}



//////////////////////////////////////////////////////////////////
PSOutput PS_Emission(PSInput In)
{
	PSOutput Out	= (PSOutput) 0;
	
	Out.Color		= tex2D(Sprite, In.TexCoord);
	
	float3 diffuse 	= PPLighting(In.Pos);
	float3 newcolor = Out.Color.rgb * diffuse;
	
 	if (Out.Color.a > 0.1 && Out.Color.a < 1.0)
  	{
  		Out.Color.r = lerp(newcolor.r, Out.Color.r, Out.Color.a);
  		Out.Color.g = lerp(newcolor.g, Out.Color.g, Out.Color.a);
  		Out.Color.b = lerp(newcolor.b, Out.Color.b, Out.Color.a);
  	}
  	else
  		Out.Color.rgb = newcolor.rgb;
  	
  	if (Out.Color.a > 0)
  		Out.Color.a		= 1.0f;
  		
	return Out;
}

PSOutput PS_PPLighting(PSInput In)
{
	PSOutput Out	= (PSOutput) 0;
	
	Out.Color		= tex2D(Sprite, In.TexCoord);
	Out.Color.rgb	*= PPLighting(In.Pos);
	
	return Out;
}

PSOutput PS_MapLighting(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	Out.Color = In.Color;
	Out.Color.r = 0.0f;
	
	return Out;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

technique PerPixelLighting
{
	pass P0
	{
		PixelShader		= compile ps_3_0 PS_PPLighting();
	}
}

technique Map
{
	pass P0
	{
		PixelShader		= compile ps_2_0 PS_MapLighting();
	}
}

technique Emit
{
	pass P0
	{
		PixelShader		= compile ps_3_0 PS_Emission();
	}
}