<?php // -*-php-*-

/*
Copyright 1999, 2000, 2001, 2002 $ThePhpWikiProgrammingTeam = array(
"Steve Wainstead", "Clifford A. Adams", "Lawrence Akka", 
"Scott R. Anderson", "Jon Åslund", "Neil Brown", "Jeff Dairiki",
"Stéphane Gourichon", "Jan Hidders", "Arno Hollosi", "John Jorgensen",
"Antti Kaihola", "Jeremie Kass", "Carsten Klapp", "Marco Milanesi",
"Grant Morgan", "Jan Nieuwenhuizen", "Aredridel Niothke", 
"Pablo Roca Rozas", "Sandino Araico Sánchez", "Joel Uckelman", 
"Reini Urban", "Tim Voght", "Joseph (Joby) Walker");

This file is part of PhpWiki.

PhpWiki is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

PhpWiki is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PhpWiki; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

define ('DEBUG', 1);
/////////////////////////////////////////////////////////////////////
// Part Null: Don't touch this!

define ('PHPWIKI_VERSION', '1.3.4pre');
require "lib/prepend.php";
rcs_id('$Id: index2.php,v 1.1.1.1 2004-04-13 23:41:39 braden Exp $');

require "config/config-user.php";
require "config/config-dist.php";

// Tested: Works with CGI also.
if (defined('VIRTUAL_PATH') and defined('USE_PATH_INFO')) {
    if ($HTTP_SERVER_VARS['SCRIPT_NAME'] == VIRTUAL_PATH) {
        include "lib/main.php";
    }
} else {
    if (defined('SCRIPT_NAME') and 
        ($HTTP_SERVER_VARS['SCRIPT_NAME'] == SCRIPT_NAME)) {
        include "lib/main.php";
    } elseif (strstr($HTTP_SERVER_VARS['PHP_SELF'],'index.php')) {
        include "lib/main.php";
    }
}

// (c-file-style: "gnu")
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:   
?>
