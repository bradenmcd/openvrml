<?php //-*-php-*-
rcs_id('$Id: upgrade.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');

/*
 Copyright 2004 $ThePhpWikiProgrammingTeam

 This file is (not yet) part of PhpWiki.

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
 * Upgrade the WikiDB and config settings after installing a new 
 * PhpWiki upgrade.
 * Status: experimental, no queries for verification yet, no db update,
 *         no merge conflict
 * Installation on an existing PhpWiki database needs some 
 * additional worksteps. Each step will require multiple pages.
 *
 * This is the plan:
 *  1. Check for new or changed database schema and update it 
 *     according to some predefined upgrade tables. (medium)
 *  2. Check for new or changed (localized) pgsrc/ pages and ask 
 *     for upgrading these. Check timestamps, upgrade silently or 
 *     show diffs if existing. Overwrite or merge (easy)
 *  3. Check for new or changed or deprecated index.php settings
 *     and help in upgrading these. (hard)
 *  4. Check for changed plugin invocation arguments. (hard)
 *  5. Check for changed theme variables. (hard)
 *
 * @author: Reini Urban
 */
require_once("lib/loadsave.php");

// see loadsave.php for saving new pages.
function CheckPgsrcUpdate(&$request) {
    $dbh = $request->getDbh(); 
    $path = FindLocalizedFile(WIKI_PGSRC);
    $pgsrc = new fileSet($path);
    // fixme: verification, ...
    foreach ($pgsrc->getFiles() as $pagename) {
        $pagename = urldecode($pagename);
        if (substr($pagename,-1,1) == '~') continue;
        if ($dbh->isWikiPage($pagename)) {
            // check mtime
            ; //echo "$pagename exists<br />\n";
        } else {
            echo "$pagename does not exist<br />\n";
            LoadAny($request,$path."/".$pagename);
            echo "<br />\n";
        }
    }
    return;
}

/**
 * search table definition in appropriate schema and 
 * create it.
 * supported: mysql
 */
function installTable(&$dbh, $table) {
    if (!in_array($DBParam['dbtype'],array('SQL','ADODB'))) return;
    $backend = 'mysql'; // fixme
    $schema = findFile("schemas/$backend.sql");
    if (!$schema)
        return false;
    switch ($table) {
    case 'session': 
        break;
    case 'user':
        break;
    case 'pref':
        break;
    case 'members':
        break;
    }
}
/** 
 * currently update only session, user, pref and member
 * jeffs-hacks database api (around 1.3.2) later
 *   people should export/import their pages if using that old versions.
 */
function CheckDatabaseUpdate($request) {
    global $DBParam, $DBAuthParams;
    if (!in_array($DBParam['dbtype'],array('SQL','ADODB'))) return;
    $dbh = &$request->_dbi;
    $backend = &$dbh->_backend->_dbh;
    if ($DBParam['dbtype'] == 'SQL') {
        $tables = $backend->getListOf('tables');
    } elseif ($DBParam['dbtype'] == 'ADODB') {
        $tables = $backend->MetaTables();
    }
    //...
    return;
}

/**
 * Upgrade: Base class for multipage worksteps
 * identify, validate, display options, next step
 */
class Upgrade {
}

class Upgrade_CheckPgsrc extends Upgrade {
}

class Upgrade_CheckDatabaseUpdate extends Upgrade {
}

// TODO: At which step are we? 
// validate and do it again or go on with next step.

/** entry function from lib/main.php
 */
function DoUpgrade($request) {

    if (!$request->_user->isAdmin()) {
        $request->_notAuthorized(WIKIAUTH_ADMIN);
        $request->finish(
                         HTML::div(array('class' => 'disabled-plugin'),
                                   fmt("Upgrade disabled: user != isAdmin")));
        return;
    }

    StartLoadDump($request, _("Upgrading this PhpWiki"));
    CheckDatabaseUpdate($request);
    CheckPgsrcUpdate($request);
    EndLoadDump($request);
}


/**
 $Log: not supported by cvs2svn $
 Revision 1.2  2004/03/12 15:48:07  rurban
 fixed explodePageList: wrong sortby argument order in UnfoldSubpages
 simplified lib/stdlib.php:explodePageList

 */

// For emacs users
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>
