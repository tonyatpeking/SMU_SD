#include "Engine/Math/Trajectory.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"


Vec2 Trajectory::Evaluate( const Vec2& launchVelocity, float time, float gravity )
{
    float xPos = launchVelocity.x * time;
    float yPos = -0.5f * gravity * time * time + launchVelocity.y * time;
    return Vec2( xPos, yPos );
}

Vec2 Trajectory::Evaluate( float launchSpeed, float launchAngleDeg, float time, float gravity )
{
    float vX = launchSpeed * CosDeg( launchAngleDeg );
    float vY = launchSpeed * SinDeg( launchAngleDeg );
    return Evaluate( Vec2( vX, vY ), time, gravity );
}

float Trajectory::GetMinimumLaunchSpeed( float distance, float gravity )
{
    return sqrtf( gravity * distance );
}

bool Trajectory::GetLaunchAngles( float launchSpeed, float distance,
                                  float& outAngle1, float& outAngle2,
                                  float height, float gravity )
{
    float vSqared = launchSpeed * launchSpeed;
    float a = -0.5f * gravity * distance * distance;
    float b = vSqared * distance;
    float c = a - vSqared * height;
    float out_tan1;
    float out_tan2;
    bool canSolve = Solver::Quadratic( a, b, c, out_tan1, out_tan2 );
    if( !canSolve )
        return false;
    float angle1 = atanf( out_tan1 );
    float angle2 = atanf( out_tan2 );
    if( angle1 < angle2 )
    {
        outAngle1 = angle1;
        outAngle2 = angle2;
    }
    else
    {
        outAngle1 = angle2;
        outAngle2 = angle1;
    }
    return true;
}

float Trajectory::GetMaxHeight( float launchSpeed, float distance, float height, float gravity )
{
    float angleMin;
    float angleMax;
    GetLaunchAngles( launchSpeed, distance, angleMin, angleMax, height, gravity );
    float vy = launchSpeed * SinDeg( angleMin );
    float t = vy / gravity;
    return -0.5f * gravity * t * t + vy * t;
}

Vec2 Trajectory::GetLaunchVelocity( float apexHeight, float distance, float height, float gravity )
{
    float vy = sqrtf( 2.f * gravity * apexHeight );
    //solve for t
    // -0.5 * g * t * t + vy * t - h = 0
    float a = -0.5f * gravity;
    float b = vy;
    float c = -height;
    float tMin;
    float tMax;
    Solver::Quadratic( a, b, c, tMin, tMax );
    // we want tMax because tMin will shoot through the target before reaching the apex
    float vx = distance / tMax;
    return Vec2( vx, vy );
}


