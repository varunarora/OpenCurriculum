<?php

/**
 * @file
 * admin settings functions
 */

/**
 * Menu callback; presents the settings form for filepicker
 */
function filepicker_settings_page($op = NULL) {
  $info = '';
  $dir = filepicker_get_files_directory();
  $orphan_status = filepicker_check_orphans($dir);
  if ($orphan_status) {
    // we have orphans
    $info .= '<ul class="item-list">';
    foreach ($orphan_status AS $user_uid) {
      $info .= '<li>' . t("Nonexistent user %u has orphan files. Click <b>!h</b> to fix.", array('%u' => $user_uid, '!h' => l( t('Here'), FILEPICKER_ADMIN_PATH . '/orphans'))) . '</li>';
    }
    $info .= '</ul>';
  }

  $content = drupal_get_form('filepicker_settings_form', $info);
  return $content;
}

/**
 * Function to display the filepicker admin settings form
 * @return
 *   Returns the form.
 */
function filepicker_settings_form($form, &$form_state, $info) {
  drupal_add_js(FILEPICKER_PATH . '/filepicker.js');
  drupal_add_css(FILEPICKER_PATH . '/filepicker.css');

  // default settings
  $form['filepicker_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set filepicker settings'),
    '#description' => t('Overall administrative settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -40,
  );
  if ($info) {
    $form['filepicker_settings']['filepicker_admin_message'] = array(
      '#markup' => $info,
    );
  }
  $node_types =  node_type_get_names();
  $form['filepicker_settings']['filepicker_node_types_enabled'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Node Types'),
    '#description' => t('Set the node types you want to enable Filepicker for.'),
    '#default_value' => filepicker_variable_get('filepicker_node_types_enabled', array_keys($node_types)),
    '#options' => $node_types,
  );

  if (module_exists('comment')) {
    $form['filepicker_settings']['filepicker_comment_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Comments'),
      '#description' => t('Setting this option enables Filepicker in comments.'),
      '#return_value' => 1,
      '#default_value' => filepicker_variable_get('filepicker_comment_enabled', 0),
    );
  }
  $form['filepicker_settings']['filepicker_blocks_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Blocks'),
    '#description' => t('Setting this option enables Filepicker in blocks.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_blocks_enabled', 0),
  );
  $form['filepicker_settings']['filepicker_account_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Filepicker in My Account'),
    '#description' => t('Setting this option enables Filepicker in My Account.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_account_enabled', 1),
  );
  $form['filepicker_settings']['filepicker_user_config_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable User configuration'),
    '#description' => t('Allow users to select their own settings.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_user_config_enable', 1),
  );
  $form['filepicker_settings']['filepicker_use_full_url'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use full url'),
    '#description' => t('Setting this option will make filepicker use a full url to the file being inserted, unsetting it will use a path relative to the Drupal root.<br />This is useful if you are developing a site with a different url than the production site will be on.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_use_full_url', 0),
  );
  $form['filepicker_settings']['filepicker_use_new_window'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open link in new window'),
    '#description' => t('Setting this option will set the inserted link to open in a new window.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_use_new_window', 0),
  );
  $form['filepicker_settings']['filepicker_use_icons'] = array(
    '#type' => 'checkbox',
    '#title' => t('Icons in tables'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_use_icons', 1),
    '#description' => t('Setting this option enables icons in tables.'),
  );
  $max_uploads = range(0, 6);
  unset($max_uploads[0]);
  $form['filepicker_settings']['filepicker_max_uploads'] = array(
    '#type' => 'select',
    '#title' => t('Multiple Uploads'),
    '#description' => t('The maximum number of uploads at a time.'),
    '#default_value' => filepicker_variable_get('filepicker_max_uploads', 1),
    '#options' => $max_uploads,
  );

  // default options
  $form['filepicker_defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set filepicker defaults'),
    '#description' => t('These settings are also available to users if User Configuration is enabled.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -38,
  );
  $form['filepicker_defaults']['filepicker_rows_per_page'] = filepicker_rows_per_page_elem(filepicker_variable_get('filepicker_rows_per_page', 25));

  $form['filepicker_defaults']['filepicker_dateformat_default'] = array(
    '#type' => 'select',
    '#title' => t('Set Date Format'),
    '#description' => t("Select the Date format to use in tables."),
    '#default_value' => filepicker_variable_get('filepicker_dateformat_default', 'medium'),
    '#options' => array('short' => t('Short'), 'medium' => t('Medium'), 'long' => t('Long')),
  );

  $form['filepicker_defaults']['filepicker_upload_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Permitted file extensions'),
    '#default_value' => filepicker_variable_get('filepicker_upload_extensions', 'jpg jpeg gif png txt doc xls pdf ppt pps odt ods odp'),
    '#maxlength' => 255,
    '#description' => t('Default extensions that users can upload. Separate extensions with a space and do not include the leading dot.'),
  );
  $form['filepicker_defaults']['filepicker_upload_mimetypes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Mimetype check'),
    '#description' => t('Setting this option will enable the built in Drupal mimetype checking.<br />This will prevent the type "application/octet-stream" from being downloaded.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_upload_mimetypes', 0),
  );
  // quota
  $form['filepicker_quotas'] = array(
    '#type' => 'fieldset',
    '#title' => t('User quotas'),
    '#collapsible' => TRUE,
    '#collapsed' => (filepicker_variable_get('filepicker_quota_enable', 1) ? TRUE : FALSE),
    '#weight' => -28,
  );
  $form['filepicker_quotas']['filepicker_quota_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Quotas in Filepicker'),
    '#description' => t('Setting this option will enable Quotas in Filepicker.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_quota_enable', 1),
  );
  $form['filepicker_quotas']['filepicker_quota_byrole'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Quotas by Role'),
    '#description' => t("Setting this option will enable 'Quotas by Role' instead of 'Per User'."),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_quota_byrole', 0),
  );
  $form['filepicker_quotas']['filepicker_quota_role'] = array(
    '#type' => 'radios',
    '#title' => t('Available Roles'),
    '#description' => t("Select which Role for 'Quotas by Role'."),
    '#options' => user_roles(TRUE),
    '#default_value' => filepicker_variable_get('filepicker_quota_role', 2),
  );
  $form['filepicker_quotas']['filepicker_quota_default'] = array(
    '#type' => 'select',
    '#title' => t('Set Quota'),
    '#description' => t("Select the Quota to apply for 'Quotas by Role' or the default for 'Per User'."),
    '#default_value' => filepicker_variable_get('filepicker_quota_default', 0),
    '#options' => filepicker_get_quota_list(),
  );
  // Iframe
  $form['filepicker_iframe'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filepicker Iframe settings'),
    '#description' => t('You can alter the default styling of the iframe here,<br />useful if the iframe is not fitting in with your theme.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -22,
  );
  if (module_exists('colorbox') && variable_get('colorbox_load', 0)) {
    $form['filepicker_iframe']['filepicker_advanced_colorbox'] = filepicker_colorbox_elem(filepicker_variable_get('filepicker_advanced_colorbox', 0));
  }
  $form['filepicker_iframe']['filepicker_advanced_iframe_width'] = filepicker_iframe_width_elem(filepicker_variable_get('filepicker_advanced_iframe_width', "100%"));
  $form['filepicker_iframe']['filepicker_advanced_iframe_height'] = filepicker_iframe_height_elem(filepicker_variable_get('filepicker_advanced_iframe_height', "500"));
  $form['filepicker_iframe']['filepicker_advanced_iframe_border'] = filepicker_iframe_border_elem(filepicker_variable_get('filepicker_advanced_iframe_border', "0"));
  $form['filepicker_iframe']['filepicker_advanced_collapsed'] = filepicker_collapsed_elem(filepicker_variable_get('filepicker_advanced_collapsed', 0));
  $form['filepicker_iframe']['filepicker_use_admin_theme'] = filepicker_use_admin_theme_elem(filepicker_variable_get('filepicker_use_admin_theme', 0));

  // progress bar
  $form['filepicker_progress'] = array(
    '#type' => 'fieldset',
    '#title' => t('Progress bar'),
    '#description' => t('Enable and configure progress bar.'),
    '#collapsible' => TRUE,
    '#collapsed' => (filepicker_variable_get('filepicker_upload_progress_enabled', 1) ? TRUE : FALSE),
    '#weight' => -18,
  );
  $form['filepicker_progress']['filepicker_upload_progress_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Progress bar in upload'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_upload_progress_enabled', 1),
  );
  $form['filepicker_progress']['filepicker_upload_progress_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Progress bar Delay'),
    '#description' => t('Show the progress bar if processing is longer than this value.'),
    '#size' => 3,
    '#required' => TRUE,
    '#default_value' => filepicker_variable_get('filepicker_upload_progress_delay', 3),
    '#field_suffix' => t('seconds'),
  );

  $form['filepicker_progress']['filepicker_upload_progress_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Progress bar message'),
    '#description' => t('Message to display while uploading.'),
    '#size' => 60,
    '#default_value' => filepicker_variable_get('filepicker_upload_progress_message', t('Processing form... please wait.')),
  );
  $vok = FALSE;
  $extok = FALSE;
  if (version_compare(phpversion(), FILEPICKER_UPLOAD_STATUS_MIN_PHP) >= 0 ) {
    $vok = TRUE;
    if (extension_loaded('uploadprogress')) {
      $extok = TRUE;
    }
  }
  if ($vok && $extok) {
    $form['filepicker_progress']['filepicker_uploadprogress'] = array(
      '#type' => 'fieldset',
      '#title' => t('Upload Progress Extension'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['filepicker_progress']['filepicker_uploadprogress']['filepicker_uploadprogress_server'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable PECL uploadprogress'),
      '#default_value' => filepicker_variable_get('filepicker_uploadprogress_server', ''),
    );
    $form['filepicker_progress']['filepicker_uploadprogress']['filepicker_uploadprogress_interval'] = array(
      '#title' => t('Interval Time'),
      '#description' => t('The interval between each check.'),
      '#type' => 'textfield',
      '#size' => 5,
      '#default_value' => filepicker_variable_get('filepicker_uploadprogress_interval', 1),
      '#field_suffix' => t('seconds'),
    );
  }

  // Groups
  $form['filepicker_groups'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filepicker group settings'),
    '#description' => t('Enable and configure groups.'),
    '#collapsible' => TRUE,
    '#collapsed' => (filepicker_variable_get('filepicker_groups_enabled', 1) ? TRUE : FALSE),
    '#weight' => -16,
  );
  $form['filepicker_groups']['filepicker_groups_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Groups'),
    '#description' => t('Allow users to manage groups.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_groups_enabled', 1),
  );
  $form['filepicker_groups']['filepicker_public_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Public option'),
    '#description' => t('Allow users to make file groups publicly available.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_public_enabled', 1),
  );
  $form['filepicker_groups']['filepicker_publicroles_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Public Role Control option'),
    '#description' => t('Allow users to make publicly available file groups restricted by role.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_publicroles_enabled', 1),
  );
  $form['filepicker_groups']['filepicker_groups_in_upload_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Groups in upload form'),
    '#description' => t('Allow users to select a group in the upload form.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_groups_in_upload_enabled', 1),
  );
  // Import
  $form['filepicker_import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filepicker Bulk Import'),
    '#description' => t('Enable bulk import of files.'),
    '#collapsible' => TRUE,
    '#collapsed' => (filepicker_variable_get('filepicker_import_enabled', 0) ? TRUE : FALSE),
    '#weight' => -14,
  );
  $form['filepicker_import']['filepicker_import_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Bulk import processing'),
    '#description' => t('Import files into filepicker from an import folder in the filesystem.<br />Requires FTP or SSH access'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_import_enabled', 0),
  );
  $form['filepicker_import']['filepicker_import_delete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Bulk import deletion'),
    '#description' => t('Delete uploaded source files after succesful import into filepicker.<br />The files should be writeable by the web server for this option to work.'),
    '#return_value' => 1,
    '#default_value' => filepicker_variable_get('filepicker_import_delete', 0),
  );
  $form['filepicker_import']['filepicker_import_max'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Import Batch size'),
    '#description' => t('How many files to process at a time. Must be greater than 0 and less than 100.'),
    '#size' => 3,
    '#required' => TRUE,
    '#default_value' => filepicker_variable_get('filepicker_import_max', 5),
  );

  $form = system_settings_form($form);
  unset($form['#submit']);
  $form['#submit'][] = 'filepicker_settings_form_submit';
  unset($form['#theme']);
  $form['#theme'] = 'filepicker_settings_form';
  return $form;
}

