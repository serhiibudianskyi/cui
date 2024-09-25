#include "CUIText.h"

CUI::Text::Text(CUI::Window *parent, const std::string &text) : CUI::Widget(parent), text_(text)
{
    setSize({static_cast<int>(text_.length()), 1});
}

int CUI::Text::run()
{
    CUI::Window *parent = dynamic_cast<CUI::Window *>(getParent());
    assert(parent); // Parent should be a window widget.

    activate();
    refresh();

    int character = wgetch(parent->getPad());

    deactivate();

    return character;
}

void CUI::Text::refresh()
{
    update();
    draw();
}

void CUI::Text::update()
{
    return; // Nothing to do.
}

void CUI::Text::draw()
{
    CUI::Window *parent = dynamic_cast<CUI::Window *>(getParent());
    assert(parent); // Parent should be a window widget.

    /* Active text color on */
    wattron(parent->getPad(), COLOR_PAIR(isActive() ? WIN_CURSOR_COLOR : parent->isActive() ? WIN_PAD_COLOR : WIN_BORDER_COLOR));
    /* Print text */
    mvwprintw(parent->getPad(), getPosition().y_, getPosition().x_, "%s", text_.c_str());
}

void CUI::Text::resize()
{
    return; // Nothing to do.
}