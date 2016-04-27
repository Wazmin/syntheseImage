#ifndef __AVATAR__
#define __AVATAR__

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "world.h"

class NYAvatar
{
	public :
		NYVert3Df Position;
		NYVert3Df nextPos;
		NYVert3Df Speed;

		NYVert3Df p1;
		NYVert3Df p2;
		NYVert3Df p3;
		NYVert3Df p4;
		//son milieu
		NYVert3Df p5;

		// Face Haut
		NYVert3Df p6;
		NYVert3Df p7;
		NYVert3Df p8;
		NYVert3Df p9;
		NYVert3Df p10;

		NYVert3Df MoveDir;
		bool Move;
		bool Jump;
		float Height;
		float Width;
		float factGrav = 1;
		bool avance;
		bool recule;
		bool gauche;
		bool droite;
		bool Standing;

		NYCamera * Cam;
		NYWorld * World;

		NYAvatar(NYCamera * cam,NYWorld * world)
		{
			Position = NYVert3Df(0,0,0);
			Height = 10;
			Width = 3;
			Cam = cam;
			avance = false;
			recule = false;
			gauche = false;
			droite = false;
			Standing = false;
			Jump = false;
			World = world;
		}


		void render(void)
		{
			glDisable(GL_LIGHTING);
			glColor3f(1.0,0.0,0.0);
			glTranslatef(Position.X, Position.Y, Position.Z);
			glutSolidCube(Width);
		}

		void update(float elapsed)
		{
			//Par defaut, on applique la gravité (-100 sur Z)
			NYVert3Df force = NYVert3Df(0, 0, -1) * 3.0f;

			//Si l'avatar n'est pas au sol, alors il ne peut pas sauter
			if (!Standing)
				//Jump = false;


			//Si il est au sol, on applique les controles "ground"
			if (Standing)
			{
				if (avance)
					force += Cam->_Direction * 400;
				if (recule)
					force += Cam->_Direction * -400;
				if (gauche)
					force += Cam->_NormVec * -400;
				if (droite)
					force += Cam->_NormVec * 400;
			}
			else //Si il est en l'air, c'est du air control
			{
				if (avance)
					force += Cam->_Direction * 50;
				if (recule)
					force += Cam->_Direction * -50;
				if (gauche)
					force += Cam->_NormVec * -50;
				if (droite)
					force += Cam->_NormVec * 50;
			}

			//On applique le jump
			if (Jump)
			{
				force += NYVert3Df(0, 0, 1) * 5.0f / elapsed; //(impulsion, pas fonction du temps)
				//Jump = false;
			}

			//On applique les forces en fonction du temps écoulé
			Speed += force * elapsed;

			//On met une limite a sa vitesse horizontale
			NYVert3Df horSpeed = Speed;
			horSpeed.Z = 0;
			if (horSpeed.getSize() > 70.0f)
			{
				horSpeed.normalize();
				horSpeed *= 70.0f;
				Speed.X = horSpeed.X;
				Speed.Y = horSpeed.Y;
			}

			//On le déplace, en sauvegardant son ancienne position
			NYVert3Df oldPosition = Position;
			Position += (Speed * elapsed);

			Standing = false;

			for (int i = 0; i < 3; i++)
			{
				float valueColMin = 0;
				NYAxis axis = World->getMinCol(Position, Width, Height, valueColMin);
				if (axis != 0)
				{
					valueColMin = max(abs(valueColMin), 0.0001f) * (valueColMin > 0 ? 1.0f : -1.0f);
					if (axis & NY_AXIS_X)
					{
						Position.X += valueColMin;
						Speed.X = 0;
					}
					if (axis & NY_AXIS_Y)
					{
						Position.Y += valueColMin;
						Speed.Y = 0;
					}
					if (axis & NY_AXIS_Z)
					{
						Speed.Z = 0;
						Position.Z += valueColMin;
						Speed *= pow(0.01f, elapsed);
						Standing = true;
					}
				}
			}
		}

};

#endif