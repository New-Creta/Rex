#pragma once

#include "rex_renderer_core/commands/create_buffer_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      class CreateIndexBuffer : public CreateBuffer
      {
      public:
        CreateIndexBuffer(CreateBufferCommandDesc&& desc)
            :CreateBuffer(rsl::move(desc))
        {}

        bool execute() override
        {
          result = backend::create_index_buffer(cmd.create_buffer_params, cmd.resource_slot);
          memory_free(cmd.create_buffer_params.data);

          return result;
        }
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex