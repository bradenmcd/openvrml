<?php
/**
 * Amazon API
 *
 * @author: Chris Petersen
 */
/*
Copyright (c) 2002 Intercept Vector

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

If you have any questions or comments, please email:

Chris Petersen
admin@interceptvector.com
Intercept Vector
http://www.interceptvector.com
*/
  /**
   * @seealso: http://scripts.incutio.com/amazon/
   */

function getAmazonId() {
    return "xxxxxxxxxxxxxx";
}

function getCacheDirectory() {
    return "/some/directory/";
}

class AmazonProduct {
    var $url;
    var $asin;
    var $name;
    var $catalog;
    var $releaseDate;
    var $manufacturer;
    var $imgSmall;
    var $imgMedium;
    var $imgLarge;
    var $listPrice;
    var $ourPrice;
    var $usedPrice;
    var $coolectiblePrice;
    var $thirdPartyNewPrice;
    var $salesRank;
    var $media;
    var $numMedia;
    var $isbn;
    var $esrbRating;
    var $availablity;
    var $ucp;
    var $mpn;
    var $averageCustomerRating;

    var $authors;
    var $lists;
    var $similarProducts;
    var $platforms;

    var $reviews;

    function load($domNode) {
        $this->url = $domNode->get_attribute("url");
        $children = $domNode->child_nodes();
        for($i=0; $i<count($children); $i++) {
            $node = $children[$i];
            switch($node->node_name()) {
                case 'Asin':             $this->asin             = $node->get_content(); break;
                case 'ProductName':      $this->name             = $node->get_content(); break;
                case 'Catalog':          $this->catalog          = $node->get_content(); break;
                case 'ReleaseDate':      $this->releaseDate      = $node->get_content(); break;
                case 'Manufacturer':     $this->manufacturer     = $node->get_content(); break;
                case 'ImageUrlSmall':    $this->imgSmall         = $node->get_content(); break;
                case 'ImageUrlMedium':   $this->imgMedium        = $node->get_content(); break;
                case 'ImageUrlLarge':    $this->imgLarge         = $node->get_content(); break;
                case 'ListPrice':        $this->listPrice        = $node->get_content(); break;
                case 'OurPrice':         $this->ourPrice         = $node->get_content(); break;
                case 'UsedPrice':        $this->usedPrice        = $node->get_content(); break;
                case 'CollectiblePrice': $this->collectiblePrice = $node->get_content(); break;
                case 'ThirdPartyNewPrice':    $this->thirdPartyNewPrice  = $node->get_content(); break;
                case 'SalesRank':        $this->salesRank        = $node->get_content(); break;
                case 'Media':            $this->media            = $node->get_content(); break;
                case 'NumMedia':         $this->numMedia         = $node->get_content(); break;
                case 'Isbn':             $this->isbn             = $node->get_content(); break;
                case 'Availability':     $this->availability     = $node->get_content(); break;
                case 'EsrbRating':       $this->esrbRating       = $node->get_content(); break;
                case 'Upc':              $this->upc              = $node->get_content(); break;
                case 'Mpn':              $this->mpn              = $node->get_content(); break;

                case 'Lists':
                    $this->lists = array();
                    $children2 = $node->child_nodes();
                    for($j=0; $j<count($children2); $j++) {
                        $node2 = $children2[$j];
                        array_push($this->lists, $node2->get_content());
                    }
                    break;
                case 'Authors':
                    $this->authors = array();
                    $children2 = $node->child_nodes();
                    for($j=0; $j<count($children2); $j++) {
                        $node2 = $children2[$j];
                        array_push($this->authors, $node2->get_content());
                    }
                    break;
                case 'SimilarProducts':
                    $this->similarProducts = array();
                    $children2 = $node->child_nodes();
                    for($j=0; $j<count($children2); $j++) {
                        $node2 = $children2[$j];
                        array_push($this->similarProducts, $node2->get_content());
                    }
                    break;
                case 'Platforms':
                    $this->platforms = array();
                    $children2 = $node->child_nodes();
                    for($j=0; $j<count($children2); $j++) {
                        $node2 = $children2[$j];
                        array_push($this->platforms, $node2->get_content());
                    }
                    break;
                case 'Reviews':
                    $this->reviews = array();
                    $children2 = $node->child_nodes();
                    for($j=0; $j<count($children2); $j++) {
                        $node2 = $children2[$j];
                        switch($node2->node_name()) {
                            case 'AvgCustomerRating': 
                                $this->averageCustomerRating = $node2->get_content(); 
                                break;
                            case 'CustomerReview':
                                $review = new AmazonCustomerReview();
                                $review->load($node2);
                                array_push($this->reviews, $review);
                                break;
                        }
                    }
                    break;
            }
        }
    }
}

