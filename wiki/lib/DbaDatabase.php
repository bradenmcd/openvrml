<?php rcs_id('$Id: DbaDatabase.php,v 1.1.1.1 2004-04-13 23:41:39 braden Exp $');

require_once('lib/ErrorManager.php');
// FIXME: autodetect supported handlers.

define('DBA_DATABASE_DEFAULT_TIMEOUT', 20);

class DbaDatabase
{
    function DbaDatabase($filename, $mode = false, $handler = 'gdbm') {
        $this->_file = $filename;
        $this->_handler = $handler;
        $this->_timeout = DBA_DATABASE_DEFAULT_TIMEOUT;
        $this->_dbh = false;
        if ($mode)
            $this->open($mode);
    }

    function set_timeout($timeout) {
        $this->_timeout = $timeout;
    }
    
    function open($mode = 'w') {
        if ($this->_dbh)
            return;             // already open.
        
        $watchdog = $this->_timeout;

        global $ErrorManager;
        $this->_dba_open_error = false;
        $ErrorManager->pushErrorHandler(new WikiMethodCb($this, '_dba_open_error_handler'));

        // oops, you don't have DBM support.
        if (!function_exists("dba_open")) {
            echo "You don't seem to have DBM file support compiled into PHP.";
        }

        // PHP 4.3.x Windows lock bug workaround: http://bugs.php.net/bug.php?id=23975
        if (isWindows() and (strlen($mode) == 1)) {
            $mode .= "-"; // suppress locking
        }
        while (($dbh = dba_open($this->_file, $mode, $this->_handler)) < 1) {
            if (--$watchdog <= 0)
                break;
            flush();
            sleep(1);
        }
        $ErrorManager->popErrorHandler();

        if (!$dbh) {
            if ( ($error = $this->_dba_open_error) ) {
                $error->errno = E_USER_ERROR;
                $ErrorManager->handleError($error);
            }
            else {
                trigger_error("dba_open failed", E_USER_ERROR);
            }
        }
        $this->_dbh = $dbh;
        return !empty($dbh);
    }

    function close() {
        if ($this->_dbh)
            dba_close($this->_dbh);
        $this->_dbh = false;
    }

    function exists($key) {
        return dba_exists($key, $this->_dbh);
    }
    
    function fetch($key) {
        $val = dba_fetch($key, $this->_dbh);
        if ($val === false)
            return $this->_error("fetch($key)");
        return $val;
    }

    function insert($key, $val) {
        if (!dba_insert($key, $val, $this->_dbh))
            return $this->_error("insert($key)");
    }

    function replace($key, $val) {
        if (!dba_replace($key, $val, $this->_dbh))
            return $this->_error("replace($key)");
    }

    
    function firstkey() {
        return dba_firstkey($this->_dbh);
    }

    function nextkey() {
        return dba_nextkey($this->_dbh);
    }

    function delete($key) {
        if (!dba_delete($key, $this->_dbh))
            return $this->_error("delete($key)");
    }

    function get($key) {
        return dba_fetch($key, $this->_dbh);
    }

    function set($key, $val) {
        $dbh = &$this->_dbh;
        if (dba_exists($key, $dbh)) {
            if ($val !== false) {
                if (!dba_replace($key, $val, $dbh))
                    return $this->_error("store[replace]($key)");
            }
            else {
                if (!dba_delete($key, $dbh))
                    return $this->_error("store[delete]($key)");
            }
        }
        else {
            if (!dba_insert($key, $val, $this->_dbh))
                return $this->_error("store[insert]($key)");
        }
    }

    function sync() {
        if (!dba_sync($this->_dbh))
            return $this->_error("sync()");
    }

    function optimize() {
        if (!dba_optimize($this->_dbh))
            return $this->_error("optimize()");
        return 1;
    }
    
    function _error($mes) {
        trigger_error("DbaDatabase: $mes", E_USER_WARNING);
        return false;
        
        trigger_error("$this->_file: dba error: $mes", E_USER_ERROR);
    }

    function _dump() {
        $dbh = &$this->_dbh;
        for ($key = $this->firstkey($dbh); $key; $key = $this->nextkey($dbh))
            printf("%10s: %s\n", $key, $this->fetch($key));
    }

    function _dba_open_error_handler ($error) {
        $this->_dba_open_error = $error;
        return true;
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
