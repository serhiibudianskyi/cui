#include "CUIWindow.h"

CUI::Window::Window(CUI::Window *parent, const std::string &title, CUI::Direction direction) : CUI::Widget(parent), title_(title)
{
    pad_ = newpad(1, 1);
    wbkgd(pad_, COLOR_PAIR(WIN_PAD_COLOR));

    border_ = parent ? derwin(parent->getPad(), 0, 0, 0, 0) : newwin(0, 0, 0, 0);
    wbkgd(border_, COLOR_PAIR(WIN_BORDER_COLOR));

    panel_ = new_panel(border_);

    keypad(pad_, true);

    setDirection(direction);
    setPadding({1, 1, 1, 1});
}

CUI::Window::~Window()
{
    for (const auto &widget : widgets_)
    {
        delete widget;
    }

    del_panel(panel_);
    delwin(border_);
    delwin(pad_);
}

int CUI::Window::run()
{
    int character = 0;
    bool exit = false;

    refresh();

    for (std::size_t i = 0; i < widgets_.size() && !exit;)
    {
        refresh();

        character = widgets_[i]->run();

        switch (character)
        {
        case KEY_PPAGE:
        case KEY_NPAGE:
        case KEY_LEFT:
        case KEY_RIGHT:
            if (scrolling(character, 1) || !getParent())
            {
                break;
            }
            else
            {
                exit = true;
            }
        case KEY_F(1):
        case 27:
            exit = true;
            break;
        case KEY_RESIZE:
            if (getParent())
            {
                exit = true;
            }
            offset_ = {0, 0};
            break;
        default:
            i++;
            break;
        }
    }

    return character;
}

void CUI::Window::refresh()
{
    update();

    clear();

    draw();

    doupdate();
}

void CUI::Window::update()
{
    resize();

    move();

    replace();

    for (auto &widget : widgets_)
    {
        widget->update();
    }
}

void CUI::Window::draw()
{
    drawBorder();
    drawTitle();
    drawWidgets();

    update_panels();

    drawPad();

    drawWidgets();
}

void CUI::Window::resize()
{
    for (const auto &widget : widgets_)
    {
        widget->resize();
    }

    // Re-calculate border size
    CUI::Size size = {0, 0};

    auto maxWidthPtr = std::max_element(widgets_.begin(), widgets_.end(), [](const auto &a, const auto &b)
                                        { return a->getSize().width_ + a->getPosition().x_ < b->getSize().width_ + b->getPosition().x_; });
    auto maxHeightPtr = std::max_element(widgets_.begin(), widgets_.end(), [](const auto &a, const auto &b)
                                         { return a->getSize().height_ + a->getPosition().y_ < b->getSize().height_ + b->getPosition().y_; });

    if (getParent())
    {
        size.width_ = widgets_.end() != maxWidthPtr ? (*maxWidthPtr)->getPosition().x_ + (*maxWidthPtr)->getSize().width_ + getPadding().left_ + getPadding().right_ : 0;
        size.height_ = widgets_.end() != maxHeightPtr ? (*maxHeightPtr)->getPosition().y_ + (*maxHeightPtr)->getSize().height_ + getPadding().top_ + getPadding().bottom_ : 0;

        size.width_ = std::min((int)size.width_, 30);
        size.height_ = std::min((int)size.height_, 8);
    }
    else
    {
        getmaxyx(stdscr, size.height_, size.width_);
    }

    setSize(size);
    wresize(border_, size.height_, size.width_);

    // Re-calculate pad size
    CUI::Size padSize = size;

    padSize.width_ = widgets_.end() != maxWidthPtr ? (*maxWidthPtr)->getPosition().x_ + (*maxWidthPtr)->getSize().width_ : padSize.width_;
    padSize.height_ = widgets_.end() != maxHeightPtr ? (*maxHeightPtr)->getPosition().y_ + (*maxHeightPtr)->getSize().height_ : padSize.height_;

    // setMaxSize(maxSize);
    padSize_ = padSize;
    wresize(pad_, padSize_.height_, padSize_.width_);
}

