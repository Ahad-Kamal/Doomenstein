#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <vector>
#include <string>


//-----------------------------------------------------------------------------------------------
class Image;
class Shader;
class Texture;

//-----------------------------------------------------------------------------------------------
struct SpawnInfo
{
	std::string m_actorName;
	Vec3 m_position;
	Vec3 m_velocity;
	EulerAngles m_orientation;
};

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

	static MapDefinition* GetMapDefFromName( std::string name );

public:
	static std::vector<MapDefinition> s_mapDefs;

private:
	std::vector<SpawnInfo> m_spawnPoints;
	std::string m_name;
	Image* m_image;
	Shader* m_shader;
	Texture* m_spriteSheetTexture;
	IntVec2 m_spriteSheetCellCount;
};