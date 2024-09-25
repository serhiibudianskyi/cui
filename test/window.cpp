#include "CUI.h"

int main(int argc, char *argv[])
{
    CUI::Application app;

    CUI::Window *main = new CUI::Window(nullptr, "Main", CUI::VERTICAL);
    main->add(new CUI::Text(main, "This is line line line line line line"));
    CUI::Window *subh0 = new CUI::Window(main, "Sub 0", CUI::HORIZONTAL);
    subh0->add(new CUI::Text(subh0, "This is line " + std::to_string(0)));
    CUI::Window *subh0v0 = new CUI::Window(subh0, "Sub 0 0", CUI::VERTICAL);
    CUI::Window *subh0v0v0 = new CUI::Window(subh0v0, "Sub 0 0 0", CUI::VERTICAL);
    subh0v0v0->setMaxSize({20, 5});
    subh0v0v0->setPadding({1, 2, 3, 4});
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
    main->add(subh0);

    app.add(main);

    ungetch(27);

    return !app.run();
}