<?php

/**
 * Dummy title class
 * TODO: Major cleanup
 */
class Title {

  /**
   * Static cache variables
   */
  static private $titleCache=array();
  static private $interwikiCache=array();

  var $mTextform;           // Text form (spaces not underscores) of the main part
  var $mUrlform;            // URL-encoded form of the main part
  var $mDbkeyform;          // Main part with underscores
  var $mNamespace;          // Namespace index, i.e. one of the NS_xxxx constants
  var $mInterwiki;          // Interwiki prefix (or NULL string)
  var $mArticleID;          // Article ID, fetched from the link cache on demand
  var $mPrefixedText;       // Text form including namespace/interwiki, initialised on demand

  /**
   * Constructor
   * @private
   */
  function __construct() {
    $this->mInterwiki = $this->mUrlform = '';
    $this->mTextform = $this->mDbkeyform = '';
    $this->mArticleID = -1;
    $this->mNamespace = NS_MAIN;
    $this->unmodifiedForm = '';
  }

  /**
   * Create a new Title from text, such as what one would
   * find in a link. Decodes any HTML entities in the text.
   *
   * @param string $text the link text; spaces, prefixes,
   *  and an initial ':' indicating the main namespace
   *  are accepted
   * @param int $defaultNamespace the namespace to use if
   *   none is specified by a prefix
   * @return Title the new object, or NULL on an error
   */
  public static function newFromText( $text, $defaultNamespace = NS_MAIN ) {
    if ( is_object( $text ) ) {
      throw new MWException( 'Title::newFromText given an object' );
    }

    /**
     * Wiki pages often contain multiple links to the same page.
     * Title normalization and parsing can become expensive on
     * pages with many links, so we can save a little time by
     * caching them.
     *
     * In theory these are value objects and won't get changed...
     */
    if ( $defaultNamespace == NS_MAIN && isset( Title::$titleCache[$text] ) ) {
      return Title::$titleCache[$text];
    }

    /**
     * Convert things like &eacute; &#257; or &#x3017; into real text...
     */
    $filteredText = Sanitizer::decodeCharReferences( $text );

    $t = new Title();
    $t->mDbkeyform = str_replace( ' ', '_', $filteredText );
    $t->mDefaultNamespace = $defaultNamespace;
    $t->unmodifiedForm = $text;

    if ( $t->secureAndSplit() ) {
      if ( $defaultNamespace == NS_MAIN ) {
        Title::$titleCache[$text] =& $t;
      }
      return $t;
    }
    else {
      $ret = NULL;
      return $ret;
    }
  }

  /**
   * Create a new Title from a namespace index and a DB key.
   * It's assumed that $ns and $title are *valid*, for instance when
   * they came directly from the database or a special page name.
   * For convenience, spaces are converted to underscores so that
   * eg user_text fields can be used directly.
   *
   * @param int $ns the namespace of the article
   * @param string $title the unprefixed database key form
   * @return Title the new object
   */
  public static function &makeTitle( $ns, $title ) {
    $t = new Title();
    $t->mInterwiki = '';
    $t->mFragment = '';
    $t->mNamespace = $ns = intval( $ns );
    $t->mDbkeyform = str_replace( ' ', '_', $title );
    $t->mArticleID = ( $ns >= 0 ) ? -1 : 0;
    $t->mUrlform = wfUrlencode( $t->mDbkeyform );
    $t->mTextform = str_replace( '_', ' ', $title );
    $t->unmodifiedForm = $title;
    return $t;
  }

