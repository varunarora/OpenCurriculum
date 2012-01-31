<?php
  // $Id: mediawiki_filter.lib.inc 3849 2011-01-13 08:36:55Z bherlig $

/**
 * @file
 * These functions and classes provide an environment for the Mediawiki parser.
 *
 * The mapping is the following:
 *
 * Mediawiki       | Drupal
 * ------------------------------
 * Article/Page ID | Node ID
 * Revision ID     | Revision ID       / not really needed, we always take the latest revision of templates/images
 * Article title   | Node title
 * Namespace       | Node type
 * NS_MAIN         | type = 'wiki'
 * NS_TEMPLATE     | type = 'template'
 * NS_IMAGE        | type = 'image'
 * NS_USER         | ?
 * NS_PROJECT      | ?
 * User ID         | User ID
 * User Name       | User Name
 *
 * No talk pages are available, comments are used instead.
 * No special pages are available.
 *
 * NOTE:
 * Some of Drupal's Coding Guidelines had to be broken (CamelCase usage and
 * global-variable's names), because of the external dependencies on Mediawiki,
 * and thus it's naming convention.
 *
 * TODO:
 * The mapping of namespace to node type is too restricting. There should be a way to map namespaces to a category,
 * then the titles inside this category should be unique.
 */

/**
 * Parse $text with mediawiki parser.
 */
function _mediawiki_filter_parse($text) {
  global $wgParser, $wgUser;

  _mediawiki_filter_initialize_environment();

  $title = new Title('Dummy');
  $options = new ParserOptions();
  // Make signature replacement (~~~~)
//  $text = $wgParser->preSaveTransform($text, $title, $wgUser, $options);
  // Do rendering
  $output = $wgParser->parse($text, $title, $options);

  return $output->getText();
}


