<?php // -*-php-*-
// iso-8859-1

// IMPORTANT NOTE: Use of the ***configurator.php*** to generate an
// index.php is depreciated, because it is out of date and a new
// configuration system is in the works (see the config directory, not
// finished yet though). DO compare or diff the configurator's output
// against this file if you feel you must use it to generate an
// index.php!

/*
Copyright 1999,2000,2001,2002,2003,2004 $ThePhpWikiProgrammingTeam 
= array(
"Steve Wainstead", "Clifford A. Adams", "Lawrence Akka", 
"Scott R. Anderson", "Jon Åslund", "Neil Brown", "Jeff Dairiki",
"Stéphane Gourichon", "Jan Hidders", "Arno Hollosi", "John Jorgensen",
"Antti Kaihola", "Jeremie Kass", "Carsten Klapp", "Marco Milanesi",
"Grant Morgan", "Jan Nieuwenhuizen", "Aredridel Niothke", 
"Pablo Roca Rozas", "Sandino Araico Sánchez", "Joel Uckelman", 
"Reini Urban", "Tim Voght", "Jochen Kalmbach");

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


/////////////////////////////////////////////////////////////////////
/*
  This is the starting file for PhpWiki. All this file does is set
  configuration options, and at the end of the file it includes() the
  file lib/main.php, where the real action begins.

  This file is divided into seven parts: Parts Zero, One, Two, Three,
  Four, Five and Six. Each one has different configuration settings you can
  change; in all cases the default should work on your system,
  however, we recommend you tailor things to your particular setting.
*/

/////////////////////////////////////////////////////////////////////
// Part Zero: If PHP needs help in finding where you installed the
//   rest of the PhpWiki code, you can set the include_path here.

// Define PHP's include path so that it can find the PHP source code
// for this PhpWiki.
// 
// You shouldn't need to do this unless you've moved index.php out
// of the PhpWiki install directory.
//
// Note that on Windows-based servers, you should use ; rather than :
// as the path separator.
//ini_set('include_path', '.:/usr/local/httpd/phpwiki');

// Set DEBUG to 1 to view the XHTML and CSS validator icons, page
// processing timer, and possibly other debugging messages at the
// bottom of each page.
if (!defined('DEBUG')) define ('DEBUG', 0);
define('ENABLE_USER_NEW',true);    // this will disappear with 1.4.0
define('JS_SEARCHREPLACE',true);   // experimental edit feature

/////////////////////////////////////////////////////////////////////
// Part Null: Don't touch this!

define ('PHPWIKI_VERSION', '1.3.9');
require "lib/prepend.php";
rcs_id('$Id: index.php,v 1.1.1.1 2004-04-13 23:41:39 braden Exp $');

/////////////////////////////////////////////////////////////////////
//
// Part One:
// Authentication and security settings. See Part Three for more.
// 
/////////////////////////////////////////////////////////////////////

// The name of your wiki.
//
// This is used to generate a keywords meta tag in the HTML templates,
// in bookmark titles for any bookmarks made to pages in your wiki,
// and during RSS generation for the <title> of the RSS channel.
//
// To use your own logo and signature files, name them PhpWikiLogo.png
// and PhpWikiSignature.png and put them into themes/default/images
// (substituting "PhpWiki" in the filename with the name you define
// here).
//
// It is recommended this be a relatively short WikiWord like the
// InterWiki monikers found in the InterWikiMap. (For examples, see
// lib/interwiki.map).
if (!defined('WIKI_NAME')) define('WIKI_NAME', 'PhpWiki');

// Visitor Hostname Lookup
//
// If set, reverse dns lookups will be performed to attempt to convert
// the user's IP number into a host name, in the case where the http
// server does not do this.
if (!defined('ENABLE_REVERSE_DNS')) define('ENABLE_REVERSE_DNS', true);

// Username and password of administrator.
//
// Set these to your preferences. For heaven's sake pick a good
// password and use the passencrypt.php tool. See:
// http://wolfram.org/writing/howto/password.html
// 
// Log into the wiki with the admin user and password to lock, unlock,
// or remove pages and to perform other PhpWikiAdministration
// functions. On all other occasions you should simply log in with
// your regular WikiName.
if (!defined('ADMIN_USER')) define('ADMIN_USER', "");
if (!defined('ADMIN_PASSWD')) define('ADMIN_PASSWD', "");
// It is recommended to use the passencrypt.php utility to encode the
// admin password, in the unlikely event someone gains ftp or ssh
// access to the server and directory containing phpwiki. Once you
// have pasted the encrypted password into ADMIN_PASSWD, uncomment
// this next line.
//if (!defined('ENCRYPTED_PASSWD')) define('ENCRYPTED_PASSWD', true);

// Private ZIP Dumps of All Wiki Pages
//
// If true, only the admin user can make zip dumps. Otherwise anyone
// may download all wiki pages as a single zip archive.
if (!defined('ZIPDUMP_AUTH')) define('ZIPDUMP_AUTH', false);

// Define to false to disable the RawHtml plugin.
//if (!defined('ENABLE_RAW_HTML')) define('ENABLE_RAW_HTML', false);

// If you define this to true, (MIME-type) page-dumps (either zip dumps,
// or "dumps to directory" will be encoded using the quoted-printable
// encoding.  If you're actually thinking of mailing the raw page dumps,
// then this might be useful, since (among other things,) it ensures
// that all lines in the message body are under 80 characters in length.
//
// Also, setting this will cause a few additional mail headers
// to be generated, so that the resulting dumps are valid
// RFC 2822 e-mail messages.
//
// Probably you can just leave this set to false, in which case you get
// raw ('binary' content-encoding) page dumps.
if (!defined('STRICT_MAILABLE_PAGEDUMPS')) define('STRICT_MAILABLE_PAGEDUMPS', false);

// Here you can change the filename suffix used for XHTML page dumps.
// If you don't want any suffix just comment this out.
$HTML_DUMP_SUFFIX = '.html';