  /**
   * Secure and split - main initialisation function for this object
   *
   * Assumes that mDbkeyform has been set, and is urldecoded
   * and uses underscores, but not otherwise munged.  This function
   * removes illegal characters, splits off the interwiki and
   * namespace prefixes, sets the other forms, and canonicalizes
   * everything.
   * @return bool TRUE on success
   */
  private function secureAndSplit() {
    global $wgContLang, $wgLocalInterwiki, $wgCapitalLinks;

    // Initialisation
    static $rxTc = FALSE;
    if ( !$rxTc ) {
      // % is needed as well
      $rxTc = '/[^'. Title::legalChars() .']|%[0-9A-Fa-f]{2}/S';
    }

    $this->mInterwiki = $this->mFragment = '';
    $this->mNamespace = $this->mDefaultNamespace; // Usually NS_MAIN

    $dbkey = $this->mDbkeyform;
    $unmodifiedForm = $this->unmodifiedForm;

    // Strip Unicode bidi override characters.
    // Sometimes they slip into cut-n-pasted page titles, where the
    // override chars get included in list displays.
    $dbkey = str_replace( "\xE2\x80\x8E", '', $dbkey ); // 200E LEFT-TO-RIGHT MARK
    $dbkey = str_replace( "\xE2\x80\x8F", '', $dbkey ); // 200F RIGHT-TO-LEFT MARK

    // Clean up whitespace
    $dbkey = preg_replace( '/[ _]+/', '_', $dbkey );
    $dbkey = trim( $dbkey, '_' );

    if ( '' == $dbkey ) {
      return FALSE;
    }

    if ( FALSE !== strpos( $dbkey, UTF8_REPLACEMENT ) ) {
      // Contained illegal UTF-8 sequences or forbidden Unicode chars.
      return FALSE;
    }

    $this->mDbkeyform = $dbkey;

    // Initial colon indicates main namespace rather than specified default
    // but should not create invalid {ns,title} pairs such as {0,Project:Foo}
    if ( ':' == $dbkey{0} ) {
      $this->mNamespace = NS_MAIN;
      $dbkey = substr( $dbkey, 1 ); // remove the colon but continue processing
      $dbkey = trim( $dbkey, '_' ); // remove any subsequent whitespace
    }

    // Namespace or interwiki prefix
    $firstPass = TRUE;
    do {
      $m = array();
      if ( preg_match( "/^(.+?)_*:_*(.*)$/S", $dbkey, $m ) ) {
        $p = $m[1];
        if ( $ns = $wgContLang->getNsIndex( $p )) {
          // Ordinary namespace
          $dbkey = $m[2];
          $this->mNamespace = $ns;

          // parse away the namespace from the "unmodified" form.
          $matches = array();
          if ( preg_match( "/^(.+?)_*:_*(.*)$/S", $unmodifiedForm, $matches ) ) {
            $this->unmodifiedForm = $matches[2];
          }
          unset($matches);
          unset($unmodifiedForm);

        }
        elseif ( $this->getInterwikiLink( $p ) ) {
          if ( !$firstPass ) {
            // Can't make a local interwiki link to an interwiki link.
            // That's just crazy!
            return FALSE;
          }

          // Interwiki link
          $dbkey = $m[2];
          $this->mInterwiki = $wgContLang->lc( $p );

          // Redundant interwiki prefix to the local wiki
          if ( 0 == strcasecmp( $this->mInterwiki, $wgLocalInterwiki ) ) {
            if ( $dbkey == '' ) {
              // Can't have an empty self-link
              return FALSE;
            }
            $this->mInterwiki = '';
            $firstPass = FALSE;
            // Do another namespace split...
            continue;
          }

          // If there's an initial colon after the interwiki, that also
          // resets the default namespace
          if ( $dbkey !== '' && $dbkey[0] == ':' ) {
            $this->mNamespace = NS_MAIN;
            $dbkey = substr( $dbkey, 1 );
          }
        }
        // If there's no recognized interwiki or namespace,
        // then let the colon expression be part of the title.
      }
      break;
    } while ( TRUE );

    // We already know that some pages won't be in the database!
    if ( '' != $this->mInterwiki || NS_SPECIAL == $this->mNamespace ) {
      $this->mArticleID = 0;
    }
    $fragment = strstr( $dbkey, '#' );
    if ( FALSE !== $fragment ) {
      $this->setFragment( $fragment );
      $dbkey = substr( $dbkey, 0, strlen( $dbkey ) - strlen( $fragment ) );
      // remove whitespace again: prevents "Foo_bar_#"
      // becoming "Foo_bar_"
      $dbkey = preg_replace( '/_*$/', '', $dbkey );
    }

    // Reject illegal characters.
    if ( preg_match( $rxTc, $dbkey ) ) {
      return FALSE;
    }

    /**
     * Pages with "/./" or "/../" appearing in the URLs will
     * often be unreachable due to the way web browsers deal
     * with 'relative' URLs. Forbid them explicitly.
     */
    if ( strpos($dbkey, '.') !== FALSE &&
         ( $dbkey === '.' || $dbkey === '..' ||
           strpos( $dbkey, './' ) === 0  ||
           strpos( $dbkey, '../' ) === 0 ||
           strpos( $dbkey, '/./' ) !== FALSE ||
           strpos( $dbkey, '/../' ) !== FALSE ) ) {
      return FALSE;
    }

    /**
     * Magic tilde sequences? Nu-uh!
     */
    if ( strpos( $dbkey, '~~~' ) !== FALSE ) {
      return FALSE;
    }

    /**
     * Limit the size of titles to 255 bytes.
     * This is typically the size of the underlying database field.
     * We make an exception for special pages, which don't need to be stored
     * in the database, and may edge over 255 bytes due to subpage syntax
     * for long titles, e.g. [[Special:Block/Long name]]
     */
    if ( ( $this->mNamespace != NS_SPECIAL && strlen( $dbkey ) > 255 ) ||
      strlen( $dbkey ) > 512 ) {
      return FALSE;
    }

    /**
     * Normally, all wiki links are forced to have
     * an initial capital letter so [[foo]] and [[Foo]]
     * point to the same place.
     *
     * Don't force it for interwikis, since the other
     * site might be case-sensitive.
     */
    $this->mUserCaseDBKey = $dbkey;
    if ( $wgCapitalLinks && $this->mInterwiki == '') {
      $dbkey = $wgContLang->ucfirst( $dbkey );
    }

    /**
     * Can't make a link to a namespace alone...
     * "empty" local links can only be self-links
     * with a fragment identifier.
     */
    if ( $dbkey == '' &&
      $this->mInterwiki == '' &&
      $this->mNamespace != NS_MAIN ) {
      return FALSE;
    }
    // Allow IPv6 usernames to start with '::' by canonicalizing IPv6 titles.
    // IP names are not allowed for accounts, and can only be referring to
    // edits from the IP. Given '::' abbreviations and caps/lowercaps,
    // there are numerous ways to present the same IP. Having sp:contribs scan
    // them all is silly and having some show the edits and others not is
    // inconsistent. Same for talk/userpages. Keep them normalized instead.
    //
    // TODO: Is this necessary
    //    $dbkey = ($this->mNamespace == NS_USER || $this->mNamespace == NS_USER_TALK) ?
    //      IP::sanitizeIP( $dbkey ) : $dbkey;
    // Any remaining initial :s are illegal.
    if ( $dbkey !== '' && ':' == $dbkey{0} ) {
      return FALSE;
    }

    // Fill fields
    $this->mDbkeyform = $dbkey;
    $this->mUrlform = wfUrlencode( $dbkey );

    $this->mTextform = str_replace( '_', ' ', $dbkey );

    return TRUE;
  }

