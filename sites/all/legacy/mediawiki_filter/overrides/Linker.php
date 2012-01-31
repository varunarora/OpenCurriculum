<?php
/**
 * Dummy linker class
 * TODO: Major cleanup
 */
class Linker {

  function makeExternalLink( $url, $text, $escape = TRUE, $linktype = '', $ns = NULL ) {
    printLine("makeExternalLink($url, $text, $escape)");
    $style = $this->getExternalLinkAttributes( $url, $text, 'external '. $linktype );
    $url = htmlspecialchars( $url );
    if ( $escape ) {
      $text = htmlspecialchars( $text );
    }
    return '<a href="'. $url .'"'. $style .'>'. $text .'</a>';
  }

  /** @todo document */
  function tocIndent() {
    return "\n<ul>";
  }

  /** @todo document */
  function tocUnindent($level) {
    return "</li>\n" . str_repeat( "</ul>\n</li>\n", $level>0 ? $level : 0 );
  }

  /**
   * parameter level defines if we are on an indentation level
   */
  function tocLine( $anchor, $tocline, $tocnumber, $level ) {
    return "\n<li class=\"toclevel-$level\"><a href=\"#" .
      $anchor .'"><span class="tocnumber">'.
      $tocnumber .'</span> <span class="toctext">'.
      $tocline .'</span></a>';
  }

  /** @todo document */
  function tocLineEnd() {
    return "</li>\n";
  }

  /** @todo document */
  function tocList($toc) {
    global $wgJsMimeType;
    $title =  wfMsgHtml('toc') ;
    return
       '<table id="toc" class="toc" summary="'. $title .'"><tr><td>'
     .'<div id="toctitle"><strong>'. $title ."</strong></div>\n<div id=\"tocbody\">"
     . $toc
     // no trailing newline, script should not be wrapped in a
     // paragraph
     ."</ul></div>\n</td></tr></table>";
/*     . '<script type="' . $wgJsMimeType . '">'
     . ' if (window.showTocToggle) {'
     . ' var tocShowText = "' . wfEscapeJsString( wfMsg('showtoc') ) . '";'
     . ' var tocHideText = "' . wfEscapeJsString( wfMsg('hidetoc') ) . '";'
     . ' showTocToggle();'
     . ' } '
     . "</script>\n";*/
  }


  function makeHeadline( $level, $attribs, $anchor, $text, $link ) {
    return "<a name=\"$anchor\"></a><h$level$attribs$link <span class=\"mw-headline\">$text</span></h$level>";
  }

  /**
   * @param $nt Title object.
   * @param $section Integer: section number.
   * @param $hint Link String: title, or default if omitted or empty
   */
  public function editSectionLink( Title $nt, $section, $hint='' ) {
    // No edit section links provided (yet)
    return '';
  }

  /**
   * @param $nt Title object.
   * @param $section Integer: section number.
   * @param $hint Link String: title, or default if omitted or empty
   */
  public function editSectionLinkForOther( Title $nt, $section, $hint='' ) {
    // No edit section links provided (yet)
    return '';
  }

