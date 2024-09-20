#include "CUIApplication.h"

CUI::Application::Application()
{
    initscr();
    cbreak();
    noecho();
    set_escdelay(false);
    curs_set(false);

    start_color();

    init_pair(WIN_BORDER_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(WIN_PAD_COLOR, COLOR_BLACK, COLOR_GREEN);
}

CUI::Application::~Application()
{
    for (const auto &window : windows_)
    {
        delete window;
    }

    curs_set(true);
    endwin();
}

int CUI::Application::run()
{
    int result = 0;

    for (std::size_t i = 0; i < windows_.size() && !result; ++i)
    {
        result = windows_[i]->run();
    }

    return result;
}

void CUI::Application::add(CUI::Window *window)
{
    assert(window);

    auto found = std::find_if(windows_.begin(), windows_.end(), [window](const auto &item)
                              { return item == window; });

    assert(windows_.end() == found);

    windows_.emplace_back(window);
}