#pragma once

class Vec2;

namespace Trajectory
{
Vec2 Evaluate( const Vec2& launchVelocity, float time, float gravity = 9.8f );

Vec2 Evaluate( float launchSpeed, float launchAngleDeg, float time, float gravity = 9.8f );

float GetMinimumLaunchSpeed( float distance, float gravity = 9.8f );

//Given a fixed launch speed, determine the angle needed to hit a target distance away, at a given height.
//This can be at most two angles.
//Distance must be positive, height can be negative.

bool GetLaunchAngles( float launchSpeed,
                      float distance,
                      float& outAngle1,
                      float& outAngle2,
                      float height = 0.0f,
                      float gravity = 9.8f );

// Given a launch speed, and a target to reach,
// return the max possible height we could reach by just adjusting the angle.
float GetMaxHeight( float launchSpeed, float distance, float height, float gravity = 9.8f );

// Given a target apex height and a target( distance and height ) to hit,
// determine a launch velocity.
// Assume the apex height is positive and greater than the target height.
Vec2 GetLaunchVelocity( float apexHeight,        // must be greater than height
                           float distanceX,          // target distance
                           float height,            // target height
                           float gravity = 9.8f );

}