// The maximum file upload size.
if (!defined('MAX_UPLOAD_SIZE')) define('MAX_UPLOAD_SIZE', 16 * 1024 * 1024);

// If the last edit is older than MINOR_EDIT_TIMEOUT seconds, the
// default state for the "minor edit" checkbox on the edit page form
// will be off.
if (!defined('MINOR_EDIT_TIMEOUT')) define("MINOR_EDIT_TIMEOUT", 7 * 24 * 3600);

// Actions listed in this array will not be allowed. The complete list
// of actions can be found in lib/main.php within the function
// getActionDescription.
//$DisabledActions = array('dumpserial', 'loadfile');

// PhpWiki can generate an access_log (in "NCSA combined log" format)
// for you. If you want one, define this to the name of the log
// file. The server must have write access to the directory specified.
//define('ACCESS_LOG', '/var/tmp/wiki_access_log');


// By default PhpWiki will try to have PHP compress its output
// before sending it to the browser (if you have a recent enough
// version of PHP and the browser supports it.)
// Define COMPRESS_OUTPUT to false to prevent output compression.
// Define COMPRESS_OUTPUT to true to force output compression,
// even if we think your version of PHP does this in a buggy
// fashion.
// Leave it undefined to leave the choice up to PhpWiki.
//define('COMPRESS_OUTPUT', false);


// HTTP CACHE_CONTROL
//
// This controls how PhpWiki sets the HTTP cache control
// headers (Expires: and Cache-Control:) 
//
// Choose one of:
//
// NONE: This is roughly the old (pre 1.3.4) behavior.  PhpWiki will
//       instruct proxies and browsers never to cache PhpWiki output.
//
// STRICT: Cached pages will be invalidated whenever the database global
//       timestamp changes.  This should behave just like NONE (modulo
//       bugs in PhpWiki and your proxies and browsers), except that
//       things will be slightly more efficient.
//
// LOOSE: Cached pages will be invalidated whenever they are edited,
//       or, if the pages include plugins, when the plugin output could
//       concievably have changed.
//
//       Behavior should be much like STRICT, except that sometimes
//       wikilinks will show up as undefined (with the question mark)
//       when in fact they refer to (recently) created pages.
//       (Hitting your browsers reload or perhaps shift-reload button
//       should fix the problem.)
//
// ALLOW_STALE: Proxies and browsers will be allowed to used stale pages.
//       (The timeout for stale pages is controlled by CACHE_CONTROL_MAX_AGE.)
//
//       This setting will result in quirky behavior.  When you edit a
//       page your changes may not show up until you shift-reload the
//       page, etc...
//
//       This setting is generally not advisable, however it may be useful
//       in certain cases (e.g. if your wiki gets lots of page views,
//       and few edits by knowledgable people who won't freak over the quirks.)
//
// The default is currently LOOSE.
//
if (!defined('CACHE_CONTROL')) define('CACHE_CONTROL', 'LOOSE');

// Maximum page staleness, in seconds.
//
// This only has effect if CACHE_CONTROL is set to ALLOW_STALE.
if (!defined('CACHE_CONTROL_MAX_AGE')) define('CACHE_CONTROL_MAX_AGE', 600);


// MARKUP CACHING
//
// PhpWiki normally caches a preparsed version (i.e. mostly
// converted to HTML) of the most recent version of each page.
// (Parsing the wiki-markup takes a fair amount of CPU.)
//
// Define WIKIDB_NOCACHE_MARKUP to true to disable the
// caching of marked-up page content.
//
// Note that you can also disable markup caching on a per-page
// temporary basis by addinging a query arg of '?nocache=1'
// to the URL to the page.  (Use '?nocache=purge' to completely
// discard the cached version of the page.)
//
// You can also purge the cached markup globally by using the
// "Purge Markup Cache" button on the PhpWikiAdministration page.
//if (!defined('WIKIDB_NOCACHE_MARKUP')) define ('WIKIDB_NOCACHE_MARKUP', true);

/////////////////////////////////////////////////////////////////////
//
// Part Two:
// Database Selection
//
/////////////////////////////////////////////////////////////////////

//
// This array holds the parameters which select the database to use.
//
// Not all of these parameters are used by any particular DB backend.
//
$DBParams = array(
   // Select the database type:
   //
   // Choose ADODB or SQL to use an SQL database with ADODB or PEAR
   // respectively (both ADODB and PEAR libraries are already included
   // with PhpWiki).
   // Choose dba to use one of the standard UNIX dbm libraries.
   // Choose file to use a flat file database.
   //'dbtype' => 'ADODB',
   //'dbtype' => 'SQL',
   'dbtype'   => 'dba',
   //'dbtype' => 'file',
   //'dbtype' => 'cvs',
   
   // For SQL based backends, specify the database as a DSN
   // The most general form of a DSN looks like:
   //
   //   phptype(dbsyntax)://username:password@protocol+hostspec/database
   //
   // For a MySQL database, the following should work:
   //
   //   mysql://user:password@host/databasename
   //
   // To connect over a unix socket, use something like
   //
   //   mysql://user:password@unix(/path/to/socket)/databasename
   //
   //'dsn' => 'mysql://guest@unix(/var/lib/mysql/mysql.sock)/test',
   //'dsn' => 'mysql://guest:pass@localhost/test',
   //'dsn' => 'pgsql://localhost/test',

   // The common table prefix (see below) is added if defined.
   // Undefine this if you use dbtype = "cvs" or "file"
   'db_session_table'   => 'session',
   
   // Used by all DB types:

   // prefix for filenames or table names
   /* 
    * currently you MUST EDIT THE SQL file too (in the schemas/
    * directory because we aren't doing on the fly sql generation
    * during the installation.
   */
   //'prefix' => 'phpwiki_',
   
   // Used by either 'dba' or 'file' and must be writable by the web
   // server If you leave this as '/tmp' you will probably lose all
   // your files eventually
   'directory'     => "/tmp",

   // choose the type of DB database file to use; most GNU systems have gdbm
   'dba_handler'   => 'gdbm',   // Either of 'gdbm' or 'db2' work great for me.
   //'dba_handler' => 'db2',
   //'dba_handler' => 'db3',    // Works fine on Windows, but not on every linux.
   //'dba_handler' => 'dbm',    // On sf.net redhat there's dbm and gdbm.
                                // dbm suffers from limits on size of data items?

   'timeout'   => 20,
   //'timeout' => 5
);

