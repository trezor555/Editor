#include "TerrainV2.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <thread>
#include <stdio.h>      
#include <stdlib.h>    
#include <time.h>
#include <string>
#include <FreeImage.h>
using namespace std;

const unsigned int BLENDMAP_SIZE = 128;

TerrainV2::TerrainV2(unsigned int terrainSize, unsigned int numCols, unsigned int numRows, float distanceX, float distanceZ)
{
	m_TerrainSize = terrainSize;
	m_NumCols = numCols;
	m_NumRows = numRows;
	m_distanceX = distanceX;
	m_distanceZ = distanceZ;

	m_VertexCount = m_NumCols* m_NumRows;
	m_numTriangles = ((m_NumCols )*(m_NumRows )) * 2;

	m_TerrainWidth = (m_NumCols - 1) * distanceX;
	m_TerrainDepth = (m_NumRows - 1) * distanceZ;
	m_TileCountInRow = (terrainSize / m_NumCols);

	m_TerrainRepeat = m_TileCountInRow;

	m_EditBrushRadius = 10;
	m_EditBrushStrength = 100;
	m_EditBrushHeight = 32918;

	m_PaintBrushRadius = 10;
	m_PaintBrushStrength = 100;


	m_TerrainViewDistance = 22000;
	m_WorkingLayerIndex = 0;

	m_LayerSet[0] = true;
	m_LayerSet[1] = false;
	m_LayerSet[2] = false;
	m_LayerSet[3] = false;

	m_LayerRepeat[0] = 128 * (terrainSize / 1024.0f)* (distanceX / 32);
	m_LayerRepeat[1] = 128 * (terrainSize / 1024.0f)* (distanceX / 32);
	m_LayerRepeat[2] = 128 * (terrainSize / 1024.0f)* (distanceX / 32);
	m_LayerRepeat[3] = 128 * (terrainSize / 1024.0f)* (distanceX / 32);

	tilePositions.resize(m_TileCountInRow* m_TileCountInRow);
	BlendMapArray.resize(m_TileCountInRow* m_TileCountInRow);
	m_BoundingBoxes.resize(m_TileCountInRow * m_TileCountInRow);

	m_showCircle = false;
	m_terrainModified = false;
	m_bVisible = true;
	srand(time(nullptr));
}

TerrainV2::TerrainV2()
{

}

bool TerrainV2::Setup()
{
	
	for (size_t i = 0; i < m_TileCountInRow; ++i)
	{
		for (size_t j = 0; j < m_TileCountInRow; ++j)
		{
			tilePositions[i * m_TileCountInRow + j].position.x = i*(m_NumCols)*(m_distanceX);
			tilePositions[i * m_TileCountInRow + j].position.y = 0;
			tilePositions[i * m_TileCountInRow + j].position.z = j*(m_NumCols)*(m_distanceX);
			tilePositions[i * m_TileCountInRow + j].center = glm::vec3(tilePositions[i * m_TileCountInRow + j].position.x + ((m_NumCols / 2) * m_distanceX), 0, tilePositions[i * m_TileCountInRow + j].position.z + ((m_NumCols / 2) * m_distanceX));
		}
	}

	m_TerrainHeights = new unsigned short[m_TerrainSize * m_TerrainSize];
	std::fill_n(m_TerrainHeights, m_TerrainSize * m_TerrainSize, 32768);
	unsigned int index = 0;
	

	
	double texStep = 1.0 / (m_TerrainSize);
	texOffsetX = texStep*(m_NumCols);
	texOffsetY = texStep * (m_NumCols);
	for (int i = 0; i < m_NumRows+1; ++i)
	{
		for (int j = 0; j < m_NumCols+1; ++j)
		{
			Vertex v;
			v.position.x = j * m_distanceX;
			v.position.y = 0;
			v.position.z = i * m_distanceZ;

			v.texcoord.x = texStep*j;
			v.texcoord.y = texStep*i;

			v.normal = glm::vec3(0, 0, 0);
			m_terrainVertices.push_back(v);
		}
	}

	//bbox
	for (int i = 0; i < m_TileCountInRow; ++i)
	{
		for (int j = 0; j < m_TileCountInRow; ++j)
		{
			m_BoundingBoxes[i * m_TileCountInRow + j] = BoundingBox(tilePositions[i * m_TileCountInRow + j].position, glm::vec3(tilePositions[i * m_TileCountInRow + j].position.x + ((m_NumCols - 1) * m_distanceX), 0, tilePositions[i * m_TileCountInRow + j].position.z + ((m_NumCols - 1) * m_distanceZ)));
		}
	}


	m_terrainIndices.resize(m_numTriangles * 3);
	unsigned int k = 0;
	for (int i = 0; i < m_NumRows; ++i)
	{
		for (int j = 0; j < m_NumCols ; ++j)
		{
			m_terrainIndices[k] = (i + 1) * (m_NumCols+1)+j;
			m_terrainIndices[k + 1] = i   * (m_NumCols + 1) + j + 1;
			m_terrainIndices[k + 2] = i   * (m_NumCols + 1) + j;

			m_terrainIndices[k + 3] = (i + 1) * (m_NumCols + 1) + j + 1;
			m_terrainIndices[k + 4] = i   * (m_NumCols + 1) + j + 1;
			m_terrainIndices[k + 5] = (i + 1) * (m_NumCols + 1) + j;
			k += 6;
		}
	}

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_terrainVertices.size() * sizeof(Vertex), &m_terrainVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_terrainIndices.size() * sizeof(GLuint), &m_terrainIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)offsetof(Vertex, texcoord));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)offsetof(Vertex, normal));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GenerateHeightmap();
	
	LoadShader();
	terrainLayers[0].CreateTexture("../Data/Textures/Terrain/DiffuseMaps/sand_diffuse.bmp");
	m_TerrainLayerPath[0] = "../Data/Textures/Terrain/DiffuseMaps/sand_diffuse.bmp";

	//macroTex.CreateTexture("../Data/Textures/Terrain/Macro/macro_1.jpg");
	//terrainLayers[1].CreateTexture("../Data/Textures/Terrain/DiffuseMaps/snow_2.jpg");

	GenerateBlendMaps();

	return true;
}

