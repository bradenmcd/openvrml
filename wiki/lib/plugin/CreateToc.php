<?php // -*-php-*-
rcs_id('$Id: CreateToc.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
/*
 Copyright 2004 $ThePhpWikiProgrammingTeam

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

/**
 * CreateToc:  Automatically link to headers
 *
 * Usage:   <?plugin CreateToc headers=!!!,!! with_toclink||=1 jshide||=1 align=left noheaders=0 ?>
 * @author:  Reini Urban
 */

class WikiPlugin_CreateToc
extends WikiPlugin
{
    function getName() {
        return _("CreateToc");
    }

    function getDescription() {
        return _("Automatically link headers at the top");
    }

    function getVersion() {
        return preg_replace("/[Revision: $]/", '',
                            "\$Revision: 1.1.1.1 $");
    }

    function getDefaultArguments() {
        return array( 'pagename'  => '[pagename]', // not sure yet. TOC of another page here?
                      // or headers=1,2,3 is also possible.
                      'headers'   => "!!!,!!,!",   // "!!!" => h1, "!!" => h2, "!" => h3
                      'noheader'  => 0,            // omit <h1>Table of Contents</h1>
                      'align'     => 'left',
                      'with_toclink' => 0,         // link back to TOC
                      'jshide'    => 0,            // collapsed TOC as DHTML button
                      'liststyle' => 'dl',         // or 'ul' or 'ol'
                      'indentstr' => '&nbsp;&nbsp;',
                      );
    }

    function preg_quote ($heading) {
        return str_replace(array("/",".","?","*"),
    		           array('\/','\.','\?','\*'), $heading);
    }
    
    function searchHeader ($content, $start_index, $heading, $level) {
    	$count = substr_count($level,'!');
    	switch ($count) {
    		case 1: $h = "h4"; break;
    		case 2: $h = "h3"; break;
    		case 3: $h = "h2"; break;
    	}
    	for ($j=$start_index; $j<count($content); $j++) {
            if (is_string($content[$j])) {
    		$heading = preg_quote($heading);
    		if (preg_match("/<$h>$heading<\/$h>/",$content[$j]))
    		    return $j;
    	    }
    	}
    	trigger_error("Heading <$h> $heading </$h> not found\n", E_USER_NOTICE);
    	return 0;
    }

    /** prevent from duplicate anchors,
     *  beautify spaces: " " => "_" and not "x20."
     */
    function _nextAnchor($s) {
        static $anchors = array();

        $s = str_replace(' ','_',$s);
        $i = 1;
        $anchor = MangleXmlIdentifier($s);
        while (!empty($anchors[$anchor])) {
            $anchor = MangleXmlIdentifier(sprintf("%s_%d",$s,$i++));
        }
        $anchors[$anchor] = $i;
        return $anchor;
    }
    
    // Feature request: proper nesting; multiple levels (e.g. 1,3)
    function extractHeaders (&$content, &$markup, $backlink=0, $levels=false, $basepage='') {
        if (!$levels) $levels = array(1,2);
        reset($levels);
        sort($levels);
        $headers = array();
        $j = 0;
        for ($i=0; $i<count($content); $i++) {
            foreach ($levels as $level) {
                if ($level < 1 or $level > 3) continue;
                if (preg_match('/^\s*(!{'.$level.','.$level.'})([^!].+)$/',$content[$i],$match)) {
                    if (!strstr($content[$i],'#[')) {
                        $s = trim($match[2]);
                        $anchor = $this->_nextAnchor($s);
                        $headers[] = array('text' => $s, 'anchor' => $anchor, 'level' => $level);
                        // Change original wikitext, but that is useless art...
                        $content[$i] = $match[1]." #[|$anchor][$s|#TOC]";
                        // And now change the to be printed markup (XmlTree):
                        // Search <hn>$s</hn> line in markup
                        //Fixme: Find wikilink'ed headers also.
                        $j = $this->searchHeader($markup->_content, $j, $s, $match[1]);
                        if (  $j and isset($markup->_content[$j]) and 
                              is_string($markup->_content[$j])  ) {
                            $x = $markup->_content[$j];
                            $heading = preg_quote($s);
                            if ($x = preg_replace('/(<h\d>)('.$heading.')(<\/h\d>)/',
                                                  "\$1<a name=\"$anchor\">\$2</a>\$3",$x,1)) {
                                if ($backlink)
                                    $x = preg_replace('/(<h\d>)('.$heading.')(<\/h\d>)/',
                                                      "\$1<a href=\"$basepage#TOC\" name=\"$anchor\">\$2</a>\$3",
                                                      $markup->_content[$j],1);
                                $markup->_content[$j] = $x;
                            }
                        }
                    }
                }
            }
        }
        return $headers;
    }
                
    function run($dbi, $argstr, $request, $basepage) {
        extract($this->getArgs($argstr, $request));
        if ($pagename) {
            // Expand relative page names.
            $page = new WikiPageName($pagename, $basepage);
            $pagename = $page->name;
        }
        if (!$pagename) {
            return $this->error(_("no page specified"));
        }
        $page = $dbi->getPage($pagename);
        $current = $page->getCurrentRevision();
        $content = $current->getContent();
        $html = HTML::div(array('class' => 'toc','align' => $align));
        if ($liststyle == 'dl')
            $list = HTML::dl(array('name'=>'toclist','id'=>'toclist','class' => 'toc'));
        elseif ($liststyle == 'ul')
            $list = HTML::ul(array('name'=>'toclist','id'=>'toclist','class' => 'toc'));
        elseif ($liststyle == 'ol')
            $list = HTML::ol(array('name'=>'toclist','id'=>'toclist','class' => 'toc'));
        if (!strstr($headers,",")) {
            $headers = array($headers);	
        } else {
            $headers = explode(",",$headers);
        }
        $levels = array();
        foreach ($headers as $h) {
            //replace !!! with level 1, ...
            if (strstr($h,"!")) {
                $hcount = substr_count($h,'!');
                $level = min(max(1, $hcount),3);
                $levels[] = $level;
            } else {
                $level = min(max(1, (int) $h), 3);
                $levels[] = $level;
            }
        }
        if ($headers = $this->extractHeaders(&$content, &$dbi->_markup, $with_toclink, $levels, $basepage)) {
            foreach ($headers as $h) {
                // proper heading indent
                $level = $h['level'];
                $indent = 3 - $level;
                $link = new WikiPageName($pagename,$page,$h['anchor']);
                $li = WikiLink($link,'known',$h['text']);
                if ($liststyle == 'dl')
                    $list->pushContent(HTML::dt(HTML::raw(str_repeat($indentstr,$indent)),$li));
                else
                    $list->pushContent(HTML::li(HTML::raw(str_repeat($indentstr,$indent)),$li));
            }
        }
        if ($jshide) {
            $list->setAttr('style','display:none;');
            $html->pushContent(Javascript("
function toggletoc(a) {
  toc=document.getElementById('toclist');
  if (toc.style.display=='none') {
    toc.style.display='block';
    a.title='"._("Click to hide the TOC")."';
  } else {
    toc.style.display='none';
    a.title='"._("Click to display")."';
  }
}"));
            $html->pushContent(HTML::h4(HTML::a(array('name'=>'TOC',
                                                      'class'=>'wikiaction',
                                                      'title'=>_("Click to display"),
                                                      'onclick'=>"toggletoc(this)"),
                                                _("Table Of Contents"))));
        } else {
            if (!$noheader)
                $html->pushContent(HTML::h2(HTML::a(array('name'=>'TOC'),_("Table Of Contents"))));
            else 
                $html->pushContent(HTML::a(array('name'=>'TOC'),""));
        }
        $html->pushContent($list);
        return $html;
    }
};

// $Log: not supported by cvs2svn $
// Revision 1.12  2004/03/22 14:13:53  rurban
// fixed links to equal named headers
//
// Revision 1.11  2004/03/15 09:52:59  rurban
// jshide button: dynamic titles
//
// Revision 1.10  2004/03/14 20:30:21  rurban
// jshide button
//
// Revision 1.9  2004/03/09 19:24:20  rurban
// custom indentstr
// h2 toc header
//
// Revision 1.8  2004/03/09 19:05:12  rurban
// new liststyle arg. default: dl (no bullets)
//
// Revision 1.7  2004/03/09 11:51:54  rurban
// support jshide=1: DHTML button hide/unhide TOC
//
// Revision 1.6  2004/03/09 10:25:37  rurban
// slightly better formatted TOC indentation
//
// Revision 1.5  2004/03/09 08:57:10  rurban
// convert space to "_" instead of "x20." in anchors
// proper heading indent
// handle duplicate headers
// allow multiple headers like "!!!,!!" or "1,2"
//
// Revision 1.4  2004/03/02 18:21:29  rurban
// typo: ref=>href
//
// Revision 1.1  2004/03/01 18:10:28  rurban
// first version, without links, anchors and jscript folding
//
//

// For emacs users
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>