/////////////////////////////////////////////////////////////////////
// PHP Session settings:
//

// Tested for dbtype: 'SQL', 'ADODB' and 'dba'. See schemas/mysql.sql, 
// schemas/sqlite.sql or schemas/psql.sql. 
// $DBParams['db_session_table'] must be defined.
if (!defined('USE_DB_SESSION') and 
    !empty($DBParams['db_session_table']))
  define('USE_DB_SESSION',true);

// If your php was compiled with --enable-trans-sid it tries to
// add a PHPSESSID query argument to all URL strings when cookie
// support isn't detected in the client browser.  For reasons
// which aren't entirely clear (PHP bug) this screws up the URLs
// generated by PhpWiki.  Therefore, transparent session ids
// should be disabled.  This next line does that.
//
// (At the present time, you will not be able to log-in to PhpWiki,
// unless your browser supports cookies.)
@ini_set('session.use_trans_sid', 0);

// The login code now uses PHP's session support. Usually, the default
// configuration of PHP is to store the session state information in
// /tmp. That probably will work fine, but fails e.g. on clustered
// servers where each server has their own distinct /tmp (this is the
// case on SourceForge's project web server.) You can specify an
// alternate directory in which to store state information like so
// (whatever user your httpd runs as must have read/write permission
// in this directory):

//ini_set('session.save_path', 'some_other_directory');

/////////////////////////////////////////////////////////////////////
//
// The next section controls how many old revisions of each page are
// kept in the database.
//
// There are two basic classes of revisions: major and minor. Which
// class a revision belongs in is determined by whether the author
// checked the "this is a minor revision" checkbox when they saved the
// page.
// 
// There is, additionally, a third class of revisions: author
// revisions. The most recent non-mergable revision from each distinct
// author is and author revision.
//
// The expiry parameters for each of those three classes of revisions
// can be adjusted seperately. For each class there are five
// parameters (usually, only two or three of the five are actually
// set) which control how long those revisions are kept in the
// database.
//
//   max_keep: If set, this specifies an absolute maximum for the
//             number of archived revisions of that class. This is
//             meant to be used as a safety cap when a non-zero
//             min_age is specified. It should be set relatively high,
//             and it's purpose is to prevent malicious or accidental
//             database overflow due to someone causing an
//             unreasonable number of edits in a short period of time.
//
//   min_age:  Revisions younger than this (based upon the supplanted
//             date) will be kept unless max_keep is exceeded. The age
//             should be specified in days. It should be a
//             non-negative, real number,
//
//   min_keep: At least this many revisions will be kept.
//
//   keep:     No more than this many revisions will be kept.
//
//   max_age:  No revision older than this age will be kept.
//
// Supplanted date: Revisions are timestamped at the instant that they
// cease being the current revision. Revision age is computed using
// this timestamp, not the edit time of the page.
//
// Merging: When a minor revision is deleted, if the preceding
// revision is by the same author, the minor revision is merged with
// the preceding revision before it is deleted. Essentially: this
// replaces the content (and supplanted timestamp) of the previous
// revision with the content after the merged minor edit, the rest of
// the page metadata for the preceding version (summary, mtime, ...)
// is not changed.
//
// Keep up to 8 major edits, but keep them no longer than a month.
$ExpireParams['major'] = array('max_age' => 32,
                               'keep'    => 8);
// Keep up to 4 minor edits, but keep them no longer than a week.
$ExpireParams['minor'] = array('max_age' => 7,
                               'keep'    => 4);
// Keep the latest contributions of the last 8 authors up to a year.
// Additionally, (in the case of a particularly active page) try to
// keep the latest contributions of all authors in the last week (even
// if there are more than eight of them,) but in no case keep more
// than twenty unique author revisions.
$ExpireParams['author'] = array('max_age'  => 365,
                                'keep'     => 8,
                                'min_age'  => 7,
                                'max_keep' => 20);

/////////////////////////////////////////////////////////////////////
//
// Part Three: (optional)
// User Authentication
//
/////////////////////////////////////////////////////////////////////
// 
// New user authentication configuration:
// We support three basic authentication methods and a stacked array 
// of advanced auth methods to get and check the passwords:
//
// ALLOW_ANON_USER         default true
// ALLOW_ANON_EDIT         default true
// ALLOW_BOGO_LOGIN        default true
// ALLOW_USER_PASSWORDS    default true

// allow anon users to view pages! (not edit)
if (!defined('ALLOW_ANON_USER')) define('ALLOW_ANON_USER', true); 
// allow anon users to edit pages
if (!defined('ALLOW_ANON_EDIT')) define('ALLOW_ANON_EDIT', true); 

// This was replaced by ALLOW_ANON_EDIT
if (!defined('REQUIRE_SIGNIN_BEFORE_EDIT')) define('REQUIRE_SIGNIN_BEFORE_EDIT', ! ALLOW_ANON_EDIT);

// If ALLOW_BOGO_LOGIN is true, users are allowed to login (with
// any/no password) using any userid which: 
//  1) is not the ADMIN_USER, and
//  2) is a valid WikiWord (matches $WikiNameRegexp.)
// If true, users may be created by themselves. Otherwise we need seperate auth. 
// If such a user will create a so called HomePage with his userid, he will 
// be able to store his preferences and password there.
if (!defined('ALLOW_BOGO_LOGIN')) define('ALLOW_BOGO_LOGIN', true);

// True User Authentication:
// To require user passwords:
//   ALLOW_ANON_USER = false
//   ALLOW_ANON_EDIT = false
//   ALLOW_BOGO_LOGIN = false,
//   ALLOW_USER_PASSWORDS = true.
// Otherwise any anon or bogo user might login without any or a wrong password.
if (!defined('ALLOW_USER_PASSWORDS')) define('ALLOW_USER_PASSWORDS', true);

