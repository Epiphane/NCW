// By Thomas Steinke

#include <algorithm>
#include <cassert>
#include <functional>
#include <queue>

#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <Engine/Entity/Transform.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/Components/VoxModel.h>
#include <Shared/Systems/AnimationSystem.h>
#include <Shared/Systems/AnimationEventSystem.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/CombatSystem.h>
#include <Shared/Systems/FollowerSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/BulletPhysicsDebug.h>
#include <Shared/Systems/BulletPhysicsSystem.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/SimpleParticleSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>
#include <Shared/Systems/WalkSystem.h>
#include <Shared/Systems/WalkAnimationSystem.h>

#include <RGBDesignPatterns/Macros.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "World.h"

namespace CubeWorld
{

///
///
///
Chunk::Chunk(const ChunkCoords& coords)
    : mCoords(coords)
    , blocks(new Block[kChunkSize * kChunkSize * kChunkHeight])
{}

///
///
///
Chunk::~Chunk()
{}

///
///
///
int Chunk::GetHeight(int x, int z)
{
    for (int y = kChunkHeight - 1; y > 0; y--)
    {
        if (!Get(x, y, z).isEmpty)
        {
            return y;
        }
    }

    return 0;
}

///
///
///
Block& Chunk::Get(int x, int y, int z)
{
    int index = int((x + z * kChunkSize) * kChunkSize + y);
    return blocks.get()[index];
}

///
///
///
World::World()
{}

///
///
///
World::~World()
{}

///
///
///
void World::Build()
{
    mBuilder.SetSourceModule(mHeightmodule);
    mBuilder.SetDestNoiseMap(mHeightmap);
    mBuilder.SetDestSize(kChunkSize, kChunkSize);
    mBuilder.SetBounds(6, 10, 1, 5);
    mBuilder.Build();
}

///
///
///
Engine::Entity World::Create(int chunkX, int chunkY, int chunkZ, Engine::EntityManager& entities)
{
    noise::utils::NoiseMap map;
    noise::utils::NoiseMapBuilderPlane builder;

    builder.SetSourceModule(mHeightmodule);
    builder.SetDestNoiseMap(map);
    builder.SetDestSize(kChunkSize, kChunkSize);
    builder.SetBounds(chunkX, chunkX + 1, chunkZ, chunkZ + 1);
    builder.Build();

    Engine::Entity entity = entities.Create(
        float(chunkX) * kChunkSize,
        kChunkHeight / -2,
        float(chunkZ) * kChunkSize
    );

    Chunk& chunk = Get({chunkX, chunkY, chunkZ});
    std::vector<Voxel::Data> carpet;
    for (int x = 0; x < kChunkSize; ++x)
    {
        for (int z = 0; z < kChunkSize; ++z)
        {
            float elevation = 2 * map.GetValue(x, z);
            int height = int(std::floor((elevation + 1) * kChunkHeight / 8));

            for (int i = 0; i < height; ++i)
            {
                chunk.Get(x, i, z).isEmpty = false;
            }
        }
    }

    glm::vec4 DEEP(0, 0, 128, 1);
    glm::vec4 SHALLOW(0, 0, 255, 1);
    glm::vec4 SHORE(0, 128, 255, 1);
    glm::vec4 SAND(240, 240, 64, 1);
    glm::vec4 GRASS(32, 160, 0, 1);
    glm::vec4 DIRT(224, 224, 0, 1);
    glm::vec4 ROCK(128, 128, 128, 1);
    glm::vec4 SNOW(255, 255, 255, 1);

    const float d = 0.5f;
    const glm::vec3 d___{-d,-d,-d};
    const glm::vec3 d__0{-d,-d, 0};
    const glm::vec3 d__1{-d,-d, d};
    const glm::vec3 d_0_{-d, 0,-d};
    const glm::vec3 d_00{-d, 0, 0};
    const glm::vec3 d_01{-d, 0, d};
    const glm::vec3 d_1_{-d, d,-d};
    const glm::vec3 d_10{-d, d, 0};
    const glm::vec3 d_11{-d, d, d};

    const glm::vec3 d0__{ 0,-d,-d};
    const glm::vec3 d0_0{ 0,-d, 0};
    const glm::vec3 d0_1{ 0,-d, d};
    const glm::vec3 d00_{ 0, 0,-d};
    const glm::vec3 d000{ 0, 0, 0};
    const glm::vec3 d001{ 0, 0, d};
    const glm::vec3 d01_{ 0, d,-d};
    const glm::vec3 d010{ 0, d, 0};
    const glm::vec3 d011{ 0, d, d};

    const glm::vec3 d1__{ d,-d,-d};
    const glm::vec3 d1_0{ d,-d, 0};
    const glm::vec3 d1_1{ d,-d, d};
    const glm::vec3 d10_{ d, 0,-d};
    const glm::vec3 d100{ d, 0, 0};
    const glm::vec3 d101{ d, 0, d};
    const glm::vec3 d11_{ d, d,-d};
    const glm::vec3 d110{ d, d, 0};
    const glm::vec3 d111{ d, d, d};

    const glm::vec3 top{0,  1,  0};
    const glm::vec3 bottom{0, -1,  0};
    const glm::vec3 left{-1,  0,  0};
    const glm::vec3 right{1,  0,  0};
    const glm::vec3 front{0,  0,  1};
    const glm::vec3 back{0,  0, -1};

    std::vector<ShadedMesh::Point> vertices;
    std::vector<GLuint> indices;
    for (int x = 0; x < kChunkSize; ++x)
    {
        for (int z = 0; z < kChunkSize; ++z)
        {
            int height = chunk.GetHeight(x, z);

            for (int yDiff = -1; yDiff <= 0; ++yDiff)
            {
                int y = height + yDiff;
                float elevation = float(y) * 3 / kChunkHeight - 0.1f;

                //float elevation = 0.5f + 2 * float(map.GetValue(x, z));
                //elevation = std::floor(elevation * 30 + float(dy) * kChunkHeight / 8) / 30.0f;
                glm::vec4 source, dest;
                float start, end;
                if (elevation >= 0.75f) { source = ROCK; dest = SNOW; start = 0.75f; end = 1.0f; }
                else if (elevation >= 0.375f) { source = DIRT; dest = ROCK; start = 0.375f; end = 0.75f; }
                else if (elevation >= 0.125f) { source = GRASS; dest = DIRT; start = 0.125f; end = 0.375f; }
                else if (elevation >= 0.0625f) { source = SAND; dest = GRASS; start = 0.0625f; end = 0.125f; }
                else if (elevation >= 0.0f) { source = SHORE; dest = SAND; start = 0; end = 0.0625f; }
                else if (elevation >= -0.25f) { source = SHALLOW; dest = SHORE; start = -0.25f; end = 0; }
                else { source = DEEP; dest = SHALLOW; start = -1.0f; end = -0.25f; }
                float perc = (elevation - start) / (end - start);

                glm::vec3 position = glm::vec3(x, y, z);
                glm::vec4 color = glm::vec4(87, 239, 7, 1);
                color = dest * perc + source * (1 - perc);

                //Voxel::Side side = Voxel::All;
                //bool occupied[27];
                bool occupied[3][3][3];

                for (int dx = 0; dx <= 2; ++dx)
                {
                    int nx = x + dx - 1;
                    for (int dy = 0; dy <= 2; ++dy)
                    {
                        int ny = y + dy - 1;
                        for (int dz = 0; dz <= 2; ++dz)
                        {
                            int nz = z + dz - 1;

                            if (nx < 0 || nx >= kChunkSize ||
                                nz < 0 || nz >= kChunkSize ||
                                ny < 0 || ny >= kChunkHeight)
                            {
                                occupied[dx][dy][dz] = false;
                            }
                            else
                            {
                                occupied[dx][dy][dz] = !chunk.Get(nx, ny, nz).isEmpty;
                            }
                        }
                    }
                }

                uint8_t side = Voxel::All;
                if (occupied[0][1][1]) { side ^= Voxel::Left; }
                if (occupied[1][0][1]) { side ^= Voxel::Bottom; }
                if (occupied[1][1][0]) { side ^= Voxel::Back; }
                if (occupied[1][1][2]) { side ^= Voxel::Front; }
                if (occupied[1][2][1]) { side ^= Voxel::Top; }
                if (occupied[2][1][1]) { side ^= Voxel::Right; }

                auto MakeCenter = [&](glm::vec3 dposition, glm::vec3 normal)
                {
                    int blocked = 0;
                    int dx = int(dposition.x / d) + 1;
                    int dy = int(dposition.y / d) + 1;
                    int dz = int(dposition.z / d) + 1;
                    blocked += occupied[dx][dy][dz] ? 1 : 0;
                    if (normal.x == 0)
                    {
                        blocked += occupied[0][dy][dz] ? 1 : 0;
                        blocked += occupied[2][dy][dz] ? 1 : 0;
                    }
                    if (normal.y == 0)
                    {
                        blocked += occupied[dx][0][dz] ? 1 : 0;
                        blocked += occupied[dz][2][dz] ? 1 : 0;
                    }
                    if (normal.z == 0)
                    {
                        blocked += occupied[dx][dy][0] ? 1 : 0;
                        blocked += occupied[dz][dy][2] ? 1 : 0;
                    }

                    if (normal.y == 0)
                    {
                        blocked /= 2;
                    }

                    float opacity = 1.0f - std::min(float(blocked / 3.0f), 1.0f);
                    opacity = 1.0f;
                    GLuint ndx = GLuint(vertices.size());
                    vertices.push_back(ShadedMesh::Point{position + dposition, {color.x, color.y, color.z}, normal, opacity});
                    indices.push_back(ndx);
                    return ndx;
                };

                auto ComputeOcclusion = [&](glm::vec3 dposition, glm::vec3 normal)
                {
                    float blocked = 0;
                    float total = 0;
                    int dx = int(dposition.x / d) + 1;
                    int dy = int(dposition.y / d) + 1;
                    int dz = int(dposition.z / d) + 1;

                    if (normal.x != 0)
                    {
                        for (int dy_ = dy - 1; dy_ <= dy + 1; ++dy_)
                        {
                            if (dy_ < 0 || dy_ > 2) continue;
                            for (int dz_ = dz - 1; dz_ <= dz + 1; ++dz_)
                            {
                                if (dz_ < 0 || dz_ > 2) continue;
                                total++;
                                blocked += occupied[int(normal.x) + 1][dy_][dz_] ? 1 : 0;
                            }
                        }
                    }
                    else if (normal.y != 0)
                    {
                        for (int dx_ = dx - 1; dx_ <= dx + 1; ++dx_)
                        {
                            if (dx_ < 0 || dx_ > 2) continue;
                            for (int dz_ = dz - 1; dz_ <= dz + 1; ++dz_)
                            {
                                if (dz_ < 0 || dz_ > 2) continue;
                                total++;
                                blocked += occupied[dx_][int(normal.y) + 1][dz_] ? 1 : 0;
                            }
                        }
                    }
                    else if (normal.z != 0)
                    {
                        for (int dx_ = dx - 1; dx_ <= dx + 1; ++dx_)
                        {
                            for (int dy_ = dy - 1; dy_ <= dy + 1; ++dy_)
                            {
                                if (dy_ < 0 || dy_ > 2) continue;
                                total++;
                                blocked += occupied[dx_][dy_][int(normal.z) + 1] ? 1 : 0;
                            }
                        }
                    }

                    if (normal.y == 0)
                    {
                        blocked /= 2;
                    }

                    return 1.0f - std::min(blocked / total, 1.0f);
                };

                auto MakePoint = [&](glm::vec3 dposition, glm::vec3 normal, float opacity)
                {
                    glm::vec3 col{color.x, color.y, color.z};

                    GLuint ndx = GLuint(vertices.size());
                    vertices.push_back(ShadedMesh::Point{position + dposition, col, normal, opacity});
                    indices.push_back(ndx);
                    return ndx;
                };

                // Top
                if (side & Voxel::Top)
                {
                    float o111 = ComputeOcclusion(d111, top);
                    float o11_ = ComputeOcclusion(d11_, top);
                    float o_1_ = ComputeOcclusion(d_1_, top);
                    float o_11 = ComputeOcclusion(d_11, top);
                    float o110 = (o111 + o11_) / 2.0f;
                    float o011 = (o111 + o_11) / 2.0f;
                    float o_10 = (o_11 + o_1_) / 2.0f;
                    float o01_ = (o11_ + o_1_) / 2.0f;

                    MakePoint({0, d, 0}, top, (o111 + o11_ + o_1_ + o_11) / 4.0f);
                    GLuint begin = MakePoint(d111, top, o111);
                    MakePoint(d110, top, o110);
                    MakePoint(d11_, top, o11_);
                    MakePoint(d01_, top, o01_);
                    MakePoint(d_1_, top, o_1_);
                    MakePoint(d_10, top, o_10);
                    MakePoint(d_11, top, o_11);
                    MakePoint(d011, top, o011);
                    indices.push_back(begin);
                    indices.push_back(kPrimitiveRestart);
                }

                // Bottom
                if (side & Voxel::Bottom)
                {
                    float o1_1 = ComputeOcclusion(d1_1, bottom);
                    float o1__ = ComputeOcclusion(d1__, bottom);
                    float o___ = ComputeOcclusion(d___, bottom);
                    float o__1 = ComputeOcclusion(d__1, bottom);
                    float o1_0 = (o1_1 + o1__) / 2.0f;
                    float o0_1 = (o1_1 + o__1) / 2.0f;
                    float o__0 = (o__1 + o___) / 2.0f;
                    float o0__ = (o1__ + o___) / 2.0f;

                    MakePoint({0,-d, 0}, bottom, (o1_1 + o1__ + o___ + o__1) / 4.0f);
                    GLuint begin = MakePoint(d1_1, bottom, o1_1);
                    MakePoint(d0_1, bottom, o0_1);
                    MakePoint(d__1, bottom, o__1);
                    MakePoint(d__0, bottom, o__0);
                    MakePoint(d___, bottom, o___);
                    MakePoint(d0__, bottom, o0__);
                    MakePoint(d1__, bottom, o1__);
                    MakePoint(d1_0, bottom, o1_0);
                    indices.push_back(begin);
                    indices.push_back(kPrimitiveRestart);
                }

                // Left (x = -1)
                if (side & Voxel::Left)
                {
                    float o_11 = ComputeOcclusion(d_11, left);
                    float o_1_ = ComputeOcclusion(d_1_, left);
                    float o___ = ComputeOcclusion(d___, left);
                    float o__1 = ComputeOcclusion(d__1, left);
                    float o_10 = (o_11 + o_1_) / 2.0f;
                    float o_01 = (o_11 + o__1) / 2.0f;
                    float o__0 = (o__1 + o___) / 2.0f;
                    float o_0_ = (o_1_ + o___) / 2.0f;

                    MakePoint({-d, 0, 0}, left, (o_11 + o_1_ + o___ + o__1) / 4.0f);
                    GLuint begin = MakePoint(d_11, left, o_11);
                    MakePoint(d_10, left, o_10);
                    MakePoint(d_1_, left, o_1_);
                    MakePoint(d_0_, left, o_0_);
                    MakePoint(d___, left, o___);
                    MakePoint(d__0, left, o__0);
                    MakePoint(d__1, left, o__1);
                    MakePoint(d_01, left, o_01);
                    indices.push_back(begin);
                    indices.push_back(kPrimitiveRestart);
                }

                // Right (x = 1)
                if (side & Voxel::Right)
                {
                    float o111 = ComputeOcclusion(d111, right);
                    float o11_ = ComputeOcclusion(d11_, right);
                    float o1__ = ComputeOcclusion(d1__, right);
                    float o1_1 = ComputeOcclusion(d1_1, right);

                    float o110 = (o111 + o11_) / 2.0f;
                    float o101 = (o111 + o1_1) / 2.0f;
                    float o1_0 = (o1_1 + o1__) / 2.0f;
                    float o10_ = (o11_ + o1__) / 2.0f;

                    MakePoint({d, 0, 0}, right, (o111 + o11_ + o1__ + o1_1) / 4.0f);
                    GLuint begin = MakePoint(d111, right, o111);
                    MakePoint(d101, right, o101);
                    MakePoint(d1_1, right, o1_1);
                    MakePoint(d1_0, right, o1_0);
                    MakePoint(d1__, right, o1__);
                    MakePoint(d10_, right, o10_);
                    MakePoint(d11_, right, o11_);
                    MakePoint(d110, right, o110);
                    indices.push_back(begin);
                    indices.push_back(kPrimitiveRestart);
                }

                // Back (z = -1)
                if (side & Voxel::Back)
                {
                    float o1__ = ComputeOcclusion(d1__, back);
                    float o11_ = ComputeOcclusion(d11_, back);
                    float o___ = ComputeOcclusion(d___, back);
                    float o_1_ = ComputeOcclusion(d_1_, back);
                    float o10_ = (o11_ + o1__) / 2.0f;
                    float o0__ = (o1__ + o___) / 2.0f;
                    float o_0_ = (o___ + o_1_) / 2.0f;
                    float o01_ = (o_1_ + o11_) / 2.0f;

                    MakePoint({0, 0, -d}, back, (o___ + o11_ + o1__ + o_1_) / 4.0f);
                    GLuint begin = MakePoint(d11_, back, o11_);
                    MakePoint(d10_, back, o10_);
                    MakePoint(d1__, back, o1__);
                    MakePoint(d0__, back, o0__);
                    MakePoint(d___, back, o___);
                    MakePoint(d_0_, back, o_0_);
                    MakePoint(d_1_, back, o_1_);
                    MakePoint(d01_, back, o01_);
                    indices.push_back(begin);
                    indices.push_back(kPrimitiveRestart);
                }

                // Front (z = 1)
                if (side & Voxel::Front)
                {
                    float o1_1 = ComputeOcclusion(d1_1, front);
                    float o111 = ComputeOcclusion(d111, front);
                    float o__1 = ComputeOcclusion(d__1, front);
                    float o_11 = ComputeOcclusion(d_11, front);
                    float o101 = (o111 + o1_1) / 2.0f;
                    float o0_1 = (o1_1 + o__1) / 2.0f;
                    float o_01 = (o__1 + o_11) / 2.0f;
                    float o011 = (o_11 + o111) / 2.0f;

                    MakePoint({0, 0, d}, front, (o__1 + o111 + o1_1 + o_11) / 4.0f);
                    GLuint begin = MakePoint(d111, front, o111);
                    MakePoint(d011, front, o011);
                    MakePoint(d_11, front, o_11);
                    MakePoint(d_01, front, o_01);
                    MakePoint(d__1, front, o__1);
                    MakePoint(d0_1, front, o0_1);
                    MakePoint(d1_1, front, o1_1);
                    MakePoint(d101, front, o101);
                    indices.push_back(begin);
                    indices.push_back(kPrimitiveRestart);
                }
            }
        }
    }

    // Create mesh from carpet.
    //std::vector<ShadedMesh::Point> vertices;
    //std::vector<GLuint> indices;
    /*
    for (const Voxel::Data& voxel : carpet)
    {
        float o000 = 1.0f - float((voxel.occlusion >> 28) & 0xf) / 16.0f;
        float o001 = 1.0f - float((voxel.occlusion >> 24) & 0xf) / 16.0f;
        float o010 = 1.0f - float((voxel.occlusion >> 20) & 0xf) / 16.0f;
        float o011 = 1.0f - float((voxel.occlusion >> 16) & 0xf) / 16.0f;
        float o100 = 1.0f - float((voxel.occlusion >> 12) & 0xf) / 16.0f;
        float o101 = 1.0f - float((voxel.occlusion >>  8) & 0xf) / 16.0f;
        float o110 = 1.0f - float((voxel.occlusion >>  4) & 0xf) / 16.0f;
        float o111 = 1.0f - float((voxel.occlusion      ) & 0xf) / 16.0f;

        const glm::vec3 color = voxel.color / 255.0f;
        const glm::vec3 p000 = voxel.position + d000;
        const glm::vec3 p001 = voxel.position + d001;
        const glm::vec3 p010 = voxel.position + d010;
        const glm::vec3 p011 = voxel.position + d011;
        const glm::vec3 p100 = voxel.position + d100;
        const glm::vec3 p101 = voxel.position + d101;
        const glm::vec3 p110 = voxel.position + d110;
        const glm::vec3 p111 = voxel.position + d111;

        GLuint i;

        // Top
        i = (GLuint)vertices.size();
        vertices.push_back(ShadedMesh::Point{p111, color, top, o111});
        vertices.push_back(ShadedMesh::Point{p110, color, top, o110});
        vertices.push_back(ShadedMesh::Point{p010, color, top, o010});
        vertices.push_back(ShadedMesh::Point{p011, color, top, o011});
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(kPrimitiveRestart);

        // Bottom
        i = (GLuint)vertices.size();
        vertices.push_back(ShadedMesh::Point{p101, color, bottom, o101});
        vertices.push_back(ShadedMesh::Point{p001, color, bottom, o001});
        vertices.push_back(ShadedMesh::Point{p000, color, bottom, o000});
        vertices.push_back(ShadedMesh::Point{p100, color, bottom, o100});
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(kPrimitiveRestart);

        // Left (x = -1)
        i = (GLuint)vertices.size();
        vertices.push_back(ShadedMesh::Point{p011, color, left, o011});
        vertices.push_back(ShadedMesh::Point{p010, color, left, o010});
        vertices.push_back(ShadedMesh::Point{p000, color, left, o000});
        vertices.push_back(ShadedMesh::Point{p001, color, left, o001});
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(kPrimitiveRestart);

        // Right (x = 1)
        i = (GLuint)vertices.size();
        vertices.push_back(ShadedMesh::Point{p111, color, right, o111});
        vertices.push_back(ShadedMesh::Point{p101, color, right, o101});
        vertices.push_back(ShadedMesh::Point{p100, color, right, o100});
        vertices.push_back(ShadedMesh::Point{p110, color, right, o110});
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(kPrimitiveRestart);

        // Back (z = -1)
        i = (GLuint)vertices.size();
        vertices.push_back(ShadedMesh::Point{p110, color, back, o110});
        vertices.push_back(ShadedMesh::Point{p100, color, back, o100});
        vertices.push_back(ShadedMesh::Point{p000, color, back, o000});
        vertices.push_back(ShadedMesh::Point{p010, color, back, o010});
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(kPrimitiveRestart);

        // Front (z = 1)
        i = (GLuint)vertices.size();
        vertices.push_back(ShadedMesh::Point{p111, color, front, o111});
        vertices.push_back(ShadedMesh::Point{p011, color, front, o011});
        vertices.push_back(ShadedMesh::Point{p001, color, front, o001});
        vertices.push_back(ShadedMesh::Point{p101, color, front, o101});
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(kPrimitiveRestart);
    }
    */

    entity.Add<ShadedMesh>(std::move(vertices), std::move(indices));
    entity.Get<ShadedMesh>()->renderType = GL_TRIANGLE_FAN;

    /*
    glm::vec3 origin{1, 1, 1};
    glm::vec3 o000 = 15.f / 16.f * origin;
    glm::vec3 o001 = 13.f / 16.f * origin;
    glm::vec3 o010 = 11.f / 16.f * origin;
    glm::vec3 o011 = 9.f / 16.f * origin;
    glm::vec3 o100 = 7.f / 16.f * origin;
    glm::vec3 o101 = 5.f / 16.f * origin;
    glm::vec3 o110 = 3.f / 16.f * origin;
    glm::vec3 o111 = 1.f / 16.f * origin;

    const float d = 0.5f;
    glm::vec3 p000{-d, -d, -d};
    glm::vec3 p001{-d, -d, d};
    glm::vec3 p010{-d, d, -d};
    glm::vec3 p011{-d, d, d};
    glm::vec3 p100{d, -d, -d};
    glm::vec3 p101{d, -d, d};
    glm::vec3 p110{d, d, -d};
    glm::vec3 p111{d, d, d};

    std::vector<glm::vec3> points{
        p111,
        (p111 + p110) / 2.0f,
        (p111 + p010) / 2.0f,
        (p111 + p011) / 2.0f,
        (p111 + p001) / 2.0f,
        (p111 + p101) / 2.0f,
        (p111 + p100) / 2.0f,

        // 7
        p110,
        (p110 + p010) / 2.0f,
        (p110 + p000) / 2.0f,
        (p110 + p100) / 2.0f,

        // 11
        p010,
        (p010 + p000) / 2.0f,
        (p010 + p001) / 2.0f,
        (p010 + p011) / 2.0f,

        // 15
        p011,
        (p011 + p001) / 2.0f,

        // 17
        p101,
        (p101 + p100) / 2.0f,
        (p101 + p000) / 2.0f,
        (p101 + p001) / 2.0f,

        // 21
        p001,
        (p001 + p000) / 2.0f,

        // 23
        p000,
        (p000 + p100) / 2.0f,

        // 25
        p100,
    };
    std::vector<glm::vec3> colors{
        o111,
        (o111 + o110) / 2.0f,
        (o111 + o010) / 2.0f,
        (o111 + o011) / 2.0f,
        (o111 + o001) / 2.0f,
        (o111 + o101) / 2.0f,
        (o111 + o100) / 2.0f,

        o110,
        (o110 + o010) / 2.0f,
        (o110 + o000) / 2.0f,
        (o110 + o100) / 2.0f,

        o010,
        (o010 + o000) / 2.0f,
        (o010 + o001) / 2.0f,
        (o010 + o011) / 2.0f,

        o011,
        (o011 + o001) / 2.0f,

        o101,
        (o101 + o100) / 2.0f,
        (o101 + o000) / 2.0f,
        (o101 + o001) / 2.0f,

        o001,
        (o001 + o000) / 2.0f,

        o000,
        (o000 + o100) / 2.0f,

        o100,
    };

    GLuint PRIMITIVE_RESTART = 12345;
    std::vector<GLuint> indices{
        0, 1, 2, 3, 4, 5, 6, 1, PRIMITIVE_RESTART,
        7, 1, 6, 10, 9, 8, 2, 1, PRIMITIVE_RESTART,
        11, 2, 8, 9, 12, 13, 14, 2, PRIMITIVE_RESTART,
        15, 3, 2, 14, 13, 16, 4, 3, PRIMITIVE_RESTART,
        17, 18, 6, 5, 4, 20, 19, 18, PRIMITIVE_RESTART,
        21, 20, 4, 16, 13, 22, 19, 20, PRIMITIVE_RESTART,
        23, 19, 22, 13, 12, 9, 24, 19, PRIMITIVE_RESTART,
        25, 6, 18, 19, 24, 9, 10, 6, PRIMITIVE_RESTART
    };
    std::vector<glm::vec3> offsets;
    offsets.push_back(glm::vec3(0));

    entity.Add<Index3DRender>(std::move(points), std::move(colors), std::move(indices), std::move(offsets));
    entity.Get<Index3DRender>()->renderType = GL_TRIANGLE_FAN;
    */

    //entity.Add<VoxelRender>(std::move(carpet));

    /*
    auto makeCollider = [&](int i, int j, int height, int width, int length) {
        Engine::Entity collider = entities.Create(i - size + float(width - 1) / 2, float(height), j - size + float(length - 1) / 2);
        collider.Add<BulletPhysics::StaticBody>(glm::vec3(width, 1, length));

        for (int x = i; x < i + width; ++x)
        {
            for (int y = j; y < j + length; ++y)
            {
                assert(!used[index(x, y)]);
                if (heights[index(x, y)] == height) used[index(x, y)] = true;
            }
        }
    };

    for (int x = 0; x < kChunkSize; ++x)
    {
        for (int z = 0; z < kChunkSize; ++z)
        {
            int32_t height = chunk->GetHeight(x, z);

            // Attempt 3: Same as 2, but allow blocks to sit under each other
            // Result: Generated 933 blocks
            int width = 0, length = 0;
            int nextWidth = 1, nextLength = 1;
            while (nextWidth > width || nextLength > length)
            {
                width = nextWidth++;
                length = nextLength++;

                // Don't exceed the bounds of the chunk
                if (x + width >= kChunkSize)
                {
                    --nextWidth;
                }
                else
                {
                    for (int n = 0; n < length; ++n)
                    {
                        if (chunk->GetHeight(x + width,
                            used[index(i + nextWidth - 1, j + n)] ||
                            heights[index(i + nextWidth - 1, j + n)] < height
                            )
                        {
                            --nextWidth;
                            break;
                        }
                    }
                }

                // Don't exceed the bounds of the chunk
                if (z + length >= kChunkSize)
                {
                    --nextLength;
                }
                else
                {
                    for (int n = 0; n < nextWidth; ++n)
                    {
                        if (
                            used[index(i + n, j + nextLength - 1)] ||
                            heights[index(i + n, j + nextLength - 1)] < height
                            )
                        {
                            --nextLength;
                            break;
                        }
                    }
                }
            }

            makeCollider(i, j, height, width, length);
        }
    }
    */

    return entity;
}

///
///
///
Chunk& World::Get(const ChunkCoords& coords)
{
    if (mChunks.count(coords) == 0)
    {
        mChunks.emplace(coords, coords);
    }

    return mChunks.at(coords);
}

}; // namespace CubeWorld
