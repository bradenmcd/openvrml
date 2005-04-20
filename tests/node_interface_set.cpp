# if 0
# include <dejagnu.h>
# endif
# include <boost/test/unit_test.hpp>
# include <openvrml/node.h>

//
// Check to see that node_interface_set is working correctly.
// node_interface_set is an instance of std::set, so basically this
// makes sure that node_interface_compare is doing the right thing.
//

using namespace std;
using namespace openvrml;

void insert_eventin()
{
    node_interface_set interfaces;
    const node_interface interface(node_interface::eventin_id,
				   field_value::sfbool_id,
				   "set_zzz");
    BOOST_REQUIRE(interfaces.insert(interface).second);
}

void insert_exactly_conflicting_eventin()
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);

    const node_interface interface2(node_interface::eventin_id,
				    field_value::sfcolor_id,
				    "set_zzz");
    BOOST_REQUIRE(!interfaces.insert(interface2).second);
}

void insert_conflicting_exposedfield()
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::exposedfield_id,
				    field_value::sfcolor_id,
				    "zzz");
    BOOST_REQUIRE(!interfaces.insert(interface2).second);
}

void insert_eventout()
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::eventout_id,
				    field_value::sfbool_id,
				    "zzz_changed");
    BOOST_REQUIRE(interfaces.insert(interface2).second);
}

void insert_exactly_conflicting_eventout()
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventout_id,
				    field_value::sfbool_id,
				    "zzz_changed");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::eventout_id,
				    field_value::sfcolor_id,
				    "zzz_changed");
    BOOST_REQUIRE(!interfaces.insert(interface2).second);
}

void insert_field()
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::eventout_id,
				    field_value::sfbool_id,
				    "zzz_changed");
    BOOST_CHECK(interfaces.insert(interface2).second);
    const node_interface interface3(node_interface::field_id,
				    field_value::sfint32_id,
				    "zzz");
    BOOST_REQUIRE(interfaces.insert(interface3).second);
}

void insert_exposedfield()
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::eventout_id,
				    field_value::sfbool_id,
				    "zzz_changed");
    BOOST_CHECK(interfaces.insert(interface2).second);
    const node_interface interface3(node_interface::field_id,
				    field_value::sfint32_id,
				    "zzz");
    BOOST_CHECK(interfaces.insert(interface3).second);
    const node_interface interface4(node_interface::exposedfield_id,
				    field_value::sfrotation_id,
				    "yyy");
    BOOST_REQUIRE(interfaces.insert(interface4).second);
}

void find_exposedfield_matching_eventin()
{
    node_interface_set interfaces;
    const node_interface interface(node_interface::exposedfield_id,
				   field_value::sfrotation_id,
				   "yyy");
    BOOST_CHECK(interfaces.insert(interface).second);
    const node_interface_set::const_iterator pos =
	interfaces.find(interface);
    BOOST_CHECK(pos != interfaces.end());
    BOOST_REQUIRE(interface == *pos);
}

boost::unit_test::test_suite * init_unit_test_suite(int, char * [])
{
    using boost::unit_test::test_suite;
    test_suite * const suite =
	BOOST_TEST_SUITE("node_interface_set");
    suite->add(BOOST_TEST_CASE(&insert_eventin));
    suite->add(BOOST_TEST_CASE(&insert_exactly_conflicting_eventin));
    suite->add(BOOST_TEST_CASE(&insert_conflicting_exposedfield));
    suite->add(BOOST_TEST_CASE(&insert_eventout));
    suite->add(BOOST_TEST_CASE(&insert_exactly_conflicting_eventout));
    suite->add(BOOST_TEST_CASE(&insert_field));
    suite->add(BOOST_TEST_CASE(&insert_exposedfield));
    suite->add(BOOST_TEST_CASE(&find_exposedfield_matching_eventin));
    return suite;
}


# if 0
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
# endif