  function makeLinkObj( $nt, $text= '', $query = '', $trail = '', $prefix = '' ) {
    global $wgUser;
    $fname = 'Linker::makeLinkObj';
    wfProfileIn( $fname );

    // Fail gracefully
    if ( ! is_object($nt) ) {
      // throw new MWException();
      wfProfileOut( $fname );
      return "<!-- ERROR -->{$prefix}{$text}{$trail}";
    }

    if ( $nt->isExternal() ) {
      $local_url = url(NULL, array('absolute' => TRUE));
      $u = $nt->getFullURL();
      if (strpos($u, $local_url) === 0) {
        $class = 'internal';
      }
      else {
        $class = 'external';
      }
      $link = $nt->getPrefixedURL();
      if ( '' == $text ) {
        $text = $nt->getPrefixedText();
      }
      $style = $this->getInterwikiLinkAttributes( $link, $text, $class );

      $inside = '';
      if ( '' != $trail ) {
        $m = array();
        if ( preg_match( '/^([a-z]+)(.*)$$/sD', $trail, $m ) ) {
          $inside = $m[1];
          $trail = $m[2];
        }
      }
      $t = "<a href=\"{$u}\"{$style}>{$text}{$inside}</a>";

      wfProfileOut( $fname );
      return $t;
    }
    elseif ( $nt->isAlwaysKnown() ) {
      // Image links, special page links and self-links with fragements are always known.
      $retVal = $this->makeKnownLinkObj( $nt, $text, $query, $trail, $prefix );
    }
    else {
      wfProfileIn( $fname .'-immediate' );

      // Handles links to special pages wich do not exist in the database:
      if ( $nt->getNamespace() == NS_SPECIAL ) {
        if ( SpecialPage::exists( $nt->getDbKey() ) ) {
          $retVal = $this->makeKnownLinkObj( $nt, $text, $query, $trail, $prefix );
        }
        else {
          $retVal = $this->makeBrokenLinkObj( $nt, $text, $query, $trail, $prefix );
        }
        wfProfileOut( $fname .'-immediate' );
        wfProfileOut( $fname );
        return $retVal;
      }

      // Work out link colour immediately
      $aid = $nt->getArticleID() ;
      if ( 0 == $aid ) {
        $retVal = $this->makeBrokenLinkObj( $nt, $text, $query, $trail, $prefix );
      }
      else {
        $stub = FALSE;
        if ( $nt->isContentPage() ) {
          $threshold = $wgUser->getOption('stubthreshold');
          if ( $threshold > 0 ) {
            $dbr = wfGetDB( DB_SLAVE );
            $s = $dbr->selectRow(
              array( 'page' ),
              array( 'page_len',
                     'page_is_redirect' ),
              array( 'page_id' => $aid ), $fname ) ;
            $stub = ( $s !== FALSE && !$s->page_is_redirect &&
                $s->page_len < $threshold );
          }
        }
        if ( $stub ) {
          $retVal = $this->makeStubLinkObj( $nt, $text, $query, $trail, $prefix );
        }
        else {
          $retVal = $this->makeKnownLinkObj( $nt, $text, $query, $trail, $prefix );
        }
      }
      wfProfileOut( $fname .'-immediate' );
    }
    wfProfileOut( $fname );
    return $retVal;
  }


  /**
   * Make a link for a title which definitely exists. This is faster than makeLinkObj because
   * it doesn't have to do a database query. It's also valid for interwiki titles and special
   * pages.
   *
   * @param $nt Title object of target page
   * @param $text   String: text to replace the title
   * @param $query  String: link target
   * @param $trail  String: text after link
   * @param $prefix String: text before link text
   * @param $aprops String: extra attributes to the a-element
   * @param $style  String: style to apply - if empty, use getInternalLinkAttributesObj instead
   * @return the a-element
   */
  function makeKnownLinkObj( $nt, $text = '', $query = '', $trail = '', $prefix = '' , $aprops = '', $style = '' ) {
    $fname = 'Linker::makeKnownLinkObj';
    wfProfileIn( $fname );

    if ( !is_object( $nt ) ) {
      wfProfileOut( $fname );
      return $text;
    }

    // Get the node id of the linked node
    $linkCache =& LinkCache::singleton();
    $nid = $linkCache->getGoodLinkID($nt->getPrefixedDBkey());
    $nt->nid = $nid;

    $u = mediawiki_filter_url_for_existing_page($nt, $nt->getText(), $nt->getNamespace(), $query);
//    $u = $nt->escapeLocalURL( $query );
    if ( $nt->getFragment() != '' ) {
      if ( $nt->getPrefixedDbkey() == '' ) {
        $u = '';
        if ( '' == $text ) {
          $text = htmlspecialchars( $nt->getFragment() );
        }
      }
      $u .= $nt->getFragmentForURL();
    }
    if ( $text == '' ) {
      $text = htmlspecialchars( $nt->getPrefixedText() );
    }
    if ( $style == '' ) {
      $style = $this->getInternalLinkAttributesObj( $nt, $text );
    }

    if ( $aprops !== '' ) $aprops = ' '. $aprops;

    list( $inside, $trail ) = Linker::splitTrail( $trail );
    $r = "<a href=\"{$u}\"{$style}{$aprops}>{$prefix}{$text}{$inside}</a>{$trail}";
    wfProfileOut( $fname );

    return $r;
  }

