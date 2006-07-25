# include <boost/test/unit_test.hpp>
# include <openvrml/browser.h>

using namespace std;
using namespace openvrml;

void construct_from_urn()
{
    node_metatype_id id("urn:foo:bar");
}

void construct_from_url()
{
    node_metatype_id id("http://example.com");
}

void construct_from_url_with_fragment_id()
{
    node_metatype_id id("http://example.com#Foo");
}

void construct_nested_proto_id()
{
    node_metatype_id id("http://example.com#Foo#Bar");
}

void construct_from_relative_url()
{
    BOOST_CHECK_THROW(node_metatype_id id("../foo/bar"),
                      std::invalid_argument);
}

void construct_from_arbitrary_string()
{
    BOOST_CHECK_THROW(node_metatype_id id("string"), std::invalid_argument);
}

boost::unit_test::test_suite * init_unit_test_suite(int, char * [])
{
    using boost::unit_test::test_suite;
    test_suite * const suite = BOOST_TEST_SUITE("node_metatype_id");
    suite->add(BOOST_TEST_CASE(&construct_from_urn));
    suite->add(BOOST_TEST_CASE(&construct_from_url));
    suite->add(BOOST_TEST_CASE(&construct_from_url_with_fragment_id));
    suite->add(BOOST_TEST_CASE(&construct_nested_proto_id));
    suite->add(BOOST_TEST_CASE(&construct_from_relative_url));
    suite->add(BOOST_TEST_CASE(&construct_from_arbitrary_string));
    return suite;
}