bool TerrainV2::Setup(std::string fileName, unsigned int numCols, unsigned int numRows, float distanceX, float distanceZ, std::vector<std::string> layers)
{
	std::string terrainPath = fileName;
	terrainPath += "terrain.tiff";
	// load in terrainImage
	int iBPP;
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(terrainPath.c_str(), 0); 

	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(terrainPath.c_str());

	if (fif == FIF_UNKNOWN)
		return false;

	if (FreeImage_FIFSupportsReading(fif)) 
		dib = FreeImage_Load(fif, terrainPath.c_str());
	if (!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); 

	m_TerrainSize = FreeImage_GetWidth(dib); 
	m_TerrainSize = FreeImage_GetHeight(dib);
	iBPP = FreeImage_GetBPP(dib);


	if (bDataPointer == nullptr || m_TerrainSize == 0 || m_TerrainSize == 0)
		return false;

	GLenum internalFormat;
	GLenum format;
	GLenum type;
	// create height
	glGenTextures(1, &m_heightMapID);
	glBindTexture(GL_TEXTURE_2D, m_heightMapID);

	
	internalFormat = GL_LUMINANCE16;
	format = GL_LUMINANCE;
	type = GL_UNSIGNED_SHORT;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE16, m_TerrainSize, m_TerrainSize, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, bDataPointer);
	unsigned short* temp = new unsigned short[m_TerrainSize * m_TerrainSize];
	m_TerrainHeights = new unsigned short[m_TerrainSize * m_TerrainSize];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, temp);
	glBindTexture(GL_TEXTURE_2D, 0);
	FreeImage_Unload(dib);
	
	// cons

	for (unsigned int i = 0; i < m_TerrainSize; ++i)
	{
		for (unsigned int j = 0; j < m_TerrainSize; ++j)
		{
			m_TerrainHeights[i * m_TerrainSize + j] = temp[j * m_TerrainSize + i];
		}
	}
	delete[] temp;

	m_NumCols = numCols;
	m_NumRows = numRows;
	m_distanceX = distanceX;
	m_distanceZ = distanceZ;

	m_VertexCount = numCols* numRows;
	m_numTriangles = ((numCols - 1)*(numRows - 1)) * 2;

	m_TerrainWidth = (numCols - 1) * distanceX;
	m_TerrainDepth = (numRows - 1) * distanceZ;
	m_TileCountInRow = (m_TerrainSize / m_NumCols);

	m_TerrainRepeat = m_TileCountInRow;

	m_EditBrushRadius = 10;
	m_EditBrushStrength = 100;
	m_EditBrushHeight = 32918;

	m_PaintBrushRadius = 10;
	m_PaintBrushStrength = 100;


	m_TerrainViewDistance = 15000;
	m_WorkingLayerIndex = 0;

	m_LayerSet[0] = true;
	m_LayerSet[1] = false;
	m_LayerSet[2] = false;
	m_LayerSet[3] = false;

	m_LayerRepeat[0] = 128 * (m_TerrainSize / 1024)* (distanceX / 32);
	m_LayerRepeat[1] = 128 * (m_TerrainSize / 1024)* (distanceX / 32);
	m_LayerRepeat[2] = 128 * (m_TerrainSize / 1024)* (distanceX / 32);
	m_LayerRepeat[3] = 128 * (m_TerrainSize / 1024)* (distanceX / 32);

	tilePositions.resize(m_TileCountInRow* m_TileCountInRow);
	BlendMapArray.resize(m_TileCountInRow* m_TileCountInRow);
	m_BoundingBoxes.resize(m_TileCountInRow * m_TileCountInRow);

	m_showCircle = false;
	m_terrainModified = false;

	tilePositions.resize((m_TerrainSize / m_NumCols) * (m_TerrainSize / m_NumCols));
	BlendMapArray.resize((m_TerrainSize / m_NumCols) * (m_TerrainSize / m_NumCols));
	m_BoundingBoxes.resize((m_TerrainSize / m_NumCols) * (m_TerrainSize / m_NumCols));
	srand(time(nullptr));

	size_t t = sizeof(Vertex);
	for (size_t i = 0; i < m_TileCountInRow; ++i)
	{
		for (size_t j = 0; j < m_TileCountInRow; ++j)
		{
			tilePositions[i * m_TileCountInRow + j].position.x = i*(m_NumCols - 1)*m_distanceX;
			tilePositions[i * m_TileCountInRow + j].position.y = 0;
			tilePositions[i * m_TileCountInRow + j].position.z = j*(m_NumCols - 1)*m_distanceX;
			tilePositions[i * m_TileCountInRow + j].center = glm::vec3(tilePositions[i * m_TileCountInRow + j].position.x + ((m_NumCols / 2) * m_distanceX), 0, tilePositions[i * m_TileCountInRow + j].position.z + ((m_NumCols / 2) * m_distanceX));
		}
	}

	// setup
	double texStep = 1.0 / (m_TerrainSize);
	texOffsetX = texStep*(m_NumCols - 1);
	texOffsetY = texStep * (m_NumCols - 1);
	for (int i = 0; i < m_NumRows; ++i)
	{
		for (int j = 0; j < m_NumCols; ++j)
		{
			Vertex v;
			v.position.x = j * m_distanceX;
			v.position.y = 0;
			v.position.z = i * m_distanceZ;

			v.texcoord.x = texStep*j;
			v.texcoord.y = texStep*i;

			v.normal = glm::vec3(0, 0, 0);
			m_terrainVertices.push_back(v);
		}
	}

	//bbox
	for (int i = 0; i < m_TileCountInRow; ++i)
	{
		for (int j = 0; j < m_TileCountInRow; ++j)
		{
			int startI = (i) * (m_NumCols);
			int endI = (i+1) * (m_NumCols);

			int startJ = (j) * (m_NumCols);
			int endJ = (j+1) * (m_NumCols);

			float tmax = -999999;
			float tmin = 9999999;
			for (int m = startI; m < endI; ++m)
			{
				for (int n = startJ; n < endJ; ++n)
				{
					if ((m_TerrainHeights[m * m_TerrainSize + n]-32768) > tmax)
					{
						tmax = m_TerrainHeights[m * m_TerrainSize + n] - 32768;
					}

					if ((m_TerrainHeights[m * m_TerrainSize + n] - 32768) < tmin)
					{
						tmin = m_TerrainHeights[m * m_TerrainSize + n] - 32768;
					}
					
				}
			}
			m_BoundingBoxes[i * m_TileCountInRow + j] = BoundingBox(glm::vec3(tilePositions[i * m_TileCountInRow + j].position.x, tmin, tilePositions[i * m_TileCountInRow + j].position.z), glm::vec3(tilePositions[i * m_TileCountInRow + j].position.x + ((m_NumCols - 1) * m_distanceX), tmax, tilePositions[i * m_TileCountInRow + j].position.z + ((m_NumCols - 1) * m_distanceZ)));
		}
	}

	m_terrainIndices.resize(m_numTriangles * 3);
	unsigned int k = 0;
	for (int i = 0; i < m_NumRows - 1; ++i)
	{
		for (int j = 0; j < m_NumCols - 1; ++j)
		{
			m_terrainIndices[k] = (i + 1) * (m_NumCols)+j;
			m_terrainIndices[k + 1] = i   * (m_NumCols)+j + 1;
			m_terrainIndices[k + 2] = i   * (m_NumCols)+j;

			m_terrainIndices[k + 3] = (i + 1) * (m_NumCols)+j + 1;
			m_terrainIndices[k + 4] = i   * (m_NumCols)+j + 1;
			m_terrainIndices[k + 5] = (i + 1) * (m_NumCols)+j;
			k += 6;
		}
	}

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_terrainVertices.size() * sizeof(Vertex), &m_terrainVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_terrainIndices.size() * sizeof(GLuint), &m_terrainIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)offsetof(Vertex, texcoord));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)offsetof(Vertex, normal));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	LoadShader();
	for (int i = 0; i < 4; ++i)
	{
		if (layers[i] != "")
		{
			terrainLayers[i].CreateTexture(layers[i].c_str());
			m_TerrainLayerPath[i] = layers[i];
			m_LayerSet[i] = true;
		}
	}
	
	LoadBlendmaps(fileName);

	return true;
}

