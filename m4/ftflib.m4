dnl Autoconf macro to check for system font files and libfreetype
dnl Copyright (C) 2002  S. K. Bose
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

dnl
dnl OV_CHECK_FONTS
dnl
AC_DEFUN(OV_CHECK_FONTS,
  [
dnl
dnl Check for at least default Serif family and plain style font
dnl
   if test "X${SERIFPLAIN}" = "X"; then
    SERIFPLAIN="/usr/lib/X11/fonts/Type1/lcdxrr.pfa"
   fi
   AC_MSG_CHECKING([SerifPlain font])
   AC_CHECK_FILE($SERIFPLAIN,
    [
     have_font=yes
     AC_DEFINE_UNQUOTED(SERIFPLAIN,["$SERIFPLAIN"],[Serif Plain Font File])
dnl
dnl Check for Serif family and bold style
dnl
     if test "X${SERIFBOLD}" = "X"; then
      SERIFBOLD="/usr/lib/X11/fonts/Type1/lcdxrb.pfa"
     fi
     AC_MSG_CHECKING([SerifBold font])
     AC_CHECK_FILE($SERIFBOLD,
      [
       AC_DEFINE_UNQUOTED(SERIFBOLD,["$SERIFBOLD"],[Serif Bold Font File]) 
      ],
      [
       AC_DEFINE_UNQUOTED(SERIFBOLD,["$SERIFPLAIN"],[Serif Bold Font File])
       AC_MSG_WARN([Set it to SerifPlain font, Check SEIFBOLD variable])
      ]
     )
dnl
dnl Check for Serif family and Italic style
dnl
     if test "X${SERIFITALIC}" = "X"; then
      SERIFITALIC="/usr/lib/X11/fonts/Type1/lcdxri.pfa"
     fi
     AC_MSG_CHECKING([SerifItalic font])
     AC_CHECK_FILE($SERIFITALIC,
      [
       AC_DEFINE_UNQUOTED(SERIFITALIC,["$SERIFITALIC"],[Serif Italic Font File]) 
      ],
      [
       AC_DEFINE_UNQUOTED(SERIFITALIC,["$SERIFPLAIN"],[Serif Italic Font File])
       AC_MSG_WARN([Set it to SerifPlain font, Check SEIFITALIC variable])
      ]
     )
dnl
dnl Check for Serif family and BoldItalic style
dnl
     if test "X${SERIFBOLDITALIC}" = "X"; then
      SERIFBOLDITALIC="/usr/lib/X11/fonts/Type1/lcdxrbi.pfa"
     fi
     AC_MSG_CHECKING([SerifBoldItalic font])
     AC_CHECK_FILE($SERIFBOLDITALIC,
      [
       AC_DEFINE_UNQUOTED(SERIFBOLDITALIC,["$SERIFBOLDITALIC"],[Serif BoldItalic Font File]) 
      ],
      [
       AC_DEFINE_UNQUOTED(SERIFBOLDITALIC,["$SERIFPLAIN"],[Serif BoldItalic Font File])
       AC_MSG_WARN([Set it to SerifPlain font, Check SEIFBOLDITALIC variable])
      ]
     )
dnl
dnl Check for at least sans family and plain style font if serif plain is found
dnl
     if test "X${SANSPLAIN}" = "X"; then
      SANSPLAIN="/usr/lib/X11/fonts/Type1/lcdxsr.pfa"
     fi
     AC_MSG_CHECKING([SansPlain font])
     AC_CHECK_FILE($SANSPLAIN,
      [
       have_font=yes
       AC_DEFINE_UNQUOTED(SANSPLAIN,["$SANSPLAIN"],[Sans Plain Font File])
dnl
dnl Check for Sans family and bold style
dnl
       if test "X${SANSBOLD}" = "X"; then
        SANSBOLD="/usr/lib/X11/fonts/Type1/lcdxsb.pfa"
       fi
       AC_MSG_CHECKING([SansBold font])
       AC_CHECK_FILE($SANSBOLD,
        [
         AC_DEFINE_UNQUOTED(SANSBOLD,["$SANSBOLD"],[Sans Bold Font File]) 
        ],
        [
         AC_DEFINE_UNQUOTED(SANSBOLD,["$SANSPLAIN"],[Sans Bold Font File])
         AC_MSG_WARN([Set it to SansPlain font, Check SANSBOLD variable])
        ]
       )
dnl
dnl Check for Sans family and Italic style
dnl
       if test "X${SANSITALIC}" = "X"; then
        SANSITALIC="/usr/lib/X11/fonts/Type1/lcdxsi.pfa"
       fi
       AC_MSG_CHECKING([SansItalic font])
       AC_CHECK_FILE($SANSITALIC,
        [
         AC_DEFINE_UNQUOTED(SANSITALIC,["$SANSITALIC"],[Sans Italic Font File]) 
        ],
        [
         AC_DEFINE_UNQUOTED(SANSITALIC,["$SANSPLAIN"],[Sans Italic Font File])
         AC_MSG_WARN([Set it to SansPlain font, Check SANSITALIC variable])
        ]
       )
dnl
dnl Check for Sans family and BoldItalic style
dnl
       if test "X${SANSBOLDITALIC}" = "X"; then
        SANSBOLDITALIC="/usr/lib/X11/fonts/Type1/lcdxsbi.pfa"
       fi
       AC_MSG_CHECKING([SansBoldItalic font])
       AC_CHECK_FILE($SANSBOLDITALIC,
        [
         AC_DEFINE_UNQUOTED(SANSBOLDITALIC,["$SANSBOLDITALIC"],[Sans BoldItalic Font File]) 
        ],
        [
         AC_DEFINE_UNQUOTED(SANSBOLDITALIC,["$SANSPLAIN"],[Sans BoldItalic Font File])
         AC_MSG_WARN([Set it to SansPlain font, Check SANSBOLDITALIC variable])
        ]
       )
dnl
dnl Check for at least Typewriter family and plain style font if serif plain
dnl and sans plain is found
dnl
       if test "X${TYPEWRITERPLAIN}" = "X"; then
        TYPEWRITERPLAIN="/usr/lib/X11/fonts/Type1/cour.pfa"
       fi
       AC_MSG_CHECKING([TypewriterPlain font])
       AC_CHECK_FILE($TYPEWRITERPLAIN,
        [
         have_font=yes
         AC_DEFINE_UNQUOTED(TYPEWRITERPLAIN,["$TYPEWRITERPLAIN"],[Typewriter Plain Font File])
dnl
dnl Check for Typewriter family and bold style
dnl
        if test "X${TYPEWRITERBOLD}" = "X"; then
         TYPEWRITERBOLD="/usr/lib/X11/fonts/Type1/courb.pfa"
        fi
         AC_MSG_CHECKING([TyepwriterBold font])
         AC_CHECK_FILE($TYPEWRITERBOLD,
         [
           AC_DEFINE_UNQUOTED(TYPEWRITERBOLD,["$TYPEWRITERBOLD"],[Tyepwriter Bold Font File]) 
          ],
          [
           AC_DEFINE_UNQUOTED(TYPEWRITERBOLD,["$TYPEWRITERPLAIN"],[Tyepwriter Bold Font File])
           AC_MSG_WARN([Set it to TyepwriterPlain font, Check TYPEWRITERBOLD variable])
          ]
         )
dnl
dnl Check for Tyepwriter family and Italic style
dnl
         if test "X${TYPEWRITERITALIC}" = "X"; then
          TYPEWRITERITALIC="/usr/lib/X11/fonts/Type1/couri.pfa"
         fi
         AC_MSG_CHECKING([TyepwriterItalic font])
         AC_CHECK_FILE($TYPEWRITERITALIC,
          [
           AC_DEFINE_UNQUOTED(TYPEWRITERITALIC,["$TYPEWRITERITALIC"],[Tyepwriter Italic Font File]) 
          ],
          [
           AC_DEFINE_UNQUOTED(ITALIC,["$TYPEWRITERPLAIN"],[Tyepwriter Italic Font File])
           AC_MSG_WARN([Sei it to TyepwriterPlain, Check TYPEWRITERITALIC variable])
          ]
         )
dnl
dnl Check for Tyepwriter family and BoldItalic style
dnl
         if test "X${TYPEWRITERBOLDITALIC}" = "X"; then
          TYPEWRITERBOLDITALIC="/usr/lib/X11/fonts/Type1/courbi.pfa"
         fi
         AC_MSG_CHECKING([TyepwriterBoldItalic font])
         AC_CHECK_FILE($TYPEWRITERBOLDITALIC,
          [
           AC_DEFINE_UNQUOTED(TYPEWRITERBOLDITALIC,["$TYPEWRITERBOLDITALIC"],[Tyepwriter BoldItalic Font File]) 
          ],
          [
           AC_DEFINE_UNQUOTED(TYPEWRITERBOLDITALIC,["$TYPEWRITERPLAIN"],[Tyepwriter BoldItalic Font File])
           AC_MSG_WARN([Set it to TyepwriterPlain, Check TYPEWRITERBOLDITALIC variable])
          ]
         )
        ],
        [
        have_font=no
        AC_MSG_WARN([Check TYPEWRITERPLAIN variable])
        ]
       ) 
      ],
      [
      have_font=no
      AC_MSG_WARN([Check SANSPLAIN variable])
      ]
     ) 
    ],
    [
    have_font=no
    AC_MSG_WARN([Check SEIFPLAIN variable])
    ]
   ) 
  ]
)

dnl
dnl OV_CHECK_FTFLIB
dnl
AC_DEFUN(OV_CHECK_FTFLIB,
  [
    FTF_LIBS="-lfreetype"
    
    AC_LANG_PUSH(C)
    
    ov_save_LIBS="${LIBS}"
    LIBS="${FTF_LIBS} ${LIBS}"
    have_ftflib=no
    AC_CHECK_LIB(freetype, FT_Init_FreeType, [AC_CHECK_HEADER(ft2build.h, [have_ftflib=yes])])
    LIBS="${ov_save_LIBS}"
    
    AC_LANG_POP(C)
    
    if test "X${have_ftflib}" = Xyes; then
      no_ftflib=""
    else
      no_ftflib="yes"
      FTF_LIBS=""
    fi
    
    have_ftflib="" dnl Clear temporary variable.
    
    AC_SUBST(FTF_LIBS)
  ]
)
