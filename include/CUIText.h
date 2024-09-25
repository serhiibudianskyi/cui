#pragma once

#include "CUIWindow.h"

namespace CUI
{
    class Text : public Widget
    {
    public:
        Text(Window *parent, const std::string &text);

        virtual int run() override;
        virtual void refresh() override;
        virtual void update() override;
        virtual void draw() override;
        virtual void resize() override;

    private:
        Window *parent_;

        std::string text_;
    };
};