/*
LibVRML97: a library for loading and viewing VRML
This software is copyright (C) the Open VRML Advancement League
All Rights Reserved
See the file COPYING for license details.
*/

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