  /**
   * Get a regex character class describing the legal characters in a link
   * @return string the list of characters, not delimited
   */
  public static function legalChars() {
    global $wgLegalTitleChars;
    return $wgLegalTitleChars;
  }

  /**
   * Is this a talk page of some sort?
   * @return bool
   */
  public function isTalkPage() {
    return FALSE;
  }

  /**
   * Get the prefixed title with spaces.
   * This is the form usually used for display
   * @return string the prefixed title, with spaces
   */
  public function getPrefixedText() {
    if ( empty( $this->mPrefixedText ) ) { // FIXME: bad usage of empty() ?
      $s = $this->prefix( $this->mTextform );
      $s = str_replace( '_', ' ', $s );
      $this->mPrefixedText = $s;
    }
    return $this->mPrefixedText;
  }

  public function quickUserCan( $action ) {
    printLine("Title.quickUserCan($action)");
    return TRUE;
    return $this->userCan( $action, FALSE );
  }

  /**
   * Prefix some arbitrary text with the namespace or interwiki prefix
   * of this object
   *
   * @param string $name the text
   * @return string the prefixed text
   * @private
   */
  function prefix( $name ) {
    $p = '';
    if ( '' != $this->mInterwiki ) {
            $p = $this->mInterwiki .':';
    }
    if ( 0 != $this->mNamespace ) {
            $p .= $this->getNsText() .':';
    }
    return $p . $name;
  }

