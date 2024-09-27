#include "CUIWindow.h"

CUI::Window::Window(CUI::Window *parent, const std::string &title, CUI::Direction direction)
    : CUI::Widget(parent), title_(title)
{
    /* Pad initialization */
    pad_ = newpad(1, 1); // Use non-zero space to avoid displaying issue.
    keypad(pad_, true);  // Enable special keys to use F1, Esc and etc.

    /* Border initialization */
    border_ = parent ? derwin(parent->getPad(), 0, 0, 0, 0) : newwin(0, 0, 0, 0);

    /* Border panel initialization */
    panel_ = new_panel(border_); // Use to hold the border window, ensuring it doesn't overlap with other windows.

    /* Widget settings */
    setDirection(direction);  // Direction of the window's related widgets.
    setPadding({1, 1, 1, 1}); // Default padding to 1 for each border length.
}

CUI::Window::~Window()
{
    /* Removing all child widgets */
    for (auto &widget : widgets_)
    {
        delete widget;
    }

    /* Clean up window resources */
    del_panel(panel_);
    delwin(border_);
    delwin(pad_);
}

int CUI::Window::run()
{
    int character = 0;
    bool exit = false;

    activate(); // Refresh window despite of existed child widgets and activa status.

    /* Main loop: process user input and update the window */
    for (std::size_t i = 0; i < widgets_.size() && !exit;)
    {
        /* (Dea-|A-)ctivate current window widget due to next one */
        dynamic_cast<CUI::Window *>(widgets_[i]) ? deactivate() : activate();

        refresh(); // Repaint window with updated data.

        /* Running child widget */
        character = widgets_[i]->run();

        /* Run result processing */
        switch (character)
        {
        case KEY_PPAGE:
        case KEY_NPAGE:
        case CUI::KEY_LPAGE:
        case CUI::KEY_RPAGE:
            /* Scrolling */
            if (scrolling(character, getSize().width_ / 10) || !getParent()) // Scroll the window if it's possible or check the window is not a main.
            {
                if (i) // Try to go to previous widget.
                {
                    i--;
                }

                break;
            }
            else
            {
                exit = true; // Exit current widget to scroll parent.
            }
        case KEY_F(1):
        case 27:
            /* Exit */
            exit = true;
            break;
        case KEY_RESIZE:
            /* Resize */
            if (getParent()) // Resize windows down from main.
            {
                exit = true;
            }
            offset_ = {0, 0}; // Reset window offset.
            break;
        default:
            /* Next */
            i++; // Go to next widget.
            break;
        }
    }

    deactivate();

    return character;
}

void CUI::Window::refresh()
{
    update(); // Update window dimensions and settings.

    /* Re-draw window */
    clear();
    draw();

    doupdate(); // Draw updated window on screen.
}

void CUI::Window::update()
{
    /* Updating window */
    replace(); // Replace widgets.
    resize();  // Resize before movement.
    move();    // Movement after resize.

    /* Updating all child widgets correspondingly */
    for (auto &widget : widgets_)
    {
        widget->update();
    }
}

void CUI::Window::draw()
{
    /* Draw content */
    drawBorder();
    drawTitle();
    drawWidgets();

    /* Border panel update */
    update_panels();

    // Draw pad and entire widgets(may has pads) again to avoid panel overlap issue.
    drawPad();
    drawWidgets();
}