function _mediawiki_filter_initialize_environment() {
  static $is_initialized = FALSE;
  if ($is_initialized) {
    return;
  }
  $is_initialized = TRUE;

  // Needed to include Mediawiki files.
  define('MEDIAWIKI', 1);

  // Set up Mediawiki's fake root path.
  global $IP;
  $IP = realpath(drupal_get_path('module', 'mediawiki_filter') .'/lib');
  $overrides = realpath(drupal_get_path('module', 'mediawiki_filter') .'/overrides');

  // Set up some global variables needed for Mediawiki.
  $settings = array(
    'wgServer' => url(NULL, array('absolute' => TRUE)),
    'wgScriptPath' => '/xyz',
    'wgScript' => '/xyz/index.php',
    'wgRedirectScript' => '/xyz/redirect.php',
    'wgArticlePath' => mediawiki_filter_wgArticlePath(),
    'wgActionPaths' => array(),
    'wgContLanguageCode' => 'en',
    'wgRawHtml' => FALSE, // Should be disabled for security
    'wgNamespacesWithSubpages' => array(),  // ?
    'wgCapitalLinks' => TRUE,
    'wgUseTeX' => FALSE,
    'wgLocaltimezone' => 'UTC',
    'wgAllowExternalImages' => TRUE,
    'wgUseTidy' => FALSE,
    'wgVariantArticlePath' => FALSE,
    'wgSitename' => variable_get('site_name', 'Drupal'),  // Needed for {{SITENAME}} variable
    'wgMaxArticleSize' => 2048,                 // Needed that templates get loaded
    'wgSpecialPages' => array(),                // Needed that no PHP-errors occur
    'wgMaxTocLevel' => 999,                     // Needed that TOC is generated
    'wgUrlProtocols' => array(
      'http://',
      'https://',
      'ftp://',
      'irc://',
      'gopher://',
      'telnet://', // Well if we're going to support the above.. -ævar
      'nntp://', // @bug 3808 RFC 1738
      'worldwind://',
      'mailto:',
      'news:'
    ),
    'wgLegalTitleChars' => " %!\"$&'(){}*,\\-.\\/0-9:;=?@A-Z\\\\^_`a-z~\\x80-\\xFF+",
    'wgHooks' => array(),
    'wgExtensionMessagesFiles' => array(),  // Needed for extensions (e.g. Cite)
    'wgExtensionFunctions' => array(),  // Needed for extensions (e.g. Cite)

    'wgMaxPPNodeCount' => 1000000,  # A complexity limit on template expansion

    /**
     * Maximum recursion depth for templates within templates.
     * The current parser adds two levels to the PHP call stack for each template,
     * and xdebug limits the call stack to 100 by default. So this should hopefully
     * stop the parser before it hits the xdebug limit.
     */
    'wgMaxTemplateDepth' => 40,
    'wgMaxPPExpandDepth' => 40,
    'wgExtensionAliasesFiles' => array(),
  );
  foreach ($settings as $var => $val) {
    $GLOBALS[$var] = $val;
  }

  // Faked (and some copied) global functions.
  require_once("mediawiki_filter.global_functions.inc");

  // Include Required Mediawiki files.
  require_once("$IP/includes/Defines.php");
  require_once("$IP/includes/Hooks.php");
  require_once("$IP/includes/ImageGallery.php");
  require_once("$IP/includes/MagicWord.php");
  require_once("$IP/includes/MessageCache.php");
  require_once($overrides .'/Namespace.php');
//  require_once("$IP/includes/Namespace.php"); // CHECK REMOVAL
  $GLOBALS['wgCanonicalNamespaceNames'] = $wgCanonicalNamespaceNames;
  require_once("$IP/includes/ObjectCache.php");                       // CHECK REMOVAL
  require_once("$IP/includes/StringUtils.php");
  require_once("$IP/includes/Sanitizer.php");
  require_once("$IP/includes/SpecialPage.php");
  require_once("$IP/includes/Xml.php");
  require_once("$IP/includes/XmlFunctions.php");
  require_once("$IP/includes/normal/UtfNormal.php");
  require_once("$IP/languages/Language.php");

      # includes/parser
  require_once("$IP/includes/parser/CoreLinkFunctions.php");
  require_once("$IP/includes/parser/CoreParserFunctions.php");
  require_once("$IP/includes/parser/DateFormatter.php");
  require_once("$IP/includes/parser/LinkHolderArray.php");
  require_once("$IP/includes/parser/Parser.php");
  require_once("$IP/includes/parser/Parser_LinkHooks.php");
  require_once("$IP/includes/parser/Preprocessor.php");
  require_once("$IP/includes/parser/Preprocessor_DOM.php");
  require_once("$IP/includes/parser/Preprocessor_Hash.php");
  require_once("$IP/includes/parser/ParserCache.php");
  require_once("$IP/includes/parser/ParserOptions.php");
  require_once("$IP/includes/parser/ParserOutput.php");
  require_once("$IP/includes/parser/Parser_DiffTest.php");
  require_once("$IP/includes/parser/Parser.php");
  require_once("$IP/includes/parser/Tidy.php");


  require_once($overrides .'/Linker.php');
  require_once($overrides .'/Title.php');
  $GLOBALS['wgLanguageNames'] = $wgLanguageNames;

  // Faked classes.
  require_once("mediawiki_filter.classes.inc");

  global $wgMemc;
  $wgMemc = new FakeMemCachedClient;

  global $wgParser;
  $wgParser = new MediawikiFilterParser();

  global $wgUser;
  $wgUser = new User();

  global $wgContLang, $wgLang;
  $wgContLang = Language::factory( 'en' );
  $wgLang = $wgContLang;

  global $wgMessageCache;
  $temp = NULL;
  $wgMessageCache = new MessageCache($temp, FALSE, 0, NULL);

  // Load extensions
  global $wgHooks;
  $wgHooks['ParserAfterTidy'][] = 'ignoreCodeTagAfterTidy';
  $wgHooks['ParserBeforeStrip'][] = 'stripCodeTagBeforeStrip';
  $wgParser->setHook('code', 'ignoreCodeTag');

// TODO: doesn't work in the stripped version of Mediawiki...
//   global $wgExtensionFunctions;
//   require_once("$IP/extensions/Cite/Cite.php");
//   foreach ( $wgExtensionFunctions as $func ) {
//     $profName = $fname.'-extensions-'.strval( $func );
//     wfProfileIn( $profName );
//     call_user_func( $func );
//     wfProfileOut( $profName );
//   }

}

function stripCodeTagBeforeStrip(&$parser, &$text, &$strip_state) {
  global $_tagMarkers;
  $matches = array();
  preg_match_all('#\\[geshifilter-code([^\\]]*)\\](.*?)(\\[/geshifilter-code\\])#s', $text, $matches);
  foreach($matches[0] as $match)
  {
    $markercount =  count($_tagMarkers);
    $marker = "xx-tagmarker". $markercount ."-xx";
    $_tagMarkers[$markercount] = $match;
    $text = str_replace($match, $marker, $text);
  }
  return TRUE;
}

/**
 * TODO: this is hard-coded at the moment and shoul be more customizable
 */
function ignoreCodeTag($text, $args) {
  global $_tagMarkers;
  $arguments = '';
  foreach ($args as $name => $value) {
    $arguments .= ' '. $name .'="'. $value .'"';
  }
  $output = "<code$arguments>$text</code>";
  $markercount =  count($_tagMarkers);
  $marker = "xx-tagmarker". $markercount ."-xx";
  $_tagMarkers[$markercount] = $output;
  return $marker;
}

