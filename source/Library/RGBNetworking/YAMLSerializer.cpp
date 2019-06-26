// By Thomas Steinke

#include <stack>
#include <yaml.h>

#include <RGBDesignPatterns/Scope.h>
#include <RGBBinding/BindingPropertyReader.h>
#include "YAMLWriter.h"
#include "YAMLSerializer.h"

namespace CubeWorld
{

namespace YAMLSerializerInternal
{

inline double Pow10(int n) {
   static const double e[] = { // 1e-0...1e308: 309 * 8 bytes = 2472 bytes
      1e+0,
      1e+1,  1e+2,  1e+3,  1e+4,  1e+5,  1e+6,  1e+7,  1e+8,  1e+9,  1e+10, 1e+11, 1e+12, 1e+13, 1e+14, 1e+15, 1e+16, 1e+17, 1e+18, 1e+19, 1e+20,
      1e+21, 1e+22, 1e+23, 1e+24, 1e+25, 1e+26, 1e+27, 1e+28, 1e+29, 1e+30, 1e+31, 1e+32, 1e+33, 1e+34, 1e+35, 1e+36, 1e+37, 1e+38, 1e+39, 1e+40,
      1e+41, 1e+42, 1e+43, 1e+44, 1e+45, 1e+46, 1e+47, 1e+48, 1e+49, 1e+50, 1e+51, 1e+52, 1e+53, 1e+54, 1e+55, 1e+56, 1e+57, 1e+58, 1e+59, 1e+60,
      1e+61, 1e+62, 1e+63, 1e+64, 1e+65, 1e+66, 1e+67, 1e+68, 1e+69, 1e+70, 1e+71, 1e+72, 1e+73, 1e+74, 1e+75, 1e+76, 1e+77, 1e+78, 1e+79, 1e+80,
      1e+81, 1e+82, 1e+83, 1e+84, 1e+85, 1e+86, 1e+87, 1e+88, 1e+89, 1e+90, 1e+91, 1e+92, 1e+93, 1e+94, 1e+95, 1e+96, 1e+97, 1e+98, 1e+99, 1e+100,
      1e+101,1e+102,1e+103,1e+104,1e+105,1e+106,1e+107,1e+108,1e+109,1e+110,1e+111,1e+112,1e+113,1e+114,1e+115,1e+116,1e+117,1e+118,1e+119,1e+120,
      1e+121,1e+122,1e+123,1e+124,1e+125,1e+126,1e+127,1e+128,1e+129,1e+130,1e+131,1e+132,1e+133,1e+134,1e+135,1e+136,1e+137,1e+138,1e+139,1e+140,
      1e+141,1e+142,1e+143,1e+144,1e+145,1e+146,1e+147,1e+148,1e+149,1e+150,1e+151,1e+152,1e+153,1e+154,1e+155,1e+156,1e+157,1e+158,1e+159,1e+160,
      1e+161,1e+162,1e+163,1e+164,1e+165,1e+166,1e+167,1e+168,1e+169,1e+170,1e+171,1e+172,1e+173,1e+174,1e+175,1e+176,1e+177,1e+178,1e+179,1e+180,
      1e+181,1e+182,1e+183,1e+184,1e+185,1e+186,1e+187,1e+188,1e+189,1e+190,1e+191,1e+192,1e+193,1e+194,1e+195,1e+196,1e+197,1e+198,1e+199,1e+200,
      1e+201,1e+202,1e+203,1e+204,1e+205,1e+206,1e+207,1e+208,1e+209,1e+210,1e+211,1e+212,1e+213,1e+214,1e+215,1e+216,1e+217,1e+218,1e+219,1e+220,
      1e+221,1e+222,1e+223,1e+224,1e+225,1e+226,1e+227,1e+228,1e+229,1e+230,1e+231,1e+232,1e+233,1e+234,1e+235,1e+236,1e+237,1e+238,1e+239,1e+240,
      1e+241,1e+242,1e+243,1e+244,1e+245,1e+246,1e+247,1e+248,1e+249,1e+250,1e+251,1e+252,1e+253,1e+254,1e+255,1e+256,1e+257,1e+258,1e+259,1e+260,
      1e+261,1e+262,1e+263,1e+264,1e+265,1e+266,1e+267,1e+268,1e+269,1e+270,1e+271,1e+272,1e+273,1e+274,1e+275,1e+276,1e+277,1e+278,1e+279,1e+280,
      1e+281,1e+282,1e+283,1e+284,1e+285,1e+286,1e+287,1e+288,1e+289,1e+290,1e+291,1e+292,1e+293,1e+294,1e+295,1e+296,1e+297,1e+298,1e+299,1e+300,
      1e+301,1e+302,1e+303,1e+304,1e+305,1e+306,1e+307,1e+308
   };
   assert(n >= 0 && n <= 308);
   return e[n];
}

inline double FastPath(double significand, int exp) {
   if (exp < -308)
      return 0.0;
   else if (exp >= 0)
      return significand * Pow10(exp);
   else
      return significand / Pow10(-exp);
}

// Returns true if a number was successfully parsed.
bool ParseNumber(BindingPropertyReader& reader, yaml_char_t* str, size_t /*len*/)
{
   yaml_char_t* start = str;

   double d = 0.0;
   bool is64Bit = false;
   bool isDouble = false;
   uint16_t significandDigit = 0;
   uint32_t i32 = 0;
   uint64_t i64 = 0;
   
   // Parse minus first
   bool neg = *str == '-';
   if (neg) { ++str; }

   if (*str == '0')
   {
      // Move on; possibly 0 or decimal
      ++str;
   }
   else if (*str >= '1' && *str <= '9')
   {
      i32 = (*str++ - '0');

      if (neg)
      {
         while (*str >= '0' && *str <= '9')
         {
            // INT_MIN is 2147483648, see if we would exceed that
            if (i32 >= 214748364 && (i32 != 214748364 || *str > '8'))
            {
               i64 = i32;
               is64Bit = true;
               break;
            }
            i32 = i32 * 10 + (*str++ - '0');
            ++significandDigit;
         }
      }
      else
      {
         while (*str >= '0' && *str <= '9')
         {
            // UINT_MAX is 4294967295, see if we would exceed that
            if (i32 >= 429496729 && (i32 != 429496729 || *str > '5'))
            {
               i64 = i32;
               is64Bit = true;
               break;
            }
            i32 = i32 * 10 + (*str++ - '0');
            ++significandDigit;
         }
      }
   }
   else
   {
      return false;
   }

   if (is64Bit)
   {
      if (neg)
      {
         while (*str >= '0' && *str <= '9')
         {
            // std::numeric_limits<int64_t>::min() = 9223372036854775808
            if (i64 >= 0x0CCCCCCCCCCCCCCC && (i64 != 0x0CCCCCCCCCCCCCCC || *str > '8'))
            {
               d = static_cast<double>(i64);
               isDouble = true;
               break;
            }
            i64 = i64 * 10 + (*str++ - '0');
            ++significandDigit;
         }
      }
      else
      {
         while (*str >= '0' && *str <= '9')
         {
            // std::numeric_limits<uint64_t>::max() = 18446744703709551615
            if (i64 >= 0x1999999999999999 && (i64 != 0x1999999999999999 || *str > '5'))
            {
               d = static_cast<double>(i64);
               isDouble = true;
               break;
            }
            i64 = i64 * 10 + (*str++ - '0');
            ++significandDigit;
         }
      }
   }

   if (isDouble)
   {
      while (*str >= '0' && *str <= '9')
      {
         if (d >= std::numeric_limits<double>::max())
         {
            // too big dude
            return false;
         }
         d = d * 10 + (*str++ - '0');
      }
   }

   int exponentFraction = 0;
   size_t decimalPos;
   if (*str == '.')
   {
      decimalPos = ++str - start;

      if (*str < '0' || *str > '9')
      {
         return false;
      }

      if (!isDouble)
      {
         d = static_cast<double>(is64Bit ? i64 : i32);
         isDouble = true;
      }

      while (*str >= '0' && *str <= '9')
      {
         if (significandDigit < 17)
         {
            d = d * 10.0 + (*str++ - '0');
            --exponentFraction;
            if (d > 0)
            {
               ++significandDigit;
            }
         }
         else
         {
            // too many digits
            ++str;
         }
      }
   }
   else
   {
      decimalPos = str - start;
   }

   int exponent = 0;
   if (*str == 'e' || *str == 'E')
   {
      ++str;
      if (!isDouble)
      {
         d = static_cast<double>(is64Bit ? i64 : i32);
         isDouble = true;
      }

      bool negExp = false;
      if (*str == '+') { ++str; }
      else if (*str == '-')
      {
         negExp = true;
         ++str;
      }

      if (*str < '0' || *str > '9')
      {
         return false;
      }

      exponent = *str++ - '0';
      if (negExp)
      {
         while (*str >= '0' && *str <= '9')
         {
            exponent = exponent * 10 + (*str++ - '0');
            // std::numeric_limits<int>::max() = 2147483647
            if (exponent >= 214748364)
            {
               while (*str >= '0' && *str <= '9') { ++str; }
            }
         }
         exponent = -exponent;
      }
      else
      {
         int maxExponent = 308 - exponentFraction;
         while (*str >= '0' && *str <= '9')
         {
            exponent = exponent * 10 + (*str++ - '0');
            if (exponent > maxExponent)
            {
               // too big dude
               return false;
            }
         }
      }
   }

   // Done parsing!
   if (isDouble)
   {
      int p = exponent + exponentFraction;
      if (p < -308) {
         // Prevent expSum < -308, making Pow10(p) = 0
         d = FastPath(d, -308);
         d = FastPath(d, p + 308);
      }
      else
         d = FastPath(d, p);
      return reader.Double(neg ? -d : d);
   }
   else if (is64Bit && neg)
   {
      return reader.Int64(static_cast<int64_t>(~i64 + 1));
   }
   else if (is64Bit)
   {
      return reader.Uint64(i64);
   }
   else if (neg)
   {
      return reader.Int(static_cast<int64_t>(~i32 + 1));
   }
   else
   {
      return reader.Uint(i32);
   }
}

int WriteString(void* _output, unsigned char *buffer, size_t size) {
   std::string* output = static_cast<std::string*>(_output);
   std::string toAdd((char*)buffer, size);

   (*output) += toAdd;

   return 1;
}

}; // namespace YAMLSerializerInternal

Maybe<BindingProperty> YAMLSerializer::Deserialize(const std::string& buffer)
{
   yaml_parser_t parser;
   yaml_parser_initialize(&parser);
   CUBEWORLD_SCOPE_EXIT([&]() { yaml_parser_delete(&parser); });

   int done = 0;

   yaml_parser_set_input_string(&parser, (const unsigned char*)buffer.data(), buffer.size());

   BindingPropertyReader reader;

   yaml_event_t event;
   while (!done)
   {
      if (!yaml_parser_parse(&parser, &event))
      {
         return Failure(parser.error, "Failed parsing buffer: Error %1", parser.error);
      }

      bool map = false;
      switch (event.type) {
      case YAML_NO_EVENT:
      case YAML_STREAM_START_EVENT:
      case YAML_STREAM_END_EVENT:
      case YAML_DOCUMENT_START_EVENT:
      case YAML_DOCUMENT_END_EVENT:
         // Don't care
         break;
      case YAML_MAPPING_START_EVENT:
         if (!reader.StartObject())
         {
            return Failure{"Failed starting object"};
         }
         map = true;
         break;
      case YAML_MAPPING_END_EVENT:
         if (!reader.EndObject())
         {
            return Failure{"Failed ending object"};
         }
         break;
      case YAML_ALIAS_EVENT:
         break;
      case YAML_SCALAR_EVENT:
         if (reader.CurrentIsObject())
         {
            if (!reader.Key((char*)event.data.scalar.value, (rapidjson::SizeType)event.data.scalar.length, true))
            {
               return Failure{"Failed writing key"};
            }
         }
         else
         {
            // Try to parse a number first, then interpret as a string
            if (!YAMLSerializerInternal::ParseNumber(reader, event.data.scalar.value, event.data.scalar.length))
            {
               if (event.data.scalar.length == 4 && strncmp((char*)event.data.scalar.value, "true", 4) == 0)
               {
                  if (!reader.Bool(true))
                  {
                     return Failure{"Failed writing true"};
                  }
               }
               else if (event.data.scalar.length == 5 && strncmp((char*)event.data.scalar.value, "false", 5) == 0)
               {
                  if (!reader.Bool(false))
                  {
                     return Failure{"Failed writing false"};
                  }
               }
               else 
               if (!reader.String((char*)event.data.scalar.value, (rapidjson::SizeType)event.data.scalar.length, true))
               {
                  return Failure{"Failed writing string"};
               }
            }
         }
         break;
      case YAML_SEQUENCE_START_EVENT:
         if (!reader.StartArray())
         {
            return Failure{"Failed starting array"};
         }
         break;
      case YAML_SEQUENCE_END_EVENT:
         if (!reader.EndArray())
         {
            return Failure{"Failed starting array"};
         }
         break;
      }

      done = (event.type == YAML_STREAM_END_EVENT);

      yaml_event_delete(&event);
   }

   return reader.GetResult();
}

Maybe<BindingProperty> YAMLSerializer::DeserializeFile(FileSystem& fs, const std::string& path)
{
   Maybe<std::string> maybeResult = fs.ReadEntireFile(path);
   if (!maybeResult)
   {
      return maybeResult.Failure().WithContext("Failed reading file");
   }

   return Deserialize(std::move(*maybeResult));
}

Maybe<BindingProperty> YAMLSerializer::DeserializeFile(const std::string& path)
{
   DiskFileSystem fs;
   return DeserializeFile(fs, path);
}

Maybe<std::string> YAMLSerializer::Serialize(const BindingProperty& data)
{
   yaml_emitter_t emitter;
   yaml_event_t event;

   yaml_emitter_initialize(&emitter);
   CUBEWORLD_SCOPE_EXIT([&]() { yaml_emitter_delete(&emitter); });

   std::string serialized;
   serialized.reserve(1024);

   yaml_emitter_set_output(&emitter, &YAMLSerializerInternal::WriteString, &serialized);

   yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
   if (!yaml_emitter_emit(&emitter, &event))
   {
      return Failure{"Failed to start stream"};
   }

   yaml_document_start_event_initialize(&event, nullptr, nullptr, nullptr, 0);
   if (!yaml_emitter_emit(&emitter, &event))
   {
      return Failure{"Failed to start document"};
   }

   YAMLWriter writer(emitter);
   Maybe<void> result = data.Write(writer);
   if (!result)
   {
      return result.Failure();
   }

   yaml_document_end_event_initialize(&event, 1);
   if (!yaml_emitter_emit(&emitter, &event))
   {
      return Failure{"Failed to end document"};
   }

   yaml_stream_end_event_initialize(&event);
   if (!yaml_emitter_emit(&emitter, &event))
   {
      return Failure{"Failed to end stream"};
   }

   return serialized;
}

Maybe<void> YAMLSerializer::SerializeFile(FileSystem& fs, const std::string& path, const BindingProperty& data)
{
   Maybe<std::string> serialized = Serialize(data);
   if (!serialized)
   {
      return serialized.Failure().WithContext("Failed to serialize data");
   }
   
   return fs.WriteFile(path, std::move(*serialized));
}

Maybe<void> YAMLSerializer::SerializeFile(const std::string& path, const BindingProperty& data)
{
   DiskFileSystem fs;
   return SerializeFile(fs, path, data);
}

}; // namespace CubeWorld