  /**
   * Make a red link to the edit page of a given title.
   *
   * @param $title String: The text of the title
   * @param $text  String: Link text
   * @param $query String: Optional query part
   * @param $trail String: Optional trail. Alphabetic characters at the start of this string will
   *                      be included in the link text. Other characters will be appended after
   *                      the end of the link.
   */
  function makeBrokenLinkObj( $nt, $text = '', $query = '', $trail = '', $prefix = '' ) {

    // Fail gracefully
    if ( ! isset($nt) ) {
      // throw new MWException();
      return "<!-- ERROR -->{$prefix}{$text}{$trail}";
    }

    $fname = 'Linker::makeBrokenLinkObj';
    wfProfileIn( $fname );

    if ( $nt->getNamespace() == NS_SPECIAL ) {
      $q = $query;
    }
    else if ( '' == $query ) {
      $q = 'action=edit';
    }
    else {
      $q = 'action=edit&'. $query;
    }
//    $u = $nt->escapeLocalURL( $q );
    $u = mediawiki_filter_url_for_nonexisting_page($nt, $nt->getText(), $nt->getNamespace(), $q);

    if ( '' == $text ) {
      $text = htmlspecialchars( $nt->getPrefixedText() );
    }
    $style = $this->getInternalLinkAttributesObj( $nt, $text, "yes" );

    list( $inside, $trail ) = Linker::splitTrail( $trail );
    if ($u) {
      $s = "<a href=\"{$u}\"{$style}>{$prefix}{$text}{$inside}</a>{$trail}";
    }
    else {
      $s = "<span {$style}>{$prefix}{$text}{$inside}</span>{$trail}";
    }

    wfProfileOut( $fname );
    return $s;
  }

  /**
   * Create a direct link to a given uploaded file.
   *
   * @param $title Title object.
   * @param $text  String: pre-sanitized HTML
   * @return string HTML
   *
   * @public
   * @todo Handle invalid or missing images better.
   */
  function makeMediaLinkObj( $title, $text = '' ) {
    if ( is_NULL( $title ) ) {
      // HOTFIX: Instead of breaking, return empty string.
      return $text;
    }
    else {
      $img  = wfFindFile( $title );
      if ( $img ) {
        $url  = $img->getImageURL();
        $class = 'internal';
      }
      else {
        $upload = SpecialPage::getTitleFor( 'Upload' );
        $url = $upload->getLocalUrl( 'wpDestFile='. urlencode( $title->getDbKey() ) );
        $class = 'new';
      }
      $alt = htmlspecialchars( $title->getText() );
      if ( $text == '' ) {
        $text = $alt;
      }
      $u = htmlspecialchars( $url );
      return "<a href=\"{$u}\" class=\"$class\" title=\"{$alt}\">{$text}</a>";
    }
  }

  /**
   * Make a "broken" link to an image
   *
   * @param Title $title Image title
   * @param string $text Link label
   * @param string $query Query string
   * @param string $trail Link trail
   * @param string $prefix Link prefix
   * @return string
   */
  public function makeBrokenImageLinkObj( $title, $text = '', $query = '', $trail = '', $prefix = '' ) {
    if ( $title instanceof Title ) {
      wfProfileIn( __METHOD__ );
      if (node_access('create', 'image')) {
        if ( $text == '' ) {
          $text = htmlspecialchars( $title->getPrefixedText() );
        }
        list( $inside, $trail ) = self::splitTrail( $trail );
        $style = $this->getInternalLinkAttributesObj( $title, $text, 'yes' );
        wfProfileOut( __METHOD__ );
        return '<a href="'. mediawiki_filter_url_for_nonexisting_page($title, $title->getText(), NS_IMAGE) .'"'
          . $style .'>'. $prefix . $text . $inside .'</a>'. $trail;
      }
      else {
        wfProfileOut( __METHOD__ );
        return $this->makeKnownLinkObj( $title, $text, $query, $trail, $prefix );
      }
    }
    else {
      return "<!-- ERROR -->{$prefix}{$text}{$trail}";
    }
  }

