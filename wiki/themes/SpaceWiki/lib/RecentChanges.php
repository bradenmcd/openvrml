<?php rcs_id('$Id: RecentChanges.php,v 1.1.1.1 2004-04-13 23:41:42 braden Exp $');
/*
 * Extensions/modifications to the stock RecentChanges (and PageHistory) format.
 */


require_once('lib/plugin/RecentChanges.php');
require_once('lib/plugin/PageHistory.php');

function SpaceWiki_RC_revision_formatter (&$fmt, &$rev) {
    $class = 'rc-' . $fmt->importance($rev);
        
    return HTML::li(array('class' => $class),
                    $fmt->diffLink($rev), ' ',
                    $fmt->pageLink($rev), ' ',
                    ' . ',
                    $rev->get('is_minor_edit') ? $fmt->time($rev) : HTML::strong($fmt->time($rev)), ' ',
                    ' . . . ',
                    $fmt->summaryAsHTML($rev),
                    ' . . . ',
                    $fmt->authorLink($rev));
}

function SpaceWiki_PH_revision_formatter (&$fmt, &$rev) {
    $class = 'rc-' . $fmt->importance($rev);

    return HTML::li(array('class' => $class),
                    $fmt->diffLink($rev), ' ',
                    $fmt->pageLink($rev), ' ',
                    $rev->get('is_minor_edit') ? $fmt->time($rev) : HTML::strong($fmt->time($rev)), ' ',
                    ' . . . ',
                    $fmt->summaryAsHTML($rev),
                    ' . . . ',
                    $fmt->authorLink($rev),
                    ($fmt->importance($rev)=='minor') ? HTML::small(" (" . _("minor edit") . ")") : '');
}

class _SpaceWiki_RecentChanges_Formatter
extends _RecentChanges_HtmlFormatter
{
    function format_revision (&$rev) {
        return SpaceWiki_RC_revision_formatter($this, $rev);
    }
    function summaryAsHTML ($rev) {
        if ( !($summary = $this->summary($rev)) )
            return '';
        return  HTML::strong( array('class' => 'wiki-summary'),
                              " ",
                              TransformLinks($summary, $rev->get('markup'), $rev->getPageName()),
                              " ");
    }

    function diffLink ($rev) {
        global $Theme;
        return $Theme->makeButton(_("diff"), $this->diffURL($rev), 'wiki-rc-action');
    }

}

class _SpaceWiki_PageHistory_Formatter
extends _PageHistory_HtmlFormatter
{
    function format_revision (&$rev) {
        return SpaceWiki_PH_revision_formatter($this, $rev);
    }
    function summaryAsHTML ($rev) {
        if ( !($summary = $this->summary($rev)) )
            return '';
        return  HTML::strong( array('class' => 'wiki-summary'),
                              " ",
                              TransformLinks($summary, $rev->get('markup'), $rev->getPageName()),
                              " ");
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
