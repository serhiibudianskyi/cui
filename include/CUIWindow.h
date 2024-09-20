#pragma once

#include "CUIWidget.h"

#define WIN_BORDER_COLOR 1
#define WIN_PAD_COLOR 2

namespace CUI
{
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

        WINDOW *getPad() const;
        WINDOW *getBorder() const;

        Position getOffset() const;

        void add(Widget *);

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

        std::vector<Widget *> widgets_;

        int padmaxy_;
        int padmaxx_;
        int padx_;
        int pady_;

        Size padSize_{0, 0};
        Position savePadPosition_{0, 0};
        Position savePadMaxPosition_{0, 0};
    };
};