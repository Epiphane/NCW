// By Thomas Steinke

#pragma once

#include <mutex>

#include <BulletCollision/CollisionShapes/btConcaveShape.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

class VoxelHeightfieldTerrainShape : public btHeightfieldTerrainShape
{
public:
	VoxelHeightfieldTerrainShape(
		int heightStickWidth,
		int heightStickLength,
		const short* heightfieldData,
		btScalar heightScale,
		btScalar minHeight, 
		btScalar maxHeight
	);

	void SetVertex(int x, int y, btScalar height, btVector3& vertex) const;

	void processAllTriangles(btTriangleCallback * callback, const btVector3& aabbMin, const btVector3& aabbMax) const override;
	
	//debugging
	virtual const char* getName() const { return "VOXEL_HEIGHTFIELD"; }
};
