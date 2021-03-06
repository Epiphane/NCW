// By Thomas Steinke

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <queue>
#include <vector>

#include <RGBDesignPatterns/Scope.h>
#include <RGBLogger/Logger.h>
#include <RGBText/StringHelper.h>
#include <Engine/Core/Config.h>
#include <Engine/Core/FileSystemProvider.h>

#include "VoxFormat.h"

namespace CubeWorld
{

namespace Voxel
{

std::unordered_map<std::string, std::unique_ptr<Model>> VoxFormat::sDepModels;
std::unordered_map<std::string, std::unique_ptr<VoxModel>> VoxFormat::sModels;

const VoxModelData::Material VoxModelData::Material::Default = {
   0, // id
   VoxModelData::Material::Diffuse, // type
   1, // weight
   0.1f, // rough
   0.5f, // spec
   0.3f, // ior
   0, // att
   0, // flux
   false, // plastic
   false, // ldr
};

struct VoxFormat::FileHeader {
   char id[4];
   int32_t version;
};

struct VoxFormat::Chunk {
   constexpr static uint32_t MakeChunkID(const char id[])
   {
      return uint32_t((id[3] << 24) | (id[2] << 16) | (id[1] << 8) | id[0]);
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
   const static uint32_t IMAP;
   const static uint32_t MATL;
   const static uint32_t nTRN;
   const static uint32_t nGRP;
   const static uint32_t nSHP;
   const static uint32_t rOBJ;
};

const uint32_t VoxFormat::Chunk::MAIN = MakeChunkID("MAIN");
const uint32_t VoxFormat::Chunk::SIZE = MakeChunkID("SIZE");
const uint32_t VoxFormat::Chunk::PACK = MakeChunkID("PACK");
const uint32_t VoxFormat::Chunk::XYZI = MakeChunkID("XYZI");
const uint32_t VoxFormat::Chunk::RGBA = MakeChunkID("RGBA");
const uint32_t VoxFormat::Chunk::LAYR = MakeChunkID("LAYR");
const uint32_t VoxFormat::Chunk::IMAP = MakeChunkID("IMAP");
const uint32_t VoxFormat::Chunk::MATL = MakeChunkID("MATL");
const uint32_t VoxFormat::Chunk::nTRN = MakeChunkID("nTRN");
const uint32_t VoxFormat::Chunk::nGRP = MakeChunkID("nGRP");
const uint32_t VoxFormat::Chunk::nSHP = MakeChunkID("nSHP");
const uint32_t VoxFormat::Chunk::rOBJ = MakeChunkID("rOBJ");

struct VoxFormat::PACK {
   uint32_t numModels;
};

struct VoxFormat::SIZE {
   uint32_t width; // size x
   uint32_t length; // size z
   uint32_t height; // size y
};

struct VoxFormat::XYZI {
   uint32_t numVoxels; // N
   // int32_t[4 * N] voxels; // (x, y, z, colorIndex)
};

struct VoxFormat::RGBA {
   uint32_t values[256];
};

const uint32_t VoxFormat::default_palette[] = {
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

struct VoxFormat::ROTATION {
   // bit | value
   // 0-1 : 1 : index of the non-zero entry in the first row
   // 2-3 : 2 : index of the non-zero entry in the second row
   // 4   : 0 : the sign in the first row (0 : positive; 1 : negative)
   // 5   : 1 : the sign in the second row (0 : positive; 1 : negative)
   // 6   : 1 : the sign in the third row (0 : positive; 1 : negative)
   uint8_t _r;
};

struct VoxFormat::nTRN {
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

struct VoxFormat::nGRP {
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

struct VoxFormat::nSHP {
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

struct VoxFormat::MATL {
   int32_t id;
   // DICT properties;
};

struct VoxFormat::LAYR {
   int32_t id;
   // DICT attributes;
   int32_t reserved = -1;
};

Maybe<VoxFormat::Chunk> VoxFormat::ReadChunk(FileSystem& fs, FileSystem::FileHandle handle)
{
   Chunk chunk;

   if (Maybe<void> read = fs.ReadFile(handle, &chunk.header, sizeof(Chunk::Header)); !read)
   {
      return read.Failure().WithContext("Failed reading chunk header");
   }

   chunk.data.resize(chunk.header.length);

   if (chunk.header.length > 0)
   {
      if (Maybe<void> read = fs.ReadFile(handle, &chunk.data[0], chunk.header.length); !read)
      {
         return read.Failure().WithContext("Failed reading chunk data");
      }
   }

   uint32_t read = 0;
   while (read < chunk.header.childLength)
   {
      Maybe<Chunk> child = ReadChunk(fs, handle);
      if (!child)
      {
         return child.Failure().WithContext("Failed reading child {num} of {id} chunk", chunk.children.size(), chunk.id());
      }
      read += sizeof(child->header) + child->header.length + child->header.childLength;
      chunk.children.push_back(std::move(*child));
   }

   return chunk;
}

std::string VoxFormat::ParseString(int32_t*& data)
{
   int32_t size = *data++;
   int8_t* start = (int8_t*)data;
   int8_t* end = start + size;

   data = (int32_t*)end;
   return std::string(start, end);
}

int32_t* VoxFormat::WriteString(int32_t* data, const std::string& string)
{
   *data++ = int32_t(string.size());
   char* start = (char*)data;
   char* end = start + string.size();
   strncpy(start, string.c_str(), string.size());
   return (int32_t*)end;
}

std::vector<std::pair<std::string, std::string>> VoxFormat::ParseDict(int32_t*& data)
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

int32_t* VoxFormat::WriteDict(int32_t* data, const std::vector<std::pair<std::string, std::string>>& pairs)
{
   *data++ = int32_t(pairs.size());
   for (const auto& pair : pairs)
   {
      data = WriteString(data, pair.first);
      data = WriteString(data, pair.second);
   }
   return data;
}

std::string VoxFormat::ToShortString(float val)
{
   std::string result = std::to_string(val);
   result.erase(result.find_last_not_of('0') + 1, std::string::npos);
   result.erase(result.find_last_not_of('.') + 1, std::string::npos);
   return result;
}

Maybe<void> VoxFormat::ParseChunk(VoxModelData* model, const Chunk& chunk)
{
   int32_t* data = (int32_t*)&chunk.data[0];
   if (chunk.header.id == Chunk::SIZE)
   {
      VoxModelData::Model m;

      if (chunk.header.length != sizeof(SIZE))
      {
         return Failure("Chunk body was not {expected} bytes (was {actual} instead)", sizeof(SIZE), chunk.header.length);
      }
      SIZE* size = (SIZE*)data;
      m.width = size->width;
      m.height = size->height;
      m.length = size->length;

      model->models.push_back(m);
      data = (int32_t*)(&chunk.data[0] + chunk.header.length);
   }
   else if (chunk.header.id == Chunk::XYZI)
   {
      XYZI* xyzi = (XYZI*)data;
      size_t chunkSize = sizeof(XYZI) + sizeof(uint32_t) * xyzi->numVoxels;
      if (chunk.header.length != chunkSize)
      {
         return Failure("Chunk body was not {expected} bytes (was {actual} instead)", chunkSize, chunk.header.length);
      }

      model->models[model->models.size() - 1].voxels.assign(
         (uint32_t*)(xyzi + 1),
         (uint32_t*)data + chunk.header.length / sizeof(uint32_t));
      data = (int32_t*)(&chunk.data[0] + chunk.header.length);
   }
   else if (chunk.header.id == Chunk::nTRN)
   {
      VoxModelData::TransformNode node;
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
            return Failure("Unrecognized attribute: {key}={val}", keyval.first, keyval.second);
         }
      }

      nTRN::Body* body = (nTRN::Body*)data;
      if (body->reserved != -1)
      {
         return Failure("Reserved id was not -1 (was {val})", body->reserved);
      }
      if (body->nFrames != 1)
      {
         return Failure("Num frames was not 1 (was {num})", body->nFrames);
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
            node.rotate = int8_t(std::stoi(keyval.second));
         }
         else if (keyval.first == "_t")
         {
            std::vector<std::string> components = StringHelper::Split(keyval.second, ' ');
            if (components.size() != 3)
            {
               return Failure("Unexpected number of components: {attr} must have 3 parts, not {num}", keyval.second, components.size());
            }
            node.translate[0] = std::stoi(components[0]);
            node.translate[1] = std::stoi(components[1]);
            node.translate[2] = std::stoi(components[2]);
         }
         else
         {
            return Failure("Unrecognized frame attribute: {key}={value}", keyval.first, keyval.second);
         }
      }

      model->transforms.emplace(node.id, node);
   }
   else if (chunk.header.id == Chunk::nGRP)
   {
      VoxModelData::GroupNode node;
      node.id = *data++;

      // Read node attributes
      auto attributes = ParseDict(data);
      if (attributes.size() > 0)
      {
         return Failure("Unrecognized attribute: {key}={value}", attributes[0].first, attributes[1].second);
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
      VoxModelData::ShapeNode node;
      node.id = *data++;

      // Read node attributes
      auto attributes = ParseDict(data);
      if (attributes.size() > 0)
      {
         return Failure("Unrecognized attribute: {key}={value}", attributes[0].first, attributes[1].second);
      }

      nSHP::Body* body = (nSHP::Body*)data;
      data = (int32_t*)(body + 1);

      if (body->nModels != 1)
      {
         return Failure("nSHP object must have only 1 model (value: {value})", body->nModels);
      }

      // Parse the one model
      node.model = *data++;

      // Read model attributes
      attributes = ParseDict(data);
      if (attributes.size() > 0)
      {
         return Failure("Unrecognized attribute: {key}={value}", attributes[0].first, attributes[1].second);
      }

      model->shapes.emplace(node.id, node);
   }
   else if (chunk.header.id == Chunk::IMAP)
   {
      // Unused??
      data = (int32_t*)(&chunk.data[0] + chunk.header.length);
   }
   else if (chunk.header.id == Chunk::LAYR)
   {
      VoxModelData::Layer layer;
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
            return Failure("Unrecognized attribute: {key}={value}", keyval.first, keyval.second);
         }
      }

      int32_t reserved = *data++;
      if (reserved != -1)
      {
         return Failure("Reserved ID was not -1 (was {value})", reserved);
      }

      model->layers.push_back(layer);
   }
   else if (chunk.header.id == Chunk::RGBA)
   {
      if (chunk.header.length != sizeof(model->palette))
      {
         return Failure("Chunk body was not {expected} bytes (was {actual} instead)", sizeof(model->palette), chunk.header.length);
      }

      memcpy(model->palette, data, sizeof(model->palette));
      data += sizeof(model->palette) / sizeof(int32_t);
   }
   else if (chunk.header.id == Chunk::MATL)
   {
      VoxModelData::Material material;
      material.id = *data++;

      auto attributes = ParseDict(data);
      for (const auto& keyval : attributes)
      {
         if (keyval.first == "_type")
         {
            if (keyval.second == "_diffuse") { material.type = VoxModelData::Material::Diffuse; }
            else if (keyval.second == "_metal") { material.type = VoxModelData::Material::Metal; }
            else if (keyval.second == "_glass") { material.type = VoxModelData::Material::Glass; }
            else if (keyval.second == "_emit") { material.type = VoxModelData::Material::Emit; }
            else
            {
               return Failure("Unrecognized material type: {type}", keyval.second);
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
            return Failure("Unknown material property: {key}={value}", keyval.first, keyval.second);
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
         return Failure("First key expected to be _type (was {key})", properties[0].first);
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         model->sun.intensity = std::stof(properties[1].second);
         std::vector<std::string> components = StringHelper::Split(properties[2].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[2].second, components.size());
         }
         model->sun.color[0] = uint8_t(std::stoi(components[0]));
         model->sun.color[1] = uint8_t(std::stoi(components[1]));
         model->sun.color[2] = uint8_t(std::stoi(components[2]));
         components = StringHelper::Split(properties[3].second, ' ');
         if (components.size() != 2)
         {
            return Failure("Unexpected number of components: {prop} must have 2 parts, not {num}", properties[2].second, components.size());
         }
         model->sun.angle[0] = int8_t(std::stoi(components[0]));
         model->sun.angle[1] = int8_t(std::stoi(components[1]));
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         model->sky.intensity = std::stof(properties[1].second);
         std::vector<std::string> components = StringHelper::Split(properties[2].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num", properties[2].second, components.size());
         }
         model->sky.color[0] = uint8_t(std::stoi(components[0]));
         model->sky.color[1] = uint8_t(std::stoi(components[1]));
         model->sky.color[2] = uint8_t(std::stoi(components[2]));
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         model->atm.rayleighDensity = std::stof(properties[1].second);
         std::vector<std::string> components = StringHelper::Split(properties[2].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[2].second, components.size());
         }
         model->atm.rayleighColor[0] = uint8_t(std::stoi(components[0]));
         model->atm.rayleighColor[1] = uint8_t(std::stoi(components[1]));
         model->atm.rayleighColor[2] = uint8_t(std::stoi(components[2]));

         model->atm.mieDensity = std::stof(properties[3].second);
         components = StringHelper::Split(properties[4].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[4].second, components.size());
         }
         model->atm.mieColor[0] = uint8_t(std::stoi(components[0]));
         model->atm.mieColor[1] = uint8_t(std::stoi(components[1]));
         model->atm.mieColor[2] = uint8_t(std::stoi(components[2]));
         model->atm.miePhase = std::stof(properties[5].second);

         model->atm.ozoneDensity = std::stof(properties[6].second);
         components = StringHelper::Split(properties[7].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[7].second, components.size());
         }
         model->atm.ozoneColor[0] = uint8_t(std::stoi(components[0]));
         model->atm.ozoneColor[1] = uint8_t(std::stoi(components[1]));
         model->atm.ozoneColor[2] = uint8_t(std::stoi(components[2]));
      }
      else if (type == "_fog_uni")
      {
         // Who needs flexibility?
         if (properties.size() != 3 ||
             properties[1].first != "_d" ||
             properties[2].first != "_k")
         {
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         model->fog.density = std::stof(properties[1].second);
         std::vector<std::string> components = StringHelper::Split(properties[2].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[2].second, components.size());
         }
         model->fog.color[0] = uint8_t(std::stoi(components[0]));
         model->fog.color[1] = uint8_t(std::stoi(components[1]));
         model->fog.color[2] = uint8_t(std::stoi(components[2]));
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         model->lens.fov = std::stoi(properties[1].second);
         model->lens.depthOfField = std::stof(properties[2].second);
         model->lens.exposure = std::stof(properties[3].second);
         model->lens.vignette = properties[4].second == "1";
         model->lens.stereographics = properties[5].second == "1";
         model->lens.bladeNumber = uint8_t(std::stoi(properties[6].second));
         model->lens.bladeRotation = int16_t(std::stoi(properties[7].second));
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         std::vector<std::string> components = StringHelper::Split(properties[1].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[2].second, components.size());
         }
         model->ground.color[0] = uint8_t(std::stoi(components[0]));
         model->ground.color[1] = uint8_t(std::stoi(components[1]));
         model->ground.color[2] = uint8_t(std::stoi(components[2]));
         model->ground.horizon = std::stof(properties[2].second);
      }
      else if (type == "_bg")
      {
         // Who needs flexibility?
         if (properties.size() != 2 ||
             properties[1].first != "_color")
         {
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         std::vector<std::string> components = StringHelper::Split(properties[1].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[2].second, components.size());
         }
         model->bg.color[0] = uint8_t(std::stoi(components[0]));
         model->bg.color[1] = uint8_t(std::stoi(components[1]));
         model->bg.color[2] = uint8_t(std::stoi(components[2]));
      }
      else if (type == "_edge")
      {
         // Who needs flexibility?
         if (properties.size() != 3 ||
             properties[1].first != "_color" ||
             properties[2].first != "_width")
         {
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         std::vector<std::string> components = StringHelper::Split(properties[1].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[2].second, components.size());
         }
         model->edge.color[0] = uint8_t(std::stoi(components[0]));
         model->edge.color[1] = uint8_t(std::stoi(components[1]));
         model->edge.color[2] = uint8_t(std::stoi(components[2]));
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
         }