  /**
   * Get an HTML-escaped version of the URL form, suitable for
   * using in a link, without a server name or fragment
   * @param string $query an optional query string
   * @return string the URL
   */
  public function escapeLocalURL( $query = '' ) {
    return htmlspecialchars( $this->getLocalURL( $query ) );
  }

  public function getArticleID() { return $this->mArticleID; }

  /**
   * Get the text form (spaces not underscores) of the main part
   * @return string
   */
  public function getText() { return $this->mTextform; }

  /**
   * Get the URL-encoded form of the main part
   * @return string
   */
  public function getPartialURL() { return $this->mUrlform; }

  /**
   * Get the main part with underscores
   * @return string
   */
  public function getDBkey() { return $this->mDbkeyform; }

  /**
   * Unmodified Form of the Title.
   * This contains all characters "as is", e.g. no underscore replacement takes place.
   * This is used for Drupal compability for Image titles, as the node titles are stored unmodified in the {node} table.
   */
  public function getUnmodifiedForm() { return $this->unmodifiedForm; }

  /**
   * Get the namespace index, i.e. one of the NS_xxxx constants
   * @return int
   */
  public function getNamespace() { return $this->mNamespace; }

  /**
   * Get the namespace text
   * @return string
   */
  public function getNsText() {
    global $wgContLang, $wgCanonicalNamespaceNames;

    if ( '' != $this->mInterwiki ) {
      // This probably shouldn't even happen. ohh man, oh yuck.
      // But for interwiki transclusion it sometimes does.
      // Shit. Shit shit shit.
      //
      // Use the canonical namespaces if possible to try to
      // resolve a foreign namespace.
      if ( isset( $wgCanonicalNamespaceNames[$this->mNamespace] ) ) {
        return $wgCanonicalNamespaceNames[$this->mNamespace];
      }
    }
    return $wgContLang->getNsText( $this->mNamespace );
  }

  /**
   * Get the interwiki prefix (or NULL string)
   * @return string
   */
  public function getInterwiki() { return $this->mInterwiki; }

  /**
   * Get the Title fragment (i.e. the bit after the #) in text form
   * @return string
   */
  public function getFragment() { return $this->mFragment; }

  /**
   * Get the fragment in URL form, including the "#" character if there is one
   * @return string
   */
  public function getFragmentForURL() {
    if ( $this->mFragment == '' ) {
      return '';
    }
    else {
      return '#'. Title::escapeFragmentForURL( $this->mFragment );
    }
  }

  /**
   * Set the fragment for this title
   * This is kind of bad, since except for this rarely-used function, Title objects
   * are immutable. The reason this is here is because it's better than setting the
   * members directly, which is what Linker::formatComment was doing previously.
   *
   * @param string $fragment text
   * @todo clarify whether access is supposed to be public (was marked as "kind of public")
   */
  public function setFragment( $fragment ) {
    $this->mFragment = str_replace( '_', ' ', substr( $fragment, 1 ) );
  }

  /**
   * Get the prefixed database key form
   * @return string the prefixed title, with underscores and
   *   any interwiki and namespace prefixes
   */
  public function getPrefixedDBkey() {
    $s = $this->prefix( $this->mDbkeyform );
    $s = str_replace( ' ', '_', $s );
    return strtolower($s);
  }

  /**
   * Get the HTML-escaped displayable text form.
   * Used for the title field in <a> tags.
   * @return string the text, including any prefixes
   */
  public function getEscapedText() {
    return htmlspecialchars( $this->getPrefixedText() );
  }

