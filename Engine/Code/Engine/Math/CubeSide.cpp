#include "Engine/Math/CubeSide.hpp"
#include "Engine/Math/IVec3.hpp"


IVec3 CubeSideToNormal( CubeSide side )
{
    switch( side )
    {
    case CubeSide::RIGHT:
        return IVec3::RIGHT;
    case CubeSide::LEFT:
        return IVec3::LEFT;
    case CubeSide::TOP:
        return IVec3::UP;
    case CubeSide::BOTTOM:
        return IVec3::DOWN;
    case CubeSide::BACK:
        return IVec3::FORWARD;
    case CubeSide::FRONT:
        return IVec3::BACKWARD;
    default:
        return IVec3::ZEROS;
    }
}

CubeSide NormalToCubeSide( const IVec3& normal )
{
    if( normal == IVec3::RIGHT )
        return CubeSide::RIGHT;
    if( normal == IVec3::LEFT )
        return CubeSide::LEFT;
    if( normal == IVec3::UP )
        return CubeSide::TOP;
    if( normal == IVec3::DOWN )
        return CubeSide::BOTTOM;
    if( normal == IVec3::FORWARD )
        return CubeSide::BACK;
    if( normal == IVec3::BACKWARD )
        return CubeSide::FRONT;
    return CubeSide::INVALID;
}
