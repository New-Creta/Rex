#pragma once

namespace rex
{
    struct FrameInfo;

    class Layer
    {
    public:
        using StackOrder = int32;

        Layer(const std::string& name, StackOrder order = -1);
        virtual ~Layer();

        void enable();
        void disable();

        void attach();
        void detach();

        void update(const FrameInfo& info);
        void handle_event();

        const std::string& get_name() const;
        const StackOrder get_order() const;

        bool is_enabled() const;

    protected:
        virtual void on_attach(){/* Nothing to implement */};
        virtual void on_detach(){/* Nothing to implement */};

        virtual void on_update(const FrameInfo& /* info */);
        virtual void on_event();

    private:
        std::string m_name;
        StackOrder m_order;

        bool m_enabled;
    };
}