# include <cstdlib>
# include <iostream>
# include <sstream>
# include <openvrml/browser.h>

int main()
{
    using namespace std;
    using namespace openvrml;

    try {
        browser b(cout, cerr);

        const char vrmlstring[] = "Group {}";
        stringstream vrmlstream(vrmlstring);

        vector<node_ptr> nodes = b.create_vrml_from_stream(vrmlstream);
        if (nodes.size() != 1) {
            return EXIT_FAILURE;
        }
        if (!nodes[0]) {
            return EXIT_FAILURE;
        }
        if (nodes[0]->type().id() != "Group") {
            return EXIT_FAILURE;
        }
    } catch (...) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
