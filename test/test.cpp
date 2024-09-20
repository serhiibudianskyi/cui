#include "CUI.h"

int main(int argc, char *argv[])
{
    CUI::Application app;

    CUI::Window *main = new CUI::Window(nullptr, "Main", CUI::VERTICAL);
    CUI::Window *subh0 = new CUI::Window(main, "Sub 0", CUI::HORIZONTAL);
    subh0->add(new CUI::Text(subh0, "This is line " + std::to_string(0)));
    CUI::Window *subh0v0 = new CUI::Window(subh0, "Sub 0 0", CUI::VERTICAL);
    CUI::Window *subh0v0v0 = new CUI::Window(subh0v0, "Sub 0 0 0", CUI::VERTICAL);
    for (std::size_t i = 0; i < 5; ++i)
    {
        subh0v0v0->add(new CUI::Text(subh0v0v0, "This is line line line line line line " + std::to_string(i)));
    }
    subh0v0->add(subh0v0v0);
    subh0->add(subh0v0);
    CUI::Window *subh0v1 = new CUI::Window(subh0, "Sub 0 1", CUI::VERTICAL);
    CUI::Window *subh0v1v1 = new CUI::Window(subh0v1, "Sub 0 1 1", CUI::VERTICAL);
    for (std::size_t i = 0; i < 5; ++i)
    {
        subh0v1v1->add(new CUI::Text(subh0v1v1, "This is line line line line line line " + std::to_string(i)));
    }
    subh0v1->add(subh0v1v1);
    subh0->add(subh0v1);
    subh0->add(new CUI::Text(subh0, "This is line " + std::to_string(0)));
    // CUI::Window *subh0v1 = new CUI::Window(subh0, "Sub 0 1", CUI::VERTICAL);
    // for (std::size_t i = 0; i < 5; ++i)
    // {
    //     subh0v1->add(new CUI::Text(subh0v1, "This is line " + std::to_string(i)));
    // }
    // subh0->add(subh0v1);
    // CUI::Window *subh0v2 = new CUI::Window(subh0, "Sub 021", CUI::VERTICAL);
    // for (std::size_t i = 0; i < 5; ++i)
    // {
    //     subh0v2->add(new CUI::Text(subh0v2, "This is line " + std::to_string(i)));
    // }
    // subh0->add(subh0v2);
    main->add(subh0);
    // CUI::Window *subWindow0 = new CUI::Window(mainWindow, "Sub 0", CUI::VERTICAL);
    // for (std::size_t i = 0; i < 5; ++i)
    // {
    //     subWindow0->add(new CUI::Text(subWindow0, "This is line " + std::to_string(i)));
    // }
    // mainWindow->add(subWindow0);
    // for (std::size_t i = 0; i < 5; ++i)
    // {
    //     mainWindow->add(new CUI::Text(mainWindow, "This is line long line long line long line long line long line " + std::to_string(i)));
    // }

    app.add(main);

    return app.run();
}