WINDOW *CUI::Window::getPad() const
{
    return pad_;
}

WINDOW *CUI::Window::getBorder() const
{
    return border_;
}

CUI::Position CUI::Window::getOffset() const
{
    return offset_;
}

void CUI::Window::add(Widget *widget)
{
    assert(widget);

    auto found = std::find_if(widgets_.begin(), widgets_.end(), [widget](const auto &item)
                              { return item == widget; });

    assert(widgets_.end() == found);

    widgets_.emplace_back(widget);

    replace();
}

void CUI::Window::move()
{
    CUI::Position position = getPosition();

    wmove(border_, position.y_, position.x_);

    if (getParent())
    {
        mvderwin(border_, position.y_, position.x_);
    }

    move_panel(panel_, position.y_, position.x_);
}

void CUI::Window::replace()
{
    CUI::Widget *previous = nullptr;
    CUI::Position position = {0, 0};

    for (const auto &widget : widgets_)
    {
        switch (getDirection())
        {
        case CUI::HORIZONTAL:
            if (previous)
            {
                position.x_ = previous->getPosition().x_ + previous->getSize().width_;
                break;
            }
        case CUI::VERTICAL:
        default:
            if (previous)
            {
                position.y_ = previous->getPosition().y_ + previous->getSize().height_;
            }
            break;
        }

        widget->setPosition(position);

        previous = widget;
    }
}

void CUI::Window::clear()
{
    wclear(pad_);
    wclear(border_);
}

void CUI::Window::drawBorder()
{
    box(border_, 0, 0);
    // wborder(border_, '|', '|', '-', '-', '+', '+', '+', '+');
}

void CUI::Window::drawTitle()
{
    CUI::Size size = getSize();

    wattron(border_, A_BOLD);
    mvwprintw(border_, 0, (size.width_ - title_.length()) / 2, "%s", title_.c_str());
    wattroff(border_, A_BOLD);
}

