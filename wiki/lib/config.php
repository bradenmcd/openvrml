<?php
rcs_id('$Id: config.php,v 1.1.1.1 2004-04-13 23:41:39 braden Exp $');
/*
 * NOTE: the settings here should probably not need to be changed.
*
*
* (The user-configurable settings have been moved to index.php.)
*/

if (!defined("LC_ALL")) {
    // Backward compatibility (for PHP < 4.0.5)
    define("LC_ALL",   0);
    define("LC_CTYPE", 2);
}

function isCGI() {
    return @preg_match('/CGI/',$GLOBALS['HTTP_ENV_VARS']['GATEWAY_INTERFACE']);
}

/*
// copy some $_ENV vars to $_SERVER for CGI compatibility. php does it automatically since when?
if (isCGI()) {
    foreach (explode(':','SERVER_SOFTWARE:SERVER_NAME:GATEWAY_INTERFACE:SERVER_PROTOCOL:SERVER_PORT:REQUEST_METHOD:HTTP_ACCEPT:PATH_INFO:PATH_TRANSLATED:SCRIPT_NAME:QUERY_STRING:REMOTE_HOST:REMOTE_ADDR:REMOTE_USER:AUTH_TYPE:CONTENT_TYPE:CONTENT_LENGTH') as $key) {
        $GLOBALS['HTTP_SERVER_VARS'][$key] = &$GLOBALS['HTTP_ENV_VARS'][$key];
    }
}
*/

// essential internal stuff
set_magic_quotes_runtime(0);

// Some constants.

// "\x80"-"\x9f" (and "\x00" - "\x1f") are non-printing control
// chars in iso-8859-*
// $FieldSeparator = "\263"; //this is a superscript 3 in ISO-8859-1.
// $FieldSeparator = "\xFF"; // this byte should never appear in utf-8
if (strtolower(CHARSET) == 'utf-8')
    $FieldSeparator = "\xFF";
else
    $FieldSeparator = "\x81";

/** 
 * Browser Detection Functions
 *
 * Current Issues:
 *  NS/IE < 4.0 doesn't accept < ? xml version="1.0" ? >
 *  NS/IE < 4.0 cannot display PNG
 *  NS/IE < 4.0 cannot display all XHTML tags
 *  NS < 5.0 needs textarea wrap=virtual
 *  IE55 has problems with transparent PNG's
 * @author: ReiniUrban
 */
function browserAgent() {
    static $HTTP_USER_AGENT = false;
    if (!$HTTP_USER_AGENT)
        $HTTP_USER_AGENT = @$GLOBALS['HTTP_SERVER_VARS']['HTTP_USER_AGENT'];
    if (!$HTTP_USER_AGENT) // CGI
        $HTTP_USER_AGENT = $GLOBALS['HTTP_ENV_VARS']['HTTP_USER_AGENT'];
    return $HTTP_USER_AGENT;
}
function browserDetect($match) {
    return strstr(browserAgent(), $match);
}
// returns a similar number for Netscape/Mozilla (gecko=5.0)/IE/Opera features.
function browserVersion() {
    if (strstr(browserAgent(),"Mozilla/4.0 (compatible; MSIE"))
        return (float) substr(browserAgent(),30);
    else
        return (float) substr(browserAgent(),8);
}
function isBrowserIE() {
    return (browserDetect('Mozilla/') and 
            browserDetect('MSIE'));
}
// problem with transparent PNG's
function isBrowserIE55() {
    return (isBrowserIE() and 
            browserVersion() > 5.1 and browserVersion() < 6.0);
}
function isBrowserNetscape() {
    return (browserDetect('Mozilla/') and 
            ! browserDetect('Gecko/') and
            ! browserDetect('MSIE'));
}
// NS3 or less
function isBrowserNS3() {
    return (isBrowserNetscape() and browserVersion() < 4.0);
}

require_once('lib/FileFinder.php');
// Search PHP's include_path to find file or directory.
function FindFile ($file, $missing_okay = false, $slashify = false)
{
    static $finder;
    if (!isset($finder))
        $finder = new FileFinder;
    $s = $finder->findFile($file, $missing_okay);
    if ($slashify)
      $s = $finder->slashifyPath($s);
    return $s;
}

