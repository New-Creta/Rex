#pragma once

#include "rex_graphics_global.h"

namespace rex
{
    namespace gl
    {
        struct DriverData
        {
            DriverData(void* deviceContext)
                :device_context(deviceContext)
            {}

            void* device_context;
        };
        struct GLContextData
        {
            GLContextData()
                : HAS_WGL_ARB_context_flush_control(false)
                , HAS_WGL_ARB_create_context_robustness(false)
                , HAS_WGL_ARB_create_context_no_error(false)
            {}
            GLContextData(bool contextFlushControl, bool contextRobustness, bool contextNoError)
                : HAS_WGL_ARB_context_flush_control(contextFlushControl)
                , HAS_WGL_ARB_create_context_robustness(contextRobustness)
                , HAS_WGL_ARB_create_context_no_error(contextNoError)
            {}

            bool HAS_WGL_ARB_context_flush_control;
            bool HAS_WGL_ARB_create_context_robustness;
            bool HAS_WGL_ARB_create_context_no_error;
        };
        struct GLContextConfiguration
        {
            GLContextConfiguration()
                :profile_mask(0)
                , flags(0)
                , share_with_current_context(false)
                , force_legacy_opengl(false)
            {}
            GLContextConfiguration(int contextProfileMask, int contextFlags, bool share, bool useLegactOpenGL)
                :profile_mask(contextProfileMask)
                , flags(contextFlags)
                , share_with_current_context(share)
                , force_legacy_opengl(useLegactOpenGL)
            {}

            int profile_mask;
            int flags;

            bool share_with_current_context;
            bool force_legacy_opengl;
        };

        class Context
        {
        public:
            REX_GRAPHICS_EXPORT Context(const DriverData& driverData, const GLContextData& glData = GLContextData(), const GLContextConfiguration& glConfig = GLContextConfiguration());
            REX_GRAPHICS_EXPORT virtual ~Context();

            REX_GRAPHICS_EXPORT void initialize();

            REX_GRAPHICS_EXPORT std::string version() const;

        protected:
            virtual void* createLegacyContext() = 0;
            virtual void* createModernContext() = 0;

            virtual void makeCurrent(void* context) = 0;
            virtual void deleteContext(void* context) = 0;

            REX_GRAPHICS_EXPORT const DriverData& driverData() const;

            REX_GRAPHICS_EXPORT const GLContextData& glContextData() const;
            REX_GRAPHICS_EXPORT const GLContextConfiguration& glContextConfiguration() const;

        private:
            void create(void** context);
            void setup(void** context);

            bool initalizeGLEW();

            DriverData m_driver_data;

            GLContextData m_gl_data;
            GLContextConfiguration m_gl_config;
        };
    }
}