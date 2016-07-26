#ifndef TERRAIN_ZONE_H
#define TERRAIN_ZONE_H
#define GLEW_STATIC
#include <GL/glew.h>
#include "BoundingBox.h"

enum ZoneNeighbour
{
	TOPLEFT,
	TOPRIGHT,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	BOTOMLEFT,
	BOTTOMRIGHT
};

struct NeighbourZones
{
	TerrainZone* zone[8];
};

class TerrainZone
{
public:
	void Draw();
	//void Set(set)
	//NeighbourZones nZones;
	GLuint VAO;
};

#endif 