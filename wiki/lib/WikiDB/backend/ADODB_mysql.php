<?php // -*-php-*-
rcs_id('$Id: ADODB_mysql.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');

require_once('lib/WikiDB/backend/ADODB.php');

/**
 * WikiDB layer for ADODB-mysql, called by lib/WikiDB/ADODB.php.
 * Support for a newer adodb library, the adodb extension library 
 * and more databases will come with PhpWiki v1.3.9
 * 
 * @author: Lawrence Akka
 */
class WikiDB_backend_ADODB_mysql
extends WikiDB_backend_ADODB
{
    /**
     * Constructor.
     */
    function WikiDB_backend_ADODB_mysql($dbparams) {
        $this->WikiDB_backend_ADODB($dbparams);

        // Older MySQL's don't have CASE WHEN ... END
        $this->_expressions['maxmajor'] = "MAX(IF(minor_edit=0,version,0))";
        $this->_expressions['maxminor'] = "MAX(IF(minor_edit<>0,version,0))";
    }
    
    /**
     * Pack tables.
     */
    function optimize() {
        $dbh = &$this->_dbh;
        foreach ($this->_table_names as $table) {
            $dbh->Execute("OPTIMIZE TABLE $table");
        }
        return 1;
    }

    /**
     * Lock tables.
     */
    function _lock_tables($write_lock = true) {
        $lock_type = $write_lock ? "WRITE" : "READ";
        foreach ($this->_table_names as $table) {
            $tables[] = "$table $lock_type";
        }
        $this->_dbh->Execute("LOCK TABLES " . join(",", $tables));
    }

    /**
     * Release all locks.
     */
    function _unlock_tables() {
        $this->_dbh->Execute("UNLOCK TABLES");
    }
};

// (c-file-style: "gnu")
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:   
?>
