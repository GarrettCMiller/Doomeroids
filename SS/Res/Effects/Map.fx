#include "Globals.fx"

texture pTexture <>;
texture pWallTexture <>;

sampler sMapTexture = sampler_state
{
	Texture = <pTexture>;
	
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler sWallTexture = sampler_state
{
	Texture = <pWallTexture>;
	
	AddressU = WRAP;
	AddressV = WRAP;
};

///////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////

PSOutput PS_MapLighting(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
		
	Out.Color.a = 1.0f;
	Out.Color.rgb = tex2D(sMapTexture, In.TexCoord);
	
	return Out;
}

PSOutput PS_MapRender(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	Out.Color.a = 1.0f;
	Out.Color.rgb = tex2D(sMapTexture, In.TexCoord);
	
	if (!any(Out.Color.rgb))
		Out.Color.rgb = 0.1f;
		
	return Out;
}

PSOutput PS_MapRenderWire(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	Out.Color = 1.0f;
	//Out.Color.rgb = tex2D(sMapTexture, In.TexCoord) * 2.0f;
	
	//if (!any(Out.Color.rgb))
	//	Out.Color.rgb = 0.1f;
		
	return Out;
}

PSOutput PS_RenderMapObjects(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	Out.Color.a = 1.0f;
	Out.Color = tex2D(sWallTexture, In.TexCoord);
		
	return Out;
}

/////////////////////////////////////////////////////////////////

VSOutput VS_MapTexture(VSInput In)
{
	VSOutput Out = (VSOutput) 0;
	
	Out.Color = In.Color;
	Out.TexCoord = In.TexCoord;
	
	In.Pos.y = 0.0f;
	//In.Pos.z += 0.25f;
	
	Out.Pos = mul(In.Pos, matWorldViewProj);
	
	return Out;
}

VSOutput VS_RenderMapObjects(VSInput In)
{
	VSOutput Out = (VSOutput) 0;
	
	Out.Color = In.Color;
	Out.TexCoord = In.TexCoord;

	Out.Pos.z += 5.0f;
	
	Out.Pos = mul(In.Pos, matWorldViewProj);
	
	return Out;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

technique Map
{
	pass P0
	{
		//FILLMODE = WIREFRAME;
		
		PixelShader		= compile ps_2_0 PS_MapRender();
		VertexShader	= compile vs_2_0 VS_MapTexture();
	}
	
	/*
	pass P1
	{
		FILLMODE = WIREFRAME;
		
		ZENABLE = FALSE;
		
		//PixelShader		= compile ps_2_0 PS_MapRender();
		PixelShader		= compile ps_2_0 PS_MapRenderWire();
		VertexShader	= compile vs_2_0 VS_MapTexture();
	}
	*/
}

technique MapLighting
{
	pass P0
	{
		PixelShader		= compile ps_2_0 PS_MapLighting();
	}
}

technique RenderMapObjects
{
	pass P0
	{
		//CULLMODE = NONE;
		
		FILLMODE = WIREFRAME;
		
		VertexShader	= compile vs_2_0 VS_RenderMapObjects();
		//PixelShader		= compile ps_2_0 PS_RenderMapObjects();
	}
}