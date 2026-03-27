#include "Game/Gameplay/MapDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Image.hpp"


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

		s_mapDefs[ elementIndex ].m_name = mapDefBlackboard.GetValue( "name", "" );
		std::string imagePath = mapDefBlackboard.GetValue( "image", "" );
		std::string shaderPath = mapDefBlackboard.GetValue( "shader", "" );
		std::string texturePath = mapDefBlackboard.GetValue( "spriteSheetTexture", "" );
		s_mapDefs[ elementIndex ].m_spriteSheetCellCount = mapDefBlackboard.GetValue( "spriteSheetCellCount", IntVec2() );

		if( !imagePath.empty() )
		{
			s_mapDefs[ elementIndex ].m_image = new Image( imagePath.c_str() );
		}
		
		if( !shaderPath.empty() )
		{
			s_mapDefs[ elementIndex ].m_shader = g_engine->m_render->CreateOrGetShader( shaderPath.c_str(), VertexType::VERTEX_PCUTBN );
		}

		if( !texturePath.empty() )
		{
			s_mapDefs[ elementIndex ].m_spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( texturePath.c_str() );
		}

		currentElement = currentElement->NextSiblingElement();
	}
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