  /**
   * Make an image link
   * @param Title $title Title object
   * @param File $file File object, or FALSE if it doesn't exist
   *
   * @param array $frameParams Associative array of parameters external to the media handler.
   *     Boolean parameters are indicated by presence or absence, the value is arbitrary and
   *     will often be FALSE.
   *          thumbnail       If present, downscale and frame
   *          manualthumb     Image name to use as a thumbnail, instead of automatic scaling
   *          framed          Shows image in original size in a frame
   *          frameless       Downscale but don't frame
   *          upright         If present, tweak default sizes for portrait orientation
   *          upright_factor  Fudge factor for "upright" tweak (default 0.75)
   *          border          If present, show a border around the image
   *          align           Horizontal alignment (left, right, center, none)
   *          valign          Vertical alignment (baseline, sub, super, top, text-top, middle,
   *                          bottom, text-bottom)
   *          alt             Alternate text for image (i.e. alt attribute). Plain text.
   *          caption         HTML for image caption.
   *
   * @param array $handlerParams Associative array of media handler parameters, to be passed
   *       to transform(). Typical keys are "width" and "page".
   */
  function makeImageLink2( Title $title, $file, $frameParams = array(), $handlerParams = array() ) {

    global $wgContLang, $wgUser, $wgThumbLimits, $wgThumbUpright;
    if ( $file && !$file->allowInlineDisplay() ) {
      wfDebug( __METHOD__ .': '. $title->getPrefixedDBkey() ." does not allow inline display\n" );
      return $this->makeKnownLinkObj( $title );
    }

    // Shortcuts
    $fp =& $frameParams;
    $hp =& $handlerParams;

    // Clean up parameters
    $page = isset( $hp['page'] ) ? $hp['page'] : FALSE;
    if ( !isset( $fp['align'] ) ) $fp['align'] = '';
    if ( !isset( $fp['alt'] ) ) $fp['alt'] = '';

    $prefix = $postfix = '';

    if ( 'center' == $fp['align'] ) {
      $prefix  = '<div class="center">';
      $postfix = '</div>';
      $fp['align']   = 'none';
    }
    if ( $file && !isset( $hp['width'] ) ) {
      $hp['width'] = $file->getWidth( $page );

      if ( isset( $fp['thumbnail'] ) || isset( $fp['framed'] ) || isset( $fp['frameless'] ) || !$hp['width'] ) {
        $wopt = $wgUser->getOption( 'thumbsize' );

        if ( !isset( $wgThumbLimits[$wopt] ) ) {
            $wopt = User::getDefaultOption( 'thumbsize' );
        }

        // Reduce width for upright images when parameter 'upright' is used
        if ( isset( $fp['upright'] ) && $fp['upright'] == 0 ) {
          $fp['upright'] = $wgThumbUpright;
        }
        // Use width which is smaller: real image width or user preference width
        // For caching health: If width scaled down due to upright parameter, round to full __0 pixel to avoid the creation of a lot of odd thumbs
        $prefWidth = isset( $fp['upright'] ) ?
          round( $wgThumbLimits[$wopt] * $fp['upright'], -1 ) :
          $wgThumbLimits[$wopt];
        if ( $hp['width'] <= 0 || $prefWidth < $hp['width'] ) {
          $hp['width'] = $prefWidth;
        }
      }
    }

    if ( isset( $fp['thumbnail'] ) || isset( $fp['manualthumb'] ) || isset( $fp['framed'] ) ) {

      // Create a thumbnail. Alignment depends on language
      // writing direction, # right aligned for left-to-right-
      // languages ("Western languages"), left-aligned
      // for right-to-left-languages ("Semitic languages")
      //
      // If  thumbnail width has not been provided, it is set
      // to the default user option as specified in Language*.php
      if ( $fp['align'] == '' ) {
        $fp['align'] = $wgContLang->isRTL() ? 'left' : 'right';
      }
      return $prefix . $this->makeThumbLink2( $title, $file, $fp, $hp ) . $postfix;
    }

    if ( $file && $hp['width'] ) {
      // Create a resized image, without the additional thumbnail features
      $thumb = $file->transform( $hp );
    }
    else {
      $thumb = FALSE;
    }

    if ( !$thumb ) {
      $s = $this->makeBrokenImageLinkObj( $title );
    }
    else {
      $s = $thumb->toHtml( array(
        'desc-link' => TRUE,
        'alt' => $fp['alt'],
        'valign' => isset( $fp['valign'] ) ? $fp['valign'] : FALSE ,
        'img-class' => isset( $fp['border'] ) ? 'thumbborder' : FALSE ) );
    }
    if ( '' != $fp['align'] ) {
      $s = "<div class=\"float{$fp['align']}\"><span>{$s}</span></div>";
    }
    return str_replace("\n", ' ', $prefix . $s . $postfix);
  }

