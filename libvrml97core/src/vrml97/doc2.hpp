/*
LibVRML97: a library for loading and viewing VRML

The authors make NO WARRANTY or representation, either express or implied, with
respect to this software, its quality, accuracy, merchantability, or fitness for
a particular purpose.  This software is provided "AS IS", and you, its user,
assume the entire risk as to its quality and accuracy.

This software is copyright (C) the Open VRML Advancement League
All Rights Reserved except as specified below.

Permission is hereby granted to use, copy, modify, and distribute this software
(or portions thereof) for any purpose, without fee, subject to these conditions:

 (1) If any part of the source code for this software is distributed, then this
    file must be included, with this copyright and no-warranty notice unaltered;
    and any additions, deletions, or changes to the original files must be
    clearly indicated in accompanying documentation.

 (2) If only executable code is distributed, then the accompanying documentation
    must state that "this software is based in part on the LibVRML97 library".

 (3) Permission for use of this software is granted only if the user accepts
    full responsibility for any undesirable consequences; the authors accept NO
    LIABILITY for damages of any kind.

These conditions apply to any software derived from or based on the LibVRML97
code, not just to the unmodified library.  If you use our work, you ought to
acknowledge us.

We specifically permit and encourage the use of this software as the basis of
commercial products, provided that all warranty or liability claims are assumed
by the product vendor.
*/

# ifndef DOC2_HPP
# define DOC2_HPP

# include <iostream.h>

class Doc2 {
    public:
        
        Doc2(const char *url = 0, Doc2 const *relative = 0);
        Doc2(Doc2 const *);
        ~Doc2();
        
        void seturl(const char *url, Doc2 const *relative = 0);
        
        char const * url() const;         // "http://www.foo.com/dir/file.xyz#Viewpoint"
        char const * urlBase() const;     // "file" or ""
        char const * urlExt() const;      // "xyz" or ""
        char const * urlPath() const;     // "http://www.foo.com/dir/" or ""
        char const * urlProtocol() const; // "http"
        char const * urlModifier() const; // "#Viewpoint" or ""
        
        const char *localName();    // "/tmp/file.xyz" or NULL
        const char *localPath();    // "/tmp/" or NULL
        
        ::istream & inputStream();
        ::ostream & outputStream();
        
    private:
        //
        // Non-copyable; copy ctor and operator= are declared private and not
        // defined.
        //
        Doc2(Doc2 const &);
        Doc2 & operator=(Doc2 const &);
        
        static char const * stripProtocol(char const * url);
        static bool isAbsolute(char const * url);
        
        bool filename(char * fn, int nfn);
        
# ifdef macintosh
        char * convertCommonToMacPath(char * fn, int nfn);
# endif
        
        char * url_;
        char * tmpfile_;            // Local copy of http: files
        istream * istm_;
        ostream * ostm_;
};

# endif
