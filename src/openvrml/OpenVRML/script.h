// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2001  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifndef OPENVRML_SCRIPT_H
#   define OPENVRML_SCRIPT_H

#   include <OpenVRML/field_value_ptr.h>
#   include <OpenVRML/node.h>

namespace OpenVRML {

    class ScriptNode;

    class OPENVRML_SCOPE Script {
    public:
        virtual ~Script() = 0;
        virtual void initialize(double timestamp) = 0;
        virtual void processEvent(const std::string & id,
                                  const field_value & value,
                                  double timestamp) = 0;
        virtual void eventsProcessed(double timestamp) = 0;
        virtual void shutdown(double timestamp) = 0;

    protected:
        ScriptNode & scriptNode;

        Script(ScriptNode & scriptNode);

    private:
        // non-copyable
        Script(const Script &);
        Script & operator=(const Script &);
    };


    class OPENVRML_SCOPE ScriptNodeClass : public node_class {
    public:
        ScriptNodeClass(Browser & browser);
        virtual ~ScriptNodeClass() throw ();

        virtual const node_type_ptr
        create_type(const std::string & id,
                    const node_interface_set & interfaces)
            throw ();
    };


    class OPENVRML_SCOPE ScriptNode : public child_node {
    public:
        typedef std::map<std::string, field_value_ptr> FieldValueMap;
        typedef std::map<std::string, polled_eventout_value> EventOutValueMap;

    private:
        class ScriptNodeType : public node_type {
            node_interface_set interfaces_;

        public:
            ScriptNodeType(ScriptNodeClass & nodeClass);
            virtual ~ScriptNodeType() throw ();

            void addInterface(const node_interface & interface)
                    throw (std::invalid_argument);

            virtual const node_interface_set & interfaces() const throw ();
            virtual const NodePtr create_node(const ScopePtr & scope) const
                    throw (std::bad_alloc);
        };

        friend class ScriptNodeType;

        ScriptNodeType scriptNodeType;
        sfbool directOutput;
        sfbool mustEvaluate;
        mfstring url;
        FieldValueMap fieldValueMap;
        EventOutValueMap eventOutValueMap;
        Script * script;
        int eventsReceived;

    public:
        ScriptNode(ScriptNodeClass & nodeClass,
                   const ScopePtr & scope);
        virtual ~ScriptNode() throw ();

        void setUrl(const mfstring & value, double timestamp);
        const mfstring & getUrl() const;

        void addEventIn(field_value::type_id type_id, const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        void addEventOut(field_value::type_id type_id, const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        void addField(const std::string & id,
                      const field_value_ptr & defaultValue)
            throw (std::invalid_argument, std::bad_alloc);

        void update(double timestamp);

        void setEventOut(const std::string & id, const field_value & value)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);

        const FieldValueMap & getFieldValueMap() const throw ();
        const EventOutValueMap & getEventOutValueMap() const throw ();

        virtual const ScriptNode * to_script() const throw ();
        virtual ScriptNode * to_script() throw ();

    private:
        Script * createScript();

        void assignWithSelfRefCheck(const sfnode &, sfnode &) const throw ();
        void assignWithSelfRefCheck(const mfnode &, mfnode &) const throw ();

        virtual void do_initialize(double timestamp) throw (std::bad_alloc);
        virtual void do_field(const std::string & id,
                              const field_value & value)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const field_value & do_field(const std::string & id) const
            throw (unsupported_interface);
        virtual void do_process_event(const std::string & id,
                                      const field_value & value,
                                      double timestamp)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const field_value &
        do_eventout(const std::string & id) const
            throw (unsupported_interface);
        virtual void do_shutdown(double timestamp) throw ();
    };

    inline const ScriptNode::FieldValueMap &
            ScriptNode::getFieldValueMap() const throw () {
        return this->fieldValueMap;
    }

    inline const ScriptNode::EventOutValueMap &
            ScriptNode::getEventOutValueMap() const throw () {
        return this->eventOutValueMap;
    }
}

# endif
