// By Thomas Steinke

#if CUBEWORLD_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdio>
#include <fstream>
#include <glad/glad.h>
#include <vector>
#include <unordered_set>

#include <Engine/Core/Scope.h>
#include <Engine/Helpers/StringHelper.h>
#include <Engine/Logger/Logger.h>

#include "VoxFormat.h"

namespace CubeWorld
{

namespace Voxel
{

std::unordered_map<std::string, std::unique_ptr<Model>> VoxFormat::sModels;

const VoxModel::Material VoxModel::Material::Default = {
   0, // id
   VoxModel::Material::Diffuse, // type
   1, // weight
   0.1, // rough
   0.5, // spec
   0.3, // ior
   0, // att
   0, // flux
   false, // plastic
   false, // ldr
};

namespace
{

struct FileHeader {
   char id[4];
   int32_t version;
};

struct Chunk {
   constexpr static uint32_t MakeChunkID(const char id[])
   {
      return (id[3] << 24) | (id[2] << 16) | (id[1] << 8) | id[0];
   }

   inline static std::string ReadChunkID(uint32_t id)
   {
      char split[] = {
         static_cast<char>(id & 0xff),
         static_cast<char>(id >> 8 & 0xff),
         static_cast<char>(id >> 16 & 0xff),
         static_cast<char>(id >> 24 & 0xff),
      };
      return std::string(split, 4);
   }

   struct Header {
      uint32_t id; // char id[4];
      uint32_t length;
      uint32_t childLength;
   };

   Chunk() : header{0,0,0}, data{}, children{} {};
   Chunk(Chunk&& other)
      : header(other.header)
      , data(std::move(other.data))
      , children(std::move(other.children))
   {};
   
   Header header;
   std::vector<uint8_t> data;
   std::vector<Chunk> children;

public:
   std::string id() const { return ReadChunkID(header.id); }

public:
   const static uint32_t MAIN;
   const static uint32_t SIZE;
   const static uint32_t PACK;
   const static uint32_t XYZI;
   const static uint32_t RGBA;
   const static uint32_t LAYR;
   const static uint32_t MATL;
   const static uint32_t nTRN;
   const static uint32_t nGRP;
   const static uint32_t nSHP;
   const static uint32_t rOBJ;
};

const uint32_t Chunk::MAIN = MakeChunkID("MAIN");
const uint32_t Chunk::SIZE = MakeChunkID("SIZE");
const uint32_t Chunk::PACK = MakeChunkID("PACK");
const uint32_t Chunk::XYZI = MakeChunkID("XYZI");
const uint32_t Chunk::RGBA = MakeChunkID("RGBA");
const uint32_t Chunk::LAYR = MakeChunkID("LAYR");
const uint32_t Chunk::MATL = MakeChunkID("MATL");
const uint32_t Chunk::nTRN = MakeChunkID("nTRN");
const uint32_t Chunk::nGRP = MakeChunkID("nGRP");
const uint32_t Chunk::nSHP = MakeChunkID("nSHP");
const uint32_t Chunk::rOBJ = MakeChunkID("rOBJ");

struct PACK {
   uint32_t numModels;
};

struct SIZE {
   uint32_t width; // size x
   uint32_t length; // size z
   uint32_t height; // size y
};

struct XYZI {
   uint32_t numVoxels; // N
   // int32_t[4 * N] voxels; // (x, y, z, colorIndex)
};

struct RGBA {
   uint32_t values[256];
};

uint32_t default_palette[] = {
   0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
   0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
   0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
   0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
   0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
   0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
   0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
   0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
   0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
   0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
   0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
   0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
   0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
   0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
   0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
   0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
};

struct ROTATION {
   // bit | value
   // 0-1 : 1 : index of the non-zero entry in the first row
   // 2-3 : 2 : index of the non-zero entry in the second row
   // 4   : 0 : the sign in the first row (0 : positive; 1 : negative)
   // 5   : 1 : the sign in the second row (0 : positive; 1 : negative)
   // 6   : 1 : the sign in the third row (0 : positive; 1 : negative)
   uint8_t _r;
};

struct nTRN {
   struct Header {
      int32_t id;
   };
   // DICT attributes;
   struct Body {
      int32_t child;
      int32_t reserved = -1;
      int32_t layer;
      int32_t nFrames = 1;
   };
   // DICT frames;

   Header header;
   Body body;
};

struct nGRP {
   struct Header {
      int32_t id;
   };
   // DICT attributes;
   struct Body {
      int32_t children;
   };
   // std::vector<int32_t> children;

   Header header;
   Body body;
};

struct nSHP {
   struct Header {
      int32_t id;
   };
   // DICT attributes;
   struct Body {
      int32_t nModels = 1;
   };
   // std::vector<struct {
   //    int32_t id,
   //    DICT attributes
   // }> models;