/**
 * Validate settings form
 */
function filepicker_settings_form_validate($form, &$form_state) {
  global $user;
  if (! is_numeric($form_state['values']['filepicker_rows_per_page']) || $form_state['values']['filepicker_rows_per_page'] < 1 ) {
    form_set_error('filepicker_rows_per_page', t('The rows per page field must be greater than one.'));
  }
  if (! is_numeric($form_state['values']['filepicker_advanced_iframe_height']) || $form_state['values']['filepicker_advanced_iframe_height'] < 1 ) {
    form_set_error('filepicker_advanced_iframe_height', t('The Iframe height must be a positive number'));
  }
  if (! drupal_strlen($form_state['values']['filepicker_advanced_iframe_width'])) {
    form_set_error('filepicker_advanced_iframe_width', t('You must fill in the Iframe width'));
  }
  if (! drupal_strlen($form_state['values']['filepicker_advanced_iframe_border'])) {
    form_set_error('filepicker_advanced_iframe_border', t('You must fill in the Iframe border'));
  }
  if (! is_numeric($form_state['values']['filepicker_upload_progress_delay']) || $form_state['values']['filepicker_upload_progress_delay'] < 0) {
    form_set_error('filepicker_upload_progress_delay', t('The progress delay must be a positive number or zero'));
  }
  if (isset($form_state['values']['filepicker_uploadprogress_interval'])) {
    if ( ! is_numeric($form_state['values']['filepicker_uploadprogress_interval']) || $form_state['values']['filepicker_uploadprogress_interval'] < 1) {
      form_set_error('filepicker_uploadprogress_interval', t('The progress interval must be a positive number'));
    }
  }
  if (! is_numeric($form_state['values']['filepicker_import_max']) || ($form_state['values']['filepicker_import_max'] < 1 || $form_state['values']['filepicker_import_max'] > 100 )) {
    form_set_error('filepicker_import_max', t('The Import batch size must be must be a positive number.'));
  }

  if ($form_state['values']['filepicker_groups_enabled'] != 1) {
    $form_state['values']['filepicker_public_enabled'] = 0;
    $form_state['values']['filepicker_publicroles_enabled'] = 0;
    $form_state['values']['filepicker_groups_in_upload_enabled'] = 0;
  }
  if ($form_state['values']['filepicker_quota_enable'] != 1) {
    $form_state['values']['filepicker_quota_byrole'] = 0;
  }
}

