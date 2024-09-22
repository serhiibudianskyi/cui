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

        if (getMaxSize())
        {
            size.width_ = std::min((int)size.width_, getMaxSize().width_);
            size.height_ = std::min((int)size.height_, getMaxSize().height_);
        }
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
    // box(border_, 0, 0);

    char topBorder = getPadding().top_ ? '-' : ' ';
    char rigthBorder = getPadding().right_ ? '|' : ' ';
    char bottomBorder = getPadding().bottom_ ? '-' : ' ';
    char leftBorder = getPadding().left_ ? '|' : ' ';
    
    wborder(border_, leftBorder, rigthBorder, topBorder, bottomBorder, '+', '+', '+', '+');
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
    CUI::Position offsetPosition = offset_;
    CUI::Position padTopLeftPosition = getAbsolutePositionWithScroll() + CUI::Position{getPadding().left_, getPadding().top_};
    CUI::Position padBottomRightPosition = getAbsolutePositionWithScroll() + CUI::Position{getSize().width_, getSize().height_} - CUI::Position{getPadding().left_ + getPadding().right_, getPadding().top_ + getPadding().bottom_};

    if (getParent())
    {
        CUI::Window *parent = dynamic_cast<CUI::Window *>(getParent());

        assert(parent);

        CUI::Position parentTopLeftPosition = parent->getAbsolutePositionWithScroll() + CUI::Position{parent->getPadding().left_, parent->getPadding().top_};

        if (padTopLeftPosition.x_ < parentTopLeftPosition.x_)
        {
            offsetPosition.x_ += parentTopLeftPosition.x_ - padTopLeftPosition.x_;
            padTopLeftPosition.x_ = parentTopLeftPosition.x_;
        }

        if (padTopLeftPosition.x_ < parent->padTopLeftPosition_.x_)
        {
            offsetPosition.x_ += parent->padTopLeftPosition_.x_ - padTopLeftPosition.x_;
            padTopLeftPosition.x_ = parent->padTopLeftPosition_.x_;
        }

        if (padTopLeftPosition.y_ < parentTopLeftPosition.y_)
        {
            offsetPosition.y_ += parentTopLeftPosition.y_ - padTopLeftPosition.y_;
            padTopLeftPosition.y_ = parentTopLeftPosition.y_;
        }

        if (padTopLeftPosition.y_ < parent->padTopLeftPosition_.y_)
        {
            offsetPosition.y_ += parent->padTopLeftPosition_.y_ - padTopLeftPosition.y_;
            padTopLeftPosition.y_ = parent->padTopLeftPosition_.y_;
        }

        CUI::Position parentBottomRightPosition = parent->getAbsolutePositionWithScroll() + CUI::Position{parent->getSize().width_, parent->getSize().height_} - CUI::Position{parent->getPadding().left_ + parent->getPadding().right_, parent->getPadding().top_ + parent->getPadding().bottom_};

        if (padBottomRightPosition.x_ > parentBottomRightPosition.x_)
        {
            padBottomRightPosition.x_ = parentBottomRightPosition.x_;
        }

        if (padBottomRightPosition.x_ > parent->padBottomRightPosition_.x_)
        {
            padBottomRightPosition.x_ = parent->padBottomRightPosition_.x_;
        }

        if (padBottomRightPosition.y_ > parentBottomRightPosition.y_)
        {
            padBottomRightPosition.y_ = parentBottomRightPosition.y_;
        }

        if (padBottomRightPosition.y_ > parent->padBottomRightPosition_.y_)
        {
            padBottomRightPosition.y_ = parent->padBottomRightPosition_.y_;
        }
    }

    padTopLeftPosition_ = padTopLeftPosition;
    padBottomRightPosition_ = padBottomRightPosition;

    // Pad position according to screen size
    CUI::Size screenSize = {0, 0};
    getmaxyx(stdscr, screenSize.height_, screenSize.width_);

    screenSize.width_ -= getPadding().left_ + getPadding().right_;
    screenSize.height_ -= getPadding().top_ + getPadding().right_;

    if (padBottomRightPosition.x_ > screenSize.width_)
    {
        padBottomRightPosition.x_ -= padBottomRightPosition.x_ - screenSize.width_;
    }

    if (padBottomRightPosition.y_ > screenSize.height_)
    {
        padBottomRightPosition.y_ -= padBottomRightPosition.y_ - screenSize.height_;
    }

    pnoutrefresh(pad_, offsetPosition.y_, offsetPosition.x_, padTopLeftPosition.y_, padTopLeftPosition.x_, padBottomRightPosition.y_, padBottomRightPosition.x_);
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