  /**
   * Get a URL with no fragment or server name.  If this page is generated
   * with action=render, $wgServer is prepended.
   * @param string $query an optional query string; if not specified,
   *   $wgArticlePath will be used.
   * @param string $variant language variant of url (for sr, zh..)
   * @return string the URL
   */
  public function getLocalURL( $query = '', $variant = FALSE ) {
    global $wgArticlePath, $wgScript, $wgServer, $wgRequest;
    global $wgVariantArticlePath, $wgContLang, $wgUser;

    // internal links should point to same variant as current page (only anonymous users)
    if ($variant == FALSE && $wgContLang->hasVariants() && !$wgUser->isLoggedIn()) {
      $pref = $wgContLang->getPreferredVariant(FALSE);
      if ($pref != $wgContLang->getCode())
        $variant = $pref;
    }

    if ( $this->isExternal() ) {
      $url = $this->getFullURL();
      if ( $query ) {
        // This is currently only used for edit section links in the
        // context of interwiki transclusion. In theory we should
        // append the query to the end of any existing query string,
        // but interwiki transclusion is already broken in that case.
        $url .= "?$query";
      }
    }
    else {
      $dbkey = wfUrlencode( $this->getPrefixedDBkey() );
      if ( $query == '' ) {
        if ($variant!=FALSE && $wgContLang->hasVariants()) {
          if ($wgVariantArticlePath==FALSE) {
            $variantArticlePath =  "$wgScript?title=$1&variant=$2"; // default
          }
          else {
            $variantArticlePath = $wgVariantArticlePath;
          }
          $url = str_replace( '$2', urlencode( $variant ), $variantArticlePath );
          $url = str_replace( '$1', $dbkey, $url  );
        }
        else {
          $url = str_replace( '$1', $dbkey, $wgArticlePath );
        }
      }
      else {
        global $wgActionPaths;
        $url = FALSE;
        $matches = array();
        if ( !empty( $wgActionPaths ) &&
          preg_match( '/^(.*&|)action=([^&]*)(&(.*)|)$/', $query, $matches ) ) {
          $action = urldecode( $matches[2] );
          if ( isset( $wgActionPaths[$action] ) ) {
            $query = $matches[1];
            if ( isset( $matches[4] ) ) $query .= $matches[4];
            $url = str_replace( '$1', $dbkey, $wgActionPaths[$action] );
            if ( $query != '' ) $url .= '?'. $query;
          }
        }
        if ( $url === FALSE ) {
          if ( $query == '-' ) {
            $query = '';
          }
          $url = "{$wgScript}?title={$dbkey}&{$query}";
        }
      }

      // FIXME: this causes breakage in various places when we
      // actually expected a local URL and end up with dupe prefixes.
      // TODO: For Drupal we always render, no matter what
//      if ($wgRequest->getVal('action') == 'render') {
        $url = $wgServer . $url;
//      }
    }
    wfRunHooks( 'GetLocalURL', array( &$this, &$url, $query ) );
    return $url;
  }

  /**
   * Get a real URL referring to this title, with interwiki link and
   * fragment
   *
   * @param string $query an optional query string, not used
   *   for interwiki links
   * @param string $variant language variant of url (for sr, zh..)
   * @return string the URL
   */
  public function getFullURL( $query = '', $variant = FALSE ) {
    global $wgContLang, $wgServer, $wgRequest;

    if ( '' == $this->mInterwiki ) {
      $url = $this->getLocalUrl( $query, $variant );

      // Ugly quick hack to avoid duplicate prefixes (bug 4571 etc)
      // Correct fix would be to move the prepending elsewhere.
/*      if ($wgRequest->getVal('action') != 'render') {
        $url = $wgServer . $url;
      }*/
    }
    else {
      $baseUrl = $this->getInterwikiLink( $this->mInterwiki );

      $namespace = wfUrlencode( $this->getNsText() );
      if ( '' != $namespace ) {
        // Can this actually happen? Interwikis shouldn't be parsed.
        // Yes! It can in interwiki transclusion. But... it probably shouldn't.
        $namespace .= ':';
      }
      $url = str_replace( '$1', $namespace . $this->mUrlform, $baseUrl );
      $url = wfAppendQuery( $url, $query );
    }

    // Finally, add the fragment.
    $url .= $this->getFragmentForURL();

    wfRunHooks( 'GetFullURL', array( &$this, &$url, $query ) );
    return $url;
  }

  public function getPrefixedURL() {
    $s = $this->prefix( $this->mDbkeyform );
    $s = str_replace( ' ', '_', $s );

    $s = wfUrlencode( $s ) ;

    // Cleaning up URL to make it look nice -- is this safe?
    $s = str_replace( '%28', '(', $s );
    $s = str_replace( '%29', ')', $s );

    return $s;
  }


