<?php

rcs_id('$Id: themeinfo.php,v 1.1.1.1 2004-04-13 23:41:41 braden Exp $');

/**
 * WikiWiki Hawaiian theme for PhpWiki.
 */

require_once('lib/Theme.php');

class Theme_Hawaiian extends Theme {
    function getCSS() {
        // FIXME: this is a hack which will not be needed once
        //        we have dynamic CSS.
        $css = Theme::getCSS();
        $css->pushcontent(HTML::style(array('type' => 'text/css'),
                             new RawXml(sprintf("<!--\nbody {background-image: url(%s);}\n-->",
                                                $this->getImageURL('uhhbackground.jpg')))));
        return $css;
    }
}
$Theme = new Theme_Hawaiian('Hawaiian');

// CSS file defines fonts, colors and background images for this
// style.  The companion '*-heavy.css' file isn't defined, it's just
// expected to be in the same directory that the base style is in.

$Theme->setDefaultCSS('Hawaiian', 'Hawaiian.css');
$Theme->addAlternateCSS(_("Printer"), 'phpwiki-printer.css', 'print, screen');
$Theme->addAlternateCSS(_("Modern"), 'phpwiki-modern.css');
$Theme->addAlternateCSS('PhpWiki', 'phpwiki.css');

/**
 * The logo image appears on every page and links to the HomePage.
 */
$Theme->addImageAlias('logo', 'PalmBeach.jpg');
$Theme->addImageAlias('logo', WIKI_NAME . 'Logo.png');

/**
 * The Signature image is shown after saving an edited page. If this
 * is set to false then the "Thank you for editing..." screen will
 * be omitted.
 */
//$Theme->addImageAlias('signature', 'SubmersiblePiscesV.jpg');
$Theme->addImageAlias('signature', 'WaterFall.jpg');
$Theme->addImageAlias('signature', WIKI_NAME . "Signature.png");
// Uncomment this next line to disable the signature.
//$Theme->addImageAlias('signature', false);

// If you want to see more than just the waterfall let a random
// picture be chosen for the signature image:
//include_once($Theme->file('lib/random.php'));
include_once("themes/$Theme->_name/lib/random.php");
$imgSet = new randomImage($Theme->file("images/pictures"));
$imgFile = "pictures/" . $imgSet->filename;
$Theme->addImageAlias('signature', $imgFile);

//To test out the randomization just use logo instead of signature
//$Theme->addImageAlias('logo', $imgFile);

/*
 * Link Icons
 */
$Theme->setLinkIcon('interwiki');
$Theme->setLinkIcon('*', 'flower.png');

$Theme->setButtonSeparator(' ');

/**
 * WikiWords can automatically be split by inserting spaces between
 * the words. The default is to leave WordsSmashedTogetherLikeSo.
 */
$Theme->setAutosplitWikiWords(true);

/*
 * You may adjust the formats used for formatting dates and times
 * below.  (These examples give the default formats.)
 * Formats are given as format strings to PHP strftime() function See
 * http://www.php.net/manual/en/function.strftime.php for details.
 * Do not include the server's zone (%Z), times are converted to the
 * user's time zone.
 */
//$Theme->setDateFormat("%B %d, %Y");	    // must not contain time


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
