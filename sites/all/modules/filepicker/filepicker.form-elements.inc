<?php

/**
 * @file
 * Provides various form elements.
 */

function filepicker_colorbox_elem($default = NULL) {
  return array(
      '#type' => 'checkbox',
      '#title' => t('Colorbox Iframe'),
      '#description' => t('Use colorbox iframe instead of the builtin one.'),
      '#return_value' => 1,
      '#default_value' => (is_numeric($default) ? $default : filepicker_variable_get('filepicker_advanced_colorbox', 0)),
    );
}
function filepicker_iframe_width_elem($default = NULL) {
  return array(
    '#type' => 'textfield',
    '#title' => t('Iframe width'),
    '#description' => t('Configure the iframe width. This can be a number or a percentage, eg 400 or 100%'),
    '#size' => 10,
    '#required' => TRUE,
    '#default_value' => ($default != NULL ? $default : filepicker_variable_get('filepicker_advanced_iframe_width', "100%")),
  );
}
function filepicker_iframe_height_elem($default = NULL) {
  return array(
    '#type' => 'textfield',
    '#title' => t('Iframe height'),
    '#description' => t('Configure the iframe height. This must be a number, eg 500'),
    '#size' => 10,
    '#required' => TRUE,
    '#default_value' => ($default != NULL ? $default : filepicker_variable_get('filepicker_advanced_iframe_height', "500")),
  );
}
function filepicker_iframe_border_elem($default = NULL) {
  return array(
    '#type' => 'textfield',
    '#title' => t('Iframe border'),
    '#description' => t('Configure the iframe border. This can be a number, eg 0 or 1, or a css entry such as 1px solid #808080'),
    '#size' => 25,
    '#required' => TRUE,
    '#default_value' => ($default != NULL ? $default : filepicker_variable_get('filepicker_advanced_iframe_border', "0")),
  );
}
function filepicker_collapsed_elem($default = NULL) {
  return array(
    '#type' => 'checkbox',
    '#title' => t('Fieldset state'),
    '#description' => t('Setting this option will collapse the fieldset the iframe is in by default.'),
    '#return_value' => 1,
    '#default_value' => (is_numeric($default) ? $default : filepicker_variable_get('filepicker_advanced_collapsed', 0)),
  );
}

function filepicker_rows_per_page_elem($default = NULL) {
  return array(
    '#type' => 'textfield',
    '#title' => t('Rows per page in tables'),
    '#size' => 5,
    '#required' => TRUE,
    '#default_value' => ($default != NULL ? $default : filepicker_variable_get('filepicker_rows_per_page', 25)),
  );
}
function filepicker_use_admin_theme_elem($default = NULL) {
  return array(
    '#type' => 'checkbox',
    '#title' => t('Use the administration theme for Filepicker Iframe'),
    '#return_value' => 1,
    '#default_value' => (is_numeric($default) ? $default : filepicker_variable_get('filepicker_use_admin_theme', 0)),
  );
}
