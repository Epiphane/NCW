// By Thomas Steinke

#include <assert.h>

#include "VoxelHeightfieldTerrainShape.h"

VoxelHeightfieldTerrainShape::VoxelHeightfieldTerrainShape(
	int heightStickWidth,
	int heightStickLength, 
	const short* heightfieldData,
	btScalar heightScale, 
	btScalar minHeight, 
	btScalar maxHeight
)
	: btHeightfieldTerrainShape(
		heightStickWidth,
		heightStickLength, 
		heightfieldData,
		heightScale, 
		minHeight,
		maxHeight,
		1, /* upAxis */
		PHY_SHORT,
		false /* flipQuadEdges */
	)
{}

void VoxelHeightfieldTerrainShape::SetVertex(int x, int y, btScalar height, btVector3& vertex) const
{
	btAssert(x >= 0);
	btAssert(y >= 0);
	btAssert(x < m_heightStickWidth);
	btAssert(y < m_heightStickLength);

	switch (m_upAxis)
	{
	case 0:
		vertex.setValue(
			height - m_localOrigin.getX(),
			(-m_width / btScalar(2.0)) + x,
			(-m_length / btScalar(2.0)) + y);
		break;
	case 1:
		vertex.setValue(
			(-m_width / btScalar(2.0)) + x,
			height - m_localOrigin.getY(),
			(-m_length / btScalar(2.0)) + y);
		break;
	case 2:
		vertex.setValue(
			(-m_width / btScalar(2.0)) + x,
			(-m_length / btScalar(2.0)) + y,
			height - m_localOrigin.getZ());
		break;
	default:
		assert(false && "Bad up axis");
		break;
	}

	vertex *= m_localScaling;
}

/// process all triangles within the provided axis-aligned bounding box
void VoxelHeightfieldTerrainShape::processAllTriangles(btTriangleCallback* callback, const btVector3& aabbMin, const btVector3& aabbMax) const
{
	// scale down the input aabb's so they are in local (non-scaled) coordinates
	btVector3 localAabbMin = aabbMin * btVector3(1.f / m_localScaling[0], 1.f / m_localScaling[1], 1.f / m_localScaling[2]);
	btVector3 localAabbMax = aabbMax * btVector3(1.f / m_localScaling[0], 1.f / m_localScaling[1], 1.f / m_localScaling[2]);

	// account for local origin
	localAabbMin += m_localOrigin;
	localAabbMax += m_localOrigin;

	//quantize the aabbMin and aabbMax, and adjust the start/end ranges
	int quantizedAabbMin[3];
	int quantizedAabbMax[3];
	quantizeWithClamp(quantizedAabbMin, localAabbMin, 0);
	quantizeWithClamp(quantizedAabbMax, localAabbMax, 1);

	// expand the min/max quantized values
	// this is to catch the case where the input aabb falls between grid points!
	for (int i = 0; i < 3; ++i)
	{
		quantizedAabbMin[i]--;
		quantizedAabbMax[i]++;
	}

	int startX = 0;
	int endX = m_heightStickWidth - 1;
	int startJ = 0;
	int endJ = m_heightStickLength - 1;

	uint8_t ndx1 = 0;
	uint8_t ndx2 = 2;
	switch (m_upAxis)
	{
	case 0:
		ndx1 = 1;
		break;
	case 1:
		break;
	case 2:
		ndx2 = 1;
		break;
	default:
		assert(false && "Bad up axis");
		break;
	}

	if (quantizedAabbMin[ndx1] > startX)
	{
		startX = quantizedAabbMin[ndx1];
	}
	if (quantizedAabbMax[ndx1] < endX)
	{
		endX = quantizedAabbMax[ndx1];
	}
	if (quantizedAabbMin[ndx2] > startJ)
	{
		startJ = quantizedAabbMin[ndx2];
	}
	if (quantizedAabbMax[ndx2] < endJ)
	{
		endJ = quantizedAabbMax[ndx2];
	}

	for (int j = startJ; j < endJ; j++)
	{
		for (int x = startX; x < endX; x++)
		{
			btVector3 vertices[3];
			int indices[3] = { 0, 1, 2 };

			btScalar height = getRawHeightFieldValue(x, j);
			//btScalar height01 = getRawHeightFieldValue(x, j + 1);
			btScalar height_0 = x > 0 ? getRawHeightFieldValue(x - 1, j) : height;
			//btScalar height11 = getRawHeightFieldValue(x + 1, j + 1);

			//first triangle
			SetVertex(x, j, height, vertices[indices[0]]);
			SetVertex(x, j + 1, height, vertices[indices[1]]);
			SetVertex(x + 1, j + 1, height, vertices[indices[2]]);
			callback->processTriangle(vertices, 3 * x, j);

			//second triangle
			//  getVertex(x,j,vertices[0]);//already got this vertex before, thanks to Danny Chapman
			SetVertex(x + 1, j + 1, height, vertices[indices[1]]);
			SetVertex(x + 1, j, height, vertices[indices[2]]);
			callback->processTriangle(vertices, 3 * x + 1, j);

			if (std::abs(height - height_0) > 1)
			{
				SetVertex(x + 1, j, height_0, vertices[indices[1]]);
				callback->processTriangle(vertices, 3 * x + 2, j);

				SetVertex(x, j, height_0, vertices[indices[2]]);
				callback->processTriangle(vertices, 3 * x + 2, j);
			}
		}
	}
}
