<?php // -*-php-*-
rcs_id('$Id: CategoryPage.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
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

#require_once('lib/InlineParser.php');
require_once('lib/BlockParser.php');

/**
 * CategoryPage plugin.
 *
 * This puts boilerplate text on a category page to make it easily usable
 * by novices.
 *
 * Usage:
 * <?plugin-form CategoryPage ?>
 */
class WikiPlugin_CategoryPage
extends WikiPlugin
{
    function getName () {
        return _("CategoryPage");
    }

    function getDescription () {
        return _("Create a Wiki page.");
    }

    function getVersion() {
        return preg_replace("/[Revision: $]/", '',
                            "\$Revision: 1.1.1.1 $");
    }

    function getDefaultArguments() {
        return array(// Assume the categories are listed on the HomePage
                     'exclude'              => "HomePage",
                     'pagename'             => '[pagename]',
                     'plural'               => false,
                     'singular'             => false);
    }

    function run($dbi, $argstr, $request) {
        $args = $this->getArgs($argstr, $request);

        if (empty($args['singular'])) {
            $args['singular'] = $args['pagename'];
        }
        if (empty($args['plural'])) {
            $args['plural'] = $args['singular'] . 's';
        }

        return new Template('categorypage', $request,
                            array('EXCLUDE' => $args['exclude'],
                                  'PAGENAME' => $args['pagename'],
                                  'PLURAL' => $args['plural'],
                                  'SINGULAR' => $args['singular']));
    }
};

// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>
