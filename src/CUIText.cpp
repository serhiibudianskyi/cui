#include "CUIText.h"

CUI::Text::Text(CUI::Window *parent, const std::string &text) : CUI::Widget(parent), text_(text)
{
    setSize({static_cast<int>(text_.length()), 1});
}

int CUI::Text::run()
{
    CUI::Window *window = dynamic_cast<CUI::Window *>(getParent());

    assert(window);

    return wgetch(window->getPad());
}

void CUI::Text::refresh()
{
    update();

    draw();
}

void CUI::Text::update()
{
    return;
}

void CUI::Text::draw()
{
    CUI::Window *window = dynamic_cast<CUI::Window *>(getParent());

    assert(window);

    mvwprintw(window->getPad(), getPosition().y_, getPosition().x_, "%s", text_.c_str());
}

void CUI::Text::resize()
{
    return;
}