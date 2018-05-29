#include "Engine/Renderer/RendererEnums.hpp"

#include "ThirdParty/gl/glcorearb.h"


namespace
{

uint g_GLTextureTarget[TextureTarget::COUNT] =
{
    GL_TEXTURE_2D,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_CUBE_MAP
};

uint g_GLPrimitiveTypes[DrawPrimitive::COUNT] =
{
    GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
    GL_LINE_LOOP,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN
};

uint g_GLBlendOP[BlendOP::COUNT] =
{
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX
};

uint g_GLCompareMode[DepthCompareMode::COUNT] =
{
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

uint g_GLBlendFactor[BlendFactor::COUNT] =
{
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA
};

uint g_GLCullMode[CullMode::COUNT] =
{
    GL_BACK,
    GL_FRONT,
    GL_NONE
};

uint g_GLFillMode[FillMode::COUNT] =
{
    GL_FILL,
    GL_LINE
};

uint g_GLWindOrder[WindOrder::COUNT] =
{
    GL_CW,
    GL_CCW
};

uint g_GLRenderDataType[RenderDataType::COUNT] =
{
    GL_BYTE,
    GL_UNSIGNED_BYTE,
    GL_SHORT,
    GL_UNSIGNED_SHORT,
    GL_INT,
    GL_UNSIGNED_INT,
    GL_HALF_FLOAT,
    GL_FLOAT,
    GL_DOUBLE,
    GL_FIXED
};

}

uint ToGLEnum( TextureTarget target )
{
    return g_GLTextureTarget[target];
}

uint ToGLEnum( DrawPrimitive prim )
{
    return g_GLPrimitiveTypes[prim];
}

uint ToGLEnum( BlendOP op )
{
    return g_GLBlendOP[op];
}

uint ToGLEnum( BlendFactor factor )
{
    return g_GLBlendFactor[factor];
}

uint ToGLEnum( DepthCompareMode compare )
{
    return g_GLCompareMode[compare];
}

uint ToGLEnum( CullMode cull )
{
    return g_GLCullMode[cull];
}

uint ToGLEnum( FillMode fill )
{
    return g_GLFillMode[fill];
}

uint ToGLEnum( WindOrder order )
{
    return g_GLWindOrder[order];
}

uint ToGLEnum( RenderDataType type )
{
    return g_GLRenderDataType[type];
}

uchar ToGLEnum( bool b )
{
    return b ? GL_TRUE : GL_FALSE;
}



void BreakOutBlendMode( BlendMode blendMode, BlendOP& out_op, BlendFactor& out_src, BlendFactor& out_dst )
{
    switch( blendMode )
    {
    case BlendMode::ADDITIVE:
    {
        out_op = BlendOP::ADD;
        out_src = BlendFactor::SRC_ALPHA;
        out_dst = BlendFactor::ONE;
        return;
    }
    case BlendMode::ALPHA_BLEND:
    {
        out_op = BlendOP::ADD;
        out_src = BlendFactor::SRC_ALPHA;
        out_dst = BlendFactor::ONE_MINUS_SRC_ALPHA;
        return;
    }
    default:
        break;
    }
}

