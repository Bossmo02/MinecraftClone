#include "BlockMesh.h"



const int g_vertexDataLength = 4;


//std::vector<GLfloat> g_cubeVerticies =
//{
//	//-------pos--------
//	-0.0f, -0.0f, -0.0f,
//	 1.0f, -0.0f, -0.0f,
//	-0.0f,  1.0f, -0.0f,
//	 1.0f,  1.0f, -0.0f,
//
//	-0.0f, -0.0f,  1.0f,
//	 1.0f, -0.0f,  1.0f,
//	-0.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//
//	-0.0f,  1.0f,  1.0f,
//	-0.0f,  1.0f, -0.0f,
//	-0.0f, -0.0f, -0.0f,
//	-0.0f, -0.0f,  1.0f,
//
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f, -0.0f,
//	 1.0f, -0.0f, -0.0f,
//	 1.0f, -0.0f,  1.0f,
//
//	-0.0f, -0.0f, -0.0f,
//	 1.0f, -0.0f, -0.0f,
//	 1.0f, -0.0f,  1.0f,
//	-0.0f, -0.0f,  1.0f,
//
//	 1.0f,  1.0f, -0.0f,
//	 1.0f,  1.0f,  1.0f,
//	-0.0f,  1.0f,  1.0f,
//	-0.0f,  1.0f, -0.0f
//};




// DONE
std::vector<GLuint> g_indicesFaceBack =
{
	//Back
	0,	3,	1,
	0,	2,	3
};

std::vector<GLfloat> g_verticesBack =
{
	 0.0f,	0.0f,	0.0f,
	 1.0f,	0.0f,	0.0f,
	 0.0f,	1.0f,	0.0f,
	 1.0f,	1.0f,	0.0f
};


// DONE
std::vector<GLuint> g_indicesFaceFront =
{
	//Front
	0,	1,	3,
	0,	3,	2
};

std::vector<GLfloat> g_verticesFront =
{
	0.0f,	0.0f,	1.0f,
	1.0f,	0.0f,	1.0f,
	0.0f,	1.0f,	1.0f,
	1.0f,	1.0f,	1.0f
};


// DONE
std::vector<GLuint> g_indicesFaceLeft =
{
	// Left
	0,	1,	3,
	0,	3,	2
};

std::deque<GLfloat> g_verticesLeft =
{
	0.0f,	0.0f,	0.0f,
	0.0f,	0.0f,	1.0f,
	0.0f,	1.0f,	0.0f,
	0.0f,	1.0f,	1.0f
};


// DONE
std::vector<GLuint> g_indicesFaceRight =
{
	//Right
	0,	3,	1,
	0,	2,	3
};

std::vector<GLfloat> g_verticesRight =
{
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	1.0f,
	1.0f,	1.0f,	0.0f,
	1.0f,	1.0f,	1.0f
};


// DONE
std::vector<GLuint> g_indicesFaceBottom =
{
	// Bottom
	0,	1,	3,
	0,  3,	2
};

std::vector<GLfloat> g_verticesBottom =
{
	 0.0f,	0.0f,	0.0f,
	 1.0f,	0.0f,	0.0f,
	 0.0f,	0.0f,	1.0f,
	 1.0f,	0.0f,	1.0f
};


// DONE
std::vector<GLuint> g_indicesFaceTop =
{
	// Top
	0,	3,	1,
	0,	2,	3
};

std::vector<GLfloat> g_verticesTop =
{
	 0.0f,	1.0f,	0.0f,
	 1.0f,	1.0f,	0.0f,
	 0.0f,	1.0f,	1.0f,
	 1.0f,	1.0f,	1.0f
};


GLfloat g_lightLevelTop		= 0.9f;
GLfloat g_lightLevelSide	= 0.6f;
GLfloat g_lightLevelBottom	= 0.4f;


int g_singleVBOLightLevelTop	= 20;
int g_singleVBOLightLevelSide	= 10;
int g_singleVBOLightLevelBottom = 5;


BlockMeshData getBlockBackMeshData(int indicesIndex)
{
	BlockMeshData data;

	for (size_t i = 0; i < g_indicesFaceBack.size(); ++i)
		data.indices.push_back(g_indicesFaceBack[i] + (indicesIndex * g_vertexDataLength));

	for (size_t i = 0; i < g_verticesBack.size(); ++i)
		data.vertices.push_back(g_verticesBack[i]);

	data.lightLevel = g_lightLevelSide;

	return data;
}

BlockMeshData getBlockFrontMeshData(int indicesIndex)
{
	BlockMeshData data;

	for (size_t i = 0; i < g_indicesFaceFront.size(); ++i)
		data.indices.push_back(g_indicesFaceFront[i] + (indicesIndex * g_vertexDataLength));

	for (size_t i = 0; i < g_verticesFront.size(); ++i)
		data.vertices.push_back(g_verticesFront[i]);

	data.lightLevel = g_lightLevelSide;

	return data;
}

BlockMeshData getBlockLeftMeshData(int indicesIndex)
{
	BlockMeshData data;

	for (size_t i = 0; i < g_indicesFaceLeft.size(); ++i)
		data.indices.push_back(g_indicesFaceLeft[i] + (indicesIndex * g_vertexDataLength));

	for (size_t i = 0; i < g_verticesLeft.size(); ++i)
		data.vertices.push_back(g_verticesLeft[i]);

	data.lightLevel = g_lightLevelSide;

	return data;
}

BlockMeshData getBlockRightMeshData(int indicesIndex)
{
	BlockMeshData data;

	for (size_t i = 0; i < g_indicesFaceRight.size(); ++i)
		data.indices.push_back(g_indicesFaceRight[i] + (indicesIndex * g_vertexDataLength));

	for (size_t i = 0; i < g_verticesRight.size(); ++i)
		data.vertices.push_back(g_verticesRight[i]);

	data.lightLevel = g_lightLevelSide;

	return data;
}

BlockMeshData getBlockBottomMeshData(int indicesIndex)
{
	BlockMeshData data;

	for (size_t i = 0; i < g_indicesFaceBottom.size(); ++i)
		data.indices.push_back(g_indicesFaceBottom[i] + (indicesIndex * g_vertexDataLength));

	for (size_t i = 0; i < g_verticesBottom.size(); ++i)
		data.vertices.push_back(g_verticesBottom[i]);

	data.lightLevel = g_lightLevelBottom;

	return data;
}

BlockMeshData getBlockTopMeshData(int indicesIndex)
{
	BlockMeshData data;

	for (size_t i = 0; i < g_indicesFaceTop.size(); ++i)
		data.indices.push_back(g_indicesFaceTop[i] + (indicesIndex * g_vertexDataLength));

	for (size_t i = 0; i < g_verticesTop.size(); ++i)
		data.vertices.push_back(g_verticesTop[i]);

	data.lightLevel = g_lightLevelTop;

	return data;
}

std::deque<BlockMeshData> getAllBlockData(int indicesIndex)
{
	std::deque<BlockMeshData> allData;

	allData.push_back(getBlockBackMeshData(indicesIndex));
	allData.push_back(getBlockFrontMeshData(indicesIndex));
	allData.push_back(getBlockLeftMeshData(indicesIndex));
	allData.push_back(getBlockRightMeshData(indicesIndex));
	allData.push_back(getBlockBottomMeshData(indicesIndex));
	allData.push_back(getBlockTopMeshData(indicesIndex));

	return allData;
}
