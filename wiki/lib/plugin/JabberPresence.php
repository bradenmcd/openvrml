<?php // -*-php-*-
rcs_id('$Id: JabberPresence.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
/**
 * A simple Jabber presence WikiPlugin.
 * http://wiki.crao.net/index.php/JabberPr%E9sence/Source
 * http://edgar.netflint.net/howto.php
 *
 * Usage:
 *  <?plugin JabberPresence scripturl=http://edgar.netflint.net/status.php 
 *                          jid=yourid@jabberserver type=html iconset=phpbb ?>
 *
 * @author: Arnaud Fontaine
 */

class WikiPlugin_JabberPresence
extends WikiPlugin
{
    // Five required functions in a WikiPlugin.
    function getName () {
        return _("JabberPresence");
    }

    function getDescription () {
        return _("Simple jabber presence plugin");

    }

    function getVersion() {
        return preg_replace("/[Revision: $]/", '',
                            "\$Revision: 1.1.1.1 $");
    }

    // Establish default values for each of this plugin's arguments.
    function getDefaultArguments() {
        return array('scripturl' => "<mettez l'url de votre bot ici>",
                     'jid'       => "",
		     'type'      => 'image',
                     'iconset'   => "gabber");
    }

    function run($dbi, $argstr, $request) {
        extract($this->getArgs($argstr, $request));
        // Any text that is returned will not be further transformed,
        // so use html where necessary.
        if (empty($jid))
	    $html = HTML();
        else
	  $html = HTML::img(array('src' => urlencode($scripturl).
				  '&jid='.urlencode($jid).
				  '&type='.urlencode($type).
				  '&iconset='.($iconset)));
        return $html;
    }
};

// For emacs users
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>