         std::vector<std::string> components = StringHelper::Split(properties[1].second, ' ');
         if (components.size() != 3)
         {
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[2].second, components.size());
         }
         model->grid.color[0] = uint8_t(std::stoi(components[0]));
         model->grid.color[1] = uint8_t(std::stoi(components[1]));
         model->grid.color[2] = uint8_t(std::stoi(components[2]));
         model->grid.spacing = uint32_t(std::stoi(properties[2].second));
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
            return Failure("Unexpected property length or order for {type} rOBJ", type);
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
            return Failure("Unexpected number of components: {prop} must have 3 parts, not {num}", properties[2].second, components.size());
         }
         model->settings.scale[0] = uint8_t(std::stoi(components[0]));
         model->settings.scale[1] = uint8_t(std::stoi(components[1]));
         model->settings.scale[2] = uint8_t(std::stoi(components[2]));
      }
   }
   else
   {
      return Failure("Unrecognized chunk type");
   }

   if ((uint8_t*)data != &chunk.data[0] + chunk.header.length)
   {
      return Failure("Failed to parse all of body");
   }
   return Success;
}

Maybe<size_t> VoxFormat::WriteRenderObj(FileSystem& fs, FileSystem::FileHandle handle, const std::vector<std::pair<std::string, std::string>>& properties)
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
   header->length = uint32_t(chunkSize);
   header->childLength = 0;

   // Write properties
   WriteDict((int32_t*)(header + 1), properties);

   // Write the chunk to disk.
   if (Maybe<void> write = fs.WriteFile(handle, &data[0], data.size()); !write)
   {
      return write.Failure().WithContext("Failed to write {size}-byte rOBJ chunk", data.size());
   }
   return data.size();
}

