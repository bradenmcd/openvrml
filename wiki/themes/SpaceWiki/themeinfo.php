<?php // -*-php-*-

rcs_id('$Id: themeinfo.php,v 1.1.1.1 2004-04-13 23:41:42 braden Exp $');

/**
 * This theme is by design completely css-based so unfortunately it
 * doesn't render properly or even the same across different browsers.
 * Mozilla 0.98 was used for testing, it is the only Mac browser so
 * far which correctly renders most of the css used here.
 * A preview screen snapshot is included for comparison testing.
 *
 * The reverse coloring of this theme was chosen to provide an extreme
 * example of a heavily customized PhpWiki, through which any
 * potential visual problems can be identified and to eliminate any
 * remaining non-structural html elements from the html templates.
 */

require_once('lib/Theme.php');

class Theme_SpaceWiki extends Theme {
    function getRecentChangesFormatter ($format) {
        include_once($this->file('lib/RecentChanges.php'));
        if (preg_match('/^rss|^sidebar/', $format))
            return false;       // use default
        return '_SpaceWiki_RecentChanges_Formatter';
    }

    function getPageHistoryFormatter ($format) {
        include_once($this->file('lib/RecentChanges.php'));
        if (preg_match('/^rss|^sidebar/', $format))
            return false;       // use default
        return '_SpaceWiki_PageHistory_Formatter';
    }
}

$Theme = new Theme_SpaceWiki('SpaceWiki');

// CSS file defines fonts, colors and background images for this
// style.  The companion '*-heavy.css' file isn't defined, it's just
// expected to be in the same directory that the base style is in.

$Theme->setDefaultCSS('SpaceWiki', 'SpaceWiki.css');
$Theme->addAlternateCSS(_("Printer"), 'phpwiki-printer.css', 'print, screen');
$Theme->addAlternateCSS(_("Modern"), 'phpwiki-modern.css');
$Theme->addAlternateCSS('PhpWiki', 'phpwiki.css');

/**
 * The logo image appears on every page and links to the HomePage.
 */
//$Theme->addImageAlias('logo', 'logo.png');
$Theme->addImageAlias('logo', 'Ufp-logo.jpg');
$Theme->addImageAlias('logo', WIKI_NAME . 'Logo.png');

/**
 * The Signature image is shown after saving an edited page. If this
 * is set to false then the "Thank you for editing..." screen will
 * be omitted.
 */
$Theme->addImageAlias('signature', 'lights.gif');
$Theme->addImageAlias('signature', WIKI_NAME . "Signature.png");
// Uncomment this next line to disable the signature.
//$Theme->addImageAlias('signature', false);

$Theme->addImageAlias('hr', 'hr.png');

$Theme->setButtonSeparator(" ");

/**
 * WikiWords can automatically be split by inserting spaces between
 * the words. The default is to leave WordsSmashedTogetherLikeSo.
 */
//$Theme->setAutosplitWikiWords(false);

/**
 * The "stardate" format here is really just metricdate.24hourtime. A
 * "real" date2startdate conversion function might be fun but not very
 * useful on a wiki.
 */
$Theme->setTimeFormat("%H%M%S");
$Theme->setDateFormat("%Y%m%d"); // must not contain time


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// (c-file-style: "gnu")
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:   
?>
