<?php // -*-php-*-
rcs_id('$Id: GoTo.php,v 1.1.1.1 2004-04-13 23:41:40 braden Exp $');

/**
 *  Display a form with text entry box and 'Go' button.
 *  The user enters a page name... if it exists, browse
 *  that page; if not, edit (create) that page.
 *  Note: pagenames are absolute, not relative to the actual subpage.
 *
 *  Usage: <?plugin GoTo ?>
 *  @author: Michael van Dam
 */

class WikiPlugin_GoTo
extends WikiPlugin
{
    function getName () {
        return _("GoTo");
    }

    function getDescription () {
        return _("Go to or create page.");
    }
    
    function getDefaultArguments() {
        return array();
    }

    function run($dbi, $argstr, $request, $basepage) {
        $request->setArg('action',false);
        $args = $this->getArgs($argstr, $request);
        extract($args);

        if ($goto = $request->getArg('goto')) {
            // The user has pressed 'Go'; process request
            $request->setArg('goto', false);
            $target = $goto['target'];
            if ($dbi->isWikiPage($target))
                $url = WikiURL($target,0,1);
            else
                $url = WikiURL($target, array('action'=>'edit'),1);

            $request->redirect($url);
            // User should see nothing after redirect
            return '';
        } 

        $action = $request->getURLtoSelf();
        $form = HTML::form(array('action'=>$action,
                                 'method'=>'post'
                          ));
 
        $form->pushContent(HiddenInputs($request->getArgs()));

        $textfield = HTML::input(array('type'=>'text',
                                       'size'=>32,
                                       'name'=>'goto[target]'));

        $button = Button('submit:goto[go]', _("Go"), false);

        $form->pushContent($textfield, $button);

        return $form;

    }
};

// $Log: not supported by cvs2svn $
        
// Local Variables:
// mode: php
// tab-width: 8
// c-basic-offset: 4
// c-hanging-comment-ender-p: nil
// indent-tabs-mode: nil
// End:   
?>
