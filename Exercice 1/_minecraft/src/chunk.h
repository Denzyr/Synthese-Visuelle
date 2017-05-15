#pragma once

#include "engine/render/renderer.h"
#include "cube.h"

/**
  * On utilise des chunks pour que si on modifie juste un cube, on ait pas
  * besoin de recharger toute la carte dans le buffer, mais juste le chunk en question
  */
class NYChunk
{
	public :

		static const int CHUNK_SIZE = 16; ///< Taille d'un chunk en nombre de cubes (n*n*n)
		NYCube _Cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]; ///< Cubes contenus dans le chunk

		GLuint _BufWorld; ///< Identifiant du VBO pour le monde
		
		static float _WorldVert[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*4*6]; ///< Buffer pour les sommets
		static float _WorldCols[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*4*6]; ///< Buffer pour les couleurs
		static float _WorldNorm[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*4*6]; ///< Buffer pour les normales

		static const int SIZE_VERTICE = 3 * sizeof(float); ///< Taille en octets d'un vertex dans le VBO
		static const int SIZE_COLOR = 3 * sizeof(float);  ///< Taille d'une couleur dans le VBO
		static const int SIZE_NORMAL = 3 * sizeof(float);  ///< Taille d'une normale dans le VBO
		
		int _NbVertices; ///< Nombre de vertices dans le VBO (on ne met que les faces visibles)

		NYChunk * Voisins[6];
		
		NYChunk()
		{
			_BufWorld = 0;
			memset(Voisins,0x00,sizeof(void*) * 6);

			//On le detruit si il existe deja
			if (_BufWorld != 0)
				glDeleteBuffers(1, &_BufWorld);

			//Genere un identifiant
			glGenBuffers(1, &_BufWorld);
		}

		void setVoisins(NYChunk * xprev, NYChunk * xnext,NYChunk * yprev,NYChunk * ynext,NYChunk * zprev,NYChunk * znext)
		{
			Voisins[0] = xprev;
			Voisins[1] = xnext;
			Voisins[2] = yprev;
			Voisins[3] = ynext;
			Voisins[4] = zprev;
			Voisins[5] = znext;
		}

		/**
		  * Raz de l'�tat des cubes (a draw = false)
		  */
		void reset(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z]._Draw = true;
						_Cubes[x][y][z]._Type = CUBE_AIR;
					}
		}

		void fillWorldBuffers()
		{
			int i = 0;
			_NbVertices = 0;

			float size = NYCube::CUBE_SIZE;
			for (int x = 0; x<CHUNK_SIZE; x++)
				for (int y = 0; y<CHUNK_SIZE; y++)
					for (int z = 0; z<CHUNK_SIZE; z++)
					{
						NYCube * cube = &_Cubes[x][y][z];

						if (cube->_Draw && cube->_Type != CUBE_AIR)
						{
							float color[3] = { 0,0,0 };

							switch (cube->_Type)
							{
								case CUBE_EAU:
									color[0] = 0;
									color[1] = 102.0f / 255;
									color[2] = 1.0f;
									break;

								case CUBE_HERBE:
									color[0] = 0;
									color[1] = 102.0f / 255;
									color[2] = 0;
									break;

								case CUBE_TERRE:
									color[0] = 102.0f / 255;
									color[1] = 51.0f / 255;
									color[2] = 0;
									break;

								default:
									break;
							}

							///FACE DU BAS
							//VERTEX1
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;

							//VERTEX2
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;

							//VERTEX3
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;

							//VERTEX4
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;

							///FACE DU HAUT
							//VERTEX1
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;

							//VERTEX2
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;

							//VERTEX3
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;

							//VERTEX4
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;

							///FACE SUIVANT AXE X -- DOS
							//VERTEX1
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX2
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX3
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX4
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							///FACE SUIVANT AXE X -- FACE
							//VERTEX1
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX2
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX3
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX4
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							///FACE SUIVANT AXE Y -- FACE
							//VERTEX1
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0; _WorldCols[i] = 0;
							++i;

							//VERTEX2
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX3
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX4
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 1;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							///FACE SUIVANT AXE Y -- DOS
							//VERTEX1
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX2
							_WorldVert[i] = x*size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX3
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;
							_WorldVert[i] = z*size + size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;

							//VERTEX4
							_WorldVert[i] = x*size + size;
							_WorldCols[i] = color[0] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
							_WorldVert[i] = y*size + size;
							_WorldCols[i] = color[1] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = -1;
							++i;
							_WorldVert[i] = z*size;
							_WorldCols[i] = color[2] + (rand() % 100) / 2000.0f;
							_WorldNorm[i] = 0;
							++i;
						}

					}

					_NbVertices = i / 3;

					if (_BufWorld != 0)
						glDeleteBuffers(1, &_BufWorld);

					glGenBuffers(1, &_BufWorld);

					glBindBuffer(GL_ARRAY_BUFFER, _BufWorld);

					glBufferData(GL_ARRAY_BUFFER,
						_NbVertices * SIZE_VERTICE +
						_NbVertices * SIZE_COLOR +
						_NbVertices * SIZE_NORMAL,
						NULL,
						GL_STREAM_DRAW);

					GLenum error = glGetError();

					glBufferSubData(GL_ARRAY_BUFFER,
						0,
						_NbVertices * SIZE_VERTICE,
						_WorldVert);

					error = glGetError();

					glBufferSubData(GL_ARRAY_BUFFER,
						_NbVertices * SIZE_VERTICE,
						_NbVertices * SIZE_COLOR,
						_WorldCols);

					error = glGetError();

					glBufferSubData(GL_ARRAY_BUFFER,
						_NbVertices * SIZE_VERTICE +
						_NbVertices * SIZE_COLOR,
						_NbVertices * SIZE_NORMAL,
						_WorldNorm);

					error = glGetError();

					glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		//On met le chunk ddans son VBO
		void toVbo(void)
		{
			fillWorldBuffers();

			//On attache le VBO pour pouvoir le modifier
			glBindBuffer(GL_ARRAY_BUFFER, _BufWorld);

			//On reserve la quantite totale de datas (creation de la zone memoire, mais sans passer les donn�es)
			//Les tailles g_size* sont en octets, � vous de les calculer
			glBufferData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE +
				_NbVertices * SIZE_COLOR +
				_NbVertices * SIZE_NORMAL,
				NULL,
				GL_STREAM_DRAW);

			//Check error (la tester ensuite...)
			GLenum error = glGetError();

			//On copie les vertices
			glBufferSubData(GL_ARRAY_BUFFER,
				0, //Offset 0, on part du debut                        
				_NbVertices * SIZE_VERTICE, //Taille en octets des datas copi�s
				_WorldVert);  //Datas          

							  //Check error (la tester ensuite...)
			error = glGetError();

			//On copie les couleurs
			glBufferSubData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE, //Offset : on se place apr�s les vertices
				_NbVertices * SIZE_COLOR, //On copie tout le buffer couleur : on donne donc sa taille
				_WorldCols);  //Pt sur le buffer couleur       

							  //Check error (la tester ensuite...)
			error = glGetError();

			//On copie les normales (a vous de d�duire les params)
			glBufferSubData(GL_ARRAY_BUFFER,
				_NbVertices * (SIZE_VERTICE + SIZE_COLOR),
				_NbVertices * SIZE_NORMAL,
				_WorldNorm);

			//On debind le buffer pour eviter une modif accidentelle par le reste du code
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void render(void)
		{
			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_LIGHTING);

			//On bind le buuffer
			glBindBuffer(GL_ARRAY_BUFFER, _BufWorld);
			NYRenderer::checkGlError("glBindBuffer");

			//On active les datas que contiennent le VBO
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			//On place les pointeurs sur les datas, aux bons offsets
			glVertexPointer(3, GL_FLOAT, 0, (void*)(0));
			glColorPointer(3, GL_FLOAT, 0, (void*)(_NbVertices*SIZE_VERTICE));
			glNormalPointer(GL_FLOAT, 0, (void*)(_NbVertices*SIZE_VERTICE + _NbVertices*SIZE_COLOR));

			//On demande le dessin
			glDrawArrays(GL_QUADS, 0, _NbVertices);

			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);

			glDisable(GL_COLOR_MATERIAL);
			glDisable(GL_LIGHTING);
		}

		/**
		  * On verifie si le cube peut �tre vu
		  */
		bool test_hidden(int x, int y, int z)
		{
			NYCube * cubeXPrev = NULL; 
			NYCube * cubeXNext = NULL; 
			NYCube * cubeYPrev = NULL; 
			NYCube * cubeYNext = NULL; 
			NYCube * cubeZPrev = NULL; 
			NYCube * cubeZNext = NULL; 

			if(x == 0 && Voisins[0] != NULL)
				cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE-1][y][z]);
			else if(x > 0)
				cubeXPrev = &(_Cubes[x-1][y][z]);

			if(x == CHUNK_SIZE-1 && Voisins[1] != NULL)
				cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if(x < CHUNK_SIZE-1)
				cubeXNext = &(_Cubes[x+1][y][z]);

			if(y == 0 && Voisins[2] != NULL)
				cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE-1][z]);
			else if(y > 0)
				cubeYPrev = &(_Cubes[x][y-1][z]);

			if(y == CHUNK_SIZE-1 && Voisins[3] != NULL)
				cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if(y < CHUNK_SIZE-1)
				cubeYNext = &(_Cubes[x][y+1][z]);

			if(z == 0 && Voisins[4] != NULL)
				cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE-1]);
			else if(z > 0)
				cubeZPrev = &(_Cubes[x][y][z-1]);

			if(z == CHUNK_SIZE-1 && Voisins[5] != NULL)
				cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if(z < CHUNK_SIZE-1)
				cubeZNext = &(_Cubes[x][y][z+1]);

			if( cubeXPrev == NULL || cubeXNext == NULL ||
				cubeYPrev == NULL || cubeYNext == NULL ||
				cubeZPrev == NULL || cubeZNext == NULL )
				return false;

			if( cubeXPrev->isSolid() == true && //droite
				cubeXNext->isSolid() == true && //gauche
				cubeYPrev->isSolid() == true && //haut
				cubeYNext->isSolid() == true && //bas
				cubeZPrev->isSolid() == true && //devant
				cubeZNext->isSolid() == true )  //derriere
				return true;
			return false;
		}

		void disableHiddenCubes(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z]._Draw = true;
						if(test_hidden(x,y,z))
							_Cubes[x][y][z]._Draw = false;
					}
		}


};