bool VoxFormat::IsFilled(const std::vector<bool>& filled, int index)
{
   if (index < 0 || size_t(index) >= filled.size())
   {
      return false;
   }

   return filled[size_t(index)];
}

int32_t VoxFormat::Index(const Model::Metadata& metadata, uint32_t x, uint32_t y, uint32_t z)
{
   if (x >= metadata.width || y >= metadata.height || z >= metadata.length) { return -1; }
   return int32_t(x + z * metadata.width + y * metadata.width * metadata.length);
}

uint8_t VoxFormat::GetExposedFaces(const std::vector<bool>& filled, const Model::Metadata& metadata, uint32_t x, uint32_t y, uint32_t z)
{
   uint8_t faces = All;
   int right = Index(metadata, x + 1, y, z);
   int left = Index(metadata, x - 1, y, z);
   int front = Index(metadata, x, y, z - 1);
   int behind = Index(metadata, x, y, z + 1);
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

Model* VoxFormat::Load(const std::string& path, bool tintable)
{
   auto maybeModel = sDepModels.find(path);
   if (maybeModel != sDepModels.end())
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

   model->mVBO.BufferData(sizeof(Data) * model->mVoxelData.size(), &model->mVoxelData[0], GL_STATIC_DRAW);
   model->mIsTintable = tintable;

   auto emplaceResult = sDepModels.emplace(path, std::move(model));
   return emplaceResult.first->second.get();
}

Maybe<VoxModel*> VoxFormat::Load(const std::string& path)
{
   auto maybeModel = sModels.find(path);
   if (maybeModel != sModels.end())
   {
      return maybeModel->second.get();
   }

   Maybe<std::unique_ptr<VoxModelData>> maybeData = ReadScene(path);
   if (!maybeData)
   {
      return maybeData.Failure().WithContext("Failed loading scene");
   }

   // Construct a VoxModel based on the data we read.
   std::unique_ptr<VoxModel> model = std::make_unique<VoxModel>();

   std::vector<Voxel::Data> voxels{};
   std::vector<VoxModel::Part> models{};
   std::unique_ptr<VoxModelData> data = std::move(maybeData.Result());
   for (const Voxel::VoxModelData::Model& subModel : data->models)
   {
      VoxModel::Part part;
      part.tintable = false; // May be changed later
      part.start = uint32_t(voxels.size());
      part.size = 0;

      ModelData::Metadata metadata;
      metadata.width = subModel.width;
      metadata.length = subModel.length;
      metadata.height = subModel.height;

      // TODO allocation might be slow
      std::vector<bool> filled(subModel.width * subModel.height * subModel.length, false);
      for (const auto& info : subModel.voxels)
      {
         uint8_t y = (info >> 16) & 0xff;
         uint8_t z = (info >> 8) & 0xff;
         uint8_t x = info & 0xff;

         size_t ndx = (size_t)Index(metadata, x, y, z);
         filled[ndx] = true;
      }

      for (const auto& info : subModel.voxels)
      {
         Voxel::Data voxel;
         uint8_t i = (info >> 24) & 0xff;
         uint8_t y = (info >> 16) & 0xff;
         uint8_t z = (info >> 8) & 0xff;
         uint8_t x = info & 0xff;
         voxel.position.x = float(x) - float(metadata.width) / 2;
         voxel.position.y = float(y) - float(metadata.height) / 2;
         voxel.position.z = float(metadata.length) / 2 - float(z);
         uint32_t rgba = data->palette[i - 1];
         voxel.color.r = float((rgba) & 0xff);
         voxel.color.g = float((rgba >> 8) & 0xff);
         voxel.color.b = float((rgba >> 16) & 0xff);
         voxel.enabledFaces = GetExposedFaces(filled, metadata, x, y, z);
         if (voxel.enabledFaces != 0)
         {
            voxels.push_back(voxel);
            part.size ++;
         }
      }

      models.push_back(part);
   }

   // Buffer to GPU
   model->vbo.BufferData(sizeof(Data) * voxels.size(), &voxels[0], GL_STATIC_DRAW);

   // Dive down the tree, building all shapes
   std::queue<std::tuple<int32_t, uint32_t, glm::mat4>> remaining({ {int32_t(0), uint32_t(0), glm::mat4(1)} });
   model->parents.resize(data->transforms.size(), 0);
   while (!remaining.empty())
   {
      auto [id, parentID, parent] = remaining.front();
      remaining.pop();

      if (data->transforms.count(id) == 0)
      {
         return Failure("Unexpected non-nTRN node: {id}", id);
      }

      const VoxModelData::TransformNode& node = data->transforms[id];

      // Rotation
      int8_t col0 = ((node.rotate >> 0) & 0b11);
      int8_t col1 = ((node.rotate >> 2) & 0b11);
      int8_t col2 = 3 - col0 - col1;
      int8_t val0 = (node.rotate & 0b10000) ? -1 : 1;
      int8_t val1 = (node.rotate & 0b100000) ? -1 : 1;
      int8_t val2 = (node.rotate & 0b1000000) ? -1 : 1;

      glm::mat4 rotate = glm::mat4(1);
      rotate[0][0] = rotate[1][1] = rotate[2][2] = 0;

      // Another weird fallout of y/z inversion: the rotation matrix looks like
      // 1  0  0
      // 0  0 -1
      // 0  1  0
      // The value ON the identity axis (in this case, [0][0], or x) is the axis
      // we're rotating around. So in order to swap y and z correctly, we need
      // to (a) figure out what axis is being rotated around, and then shimmy the
      // values so that the REAL axis is being rotated. For example:
      // 0  0 -1       0 -1  0
      // 0  1  0   ->  1  0  0
      // 1  0  0       0  0  1
      // or:
      //  0  1  0      0  0 -1
      // -1  0  0  ->  0  1  0
      //  0  0  1      1  0  0
      // it should not change x-rotations however:
      //  1  0  0      1  0  0
      //  0  0 -1  ->  0  0 -1
      //  0  1  0      0  1  0
      // Notice that it's not a reversible transformation. If T(A) = B in the
      // first example, then T(inv(B)) = A in the second.

      if (col0 != 0)
      {
         if (col1 == 1) // Y -> Z rotation
         {
            col0 = 1;
            col1 = 0;
            col2 = 2;
            val1 = val2;
            val2 = 1; // previous val1
         }
         else // if col2 == 2, Z -> Y rotation
         {
            col0 = 2;
            col1 = 1;
            col2 = 0;
            val0 = -val0;
            val2 = -val1;
            val1 = 1; // previous val2
         }
      }

      rotate[col0][0] = val0;
      rotate[col1][1] = val1;
      rotate[col2][2] = val2;

      VoxModel::Part part;
      part.id = uint32_t(model->parts.size());
      part.name = part.id > 0 ? node.name : "root";
      if (part.name == "")
      {
         part.name = FormatString("Unnammed node {id}", part.id);
      }
      // Combine rotation and translation
      part.transform = glm::translate(parent, glm::vec3{
         node.translate[0],
         node.translate[2],
         -node.translate[1],
      }) * rotate;
      part.tintable = false;
      part.hidden = false;
      part.size = part.start = 0;
      model->parents[part.id] = parentID;

      part.position = glm::vec3{node.translate[0],node.translate[2],-node.translate[1]};
      part.rotation.y = DEGREES(asin(-rotate[0][2]));
      if (cos(part.rotation.y) != 0) {
         part.rotation.x = DEGREES(atan2(rotate[1][2], rotate[2][2]));
         part.rotation.z = DEGREES(atan2(rotate[0][1], rotate[0][0]));
      } else {
         part.rotation.x = DEGREES(atan2(-rotate[2][0], rotate[1][1]));
         part.rotation.z = 0;
      }

      // To reconstruct part.transform, run the following:
      // part.transform = glm::translate(glm::mat4(1), part.position);
      // part.transform = glm::rotate(part.transform, part.rotation.y, glm::vec3(0, 1, 0));
      // part.transform = glm::rotate(part.transform, part.rotation.x, glm::vec3(1, 0, 0));
      // part.transform = glm::rotate(part.transform, part.rotation.z, glm::vec3(0, 0, 1));

      // Respect layer names
      if (node.layer >= 0)
      {
         const VoxModelData::Layer& layer = data->layers[size_t(node.layer)];
         if (layer.name == "Tintable")
         {
            part.tintable = true;
         }
         else if (layer.name == "Exclude")
         {
            //LOG_DEBUG("Ignoring node {id} ({name}), which is in the Exclude layer", node.id, node.name);
            continue;
         }
         else if (layer.name == "Hidden" || layer.name == "Debug" || layer.hidden)
         {
            part.hidden = true;
         }
      }

      // Figure out what kind of node this is
      if (data->groups.find(node.child) != data->groups.end())
      {
         const VoxModelData::GroupNode& group = data->groups[node.child];

         for (const int32_t& child : group.children)
         {
            remaining.push({ child, part.id, part.transform });
         }

         // Add to the final model
         model->parts.push_back(part);
         if (part.name != "")
         {
            model->partLookup.emplace(part.name, model->parts.size() - 1);
         }
      }
      else if (data->shapes.find(node.child) != data->shapes.end())
      {
         const VoxModelData::ShapeNode& shape = data->shapes[node.child];

         part.start = models[(size_t)shape.model].start;
         part.size = models[(size_t)shape.model].size;

         // Add to the final model
         model->parts.push_back(part);
         if (part.name != "")
         {
            model->partLookup.emplace(part.name, model->parts.size() - 1);
         }
      }
      else
      {
         return Failure("Expected a shape or group, but got transform for node {id}", node.child);
      }
   }

   auto emplaceResult = sModels.emplace(path, std::move(model));
   return emplaceResult.first->second.get();
}

Maybe<std::unique_ptr<VoxModelData>> VoxFormat::ReadScene(const std::string& path)
{
   std::unique_ptr<VoxModelData> model = std::make_unique<VoxModelData>();

   FileSystem& fs = Engine::FileSystemProvider::Instance();
   Maybe<FileSystem::FileHandle> maybeHandle = fs.OpenFileRead(path);
   if (!maybeHandle)
   {
      return maybeHandle.Failure().WithContext("Failed opening model");
   }

   FileSystem::FileHandle handle = maybeHandle.Result();
   CUBEWORLD_SCOPE_EXIT([&] { fs.CloseFile(handle); });

   FileHeader header;
   if (Maybe<void> read = fs.ReadFile(handle, &header, sizeof(FileHeader)); !read)
   {
      return read.Failure().WithContext("Failed reading file header");
   }

   if (strncmp(header.id, "VOX ", 4) != 0)
   {
      return Failure("Header ID ({id}) did not match expected (VOXV)", std::string(header.id, header.id + sizeof(header.id)));
   }

   if (header.version != 150)
   {
      return Failure("Header version is {version}. Only 150 is supported", header.version);
   }

   Maybe<Chunk> maybeChunk = ReadChunk(fs, handle);
   if (!maybeChunk)
   {
      return maybeChunk.Failure().WithContext("Failed reading RIFF file");
   }

   Chunk chunk = std::move(maybeChunk.Result());

   if (chunk.header.id != Chunk::MAIN)
   {
      return Failure("First chunk was {chunkId} ({headerId}), expected MAIN ({main}).", chunk.id(), chunk.header.id, Chunk::MAIN);
   }

   for (const Chunk& node : chunk.children)
   {
      if (Maybe<void> result = ParseChunk(model.get(), node); !result)
      {
         return result.Failure().WithContext("Failed parsing {id} chunk", node.id());
      }
   }

   return std::move(model);
}

Maybe<std::unique_ptr<ModelData>> VoxFormat::Read(const std::string& path, bool tintable)
{
   Maybe<std::unique_ptr<VoxModelData>> maybeModel = ReadScene(path);

   if (!maybeModel)
   {
      return maybeModel.Failure();
   }

   std::unique_ptr<VoxModelData> model = std::move(maybeModel.Result());

   if (model->shapes.size() > 1)
   {
      return Failure("Multi-shape models not yet supported");
   }

   const VoxModelData::Model& shape = model->models[0];

   std::unique_ptr<ModelData> result = std::make_unique<ModelData>();

   result->mMetadata.width = shape.width;
   result->mMetadata.height = shape.height;
   result->mMetadata.length = shape.length;
   result->mIsTintable = tintable;

   std::vector<bool> filled(shape.width * shape.height * shape.length, false);
   for (const auto& info : shape.voxels)
   {
      uint8_t y = (info >> 16) & 0xff;
      uint8_t z = (info >> 8) & 0xff;
      uint8_t x = info & 0xff;

      size_t ndx = (size_t)Index(result->mMetadata, x, y, z);
      filled[ndx] = true;
   }

   for (const auto& info : shape.voxels)
   {
      Voxel::Data voxel;
      uint8_t i = (info >> 24) & 0xff;
      uint8_t y = (info >> 16) & 0xff;
      uint8_t z = (info >> 8) & 0xff;
      uint8_t x = info & 0xff;
      voxel.position.x = float(x) - float(result->mMetadata.width - 1) / 2;
      voxel.position.y = float(y) - float(result->mMetadata.height) / 2;
      voxel.position.z = float(result->mMetadata.length - 1) / 2 - float(z);
      uint32_t rgba = model->palette[i - 1];
      voxel.color.r = float((rgba) & 0xff);
      voxel.color.g = float((rgba >> 8) & 0xff);
      voxel.color.b = float((rgba >> 16) & 0xff);
      voxel.enabledFaces = GetExposedFaces(filled, result->mMetadata, x, y, z);
      result->mVoxelData.push_back(voxel);
   }

   return std::move(result);
}

Maybe<void> VoxFormat::Write(const std::string& path, const VoxModelData& voxModelData)
{
   FileSystem& fs = Engine::FileSystemProvider::Instance();
   Maybe<FileSystem::FileHandle> maybeHandle = fs.OpenFileWrite(path);
   if (!maybeHandle)
   {
      return maybeHandle.Failure().WithContext("Failed opening model");
   }

   FileSystem::FileHandle handle = maybeHandle.Result();
   CUBEWORLD_SCOPE_EXIT([&] { fs.CloseFile(handle); });

   // Write overarching file headers
   std::vector<uint8_t> data {
      'V', 'O', 'X', ' ',
      150, 0, 0, 0,
      'M', 'A', 'I', 'N',
      0, 0, 0, 0,
      0, 0, 0, 0 /* determined later */
   };
   if (Maybe<void> write = fs.WriteFile(handle, &data[0], data.size()); !write)
   {
      return write.Failure().WithContext("Failed to write metadata header");
   }

   size_t chunkBytes = 0; // Saved to offset 0x10 after computing it.

   // Then write model data
   data.resize(sizeof(Chunk::Header) + sizeof(SIZE));
   for (const VoxModelData::Model& model : voxModelData.models)
   {
      Chunk::Header* header = (Chunk::Header*)&data[0];
      header->id = Chunk::SIZE;
      header->length = sizeof(SIZE);
      header->childLength = 0;

      SIZE* size = (SIZE*)&data[sizeof(Chunk::Header)];
      size->width = model.width;
      size->height = model.height;
      size->length = model.length;

      if (Maybe<void> write = fs.WriteFile(handle, &data[0], data.size()); !write)
      {
         return write.Failure().WithContext("Failed to write {size}-byte SIZE chunk", data.size());
      }
      chunkBytes += data.size();

      header->id = Chunk::XYZI;
      header->length = uint32_t(sizeof(uint32_t) + sizeof(uint32_t) * model.voxels.size());
      header->childLength = 0;

      // Write num voxels along with the header, but not data.
      uint32_t* cursor = (uint32_t*)(header + 1);
      *cursor = uint32_t(model.voxels.size());

      if (Maybe<void> write = fs.WriteFile(handle, &data[0], sizeof(Chunk::Header) + sizeof(uint32_t)); !write)
      {
         return write.Failure().WithContext("Failed to write {size}-byte XYZI chunk", sizeof(Chunk::Header) + sizeof(uint32_t));
      }
      chunkBytes += data.size();

      // Write the voxel data next.
      if (Maybe<void> write = fs.WriteFile(handle, (void*)&model.voxels[0], sizeof(uint32_t) * model.voxels.size()); !write)
      {
         return write.Failure().WithContext("Failed to write {num} voxels", sizeof(uint32_t) * model.voxels.size());
      }
      chunkBytes += data.size();
   }

   // Write render transforms (pretty much unused)
   int32_t nNodes = int32_t(voxModelData.transforms.size() + voxModelData.groups.size() + voxModelData.shapes.size());
   for (int32_t id = 0; id < nNodes; id ++)
   {
      if (voxModelData.transforms.count(id) != 0)
      {
         const VoxModelData::TransformNode& transform = voxModelData.transforms.at(id);

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
         header->length = uint32_t(chunkSize);
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
         WriteDict((int32_t*)(body + 1), frame);
      }
      else if (voxModelData.groups.find(id) != voxModelData.groups.end())
      {
         const VoxModelData::GroupNode& group = voxModelData.groups.at(id);

         // Compute how big to make the chunk
         size_t chunkSize = sizeof(nGRP::Header)
                          + sizeof(nGRP::Body)
                          + sizeof(int32_t) /* DICT header */
                          + group.children.size() * sizeof(int32_t);

         // Start writing data.
         data.resize(sizeof(Chunk::Header) + chunkSize);

         Chunk::Header* header = (Chunk::Header*)&data[0];
         header->id = Chunk::nGRP;
         header->length = uint32_t(chunkSize);
         header->childLength = 0;

         // Write header
         nGRP::Header* nodeHeader = (nGRP::Header*)(header + 1);
         nodeHeader->id = id;

         // Write node attributes (none)
         int32_t* attributes = (int32_t*)(nodeHeader + 1);
         *attributes = 0;

         // Write body
         nGRP::Body* body = (nGRP::Body*)(attributes + 1);
         body->children = int32_t(group.children.size());

         int32_t* cursor = (int32_t*)(body + 1);
         for (const int32_t& child : group.children)
         {
            *cursor++ = child;
         }
      }
      else if (voxModelData.shapes.find(id) != voxModelData.shapes.end())
      {
         const VoxModelData::ShapeNode& shape = voxModelData.shapes.at(id);

         // Compute how big to make the chunk
         size_t chunkSize = sizeof(nSHP::Header)
                          + sizeof(nSHP::Body)
                          + 2 * sizeof(int32_t) /* DICT headers */
                          + sizeof(int32_t) /* Model ID */;

         // Start writing data.
         data.resize(sizeof(Chunk::Header) + chunkSize);

         Chunk::Header* header = (Chunk::Header*)&data[0];
         header->id = Chunk::nSHP;
         header->length = uint32_t(chunkSize);
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
         return Failure("Could not find a node with ID {id}", id);
      }

      // Write the node.
      if (Maybe<void> write = fs.WriteFile(handle, &data[0], data.size()); !write)
      {
         Chunk::Header* header = (Chunk::Header*)&data[0];
         return write.Failure().WithContext("Failed to write {size}-byte %2 chunk", data.size(), Chunk::ReadChunkID(header->id));
      }
      chunkBytes += data.size();
   }

   // Write layer info
   for (const VoxModelData::Layer& layer : voxModelData.layers)
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
      // int32   : layer id
      // DICT   : layer atrribute
      //    (_name : string)
      //    (_hidden : 0/1)
      // int32   : reserved id, must be -1

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
      header->length = uint32_t(chunkSize);
      header->childLength = 0;

      // Write header
      int32_t* cursor = (int32_t*)(header + 1);
      *cursor++ = layer.id;

      // Write node attributes
      cursor = WriteDict(cursor, attributes);
      *cursor++ = -1;

      // Write the chunk to disk.
      if (Maybe<void> write = fs.WriteFile(handle, &data[0], data.size()); !write)
      {
         return write.Failure().WithContext("Failed to write {size}-byte LAYR chunk", data.size());
      }
      chunkBytes += data.size();
   }

   // Write the palette
   {
      data.resize(sizeof(Chunk::Header) + sizeof(voxModelData.palette));

      Chunk::Header* header = (Chunk::Header*)&data[0];
      header->id = Chunk::RGBA;
      header->length = sizeof(voxModelData.palette);
      header->childLength = 0;

      memcpy(header + 1, voxModelData.palette, sizeof(voxModelData.palette));

      if (Maybe<void> write = fs.WriteFile(handle, &data[0], data.size()); !write)
      {
         return write.Failure().WithContext("Failed to write {size}-byte RGBA chunk", data.size());
      }
      chunkBytes += data.size();
   }

   // Write materials
   std::vector<VoxModelData::Material> materials(voxModelData.materials.begin(), voxModelData.materials.end());
   while (materials.size() < 256)
   {
      VoxModelData::Material material = VoxModelData::Material::Default;
      material.id = int32_t(materials.size());
      materials.push_back(material);
   }

   for (const VoxModelData::Material& material : materials)
   {
      std::vector<std::pair<std::string, std::string>> properties;
      switch (material.type)
      {
         case VoxModelData::Material::Diffuse:
            properties.push_back(std::make_pair("_type", "_diffuse"));
            break;
         case VoxModelData::Material::Emit:
            properties.push_back(std::make_pair("_type", "_emit"));
            break;
         case VoxModelData::Material::Glass:
            properties.push_back(std::make_pair("_type", "_glass"));
            break;
         case VoxModelData::Material::Metal:
            properties.push_back(std::make_pair("_type", "_metal"));
            break;
         default:
            return Failure("Material {id} does not have a type", material.id);
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
      header->length = uint32_t(chunkSize);
      header->childLength = 0;

      // Write header
      int32_t* cursor = (int32_t*)(header + 1);
      *cursor++ = material.id;

      // Write properties
      cursor = WriteDict(cursor, properties);

      // Write the chunk to disk.
      if (Maybe<void> write = fs.WriteFile(handle, &data[0], data.size()); !write)
      {
         return write.Failure().WithContext("Failed to write {size}-byte MATL chunk", data.size());
      }
      chunkBytes += data.size();
   }

   // Write render objects
   {
      // Sun
      Maybe<size_t> result = WriteRenderObj(fs, handle, {
         { "_type", "_inf" },
         { "_i", ToShortString(voxModelData.sun.intensity) },
         { "_k",
           FormatString("{} {} {}",
              voxModelData.sun.color[0],
              voxModelData.sun.color[1],
              voxModelData.sun.color[2]) },
         { "_angle", FormatString("{} {}", voxModelData.sun.angle[0], voxModelData.sun.angle[1]) },
         { "_area", ToShortString(voxModelData.sun.area) },
         { "_disk", voxModelData.sun.disk ? "1" : "0" },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Uniform sky
      result = WriteRenderObj(fs, handle, {
         { "_type", "_uni" },
         { "_i", ToShortString(voxModelData.sky.intensity) },
         { "_k",
           FormatString("{} {} {}",
              voxModelData.sky.color[0],
              voxModelData.sky.color[1],
              voxModelData.sky.color[2]) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Atmospheric sky
      result = WriteRenderObj(fs, handle, {
         { "_type", "_atm" },
         { "_ray_d", ToShortString(voxModelData.atm.rayleighDensity) },
         { "_ray_k",
           FormatString("{} {} {}",
              voxModelData.atm.rayleighColor[0],
              voxModelData.atm.rayleighColor[1],
              voxModelData.atm.rayleighColor[2]) },
         { "_mie_d", ToShortString(voxModelData.atm.mieDensity) },
         { "_mie_k",
           FormatString("{} {} {}",
              voxModelData.atm.mieColor[0],
              voxModelData.atm.mieColor[1],
              voxModelData.atm.mieColor[2]) },
         { "_mie_g", ToShortString(voxModelData.atm.miePhase) },
         { "_o3_d", ToShortString(voxModelData.atm.ozoneDensity) },
         { "_o3_k",
           FormatString("{} {} {}",
              voxModelData.atm.ozoneColor[0],
              voxModelData.atm.ozoneColor[1],
              voxModelData.atm.ozoneColor[2]) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Uniform fog
      result = WriteRenderObj(fs, handle, {
         { "_type", "_fog_uni" },
         { "_d", ToShortString(voxModelData.fog.density) },
         { "_k",
           FormatString("{} {} {}",
              voxModelData.fog.color[0],
              voxModelData.fog.color[1],
              voxModelData.fog.color[2]) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Lens
      result = WriteRenderObj(fs, handle, {
         { "_type", "_lens" },
         { "_fov", std::to_string(voxModelData.lens.fov) },
         { "_dof", ToShortString(voxModelData.lens.depthOfField) },
         { "_expo", voxModelData.lens.exposure ? "1" : "0" },
         { "_vig", voxModelData.lens.vignette ? "1" : "0" },
         { "_sg", voxModelData.lens.stereographics ? "1" : "0" },
         { "_blade_n", std::to_string(voxModelData.lens.bladeNumber) },
         { "_blade_r", std::to_string(voxModelData.lens.bladeRotation) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Bloom
      result = WriteRenderObj(fs, handle, {
         { "_type", "_bloom" },
         { "_mix", ToShortString(voxModelData.bloom.mix) },
         { "_scale", ToShortString(voxModelData.bloom.scale) },
         { "_aspect", ToShortString(voxModelData.bloom.aspect) },
         { "_threshold", ToShortString(voxModelData.bloom.threshold) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Tone
      result = WriteRenderObj(fs, handle, {
         { "_type", "_tone" },
         { "_aces", voxModelData.tone.aces ? "1" : "0" },
         { "_gam", ToShortString(voxModelData.tone.gamma) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Ground
      result = WriteRenderObj(fs, handle, {
         { "_type", "_ground" },
         { "_color",
           FormatString("{} {} {}",
              voxModelData.ground.color[0],
              voxModelData.ground.color[1],
              voxModelData.ground.color[2]) },
         { "_hor", ToShortString(voxModelData.ground.horizon) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Background
      result = WriteRenderObj(fs, handle, {
         { "_type", "_bg" },
         { "_color",
           FormatString("{} {} {}",
              voxModelData.bg.color[0],
              voxModelData.bg.color[1],
              voxModelData.bg.color[2]) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Edge
      result = WriteRenderObj(fs, handle, {
         { "_type", "_edge" },
         { "_color",
           FormatString("{} {} {}",
              voxModelData.edge.color[0],
              voxModelData.edge.color[1],
              voxModelData.edge.color[2]) },
         { "_width", ToShortString(voxModelData.edge.width) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Grid
      result = WriteRenderObj(fs, handle, {
         { "_type", "_grid" },
         { "_color",
           FormatString("{} {} {}",
              voxModelData.grid.color[0],
              voxModelData.grid.color[1],
              voxModelData.grid.color[2]) },
         { "_spacing", std::to_string(voxModelData.grid.spacing) },
         { "_width", std::to_string(voxModelData.grid.width) },
         { "_display", voxModelData.grid.onGround ? "1" : "0" },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;

      // Settings
      result = WriteRenderObj(fs, handle, {
         { "_type", "_setting" },
         { "_ground", voxModelData.settings.ground ? "1" : "0" },
         { "_sw", voxModelData.settings.shadow ? "1" : "0" },
         { "_aa", voxModelData.settings.antialias ? "1" : "0" },
         { "_grid", voxModelData.settings.grid ? "1" : "0" },
         { "_edge", voxModelData.settings.edge ? "1" : "0" },
         { "_bg_c", voxModelData.settings.background ? "1" : "0" },
         { "_bg_a", voxModelData.settings.bgTransparent ? "1" : "0" },
         { "_scale",
           FormatString("{} {} {}",
              voxModelData.settings.scale[0],
              voxModelData.settings.scale[1],
              voxModelData.settings.scale[2]) },
      });
      if (!result)
      {
         return result.Failure();
      }
      chunkBytes += *result;
   }

   if (Maybe<void> seek = fs.SeekFile(handle, FileSystem::Seek::BEGIN, 0x10); !seek)
   {
      return seek.Failure().WithContext("Failed to seek to offset 0x10");
   }

   if (Maybe<void> write = fs.WriteFile(handle, &chunkBytes, sizeof(chunkBytes)); !write)
   {
      return write.Failure().WithContext("Failed to write size of MAIN chunk");
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

   VoxModelData voxModelData;
   voxModelData.materials.clear();
   voxModelData.groups.clear();
   voxModelData.shapes.clear();
   voxModelData.transforms.clear();
   voxModelData.layers.clear();

   VoxModelData::Model model;
   model.width = modelData.mMetadata.width;
   model.height = modelData.mMetadata.height;
   model.length = modelData.mMetadata.length;

   uint8_t available = 255;
   memcpy(voxModelData.palette, default_palette, sizeof(voxModelData.palette));
   for (size_t i = 0; i < 255; i ++)
   {
      voxModelData.palette[i] = default_palette[i];
   }
   // memset(voxModelData.palette, 0, sizeof(voxModelData.palette));
   for (const Voxel::Data& voxel : modelData.mVoxelData)
   {
      uint32_t rgba = uint32_t(
         (uint8_t(voxel.color.r)) |
         (uint8_t(voxel.color.g) << 8) |
         (uint8_t(voxel.color.b) << 16) |
         0xff << 24
      );

      uint8_t colorIndex = 255;
      for (; colorIndex > 1; colorIndex --)
      {
         if (voxModelData.palette[colorIndex - 1] == rgba)
         {
            break;
         }
      }
      if (colorIndex == 1)
      {
         if (available == 0)
         {
            return Failure("Too many colors");
         }
         colorIndex = available--;

         voxModelData.palette[colorIndex - 1] = rgba;
      }
      else if (colorIndex <= available)
      {
         available = uint8_t(colorIndex - 1);
         if (available < 0)
         {
            return Failure("Too many colors");
         }

         voxModelData.palette[colorIndex - 1] = rgba;
      }

      // Inverse of how we do centering.
      int32_t x = int32_t((modelData.mMetadata.width - 1)  / 2 + int32_t(voxel.position.x));
      int32_t z = int32_t( modelData.mMetadata.height      / 2 + int32_t(voxel.position.y));
      int32_t y = int32_t((modelData.mMetadata.length - 1) / 2 - int32_t(voxel.position.z));

      uint32_t packed = uint32_t(
         uint8_t(x) |
         (uint8_t(y) << 8) |
         (uint8_t(z) << 16) |
         (colorIndex << 24)
      );
      model.voxels.push_back(packed);
   };
   for (int i = 1; i < 256; i ++)
   {
      // voxModelData.palette[i] = 0xff00f285;
   }

   voxModelData.models.push_back(model);

   voxModelData.layers.push_back({0, "0", false});
   voxModelData.layers.push_back({1, "1", false});
   voxModelData.layers.push_back({2, "2", false});
   voxModelData.layers.push_back({3, "3", false});
   voxModelData.layers.push_back({4, "4", false});
   voxModelData.layers.push_back({5, "5", false});
   voxModelData.layers.push_back({6, "6", false});
   voxModelData.layers.push_back({7, "7", false});

   VoxModelData::TransformNode transform;
   transform.id = 0;
   transform.child = 1;
   transform.layer = -1;
   voxModelData.transforms.emplace(transform.id, transform);

   VoxModelData::GroupNode group;
   group.id = 1;
   group.children.push_back(2);
   voxModelData.groups.emplace(group.id, group);

   transform.id = 2;
   transform.child = 3;
   transform.layer = 0;
   voxModelData.transforms.emplace(transform.id, transform);

   VoxModelData::ShapeNode shape;
   shape.id = 3;
   shape.model = 0;
   voxModelData.shapes.emplace(shape.id, shape);

   return Write(path, voxModelData);
}

}; // namespace Voxel

}; // namespace CubeWorld
