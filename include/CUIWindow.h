#pragma once

#include "CUIWidget.h"

namespace CUI
{
    constexpr const std::size_t WIN_BORDER_COLOR = 1;
    constexpr const std::size_t WIN_PAD_COLOR = 2;
    constexpr const std::size_t WIN_CURSOR_COLOR = 3;

    constexpr const std::size_t KEY_ESC = 27;
    constexpr const std::size_t KEY_LPAGE = 262;
    constexpr const std::size_t KEY_RPAGE = 360;

    class Window : public Widget
    {
    public:
        Window(Window *parent, const std::string &title, Direction direction);
        virtual ~Window() override;

        virtual int run() override;
        virtual void refresh() override;
        virtual void update() override;
        virtual void draw() override;
        virtual void resize() override;

        void add(Widget *);

        WINDOW *getPad() const;
        WINDOW *getBorder() const;

    protected:
        void move();
        void replace();
        void clear();
        void drawBorder();
        void drawTitle();
        void drawWidgets();
        void drawPad();
        
        bool scrolling(int to, unsigned short step);

        Position getAbsolutePositionWithScroll() const;

    private:
        std::string title_;

        WINDOW *border_{nullptr};
        WINDOW *pad_{nullptr};
        PANEL *panel_{nullptr};

        Position offset_{0, 0};

        Size padSize_{0, 0};
        Position padTopLeftPosition_{0, 0};
        Position padBottomRightPosition_{0, 0};

        std::vector<Widget *> widgets_;
    };
};