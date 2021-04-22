// By Thomas Steinke

#pragma once

#include <string>

#include <RGBFileSystem/Paths.h>

namespace CubeWorld
{

namespace Asset
{

void SetAssetRootDefault();
void SetAssetRoot(const std::string& root);
void SetShaderRoot(const std::string& root);

std::string Animation(const std::string& animation);
std::string Model(const std::string& model);
std::string Image(const std::string& model);
std::string Font(const std::string& fontName);
std::string ParticleShaders();
std::string Particle(const std::string& particle);
std::string Skeleton(const std::string& skeleton);
std::string UIElement(const std::string& elementName);
std::string Shader(const std::string& shaderName);
std::string Script(const std::string& scriptName);

};

}; // namespace CubeWorld