bool TerrainV2::LoadShader()
{
	terrainShader.CreateShader("terrain.vs", "terrain.ps");
	

	worldLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "world");
	viewLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "view");
	projectionLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "projection");

	texOffsetXLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "texoffsetX");;
	texOffsetYLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "texoffsetY");;

	terrainLayersLoc[0] = glGetUniformLocation(terrainShader.GetShaderProgram(), "layer1");
	terrainLayersLoc[1] = glGetUniformLocation(terrainShader.GetShaderProgram(), "layer2");
	terrainLayersLoc[2] = glGetUniformLocation(terrainShader.GetShaderProgram(), "layer3");
	terrainLayersLoc[3] = glGetUniformLocation(terrainShader.GetShaderProgram(), "layer4");

	RepeatLayer1Loc = glGetUniformLocation(terrainShader.GetShaderProgram(), "RepeatLayer1");
	RepeatLayer2Loc = glGetUniformLocation(terrainShader.GetShaderProgram(), "RepeatLayer2");
	RepeatLayer3Loc = glGetUniformLocation(terrainShader.GetShaderProgram(), "RepeatLayer3");
	RepeatLayer4Loc = glGetUniformLocation(terrainShader.GetShaderProgram(), "RepeatLayer4");

	terrainSizeLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "terrainSize");
	blendMapLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "blendmap");

	heightmap_Loc = glGetUniformLocation(terrainShader.GetShaderProgram(), "heightmap");

	circleCenterLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "circleCenter");
	DiffuseDirectionLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "DiffuseDirection");

	LightPositionLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "LightPosition");

	brushSizeLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "brushSize");
	repeatTexcoordsLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "repeatTexcoords");
	showCircleBrushLoc = glGetUniformLocation(terrainShader.GetShaderProgram(), "showCircleBrush");

	m_MacroTex = glGetUniformLocation(terrainShader.GetShaderProgram(), "macroTex");
	return false;
}

