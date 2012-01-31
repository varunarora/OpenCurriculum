<?php

/**
 * @file
 * Theme functions for filepicker.
 */

/**
 * theme registry
 */
function filepicker_theme() {
  return array(
    'filepicker' => array(
      'variables' => array('content' => NULL),
      'template' => 'filepicker',
    ),

    'filepicker_iframe' => array(
      'variables' => array(
        'content' => NULL,
        'file'  => NULL,
        'public'  => NULL,
      ),
    ),
    'filepicker_list' => array(
      'variables' => array(
        'header' => array(),
        'rows' => array(),
        'max' => 50,
        'message' => "",
        'pref' => "",
        'suff' => "",
        'label' => "",
      ),
    ),
    'filepicker_stats' => array(
      'variables' => array(
        'header' => array(),
        'rows' => array(),
        'pref' => "",
        'suff' => "",
        'label' => "",
      ),
    ),
    'filepicker_fullpage' => array(
      'variables' => array(
        'file' => array(),
        'source' => "",
        'link' => 1,
      ),
    ),
    'filepicker_quota_message' => array(
      'variables' => array(
        'message1' => "",
        'message2' => "",
        'form' => NULL,
        'label' => "",
        'help' => "",
      ),
    ),
    'filepicker_view' => array(
      'variables' => array(
        'file' => array(),
        'filepath' => "",
        'info' => array(),
      ),
    ),
    'filepicker_file_edit_header' => array(
      'variables' => array(
        'file' => array(),
        'source' => "",
      ),
    ),
    'filepicker_quota' => array(
      'variables' => array(
        'form' => NULL,
        'message' => "",
        'label' => "",
      ),
    ),
    'filepicker_user_config' => array(
      'variables' => array(
        'form' => NULL,
        'label' => "",
        'help' => "",
        'message1' => "",
        'message2' => "",
      ),
    ),
    'filepicker_user_view' => array(
      'variables' => array(
        'view' => "",
        'form1' => NULL,
        'form2' => NULL,
        'form3' => NULL,
      ),
    ),
    'filepicker_admin_view' => array(
      'variables' => array(
        'view' => "",
        'form1' => NULL,
        'form2' => NULL,
        'form3' => NULL,
      ),
    ),
    'filepicker_insert' => array(
      'variables' => array(
        'file' => NULL,
        'public' => NULL,
        'form1' => NULL,
        'form2' => NULL,
      ),
    ),
    'filepicker_upload_form' => array(
      'render element' => 'form'
    ),
    'filepicker_user_file_form' => array(
      'render element' => 'form'
    ),
    'filepicker_user_config_admin_form' => array(
      'render element' => 'form'
    ),
    'filepicker_admin_file_form' => array(
      'render element' => 'form'
    ),
    'filepicker_user_search_form' => array(
      'render element' => 'form'
    ),
    'filepicker_group_search_form' => array(
      'render element' => 'form'
    ),
    'filepicker_quota_form' => array(
      'render element' => 'form'
    ),
    'filepicker_edit_form' => array(
      'render element' => 'form'
    ),
    'filepicker_groups_form' => array(
      'render element' => 'form'
    ),
    'filepicker_group_delete_form' => array(
      'render element' => 'form'
    ),
    'filepicker_file_form' => array(
      'render element' => 'form'
    ),
    'filepicker_list_groups_form' => array(
      'render element' => 'form'
    ),
    'filepicker_list_public_groups_form' => array(
      'render element' => 'form'
    ),
    'filepicker_group_files_form' => array(
      'render element' => 'form'
    ),
    'filepicker_list_public_form' => array(
      'render element' => 'form'
    ),
    'filepicker_list_search_form' => array(
      'render element' => 'form'
    ),
    'filepicker_settings_form' => array(
      'render element' => 'form'
    ),
    'filepicker_list_admin_form' => array(
      'render element' => 'form'
    ),
    'filepicker_list_admin' => array(
      'variables' => array(
        'forms' => '',
        'message' => '',
        'pref' => '',
        'suff' => '',
        'label' => '',
      ),
    ),
    'filepicker_multitask_delete_form' => array(
      'render element' => 'form'
    ),
    'filepicker_multitask_groups_form' => array(
      'render element' => 'form'
    ),
    'filepicker_import_form' => array(
      'render element' => 'form'
    ),
    'filepicker_import_dir_form' => array(
      'render element' => 'form'
    ),
    'filepicker_admin_orphans_form' => array(
      'render element' => 'form',
    ),

  );
}

