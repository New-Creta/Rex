#pragma once

namespace rex
{
    namespace graphics
    {
        class Shader;
        struct MaterialID;

        class MaterialInterface
        {
        public:
            virtual ~MaterialInterface() = default;

            virtual void setData(const void* data, uint32 size) = 0;
            template <typename T>
            void setData(const T& data);

            virtual const MaterialID getId() const = 0;
            virtual const Shader* getShader() const = 0;
            virtual const MaterialInterface* getMaterial() const = 0;

            virtual void use() const = 0;

            virtual bool operator==(const MaterialInterface& rhs) const = 0;
            virtual bool operator!=(const MaterialInterface& rhs) const = 0;

        protected:
            MaterialInterface() = default;
        };

        //-------------------------------------------------------------------------
        template <typename T>
        void rex::graphics::MaterialInterface::setData(const T& data)
        {
            setData(&data, sizeof(data));
        }
    }
}