  function makeThumbLink2( Title $title, $file, $frameParams = array(), $handlerParams = array() ) {
    global $wgStylePath, $wgContLang;
    $exists = $file && $file->exists();

    // Shortcuts
    $fp =& $frameParams;
    $hp =& $handlerParams;

    $page = isset( $hp['page'] ) ? $hp['page'] : FALSE;
    if ( !isset( $fp['align'] ) ) $fp['align'] = 'right';
    if ( !isset( $fp['alt'] ) ) $fp['alt'] = '';
    if ( !isset( $fp['caption'] ) ) $fp['caption'] = '';

    if ( empty( $hp['width'] ) ) {
      // Reduce width for upright images when parameter 'upright' is used
      $hp['width'] = mediawiki_filter_thumbnail_size(isset($fp['upright']));
    }
    $thumb = FALSE;

    if ( !$exists ) {
      $outerWidth = $hp['width'] + 2;
    }
    else {
      if ( isset( $fp['manualthumb'] ) ) {
        // Use manually specified thumbnail
        $manual_title = Title::makeTitleSafe( NS_IMAGE, $fp['manualthumb'] );
        if ( $manual_title ) {
          $manual_img = wfFindFile( $manual_title );
          if ( $manual_img ) {
            $thumb = $manual_img->getUnscaledThumb();
          }
          else {
            $exists = FALSE;
          }
        }
      }
      elseif ( isset( $fp['framed'] ) ) {
        // Use image dimensions, don't scale
        $thumb = $file->getUnscaledThumb( $page );
      }
      else {
        // Do not present an image bigger than the source, for bitmap-style images
        // This is a hack to maintain compatibility with arbitrary pre-1.10 behaviour
        $srcWidth = $file->getWidth( $page );
        if ( $srcWidth && !$file->mustRender() && $hp['width'] > $srcWidth ) {
          $hp['width'] = $srcWidth;
        }

        $thumb = $file->transform( $hp );
      }

      if ( $thumb ) {
        $outerWidth = $thumb->getWidth() + 2;
      }
      else {
        $outerWidth = $hp['width'] + 2;
      }
    }

    $more = htmlspecialchars( wfMsg( 'thumbnail-more' ) );
    $magnifyalign = $wgContLang->isRTL() ? 'left' : 'right';
    $textalign = $wgContLang->isRTL() ? ' style="text-align:right"' : '';

    $s = "<div class=\"thumb t{$fp['align']}\"><div class=\"thumbinner\" style=\"width:{$outerWidth}px;\">";
    if ( !$exists ) {
      $s .= $this->makeBrokenImageLinkObj( $title );
      $zoomicon = '';
    }
    elseif ( !$thumb ) {
      $s .= htmlspecialchars( wfMsg( 'thumbnail_error', '' ) );
      $zoomicon = '';
    }
    else {
      $s .= $thumb->toHtml( array(
        'alt' => $fp['alt'],
        'img-class' => 'thumbimage',
        'desc-link' => TRUE ) );
      if ( isset( $fp['framed'] ) ) {
        $zoomicon="";
      }
      else {
        $url = $file->getImageURL();
        $zoomicon =  '<div class="magnify" style="float:'. $magnifyalign .'">'.
          '<a href="'. $url .'" class="internal" title="'. $more .'">'.
          //'<img src="'.$wgStylePath.'/common/images/magnify-clip.png" ' .
          //'width="15" height="11" alt="" /></a></div>';
          '<strong>+</strong></a></div>';
      }
    }
    $s .= '  <div class="thumbcaption"'. $textalign .'>'. $zoomicon . $fp['caption'] ."</div></div></div>";
    return str_replace("\n", ' ', $s);
  }


  /** @todo document */
  function fnamePart( $url ) {
    $basename = strrchr( $url, '/' );
    if ( FALSE === $basename ) {
      $basename = $url;
    }
    else {
      $basename = substr( $basename, 1 );
    }
    return htmlspecialchars( $basename );
  }