// Below we define which methods exists and in which order 
// they are used:
//   BogoLogin:     WikiWord enough, but with PASSWORD_LENGTH_MINIMUM
//   PersonalPage:  Store passwords in the users homepage metadata (simple)
//   Db:            Use $DBAuthParams[] (see below) with the PearDB or 
//		    ADODB only.
//   LDAP:          Authenticate against LDAP_AUTH_HOST with LDAP_BASE_DN
//   IMAP:          Authenticate against IMAP_AUTH_HOST (email account)
//   POP3:          Authenticate against POP3_AUTH_HOST (email account)
//   File:          Store username:crypted-passwords in .htaccess like files. 
//                  Use Apache's htpasswd to manage this file.
//   HttpAuth:      Use the protection by the webserver (.htaccess) or 
//                  enforce it

if (defined('ALLOW_USER_PASSWORDS')) {

    // use the following order of authentication methods:
    if (!isset($USER_AUTH_ORDER))
        $USER_AUTH_ORDER = 
            array(
//                "BogoLogin",
                  "PersonalPage",
//                "HttpAuth",
                  "Db",
//		  "LDAP",    // define LDAP_AUTH_HOST and LDAP_BASE_DN
//                "IMAP",    // define IMAP_AUTH_HOST
//                "POP3",    // define POP3_AUTH_HOST
//                "File",    // define AUTH_USER_FILE and opt. AUTH_USER_FILE_STORABLE
                  ) ;

    if (!defined('PASSWORD_LENGTH_MINIMUM')) define('PASSWORD_LENGTH_MINIMUM', 2);
    
    if (!defined('USER_AUTH_POLICY'))
        //We support the following auth policies:
        // first-only: use only the first method in USER_AUTH_ORDER
        // old:        ignore USER_AUTH_ORDER and try to use all available 
        //             methods as in the previous PhpWiki releases (slow)
        // strict:     check if the user exists for all methods: 
        //             on the first existing user, try the password. 
        //             dont try the other methods on failure then
        // stacked:    check the given user - password combination for all methods 
	//             and return true on the first success.

        //define('USER_AUTH_POLICY','first-only');
        define('USER_AUTH_POLICY','old');
        //define('USER_AUTH_POLICY','strict');
        //define('USER_AUTH_POLICY','stacked');
}

// LDAP Auth:
if (!defined('LDAP_AUTH_HOST'))   define('LDAP_AUTH_HOST', "ldap://localhost:389");
//                                                      or "ldaps://server:636"
// The organizational or domain BASE DN: e.g. "dc=mydomain,dc=com" to restrict the search
// Note: ou=Users and ou=Groups are used for GroupLdap Membership
if (!defined('LDAP_BASE_DN')) define('LDAP_BASE_DN', "ou=Users,o=Development,dc=mycompany.com");
// LDAP Auth Optional:
// Some LDAP servers disallow anonymous binds, and need some more options,
// such as for the Windows Active Directory Server:
// $LDAP_SET_OPTION = array('LDAP_OPT_PROTOCOL_VERSION' => 3,
//                          'LDAP_OPT_REFERRALS' => 0);
// define(LDAP_AUTH_USER, "CN=ldapuser,CN=Users,DC=uai,DC=int");
// define(LDAP_AUTH_PASSWORD, '');
// define(LDAP_SEARCH_FIELD, 'sAMAccountName'); // might be different from uid, 
//                                                 here's its a Windows/Samba account

// IMAP auth: 
//   check userid/passwords from a imap server, defaults to localhost
if (!defined('IMAP_AUTH_HOST'))   define('IMAP_AUTH_HOST', 'localhost:143/imap/notls');
// Some IMAP_AUTH_HOST samples:
//   "localhost", "localhost:143/imap/notls", 
//   "localhost:993/imap/ssl/novalidate-cert" (SuSE refuses non-SSL conections)

// POP3 auth:
//if (!defined('POP3_AUTH_HOST'))   define('POP3_AUTH_HOST', 'localhost');
//if (!defined('POP3_AUTH_PORT'))   define('POP3_AUTH_PORT', '110');

// File auth:
//if (!defined('AUTH_USER_FILE')) define('AUTH_USER_FILE', '/etc/shadow'); // or '/etc/httpd/.htpasswd'
// set this to true if the user may change his password into this file.
//if (!defined('AUTH_USER_FILE_STORABLE')) define('AUTH_USER_FILE_STORABLE',false);

// Group membership:
//if (!defined('GROUP_METHOD')) define('GROUP_METHOD', "NONE");
if (!defined('GROUP_METHOD')) define('GROUP_METHOD', "WIKIPAGE");
//if (!defined('GROUP_METHOD')) define('GROUP_METHOD', "DB");
//if (!defined('GROUP_METHOD')) define('GROUP_METHOD', "FILE");
//if (!defined('GROUP_METHOD')) define('GROUP_METHOD', "LDAP");
//if (!defined('AUTH_GROUP_FILE')) define('AUTH_GROUP_FILE', '/etc/groups'); // or '/etc/httpd/.htgroup'

