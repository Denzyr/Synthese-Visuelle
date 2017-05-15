
uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float screen_width;
uniform float screen_height;


float LinearizeDepth(float z)
{
	float n = 0.5; // camera z near
  	float f = 10000.0; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

void main (void)
{
	

	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );
	float depth = texture2D( Texture1 , vec2( gl_TexCoord[0] ) ).r;	
	float depthdiff = LinearizeDepth(texture2D( Texture1 , vec2( gl_TexCoord[0] )-vec2(xstep,ystep) ).r);
	depthdiff += LinearizeDepth(texture2D( Texture1 , vec2( gl_TexCoord[0] )-vec2(xstep,0) ).r);
	depthdiff += LinearizeDepth(texture2D( Texture1 , vec2( gl_TexCoord[0] )-vec2(0,ystep) ).r);
	depthdiff += LinearizeDepth(texture2D( Texture1 , vec2( gl_TexCoord[0] )+vec2(xstep,ystep) ).r);
	depthdiff += LinearizeDepth(texture2D( Texture1 , vec2( gl_TexCoord[0] )+vec2(xstep,0) ).r);
	depthdiff += LinearizeDepth(texture2D( Texture1 , vec2( gl_TexCoord[0] )+vec2(0,ystep) ).r);
	depthdiff += LinearizeDepth(texture2D( Texture1 , vec2( gl_TexCoord[0] )+vec2(xstep,-ystep) ).r);
	depthdiff += LinearizeDepth(texture2D( Texture1 , vec2( gl_TexCoord[0] )+vec2(-xstep,ystep) ).r);

	depth = LinearizeDepth(depth);
	float value = abs(8*depth-depthdiff);

	//Permet de scaler la profondeur
	
	if(value > 0.0001)
	{
		gl_FragColor = color*vec4(0.4,0.4,0.4, 1);
	}
	else
	{
		gl_FragColor = color;
	}
	
}