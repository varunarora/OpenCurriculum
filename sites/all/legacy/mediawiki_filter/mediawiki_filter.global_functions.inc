<?php
// $ Id: $

/**
 * @file
 * Global functions needed for the Mediawiki parser.
 * Either just copied from original files, or adapted for
 * use in Drupal.
 *
 * NOTE:
 * Some of Drupal's Coding Guidelines had to be broken (CamelCase usage and
 * global-variable's names), because of the external dependencies on Mediawiki,
 * and thus it's naming convention.
 */

/**
 * ORIGINAL: ImageFunctions.php
 */
function wfIsBadImage( $name, $contextTitle = FALSE ) {
  // There is no black-list for inlining images available.
  return FALSE;
}

/**
 * ORIGINAL: ImageFunctions.php
 *
 * Calculate the largest thumbnail width for a given original file size
 * such that the thumbnail's height is at most $maxHeight.
 * @param $boxWidth Integer Width of the thumbnail box.
 * @param $boxHeight Integer Height of the thumbnail box.
 * @param $maxHeight Integer Maximum height expected for the thumbnail.
 * @return Integer.
 */
function wfFitBoxWidth( $boxWidth, $boxHeight, $maxHeight ) {
  $idealWidth = $boxWidth * $maxHeight / $boxHeight;
  $roundedUp = ceil( $idealWidth );
  if ( round( $roundedUp * $boxHeight / $boxWidth ) > $maxHeight )
    return floor( $idealWidth );
  else
    return $roundedUp;
}

/**
 * ORIGINAL ProfilerStub.php
 */
function wfProfileIn( $fn = '' ) {
  global $hackwhere;
  printLine($fn);
}

/**
 * ORIGINAL ProfilerStub.php
 */
