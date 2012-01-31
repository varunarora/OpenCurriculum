<?php
// $Id: mediawiki_filter.settings.inc 3767 2010-10-19 08:09:27Z bherlig $

/**
 * @file
 * Include file for Mediawiki filter settings.
 */

/**
 * Callback for settings formular
 */
function _mediawiki_filter_filter_settings($format) {
  $form = array();

  $form['mediawiki_filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mediawiki filter'),
    '#collapsible' => TRUE,
    '#description' => t('Settings for the Mediawiki filter. These settings are global, not per input format.'),
  );
  $form['mediawiki_filter']['wikilinks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Wikilinks'),
    '#description' => t('Set handling of wikilinks, for example <em>[[Name]]</em>'),
  );
  $form['mediawiki_filter']['wikilinks']['mediawiki_filter_wikilink_existing_handler'] = array(
    '#type' => 'radios',
    '#title' => t('Handler for existing links'),
    '#options' => array(
      'wikitools' => t('Wikitools module'),
      'freelinking' => t('Freelinking module'),
      'liquid' => t('Liquid module'),
      'path' => t('Use fixed wiki path'),
      'direct' => t('Use a direct link'),
    ),
    '#default_value' => mediawiki_filter_wikilink_existing_handler(),
    '#disabled' => TRUE,
  );
  $form['mediawiki_filter']['wikilinks']['mediawiki_filter_fixed_path_for_existing'] = array(
    '#type' => 'textfield',
    '#title' => t('Fixed wiki path for existing pages'),
    '#default_value' => mediawiki_filter_wikilink_fixed_path_for_existing(),
    '#description' => t('If you use a fixed wiki path, set it here. Use <em>$1</em> as a placeholder for the wiki page name.'),
    '#disabled' => mediawiki_filter_wikilink_existing_handler() != 'path',
  );
  $form['mediawiki_filter']['wikilinks']['mediawiki_filter_wikilink_nonexisting_handler'] = array(
    '#type' => 'radios',
    '#title' => t('Handler for non-existing links'),
    '#options' => array(
      'wikitools' => t('Wikitools module'),
      'freelinking' => t('Freelinking module'),
      'liquid' => t('Liquid module'),
      'path' => t('Use fixed wiki path'),
      'create-link' => t('Link to node creation'),
    ),
    '#default_value' => mediawiki_filter_wikilink_nonexisting_handler(),
    '#disabled' => TRUE,
  );
  $form['mediawiki_filter']['wikilinks']['mediawiki_filter_fixed_path_for_nonexisting'] = array(
    '#type' => 'textfield',
    '#title' => t('Fixed wiki path for non-existing pages'),
    '#default_value' => mediawiki_filter_wikilink_fixed_path_for_nonexisting(),
    '#description' => t('If you use a fixed wiki path, set it here. Use <em>$1</em> as a placeholder for the wiki page name and $type for a placeholder for the node type.'),
    '#disabled' => mediawiki_filter_wikilink_nonexisting_handler() != 'path',
  );
  $form['mediawiki_filter']['wikilinks']['mediawiki_filter_nonexisting_link_node_type'] = array(
    '#type' => 'select',
    '#title' => t('Node type of new pages'),
    '#options' => node_type_get_names(),
    '#default_value' => mediawiki_filter_nonexisting_link_node_type(),
    '#description' => t('Default node type for new pages.'),
  );
  $form['mediawiki_filter']['wikilinks']['mediawiki_filter_nonexisting_link_css_class'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS class for non-existing links'),
    '#default_value' => mediawiki_filter_nonexisting_link_css_class(),
    '#description' => t('CSS class for links which point to non-existing pages.'),
  );
  $form['mediawiki_filter']['interwiki'] = array(
    '#type' => 'fieldset',
    '#title' => t('Interwiki links'),
    '#description' => t('Set handling of interwiki links, for example <em>[[wp:Name]]</em>'),
  );
  $form['mediawiki_filter']['interwiki']['mediawiki_filter_intewiki_links'] = array(
    '#type' => 'textarea',
    '#title' => t('Interwiki mappings'),
    '#default_value' => mediawiki_filter_intewiki_links(),
    '#description' => t('A list of interwiki mappings Each line denotes one mapping. The format is: <em>title</em> | <em>interwiki name</em> | <em>mapping url</em>. <br />&lt;path&gt; is a special mapping url which will be transformed into a Drupal path for the current site. Use <em>$1</em> as a placeholder for the wiki page name.'),
  );
  $form['mediawiki_filter']['media'] = array(
    '#type' => 'fieldset',
    '#title' => t('Media'),
    '#description' => t('Set handling of media links, for example <em>[[Image:Name.png]]</em>.') .'<p class="error">Hardcoded to image module</p>',
  );
  $form['mediawiki_filter']['categories'] = array(
    '#type' => 'fieldset',
    '#title' => t('Categories'),
    '#description' => t('Set handling of category links, for example <em>[[Category:Name]]</em>') .'<p class="error">Not yet implemented</p>',
  );
  $form['mediawiki_filter']['templates'] = array(
    '#type' => 'fieldset',
    '#title' => t('Templates'),
    '#description' => t('Set handling of templates, for example <em>{{Name}}</em>'),
  );
  $form['mediawiki_filter']['templates']['mediawiki_filter_namespace_template_node_type'] = array(
    '#type' => 'select',
    '#title' => t('Templates'),
    '#options' => array_merge(array('' => t('<disabled>')), node_type_get_names()),
    '#default_value' => mediawiki_filter_namespace_template_node_type(),
    '#description' => t('Choose the node type which should be used for templates.'),
  );
  $form['mediawiki_filter']['language'] = array(
    '#type' => 'fieldset',
    '#title' => t('Language links'),
    '#description' => t('Set handling of language links, for example <em>[[de:Name]]</em>') .'<p class="error">Not yet implemented</p>',
  );
  $form['mediawiki_filter']['namespaces'] = array(
    '#type' => 'fieldset',
    '#title' => t('Namespaces'),
    '#description' => t('Set handling of different namespaces, predefined or custom.'),
  );
  $form['mediawiki_filter']['namespaces']['mediawiki_filter_namespace_main_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Main namespace'),
    '#options' => node_type_get_names(),
    '#default_value' => mediawiki_filter_namespace_main_node_types(),
    '#description' => t('Choose the node type which should be used for the main namespace.'),
  );
  $form['mediawiki_filter']['miscellaneous'] = array(
    '#type' => 'fieldset',
    '#title' => t('Miscellaneous'),
  );
  $form['mediawiki_filter']['miscellaneous']['mediawiki_filter_options'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Options'),
    '#options' => array(
      'show_toc' => t('Show table of contents'),
      'capitalize' => t('Capitalize page names'),
    ),
    '#description' => '<p class="error">Not yet implemented</p>',
  );
  $form['mediawiki_filter']['extensions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Extensions'),
    '#description' => t('Manage filter extensions.') .'<p class="error">Not yet implemented</p>',
  );
  $form['mediawiki_filter']['compatibility'] = array(
    '#type' => 'fieldset',
    '#title' => t('Compatibility'),
    '#description' => t('Manage compatibility with other filters.') .'<p class="error">Hardcoded ignoring of &lt;code&gt; tags</p>',
  );

  return $form;
}

