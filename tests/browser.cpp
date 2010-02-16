// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2005, 2006, 2007  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; if not, see <http://www.gnu.org/licenses/>.
//

# define BOOST_TEST_MAIN
# define BOOST_TEST_MODULE browser

# include <fstream>
# include <iostream>
# include <sstream>
# include <boost/filesystem/operations.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/thread.hpp>
# include <boost/test/unit_test.hpp>
# include "test_resource_fetcher.h"

using namespace std;
using namespace openvrml;
using namespace boost::filesystem;
using namespace boost::multi_index::detail; // for scope_guard

BOOST_AUTO_TEST_CASE(create_vrml_from_stream)
{
    test_resource_fetcher fetcher;
    browser b(fetcher, std::cout, std::cerr);

    const char vrmlstring[] = "Group {}";
    stringstream vrmlstream(vrmlstring);

    vector<boost::intrusive_ptr<node> > nodes =
        b.create_vrml_from_stream(vrmlstream);

    BOOST_REQUIRE(nodes.size() == 1);
    BOOST_REQUIRE(nodes[0] != boost::intrusive_ptr<node>(0));
    BOOST_CHECK_EQUAL(nodes[0]->type().id(), "Group");
}

BOOST_AUTO_TEST_CASE(create_vrml_from_stream_with_externproto)
{
    {
        ofstream file("test.wrl");
        file << "#VRML V2.0 utf8" << endl
             << "PROTO Node [] { Group {} }" << endl;
    }
    scope_guard test_file_guard =
        make_guard(&boost::filesystem::remove<boost::filesystem::path>,
                   boost::filesystem::path("test.wrl"));
    boost::ignore_unused_variable_warning(test_file_guard);

    const char vrmlstring[] = "EXTERNPROTO Node [] [ \"test.wrl\" ] Node {}";
    stringstream vrmlstream(vrmlstring);

    test_resource_fetcher fetcher;
    browser b(fetcher, std::cout, std::cerr);

    vector<boost::intrusive_ptr<node> > nodes =
        b.create_vrml_from_stream(vrmlstream);

    BOOST_REQUIRE(nodes.size() == 1);
    BOOST_REQUIRE(nodes[0] != boost::intrusive_ptr<node>(0));
    BOOST_CHECK_EQUAL(nodes[0]->type().id(), "Node");
}

BOOST_AUTO_TEST_CASE(create_vrml_from_url)
{
    class children_listener : public openvrml::mfnode_listener {
        bool received_event_;
        boost::mutex & mutex_;
        boost::condition_variable & condition_;

    public:
        children_listener(boost::mutex & mutex,
                          boost::condition_variable & condition):
            received_event_(false),
            mutex_(mutex),
            condition_(condition)
        {}

        bool received_event() const
        {
            return this->received_event_;
        }

    private:
        virtual void do_process_event(const openvrml::mfnode &, double)
            throw (std::bad_alloc)
        {
            boost::mutex::scoped_lock lock(this->mutex_);
            this->received_event_ = true;
            this->condition_.notify_all();
        }
    };

    {
	ofstream file("test.wrl");
        file << "#VRML V2.0 utf8" << endl
	     << "Shape {}" << endl;
    }
    scope_guard test_file_guard =
        make_guard(&boost::filesystem::remove<boost::filesystem::path>,
                   boost::filesystem::path("test.wrl"));
    boost::ignore_unused_variable_warning(test_file_guard);

    test_resource_fetcher fetcher;
    browser b(fetcher, std::cout, std::cerr);
    const char vrmlstring[] = "Group {}";
    stringstream vrmlstream(vrmlstring);
    vector<boost::intrusive_ptr<node> > nodes =
        b.create_vrml_from_stream(vrmlstream);

    boost::mutex mutex;
    boost::condition_variable listener_received_event;

    children_listener listener(mutex, listener_received_event);
    mfnode_emitter & emitter =
        nodes[0]->event_emitter<mfnode>("children_changed");
    emitter.add(listener);

    vector<string> url(1, "test.wrl");
    b.create_vrml_from_url(url, nodes[0], "set_children");
    {
        boost::mutex::scoped_lock lock(mutex);
        while (!listener.received_event()) {
            boost::xtime xt;
            boost::xtime_get(&xt, boost::TIME_UTC);
            xt.sec += 1;
            const bool succeeded =
                listener_received_event.timed_wait(lock, xt);
            BOOST_REQUIRE(succeeded);
        }
    }

    grouping_node * group = node_cast<grouping_node *>(nodes[0].get());
    BOOST_REQUIRE(group);
    const vector<boost::intrusive_ptr<node> > & children = group->children();
    BOOST_REQUIRE(children.size() == 1);
    BOOST_CHECK_EQUAL(children[0]->type().id(), "Shape");
}
