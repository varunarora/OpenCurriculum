<?php
// $Id: mediawiki_filter.classes.inc 3767 2010-10-19 08:09:27Z bherlig $

/**
 * @file
 * Classes needed for the Mediawiki parser to work.
 * 
 * NOTE:
 * Some of Drupal's Coding Guidelines had to be broken (CamelCase usage and 
 * global-variable's names), because of the external dependencies on Mediawiki,
 * and thus it's naming convention.
 */

/**
 * Subclass of the mediawiki parser with Drupal-related functionality.
 */
class MediawikiFilterParser extends Parser {

  function fetchTemplateAndTitle($title) {
    static $template_cache = array();

    $lowercase_title = str_replace(' ', '_', strtolower($title->getText()));
    $type = mediawiki_filter_namespace_template_node_type();
    $result = (db_query("SELECT nid, vid FROM {node} WHERE type = '%s' AND REPLACE(LOWER(title),'_',' ') = '%s'", $type, $lowercase_title));
    if ($row = db_fetch_object($result)) {
      if (db_fetch_object($result)) {
        // TODO: do some more elaborate stuff
        drupal_set_message(t('Found mutliple templates with name %title. Using <a href="!url">first match</a> for current page.', array('%title' => $title->getText(), '!url' => url('node/'. $row->nid))));
      }
      if (isset($template_cache[$row->nid])) {
        $content = $template_cache[$row->nid];
      }
      else {
        $content = db_result(db_query("SELECT body FROM {node_revisions} WHERE vid = %d", $row->vid));
        $template_cache[$row->nid] = $content;
      }
      $this->mOutput->addTemplate($title, $row->nid, $row->vid);
    }
    else {
      $content = FALSE;
    }
    return array($content, $title);
  }

}

/**
 * User class which implements the bare functions needed for the Mediawiki
 * parser to work.
 */
class User {

  var $_skin;

  function User() {
    $this->_skin = new Linker();
  }

  function getName() {
    // Here the Drupal user comes into play.
    global $user;
    return $user->name;
  }

  function getUserPage() {
    return Title::makeTitle( NS_USER, $this->getName() );
  }

  function getOption( $oname, $defaultOverride = '' ) {
    printLine("User::getOption($oname, $defaultOverride)");
    return $this->getDefaultOption($oname);
  }

  function getBoolOption( $oname ) {
    return (bool)$this->getOption( $oname );
  }

  function getDefaultOption( $oname ) {
    printLine("User::getDefaultOption($oname)");
    $default_options = User::getDefaultOptions();
    if (isset($default_options[$oname])) {
      return $default_options[$oname];
    }
    return NULL;
  }

  static function getDefaultOptions() {
    printLine("User::getDefaultOptions");
    // TODO: Check for needed default options
    $defOpt['variant'] = 'en';
    $defOpt['language'] = 'en';
    $defOpt['numberheadings'] = FALSE;
    $defOpt['thumbsize'] = 100;

    // TODO: Check what formatting should be used
    $defOpt['date'] = variable_get('date_format_short', 'm/d/Y - H:i');

    return $defOpt;
  }

  function &getSkin() {
    return $this->_skin;
  }

}

/**
 * Dummy database class.
 */
class Database {

  var $lastResult = NULL;

  function tableName( $name ) {
    return $name;
  }

  function addQuotes( $s ) {
    if ( is_null( $s ) ) {
      return 'NULL';
    }
    else {
      return "'". urlencode($s) ."'";
    }
  }

  function query( $sql, $fname = '', $tempIgnore = FALSE ) {
    $this->lastResult = array();
    // The only query here should be the existing page query to know if a link target exists or not.
    // We need to provide the result of the Drupal equivalent.
    if (preg_match("/^SELECT page_id/", $sql)) {
      // Chances are good that it is indeed the page check query. Get the important information
      // from the query and build a Drupal query from it.
      $matches = array();
      $count = preg_match_all("/page_namespace=(\d+) AND page_title IN\(([^)]*)\)/", $sql, $matches);
      for ($i=0; $i<$count; $i++) {
        $namespace = $matches[1][$i];
        $types = mediawiki_filter_node_types_for_namespace($namespace);
        if (count($types)) {
          $pages = explode("', '", substr($matches[2][$i], 1, -1));

          // prepare pages-strings
          foreach ($pages as &$p) {
              $p = str_replace(" ", "_", strtolower(urldecode($p)));
          }

          $sql = "SELECT nid, type, title FROM {node} WHERE type IN (". db_placeholders($types, 'text') .") AND REPLACE(LOWER(title),' ','_') IN (". db_placeholders($pages, 'text') .")";
          $db_result = db_query($sql, array_merge($types, $pages));


          while ($row = db_fetch_object($db_result)) {
            $result_object = new StdClass();
            $result_object->page_id = $row->nid;
            $result_object->page_namespace = $namespace;
            $result_object->page_title = $row->title;
            $this->lastResult[] = $result_object;
          }
        }
      }
    }
    else {
      drupal_set_message('Unexpected Mediawiki query. Please report to module maintainer', 'error');
    }
    return NULL;
  }

  function fetchObject($res) {
    return array_pop($this->lastResult);
  }

}

/**
 * Dummy link cache class
 */
class LinkCache {

  var $mGoodLinks, $mBadLinks;

  static function &singleton() {
    static $instance;
    if ( !isset( $instance ) ) {
      $instance = new LinkCache;
    }
    return $instance;
  }

  function __construct() {
    $this->mGoodLinks = array();
    $this->mBadLinks = array();
  }

  function getGoodLinkID( $title ) {
    $key = strtolower($title);
    if ( array_key_exists( $key, $this->mGoodLinks ) ) {
      return $this->mGoodLinks[$key];
    }
    else {
      return 0;
    }
  }

  function isBadLink( $title ) {
    $key = strtolower($title);
    return array_key_exists( $key, $this->mBadLinks );
  }

  function addGoodLinkObj( $id, $title ) {
    $dbkey = strtolower($title->getPrefixedDbKey());
    $this->mGoodLinks[$dbkey] = $id;
  }

  function addBadLinkObj( $title ) {
    $dbkey = strtolower($title->getPrefixedDbKey());
    if ( ! $this->isBadLink( $dbkey ) ) {
      $this->mBadLinks[$dbkey] = 1;
    }
  }

}

/**
 * Dummy site stats class used for variable replacement.
 */
class SiteStats {

  /** Used for {{numberofarticles}} */
  static function articles() {
    return db_result(db_query('SELECT count(*) FROM {node}'));
  }

  /** Used for {{numberofpages}} */
  static function pages() {
    return db_result(db_query('SELECT count(*) FROM {node}'));
  }

  /** Used for {{numberofusers}} */
  static function users() {
    return db_result(db_query('SELECT count(*) FROM {users}'));
  }

  /** Used for {{numberoffiles}} */
  static function images() {
    return db_result(db_query("SELECT count(*) FROM {node} WHERE type = 'image'"));
  }

  /** Used for {{numberofadmins}} */
  static function admins() {
    return 1;
  }

  /** Used for {{numberofedits}} */
  static function edits() {
    return db_result(db_query("SELECT count(*) FROM {node_revisions}"));
  }

}
