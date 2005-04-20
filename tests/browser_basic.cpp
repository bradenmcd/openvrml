# include <cstdlib>
# include <iostream>
# include <openvrml/browser.h>

int main()
{
    using namespace std;

    try {
        openvrml::browser b(cout, cerr);
    } catch (...) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
