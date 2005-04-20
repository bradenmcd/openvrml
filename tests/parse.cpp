# include <iostream>
# include <openvrml/browser.h>

using namespace std;
using namespace openvrml;

int main()
{
    try {
        browser b(cout, cerr);
        b.create_vrml_from_stream(cin);
    } catch (invalid_vrml & ex) {
        cerr << ex.url << ':' << ex.line << ':' << ex.column << ": error: "
             << ex.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