  /** @todo document */
  function makeExternalImage( $url, $alt = '' ) {
    if ( '' == $alt ) {
      $alt = $this->fnamePart( $url );
    }
    $s = '<img src="'. $url .'" alt="'. $alt .'" />';
    return $s;
  }

  /**
   * Split a link trail, return the "inside" portion and the remainder of the trail
   * as a two-element array
   */
  static function splitTrail( $trail ) {
    static $regex = FALSE;
    if ( $regex === FALSE ) {
      global $wgContLang;
      $regex = $wgContLang->linkTrail();
    }
    $inside = '';
    if ( '' != $trail ) {
      $m = array();
      if ( preg_match( $regex, $trail, $m ) ) {
        $inside = $m[1];
        $trail = $m[2];
      }
    }
    return array( $inside, $trail );
  }

  /**
   * @param $broken 'stub' or 'yes' or FALSE
   */
  function getInternalLinkAttributesObj( &$nt, $text, $broken = FALSE ) {
    $classes = array();
    if ($nt->nid) {
      $classes = module_invoke_all('wiki_nodelink_attributes', $nt->nid);
      if (!is_array($classes))
        $classes = array();
    }
    if ( $broken == 'stub' ) {
      $classes[] = 'stub';
    }
    else if ( $broken == 'yes' ) {
      $classes[] = mediawiki_filter_nonexisting_link_css_class();
    }
    else {
    }
    $r = '';
    $r .= ' class="'. implode(' ', $classes) .'"';
    $r .= ' title="'. $nt->getEscapedText() .'"';
    return $r;
  }

  function getInterwikiLinkAttributes( $link, $text, $class='' ) {
    global $wgContLang;

    $link = urldecode( $link );
    $link = $wgContLang->checkTitleEncoding( $link );
    $link = preg_replace( '/[\\x00-\\x1f]/', ' ', $link );
    $link = htmlspecialchars( $link );

    $r = ($class != '') ? " class=\"$class\"" : " class=\"external\"";
    $r .= " title=\"{$link}\"";
    return $r;
  }

  function getExternalLinkAttributes( $link, $text, $class='' ) {
    $link = htmlspecialchars( $link );
    $r = ($class != '') ? " class=\"$class\"" : " class=\"external\"";
    $r .= " title=\"{$link}\"";
    return $r;
  }