class AmazonCustomerReview {
    var $rating;
    var $summary;
    var $comment;

    function load($domNode) {
        $children = $domNode->child_nodes();
        for($i=0; $i<count($children); $i++) {
            $node = $children[$i];
            switch($node->node_name()) {
                case 'Rating':     $this->rating    = $node->get_content(); break;
                case 'Summary':    $this->summary   = $node->get_content(); break;
                case 'Comment':    $this->comment   = $node->get_content(); break;
            }
        }
    }
}

function amazonKeywordSearch($searchTerm, $catalog) {
    $id = getAmazonId();
    $url="http://xml.amazon.com/onca/xml2?t=webservices-20&dev-t=$id&".
          "KeywordSearch=$searchTerm&mode=$catalog&type=heavy&page=1&f=xml";
    if(!$dom = domxml_open_file($url)) {
        die("DOM Error");
    }
    $root = $dom->document_element();
    $tags = $root->get_elements_by_tagname("Details");
    $results = array();
    for($i=0; $i<count($tags); $i++) {
        $details = $tags[$i];
        $asin = $details->get_elements_by_tagname("Asin");
        $newDoc = domxml_new_doc("1.0");
        $newDoc->append_child($details);
        $newDoc->dump_file(getCacheDirectory()."/amazon/".$asin[0]->get_content());
        $result = new AmazonProduct();
        $result->load($details);
        array_push($results, $result);
    }
    return $results;
}

function amazonProduct($asin) {
    $id = getAmazonId();
    $url = "http://xml.amazon.com/onca/xml2?t=webservices-20&dev-t=$id&".
           "AsinSearch=$asin&type=heavy&f=xml";
    $cache_file = getCacheDirectory()."/amazon/".$asin;
    $cache_time = 60*60*24; // cache for one day

    if( (!(file_exists($cache_file)))                ||
        (filectime($cache_file) + $cache_time <= time() )    ||
        (!(filesize($cache_file)))                ){

        if(!$dom = domxml_open_file($url)) {
            die("DOM Error");
        }
        $root = $dom->document_element();
        $tags = $root->get_elements_by_tagname("Details");
        $details = $tags[0];
        $newDoc = domxml_new_doc("1.0");
        $newDoc->append_child($details);
        $newDoc->dump_file(getCacheDirectory()."/amazon/$asin");
    } else {
        if(!$dom = domxml_open_file($cache_file)) {
            die("DOM Error");
        }
        $details = $dom->document_element();
    }
    $result = new AmazonProduct();
    $result->load($details);
    return $result;
}

function getAmazonCatalogs() {
    $result = array();
    $results[0]  = "music";
    $results[1]  = "classical";
    $results[2]  = "books";
    $results[3]  = "dvd";
    $results[4]  = "vhs";
    $results[5]  = "video";
    $results[6]  = "electronics";
    $results[7]  = "kitchen";
    $results[8]  = "videogames";
    $results[9]  = "software";
    $results[10] = "photo";
    $results[11] = "pc-hardware";
    return $results;
}

// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>