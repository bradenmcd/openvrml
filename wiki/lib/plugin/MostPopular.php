<?php // -*-php-*-
rcs_id('$Id: MostPopular.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
/**
 Copyright 1999, 2000, 2001, 2002 $ThePhpWikiProgrammingTeam

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

/**
 */

require_once('lib/PageList.php');

class WikiPlugin_MostPopular
extends WikiPlugin
{
    function getName () {
        return _("MostPopular");
    }

    function getDescription () {
        return _("List the most popular pages.");
    }

    function getVersion() {
        return preg_replace("/[Revision: $]/", '',
                            "\$Revision: 1.1.1.1 $");
    }

    function getDefaultArguments() {
        return array('pagename' => '[pagename]', // hackish
                     'exclude'  => '',
                     'limit'    => 20, // limit <0 returns least popular pages
                     'noheader' => 0,
                     'sortby'   => 'hits',
                     'info'     => false
                    );
    }
    
    // info arg allows multiple columns
    // info=mtime,hits,summary,version,author,locked,minor
    // exclude arg allows multiple pagenames exclude=HomePage,RecentChanges

    function run($dbi, $argstr, &$request, $basepage) {
    	//$request->setArg('nocache','1');
        extract($this->getArgs($argstr, $request));

        $columns = $info ? explode(",", $info) : array();
        array_unshift($columns, 'hits');

        $pagelist = new PageList($columns, $exclude);

        $pages = $dbi->mostPopular($limit,$sortby);
        while ($page = $pages->next()) {
            $hits = $page->get('hits');
            // don't show pages with no hits if most popular pages
            // wanted
            if ($hits == 0 && $limit > 0)
                break;
            $pagelist->addPage($page);
        }
        $pages->free();

        if (! $noheader) {
            if ($limit > 0) {
                $pagelist->setCaption(_("The %d most popular pages of this wiki:"));
            } else {
                if ($limit < 0) {
                    $pagelist->setCaption(_("The %d least popular pages of this wiki:"));
                } else {
                    $pagelist->setCaption(_("Visited pages on this wiki, ordered by popularity:"));
                }}
        }

        return $pagelist;
    }
};

// $Log: not supported by cvs2svn $
// Revision 1.25  2004/03/30 02:38:06  rurban
// RateIt support (currently no recommendation engine yet)
//
// Revision 1.24  2004/03/01 13:48:46  rurban
// rename fix
// p[] consistency fix
//
// Revision 1.23  2004/02/17 12:11:36  rurban
// added missing 4th basepage arg at plugin->run() to almost all plugins. This caused no harm so far, because it was silently dropped on normal usage. However on plugin internal ->run invocations it failed. (InterWikiSearch, IncludeSiteMap, ...)
//
// Revision 1.22  2003/01/18 21:48:56  carstenklapp
// Code cleanup:
// Reformatting & tabs to spaces;
// Added copyleft, getVersion, getDescription, rcs_id.
//

// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>
