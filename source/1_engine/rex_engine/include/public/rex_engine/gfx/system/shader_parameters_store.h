#pragma once

#include "rex_std/vector.h"
#include "rex_std/unordered_map.h"

#include "rex_engine/gfx/system/shader_type.h"
#include "rex_engine/gfx/resources/resource.h"
#include "rex_engine/gfx/system/shader_parameter_location.h"
#include "rex_engine/gfx/system/shader_parameter.h"

namespace rex
{
    namespace gfx
    {
        //
        //	+-----------------------------------------+-----------------------------------------+
        //  |										                      |           									        		|
        //  |		Vertex Shader Parameters		          |		Pixel Shader Parameters		        		|
        //  |										                      | 					            			        		|
        //	+-----------------------------------------+-----------------------------------------+
        //    [0] constant buffer 1x (cb_a)						[2] textures 2x (texture_a, texture_b)
        //    [1] constant buffer 1x (cb_b)						[3] samplers 2x (sampler_a, sampler_b)
        //  
        //  
        //   STORED IN SHADER PARAMETER STORE AS FOLLOWED
        //  
        //	          +-------------------------+
        //            |                         |
        //      [0]   |	     VIEW TO CB_A		    |
        //            |							            |
        //	          +-------------------------+
        //            |							            |
        //      [1]   |	     VIEW TO CB_B		    |
        //            |							            |
        //	          +-------------------------+-------------------------+
        //            |							            |						              |
        //      [2]   |	  VIEW TO TEXTURE_A		  |	VIEW TO TEXTURE_B	      |
        //            |							            |						              |
        //	          +-------------------------+-------------------------+
        //            |           							|						              |
        //      [3]   |   VIEW TO SAMPLER_A		  |	VIEW TO SAMPLER_B	      |
        //            |						            	|           						  |
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
        // A shader parameter can be of 2 types
        // 1. view
        // A view is a single pointer like structure that points to a resource on the GPU
        // 
        // 2. view tables
        // A view table holds set of view ranges, each of which can hold 1 or more views
        // Views in a set of view ranges all need to be of the same type (eg. all need to be a shader resource view)
        // They also need to be laid out continuously in memory
        // This means that the following needs to be true
        // Let's say we need to bind 5 textures, with the first texture being at address 0x0000 (start of the view heap)
        // If a SRV view is 128 bytes in memory, then the textures need to have the following layout in the view heap
        // texture 0 - 0x0000 == 000
        // texture 1 - 0x0080 == 128
        // texture 2 - 0x0100 == 256
        // texture 3 - 0x0180 == 384
        // texture 4 - 0x0200 == 512
        //
        // This is where the shader parameters member comes in
        // On construction view ranges are deserialized and laid out back to back
        // When they get bound, they are copied into a shader accessible view heap
        // As they're already laid out back to back in the shader parameters member,
        // they are also copied into this shader accessible view heap.
        // This makes it easier to bind them to the shader as we don't have to perform
        // any additional copying to lay them out correctly
        //
        // Using the same example as above
        //
        //
        //	                  +-------------------------+
        //                    |                         |
        //      [0]           |	     VIEW TO CB_A		    |   Single view, so no need to copy over view as the resource is bound directly
        //                    |							            |
        //	                  +-------------------------+
        //                    |						            	|
        //      [1]           |	     VIEW TO CB_B	    	|   Single view, so no need to copy over view as the resource is bound directly
        //                    |						            	|
        //	                  +-------------------------+-------------------------+
        //                    |				            			|						              |
        //      [2]  +--------|	  VIEW TO TEXTURE_A		  |	VIEW TO TEXTURE_B	      |------+
        //           |        |							            |						              |      |
        //	         |        +-------------------------+-------------------------+      |
        //           |        |							            |						              |      |
        //      [3]  |   +----|   VIEW TO SAMPLER_A		  |	VIEW TO SAMPLER_B	      |------+----+
        //           |   |    |							            |						              |      |    |
        //	         |   |    +-------------------------+-------------------------+      |    |
        //  +--------+---+                                                               |    |
        //  |        |                 [0]                        [1]                    |    |
        //  |        |              +----------------------------------------------------+    |
        //  |        |              |                                                         |
        //  |        v              v                                                         |
        //  |  +---------------------------------------------------------------------+        |
        //  |  |             |             |             |             |             |        |
        //  |  |    SRV 1    |    SRV 2    |    SRV 3    |    SRV 4    |   SRV 5     |        |
        //  |  |             |             |             |             |             |        |
        //  |  +---------------------------------------------------------------------+        |
        //  |      ^^^^     Shader accessible view heap for shader resources  ^^^^            |
        //  +--------+               +--------------------------------------------------------+
        //  |        |               |
        //  |        v               v
        //  |  +---------------------------------------------------------------------+
        //  |  |             |             |             |             |             |
        //  |  |  Sampler 1  |  Sampler 2  |  Sampler 3  |  Sampler 4  |  Sampler 5  |
        //  |  |             |             |             |             |             |
        //  |  +---------------------------------------------------------------------+
        //  |      ^^^^     Shader accessible view heap for samplers  ^^^^
        //
        //

        class ConstantBuffer;
        class UnorderedAccessBuffer;
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

        // A shader parameter store is an object that holds a list of shader parameters
        // Each of which could hold a list of view ranges of their own for shader resources
        // To simply lookup where a view of a certain shader resource is stored within the shader parameters
        // the store also has a reference to a map that can look up these locations by key
        // It doesn't own this map though, as all the views will always be at the same location
        // if the same shader pipeline is used.
        // Therefore this map is stored in the cache instead and each shader parameter store
        // only holds a reference to this map to save on memory
        class ShaderParametersStore
        {
        public:
            ShaderParametersStore(const ShaderParametersStoreDesc& desc);

            // Set a resource to a new value
            void set(rsl::string_view name, const ConstantBuffer* cb);
            void set(rsl::string_view name, const UnorderedAccessBuffer* cb);
            void set(rsl::string_view name, const Texture2D* texture);
            void set(rsl::string_view name, const Sampler2D* sampler);

            // Return the location of a resource
            ShaderParameterLocation location(rsl::string_view name) const;

            // Return all parameters stored in the store
            const rsl::vector<rsl::unique_ptr<ShaderParameter>>& params() const;

            // Copy all parameters from another store into this store.
            // Only parameters that are named in this store are copied from the other
            void copy_params_from(const ShaderParametersStore* other);

        private:
            // To make parameters easily accessible to users, we have a map from name of parameter to their view idx
            // This view idx is the index within the parameter container that the param type belongs to
            const rsl::unordered_map<rsl::string, ShaderParameterLocation>* m_param_to_location_lookup;

            // Holds the list of resources that are directly tied to the shader
            rsl::vector<rsl::unique_ptr<ShaderParameter>> m_shader_parameters;
        };
    }
}