#include "Globals.fx"

#include "Lighting.fx"

texture pTexture <>;

sampler sTexture = sampler_state
{
	Texture = <pTexture>;
	
	AddressU = CLAMP;
	AddressV = CLAMP;
};

///////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////

VSOutput VS_Entity(VSInput In)
{
	VSOutput Out = (VSOutput) 0;

	Out.Color.a		= 1.0f;

	float4x4 mat	= mul(matWorld, matViewProj);

	Out.Pos			= mul(In.Pos, mat);
	Out.Color.rgb	= VS_Lighting(In);
	Out.TexCoord	= In.TexCoord;

	return Out;
}

/////////////////////////////////////////////////////////////////

PSOutput PS_Entity(PSInput In)
{
	PSOutput Out = (PSOutput) 0;

	Out.Color = tex2D(sTexture, In.TexCoord);

	Out.Color.a = 1.0f;

	return Out;
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

technique Entity
{
	pass P0
	{
		PixelShader		= compile ps_3_0 PS_Entity();
		VertexShader	= compile vs_3_0 VS_Entity();
	}
}