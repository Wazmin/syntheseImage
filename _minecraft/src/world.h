#ifndef __WORLD_H__
#define __WORLD_H__

#include "gl/glew.h"
#include "gl/freeglut.h" 
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "chunk.h"
#include "my_physics.h"
#include <time.h>  
#include <math.h> 

#include "engine/render/graph/tex_manager.h"



typedef uint8 NYAxis;
#define NY_AXIS_X 0x01
#define NY_AXIS_Y 0x02
#define NY_AXIS_Z 0x04

#define MAT_SIZE 12 //en nombre de chunks
#define MAT_HEIGHT 5 //en nombre de chunks
#define MAT_SIZE_CUBES (MAT_SIZE * NYChunk::CHUNK_SIZE)
#define MAT_HEIGHT_CUBES (MAT_HEIGHT * NYChunk::CHUNK_SIZE)


class NYWorld
{
public :
	NYChunk * _Chunks[MAT_SIZE][MAT_SIZE][MAT_HEIGHT];
	int _MatriceHeights[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	float _FacteurGeneration;

	GLfloat mShininess = 100;
	int altitudeNeige = 0.85f* MAT_HEIGHT_CUBES;
	int altitudeEau = 0.3f *MAT_HEIGHT_CUBES;
	float indiceNuageux = 50;
	NYTexFile * _TexGrass;
	
	struct Point2D {
		int x;
		int y;
		int moyHeight = -1;

		Point2D(){}
		Point2D(int _x, int _y) :x(_x), y(_y) {}

		Point2D getMid2(Point2D A, Point2D B) {
			Point2D res = Point2D((A.x + B.x) / 2, (A.y + B.y) / 2);
			return res;
		}

		/*Point2D getMid4(Point2D A, Point2D B, Point2D C, Point2D D) {
		return getMid2(getMid2(A,B),getMid2(C,D));
		}*/
	};

	int getMoyeHeight(Point2D A, Point2D B) {
		return (_MatriceHeights[A.x][A.y] + _MatriceHeights[B.x][B.y]) / 2;
	}

	int getRand(int min, int max) {
		if (max < 1) max = 1;
		return rand() % max + min;
	}

	// fonction collision ray
	bool getRayCollision(NYVert3Df & debSegment, NYVert3Df & finSegment,
		NYVert3Df & inter,
		int &xCube, int&yCube, int&zCube)
	{
		float len = (finSegment - debSegment).getSize();

		int x = (int)(debSegment.X / NYCube::CUBE_SIZE);
		int y = (int)(debSegment.Y / NYCube::CUBE_SIZE);
		int z = (int)(debSegment.Z / NYCube::CUBE_SIZE);

		int l = (int)(len / NYCube::CUBE_SIZE) + 1;

		int xDeb = x - l;
		int yDeb = y - l;
		int zDeb = z - l;

		int xFin = x + l;
		int yFin = y + l;
		int zFin = z + l;

		if (xDeb < 0)
			xDeb = 0;
		if (yDeb < 0)
			yDeb = 0;
		if (zDeb < 0)
			zDeb = 0;

		if (xFin >= MAT_SIZE_CUBES)
			xFin = MAT_SIZE_CUBES - 1;
		if (yFin >= MAT_SIZE_CUBES)
			yFin = MAT_SIZE_CUBES - 1;
		if (zFin >= MAT_HEIGHT_CUBES)
			zFin = MAT_HEIGHT_CUBES - 1;

		float minDist = -1;
		NYVert3Df interTmp;
		for (x = xDeb;x <= xFin;x++)
			for (y = yDeb;y <= yFin;y++)
				for (z = zDeb;z <= zFin;z++)
				{
					if (getCube(x, y, z)->isSolid())
					{
						if (getRayCollisionWithCube(debSegment, finSegment, x, y, z, interTmp))
						{
							if ((debSegment - interTmp).getMagnitude() < minDist || minDist == -1)
							{
								minDist = (debSegment - interTmp).getMagnitude();
								inter = interTmp;
								xCube = x;
								yCube = y;
								zCube = z;

							}
						}
					}
				}

		if (minDist != -1)
			return true;

		return false;

	}

	/**
	* De meme cette fonction peut être grandement opitimisée, on a priviligié la clarté
	*/
	bool getRayCollisionWithCube(NYVert3Df & debSegment, NYVert3Df & finSegment,
		int x, int y, int z,
		NYVert3Df & inter)
	{


		float minDist = -1;
		NYVert3Df interTemp;

		//Face1
		if (intersecDroiteCubeFace(debSegment, finSegment,
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face2
		if (intersecDroiteCubeFace(debSegment, finSegment,
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face3
		if (intersecDroiteCubeFace(debSegment, finSegment,
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face4
		if (intersecDroiteCubeFace(debSegment, finSegment,
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face5
		if (intersecDroiteCubeFace(debSegment, finSegment,
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 0)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face6
		if (intersecDroiteCubeFace(debSegment, finSegment,
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 0)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 1)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			NYVert3Df((x + 1)*NYCube::CUBE_SIZE, (y + 0)*NYCube::CUBE_SIZE, (z + 1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getMagnitude();
				inter = interTemp;
			}
		}


		if (minDist < 0)
			return false;

		return true;
	}

	NYWorld()
	{
		_FacteurGeneration = .50;
		//On crée les chunks
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z] = new NYChunk();

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					NYChunk * cxPrev = NULL;
					if(x > 0)
						cxPrev = _Chunks[x-1][y][z];
					NYChunk * cxNext = NULL;
					if(x < MAT_SIZE-1)
						cxNext = _Chunks[x+1][y][z];

					NYChunk * cyPrev = NULL;
					if(y > 0)
						cyPrev = _Chunks[x][y-1][z];
					NYChunk * cyNext = NULL;
					if(y < MAT_SIZE-1)
						cyNext = _Chunks[x][y+1][z];

					NYChunk * czPrev = NULL;
					if(z > 0)
						czPrev = _Chunks[x][y][z-1];
					NYChunk * czNext = NULL;
					if(z < MAT_HEIGHT-1)
						czNext = _Chunks[x][y][z+1];

					_Chunks[x][y][z]->setVoisins(cxPrev,cxNext,cyPrev,cyNext,czPrev,czNext);
				}

		//_TexGrass = NYTexManager::getInstance()->loadTexture(std::string("_minecraft/run/grass.png"));
					
	}

	inline NYCube * getCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * NYChunk::CHUNK_SIZE) x = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * NYChunk::CHUNK_SIZE) y = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE) z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE)-1;

		return &(_Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE]->_Cubes[x % NYChunk::CHUNK_SIZE][y % NYChunk::CHUNK_SIZE][z % NYChunk::CHUNK_SIZE]);
	}

	void updateCube(int x, int y, int z)
	{	
		if (x < 0)x = 0;
		if (y < 0)y = 0;
		if (z < 0)z = 0;
		if (x >= MAT_SIZE * NYChunk::CHUNK_SIZE)x = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (y >= MAT_SIZE * NYChunk::CHUNK_SIZE)y = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE)z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE) - 1;

		NYChunk * chk = _Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE];