// Search PHP's include_path to find file or directory.
// Searches for "locale/$LANG/$file", then for "$file".
function FindLocalizedFile ($file, $missing_okay = false, $re_init = false)
{
    static $finder;
    if ($re_init or !isset($finder))
        $finder = new LocalizedFileFinder;
    return $finder->findFile($file, $missing_okay);
}

function FindLocalizedButtonFile ($file, $missing_okay = false, $re_init = false)
{
    static $buttonfinder;
    if ($re_init or !isset($buttonfinder))
        $buttonfinder = new LocalizedButtonFinder;
    return $buttonfinder->findFile($file, $missing_okay);
}


/**
 * Smart? setlocale().
 *
 * This is a version of the builtin setlocale() which is
 * smart enough to try some alternatives...
 *
 * @param mixed $category
 * @param string $locale
 * @return string The new locale, or <code>false</code> if unable
 *  to set the requested locale.
 * @see setlocale
 */
function guessing_setlocale ($category, $locale) {
    if ($res = setlocale($category, $locale))
        return $res;
    $alt = array('en' => array('C', 'en_US', 'en_GB', 'en_AU', 'en_CA', 'english'),
                 'de' => array('de_DE', 'de_DE', 'de_DE@euro', 
                               'de_AT@euro', 'de_AT', 'German_Austria.1252', 'deutsch', 
                               'german', 'ge'),
                 'es' => array('es_ES', 'es_MX', 'es_AR', 'spanish'),
                 'nl' => array('nl_NL', 'dutch'),
                 'fr' => array('fr_FR', 'français', 'french'),
                 'it' => array('it_IT'),
                 'sv' => array('sv_SE'),
                 'ja' => array('ja_JP','ja_JP.eucJP','japanese.euc')
                 );
    if (strlen($locale) == 2)
        $lang = $locale;
    else 
        list ($lang) = split('_', $locale);
    if (!isset($alt[$lang]))
        return false;
        
    foreach ($alt[$lang] as $try) {
        if ($res = setlocale($category, $try))
            return $res;
        // Try with charset appended...
        $try = $try . '.' . CHARSET;
        if ($res = setlocale($category, $try))
            return $res;
        foreach (array('@', ".", '_') as $sep) {
            list ($try) = split($sep, $try);
            if ($res = setlocale($category, $try))
                return $res;
        }
    }
    return false;

    // A standard locale name is typically of  the  form
    // language[_territory][.codeset][@modifier],  where  language is
    // an ISO 639 language code, territory is an ISO 3166 country code,
    // and codeset  is  a  character  set or encoding identifier like
    // ISO-8859-1 or UTF-8.
}

function update_locale($loc) {
    $newlocale = guessing_setlocale(LC_ALL, $loc);
    if (!$newlocale) {
        //trigger_error(sprintf(_("Can't setlocale(LC_ALL,'%s')"), $loc), E_USER_NOTICE);
        // => LC_COLLATE=C;LC_CTYPE=German_Austria.1252;LC_MONETARY=C;LC_NUMERIC=C;LC_TIME=C
        //$loc = setlocale(LC_CTYPE, '');  // pull locale from environment.
        $newlocale = FileFinder::_get_lang();
        list ($newlocale,) = split('_', $newlocale, 2);
        //$GLOBALS['LANG'] = $loc;
        //$newlocale = $loc;
        //return false;
    }
    //if (substr($newlocale,0,2) == $loc) // don't update with C or failing setlocale
    $GLOBALS['LANG'] = $loc;
    // Try to put new locale into environment (so any
    // programs we run will get the right locale.)
    //
    // If PHP is in safe mode, this is not allowed,
    // so hide errors...
    @putenv("LC_ALL=$newlocale");
    @putenv("LANG=$newlocale");
    @putenv("LANGUAGE=$newlocale");
    
    if (!function_exists ('bindtextdomain'))  {
        // Reinitialize translation array.
        global $locale;
        $locale = array();
        // do reinit to purge PHP's static cache
        if ( ($lcfile = FindLocalizedFile("LC_MESSAGES/phpwiki.php", 'missing_ok','reinit')) ) {
            include($lcfile);
        }
    }

    // To get the POSIX character classes in the PCRE's (e.g.
    // [[:upper:]]) to match extended characters (e.g. GrüßGott), we have
    // to set the locale, using setlocale().
    //
    // The problem is which locale to set?  We would like to recognize all
    // upper-case characters in the iso-8859-1 character set as upper-case
    // characters --- not just the ones which are in the current $LANG.
    //
    // As it turns out, at least on my system (Linux/glibc-2.2) as long as
    // you setlocale() to anything but "C" it works fine.  (I'm not sure
    // whether this is how it's supposed to be, or whether this is a bug
    // in the libc...)
    //
    // We don't currently use the locale setting for anything else, so for
    // now, just set the locale to US English.
    //
    // FIXME: Not all environments may support en_US?  We should probably
    // have a list of locales to try.
    if (setlocale(LC_CTYPE, 0) == 'C') {
        $x = setlocale(LC_CTYPE, 'en_US.' . CHARSET );
    } else {
        $x = setlocale(LC_CTYPE, $newlocale);
    }

    return $newlocale;
}

