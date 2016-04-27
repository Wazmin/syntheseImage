varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;
varying vec3 posCam;


uniform float ambientLevel;
uniform mat4 invertView;

struct material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
material mymaterial = material(
  vec4(0.2, 0.2, 0.2, 1.0),
  vec4(1.0, 0.8, 0.8, 1.0),
  vec4(1.0, 1.0, 1.0, 1.0),
  10.0
);

void main()
{
	// Scaling The Input Vector To Length 1
	vec3 normalized_normal = normalize(normal);
	vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	vec3 viewDirection = - normalize(posCam);

	if(color.a > 0.05 && color.a <= 0.15){// CUBE DE TERRE
	
	}
	else if(color.a > 0.15 && color.a <= 0.25){ // CUBE d'HERBE
		color.a = 1.0f;

	}
	else if (color.a >= 0.25 && color.a <= 0.35){ // CUBE D'EAU
		color.rg = 0.1f;
		color.b = 0.95f;
		color.a=1.0f;

	}
	else{
		color.a = 1.0f;
	
	}

	// Calculating The Diffuse Term And Clamping It To [0;1]
	float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);

	// Calculating The Final Color
	
	gl_FragColor = color * (DiffuseTerm*(1-ambientLevel) + ambientLevel );
	gl_FragColor.a = color.a;
}