void CUI::Window::resize()
{
    // Resize child widgets at first to  avoid size mismatch or incompleteness.
    for (auto &widget : widgets_)
    {
        widget->resize();
    }

    CUI::Size size = {0, 0};

    auto maxWidthPtr = std::max_element(widgets_.begin(), widgets_.end(), [](const auto &a, const auto &b)
                                        { return a->getSize().width_ + a->getPosition().x_ < b->getSize().width_ + b->getPosition().x_; });
    auto maxHeightPtr = std::max_element(widgets_.begin(), widgets_.end(), [](const auto &a, const auto &b)
                                         { return a->getSize().height_ + a->getPosition().y_ < b->getSize().height_ + b->getPosition().y_; });

    // Calculate new border size.
    {
        if (getParent()) // No limits if no parent, besides widget max size.
        {
            // Get max size according to max value of widget position, size and border.
            size.width_ = widgets_.end() != maxWidthPtr ? (*maxWidthPtr)->getPosition().x_ + (*maxWidthPtr)->getSize().width_ + getPadding().isLeft() + getPadding().right_ : 0;
            size.height_ = widgets_.end() != maxHeightPtr ? (*maxHeightPtr)->getPosition().y_ + (*maxHeightPtr)->getSize().height_ + getPadding().isTop() + getPadding().bottom_ : 0;

            // Limit new calculated sizes if > widget max size exists.
            if (getMaxSize())
            {
                size.width_ = std::min(size.width_, getMaxSize().width_);
                size.height_ = std::min(size.height_, getMaxSize().height_);
            }
        }
        else
        {
            getmaxyx(stdscr, size.height_, size.width_); // Get screen size.
        }

        setSize(size);                               // Save calculated size.
        wresize(border_, size.height_, size.width_); // Resize border.
    }

    CUI::Size padSize = size; // Border size is default value for pad size to avoid displaying issue.

    // Calculate new pad size.
    {
        // Get pad size according to max value of widget position and size.
        padSize.width_ = widgets_.end() != maxWidthPtr ? (*maxWidthPtr)->getPosition().x_ + (*maxWidthPtr)->getSize().width_ : padSize.width_;
        padSize.height_ = widgets_.end() != maxHeightPtr ? (*maxHeightPtr)->getPosition().y_ + (*maxHeightPtr)->getSize().height_ : padSize.height_;
        // Add padding to previous calculated pad size.
        padSize.width_ += std::max(0, getPadding().right_ - 1);
        padSize.height_ += std::max(0, getPadding().bottom_ - 1);

        padSize_ = padSize;                               // Save pad size.
        wresize(pad_, padSize_.height_, padSize_.width_); // Resize pad.
    }
}

void CUI::Window::add(Widget *widget)
{
    assert(widget); // Check if widget exists.

    auto found = std::find_if(widgets_.begin(), widgets_.end(), [widget](const auto &item)
                              { return item == widget; });

    assert(widgets_.end() == found); // Check if widget is unique.

    widgets_.emplace_back(widget); // Add child widget to common array.

    replace(); // Replace child widget according current direction to corresponding position.
}

WINDOW *CUI::Window::getPad() const
{
    return pad_;
}

WINDOW *CUI::Window::getBorder() const
{
    return border_;
}

void CUI::Window::move()
{
    CUI::Position position = getPosition();

    wmove(border_, position.y_, position.x_); // Window movement.

    if (getParent())
    {
        mvderwin(border_, position.y_, position.x_); // Derwin movement.
    }

    move_panel(panel_, position.y_, position.x_); // Panel movement.
}

void CUI::Window::replace()
{
    CUI::Widget *previous = nullptr;
    CUI::Position position = {std::max(0, getPadding().left_ - 1), std::max(0, getPadding().top_ - 1)};

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
    wclear(pad_);    // Clear pad.
    wclear(border_); // Clear border.
}

void CUI::Window::drawBorder()
{
    char topBorder = getPadding().isTop() ? '-' : ' ';
    char rigthBorder = getPadding().isRight() ? '|' : ' ';
    char bottomBorder = getPadding().isBottom() ? '-' : ' ';
    char leftBorder = getPadding().isLeft() ? '|' : ' ';

    wborder(border_, leftBorder, rigthBorder, topBorder, bottomBorder, '+', '+', '+', '+');
}

void CUI::Window::drawTitle()
{
    if (!getPadding().isTop())
    {
        return; // The is no space for title.
    }

    CUI::Size size = getSize();

    wattron(border_, A_BOLD); // Turn of bold font.
    mvwprintw(border_, 0, (size.width_ - title_.length()) / 2, "%s", title_.c_str());
    wattroff(border_, A_BOLD); // Turn off bold font.
}

void CUI::Window::drawWidgets()
{
    for (auto &widget : widgets_)
    {
        widget->draw();
    }
}