function filepicker_settings_form_submit($form, &$form_state) {
  filepicker_variable_set('filepicker_node_types_enabled', $form_state['values']['filepicker_node_types_enabled']);
  if (module_exists('comment')) {
    filepicker_variable_set('filepicker_comment_enabled', $form_state['values']['filepicker_comment_enabled']);
  }
  filepicker_variable_set('filepicker_blocks_enabled', $form_state['values']['filepicker_blocks_enabled']);
  filepicker_variable_set('filepicker_account_enabled', $form_state['values']['filepicker_account_enabled']);
  filepicker_variable_set('filepicker_user_config_enable', $form_state['values']['filepicker_user_config_enable']);
  filepicker_variable_set('filepicker_use_full_url', $form_state['values']['filepicker_use_full_url']);
  filepicker_variable_set('filepicker_use_new_window', $form_state['values']['filepicker_use_new_window']);
  filepicker_variable_set('filepicker_use_icons', $form_state['values']['filepicker_use_icons']);
  filepicker_variable_set('filepicker_max_uploads', $form_state['values']['filepicker_max_uploads']);
  filepicker_variable_set('filepicker_rows_per_page', $form_state['values']['filepicker_rows_per_page']);
  filepicker_variable_set('filepicker_dateformat_default', $form_state['values']['filepicker_dateformat_default']);
  filepicker_variable_set('filepicker_upload_extensions', $form_state['values']['filepicker_upload_extensions']);
  filepicker_variable_set('filepicker_upload_mimetypes', $form_state['values']['filepicker_upload_mimetypes']);
  if (module_exists('colorbox') && variable_get('colorbox_load', 0)) {
    filepicker_variable_set('filepicker_advanced_colorbox', $form_state['values']['filepicker_advanced_colorbox']);
  }
  filepicker_variable_set('filepicker_quota_enable', $form_state['values']['filepicker_quota_enable']);
  filepicker_variable_set('filepicker_quota_byrole', $form_state['values']['filepicker_quota_byrole']);
  filepicker_variable_set('filepicker_quota_role', $form_state['values']['filepicker_quota_role']);
  filepicker_variable_set('filepicker_quota_default', $form_state['values']['filepicker_quota_default']);
  filepicker_variable_set('filepicker_advanced_iframe_width', $form_state['values']['filepicker_advanced_iframe_width']);
  filepicker_variable_set('filepicker_advanced_iframe_height', $form_state['values']['filepicker_advanced_iframe_height']);
  filepicker_variable_set('filepicker_advanced_iframe_border', $form_state['values']['filepicker_advanced_iframe_border']);
  filepicker_variable_set('filepicker_advanced_collapsed', $form_state['values']['filepicker_advanced_collapsed']);
  filepicker_variable_set('filepicker_use_admin_theme', $form_state['values']['filepicker_use_admin_theme']);
  filepicker_variable_set('filepicker_upload_progress_enabled', $form_state['values']['filepicker_upload_progress_enabled']);
  filepicker_variable_set('filepicker_upload_progress_delay', $form_state['values']['filepicker_upload_progress_delay']);
  filepicker_variable_set('filepicker_upload_progress_message', $form_state['values']['filepicker_upload_progress_message']);
  if (isset($form_state['values']['filepicker_uploadprogress_server'])) {
    filepicker_variable_set('filepicker_uploadprogress_server', $form_state['values']['filepicker_uploadprogress_server']);
    filepicker_variable_set('filepicker_uploadprogress_interval', $form_state['values']['filepicker_uploadprogress_interval']);
  }
  filepicker_variable_set('filepicker_groups_enabled', $form_state['values']['filepicker_groups_enabled']);
  filepicker_variable_set('filepicker_public_enabled', $form_state['values']['filepicker_public_enabled']);
  filepicker_variable_set('filepicker_publicroles_enabled', $form_state['values']['filepicker_publicroles_enabled']);
  filepicker_variable_set('filepicker_groups_in_upload_enabled', $form_state['values']['filepicker_groups_in_upload_enabled']);
  filepicker_variable_set('filepicker_import_enabled', $form_state['values']['filepicker_import_enabled']);
  filepicker_variable_set('filepicker_import_delete', $form_state['values']['filepicker_import_delete']);
  filepicker_variable_set('filepicker_import_max', $form_state['values']['filepicker_import_max']);

}