// Seperate DB User Authentication.
//   Can be external, like radius, phpnuke, courier authmysql,
//   apache auth_mysql or something else.
// The default is to store the data as metadata in the users PersonalPage.
// The most likely dsn option is the same dsn as the wikipages.
// 
// Note: Order of variables important!
$DBAuthParams = array (
   // if not defined use $DBParams['dsn'] i.e. the phpwiki database
   //'auth_dsn'         => 'mysql://localhost/phpwiki',

   // USER => PASSWORD
   // plaintext passwords:
   //  'auth_check'  => 'SELECT IF(passwd="$password",1,0) as ok FROM user WHERE userid="$userid"',
   // database (md5) passwords (more secure):
   'auth_check'  => 'SELECT IF(passwd=PASSWORD("$password"),1,0) as ok FROM user WHERE userid="$userid"',
   // crypt passwords:
   //'auth_check'  => 'SELECT password as password FROM user WHERE userid="$userid"',
   // this is only needed with auth_crypt_method plain:
   'auth_user_exists'  => 'SELECT userid FROM user WHERE userid="$userid"',

   //'auth_crypt_method'  => 'crypt',     // 'crypt' (unix)
   'auth_crypt_method'  => 'plain',       // plain or secure mysql PASSWORD()

   // If 'auth_update' is not defined but 'auth_check' is defined, the user cannot 
   // change his password.
   // $password is processed  by the 'auth_crypt_method'
   //'auth_update'  => 'UPDATE user SET passwd="$password" WHERE userid="$userid"',
   // for mysql md5 use 'auth_crypt_method'  => 'plain'
   'auth_update'  => 'UPDATE user SET passwd=PASSWORD("$password") WHERE userid="$userid"',

   // Let a user create himself. Generally in external databases not wanted.
   // Not yet tested!
   //'auth_create'  => 'INSERT INTO user SET passwd=PASSWORD("$password"),userid="$userid"',

   // USER => PREFERENCES
   //   This can be optionally defined in the phpwiki db.
   //   The default is to store it the users homepage.
   // If you choose the user table, only registered user get their prefs from the DB,
   // self-created users not. Better use the special pref table.
   //'pref_select' => 'SELECT prefs FROM user WHERE userid="$userid"',
   'pref_select' => 'SELECT prefs FROM pref WHERE userid="$userid"',
   //Don't use replace with user or all other fields here get erased! (passwords e.g.)
   //'pref_update' => 'UPDATE user SET prefs="$pref_blob" WHERE userid="$userid"',
   // The special prefs table is safe to erase. All users can store their prefs here.
   'pref_update' => 'REPLACE INTO pref SET prefs="$pref_blob",userid="$userid"',

   // USERS <=> GROUPS
   //   DB methods for lib/WikiGroup.php, see also AUTH_GROUP_FILE above.
   // You can define 1:n or n:m user<=>group relations, as you wish.
   // Sample configurations
   //   only one group per user (1:n):
   //'is_member' => 'SELECT user FROM user WHERE user="$userid" AND group="$groupname"',
   //'group_members' => 'SELECT user FROM user WHERE group="$groupname"',
   //'user_groups' => 'SELECT group FROM user WHERE user="$userid"',
   //   or multiple groups per user (n:m):
   'is_member' => 'SELECT userid FROM member WHERE userid="$userid" AND groupname="$groupname"',
   'group_members' => 'SELECT DISTINCT userid FROM member WHERE groupname="$groupname"',
   'user_groups' => 'SELECT groupname FROM member WHERE userid="$userid"',

   'dummy' => false,
);

// Old settings, only needed for ENABLE_USER_NEW = false
//
// The wiki can be protected by HTTP Auth. Use the username and password 
// from there, but this is not sufficient. Try the other methods also.
if (!defined('ALLOW_HTTP_AUTH_LOGIN')) define('ALLOW_HTTP_AUTH_LOGIN', false);
// If ALLOW_USER_LOGIN is true, any defined internal and external
// authentication method is tried. If not, we don't care about
// passwords, but listen to the next two constants. Note that external
// authentication is not supported at this time, you will likely have
// to patch code yourself to get PhpWiki to recognise userids and
// passwords from an external source.
if (!defined('ALLOW_USER_LOGIN')) define('ALLOW_USER_LOGIN', false);
// also only if ENABLE_USER_NEW = false:
//if (!defined('ALLOW_LDAP_LOGIN')) define('ALLOW_LDAP_LOGIN', true and function_exists('ldap_connect'));
//if (!defined('ALLOW_IMAP_LOGIN')) define('ALLOW_IMAP_LOGIN', true and function_exists('imap_open'));

// A interim page which gets displayed on every edit attempt
//if (!defined('EDITING_POLICY')) define('EDITING_POLICY', "EditingPolicy");


/////////////////////////////////////////////////////////////////////
//
// Part Four:
// Page appearance and layout
//
/////////////////////////////////////////////////////////////////////

/* THEME
 *
 * Most of the page appearance is controlled by files in the theme
 * subdirectory.
 *
 * There are a number of pre-defined themes shipped with PhpWiki.
 * Or you may create your own (e.g. by copying and then modifying one of
 * stock themes.)
 *
 * Pick one.
 */
if (!defined('THEME')) {
define('THEME', 'default');
//define('THEME', 'MacOSX');
//define('THEME', 'smaller');
//define('THEME', 'Wordpress');
//define('THEME', 'Portland');
//define('THEME', 'Hawaiian');
//define('THEME', 'Sidebar');
//define('THEME', 'SpaceWiki');
//define('THEME', 'wikilens');
}

// Select a valid charset name to be inserted into the xml/html pages,
// and to reference links to the stylesheets (css). For more info see:
// <http://www.iana.org/assignments/character-sets>. Note that PhpWiki
// has been extensively tested only with the latin1 (iso-8859-1)
// character set.
//
// If you change the default from iso-8859-1 PhpWiki may not work
// properly and will require code modifications, at the very least you
// will have to convert the files in pgsrc or locale/xx/pgsrc to
// match!
//
// Character sets similar to iso-8859-1 may work with little or no
// modification depending on your setup. The database must also
// support the same charset, and of course the same is true for the
// web browser. (Some work is in progress hopefully to allow more
// flexibility in this area in the future).
// Note: For $GLOBALS['LANG']="ja" CHARSET "utf-8" must be defined.
if (!defined('CHARSET')) define("CHARSET", "iso-8859-1");

