<?php // -*-php-*-
rcs_id('$Id: SqlResult.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
/*
 Copyright 2004 $ThePhpWikiProgrammingTeam
 
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
 * This plugin displays result sets of arbitrary SQL select statements in table form.
 * The database definition, the dsn, must be defined in the local file lib/SqlResult.ini
 * A simple testfile with alias=dsn lines.
 *
 * Usage:
 *   <?plugin SqlResult dsn=myforum
 *            SELECT 'mysql password for string "xx":',
 *                   PASSWORD('xx')
 *   ?>
 *
 * @Author: ReiniUrban
 */

class WikiPlugin_SqlResult
extends WikiPlugin
{
    var $_args;	
    
    function getName () {
        return _("SqlResult");
    }

    function getDescription () {
        return _("Display arbitrary SQL result tables");
    }

    function getVersion() {
        return preg_replace("/[Revision: $]/", '',
                            "\$Revision: 1.1.1.1 $");
    }

    function getDefaultArguments() {
        return array('page'        => '[pagename]',
                     'dsn'         => false,
                    );
    }

    function getDsn($alias) {
    }

    /** Get the SQL statement from the rest of the lines
     */
    function handle_plugin_args_cruft($argstr, $args) {
    	$this->_sql = str_replace("\n"," ",$argstr);
        return;
    }
   
    function run($dbi, $argstr, &$request, $basepage) {
        global $DBParams;
    	//$request->setArg('nocache','1');
        extract($this->getArgs($argstr, $request));
        if (!$dsn)
            return $this->error(_("No dsn specified"));
	$sql = $this->_sql;
        //TODO: handle variables

        //TODO: handle dsn cache
        $dbh = $dbi->_backend->_dbh;
        $all = $dbh->getAll($sql);

        $html = HTML::table(array('class'=>'sqlresult'));
	foreach ($all as $row) {
            $tr = HTML::tr(array('class'=>'sqlresult'));
            foreach ($row as $col) {
                $tr->pushContent(HTML::td(array('class'=>'sqlresult'),$col));
            }
            $html->pushContent($tr);
        }
        return $html;
    }

};

// $Log: not supported by cvs2svn $

// For emacs users
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>