   Header header;
   Body body;
};

struct MATL {
   int32_t id;
   // DICT properties;
};

struct LAYR {
   int32_t id;
   // DICT attributes;
   int32_t reserved = -1;
};

Maybe<Chunk> ReadChunk(FILE* f)
{
   Chunk chunk;

   size_t bytes = fread(&chunk.header, 1, sizeof(Chunk::Header), f);
   if (bytes != sizeof(Chunk::Header))
   {
      return Failure("Failed to read chunk header: only received %1 bytes out of %2.", bytes, sizeof(Chunk::Header));
   }

   chunk.data.resize(chunk.header.length);

   bytes = fread(&chunk.data[0], 1, chunk.header.length, f);
   if (bytes != chunk.header.length)
   {
      return Failure("Failed to read chunk data: only received %1 bytes out of %2.", bytes, chunk.header.length);
   }

   uint32_t read = 0;
   while (read < chunk.header.childLength)
   {
      Maybe<Chunk> child = ReadChunk(f);
      if (!child)
      {
         return child.Failure().WithContext("Failed reading child %1 of %2 chunk", chunk.children.size(), chunk.id());
      }
      read += sizeof(child->header) + child->header.length + child->header.childLength;
      chunk.children.push_back(std::move(*child));
   }

   return chunk;
}

std::string ParseString(int32_t*& data)
{
   int32_t size = *data++;
   int8_t* start = (int8_t*)data;
   int8_t* end = start + size;

   data = (int32_t*)end;
   return std::string(start, end);
}

int32_t* WriteString(int32_t* data, const std::string& string)
{
   *data++ = string.size();
   char* start = (char*)data;
   char* end = start + string.size();
   strncpy(start, string.c_str(), string.size());
   return (int32_t*)end;
}

std::vector<std::pair<std::string, std::string>> ParseDict(int32_t*& data)
{
   std::vector<std::pair<std::string, std::string>> result;
   int32_t nAttributes = *data++;
   while (nAttributes-- > 0)
   {
      std::string key = ParseString(data);
      std::string value = ParseString(data);
      result.push_back(std::make_pair(key, value));
   }
   return result;
}

int32_t* WriteDict(int32_t* data, const std::vector<std::pair<std::string, std::string>>& pairs)
{
   *data++ = pairs.size();
   for (const auto& pair : pairs)
   {
      data = WriteString(data, pair.first);
      data = WriteString(data, pair.second);
   }
   return data;
}

std::string ToShortString(float val)
{
   std::string result = std::to_string(val);
   result.erase(result.find_last_not_of('0') + 1, std::string::npos);
   result.erase(result.find_last_not_of('.') + 1, std::string::npos);
   return result;
}

Maybe<void> ParseChunk(VoxModel* model, const Chunk& chunk)
{
   int32_t* data = (int32_t*)&chunk.data[0];
   if (chunk.header.id == Chunk::SIZE)
   {
      if (model->models.size() > 0)
      {
         return Failure("Multi-model objects are not supported");
      }

      VoxModel::Model m;

      if (chunk.header.length != sizeof(SIZE))
      {
         return Failure("Chunk body was not %1 bytes (was %2 instead)", sizeof(SIZE), chunk.header.length);
      }
      SIZE* size = (SIZE*)data;
      m.width = size->width;
      m.height = size->height;
      m.length = size->length;

      model->models.push_back(m);

      data = (int32_t*)(&chunk.data[chunk.header.length]);
   }
   else if (chunk.header.id == Chunk::XYZI)
   {
      XYZI* xyzi = (XYZI*)data;
      size_t chunkSize = sizeof(XYZI) + sizeof(uint32_t) * xyzi->numVoxels;
      if (chunk.header.length != chunkSize)
      {
         return Failure("Chunk body was not %1 bytes (was %2 instead)", chunkSize, chunk.header.length);
      }

      model->models[model->models.size() - 1].voxels.assign(
         (uint32_t*)(xyzi + 1),
         (uint32_t*)data + chunk.header.length / sizeof(uint32_t));
      data = (int32_t*)(&chunk.data[chunk.header.length]);
   }
   else if (chunk.header.id == Chunk::nTRN)
   {
      VoxModel::TransformNode node;
      node.id = *data++;

      // Read node attributes
      auto attributes = ParseDict(data);
      for (const auto& keyval : attributes)
      {
         if (keyval.first == "_name")
         {
            node.name = keyval.second;
         }
         else if (keyval.first == "_hidden")
         {
            node.hidden = keyval.second == "1";
         }
         else
         {
            return Failure("Unrecognized attribute: %1=%2", keyval.first, keyval.second);
         }
      }

      nTRN::Body* body = (nTRN::Body*)data;
      if (body->reserved != -1)
      {
         return Failure("Reserved id was not -1 (was %1)", body->reserved);
      }
      if (body->nFrames != 1)
      {
         return Failure("Num frames was not 1 (was %1)", body->nFrames);
      }

      node.child = body->child;
      node.layer = body->layer;

      // Parse frame attributes
      data = (int32_t*)(body + 1);
      attributes = ParseDict(data);
      for (const auto& keyval : attributes)
      {
         if (keyval.first == "_r")
         {
            node.rotate = std::stoi(keyval.second);
         }
         else if (keyval.first == "_t")
         {
            std::vector<std::string> components = StringHelper::Split(keyval.second, ' ');
            if (components.size() != 3)
            {
               return Failure("Unexpected number of components: %1 must have 3 parts, not %2", keyval.second, components.size());
            }
            node.translate[0] = std::stoi(components[0]);
            node.translate[1] = std::stoi(components[1]);
            node.translate[2] = std::stoi(components[2]);
         }
         else
         {
            return Failure("Unrecognized frame attribute: %1=%2", keyval.first, keyval.second);
         }
      }

      model->transforms.emplace(node.id, node);
   }
   else if (chunk.header.id == Chunk::nGRP)
   {
      VoxModel::GroupNode node;
      node.id = *data++;

      // Read node attributes
      auto attributes = ParseDict(data);
      for (const auto& keyval : attributes)
      {
         return Failure("Unrecognized attribute: %1=%2", keyval.first, keyval.second);
      }

      nGRP::Body* body = (nGRP::Body*)data;
      data = (int32_t*)(body + 1);
      for (int c = 0; c < body->children; c ++)
      {
         node.children.push_back(*data++);
      }

      model->groups.emplace(node.id, node);
   }
   else if (chunk.header.id == Chunk::nSHP)
   {
      VoxModel::ShapeNode node;
      node.id = *data++;

      // Read node attributes
      auto attributes = ParseDict(data);
      for (const auto& keyval : attributes)
      {
         return Failure("Unrecognized attribute: %1=%2", keyval.first, keyval.second);
      }

      nSHP::Body* body = (nSHP::Body*)data;
      data = (int32_t*)(body + 1);

      if (body->nModels != 1)
      {
         return Failure("nSHP object must have only 1 model (value: %1)", body->nModels);
      }

      // Parse the one model
      node.model = *data++;

      // Read model attributes
      attributes = ParseDict(data);
      for (const auto& keyval : attributes)
      {
         return Failure("Unrecognized model attribute: %1=%2", keyval.first, keyval.second);
      }

      model->shapes.emplace(node.id, node);
   }
   else if (chunk.header.id == Chunk::LAYR)
   {
      VoxModel::Layer layer;
      layer.id = *data++;

      // Read layer attributes
      auto attributes = ParseDict(data);
      for (const auto& keyval : attributes)
      {
         if (keyval.first == "_name")
         {
            layer.name = keyval.second;
         }
         else if (keyval.first == "_hidden")
         {
            layer.hidden = keyval.second == "1";
         }
         else
         {
            return Failure("Unrecognized attribute: %1=%2", keyval.first, keyval.second);
         }
      }

      int32_t reserved = *data++;
      if (reserved != -1)
      {
         return Failure("Reserved ID was not -1 (was %1)", reserved);
      }

      model->layers.push_back(layer);
   }
   else if (chunk.header.id == Chunk::RGBA)
   {
      if (chunk.header.length != sizeof(model->palette))
      {
         return Failure("Chunk body was not %1 bytes (was %2 instead)", sizeof(model->palette), chunk.header.length);
      }

      memcpy(model->palette, data, sizeof(model->palette));
      data += sizeof(model->palette) / sizeof(int32_t);
   }
   else if (chunk.header.id == Chunk::MATL)
   {
      VoxModel::Material material;
      material.id = *data++;

      auto attributes = ParseDict(data);
      for (const auto& keyval : attributes)
      {
         if (keyval.first == "_type")
         {
            if (keyval.second == "_diffuse") { material.type = VoxModel::Material::Diffuse; }
            else if (keyval.second == "_metal") { material.type = VoxModel::Material::Metal; }
            else if (keyval.second == "_glass") { material.type = VoxModel::Material::Glass; }
            else if (keyval.second == "_emit") { material.type = VoxModel::Material::Emit; }
            else
            {
               return Failure("Unrecognized material type: %1", keyval.second);
            }
         }
         else if (keyval.first == "_weight")
         {
            material.weight = std::stof(keyval.second);
         }
         else if (keyval.first == "_rough")
         {
            material.rough = std::stof(keyval.second);
         }
         else if (keyval.first == "_spec")
         {
            material.spec = std::stof(keyval.second);
         }
         else if (keyval.first == "_ior")
         {
            material.ior = std::stof(keyval.second);
         }
         else if (keyval.first == "_att")
         {
            material.att = std::stof(keyval.second);
         }
         else if (keyval.first == "_flux")
         {
            material.flux = std::stof(keyval.second);
         }
         else if (keyval.first == "_plastic")
         {
            material.plastic = keyval.second == "1";
         }
         else if (keyval.first == "_ldr")
         {
            material.ldr = keyval.second == "1";
         }
         else
         {
            return Failure("Unknown material property: %1=%2", keyval.first, keyval.second);
         }
      }

      model->materials.push_back(material);
   }
   else if (chunk.header.id == Chunk::rOBJ)
   {
      // Render settings are ignored
      auto properties = ParseDict(data);
      if (properties.size() == 0)
      {
         return Failure("No properties provided for rOBJ");
      }
      if (properties[0].first != "_type")
      {
         return Failure("First key expected to be _type (was %1)", properties[0].first);
      }

      std::string type = properties[0].second;
      if (type == "_inf")
      {
         // Who needs flexibility?
         if (properties.size() != 6 ||
             properties[1].first != "_i" ||
             properties[2].first != "_k" ||
             properties[3].first != "_angle" ||
             properties[4].first != "_area" ||
             properties[5].first != "_disk")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         model->sun.intensity = std::stof(properties[1].second);
         std::vector<std::string> components = StringHelper::Split(properties[2].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->sun.color[0] = std::stoi(components[0]);
         model->sun.color[1] = std::stoi(components[1]);
         model->sun.color[2] = std::stoi(components[2]);
         components = StringHelper::Split(properties[3].second, ' ');
         if (components.size() != 2)
         {
            return Failure("Unexpected number of components: %1 must have 2 parts, not %2", properties[2].second, components.size());
         }
         model->sun.angle[0] = std::stoi(components[0]);
         model->sun.angle[1] = std::stoi(components[1]);
         model->sun.area = std::stof(properties[4].second);
         model->sun.disk = properties[5].second == "1";
      }
      else if (type == "_uni")
      {
         // Who needs flexibility?
         if (properties.size() != 3 ||
             properties[1].first != "_i" ||
             properties[2].first != "_k")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         model->sky.intensity = std::stof(properties[1].second);
         std::vector<std::string> components = StringHelper::Split(properties[2].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->sky.color[0] = std::stoi(components[0]);
         model->sky.color[1] = std::stoi(components[1]);
         model->sky.color[2] = std::stoi(components[2]);
      }
      else if (type == "_atm")
      {
         // Who needs flexibility?
         if (properties.size() != 8 ||
             properties[1].first != "_ray_d" ||
             properties[2].first != "_ray_k" ||
             properties[3].first != "_mie_d" ||
             properties[4].first != "_mie_k" ||
             properties[5].first != "_mie_g" ||
             properties[6].first != "_o3_d" ||
             properties[7].first != "_o3_k")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         model->atm.rayleighDensity = std::stof(properties[1].second);
         std::vector<std::string> components = StringHelper::Split(properties[2].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->atm.rayleighColor[0] = std::stoi(components[0]);
         model->atm.rayleighColor[1] = std::stoi(components[1]);
         model->atm.rayleighColor[2] = std::stoi(components[2]);

         model->atm.mieDensity = std::stof(properties[3].second);
         components = StringHelper::Split(properties[4].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[4].second, components.size());
         }
         model->atm.mieColor[0] = std::stoi(components[0]);
         model->atm.mieColor[1] = std::stoi(components[1]);
         model->atm.mieColor[2] = std::stoi(components[2]);
         model->atm.miePhase = std::stof(properties[5].second);

         model->atm.ozoneDensity = std::stof(properties[6].second);
         components = StringHelper::Split(properties[7].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[7].second, components.size());
         }
         model->atm.ozoneColor[0] = std::stoi(components[0]);
         model->atm.ozoneColor[1] = std::stoi(components[1]);
         model->atm.ozoneColor[2] = std::stoi(components[2]);
      }
      else if (type == "_fog_uni")
      {
         // Who needs flexibility?
         if (properties.size() != 3 ||
             properties[1].first != "_d" ||
             properties[2].first != "_k")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         model->fog.density = std::stof(properties[1].second);
         std::vector<std::string> components = StringHelper::Split(properties[2].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->fog.color[0] = std::stoi(components[0]);
         model->fog.color[1] = std::stoi(components[1]);
         model->fog.color[2] = std::stoi(components[2]);
      }
      else if (type == "_lens")
      {
         // Who needs flexibility?
         if (properties.size() != 8 ||
             properties[1].first != "_fov" ||
             properties[2].first != "_dof" ||
             properties[3].first != "_expo" ||
             properties[4].first != "_vig" ||
             properties[5].first != "_sg" ||
             properties[6].first != "_blade_n" ||
             properties[7].first != "_blade_r")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         model->lens.fov = std::stoi(properties[1].second);
         model->lens.depthOfField = std::stof(properties[2].second);
         model->lens.exposure = std::stof(properties[3].second);
         model->lens.vignette = properties[4].second == "1";
         model->lens.stereographics = properties[5].second == "1";
         model->lens.bladeNumber = std::stoi(properties[6].second);
         model->lens.bladeRotation = std::stoi(properties[7].second);
      }
      else if (type == "_bloom")
      {
         // Who needs flexibility?
         if (properties.size() != 5 ||
             properties[1].first != "_mix" ||
             properties[2].first != "_scale" ||
             properties[3].first != "_aspect" ||
             properties[4].first != "_threshold")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         model->bloom.mix = std::stof(properties[1].second);
         model->bloom.scale = std::stof(properties[2].second);
         model->bloom.aspect = std::stof(properties[3].second);
         model->bloom.threshold = std::stof(properties[4].second);
      }
      else if (type == "_tone")
      {
         // Who needs flexibility?
         if (properties.size() != 3 ||
             properties[1].first != "_aces" ||
             properties[2].first != "_gam")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         model->tone.aces = properties[1].second == "1";
         model->tone.gamma = std::stof(properties[2].second);
      }
      else if (type == "_ground")
      {
         // Who needs flexibility?
         if (properties.size() != 3 ||
             properties[1].first != "_color" ||
             properties[2].first != "_hor")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         std::vector<std::string> components = StringHelper::Split(properties[1].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->ground.color[0] = std::stoi(components[0]);
         model->ground.color[1] = std::stoi(components[1]);
         model->ground.color[2] = std::stoi(components[2]);
         model->ground.horizon = std::stof(properties[2].second);
      }
      else if (type == "_bg")
      {
         // Who needs flexibility?
         if (properties.size() != 2 ||
             properties[1].first != "_color")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         std::vector<std::string> components = StringHelper::Split(properties[1].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->bg.color[0] = std::stoi(components[0]);
         model->bg.color[1] = std::stoi(components[1]);
         model->bg.color[2] = std::stoi(components[2]);
      }
      else if (type == "_edge")
      {
         // Who needs flexibility?
         if (properties.size() != 3 ||
             properties[1].first != "_color" ||
             properties[2].first != "_width")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         std::vector<std::string> components = StringHelper::Split(properties[1].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->edge.color[0] = std::stoi(components[0]);
         model->edge.color[1] = std::stoi(components[1]);
         model->edge.color[2] = std::stoi(components[2]);
         model->edge.width = std::stof(properties[2].second);
      }
      else if (type == "_grid")
      {
         // Who needs flexibility?
         if (properties.size() != 5 ||
             properties[1].first != "_color" ||
             properties[2].first != "_spacing" ||
             properties[3].first != "_width" ||
             properties[4].first != "_display")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         std::vector<std::string> components = StringHelper::Split(properties[1].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->grid.color[0] = std::stoi(components[0]);
         model->grid.color[1] = std::stoi(components[1]);
         model->grid.color[2] = std::stoi(components[2]);
         model->grid.spacing = std::stoi(properties[2].second);
         model->grid.width = std::stof(properties[3].second);
         model->grid.onGround = properties[4].second == "1";
      }
      else if (type == "_setting")
      {
         // Who needs flexibility?
         if (properties.size() != 9 ||
             properties[1].first != "_ground" ||
             properties[2].first != "_sw" ||
             properties[3].first != "_aa" ||
             properties[4].first != "_grid" ||
             properties[5].first != "_edge" ||
             properties[6].first != "_bg_c" ||
             properties[7].first != "_bg_a" ||
             properties[8].first != "_scale")
         {
            return Failure("Unexpected property length or order for %1 rOBJ", type);
         }

         model->settings.ground = properties[1].second == "1";
         model->settings.shadow = properties[2].second == "1";
         model->settings.antialias = properties[3].second == "1";
         model->settings.grid = properties[4].second == "1";
         model->settings.edge = properties[5].second == "1";
         model->settings.background = properties[6].second == "1";
         model->settings.bgTransparent = properties[7].second == "1";
         std::vector<std::string> components = StringHelper::Split(properties[8].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: %1 must have 3 parts, not %2", properties[2].second, components.size());
         }
         model->settings.scale[0] = std::stoi(components[0]);
         model->settings.scale[1] = std::stoi(components[1]);
         model->settings.scale[2] = std::stoi(components[2]);
      }
   }
   else
   {
      return Failure("Unrecognized chunk type");
   }

   if ((uint8_t*)data != &chunk.data[chunk.header.length])
   {
      return Failure("Failed to parse all of body");
   }
   return Success;
}

Maybe<uint32_t> WriteRenderObj(FILE* f, const std::vector<std::pair<std::string, std::string>>& properties)
{
   size_t chunkSize = sizeof(int32_t) /* DICT header */;
   for (const auto& keyval : properties)
   {
      chunkSize += 2 * sizeof(int32_t) /* STRING headers */
                  + keyval.first.size()
                  + keyval.second.size();
   }

   // Start writing data.
   std::vector<uint8_t> data;
   data.resize(sizeof(Chunk::Header) + chunkSize);

   Chunk::Header* header = (Chunk::Header*)&data[0];
   header->id = Chunk::rOBJ;
   header->length = chunkSize;
   header->childLength = 0;

   // Write properties
   WriteDict((int32_t*)(header + 1), properties);

   // Write the chunk to disk.
   size_t written = fwrite(&data[0], 1, data.size(), f);
   if (written != data.size())
   {
      return Failure("Failed to write %1-byte rOBJ chunk: Only wrote %2 bytes", data.size(), written);
   }

   return written;
}

}; // anonymous namespace

namespace
{

bool IsFilled(const std::vector<bool>& filled, int index)
{
   if (index < 0 || index >= filled.size())
   {
      return false;
   }

   return filled[index];
}

int Index(const Model::Metadata& metadata, uint32_t x, uint32_t y, uint32_t z)
{
   if (x >= metadata.width || y >= metadata.height || z >= metadata.length) { return -1; }
   return x + z * metadata.width + y * metadata.width * metadata.length;
}

uint8_t GetExposedFaces(const std::vector<bool>& filled, const Model::Metadata& metadata, uint32_t x, uint32_t y, uint32_t z)
{
   uint8_t faces = All;
   int right = Index(metadata, x - 1, y, z);
   int left = Index(metadata, x + 1, y, z);
   int front = Index(metadata, x, y, z + 1);
   int behind = Index(metadata, x, y, z - 1);
   int above = Index(metadata, x, y + 1, z);
   int below = Index(metadata, x, y - 1, z);

   // Check overflows
   if (right % metadata.width == 0) { right = -1; }
   if (front % (metadata.width * metadata.height) == 0) { front = -1; }

   if (IsFilled(filled, right)) { faces ^= Right; }
   if (IsFilled(filled, left)) { faces ^= Left; }
   if (IsFilled(filled, front)) { faces ^= Front; }
   if (IsFilled(filled, behind)) { faces ^= Back; }
   if (IsFilled(filled, above)) { faces ^= Top; }
   if (IsFilled(filled, below)) { faces ^= Bottom; }

   return faces;
}

}; // namespace

Model* VoxFormat::Load(const std::string& path, bool tintable)
{
   auto maybeModel = sModels.find(path);
   if (maybeModel != sModels.end())
   {
      assert(tintable == maybeModel->second->mIsTintable);

      return maybeModel->second.get();
   }

   Maybe<std::unique_ptr<ModelData>> result = Read(path, tintable);
   if (!result)
   {
      LOG_ERROR(result.Failure().WithContext("Failed loading model").GetMessage());
      return nullptr;
   }
   std::unique_ptr<Model> model = std::make_unique<Model>(std::move(result.Result()));

   model->mVBO.BufferData(sizeof(Data) * int(model->mVoxelData.size()), &model->mVoxelData[0], GL_STATIC_DRAW);
   model->mIsTintable = tintable;

   auto emplaceResult = sModels.emplace(path, std::move(model));
   return emplaceResult.first->second.get();
}

Maybe<std::unique_ptr<VoxModel>> VoxFormat::ReadScene(const std::string& path)
{
   std::unique_ptr<VoxModel> model = std::make_unique<VoxModel>();

   FILE* f = fopen(path.c_str(), "rb");
   if (f == nullptr)
   {
      return Failure("Failed opening %1 with errno %2", path, errno);
   }

   CUBEWORLD_SCOPE_EXIT([&] { fclose(f); })

   FileHeader header;
   size_t bytes = fread(&header, 1, sizeof(FileHeader), f);
   if (bytes != sizeof(FileHeader))
   {
      return Failure("Failed to read %1 bytes: got %2 instead.", sizeof(FileHeader), bytes);
   }
   
   if (strncmp(header.id, "VOX ", 4) != 0)
   {
      return Failure("Header ID (%1) did not match expected (VOXV)", std::string(header.id, header.id + sizeof(header.id)));
   }
   
   if (header.version != 150)
   {
      return Failure("Header version is %1. Only 150 is supported", header.version);
   }

   Maybe<Chunk> maybeChunk = ReadChunk(f);
   if (!maybeChunk)
   {
      return maybeChunk.Failure().WithContext("Failed reading RIFF file");
   }

   Chunk chunk = std::move(maybeChunk.Result());

   if (chunk.header.id != Chunk::MAIN)
   {
      return Failure("First chunk was %1 (%2), expected MAIN (%3).", chunk.id(), chunk.header.id, Chunk::MAIN);
   }

   for (const Chunk& node : chunk.children)
   {
      if (Maybe<void> result = ParseChunk(model.get(), node); !result)
      {
         return result.Failure().WithContext("Failed parsing %1 chunk", node.id());
      }
   }

   return std::move(model);
}

Maybe<std::unique_ptr<ModelData>> VoxFormat::Read(const std::string& path, bool tintable)
{
   Maybe<std::unique_ptr<VoxModel>> maybeModel = ReadScene(path);

   if (!maybeModel)
   {
      return maybeModel.Failure();
   }

   std::unique_ptr<VoxModel> model = std::move(maybeModel.Result());

   if (model->shapes.size() > 1)
   {
      return Failure("Multi-shape models not yet supported");
   }

   LOG_INFO("Ignoring node tree, simply returning voxel model");

   const VoxModel::Model& shape = model->models[0];

   std::unique_ptr<ModelData> result = std::make_unique<ModelData>();

   result->mMetadata.width = shape.width;
   result->mMetadata.height = shape.height;
   result->mMetadata.length = shape.length;
   result->mIsTintable = tintable;

   std::vector<bool> filled(shape.width * shape.height * shape.length, false);
   for (const auto& info : shape.voxels)
   {
      Voxel::Data voxel;
      uint8_t i = (info >> 24) & 0xff;
      uint8_t z = (info >> 16) & 0xff;
      uint8_t y = (info >> 8) & 0xff;
      uint8_t x = info & 0xff;
      voxel.position.x = float(x) - result->mMetadata.width / 2 + 1;
      voxel.position.y = float(z) - result->mMetadata.height / 2;
      voxel.position.z = result->mMetadata.length / 2 - float(y) - 1;
      uint32_t rgba = model->palette[i - 1];
      voxel.color.r = (rgba) & 0xff;
      voxel.color.g = (rgba >> 8) & 0xff;
      voxel.color.b = (rgba >> 16) & 0xff;
      result->mVoxelData.push_back(voxel);

      size_t ndx = Index(result->mMetadata, x, y, z);
      filled[ndx] = true;
   }

   for (auto& voxel : result->mVoxelData)
   {
      size_t ndx = Index(result->mMetadata, voxel.position.x, voxel.position.y, voxel.position.z);
      voxel.enabledFaces = GetExposedFaces(filled, result->mMetadata, voxel.position.x, voxel.position.y, voxel.position.z);
   }

   return std::move(result);
}

Maybe<void> VoxFormat::Write(const std::string& path, const VoxModel& voxModel)
{
   FILE* f = fopen(path.c_str(), "wb+");
   if (f == nullptr)
   {
      return Failure("Failed opening %1 with errno %2", path, errno);
   }

   CUBEWORLD_SCOPE_EXIT([&] { fclose(f); });

   // Write overarching file headers
   std::vector<uint8_t> data {
      'V', 'O', 'X', ' ',
      150, 0, 0, 0,
      'M', 'A', 'I', 'N',
      0, 0, 0, 0,
      0, 0, 0, 0 /* determined later */
   };
   size_t written = fwrite(&data[0], 1, data.size(), f);
   if (written != data.size())
   {
      return Failure("Failed to write %1-byte header: wrote %2 instead", data.size(), written);
   }

   uint32_t chunkBytes = 0; // Saved to offset 0x10 after computing it.

   // Then write model data
   data.resize(sizeof(Chunk::Header) + sizeof(SIZE));
   for (const VoxModel::Model& model : voxModel.models)
   {
      Chunk::Header* header = (Chunk::Header*)&data[0];
      header->id = Chunk::SIZE;
      header->length = sizeof(SIZE);
      header->childLength = 0;

      SIZE* size = (SIZE*)&data[sizeof(Chunk::Header)];
      size->width = model.width;
      size->height = model.height;
      size->length = model.length;

      chunkBytes += written = fwrite(&data[0], sizeof(uint8_t), data.size(), f);
      if (written != data.size())
      {
         return Failure("Failed to write %1-byte SIZE chunk: Only wrote %2 bytes", data.size(), written);
      }

      header->id = Chunk::XYZI;
      header->length = sizeof(uint32_t) + sizeof(uint32_t) * model.voxels.size();
      header->childLength = 0;

      // Write num voxels along with the header, but not data.
      uint32_t* cursor = (uint32_t*)(header + 1);
      *cursor = model.voxels.size();

      chunkBytes += written = fwrite(&data[0], 1, sizeof(Chunk::Header) + sizeof(uint32_t), f);
      if (written != sizeof(Chunk::Header) + sizeof(uint32_t))
      {
         return Failure("Failed to write %1-byte XYZI header: Only wrote %2 bytes", sizeof(Chunk::Header) + sizeof(uint32_t), written);
      }

      // Write the voxel data next.
      chunkBytes += written = fwrite(&model.voxels[0], 1, sizeof(uint32_t) * model.voxels.size(), f);
      if (written != sizeof(uint32_t) * model.voxels.size())
      {
         return Failure("Failed to write %1 voxels: Only wrote %2 bytes", sizeof(uint32_t) * model.voxels.size(), written);
      }
   }

   // Write render transforms (pretty much unused)
   size_t nNodes = voxModel.transforms.size() + voxModel.groups.size() + voxModel.shapes.size();
   for (size_t id = 0; id < nNodes; id ++)
   {
      if (auto node = voxModel.transforms.find(id); node != voxModel.transforms.end())
      {
         const VoxModel::TransformNode& transform = node->second;

         // Predetermine what the property dicts look like.
         std::vector<std::pair<std::string, std::string>> attributes;
         if (transform.name != "")
         {
            attributes.push_back(std::make_pair("_name", transform.name));
         }
         if (transform.hidden)
         {
            attributes.push_back(std::make_pair("_hidden", "1"));
         }

         std::vector<std::pair<std::string, std::string>> frame;
         if (transform.rotate != 0)
         {
            auto keyval = std::make_pair("_r", std::to_string(transform.rotate));
            frame.push_back(keyval);
         }
         if (transform.translate[0] != 0 || transform.translate[1] != 0 || transform.translate[2] != 0)
         {
            frame.push_back(std::make_pair("_t", 
               std::to_string(transform.translate[0]) + " " + 
               std::to_string(transform.translate[1]) + " " + 
               std::to_string(transform.translate[2])));
         }

         // Compute how big to make the chunk
         size_t chunkSize = sizeof(nTRN::Header)
                          + sizeof(nTRN::Body)
                          + 2 * sizeof(int32_t) /* DICT headers */;
         for (const auto& keyval : attributes)
         {
            chunkSize += 2 * sizeof(int32_t) /* STRING headers */
                       + keyval.first.size()
                       + keyval.second.size();
         }
         for (const auto& keyval : frame)
         {
            chunkSize += 2 * sizeof(int32_t) /* STRING headers */
                       + keyval.first.size()
                       + keyval.second.size();
         }

         // Start writing data.
         data.resize(sizeof(Chunk::Header) + chunkSize);

         Chunk::Header* header = (Chunk::Header*)&data[0];
         header->id = Chunk::nTRN;
         header->length = chunkSize;
         header->childLength = 0;

         // Write header
         nTRN::Header* nodeHeader = (nTRN::Header*)(header + 1);
         nodeHeader->id = id;

         // Write node attributes
         int32_t* postAttributes = WriteDict((int32_t*)(nodeHeader + 1), attributes);

         // Write body
         nTRN::Body* body = (nTRN::Body*)(postAttributes);
         body->child = transform.child;
         body->reserved = -1;
         body->layer = transform.layer;
         body->nFrames = 1;

         // Write frame attributes
         int32_t* postFrame = WriteDict((int32_t*)(body + 1), frame);
      }
      else if (auto node = voxModel.groups.find(id); node != voxModel.groups.end())
      {
         const VoxModel::GroupNode& group = node->second;

         // Compute how big to make the chunk
         size_t chunkSize = sizeof(nGRP::Header)
                          + sizeof(nGRP::Body)
                          + sizeof(int32_t) /* DICT header */
                          + group.children.size() * sizeof(int32_t);

         // Start writing data.
         data.resize(sizeof(Chunk::Header) + chunkSize);

         Chunk::Header* header = (Chunk::Header*)&data[0];
         header->id = Chunk::nGRP;
         header->length = chunkSize;
         header->childLength = 0;

         // Write header
         nGRP::Header* nodeHeader = (nGRP::Header*)(header + 1);
         nodeHeader->id = id;

         // Write node attributes (none)
         int32_t* attributes = (int32_t*)(nodeHeader + 1);
         *attributes = 0;

         // Write body
         nGRP::Body* body = (nGRP::Body*)(attributes + 1);
         body->children = group.children.size();

         int32_t* cursor = (int32_t*)(body + 1);
         for (const int32_t& child : group.children)
         {
            *cursor++ = child;
         }
      }
      else if (auto node = voxModel.shapes.find(id); node != voxModel.shapes.end())
      {
         const VoxModel::ShapeNode& shape = node->second;

         // Compute how big to make the chunk
         size_t chunkSize = sizeof(nSHP::Header)
                          + sizeof(nSHP::Body)
                          + 2 * sizeof(int32_t) /* DICT headers */
                          + sizeof(int32_t) /* Model ID */;

         // Start writing data.
         data.resize(sizeof(Chunk::Header) + chunkSize);

         Chunk::Header* header = (Chunk::Header*)&data[0];
         header->id = Chunk::nSHP;
         header->length = chunkSize;
         header->childLength = 0;

         // Write header
         nSHP::Header* nodeHeader = (nSHP::Header*)(header + 1);
         nodeHeader->id = id;

         // Write node attributes (none)
         int32_t* attributes = (int32_t*)(nodeHeader + 1);
         *attributes = 0;

         // Write body
         nSHP::Body* body = (nSHP::Body*)(attributes + 1);
         body->nModels = 1;

         int32_t* cursor = (int32_t*)(body + 1);
         *cursor++ = shape.model;
         *cursor++ = 0; // model attributes: reserved
      }
      else
      {
         return Failure("Could not find a node with ID %1", id);
      }
      
      // Write the node.
      chunkBytes += written = fwrite(&data[0], 1, data.size(), f);
      if (written != data.size())
      {
         Chunk::Header* header = (Chunk::Header*)&data[0];
         return Failure("Failed to write %1-byte %2 chunk: Only wrote %3 bytes", data.size(), Chunk::ReadChunkID(header->id), written);
      }
   }

   // Write layer info
   for (const VoxModel::Layer& layer : voxModel.layers)
   {
      // Predetermine what the attribute dict looks like.
      std::vector<std::pair<std::string, std::string>> attributes;
      if (layer.name != "")
      {
         attributes.push_back(std::make_pair("_name", layer.name));
      }
      if (layer.hidden)
      {
         attributes.push_back(std::make_pair("_hidden", "1"));
      }

      // Layout
      // int32	: layer id
      // DICT	: layer atrribute
      //    (_name : string)
      //    (_hidden : 0/1)
      // int32	: reserved id, must be -1

      // Compute how big to make the chunk
      size_t chunkSize = 3 * sizeof(int32_t) /* DICT headers */;
      for (const auto& keyval : attributes)
      {
         chunkSize += 2 * sizeof(int32_t) /* STRING headers */
                    + keyval.first.size()
                    + keyval.second.size();
      }

      // Start writing data.
      data.resize(sizeof(Chunk::Header) + chunkSize);

      Chunk::Header* header = (Chunk::Header*)&data[0];
      header->id = Chunk::LAYR;
      header->length = chunkSize;
      header->childLength = 0;

      // Write header
      int32_t* cursor = (int32_t*)(header + 1);
      *cursor++ = layer.id;

      // Write node attributes
      cursor = WriteDict(cursor, attributes);
      *cursor++ = -1;

      // Write the chunk to disk.
      chunkBytes += written = fwrite(&data[0], 1, data.size(), f);
      if (written != data.size())
      {
         return Failure("Failed to write %1-byte LAYR chunk: Only wrote %2 bytes", data.size(), written);
      }
   }

   // Write the palette
   {
      data.resize(sizeof(Chunk::Header) + sizeof(voxModel.palette));

      Chunk::Header* header = (Chunk::Header*)&data[0];
      header->id = Chunk::RGBA;
      header->length = sizeof(voxModel.palette);
      header->childLength = 0;

      memcpy(header + 1, voxModel.palette, sizeof(voxModel.palette));

      chunkBytes += written = fwrite(&data[0], 1, data.size(), f);
      if (written != data.size())
      {
         return Failure("Failed to write %1-byte RGBA chunk: Only wrote %2 bytes", data.size(), written);
      }
   }

   // Write materials
   std::vector<VoxModel::Material> materials(voxModel.materials.begin(), voxModel.materials.end());
   while (materials.size() < 256)
   {
      size_t id = materials.size();
      materials.push_back(VoxModel::Material::Default);
      materials[id].id = id;
   }

   for (const VoxModel::Material& material : materials)
   {
      std::vector<std::pair<std::string, std::string>> properties;
      switch (material.type)
      {
         case VoxModel::Material::Diffuse:
            properties.push_back(std::make_pair("_type", "_diffuse"));
            break;
         case VoxModel::Material::Emit:
            properties.push_back(std::make_pair("_type", "_emit"));
            break;
         case VoxModel::Material::Glass:
            properties.push_back(std::make_pair("_type", "_glass"));
            break;
         case VoxModel::Material::Metal:
            properties.push_back(std::make_pair("_type", "_metal"));
            break;
         default:
            return Failure("Material %1 does not have a type", material.id);
      }

      properties.push_back(std::make_pair("_weight", ToShortString(material.weight)));
      properties.push_back(std::make_pair("_plastic", material.plastic ? "1" : "0"));
      properties.push_back(std::make_pair("_rough", ToShortString(material.rough)));
      properties.push_back(std::make_pair("_spec", ToShortString(material.spec)));
      properties.push_back(std::make_pair("_ior", ToShortString(material.ior)));
      properties.push_back(std::make_pair("_att", ToShortString(material.att)));
      properties.push_back(std::make_pair("_flux", ToShortString(material.flux)));
      properties.push_back(std::make_pair("_ldr", material.ldr ? "1" : "0"));

      size_t chunkSize = 2 * sizeof(int32_t) /* id & DICT headers */;
      for (const auto& keyval : properties)
      {
         chunkSize += 2 * sizeof(int32_t) /* STRING headers */
                    + keyval.first.size()
                    + keyval.second.size();
      }

      // Start writing data.
      data.resize(sizeof(Chunk::Header) + chunkSize);

      Chunk::Header* header = (Chunk::Header*)&data[0];
      header->id = Chunk::MATL;
      header->length = chunkSize;
      header->childLength = 0;

      // Write header
      int32_t* cursor = (int32_t*)(header + 1);
      *cursor++ = material.id;

      // Write properties
      cursor = WriteDict(cursor, properties);

      // Write the chunk to disk.
      chunkBytes += written = fwrite(&data[0], 1, data.size(), f);
      if (written != data.size())
      {
         return Failure("Failed to write %1-byte MATL chunk: Only wrote %2 bytes", data.size(), written);
      }
   }

   // Write render objects
   {
      // Sun
      Maybe<uint32_t> result = WriteRenderObj(f, {
         { "_type", "_inf" },
         { "_i", ToShortString(voxModel.sun.intensity) },
         { "_k", 
           Format::FormatString("%1 %2 %3", 
              voxModel.sun.color[0],
              voxModel.sun.color[1],
              voxModel.sun.color[2]) },
         { "_angle", Format::FormatString("%1 %2", voxModel.sun.angle[0], voxModel.sun.angle[1]) },
         { "_area", ToShortString(voxModel.sun.area) },
         { "_disk", voxModel.sun.disk ? "1" : "0" },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Uniform sky
      result = WriteRenderObj(f, {
         { "_type", "_uni" },
         { "_i", ToShortString(voxModel.sky.intensity) },
         { "_k", 
           Format::FormatString("%1 %2 %3", 
              voxModel.sky.color[0],
              voxModel.sky.color[1],
              voxModel.sky.color[2]) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Atmospheric sky
      result = WriteRenderObj(f, {
         { "_type", "_atm" },
         { "_ray_d", ToShortString(voxModel.atm.rayleighDensity) },
         { "_ray_k", 
           Format::FormatString("%1 %2 %3", 
              voxModel.atm.rayleighColor[0],
              voxModel.atm.rayleighColor[1],
              voxModel.atm.rayleighColor[2]) },
         { "_mie_d", ToShortString(voxModel.atm.mieDensity) },
         { "_mie_k", 
           Format::FormatString("%1 %2 %3", 
              voxModel.atm.mieColor[0],
              voxModel.atm.mieColor[1],
              voxModel.atm.mieColor[2]) },
         { "_mie_g", ToShortString(voxModel.atm.miePhase) },
         { "_o3_d", ToShortString(voxModel.atm.ozoneDensity) },
         { "_o3_k", 
           Format::FormatString("%1 %2 %3", 
              voxModel.atm.ozoneColor[0],
              voxModel.atm.ozoneColor[1],
              voxModel.atm.ozoneColor[2]) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Uniform fog
      result = WriteRenderObj(f, {
         { "_type", "_fog_uni" },
         { "_d", ToShortString(voxModel.fog.density) },
         { "_k", 
           Format::FormatString("%1 %2 %3", 
              voxModel.fog.color[0],
              voxModel.fog.color[1],
              voxModel.fog.color[2]) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Lens
      result = WriteRenderObj(f, {
         { "_type", "_lens" },
         { "_fov", std::to_string(voxModel.lens.fov) },
         { "_dof", ToShortString(voxModel.lens.depthOfField) },
         { "_expo", voxModel.lens.exposure ? "1" : "0" },
         { "_vig", voxModel.lens.vignette ? "1" : "0" },
         { "_sg", voxModel.lens.stereographics ? "1" : "0" },
         { "_blade_n", std::to_string(voxModel.lens.bladeNumber) },
         { "_blade_r", std::to_string(voxModel.lens.bladeRotation) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Bloom
      result = WriteRenderObj(f, {
         { "_type", "_bloom" },
         { "_mix", ToShortString(voxModel.bloom.mix) },
         { "_scale", ToShortString(voxModel.bloom.scale) },
         { "_aspect", ToShortString(voxModel.bloom.aspect) },
         { "_threshold", ToShortString(voxModel.bloom.threshold) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Tone
      result = WriteRenderObj(f, {
         { "_type", "_tone" },
         { "_aces", voxModel.tone.aces ? "1" : "0" },
         { "_gam", ToShortString(voxModel.tone.gamma) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Ground
      result = WriteRenderObj(f, {
         { "_type", "_ground" },
         { "_color", 
           Format::FormatString("%1 %2 %3", 
              voxModel.ground.color[0],
              voxModel.ground.color[1],
              voxModel.ground.color[2]) },
         { "_hor", ToShortString(voxModel.ground.horizon) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Background
      result = WriteRenderObj(f, {
         { "_type", "_bg" },
         { "_color", 
           Format::FormatString("%1 %2 %3", 
              voxModel.bg.color[0],
              voxModel.bg.color[1],
              voxModel.bg.color[2]) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Edge
      result = WriteRenderObj(f, {
         { "_type", "_edge" },
         { "_color", 
           Format::FormatString("%1 %2 %3", 
              voxModel.edge.color[0],
              voxModel.edge.color[1],
              voxModel.edge.color[2]) },
         { "_width", ToShortString(voxModel.edge.width) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Grid
      result = WriteRenderObj(f, {
         { "_type", "_grid" },
         { "_color", 
           Format::FormatString("%1 %2 %3", 
              voxModel.grid.color[0],
              voxModel.grid.color[1],
              voxModel.grid.color[2]) },
         { "_spacing", ToShortString(voxModel.grid.spacing) },
         { "_width", ToShortString(voxModel.grid.width) },
         { "_display", voxModel.grid.onGround ? "1" : "0" },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;

      // Settings
      result = WriteRenderObj(f, {
         { "_type", "_setting" },
         { "_ground", voxModel.settings.ground ? "1" : "0" },
         { "_sw", voxModel.settings.shadow ? "1" : "0" },
         { "_aa", voxModel.settings.antialias ? "1" : "0" },
         { "_grid", voxModel.settings.grid ? "1" : "0" },
         { "_edge", voxModel.settings.edge ? "1" : "0" },
         { "_bg_c", voxModel.settings.background ? "1" : "0" },
         { "_bg_a", voxModel.settings.bgTransparent ? "1" : "0" },
         { "_scale", 
           Format::FormatString("%1 %2 %3", 
              voxModel.settings.scale[0],
              voxModel.settings.scale[1],
              voxModel.settings.scale[2]) },
      });
      if (!result)
      {
         return result.Failure(); 
      }
      chunkBytes += *result;
   }

   if (fseek(f, 0x10, SEEK_SET) != 0)
   {
      return Failure("Failed to seek to offset 0x10: errno %1", errno);
   }

   written = fwrite(&chunkBytes, 1, sizeof(chunkBytes), f);
   if (written != sizeof(chunkBytes))
   {
      return Failure("Failed to write size of MAIN chunk. Only wrote %1 bytes", written);
   }

   return Success;
}

Maybe<void> VoxFormat::Write(const std::string& path, const ModelData& modelData)
{
   // Validate input. Sometimes mVoxelData is optimized out, so make sure
   // it exists.
   if (modelData.mVoxelData.size() == 0)
   {
      return Failure{"Voxel data provided was empty."};
   }

   VoxModel voxModel;
   voxModel.materials.clear();
   voxModel.groups.clear();
   voxModel.shapes.clear();
   voxModel.transforms.clear();
   voxModel.layers.clear();

   VoxModel::Model model;
   model.width = modelData.mMetadata.width;
   model.height = modelData.mMetadata.height;
   model.length = modelData.mMetadata.length;

   int32_t available = 255;
   memcpy(voxModel.palette, default_palette, sizeof(voxModel.palette));
   memset(voxModel.palette, 0, sizeof(voxModel.palette));
   for (const Voxel::Data& voxel : modelData.mVoxelData)
   {
      uint32_t rgba = (uint8_t(voxel.color.r)) | (uint8_t(voxel.color.g) << 8) | (uint8_t(voxel.color.b) << 16) | 0xff << 24;

      uint8_t colorIndex = 255;
      for (; colorIndex > available; colorIndex --)
      {
         if (voxModel.palette[colorIndex - 1] == rgba)
         {
            break;
         }
      }
      if (colorIndex == available)
      {
         available --;
         if (available < 0)
         {
            return Failure("Too many colors");
         }

         voxModel.palette[colorIndex - 1] = rgba;
      }

      // Inverse of how we do centering.
      int x = voxel.position.x + modelData.mMetadata.width / 2 - 1;
      int z = voxel.position.y + modelData.mMetadata.height / 2;
      int y = modelData.mMetadata.length / 2 - voxel.position.z - 1;

      // LOG_INFO("Voxel position: %1 %2 %3", uint8_t(x), uint8_t(y), uint8_t(z));
      uint32_t packed = uint8_t(x) | 
         (uint8_t(y) << 8) | 
         (uint8_t(z) << 16) | 
         (colorIndex << 24);
      // printf("Result: %08X\n", packed);
      // return Failure("Idk man");
      model.voxels.push_back(packed);
   };

   voxModel.models.push_back(model);

   voxModel.layers.push_back({0, "0", false});
   voxModel.layers.push_back({1, "1", false});
   voxModel.layers.push_back({2, "2", false});
   voxModel.layers.push_back({3, "3", false});
   voxModel.layers.push_back({4, "4", false});
   voxModel.layers.push_back({5, "5", false});
   voxModel.layers.push_back({6, "6", false});
   voxModel.layers.push_back({7, "7", false});

   VoxModel::TransformNode transform;
   transform.id = 0;
   transform.child = 1;
   transform.layer = -1;
   voxModel.transforms.emplace(transform.id, transform);

   VoxModel::GroupNode group;
   group.id = 1;
   group.children.push_back(2);
   voxModel.groups.emplace(group.id, group);

   transform.id = 2;
   transform.child = 3;
   transform.layer = 0;
   voxModel.transforms.emplace(transform.id, transform);

   VoxModel::ShapeNode shape;
   shape.id = 3;
   shape.model = 0;
   voxModel.shapes.emplace(shape.id, shape);

   return Write(path, voxModel);
}

}; // namespace Voxel

}; // namespace CubeWorld