// Select your language/locale - default language is "en" for English.
// Other languages available:
// English "en"  (English    - HomePage)
// Dutch   "nl" (Nederlands - ThuisPagina)
// Spanish "es" (Español    - PáginaPrincipal)
// French  "fr" (Français   - Accueil)
// German  "de" (Deutsch    - StartSeite)
// Swedish "sv" (Svenska    - Framsida)
// Italian "it" (Italiano   - PaginaPrincipale)
// Japanese "ja" (Japanese   - ¥Û¡¼¥à¥Ú¡¼¥¸)
//
// If you set DEFAULT_LANGUAGE to the empty string, your system's
// default language (as determined by the applicable environment
// variables) will be used.
// Note: The users language will be defined in $GLOBALS['LANG'], 
// which overrides the DEFAULT_LANGUAGE. See wiki.
//
// Note: User-specified languages will set $GLOBALS['LANG'], this is just 
// the system default.
if (!defined('DEFAULT_LANGUAGE')) define('DEFAULT_LANGUAGE', 'en');

/* WIKI_PGSRC -- specifies the source for the initial page contents of
 * the Wiki. The setting of WIKI_PGSRC only has effect when the wiki is
 * accessed for the first time (or after clearing the database.)
 * WIKI_PGSRC can either name a directory or a zip file. In either case
 * WIKI_PGSRC is scanned for files -- one file per page.
 */
if (!defined('WIKI_PGSRC')) define('WIKI_PGSRC', "pgsrc"); // Default (old) behavior.
//define('WIKI_PGSRC', 'wiki.zip'); // New style.
//define('WIKI_PGSRC', '../../../Logs/Hamwiki/hamwiki-20010830.zip'); // New style.

/*
 * DEFAULT_WIKI_PGSRC is only used when the language is *not* the
 * default (English) and when reading from a directory: in that case
 * some English pages are inserted into the wiki as well.
 * DEFAULT_WIKI_PGSRC defines where the English pages reside.
 */
// FIXME: is this really needed?  Can't we just copy these pages into
// the localized pgsrc?
define('DEFAULT_WIKI_PGSRC', "pgsrc");
// These are the pages which will get loaded from DEFAULT_WIKI_PGSRC.	
$GenericPages = array("ReleaseNotes", "SteveWainstead", "TestPage");

/////////////////////////////////////////////////////////////////////
//
// Part Five:
// Mark-up options.
// 
/////////////////////////////////////////////////////////////////////

// allowed protocols for links - be careful not to allow "javascript:"
// URL of these types will be automatically linked.
// within a named link [name|uri] one more protocol is defined: phpwiki
$AllowedProtocols = "http|https|mailto|ftp|news|nntp|ssh|gopher";

// URLs ending with the following extension should be inlined as images
$InlineImages = "png|jpg|gif";

// Perl regexp for WikiNames ("bumpy words")
// (?<!..) & (?!...) used instead of '\b' because \b matches '_' as well
$WikiNameRegexp = "(?<![[:alnum:]])(?:[[:upper:]][[:lower:]]+){2,}(?![[:alnum:]])";

// Defaults to '/', but '.' was also used.
if (!defined('SUBPAGE_SEPARATOR')) define('SUBPAGE_SEPARATOR', '/');

// InterWiki linking -- wiki-style links to other wikis on the web
//
// The map will be taken from a page name InterWikiMap.
// If that page is not found (or is not locked), or map
// data can not be found in it, then the file specified
// by INTERWIKI_MAP_FILE (if any) will be used.
define('INTERWIKI_MAP_FILE', "lib/interwiki.map");

// Display a warning if the internal lib/interwiki.map is used, and 
// not the public InterWikiMap page. This file is not readable from outside.
//define('WARN_NONPUBLIC_INTERWIKIMAP', false);

// Regexp used for automatic keyword extraction.
//
// Any links on a page to pages whose names match this regexp will
// be used keywords in the keywords meta tag.  (This is an aid to
// classification by search engines.)  The value of the match is
// used as the keyword.
//
// The default behavior is to match Category* and Topic* links.
$keywords = array("Category", "Topic");
$KeywordLinkRegexp = '(?<=^'. join('|^', $keywords) . ')[[:upper:]].*$';

// Author and Copyright Site Navigation Links
//
// These will be inserted as <link rel> tags in the html header of
// every page, for search engines and for browsers like Mozilla which
// take advantage of link rel site navigation.
//
// If you have your own copyright and contact information pages change
// these as appropriate.
if (!defined('COPYRIGHTPAGE_TITLE')) define('COPYRIGHTPAGE_TITLE',
    "GNU General Public License");
if (!defined('COPYRIGHTPAGE_URL')) define('COPYRIGHTPAGE_URL',
    'http://www.gnu.org/copyleft/gpl.html#SEC1');
if (!defined('AUTHORPAGE_TITLE')) define('AUTHORPAGE_TITLE',
    "The PhpWiki Programming Team");
if (!defined('AUTHORPAGE_URL')) define('AUTHORPAGE_URL',
    'http://phpwiki.sourceforge.net/phpwiki/ThePhpWikiProgrammingTeam');


/////////////////////////////////////////////////////////////////////
//
// Part Six:
// URL options -- you can probably skip this section.
//
/////////////////////////////////////////////////////////////////////
/******************************************************************
 *
 * The following section contains settings which you can use to tailor
 * the URLs which PhpWiki generates.
 *
 * Any of these parameters which are left undefined will be deduced
 * automatically. You need only set them explicitly if the
 * auto-detected values prove to be incorrect.
 *
 * In most cases the auto-detected values should work fine, so
 * hopefully you don't need to mess with this section.
 *
 * In case of local overrides of short placeholders, which themselves 
 * include index.php, we check for most constants. See '/wiki'.
 * We can override DATA_PATH and PHPWIKI_DIR to support multiple phpwiki 
 * versions (for development), but most likely other values like 
 * THEME, $LANG and $DbParams for a WikiFarm.
 *
 ******************************************************************/

/*
 * Canonical name and httpd port of the server on which this PhpWiki
 * resides.
 */
//if (!defined('SERVER_NAME')) define('SERVER_NAME', 'some.host.com');
//define('SERVER_PORT', 80);

/*
 * Relative URL (from the server root) of the PhpWiki
 * script.
 */
//if (!defined('SCRIPT_NAME')) define('SCRIPT_NAME', '/some/where/index.php');

