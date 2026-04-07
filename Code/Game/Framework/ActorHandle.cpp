#include "Game/Framework/ActorHandle.hpp"


//-----------------------------------------------------------------------------------------------
const ActorHandle ActorHandle::INVALID = ActorHandle( 0x0000ffffu, 0x0000ffffu );

//-----------------------------------------------------------------------------------------------
ActorHandle::ActorHandle()
{
	m_data = 0xffffffffu;
}

//-----------------------------------------------------------------------------------------------
ActorHandle::ActorHandle( unsigned int uid, unsigned int index )
{
	m_data = ( uid << 16 ) | ( index & 0x0000ffffu );
}

//-----------------------------------------------------------------------------------------------
bool ActorHandle::IsValid() const
{
	if( m_data != 0xffffffff )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
unsigned int ActorHandle::GetIndex() const
{
	return ( m_data & 0x0000ffffu );
}

//-----------------------------------------------------------------------------------------------
bool ActorHandle::operator!=( ActorHandle const& other ) const
{
	return ( m_data != other.m_data );
}

//-----------------------------------------------------------------------------------------------
bool ActorHandle::operator==( ActorHandle const& other ) const
{
	return ( m_data == other.m_data );
}