function wfProfileOut( $fn = '' ) {
  global $hackwhere, $wgDBname, $haveProctitle;
  if (count($hackwhere)) {
          array_pop($hackwhere);
  }
  if (count($hackwhere)) {
    printLine($hackwhere[count($hackwhere)-1]);
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfUrlencode( $s ) {
  $s = urlencode( $s );
  $s = preg_replace( '/%3[Aa]/', ':', $s );
  $s = preg_replace( '/%2[Ff]/', '/', $s );

  return $s;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfDebug( $text, $logonly = FALSE ) {
  // Ignore debug messages (or maybe send them to watchdog?)
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfDebugLog( $logGroup, $text, $public = TRUE ) {
  // Ignore debug messages (or maybe send them to watchdog?)
}

/**
 * Throws an E_USER_NOTICE saying that $function is deprecated
 * @param string $function
 * @return null
 */
function wfDeprecated( $function ) {
  global $wgDebugLogFile;
  if ( !$wgDebugLogFile ) {
    return;
  }
  $callers = wfDebugBacktrace();
  if( isset( $callers[2] ) ){
    $callerfunc = $callers[2];
    $callerfile = $callers[1];
    if( isset( $callerfile['file'] ) && isset( $callerfile['line'] ) ){
      $file = $callerfile['file'] . ' at line ' . $callerfile['line'];
    } else {
      $file = '(internal function)';
    }
    $func = '';
    if( isset( $callerfunc['class'] ) )
      $func .= $callerfunc['class'] . '::';
    $func .= @$callerfunc['function'];
    $msg = "Use of $function is deprecated. Called from $func in $file";
  } else {
    $msg = "Use of $function is deprecated.";
  }
  wfDebug( "$msg\n" );
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsg( $key ) {
  $args = func_get_args();
  array_shift( $args );
  return wfMsgReal( $key, $args, TRUE );
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsgForContent( $key ) {
  global $wgForceUIMsgAsContentMsg;
  $args = func_get_args();
  array_shift( $args );
  $forcontent = TRUE;
  if ( is_array( $wgForceUIMsgAsContentMsg ) &&
      in_array( $key, $wgForceUIMsgAsContentMsg ) )
      $forcontent = FALSE;
  return wfMsgReal( $key, $args, TRUE, $forcontent );
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsgForContentNoTrans( $key ) {
  global $wgForceUIMsgAsContentMsg;
  $args = func_get_args();
  array_shift( $args );
  $forcontent = TRUE;
  if ( is_array( $wgForceUIMsgAsContentMsg ) &&
      in_array( $key, $wgForceUIMsgAsContentMsg ) )
      $forcontent = FALSE;
  return wfMsgReal( $key, $args, TRUE, $forcontent, FALSE );
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsgReal( $key, $args, $useDB = TRUE, $forContent=FALSE, $transform = TRUE ) {
  $fname = 'wfMsgReal';
  wfProfileIn( $fname );
  $message = wfMsgGetKey( $key, $useDB, $forContent, $transform );
  $message = wfMsgReplaceArgs( $message, $args );
  wfProfileOut( $fname );
  return $message;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsgWeirdKey( $key ) {
  $source = wfMsgGetKey( $key, FALSE, TRUE, FALSE );
  if ( wfEmptyMsg( $key, $source ) )
    return "";
  else
    return $source;
}

/**
 * Return a Language object from $langcode
 * @param $langcode Mixed: either:
 *                  - a Language object
 *                  - code of the language to get the message for, if it is
 *                    a valid code create a language for that language, if
 *                    it is a string but not a valid code then make a basic
 *                    language object
 *                  - a boolean: if it's false then use the current users
 *                    language (as a fallback for the old parameter
 *                    functionality), or if it is true then use the wikis
 * @return Language object
 */
function wfGetLangObj( $langcode = false ){
  # Identify which language to get or create a language object for.
  if( $langcode instanceof Language )
    # Great, we already have the object!
    return $langcode;

  global $wgContLang;
  if( $langcode === $wgContLang->getCode() || $langcode === true )
    # $langcode is the language code of the wikis content language object.
    # or it is a boolean and value is true
    return $wgContLang;

  global $wgLang;
  if( $langcode === $wgLang->getCode() || $langcode === false )
    # $langcode is the language code of user language object.
    # or it was a boolean and value is false
    return $wgLang;

  $validCodes = array_keys( Language::getLanguageNames() );
  if( in_array( $langcode, $validCodes ) )
    # $langcode corresponds to a valid language.
    return Language::factory( $langcode );

  # $langcode is a string, but not a valid language code; use content language.
  wfDebug( "Invalid language code passed to wfGetLangObj, falling back to content language.\n" );
  return $wgContLang;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsgGetKey( $key, $useDB, $forContent = FALSE, $transform = TRUE ) {
  global $wgParser, $wgContLang, $wgMessageCache, $wgLang;

  if ( is_object( $wgMessageCache ) )
    $transstat = $wgMessageCache->getTransform();

  if ( is_object( $wgMessageCache ) ) {
    if ( ! $transform )
      $wgMessageCache->disableTransform();
    $message = $wgMessageCache->get( $key, $useDB, $forContent );
  }
  else {
    if ( $forContent ) {
      $lang = &$wgContLang;
    }
    else {
      $lang = &$wgLang;
    }

    # MessageCache::get() does this already, Language::getMessage() doesn't
    # ISSUE: Should we try to handle "message/lang" here too?
    $key = str_replace( ' ' , '_' , $wgContLang->lcfirst( $key ) );

    wfSuppressWarnings();
    if ( is_object( $lang ) ) {
      $message = $lang->getMessage( $key );
    }
    else {
      $message = FALSE;
    }
    wfRestoreWarnings();

    if ( $transform && strstr( $message, '{{' ) !== FALSE ) {
      $message = $wgParser->transformMsg($message, $wgMessageCache->getParserOptions() );
    }
  }

  if ( is_object( $wgMessageCache ) && ! $transform )
    $wgMessageCache->setTransform( $transstat );

  return $message;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfEmptyMsg( $msg, $wfMsgOut ) {
  return $wfMsgOut === htmlspecialchars( "<$msg>" );
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsgHtml( $key ) {
  $args = func_get_args();
  array_shift( $args );
  return wfMsgReplaceArgs( htmlspecialchars( wfMsgGetKey( $key, TRUE ) ), $args );
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsgExt( $key, $options ) {
  global $wgOut, $wgParser;

  $args = func_get_args();
  array_shift( $args );
  array_shift( $args );

  if ( !is_array($options) ) {
    $options = array($options);
  }

  $string = wfMsgGetKey( $key, TRUE, FALSE, FALSE );

  if ( !in_array('replaceafter', $options) ) {
    $string = wfMsgReplaceArgs( $string, $args );
  }

  if ( in_array('parse', $options) ) {
    $string = $wgOut->parse( $string, TRUE, TRUE );
  }
  elseif ( in_array('parseinline', $options) ) {
    $string = $wgOut->parse( $string, TRUE, TRUE );
    $m = array();
    if ( preg_match( '/^<p>(.*)\n?<\/p>$/sU', $string, $m ) ) {
      $string = $m[1];
    }
  }
  elseif ( in_array('parsemag', $options) ) {
    global $wgMessageCache;
    if ( isset( $wgMessageCache ) ) {
      $string = $wgMessageCache->transform( $string );
    }
  }

  if ( in_array('escape', $options) ) {
    $string = htmlspecialchars ( $string );
  }

  if ( in_array('replaceafter', $options) ) {
    $string = wfMsgReplaceArgs( $string, $args );
  }

  return $string;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMsgReplaceArgs( $message, $args ) {
  # Fix windows line-endings
  # Some messages are split with explode("\n", $msg)
  $message = str_replace( "\r", '', $message );

  // Replace arguments
  if ( count( $args ) ) {
    if ( is_array( $args[0] ) ) {
      foreach ( $args[0] as $key => $val ) {
        $message = str_replace( '$' . $key, $val, $message );
      }
    }
    else {
      foreach ( $args as $n => $param ) {
        $replacementKeys['$' . ($n + 1)] = $param;
      }
      $message = strtr( $message, $replacementKeys );
    }
  }

  return $message;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfDebugDieBacktrace( $msg = '' ) {
  // Do nothing
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfEscapeWikiText( $text ) {
  $text = str_replace(
    array( '[',     '|',      '\'',    'ISBN ',     'RFC ',     '://',     "\n=",     '{{' ),
    array( '&#91;', '&#124;', '&#39;', 'ISBN&#32;', 'RFC&#32;', '&#58;//', "\n&#61;", '&#123;&#123;' ),
    htmlspecialchars($text) );
  return $text;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfSetVar( &$dest, $source ) {
  $temp = $dest;
  if ( !is_null( $source ) ) {
    $dest = $source;
  }
  return $temp;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfSuppressWarnings( $end = FALSE ) {
  static $suppressCount = 0;
  static $originalLevel = FALSE;

  if ( $end ) {
    if ( $suppressCount ) {
      --$suppressCount;
      if ( !$suppressCount ) {
        error_reporting( $originalLevel );
      }
    }
  }
  else {
    if ( !$suppressCount ) {
      $originalLevel = error_reporting( E_ALL & ~( E_WARNING | E_NOTICE ) );
    }
    ++$suppressCount;
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfRestoreWarnings() {
  wfSuppressWarnings( TRUE );
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfUrlProtocols() {
  global $wgUrlProtocols;

  // Support old-style $wgUrlProtocols strings, for backwards compatibility
  // with LocalSettings files from 1.5
  if ( is_array( $wgUrlProtocols ) ) {
    $protocols = array();
    foreach ($wgUrlProtocols as $protocol)
      $protocols[] = preg_quote( $protocol, '/' );

    return implode( '|', $protocols );
  }
  else {
    return $wgUrlProtocols;
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function in_string( $needle, $str ) {
  return strpos( $str, $needle ) !== FALSE;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
if ( !function_exists('iconv') ) {
  function iconv( $from, $to, $string ) {
    if (strcasecmp( $from, $to ) == 0) return $string;
    if (strcasecmp( $from, 'utf-8' ) == 0) return utf8_decode( $string );
    if (strcasecmp( $to, 'utf-8' ) == 0) return utf8_encode( $string );
    return $string;
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
if ( !function_exists( 'mb_substr' ) ) {
  function mb_substr( $str, $start ) {
    $ar = array();
    preg_match_all( '/./us', $str, $ar );

    if ( func_num_args() >= 3 ) {
      $end = func_get_arg( 2 );
      return join( '', array_slice( $ar[0], $start, $end ) );
    }
    else {
      return join( '', array_slice( $ar[0], $start ) );
    }
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
if ( !function_exists( 'mb_strlen' ) ) {
  function mb_strlen( $str, $enc="" ) {
    $counts = count_chars( $str );
    $total = 0;

    // Count ASCII bytes
    for ( $i = 0; $i < 0x80; $i++ ) {
      $total += $counts[$i];
    }

    // Count multibyte sequence heads
    for ( $i = 0xc0; $i < 0xff; $i++ ) {
      $total += $counts[$i];
    }
    return $total;
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfGetCaller( $level = 2 ) {
  // Not implemented
  return 'unknown';
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfGetPrecompiledData( $name ) {
  return FALSE;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function &wfGetDB( $db = DB_LAST, $groups = array() ) {
  $db = new Database;
  return $db;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfFindFile( $title, $time = FALSE ) {
  if( !( $title instanceof Title ) ) {
    $title = Title::makeTitleSafe( NS_FILE, $title );
  }
  if( !$title ) {
    return false;  // invalid title?
  }

  $image = new DrupalImageFile($title);
  if ($image->exists()) {
    return $image;
  }
  return FALSE;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfGetNull() {
  return wfIsWindows() ? 'NUL' : '/dev/null';
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfTimestampNow() {
  # return NOW
  return wfTimestamp( TS_MW, time() );
}
define('TS_UNIX', 0);
define('TS_MW', 1);
define('TS_DB', 2);
define('TS_RFC2822', 3);
define('TS_ISO_8601', 4);
define('TS_EXIF', 5);
define('TS_ORACLE', 6);
define('TS_POSTGRES', 7);
function wfTimestamp($outputtype=TS_UNIX,$ts=0) {
  $uts = 0;
  $da = array();
  if ($ts==0) {
    $uts=time();
  }
  elseif (preg_match('/^(\d{4})\-(\d\d)\-(\d\d) (\d\d):(\d\d):(\d\d)$/D', $ts, $da)) {
    # TS_DB
    $uts=gmmktime((int)$da[4], (int)$da[5], (int)$da[6],
          (int)$da[2], (int)$da[3], (int)$da[1]);
  }
  elseif (preg_match('/^(\d{4}):(\d\d):(\d\d) (\d\d):(\d\d):(\d\d)$/D', $ts, $da)) {
    # TS_EXIF
    $uts=gmmktime((int)$da[4], (int)$da[5], (int)$da[6],
      (int)$da[2], (int)$da[3], (int)$da[1]);
  }
  elseif (preg_match('/^(\d{4})(\d\d)(\d\d)(\d\d)(\d\d)(\d\d)$/D', $ts, $da)) {
    # TS_MW
    $uts=gmmktime((int)$da[4], (int)$da[5], (int)$da[6],
          (int)$da[2], (int)$da[3], (int)$da[1]);
  }
  elseif (preg_match('/^(\d{1,13})$/D', $ts, $da)) {
    # TS_UNIX
    $uts = $ts;
  }
  elseif (preg_match('/^(\d{1,2})-(...)-(\d\d(\d\d)?) (\d\d)\.(\d\d)\.(\d\d)/', $ts, $da)) {
    # TS_ORACLE
    $uts = strtotime(preg_replace('/(\d\d)\.(\d\d)\.(\d\d)(\.(\d+))?/', "$1:$2:$3",
        str_replace("+00:00", "UTC", $ts)));
  }
  elseif (preg_match('/^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})Z$/', $ts, $da)) {
    # TS_ISO_8601
    $uts=gmmktime((int)$da[4], (int)$da[5], (int)$da[6],
      (int)$da[2], (int)$da[3], (int)$da[1]);
  }
  elseif (preg_match('/^(\d{4})\-(\d\d)\-(\d\d) (\d\d):(\d\d):(\d\d)[\+\- ](\d\d)$/', $ts, $da)) {
    # TS_POSTGRES
    $uts=gmmktime((int)$da[4], (int)$da[5], (int)$da[6],
    (int)$da[2], (int)$da[3], (int)$da[1]);
  }
  elseif (preg_match('/^(\d{4})\-(\d\d)\-(\d\d) (\d\d):(\d\d):(\d\d) GMT$/', $ts, $da)) {
    # TS_POSTGRES
    $uts=gmmktime((int)$da[4], (int)$da[5], (int)$da[6],
    (int)$da[2], (int)$da[3], (int)$da[1]);
  }
  else {
    // Bogus value; fall back to the epoch...
    wfDebug("wfTimestamp() fed bogus time value: $outputtype; $ts\n");
    $uts = 0;
  }


   switch ($outputtype) {
    case TS_UNIX:
      return $uts;
    case TS_MW:
      return gmdate( 'YmdHis', $uts );
    case TS_DB:
      return gmdate( 'Y-m-d H:i:s', $uts );
    case TS_ISO_8601:
      return gmdate( 'Y-m-d\TH:i:s\Z', $uts );
    // This shouldn't ever be used, but is included for completeness
    case TS_EXIF:
      return gmdate(  'Y:m:d H:i:s', $uts );
    case TS_RFC2822:
      return gmdate( 'D, d M Y H:i:s', $uts ) .' GMT';
    case TS_ORACLE:
      return gmdate( 'd-M-y h.i.s A', $uts) .' +00:00';
    case TS_POSTGRES:
      return gmdate( 'Y-m-d H:i:s', $uts) .' GMT';
    default:
      throw new MWException( 'wfTimestamp() called with illegal output type.');
  }
}
function wfTimestampOrNull( $outputtype = TS_UNIX, $ts = NULL ) {
  if ( is_null( $ts ) ) {
    return NULL;
  }
  else {
    return wfTimestamp( $outputtype, $ts );
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfIsWindows() {
  if (substr(php_uname(), 0, 7) == 'Windows') {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfArrayToCGI( $array1, $array2 = NULL ) {
  if ( !is_null( $array2 ) ) {
    $array1 = $array1 + $array2;
  }

  $cgi = '';
  foreach ( $array1 as $key => $value ) {
    if ( '' !== $value ) {
      if ( '' != $cgi ) {
        $cgi .= '&';
      }
      $cgi .= urlencode( $key ) .'='. urlencode( $value );
    }
  }
  return $cgi;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfAppendQuery( $url, $query ) {
  if (is_array($query) && (count($query) == 0)) {
    return $url;
  }
  if ( $query != '' ) {
    if ( FALSE === strpos( $url, '?' ) ) {
      $url .= '?';
    }
    else {
      $url .= '&';
    }
    $url .= $query;
  }
  return $url;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfCreateObject( $name, $p ){
  $p = array_values( $p );
  switch ( count( $p ) ) {
    case 0:
      return new $name;
    case 1:
      return new $name( $p[0] );
    case 2:
      return new $name( $p[0], $p[1] );
    case 3:
      return new $name( $p[0], $p[1], $p[2] );
    case 4:
      return new $name( $p[0], $p[1], $p[2], $p[3] );
    case 5:
      return new $name( $p[0], $p[1], $p[2], $p[3], $p[4] );
    case 6:
      return new $name( $p[0], $p[1], $p[2], $p[3], $p[4], $p[5] );
    default:
      throw new MWException( "Too many arguments to construtor in wfCreateObject" );
  }
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfMemcKey( /*... */ ) {
  global $wgDBprefix, $wgDBname;
  $args = func_get_args();
  if ( $wgDBprefix ) {
    $key = "$wgDBname-$wgDBprefix:" . implode( ':', $args );
  }
  else {
    $key = $wgDBname .':'. implode( ':', $args );
  }
  return $key;
}

/**
 * ORIGINAL GlobalFunctions.php
 */
function wfLoadExtensionMessages( $extensionName ) {
    global $wgExtensionMessagesFiles, $wgMessageCache;
    if ( !empty( $wgExtensionMessagesFiles[$extensionName] ) ) {
        $wgMessageCache->loadMessagesFile( $wgExtensionMessagesFiles[$extensionName] );
        // Prevent double-loading
        $wgExtensionMessagesFiles[$extensionName] = FALSE;
    }
}