/*
 * URL of the PhpWiki install directory.  (You only need to set this
 * if you've moved index.php out of the install directory.)  This can
 * be either a relative URL (from the directory where the top-level
 * PhpWiki script is) or an absolute one.
 */
//if (!defined('DATA_PATH')) define('DATA_PATH', '/home/user/phpwiki');

/*
 * Path to the PhpWiki install directory.  This is the local
 * filesystem counterpart to DATA_PATH.  (If you have to set
 * DATA_PATH, your probably have to set this as well.)  This can be
 * either an absolute path, or a relative path interpreted from the
 * directory where the top-level PhpWiki script (normally index.php)
 * resides.
 */
//if (!defined('PHPWIKI_DIR')) define('PHPWIKI_DIR', 'C:/Apache/phpwiki');
//if (!defined('PHPWIKI_DIR')) define('PHPWIKI_DIR', '/home/user/public_html/phpwiki');

/*
 * PhpWiki will try to use short urls to pages, eg 
 * http://www.example.com/index.php/HomePage
 * If you want to use urls like 
 * http://www.example.com/index.php?pagename=HomePage
 * then define 'USE_PATH_INFO' as false by uncommenting the line below.
 * NB:  If you are using Apache >= 2.0.30, then you may need to to use
 * the directive "AcceptPathInfo On" in your Apache configuration file
 * (or in an appropriate <.htaccess> file) for the short urls to work:  
 * See http://httpd.apache.org/docs-2.0/mod/core.html#acceptpathinfo
 * 
 * See also http://phpwiki.sourceforge.net/phpwiki/PrettyWiki for more ideas
 * on prettifying your urls.
 *
 * Default: PhpWiki will try to divine whether use of PATH_INFO
 * is supported in by your webserver/PHP configuration, and will
 * use PATH_INFO if it thinks that is possible.
 */
//if (!defined('USE_PATH_INFO')) define('USE_PATH_INFO', false);

/*
 * VIRTUAL_PATH is the canonical URL path under which your your wiki
 * appears. Normally this is the same as dirname(SCRIPT_NAME), however
 * using, e.g. apaches mod_actions (or mod_rewrite), you can make it
 * something different.
 *
 * If you do this, you should set VIRTUAL_PATH here.
 *
 * E.g. your phpwiki might be installed at at /scripts/phpwiki/index.php,
 * but  * you've made it accessible through eg. /wiki/HomePage.
 *
 * One way to do this is to create a directory named 'wiki' in your
 * server root. The directory contains only one file: an .htaccess
 * file which reads something like:
 *
 *    Action x-phpwiki-page /scripts/phpwiki/index.php
 *    SetHandler x-phpwiki-page
 *    DirectoryIndex /scripts/phpwiki/index.php
 *
 * In that case you should set VIRTUAL_PATH to '/wiki'.
 *
 * (VIRTUAL_PATH is only used if USE_PATH_INFO is true.)
 */
//if (!defined('VIRTUAL_PATH')) define('VIRTUAL_PATH', '/SomeWiki');

/////////////////////////////////////////////////////////////////////
//
// Part Seven:
// Miscellaneous settings
//
/////////////////////////////////////////////////////////////////////

/*
 * Disable HTTP redirects.
 *
 * (You probably don't need to touch this.)
 *
 * PhpWiki uses HTTP redirects for some of it's functionality.
 * (e.g. after saving changes, PhpWiki redirects your browser to
 * view the page you just saved.)
 *
 * Some web service providers (notably free European Lycos) don't seem to
 * allow these redirects.  (On Lycos the result in an "Internal Server Error"
 * report.)  In that case you can set DISABLE_HTTP_REDIRECT to true.
 * (In which case, PhpWiki will revert to sneakier tricks to try to
 * redirect the browser...)
 */
//if (!defined('DISABLE_HTTP_REDIRECT')) define ('DISABLE_HTTP_REDIRECT', true);

//if (defined('WIKI_SOAP') and WIKI_SOAP) return;

////////////////////////////////////////////////////////////////
// PrettyWiki
// Check if we were included by some other wiki version 
// (getimg.php, en, de, wiki, ...) or not. 
// If the server requested this index.php fire up the code by loading lib/main.php.
// Parallel wiki scripts can now simply include /index.php for the 
// main configuration, extend or redefine some settings and 
// load lib/main.php by themselves. See the file 'wiki'.
// This overcomes the IndexAsConfigProblem.
// Generally a simple 
//   define('VIRTUAL_PATH', $_SERVER['SCRIPT_NAME']);
// is enough in the wiki file, plus the action definition in a .htaccess file
////////////////////////////////////////////////////////////////

