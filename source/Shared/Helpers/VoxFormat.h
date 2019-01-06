// By Thomas Steinke

#pragma once

#include <memory>
#include <unordered_map>

#include <Engine/Core/Maybe.h>
#include "../Voxel.h"

namespace CubeWorld
{

namespace Voxel
{

struct VoxModelData {
   struct TransformNode {
      int32_t id;
      std::string name = "";
      bool hidden = false;
      int32_t child;
      int32_t layer;

      // Frame attributes
      int8_t rotate = 0b0000100;
      int32_t translate[3] = {0, 0, 0};
   };

   struct GroupNode {
      int32_t id;
      std::vector<int32_t> children;
   };

   struct ShapeNode {
      int32_t id;
      int32_t model;
   };

   struct Layer {
      int32_t id;
      std::string name = "";
      bool hidden = false;
   };

   struct Material {
      int32_t id;
      enum { Diffuse, Metal, Glass, Emit } type;
      float weight;
      float rough;
      float spec;
      float ior;
      float att;
      float flux;
      bool plastic;
      bool ldr;

      const static Material Default;
   };

   struct Model {
      uint32_t width; // x
      uint32_t height; // y
      uint32_t length; // z
      std::vector<uint32_t> voxels; // x, y, z, colorIndex are packed into one uint32_t
   };

   struct Render {
      // _type: _inf
      struct Sun {
         float intensity = 0.6f;             // _i
         uint8_t color[3] = {255, 255, 255}; // _k
         int8_t angle[2] = {50, 50};         // _angle
         float area = 0.07f;                 // _area
         bool disk = false;                  // _disk
      };

      // _type: _uni
      struct UniformSky {
         float intensity = 0.7f;             // _i
         uint8_t color[3] = {255, 255, 255}; // _k
      };

      // _type: _atm
      struct AtmosphericSky {
         float rayleighDensity = 0.4f;                // _ray_d
         uint8_t rayleighColor[3] = {45, 104, 255};   // _ray_k
         float mieDensity = 0.4f;                     // _mie_d
         float miePhase = 0.85f;                      // _mie_g
         uint8_t mieColor[3] = {255, 255, 255};       // _mie_k
         float ozoneDensity = 0;                      // _o3_d
         uint8_t ozoneColor[3] = {105, 255, 110};     // _o3_k
      };

      // _type: _fog_uni
      struct Fog {
         float density = 0;                  // _d
         uint8_t color[3] = {255, 255, 255}; // _k
      };

      // _type: _lens
      struct Lens {
         int32_t fov = 45;              // _fov
         float depthOfField = 0.25f;    // _dof
         float exposure = 1;            // _expo
         bool vignette = false;         // _vig
         bool stereographics = false;   // _sg
         uint8_t bladeNumber = 0;       // _blade_n
         int16_t bladeRotation = 0;     // _blade_r
      };

      // _type: _bloom
      struct Bloom {
         float mix = 0.5;        // _mix
         float scale = 0;        // _scale
         float aspect = 0;       // _aspect
         float threshold = 1;    // _threshold
      };

      // _type: _tone
      struct Tone {
         bool aces = false;   // _aces
         float gamma = 2.2f;  // _gam
      };

      // _type: _ground
      struct Ground {
         uint8_t color[3] = {80, 80, 80}; // _color
         float horizon = 0.1f;            // _hor
      };

      // _type: _bg
      struct Background {
         uint8_t color[3] = {0, 0, 0}; // _color
      };

      // _type: _edge
      struct Edge {
         uint8_t color[3] = {0, 0, 0}; // _color
         float width = 0.2f;           // _width
      };

      // _type: _grid
      struct Grid {
         uint8_t color[3] = {0, 0, 0}; // _color
         uint32_t spacing = 1;         // _spacing
         float width = 0.05f;          // _width
         bool onGround = false;        // _display
      };

      // _type: _setting
      struct Settings {
         bool ground = true;           // _ground
         bool shadow = true;           // _sw
         bool antialias = true;        // _aa
         bool grid = false;            // _grid
         bool edge = false;            // _edge
         bool background = false;      // _bg_c
         bool bgTransparent = false;   // _ba_a
         uint8_t scale[3] = {1, 1, 1}; // _scale
      };
   };

   std::vector<Model> models;
   uint32_t palette[256]; // r, g, b, a packed

   std::unordered_map<int32_t, TransformNode> transforms;
   std::unordered_map<int32_t, GroupNode> groups;
   std::unordered_map<int32_t, ShapeNode> shapes;
   std::vector<Layer> layers;
   std::vector<Material> materials;
   
   // Render options
   Render::Sun sun;
   Render::UniformSky sky;
   Render::AtmosphericSky atm;
   Render::Fog fog;
   Render::Lens lens;
   Render::Bloom bloom;
   Render::Tone tone;
   Render::Ground ground;
   Render::Background bg;
   Render::Edge edge;
   Render::Grid grid;
   Render::Settings settings;
};

//
// Contains a single VBO for all the shapes in a multi-shape model, as well as information
// about rendering them all.
//
class VoxModel {
public:
   struct Part {
      std::string name;
      bool tintable;
      //
      // To draw this part, draw the Model's VBO using indices [start, start + size)
      // Size 0 represents an ethereal part, used for skeleton reasoning but not drawn.
      //
      uint32_t start, size;
      // Transformation relative to the root of the model.
      glm::mat4 transform;
   };

public:
   //
   // Creation functions.
   //
   VoxModel() 
      : parts{}
      , partLookup{}
      , vbo(Engine::Graphics::VBO::Vertices)
   {};

public:
   // Member data
   std::vector<Part> parts;
   std::unordered_map<std::string, size_t> partLookup;

   Engine::Graphics::VBO vbo;
};

//
// Documentation:
// https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
//
class VoxFormat {
public:
   //
   // Load performs a read, and then buffers that data to the GPU as well.
   // Not supported for multi-model skeletons, use the VoxModel component for that.
   // Also soft-deprecated, I want to use VoxModel for everything.
   //
   static Model* Load(const std::string& path, bool tintable);
   static Maybe<VoxModel*> Load(const std::string& path);

   static Maybe<std::unique_ptr<VoxModelData>> ReadScene(const std::string& path);
   static Maybe<std::unique_ptr<ModelData>> Read(const std::string& path, bool tintable);
   static Maybe<void> Write(const std::string& path, const VoxModelData& model);
   static Maybe<void> Write(const std::string& path, const ModelData& model);

private:
   static std::unordered_map<std::string, std::unique_ptr<Model>> sDepModels;
   static std::unordered_map<std::string, std::unique_ptr<VoxModel>> sModels;
};

}; // namespace Voxel

}; // namespace CubeWorld