void CUI::Window::drawPad()
{
    int offsety = offset_.y_;
    int offsetx = offset_.x_;
    int pady = getAbsolutePositionWithScroll().y_ + getPadding().top_;
    int padx = getAbsolutePositionWithScroll().x_ + getPadding().left_;
    int padmaxy = getAbsolutePositionWithScroll().y_ + getSize().height_ - getPadding().top_ - getPadding().left_;
    int padmaxx = getAbsolutePositionWithScroll().x_ + getSize().width_ - getPadding().left_ - getPadding().right_;

    // mvwprintw(getBorder(), 0, 1, "%d %d", getVisibleSize().width_, getVisibleSize().height_);

    if (getParent())
    {
        CUI::Window *parent = dynamic_cast<CUI::Window *>(getParent());

        assert(parent);

        int parentminx = parent->getAbsolutePositionWithScroll().x_ + parent->getPadding().left_;

        mvwprintw(getBorder(), 0, 3, "%d %d", parent->getAbsolutePositionWithScroll().x_, parent->getAbsolutePositionWithScroll().y_);

        if (padx < parentminx)
        {
            offsetx += parentminx - padx;
            padx = parentminx;
        }

        if (padx < parent->padx_)
        {
            offsetx += parent->padx_ - padx;
            padx = parent->padx_;
        }

        int parentminy = parent->getAbsolutePositionWithScroll().y_ + parent->getPadding().top_;

        if (pady < parentminy)
        {
            offsety += parentminy - pady;
            pady = parentminy;
        }

        if (pady < parent->pady_)
        {
            offsety += parent->pady_ - pady;
            pady = parent->pady_;
        }

        int parentmaxx = parent->getAbsolutePositionWithScroll().x_ + parent->getSize().width_ - parent->getPadding().left_ - parent->getPadding().right_;

        if (padmaxx > parentmaxx)
        {
            padmaxx = parentmaxx;
        }

        if (padmaxx > parent->padmaxx_)
        {
            padmaxx = parent->padmaxx_;
        }

        int parentmaxy = parent->getAbsolutePositionWithScroll().y_ + parent->getSize().height_ - parent->getPadding().top_ - parent->getPadding().bottom_;

        if (padmaxy > parentmaxy)
        {
            padmaxy = parentmaxy;
        }

        if (padmaxy > parent->padmaxy_)
        {
            padmaxy = parent->padmaxy_;
        }

        // padmaxx = getVisibleSize().width_ - 1;

        // /* Pad position according to screen size */
        // CUI::Size screenSize = {0, 0};
        // getmaxyx(parent->getBorder(), screenSize.height_, screenSize.width_);

        // screenSize.width_ -= getPadding().left_ + getPadding().right_;
        // screenSize.height_ -= getPadding().top_ + getPadding().right_;

        // if (padmaxx > screenSize.width_)
        // {
        //     padmaxx -= padmaxx - screenSize.width_;
        // }

        // if (padmaxy > screenSize.height_)
        // {
        //     padmaxy -= padmaxy - screenSize.height_;
        // }
    }

    padmaxy_ = padmaxy;
    padmaxx_ = padmaxx;

    padx_ = padx;
    pady_ = pady;

    // Pad position according to screen size
    CUI::Size screenSize = {0, 0};
    getmaxyx(stdscr, screenSize.height_, screenSize.width_);

    screenSize.width_ -= getPadding().left_ + getPadding().right_;
    screenSize.height_ -= getPadding().top_ + getPadding().right_;

    if (padmaxx > screenSize.width_)
    {
        padmaxx -= padmaxx - screenSize.width_;
    }

    if (padmaxy > screenSize.height_)
    {
        padmaxy -= padmaxy - screenSize.height_;
    }

    // ││This is line 0┌29 7──────Sub

    pnoutrefresh(pad_, offsety, offsetx, pady, padx, padmaxy, padmaxx);
}

void CUI::Window::drawWidgets()
{
    for (const auto &widget : widgets_)
    {
        widget->draw();
    }
}

bool CUI::Window::scrolling(int to, unsigned short step)
{
    CUI::Position oldOffset = offset_;

    switch (to)
    {
    case KEY_PPAGE:
        if (offset_.y_ >= step)
        {
            offset_.y_ -= step;
        }
        else if (offset_.y_ > 0)
        {
            offset_.y_--;
        }
        break;
    case KEY_NPAGE:
        if (padSize_.height_ + getPadding().top_ + getPadding().bottom_ >= offset_.y_ + getSize().height_ + step)
        {
            offset_.y_ += step;
        }
        else if (padSize_.height_ + getPadding().top_ + getPadding().bottom_ >= offset_.y_ + getSize().height_ + 1)
        {
            offset_.y_++;
        }
        break;
    case KEY_RIGHT:
        if (padSize_.width_ + getPadding().left_ + getPadding().right_ >= offset_.x_ + getSize().width_ + step)
        {
            offset_.x_ += step;
        }
        else if (padSize_.width_ + getPadding().left_ + getPadding().right_ >= offset_.x_ + getSize().width_ + 1)
        {
            offset_.x_++;
        }
        break;
    case KEY_LEFT:
        if (offset_.x_ >= step)
        {
            offset_.x_ -= step;
        }
        if (offset_.x_ > 0)
        {
            offset_.x_--;
        }
        break;
    default:
        break;
    }

    return oldOffset != offset_;
}

CUI::Position CUI::Window::getAbsolutePositionWithScroll() const
{
    CUI::Position result = getPosition();

    if (getParent())
    {
        CUI::Window *parent = dynamic_cast<CUI::Window *>(getParent());

        assert(parent);

        result += parent->getAbsolutePositionWithScroll() + CUI::Position{parent->getPadding().left_, parent->getPadding().top_} - parent->getOffset();
    }

    return result;
}