/**
 * Administration functions
 */
function filepicker_admin_users_list($dir, $label='') {

  $max = filepicker_variable_get('filepicker_rows_per_page', 25);
  $header = array(
    array('data' => t('Name'), 'field' => 'u.name'),
    t('Files'),
    (filepicker_variable_get('filepicker_groups_enabled', 1) ? t('Groups') : ''),
    (filepicker_variable_get('filepicker_quota_enable', 1) ? t('Quotas') : ''),
    t('Used'),
  );

  $rowcount = 0;
  $rows = NULL;
  $countquery = db_select('users', 'u');
  $countquery->addExpression('COUNT(DISTINCT u.uid)', 'ct');
  $countquery->join('filepicker', 'i', 'i.uid = u.uid');
  $countquery->condition('u.status', 1);

  $query = db_select('users', 'u')
    ->extend('PagerDefault')->limit($max)
    ->extend('TableSort')->orderByHeader($header);
  $query->setCountQuery($countquery);
  $query->distinct();
  $query->fields('u', array('uid', 'name'))
    ->addExpression('COUNT( i.file_id)', 'ct');
  $query->join('filepicker', 'i', 'i.uid = u.uid');
  $query->condition('u.status', 1)
    ->groupBy('u.uid')
    ->groupBy('u.name');
  $records = $query->execute();

  foreach ($records AS $row) {
    $account = user_load($row->uid);
    $allcount = $row->ct;
    $groupcount = _filepicker_user_has_groups('all', $account);
    $usedbytesprint = _filepicker_bkmg(filepicker_get_all_bytes($account)+1);
    $quota = 0;
    if (filepicker_variable_get('filepicker_quota_enable', 1)) {
      if (filepicker_variable_get('filepicker_quota_byrole', 0)) {
        $roleid = filepicker_variable_get('filepicker_quota_role', 2);
        if (filepicker_user_has_role($roleid, $account) && $account->uid > 1) {
          $quota = filepicker_variable_get('filepicker_quota_default', 0);
        }
      }
      else {
        $quota = filepicker_variable_get('filepicker_quota_default', filepicker_variable_get('filepicker_quota_default', 0), $account->uid);
      }
    }

    $row_data = array(
      check_plain($row->name),
      ($allcount ? l(format_plural($allcount, '1 file', '@count files'), FILEPICKER_ADMIN_PATH . '/files/user/' . $row->uid . '/list' ) : t('No files')),
      (filepicker_variable_get('filepicker_groups_enabled', 1) ? ($groupcount ? (l(format_plural($groupcount, '1 group', '@count groups'), FILEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid . '/browse' )) : t('No groups')) :  t('n/a')),
      (filepicker_variable_get('filepicker_quota_enable', 1) ? l(filepicker_get_quota_list($quota+1), FILEPICKER_ADMIN_PATH . '/files/user/' . $row->uid . '/quota') : t('n/a')),
      $usedbytesprint,
    );

    $rows[] = $row_data;
    $rowcount++;
  }

  $message = t('No users found.');
  $content = theme('filepicker_list', array('header' => $header, 'rows' => $rows, 'max' => $max, 'message' => $message, 'pref' => '', 'suff' => '', 'label' => $label));
  return $content;

}

function filepicker_admin_groups_list($label="") {
  $max = filepicker_variable_get('filepicker_rows_per_page', 25);
  $use_icons = filepicker_variable_get('filepicker_use_icons', 1);
  $rows = NULL;
  $header = array(
    array('data' => t('Group Name'), 'field' => 'g.group_name'),
    array('data' => t('User Name'), 'field' => 'u.name'),
    array('data' => t('Status'), 'field' => 'g.public'),
    array('data' => t('Files'), 'field' => 'ct'),
    array('data' => t('Actions'), 'colspan' => 2),
  );
  $countquery = db_select('filepicker_user_groups', 'g');
  $countquery->addExpression('COUNT(g.gid)', 'ct');

  $query = db_select('filepicker_user_groups', 'g')
    ->extend('PagerDefault')->limit($max)
    ->extend('TableSort')->orderByHeader($header);
  $query->setCountQuery($countquery);
  $query->fields('g', array('gid', 'uid', 'group_name', 'public'))
    ->addField('u', 'name');
  $query->addExpression('COUNT(i.file_id)', 'ct');
  $query->leftjoin('filepicker_group_files', 'i', 'g.gid = i.gid');
  $query->leftjoin('users', 'u', 'g.uid = u.uid');
  $query->groupBy('g.gid');
  $records = $query->execute();

  foreach ($records AS $row) {
    // get file count
    $ict = $row->ct;
    $editpath = FILEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid . '/edit/' . $row->gid;
    $deletepath = FILEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid . '/delete/' . $row->gid;
    $row_data = array(
      $row->group_name,
      l($row->name, FILEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid),
      ($row->public ? t('Public') : t('Private')),
      ($ict ? l(format_plural($ict, '1 file', '@count files'), FILEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid . '/list/' . $row->gid) : t('No files')),
      ($use_icons ? _filepicker_get_icon('edit', $editpath, array('title' => t('Edit'))) : l(t('Edit'), $editpath)),
      ($use_icons ?  _filepicker_get_icon('delete', $deletepath, array('title' => t('Delete'))) : l(t('Delete'), $deletepath)),
    );
    $rows[] = $row_data;
  }
  $pref = '';
  $suff = '';
  return theme('filepicker_list', array('header' => $header, 'rows' => $rows, 'max' => $max, 'message' => t('No groups found.'), 'pref' => $pref, 'suff' => $suff, 'label' => $label));

}

