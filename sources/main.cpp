#include "app/application.hpp"

int main ()
{
    Application app;

    while ( app.should_run() )
    {
        app.handle_event();
        app.render();
    }

    return 0;
}
