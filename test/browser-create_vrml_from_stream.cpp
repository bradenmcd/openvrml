# include <cstdlib>
# include <iostream>
# include <sstream>
# include "test_browser.h"

int main()
{
    using namespace std;
    using namespace openvrml;

    try {
        test_browser b;

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
