<?php //-*-php-*-
rcs_id('$Id: WikiUserNew.php.php5,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
/* Copyright (C) 2004 $ThePhpWikiProgrammingTeam
 */
/**
 * This is a complete OOP rewrite of the old WikiUser code with various
 * configurable external authentication methods.
 *
 * There's only one entry point, the function WikiUser which returns 
 * a WikiUser object, which contains the user's preferences.
 * This object might get upgraded during the login step and later also.
 * There exist three preferences storage methods: cookie, homepage and db,
 * and multiple password checking methods.
 * See index.php for $USER_AUTH_ORDER[] and USER_AUTH_POLICY if 
 * ALLOW_USER_PASSWORDS is defined.
 *
 * Each user object must define the two preferences methods 
 *  getPreferences(), setPreferences(), 
 * and the following 1-4 auth methods
 *  checkPass()  must be defined by all classes,
 *  userExists() only if USER_AUTH_POLICY'=='strict' 
 *  mayChangePass()  only if the password is storable.
 *  storePass()  only if the password is storable.
 *
 * WikiUser() given no name, returns an _AnonUser (anonymous user)
 * object, who may or may not have a cookie. 
 * However, if the there's a cookie with the userid or a session, 
 * the user is upgraded to the matching user object.
 * Given a user name, returns a _BogoUser object, who may or may not 
 * have a cookie and/or PersonalPage, one of the various _PassUser objects 
 * or an _AdminUser object.
 * BTW: A BogoUser is a userid (loginname) as valid WikiWord, who might 
 * have stored a password or not. If so, his account is secure, if not
 * anybody can use it, because the username is visible e.g. in RecentChanges.
 *
 * Takes care of passwords, all preference loading/storing in the
 * user's page and any cookies. lib/main.php will query the user object to
 * verify the password as appropriate.
 *
 * @author: Reini Urban (the tricky parts), 
 *          Carsten Klapp (started rolling the ball)
 *
 * Notes by 2004-01-25 03:43:45 rurban
 * Test it by defining ENABLE_USER_NEW in index.php
 * 1) Now a ForbiddenUser is returned instead of false.
 * 2) Previously ALLOW_ANON_USER = false meant that anon users cannot edit, 
 *    but may browse. Now with ALLOW_ANON_USER = false he may not browse, 
 *    which is needed to disable browse PagePermissions.
 *    I added now ALLOW_ANON_EDIT = true to makes things clear. 
 *    (which replaces REQUIRE_SIGNIN_BEFORE_EDIT)
 * 2004-02-27 rurban:
 * 3) Removed pear prepare. Performance hog, and using integers as 
 *    handler doesn't help. Do simple sprintf as with adodb. And a prepare
 *    in the object init is no advantage, because in the init loop a lot of 
 *    objects are tried, but not used.
 * 4) Already gotten prefs are passed to the next object to avoid 
 *    duplicate getPreferences() calls.
 * 2004-03-18 17:32:21 rurban
 * 5) Major php-5 problem: $this re-assignment is disallowed by the parser
 *    So we cannot just discrimate with 
 *      if (!check_php_version(5))
 *          $this = $user;
 * 2004-03-24 rurban
 * 6) enforced new cookie policy: prefs don't get stored in cookies
 *    anymore, only in homepage and/or database, but always in the 
 *    current session. old pref cookies will get deleted.
 */

define('WIKIAUTH_FORBIDDEN', -1); // Completely not allowed.
define('WIKIAUTH_ANON', 0);       // Not signed in.
define('WIKIAUTH_BOGO', 1);       // Any valid WikiWord is enough.
define('WIKIAUTH_USER', 2);       // Bogo user with a password.
define('WIKIAUTH_ADMIN', 10);     // UserName == ADMIN_USER.

if (!defined('COOKIE_EXPIRATION_DAYS')) define('COOKIE_EXPIRATION_DAYS', 365);
if (!defined('COOKIE_DOMAIN'))          define('COOKIE_DOMAIN', '/');

if (!defined('EDITWIDTH_MIN_COLS'))     define('EDITWIDTH_MIN_COLS',     30);
if (!defined('EDITWIDTH_MAX_COLS'))     define('EDITWIDTH_MAX_COLS',    150);
if (!defined('EDITWIDTH_DEFAULT_COLS')) define('EDITWIDTH_DEFAULT_COLS', 80);

if (!defined('EDITHEIGHT_MIN_ROWS'))     define('EDITHEIGHT_MIN_ROWS',      5);
if (!defined('EDITHEIGHT_MAX_ROWS'))     define('EDITHEIGHT_MAX_ROWS',     80);
if (!defined('EDITHEIGHT_DEFAULT_ROWS')) define('EDITHEIGHT_DEFAULT_ROWS', 22);

define('TIMEOFFSET_MIN_HOURS', -26);
define('TIMEOFFSET_MAX_HOURS',  26);
if (!defined('TIMEOFFSET_DEFAULT_HOURS')) define('TIMEOFFSET_DEFAULT_HOURS', 0);

/**
 * There are be the following constants in index.php to 
 * establish login parameters:
 *
 * ALLOW_ANON_USER         default true
 * ALLOW_ANON_EDIT         default true
 * ALLOW_BOGO_LOGIN        default true
 * ALLOW_USER_PASSWORDS    default true
 * PASSWORD_LENGTH_MINIMUM default 6 ?
 *
 * To require user passwords for editing:
 * ALLOW_ANON_USER  = true
 * ALLOW_ANON_EDIT  = false   (before named REQUIRE_SIGNIN_BEFORE_EDIT)
 * ALLOW_BOGO_LOGIN = false
 * ALLOW_USER_PASSWORDS = true
 *
 * To establish a COMPLETELY private wiki, such as an internal
 * corporate one:
 * ALLOW_ANON_USER = false
 * (and probably require user passwords as described above). In this
 * case the user will be prompted to login immediately upon accessing
 * any page.
 *
 * There are other possible combinations, but the typical wiki (such
 * as http://PhpWiki.sf.net/phpwiki) would usually just leave all four 
 * enabled.
 *
 */

// The last object in the row is the bad guy...
if (!is_array($USER_AUTH_ORDER))
    $USER_AUTH_ORDER = array("Forbidden");
else
    $USER_AUTH_ORDER[] = "Forbidden";

// Local convenience functions.
function _isAnonUserAllowed() {
    return (defined('ALLOW_ANON_USER') && ALLOW_ANON_USER);
}
function _isBogoUserAllowed() {
    return (defined('ALLOW_BOGO_LOGIN') && ALLOW_BOGO_LOGIN);
}
function _isUserPasswordsAllowed() {
    return (defined('ALLOW_USER_PASSWORDS') && ALLOW_USER_PASSWORDS);
}

// Possibly upgrade userobject functions.
function _determineAdminUserOrOtherUser($UserName) {
    // Sanity check. User name is a condition of the definition of the
    // _AdminUser, _BogoUser and _passuser.
    if (!$UserName)
        return $GLOBALS['ForbiddenUser'];

    if ($UserName == ADMIN_USER)
        return new _AdminUser($UserName);
    else
        return _determineBogoUserOrPassUser($UserName);
}

function _determineBogoUserOrPassUser($UserName) {
    global $ForbiddenUser;

    // Sanity check. User name is a condition of the definition of
    // _BogoUser and _PassUser.
    if (!$UserName)
        return $ForbiddenUser;

    // Check for password and possibly upgrade user object.
    // $_BogoUser = new _BogoUser($UserName);
    if (_isBogoUserAllowed()) {
        $_BogoUser = new _BogoLoginPassUser($UserName);
        if ($_BogoUser->userExists())
            return $_BogoUser;
    }
    if (_isUserPasswordsAllowed()) {
    	// PassUsers override BogoUsers if a password is stored
        if (isset($_BogoUser) and $_BogoUser->_prefs->get('passwd'))
            return new _PassUser($UserName,$_BogoUser->_prefs);
        else { 
            $_PassUser = new _PassUser($UserName,isset($_BogoUser) ? $_BogoUser->_prefs : false);
            if ($_PassUser->userExists())
                return $_PassUser;
        }
    }
    // No Bogo- or PassUser exists, or
    // passwords are not allowed, and bogo is disallowed too.
    // (Only the admin can sign in).
    return $ForbiddenUser;
}

/**
 * Primary WikiUser function, called by lib/main.php.
 * 
 * This determines the user's type and returns an appropriate user
 * object. lib/main.php then querys the resultant object for password
 * validity as necessary.
 *
 * If an _AnonUser object is returned, the user may only browse pages
 * (and save prefs in a cookie).
 *
 * To disable access but provide prefs the global $ForbiddenUser class 
 * is returned. (was previously false)
 * 
 */
function WikiUser ($UserName = '') {
    global $ForbiddenUser;

    //Maybe: Check sessionvar for username & save username into
    //sessionvar (may be more appropriate to do this in lib/main.php).
    if ($UserName) {
        $ForbiddenUser = new _ForbiddenUser($UserName);
        // Found a user name.
        return _determineAdminUserOrOtherUser($UserName);
    }
    elseif (!empty($_SESSION['userid'])) {
        // Found a user name.
        $ForbiddenUser = new _ForbiddenUser($_SESSION['userid']);
        return _determineAdminUserOrOtherUser($_SESSION['userid']);
    }
    else {
        // Check for autologin pref in cookie and possibly upgrade
        // user object to another type.
        $_AnonUser = new _AnonUser();
        if ($UserName = $_AnonUser->_userid && $_AnonUser->_prefs->get('autologin')) {
            // Found a user name.
            $ForbiddenUser = new _ForbiddenUser($UserName);
            return _determineAdminUserOrOtherUser($UserName);
        }
        else {
            $ForbiddenUser = new _ForbiddenUser();
            if (_isAnonUserAllowed())
                return $_AnonUser;
            return $ForbiddenUser; // User must sign in to browse pages.
        }
        return $ForbiddenUser;     // User must sign in with a password.
    }
    /*
    trigger_error("DEBUG: Note: End of function reached in WikiUser." . " "
                  . "Unexpectedly, an appropriate user class could not be determined.");
    return $ForbiddenUser; // Failsafe.
    */
}

/**
 * WikiUser.php use the name 'WikiUser'
 */
function WikiUserClassname() {
    return '_WikiUser';
}


/**
 * Upgrade olduser by copying properties from user to olduser.
 * We are not sure yet, for which php's a simple $this = $user works reliably,
 * (on php4 it works ok, on php5 it's currently disallowed on the parser level)
 * that's why try it the hard way.
 */
function UpgradeUser ($olduser, $user) {
    if (isa($user,'_WikiUser') and isa($olduser,'_WikiUser')) {
        // populate the upgraded class $olduser with the values from the new user object
        //only _auth_level, _current_method, _current_index,
        if (!empty($user->_level) and 
            $user->_level > $olduser->_level)
            $olduser->_level = $user->_level;
        if (!empty($user->_current_index) and
            $user->_current_index > $olduser->_current_index) {
            $olduser->_current_index = $user->_current_index;
            $olduser->_current_method = $user->_current_method;
        }
        if (!empty($user->_authmethod))
            $olduser->_authmethod = $user->_authmethod;
        /*
        foreach (get_object_vars($user) as $k => $v) {
            if (!empty($v)) $olduser->$k = $v;	
        }
        */
        $olduser->hasHomePage(); // revive db handle, because these don't survive sessions
        $GLOBALS['request']->_user = $olduser;
        return $olduser;
    } else {
        return false;
    }
}

/**
 * Probably not needed, since we use the various user objects methods so far.
 * Anyway, here it is, looping through all available objects.
 */
function UserExists ($UserName) {
    global $request;
    if (!($user = $request->getUser()))
        $user = WikiUser($UserName);
    if (!$user) 
        return false;
    if ($user->userExists($UserName)) {
        $request->_user = $user;
        return true;
    }
    if (isa($user,'_BogoUser'))
        $user = new _PassUser($UserName,$user->_prefs);
    $class = $user->nextClass();
    if ($user = new $class($UserName,$user->_prefs)) {
        return $user->userExists($UserName);
    }
    $request->_user = $GLOBALS['ForbiddenUser'];
    return false;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** 
 * Base WikiUser class.
 */
class _WikiUser
{
     var $_userid = '';
     var $_level = WIKIAUTH_FORBIDDEN;
     var $_prefs = false;
     var $_HomePagehandle = false;

    // constructor
    function _WikiUser($UserName='', $prefs=false) {

        $this->_userid = $UserName;
        $this->_HomePagehandle = false;
        if ($UserName) {
            $this->hasHomePage();
        }
        $this->_level = WIKIAUTH_FORBIDDEN;
        if (empty($this->_prefs)) {
            if ($prefs) $this->_prefs = $prefs;
            else $this->getPreferences();
        }
    }

    function UserName() {
        if (!empty($this->_userid))
            return $this->_userid;
    }

    function getPreferences() {
        trigger_error("DEBUG: Note: undefined _WikiUser class trying to load prefs." . " "
                      . "New subclasses of _WikiUser must override this function.");
        return false;
    }

    function setPreferences($prefs, $id_only) {
        trigger_error("DEBUG: Note: undefined _WikiUser class trying to save prefs." . " "
                      . "New subclasses of _WikiUser must override this function.");
        return false;
    }

    function userExists() {
        return $this->hasHomePage();
    }

    function checkPass($submitted_password) {
        // By definition, an undefined user class cannot sign in.
        trigger_error("DEBUG: Warning: undefined _WikiUser class trying to sign in." . " "
                      . "New subclasses of _WikiUser must override this function.");
        return false;
    }

    // returns page_handle to user's home page or false if none
    function hasHomePage() {
        if ($this->_userid) {
            if (!empty($this->_HomePagehandle) and is_object($this->_HomePagehandle)) {
                return $this->_HomePagehandle->exists();
            }
            else {
                // check db again (maybe someone else created it since
                // we logged in.)
                global $request;
                $this->_HomePagehandle = $request->getPage($this->_userid);
                return $this->_HomePagehandle->exists();
            }
        }
        // nope
        return false;
    }

    // innocent helper: case-insensitive position in _auth_methods
    function array_position ($string, $array) {
        $string = strtolower($string);
        for ($found = 0; $found < count($array); $found++) {
            if (strtolower($array[$found]) == $string)
                return $found;
        }
        return false;
    }

    function nextAuthMethodIndex() {
        if (empty($this->_auth_methods)) 
            $this->_auth_methods = $GLOBALS['USER_AUTH_ORDER'];
        if (empty($this->_current_index)) {
            if (get_class($this) != '_passuser') {
            	$this->_current_method = substr(get_class($this),1,-8);
                $this->_current_index = $this->array_position($this->_current_method,
                                                              $this->_auth_methods);
            } else {
            	$this->_current_index = -1;
            }
        }
        $this->_current_index++;
        if ($this->_current_index >= count($this->_auth_methods))
            return false;
        $this->_current_method = $this->_auth_methods[$this->_current_index];
        return $this->_current_index;
    }

    function AuthMethod($index = false) {
        return $this->_auth_methods[ $index === false ? 0 : $index];
    }

    // upgrade the user object
    function nextClass() {
        if (($next = $this->nextAuthMethodIndex()) !== false) {
            $method = $this->AuthMethod($next);
            return "_".$method."PassUser";
            /*          
            if ($user = new $class($this->_userid)) {
                // prevent from endless recursion.
                //$user->_current_method = $this->_current_method;
                //$user->_current_index = $this->_current_index;
                $user = UpgradeUser($user, $this);
            }
            return $user;
            */
        }
        return "_ForbiddenPassUser";
    }

    //Fixme: for _HttpAuthPassUser
    function PrintLoginForm (&$request, $args, $fail_message = false,
                             $seperate_page = true) {
        include_once('lib/Template.php');
        // Call update_locale in case the system's default language is not 'en'.
        // (We have no user pref for lang at this point yet, no one is logged in.)
        update_locale(DEFAULT_LANGUAGE);
        $userid = $this->_userid;
        $require_level = 0;
        extract($args); // fixme

        $require_level = max(0, min(WIKIAUTH_ADMIN, (int)$require_level));

        $pagename = $request->getArg('pagename');
        $nocache = 1;
        $login = new Template('login', $request,
                              compact('pagename', 'userid', 'require_level',
                                      'fail_message', 'pass_required', 'nocache'));
        if ($seperate_page) {
            $top = new Template('html', $request,
                                array('TITLE' => _("Sign In")));
            return $top->printExpansion($login);
        } else {
            return $login;
        }
    }

    /** Signed in but probably not password checked.
     */
    function isSignedIn() {
        return (isa($this,'_BogoUser') or isa($this,'_PassUser'));
    }

    /** This is password checked for sure.
     */
    function isAuthenticated () {
        //return isa($this,'_PassUser');
        //return isa($this,'_BogoUser') || isa($this,'_PassUser');
        return $this->_level >= WIKIAUTH_BOGO; // hmm.
    }

    function isAdmin () {
        return $this->_level == WIKIAUTH_ADMIN;
    }

    /** Name or IP for a signed user. UserName could come from a cookie e.g.
     */
    function getId () {
        return ( $this->UserName()
                 ? $this->UserName()
                 : $GLOBALS['request']->get('REMOTE_ADDR') ); // FIXME: globals
    }

    /** Name for an authenticated user. No IP here.
     */
    function getAuthenticatedId() {
        return ( $this->isAuthenticated()
                 ? $this->_userid
                 : ''); //$GLOBALS['request']->get('REMOTE_ADDR') ); // FIXME: globals
    }

    function hasAuthority ($require_level) {
        return $this->_level >= $require_level;
    }

    /**
     * Called on an auth_args POST request, such as login, logout or signin.
     */
    function AuthCheck ($postargs) {
        // Normalize args, and extract.
        $keys = array('userid', 'passwd', 'require_level', 'login', 'logout',
                      'cancel');
        foreach ($keys as $key)
            $args[$key] = isset($postargs[$key]) ? $postargs[$key] : false;
        extract($args);
        $require_level = max(0, min(WIKIAUTH_ADMIN, (int)$require_level));

        if ($logout) { // Log out
            $GLOBALS['request']->_user = new _AnonUser();
            return $GLOBALS['request']->_user; 
        } elseif ($cancel)
            return false;        // User hit cancel button.
        elseif (!$login && !$userid)
            return false;       // Nothing to do?

        $authlevel = $this->checkPass($passwd === false ? '' : $passwd);
        if (!$authlevel)
            return _("Invalid password or userid.");
        elseif ($authlevel < $require_level)
            return _("Insufficient permissions.");

        // Successful login.
        $user = $GLOBALS['request']->_user;
        $user->_userid = $userid;
        $user->_level = $authlevel;
        return $user;
    }

}

/**
 * Not authenticated in user, but he may be signed in. Basicly with view access only.
 * prefs are stored in cookies, but only the userid.
 */
class _AnonUser
extends _WikiUser
{
    var $_level = WIKIAUTH_ANON; 	// var in php-5.0.0RC1 deprecated

    /** Anon only gets to load and save prefs in a cookie, that's it.
     */
    function getPreferences() {
        global $request;

        if (empty($this->_prefs))
            $this->_prefs = new UserPreferences;
        $UserName = $this->UserName();

        // Try to read deprecated 1.3.x style cookies
        if ($cookie = $request->cookies->get_old(WIKI_NAME)) {
            if (! $unboxedcookie = $this->_prefs->retrieve($cookie)) {
                trigger_error(_("Format of UserPreferences cookie not recognised.") . " "
                              . _("Default preferences will be used."),
                              E_USER_WARNING);
            }
            /**
             * Only set if it matches the UserName who is
             * signing in or if this really is an Anon login (no
             * username). (Remember, _BogoUser and higher inherit this
             * function too!).
             */
            if (! $UserName || $UserName == $unboxedcookie['userid']) {
                $updated = $this->_prefs->updatePrefs($unboxedcookie);
                //$this->_prefs = new UserPreferences($unboxedcookie);
                $this->_userid = $unboxedcookie['userid'];
                $UserName = $this->_userid;
            }
            // v1.3.8 policy: don't set PhpWiki cookies, only plaintext WIKI_ID cookies
            $request->deleteCookieVar(WIKI_NAME);
        }
        // Try to read deprecated 1.3.4 style cookies
        if (! $UserName and ($cookie = $request->cookies->get_old("WIKI_PREF2"))) {
            if (! $unboxedcookie = $this->_prefs->retrieve($cookie)) {
                if (! $UserName || $UserName == $unboxedcookie['userid']) {
                    $updated = $this->_prefs->updatePrefs($unboxedcookie);
                    //$this->_prefs = new UserPreferences($unboxedcookie);
                    $this->_userid = $unboxedcookie['userid'];
                    $UserName = $this->_userid;
                }
                $request->deleteCookieVar("WIKI_PREF2");
            }
        }
        if (! $UserName ) {
            // Try reading userid from old PhpWiki cookie formats:
            if ($cookie = $request->cookies->get_old('WIKI_ID')) {
                if (is_string($cookie))
                    $this->_userid = $cookie;
                elseif (is_array($cookie) and !empty($cookie['userid']))
                    $this->_userid = $cookie['userid'];
            }
            $UserName = $this->_userid;
        }

        // initializeTheme() needs at least an empty object
        /*
         if (empty($this->_prefs))
            $this->_prefs = new UserPreferences;
        */
        return $this->_prefs;
    }

    /** _AnonUser::setPreferences(): Save prefs in a cookie and session and update all global vars
     *
     * Allow for multiple wikis in same domain. Encode only the
     * _prefs array of the UserPreference object. Ideally the
     * prefs array should just be imploded into a single string or
     * something so it is completely human readable by the end
     * user. In that case stricter error checking will be needed
     * when loading the cookie.
     */
    function setPreferences($prefs, $id_only=false) {
        if (!is_object($prefs)) {
            if (is_object($this->_prefs)) {
                $updated = $this->_prefs->updatePrefs($prefs);
                $prefs =& $this->_prefs;
            } else {
                // update the prefs values from scratch. This could leed to unnecessary
                // side-effects: duplicate emailVerified, ...
                $prefs = new UserPreferences($prefs);
                // check if different from the current prefs
                $updated = $this->_prefs->isChanged($prefs);
            }
        } else {
            $updated = $this->_prefs->isChanged($prefs);
        }
        if ($updated) {
            if ($id_only) {
                global $request;
                // new 1.3.8 policy: no array cookies, only plain userid string as in 
                // the pre 1.3.x versions.
                // prefs should be stored besides the session in the homepagehandle or in a db.
                $request->setCookieVar('WIKI_ID', $this->_userid,
                                       COOKIE_EXPIRATION_DAYS, COOKIE_DOMAIN);
                //$request->setCookieVar(WIKI_NAME, array('userid' => $prefs->get('userid')),
                //                       COOKIE_EXPIRATION_DAYS, COOKIE_DOMAIN);
            }
        }
        $packed = $prefs->store();
        $unpacked = $prefs->unpack($packed);
        if (count($unpacked)) {
            global $request;
            foreach (array('_method','_select','_update') as $param) {
            	if (!empty($this->_prefs->{$param}))
            	    $prefs->{$param} = $this->_prefs->{$param};
            }
            $this->_prefs = $prefs;
            $request->_prefs =& $this->_prefs; 
            $request->_user->_prefs =& $this->_prefs;
            if (isset($request->_user->_auth_dbi)) {
            	$user = $request->_user;
            	unset($user->_auth_dbi);
            	$request->setSessionVar('wiki_user', $user);
            } else {
              //$request->setSessionVar('wiki_prefs', $this->_prefs);
              $request->setSessionVar('wiki_user', $request->_user);
            }
        }
        return $updated;
    }

    function userExists() {
        return true;
    }

    function checkPass($submitted_password) {
        return false;
        // this might happen on a old-style signin button.

        // By definition, the _AnonUser does not HAVE a password
        // (compared to _BogoUser, who has an EMPTY password).
        trigger_error("DEBUG: Warning: _AnonUser unexpectedly asked to checkPass()." . " "
                      . "Check isa(\$user, '_PassUser'), or: isa(\$user, '_AdminUser') etc. first." . " "
                      . "New subclasses of _WikiUser must override this function.");
        return false;
    }

}

/** 
 * Helper class to finish the PassUser auth loop. 
 * This is added automatically to USER_AUTH_ORDER.
 */
class _ForbiddenUser
extends _AnonUser
{
    var $_level = WIKIAUTH_FORBIDDEN;

    function checkPass($submitted_password) {
        return WIKIAUTH_FORBIDDEN;
    }

    function userExists() {
        if ($this->_HomePagehandle) return true;
        return false;
    }
}
/** 
 * The PassUser name gets created automatically. 
 * That's why this class is empty, but must exist.
 */
class _ForbiddenPassUser
extends _ForbiddenUser
{
    function dummy() {
        return;
    }
}

/**
 * Do NOT extend _BogoUser to other classes, for checkPass()
 * security. (In case of defects in code logic of the new class!)
 * The intermediate step between anon and passuser.
 * We also have the _BogoLoginPassUser class with stricter 
 * password checking, which fits into the auth loop.
 * Note: This class is not called anymore by WikiUser()
 */
class _BogoUser
extends _AnonUser
{
    function userExists() {
        if (isWikiWord($this->_userid)) {
            $this->_level = WIKIAUTH_BOGO;
            return true;
        } else {
            $this->_level = WIKIAUTH_ANON;
            return false;
        }
    }

    function checkPass($submitted_password) {
        // By definition, BogoUser has an empty password.
        $this->userExists();
        return $this->_level;
    }
}

class _PassUser
extends _AnonUser
/**
 * Called if ALLOW_USER_PASSWORDS and Anon and Bogo failed.
 *
 * The classes for all subsequent auth methods extend from this class. 
 * This handles the auth method type dispatcher according $USER_AUTH_ORDER, 
 * the three auth method policies first-only, strict and stacked
 * and the two methods for prefs: homepage or database, 
 * if $DBAuthParams['pref_select'] is defined.
 *
 * Default is PersonalPage auth and prefs.
 * 
 * TODO: email verification
 *
 * @author: Reini Urban
 * @tables: pref
 */
{
    var $_auth_dbi, $_prefs;
    var $_current_method, $_current_index;

    // check and prepare the auth and pref methods only once
    function _PassUser($UserName='', $prefs=false) {
        global $DBAuthParams, $DBParams;
        if ($UserName) {
            $this->_userid = $UserName;
            if ($this->hasHomePage())
                $this->_HomePagehandle = $GLOBALS['request']->getPage($this->_userid);
        }
        $this->_authmethod = substr(get_class($this),1,-8);
        if ($this->_authmethod == 'a') $this->_authmethod = 'admin';
        if (! $this->_prefs) {
            if ($prefs) $this->_prefs = $prefs;
            else $this->getPreferences();
        }

        // Check the configured Prefs methods
        if ( !isset($this->_prefs->_select) and !empty($DBAuthParams['pref_select']) 
             and in_array($DBParams['dbtype'],array('SQL','ADODB'))) {
            $this->_prefs->_method = $DBParams['dbtype'];
            $this->getAuthDbh();
            // preparate the SELECT statement
            $this->_prefs->_select = str_replace('"$userid"','%s',$DBAuthParams['pref_select']);
        //} else {
        //    unset($this->_prefs->_select);
        } else {
            $this->_prefs->_method = 'HomePage';
        }
        if (  !isset($this->_prefs->_update) and !empty($DBAuthParams['pref_update'])
             and in_array($DBParams['dbtype'],array('SQL','ADODB'))) {
            $this->_prefs->_method = $DBParams['dbtype'];
            $this->getAuthDbh();
            // preparate the SELECT statement
            $this->_prefs->_update = str_replace(array('"$userid"','"$pref_blob"'),
                                                 array('%s','%s'),
                                             $DBAuthParams['pref_update']);
        }
        
        // Upgrade to the next parent _PassUser class. Avoid recursion.
        if ( strtolower(get_class($this)) === '_passuser' ) {
            //auth policy: Check the order of the configured auth methods
            // 1. first-only: Upgrade the class here in the constructor
            // 2. old:       ignore USER_AUTH_ORDER and try to use all available methods as 
            ///              in the previous PhpWiki releases (slow)
            // 3. strict:    upgrade the class after checking the user existance in userExists()
            // 4. stacked:   upgrade the class after the password verification in checkPass()
            // Methods: PersonalPage, HttpAuth, DB, Ldap, Imap, File
            if (!defined('USER_AUTH_POLICY')) define('USER_AUTH_POLICY','old');
            if (defined('USER_AUTH_POLICY')) {
                // policy 1: only pre-define one method for all users
                if (USER_AUTH_POLICY === 'first-only') {
                    $class = $user->nextClass();
                    return new $class($UserName,$user->_prefs);
                }
                // use the default behaviour from the previous versions:
                elseif (USER_AUTH_POLICY === 'old') {
                    // default: try to be smart
                    // On php5 we can directly return and upgrade the Object,
                    // before we have to upgrade it manually.
                    if (!empty($GLOBALS['PHP_AUTH_USER'])) {
                        if (check_php_version(5))
                            return new _HttpAuthPassUser($UserName,$this->_prefs);
                        else {
                            $user = new _HttpAuthPassUser($UserName,$this->_prefs);
                            //todo: with php5 comment the following line.
                            // /*PHP5 patch*/$this = $user;
                            return UpgradeUser($user,$this);
                        }
                    } elseif (!empty($DBAuthParams['auth_check']) and 
                              (!empty($DBAuthParams['auth_dsn']) or !empty($GLOBALS ['DBParams']['dsn']))) {
                        if (check_php_version(5))
                            return new _DbPassUser($UserName,$this->_prefs);
                        else {
                            $user = new _DbPassUser($UserName,$this->_prefs);
                            //todo: with php5 comment the following line.
                            // /*PHP5 patch*/$this = $user;
                            return UpgradeUser($user,$this);
                        }
                    } elseif (defined('LDAP_AUTH_HOST') and defined('LDAP_BASE_DN') and function_exists('ldap_open')) {
                        if (check_php_version(5))
                            return new _LDAPPassUser($UserName,$this->_prefs);
                        else {
                            $user = new _LDAPPassUser($UserName,$this->_prefs);
                            //todo: with php5 comment the following line.
                            // /*PHP5 patch*/$this = $user;
                            return UpgradeUser($user,$this);
                        }
                    } elseif (defined('IMAP_AUTH_HOST') and function_exists('imap_open')) {
                        if (check_php_version(5))
                            return new _IMAPPassUser($UserName,$this->_prefs);
                        else {
                            $user = new _IMAPPassUser($UserName,$this->_prefs);
                            //todo: with php5 comment the following line.
                            // /*PHP5 patch*/$this = $user;
                            return UpgradeUser($user,$this);
                        }
                    } elseif (defined('AUTH_USER_FILE')) {
                        if (check_php_version(5))
                            return new _FilePassUser($UserName,$this->_prefs);
                        else {
                            $user = new _FilePassUser($UserName,$this->_prefs);
                            //todo: with php5 comment the following line.
                            // /*PHP5 patch*/$this = $user;
                            return UpgradeUser($user,$this);
                        }
                    } else {
                        if (check_php_version(5))
                            return new _PersonalPagePassUser($UserName,$this->_prefs);
                        else {
                            $user = new _PersonalPagePassUser($UserName,$this->_prefs);
                            //todo: with php5 comment the following line.
                            // /*PHP5 patch*/$this = $user;
                            return UpgradeUser($user,$this);
                        }
                    }
                }
                else 
                    // else use the page methods defined in _PassUser.
                    return $this;
            }
        }
    }

    function getAuthDbh () {
        global $request, $DBParams, $DBAuthParams;

        // session restauration doesn't re-connect to the database automatically, so dirty it here.
        if (($DBParams['dbtype'] == 'SQL') and isset($this->_auth_dbi) and 
             empty($this->_auth_dbi->connection))
            unset($this->_auth_dbi);
        if (($DBParams['dbtype'] == 'ADODB') and isset($this->_auth_dbi) and 
             empty($this->_auth_dbi->_connectionID))
            unset($this->_auth_dbi);

        if (empty($this->_auth_dbi)) {
            if ($DBParams['dbtype'] != 'SQL' and $DBParams['dbtype'] != 'ADODB')
                return false;
            if (empty($DBAuthParams['auth_dsn'])) {
                $dbh = $request->getDbh(); // use phpwiki database 
            } elseif ($DBAuthParams['auth_dsn'] == $DBParams['dsn']) {
                $dbh = $request->getDbh(); // same phpwiki database 
            } else { // use another external database handle. needs PHP >= 4.1
                $dbh = WikiDB::open($DBAuthParams);
            }       
            $this->_auth_dbi =& $dbh->_backend->_dbh;    
        }
        return $this->_auth_dbi;
    }

    // not used anymore. have to do the prefix fixing somewhere else
    function prepare ($stmt, $variables) {
        global $DBParams, $request;
        // preparate the SELECT statement, for ADODB and PearDB (MDB not)
        $this->getAuthDbh();
        $place = ($DBParams['dbtype'] == 'ADODB') ? '%s' : '?';
        if (is_array($variables)) {
            $new = array();
            foreach ($variables as $v) { $new[] = $place; }
        } else {
            $new = $place;
        }
        // probably prefix table names if in same database
        if (!empty($DBParams['prefix']) and 
            $this->_auth_dbi === $request->_dbi->_backend->_dbh) {
            if (!stristr($DBParams['prefix'],$stmt)) {
                //Do it automatically for the lazy admin? Esp. on sf.net it's nice to have
                trigger_error("TODO: Need to prefix the DBAuthParam tablename in index.php: $stmt",
                              E_USER_WARNING);
                $stmt = str_replace(array(" user "," pref "," member "),
                                    array(" ".$prefix."user ",
                                          " ".$prefix."prefs ",
                                          " ".$prefix."member "),$stmt);
            }
        }
        return $this->_auth_dbi->prepare(str_replace($variables,$new,$stmt));
    }

    function getPreferences() {
        if (!empty($this->_prefs->_method)) {
            if ($this->_prefs->_method == 'ADODB') {
                _AdoDbPassUser::_AdoDbPassUser($this->_userid,$this->_prefs);
                return _AdoDbPassUser::getPreferences();
            } elseif ($this->_prefs->_method == 'SQL') {
                _PearDbPassUser::_PearDbPassUser($this->_userid,$this->_prefs);
                return _PearDbPassUser::getPreferences();
            }
        }

        // We don't necessarily have to read the cookie first. Since
        // the user has a password, the prefs stored in the homepage
        // cannot be arbitrarily altered by other Bogo users.
        _AnonUser::getPreferences();
        // User may have deleted cookie, retrieve from his
        // PersonalPage if there is one.
        if ($this->_HomePagehandle) {
            if ($restored_from_page = $this->_prefs->retrieve($this->_HomePagehandle->get('pref'))) {
                $updated = $this->_prefs->updatePrefs($restored_from_page);
                //$this->_prefs = new UserPreferences($restored_from_page);
                return $this->_prefs;
            }
        }
        return $this->_prefs;
    }

    function setPreferences($prefs, $id_only=false) {
        if (!empty($this->_prefs->_method)) {
            if ($this->_prefs->_method == 'ADODB') {
                _AdoDbPassUser::_AdoDbPassUser($this->_userid,$prefs);
                return _AdoDbPassUser::setPreferences($prefs, $id_only);
            }
            elseif ($this->_prefs->_method == 'SQL') {
                _PearDbPassUser::_PearDbPassUser($this->_userid,$prefs);
                return _PearDbPassUser::setPreferences($prefs, $id_only);
            }
        }
        if (_AnonUser::setPreferences($prefs, $id_only)) {
            // Encode only the _prefs array of the UserPreference object
            if ($this->_HomePagehandle and !$id_only)
                $this->_HomePagehandle->set('pref', $prefs->store());
        }
        return;
    }

    function mayChangePass() {
        return true;
    }

    //The default method is getting the password from prefs. 
    // child methods obtain $stored_password from external auth.
    function userExists() {
        //if ($this->_HomePagehandle) return true;
        $class = $this->nextClass();
        while ($user = new $class($this->_userid,$this->_prefs)) {
            //todo: with php5 comment the following line:
            // /*PHP5 patch*/$this = $user;
            UpgradeUser($this,$user);
            if ($user->userExists()) {
                return true;
            }
            // prevent endless loop. does this work on all PHP's?
            // it just has to set the classname, what it correctly does.
            $class = $user->nextClass();
            if ($class == "_ForbiddenPassUser")
                return false;
        }
        return false;
    }

    //The default method is getting the password from prefs. 
    // child methods obtain $stored_password from external auth.
    function checkPass($submitted_password) {
        $stored_password = $this->_prefs->get('passwd');
        if ($this->_checkPass($submitted_password, $stored_password)) {
            $this->_level = WIKIAUTH_USER;
            return $this->_level;
        } else {
            return $this->_tryNextPass($submitted_password);
        }
    }

    /**
     * The basic password checker for all PassUser objects.
     * Uses global ENCRYPTED_PASSWD and PASSWORD_LENGTH_MINIMUM.
     * Empty passwords are always false!
     * PASSWORD_LENGTH_MINIMUM is enforced here and in the preference set method.
     * @see UserPreferences::set
     *
     * DBPassUser password's have their own crypt definition.
     * That's why DBPassUser::checkPass() doesn't call this method, if 
     * the db password method is 'plain', which means that the DB SQL 
     * statement just returns 1 or 0. To use CRYPT() or PASSWORD() and 
     * don't store plain passwords in the DB.
     * 
     * TODO: remove crypt() function check from config.php:396 ??
     */
    function _checkPass($submitted_password, $stored_password) {
        if(!empty($submitted_password)) {
            if (strlen($stored_password) < PASSWORD_LENGTH_MINIMUM) {
                // Todo. hmm...
                trigger_error(_("The length of the stored password is shorter than the system policy allows. Sorry, you cannot login.\n You have to ask the System Administrator to reset your password."));
                return false;
            }
            if (strlen($submitted_password) < PASSWORD_LENGTH_MINIMUM)
                return false;
            if (defined('ENCRYPTED_PASSWD') && ENCRYPTED_PASSWD) {
                // Verify against encrypted password.
                if (function_exists('crypt')) {
                    if (crypt($submitted_password, $stored_password) == $stored_password )
                        return true; // matches encrypted password
                    else
                        return false;
                }
                else {
                    trigger_error(_("The crypt function is not available in this version of PHP.") . " "
                                  . _("Please set ENCRYPTED_PASSWD to false in index.php and probably change ADMIN_PASSWD."),
                                  E_USER_WARNING);
                    return false;
                }
            }
            else {
                // Verify against cleartext password.
                if ($submitted_password == $stored_password)
                    return true;
                else {
                    // Check whether we forgot to enable ENCRYPTED_PASSWD
                    if (function_exists('crypt')) {
                        if (crypt($submitted_password, $stored_password) == $stored_password) {
                            trigger_error(_("Please set ENCRYPTED_PASSWD to true in index.php."),
                                          E_USER_WARNING);
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    /** The default method is storing the password in prefs. 
     *  Child methods (DB,File) may store in external auth also, but this 
     *  must be explicitly enabled.
     *  This may be called by plugin/UserPreferences or by ->SetPreferences()
     */
    function changePass($submitted_password) {
        $stored_password = $this->_prefs->get('passwd');
        // check if authenticated
        if ($this->isAuthenticated() and $stored_password != $submitted_password) {
            $this->_prefs->set('passwd',$submitted_password);
            //update the storage (session, homepage, ...)
            $this->SetPreferences($this->_prefs);
            return true;
        }
        //Todo: return an error msg to the caller what failed? 
        // same password or no privilege
        return false;
    }

    function _tryNextPass($submitted_password) {
        if (USER_AUTH_POLICY === 'strict') {
        	$class = $this->nextClass();
            if ($user = new $class($this->_userid,$this->_prefs)) {
                if ($user->userExists()) {
                    return $user->checkPass($submitted_password);
                }
            }
        }
        if (USER_AUTH_POLICY === 'stacked' or USER_AUTH_POLICY === 'old') {
        	$class = $this->nextClass();
            if ($user = new $class($this->_userid,$this->_prefs))
                return $user->checkPass($submitted_password);
        }
        return $this->_level;
    }

    function _tryNextUser() {
        if (USER_AUTH_POLICY === 'strict') {
        	$class = $this->nextClass();
            while ($user = new $class($this->_userid,$this->_prefs)) {
                //todo: with php5 comment the following line:
                // /*PHP5 patch*/$this = $user;
                $user = UpgradeUser($this, $user);
                if ($user->userExists()) {
                    return true;
                }
                $class = $this->nextClass();
            }
        }
        return false;
    }

}

/** Without stored password. A _BogoLoginPassUser with password 
 *  is automatically upgraded to a PersonalPagePassUser.
 */
class _BogoLoginPassUser
extends _PassUser
{
    var $_authmethod = 'BogoLogin';
    function userExists() {
        if (isWikiWord($this->_userid)) {
            $this->_level = WIKIAUTH_BOGO;
            return true;
        } else {
            $this->_level = WIKIAUTH_ANON;
            return false;
        }
    }

    /** A BogoLoginUser requires no password at all
     *  But if there's one stored, we should prefer PersonalPage instead
     */
    function checkPass($submitted_password) {
        if ($this->_prefs->get('passwd')) {
            $user = new _PersonalPagePassUser($this->_userid);
            if ($user->checkPass($submitted_password)) {
                //todo: with php5 comment the following line:
                // /*PHP5 patch*/$this = $user;
                $user = UpgradeUser($this, $user);
                $this->_level = WIKIAUTH_USER;
                return $this->_level;
            } else {
                $this->_level = WIKIAUTH_ANON;
                return $this->_level;
            }
        }
        $this->userExists();
        return $this->_level;
    }
}


/**
 * This class is only to simplify the auth method dispatcher.
 * It inherits almost all all methods from _PassUser.
 */
class _PersonalPagePassUser
extends _PassUser
{
    var $_authmethod = 'PersonalPage';

    function userExists() {
        return $this->_HomePagehandle and $this->_HomePagehandle->exists();
    }
        
    /** A PersonalPagePassUser requires PASSWORD_LENGTH_MINIMUM.
     *  BUT if the user already has a homepage with an empty password 
     *  stored, allow login but warn him to change it.
     */
    function checkPass($submitted_password) {
        if ($this->userExists()) {
            $stored_password = $this->_prefs->get('passwd');
            if (empty($stored_password)) {
                trigger_error(sprintf(
                _("\nYou stored an empty password in your '%s' page.\n").
                _("Your access permissions are only for a BogoUser.\n").
                _("Please set your password in UserPreferences."),
                                        $this->_userid), E_USER_NOTICE);
                $this->_level = WIKIAUTH_BOGO;
                return $this->_level;
            }
            if ($this->_checkPass($submitted_password, $stored_password))
                return ($this->_level = WIKIAUTH_USER);
            return _PassUser::checkPass($submitted_password);
        }
        return WIKIAUTH_ANON;
    }
}

/**
 * We have two possibilities here.
 * 1) The webserver location is already HTTP protected (usually Basic). Then just 
 *    use the username and do nothing
 * 2) The webserver location is not protected, so we enforce basic HTTP Protection
 *    by sending a 401 error and let the client display the login dialog.
 *    This makes only sense if HttpAuth is the last method in USER_AUTH_ORDER,
 *    since the other methods cannot be transparently called after this enforced 
 *    external dialog.
 *    Try the available auth methods (most likely Bogo) and sent this header back.
 *    header('Authorization: Basic '.base64_encode("$userid:$passwd")."\r\n";
 */
class _HttpAuthPassUser
extends _PassUser
{
    function _HttpAuthPassUser($UserName='',$prefs=false) {
        if (!$this->_prefs) {
            if ($prefs) $this->_prefs = $prefs;
            else _PassUser::_PassUser($UserName);
        }
        $this->_authmethod = 'HttpAuth';
        if ($this->userExists())
            return $this;
        else 
            return $GLOBALS['ForbiddenUser'];
    }

    //force http auth authorization
    function userExists() {
        // todo: older php's
        if (empty($_SERVER['PHP_AUTH_USER']) or 
            $_SERVER['PHP_AUTH_USER'] != $this->_userid) {
            header('WWW-Authenticate: Basic realm="'.WIKI_NAME.'"');
            header('HTTP/1.0 401 Unauthorized'); 
            exit;
        }
        $this->_userid = $_SERVER['PHP_AUTH_USER'];
        $this->_level = WIKIAUTH_USER;
        return $this;
    }
        
    function checkPass($submitted_password) {
        return $this->userExists() ? WIKIAUTH_USER : WIKIAUTH_ANON;
    }

    function mayChangePass() {
        return false;
    }

    // hmm... either the server dialog or our own.
    function PrintLoginForm (&$request, $args, $fail_message = false,
                             $seperate_page = true) {
        header('WWW-Authenticate: Basic realm="'.WIKI_NAME.'"');
        header('HTTP/1.0 401 Unauthorized'); 
        exit;

        include_once('lib/Template.php');
        // Call update_locale in case the system's default language is not 'en'.
        // (We have no user pref for lang at this point yet, no one is logged in.)
        update_locale(DEFAULT_LANGUAGE);
        $userid = $this->_userid;
        $require_level = 0;
        extract($args); // fixme

        $require_level = max(0, min(WIKIAUTH_ADMIN, (int)$require_level));

        $pagename = $request->getArg('pagename');
        $nocache = 1;
        $login = new Template('login', $request,
                              compact('pagename', 'userid', 'require_level',
                                      'fail_message', 'pass_required', 'nocache'));
        if ($seperate_page) {
            $top = new Template('html', $request,
                                array('TITLE' => _("Sign In")));
            return $top->printExpansion($login);
        } else {
            return $login;
        }
    }

}


/**
 * Baseclass for PearDB and ADODB PassUser's
 * Authenticate against a database, to be able to use shared users.
 *   internal: no different $DbAuthParams['dsn'] defined, or
 *   external: different $DbAuthParams['dsn']
 * The magic is done in the symbolic SQL statements in index.php, similar to
 * libnss-mysql.
 *
 * We support only the SQL and ADODB backends.
 * The other WikiDB backends (flat, cvs, dba, ...) should be used for pages, 
 * not for auth stuff. If one would like to use e.g. dba for auth, he should 
 * use PearDB (SQL) with the right $DBAuthParam['auth_dsn']. 
 * (Not supported yet, since we require SQL. SQLite would make since when 
 * it will come to PHP)
 *
 * @tables: user, pref
 *
 * Preferences are handled in the parent class _PassUser, because the 
 * previous classes may also use DB pref_select and pref_update.
 *
 * Flat files auth is handled by the auth method "File".
 */
class _DbPassUser
extends _PassUser
{
    var $_authselect, $_authupdate, $_authcreate;

    // This can only be called from _PassUser, because the parent class 
    // sets the auth_dbi and pref methods, before this class is initialized.
    function _DbPassUser($UserName='',$prefs=false) {
        if (!$this->_prefs) {
            if ($prefs) $this->_prefs = $prefs;
        }
        if (!isset($this->_prefs->_method))
           _PassUser::_PassUser($UserName);
        $this->_authmethod = 'Db';
        //$this->getAuthDbh();
        //$this->_auth_crypt_method = @$GLOBALS['DBAuthParams']['auth_crypt_method'];
        if ($GLOBALS['DBParams']['dbtype'] == 'ADODB') {
            if (check_php_version(5))
                return new _AdoDbPassUser($UserName,$this->_prefs);
            else {
                $user = new _AdoDbPassUser($UserName,$this->_prefs);
                //todo: with php5 comment the following line:
                // /*PHP5 patch*/$this = $user;
                return UpgradeUser($user, $this);
            }
        }
        elseif ($GLOBALS['DBParams']['dbtype'] == 'SQL') {
            if (check_php_version(5))
                return new _PearDbPassUser($UserName,$this->_prefs);
            else {
                $user = new _PearDbPassUser($UserName,$this->_prefs);
                //todo: with php5 comment the following line:
                // /*PHP5 patch*/$this = $user;
                return UpgradeUser($user, $this);
            }
        }
        return false;
    }

    function mayChangePass() {
        return !isset($this->_authupdate);
    }

}

class _PearDbPassUser
extends _DbPassUser
/**
 * Pear DB methods
 * Now optimized not to use prepare, ...query(sprintf($sql,quote())) instead.
 * We use FETCH_MODE_ROW, so we don't need aliases in the auth_* SQL statements.
 *
 * @tables: user
 * @tables: pref
 */
{
    function _PearDbPassUser($UserName='',$prefs=false) {
        global $DBAuthParams;
        if (!$this->_prefs and isa($this,"_PearDbPassUser")) {
            if ($prefs) $this->_prefs = $prefs;
        }
        if (!isset($this->_prefs->_method))
            _PassUser::_PassUser($UserName);
        $this->_userid = $UserName;
        // make use of session data. generally we only initialize this every time, 
        // but do auth checks only once
        $this->_auth_crypt_method = @$DBAuthParams['auth_crypt_method'];
        //$this->getAuthDbh();
        return $this;
    }

    function getPreferences() {
        // override the generic slow method here for efficiency and not to 
        // clutter the homepage metadata with prefs.
        _AnonUser::getPreferences();
        $this->getAuthDbh();
        if (isset($this->_prefs->_select)) {
            $dbh = &$this->_auth_dbi;
            $db_result = $dbh->query(sprintf($this->_prefs->_select,$dbh->quote($this->_userid)));
            // patched by frederik@pandora.be
            $prefs = $db_result->fetchRow();
            $prefs_blob = $prefs["prefs"]; 
            if ($restored_from_db = $this->_prefs->retrieve($prefs_blob)) {
                $updated = $this->_prefs->updatePrefs($restored_from_db);
                //$this->_prefs = new UserPreferences($restored_from_db);
                return $this->_prefs;
            }
        }
        if ($this->_HomePagehandle) {
            if ($restored_from_page = $this->_prefs->retrieve($this->_HomePagehandle->get('pref'))) {
                $updated = $this->_prefs->updatePrefs($restored_from_page);
                //$this->_prefs = new UserPreferences($restored_from_page);
                return $this->_prefs;
            }
        }
        return $this->_prefs;
    }

    function setPreferences($prefs, $id_only=false) {
        // if the prefs are changed
        if (_AnonUser::setPreferences($prefs, 1)) {
            global $request;
            $user = $request->_user;
            //unset($user->_auth_dbi);
            //$request->setSessionVar('wiki_user', $user);
            $this->getAuthDbh();
            $packed = $this->_prefs->store();
            if (!$id_only and isset($this->_prefs->_update)) {
                $dbh = &$this->_auth_dbi;
                
                $dbh->simpleQuery(sprintf($this->_prefs->_update,
                                          $dbh->quote($packed),
                                          $dbh->quote($this->_userid)));
            } else {
                //store prefs in homepage, not in cookie
                if ($this->_HomePagehandle and !$id_only)
                    $this->_HomePagehandle->set('pref', $packed);
            }
            return count($this->_prefs->unpack($packed));
        }
        return 0;
    }

    function userExists() {
        global $DBAuthParams;
        $this->getAuthDbh();
        $dbh = &$this->_auth_dbi;
        if (!$dbh) { // needed?
            return $this->_tryNextUser();
        }
        // Prepare the configured auth statements
        if (!empty($DBAuthParams['auth_check']) and empty($this->_authselect)) {
            $this->_authselect = str_replace(array('"$userid"','"$password"'),
                                             array('%s','%s'),
                                             $DBAuthParams['auth_check']);
        }
        if (empty($this->_authselect))
            trigger_error("Either \$DBAuthParams['auth_check'] is missing or \$DBParams['dbtype'] != 'SQL'",
                          E_USER_WARNING);
        //NOTE: for auth_crypt_method='crypt' no special auth_user_exists is needed
        if ($this->_auth_crypt_method == 'crypt') {
            $rs = $dbh->query(sprintf($this->_authselect,$dbh->quote($this->_userid)));
            if ($rs->numRows())
                return true;
        }
        else {
            if (! $GLOBALS['DBAuthParams']['auth_user_exists'])
                trigger_error("\$DBAuthParams['auth_user_exists'] is missing",
                              E_USER_WARNING);
            $this->_authcheck = str_replace('"$userid"','%s',
                                             $DBAuthParams['auth_user_exists']);
            $rs = $dbh->query(sprintf($this->_authcheck,$dbh->quote($this->_userid)));
            if ($rs->numRows())
                return true;
        }
        // maybe the user is allowed to create himself. Generally not wanted in 
        // external databases, but maybe wanted for the wiki database, for performance 
        // reasons
        if (empty($this->_authcreate) and !empty($DBAuthParams['auth_create'])) {
            $this->_authcreate = str_replace(array('"$userid"','"$password"'),array('%s','%s'),
                                              $DBAuthParams['auth_create']);
        }
        if (!empty($this->_authcreate)) return true;

        return $this->_tryNextUser();
    }
 
    function checkPass($submitted_password) {
        global $DBAuthParams;
        $this->getAuthDbh();
        if (!$this->_auth_dbi) {  // needed?
            return $this->_tryNextPass($submitted_password);
        }
        if (empty($this->_authselect))
            trigger_error("Either \$DBAuthParams['auth_check'] is missing or \$DBParams['dbtype'] != 'SQL'",
                          E_USER_WARNING);

        //NOTE: for auth_crypt_method='crypt'  defined('ENCRYPTED_PASSWD',true) must be set
        $dbh = &$this->_auth_dbi;
        if ($this->_auth_crypt_method == 'crypt') {
            $stored_password = $dbh->getOne(sprintf($this->_authselect,$dbh->quote($this->_userid)));
            $result = $this->_checkPass($submitted_password, $stored_password);
        } else {
            $okay = $dbh->getOne(sprintf($this->_authselect,
                                         $dbh->quote($submitted_password),
                                         $dbh->quote($this->_userid)));
            $result = !empty($okay);
        }

        if ($result) {
            $this->_level = WIKIAUTH_USER;
            return $this->_level;
        } else {
            return $this->_tryNextPass($submitted_password);
        }
    }

    function mayChangePass() {
        global $DBAuthParams;
        return !empty($DBAuthParams['auth_update']);
    }

    function storePass($submitted_password) {
        global $DBAuthParams;
        $dbh = &$this->_auth_dbi;
        if (!empty($DBAuthParams['auth_update']) and empty($this->_authupdate)) {
            $this->_authupdate = str_replace(array('"$userid"','"$password"'),
                                             array('%s','%s'),
                                             $DBAuthParams['auth_update']);
        }
        if (empty($this->_authupdate)) {
            trigger_error("Either \$DBAuthParams['auth_update'] not defined or \$DBParams['dbtype'] != 'SQL'",
                          E_USER_WARNING);
            return false;
        }

        if ($this->_auth_crypt_method == 'crypt') {
            if (function_exists('crypt'))
                $submitted_password = crypt($submitted_password);
        }
        $dbh->simpleQuery(sprintf($this->_authupdate,
                                  $dbh->quote($submitted_password),
        			  $dbh->quote($this->_userid)
                                  ));
    }

}

class _AdoDbPassUser
extends _DbPassUser
/**
 * ADODB methods
 * Simple sprintf, no prepare.
 *
 * Warning: Since we use FETCH_MODE_ASSOC (string hash) and not the also faster 
 * FETCH_MODE_ROW (numeric), we have to use the correct aliases in auth_* sql statements!
 *
 * TODO: Change FETCH_MODE in adodb WikiDB sublasses.
 *
 * @tables: user
 */
{
    function _AdoDbPassUser($UserName='',$prefs=false) {
        if (!$this->_prefs and isa($this,"_AdoDbPassUser")) {
            if ($prefs) $this->_prefs = $prefs;
            if (!isset($this->_prefs->_method))
              _PassUser::_PassUser($UserName);
        }
        $this->_userid = $UserName;
        $this->_auth_crypt_method = $GLOBALS['DBAuthParams']['auth_crypt_method'];
        $this->getAuthDbh();
        // Don't prepare the configured auth statements anymore
        return $this;
    }

    function getPreferences() {
        // override the generic slow method here for efficiency
        _AnonUser::getPreferences();
        $this->getAuthDbh();
        if (isset($this->_prefs->_select)) {
            $dbh = & $this->_auth_dbi;
            $rs = $dbh->Execute(sprintf($this->_prefs->_select,$dbh->qstr($this->_userid)));
            if ($rs->EOF) {
                $rs->Close();
            } else {
                $prefs_blob = $rs->fields['prefs'];
                $rs->Close();
                if ($restored_from_db = $this->_prefs->retrieve($prefs_blob)) {
                    $updated = $this->_prefs->updatePrefs($restored_from_db);
                    //$this->_prefs = new UserPreferences($restored_from_db);
                    return $this->_prefs;
                }
            }
        }
        if ($this->_HomePagehandle) {
            if ($restored_from_page = $this->_prefs->retrieve($this->_HomePagehandle->get('pref'))) {
                $updated = $this->_prefs->updatePrefs($restored_from_page);
                //$this->_prefs = new UserPreferences($restored_from_page);
                return $this->_prefs;
            }
        }
        return $this->_prefs;
    }

    function setPreferences($prefs, $id_only=false) {
        // if the prefs are changed
        if (_AnonUser::setPreferences($prefs, 1)) {
            global $request;
            $packed = $this->_prefs->store();
            $user = $request->_user;
            unset($user->_auth_dbi);
            if (!$id_only and isset($this->_prefs->_update)) {
                $this->getAuthDbh();
                $dbh = &$this->_auth_dbi;
                $db_result = $dbh->Execute(sprintf($this->_prefs->_update,
                                                   $dbh->qstr($packed),
                                                   $dbh->qstr($this->_userid)));
                $db_result->Close();
            } else {
                //store prefs in homepage, not in cookie
                if ($this->_HomePagehandle and !$id_only)
                    $this->_HomePagehandle->set('pref', $packed);
            }
            return count($this->_prefs->unpack($packed));
        }
        return 0;
    }
 
    function userExists() {
        global $DBAuthParams;
        if (empty($this->_authselect) and !empty($DBAuthParams['auth_check'])) {
            $this->_authselect = str_replace(array('"$userid"','"$password"'),array('%s','%s'),
                                              $DBAuthParams['auth_check']);
        }
        if (empty($this->_authselect))
            trigger_error("Either \$DBAuthParams['auth_check'] is missing or \$DBParams['dbtype'] != 'ADODB'",
                          E_USER_WARNING);
        //$this->getAuthDbh();
        $dbh = &$this->_auth_dbi;
        //NOTE: for auth_crypt_method='crypt' no special auth_user_exists is needed
        if ($this->_auth_crypt_method == 'crypt') {
            $rs = $dbh->Execute(sprintf($this->_authselect,$dbh->qstr($this->_userid)));
            if (!$rs->EOF) {
                $rs->Close();
                return true;
            } else {
                $rs->Close();
            }
        }
        else {
            if (! $DBAuthParams['auth_user_exists'])
                trigger_error("\$DBAuthParams['auth_user_exists'] is missing",
                              E_USER_WARNING);
            $this->_authcheck = str_replace('"$userid"','%s',
                                             $DBAuthParams['auth_user_exists']);
            $rs = $dbh->Execute(sprintf($this->_authcheck,$dbh->qstr($this->_userid)));
            if (!$rs->EOF) {
                $rs->Close();
                return true;
            } else {
                $rs->Close();
            }
        }
        // maybe the user is allowed to create himself. Generally not wanted in 
        // external databases, but maybe wanted for the wiki database, for performance 
        // reasons
        if (!$this->_authcreate and !empty($DBAuthParams['auth_create'])) {
            $this->_authcreate = str_replace(array('"$userid"','"$password"'),array('%s','%s'),
                                              $DBAuthParams['auth_create']);
        }
        if (!empty($this->_authcreate)) return true;
        
        return $this->_tryNextUser();
    }

    function checkPass($submitted_password) {
        global $DBAuthParams;
        if (empty($this->_authselect) and !empty($DBAuthParams['auth_check'])) {
            $this->_authselect = str_replace(array('"$userid"','"$password"'),array('%s','%s'),
                                              $DBAuthParams['auth_check']);
        }
        if (!isset($this->_authselect))
            trigger_error("Either \$DBAuthParams['auth_check'] is missing or \$DBParams['dbtype'] != 'ADODB'",
                          E_USER_WARNING);
        //$this->getAuthDbh();
        $dbh = &$this->_auth_dbi;
        //NOTE: for auth_crypt_method='crypt'  defined('ENCRYPTED_PASSWD',true) must be set
        if ($this->_auth_crypt_method == 'crypt') {
            $rs = $dbh->Execute(sprintf($this->_authselect,$dbh->qstr($this->_userid)));
            if (!$rs->EOF) {
                $stored_password = $rs->fields['password'];
                $rs->Close();
                $result = $this->_checkPass($submitted_password, $stored_password);
            } else {
                $rs->Close();
                $result = false;
            }
        }
        else {
            $rs = $dbh->Execute(sprintf($this->_authselect,
                                        $dbh->qstr($submitted_password),
                                        $dbh->qstr($this->_userid)));
            $okay = $rs->fields['ok'];
            $rs->Close();
            $result = !empty($okay);
        }

        if ($result) { 
            $this->_level = WIKIAUTH_USER;
            return $this->_level;
        } else {
            return $this->_tryNextPass($submitted_password);
        }
    }

    function mayChangePass() {
        global $DBAuthParams;
        return !empty($DBAuthParams['auth_update']);
    }

    function storePass($submitted_password) {
    	global $DBAuthParams;
        if (!isset($this->_authupdate) and !empty($DBAuthParams['auth_update'])) {
            $this->_authupdate = str_replace(array('"$userid"','"$password"'),array("%s","%s"),
                                              $DBAuthParams['auth_update']);
        }
        if (!isset($this->_authupdate)) {
            trigger_error("Either \$DBAuthParams['auth_update'] not defined or \$DBParams['dbtype'] != 'ADODB'",
                          E_USER_WARNING);
            return false;
        }

        if ($this->_auth_crypt_method == 'crypt') {
            if (function_exists('crypt'))
                $submitted_password = crypt($submitted_password);
        }
        $this->getAuthDbh();
        $dbh = &$this->_auth_dbi;
        $rs = $dbh->Execute(sprintf($this->_authupdate,
                                    $dbh->qstr($submitted_password),
                                    $dbh->qstr($this->_userid)
                                    ));
        $rs->Close();
        return $rs;
    }

}

class _LDAPPassUser
extends _PassUser
/**
 * Define the vars LDAP_AUTH_HOST and LDAP_BASE_DN in index.php
 *
 * Preferences are handled in _PassUser
 */
{
    function checkPass($submitted_password) {
        global $LDAP_SET_OPTION;

        $this->_authmethod = 'LDAP';
        $userid = $this->_userid;
        if ($ldap = ldap_connect(LDAP_AUTH_HOST)) { // must be a valid LDAP server!
            if (defined('LDAP_AUTH_USER'))
                if (defined('LDAP_AUTH_PASSWORD'))
                    // Windows Active Directory Server is strict
                    $r = @ldap_bind($ldap,LDAP_AUTH_USER,LDAP_AUTH_PASSWORD); 
                else
                    $r = @ldap_bind($ldap,LDAP_AUTH_USER); 
            else
                $r = @ldap_bind($ldap); // this is an anonymous bind
            if (!empty($LDAP_SET_OPTION)) {
                foreach ($LDAP_SET_OPTION as $key => $value) {
                    ldap_set_option($ldap,$key,$value);
                }
            }
            // Need to set the right root search information. see ../index.php
            $st_search = defined('LDAP_SEARCH_FIELD') 
                ? LDAP_SEARCH_FIELD."=$userid"
                : "uid=$userid";
            $sr = ldap_search($ldap, LDAP_BASE_DN, $st_search);
            $info = ldap_get_entries($ldap, $sr); 
            // there may be more hits with this userid.
            // of course it would be better to narrow down the BASE_DN
            for ($i = 0; $i < $info["count"]; $i++) {
                $dn = $info[$i]["dn"];
                // The password is still plain text.
                if ($r = @ldap_bind($ldap, $dn, $submitted_password)) {
                    // ldap_bind will return TRUE if everything matches
                    ldap_close($ldap);
                    $this->_level = WIKIAUTH_USER;
                    return $this->_level;
                }
            }
        } else {
            trigger_error(fmt("Unable to connect to LDAP server %s", LDAP_AUTH_HOST), 
                          E_USER_WARNING);
            //return false;
        }

        return $this->_tryNextPass($submitted_password);
    }

    function userExists() {
        global $LDAP_SET_OPTION;

        $userid = $this->_userid;
        if ($ldap = ldap_connect(LDAP_AUTH_HOST)) { // must be a valid LDAP server!
            if (defined('LDAP_AUTH_USER'))
                if (defined('LDAP_AUTH_PASSWORD'))
                    // Windows Active Directory Server is strict
                    $r = @ldap_bind($ldap,LDAP_AUTH_USER,LDAP_AUTH_PASSWORD); 
                else
                    $r = @ldap_bind($ldap,LDAP_AUTH_USER); 
            else
                $r = @ldap_bind($ldap); // this is an anonymous bind
            if (!empty($LDAP_SET_OPTION)) {
                foreach ($LDAP_SET_OPTION as $key => $value) {
                    ldap_set_option($ldap,$key,$value);
                }
            }
            // Need to set the right root search information. see ../index.php
            $st_search = defined('LDAP_SEARCH_FIELD') 
                ? LDAP_SEARCH_FIELD."=$userid"
                : "uid=$userid";
            $sr = ldap_search($ldap, LDAP_BASE_DN, $st_search);
            $info = ldap_get_entries($ldap, $sr); 

            if ($info["count"] > 0) {
                ldap_close($ldap);
                return true;
            }
        } else {
            trigger_error(_("Unable to connect to LDAP server "). LDAP_AUTH_HOST, E_USER_WARNING);
        }

        return $this->_tryNextUser();
    }

    function mayChangePass() {
        return false;
    }

}

class _IMAPPassUser
extends _PassUser
/**
 * Define the var IMAP_AUTH_HOST in index.php (with port probably)
 *
 * Preferences are handled in _PassUser
 */
{
    function checkPass($submitted_password) {
        $userid = $this->_userid;
        $mbox = @imap_open( "{" . IMAP_AUTH_HOST . "}",
                            $userid, $submitted_password, OP_HALFOPEN );
        if ($mbox) {
            imap_close($mbox);
            $this->_authmethod = 'IMAP';
            $this->_level = WIKIAUTH_USER;
            return $this->_level;
        } else {
            trigger_error(_("Unable to connect to IMAP server "). IMAP_AUTH_HOST, E_USER_WARNING);
        }

        return $this->_tryNextPass($submitted_password);
    }

    //CHECKME: this will not be okay for the auth policy strict
    function userExists() {
        return true;
        if (checkPass($this->_prefs->get('passwd')))
            return true;
            
        return $this->_tryNextUser();
    }

    function mayChangePass() {
        return false;
    }
}


class _POP3PassUser
extends _IMAPPassUser {
/**
 * Define the var POP3_AUTH_HOST in index.php
 * Preferences are handled in _PassUser
 */
    function checkPass($submitted_password) {
        $userid = $this->_userid;
        $pass = $submitted_password;
        $host = defined('POP3_AUTH_HOST') ? POP3_AUTH_HOST : 'localhost';
        $port = defined('POP3_AUTH_PORT') ? POP3_AUTH_PORT : 110;
        $retval = false;
        $fp = fsockopen($host, $port, $errno, $errstr, 10);
        if ($fp) {
            // Get welcome string
            $line = fgets($fp, 1024);
            if (! strncmp("+OK ", $line, 4)) {
                // Send user name
                fputs($fp, "user $user\n");
                // Get response
                $line = fgets($fp, 1024);
                if (! strncmp("+OK ", $line, 4)) {
                    // Send password
                    fputs($fp, "pass $pass\n");
                    // Get response
                    $line = fgets($fp, 1024);
                    if (! strncmp("+OK ", $line, 4)) {
                        $retval = true;
                    }
                }
            }
            // quit the connection
            fputs($fp, "quit\n");
            // Get the sayonara message
            $line = fgets($fp, 1024);
            fclose($fp);
        }
        $this->_authmethod = 'POP3';
        if ($retval) {
            $this->_level = WIKIAUTH_USER;
        } else {
            $this->_level = WIKIAUTH_ANON;
        }
        return $this->_level;
    }
}

class _FilePassUser
extends _PassUser
/**
 * Check users defined in a .htaccess style file
 * username:crypt\n...
 *
 * Preferences are handled in _PassUser
 */
{
    var $_file, $_may_change;

    // This can only be called from _PassUser, because the parent class 
    // sets the pref methods, before this class is initialized.
    function _FilePassUser($UserName='',$file='') {
        if (!$this->_prefs)
            _PassUser::_PassUser($UserName);

        // read the .htaccess style file. We use our own copy of the standard pear class.
        include_once 'lib/pear/File_Passwd.php';
        // if passwords may be changed we have to lock them:
        $this->_may_change = defined('AUTH_USER_FILE_STORABLE') && AUTH_USER_FILE_STORABLE;
        if (empty($file) and defined('AUTH_USER_FILE'))
            $this->_file = new File_Passwd(AUTH_USER_FILE, !empty($this->_may_change));
        elseif (!empty($file))
            $this->_file = new File_Passwd($file, !empty($this->_may_change));
        else
            return false;
        return $this;
    }
 
    function mayChangePass() {
        return $this->_may_change;
    }

    function userExists() {
        if (isset($this->_file->users[$this->_userid]))
            return true;
            
        return $this->_tryNextUser();
    }

    function checkPass($submitted_password) {
        if ($this->_file->verifyPassword($this->_userid,$submitted_password)) {
            $this->_authmethod = 'File';
            $this->_level = WIKIAUTH_USER;
            return $this->_level;
        }
        
        return $this->_tryNextPass($submitted_password);
    }

    function storePass($submitted_password) {
        if ($this->_may_change)
            return $this->_file->modUser($this->_userid,$submitted_password);
        else 
            return false;
    }

}

/**
 * Insert more auth classes here...
 * For example a customized db class for another db connection 
 * or a socket-based auth server
 *
 */


/**
 * For security, this class should not be extended. Instead, extend
 * from _PassUser (think of this as unix "root").
 */
class _AdminUser
extends _PassUser
{
    function mayChangePass() {
        return false;
    }
    function checkPass($submitted_password) {
        $stored_password = ADMIN_PASSWD;
        if ($this->_checkPass($submitted_password, $stored_password)) {
            $this->_level = WIKIAUTH_ADMIN;
            return $this->_level;
        } else {
            $this->_level = WIKIAUTH_ANON;
            return $this->_level;
        }
    }
    function storePass($submitted_password) {
        return false;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Various data classes for the preference types, 
 * to support get, set, sanify (range checking, ...)
 * update() will do the neccessary side-effects if a 
 * setting gets changed (theme, language, ...)
*/

class _UserPreference
{
    var $default_value;

    function _UserPreference ($default_value) {
        $this->default_value = $default_value;
    }

    function sanify ($value) {
        return (string)$value;
    }

    function get ($name) {
    	if (isset($this->{$name}))
	    return $this->{$name};
    	else 
            return $this->default_value;
    }

    function getraw ($name) {
    	if (!empty($this->{$name}))
	    return $this->{$name};
    }

    // stores the value as $this->$name, and not as $this->value (clever?)
    function set ($name, $value) {
	if ($this->get($name) != $value) {
	    $this->update($value);
	}
	if ($value != $this->default_value) {
	    $this->{$name} = $value;
        }
        else 
            unset($this->{$name});
    }

    // default: no side-effects 
    function update ($value) {
    	;
    }
}

class _UserPreference_numeric
extends _UserPreference
{
    function _UserPreference_numeric ($default, $minval = false,
                                      $maxval = false) {
        $this->_UserPreference((double)$default);
        $this->_minval = (double)$minval;
        $this->_maxval = (double)$maxval;
    }

    function sanify ($value) {
        $value = (double)$value;
        if ($this->_minval !== false && $value < $this->_minval)
            $value = $this->_minval;
        if ($this->_maxval !== false && $value > $this->_maxval)
            $value = $this->_maxval;
        return $value;
    }
}

class _UserPreference_int
extends _UserPreference_numeric
{
    function _UserPreference_int ($default, $minval = false, $maxval = false) {
        $this->_UserPreference_numeric((int)$default, (int)$minval, (int)$maxval);
    }

    function sanify ($value) {
        return (int)parent::sanify((int)$value);
    }
}

class _UserPreference_bool
extends _UserPreference
{
    function _UserPreference_bool ($default = false) {
        $this->_UserPreference((bool)$default);
    }

    function sanify ($value) {
        if (is_array($value)) {
            /* This allows for constructs like:
             *
             *   <input type="hidden" name="pref[boolPref][]" value="0" />
             *   <input type="checkbox" name="pref[boolPref][]" value="1" />
             *
             * (If the checkbox is not checked, only the hidden input
             * gets sent. If the checkbox is sent, both inputs get
             * sent.)
             */
            foreach ($value as $val) {
                if ($val)
                    return true;
            }
            return false;
        }
        return (bool) $value;
    }
}

class _UserPreference_language
extends _UserPreference
{
    function _UserPreference_language ($default = DEFAULT_LANGUAGE) {
        $this->_UserPreference($default);
    }

    // FIXME: check for valid locale
    function sanify ($value) {
        // Revert to DEFAULT_LANGUAGE if user does not specify
        // language in UserPreferences or chooses <system language>.
        if ($value == '' or empty($value))
            $value = DEFAULT_LANGUAGE;

        return (string) $value;
    }
}

class _UserPreference_theme
extends _UserPreference
{
    function _UserPreference_theme ($default = THEME) {
        $this->_UserPreference($default);
    }

    function sanify ($value) {
        if (!empty($value) and FindFile($this->_themefile($value)))
            return $value;
        return $this->default_value;
    }

    function update ($newvalue) {
        global $Theme;
        if ($newvalue)
            include_once($this->_themefile($newvalue));
        if (empty($Theme))
            include_once($this->_themefile(THEME));
    }

    function _themefile ($theme) {
        return "themes/$theme/themeinfo.php";
    }
}

class _UserPreference_notify
extends _UserPreference
{
    function sanify ($value) {
    	if (!empty($value))
            return $value;
        else
            return $this->default_value;
    }

    /** update to global user prefs: side-effect on set notify changes
     * use a global_data notify hash:
     * notify = array('pagematch' => array(userid => ('email' => mail, 
     *                                                'verified' => 0|1),
     *                                     ...),
     *                ...);
     */
    function update ($value) {
        $dbh = $GLOBALS['request']->getDbh();
        $notify = $dbh->get('notify');
        if (empty($notify))
            $data = array();
        else 
            $data = & $notify;
        // expand to existing pages only or store matches?
        // for now we store (glob-style) matches which is easier for the user
        $pages = $this->_page_split($value);
        $user = $GLOBALS['request']->getUser();
        $userid = $user->UserName();
        $email  = $user->_prefs->get('email');
        $verified = $user->_prefs->_prefs['email']->getraw('emailVerified');
        // check existing notify hash and possibly delete pages for email
        if (!empty($data)) {
            foreach ($data as $page => $users) {
                if (isset($data[$page][$userid]) and !in_array($page, $pages)) {
                    unset($data[$page][$userid]);
                }
                if (count($data[$page]) == 0)
                    unset($data[$page]);
            }
        }
        // add the new pages
        if (!empty($pages)) {
            foreach ($pages as $page) {
                if (!isset($data[$page]))
                    $data[$page] = array();
                if (!isset($data[$page][$userid])) {
                    // should we really store the verification notice here or 
                    // check it dynamically at every page->save?
                    if ($verified) {
                        $data[$page][$userid] = array('email' => $email,
                                                      'verified' => $verified);
                    } else {
                        $data[$page][$userid] = array('email' => $email);
                    }
                }
            }
        }
        // store users changes
        $dbh->set('notify',$data);
    }

    /** split the user-given comma or whitespace delimited pagenames
     *  to array
     */
    function _page_split($value) {
        return preg_split('/[\s,]+/',$value,-1,PREG_SPLIT_NO_EMPTY);
    }
}

class _UserPreference_email
extends _UserPreference
{
    function sanify ($value) {
        // check for valid email address
        if ($this->get('email') == $value and $this->getraw('emailVerified'))
            return $value;
        list($ok,$msg) = ValidateMail($value,'noconnect');
        if ($ok) {
            return $value;
        } else {
            trigger_error($msg, E_USER_WARNING);
            return $this->default_value;
        }
    }

    /** Side-effect on email changes:
     * Send a verification mail or for now just a notification email.
     * For true verification (value = 2), we'd need a mailserver hook.
     */
    function update ($value) {
        $verified = $this->getraw('emailVerified');
        if (!empty($value) and !$verified) {
            list($ok,$msg) = ValidateMail($value);
            if ($ok and mail($value,"[".WIKI_NAME ."] "._("Email Verification"),
                     sprintf(_("Welcome to %s!\nYou email account is verified and\nwill be used to send pagechange notifications.\nSee %s"),
                             WIKI_NAME, WikiUrl($GLOBALS['request']->getArg('pagename'),'',true))))
                $this->set('emailVerified',1);
        }
    }
}

/** Check for valid email address
    fixed version from http://www.zend.com/zend/spotlight/ev12apr.php
 */
function ValidateMail($email, $noconnect=false) {
    $HTTP_HOST = $_SERVER['HTTP_HOST'];
    $result = array();
    // well, technically ".a.a.@host.com" is also valid
    if (!eregi("^[_a-z0-9-]+(\.[_a-z0-9-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,3})$", $email)) {
        $result[0] = false;
        $result[1] = "$email is not properly formatted";
        return $result;
    }
    if ($noconnect)
      return array(true,"$email is properly formatted");

    list ( $Username, $Domain ) = split ("@",$email);
    //Todo: getmxrr workaround on windows or manual input field to verify it manually
    if (!isWindows() and getmxrr($Domain, $MXHost)) { // avoid warning on Windows. 
        $ConnectAddress = $MXHost[0];
    } else {
        $ConnectAddress = $Domain;
    }
    $Connect = fsockopen ( $ConnectAddress, 25 );
    if ($Connect) {
        if (ereg("^220", $Out = fgets($Connect, 1024))) {
            fputs ($Connect, "HELO $HTTP_HOST\r\n");
            $Out = fgets ( $Connect, 1024 );
            fputs ($Connect, "MAIL FROM: <".$email.">\r\n");
            $From = fgets ( $Connect, 1024 );
            fputs ($Connect, "RCPT TO: <".$email.">\r\n");
            $To = fgets ($Connect, 1024);
            fputs ($Connect, "QUIT\r\n");
            fclose($Connect);
            if (!ereg ("^250", $From)) {
                $result[0]=false;
                $result[1]="Server rejected address: ". $From;
                return $result;
            }
            if (!ereg ( "^250", $To )) {
                $result[0]=false;
                $result[1]="Server rejected address: ". $To;
                return $result;
            }
        } else {
            $result[0] = false;
            $result[1] = "No response from server";
            return $result;
          }
    }  else {
        $result[0]=false;
        $result[1]="Can not connect E-Mail server.";
        return $result;
    }
    $result[0]=true;
    $result[1]="$email appears to be valid.";
    return $result;
} // end of function 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * UserPreferences
 * 
 * This object holds the $request->_prefs subobjects.
 * A simple packed array of non-default values get's stored as cookie,
 * homepage, or database, which are converted to the array of 
 * ->_prefs objects.
 * We don't store the objects, because otherwise we will
 * not be able to upgrade any subobject. And it's a waste of space also.
 *
 */
class UserPreferences
{
    function UserPreferences($saved_prefs = false) {
        // userid stored too, to ensure the prefs are being loaded for
        // the correct (currently signing in) userid if stored in a
        // cookie.
        $this->_prefs
            = array(
                    'userid'        => new _UserPreference(''),
                    'passwd'        => new _UserPreference(''),
                    'autologin'     => new _UserPreference_bool(),
                    //'emailVerified' => new _UserPreference_emailVerified(),
                    //fixed: store emailVerified as email parameter
                    'email'         => new _UserPreference_email(''),
                    'notifyPages'   => new _UserPreference_notify(''),
                    'theme'         => new _UserPreference_theme(THEME),
                    'lang'          => new _UserPreference_language(DEFAULT_LANGUAGE),
                    'editWidth'     => new _UserPreference_int(EDITWIDTH_DEFAULT_COLS,
                                                               EDITWIDTH_MIN_COLS,
                                                               EDITWIDTH_MAX_COLS),
                    'noLinkIcons'   => new _UserPreference_bool(),
                    'editHeight'    => new _UserPreference_int(EDITHEIGHT_DEFAULT_ROWS,
                                                               EDITHEIGHT_MIN_ROWS,
                                                               EDITHEIGHT_DEFAULT_ROWS),
                    'timeOffset'    => new _UserPreference_numeric(TIMEOFFSET_DEFAULT_HOURS,
                                                                   TIMEOFFSET_MIN_HOURS,
                                                                   TIMEOFFSET_MAX_HOURS),
                    'relativeDates' => new _UserPreference_bool()
                    );

        if (is_array($saved_prefs)) {
            foreach ($saved_prefs as $name => $value)
                $this->set($name, $value);
        }
    }

    function _getPref($name) {
    	if ($name == 'emailVerified')
    	    $name = 'email';
        if (!isset($this->_prefs[$name])) {
            if ($name == 'passwd2') return false;
            trigger_error("$name: unknown preference", E_USER_NOTICE);
            return false;
        }
        return $this->_prefs[$name];
    }
    
    // get the value or default_value of the subobject
    function get($name) {
    	if ($_pref = $this->_getPref($name))
    	    if ($name == 'emailVerified')
    	        return $_pref->getraw($name);
    	    else
    	        return $_pref->get($name);
    	else
    	    return false;  
    }

    // check and set the new value in the subobject
    function set($name, $value) {
        $pref = $this->_getPref($name);
        if ($pref === false)
            return false;

        /* do it here or outside? */
        if ($name == 'passwd' and 
            defined('PASSWORD_LENGTH_MINIMUM') and 
            strlen($value) <= PASSWORD_LENGTH_MINIMUM ) {
            //TODO: How to notify the user?
            return false;
        }
        /*
        if ($name == 'theme' and $value == '')
           return true;
        */
        if (!isset($pref->{$value}) or $pref->{$value} != $pref->default_value) {
            $newvalue = $pref->sanify($value);
	    $pref->set($name,$newvalue);
        }
        $this->_prefs[$name] = $pref;
        return true;
    }

    function updatePrefs($prefs) {
        $count = 0;
        if (is_object($prefs)) {
            foreach ($this->_prefs as $type => $obj) {
                if ($obj->get($type) !== $prefs->get($type)) {
                    $this->_prefs[$type]->set($type,$prefs->get($type));
                    $count++;
                }
            }
        } elseif (is_array($prefs)) {
            foreach ($this->_prefs as $type => $obj) {
                if (isset($prefs[$type]) and $obj->get($type) != $prefs[$type]) {
                    $this->_prefs[$type]->set($type,$prefs[$type]);
                    $count++;
                }
            }
        }
        return $count;
    }

    // for now convert just array of objects => array of values
    // Todo: the specialized subobjects must override this.
    function store() {
        $prefs = array();
        foreach ($this->_prefs as $name => $object) {
            if ($value = $object->getraw($name))
                $prefs[$name] = $value;
            if ($name == 'email' and ($value = $object->getraw('emailVerified')))
                $prefs['emailVerified'] = $value;
        }
        return $this->pack($prefs);
    }

    // packed string or array of values => array of values
    // Todo: the specialized subobjects must override this.
    function retrieve($packed) {
        if (is_string($packed) and (substr($packed, 0, 2) == "a:"))
            $packed = unserialize($packed);
        if (!is_array($packed)) return false;
        $prefs = array();
        foreach ($packed as $name => $packed_pref) {
            if (is_string($packed_pref) and substr($packed_pref, 0, 2) == "O:") {
                //legacy: check if it's an old array of objects
                // Looks like a serialized object. 
                // This might fail if the object definition does not exist anymore.
                // object with ->$name and ->default_value vars.
                $pref =  @unserialize($packed_pref);
                if (empty($pref))
                    $pref = @unserialize(base64_decode($packed_pref));
                $prefs[$name] = $pref->get($name);
            // fix old-style prefs
            } elseif (is_numeric($name) and is_array($packed_pref)) {
            	if (count($packed_pref) == 1) {
            	    list($name,$value) = each($packed_pref);
            	    $prefs[$name] = $value;
            	}
            } else {
                $prefs[$name] = @unserialize($packed_pref);
                if (empty($prefs[$name]))
                    $prefs[$name] = @unserialize(base64_decode($packed_pref));
                // patched by frederik@pandora.be
                if (empty($prefs[$name]))
                    $prefs[$name] = $packed_pref;
            }
        }
        return $prefs;
    }

    /**
     * Check if the given prefs object is different from the current prefs object
     */
    function isChanged($other) {
        foreach ($this->_prefs as $type => $obj) {
            if ($obj->get($type) !== $other->get($type))
                return true;
        }
        return false;
    }

    // array of objects
    function getAll() {
        return $this->_prefs;
    }

    function pack($nonpacked) {
        return serialize($nonpacked);
    }

    function unpack($packed) {
        if (!$packed)
            return false;
        //$packed = base64_decode($packed);
        if (substr($packed, 0, 2) == "O:") {
            // Looks like a serialized object
            return unserialize($packed);
        }
        if (substr($packed, 0, 2) == "a:") {
            return unserialize($packed);
        }
        //trigger_error("DEBUG: Can't unpack bad UserPreferences",
        //E_USER_WARNING);
        return false;
    }

    function hash () {
        return hash($this->_prefs);
    }
}

/** TODO: new pref storage classes
 *  These are currently user specific and should be rewritten to be pref specific.
 *  i.e. $this == $user->_prefs
 */
class CookieUserPreferences
extends UserPreferences
{
    function CookieUserPreferences ($saved_prefs = false) {
    	//_AnonUser::_AnonUser('',$saved_prefs);
        UserPreferences::UserPreferences($saved_prefs);
    }
}

class PageUserPreferences
extends UserPreferences
{
    function PageUserPreferences ($saved_prefs = false) {
        UserPreferences::UserPreferences($saved_prefs);
    }
}

class PearDbUserPreferences
extends UserPreferences
{
    function PearDbUserPreferences ($saved_prefs = false) {
        UserPreferences::UserPreferences($saved_prefs);
    }
}

class AdoDbUserPreferences
extends UserPreferences
{
    function AdoDbUserPreferences ($saved_prefs = false) {
        UserPreferences::UserPreferences($saved_prefs);
    }
    function getPreferences() {
        // override the generic slow method here for efficiency
        _AnonUser::getPreferences();
        $this->getAuthDbh();
        if (isset($this->_select)) {
            $dbh = & $this->_auth_dbi;
            $rs = $dbh->Execute(sprintf($this->_select,$dbh->qstr($this->_userid)));
            if ($rs->EOF) {
                $rs->Close();
            } else {
                $prefs_blob = $rs->fields['pref_blob'];
                $rs->Close();
                if ($restored_from_db = $this->_prefs->retrieve($prefs_blob)) {
                    $updated = $this->_prefs->updatePrefs($restored_from_db);
                    //$this->_prefs = new UserPreferences($restored_from_db);
                    return $this->_prefs;
                }
            }
        }
        if (empty($this->_prefs->_prefs) and $this->_HomePagehandle) {
            if ($restored_from_page = $this->_prefs->retrieve($this->_HomePagehandle->get('pref'))) {
                $updated = $this->_prefs->updatePrefs($restored_from_page);
                //$this->_prefs = new UserPreferences($restored_from_page);
                return $this->_prefs;
            }
        }
        return $this->_prefs;
    }
}


// $Log: not supported by cvs2svn $
// Revision 1.37  2004/03/25 17:00:31  rurban
// more code to convert old-style pref array to new hash
//
// Revision 1.36  2004/03/24 19:39:02  rurban
// php5 workaround code (plus some interim debugging code in XmlElement)
//   php5 doesn't work yet with the current XmlElement class constructors,
//   WikiUserNew does work better than php4.
// rewrote WikiUserNew user upgrading to ease php5 update
// fixed pref handling in WikiUserNew
// added Email Notification
// added simple Email verification
// removed emailVerify userpref subclass: just a email property
// changed pref binary storage layout: numarray => hash of non default values
// print optimize message only if really done.
// forced new cookie policy: delete pref cookies, use only WIKI_ID as plain string.
//   prefs should be stored in db or homepage, besides the current session.
//
// Revision 1.35  2004/03/18 22:18:31  rurban
// workaround for php5 object upgrading problem
//
// Revision 1.34  2004/03/18 21:41:09  rurban
// fixed sqlite support
// WikiUserNew: PHP5 fixes: don't assign $this (untested)
//
// Revision 1.33  2004/03/16 15:42:04  rurban
// more fixes for undefined property warnings
//
// Revision 1.32  2004/03/14 16:30:52  rurban
// db-handle session revivification, dba fixes
//
// Revision 1.31  2004/03/12 23:20:58  rurban
// pref fixes (base64)
//
// Revision 1.30  2004/03/12 20:59:17  rurban
// important cookie fix by Konstantin Zadorozhny
// new editpage feature: JS_SEARCHREPLACE
//
// Revision 1.29  2004/03/11 13:30:47  rurban
// fixed File Auth for user and group
// missing only getMembersOf(Authenticated Users),getMembersOf(Every),getMembersOf(Signed Users)
//
// Revision 1.28  2004/03/08 18:17:09  rurban
// added more WikiGroup::getMembersOf methods, esp. for special groups
// fixed $LDAP_SET_OPTIONS
// fixed _AuthInfo group methods
//
// Revision 1.27  2004/03/01 09:35:13  rurban
// fixed DbPassuser pref init; lost userid
//
// Revision 1.26  2004/02/29 04:10:56  rurban
// new POP3 auth (thanks to BiloBilo: pentothal at despammed dot com)
// fixed syntax error in index.php
//
// Revision 1.25  2004/02/28 22:25:07  rurban
// First PagePerm implementation:
//
// $Theme->setAnonEditUnknownLinks(false);
//
// Layout improvement with dangling links for mostly closed wiki's:
// If false, only users with edit permissions will be presented the
// special wikiunknown class with "?" and Tooltip.
// If true (default), any user will see the ?, but will be presented
// the PrintLoginForm on a click.
//
// Revision 1.24  2004/02/28 21:14:08  rurban
// generally more PHPDOC docs
//   see http://xarch.tu-graz.ac.at/home/rurban/phpwiki/xref/
// fxied WikiUserNew pref handling: empty theme not stored, save only
//   changed prefs, sql prefs improved, fixed password update,
//   removed REPLACE sql (dangerous)
// moved gettext init after the locale was guessed
// + some minor changes
//
// Revision 1.23  2004/02/27 13:21:17  rurban
// several performance improvements, esp. with peardb
// simplified loops
// storepass seperated from prefs if defined so
// stacked and strict still not working
//
// Revision 1.22  2004/02/27 05:15:40  rurban
// more stability. detected by Micki
//
// Revision 1.21  2004/02/26 20:43:49  rurban
// new HttpAuthPassUser class (forces http auth if in the auth loop)
// fixed user upgrade: don't return _PassUser in the first hand.
//
// Revision 1.20  2004/02/26 01:29:11  rurban
// important fixes: endless loops in certain cases. minor rewrite
//
// Revision 1.19  2004/02/25 17:15:17  rurban
// improve stability
//
// Revision 1.18  2004/02/24 15:20:05  rurban
// fixed minor warnings: unchecked args, POST => Get urls for sortby e.g.
//
// Revision 1.17  2004/02/17 12:16:42  rurban
// started with changePass support. not yet used.
//
// Revision 1.16  2004/02/15 22:23:45  rurban
// oops, fixed showstopper (endless recursion)
//
// Revision 1.15  2004/02/15 21:34:37  rurban
// PageList enhanced and improved.
// fixed new WikiAdmin... plugins
// editpage, Theme with exp. htmlarea framework
//   (htmlarea yet committed, this is really questionable)
// WikiUser... code with better session handling for prefs
// enhanced UserPreferences (again)
// RecentChanges for show_deleted: how should pages be deleted then?
//
// Revision 1.14  2004/02/15 17:30:13  rurban
// workaround for lost db connnection handle on session restauration (->_auth_dbi)
// fixed getPreferences() (esp. from sessions)
// fixed setPreferences() (update and set),
// fixed AdoDb DB statements,
// update prefs only at UserPreferences POST (for testing)
// unified db prefs methods (but in external pref classes yet)
//
// Revision 1.13  2004/02/09 03:58:12  rurban
// for now default DB_SESSION to false
// PagePerm:
//   * not existing perms will now query the parent, and not
//     return the default perm
//   * added pagePermissions func which returns the object per page
//   * added getAccessDescription
// WikiUserNew:
//   * added global ->prepare (not yet used) with smart user/pref/member table prefixing.
//   * force init of authdbh in the 2 db classes
// main:
//   * fixed session handling (not triple auth request anymore)
//   * don't store cookie prefs with sessions
// stdlib: global obj2hash helper from _AuthInfo, also needed for PagePerm
//
// Revision 1.12  2004/02/07 10:41:25  rurban
// fixed auth from session (still double code but works)
// fixed GroupDB
// fixed DbPassUser upgrade and policy=old
// added GroupLdap
//
// Revision 1.11  2004/02/03 09:45:39  rurban
// LDAP cleanup, start of new Pref classes
//
// Revision 1.10  2004/02/01 09:14:11  rurban
// Started with Group_Ldap (not yet ready)
// added new _AuthInfo plugin to help in auth problems (warning: may display passwords)
// fixed some configurator vars
// renamed LDAP_AUTH_SEARCH to LDAP_BASE_DN
// changed PHPWIKI_VERSION from 1.3.8a to 1.3.8pre
// USE_DB_SESSION defaults to true on SQL
// changed GROUP_METHOD definition to string, not constants
// changed sample user DBAuthParams from UPDATE to REPLACE to be able to
//   create users. (Not to be used with external databases generally, but
//   with the default internal user table)
//
// fixed the IndexAsConfigProblem logic. this was flawed:
//   scripts which are the same virtual path defined their own lib/main call
//   (hmm, have to test this better, phpwiki.sf.net/demo works again)
//
// Revision 1.9  2004/01/30 19:57:58  rurban
// fixed DBAuthParams['pref_select']: wrong _auth_dbi object used.
//
// Revision 1.8  2004/01/30 18:46:15  rurban
// fix "lib/WikiUserNew.php:572: Notice[8]: Undefined variable: DBParams"
//
// Revision 1.7  2004/01/27 23:23:39  rurban
// renamed ->Username => _userid for consistency
// renamed mayCheckPassword => mayCheckPass
// fixed recursion problem in WikiUserNew
// fixed bogo login (but not quite 100% ready yet, password storage)
//
// Revision 1.6  2004/01/26 09:17:49  rurban
// * changed stored pref representation as before.
//   the array of objects is 1) bigger and 2)
//   less portable. If we would import packed pref
//   objects and the object definition was changed, PHP would fail.
//   This doesn't happen with an simple array of non-default values.
// * use $prefs->retrieve and $prefs->store methods, where retrieve
//   understands the interim format of array of objects also.
// * simplified $prefs->get() and fixed $prefs->set()
// * added $user->_userid and class '_WikiUser' portability functions
// * fixed $user object ->_level upgrading, mostly using sessions.
//   this fixes yesterdays problems with loosing authorization level.
// * fixed WikiUserNew::checkPass to return the _level
// * fixed WikiUserNew::isSignedIn
// * added explodePageList to class PageList, support sortby arg
// * fixed UserPreferences for WikiUserNew
// * fixed WikiPlugin for empty defaults array
// * UnfoldSubpages: added pagename arg, renamed pages arg,
//   removed sort arg, support sortby arg
//
// Revision 1.5  2004/01/25 03:05:00  rurban
// First working version, but has some problems with the current main loop.
// Implemented new auth method dispatcher and policies, all the external
// _PassUser classes (also for ADODB and Pear DB).
// The two global funcs UserExists() and CheckPass() are probably not needed,
// since the auth loop is done recursively inside the class code, upgrading
// the user class within itself.
// Note: When a higher user class is returned, this doesn't mean that the user
// is authorized, $user->_level is still low, and only upgraded on successful
// login.
//
// Revision 1.4  2003/12/07 19:29:48  carstenklapp
// Code Housecleaning: fixed syntax errors. (php -l *.php)
//
// Revision 1.3  2003/12/06 19:10:46  carstenklapp
// Finished off logic for determining user class, including
// PassUser. Removed ability of BogoUser to save prefs into a page.
//
// Revision 1.2  2003/12/03 21:45:48  carstenklapp
// Added admin user, password user, and preference classes. Added
// password checking functions for users and the admin. (Now the easy
// parts are nearly done).
//
// Revision 1.1  2003/12/02 05:46:36  carstenklapp
// Complete rewrite of WikiUser.php.
//
// This should make it easier to hook in user permission groups etc. some
// time in the future. Most importantly, to finally get UserPreferences
// fully working properly for all classes of users: AnonUser, BogoUser,
// AdminUser; whether they have a NamesakePage (PersonalHomePage) or not,
// want a cookie or not, and to bring back optional AutoLogin with the
// UserName stored in a cookie--something that was lost after PhpWiki had
// dropped the default http auth login method.
//
// Added WikiUser classes which will (almost) work together with existing
// UserPreferences class. Other parts of PhpWiki need to be updated yet
// before this code can be hooked up.
//

// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>