void TerrainV2::Draw(glm::mat4& view, glm::mat4& projection, glm::vec3& cameraPos, Frustum& frustum)
{
	if (!m_bVisible)
		return;

	visibleTileIndices.clear();
	terrainShader.Begin();
	m_RenderedTiles = 0;
	
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(terrainSizeLoc, m_TerrainSize);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(terrainLayersLoc[0], 0);
	glBindTexture(GL_TEXTURE_2D, terrainLayers[0].GetID());

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(terrainLayersLoc[1], 1);
	glBindTexture(GL_TEXTURE_2D, terrainLayers[1].GetID());

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(terrainLayersLoc[2], 2);
	glBindTexture(GL_TEXTURE_2D, terrainLayers[2].GetID());

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(terrainLayersLoc[3], 3);
	glBindTexture(GL_TEXTURE_2D, terrainLayers[3].GetID());

	glActiveTexture(GL_TEXTURE4);
	glUniform1i(heightmap_Loc, 4);
	glBindTexture(GL_TEXTURE_2D, m_heightMapID);

	glActiveTexture(GL_TEXTURE5);
	glUniform1i(m_MacroTex, 5);
	glBindTexture(GL_TEXTURE_2D, macroTex.GetID());

	glActiveTexture(GL_TEXTURE6);
	glUniform1i(blendMapLoc, 6);

	glUniform1i(repeatTexcoordsLoc, m_TerrainRepeat);
	glUniform1i(RepeatLayer1Loc, m_LayerRepeat[0]);
	glUniform1i(RepeatLayer2Loc, m_LayerRepeat[1]);
	glUniform1i(RepeatLayer3Loc, m_LayerRepeat[2]);
	glUniform1i(RepeatLayer4Loc, m_LayerRepeat[3]);



	float bSize =  m_CircleBrushSize;
	glUniform1f(brushSizeLoc, bSize);
	glUniform1i(showCircleBrushLoc, m_showCircle);
	glBindVertexArray(VAO);
	for (unsigned int i = 0; i < m_TileCountInRow; ++i)
	{
		for (unsigned int j = 0; j < m_TileCountInRow; ++j)
		{
			
			glm::vec3 s = cameraPos - tilePositions[i * m_TileCountInRow + j].center;

			if (glm::length(s) < m_TerrainViewDistance)
			{
				TileIndex t;
				t.i = i + 1;
				t.j = j + 1;
				visibleTileIndices.push_back(t);
				
				if (frustum.Intersect(m_BoundingBoxes[i * m_TileCountInRow + j]))
				{
					m_RenderedTiles++;
					glm::vec3 diff(0, 10000, 10000);
					glm::mat4 world;

					glBindTexture(GL_TEXTURE_2D, BlendMapArray[i * m_TileCountInRow + j]);

					world = glm::translate(world, tilePositions[i * m_TileCountInRow + j].position);
					glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(world));
					glUniform3f(DiffuseDirectionLoc, difusseDirection.x, difusseDirection.y, difusseDirection.z);
					glUniform3f(LightPositionLoc, diff.x, diff.y, diff.z);
					glUniform1f(texOffsetXLoc, i*texOffsetX);
					glUniform1f(texOffsetYLoc, j*texOffsetX);
					glUniform2f(circleCenterLoc, m_MousePosOnTerrain.x / (m_TerrainSize * m_distanceZ), m_MousePosOnTerrain.z / (m_TerrainSize * m_distanceX));
					glDrawElements(GL_TRIANGLES, m_numTriangles * 3, GL_UNSIGNED_INT, 0);
				}
			}
		}
	}

	glBindVertexArray(0);
	
}

