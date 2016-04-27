varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;
varying vec3 posCam;

uniform float elapsed;
uniform mat4 invertView;
uniform vec3 camPos;


void main()
{
	float pi = 3.141592f; 
	// Transforming The Vertex
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	// Transforming The Normal To ModelView-Space
	normal = gl_NormalMatrix * gl_Normal; 

	//Direction lumiere
	vertex_to_light_vector = vec3(gl_LightSource[0].position);

	vec4 vertexModel = gl_Vertex;

		// passer en coordonnee monde global
		vec4 vertexMonde = gl_ModelViewMatrix * gl_Vertex;
		vertexMonde = invertView * vertexMonde;


	/* EFFET VAGUE EAU et NUAGE */
	if(gl_Color.a == 0.3f  ){
		//vertexModel.z += (sin(elapsed *3 + vertexMonde.x ) / 2) * 4;
		vertexModel.z += 2.0f * cos(((2*pi*vertexMonde.x)/100) - ((2*pi*elapsed)/4)) + 10 + 4*cos(((2*pi*vertexMonde.y)/125) - ((2*pi*elapsed)/4)) ;
		// cheminement inverse
		gl_Position = gl_ModelViewProjectionMatrix * vertexModel;
	
		//gl_Color.a = 1.0f;
	}
	else if(gl_Color.a == 0.13f){
	// cheminement inverse
		gl_Position = gl_ModelViewProjectionMatrix * vertexModel;

		

		//gl_Color.a = 1.0f;
	
	}
	else{ //traitement autre que les cubes
		// cheminement inverse
		gl_Position = gl_ModelViewProjectionMatrix * vertexModel;
		gl_Color.a = 1.0f;
	}
		
	
	posCam = ( invertView * (gl_ModelViewMatrix * vec4(camPos,1.0f) ) ).xyz;

	//Couleur
	color = gl_Color;

}