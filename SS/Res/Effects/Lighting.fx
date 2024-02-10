//////////////////////////////////////////////////////////////////////////

float3 g_Attenuation = 
{
	1.0f,	//Constant
	0.0f,	//Linear
	0.0f	//Quadratic
};

//////////////////////////////////////////////////////////////////////////

const int eLT_Point			= 1;
const int eLT_Spot			= 2;
const int eLT_Directional	= 3;

int g_eLightType = 1;

float	g_fLightDistance = 1200.0f;

float4	g_cAmbient = {0.0f, 0.0f, 0.0f, 1.0f};

float3	g_vLightPos = {0.0f, 0.0f, 0.0f};

float4	g_cLightDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
float4	g_cLightAmbient = {0.05f, 0.05f, 0.05f, 1.0f};

//////////////////////////////////////////////////////////////////////////

float Attenuation(float fDistance)
{
	float att = 0.0f;
	
	if (fDistance < g_fLightDistance)
	{
		if (g_eLightType == eLT_Directional)
			att = 1.0f;
		else
		{
			att = 1.0f / (	g_Attenuation[0] + 
							g_Attenuation[1] * fDistance + 
							g_Attenuation[2] * fDistance * fDistance);
		}
	}

	return att;
}

//////////////////////////////////////////////////////////////////////////

float3 Ambient(float3 matColor, float fDist)
{
	float3 cAmbientOut = {0.0f, 0.0f, 0.0f};
	
	cAmbientOut = matColor * (g_cAmbient.rgb + g_cLightAmbient.rgb * Attenuation(fDist));
	
	return clamp(cAmbientOut, 0.0f, 1.0f);
}

float3 Diffuse(float3 vPos, float3 vNormal, float3 cColor)
{
	float3 cDiffuseOut = g_cLightDiffuse * cColor;
	
	//vNormal = mul(vNormal, matWorld);
	
	float fDot = dot(vNormal, normalize(g_vLightPos - vPos));
		
	//fDot *= 0.25f;
	
	fDot = clamp(fDot, 0.0f, 1.0f);
	
	cDiffuseOut *= fDot;
	
	cDiffuseOut = clamp(cDiffuseOut, 0.0f, 1.0f);
		
	return cDiffuseOut;
}

float3 VS_Lighting(VSInput In)
{
	float3 color = {0.0f, 0.0f, 0.0f};
	
	float fDist = distance(In.Pos, g_vLightPos);
	
	color = Ambient(In.Color.rgb, fDist) + Diffuse(In.Pos.xyz, In.Normal.xyz, In.Color.rgb);
	
	return color;
}