function mediawiki_filter_wikilink_existing_handler() {
  return variable_get('mediawiki_filter_wikilink_existing_handler', 'direct');
}

function mediawiki_filter_wikilink_nonexisting_handler() {
  return variable_get('mediawiki_filter_wikilink_nonexisting_handler', 'create-link');
}

function mediawiki_filter_wikilink_fixed_path_for_existing() {
  return variable_get('mediawiki_filter_fixed_path_for_existing', 'wiki/$1');
}

function mediawiki_filter_wikilink_fixed_path_for_nonexisting() {
  return variable_get('mediawiki_filter_fixed_path_for_nonexisting', 'node/create/$type?edit[title]=$1');
}

function mediawiki_filter_nonexisting_link_css_class() {
  return variable_get('mediawiki_filter_nonexisting_link_css_class', 'wiki-link-new');
}

function mediawiki_filter_nonexisting_link_node_type() {
  return variable_get('mediawiki_filter_nonexisting_link_node_type', 'wiki');
}

function mediawiki_filter_intewiki_links() {
  return variable_get('mediawiki_filter_intewiki_links', "Local path | path | <path>\nWikipedia  | wp   | http://en.wikipedia.org/wiki/$1\n");
}

function mediawiki_filter_namespace_template_node_type() {
  return variable_get('mediawiki_filter_namespace_template_node_type', '');
}

function mediawiki_filter_namespace_main_node_types() {
  return variable_get('mediawiki_filter_namespace_main_node_types', array());
}

function mediawiki_filter_namespace_image_node_type() {
  return variable_get('mediawiki_filter_namespace_image_node_type', 'image');
}

function mediawiki_filter_wgArticlePath() {
  $handler = mediawiki_filter_wikilink_existing_handler();
  switch ($handler) {
    case 'path':
      return mediawiki_filter_wikilink_fixed_path_for_existing();
    case 'wikitools':
    case 'freelinking':
    case 'liquid':
  }
  return NULL;
}

function mediawiki_filter_interwiki_mappings($op = 'url') {
  $mappings = array();
  $lines = explode("\n", mediawiki_filter_intewiki_links());
  foreach ($lines as $line) {
    if ($line) {
      list($title, $id, $url) = explode('|', $line, 3);
      if (trim($url) == '<path>') {
        $url = '$1'; //str_replace('__QUERY_POSITION__', '$1', url('__QUERY_POSITION__', NULL, NULL, TRUE));
      }
      if ($op == 'url') {
        $mappings[trim($id)] = trim($url);
      }
      elseif ($op == 'array') {
        $mappings[trim($id)] = array(
          'id' => trim($id),
          'title' => trim($title),
          'url' => trim($url)
        );
      }
    }
  }
  return $mappings;
}
