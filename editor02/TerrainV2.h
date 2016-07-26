#ifndef TERRAINV2_H
#define TERRAINV2_H
#define GLEW_STATIC
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>

#include <vector>
#include "Texture2D.h"
#include "Shader.h"
#include "BoundingBox.h"
#include "Frustum.h"


const unsigned int TILE_SIZE = 64;

class TerrainV2
{
public:
	TerrainV2(unsigned int terrainSize, unsigned int numCols, unsigned int numRows, float distanceX, float distanceZ);
	TerrainV2();
	bool Setup();
	bool Setup(std::string fileName, unsigned int numCols, unsigned int numRows, float distanceX, float distanceZ, std::vector<std::string> layers);
	
	bool TerrainV2::PickTerrain(int mouseX, int mouseY, glm::dmat4& view, glm::dmat4& projection, glm::tvec3<double>& intersectionPoint);
	void Draw(glm::mat4& view, glm::mat4& projection, glm::vec3& cameraPos, Frustum& frustum);
	void DrawWithShadows(glm::mat4& view, glm::mat4& projection, glm::vec3& cameraPos, Frustum& frustum);
	void ModifyHeight();
	void SetViewportSize(glm::tvec4<double> viewportSize);
	void ComputeTh();
	void ComputeNormals();
	
	void filter3x3();
	void PaintTerrain(float deltaTime);
	void SetEditBrushRadius(int val);
	void SetEditBrushStrength(int val);
	void SetEditBrushHeight(int val);
	void SetPaintBrushRadius(int val);
	void SetPaintBrushStrength(int val);
	void SetTerrainViewDistance(int val);
	void SetTerrainLayer(std::string path, unsigned int layerIndex);
	unsigned short* GetHeightData();
	float GetHeight(int x, int z);
	void SetWorkingLayer(unsigned int index);

	void SetTextureLayer_1_Repeat(int val);
	void SetTextureLayer_2_Repeat(int val);
	void SetTextureLayer_3_Repeat(int val);
	void SetTextureLayer_4_Repeat(int val);
	void Release();
	unsigned int GetTerrainSize();
	unsigned int GetCellWidth();
	void SaveTerrain(std::string pathName, std::string fileName);
	void LoadBlendmaps(std::string path);
	void SetCircleBrushSize(float size);
	void ShowCircleBrush(bool val);
	int GetEditBrushRadius();
	int GetPaintBrushRadius();
	int GetTerrainCellSize();
	unsigned int GetLayerRepeat(unsigned int LayerIndex);
	void SetVisible(bool visible);
	bool IsVisible();
	std::string m_TerrainLayerPath[4];
	bool GetHeightModified() const;
	void SetHeightModified(bool val);
	unsigned int& GetRenderedTileCount();

private:
	bool LoadShader();
	void GenerateHeightmap();
	void GenerateBlendmap();
	void GenerateBlendMaps();
	bool inBounds(int i, int j);
	float sampleHeight3x3(int i, int j);

	unsigned int m_TerrainSize;
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 texcoord;
		glm::vec3 normal;
	};

	struct PixelRGBA
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	PixelRGBA* blendMapValuePtr;

	struct TileData
	{
		glm::vec3 position;
		glm::vec3 center;
	};

	unsigned short* m_TerrainHeights;
	glm::mat4 m_terrainPosition;

	GLuint EBO;
	GLuint VBO;
	GLuint VAO;

	GLuint m_heightMapID;

	glm::mat4 worldM;

	unsigned int m_TerrainWidth;
	unsigned int m_TerrainDepth;

	unsigned int m_VertexCount;
	unsigned int m_numTriangles;

	unsigned int m_NumCols;
	unsigned int m_NumRows;
	float m_distanceX;
	float m_distanceZ;

	int m_TileCountInRow;

	// Shader vars
	Shader terrainShader;
	GLint worldLoc;
	GLint viewLoc;
	GLint projectionLoc;

	GLint texOffsetXLoc;
	GLint texOffsetYLoc;

	GLint circleCenterLoc;

	GLuint textureLayer_1_Loc;
	GLuint heightmap_Loc;
	GLuint DiffuseDirectionLoc;
	GLuint LightPositionLoc;
	GLuint brushSizeLoc;
	GLuint terrainSizeLoc;
	GLuint repeatTexcoordsLoc;

	GLuint RepeatLayer1Loc;
	GLuint RepeatLayer2Loc;
	GLuint RepeatLayer3Loc;
	GLuint RepeatLayer4Loc;
	GLuint showCircleBrushLoc;

	glm::tvec3<double> m_MousePosOnTerrain;
	glm::tvec4<double> viewport;
	std::vector<TileData> tilePositions;

	double texOffsetX;
	double texOffsetY;
	int m_TerrainRepeat;

	struct TileIndex
	{
		unsigned int i, j;
	};

	struct TerrainLayer
	{
		Texture2D* terrainLayers[4];
	};

	std::vector<TileIndex> visibleTileIndices;

	std::vector<Vertex> m_terrainVertices;
	std::vector<GLuint> m_terrainIndices;
	std::vector<BoundingBox> m_BoundingBoxes;
	std::vector<GLuint> BlendMapArray;
	


	Texture2D terrainLayers[4];
	GLuint terrainLayersLoc[4];
	GLuint blendMapLoc;
	GLuint m_blendMap;
	GLuint m_MacroTex;
	Texture2D macroTex;

	GLuint* valu;

	// Terrain edit brush
	int m_EditBrushRadius;
	int m_EditBrushStrength;
	int m_EditBrushHeight;

	int m_PaintBrushRadius;
	int m_PaintBrushStrength;

	int m_TerrainViewDistance;

	
	bool m_LayerSet[4];
	unsigned int m_WorkingLayerIndex;

	unsigned int m_LayerRepeat[4];
	
	float m_CircleBrushSize;
	bool m_showCircle;
	bool m_terrainModified;
	glm::vec3 difusseDirection;

	bool m_bVisible;
	unsigned int m_RenderedTiles;
};

#endif