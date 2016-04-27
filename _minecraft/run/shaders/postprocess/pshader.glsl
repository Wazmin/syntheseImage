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


	// pour effet de bordures noires
	float dVoisinHaut = texture2D( Texture1 , vec2(gl_TexCoord[0].x , gl_TexCoord[0].y + 1*ystep ) ).r;
	float dVoisinBas = texture2D( Texture1 , vec2(gl_TexCoord[0].x , gl_TexCoord[0].y - 1*ystep ) ).r;
	float dVoisinGauche = texture2D( Texture1 , vec2(gl_TexCoord[0].x +1*xstep , gl_TexCoord[0].y ) ).r;
	float dVoisinDroite = texture2D( Texture1 , vec2(gl_TexCoord[0].x -1*xstep, gl_TexCoord[0].y ) ).r;	
	
	//Permet de scaler la profondeur
	depth = LinearizeDepth(depth);
	dVoisinHaut = LinearizeDepth(dVoisinHaut);
	dVoisinBas =  LinearizeDepth(dVoisinBas);
	dVoisinGauche =  LinearizeDepth(dVoisinGauche);
	dVoisinDroite =  LinearizeDepth(dVoisinDroite);

	float indiceDif = 4* depth - (dVoisinHaut +dVoisinBas+ dVoisinGauche+dVoisinDroite);

	if(indiceDif > 0.0001 && color.b<0.5f){
	color.r =0.0f ;
	color.g =0.0f;
	color.b =0.0f ;
	color.a =1.0f;
	}else{
		color = texture2D( Texture0 , vec2( gl_TexCoord[0].x , gl_TexCoord[0].y + 1*ystep ) ) +texture2D( Texture0 , vec2( gl_TexCoord[0].x , gl_TexCoord[0].y - 1*ystep ))+
		texture2D( Texture0 , vec2( gl_TexCoord[0].x +1*xstep , gl_TexCoord[0].y  ))+texture2D( Texture0 , vec2( gl_TexCoord[0].x -1*xstep, gl_TexCoord[0].y ));
		color /= 4.0f;
		color.a = 1.0f;
	}


/*
*	Effet de brouillard
*
*/
//	float d = 1/ (depth * 40);
//
//	if(depth > 0.04){
//		float f = (color.b + color.r + color.g)/3.0f;
//		color.r = color.r /d ;
//		color.g = color.g/d ;
//		color.b = color.b/d;
//		color.a =d;
//	}




	

	gl_FragColor = color;
}