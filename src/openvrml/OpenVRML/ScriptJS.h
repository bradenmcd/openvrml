//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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
#ifndef SCRIPTJS_H
#define SCRIPTJS_H

#include "common.h"
#include "ScriptObject.h"
#include "field.h"

class VrmlNodeScript;
class VrmlScene;

typedef long jsval;

class OPENVRML_SCOPE ScriptJS: public ScriptObject {
    static struct JSRuntime * rt;
    static size_t nInstances;

    double d_timeStamp;

    struct JSContext * d_cx;
    struct JSObject * d_globalObj;
    struct JSObject * d_browserObj;

public:
    ScriptJS(VrmlNodeScript & scriptNode, const char * source);
    virtual ~ScriptJS();

    virtual void activate(double timeStamp, const char * fname,
                          size_t argc, const VrmlField * argv[]);

    VrmlNodeScript & getScriptNode() { return this->scriptNode; }

    jsval vrmlFieldToJSVal(VrmlField::VrmlFieldType, const VrmlField * f,
                           bool protect);

private:
    void defineFields();
};

#endif

