varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;
varying vec3 posCam;
varying vec3 lightDirection;


uniform float elapsed;
uniform mat4 invertView;
uniform vec3 camPos;
uniform vec3 sunPos;



void main()
{
	float pi = 3.141592f; 
	// Transforming The Vertex

	// Transforming The Normal To ModelView-Space
	normal = gl_NormalMatrix * gl_Normal;

	//Direction lumiere 
	vertex_to_light_vector = vec3(sunPos );
	
	vec4 vertexModel = gl_Vertex;
	// passer en coordonnee monde global
	vec4 vertexMonde = gl_ModelViewMatrix * gl_Vertex;
	vertexMonde = invertView * vertexMonde;
	posCam = camPos.xyz - vertexMonde;
	lightDirection = normalize(vertex_to_light_vector);

	//posCam = ( invertView * (gl_ModelViewMatrix * vec4(camPos,1.0f) ) ).xyz;
	

	/* EFFET VAGUE EAU et NUAGE */
	if(gl_Color.a >= 0.28f && gl_Color.a <=0.32f  ){
		vertexModel.z += 3.0f * cos(((2*pi*(vertexMonde.x+vertexMonde.y*0.5))/75) - ((2*pi*elapsed)/4)) + 10 + 2.0f*cos(((2*pi*vertexMonde.y)/100) - ((2*pi*elapsed)/8)) ;
	
		// cheminement inverse
		gl_Position = gl_ModelViewProjectionMatrix * vertexModel;



	
	}
	else if(color.a >= 0.17 && color.a <= 0.22){
	// cheminement inverse
		gl_Position = gl_ModelViewProjectionMatrix * vertexModel;
	}
	else{ //traitement autres cubes
		// cheminement inverse
		gl_Position = gl_ModelViewProjectionMatrix * vertexModel;
		gl_Color.a = 1.0f;
	}
		
	
	



	//Couleur
	color = gl_Color;

}