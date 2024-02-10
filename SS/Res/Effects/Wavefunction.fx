///////////////////////////////////////////////////////////////

float	timeOffset = 0.0f;

int nRenderMode = 0;
int numWaveFuncs = 1;

static const float cExp =	0.1f;
							//0.05f;
							//0.1f;

float x0 = 0.0f;
float z0 = 0.0f;

float sigmaX =	//0.2f;
				//0.5f;
				0.15f;
				//100.0f;

float sigmaZ =	//0.2f;
				//0.5f;
				0.15f;
				//100.0f;

float v0 = 1.0f;

struct WaveFunc
{
	float	cExp;
	
	float	x0;
	float	z0;
	
	float	v0;
	
	float	a;
	float	c;
	
	float	sigmaX;
	float	sigmaZ;
};

WaveFunc mainWF;

void InitMainWF()
{
	mainWF.cExp		= cExp;
	mainWF.x0		= x0;
	mainWF.z0		= z0;
	mainWF.v0		= v0;
	mainWF.a		= 1.0f;
	mainWF.c		= 1.0f;
	mainWF.sigmaX	= sigmaX;
	mainWF.sigmaZ	= sigmaZ;
}

///////////////////////////////////////////////

float WaveAmplitude(VSInput In, WaveFunc wf)
{
	float ampX = pow(TWOPI, wf.cExp);
	ampX *= sqrt(wf.sigmaX);
	ampX = 1.0f / ampX;
	
	float ampZ = pow(TWOPI, wf.cExp);
	ampZ *= sqrt(wf.sigmaZ);
	ampZ = 1.0f / ampZ;
	
	float	a = 0.00075f * wf.a,
			b = 0.0f,
			c = 0.00075f * wf.c;
			
	float	ax	= a * ( (In.Pos.x - wf.x0 - wf.v0*timeOffset) * (In.Pos.x - wf.x0 - wf.v0*timeOffset) ),
			bxz	= b * (In.Pos.x - wf.x0 - wf.v0*timeOffset) * (In.Pos.z - wf.z0),
			cz	= c * (In.Pos.z - wf.z0) * (In.Pos.z - wf.z0);
			
	float	ex = ax + bxz + cz;
	
	ex = exp(-ex);
	
	return ampX * ampZ * ex;
}

float Phase(VSInput In)
{
	float3 pos = {In.Pos.x, In.Pos.y, 0.0f};
	float3 dir = {1.0f, 0.0f, 0.0f};
	
	float dv = dot(dir, pos);
	
	return dv - timeOffset;
}

Complex WaveFunction(VSInput In, WaveFunc wf)
{
	Complex psi = (Complex) 0;
	
	float amp =		WaveAmplitude(In, wf);
	float theta =	Phase(In);
	
	psi.real = amp * cos(theta);
	psi.imag = amp * sin(theta);
	
	return psi;	
}

///////////////////////////////////////////////////////////////

float SetAmp(Complex psi)
{
	float	r = psi.real,
			i = psi.imag;
			
	float	amp = 0.0f;
	
	if (nRenderMode == 0)
		amp = r * r + i * i;	
	
	/*if (nRenderMode == 1)
		amp = r;
		
	if (nRenderMode == 2)
		amp = i;*/
		
	return amp;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