  /**
   * This function returns an HTML link to the given target.  It serves a few
   * purposes:
   *   1) If $target is a Title, the correct URL to link to will be figured
   *      out automatically.
   *   2) It automatically adds the usual classes for various types of link
   *      targets: "new" for red links, "stub" for short articles, etc.
   *   3) It escapes all attribute values safely so there's no risk of XSS.
   *   4) It provides a default tooltip if the target is a Title (the page
   *      name of the target).
   * link() replaces the old functions in the makeLink() family.
   *
   * @param $target        Title  Can currently only be a Title, but this may
   *   change to support Images, literal URLs, etc.
   * @param $text          string The HTML contents of the <a> element, i.e.,
   *   the link text.  This is raw HTML and will not be escaped.  If null,
   *   defaults to the prefixed text of the Title; or if the Title is just a
   *   fragment, the contents of the fragment.
   * @param $customAttribs array  A key => value array of extra HTML attri-
   *   butes, such as title and class.  (href is ignored.)  Classes will be
   *   merged with the default classes, while other attributes will replace
   *   default attributes.  All passed attribute values will be HTML-escaped.
   *   A false attribute value means to suppress that attribute.
   * @param $query         array  The query string to append to the URL
   *   you're linking to, in key => value array form.  Query keys and values
   *   will be URL-encoded.
   * @param $options       mixed  String or array of strings:
   *     'known': Page is known to exist, so don't check if it does.
   *     'broken': Page is known not to exist, so don't check if it does.
   *     'noclasses': Don't add any classes automatically (includes "new",
   *       "stub", "mw-redirect", "extiw").  Only use the class attribute
   *       provided, if any, so you get a simple blue link with no funny i-
   *       cons.
   *     'forcearticlepath': Use the article path always, even with a querystring.
   *       Has compatibility issues on some setups, so avoid wherever possible.
   * @return string HTML <a> attribute
   */
  public function link( $target, $text = null, $customAttribs = array(), $query = array(), $options = array() ) {
    wfProfileIn( __METHOD__ );
    if( !$target instanceof Title ) {
      return "<!-- ERROR -->$text";
    }
    $options = (array)$options;

    $ret = null;
    if( !wfRunHooks( 'LinkBegin', array( $this, $target, &$text,
    &$customAttribs, &$query, &$options, &$ret ) ) ) {
      wfProfileOut( __METHOD__ );
      return $ret;
    }

    # Normalize the Title if it's a special page
    // $target = $this->normaliseSpecialPage( $target );

    # If we don't know whether the page exists, let's find out.
    wfProfileIn( __METHOD__ . '-checkPageExistence' );
    if( !in_array( 'known', $options ) and !in_array( 'broken', $options ) ) {
      if( $target->isKnown() ) {
        $options []= 'known';
      } else {
        $options []= 'broken';
      }
    }
    wfProfileOut( __METHOD__ . '-checkPageExistence' );

    $oldquery = array();
    if( in_array( "forcearticlepath", $options ) && $query ){
      $oldquery = $query;
      $query = array();
    }

    # Note: we want the href attribute first, for prettiness.
    $attribs = array( 'href' => $this->linkUrl( $target, $query, $options ) );
    if( in_array( 'forcearticlepath', $options ) && $oldquery ){
      $attribs['href'] = wfAppendQuery( $attribs['href'], wfArrayToCgi( $oldquery ) );
    }

    $attribs = array_merge(
      $attribs,
      $this->linkAttribs( $target, $customAttribs, $options )
    );
    if( is_null( $text ) ) {
      $text = $this->linkText( $target );
    }

    $ret = null;
    if( wfRunHooks( 'LinkEnd', array( $this, $target, $options, &$text, &$attribs, &$ret ) ) ) {
      $ret = Xml::openElement( 'a', $attribs ) . $text . Xml::closeElement( 'a' );
    }

    wfProfileOut( __METHOD__ );
    return $ret;
  }

  /**
   * Return the CSS colour of a known link
   *
   * @param Title $t
   * @param integer $threshold user defined threshold
   * @return string CSS class
   */
  function getLinkColour( $t, $threshold ) {
    $colour = '';

    // Different colors are not supported yet.
    // 2010-05-30, bherlig
    /*
    if ( $t->isRedirect() ) {
      # Page is a redirect
      $colour = 'mw-redirect';
    } elseif ( $threshold > 0 &&
         $t->exists() && $t->getLength() < $threshold &&
         MWNamespace::isContent( $t->getNamespace() ) ) {
      # Page is a stub
      $colour = 'stub';
    }
    */
    return $colour;
  }

  /**
   * @deprecated Use link()
   *
   * Make a coloured link.
   *
   * @param $nt Title object of the target page
   * @param $colour Integer: colour of the link
   * @param $text   String:  link text
   * @param $query  String:  optional query part
   * @param $trail  String:  optional trail. Alphabetic characters at the start of this string will
   *                      be included in the link text. Other characters will be appended after
   *                      the end of the link.
   */
  function makeColouredLinkObj( $nt, $colour, $text = '', $query = '', $trail = '', $prefix = '' ) {
    if($colour != ''){
      $style = $this->getInternalLinkAttributesObj( $nt, $text, $colour );
    } else $style = '';
    return $this->makeKnownLinkObj( $nt, $text, $query, $trail, $prefix, '', $style );
  }