function filepicker_admin_view($file_id, $account = FALSE) {
  if ($account) {
    $user = $account;
    $userdir = array('uid' => $user->uid);
  }
  else {
    global $user;
    $userdir = FALSE;
  }
  $form1 = NULL;
  $form2 = NULL;
  $form3 = NULL;

  drupal_add_js(FILEPICKER_PATH . '/filepicker.js');
  $file = _filepicker_get_file($file_id, FALSE, $user);
  if ($file) {
    $filepath = filepicker_get_file_path($file, 'full', $userdir);
    $info = filepicker_get_info($file);
    $view = theme('filepicker_view', array('file' => $file, 'filepath' => $filepath, 'info' => $info));
    $form1 = render(drupal_get_form('filepicker_admin_file_form', $file_id, $user));
    // group form
    if (_filepicker_has_groups($user)) {
      $form3 = render(drupal_get_form('filepicker_group_files_form', $file_id, $user));
    }
    $content = theme('filepicker_admin_view', array('view' => $view, 'form1' => $form1, 'form2' => $form2, 'form3' => $form3));
  }
  else {
    drupal_set_message(t('File not found.'), 'error');
    $content = '';
  }
  return $content;
}

/**
 * Function to display the file edit/delete form
 * @return
 *   Returns the form.
 */
function filepicker_admin_file_form($form, &$form_state, $file_id, $account = FALSE) {
  if ($account) {
    $form['account'] = array(
      '#type' => 'value',
      '#value' => $account->uid,
    );
  }
  $form['file_id'] = array(
    '#type' => 'value',
    '#value' => $file_id,
  );
  $form['edit'] = array(
    '#type' => 'submit',
    '#value' => t('Edit file'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete file'),
  );
  return $form;
}

function filepicker_admin_file_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == t('Delete file')) {
    $account = user_load($form_state['values']['account']);
    filepicker_file_delete($form_state['values']['file_id'], $account, 'admin');
  }
  elseif ($form_state['values']['op'] == t('Edit file')) {
    drupal_goto(FILEPICKER_ADMIN_PATH . '/files/user/' . $form_state['values']['account'] . '/edit/' . $form_state['values']['file_id']);
  }
}

/**
 * Function to display the filepicker user search form
 * @return
 *   Returns the form.
 */
