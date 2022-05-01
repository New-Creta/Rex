#include "rex_std_pch.h"

#include "memory/ref_ptr_utilities.h"
#include "memory/ref_counted_object.h"

namespace rex
{
    namespace memory
    {
        std::mutex g_live_references_mutex;
        RefCountedObjectsContainer g_live_reference_pool;

        //-------------------------------------------------------------------------
        void add_to_reference_pool(const rex::IRefCountedObject* instance)
        {
            std::unique_lock<std::mutex> lock(g_live_references_mutex);

            R_ASSERT_X(instance, "Referencing a nullptr");

            g_live_reference_pool.insert(instance);
        }
        //-------------------------------------------------------------------------
        void remove_from_reference_pool(const rex::IRefCountedObject* instance)
        {
            std::unique_lock<std::mutex> lock(g_live_references_mutex);

            R_ASSERT(is_alive(instance));

            g_live_reference_pool.erase(instance);
        }

        //-------------------------------------------------------------------------
        const RefCountedObjectsContainer& get_all_references()
        {
            return g_live_reference_pool;
        }
        //-------------------------------------------------------------------------
        bool is_alive(const rex::IRefCountedObject* instance)
        {
            R_ASSERT_X(instance, "Looking for a nullptr ... ");

            return g_live_reference_pool.find(instance) != g_live_reference_pool.end();
        }
    }
}