  /**
   * Create a section edit link.  This supersedes editSectionLink() and
   * editSectionLinkForOther().
   *
   * @param $nt      Title  The title being linked to (may not be the same as
   *   $wgTitle, if the section is included from a template)
   * @param $section string The designation of the section being pointed to,
   *   to be included in the link, like "&section=$section"
   * @param $tooltip string The tooltip to use for the link: will be escaped
   *   and wrapped in the 'editsectionhint' message
   * @return         string HTML to use for edit link
   */
  public function doEditSectionLink( Title $nt, $section, $tooltip = null ) {

    // No edit section links provided (yet)
    return null;

    /*
    $attribs = array();
    if( !is_null( $tooltip ) ) {
      $attribs['title'] = wfMsg( 'editsectionhint', $tooltip );
    }
    $link = $this->link( $nt, wfMsg('editsection'),
      $attribs,
      array( 'action' => 'edit', 'section' => $section ),
      array( 'noclasses', 'known' )
    );

    # Run the old hook.  This takes up half of the function . . . hopefully
    # we can rid of it someday.
    $attribs = '';
    if( $tooltip ) {
      $attribs = wfMsgHtml( 'editsectionhint', htmlspecialchars( $tooltip ) );
      $attribs = " title=\"$attribs\"";
    }
    $result = null;
    wfRunHooks( 'EditSectionLink', array( &$this, $nt, $section, $attribs, $link, &$result ) );
    if( !is_null( $result ) ) {
      # For reverse compatibility, add the brackets *after* the hook is
      # run, and even add them to hook-provided text.  (This is the main
      # reason that the EditSectionLink hook is deprecated in favor of
      # DoEditSectionLink: it can't change the brackets or the span.)
      $result = wfMsgHtml( 'editsection-brackets', $result );
      return "<span class=\"editsection\">$result</span>";
    }

    # Add the brackets and the span, and *then* run the nice new hook, with
    # clean and non-redundant arguments.
    $result = wfMsgHtml( 'editsection-brackets', $link );
    $result = "<span class=\"editsection\">$result</span>";

    wfRunHooks( 'DoEditSectionLink', array( $this, $nt, $section, $tooltip, &$result ) );
    return $result;
    */
  }

  private function linkUrl( $target, $query, $options ) {
    wfProfileIn( __METHOD__ );
    # We don't want to include fragments for broken links, because they
    # generally make no sense.
    if( in_array( 'broken', $options ) and $target->mFragment !== '' ) {
      $target = clone $target;
      $target->mFragment = '';
    }

    # If it's a broken link, add the appropriate query pieces, unless
    # there's already an action specified, or unless 'edit' makes no sense
    # (i.e., for a nonexistent special page).
    if( in_array( 'broken', $options ) and empty( $query['action'] )
    and $target->getNamespace() != NS_SPECIAL ) {
      $query['action'] = 'edit';
      $query['redlink'] = '1';
    }
    $ret = $target->getLinkUrl( $query );
    wfProfileOut( __METHOD__ );
    return $ret;
  }

  private function linkAttribs( $target, $attribs, $options ) {
    wfProfileIn( __METHOD__ );
    global $wgUser;
    $defaults = array();

    if( !in_array( 'noclasses', $options ) ) {
      wfProfileIn( __METHOD__ . '-getClasses' );
      # Now build the classes.
      $classes = array();

      if( in_array( 'broken', $options ) ) {
        $classes[] = 'new';
      }

      if( $target->isExternal() ) {
        $classes[] = 'extiw';
      }

      // Add interwiki namespace to the classes
      if( isset($target->mInterwiki)) {
        $classes[] = $target->mInterwiki;
      }

      # Note that redirects never count as stubs here.
      if ( $target->isRedirect() ) {
        $classes[] = 'mw-redirect';
      } elseif( $target->isContentPage() ) {
        # Check for stub.
        $threshold = $wgUser->getOption( 'stubthreshold' );
        if( $threshold > 0 and $target->exists() and $target->getLength() < $threshold ) {
          $classes[] = 'stub';
        }
      }
      if( $classes != array() ) {
        $defaults['class'] = implode( ' ', $classes );
      }
      wfProfileOut( __METHOD__ . '-getClasses' );
    }

    # Get a default title attribute.
    if( in_array( 'known', $options ) ) {
      $defaults['title'] = $target->getPrefixedText();
    } else {
      $defaults['title'] = wfMsg( 'red-link-title', $target->getPrefixedText() );
    }

    # Finally, merge the custom attribs with the default ones, and iterate
    # over that, deleting all "false" attributes.
    $ret = array();
    $merged = Sanitizer::mergeAttributes( $defaults, $attribs );
    foreach( $merged as $key => $val ) {
      # A false value suppresses the attribute, and we don't want the
      # href attribute to be overridden.
      if( $key != 'href' and $val !== false ) {
        $ret[$key] = $val;
      }
    }
    wfProfileOut( __METHOD__ );
    return $ret;
  }
}
