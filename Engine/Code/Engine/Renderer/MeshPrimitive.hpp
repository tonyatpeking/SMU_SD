#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include <vector>

class Mesh;
class MeshBuilder;

namespace MeshPrimitive
{
MeshBuilder MakeCube(
    const Vec3& sideLengths = Vec3::ONES,
    const Rgba& tint = Rgba::WHITE,
    const Vec3& centerPos = Vec3::ZEROS,
    const AABB2& uvTop = AABB2::ZEROS_ONES,
    const AABB2& uvSide = AABB2::ZEROS_ONES,
    const AABB2& uvBottom = AABB2::ZEROS_ONES );

MeshBuilder MakeQuad(
    const AABB2& bounds = AABB2::NEG_ONES_ONES,
    const Rgba& tint = Rgba::WHITE,
    const AABB2& uvCoords = AABB2::ZEROS_ONES );

MeshBuilder MakeUVSphere(
    float radius = 0.5f,
    int stacks = 10,
    int slices = 20,
    const Rgba& tint = Rgba::WHITE,
    const Vec3 centerPos = Vec3::ZEROS );

MeshBuilder MakeLineStrip(
    const std::vector<Vec3>& points,
    const Rgba& startColor, const Rgba& endColor );

};