if (!defined('DEFAULT_LANGUAGE'))
     define('DEFAULT_LANGUAGE', 'en');

//
// Set up (possibly fake) gettext()
//
if (!function_exists ('bindtextdomain')) {
    $locale = array();

    function gettext ($text) { 
        global $locale;
        if (!empty ($locale[$text]))
            return $locale[$text];
        return $text;
    }

    function _ ($text) {
        return gettext($text);
    }
}
else {
    // Working around really weird gettext problems: (4.3.2, 4.3.6 win)
    // bindtextdomain() returns the current domain path.
    // 1. If the script is not index.php but something like "de", on a different path
    //    then bindtextdomain() fails, but after chdir to the correct path it will work okay.
    // 2. But the weird error "Undefined variable: bindtextdomain" is generated then.
    $bindtextdomain_path = FindFile("locale", false, true);
    if (isWindows())
        $bindtextdomain_path = str_replace("/","\\",$bindtextdomain_path);
    $bindtextdomain_real = @bindtextdomain("phpwiki", $bindtextdomain);
    if ($bindtextdomain_real != $bindtextdomain_path) {
        // this will happen with virtual_paths. chdir and try again.
        chdir($bindtextdomain_path);
        $bindtextdomain_real = @bindtextdomain("phpwiki", $bindtextdomain);
    }
    textdomain("phpwiki");
    if ($bindtextdomain_real != $bindtextdomain_path) { // change back
        chdir($bindtextdomain_real . (isWindows() ? "\\.." : "/.."));
    }
}

/** string pcre_fix_posix_classes (string $regexp)
*
* Older version (pre 3.x?) of the PCRE library do not support
* POSIX named character classes (e.g. [[:alnum:]]).
*
* This is a helper function which can be used to convert a regexp
* which contains POSIX named character classes to one that doesn't.
*
* All instances of strings like '[:<class>:]' are replaced by the equivalent
* enumerated character class.
*
* Implementation Notes:
*
* Currently we use hard-coded values which are valid only for
* ISO-8859-1.  Also, currently on the classes [:alpha:], [:alnum:],
* [:upper:] and [:lower:] are implemented.  (The missing classes:
* [:blank:], [:cntrl:], [:digit:], [:graph:], [:print:], [:punct:],
* [:space:], and [:xdigit:] could easily be added if needed.)
*
* This is a hack.  I tried to generate these classes automatically
* using ereg(), but discovered that in my PHP, at least, ereg() is
* slightly broken w.r.t. POSIX character classes.  (It includes
* "\xaa" and "\xba" in [:alpha:].)
*
* So for now, this will do.  --Jeff <dairiki@dairiki.org> 14 Mar, 2001
*/
function pcre_fix_posix_classes ($regexp) {
    // First check to see if our PCRE lib supports POSIX character
    // classes.  If it does, there's nothing to do.
    if (preg_match('/[[:upper:]]/', 'Ä'))
        return $regexp;

    static $classes = array(
                            'alnum' => "0-9A-Za-z\xc0-\xd6\xd8-\xf6\xf8-\xff",
                            'alpha' => "A-Za-z\xc0-\xd6\xd8-\xf6\xf8-\xff",
                            'upper' => "A-Z\xc0-\xd6\xd8-\xde",
                            'lower' => "a-z\xdf-\xf6\xf8-\xff"
                            );

    $keys = join('|', array_keys($classes));

    return preg_replace("/\[:($keys):]/e", '$classes["\1"]', $regexp);
}

