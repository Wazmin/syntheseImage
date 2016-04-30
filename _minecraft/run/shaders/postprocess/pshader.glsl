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

	float voisins[8];
	float indiceDiff = LinearizeDepth(depth);
	indiceDiff *= 8;
	voisins[0] = texture2D( Texture1 , vec2(gl_TexCoord[0].x - 1*xstep , gl_TexCoord[0].y - 1*ystep ) ).r;
	voisins[1] = texture2D( Texture1 , vec2(gl_TexCoord[0].x - 0*xstep , gl_TexCoord[0].y - 1*ystep ) ).r;
	voisins[2] = texture2D( Texture1 , vec2(gl_TexCoord[0].x + 1*xstep , gl_TexCoord[0].y - 1*ystep ) ).r;
	voisins[3] = texture2D( Texture1 , vec2(gl_TexCoord[0].x - 1*xstep , gl_TexCoord[0].y - 0*ystep ) ).r;
	voisins[4] = texture2D( Texture1 , vec2(gl_TexCoord[0].x + 1*xstep , gl_TexCoord[0].y - 0*ystep ) ).r;
	voisins[5] = texture2D( Texture1 , vec2(gl_TexCoord[0].x - 1*xstep , gl_TexCoord[0].y + 1*ystep ) ).r;	
	voisins[6] = texture2D( Texture1 , vec2(gl_TexCoord[0].x - 0*xstep , gl_TexCoord[0].y + 1*ystep ) ).r;
	voisins[7] = texture2D( Texture1 , vec2(gl_TexCoord[0].x + 1*xstep , gl_TexCoord[0].y + 1*ystep ) ).r;	
	
	for (int i=0;i<8;i++){
		voisins[i] = LinearizeDepth(voisins[i]);
		indiceDiff -= voisins[i];
	}

	float deltaDiff = 0.0001;

	if(color.b >= 0.28f && color.r <= 0.8f ){
		deltaDiff = 0.01;
	}

	if(abs(indiceDiff) > deltaDiff){
		if(color.b >= 0.28f && color.r <= 0.8f){
			color.r =0.7f ;
			color.g =0.7f;
			color.b =0.9f ;
			color.a =1.0f;		
		}
		else{
			color.r =0.0f ;
			color.g =0.0f;
			color.b =0.0f ;
			color.a =1.0f;
		}
	}



/*
*	Effet de brouillard
*
*/
	// float d = (LinearizeDepth(depth) ) ;

	// if(d > 0.04 ){
		// float f = (color.b + color.r + color.g)/3.0f;
		// color.r = color.r  ;
		// color.g = color.g ;
		// color.b = color.b;
		// color.a =d;
	// }




	

	gl_FragColor = color;
}