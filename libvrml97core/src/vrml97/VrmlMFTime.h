/*
LibVRML97: a library for loading and viewing VRML
This software is copyright (C) the Open VRML Advancement League
All Rights Reserved
See the file COPYING for license details.
*/

# ifndef VRMLMFTIME_H
# define VRMLMFTIME_H

# include "VrmlField.h"
# include <cstddef>

class VrmlMFTime : public VrmlField {
    public:
        VrmlMFTime();
        VrmlMFTime(double);
        VrmlMFTime(std::size_t, double const *);
        VrmlMFTime(VrmlMFTime const &);
        
        ~VrmlMFTime();
        
        VrmlMFTime & operator=(VrmlMFTime const &);
        
        void set(std::size_t, double const *);
        double const * get() const;
        double const & operator[](std::size_t) const;
        std::size_t size() const;
        
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