void CUI::Window::drawPad()
{
    CUI::Position offsetPosition = offset_;
    CUI::Position padTopLeftPosition = getAbsolutePositionWithScroll() + CUI::Position{getPadding().isLeft(), getPadding().isTop()};
    CUI::Position padBottomRightPosition = getAbsolutePositionWithScroll() + CUI::Position{getSize().width_, getSize().height_} - CUI::Position{getPadding().isLeft() + getPadding().isRight(), getPadding().isTop() + getPadding().isBottom()};

    /* Pad position limit relative to parent */
    if (getParent())
    {
        CUI::Window *parent = dynamic_cast<CUI::Window *>(getParent());

        assert(parent); // Parent must be a window.

        // Limit pad top left position relative to parent.
        {
            CUI::Position parentTopLeftPosition = parent->getAbsolutePositionWithScroll() + CUI::Position{parent->getPadding().isLeft(), parent->getPadding().isTop()};

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
        }

        // Limit pad bottom right position relative to parent.
        {
            CUI::Position parentBottomRightPosition = parent->getAbsolutePositionWithScroll() + CUI::Position{parent->getSize().width_, parent->getSize().height_} - CUI::Position{parent->getPadding().isLeft() + parent->getPadding().isRight(), parent->getPadding().isTop() + parent->getPadding().isBottom()};

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
    }

    /* Pad position limit relative to screen */
    {
        CUI::Size screenSize = {0, 0};
        getmaxyx(stdscr, screenSize.height_, screenSize.width_);

        screenSize.width_ -= getPadding().isLeft() + getPadding().isRight();
        screenSize.height_ -= getPadding().isTop() + getPadding().isRight();

        if (padBottomRightPosition.x_ > screenSize.width_)
        {
            padBottomRightPosition.x_ -= padBottomRightPosition.x_ - screenSize.width_;
        }

        if (padBottomRightPosition.y_ > screenSize.height_)
        {
            padBottomRightPosition.y_ -= padBottomRightPosition.y_ - screenSize.height_;
        }
    }

    padTopLeftPosition_ = padTopLeftPosition;         // Save top left position.
    padBottomRightPosition_ = padBottomRightPosition; // Save bottom right position.

    /* Active pad color on */
    wattron(pad_, COLOR_PAIR(isActive() ? CUI::WIN_PAD_COLOR : CUI::WIN_BORDER_COLOR));
    /* Pad visible region displaying */
    pnoutrefresh(pad_, offsetPosition.y_, offsetPosition.x_, padTopLeftPosition.y_, padTopLeftPosition.x_, padBottomRightPosition.y_, padBottomRightPosition.x_);
}

bool CUI::Window::scrolling(int to, unsigned short step)
{
    CUI::Position oldOffset = offset_;

    switch (to)
    {
    case KEY_PPAGE:
        /* Up */
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
        /* Down */
        if (padSize_.height_ + getPadding().isTop() + getPadding().isBottom() >= offset_.y_ + getSize().height_ + step)
        {
            offset_.y_ += step;
        }
        else if (padSize_.height_ + getPadding().isTop() + getPadding().isBottom() >= offset_.y_ + getSize().height_ + 1)
        {
            offset_.y_++;
        }
        break;
    case CUI::KEY_RPAGE:
        /* Right */
        if (padSize_.width_ + getPadding().isLeft() + getPadding().isRight() >= offset_.x_ + getSize().width_ + step)
        {
            offset_.x_ += step;
        }
        else if (padSize_.width_ + getPadding().isLeft() + getPadding().isRight() >= offset_.x_ + getSize().width_ + 1)
        {
            offset_.x_++;
        }
        break;
    case CUI::KEY_LPAGE:
        /* Left */
        if (offset_.x_ >= step)
        {
            offset_.x_ -= step;
        }
        else if (offset_.x_ > 0)
        {
            offset_.x_--;
        }
        break;
    default:
        break;
    }

    return oldOffset != offset_; // Was offset changed?
}

CUI::Position CUI::Window::getAbsolutePositionWithScroll() const
{
    CUI::Position result = getPosition();

    if (getParent())
    {
        CUI::Window *parent = dynamic_cast<CUI::Window *>(getParent());

        assert(parent);

        result += parent->getAbsolutePositionWithScroll() + CUI::Position{parent->getPadding().isLeft(), parent->getPadding().isTop()} - parent->offset_;
    }

    return result;
}