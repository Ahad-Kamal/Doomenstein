#include "Game/Gameplay/MapDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<MapDefinition> MapDefinition::s_mapDefs;

//-----------------------------------------------------------------------------------------------
void MapDefinition::InitializeMapDefs()
{
	// Read in MapDefinitions.xml
	XmlDocument mapDefs;
	mapDefs.LoadFile( "Data/Definitions/MapDefinitions.xml" );
	int numMapDefs = mapDefs.FirstChildElement()->ChildElementCount();
	s_mapDefs.resize( numMapDefs );

	XmlElement* currentElement = mapDefs.FirstChildElement()->FirstChildElement();
	for( int elementIndex = 0; elementIndex < numMapDefs; elementIndex++ )
	{
		NamedStrings mapDefBlackboard;
		mapDefBlackboard.PopulateFromXmlElementAttributes( *currentElement );
		MapDefinition& currentMapDef = s_mapDefs[ elementIndex ];

		currentMapDef.m_name = mapDefBlackboard.GetValue( "name", "" );
		std::string imagePath = mapDefBlackboard.GetValue( "image", "" );
		std::string shaderPath = mapDefBlackboard.GetValue( "shader", "" );
		std::string texturePath = mapDefBlackboard.GetValue( "spriteSheetTexture", "" );
		currentMapDef.m_spriteSheetCellCount = mapDefBlackboard.GetValue( "spriteSheetCellCount", IntVec2() );

		if( !imagePath.empty() )
		{
			currentMapDef.m_image = new Image( imagePath.c_str() );
		}
		
		if( !shaderPath.empty() )
		{
			currentMapDef.m_shader = g_engine->m_render->CreateOrGetShader( shaderPath.c_str(), VertexType::VERTEX_PCUTBN );
		}

		if( !texturePath.empty() )
		{
			currentMapDef.m_spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( texturePath.c_str() );
		}

		XmlElement* spawnInfoElement = currentElement->FirstChildElement()->FirstChildElement();
		while( spawnInfoElement )
		{
			NamedStrings mapDefSpawnInfoBlackboard;
			mapDefSpawnInfoBlackboard.PopulateFromXmlElementAttributes( *spawnInfoElement );
			
			SpawnInfo newSpawnInfo;
			newSpawnInfo.m_actorName = mapDefSpawnInfoBlackboard.GetValue( "actor", "" );
			
			std::string position = mapDefSpawnInfoBlackboard.GetValue( "position", "0.0,0.0,0.0" );
			Strings positionValues = SplitStringOnDelimiter( position, ',' );
			newSpawnInfo.m_position = Vec3( stof( positionValues[ 0 ] ), stof( positionValues[ 1 ] ), stof( positionValues[ 2 ] ) );

			std::string velocity = mapDefSpawnInfoBlackboard.GetValue( "velocity", "0.0,0.0,0.0" );
			Strings velocityValues = SplitStringOnDelimiter( velocity, ',' );
			newSpawnInfo.m_velocity = Vec3( stof( velocityValues[ 0 ] ), stof( velocityValues[ 1 ] ), stof( velocityValues[ 2 ] ) );

			std::string orientation = mapDefSpawnInfoBlackboard.GetValue( "orientation", "0.0,0.0,0.0" );
			Strings orientationValues = SplitStringOnDelimiter( orientation, ',' );
			newSpawnInfo.m_orientation = EulerAngles( stof( orientationValues[ 0 ] ), stof( orientationValues[ 1 ] ), stof( orientationValues[ 2 ] ) );

			currentMapDef.m_spawnPoints.push_back( newSpawnInfo );
			spawnInfoElement = spawnInfoElement->NextSiblingElement();
		}

 		currentElement = currentElement->NextSiblingElement();
	}
}

//-----------------------------------------------------------------------------------------------
void MapDefinition::ClearMapDefs()
{
	s_mapDefs.clear();
}

//-----------------------------------------------------------------------------------------------
std::string MapDefinition::GetName() const
{
	return m_name;
}

//-----------------------------------------------------------------------------------------------
Image* MapDefinition::GetImage() const
{
	return m_image;
}

//-----------------------------------------------------------------------------------------------
Shader* MapDefinition::GetShader() const
{
	return m_shader;
}

//-----------------------------------------------------------------------------------------------
Texture* MapDefinition::GetSpriteSheetTexture() const
{
	return m_spriteSheetTexture;
}

//-----------------------------------------------------------------------------------------------
IntVec2 MapDefinition::GetSpriteSheetCellCount() const
{
	return m_spriteSheetCellCount;
}

//-----------------------------------------------------------------------------------------------
std::vector<SpawnInfo> MapDefinition::GetSpawnPoints() const
{
	return m_spawnPoints;
}

//-----------------------------------------------------------------------------------------------
MapDefinition* MapDefinition::GetMapDefFromName( std::string name )
{
	for( unsigned int mapDefIndex = 0; mapDefIndex < s_mapDefs.size(); mapDefIndex++ )
	{
		if( s_mapDefs[ mapDefIndex ].GetName() == name )
		{
			return &s_mapDefs[ mapDefIndex ];
		}
	}

	return nullptr;
}