function filepicker_user_search_form($form, &$form_state, $dir) {
  $form['filepicker_currentuser'] = array(
    '#type' => 'textfield',
    '#title' => t('Users'),
    '#autocomplete_path' => FILEPICKER_ADMIN_PATH . '/' . $dir . '/autocomplete',
    '#default_value' => '',
    '#maxlength' => 30,
    '#size' => 25,
  );
  $form['dir'] = array(
    '#type' => 'value',
    '#value' => $dir,
  );
  $form['filepicker_user_search_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );
  return $form;
}

function filepicker_user_search_form_submit($form, &$form_state) {
  if ($form_state['values']['filepicker_currentuser']) {
    $currentuser = $form_state['values']['filepicker_currentuser'];
    if ($form_state['values']['dir'] == 'import') {
      $query = db_select('users', 'u');
      $query->fields('u', array('uid'));
    }
    else {
      $query = db_select('users', 'u');
      $query->fields('u', array('uid'));
      $query->join('filepicker', 'i', 'u.uid = i.uid');
    }
    $query->condition('u.name', $currentuser);
    $row = $query->execute()->fetchAssoc();
    if ($row['uid'] ) {
      filepicker_variable_set('filepicker_currentuser', $row['uid']);
      drupal_goto(FILEPICKER_ADMIN_PATH . '/' . $form_state['values']['dir'] . '/user/' . $row['uid'] . '/list');
    }
    else {
      filepicker_variable_del('filepicker_currentuser');
    }
  }
  else {
    filepicker_variable_del('filepicker_currentuser');
  }
}

function filepicker_check_orphans($dir) {
  $output = array();

  $numericdirlist = file_scan_directory($dir, "/.*/", array('recurse' => FALSE));
  foreach ( $numericdirlist AS $k => $v ) {
    if (is_numeric($numericdirlist[$k]->filename)) {
      $numericdir = $numericdirlist[$k]->filename;
      $query = db_select('users', 'u');
      $query->fields('u', array('uid'));
      $query->condition('u.uid', $numericdir);
      $row = $query->execute()->fetchAssoc();
      if (! $row) {
        $query = db_select('filepicker');
        $query->addExpression('COUNT(file_id)', 'ct');
        $query->condition('uid', $numericdir);
        $row2 = $query->execute()->fetchAssoc();
        if ($row2) {
          if ($row2['ct'] > 0) {
            $output[] = $numericdir;
          }
        }
      }
    }
  }

  if (count($output)) {
    return $output;
  }
  return FALSE;
}

function filepicker_admin_orphans() {
  // show a form if applicable

  $content = drupal_get_form('filepicker_admin_orphans_form');
  return $content;

}

function filepicker_admin_orphans_form($form_state) {

  // show a dropdown of nonexistent user ids
  // two choices:
  // reallocate to another user
  // delete the files

  $dir = filepicker_get_files_directory();
  $orphanids = filepicker_check_orphans($dir);
  if ($orphanids) {
    foreach ($orphanids AS $v) {
      $opt[$v] = $v;
    }
    $form['orphanids'] = array(
      '#type' => 'select',
      '#options' => $opt,
      '#title' => t('Nonexistent user ids'),
      '#description' => t('These ids no longer exist but still have files. Choose what to do below'),
    );
    $form['reallocate_to_user'] = array(
      '#type' => 'textfield',
      '#title' => t('Reallocate to'),
      '#description' => t('Select a user who will own these files.'),
      '#autocomplete_path' => FILEPICKER_ADMIN_PATH . '/orphans/autocomplete',
      '#default_value' => '',
      '#maxlength' => 30,
      '#size' => 25,
    );
    $form['orphans_delete'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete the files'),
      '#return_value' => 1,
    );
    $form['msg'] = array(
      '#markup' => t("Any content using these files will be broken unless removed or edited."),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Go'),
    );
  }
  else {
    $form['orphans_message'] = array(
      '#markup' => t('No orphans found'),
    );
  }
  return $form;
}

function filepicker_admin_orphans_form_submit($form, &$form_state) {

  $result = filepicker_admin_orphans_do($form_state['values']['orphanids'], $form_state['values']['orphans_delete'], $form_state['values']['reallocate_to_user']);
  if (is_array($result)) {
    $message = $result[0];
    $status = $result[1];
    drupal_set_message($message, ($status ? 'status' : 'error'));
  }
  drupal_goto(FILEPICKER_ADMIN_PATH);

}

function filepicker_admin_orphans_do($orphanid, $orphans_delete=FALSE, $reallocate_to_user='') {

  $OK = FALSE;
  $message = '';
  $return = '';
  // check that the incoming user id is nonexistent
  $dir = filepicker_get_files_directory();
  $orphanids = filepicker_check_orphans($dir);
  // sanity check
  if (in_array($orphanid, $orphanids)) {
    // delete files
    if ($orphans_delete == 1) {
      // create 'phony' user object
      $user_obj = new stdClass;
      $user_obj->uid = $orphanid;
      // get all file ids belonging to orphanids
      $ct = 0;
      $result = db_query('SELECT file_id FROM {filepicker} WHERE uid=:uid', array(':uid' => $user_obj->uid));
      while ($row = $result->fetchAssoc()) {
        _filepicker_image_delete($row['file_id'], $user_obj, 'admin', TRUE);
        filepicker_delete_group_image($row['file_id']);
        $ct++;
      }
      $OK = filepicker_delete_olduser_dirs($user_obj->uid);
      if ($OK) {
        $message = t('!ct Files deleted.', array('!ct' => $ct));
      }
      else {
        $message = t('There was an error in deleting files');
      }
    }
    elseif ($reallocate_to_user != '') {
      // reallocate to new user
      $result = db_query("SELECT uid FROM {users} WHERE name=:s", array(':s' => trim($reallocate_to_user)));
      if ($row = $result->fetchAssoc()) {
        $new_uid = $row['uid'];
        $ct = 0;
        $result2 = db_query('SELECT file_id FROM {filepicker} WHERE uid=:uid', array(':uid' => $orphanid));
        while ($row2 = $result2->fetchAssoc()) {
          $newf = filepicker_reallocate_image($row2['file_id'], $orphanid, $new_uid);
          if ($newf) {
            // move this entry in filepicker to $new_uid
            $file_name = basename($newf);
            db_update('filepicker')
              ->fields(array('uid' => $new_uid, 'file_name' => $file_name))
              ->condition('file_id', $row2['file_id'])
              ->execute();
            $ct++;
          }
        }
        // give the new user the groups if any
        db_update('filepicker_user_groups')
          ->fields(array('uid' => $new_uid))
          ->condition('uid', $orphanid)
          ->execute();
        $OK = filepicker_delete_olduser_dirs($orphanid);
        if ($OK) {
          $message = t('!ct Files moved.', array('!ct' => $ct));
        }
        else {
          $message = t('There was an error in moving files.');
        }
      }
    }
    if ($message) {
      $return = array($message, $OK);
    }
  }
  return $return;
}

function filepicker_reallocate_image($file_id, $old_uid, $new_uid) {

  $OK = FALSE;
  // $old_uid might not exist in users table but still have files
  $old_user_obj = new stdClass;
  $old_user_obj->uid = $old_uid;
  $file = _filepicker_get_file($file_id, FALSE, $old_user_obj);
  if ($file) {
    $old_userdir = array('uid' => $old_user_obj->uid);
    $source = filepicker_get_path(FALSE, $old_userdir) . $file->file_name;
    $new_user_obj = user_load($new_uid);
    $new_userdir = array('uid' => $new_user_obj->uid);
    $dest = filepicker_get_path(FALSE, $new_userdir);
    $OK = filepicker_file_unmanaged_move($source, $dest);
  }

  return $OK;
}

function filepicker_delete_olduser_dirs($old_uid) {
  $OK = FALSE;
  // $old_uid might not exist in users table but still have files
  $old_userdir = array('uid' => $old_uid);
  $source = filepicker_get_path(FALSE, $old_userdir);
  $OK = drupal_rmdir($source);
  return $OK;
}

function filepicker_admin_groups() {
  drupal_add_css(FILEPICKER_PATH . '/filepicker.css');
  drupal_add_js(FILEPICKER_PATH . '/filepicker.js');
  module_load_include('inc', 'filepicker', 'filepicker.group');
  $content = '';
  $action = '';
  $rows = NULL;
  $gid = FALSE;
  if (! arg(5) ) {
    filepicker_variable_del('filepicker_currentuser');
    filepicker_variable_set('filepicker_currentgroup', 0);
    $action = 'users';
  }
  $uid = filepicker_variable_get('filepicker_currentuser', '');
  if ( arg(5) == 'user' && is_numeric(arg(6))) {
    $uid = arg(6);
  }
  elseif (arg(5) == 'stats' || arg(5) == 'users') {
    $action = arg(5);
  }
  if ( (arg(7) == 'list' || arg(7) == 'edit' || arg(7) == 'delete') && is_numeric(arg(8)) ) {
    $gidaction = arg(7);
    $gid = arg(8);
    if (arg(7) == 'list') {
      filepicker_variable_set('filepicker_currentgroup', $gid);
      drupal_goto(FILEPICKER_ADMIN_PATH . '/files/user/' . $uid . '/list');
    }
  }
  $name = '';
  if ($uid) {
    $account = user_load($uid);
    $name = $account->name;
  }
  if (filepicker_variable_get('filepicker_groups_enabled', 1)) {
    $content .= render(drupal_get_form('filepicker_group_search_form', 'groups'));
    $content .= '<p>' . t('Managing groups for !n', array('!n' => ($name ? l($name, "user/$uid") : t('All')))) . '</p>';
    if ($uid) {
      // show groups for $uid
      if ($gid) {
        if ($gidaction == 'edit') {
          $record = filepicker_get_user_group($gid);
          $content .= render(drupal_get_form('filepicker_groups_form', $record));
        }
        elseif ($gidaction == 'delete') {
          $content .= render(drupal_get_form('filepicker_group_delete_form', $gid));
        }
      }
      elseif (arg(6) == 'stats') {
        $content .= filepicker_group_stats($account, t('Statistics for %name', array('%name' => $name)));
      }
      else {
        $content .= filepicker_groups_list('admin', $account, t('Groups for %name', array('%name' => $name)));
        $content .= render(drupal_get_form('filepicker_groups_form', 0, $account));
      }
    }
    else {
      if ($action == 'stats') {
        $content .= filepicker_group_stats(FALSE, t('Statistics'));
      }
      else {
        $content .= filepicker_admin_groups_list(t('Groups'));
      }
    }
  }
  return $content;
}

function filepicker_admin_files() {
  drupal_add_css(FILEPICKER_PATH . '/filepicker.css');

  $action = FALSE;
  $fileaction = FALSE;

  $content = '';
  if (! arg(5) ) {
    filepicker_variable_del('filepicker_currentuser');
    filepicker_variable_set('filepicker_currentgroup', 0);
    $action = 'users';
  }
  $uid = filepicker_variable_get('filepicker_currentuser', '');
  if ( arg(5) == 'user' && is_numeric(arg(6))) {
    $uid = arg(6);
  }
  elseif (arg(5) == 'stats' || arg(5) == 'users' || arg(5) == 'list_all') {
    $action = arg(5);
    filepicker_variable_del('filepicker_currentuser');
    filepicker_variable_set('filepicker_currentgroup', 0);
    $uid = filepicker_variable_get('filepicker_currentuser', '');
  }
  if ( arg(7) == 'list_all'
    || arg(7) == 'edit'
    || arg(7) == 'delete'
    || arg(7) == 'list'
    || arg(7) == 'upload'
    || arg(7) == 'stats'
    || arg(7) == 'groups'
    || arg(7) == 'quota') {
    $fileaction = arg(7);
  }
  $file_id = FALSE;
  if ( is_numeric(arg(8))) {
    $file_id = arg(8);
  }
  if (arg(7) == 'upload' ) {
    module_load_include('inc', 'filepicker', 'filepicker.upload');
    if (filepicker_variable_get('filepicker_upload_progress_enabled', 1)) {
      $content .= filepicker_upload_progress_get_script(filepicker_variable_get('filepicker_upload_progress_delay', 0));
    }
  }
  $name = "";
  if ($uid) {
    $account = user_load($uid);
    $name = $account->name;
  }
  $content .= render(drupal_get_form('filepicker_user_search_form', 'files'));
  $content .= '<p>' . t('Managing files for !n', array('!n' => ($name ? l($name, "user/$uid") : t('All')))) . '</p>';

  if ($uid) {
    if ($file_id) {
      if ($fileaction == 'edit' ) {
        module_load_include('inc', 'filepicker', 'filepicker.edit');
        $file =  _filepicker_get_file($file_id, FALSE, $account);
        $filesrc = filepicker_get_file_path($file, array('uid' => $account->uid , 'name' => $account->name) );
        $content .= theme('filepicker_file_edit_header', array('file' => $file, 'source' => $filesrc));
        $content .= render(drupal_get_form('filepicker_edit_form', $file, 'admin', $account));
      }
      elseif ($fileaction == 'delete') {
        filepicker_file_delete($file_id, $account, 'admin');
      }
      else {
        $content .= filepicker_admin_view($file_id, $account);
      }
    }
    elseif ($fileaction == 'upload') {
      $content .= filepicker_quota_ok('admin', $account, t('Upload file for %name', array('%name' => $name)));
    }
    elseif ($fileaction == 'stats') {
      $content .= filepicker_group_stats($account, t('Statistics for %name', array('%name' => $name)));
    }
    elseif ($fileaction == 'list') {
      $content .= _filepicker_list_admin('admin', $account, FALSE, 1, t('List files for %name', array('%name' => $name)));
    }
    elseif ($fileaction == 'quota') {
      $usedbytes = filepicker_get_all_bytes($account)+1;
      $usedbytesprint = _filepicker_bkmg($usedbytes);
      $quota = 0;
      if (filepicker_variable_get('filepicker_quota_enable', 1)) {
        if (filepicker_variable_get('filepicker_quota_byrole', 0)) {
          $roleid = filepicker_variable_get('filepicker_quota_role', 2);
          if (filepicker_user_has_role($roleid, $account) && $account->uid > 1) {
            $quota = filepicker_variable_get('filepicker_quota_default', 0);
          }
        }
        else {
          $quota = (isset($account->filepicker_quota) ? $account->filepicker_quota : filepicker_variable_get('filepicker_quota_default', 0));
        }
      }
      $label = t('Quota for %name', array('%name' => $name));
      $filetot = _filepicker_user_has_file($account);
      $pl = format_plural($filetot, '1 file', '@count files');
      if ($quota) {
        $pused = round($usedbytes / ($quota*1000000) * 100, 2);
        $message = t('User %name has used %pused percent or %usedbytesprint of quota %quotaprint and has %pl.', array('%name' => $name, '%pused' => $pused, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl, '%quotaprint' => filepicker_get_quota_list($quota+1)));
      }
      else {
        $message = t('User %name has used %usedbytesprint and has %pl.', array('%name' => $name, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
      }
      if (! filepicker_variable_get('filepicker_quota_byrole', 0) ) {
        $form = render(drupal_get_form('filepicker_quota_form', $account));
      }
      else {
        $form = "";
      }
      $content .= theme('filepicker_quota', array('quotaform' => $form, 'message' => $message, 'label' => $label));
    }
    elseif ($fileaction == 'groups') {
      drupal_goto(FILEPICKER_ADMIN_PATH . '/groups/user/' . $uid);
    }
    else {
      $content .= _filepicker_list('admin', $account, t('List files for %name', array('%name' => $name)));
    }
  }
  else {
    if ($action == 'stats') {
      $content .= filepicker_group_stats(FALSE, t('Statistics'));
    }
    elseif ($action == 'list_all') {
      $content .= _filepicker_list_admin('admin', FALSE, TRUE, filepicker_variable_get('filepicker_list_public', 0), t('List All Files'));
    }
    else {
      $content .= filepicker_admin_users_list('files', t('Users'));
    }
  }
  return $content;
}

/**
 * Function to display the filepicker group search form
 * @return
 *   Returns the form.
 */
function filepicker_group_search_form($form, &$form_state, $dir = 'groups') {
  $form['filepicker_currentgroup'] = array(
    '#type' => 'textfield',
    '#title' => t('Groups'),
    '#description' => '',
    '#autocomplete_path' => FILEPICKER_ADMIN_PATH . '/groups/autocomplete',
    '#default_value' => '',
    '#maxlength' => 30,
    '#size' => 25,
  );
  $form['dir'] = array(
    '#type' => 'value',
    '#value' => $dir,
  );
  $form['filepicker_group_search_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );
  return $form;
}

function filepicker_group_search_form_submit($form, &$form_state) {
  if ($form_state['values']['filepicker_currentgroup']) {
    $currentgroup = $form_state['values']['filepicker_currentgroup'];
    $query = db_select('filepicker_user_groups', 'iug');
    $query->fields('iug', array('gid', 'uid'));
    $query->condition('iug.group_name', $currentgroup);
    $row = $query->execute()->fetchAssoc();
    if ($row['gid'] ) {
      filepicker_variable_set('filepicker_currentgroup', $row['gid']);
      drupal_goto(FILEPICKER_ADMIN_PATH . '/' . $form_state['values']['dir'] . '/user/' . $row['uid'] . '/edit/' . $row['gid']);
    }
    else {
      filepicker_variable_set('filepicker_currentgroup', 0);
    }
  }
  else {
    filepicker_variable_set('filepicker_currentgroup', 0);
  }
}

/**
 * Function to display the filepicker quota form
 * @return
 *   Returns the form.
 */
function filepicker_quota_form($form, &$form_state, $account=FALSE) {
  $list = filepicker_get_quota_list();
  $default = filepicker_variable_get('filepicker_quota_default', 0);
  $quota = $default;
  $title = t('Set default quota');
  if ($account) {
    $title = t('Set user quota');
    $quota = (filepicker_variable_get('filepicker_quota_default', 0, $account->uid) ? filepicker_variable_get('filepicker_quota_default', 0, $account->uid) : 0);
    if (! $quota) {
      $quota = $default;
    }
  }
  $form['filepicker_quota'] = array(
    '#type' => 'select',
    '#default_value' => $quota,
    '#options' => $list,
    '#title' => $title,
  );
  $form['account'] = array(
    '#type' => 'value',
    '#value' => ($account ? $account->uid : 0),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );
  return $form;
}

function filepicker_quota_form_submit($form, &$form_state) {
    if ( $form_state['values']['account'] > 0 ) {
      filepicker_variable_set('filepicker_quota_default', $form_state['values']['filepicker_quota'], $form_state['values']['account']);
    }
    else {
      filepicker_variable_set('filepicker_quota_default', $form_state['values']['filepicker_quota']);
    }
}

function filepicker_admin_import() {
  drupal_add_css(FILEPICKER_PATH . '/filepicker.css');
  module_load_include('inc', 'filepicker', 'filepicker.import');
  if (! arg(4) ) {
    filepicker_variable_del('filepicker_currentuser');
  }
  $uid = filepicker_variable_get('filepicker_currentuser', '');
  if ( arg(4) == 'user' && is_numeric(arg(5))) {
    $uid = arg(5);
  }
  $content = "";
  if (! $importdir = filepicker_variable_get('filepicker_import_dir', '')) {
    drupal_set_message(t('You need to set the import folder'));
  }
  $content = render(drupal_get_form('filepicker_import_dir_form'));
  if ($importdir) {
    if ($import_files = filepicker_import_dir_check()) {
      $content .= render(drupal_get_form('filepicker_user_search_form', 'import'));
      if ($uid) {
        $account = user_load($uid);
        $content .= render(drupal_get_form('filepicker_import_form', count($import_files), $account, TRUE));
      }
      else {
        drupal_set_message(t('Please select a user. You have %total files to be imported', array('%total' => count($import_files)) ));
      }
    }
  }
  if (isset($_SESSION['filepicker_import_status'])) {
    drupal_get_messages('status', TRUE);
    drupal_set_message($_SESSION['filepicker_import_status']);
    unset($_SESSION['filepicker_import_status']);
  }
  return $content;
}

/**
 * Retrieve a pipe delimited string of autocomplete suggestions for existing users
 */
function filepicker_user_autocomplete($string = '') {

  $matches = array();
  if ($string) {
    if ((arg(4) == 'import') OR (arg(4) == 'orphans')) {
      $query = db_select('users', 'u');
      $query->fields('u', array('uid', 'name'));
      $query->where("LOWER(u.name) LIKE LOWER(:st)", array(':st' => $string . '%') );
      $query->condition('u.status', 1);
      $query->range(0, 10);
    }
    else {
      $query = db_select('users', 'u');
      $query->fields('u', array('name'));
      $query->distinct();
      $query->join('filepicker', 'i', 'u.uid = i.uid');
      $query->where("LOWER(u.name) LIKE LOWER(:st)", array(':st' => $string . '%') );
      $query->condition('u.status', 1);
      $query->range(0, 10);
    }
    $result = $query->execute();
    foreach ($result as $account) {
      if ((arg(4) == 'import') OR (arg(4) == 'orphans')) {
        $user = user_load($account->uid);
        if (user_access('use filepicker', $user)) {
          $matches[$account->name] = check_plain($account->name);
        }
      }
      else {
        $matches[$account->name] = check_plain($account->name);
      }
    }
  }
  drupal_json_output($matches);
}
/**
 * Retrieve a pipe delimited string of autocomplete suggestions for existing groups
 */
function filepicker_group_search_autocomplete($string = '') {

  $matches = array();
  if ($string) {
    $query = db_select('filepicker_user_groups', 'iug');
    $query->fields('iug', array('group_name'));
    $query->where("LOWER(group_name) LIKE LOWER(:st)", array(':st' => $string . '%') );
    $query->range(0, 10);
    $result = $query->execute();
    foreach ($result AS $group) {
      $matches[$group->group_name] = check_plain($group->group_name);
    }
  }
  drupal_json_output($matches);
}

