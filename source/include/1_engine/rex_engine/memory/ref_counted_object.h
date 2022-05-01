#pragma once

namespace rex
{
    class IRefCountedObject
    {
    public:
        virtual ~IRefCountedObject()
        {
        }

        virtual std::uint64_t increment_reference() const = 0;
        virtual std::uint64_t decrement_reference() const = 0;

        virtual std::uint64_t get_reference_count() const = 0;
    };

    class RefCountedObject : public IRefCountedObject
    {
    public:
        RefCountedObject();
        ~RefCountedObject() override;

        std::uint64_t increment_reference() const override;
        std::uint64_t decrement_reference() const override;
        std::uint64_t get_reference_count() const override;

    private:
        mutable std::atomic<std::uint64_t> m_references;
    };
}