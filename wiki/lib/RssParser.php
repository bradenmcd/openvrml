<?php // -*-php-*-
rcs_id('$Id: RssParser.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');
/**
 * RSSParser Class, requires the expat extension
 * Based on Duncan Gough RSSParser class
 * Copyleft Arnaud Fontaine
 * Licence : GPL

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
 * 2004-04-09 16:30:50 rurban: 
 *   added fsockopen allow_url_fopen = Off workaround
 */
class RSSParser {

    var $title = "";
    var $link  = "";
    var $description = "";
    var $inside_item = false;
    var $items = array();
    var $channel = array();
    var $divers = "";
    var $date = "";

    function startElement($parser, $name, $attrs=''){
        global $current_tag;

        $current_tag = $name;
        if ($current_tag == "ITEM")
            $this->inside_item = true;
    } // startElement

    function endElement($parser, $tagName, $attrs=''){
        global $current_tag;

        if ($tagName == "ITEM") {
            $this->items[] = array("title" => $this->title,
                                   "description" => $this->description,
                                   "link" => $this->link);
            $this->title       = "";
            $this->description = "";
            $this->link        = "";
            $this->inside_item = false;

        } elseif ($tagName == "CHANNEL") {

            $this->channel = array("title" => $this->title,
                                   "description" => $this->description,
                                   "link" => $this->link,
                                   "date" => $this->date,
                                   "divers" => $this->divers);
            $this->title       = "";
            $this->description = "";
            $this->link        = "";
            $this->divers      = "";
            $this->date        = "";
        }
    } // endElement

    function characterData($parser, $data){
        global $current_tag;

        if ($this->inside_item){
            switch ($current_tag){

            case "TITLE":
                $this->title .= trim($data);
                break;
            case "DESCRIPTION":
                $this->description .= trim($data);
                break;
            case "LINK":
                $this->link .= trim($data);
                break;

            default:
                break;
            }
        } else {
            switch($current_tag){

            case "TITLE":
                $this->title .= trim($data);
                break;
            case "DESCRIPTION":
                $this->description .= trim($data);
                break;
            case "LINK":
                $this->link .= trim($data);
                break;
            case "DC:DATE":
                $this->date .= trim($data);
            default:
                $this->divers .= $current_tag."/".$data;
                break;
            }
        }
    } // characterData

    function parse_results($xml_parser, $rss_parser, $file)   {
        xml_set_object($xml_parser, &$rss_parser);
        xml_set_element_handler($xml_parser, "startElement", "endElement");
        xml_set_character_data_handler($xml_parser, "characterData");

        if (ini_get('allow_url_fopen')) {
            $fp = fopen("$file","r") or die("Error reading XML file, $file");
            while ($data = fread($fp, 4096))  {
                xml_parse($xml_parser, $data, feof($fp)) or 
                    die(sprintf("XML error: %s at line %d", 
                                xml_error_string(xml_get_error_code($xml_parser)), 
                                xml_get_current_line_number($xml_parser)));
            }
            fclose($fp);
        } else {
            // other url_fopen workarounds: curl, socket (http 80 only)
            require_once("lib/HttpClient.php");
            $data = HttpClient::quickGet($file);
            xml_parse($xml_parser, $data, true) or 
                die(sprintf("XML error: %s at line %d", 
                            xml_error_string(xml_get_error_code($xml_parser)), 
                            xml_get_current_line_number($xml_parser)));
        }
        xml_parser_free($xml_parser);
    }
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