		chk->disableHiddenCubes();
		chk->toVbo();

		for (int i = 0;i<6;i++)
			if (chk->Voisins[i])
			{
				chk->Voisins[i]->disableHiddenCubes();
				chk->Voisins[i]->toVbo();
			}
	}

	void deleteCube(int x, int y, int z)
	{
		NYCube * cube = getCube(x, y, z);
		cube->_Draw = false;
		cube->_Type = CUBE_AIR;
		updateCube(x, y, z);
	}

	//Création d'une pile de cubes
	//only if zero permet de ne générer la  pile que si sa hauteur actuelle est de 0 (et ainsi de ne pas regénérer de piles existantes)
	void load_pile(int x, int y, int height, bool onlyIfZero = true)
	{
		if (!onlyIfZero || (_MatriceHeights[x][y] <= 0 && onlyIfZero)) {
			// check des limites 
			if (height < 0) height = 0;
			if (height > MAT_HEIGHT_CUBES-1 ) height = MAT_HEIGHT_CUBES - 1;
			if (x < 0)x = 0;
			if (y < 0)y = 0;
			if (x >= MAT_SIZE * NYChunk::CHUNK_SIZE) x = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
			if (y >= MAT_SIZE * NYChunk::CHUNK_SIZE) y = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;

			int tmpHeight = 0;

			// EAU
			for (tmpHeight;  tmpHeight < altitudeEau; tmpHeight++) {
				getCube(x, y, tmpHeight)->_Type = CUBE_EAU;
			}

			for (tmpHeight; tmpHeight < height; tmpHeight++ ) {
				getCube(x, y, tmpHeight)->_Type = CUBE_TERRE;
			}


			if (getCube(x, y, tmpHeight-1)->_Type != CUBE_EAU) {
				if (tmpHeight >= altitudeNeige) {
					getCube(x, y, tmpHeight)->_Type = CUBE_NEIGE;
				}
				else {
					getCube(x, y, tmpHeight)->_Type = CUBE_HERBE;
				}
				tmpHeight++;
			}

			for (tmpHeight;tmpHeight < MAT_HEIGHT_CUBES-1;tmpHeight++) {
				getCube(x, y, tmpHeight)->_Type = CUBE_AIR;
			}

			if (getRand(0, 1000)  < indiceNuageux) {
				getCube(x, y, tmpHeight)->_Type = CUBE_NEIGE;
			}
			else {
				getCube(x, y, tmpHeight)->_Type = CUBE_AIR;
			}
			

			//getCube(x, y, 0)->_Type = CUBE_EAU;
			//int i = 1;
			//for (i = 1; i < height - 1;i++) {
			//	if (i > altitudeNeige) {
			//		getCube(x, y, i)->_Type = CUBE_NEIGE;
			//	}
			//	else {
			//		getCube(x, y, i)->_Type = CUBE_TERRE;
			//	}
			//	
			//}


			//if (++i > altitudeNeige) {
			//	getCube(x, y, height - 1)->_Type = CUBE_NEIGE;
			//}
			//else {
			//	getCube(x, y, height - 1)->_Type = CUBE_HERBE;
			//}

			//for (i = i;i < MAT_HEIGHT_CUBES;i++) {
			//	getCube(x, y, i)->_Type = CUBE_AIR;
			//}

			//Maj tab hauteur
			_MatriceHeights[x][y] = height;
		}
	
		

	}


	

	void generate_piles(Point2D A, Point2D B, Point2D C, Point2D D,int prof, int progMax = -1 ) {
	
		int rand = getRand(0, (MAT_HEIGHT_CUBES - 1) / (3 * prof));
		int rand2 = getRand(0, (MAT_HEIGHT_CUBES - 1) / (2 * prof));

		Point2D E = A.getMid2(A, B);
		E.moyHeight = getMoyeHeight(A, B);

		Point2D F = A.getMid2(B, C);
		F.moyHeight = getMoyeHeight(B, C);

		Point2D G = A.getMid2(C, D);
		G.moyHeight = getMoyeHeight(C, D);

		Point2D H = A.getMid2(D, A);
		H.moyHeight = getMoyeHeight(D, A);

		load_pile(E.x, E.y, E.moyHeight + (rand - rand2 / 2));
		load_pile(F.x, F.y, F.moyHeight + (rand - rand2 / 2));
		load_pile(G.x, G.y, G.moyHeight + (rand - rand2 / 2));
		load_pile(H.x, H.y, H.moyHeight + (rand - rand2 / 2));

		Point2D I = A.getMid2(E, G);
		I.moyHeight = getMoyeHeight(E, G);
		load_pile(I.x, I.y, I.moyHeight + (rand - rand / 2));

		if ((A.y == E.y && A.x == E.x) || (A.x == H.x && A.y == H.y)) {
			return;
		}

		generate_piles(A, E, I, H, prof + 1);
		generate_piles(E, B, F, I, prof + 1);
		generate_piles(I, F, C, G, prof + 1);
		generate_piles(H, I, G, D, prof + 1);

		

	}

	//Creation du monde entier, en utilisant le mouvement brownien fractionnaire
	void generate_piles(int x1, int y1,
		int x2, int y2, 
		int x3, int y3,
		int x4, int y4, int prof, int profMax = -1)
	{

		generate_piles(Point2D(x1, y1), Point2D(x2, y2), Point2D(x3, y3), Point2D(x4, y4), prof);

		lisse();lisse();lisse();lisse();lisse();lisse();


		//if ((x3 - x1) <= 1 && (y3 - y1) <= 1)
		//	return;

		//int largeurRandom = (int)(MAT_HEIGHT_CUBES / (prof*_FacteurGeneration));
		//if (largeurRandom == 0)
		//	largeurRandom = 1;

		//if (profMax >= 0 && prof >= profMax)
		//{
		//	Log::log(Log::ENGINE_INFO, ("End of generation at prof " + toString(prof)).c_str());
		//	return;
		//}

		////On se met au milieu des deux coins du haut
		//int xa = (x1 + x2) / 2;
		//int ya = (y1 + y2) / 2;
		//int heighta = (_MatriceHeights[x1][y1] + _MatriceHeights[x2][y2]) / 2;
		//if ((x2 - x1)>1)
		//{
		//	heighta += (rand() % largeurRandom) - (largeurRandom / 2);
		//	load_pile(xa, ya, heighta);
		//}
		//else
		//	heighta = _MatriceHeights[xa][ya];

		////Au milieu des deux coins de droite
		//int xb = (x2 + x3) / 2;
		//int yb = (y2 + y3) / 2;
		//int heightb = (_MatriceHeights[x2][y2] + _MatriceHeights[x3][y3]) / 2;
		//if ((y3 - y2)>1)
		//{
		//	heightb += (rand() % largeurRandom) - (largeurRandom / 2);
		//	load_pile(xb, yb, heightb);
		//}
		//else
		//	heightb = _MatriceHeights[xb][yb];

		////Au milieu des deux coins du bas
		//int xc = (x3 + x4) / 2;
		//int yc = (y3 + y4) / 2;
		//int heightc = (_MatriceHeights[x3][y3] + _MatriceHeights[x4][y4]) / 2;
		//heightc += (rand() % largeurRandom) - (largeurRandom / 2);
		//if ((x3 - x4)>1)
		//{
		//	load_pile(xc, yc, heightc);
		//}
		//else
		//	heightc = _MatriceHeights[xc][yc];

		////Au milieu des deux coins de gauche
		//int xd = (x4 + x1) / 2;
		//int yd = (y4 + y1) / 2;
		//int heightd = (_MatriceHeights[x4][y4] + _MatriceHeights[x1][y1]) / 2;
		//heightd += (rand() % largeurRandom) - (largeurRandom / 2);
		//if ((y3 - y1)>1)
		//{
		//	load_pile(xd, yd, heightd);
		//}
		//else
		//	heightd = _MatriceHeights[xd][yd];

		////Au milieu milieu
		//int xe = xa;
		//int ye = yb;
		//if ((x3 - x1)>1 && (y3 - y1)>1)
		//{
		//	int heighte = (heighta + heightb + heightc + heightd) / 4;
		//	heighte += (rand() % largeurRandom) - (largeurRandom / 2);
		//	load_pile(xe, ye, heighte);
		//}

		////On genere les 4 nouveaux quads
		//generate_piles(x1, y1, xa, ya, xe, ye, xd, yd, prof + 1, profMax);
		//generate_piles(xa, ya, x2, y2, xb, yb, xe, ye, prof + 1, profMax);
		//generate_piles(xe, ye, xb, yb, x3, y3, xc, yc, prof + 1, profMax);
		//generate_piles(xd, yd, xe, ye, xc, yc, x4, y4, prof + 1, profMax);

		
	}		


	void troll() {
		int h = 0;
		for (int i = 0; i < MAT_SIZE_CUBES;i++) {
			for (int j = 0; j < MAT_SIZE_CUBES;j++) {
				load_pile(i, j, ((cos(i)+sin(j)) + sin(i)+cos(j) * 8) );
			}
		}
		lisse();
	}
	
	/*void lisseNuage() {
		float nbVoisinsNuages = 0;
		float nbVoisinsTeste = 0;
		bool tmpTabNuages[MAT_SIZE_CUBES][MAT_SIZE_CUBES];

		for (int x = 0; x < MAT_SIZE_CUBES;x++) {
			for (int y = 0; y < MAT_SIZE_CUBES;y++) {
				nbVoisinsNuages = 0;
				nbVoisinsTeste = 0;

				for (int i = 0; i < 2;i++) {
					for (int j = 0; j < 2;j++) {
						if (x > 0 && x < MAT_SIZE_CUBES - 1 && y>0 && y < MAT_SIZE_CUBES - 1) {
							nbVoisinsNuages++;
							if (getCube(x - 1 + i, y - 1 + j, MAT_HEIGHT_CUBES - 1)->_Type == CUBE_NEIGE) {
								nbVoisinsNuages++;
							}
						}
					}
				}

				if (nbVoisinsNuages / nbVoisinsTeste < 0.5) {
					getCube(x - 1 + i, y - 1 + j, MAT_HEIGHT_CUBES - 1)->_Type = CUBE_AIR;
				}
				else {
					getCube(x - 1 + i, y - 1 + j, MAT_HEIGHT_CUBES - 1)->_Type = CUBE_NEIGE;
				}

			}
		}

	}*/

	void lisse(void)
	{
		int tmpMatHeigh[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
		memset(tmpMatHeigh, 0x00, MAT_SIZE_CUBES*MAT_SIZE_CUBES*sizeof(int));
		cout << "start lisse()" << endl;
		int tmpHeight;
		int tmpNbHeight;

		for (int x = 0; x < MAT_SIZE_CUBES;x++) {
			for (int y = 0;y < MAT_SIZE_CUBES;y++) {
				tmpHeight = 0;
				tmpNbHeight = 0;

				//parcours voisins
				for (int i = 0; i < 2;i++) {
					for (int j = 0; j < 2;j++) {
						if (x > 0 && x < MAT_SIZE_CUBES - 1 && y>0 && y < MAT_SIZE_CUBES - 1) {
							tmpHeight += _MatriceHeights[x - 1 + i][y - 1 + j];
							tmpNbHeight++;
						}
					}
				}

				if (tmpNbHeight>0) {
					tmpMatHeigh[x][y] = tmpHeight / tmpNbHeight;
				}
				else {
					tmpMatHeigh[x][y] = _MatriceHeights[x][y];
				}

				
			}
			
		}

		for (int a = 0; a < MAT_SIZE_CUBES; a++) {
			for (int b= 0; b < MAT_SIZE_CUBES; b++) {
				load_pile(a, b, tmpMatHeigh[a][b],false);
			}
		}


	}

	


	void init_world(int profmax = -1)
	{
		_cprintf("Creation du monde %f \n",_FacteurGeneration);

		srand(time(NULL));
		//srand(1818);

		//Reset du monde
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z]->reset();
		memset(_MatriceHeights,0x00,MAT_SIZE_CUBES*MAT_SIZE_CUBES*sizeof(int));

		//On charge les 4 coins
		/*load_pile(0,0,MAT_HEIGHT_CUBES/2);
		load_pile(MAT_SIZE_CUBES-1,0,MAT_HEIGHT_CUBES/2);
		load_pile(MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,MAT_HEIGHT_CUBES/2);	
		load_pile(0,MAT_SIZE_CUBES-1,MAT_HEIGHT_CUBES/2);*/
		
		load_pile(0,0,getRand(0, MAT_HEIGHT_CUBES));
		load_pile(MAT_SIZE_CUBES-1,0, getRand(0, MAT_HEIGHT_CUBES));
		load_pile(MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,  getRand(0, MAT_HEIGHT_CUBES));
		load_pile(0,MAT_SIZE_CUBES-1,getRand(0, MAT_HEIGHT_CUBES ));

		//On génère a partir des 4 coins
		generate_piles(0,0,
			MAT_SIZE_CUBES-1,0,
			MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,
			0,MAT_SIZE_CUBES-1,2,profmax);

	
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z]->disableHiddenCubes();

		add_world_to_vbo();
	}

	NYCube * pick(NYVert3Df  pos, NYVert3Df  dir, NYPoint3D * point)
	{

		return NULL;
	}

	//Boites de collisions plus petites que deux cubes
	NYAxis getMinCol(NYVert3Df pos, float width, float height, float & valueColMin)
	{

		int x = (int)(pos.X / NYCube::CUBE_SIZE);
		int y = (int)(pos.Y / NYCube::CUBE_SIZE);
		int z = (int)(pos.Z / NYCube::CUBE_SIZE);

		int xNext = (int)((pos.X + width / 2.0f) / NYCube::CUBE_SIZE);
		int yNext = (int)((pos.Y + width / 2.0f) / NYCube::CUBE_SIZE);
		int zNext = (int)((pos.Z + height / 2.0f) / NYCube::CUBE_SIZE);

		int xPrev = (int)((pos.X - width / 2.0f) / NYCube::CUBE_SIZE);
		int yPrev = (int)((pos.Y - width / 2.0f) / NYCube::CUBE_SIZE);
		int zPrev = (int)((pos.Z - height / 2.0f) / NYCube::CUBE_SIZE);

		if (x < 0)	x = 0;
		if (y < 0)	y = 0;
		if (z < 0)	z = 0;

		if (xPrev < 0)	xPrev = 0;
		if (yPrev < 0)	yPrev = 0;
		if (zPrev < 0)	zPrev = 0;

		if (xNext < 0)	xNext = 0;
		if (yNext < 0)	yNext = 0;
		if (zNext < 0)	zNext = 0;

		if (x >= MAT_SIZE_CUBES)	x = MAT_SIZE_CUBES - 1;
		if (y >= MAT_SIZE_CUBES)	y = MAT_SIZE_CUBES - 1;
		if (z >= MAT_HEIGHT_CUBES)	z = MAT_HEIGHT_CUBES - 1;

		if (xPrev >= MAT_SIZE_CUBES)	xPrev = MAT_SIZE_CUBES - 1;
		if (yPrev >= MAT_SIZE_CUBES)	yPrev = MAT_SIZE_CUBES - 1;
		if (zPrev >= MAT_HEIGHT_CUBES)	zPrev = MAT_HEIGHT_CUBES - 1;

		if (xNext >= MAT_SIZE_CUBES)	xNext = MAT_SIZE_CUBES - 1;
		if (yNext >= MAT_SIZE_CUBES)	yNext = MAT_SIZE_CUBES - 1;
		if (zNext >= MAT_HEIGHT_CUBES)	zNext = MAT_HEIGHT_CUBES - 1;

		//On fait chaque axe
		NYAxis axis = 0x00;
		valueColMin = 10000.0f;

		//On verif tout les 4 angles de gauche
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid())
		{
			float depassement = ((xPrev + 1) * NYCube::CUBE_SIZE) - (pos.X - width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_X;
			}
		}

		float depassementx2 = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);

		//On verif tout les 4 angles de droite
		if (getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_X;
			}
		}

		float depassementy1 = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);

		//On verif tout les 4 angles de devant
		if (getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Y;
			}
		}

		float depassementy2 = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);

		//On verif tout les 4 angles de derriere
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid())
		{
			float depassement = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Y;
			}
		}

		//On verif tout les 4 angles du haut
		if (getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (zNext * NYCube::CUBE_SIZE) - (pos.Z + height / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Z;
			}
		}

		//On verif tout les 4 angles du bas
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid())
		{
			float depassement = ((zPrev + 1) * NYCube::CUBE_SIZE) - (pos.Z - height / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Z;
			}
		}

		return axis;
	}




	void render_world_vbo(void)
	{
	
		
		/*glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _TexGrass->Texture);*/

		for (int x = 0;x<MAT_SIZE;x++)
			for (int y = 0;y<MAT_SIZE;y++)
				for (int z = 0;z<MAT_HEIGHT;z++)
				{
					glPushMatrix();
					glTranslatef((float)(x*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE), (float)(y*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE), (float)(z*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE));
					_Chunks[x][y][z]->render();
					glPopMatrix();
				}

	}

	void add_world_to_vbo(void)
	{
		int totalNbVertices = 0;
		
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					_Chunks[x][y][z]->toVbo();
					totalNbVertices += _Chunks[x][y][z]->_NbVertices;
				}

		Log::log(Log::ENGINE_INFO,(toString(totalNbVertices) + " vertices in VBO").c_str());
	}

	void render_world_old_school(void)
	{
		GLfloat cubeMaterialDiffuse[] = { 0.0, 0.0, 0.0,1.0 };
		GLfloat materialAmbient[] = { 0.3, 0.3, 0.3, 1.0 };
		GLfloat whiteSpecularMaterial[] = { 0.3, 0.3, 0.3,0.8 };


		for (int i = 0; i <= MAT_SIZE_CUBES; i++) {
			for (int j = 0; j <= MAT_SIZE_CUBES; j++) {
				for (int k = 0; k <= MAT_HEIGHT_CUBES; k++) {
					glPushMatrix();
					if (getCube(i, j, k)->_Type == CUBE_HERBE ) {
						// Ambient
						materialAmbient[0] = 0.2;
						materialAmbient[1] = 0.8;
						materialAmbient[2] = 0.0;
						materialAmbient[3] = 1.0;

						//Specular White
						whiteSpecularMaterial[0] = 0.3;
						whiteSpecularMaterial[1] = 0.3;
						whiteSpecularMaterial[2] = 0.3;
						whiteSpecularMaterial[3] = 0.0;
						

						//Diffuse
						cubeMaterialDiffuse[0] = 0.5f;
						cubeMaterialDiffuse[1] = 1.0f;
						cubeMaterialDiffuse[2] = 0.0f;
						cubeMaterialDiffuse[3] = 1.0f;
						mShininess = 0;

						glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, cubeMaterialDiffuse);
						glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
						glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

						glTranslatef(i, j, k);
						glutSolidCube(1);
						glPopMatrix();

					}
					
					else if	(getCube(i, j, k)->_Type == CUBE_EAU) {
						// Ambient
						materialAmbient[0] = 0.3;
						materialAmbient[1] = 0.3;
						materialAmbient[2] = 0.3;
						materialAmbient[3] = 1.0;

						//Specular White
						whiteSpecularMaterial[0] = 0.3;
						whiteSpecularMaterial[1] = 0.3;
						whiteSpecularMaterial[2] = 0.3;
						whiteSpecularMaterial[3] = 0.3;

						//Diffuse
						cubeMaterialDiffuse[0] = 0.0f;
						cubeMaterialDiffuse[1] = 0.0f;
						cubeMaterialDiffuse[2] = 1.0f;
						cubeMaterialDiffuse[3] = 1.0f;
						mShininess = 80;

						glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, cubeMaterialDiffuse);
						glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
						glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

						glTranslatef(i, j, k);
						glutSolidCube(1);
						glPopMatrix();

					}
					else if (getCube(i, j, k)->_Type == CUBE_TERRE) {
						// Ambient
						materialAmbient[0] = 0.3;
						materialAmbient[1] = 0.3;
						materialAmbient[2] = 0.3;
						materialAmbient[3] = 1.0;

						//Specular White
						whiteSpecularMaterial[0] = 0;
						whiteSpecularMaterial[1] = 0;
						whiteSpecularMaterial[2] = 0;
						whiteSpecularMaterial[3] = 0;

						//Diffuse
						cubeMaterialDiffuse[0] = 0.6f;
						cubeMaterialDiffuse[1] = 0.25f;
						cubeMaterialDiffuse[2] = 0.25f;
						cubeMaterialDiffuse[3] = 1.0f;
						mShininess = 0;

						glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, cubeMaterialDiffuse);
						glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
						glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

						glTranslatef(i, j, k);
						glutSolidCube(1);
						glPopMatrix();

					}
					else if (getCube(i, j, k)->_Type == CUBE_NEIGE) {
						// Ambient
						materialAmbient[0] = 0.8;
						materialAmbient[1] = 0.8;
						materialAmbient[2] = 0.8;
						materialAmbient[3] = 1.0;

						//Specular White
						whiteSpecularMaterial[0] = 0.0f;
						whiteSpecularMaterial[1] = 0.0f;
						whiteSpecularMaterial[2] = 0.0f;
						whiteSpecularMaterial[3] = 1.0f;

						//Diffuse
						cubeMaterialDiffuse[0] = 0.8f;
						cubeMaterialDiffuse[1] = 0.8f;
						cubeMaterialDiffuse[2] = 0.8f;
						cubeMaterialDiffuse[3] = 1.0f;
						mShininess = 0;

						glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, cubeMaterialDiffuse);
						glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
						glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

						glTranslatef(i, j, k);
						glutSolidCube(1);
						glPopMatrix();

					}
					else {}

						/*if (getCube(i, j, k)->_Type == CUBE_AIR) {
							// Ambient
							materialAmbient[0] = 0;
							materialAmbient[1] = 0;
							materialAmbient[2] = 0;
							materialAmbient[3] = 0;

							//Specular White
							whiteSpecularMaterial[0] = 0;
							whiteSpecularMaterial[1] = 0;
							whiteSpecularMaterial[2] = 0;
							whiteSpecularMaterial[3] = 0;

							//Diffuse
							cubeMaterialDiffuse[0] = 0.0f;
							cubeMaterialDiffuse[1] = 0.0f;
							cubeMaterialDiffuse[2] = 0.0f;
							cubeMaterialDiffuse[3] = 0.0f;
							mShininess = 0;

							glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
							glMaterialfv(GL_FRONT, GL_DIFFUSE, cubeMaterialDiffuse);
							glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
							glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

						}*/
					

					
				}
			}
		}
	}	
};



#endif