$WikiNameRegexp = pcre_fix_posix_classes($WikiNameRegexp);
$KeywordLinkRegexp = pcre_fix_posix_classes($KeywordLinkRegexp);

//////////////////////////////////////////////////////////////////
// Autodetect URL settings:
//
if (!defined('SERVER_NAME')) define('SERVER_NAME', $HTTP_SERVER_VARS['SERVER_NAME']);
if (!defined('SERVER_PORT')) define('SERVER_PORT', $HTTP_SERVER_VARS['SERVER_PORT']);
if (!defined('SERVER_PROTOCOL')) {
    if (empty($HTTP_SERVER_VARS['HTTPS']) || $HTTP_SERVER_VARS['HTTPS'] == 'off')
        define('SERVER_PROTOCOL', 'http');
    else
        define('SERVER_PROTOCOL', 'https');
}

function deduce_script_name() {
    $s = &$GLOBALS['HTTP_SERVER_VARS'];
    $script = @$s['SCRIPT_NAME'];
    if (empty($script) or $script[0] != '/') {
        // Some places (e.g. Lycos) only supply a relative name in
        // SCRIPT_NAME, but give what we really want in SCRIPT_URL.
        if (!empty($s['SCRIPT_URL']))
            $script = $s['SCRIPT_URL'];
    }
    return $script;
}

if (!defined('SCRIPT_NAME'))
    define('SCRIPT_NAME', deduce_script_name());

if (!defined('USE_PATH_INFO'))
{
    /*
     * If SCRIPT_NAME does not look like php source file,
     * or user cgi we assume that php is getting run by an
     * action handler in /cgi-bin.  In this case,
     * I think there is no way to get Apache to pass
     * useful PATH_INFO to the php script (PATH_INFO
     * is used to the the php interpreter where the
     * php script is...)
     */
    switch (php_sapi_name()) {
    case 'apache':
    case 'apache2handler':
        define('USE_PATH_INFO', true);
        break;
    case 'cgi':
    case 'apache2filter':
        define('USE_PATH_INFO', false);
        break;
    default:
        define('USE_PATH_INFO', ereg('\.(php3?|cgi)$', SCRIPT_NAME));
        break;
    }
}
     
// If user has not defined DATA_PATH, we want to use relative URLs.
if (!defined('DATA_PATH') && USE_PATH_INFO)
     define('DATA_PATH', '..');

function IsProbablyRedirectToIndex () {
    // This might be a redirect to the DirectoryIndex,
    // e.g. REQUEST_URI = /dir/  got redirected
    // to SCRIPT_NAME = /dir/index.php

    // In this case, the proper virtual path is still
    // $SCRIPT_NAME, since pages appear at
    // e.g. /dir/index.php/HomePage.

    $requri = preg_quote($GLOBALS['HTTP_SERVER_VARS']['REQUEST_URI'], '%');
    return preg_match("%^${requri}[^/]*$%", $GLOBALS['HTTP_SERVER_VARS']['SCRIPT_NAME']);
}


if (!defined('VIRTUAL_PATH')) {
    // We'd like to auto-detect when the cases where apaches
    // 'Action' directive (or similar means) is used to
    // redirect page requests to a cgi-handler.
    //
    // In cases like this, requests for e.g. /wiki/HomePage
    // get redirected to a cgi-script called, say,
    // /path/to/wiki/index.php.  The script gets all
    // of /wiki/HomePage as it's PATH_INFO.
    //
    // The problem is:
    //   How to detect when this has happened reliably?
    //   How to pick out the "virtual path" (in this case '/wiki')?
    //
    // (Another time an redirect might occur is to a DirectoryIndex
    // -- the requested URI is '/wikidir/', the request gets
    // passed to '/wikidir/index.php'.  In this case, the
    // proper VIRTUAL_PATH is '/wikidir/index.php', since the
    // pages will appear at e.g. '/wikidir/index.php/HomePage'.
    //

    $REDIRECT_URL = &$HTTP_SERVER_VARS['REDIRECT_URL'];
    if (USE_PATH_INFO and isset($REDIRECT_URL)
        and ! IsProbablyRedirectToIndex()) {
        // FIXME: This is a hack, and won't work if the requested
        // pagename has a slash in it.
        define('VIRTUAL_PATH', dirname($REDIRECT_URL . 'x'));
    } else {
        define('VIRTUAL_PATH', SCRIPT_NAME);
    }
}