void TerrainV2::GenerateHeightmap()
{
	glGenTextures(1, &m_heightMapID);
	glBindTexture(GL_TEXTURE_2D, m_heightMapID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE16, m_TerrainSize, m_TerrainSize, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, &m_TerrainHeights[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool TerrainV2::PickTerrain(int mouseX, int mouseY, glm::dmat4& view, glm::dmat4& projection, glm::tvec3<double>& intersectionPoint )
{
	if (!m_bVisible)
		return false;

	glm::tvec3<double>  ret;
	GLfloat winX, winY, winZ;

	
	winX = mouseX;
	winY = viewport[3] - 1 - mouseY;

	glReadPixels(int(winX), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	ret = glm::unProject<double, double>(glm::tvec3<double>(winX, winY, winZ), view, projection, viewport);
	m_MousePosOnTerrain = ret;
	
	intersectionPoint = ret;
}

void TerrainV2::ModifyHeight()
{
	if (!m_bVisible)
		return;

	glBindTexture(GL_TEXTURE_2D, m_heightMapID);

	unsigned int a = ((m_MousePosOnTerrain.x / m_TerrainDepth) * ((m_NumCols - 1)));
	unsigned int c = ((m_MousePosOnTerrain.z / m_TerrainDepth) * ((m_NumCols - 1)));

	for (unsigned int k = 0; k < visibleTileIndices.size(); ++k)
	{
		unsigned int bboxIndex = (visibleTileIndices[k].i - 1)* m_TileCountInRow + (visibleTileIndices[k].j - 1);
		float maxPoint = m_BoundingBoxes[bboxIndex].max.y;
		float minPoint = m_BoundingBoxes[bboxIndex].min.y;
		int startI = (visibleTileIndices[k].i - 1) * (m_NumCols );
		int endI = (visibleTileIndices[k].i) * (m_NumCols );

		int startJ = (visibleTileIndices[k].j - 1) * (m_NumCols );
		int endJ = (visibleTileIndices[k].j) * (m_NumCols );


		for (int i = startI; i < endI; ++i)
		{
			for (int j = startJ; j < endJ; ++j)
			{
				float distance = sqrtf((i - a)*(i - a) + (j - c)*(j - c));
				if (distance < m_EditBrushRadius)
				{
					m_terrainModified = true;
					short m = cos((float)glm::pi<float>() / 2 * (m_EditBrushRadius - (m_EditBrushRadius - distance)) / (float)m_EditBrushRadius) * m_EditBrushStrength;
					
					if (m_EditBrushHeight >= 32768)
					{
						if (m + m_TerrainHeights[i * m_TerrainSize + j] < m_EditBrushHeight)
						{
							m_TerrainHeights[i * m_TerrainSize + j] += m;
							unsigned short h = m_TerrainHeights[i * m_TerrainSize + j];

							if (h - 32768 > maxPoint)
							{
								maxPoint = h - 32768;
							}

							if (h - 32768 < minPoint)
							{
								minPoint = h - 32768;
							}
							glTexSubImage2D(GL_TEXTURE_2D,
								0,
								i,
								j,
								1,
								1,
								GL_RED,
								GL_UNSIGNED_SHORT,
								&h);
						}

						if (m + m_TerrainHeights[i * m_TerrainSize + j] > m_EditBrushHeight)
						{
							m_TerrainHeights[i * m_TerrainSize + j] = m_EditBrushHeight;
							unsigned short h = m_EditBrushHeight;

							if (h - 32768 > maxPoint)
							{
								maxPoint = h - 32768;
							}

							if (h - 32768 < minPoint)
							{
								minPoint = h - 32768;
							}
							glTexSubImage2D(GL_TEXTURE_2D,
								0,
								i,
								j,
								1,
								1,
								GL_RED,
								GL_UNSIGNED_SHORT,
								&h);
						}
					}
					
					
					if (m_EditBrushHeight < 32768)
					{
						m *= -1;

						if (m + m_TerrainHeights[i * m_TerrainSize + j] > m_EditBrushHeight)
						{
							m_TerrainHeights[i * m_TerrainSize + j] += m;
							unsigned short h = m_TerrainHeights[i * m_TerrainSize + j];

							if (h - 32768 > maxPoint)
							{
								maxPoint = h-32768;
							}

							if (h - 32768 < minPoint)
							{
								minPoint = h - 32768;
							}

							glTexSubImage2D(GL_TEXTURE_2D,
								0,
								i,
								j,
								1,
								1,
								GL_RED,
								GL_UNSIGNED_SHORT,
								&h);
						}

						if (m + m_TerrainHeights[i * m_TerrainSize + j] < m_EditBrushHeight)
						{
							m_TerrainHeights[i * m_TerrainSize + j] = m_EditBrushHeight;
							unsigned short h = m_EditBrushHeight;

							if (h - 32768 > maxPoint)
							{
								maxPoint = h - 32768;
							}

							if (h - 32768 < minPoint)
							{
								minPoint = h - 32768;
							}
							glTexSubImage2D(GL_TEXTURE_2D,
								0,
								i,
								j,
								1,
								1,
								GL_RED,
								GL_UNSIGNED_SHORT,
								&h);
						}
					}
				}
			}			
		}

		m_BoundingBoxes[bboxIndex].max.y = maxPoint;
		m_BoundingBoxes[bboxIndex].min.y = minPoint;
	}
	
}

void TerrainV2::SetViewportSize(glm::tvec4<double> viewportSize)
{
	viewport = viewportSize;
}

void TerrainV2::ComputeTh()
{

}

void TerrainV2::ComputeNormals()
{
	
	std::vector<glm::vec3> m_normals;
	m_normals.reserve(m_VertexCount);

	for (int i = 0; i < m_VertexCount; i += 3)
	{
		// get the three vertices that make the faces
		glm::vec3 p0 = m_terrainVertices[m_terrainIndices[i + 0]].position;
		p0.y = m_TerrainHeights[m_terrainIndices[i + 0]];
		glm::vec3 p1 = m_terrainVertices[m_terrainIndices[i + 1]].position;
		p1.y = m_TerrainHeights[m_terrainIndices[i + 1]];
		glm::vec3 p2 = m_terrainVertices[m_terrainIndices[i + 2]].position;
		p2.y = m_TerrainHeights[m_terrainIndices[i + 2]];

		glm::vec3 e1 = p1 - p0;
		glm::vec3 e2 = p2 - p0;
		glm::vec3 normal = glm::cross(e1, e2);
		normal = glm::normalize(normal);

		// Store the face's normal for each of the vertices that make up the face.
		m_normals[m_terrainIndices[i + 0]] += normal;
		m_normals[m_terrainIndices[i + 1]] += normal;
		m_normals[m_terrainIndices[i + 2]] += normal;
	}


	// Now loop through each vertex vector, and avarage out all the normals stored.
	for (int i = 0; i < m_normals.size(); ++i)
	{
		m_normals[i] = glm::normalize(m_normals[i]);
	}
}

void TerrainV2::filter3x3()
{
	glBindTexture(GL_TEXTURE_2D, m_heightMapID);
	std::vector<float> temp(m_TerrainSize*m_TerrainSize);
	int index = 0;
	for (int i = 0; i < m_TerrainSize; ++i)
		for (int j = 0; j < m_TerrainSize; ++j)
			temp[i*m_TerrainSize + j] = sampleHeight3x3(i, j);

	for (int i = 0; i < m_TerrainSize; ++i)
	{
		for (int j = 0; j < m_TerrainSize; ++j)
		{
			m_TerrainHeights[index] = temp[index];
			index++;
		}
	}

	index = 0;
	for (int i = 0; i < m_TerrainSize; ++i)
	{
		for (int j = 0; j < m_TerrainSize; ++j)
		{
			unsigned short m = m_TerrainHeights[index];
			glTexSubImage2D(GL_TEXTURE_2D,
				0,
				i,
				j,
				1,
				1,
				GL_RED,
				GL_UNSIGNED_SHORT,
				&m);
			index++;
		}
	}
}

bool TerrainV2::inBounds(int i, int j)
{
	return
		i >= 0 &&
		i < m_TerrainSize-1 &&
		j >= 0 &&
		j < m_TerrainSize - 1;
}

float TerrainV2::sampleHeight3x3(int i, int j)
{
	float avg = 0.0f;
	float num = 0.0f;

	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
		{
			if (inBounds(m, n))
			{
				avg += m_TerrainHeights[m * m_TerrainSize + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void TerrainV2::GenerateBlendmap()
{
	
	PixelRGBA* vals = new PixelRGBA[BLENDMAP_SIZE * BLENDMAP_SIZE];
	int index = 0;
	for (int i = 0; i < BLENDMAP_SIZE; ++i)
	{
		for (int j = 0; j < BLENDMAP_SIZE; ++j)
		{
			vals[index].r = 0;
			vals[index].g = 0;
			vals[index].b = 0;
			vals[index].a = 0;
			index++;

		}
	}

	glGenTextures(1, &m_blendMap);
	glBindTexture(GL_TEXTURE_2D, m_blendMap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_T);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BLENDMAP_SIZE, BLENDMAP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, vals);
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] vals;
	valu = new GLuint[BLENDMAP_SIZE * BLENDMAP_SIZE];
}

void TerrainV2::PaintTerrain(float deltaTime)
{
	if (!m_bVisible)
		return;

	for (unsigned int k = 0; k < visibleTileIndices.size(); ++k)
	{
		unsigned int blendMapIndex = (visibleTileIndices[k].i - 1)* m_TileCountInRow + (visibleTileIndices[k].j - 1);
		glBindTexture(GL_TEXTURE_2D, BlendMapArray[blendMapIndex]);
		unsigned int a = ((m_MousePosOnTerrain.x / ((m_NumCols-1) * m_distanceX)) * ((BLENDMAP_SIZE)));
		unsigned int c = ((m_MousePosOnTerrain.z / ((m_NumCols-1) * m_distanceZ)) * ((BLENDMAP_SIZE)));

		int startI = (visibleTileIndices[k].i -1) * (BLENDMAP_SIZE);
		int endI = (visibleTileIndices[k].i) * (BLENDMAP_SIZE);

		int startJ = (visibleTileIndices[k].j-1 ) * (BLENDMAP_SIZE);
		int endJ = (visibleTileIndices[k].j) * (BLENDMAP_SIZE);

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, blendMapValuePtr);
		
		for (int i = startI; i < endI; ++i)
		{
			for (int j = startJ; j < endJ; ++j)
			{
				float dist = sqrtf((i - a)*(i - a) + (j - c)*(j - c));
				if (dist < (m_PaintBrushRadius*(BLENDMAP_SIZE/128)))
				{			
					
					int m = ( (m_PaintBrushStrength))*0.05;
					
					PixelRGBA rgbaa;
					unsigned char rgba[4];
					rgbaa = *(PixelRGBA*)&blendMapValuePtr[(j - startJ) * BLENDMAP_SIZE + (i - startI)];
					rgba[0] = rgbaa.r;
					rgba[1] = rgbaa.g;
					rgba[2] = rgbaa.b;
					rgba[3] = rgbaa.a;
					

					if (m_LayerSet[m_WorkingLayerIndex])
					{
						if (rgba[m_WorkingLayerIndex] + m > 255)
						{
							rgba[m_WorkingLayerIndex] = 255;
						}
						else
						{
							rgba[m_WorkingLayerIndex] += m;
						}

						for (int i = 0; i < 4; ++i)
						{
							
								if (m_WorkingLayerIndex != i)
								{
									if (rgba[i] - m > 0)
									{
										rgba[i] -= m;
									}
									else
									{
										rgba[i] = 0;
									}
								}
							
						}
					}
					
					glTexSubImage2D(GL_TEXTURE_2D,
						0,
						(i - startI),
						(j - startJ),
						1,
						1,
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						&rgba);
				}
				
			}
		}
	}
}

void TerrainV2::SetEditBrushRadius(int val)
{
	m_EditBrushRadius = val;
}

void TerrainV2::SetTerrainViewDistance(int val)
{
	m_TerrainViewDistance = val;
}

void TerrainV2::SetEditBrushHeight(int val)
{
	m_EditBrushHeight = val;
}

void TerrainV2::SetEditBrushStrength(int val)
{
	m_EditBrushStrength = val;
}

unsigned short* TerrainV2::GetHeightData()
{
	return m_TerrainHeights;
}

void TerrainV2::GenerateBlendMaps()
{
	
	blendMapValuePtr = new PixelRGBA[BLENDMAP_SIZE * BLENDMAP_SIZE];
	int index = 0;
	for (int i = 0; i < BLENDMAP_SIZE; ++i)
	{
		for (int j = 0; j < BLENDMAP_SIZE; ++j)
		{
			blendMapValuePtr[index].r = 255;
			blendMapValuePtr[index].g = 0;
			blendMapValuePtr[index].b = 0;
			blendMapValuePtr[index].a = 0;
			index++;
		}
	}

	for (int i = 0; i < m_TileCountInRow; ++i)
	{
		for (int j = 0; j < m_TileCountInRow; ++j)
		{
			glGenTextures(1, &BlendMapArray[i* m_TileCountInRow+j]);
			glBindTexture(GL_TEXTURE_2D, BlendMapArray[i* m_TileCountInRow + j]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BLENDMAP_SIZE, BLENDMAP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, blendMapValuePtr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	
}

void TerrainV2::SetPaintBrushRadius(int val)
{
	m_PaintBrushRadius = val;
}

void TerrainV2::SetPaintBrushStrength(int val)
{
	m_PaintBrushStrength = val;
}

float TerrainV2::GetHeight(int x, int z)
{
	float c = (z) / (float)m_distanceX;
	float d = (x) / (float)m_distanceX;

	int row = abs((int)floorf(d));
	int col = abs((int)floorf(c));


	//return TerrainHeight[row*m_numCols + col];

	if (row > (m_TerrainSize-1))
	{
		row = m_TerrainSize - 1;
	}

	if (col > (m_TerrainSize - 1))
	{
		col = (m_TerrainSize - 1);
	}
	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D

	float A = m_TerrainHeights[row*m_TerrainSize + col]-32768;
	float B = m_TerrainHeights[row*m_TerrainSize + col + 1] - 32768;
	float C = m_TerrainHeights[(row + 1)*m_TerrainSize + col] - 32768;
	float D = m_TerrainHeights[(row + 1)*m_TerrainSize + (col + 1)] - 32768;

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (t < 1.0f - s)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}


}

void TerrainV2::DrawWithShadows(glm::mat4& view, glm::mat4& projection, glm::vec3& cameraPos, Frustum& frustum)
{

}

void TerrainV2::SetTerrainLayer(std::string path, unsigned int layerIndex)
{
	if (layerIndex == 0)
	{
		if (m_TerrainLayerPath[0] != "")
		{
			m_TerrainLayerPath[0] = path;
			terrainLayers[0].Release();
			terrainLayers[0].CreateTexture(path.c_str());
		}
		else
		{
			m_TerrainLayerPath[0] = path;
			terrainLayers[0].CreateTexture(path.c_str());
		}

		m_LayerSet[0] = true;
	}
	
	if (layerIndex == 1)
	{
		if (m_TerrainLayerPath[1] != "")
		{
			m_TerrainLayerPath[1] = path;
			terrainLayers[1].Release();
			terrainLayers[1].CreateTexture(path.c_str());
		}
		else
		{
			m_TerrainLayerPath[1] = path;
			terrainLayers[1].CreateTexture(path.c_str());
		}

		m_LayerSet[1] = true;
	}

	if (layerIndex == 2)
	{
		if (m_TerrainLayerPath[2] != "")
		{
			m_TerrainLayerPath[2] = path;
			terrainLayers[2].Release();
			terrainLayers[2].CreateTexture(path.c_str());
		}
		else
		{
			m_TerrainLayerPath[2] = path;
			terrainLayers[2].CreateTexture(path.c_str());
		}

		m_LayerSet[2] = true;
	}

	if (layerIndex == 3)
	{
		if (m_TerrainLayerPath[3] != "")
		{
			m_TerrainLayerPath[3] = path;
			terrainLayers[3].Release();
			terrainLayers[3].CreateTexture(path.c_str());
		}
		else
		{
			m_TerrainLayerPath[3] = path;
			terrainLayers[3].CreateTexture(path.c_str());
		}
		m_LayerSet[3] = true;
	}
}

void TerrainV2::SetWorkingLayer(unsigned int index)
{
	m_WorkingLayerIndex = index;
}

void TerrainV2::SetTextureLayer_1_Repeat(int val)
{
	m_LayerRepeat[0] = val * (m_TerrainSize / 1024.0f)* (m_distanceX / 32);
}

void TerrainV2::SetTextureLayer_2_Repeat(int val)
{
	m_LayerRepeat[1] = val * (m_TerrainSize / 1024.0f)* (m_distanceX / 32);
}

void TerrainV2::SetTextureLayer_3_Repeat(int val)
{
	m_LayerRepeat[2] = val * (m_TerrainSize / 1024.0f)* (m_distanceX / 32);
}

void TerrainV2::SetTextureLayer_4_Repeat(int val)
{
	m_LayerRepeat[3] = val * (m_TerrainSize / 1024.0f)* (m_distanceX / 32);
}

void TerrainV2::Release()
{
	delete[] m_TerrainHeights;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteTextures(1, &m_heightMapID);

	for (int i = 0; i < 4; i++)
	{
		if (m_LayerSet[i])
		{
			terrainLayers[i].Release();
		}
	}
	
	delete[] blendMapValuePtr;

	for (int i = 0; i < m_TileCountInRow; ++i)
	{
		for (int j = 0; j < m_TileCountInRow; ++j)
		{
			glDeleteTextures(1, &BlendMapArray[i * m_TileCountInRow + j]);
		}
	}
}

unsigned int TerrainV2::GetTerrainSize()
{
	return m_TerrainSize;
}

unsigned int TerrainV2::GetCellWidth()
{
	return m_distanceX;
}

void TerrainV2::SaveTerrain(std::string pathName, std::string fileName)
{
	FIBITMAP* image = FreeImage_AllocateT(FIT_UINT16, m_TerrainSize, m_TerrainSize);
	for (unsigned int y = 0; y < FreeImage_GetHeight(image); y++) 
	{
		unsigned short* bits = (unsigned short*)FreeImage_GetScanLine(image, y);
		
		for (unsigned int x = 0; x < FreeImage_GetWidth(image); x++)
		{
			bits[x] = (unsigned short)(m_TerrainHeights[x * m_TerrainSize + y] );
		}
		
	}

	
	std::string terrainFileName = pathName + std::string("terrain.tiff");
	FreeImage_Save(FIF_TIFF, image, terrainFileName.c_str());
	FreeImage_Unload(image);

	RGBQUAD rgbaVal;

	for (int m = 0; m < m_TileCountInRow; m++)
	{
		for (int n = 0; n < m_TileCountInRow; n++)
		{

			glBindTexture(GL_TEXTURE_2D, BlendMapArray[m* m_TileCountInRow + n]);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, blendMapValuePtr);
			FIBITMAP* bitmap = FreeImage_Allocate(BLENDMAP_SIZE, BLENDMAP_SIZE, 32);
			for (int i = 0; i < BLENDMAP_SIZE; i++)
			{
				for (int j = 0; j < BLENDMAP_SIZE; j++)
				{
					rgbaVal.rgbRed = blendMapValuePtr[i * BLENDMAP_SIZE + j].r;
					rgbaVal.rgbGreen= blendMapValuePtr[i * BLENDMAP_SIZE + j].g;
					rgbaVal.rgbBlue = blendMapValuePtr[i * BLENDMAP_SIZE + j].b;
					rgbaVal.rgbReserved = blendMapValuePtr[i * BLENDMAP_SIZE + j].a;
					FreeImage_SetPixelColor(bitmap, j, i, &rgbaVal);
				}
			}
			std::string blendMapFileName = pathName + "blendMap_" + std::to_string(m*m_TileCountInRow+n) + ".tga";
			FreeImage_Save(FIF_TARGA, bitmap, blendMapFileName.c_str());
			FreeImage_Unload(bitmap);
		}
	}
}

void TerrainV2::LoadBlendmaps(std::string path)
{
	blendMapValuePtr = new PixelRGBA[BLENDMAP_SIZE*BLENDMAP_SIZE];
	for (int m = 0; m < m_TileCountInRow * m_TileCountInRow; ++m)
	{
		std::string blenMapPath = path + "blendMap_" + std::to_string(m) + ".tga";
		int iBPP;
		FIBITMAP *dib1 = nullptr;


		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		FIBITMAP* dib(0);

		fif = FreeImage_GetFileType(blenMapPath.c_str(), 0);

		if (fif == FIF_UNKNOWN) 
			fif = FreeImage_GetFIFFromFilename(blenMapPath.c_str());

		if (fif == FIF_UNKNOWN)
			return;

		if (FreeImage_FIFSupportsReading(fif))
			dib = FreeImage_Load(fif, blenMapPath.c_str());
		if (!dib)
			return;
		
		BYTE* bDataPointer = FreeImage_GetBits(dib);

		
		iBPP = FreeImage_GetBPP(dib);
	
		if (bDataPointer == nullptr)
			return;

		glGenTextures(1, &BlendMapArray[m]);
		glBindTexture(GL_TEXTURE_2D, BlendMapArray[m]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BLENDMAP_SIZE, BLENDMAP_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, bDataPointer);
		glBindTexture(GL_TEXTURE_2D, 0);

		FreeImage_Unload(dib);
	}
}

void TerrainV2::SetCircleBrushSize(float size)
{
	m_CircleBrushSize = size;
}

void TerrainV2::ShowCircleBrush(bool val)
{
	m_showCircle = val;
}

int TerrainV2::GetEditBrushRadius()
{
	return m_EditBrushRadius;
}

int TerrainV2::GetPaintBrushRadius()
{
	return m_PaintBrushRadius;
}

int TerrainV2::GetTerrainCellSize()
{
	return m_distanceX;
}

unsigned int TerrainV2::GetLayerRepeat(unsigned int LayerIndex)
{
	return m_LayerRepeat[LayerIndex];
}

void TerrainV2::SetVisible(bool visible)
{
	m_bVisible = visible;
}

bool TerrainV2::IsVisible()
{
	return m_bVisible;
}

bool TerrainV2::GetHeightModified() const
{
	return m_terrainModified;
}

void TerrainV2::SetHeightModified(bool val)
{
	m_terrainModified = val;
}

unsigned int& TerrainV2::GetRenderedTileCount()
{
	return m_RenderedTiles;
}