// If your lib/main.php is not loaded, comment that out, and  
// uncomment the include "lib/main.php" line below.
if (defined('VIRTUAL_PATH') and defined('USE_PATH_INFO')) {
    if ($HTTP_SERVER_VARS['SCRIPT_NAME'] != VIRTUAL_PATH) {
        include "lib/main.php";
    }
    elseif (defined('SCRIPT_NAME') and 
            ($HTTP_SERVER_VARS['SCRIPT_NAME'] != SCRIPT_NAME)) {
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
//include "lib/main.php";

// $Log: not supported by cvs2svn $
// Revision 1.138  2004/04/12 12:27:07  rurban
// more notes and themes
//
// Revision 1.137  2004/04/11 10:42:02  rurban
// pgsrc/CreatePagePlugin
//
// Revision 1.136  2004/04/10 04:14:13  rurban
// sf.net 906436 Suggestion
//
// Revision 1.135  2004/04/10 03:33:03  rurban
// Oops revert
//
// Revision 1.134  2004/04/10 02:55:48  rurban
// fixed old WikiUser
//
// Revision 1.133  2004/04/08 01:22:53  rurban
// fixed PageChange Notification
//
// Revision 1.132  2004/04/01 15:57:10  rurban
// simplified Sidebar theme: table, not absolute css positioning
// added the new box methods.
// remaining problems: large left margin, how to override _autosplitWikiWords in Template only
//
// Revision 1.131  2004/03/14 16:24:35  rurban
// authenti(fi)cation spelling
//
// Revision 1.130  2004/03/09 17:16:43  rurban
// fixed $LDAP_SET_OPTION
//
// Revision 1.129  2004/02/29 04:10:55  rurban
// new POP3 auth (thanks to BiloBilo: pentothal at despammed dot com)
// fixed syntax error in index.php
//
// Revision 1.128  2004/02/29 02:06:05  rurban
// And this is the SOAP server. Just a view methods for now. (page content)
// I would like to see common-wiki soap wdsl.
//
// "SOAP is a bloated, over engineered mess of a perfectly trivial concept. Sigh."
//   -- http://www.wlug.org.nz/SOAP
//
// Revision 1.127  2004/02/28 21:18:29  rurban
// new SQL auth_create, don't ever use REPLACE sql calls!
// moved HttpAuth to the end of the chain
// PrettyWiki enabled again
//
// Revision 1.126  2004/02/27 16:27:48  rurban
// REPLACE is a dirty hack, and erases passwd btw.
//
// Revision 1.125  2004/02/24 02:51:57  rurban
// release 1.3.8 ready
//
// Revision 1.124  2004/02/16 00:20:30  rurban
// new Japanses language
//
// Revision 1.123  2004/02/09 03:58:07  rurban
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
// Revision 1.122  2004/02/07 14:20:18  rurban
// consistent mysql schema with index.php (userid)
//
// Revision 1.121  2004/02/07 10:41:25  rurban
// fixed auth from session (still double code but works)
// fixed GroupDB
// fixed DbPassUser upgrade and policy=old
// added GroupLdap
//
// Revision 1.120  2004/02/03 09:45:39  rurban
// LDAP cleanup, start of new Pref classes
//
// Revision 1.119  2004/02/01 09:14:10  rurban
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
// Revision 1.118  2004/01/28 14:34:13  rurban
// session table takes the common prefix
// + various minor stuff
// reallow password changing
//
// Revision 1.117  2004/01/27 23:25:50  rurban
// added new tables to mysql schema
// fixed default DBAUthParam samples to match these
// added group constants (look terrible, I'd prefer strings instead of constants)
//
// Revision 1.116  2004/01/25 04:21:02  rurban
// WikiUserNew support (temp. ENABLE_USER_NEW constant)
//
// Revision 1.115  2003/12/22 04:58:11  carstenklapp
// Incremented release version.
//
// Revision 1.114  2003/12/05 16:00:42  carstenklapp
// ACK! gettext is not available at this point in index.php.
//
// Revision 1.113  2003/12/05 15:51:37  carstenklapp
// Added note that use of the configurator is depreciated.
//
// Enable localization/gettextification of $KeywordLinkRegexp. (Also, now
// users not familiar with regex can more easily just edit the $keywords
// array).
//
// Added four new constants to define author and copyright link rel~s
// used in html head. This makes it easier to run multiple wikis off of
// one set of code.
//
// Eliminated RECENT_CHANGES constant for RSS auto discovery because it's
// another step to watch out for when running a non-english wiki. Now
// simply defined as _("RecentChanges") in head.tmpl itself. Non-standard
// wikis where the RecentChanges page has been named to something else
// will have to modify this in head.tmpl (along with all other places the
// word RecentChanges appears in the code, something that already would
// have had to be done on such a wiki anyway).
//
// Added a little more info and instructions to flesh out:
// DEBUG, WIKI_NAME, ADMIN_USER, $DisabledActions, $DBParams, CHARSET.
//
// A few typos and spelling mistakes corrected, and some text rewrapped.
//
// Revision 1.112  2003/11/17 15:49:21  carstenklapp
// Updated version number to 1.3.7pre (beyond current release
// 1.3.6). Disabled DEBUG output by default (hide DebugInfo, XHTML &
// CSS validator buttons). Note the DebugInfo button remains visible
// for the Admin, and can be accessed by anyone else by adding
// "?action=DebugInfo" to the URL for the occasional use.
//
// Revision 1.111  2003/03/18 21:40:04  dairiki
// Copy Lawrence's memo on USE_PATH_INFO/AcceptPathInfo to configurator.php
// (as promised).
//
// Plus slight clarification of default (auto-detect) behavior.
//
// Revision 1.110  2003/03/18 20:51:10  lakka
// Revised comments on use of USE_PATH_INFO with Apache 2
//
// Revision 1.109  2003/03/17 21:24:50  dairiki
// Fix security bugs in the RawHtml plugin.
//
// Change the default configuration to allow use of plugin, since
// I believe the plugin is now safe for general use. (Raw HTML will only
// work on locked pages.)
//
// Revision 1.108  2003/03/07 22:47:01  dairiki
// A few more if(!defined(...))'s
//
// Revision 1.107  2003/03/07 20:51:54  dairiki
// New feature: Automatic extraction of keywords (for the meta keywords tag)
// from Category* and Topic* links on each page.
//
// Revision 1.106  2003/03/07 02:48:23  dairiki
// Add option to prevent HTTP redirect.
//
// Revision 1.105  2003/03/04 02:08:08  dairiki
// Fix and document the WIKIDB_NOCACHE_MARKUP config define.
//
// Revision 1.104  2003/02/26 02:55:52  dairiki
// New config settings in index.php to control cache control strictness.
//
// Revision 1.103  2003/02/22 19:43:50  dairiki
// Fix comment regarding connecting to SQL server over a unix socket.
//
// Revision 1.102  2003/02/22 18:53:38  dairiki
// Renamed method Request::compress_output to Request::buffer_output.
//
// Added config option to disable compression.
//
// Revision 1.101  2003/02/21 19:29:30  dairiki
// Update PHPWIKI_VERSION to 1.3.5pre.
//
// Revision 1.100  2003/01/04 03:36:58  wainstead
// Added 'file' as a database type alongside 'dbm'; added cvs log tag
//

// (c-file-style: "gnu")
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:   
?>