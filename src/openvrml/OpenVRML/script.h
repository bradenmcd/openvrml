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

#   include <stddef.h>
#   include <string>
#   include "common.h"
#   include "fieldvalueptr.h"
#   include "vrml97node.h"

namespace OpenVRML {

    class FieldValue;
    class ScriptNode;

    class OPENVRML_SCOPE Script {
    public:
        virtual ~Script() = 0;
        virtual void initialize(double timestamp) = 0;
        virtual void processEvent(const std::string & id,
                                  const FieldValue & value,
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
    
    
    class OPENVRML_SCOPE ScriptNodeClass : public NodeClass {
    public:
        ScriptNodeClass(VrmlScene & scene);
        virtual ~ScriptNodeClass() throw ();
        
        virtual const NodeTypePtr createType(const std::string & id,
                                             const NodeInterfaceSet &) throw ();
    };
    
    
    class Doc;
    class VrmlScene;

    class OPENVRML_SCOPE ScriptNode : public ChildNode {
    public:
        typedef std::map<std::string, FieldValuePtr> FieldValueMap;
        typedef std::map<std::string, PolledEventOutValue> EventOutValueMap;
    
    private:
        class ScriptNodeType : public NodeType {
            NodeInterfaceSet interfaces;

        public:
            ScriptNodeType(ScriptNodeClass & nodeClass);
            virtual ~ScriptNodeType() throw ();

            void addInterface(const NodeInterface & interface)
                    throw (std::invalid_argument);

            virtual const NodeInterfaceSet & getInterfaces() const throw ();
            virtual const NodePtr createNode() const throw (std::bad_alloc);
        };
        
        friend class ScriptNodeType;
    
        ScriptNodeType scriptNodeType;
        SFBool directOutput;
        SFBool mustEvaluate;
        MFString url;
        FieldValueMap fieldValueMap;
        EventOutValueMap eventOutValueMap;
        Script * script;
        int eventsReceived;
    
    public:
        ScriptNode(ScriptNodeClass & nodeClass);
        virtual ~ScriptNode() throw ();
        
        void setUrl(const MFString & value, double timestamp);
        const MFString & getUrl() const;
        
        void addEventIn(FieldValue::Type type, const std::string & id)
                throw (std::invalid_argument, std::bad_alloc);
        void addEventOut(FieldValue::Type type, const std::string & id)
                throw (std::invalid_argument, std::bad_alloc);
        void addField(const std::string & id,
                      const FieldValuePtr & defaultValue)
                throw (std::invalid_argument, std::bad_alloc);
        
        void initialize(double timestamp);
        void update(double timestamp);
        void shutdown(double timestamp);
        
        void setEventOut(const std::string & id, const FieldValue & value)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        
        const FieldValueMap & getFieldValueMap() const throw ();
        const EventOutValueMap & getEventOutValueMap() const throw ();
        
        virtual const ScriptNode * toScript() const throw ();
        virtual ScriptNode * toScript() throw ();
    
    private:
        Script * createScript();
        
        void assignWithSelfRefCheck(const SFNode &, SFNode &) const throw ();
        void assignWithSelfRefCheck(const MFNode &, MFNode &) const throw ();
        
        virtual void setFieldImpl(const std::string & id,
                                  const FieldValue & value)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        
        virtual const FieldValue & getFieldImpl(const std::string & id) const
                throw (UnsupportedInterface);
        
        virtual void processEventImpl(const std::string & id,
                                      const FieldValue & value,
                                      double timestamp)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        
        virtual const FieldValue & getEventOutImpl(const std::string & id) const
                throw (UnsupportedInterface);
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
