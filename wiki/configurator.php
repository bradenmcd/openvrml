<?php 
/*
 * Todo: 
 * validate input (fix javascript, add POST checks)
 * start this automatically the first time
 * fix include_path
 * eval index-user.php or index.php to get the actual settings.
 * ask to store it in index.php or index-user.php
 * 
 * The file index-user.php will be generated which you can use as your index.php.
 */

$tdwidth = 700;
$config_file = 'index-user.php';
$fs_config_file = dirname(__FILE__) . (substr(PHP_OS,0,3) == 'WIN' ? '\\' : "/") . $config_file;
if (isset($HTTP_POST_VARS['create']))  header('Location: configurator.php?create=1#create');
printf("<?xml version=\"1.0\" encoding=\"%s\"?>\n", 'iso-8859-1'); 
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<!-- $Id: configurator.php,v 1.1.1.1 2004-04-13 23:41:39 braden Exp $ -->
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<title>Configuration tool for PhpWiki 1.3.x</title>
<style type="text/css" media="screen">
<!--
/* TABLE { border: thin solid black } */
body { font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 80%; }
pre { font-size: 120%; }
td { border: thin solid black }
tr { border: none }
tr.hidden { border: none; display: none; }
td.part { background-color: #eeaaaa; color: inherit; }
td.instructions { background-color: #ffffee; width: <?php echo $tdwidth ?>px; color: inherit; }
td.unchangeable-variable-top   { border-bottom: none; background-color: #eeeeee; color:inherit; }
td.unchangeable-variable-left  { border-top: none; background-color: #eeeeee; color:inherit; }
-->
</style>
<script language="JavaScript" type="text/javascript">
<!--
function update(accepted, error, value, output) {
  if (accepted) {
    document.getElementById(output).innerHTML = "<font color=\"green\">Input accepted.</font>";
  } else {
    while ((index = error.indexOf("%s")) > -1) {
      error = error.substring(0, index) + value + error.substring(index+2);
    }
    document.getElementById(output).innerHTML = "<font color=\"red\">" + error + "</font>";
  }
}

function validate(error, value, output, field) {
  update(field.value == value, error, field.value, output);
}

function validate_ereg(error, ereg, output, field) {
  regex = new RegExp(ereg);
  update(regex.test(field.value), error, field.value, output);
}

function validate_range(error, low, high, empty_ok, output, field) {
  update((empty_ok == 1 && field.value == "") ||
         (field.value >= low && field.value <= high),
         error, field.value, output);
}

function toggle_group(id) {
  var text = document.getElementById(id + "_text");
  var do_hide = false;
  if (text.innerHTML == "Hide options.") {
    do_hide = true;
    text.innerHTML = "Show options.";
  } else {
    text.innerHTML = "Hide options.";
  }

  var rows = document.getElementsByTagName('tr');
  var i = 0;
  for (i = 0; i < rows.length; i++) {
    var tr = rows[i];
    if (tr.className == 'header' && tr.id == id) {
      i++;
      break;
    }
  }
  for (; i < rows.length; i++) {
    var tr = rows[i];
    if (tr.className == 'header')
      break;
    tr.className = do_hide ? 'hidden': 'nonhidden';
  }
}

function do_init() {
  // Hide all groups.  We do this via JavaScript to avoid
  // hiding the groups if JavaScript is not supported...
  var rows = document.getElementsByTagName('tr');
  for (var i = 0; i < rows.length; i++) {
    var tr = rows[i];
    if (tr.className == 'header')
      toggle_group(tr.id);
  }

  // Select text in textarea upon focus
  var area = document.getElementById('config-output');
  if (area) {
    listener = { handleEvent: function (e) { area.select(); } };
    area.addEventListener('focus', listener, false);
  }
}
  
-->
</script>
</head>
<body onload="do_init();">

<h1>Configuration tool for PhpWiki 1.3.x</h1>

<?php
//define('DEBUG', 1);
/**
 * The Configurator is a php script to aid in the configuration of PhpWiki.
 * Parts of this file are based on PHPWeather's configurator.php file.
 * http://sourceforge.net/projects/phpweather/
 *
 *
 * TO CHANGE THE CONFIGURATION OF YOUR PHPWIKI, DO *NOT* MODIFY THIS FILE!
 * more instructions go here
 *
 * Todo: 
 *   * start this automatically the first time
 *   * fix include_path
 *   * eval index.php to get the actual settings.
 *   * ask to store it in index.php or settings.php
 * 
 * The file settings.php will be generated which you can use as your index.php.
 */


//////////////////////////////
// begin configuration options


/**
 * Notes for the description parameter of $property:
 *
 * - Descriptive text will be changed into comments (preceeded by //)
 *   for the final output to index.php.
 *
 * - Only a limited set of html is allowed: pre, dl dt dd; it will be
 *   stripped from the final output.
 *
 * - Line breaks and spacing will be preserved for the final output.
 *
 * - Double line breaks are automatically converted to paragraphs
 *   for the html version of the descriptive text.
 *
 * - Double-quotes and dollar signs in the descriptive text must be
 *   escaped: \" and \$. Instead of escaping double-quotes you can use 
 *   single (') quotes for the enclosing quotes. 
 *
 * - Special characters like < and > must use html entities,
 *   they will be converted back to characters for the final output.
 */

$SEPARATOR = "///////////////////////////////////////////////////////////////////";

$copyright = '
Copyright 1999, 2000, 2001, 2002 $ThePhpWikiProgrammingTeam = array(
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
';



$preamble = "
  This is the starting file for PhpWiki. All this file does is set
  configuration options, and at the end of the file it includes() the
  file lib/main.php, where the real action begins.

  If you include this file to override the default settings here, 
  you must also include lib/main.php.

  This file is divided into seven parts: Parts Zero, One, Two, Three,
  Four, Five and Six. Each one has different configuration settings you can
  change; in all cases the default should work on your system,
  however, we recommend you tailor things to your particular setting.
";



$properties["Part Zero"] =
new part('_part0', false, "
Part Zero: If PHP needs help in finding where you installed the
rest of the PhpWiki code, you can set the include_path here.");

if (substr(PHP_OS,0,3) == 'WIN') {
    $include_path = dirname(__FILE__) . ';' . ini_get('include_path');
    if (strchr(ini_get('include_path'),'/'))
	$include_path = strtr($include_path,'\\','/');
} else {
    $include_path = dirname(__FILE__) . ':' . ini_get('include_path');
}

$properties["PHP include_path"] =
new _ini_set('include_path', $include_path, "
Define PHP's include path so that it can find the PHP source code
for this PhpWiki.

You shouldn't need to do this unless you've moved index.php out
of the PhpWiki install directory.

Note that on Windows-based servers, you should use ; rather than :
as the path separator.");

$properties["Part Null"] =
new part('_partnullheader', "", "
Part Null: Don't touch this!");



$properties["Part Null Settings"] =
new unchangeable_variable('_partnullsettings', "
define ('PHPWIKI_VERSION', '1.3.8pre');
require \"lib/prepend.php\";
rcs_id('\$Id: configurator.php,v 1.1.1.1 2004-04-13 23:41:39 braden Exp $');", "");

$properties["Part One"] =
new part('_partone', $SEPARATOR."\n", "

Part One:
Authentication and security settings. See Part Three for more.
");



$properties["Wiki Name"] =
new _define_optional('WIKI_NAME', 'PhpWiki', "
The name of your wiki.
This is used to generate a keywords meta tag in the HTML templates,
in bookmark titles for any bookmarks made to pages in your wiki,
and during RSS generation for the title of the RSS channel.");


$properties["Reverse DNS"] =
new boolean_define_optional
('ENABLE_REVERSE_DNS',
 array('true'  => "true. perform additional reverse dns lookups",
       'false' => "false. just record the address as given by the httpd server"),
" 
If set, we will perform reverse dns lookups to try to convert the
users IP number to a host name, even if the http server didn't do it for us.");

$properties["Admin Username"] =
new _define_optional_notempty('ADMIN_USER', "", "
You must set this! Username and password of the administrator.",
"onchange=\"validate_ereg('Sorry, ADMIN_USER cannot be empty.', '^.+$', 'ADMIN_USER', this);\"");

$properties["Admin Password"] =
new _define_password_optional('ADMIN_PASSWD', "", "
For heaven's sake pick a good password.
If your version of PHP supports encrypted passwords, your password will be
automatically encrypted within the generated config file. 
Use the \"Create Password\" button to create a good (random) password.",
"onchange=\"validate_ereg('Sorry, ADMIN_PASSWD must be at least 4 chars long.', '^....+$', 'ADMIN_PASSWD', this);\"");


$properties["ZIPdump Authentication"] =
new boolean_define_optional('ZIPDUMP_AUTH', 
                    array('false' => "false. Everyone may download zip dumps",
                          'true'  => "true. Only admin may download zip dumps"), "
If true, only the admin user can make zip dumps, else zip dumps
require no authentication.");

$properties["Enable Plugin RawHtml"] =
new boolean_define_commented_optional
('ENABLE_RAW_HTML', 
 array('true'  => "Enabled",
       'false' => "Disabled."), "
Allow &lt; ?plugin RawHtml ...&gt;. Define to false to disalbe the RawHtml plugin.");

$properties["Strict Mailable Pagedumps"] =
new boolean_define_optional
('STRICT_MAILABLE_PAGEDUMPS', 
 array('false' => "binary",
       'true'  => "quoted-printable"),
"
If you define this to true, (MIME-type) page-dumps (either zip dumps,
or \"dumps to directory\" will be encoded using the quoted-printable
encoding.  If you're actually thinking of mailing the raw page dumps,
then this might be useful, since (among other things,) it ensures
that all lines in the message body are under 80 characters in length.

Also, setting this will cause a few additional mail headers
to be generated, so that the resulting dumps are valid
RFC 2822 e-mail messages.

Probably, you can just leave this set to false, in which case you get
raw ('binary' content-encoding) page dumps.");



$properties["HTML Dump Filename Suffix"] =
new _variable('HTML_DUMP_SUFFIX', ".html", "
Here you can change the filename suffix used for XHTML page dumps.
If you don't want any suffix just comment this out.");



//FIXME: should be numeric_define_optional
$properties["Maximum Upload Size"] =
new numeric_define('MAX_UPLOAD_SIZE', "16 * 1024 * 1024", "
The maximum file upload size.");



//FIXME: should be numeric_define_optional
$properties["Minor Edit Timeout"] =
new numeric_define('MINOR_EDIT_TIMEOUT', "7 * 24 * 3600", "
If the last edit is older than MINOR_EDIT_TIMEOUT seconds, the
default state for the \"minor edit\" checkbox on the edit page form
will be off.");



$properties["Disabled Actions"] =
new array_variable('DisabledActions', array(), "
Actions listed in this array will not be allowed. Actions are:
browse, create, diff, dumphtml, dumpserial, edit, loadfile, lock, remove, 
unlock, upload, viewsource, zip, ziphtml");

$properties["Access Log"] =
new _define_commented('ACCESS_LOG', "/var/logs/wiki_access.log", "
PhpWiki can generate an access_log (in \"NCSA combined log\" format)
for you. If you want one, define this to the name of the log file,
such as /tmp/wiki_access_log.");

$properties["Compress Output"] =
new boolean_define_commented_optional
( 'COMPRESS_OUTPUT', 
  array(''  => 'Compress when PhpWiki thinks approriate.',
        'false' => 'Never compress output.',
        'true' => 'Always try to compress output.'),
  "
By default PhpWiki will try to have PHP compress it's output
before sending it to the browser (if you have a recent enough
version of PHP and the browser supports it.)

Define COMPRESS_OUTPUT to false to prevent output compression.

Define COMPRESS_OUTPUT to true to force output compression,
even if we think your version of PHP does this in a buggy
fashion.

Leave it undefined to leave the choice up to PhpWiki.");

$properties["HTTP Cache Control"] =
new _define_selection_optional
('CACHE_CONTROL',
 array('LOOSE' => 'LOOSE',
       'STRICT' => 'STRICT',
       'NONE' => 'NONE',
       'ALLOW_STALE' => 'ALLOW_STALE'),
 "
HTTP CACHE_CONTROL

This controls how PhpWiki sets the HTTP cache control
headers (Expires: and Cache-Control:) 

Choose one of:

<dl>
<dt>NONE</dt>
<dd>This is roughly the old (pre 1.3.4) behavior.  PhpWiki will
    instruct proxies and browsers never to cache PhpWiki output.</dd>

<dt>STRICT</dt>
<dd>Cached pages will be invalidated whenever the database global
    timestamp changes.  This should behave just like NONE (modulo
    bugs in PhpWiki and your proxies and browsers), except that
    things will be slightly more efficient.</dd>

<dt>LOOSE</dt>
<dd>Cached pages will be invalidated whenever they are edited,
    or, if the pages include plugins, when the plugin output could
    concievably have changed.

    <p>Behavior should be much like STRICT, except that sometimes
       wikilinks will show up as undefined (with the question mark)
       when in fact they refer to (recently) created pages.
       (Hitting your browsers reload or perhaps shift-reload button
       should fix the problem.)</p></dd>

<dt>ALLOW_STALE</dt>
<dd>Proxies and browsers will be allowed to used stale pages.
    (The timeout for stale pages is controlled by CACHE_CONTROL_MAX_AGE.)

    <p>This setting will result in quirky behavior.  When you edit a
       page your changes may not show up until you shift-reload the
       page, etc...</p>

    <p>This setting is generally not advisable, however it may be useful
       in certain cases (e.g. if your wiki gets lots of page views,
       and few edits by knowledgable people who won't freak over the quirks.)</p>
</dd>

The default is currently LOOSE.");

// FIXME: should be numeric_define_optional
$properties["HTTP Cache Control Max Age"] =
new numeric_define('CACHE_CONTROL_MAX_AGE', 600,
            "
Maximum page staleness, in seconds.");

$properties["Markup Caching"] =
new boolean_define_commented_optional
('WIKIDB_NOCACHE_MARKUP',
 array('false' => 'Enable markup cache',
       'true' => 'Disable markup cache'),
 "
MARKUP CACHING

PhpWiki normally caches a preparsed version (i.e. mostly
converted to HTML) of the most recent version of each page.
(Parsing the wiki-markup takes a fair amount of CPU.)

Define WIKIDB_NOCACHE_MARKUP to true to disable the
caching of marked-up page content.

Note that you can also disable markup caching on a per-page
temporary basis by addinging a query arg of '?nocache=1'
to the URL to the page.  (Use '?nocache=purge' to completely
discard the cached version of the page.)

You can also purge the cached markup globally by using the
\"Purge Markup Cache\" button on the PhpWikiAdministration page.");


$properties["Path for PHP Session Support"] =
new _ini_set('session.save_path', '/tmp', "
The login code now uses PHP's session support. Usually, the default
configuration of PHP is to store the session state information in
/tmp. That probably will work fine, but fails e.g. on clustered
servers where each server has their own distinct /tmp (this is the
case on SourceForge's project web server.) You can specify an
alternate directory in which to store state information like so
(whatever user your httpd runs as must have read/write permission
in this directory):");



$properties["Disable PHP Transparent Session ID"] =
new unchangeable_ini_set('session.use_trans_sid', "@ini_set('session.use_trans_sid', 0);", "
If your php was compiled with --enable-trans-sid it tries to
add a PHPSESSID query argument to all URL strings when cookie
support isn't detected in the client browser.  For reasons
which aren't entirely clear (PHP bug) this screws up the URLs
generated by PhpWiki.  Therefore, transparent session ids
should be disabled.  This next line does that.

(At the present time, you will not be able to log-in to PhpWiki,
or set any user preferences, unless your browser supports cookies.)");



///////// database selection


$properties["Part Two"] =
new part('_parttwo', $SEPARATOR."\n", "

Part Two:
Database Selection
");


$properties["Database Type"] =
new _variable_selection("DBParams|dbtype",
              array('dba'   => "dba DBM",
                    'SQL'   => "SQL PEAR",
                    'ADODB' => "SQL ADODB",
                    'cvs'   => "CVS File handler"), "
Select the database backend type:
Choose ADODB or SQL to use an SQL database with ADODB or PEAR.
Choose dba to use one of the standard UNIX dbm libraries.
CVS is not yet tested nor supported.
Recommended is SQL PEAR.");

$properties["Filename / Table name Prefix"] =
new _variable_commented("DBParams|prefix", "phpwiki_", "
Used by all DB types:

Prefix for filenames or table names

Currently you MUST EDIT THE SQL file too (in the schemas/
directory because we aren't doing on the fly sql generation
during the installation.");


$properties["SQL dsn Setup"] =
new unchangeable_variable('_sqldsnstuff', "", "
For SQL based backends, specify the database as a DSN
The most general form of a DSN looks like:
<pre>
  phptype(dbsyntax)://username:password@protocol+hostspec/database
</pre>
For a MySQL database, the following should work:
<pre>
   mysql://user:password@host/databasename
</pre>
To connect over a unix socket, use something like
<pre>
   mysql://user:password@unix(/path/to/socket)/databasename
</pre>
<pre>'dsn' => 'mysql://guest@:/var/lib/mysql/mysql.sock/test',
'dsn' => 'mysql://guest@localhost/test',
'dsn' => 'pgsql://localhost/test',</pre>");

// Choose ADODB or SQL to use an SQL database with ADODB or PEAR.
// Choose dba to use one of the standard UNIX dbm libraries.

$properties["SQL Type"] =
new _variable_selection('_dsn_sqltype',
              array('mysql' => "MySQL",
                    'pgsql' => "PostgreSQL"), "
SQL DB types");



$properties["SQL User"] =
new _variable('_dsn_sqluser', "wikiuser", "
SQL User Id:");



$properties["SQL Password"] =
new _variable('_dsn_sqlpass', "", "
SQL Password:");



$properties["SQL Database Host"] =
new _variable('_dsn_sqlhostorsock', "localhost", "
SQL Database Hostname:

To connect over a local named socket, use something like
<pre>
  unix(/var/lib/mysql/mysql.sock)
</pre>
here.");



$properties["SQL Database Name"] =
new _variable('_dsn_sqldbname', "phpwiki", "
SQL Database Name:");

list($dsn_sqltype,) = $properties["SQL Type"]->value();
$dsn_sqluser = $properties["SQL User"]->value();
$dsn_sqlpass = $properties["SQL Password"]->value();
$dsn_sqlhostorsock = $properties["SQL Database Host"]->value();
$dsn_sqldbname = $properties["SQL Database Name"]->value();

$properties["SQL dsn"] =
new unchangeable_variable("DBParams['dsn']", 
  "\$DBParams['dsn'] = \"\$_dsn_sqltype://{$dsn_sqluser}:{$dsn_sqlpass}@{$dsn_sqlhostorsock}/{$dsn_sqldbname}\";", "");

$properties["dba directory"] =
new _variable("DBParams|directory", "/tmp", "
dba directory:");


$properties["dba handler"] =
new _variable_selection('DBParams|dba_handler',
              array('gdbm' => "Gdbm - GNU database manager",
                    'dbm'  => "DBM - Redhat default. On sf.net there's dbm and gdbm",
                    'db2'  => "DB2 - Sleepycat Software's DB2",
                    'db3'  => "DB3 - Sleepycat Software's DB3. Fine on Windows but not on every Linux"), "
Use 'gdbm', 'dbm', 'db2' or 'db3' depending on your DBA handler methods supported:");

$properties["dba timeout"] =
new _variable("DBParams|timeout", "20", "
Recommended values are 20 or 5.");



///////////////////



$properties["Page Revisions"] =
new unchangeable_variable('_parttworevisions', "", "

The next section controls how many old revisions of each page are
kept in the database.

There are two basic classes of revisions: major and minor. Which
class a revision belongs in is determined by whether the author
checked the \"this is a minor revision\" checkbox when they saved the
page.
 
There is, additionally, a third class of revisions: author
revisions. The most recent non-mergable revision from each distinct
author is and author revision.

The expiry parameters for each of those three classes of revisions
can be adjusted seperately. For each class there are five
parameters (usually, only two or three of the five are actually
set) which control how long those revisions are kept in the
database.
<dl>
   <dt>max_keep:</dt> <dd>If set, this specifies an absolute maximum for the
            number of archived revisions of that class. This is
            meant to be used as a safety cap when a non-zero
            min_age is specified. It should be set relatively high,
            and it's purpose is to prevent malicious or accidental
            database overflow due to someone causing an
            unreasonable number of edits in a short period of time.</dd>

  <dt>min_age:</dt>  <dd>Revisions younger than this (based upon the supplanted
            date) will be kept unless max_keep is exceeded. The age
            should be specified in days. It should be a
            non-negative, real number,</dd>

  <dt>min_keep:</dt> <dd>At least this many revisions will be kept.</dd>

  <dt>keep:</dt>     <dd>No more than this many revisions will be kept.</dd>

  <dt>max_age:</dt>  <dd>No revision older than this age will be kept.</dd>
</dl>
Supplanted date: Revisions are timestamped at the instant that they
cease being the current revision. Revision age is computed using
this timestamp, not the edit time of the page.

Merging: When a minor revision is deleted, if the preceding
revision is by the same author, the minor revision is merged with
the preceding revision before it is deleted. Essentially: this
replaces the content (and supplanted timestamp) of the previous
revision with the content after the merged minor edit, the rest of
the page metadata for the preceding version (summary, mtime, ...)
is not changed.
");


// For now the expiration parameters are statically inserted as
// an unchangeable property. You'll have to edit the resulting
// config file if you really want to change these from the default.

$properties["Expiration Parameters for Major Edits"] =
new unchangeable_variable('ExpireParams|major',
"\$ExpireParams['major'] = array('max_age' => 32,
                               'keep'    => 8);", "
Keep up to 8 major edits, but keep them no longer than a month.");

$properties["Expiration Parameters for Minor Edits"] =
new unchangeable_variable('ExpireParams|minor',
"\$ExpireParams['minor'] = array('max_age' => 7,
                               'keep'    => 4);", "
Keep up to 4 minor edits, but keep them no longer than a week.");



$properties["Expiration Parameters by Author"] =
new unchangeable_variable('ExpireParams|author',
"\$ExpireParams['author'] = array('max_age'  => 365,
                                'keep'     => 8,
                                'min_age'  => 7,
                                'max_keep' => 20);", "
Keep the latest contributions of the last 8 authors up to a year.
Additionally, (in the case of a particularly active page) try to
keep the latest contributions of all authors in the last week (even
if there are more than eight of them,) but in no case keep more
than twenty unique author revisions.");

/////////////////////////////////////////////////////////////////////

$properties["Part Three"] =
new part('_partthree', $SEPARATOR."\n", "

Part Three: (optional)
User Authentication
");

$properties["User Authentication"] =
new boolean_define_optional('ALLOW_USER_LOGIN',
                    array('true'  => "true. Check any defined passwords. (Default)",
                          'false' => "false. Don't check passwords. Legacy &lt; 1.3.4"), "
If ALLOW_USER_LOGIN is true, any defined internal and external
authentication method is tried. 
If not, we don't care about passwords, but check the next two constants.");

$properties["HTTP Authentication"] =
new boolean_define_optional('ALLOW_HTTP_AUTH_LOGIN',
                    array('false' => "false. Ignore HTTP Authentication. (Default)",
			  'true'  => "true. Allow .htpasswd users login automatically."), "
The wiki can be optionally be protected by HTTP Auth. Use the username and password 
from there, and if this fails, try the other methods also.");

$properties["Strict Login"] =
new boolean_define_optional('ALLOW_BOGO_LOGIN',
                    array('true'  => "Users may Sign In with any WikiWord",
                          'false' => "Only admin may Sign In"), "
If ALLOW_BOGO_LOGIN is true, users are allowed to login (with
any/no password) using any userid which: 1) is not the ADMIN_USER,
2) is a valid WikiWord (matches \$WikiNameRegexp.)
If true, users may be created by themselves. Otherwise we need seperate auth.");

$properties["Require Sign In Before Editing"] =
new boolean_define_optional('REQUIRE_SIGNIN_BEFORE_EDIT',
                    array('false' => "Do not require Sign In",
                          'true'  => "Require Sign In"), "
If set, then if an anonymous user attempts to edit a page he will
be required to sign in.  (If ALLOW_BOGO_LOGIN is true, of course,
no password is required, but the user must still sign in under
some sort of BogoUserId.)");

if (function_exists('ldap_connect')) {
$properties["LDAP Authentication"] =
  new boolean_define_optional('ALLOW_LDAP_LOGIN',
                    array('true'  => "Allow LDAP Authentication",
	                  'false' => "Ignore LDAP"), "
LDAP Authentication
");
$properties["LDAP Host"] =
  new _define_optional('LDAP_AUTH_HOST', "localhost", "");
$properties["LDAP Root Search"] =
  new _define_optional('LDAP_BASE_DN', "ou=mycompany.com,o=My Company", "
Give the right LDAP root search information in the next statement.");

} else {

$properties["LDAP Authentication"] =
new unchangeable_define('ALLOW_LDAP_LOGIN', "
if (!defined('ALLOW_LDAP_LOGIN')) define('ALLOW_LDAP_LOGIN', true and function_exists('ldap_connect'));
if (!defined('LDAP_AUTH_HOST'))   define('LDAP_AUTH_HOST', 'localhost');
// Give the right LDAP root search information in the next statement. 
if (!defined('LDAP_BASE_DN')) define('LDAP_BASE_DN', 'ou=mycompany.com,o=My Company');
", "
Ignored. No LDAP support in this php. configure --with-ldap");
}

if (function_exists('imap_open')) {
$properties["IMAP Authentication"] =
  new boolean_define_optional('ALLOW_IMAP_LOGIN',
                    array('true'  => "Allow IMAP Authentication",
	                  'false' => "Ignore IMAP"), "
IMAP Authentication
");
$properties["IMAP Host"] =
  new _define_optional('IMAP_AUTH_HOST', 'localhost', '');
} else {
$properties["IMAP Authentication"] =
  new unchangeable_define('ALLOW_IMAP_LOGIN',"
// IMAP auth: check userid/passwords from a imap server, defaults to localhost
if (!defined('ALLOW_IMAP_LOGIN')) define('ALLOW_IMAP_LOGIN', true and function_exists('imap_open'));
if (!defined('IMAP_AUTH_HOST'))   define('IMAP_AUTH_HOST', 'localhost');
", "Ignored. No IMAP support in this php. configure --with-imap");
}


/////////////////////////////////////////////////////////////////////

$properties["Part Four"] =
new part('_partfour', $SEPARATOR."\n", "

Part Four:
Page appearance and layout
");



$properties["Theme"] =
new _define_selection_optional('THEME',
              array('default'  => "default",
                    'Hawaiian' => "Hawaiian",
                    'MacOSX'   => "MacOSX",
                    'Portland' => "Portland",
                    'Sidebar'  => "Sidebar",
                    'SpaceWiki' => "SpaceWiki"), "
THEME

Most of the page appearance is controlled by files in the theme
subdirectory.

There are a number of pre-defined themes shipped with PhpWiki.
Or you may create your own (e.g. by copying and then modifying one of
stock themes.)

Pick one.
<pre>
define('THEME', 'default');
define('THEME', 'Hawaiian');
define('THEME', 'MacOSX');
define('THEME', 'Portland');
define('THEME', 'Sidebar');
define('THEME', 'SpaceWiki');</pre>");



$properties["Character Set"] =
new _define_optional('CHARSET', 'iso-8859-1', "
Select a valid charset name to be inserted into the xml/html pages, 
and to reference links to the stylesheets (css). For more info see: 
http://www.iana.org/assignments/character-sets. Note that PhpWiki 
has been extensively tested only with the latin1 (iso-8859-1) 
character set.

If you change the default from iso-8859-1 PhpWiki may not work 
properly and it will require code modifications. However, character 
sets similar to iso-8859-1 may work with little or no modification 
depending on your setup. The database must also support the same 
charset, and of course the same is true for the web browser. (Some 
work is in progress hopefully to allow more flexibility in this 
area in the future).");



$properties["Language"] =
new _define_selection_optional('DEFAULT_LANGUAGE',
              array('en' => "English",
                    'nl' => "Nederlands",
                    'es' => "Español",
                    'fr' => "Français",
                    'de' => "Deutsch",
                    'sv' => "Svenska",
                    'it' => "Italiano",
                    'ja' => "Japanese",
                    ''   => "none"), "
Select your language/locale - default language is \"en\" for English.
Other languages available:<pre>
English \"en\"  (English    - HomePage)
Dutch   \"nl\" (Nederlands - ThuisPagina)
Spanish \"es\" (Español    - PáginaPrincipal)
French  \"fr\" (Français   - Accueil)
German  \"de\" (Deutsch    - StartSeite)
Swedish \"sv\" (Svenska    - Framsida)
Italian \"it\" (Italiano   - PaginaPrincipale)
Japanese \"ja\" (Japanese   - ¥Û¡¼¥à¥Ú¡¼¥¸)
</pre>
If you set DEFAULT_LANGUAGE to the empty string, your systems default language
(as determined by the applicable environment variables) will be
used.");

$properties["Wiki Page Source"] =
new _define_optional('WIKI_PGSRC', 'pgsrc', "
WIKI_PGSRC -- specifies the source for the initial page contents of
the Wiki. The setting of WIKI_PGSRC only has effect when the wiki is
accessed for the first time (or after clearing the database.)
WIKI_PGSRC can either name a directory or a zip file. In either case
WIKI_PGSRC is scanned for files -- one file per page.
<pre>
// Default (old) behavior:
define('WIKI_PGSRC', 'pgsrc'); 
// New style:
define('WIKI_PGSRC', 'wiki.zip'); 
define('WIKI_PGSRC', 
       '../Logs/Hamwiki/hamwiki-20010830.zip'); 
</pre>");



$properties["Default Wiki Page Source"] =
new _define('DEFAULT_WIKI_PGSRC', 'pgsrc', "
DEFAULT_WIKI_PGSRC is only used when the language is *not* the
default (English) and when reading from a directory: in that case
some English pages are inserted into the wiki as well.
DEFAULT_WIKI_PGSRC defines where the English pages reside.

FIXME: is this really needed?
");



$properties["Generic Pages"] =
new array_variable('GenericPages', array('ReleaseNotes', 'SteveWainstead', 'TestPage'), "
These are the pages which will get loaded from DEFAULT_WIKI_PGSRC.	

FIXME: is this really needed?  Can't we just copy these pages into
the localized pgsrc?
");




$properties["Part Five"] =
new part('_partfive', $SEPARATOR."\n", "

Part Five:
Mark-up options.
");



$properties["Allowed Protocols"] =
new list_variable('AllowedProtocols', 'http|https|mailto|ftp|news|nntp|ssh|gopher', "
allowed protocols for links - be careful not to allow \"javascript:\"
URL of these types will be automatically linked.
within a named link [name|uri] one more protocol is defined: phpwiki");



$properties["Inline Images"] =
new list_variable('InlineImages', 'png|jpg|gif', "
URLs ending with the following extension should be inlined as images");



$properties["WikiName Regexp"] =
new _variable('WikiNameRegexp', "(?<![[:alnum:]])(?:[[:upper:]][[:lower:]]+){2,}(?![[:alnum:]])", "
Perl regexp for WikiNames (\"bumpy words\")
(?&lt;!..) &amp; (?!...) used instead of '\b' because \b matches '_' as well");

$properties["Subpage Separator"] =
new _define_optional('SUBPAGE_SEPARATOR', '/', "
One character which seperates pages from subpages. Defaults to '/', but '.' or ':' were also used.",
"onchange=\"validate_ereg('Sorry, \'%s\' must be a single character. Currently only :, / or .', '^[/:.]$', 'SUBPAGE_SEPARATOR', this);\""
);

$properties["InterWiki Map File"] =
new _define('INTERWIKI_MAP_FILE', 'lib/interwiki.map', "
InterWiki linking -- wiki-style links to other wikis on the web

The map will be taken from a page name InterWikiMap.
If that page is not found (or is not locked), or map
data can not be found in it, then the file specified
by INTERWIKI_MAP_FILE (if any) will be used.");

$properties["WARN_NONPUBLIC_INTERWIKIMAP"] =
new boolean_define('WARN_NONPUBLIC_INTERWIKIMAP',   
	array('true'  => "true",
                          'false' => "false"), "
Display a warning if the internal lib/interwiki.map is used, and 
not the public InterWikiMap page. This map is not readable from outside.");

$properties["Keyword Link Regexp"] =
new _variable('KeywordLinkRegexp',
              '(?<=^Category|^Topic)[[:upper:]].*$',
              "
Regexp used for automatic keyword extraction.

Any links on a page to pages whose names match this regexp will
be used keywords in the keywords meta tag.  (This is an aid to
classification by search engines.)  The value of the match is
used as the keyword.

The default behavior is to match Category* and Topic* links.");

$properties["Part Six"] =
new part('_partsix', $SEPARATOR."\n", "

Part Six (optional):
URL options -- you can probably skip this section.
");

$properties["Server Name"] =
new _define_commented_optional('SERVER_NAME', $HTTP_SERVER_VARS['SERVER_NAME'], "
Canonical name and httpd port of the server on which this PhpWiki
resides.");



$properties["Server Port"] =
new numeric_define_commented('SERVER_PORT', $HTTP_SERVER_VARS['SERVER_PORT'], "",
"onchange=\"validate_ereg('Sorry, \'%s\' is no valid port number.', '^[0-9]+$', 'SERVER_PORT', this);\"");

$scriptname = preg_replace('/configurator.php/','index.php',$HTTP_SERVER_VARS["SCRIPT_NAME"]);

$properties["Script Name"] =
new _define_commented_optional('SCRIPT_NAME', $scriptname, "
Relative URL (from the server root) of the PhpWiki script.");

$properties["Data Path"] =
new _define_commented_optional('DATA_PATH', dirname($scriptname), "
URL of the PhpWiki install directory.  (You only need to set this
if you've moved index.php out of the install directory.)  This can
be either a relative URL (from the directory where the top-level
PhpWiki script is) or an absolute one.");



$properties["PhpWiki Install Directory"] =
new _define_commented_optional('PHPWIKI_DIR', dirname(__FILE__), "
Path to the PhpWiki install directory.  This is the local
filesystem counterpart to DATA_PATH.  (If you have to set
DATA_PATH, your probably have to set this as well.)  This can be
either an absolute path, or a relative path interpreted from the
directory where the top-level PhpWiki script (normally index.php)
resides.");



$properties["Use PATH_INFO"] =
new boolean_define_commented_optional('USE_PATH_INFO', 
                    array('true'  => 'use PATH_INFO',
			  'false' => 'do not use PATH_INFO'), "
PhpWiki will try to use short urls to pages, eg 
http://www.example.com/index.php/HomePage
If you want to use urls like 
http://www.example.com/index.php?pagename=HomePage
then define 'USE_PATH_INFO' as false by uncommenting the line below.
NB:  If you are using Apache >= 2.0.30, then you may need to to use
the directive \"AcceptPathInfo On\" in your Apache configuration file
(or in an appropriate <.htaccess> file) for the short urls to work:  
See http://httpd.apache.org/docs-2.0/mod/core.html#acceptpathinfo

See also http://phpwiki.sourceforge.net/phpwiki/PrettyWiki for more ideas
on prettifying your urls.

Default: PhpWiki will try to divine whether use of PATH_INFO
is supported in by your webserver/PHP configuration, and will
use PATH_INFO if it thinks that is possible.");


$properties["Virtual Path"] =
new _define_commented_optional('VIRTUAL_PATH', '/SomeWiki', "
VIRTUAL_PATH is the canonical URL path under which your your wiki
appears. Normally this is the same as dirname(SCRIPT_NAME), however
using, e.g. apaches mod_actions (or mod_rewrite), you can make it
something different.

If you do this, you should set VIRTUAL_PATH here.

E.g. your phpwiki might be installed at at /scripts/phpwiki/index.php,
but you've made it accessible through eg. /wiki/HomePage.

One way to do this is to create a directory named 'wiki' in your
server root. The directory contains only one file: an .htaccess
file which reads something like:
<pre>
    Action x-phpwiki-page /scripts/phpwiki/index.php
    SetHandler x-phpwiki-page
    DirectoryIndex /scripts/phpwiki/index.php
</pre>
In that case you should set VIRTUAL_PATH to '/wiki'.

(VIRTUAL_PATH is only used if USE_PATH_INFO is true.)
");


$properties["Part Seven"] =
new part('_partseven', $SEPARATOR."\n", "

Part Seven:

Miscellaneous settings
");

$properties["Pagename of Recent Changes"] =
new _define_optional('RECENT_CHANGES', 'RecentChanges', "
Page name of RecentChanges page.  Used for RSS Auto-discovery.");

$properties["Disable HTTP Redirects"] =
new boolean_define_commented_optional
('DISABLE_HTTP_REDIRECT',
 array('false' => 'Enable HTTP Redirects',
       'true' => 'Disable HTTP Redirects'),
"
(You probably don't need to touch this.)

PhpWiki uses HTTP redirects for some of it's functionality.
(e.g. after saving changes, PhpWiki redirects your browser to
view the page you just saved.)

Some web service providers (notably free European Lycos) don't seem to
allow these redirects.  (On Lycos the result in an \"Internal Server Error\"
report.)  In that case you can set DISABLE_HTTP_REDIRECT to true.
(In which case, PhpWiki will revert to sneakier tricks to try to
redirect the browser...)");

$end = "

$SEPARATOR
// Check if we were included by some other wiki version (getimg, en, ...) 
// or not. 
// If the server requested this index.php fire up the code by loading lib/main.php.
// Parallel wiki scripts can now simply include /index.php for the 
// main configuration, extend or redefine some settings and 
// load lib/main.php by themselves. 
// This overcomes the index as config problem.
$SEPARATOR

// This doesn't work with php as CGI yet!
if (defined('VIRTUAL_PATH') and defined('USE_PATH_INFO')) {
    if (\$HTTP_SERVER_VARS['SCRIPT_NAME'] == VIRTUAL_PATH) {
        include \"lib/main.php\";
    }
} else {
    if (defined('SCRIPT_NAME') and 
        (\$HTTP_SERVER_VARS['SCRIPT_NAME'] == SCRIPT_NAME)) {
        include \"lib/main.php\";
    } elseif (strstr(\$HTTP_SERVER_VARS['PHP_SELF'],'index.php')) {
        include \"lib/main.php\";
    }
}

// (c-file-style: \"gnu\")
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:   
?>
";



// end of configuration options
///////////////////////////////
// begin class definitions

/**
 * A basic index.php configuration line in the form of a variable.
 *
 * Produces a string in the form "$name = value;"
 * e.g.:
 * $WikiNameRegexp = "value";
 */
class _variable {

    var $config_item_name;
    var $default_value;
    var $description;
    var $prefix;
    var $jscheck;

    function _variable($config_item_name, $default_value, $description, $jscheck = '') {
        $this->config_item_name = $config_item_name;
        $this->description = $description;
        $this->default_value = $default_value;
	$this->jscheck = $jscheck;
        if (preg_match("/variable/i",get_class($this)))
	    $this->prefix = "\$";
	elseif (preg_match("/ini_set/i",get_class($this)))
            $this->prefix = "ini_get: ";
        else
	    $this->prefix = "";
    }

    function value() {
      global $HTTP_POST_VARS;
      if (isset($HTTP_POST_VARS[$this->config_item_name]))
          return $HTTP_POST_VARS[$this->config_item_name];
      else 
          return $this->default_value;
    }

    function _config_format($value) {
        $v = $this->get_config_item_name();
        // handle arrays: a|b --> a['b']
        if (strpos($v, '|')) {
            list($a, $b) = explode('|', $v);
            $v = sprintf("%s['%s']", $a, $b);
        }
        return sprintf("\$%s = \"%s\";", $v, $value);
    }

    function get_config_item_name() {
        return $this->config_item_name;
    }

    function get_config_item_id() {
        return str_replace('|', '-', $this->config_item_name);
    }

    function get_config_item_header() {
       if (strchr($this->config_item_name,'|')) {
          list($var,$param) = explode('|',$this->config_item_name);
	  return "<b>" . $this->prefix . $var . "['" . $param . "']</b><br />";
       }
       elseif ($this->config_item_name[0] != '_')
	  return "<b>" . $this->prefix . $this->config_item_name . "</b><br />";
       else 
          return '';
    }

    function _get_description() {
        return $this->description;
    }

    function _get_config_line($posted_value) {
        return "\n" . $this->_config_format($posted_value);
    }

    function get_config($posted_value) {
        $d = stripHtml($this->_get_description());
        $d = str_replace("\n", "\n// ", $d) . $this->_get_config_line($posted_value) ."\n";
        return $d;
    }

    function get_instructions($title) {
        global $tdwidth;
        $i = "<h3>" . $title . "</h3>\n    " . nl2p($this->_get_description()) . "\n";
        return "<tr>\n<td width=\"$tdwidth\" class=\"instructions\">\n" . $i . "</td>\n";
    }

    function get_html() {
	return $this->get_config_item_header() . 
	    "<input type=\"text\" size=\"50\" name=\"" . $this->get_config_item_name() . "\" value=\"" . htmlspecialchars($this->default_value) . "\" " . 
	    $this->jscheck . " />" . "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: green\">Input accepted.</p>";
    }
}

class unchangeable_variable
extends _variable {
    function _config_format($value) {
        return "";
    }
    // function get_html() { return false; }
    function get_html() {
	return $this->get_config_item_header() . 
	"<em>Not editable.</em>" . 
	"<pre>" . $this->default_value."</pre>";
    }
    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        return "${n}".$this->default_value;
    }
    function get_instructions($title) {
	global $tdwidth;
        $i = "<h3>" . $title . "</h3>\n    " . nl2p($this->_get_description()) . "\n";
        // $i = $i ."<em>Not editable.</em><br />\n<pre>" . $this->default_value."</pre>";
        return "<tr><td width=\"100%\" class=\"unchangeable-variable-top\" colspan=\"2\">\n".$i ."</td></tr>\n".
	"<tr style=\"border-top: none;\"><td class=\"unchangeable-variable-left\" width=\"$tdwidth\" bgcolor=\"#eeeeee\">&nbsp;</td>";
    }
}

class unchangeable_define
extends unchangeable_variable {
    function _config_format($value) {
        return "";
    }
}
class unchangeable_ini_set
extends unchangeable_variable {
    function _config_format($value) {
        return "";
    }
}


class _variable_selection
extends _variable {
    function value() {
        global $HTTP_POST_VARS;
        if (!empty($HTTP_POST_VARS[$this->config_item_name]))
            return $HTTP_POST_VARS[$this->config_item_name];
        else {
	    list($option, $label) = current($this->default_value);
            return $this->$option;
        }
    }
    function get_html() {
	$output = $this->get_config_item_header();
        $output .= '<select name="' . $this->get_config_item_name() . "\">\n";
        /* The first option is the default */
        while(list($option, $label) = each($this->default_value)) {
            $output .= "  <option value=\"$option\">$label</option>\n";
        }
        $output .= "</select>\n";
        return $output;
    }
}


class _define
extends _variable {
    function _config_format($value) {
        return sprintf("define('%s', '%s');", $this->get_config_item_name(), $value);
    }
    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        if ($posted_value == '')
            return "${n}//" . $this->_config_format("");
        else
            return "${n}" . $this->_config_format($posted_value);
    }
    function get_html() {
	return $this->get_config_item_header() . 
	    "<input type=\"text\" size=\"50\" name=\"" . $this->get_config_item_name() . "\" value=\"" . $this->default_value . "\" {$this->jscheck} />" .
	    "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: green\">Input accepted.</p>";
    }
}

class _define_commented
extends _define {
    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        if ($posted_value == $this->default_value)
            return "${n}//" . $this->_config_format($posted_value);
        else if ($posted_value == '')
            return "${n}//" . $this->_config_format("");
        else
            return "${n}" . $this->_config_format($posted_value);
    }
}

class _define_commented_optional
extends _define_commented {
    function _config_format($value) {
	$name = $this->get_config_item_name();
        return sprintf("if (!defined('%s')) define('%s', '%s');", $name, $name, $value);
    }
}

class _define_optional
extends _define {
    function _config_format($value) {
	$name = $this->get_config_item_name();
        return sprintf("if (!defined('%s')) define('%s', '%s');", $name, $name, $value);
    }
}

class _define_optional_notempty
extends _define_optional {
    function get_html() {
	$s = $this->get_config_item_header() . 
	    "<input type=\"text\" size=\"50\" name=\"" . $this->get_config_item_name() . "\" value=\"" . $this->default_value . "\" {$this->jscheck} />";
        if (empty($this->default_value))
	    return $s . "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: red\">Cannot be empty.</p>";
	else
	    return $s . "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: green\">Input accepted.</p>";
    }
}

class _variable_commented
extends _variable {
    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        if ($posted_value == $this->default_value)
            return "${n}//" . $this->_config_format($posted_value);
        else if ($posted_value == '')
            return "${n}//" . $this->_config_format("");
        else
            return "${n}" . $this->_config_format($posted_value);
    }
}

class numeric_define_commented
extends _define {
    //    var $jscheck = "onchange=\"validate_ereg('Sorry, \'%s\' is not an integer.', '^[-+]?[0-9]+$', '" . $this->get_config_item_name() . "', this);\"";

    function get_html() {
	return numeric_define::get_html();
    }
    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        if ($posted_value == $this->default_value)
            return "${n}//" . $this->_config_format($posted_value);
        else if ($posted_value == '')
            return "${n}//" . $this->_config_format('0');
        else
            return "${n}" . $this->_config_format($posted_value);
    }
}

class _define_selection
extends _variable_selection {
    function _config_format($value) {
        return sprintf("define('%s', '%s');", $this->get_config_item_name(), $value);
    }
    function _get_config_line($posted_value) {
        return _define::_get_config_line($posted_value);
    }
    function get_html() {
        return _variable_selection::get_html();
    }
}

class _define_selection_optional
extends _define_selection {
    function _config_format($value) {
	$name = $this->get_config_item_name();
        return sprintf("if (!defined('%s')) define('%s', '%s');", $name, $name, $value);
    }
}

class _variable_selection_optional
extends _variable_selection {
    function _config_format($value) {
        $v = $this->get_config_item_name();
        // handle arrays: a|b --> a['b']
        if (strpos($v, '|')) {
            list($a, $b) = explode('|', $v);
            $v = sprintf("%s['%s']", $a, $b);
        }
        return sprintf("if (!isset(\$%s)) { \$%s = \"%s\"; }", $v, $v, $value);
    }
}

class _define_password
extends _define {
    //    var $jscheck = "onchange=\"validate_ereg('Sorry, \'%s\' cannot be empty.', '^.+$', '" . $this->get_config_item_name() . "', this);\"";

    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        if ($posted_value == '') {
            $p = "${n}//" . $this->_config_format("");
            $p = $p . "\n// If you used the passencrypt.php utility to encode the password";
            $p = $p . "\n// then uncomment this line:";
            $p = $p . "\n//if (!defined('ENCRYPTED_PASSWD')) define('ENCRYPTED_PASSWD', true);";
            return $p;
        } else {
            if (function_exists('crypt')) {
                $salt_length = max(CRYPT_SALT_LENGTH,
                                    2 * CRYPT_STD_DES,
                                    9 * CRYPT_EXT_DES,
                                   12 * CRYPT_MD5,
                                   16 * CRYPT_BLOWFISH);
                // generate an encrypted password
                $crypt_pass = crypt($posted_value, rand_ascii($salt_length));
                $p = "${n}" . $this->_config_format($crypt_pass);
                return $p . "\ndefine('ENCRYPTED_PASSWD', true);";
            } else {
                $p = "${n}" . $this->_config_format($posted_value);
                $p = $p . "\n// Encrypted passwords cannot be used:";
                $p = $p . "\n// 'function crypt()' not available in this version of php";
                $p = $p . "\ndefine('ENCRYPTED_PASSWD', false);";
                return $p;
            }
        }
    }
    function get_html() {
        return _variable_password::get_html();
    }
}

class _define_password_optional
extends _define_password {
    function _config_format($value) {
	$name = $this->get_config_item_name();
        return sprintf("if (!defined('%s')) define('%s', '%s');", $name, $name, $value);
    }
}


class _variable_password
extends _variable {
    //    var $jscheck = "onchange=\"validate_ereg('Sorry, \'%s\' cannot be empty.', '^.+$', '" . $this->get_config_item_name() . "', this);\"";

    function get_html() {
	global $HTTP_POST_VARS, $HTTP_GET_VARS;
	$s = $this->get_config_item_header();
        if (isset($HTTP_POST_VARS['create']) or isset($HTTP_GET_VARS['create'])) {
	    $new_password = random_good_password();
	    $this->default_value = $new_password;
	    $s .= "Created password: <strong>$new_password</strong><br />&nbsp;<br />";
	}
        $s .= "<input type=\"password\" name=\"" . $this->get_config_item_name() . "\" value=\"" . $this->default_value . "\" {$this->jscheck} />" . 
"&nbsp;&nbsp;<input type=\"submit\" name=\"create\" value=\"Create Password\" />";
	if (empty($this->default_value))
	    $s .= "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: red\">Cannot be empty.</p>";
	elseif (strlen($this->default_value) < 4)
	    $s .= "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: red\">Must be longer than 4 chars.</p>";
	else
	    $s .= "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: green\">Input accepted.</p>";
	return $s;
    }
}

class numeric_define
extends _define {
    //    var $jscheck = "onchange=\"validate_ereg('Sorry, \'%s\' is not an integer.', '^[-+]?[0-9]+$', '" . $this->get_config_item_name() . "', this);\"";

    function _config_format($value) {
        return sprintf("define('%s', %s);", $this->get_config_item_name(), $value);
    }
    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        if ($posted_value == '')
            return "${n}//" . $this->_config_format('0');
        else
            return "${n}" . $this->_config_format($posted_value);
    }
}

class list_variable
extends _variable {
    function _get_config_line($posted_value) {
        // split the phrase by any number of commas or space characters,
        // which include " ", \r, \t, \n and \f
        $list_values = preg_split("/[\s,]+/", $posted_value, -1, PREG_SPLIT_NO_EMPTY);
        $list_values = join("|", $list_values);
        return _variable::_get_config_line($list_values);
    }
    function get_html() {
        $list_values = explode("|", $this->default_value);
        $rows = max(3, count($list_values) +1);
        $list_values = join("\n", $list_values);
        $ta = $this->get_config_item_header();
	$ta .= "<textarea cols=\"18\" rows=\"". $rows ."\" name=\"".$this->get_config_item_name()."\" {$this->jscheck}>";
        $ta .= $list_values . "</textarea>";
	$ta .= "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: green\">Input accepted.</p>";
        return $ta;
    }
}

class array_variable
extends _variable {
    function _config_format($value) {
        return sprintf("\$%s = array(%s);", $this->get_config_item_name(), $value);
    }
    function _get_config_line($posted_value) {
        // split the phrase by any number of commas or space characters,
        // which include " ", \r, \t, \n and \f
        $list_values = preg_split("/[\s,]+/", $posted_value, -1, PREG_SPLIT_NO_EMPTY);
        if (!empty($list_values)) {
            $list_values = "'".join("', '", $list_values)."'";
            return "\n" . $this->_config_format($list_values);
        } else
            return "\n//" . $this->_config_format('');
    }
    function get_html() {
        $list_values = join("\n", $this->default_value);
        $rows = max(3, count($this->default_value) +1);
        $ta = $this->get_config_item_header();
        $ta .= "<textarea cols=\"18\" rows=\"". $rows ."\" name=\"".$this->get_config_item_name()."\" {$this->jscheck}>";
        $ta .= $list_values . "</textarea>";
	$ta .= "<p id=\"" . $this->get_config_item_id() . "\" style=\"color: green\">Input accepted.</p>";
        return $ta;
    }

}

class _ini_set
extends _variable {
    function value() {
        global $HTTP_POST_VARS;
        if ($v = $HTTP_POST_VARS[$this->config_item_name])
            return $v;
        else {
	    return ini_get($this->get_config_item_name);
        }
    }
    function _config_format($value) {
        return sprintf("ini_set('%s', '%s');", $this->get_config_item_name(), $value);
    }
    function _get_config_line($posted_value) {
        if ($posted_value && ! $posted_value == $this->default_value)
            return "\n" . $this->_config_format($posted_value);
        else
            return "\n//" . $this->_config_format($this->default_value);
    }
}

class boolean_define
extends _define {
    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        return "${n}" . $this->_config_format($posted_value);
    }
    function _config_format($value) {
        if (strtolower(trim($value)) == 'false')
            $value = false;
        return sprintf("define('%s', %s);", $this->get_config_item_name(),
                       (bool)$value ? 'true' : 'false');
    }
    function get_html() {
        $output = $this->get_config_item_header();
        $output .= '<select name="' . $this->get_config_item_name() . "\" {$this->jscheck}>\n";
        /* The first option is the default */
        list($option, $label) = each($this->default_value);
        $output .= "  <option value=\"$option\" selected='selected'>$label</option>\n";
        /* There can usually, only be two options, there can be
         * three options in the case of a boolean_define_commented_optional */
        while (list($option, $label) = each($this->default_value)) 
          $output .= "  <option value=\"$option\">$label</option>\n";
        $output .= "</select>\n";
        return $output;
    }
}

class boolean_define_optional
extends boolean_define {
    function _config_format($value) {
	$name = $this->get_config_item_name();
        return "if (!defined('$name')) " . boolean_define::_config_format($value);
    }
}

class boolean_define_commented
extends boolean_define {
    function _get_config_line($posted_value) {
        if ($this->description)
            $n = "\n";
        list($default_value, $label) = each($this->default_value);
        if ($posted_value == $default_value)
            return "${n}//" . $this->_config_format($posted_value);
        else if ($posted_value == '')
            return "${n}//" . $this->_config_format('false');
        else
            return "${n}" . $this->_config_format($posted_value);
    }
}

class boolean_define_commented_optional
extends boolean_define_commented {
    function _config_format($value) {
	$name = $this->get_config_item_name();
        return "if (!defined('$name')) " . boolean_define_commented::_config_format($value);
    }
}

class part
extends _variable {
    function value () { return; }
    function get_config($posted_value) {
        $d = stripHtml($this->_get_description());
        global $SEPARATOR;
        return "\n".$SEPARATOR . str_replace("\n", "\n// ", $d) ."\n$this->default_value";
    }
    function get_instructions($title) {
	$group_name = preg_replace("/\W/","",$title);
	$i = "<tr class='header' id='$group_name'>\n<td class=\"part\" width=\"100%\" colspan=\"2\" bgcolor=\"#eeaaaa\">\n";
        $i .= "<h2>" . $title . "</h2>\n    " . nl2p($this->_get_description()) ."\n";
	$i .= "<p><a href=\"javascript:toggle_group('$group_name')\" id=\"{$group_name}_text\">Hide options.</a></p>";
        return  $i ."</td>\n";
    }
    function get_html() {
        return "";
    }
}

// html utility functions
function nl2p($text) {
  preg_match_all("@\s*(<pre>.*?</pre>|<dl>.*?</dl>|.*?(?=\n\n|<pre>|<dl>|$))@s",
                 $text, $m, PREG_PATTERN_ORDER);

  $text = '';
  foreach ($m[1] as $par) {
    if (!($par = trim($par)))
      continue;
    if (!preg_match('/^<(pre|dl)>/', $par))
      $par = "<p>$par</p>";
    $text .= $par;
  }
  return $text;
}

function stripHtml($text) {
        $d = str_replace("<pre>", "", $text);
        $d = str_replace("</pre>", "", $d);
        $d = str_replace("<dl>", "", $d);
        $d = str_replace("</dl>", "", $d);
        $d = str_replace("<dt>", "", $d);
        $d = str_replace("</dt>", "", $d);
        $d = str_replace("<dd>", "", $d);
        $d = str_replace("</dd>", "", $d);
        $d = str_replace("<p>", "", $d);
        $d = str_replace("</p>", "", $d);
        //restore html entities into characters
        // http://www.php.net/manual/en/function.htmlentities.php
        $trans = get_html_translation_table (HTML_ENTITIES);
        $trans = array_flip ($trans);
        $d = strtr($d, $trans);
        return $d;
}

/**
 * Seed the random number generator.
 *
 * better_srand() ensures the randomizer is seeded only once.
 * 
 * How random do you want it? See:
 * http://www.php.net/manual/en/function.srand.php
 * http://www.php.net/manual/en/function.mt-srand.php
 */
function better_srand($seed = '') {
    static $wascalled = FALSE;
    if (!$wascalled) {
	if ($seed === '') {
	    list($usec,$sec)=explode(" ",microtime());
	    if ($usec > 0.1) 
		$seed = (double) $usec * $sec;
	    else // once in a while use the combined LCG entropy
		$seed = (double) 1000000 * substr(uniqid("",true),13);
	}
	if (function_exists('mt_srand')) {
	    mt_srand($seed); // mersenne twister
	} else {
	    srand($seed);    
	}
        $wascalled = TRUE;
    }
}

function rand_ascii($length = 1) {
    better_srand();
    $s = "";
    for ($i = 1; $i <= $length; $i++) {
	// return only typeable 7 bit ascii, avoid quotes
	if (function_exists('mt_rand'))
	    // the usually bad glibc srand()
	    $s .= chr(mt_rand(40, 126)); 
	else
	    $s .= chr(rand(40, 126));
    }
    return $s;
}

////
// Function to create better user passwords (much larger keyspace),
// suitable for user passwords.
// Sequence of random ASCII numbers, letters and some special chars.
// Note: There exist other algorithms for easy-to-remember passwords.
function random_good_password ($minlength = 5, $maxlength = 8) {
  $newpass = '';
  // assume ASCII ordering (not valid on EBCDIC systems!)
  $valid_chars = "!#%&+-.0123456789=@ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
  $start = ord($valid_chars);
  $end   = ord(substr($valid_chars,-1));
  better_srand();
  if (function_exists('mt_rand')) // mersenne twister
      $length = mt_rand($minlength, $maxlength);
  else	// the usually bad glibc rand()
      $length = rand($minlength, $maxlength);
  while ($length > 0) {
      if (function_exists('mt_rand'))
	  $newchar = mt_rand($start, $end);
      else
	  $newchar = rand($start, $end);
      if (! strrpos($valid_chars,$newchar) ) continue; // skip holes
      $newpass .= sprintf("%c",$newchar);
      $length--;
  }
  return($newpass);
}

// debugging
function printArray($a) {
    echo "<hr />\n<pre>\n";
    print_r($a);
    echo "\n</pre>\n<hr />\n";
}

// end of class definitions
/////////////////////////////
// begin auto generation code

if (!function_exists('is_a'))
{
  function is_a($object, $class)
  {
    $class = strtolower($class);
    return (get_class($object) == $class) or is_subclass_of($object, $class);
  }
}


if (@$HTTP_POST_VARS['action'] == 'make_config') {

    $timestamp = date ('dS of F, Y H:i:s');

    $config = "<?php
/* This is a local configuration file for PhpWiki.
 * It was automatically generated by the configurator script
 * on the $timestamp.
 */

/*$copyright*/

/////////////////////////////////////////////////////////////////////
/*$preamble*/
";

    $posted = $GLOBALS['HTTP_POST_VARS'];

    if (defined('DEBUG'))
        printArray($GLOBALS['HTTP_POST_VARS']);

    foreach($properties as $option_name => $a) {
        $posted_value = $posted[$a->config_item_name];
        $config .= $properties[$option_name]->get_config($posted_value);
    }

    if (defined('DEBUG')) {
        $diemsg = "The configurator.php is provided for testing purposes only.
You can't use this file with your PhpWiki server yet!!";
        $config .= "\ndie(\"$diemsg\");\n";
    }
    $config .= $end;

    // I think writing this config file is a big security hole.
    // If this is installed in such a way that it can write an index-user.php,
    // then anyone can create a working index-user.php with, e.g. any
    // admin user and pw of their choosing...
    //
    // So I'm disabling it...

    if (defined(ENABLE_FILE_OUTPUT) and ENABLE_FILE_OUPUT) {
      /* We first check if the config-file exists. */
      if (file_exists($fs_config_file)) {
        /* We make a backup copy of the file */
        // $config_file = 'index-user.php';
        $new_filename = preg_replace('/\.php$/', time() . '.php', $fs_config_file);
        if (@copy($fs_config_file, $new_filename)) {
            $fp = @fopen($fs_config_file, 'w');
        }
      } else {
        $fp = @fopen($fs_config_file, 'w');
      }
    }
    else {
      $fp = false;
    }
    

    if ($fp) {
        fputs($fp, $config);
        fclose($fp);
        echo "<p>The configuration was written to <code><b>$config_file</b></code>.</p>\n";
        if ($new_filename) {
            echo "<p>A backup was made to <code><b>$new_filename</b></code>.</p>\n";
        }
        echo "<p><strong>You must rename or copy this</strong> <code><b>$config_file</b></code> <strong>file to</strong> <code><b>index.php</b></code>.</p>\n";
    } else {
        echo "<p>A configuration file could <b>not</b> be written. You should copy the above configuration to a file, and manually save it as <code><b>index.php</b></code>.</p>\n";
    }

    echo "<hr />\n<p>Here's the configuration file based on your answers:</p>\n";
    echo "<form method='get' action='$PHP_SELF'>\n";
    echo "<textarea id='config-output' readonly='readonly' style='width:100%;' rows='30' cols='100'>\n";
    echo htmlentities($config);
    echo "</textarea></form>\n";
    echo "<hr />\n";

    echo "<p>To make any corrections, <a href=\"configurator.php\">edit the settings again</a>.</p>\n";

} else { // first time or create password
    $posted = $GLOBALS['HTTP_POST_VARS'];
    /* No action has been specified - we make a form. */

    echo '
<form action="configurator.php" method="post">
<input type="hidden" name="action" value="make_config" />
<table cellpadding="4" cellspacing="0">
';

    while(list($property, $obj) = each($properties)) {
        echo $obj->get_instructions($property);
        if ($h = $obj->get_html()) {
            if (defined('DEBUG'))  $h = get_class($obj) . "<br />\n" . $h;
            echo "<td>".$h."</td>\n";
        }
	echo '</tr>';
    }

    echo '
</table>
<p><input type="submit" value="Save ',$config_file,'" /> <input type="reset" value="Clear" /></p>
</form>
';
}
?>
</body>
</html>
