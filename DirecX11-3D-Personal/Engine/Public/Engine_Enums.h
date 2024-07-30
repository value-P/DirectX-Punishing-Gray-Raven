#pragma once

namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum class PropertyType {Shader, Texture, Terrain, NonAnimModel, AnimModel, Property_End};

	enum class TerrainTexType { TEX_DIFFUSE, TEX_MASK, TEX_END };
    enum class SpriteTexType { TEX_DIFFUSE, TEX_MASK, TEX_NOISE, TEX_END };
    enum class MeshType { NON_ANIM, ANIM, TYPE_END };

    enum class StopWatchState { Free, Checking, End};

    enum CHANNELID
    {
        CHANNEL_BGM,
        CHANNEL_PLAYER1,
        CHANNEL_PLAYER2,
        CHANNEL_PLAYER3,
        CHANNEL_PLAYER4,
        CHANNEL_PLAYER5,
        CHANNEL_MONSTER1,
        CHANNEL_MONSTER2,
        CHANNEL_MONSTER3,
        CHANNEL_MONSTER4,
        CHANNEL_MONSTER5,
        CHANNEL_MONSTER6,
        CHANNEL_MONSTER7,
        CHANNEL_EFFECT1,
        CHANNEL_EFFECT2,
        CHANNEL_EFFECT3,
        CHANNEL_EFFECT4,
        CHANNEL_EFFECT5,
        CHANNEL_END,
    };

    enum MaterialTexType {
        /** Dummy value.
         *
         *  No texture, but the value to be used as 'texture semantic'
         *  (#aiMaterialProperty::mSemantic) for all material properties
         *  *not* related to textures.
         */
        MaterialTexType_NONE = 0,

        /** LEGACY API MATERIALS
         * Legacy refers to materials which
         * Were originally implemented in the specifications around 2000.
         * These must never be removed, as most engines support them.
         */

         /** The texture is combined with the result of the diffuse
          *  lighting equation.
          *  OR
          *  PBR Specular/Glossiness
          */
        MaterialTexType_DIFFUSE = 1,

        /** The texture is combined with the result of the specular
         *  lighting equation.
         *  OR
         *  PBR Specular/Glossiness
         */
        MaterialTexType_SPECULAR = 2,

        /** The texture is combined with the result of the ambient
         *  lighting equation.
         */
        MaterialTexType_AMBIENT = 3,

        /** The texture is added to the result of the lighting
         *  calculation. It isn't influenced by incoming light.
         */
        MaterialTexType_EMISSIVE = 4,

        /** The texture is a height map.
         *
         *  By convention, higher gray-scale values stand for
         *  higher elevations from the base height.
         */
        MaterialTexType_HEIGHT = 5,

        /** The texture is a (tangent space) normal-map.
         *
         *  Again, there are several conventions for tangent-space
         *  normal maps. Assimp does (intentionally) not
         *  distinguish here.
         */
        MaterialTexType_NORMALS = 6,

        /** The texture defines the glossiness of the material.
         *
         *  The glossiness is in fact the exponent of the specular
         *  (phong) lighting equation. Usually there is a conversion
         *  function defined to map the linear color values in the
         *  texture to a suitable exponent. Have fun.
        */
        MaterialTexType_SHININESS = 7,

        /** The texture defines per-pixel opacity.
         *
         *  Usually 'white' means opaque and 'black' means
         *  'transparency'. Or quite the opposite. Have fun.
        */
        MaterialTexType_OPACITY = 8,

        /** Displacement texture
         *
         *  The exact purpose and format is application-dependent.
         *  Higher color values stand for higher vertex displacements.
        */
        MaterialTexType_DISPLACEMENT = 9,

        /** Lightmap texture (aka Ambient Occlusion)
         *
         *  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
         *  covered by this material property. The texture contains a
         *  scaling value for the final color value of a pixel. Its
         *  intensity is not affected by incoming light.
        */
        MaterialTexType_LIGHTMAP = 10,

        /** Reflection texture
         *
         * Contains the color of a perfect mirror reflection.
         * Rarely used, almost never for real-time applications.
        */
        MaterialTexType_REFLECTION = 11,

        /** PBR Materials
         * PBR definitions from maya and other modelling packages now use this standard.
         * This was originally introduced around 2012.
         * Support for this is in game engines like Godot, Unreal or Unity3D.
         * Modelling packages which use this are very common now.
         */

        MaterialTexType_BASE_COLOR = 12,
        MaterialTexType_NORMAL_CAMERA = 13,
        MaterialTexType_EMISSION_COLOR = 14,
        MaterialTexType_METALNESS = 15,
        MaterialTexType_DIFFUSE_ROUGHNESS = 16,
        MaterialTexType_AMBIENT_OCCLUSION = 17,

        /** PBR Material Modifiers
        * Some modern renderers have further PBR modifiers that may be overlaid
        * on top of the 'base' PBR materials for additional realism.
        * These use multiple texture maps, so only the base type is directly defined
        */

        /** Sheen
        * Generally used to simulate textiles that are covered in a layer of microfibers
        * eg velvet
        * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_sheen
        */
        MaterialTexType_SHEEN = 19,

        /** Clearcoat
        * Simulates a layer of 'polish' or 'lacquer' layered on top of a PBR substrate
        * https://autodesk.github.io/standard-surface/#closures/coating
        * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat
        */
        MaterialTexType_CLEARCOAT = 20,

        /** Transmission
        * Simulates transmission through the surface
        * May include further information such as wall thickness
        */
        MaterialTexType_TRANSMISSION = 21,

        /** Unknown texture
         *
         *  A texture reference that does not match any of the definitions
         *  above is considered to be 'unknown'. It is still imported,
         *  but is excluded from any further post-processing.
        */
        MaterialTexType_UNKNOWN = 18,

#ifndef SWIG
        _MaterialTexType_Force32Bit = INT_MAX
#endif
    };

}