// preprocess for filepicker.tpl.php
function template_preprocess_filepicker(&$variables) {
  global $language;
  if (module_exists('admin_menu')) {
    admin_menu_suppress();
  }
  $variables['head_title']  = (drupal_get_title() ? strip_tags(drupal_get_title()) : variable_get('site_name', 'Drupal'));
  $variables['styles'] = drupal_get_css();
  $variables['scripts'] = drupal_get_js();
  $tabs = menu_local_tabs();
  $variables['tabs'] = theme('menu_local_tasks', array('primary' => $tabs['#primary']));
  $variables['messages'] = filepicker_strip_messages(theme('status_messages'));
  $variables['language'] = $language;
}

// set up the iframe
function theme_filepicker_iframe($variables) {
  $content = $variables['content'];
  $file = (isset($variables['file']) ? $variables['file'] : '');
  $public = (isset($variables['public']) ? $variables['public'] : FALSE);

  drupal_add_css(FILEPICKER_PATH . '/filepicker.css');
  if ($file) {
    global $user;
    $settings = array(
      'filepicker_iframe' => array(
        'isFCKeditor' => (module_exists('fckeditor') ? 'yes' : 'no' ),
        'isWysiwyg' => (module_exists('wysiwyg') ? 'yes' : 'no' ),
        'colorbox_enable' => (module_exists('colorbox') && filepicker_variable_get('filepicker_colorbox_enable', 0)),
        'colorbox_iframe' => (filepicker_get_colorbox_perms()),
        'file_new_window' => filepicker_variable_get('filepicker_use_new_window', 0),
        'node_editbody' => 'edit-body-' . filepicker_variable_get('filepicker_node_lang', '') . '-' . filepicker_variable_get('filepicker_node_pos', 0) . '-value',
      ),
    );
    drupal_add_js($settings, 'setting');
    drupal_add_js(FILEPICKER_PATH . '/filepicker_iframe.js');
  }
  return $content;
}

function theme_filepicker_list($variables) {
  $header = $variables['header'];
  $rows =  $variables['rows'];
  $max = $variables['max'];
  $message = $variables['message'];
  $pref = $variables['pref'];
  $suff = $variables['suff'];
  $label = $variables['label'];

  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $build['filepicker_list'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => $message,
  );
  $build['filepicker_pager'] = array('#theme' => 'pager');
  $output .= $pref . render($build) . $suff;
  $output .= ($label ? '</fieldset>' : '');
  return $output;

}

