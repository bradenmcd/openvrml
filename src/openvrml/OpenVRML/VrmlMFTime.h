//
// OpenVRML
//
// Copyright (C) 2000  Braden McDaniel
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

# ifndef VRMLMFTIME_H
# define VRMLMFTIME_H

# include "VrmlField.h"
# include <stddef.h>

class VrmlMFTime : public VrmlField {
    public:
        VrmlMFTime();
        VrmlMFTime(double);
        VrmlMFTime(size_t, double const *);
        VrmlMFTime(VrmlMFTime const &);
        
        ~VrmlMFTime();
        
        VrmlMFTime & operator=(VrmlMFTime const &);
        
        void set(size_t, double const *);
        double const * get() const;
        double const & operator[](size_t) const;
        size_t size() const;
        
        //
        // Override from VrmlField
        //
        ostream & print(ostream &) const;
        VrmlField * clone() const;
        VrmlFieldType fieldType() const;
        
    private:
        class DData;
        DData * d_data;
};

# endif