if (SERVER_PORT
    && SERVER_PORT != (SERVER_PROTOCOL == 'https' ? 443 : 80)) {
    define('SERVER_URL',
           SERVER_PROTOCOL . '://' . SERVER_NAME . ':' . SERVER_PORT);
}
else {
    define('SERVER_URL',
           SERVER_PROTOCOL . '://' . SERVER_NAME);
}

if (VIRTUAL_PATH != SCRIPT_NAME) {
    // Apache action handlers are used.
    define('PATH_INFO_PREFIX', VIRTUAL_PATH . '/');
}
else
    define('PATH_INFO_PREFIX', '/');


define('PHPWIKI_BASE_URL',
       SERVER_URL . (USE_PATH_INFO ? VIRTUAL_PATH . '/' : SCRIPT_NAME));

//////////////////////////////////////////////////////////////////
// Select database
//
if (empty($DBParams['dbtype']))
    $DBParams['dbtype'] = 'dba';

if (!defined('THEME'))
    define('THEME', 'default');

update_locale(isset($LANG) ? $LANG : DEFAULT_LANGUAGE);

if (!defined('WIKI_NAME'))
    define('WIKI_NAME', _("An unnamed PhpWiki"));

if (!defined('HOME_PAGE'))
    define('HOME_PAGE', _("HomePage"));

// FIXME: delete
// Access log
if (!defined('ACCESS_LOG'))
     define('ACCESS_LOG', '');

// FIXME: delete
// Get remote host name, if apache hasn't done it for us
if (empty($HTTP_SERVER_VARS['REMOTE_HOST']) && ENABLE_REVERSE_DNS)
     $HTTP_SERVER_VARS['REMOTE_HOST'] = gethostbyaddr($HTTP_SERVER_VARS['REMOTE_ADDR']);

// check whether the crypt() function is needed and present
if (defined('ENCRYPTED_PASSWD') && !function_exists('crypt')) {
    $error = sprintf(_("Encrypted passwords cannot be used: %s."),
                     "'function crypt()' not available in this version of php");
    trigger_error($error);
}

if (!defined('ADMIN_PASSWD') or ADMIN_PASSWD == '')
    trigger_error(_("The admin password cannot be empty. Please update your /index.php"));

if (defined('USE_DB_SESSION') and USE_DB_SESSION) {
    if (! $DBParams['db_session_table'] ) {
        trigger_error(_("Empty db_session_table. Turn USE_DB_SESSION off or define the table name."), 
                          E_USER_ERROR);
        // this is flawed. constants cannot be changed.
        define('USE_DB_SESSION',false);
    }
} else {
    // default: true (since v1.3.8)
    if (!defined('USE_DB_SESSION'))
        define('USE_DB_SESSION',true);
}
// legacy:
if (!defined('ENABLE_USER_NEW')) define('ENABLE_USER_NEW',true);
if (!defined('ALLOW_USER_LOGIN'))
    define('ALLOW_USER_LOGIN', defined('ALLOW_USER_PASSWORDS') && ALLOW_USER_PASSWORDS);
if (!defined('ALLOW_ANON_USER')) define('ALLOW_ANON_USER', true); 
if (!defined('ALLOW_ANON_EDIT')) define('ALLOW_ANON_EDIT', false); 
if (!defined('REQUIRE_SIGNIN_BEFORE_EDIT')) define('REQUIRE_SIGNIN_BEFORE_EDIT', ! ALLOW_ANON_EDIT);
if (!defined('ALLOW_BOGO_LOGIN')) define('ALLOW_BOGO_LOGIN', true);

if (ALLOW_USER_LOGIN and !empty($DBAuthParams) and empty($DBAuthParams['auth_dsn'])) {
    if (isset($DBParams['dsn']))
        $DBAuthParams['auth_dsn'] = $DBParams['dsn'];
}

// For emacs users
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>
