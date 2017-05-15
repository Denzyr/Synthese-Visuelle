varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

uniform float elapsed;
uniform mat4 invertView;

void main()
{
	vec4 vertexCoord = gl_ModelViewMatrix * gl_Vertex;
	if(gl_Color.b > 0.3)
	{
		vertexCoord.z += sin(vertexCoord.x+elapsed)*10;
	}
	// Transforming The Vertex
	gl_Position = gl_ProjectionMatrix * vertexCoord;

	// Transforming The Normal To ModelView-Space
	normal = gl_NormalMatrix * gl_Normal; 

	//Direction lumiere
	vertex_to_light_vector = vec3(gl_LightSource[0].position);

	//Couleur
	color = gl_Color;
}