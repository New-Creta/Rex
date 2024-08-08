#pragma once

#include "rex_std/vector.h"
#include "rex_std/unordered_map.h"

#include "rex_renderer_core/system/shader_type.h"
#include "rex_renderer_core/resources/resource.h"
#include "rex_renderer_core/system/shader_parameter_location.h"
#include "rex_renderer_core/system/shader_parameter.h"

namespace rex
{
    namespace gfx
    {
        class ConstantBuffer;
        class Texture2D;
        class Sampler2D;

        // Describes the information needed to construct a shader parameter store
        struct ShaderParametersStoreDesc
        {
            // This stores the location of a certain parameter inside the entire pipeline's parameter list
            rsl::unordered_map<rsl::string, ShaderParameterLocation> param_map;

            // This stores the number of views per range.
            // The number of ranges is implied by the size of this vector
            rsl::vector<ShaderParameterDesc> shader_resource_descs;
        };

        //
        //	+-----------------------------------------+-----------------------------------------+
        //  |										  |											|
        //  |		Vertex Shader Parameters		  |		Pixel Shader Parameters				|
        //  |										  | 										|
        //	+-----------------------------------------+-----------------------------------------+
        //    [0] constant buffer 1x (cb_a)						[2] textures 2x (texture_a, texture_b)
        //    [1] constant buffer 1x (cb_b)						[3] samplers 2x (sampler_a, sampler_b)
        //  
        //  
        //   STORED IN SHADER PARAMETER STORE AS FOLLOWED
        //  
        //	          +-------------------------+
        //            |                         |
        //      [0]   |	     VIEW TO CB_A		|
        //            |							|
        //	          +-------------------------+
        //            |							|
        //      [1]   |	     VIEW TO CB_B		|
        //            |							|
        //	          +-------------------------+-------------------------+
        //            |							|						  |
        //      [2]   |	  VIEW TO TEXTURE_A		|	VIEW TO TEXTURE_B	  |
        //            |							|						  |
        //	          +-------------------------+-------------------------+
        //            |							|						  |
        //      [3]   |   VIEW TO SAMPLER_A		|	VIEW TO SAMPLER_B	  |
        //            |							|						  |
        //	          +-------------------------+-------------------------+
        //    
        //                     [0]                        [1]
        //  
        //
        //     cb_a      -> slot=0, idx=0, view_offset={0,0}
        //     cb_b      -> slot=1, idx=1, view_offset={0,0}
        //     texture_a -> slot=2, idx=2, view_offset={0,0}
        //     texture_a -> slot=2, idx=2, view_offset={0,1}
        //     sampler_a -> slot=3, idx=3, view_offset={0,0}
        //     sampler_a -> slot=3, idx=3, view_offset={0,1}
        //
        //

        // A shader parameter store is an object that holds a list of shader parameters
        // Each of which could hold a list of view ranges of their own for shader resources
        // To simply lookup where a view of a certain shader resource is stored within the shader parameters
        // the store also has a reference to a map that can look up these locations by key
        // It doesn't own this map though, as all the views will always be at the same location
        // as long as different shader parameter stores use the same shader pipeline.
        // Therefore this map is stored in the cache instead and each shader parameter store
        // only holds a reference to this map to save on memory
        class ShaderParametersStore
        {
        public:
            ShaderParametersStore(const ShaderParametersStoreDesc& desc);

            // Set a resource to a new value
            void set(rsl::string_view name, ConstantBuffer* cb);
            void set(rsl::string_view name, Texture2D* texture);
            void set(rsl::string_view name, Sampler2D* sampler);

            // Return the location of a resource
            ShaderParameterLocation location(rsl::string_view name) const;

            // Return all parameters stored in the store
            const rsl::vector<rsl::unique_ptr<ShaderParameter>>& params() const;

        private:
            // To make parameters easily accessible to users, we have a map from name of parameter to their view idx
            // This view idx is the index within the parameter container that the param type belongs to
            const rsl::unordered_map<rsl::string, ShaderParameterLocation>* m_param_to_location_lookup;

            // Holds the list of resources that are directly tied to the shader
            rsl::vector<rsl::unique_ptr<ShaderParameter>> m_shader_parameters;
        };
    }
}