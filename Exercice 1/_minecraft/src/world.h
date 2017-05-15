#ifndef __WORLD_H__
#define __WORLD_H__

#include "gl/glew.h"
#include "gl/freeglut.h" 
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "chunk.h"
#include "time.h"
#include "my_physic.h"

typedef uint8 NYAxis;
#define NY_AXIS_X 0x01
#define NY_AXIS_Y 0x02
#define NY_AXIS_Z 0x04

#define MAT_SIZE 12 //en nombre de chunks
#define MAT_HEIGHT 10 //en nombre de chunks
#define MAT_SIZE_CUBES (MAT_SIZE * NYChunk::CHUNK_SIZE)
#define MAT_HEIGHT_CUBES (MAT_HEIGHT * NYChunk::CHUNK_SIZE)


class NYWorld
{
public :
	NYChunk * _Chunks[MAT_SIZE][MAT_SIZE][MAT_HEIGHT];
	int _MatriceHeights[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	int _MatriceHeightsTmp[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	float _FacteurGeneration;
	int _waterLevel = 45;

	NYWorld()
	{
		_FacteurGeneration = 1.0;

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

					
	}

	inline bool intersecDroitePlan(NYVert3Df & debSegment, NYVert3Df & finSegment,
		NYVert3Df & p1Plan, NYVert3Df & p2Plan, NYVert3Df & p3Plan,
		NYVert3Df & inter)
	{
		//Equation du plan :
		NYVert3Df nrmlAuPlan = (p1Plan - p2Plan).vecProd(p3Plan - p2Plan); //On a les a,b,c du ax+by+cz+d = 0
		float d = -(nrmlAuPlan.X * p2Plan.X + nrmlAuPlan.Y * p2Plan.Y + nrmlAuPlan.Z* p2Plan.Z); //On remarque que c'est un produit scalaire...

																								 //Equation de droite :
		NYVert3Df dirDroite = finSegment - debSegment;

		//On resout l'équation de plan
		float nominateur = -d - nrmlAuPlan.X * debSegment.X - nrmlAuPlan.Y * debSegment.Y - nrmlAuPlan.Z * debSegment.Z;
		float denominateur = nrmlAuPlan.X * dirDroite.X + nrmlAuPlan.Y * dirDroite.Y + nrmlAuPlan.Z * dirDroite.Z;

		if (denominateur == 0)
			return false;

		//Calcul de l'intersection
		float t = nominateur / denominateur;
		inter = debSegment + (dirDroite*t);

		//Si point avant le debut du segment
		if (t < 0 || t > 1)
			return false;

		return true;
	}

	inline bool intersecDroiteCubeFace(NYVert3Df & debSegment, NYVert3Df & finSegment,
		NYVert3Df & p1, NYVert3Df & p2, NYVert3Df & p3, NYVert3Df & p4,
		NYVert3Df & inter)
	{
		//On calcule l'intersection
		bool res = intersecDroitePlan(debSegment, finSegment, p1, p2, p4, inter);

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
		for (x = xDeb; x <= xFin; x++)
			for (y = yDeb; y <= yFin; y++)
				for (z = zDeb; z <= zFin; z++)
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

	void rayCast(NYVert3Df dir, NYVert3Df pos)
	{
		NYVert3Df myPos = pos;
		NYVert3Df rayPos = myPos;
		int size = NYCube::CUBE_SIZE;
		int distMax = 10;
		bool found = false;
		int cpt = 0;
		while (!found || cpt < distMax)
		{
			if (getCube(rayPos.X/size, rayPos.Y/size, rayPos.Z/size)->_Type != CUBE_AIR)
			{
				found = true;
			}
			rayPos = rayPos + dir;
			++cpt;
		}
		if (found)
		{
			deleteCube(rayPos.X, rayPos.Y, rayPos.Z);
		}
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

		for (int i = 0; i<6; i++)
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
		if (height < 1)
			height = 1;
		if (height >= MAT_HEIGHT_CUBES)
			height = MAT_HEIGHT_CUBES - 1;

		_MatriceHeights[x][y] = height;
		if ((onlyIfZero && _MatriceHeights[x][y] == 0) || !onlyIfZero)
		{
			for (int i = 0; i <= _waterLevel; ++i)
			{
				getCube(x, y, i)->_Draw = true;
				getCube(x, y, i)->_Type = CUBE_EAU;
			}
			if (height > _waterLevel)
			{
				for (int i = _waterLevel + 1; i < height; ++i)
				{
					getCube(x, y, i)->_Draw = true;
					getCube(x, y, i)->_Type = CUBE_TERRE;
				}
				getCube(x, y, height)->_Draw = true;
				getCube(x, y, height)->_Type = CUBE_HERBE;

				for (int z = height + 1; z < MAT_HEIGHT_CUBES; z++)
				{
					getCube(x, y, z)->_Draw = true;
					getCube(x, y, z)->_Type = CUBE_AIR;
				}
			}
		}
	}

	//Creation du monde entier, en utilisant le mouvement brownien fractionnaire
	void generate_piles(int x1, int y1,
		int x2, int y2, 
		int x3, int y3,
		int x4, int y4, int prof, int profMax = -1)
	{
		if ((x2 - x1) <= 1 && (y4 - y1) <= 1)
		{
			return;
		}
		else
		{

			int x5, x6, x7, x8, x9, y5, y6, y7, y8, y9;
			float h5, h6, h7, h8, h9;

			h9 = (_MatriceHeights[x1][y1] + _MatriceHeights[x3][y3]) / 2 + (MAT_HEIGHT_CUBES / 2 * (randf() - 0.5) / prof);
			x9 = (x1 + x3) / 2;
			y9 = (y1 + y3) / 2;
			_MatriceHeights[x9][y9] = h9;
			load_pile(x9, y9, h9, true);

			h5 = (_MatriceHeights[x1][y1] + _MatriceHeights[x2][y2] + _MatriceHeights[x9][y9]) / 3 + (MAT_HEIGHT_CUBES / 2 * (randf() - 0.5) / prof);
			x5 = (x1 + x2) / 2;
			y5 = (y1 + y2) / 2;
			_MatriceHeights[x5][y5] = h5;
			load_pile(x5, y5, h5, true);

			h6 = (_MatriceHeights[x2][y2] + _MatriceHeights[x3][y3] + _MatriceHeights[x9][y9]) / 3 + (MAT_HEIGHT_CUBES / 2 * (randf() - 0.5) / prof);
			x6 = (x2 + x3) / 2;
			y6 = (y2 + y3) / 2;
			_MatriceHeights[x6][y6] = h6;
			load_pile(x6, y6, h6, true);

			h7 = (_MatriceHeights[x3][y3] + _MatriceHeights[x4][y4] + _MatriceHeights[x9][y9]) / 3 + (MAT_HEIGHT_CUBES / 2 * (randf() - 0.5) / prof);
			x7 = (x3 + x4) / 2;
			y7 = (y3 + y4) / 2;
			_MatriceHeights[x7][y7] = h7;
			load_pile(x7, y7, h7, true);

			h8 = (_MatriceHeights[x1][y1] + _MatriceHeights[x4][y4] + _MatriceHeights[x9][y9]) / 3 + (MAT_HEIGHT_CUBES / 2 * (randf() - 0.5) / prof);
			x8 = (x1 + x4) / 2;
			y8 = (y1 + y4) / 2;
			_MatriceHeights[x8][y8] = h8;
			load_pile(x8, y8, h8, true);

			prof++;

			generate_piles(x1, y1, x5, y5, x9, y9, x8, y8, prof, profMax = -1);
			generate_piles(x5, y5, x2, y2, x6, y6, x9, y9, prof, profMax = -1);
			generate_piles(x8, y8, x9, y9, x7, y7, x4, y4, prof, profMax = -1);
			generate_piles(x9, y9, x6, y6, x3, y3, x7, y7, prof, profMax = -1);

		}
	}


	//On utilise un matrice temporaire _MatriceHeightsTmp à déclarer
	//Penser à appeler la fonction a la fin de la génération (plusieurs fois si besoin)
	void lisse(void)
	{
		int sizeWidow = 4;
		memset(_MatriceHeightsTmp, 0x00, sizeof(int)*MAT_SIZE_CUBES*MAT_SIZE_CUBES);
		for (int x = 0; x<MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y<MAT_SIZE_CUBES; y++)
			{
				//on moyenne sur une distance
				int nb = 0;
				for (int i = (x - sizeWidow < 0 ? 0 : x - sizeWidow);
					i < (x + sizeWidow >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES - 1 : x + sizeWidow); i++)
				{
					for (int j = (y - sizeWidow < 0 ? 0 : y - sizeWidow);
						j <(y + sizeWidow >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES - 1 : y + sizeWidow); j++)
					{
						_MatriceHeightsTmp[x][y] += _MatriceHeights[i][j];
						nb++;
					}
				}
				if (nb)
					_MatriceHeightsTmp[x][y] /= nb;
			}
		}

		//On reset les piles
		for (int x = 0; x<MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y<MAT_SIZE_CUBES; y++)
			{
				load_pile(x, y, _MatriceHeightsTmp[x][y], false);
			}
		}


	}

	//Boites de collisions plus petites que deux cubes
	NYAxis getMinCol(NYVert3Df pos, NYVert3Df dir, float width, float height, float & valueColMin, bool oneShot)
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
		valueColMin = oneShot ? 0.5 : 10000.0f;
		float seuil = 0.00001;
		float prodScalMin = 1.0f;
		if (dir.getMagnitude() > 1)
			dir.normalize();

		//On verif tout les 4 angles de gauche
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev + 1, yPrev, zPrev)->isSolid() ||
				getCube(xPrev + 1, yPrev, zNext)->isSolid() ||
				getCube(xPrev + 1, yNext, zPrev)->isSolid() ||
				getCube(xPrev + 1, yNext, zNext)->isSolid()) || !oneShot)
			{
				float depassement = ((xPrev + 1) * NYCube::CUBE_SIZE) - (pos.X - width / 2.0f);
				float prodScal = abs(dir.X);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = NY_AXIS_X;
					}
			}
		}

		//float depassementx2 = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);

		//On verif tout les 4 angles de droite
		if (getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xNext - 1, yPrev, zPrev)->isSolid() ||
				getCube(xNext - 1, yPrev, zNext)->isSolid() ||
				getCube(xNext - 1, yNext, zPrev)->isSolid() ||
				getCube(xNext - 1, yNext, zNext)->isSolid()) || !oneShot)
			{
				float depassement = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);
				float prodScal = abs(dir.X);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = NY_AXIS_X;
					}
			}
		}

		//float depassementy1 = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);

		//On verif tout les 4 angles de devant
		if (getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yNext - 1, zPrev)->isSolid() ||
				getCube(xPrev, yNext - 1, zNext)->isSolid() ||
				getCube(xNext, yNext - 1, zPrev)->isSolid() ||
				getCube(xNext, yNext - 1, zNext)->isSolid()) || !oneShot)
			{
				float depassement = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);
				float prodScal = abs(dir.Y);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = NY_AXIS_Y;
					}
			}
		}

		//float depassementy2 = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);

		//On verif tout les 4 angles de derriere
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev + 1, zPrev)->isSolid() ||
				getCube(xPrev, yPrev + 1, zNext)->isSolid() ||
				getCube(xNext, yPrev + 1, zPrev)->isSolid() ||
				getCube(xNext, yPrev + 1, zNext)->isSolid()) || !oneShot)
			{
				float depassement = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);
				float prodScal = abs(dir.Y);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = NY_AXIS_Y;
					}
			}
		}

		//On verif tout les 4 angles du haut
		if (getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev, zNext - 1)->isSolid() ||
				getCube(xPrev, yNext, zNext - 1)->isSolid() ||
				getCube(xNext, yPrev, zNext - 1)->isSolid() ||
				getCube(xNext, yNext, zNext - 1)->isSolid()) || !oneShot)
			{
				float depassement = (zNext * NYCube::CUBE_SIZE) - (pos.Z + height / 2.0f);
				float prodScal = abs(dir.Z);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = NY_AXIS_Z;
					}
			}
		}

		//On verif tout les 4 angles du bas
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev, zPrev + 1)->isSolid() ||
				getCube(xPrev, yNext, zPrev + 1)->isSolid() ||
				getCube(xNext, yPrev, zPrev + 1)->isSolid() ||
				getCube(xNext, yNext, zPrev + 1)->isSolid()) || !oneShot)
			{
				float depassement = ((zPrev + 1) * NYCube::CUBE_SIZE) - (pos.Z - height / 2.0f);
				float prodScal = abs(dir.Z);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = NY_AXIS_Z;
					}
			}
		}



		return axis;
	}


	void init_world(int profmax = -1)
	{
		_cprintf("Creation du monde %f \n",_FacteurGeneration);

		srand(time(NULL));

		//Reset du monde
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z]->reset();
		memset(_MatriceHeights,0x00,MAT_SIZE_CUBES*MAT_SIZE_CUBES*sizeof(int));

		//On charge les 4 coins
		load_pile(0,0,MAT_HEIGHT_CUBES/2);
		load_pile(MAT_SIZE_CUBES-1,0,MAT_HEIGHT_CUBES/2);
		load_pile(MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,MAT_HEIGHT_CUBES/2);	
		load_pile(0,MAT_SIZE_CUBES-1,MAT_HEIGHT_CUBES/2);

		//On génère a partir des 4 coins
		generate_piles(0,0,
			MAT_SIZE_CUBES-1,0,
			MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,
			0,MAT_SIZE_CUBES-1,1,profmax);	
		lisse();

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
	NYAxis getMinCol(NYVert3Df pos, float width, float height, float & valueColMin, int i)
	{
		NYAxis axis = 0x00;
		return axis;
	}


	void render_world_vbo(void)
	{
		//glEnable(GL_COLOR_MATERIAL);
		//glEnable(GL_LIGHTING);

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					glPushMatrix();
					glTranslatef((float)(x*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE),(float)(y*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE),(float)(z*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE));
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
		//Diffuse
		GLfloat materialDiffuse[] = { 0, 0, 0.8, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

		//Speculaire
		GLfloat whiteSpecularMaterialZ[] = { 0.3, 0.3, 0.3,1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterialZ);
		float mShininess = 100;
		glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

		//Ambient
		GLfloat materialAmbient[] = { 0, 0, 0.2, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

		//Emissive
		GLfloat emissive[] = { 0.1, 0.1, 0.1,1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, emissive);

		for (int x = 0; x < MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y < MAT_SIZE_CUBES; y++)
			{
				for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
				{
					NYCube * cube = getCube(x, y, z);
					if (cube->_Draw && cube->_Type != CUBE_AIR)
					{
						if (cube->_Type == CUBE_EAU)
						{
							materialDiffuse[0] = 0;
							materialDiffuse[1] = 102.0f/255;
							materialDiffuse[2] = 1.0f;
							glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
							materialAmbient[0] = 0;
							materialAmbient[1] = 102.0f /255*0.1;
							materialAmbient[2] = 0.1f;
							glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						}

						if (cube->_Type == CUBE_TERRE)
						{
							materialDiffuse[0] = 102.0f/255;
							materialDiffuse[1] = 51.0f/255;
							materialDiffuse[2] = 0;
							glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
							materialAmbient[0] = 102.0f / 255 * 0.1;
							materialAmbient[1] = 51.0f / 255 * 0.1;
							materialAmbient[2] = 0;
							glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						}

						if (cube->_Type == CUBE_HERBE)
						{
							materialDiffuse[0] = 0;
							materialDiffuse[1] = 102.0f/255;
							materialDiffuse[2] = 0;
							glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
							materialAmbient[0] = 0;
							materialAmbient[1] = 102.0f/255 * 0.1;
							materialAmbient[2] = 0;
							glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						}

						int size = NYCube::CUBE_SIZE;
						glPushMatrix();
						glTranslatef(x*size, y*size, z*size);
						glutSolidCube(size);
						glPopMatrix();
					}
				}
			}
		}		
	}	
};



#endif