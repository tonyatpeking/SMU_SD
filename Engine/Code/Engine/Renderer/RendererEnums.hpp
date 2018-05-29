#pragma once
#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Core/Types.hpp"

// if you change this don't forget to change shader or inject defines
constexpr uint MAX_LIGHTS = 8;

SMART_ENUM(
    TextureTarget,

    TEXTURE_2D,
    CUBE_MAP_POSITIVE_X,
    CUBE_MAP_NEGATIVE_X,
    CUBE_MAP_POSITIVE_Y,
    CUBE_MAP_NEGATIVE_Y,
    CUBE_MAP_POSITIVE_Z,
    CUBE_MAP_NEGATIVE_Z,
    TEXTURE_1D,
    CUBE_MAP
)

uint ToGLEnum( TextureTarget target );

SMART_ENUM(
    DrawPrimitive,

    POINTS = 0,
    LINES,
    LINE_STRIP,
    LINE_LOOP,
    TRIANGLES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN
)

uint ToGLEnum( DrawPrimitive prim );

SMART_ENUM(
    Alignment,

    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
)

SMART_ENUM(
    BlendOP,

    ADD,
    SUB,
    REV_SUB,
    MIN,
    MAX
)

uint ToGLEnum( BlendOP op );

SMART_ENUM(
    BlendFactor,

    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA
)

uint ToGLEnum( BlendFactor factor );

SMART_ENUM(
    BlendMode,
    CUSTOM,
    ADDITIVE,
    ALPHA_BLEND
)

void BreakOutBlendMode( BlendMode blendMode,
                        BlendOP& out_op, BlendFactor& out_src, BlendFactor& out_dst );

SMART_ENUM(
    DepthCompareMode,

    NEVER = 0,
    LESS,
    EQUAL,
    LEQUAL,
    GREATER,
    NOT_EQUAL,
    GEQUAL,
    ALWAYS
)

uint ToGLEnum( DepthCompareMode compare );

SMART_ENUM(
    TextDrawMode,

    SHRINK_FIT,
    WORD_WRAP,
    OVERRUN
)

SMART_ENUM(
    CullMode,

    BACK,
    FRONT,
    NONE
)

uint ToGLEnum( CullMode cull );

SMART_ENUM(
    FillMode,

    SOLID,
    WIRE
)

uint ToGLEnum( FillMode fill );

SMART_ENUM(
    WindOrder,

    CLOCKWISE,
    COUNTER_CLOCKWISE
)

uint ToGLEnum( WindOrder order );

SMART_ENUM(
    RenderDataType,

    BYTE,
    UNSIGNED_BYTE,
    SHORT,
    UNSIGNED_SHORT,
    INT,
    UNSIGNED_INT,
    HALF_FLOAT,
    FLOAT,
    DOUBLE,
    FIXED
)

SMART_ENUM(
    RenderDebugMode,

    NORMAL,
    WIREFRAME,
    LIGHTING_INFO,
    CUSTOM
)

uint ToGLEnum( RenderDataType type );
uchar ToGLEnum( bool b );