  /**
   * Is this Title interwiki?
   * @return boolean
   */
  public function isExternal() { return ( '' != $this->mInterwiki ); }

  /**
   * Do we know that this title definitely exists, or should we otherwise
   * consider that it exists?
   *
   * @return bool
   */
  public function isAlwaysKnown() {
    return $this->isExternal()
            || ( $this->mNamespace == NS_MAIN && $this->mDbkeyform == '' )
            || ( $this->mNamespace == NS_MEDIAWIKI && wfMsgWeirdKey( $this->mDbkeyform ) );
  }

  /**
   * Returns the URL associated with an interwiki prefix
   * @param string $key the interwiki prefix (e.g. "MeatBall")
   * @return the associated URL, containing "$1", which should be
   *   replaced by an article title
   * @static (arguably)
   */
  public function getInterwikiLink( $key )  {
    global $wgMemc, $wgInterwikiExpiry;
    global $wgInterwikiCache, $wgContLang;
    $fname = 'Title::getInterwikiLink';
    $key = $wgContLang->lc( $key );
    // TODO: implement interwiki links
    return mediawiki_filter_get_interwiki_url($key);

    $k = wfMemcKey( 'interwiki', $key );
    if ( array_key_exists( $k, Title::$interwikiCache ) ) {
      return Title::$interwikiCache[$k]->iw_url;
    }

    if ($wgInterwikiCache) {
      return Title::getInterwikiCached( $key );
    }

    $s = $wgMemc->get( $k );
    // Ignore old keys with no iw_local
    if ( $s && isset( $s->iw_local ) && isset($s->iw_trans)) {
      Title::$interwikiCache[$k] = $s;
      return $s->iw_url;
    }

    $dbr = wfGetDB( DB_SLAVE );
    $res = $dbr->select( 'interwiki',
      array( 'iw_url', 'iw_local', 'iw_trans' ),
      array( 'iw_prefix' => $key ), $fname );
    if ( !$res ) {
      return '';
    }

    $s = $dbr->fetchObject( $res );
    if ( !$s ) {
      // Cache non-existence: create a blank object and save it to memcached
      $s = (object)FALSE;
      $s->iw_url = '';
      $s->iw_local = 0;
      $s->iw_trans = 0;
    }
    $wgMemc->set( $k, $s, $wgInterwikiExpiry );
    Title::$interwikiCache[$k] = $s;

    return $s->iw_url;
  }

  /**
   * Determine whether the object refers to a page within
   * this project and is transcludable.
   *
   * @return \type{\bool} TRUE if this is transcludable
   */
  public function isTrans() {
    // not implemented
    return true;
  }

  /**
   * Escape a text fragment, say from a link, for a URL
   */
  static function escapeFragmentForURL( $fragment ) {
    $fragment = str_replace( ' ', '_', $fragment );
    $fragment = urlencode( Sanitizer::decodeCharReferences( $fragment ) );
    $replaceArray = array(
      '%3A' => ':',
      '%' => '.'
    );
    return strtr( $fragment, $replaceArray );
  }

  /**
   * Does this title refer to a page that can (or might) be meaningfully
   * viewed?  In particular, this function may be used to determine if
   * links to the title should be rendered as "bluelinks" (as opposed to
   * "redlinks" to non-existent pages).
   *
   * @return \type{\bool} TRUE or FALSE
   */
  public function isKnown() {
    return $this->exists() || $this->isAlwaysKnown();
  }

  /**
   * Check if page exists.  For historical reasons, this function simply
   * checks for the existence of the title in the page table, and will
   * thus return false for interwiki links, special pages and the like.
   * If you want to know if a title can be meaningfully viewed, you should
   * probably call the isKnown() method instead.
   *
   * @return \type{\bool} TRUE or FALSE
   */
  public function exists() {
    return $this->getArticleId() != 0;
  }

  /**
   * Compare with another title.
   *
   * @param \type{Title} $title
   * @return \type{\bool} TRUE or FALSE
   */
  public function equals( Title $title ) {
    // Note: === is necessary for proper matching of number-like titles.
    return $this->getInterwiki() === $title->getInterwiki()
      && $this->getNamespace() == $title->getNamespace()
      && $this->getDBkey() === $title->getDBkey();
  }

