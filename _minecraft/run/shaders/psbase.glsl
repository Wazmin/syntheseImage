varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;
varying vec3 posCam;
varying vec3 lightDirection;



uniform float ambientLevel;
uniform mat4 invertView;

struct material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
material water = material(
  vec4(0.2f, 0.2f, 0.2f, 1.0f),
  vec4(0.1f, 0.1f, 0.95f, 1.0f),
  vec4(0.1f, 0.1f, 0.95f, 1.0f),
  0.250f
);

material herbe = material(
  vec4(0.0f, 0.70f, 0.1f, 1.0f),
  vec4(0.0f, 0.45f, 0.05f, 1.0f),
  vec4(0.0f, 0.0f, 0.0f, 1.0f),
  1.0f
);

void main()
{
	// Scaling The Input Vector To Length 1
	vec3 normalized_normal = normalize(normal);
	vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	vec3 viewDirection = normalize(posCam);
	float dist = 1.50f;
	
	if(color.a > 0.05 && color.a < 0.15){// CUBE DE TERRE
		
	color.a = 1.0f;
	
	}
	else if(color.a >= 0.17 && color.a <= 0.22){ // CUBE d'HERBE
		color.a=1.0f;
	}
	else if (color.a >= 0.25 && color.a <= 0.35){ // CUBE D'EAU
		vec3 ambientLighting = gl_LightSource[0].ambient * vec3(water.ambient);
	
		float attenuation =  1.0f / (1.0f * dist
			   + 0.05f * dist * dist);
	
		vec3 diffuseReflection = attenuation
			* gl_LightSource[0].diffuse * vec3(water.diffuse)
			* max(0.0f, dot(normal, lightDirection));
	
		vec3 specularReflection;
		
		if (dot(normal, lightDirection) < 0.0) // light source on the wrong side?
		{
		  specularReflection = vec3(0.0f, 0.0f, 0.0f); // no specular reflection
		}
		else // light source on the right side
		{
			specularReflection = attenuation * vec3(water.specular)
			* pow(max(0.0, dot(reflect(-lightDirection, normal), viewDirection)),
			water.shininess);
		}

		color.a=1.0f;
		color.rgb = (ambientLighting + diffuseReflection + specularReflection).rgb;
	}
	else{
		color.a = 1.0f;
	}

	// Calculating The Final Color
	
	gl_FragColor = color ;//* (DiffuseTerm*(1-ambientLevel) + ambientLevel );
	gl_FragColor.a = color.a;
}