function ignoreCodeTagAfterTidy(&$parser, &$text) {
  global $_tagMarkers;
  for ($i=0;$i<count($_tagMarkers);$i++)
    $text = preg_replace('/xx-tagmarker'. $i .'-xx/', $_tagMarkers[$i], $text);
  return TRUE;
}

/** Debug function */
function printLine($line) {
//  print("$line\t\t<br/>\n");
}

/**
 * Abstraction of an 'image' node of the image module.
 */
class DrupalImageFile {

  var $node;

  function __construct($title) {
    $type = mediawiki_filter_namespace_image_node_type();
    $result = db_query("SELECT nid,vid FROM {node} WHERE type = '%s' AND LOWER(title) = '%s'", $type, strtolower($title->getText()));
    if ($row = db_fetch_object($result)) {
      if (db_fetch_object($result)) {
        // TODO: do some more elaborate stuff
        drupal_set_message(t('Found mutliple images with name %title. Using <a href="!url">first match</a> for current page.', array('%title' => $title, '!url' => url('node/'. $row->nid))));
      }
      $this->node = node_load($row->nid);
      $this->info = image_get_info(file_create_path($this->node->images[IMAGE_ORIGINAL]));
    }
    else {
      // Second try: lookup using unmodified form (fixes Oriact issue #2)
      $result = db_query("SELECT nid,vid FROM {node} WHERE type = '%s' AND LOWER(title) = '%s'", $type, strtolower($title->getUnmodifiedForm()));
      if ($row = db_fetch_object($result)) {
        if (db_fetch_object($result)) {
          // TODO: do some more elaborate stuff
          drupal_set_message(t('Found mutliple images with name %title. Using <a href="!url">first match</a> for current page.', array('%title' => $title, '!url' => url('node/'. $row->nid))));
        }
        $this->node = node_load($row->nid);
        $this->info = image_get_info(file_create_path($this->node->images[IMAGE_ORIGINAL]));
      }
      else {
        // try to find it using URL form
        $this->node = NULL;
      }
    }
  }

  function exists() {
    return !is_null($this->node);
  }

  function getTitle() {
    return $node->title;
  }

  function getHandler() {
    return new DrupalImageFileHandler;
  }

  function allowInlineDisplay() {
    return TRUE;
  }

  function getWidth() {
    return $this->info['width'];
  }

  function getHeight() {
    return $this->info['height'];
  }

  function mustRender() {
    return FALSE;
  }

  function getImageURL() {
    return $url = url('node/'. $this->node->nid);
  }

  /**
   * Transform a media file
   *
   * @param array $params An associative array of handler-specific parameters. Typical
   *                      keys are width, height and page.
   * @param integer $flags A bitfield, may contain self::RENDER_NOW to force rendering
   * @return MediaTransformOutput
   */
  function transform( $params, $flags = 0 ) {
    if (!$this->exists()) {
      return FALSE;
    }

    if (isset($params['width']) && !isset($params['height'])) {
      // IE doesn't like to calculate the image size on its own.
      // Thus we do it here: Calculate the new height from ratio of image file times the new width
      $params['height'] = ($this->info['height'] / $this->info['width']) * $params['width'];
    }

    if (module_exists('image')) {
      $image_label = IMAGE_ORIGINAL;
      $image_sizes_unsorted = image_get_sizes();

      // Sort according to image size, smallest first.
      $image_sizes = array();
      if (isset($image_sizes_unsorted[IMAGE_THUMBNAIL])) {
        $image_sizes[] = $image_sizes_unsorted[IMAGE_THUMBNAIL];
      }
      if (isset($image_sizes_unsorted[IMAGE_PREVIEW])) {
        $image_sizes[] = $image_sizes_unsorted[IMAGE_PREVIEW];
      }
      if (isset($image_sizes_unsorted[IMAGE_ORIGINAL])) {
        $image_sizes[] = $image_sizes_unsorted[IMAGE_ORIGINAL];
      }
    }
    else {
      $image_sizes = array();
    }

    // Determine which image-size, i.e. original, preview or thumb to use.
    $upright = $this->info['width'] < $this->info['height'];
    foreach($image_sizes as $image_label => $image_size) {
      $image_label = strtolower($image_size['label']);
      if ($upright) {
        if ($image_size['height'] >= $params['height']) {
          break;
        }
        else {
          unset($image_label);
        }
      }
      else {
        if ($image_size['width'] >= $params['width']) {
          break;
        }
        else {
          unset($image_label);
        }
      }
    }

    if (!$image_label) {
      $image_label = IMAGE_ORIGINAL;
    }
    wfProfileIn( __METHOD__ );
    $path = file_create_path($this->node->images[$image_label]);
    $url = url($this->node->images[$image_label]);

    $thumb = new DrupalImageTransformOutput($this, $url, $path, $params['width'], $params['height']);

    wfProfileOut( __METHOD__ );
    return $thumb;
  }

