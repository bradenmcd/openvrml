# include <dejagnu.h>
# include <openvrml/node.h>

//
// Check to see that node_interface_set is working correctly.
// node_interface_set is an instance of std::set, so basically this
// makes sure that node_interface_compare is doing the right thing.
//

using namespace std;
using namespace openvrml;

TestState ts;

int main()
{
    node_interface_set interfaces;
    string test_desc;
    bool succeeded;

    {
        test_desc = "insert an eventIn";

        const node_interface interface(node_interface::eventin_id,
                                       field_value::sfbool_id,
                                       "set_zzz");
        succeeded = interfaces.insert(interface).second;
        if (succeeded) {
            ts.pass(test_desc);
        } else {
            ts.fail(test_desc);
        }
    }

    {
        test_desc = "insert an exactly conflicting eventIn";

        const node_interface interface(node_interface::eventin_id,
                                       field_value::sfcolor_id,
                                       "set_zzz");
        succeeded = interfaces.insert(interface).second;
        if (succeeded) {
            ts.fail(test_desc);
        } else {
            ts.pass(test_desc);
        }
    }

    {
        test_desc = "insert a conflicting exposedField";

        const node_interface interface(node_interface::exposedfield_id,
                                       field_value::sfcolor_id,
                                       "zzz");
        succeeded = interfaces.insert(interface).second;
        if (succeeded) {
            ts.fail(test_desc);
        } else {
            ts.pass(test_desc);
        }
    }

    {
        test_desc = "insert an eventOut";

        const node_interface interface(node_interface::eventout_id,
                                       field_value::sfbool_id,
                                       "zzz_changed");
        succeeded = interfaces.insert(interface).second;
        if (succeeded) {
            ts.pass(test_desc);
        } else {
            ts.fail(test_desc);
        }
    }

    {
        test_desc = "insert an exactly conflicting eventOut";

        const node_interface interface(node_interface::eventout_id,
                                       field_value::sfcolor_id,
                                       "zzz_changed");
        succeeded = interfaces.insert(interface).second;
        if (succeeded) {
            ts.fail(test_desc);
        } else {
            ts.pass(test_desc);
        }
    }

    {
        test_desc = "insert a field";

        const node_interface interface(node_interface::field_id,
                                       field_value::sfint32_id,
                                       "zzz");
        succeeded = interfaces.insert(interface).second;
        if (succeeded) {
            ts.pass(test_desc);
        } else {
            ts.fail(test_desc);
        }
    }

    {
        test_desc = "insert an exposedfield";

        const node_interface interface(node_interface::exposedfield_id,
                                       field_value::sfrotation_id,
                                       "yyy");
        succeeded = interfaces.insert(interface).second;
        if (succeeded) {
            ts.pass(test_desc);
        } else {
            ts.fail(test_desc);
        }
    }

    {
        test_desc = "find an exposedField that matches an eventIn";

        const node_interface interface(node_interface::eventin_id,
                                       field_value::sfbool_id,
                                       "set_yyy");

        const node_interface_set::const_iterator pos =
            interfaces.find(interface);

        if (pos != interfaces.end()) {
            ts.pass(test_desc);
        } else {
            ts.fail(test_desc);
        }
    }

    return ::failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
