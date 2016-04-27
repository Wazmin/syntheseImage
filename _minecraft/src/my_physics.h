#pragma once

#include "engine/utils/types_3d.h"

inline bool intersecPlanSeg(NYVert3Df& segA, NYVert3Df& segB, NYVert3Df& p1Plan,
	NYVert3Df& p2Plan, NYVert3Df& p3Plan, NYVert3Df& pointIntersec) {
	//equation plan

	NYVert3Df vecNormPlan = (p1Plan - p2Plan).vecProd(p3Plan-p2Plan);
	float d = -(p1Plan.X * vecNormPlan.X + p1Plan.Y * vecNormPlan.Y + p1Plan.Z * vecNormPlan.Z);

	NYVert3Df vecDirDroite = segB - segA;
	
	float numerateur = -d - vecNormPlan.X * segA.X - vecNormPlan.Y * segA.Y - vecNormPlan.Z * segA.Z;
	float denominateur = vecNormPlan.X *vecDirDroite.X + vecNormPlan.Y * vecDirDroite.Y + vecNormPlan.Z * vecDirDroite.Z;

	if (denominateur == 0) return false;

	float t = numerateur / denominateur;
	pointIntersec = segA +  vecDirDroite * t;

	if (t < 0 || t>1) return false;

	return true;
}

/**
* Attention ce code n'est pas optimal, il est compréhensible. Il existe de nombreuses
* versions optimisées de ce calcul. Il faut donner les points dans l'ordre (CW ou CCW)
*/
inline bool intersecDroiteCubeFace(NYVert3Df & debSegment, NYVert3Df & finSegment,
	NYVert3Df & p1, NYVert3Df & p2, NYVert3Df & p3, NYVert3Df & p4,
	NYVert3Df & inter)
{
	//On calcule l'intersection
	bool res = intersecPlanSeg(debSegment, finSegment, p1, p2, p4, inter);

	if (!res)
		return false;

	//On fait les produits vectoriels
	NYVert3Df v1 = p2 - p1;
	NYVert3Df v2 = p3 - p2;
	NYVert3Df v3 = p4 - p3;
	NYVert3Df v4 = p1 - p4;

	NYVert3Df n1 = v1.vecProd(inter - p1);
	NYVert3Df n2 = v2.vecProd(inter - p2);
	NYVert3Df n3 = v3.vecProd(inter - p3);
	NYVert3Df n4 = v4.vecProd(inter - p4);

	//on compare le signe des produits scalaires
	float ps1 = n1.scalProd(n2);
	float ps2 = n2.scalProd(n3);
	float ps3 = n3.scalProd(n4);

	if (ps1 >= 0 && ps2 >= 0 && ps3 >= 0)
		return true;

	return false;


}