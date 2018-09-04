#include "Engine/Renderer/SpriteAnimType.hpp"
#include "Engine/FileIO/XmlUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Core/ErrorUtils.hpp"


SpriteAnimType SpriteAnimTypeWithDirection( SpriteAnimType baseAnim, const IVec2& direction )
{
    int directionEnumOffset = 0;
    if( direction == IVec2::NORTH )        directionEnumOffset = 1;
    if( direction == IVec2::WEST )         directionEnumOffset = 2;
    if( direction == IVec2::SOUTH )        directionEnumOffset = 3;
    if( direction == IVec2::EAST )         directionEnumOffset = 4;

    if( directionEnumOffset == 0 )
        ERROR_RECOVERABLE( "Invalid direction" );

    int enumVal = (int) baseAnim + directionEnumOffset;
    return SpriteAnimType(enumVal);
}

const SpriteAnimType ParseXmlAttribute( const XMLElement& element, const char* attributeName, const SpriteAnimType defaultValue )
{
    String strValue = ParseXmlAttribute( element, attributeName, "" );
    SpriteAnimType animType = SpriteAnimType::FromString( strValue );

    if( animType == SpriteAnimType::INVALID )
        return defaultValue;

    return animType;
}