function theme_filepicker_stats($variables) {
  $header = $variables['header'];
  $rows =  $variables['rows'];
  $message = $variables['message'];
  $pref = $variables['pref'];
  $suff = $variables['suff'];
  $label = $variables['label'];

  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  if (count($rows)) {
    $output .= $pref . theme('table', array('header' => $header, 'rows' => $rows)) . $suff;
  }
  else {
    $output .= '<div class="messages">' . $message . '</div>';
  }
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_filepicker_quota_message($variables) {
  $message1 = $variables['message1'];
  $message2 = $variables['message2'];
  $form = $variables['form'];
  $label = $variables['label'];
  $help = $variables['help'];

  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $output .= ($message1 ? '<div class="messages">' . $message1 . '</div>' : '') ;
  $output .= ($help ? '<div class="filep_help">' . $help . '</div>' : '');
  $output .= $form;
  $output .= ($message2 ? '<div class="messages">' . $message2 . '</div>' : '') ;
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_filepicker_view($variables) {
  $file = $variables['file'];
  $filepath = $variables['filepath'];
  $info = $variables['info'];
  $output = '<div id="filep_file_view">';
  $output .= '<div id="filep_file_view_name">' . t('Name') . ": " . $file->file_name . '</div>';
  $output .= ($file->file_title ? '<div id="filep_file_view_title">' . t('Title') . ": " . $file->file_title . '</div>' : '');
  $output .= '<div id="filep_file_view_info">' . t('Size') . ": " . _filepicker_bkmg($info['file_size']) . '</div>';
  $output .= '</div>';
  return $output;
}

function theme_filepicker_file_edit_header($variables) {
  $file = $variables['file'];
  $source = $variables['source'];
  $output = '<div class="filep_help">' . t('Edit details for %f', array('%f' => $file->file_name )) . '</div>';
  return $output;
}

function theme_filepicker_quota($variables) {
  $form = $variables['quotaform'];
  $message = $variables['message'];
  $label = $variables['label'];
  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $output .= ($message ? '<div class="messages">' . $message . '</div>' : '') ;
  $output .= $form;
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_filepicker_user_config($variables) {
  $form = $variables['form'];
  $label = $variables['label'];
  $help = $variables['help'];
  $message1 = $variables['message1'];
  $message2 = $variables['message2'];
  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $output .= ($help ? '<div class="filep_help">' . $help . '</div>' : '');
  $output .= ($message1 ? '<div class="messages">' . $message1 . '</div>' : '') ;
  $output .= $form;
  $output .= ($message2 ? '<div class="messages">' . $message2 . '</div>' : '') ;
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_filepicker_user_view($variables) {
  $view = $variables['view'];
  $form1 = $variables['form1'];
  $form2 = $variables['form2'];
  $form3 = $variables['form3'];

  $output = "";
  $output .= $view;
  $output .= '<br />';
  $output .= $form1;
  $output .= '<br />';
  $output .= $form2;
  $output .= $form3;
  return $output;
}

function theme_filepicker_admin_view($variables) {
  $view = $variables['view'];
  $form1 = $variables['form1'];
  $form2 = $variables['form2'];
  $form3 = $variables['form3'];

  $output = "";
  $output .= $view;
  $output .= '<br />';
  $output .= $form1;
  $output .= '<br />';
  $output .= $form2;
  $output .= $form3;
  return $output;
}

function theme_filepicker_insert($variables) {
  $file = $variables['file'];
  $public = $variables['public'];
  $form1 = $variables['form1'];
  $form2 = $variables['form2'];
  $output = "";
  if ($file) {
    $filepath = filepicker_get_file_path($file, ($public ? $file->uid : FALSE ));
    $output .= '<div class="filep_help">' . t('Choose the settings you want, place the cursor in the Body box above and Insert file.') . '</div>';
    $output .= '<div id="filep_file_view_name">' . t('Name') . ": " . $file->file_name . '</div>';
    $output .= ($file->file_title ? '<div id="filep_file_view_title">' . t('Title') . ": " . $file->file_title . '</div>' : '');
    $output .= $form1;
    $output .= $form2;
  }
  return $output;
}

function theme_filepicker_upload_form($variables) {
  $form = $variables['form'];
  $output = '';
  $max_uploads = filepicker_variable_get('filepicker_max_uploads', 1);
  $max_filesize = ini_get('upload_max_filesize');

  $form['maxmsg']['#markup'] = '<div class="messages">' . t('Browse your computer for up to !c, Max %max Total', array('!c' => format_plural($max_uploads, '1 File', '@count Files'), '%max' => $max_filesize) ) . '</div>';
  for ($ct = 1; $ct <= $max_uploads; $ct++) {
    $form['file_upload_' . $ct]['#prefix'] = "<div id='filepicker_upload_link_wrapper_$ct'>";
    $form['title_' . $ct]['#suffix'] = '</div>';
    if (isset($form['link_' . $ct])) {
      $form['link_' . $ct]['#markup'] = "<div class='filepicker_upload_link' id='filepicker_upload_link_$ct' style='display:none'>" . l(t('Upload another'), '', array(
        'attributes' => array(
          'onClick' => "Drupal.filepicker_upload_link_click($ct); return false;"
        )
      )) . '</div>';
    }
  }
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_user_file_form($variables) {
  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_user_config_admin_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['submit']['#prefix'] = '<div class="container-inline">';
  $form['reset']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_admin_file_form($variables) {
  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_user_search_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['filepicker_currentuser']['#prefix'] = '<div id="filep_users_form" class="container-inline">';
  $form['filepicker_user_search_submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_group_search_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['filepicker_currentgroup']['#prefix'] = '<div id="filep_groups_form" class="container-inline">';
  $form['filepicker_group_search_submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_quota_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['filepicker_quota']['#prefix'] = '<div id="filep_quota_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_edit_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['submit']['#prefix'] = '<div id="filep_controls">';
  $form['cancel']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_groups_form($variables) {
  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_group_delete_form($variables) {
  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_file_form($variables) {
  $form = $variables['form'];
  $output = '';
  if (isset($form['delete'])) {
    $form['insert']['#prefix'] = '<div id="filep_controls">';
    $form['delete']['#suffix'] = '</div>';
  }
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_list_groups_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['gid']['#prefix'] = '<div id="filep_groups_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_list_public_groups_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['gid']['#prefix'] = '<div id="filep_groups_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_group_files_form($variables) {
  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_list_public_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['filepicker_list_public']['#prefix'] = '<div id="filep_list_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_list_search_form($variables) {
  $form = $variables['form'];
  $output = '';
  $form['filepicker_list_search']['#prefix'] = '<div id="filep_search_form" class="container-inline">';
  $form['reset']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_settings_form($variables) {
  $form = $variables['form'];
  $output = '';

  $jshide = ($form['filepicker_import']['filepicker_import_enabled']['#default_value'] ? '' : ' class="js-hide"');
  $form['filepicker_import']['filepicker_import_delete']['#prefix'] = '<div id="wrap-filepicker-import"' . $jshide . '>';
  $form['filepicker_import']['filepicker_import_max']['#suffix'] = '</div>';

  $jshide = ($form['filepicker_groups']['filepicker_groups_enabled']['#default_value'] ? '' : ' class="js-hide"');
  $form['filepicker_groups']['filepicker_public_enabled']['#prefix'] = '<div id="wrap-filepicker-groups"' . $jshide . '>';
  $form['filepicker_groups']['filepicker_groups_in_upload_enabled']['#suffix'] = '</div>';

  $jshide = ($form['filepicker_progress']['filepicker_upload_progress_enabled']['#default_value'] ? '' : ' class="js-hide"');
  $form['filepicker_progress']['filepicker_upload_progress_delay']['#prefix'] = '<div id="wrap-filepicker-upload-progress"' . $jshide . '>';
  if (version_compare(phpversion(), FILEPICKER_UPLOAD_STATUS_MIN_PHP) >= 0 && extension_loaded('uploadprogress')) {
    $form['filepicker_progress']['filepicker_uploadprogress']['#suffix'] = '</div>';
  }
  else {
    $form['filepicker_progress']['filepicker_upload_progress_message']['#suffix'] = '</div>';
  }

  $jshide = ($form['filepicker_quotas']['filepicker_quota_byrole']['#default_value'] ? '' : ' class="js-hide"');
  $form['filepicker_quotas']['filepicker_quota_role']['#prefix'] = '<div id="wrap-filepicker-quota-role"' . $jshide . '>';
  $form['filepicker_quotas']['filepicker_quota_role']['#suffix'] = '</div>';
  if (isset($form['filepicker_settings']['filepicker_admin_message'])) {
    $form['filepicker_settings']['filepicker_admin_message']['#prefix'] = '<div class="messages">';
    $form['filepicker_settings']['filepicker_admin_message']['#suffix'] = '</div>';
  }

  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_list_admin_form($variables) {
  $form = $variables['form'];
  $output = '';
  $public = $form['options']['public']['#value'];
  if ($public) {
    $rows = $form['files']['#options'];
    $header = $form['files']['#header'];
    $output .= theme('table', array('header' => $header, 'rows' => $rows));
    $output .= theme('pager');
  }
  else {
    $form['options']['#prefix'] = '<div class="container-inline">';
    $form['options']['#suffix'] = '</div>';
    $output .= drupal_render_children($form);
  }
  return $output;
}

function theme_filepicker_list_admin($variables) {
  $forms = $variables['forms'];
  $pref = $variables['pref'];
  $suff = $variables['suff'];
  $label = $variables['label'];
  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $output .= $pref;
  $output .= $forms['list_search'];
  $output .= $forms['list_groups'];
  $output .= $forms['list_public'];
  $output .= $forms['list_public_groups'];
  $output .= $forms['list_admin'];
  $output .= $suff;
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_filepicker_multitask_delete_form($variables) {
  $form = $variables['form'];
  $output = '';
  $count = $form['countnids']['#value'];
  $output .= '<p>' . t('You have selected %c to be deleted', array('%c' => format_plural( $count, '1 file', '@count files'))) . '</p>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_multitask_groups_form($variables) {
  $form = $variables['form'];
  $output = '';
  $count = $form['countnids']['#value'];
  $output .= '<p>' . t('You have selected %c to be grouped', array('%c' => format_plural( $count, '1 file', '@count files'))) . '</p>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_import_form($variables) {
  $form = $variables['form'];
  $output = '';
  $markup = $form['total']['#markup'];
  $form['total']['#markup'] = '<p>' . $markup . '</p>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_import_dir_form($variables) {
  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_filepicker_admin_orphans_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['msg']['#prefix'] = '<div class="messages">';
  $form['msg']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}