  function getUnscaledThumb() {
    return $this->transform(array('width' => $this->info['width']));
  }
}

/**
 * Image file handler dummy.
 */
class DrupalImageFileHandler {
  function getParamMap() {
    return array('img_width' => 'width');
  }

  function validateParam( $name, $value ) {
    if ( in_array( $name, array( 'width', 'height' ) ) ) {
      if ( $value <= 0 ) {
        return FALSE;
      }
      else {
        return TRUE;
      }
    }
    else {
      return FALSE;
    }
  }

  function parserTransformHook( $parser, $file ) {}
}

/**
 * Abstraction of an 'image' attachment of a certain size.
 */
class DrupalImageTransformOutput {

  var $file, $width, $height, $url, $page, $path;

  function __construct($file, $url, $path, $width, $height) {
    $this->file = $file;
    $this->url = $url;
    $this->path = $path;
    $this->width = $width;
    $this->height = $height;
  }

  /**
   * Get the width of the output box
   */
  function getWidth() {
    return $this->width;
  }

  /**
   * Get the height of the output box
   */
  function getHeight() {
    return $this->height;
  }

  /**
   * @return string The thumbnail URL
   */
  function getUrl() {
    return $this->url;
  }

  /**
   * @return string Destination file path (local filesystem)
   */
  function getPath() {
    return $this->path;
  }

  /**
   * Return HTML <img ... /> tag for the thumbnail, will include
   * width and height attributes and a blank alt text (as required).
   *
   * @param array $options Associative array of options. Boolean options
   *     should be indicated with a value of TRUE for TRUE, and FALSE or
   *     absent for FALSE.
   *
   *     alt          Alternate text or caption
   *     desc-link    Boolean, show a description link
   *     file-link    Boolean, show a file download link
   *     valign       vertical-align property, if the output is an inline element
   *     img-class    Class applied to the <img> tag, if there is such a tag
   *
   * For images, desc-link and file-link are implemented as a click-through. For
   * sounds and videos, they may be displayed in other ways.
   *
   * @return string
   * @public
   */
  function toHtml( $options = array() ) {
    if ( count( func_get_args() ) == 2 ) {
      throw new MWException( __METHOD__ .' called in the old style' );
    }

    $alt = empty( $options['alt'] ) ? '' : $options['alt'];
    if ( !empty( $options['desc-link'] ) ) {
      $linkAttribs = $this->getDescLinkAttribs( $alt );
    }
    elseif ( !empty( $options['file-link'] ) ) {
      $linkAttribs = array( 'href' => $this->file->getURL() );
    }
    else {
      $linkAttribs = FALSE;
    }

    $attribs = array(
      'alt' => $alt,
      'src' => $this->url,
      'width' => $this->width,
      'height' => $this->height,
      'border' => 0,
    );
    if ( !empty( $options['valign'] ) ) {
      $attribs['style'] = "vertical-align: {$options['valign']}";
    }
    if ( !empty( $options['img-class'] ) ) {
      $attribs['class'] = $options['img-class'];
    }
    return $this->linkWrap( $linkAttribs, Xml::element( 'img', $attribs ) );
  }

  /**
   * This will be overridden to return TRUE in error classes
   */
  function isError() {
    return FALSE;
  }

  /**
   * Wrap some XHTML text in an anchor tag with the given attributes
   */
  protected function linkWrap( $linkAttribs, $contents ) {
    if ( $linkAttribs ) {
      return Xml::tags( 'a', $linkAttribs, $contents );
    }
    else {
      return $contents;
    }
  }

  function getDescLinkAttribs( $alt = FALSE ) {
    $query = $this->page ? ( 'page='. urlencode( $this->page ) ) : '';
    $title = $this->file->getTitle();
    if ( strval( $alt ) === '' ) {
      $alt = $title;
    }
    return array(
      'href' => url('node/'. $this->file->node->nid),
      'class' => 'image',
      'title' => $alt
    );
  }

}

/*
 * PHP error prevention...
 */
class DummyStub {
  function __call($name, $arguments) {
    drupal_set_message(t("Non-existing function %name called. Please report this error to the maintainer.", array('%name' => $name)), ERROR);
    return NULL;
  }
}

