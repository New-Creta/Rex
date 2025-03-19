#include "memory/memory_manager.h"
#include "memory/memory_tracker.h"

#include "util/log.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        memory_region::memory_region(const memory_requirements& requirements)
            : m_heap(requirements.total_memory())
            , m_frame_heap(&m_heap, requirements.frame_heap_size)
            , m_tagged_heap(&m_heap, requirements.tagged_heap_block_size, requirements.tagged_heap_block_count)
            , m_scratch_heap(&m_heap, requirements.scratch_heap_size)
        {}

        //-------------------------------------------------------------------------
        heap* memory_region::get_heap()
        {
            return &m_heap;
        }

        //-------------------------------------------------------------------------
        linear_heap* memory_region::get_frame_heap()
        {
            return &m_frame_heap;
        }

        //-------------------------------------------------------------------------
        tagged_heap* memory_region::get_tagged_heap()
        {
            return &m_tagged_heap;
        }

        //-------------------------------------------------------------------------
        circular_heap* memory_region::get_scratch_heap()
        {
            return &m_scratch_heap;
        }

        //-------------------------------------------------------------------------
        void memory_region::free()
        {
            m_frame_heap.free();
            m_tagged_heap.free();
            m_scratch_heap.free();

            m_heap.free();
        }

        //-------------------------------------------------------------------------
        // DEVELOPMENT MEMORY MANAGER
        // Static function that returns the singleton instance
        imemory_manager& development_memory_manager::instance()
        {
            bool was_tracking = disable_tracking();

            static memory_requirements persistant;
            persistant.frame_heap_size = 1_mb;
            persistant.tagged_heap_block_size = 500_kb;
            persistant.tagged_heap_block_count = 10;
            persistant.scratch_heap_size = 1_mb;
            
            static memory_requirements staging;
            staging.frame_heap_size = 0_mb;
            staging.tagged_heap_block_size = 0_kb;
            staging.tagged_heap_block_count = 0;
            staging.scratch_heap_size = 20_mb;

            static memory_requirements debug;
            debug.frame_heap_size = 1_mb;
            debug.tagged_heap_block_size = 500_kb;
            debug.tagged_heap_block_count = 10;
            debug.scratch_heap_size = 1_mb;

            static development_memory_manager s_instance(persistant, staging, debug);

            if (was_tracking)
            {
                enable_tracking();
            }

            return s_instance;
        }

        //-------------------------------------------------------------------------
        development_memory_manager::development_memory_manager(const memory_requirements& persistant, const memory_requirements& staging, const memory_requirements& debug)
            : m_persistant_region(persistant)
            , m_transient_region(staging)
            , m_debug_region(debug)
        {}

        //-------------------------------------------------------------------------
        // SHIPPING MEMORY MANAGER
        // Static function that returns the singleton instance
        imemory_manager& shipping_memory_manager::instance()
        {
            bool was_tracking = disable_tracking();

            static memory_requirements persistant;
            persistant.frame_heap_size = 1_mb;
            persistant.tagged_heap_block_size = 500_kb;
            persistant.tagged_heap_block_count = 10;
            persistant.scratch_heap_size = 1_mb;

            static memory_requirements staging;
            staging.frame_heap_size = 0_mb;
            staging.tagged_heap_block_size = 0_kb;
            staging.tagged_heap_block_count = 0;
            staging.scratch_heap_size = 20_mb;

            static memory_requirements debug;
            debug.frame_heap_size = 0_mb;
            debug.tagged_heap_block_size = 0_kb;
            debug.tagged_heap_block_count = 0;
            debug.scratch_heap_size = 0_mb;

            static shipping_memory_manager s_instance(persistant, staging, debug);

            if (was_tracking)
            {
                enable_tracking();
            }

            return s_instance;
        }

        //-------------------------------------------------------------------------
        shipping_memory_manager::shipping_memory_manager(const memory_requirements& persistant, const memory_requirements& staging, const memory_requirements& debug)
            : m_persistant_region(persistant)
            , m_transient_region(staging)
            , m_debug_region(debug)
        {}

        //-------------------------------------------------------------------------
        // GLOBAL ACCESSORS
        imemory_manager& get_memory_manager()
        {
#ifdef _DEBUG
            return development_memory_manager::instance();
#else
            return shipping_memory_manager::instance();
#endif
        }

        //-------------------------------------------------------------------------
        void print_memory_region(memory_region& region, std::string_view region_name)
        {
            log::info("=== region | {} ===", region_name);

            // Base heap information.
            auto base_heap = region.get_heap();
            log::info("Base Heap: {} / {} bytes", base_heap->current_memory().size_in_bytes(), base_heap->total_memory().size_in_bytes());

            // Frame (linear) heap.
            auto frame_heap = region.get_frame_heap();
            log::info("Frame Heap: {} / {} bytes", frame_heap->current_memory().size_in_bytes(), frame_heap->total_memory().size_in_bytes());

            // Tagged heap: iterate over each block.
            auto tagged_heap = region.get_tagged_heap();
            log::info("Tagged Heap ({} blocks):", tagged_heap->block_count());
            for (int i = 0; i < tagged_heap->block_count(); ++i)
            {
                log::info("  Block {} (Tag: {}): {} / {} bytes",
                    i,
                    tagged_heap->block_tag(i),
                    tagged_heap->block_current_size(i).size_in_bytes(),
                    tagged_heap->block_total_size().size_in_bytes());
            }

            // Scratch (circular) heap.
            auto scratch_heap = region.get_scratch_heap();
            log::info("Scratch Heap: {} / {} bytes", scratch_heap->current_memory().size_in_bytes(), scratch_heap->total_memory().size_in_bytes());

            log::info("==========================");

        }
        //-------------------------------------------------------------------------
        void print_memory_manager(imemory_manager& manager)
        {
            print_memory_region(manager.get_persistent_region(), "persistant");
            print_memory_region(manager.get_transient_region(), "staging");
            print_memory_region(manager.get_debug_region(), "debug");
        }
    }
}