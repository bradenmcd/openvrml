<?php // -*-php-*-
rcs_id('$Id: pdf.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');

// PDF functions taken from FPDF http://www.fpdf.org
// Edited for PHPWebthings by Don Sebà 
// Feel free to edit , enhance the module, and please share it at http://www.phpdbform.com
// Keep PHPWT COOL submit your modules/themes/mods, it will help to improve ! :)

require_once('lib/fpdf.php');

class PDF extends FPDF {
    var $B = 0;
    var $I = 0;
    var $U = 0;
    var $HREF = '';

    function PDF ($orientation='P', $unit='mm', $format='A4') {
        $this->FPDF($orientation,$unit,$format);
    }

    // Simple HTML to PDF converter
    function ConvertFromHTML($html) {
        $html = str_replace("\n",' ',$html);
        $a = preg_split('/<(.*)>/U',$html,-1,PREG_SPLIT_DELIM_CAPTURE);
        foreach($a as $i=>$e) {
            if ($i % 2 == 0) {
                //Tekst
                if($this->HREF)
                    $this->PutLink($this->HREF,$e);
                else
                    $this->Write(5,$e);
            } else {
                //Tag
                if ($e{0} == '/')
                    $this->CloseTag(strtoupper(substr($e,1)));
                else {
                    //Filter de attributen
                    $a2 = explode(' ',$e);
                    $tag = strtoupper(array_shift($a2));
                    $attr = array();
                    foreach ($a2 as $v)
                        if (ereg('^([^=]*)=["\']?([^"\']*)["\']?$',$v,$a3))
                            $attr[strtoupper($a3[1])]=$a3[2];
                    $this->OpenTag($tag,$attr);
                }
            }
        }
    }

    /* not yet used */
    function Header() {
	//URL - space from side - space from top - width
        $imgurl = "..."; // header and wikilogo
        $this->Image($imgurl,3,3,$imgwidth);
        //Line break
        $this->Ln(30);
    }

    /* not yet used */
    function Footer() {
        //global $cfg, $config, $lang;
        //Positie 1.5 cm van de onderkant
        $this->SetY(-15);
        //Arial cursief 8
        $this->SetFont('Arial','I',8);
        //Page number
        //$this->Cell(0,10,$cfg["core"]["url"],0,0,'C');
        //$this->Cell(0,10,' Page '.$this->PageNo().'/{nb}',0,0,'C');
    }

    function OpenTag($tag,$attr) {
        if($tag=='B' or $tag=='I' or $tag=='U')
            $this->SetStyle($tag,true);
        if($tag=='A')
            $this->HREF=$attr['HREF'];
        if($tag=='BR')
            $this->Ln(5);
    }

    function CloseTag($tag) {
        if($tag=='B' or $tag=='I' or $tag=='U')
            $this->SetStyle($tag,false);
        if($tag=='A')
            $this->HREF='';
    }
    
    //Wijzig stijl en selecteer lettertype
    function SetStyle($tag,$enable) {
        $this->$tag+=($enable ? 1 : -1);
        $style='';
        foreach(array('B','I','U') as $s)
            if($this->$s>0)
                $style.=$s;
        $this->SetFont('',$style);
    }

    function PutLink($URL,$txt) {
        // hyperlink as simple underlined text
        $this->SetTextColor(0,0,255);
        $this->SetStyle('U',true);
        $this->Write(5,$txt,$URL);
        $this->SetStyle('U',false);
        $this->SetTextColor(0);
    }
}

// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:
?>
