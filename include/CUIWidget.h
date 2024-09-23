#pragma once

#include <vector>
#include <cassert>
#include <panel.h>
#include <ncurses.h>
#include <algorithm>

#include "CUISize.h"
#include "CUIPadding.h"
#include "CUIPosition.h"
#include "CUIDirection.h"

namespace CUI
{
    class Widget
    {
    public:
        Widget(Widget *parent);
        virtual ~Widget() = default;

        virtual int run();
        virtual void refresh() = 0;
        virtual void update() = 0;
        virtual void draw() = 0;
        virtual void resize() = 0;

        std::uint32_t getId() const;

        Widget *getParent() const;

        void setSize(const Size &size);
        Size getSize() const;

        void setMinSize(const Size &size);
        Size getMinSize() const;

        void setMaxSize(const Size &size);
        Size getMaxSize() const;

        void setPadding(const Padding &padding);
        Padding getPadding() const;

        void setPosition(const Position &position);
        Position getPosition() const;
        Position getAbsolutePosition() const;

        void setDirection(const Direction &direction);
        Direction getDirection() const;

        void setActive(bool active);
        bool isActive() const;

    private:
        const std::uint32_t id_{0};

        Widget *parent_{nullptr};

        Size size_{0, 0};
        Size minSize_{0, 0};
        Size maxSize_{0, 0};

        Padding padding_{0, 0, 0, 0};
        Position position_{0, 0};
        Direction direction_{VERTICAL};

        bool active_{false};

        static uint32_t nextId_;
    };
};