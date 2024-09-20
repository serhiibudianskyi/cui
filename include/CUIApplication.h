#pragma once

#include "CUIWindow.h"

namespace CUI
{
    class Application
    {
    public:
        explicit Application();
        virtual ~Application();

        int run();

        void add(Window *window);

        Application(Application &&) = delete;
        Application(const Application &) = delete;
        Application &operator=(Application &&) = delete;
        Application &operator=(const Application &) = delete;

    private:
        std::vector<Window *> windows_;
    };
};