  /**
   * Get a URL that's the simplest URL that will be valid to link, locally,
   * to the current Title.  It includes the fragment, but does not include
   * the server unless action=render is used (or the link is external).  If
   * there's a fragment but the prefixed text is empty, we just return a link
   * to the fragment.
   *
   * @param $query \type{\arrayof{\string}} An associative array of key => value pairs for the
   *   query string.  Keys and values will be escaped.
   * @param $variant \type{\string} Language variant of URL (for sr, zh..).  Ignored
   *   for external links.  Default is "false" (same variant as current page,
   *   for anonymous users).
   * @return \type{\string} the URL
   */
  public function getLinkUrl( $query = array(), $variant = false ) {
    wfProfileIn( __METHOD__ );
    if( !is_array( $query ) ) {
      wfProfileOut( __METHOD__ );
      throw new MWException( 'Title::getLinkUrl passed a non-array for '.
      '$query' );
    }
    if( $this->isExternal() ) {
      $ret = $this->getFullURL( $query );
    } elseif( $this->getPrefixedText() === '' && $this->getFragment() !== '' ) {
      $ret = $this->getFragmentForURL();
    } else {
      $ret = $this->getLocalURL( $query, $variant ) . $this->getFragmentForURL();
    }
    wfProfileOut( __METHOD__ );
    return $ret;
  }

  /**
   * Is this an article that is a redirect page?
   * Uses link cache, adding it if necessary
   * @param $flags \type{\int} a bit field; may be GAID_FOR_UPDATE to select for update
   * @return \type{\bool}
   */
  public function isRedirect( $flags = 0 ) {
    // its never a redirect in drupal
    return false;

/*
    if( !is_null($this->mRedirect) )
      return $this->mRedirect;
    # Calling getArticleID() loads the field from cache as needed
    if( !$this->getArticleID($flags) ) {
      return $this->mRedirect = false;
    }
    $linkCache = LinkCache::singleton();
    $this->mRedirect = (bool)$linkCache->getGoodLinkFieldObj( $this, 'redirect' );

    return $this->mRedirect;
*/
  }

  /**
   * Is this Title in a namespace which contains content?
   * In other words, is this a content page, for the purposes of calculating
   * statistics, etc?
   *
   * @return \type{\bool} TRUE or FALSE
   */
  public function isContentPage() {
    return MWNamespace::isContent( $this->getNamespace() );
  }

  /**
   * Could this title have a corresponding talk page?
   * @return \type{\bool} TRUE or FALSE
   */
  public function canTalk() {
    // Talk pages are not supported
    return False;
    // return( MWNamespace::canTalk( $this->mNamespace ) );
  }

  /**
   * Create a new Title from a namespace index and a DB key.
   * The parameters will be checked for validity, which is a bit slower
   * than makeTitle() but safer for user-provided data.
   *
   * @param $ns \type{\int} the namespace of the article
   * @param $title \type{\string} the database key form
   * @param $fragment \type{\string} The link fragment (after the "#")
   * @return \type{Title} the new object, or NULL on an error
   */
  public static function makeTitleSafe( $ns, $title, $fragment = '' ) {
    $t = new Title();
    $t->mDbkeyform = Title::makeName( $ns, $title, $fragment );
    if( $t->secureAndSplit() ) {
      return $t;
    } else {
      return NULL;
    }
  }

  /*
   * Make a prefixed DB key from a DB key and a namespace index
   * @param $ns \type{\int} numerical representation of the namespace
   * @param $title \type{\string} the DB key form the title
   * @param $fragment \type{\string} The link fragment (after the "#")
   * @return \type{\string} the prefixed form of the title
   */
  public static function makeName( $ns, $title, $fragment = '' ) {
    global $wgContLang;

    $namespace = $wgContLang->getNsText( $ns );
    $name = $namespace == '' ? $title : "$namespace:$title";
    if ( strval( $fragment ) != '' ) {
      $name .= '#' . $fragment;
    }
    return $name;
  }
}
