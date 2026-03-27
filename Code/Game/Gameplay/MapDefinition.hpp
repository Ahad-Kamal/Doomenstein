#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <string>

//-----------------------------------------------------------------------------------------------
class Image;
class Shader;
class Texture;

//-----------------------------------------------------------------------------------------------
class MapDefinition
{
public:
	static void InitializeMapDefs();

	std::string GetName() const;
	Image* GetImage() const;
	Shader* GetShader() const;
	Texture* GetSpriteSheetTexture() const;
	IntVec2 GetSpriteSheetCellCount() const;

public:
	static std::vector<MapDefinition> s_mapDefs;

private:
	std::string m_name;
	Image* m_image;
	Shader* m_shader;
	Texture* m_spriteSheetTexture;
	IntVec2 m_spriteSheetCellCount;
};