#pragma once

#include "engine/render/renderer.h"
#include "engine/utils/types_3d.h"
#include "cube.h"

class MyPhysic
{
	public : 
		
		MyPhysic()
		{

		}

		bool getIntersection(NYVert3Df A, NYVert3Df B, NYVert3Df pA, NYVert3Df pB, NYVert3Df pC)
		{
			bool inter = false;
			NYVert3Df vectpApB = NYVert3Df(pB.X - pA.X, pB.Y - pA.Y, pB.Z - pA.Z);
			NYVert3Df vectpApC = NYVert3Df(pC.X - pA.X, pC.Y - pA.Y, pC.Z - pA.Z);
			NYVert3Df vectNorm = vectpApB.vecProd(vectpApC);
			float d = - (pA.X*vectNorm.X + pA.Y*vectNorm.Y + pA.Z*vectNorm.Z);
			if (vectNorm.X*(B.X - A.X) + vectNorm.Y*(B.Y - A.Y) + vectNorm.Z*(B.Z - A.Z) != 0)
			{
				float t = -(d + vectNorm.X*A.X + vectNorm.Y*A.Y + vectNorm.Z*A.Z) / vectNorm.X*(B.X - A.X) + vectNorm.Y*(B.Y - A.Y) + vectNorm.Z*(B.Z - A.Z);
				NYVert3Df intersec = NYVert3Df(A.X + (B.X - A.X)*t, A.Y + (B.Y - A.Y)*t, A.Z + (B.Z - A.Z)*t);
				inter = true;
			}
			return inter;
		}


};