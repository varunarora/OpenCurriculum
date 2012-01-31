<?php

/**
 * @file
 * manage batch imports.
 */

/**
 * Function to check the import folder.
 * @param $account
 *   Optional. user override
 * @return
 *   Returns the number of files.
 */
function filepicker_import_dir_check($checkonly = FALSE) {
  $importok = TRUE;
  if (! $importdir = filepicker_variable_get('filepicker_import_dir', '')) {
    $importok = FALSE;
    $errmsg = t('Import Directory not set');
    $errstatus = 'status';
  }
  elseif (filepicker_variable_get('filepicker_import_delete', 0)) {
    if (! file_prepare_directory($importdir)) {
      $importok = FALSE;
      $errmsg = t('Directory not found or not writable');
      $errstatus = 'error';
    }
  }
  elseif (! file_exists($importdir)) {
    $importok = FALSE;
    $errmsg = t('Directory not found or not readable');
    $errstatus = 'error';
  }
  if (! $importok) {
    drupal_set_message($errmsg, $errstatus);
    return FALSE;
  }

  if ($checkonly) {
    return TRUE;
  }

  // clear out the noisy 'created' messages
  drupal_get_messages('status', TRUE);
  $importlist = file_scan_directory($importdir, "/.*/", array('recurse' => FALSE));
  $checked_importlist = array();
  if (count($importlist)) {
    foreach ($importlist AS $k => $v) {
      $filename = $importlist[$k]->filename;
      if (filepicker_validate_is_file($filename)) {
        $checked_importlist[] = $filename;
      }
    }
  }
  else {
    drupal_set_message(t('No files to import.'), 'warning');
    return FALSE;
  }
  return (count($checked_importlist) > 0 ? $checked_importlist : 0);
}

/**
 * Function to display the file import form.
 *
 * @param $total
 *   Required. total number of files being imported.
 * @param $account
 *   Optional. user override
 * @param $admin
 *   Optional. admin switch
 * @return
 *   Returns the file form.
 */
function filepicker_import_form($form, &$form_state, $total, $account=FALSE, $admin = FALSE) {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $form['total'] = array(
    '#markup' => t('!total files found in the import folder.<br />Selected user is %user', array('!total' => check_plain($total), '%user' => $user->name )),
  );

  // provide checkbox list of groups if any
  if (filepicker_variable_get('filepicker_groups_enabled', 1) && filepicker_variable_get('filepicker_groups_in_upload_enabled', 1)) {
    $grouplist = filepicker_get_groups( ($admin ? $user : FALSE));
    if ($grouplist) {
      $form['grouplist'] = array(
        '#type' => 'checkboxes',
        '#options' => $grouplist,
        '#title' => t('Your Groups'),
        '#description' => t('Select a group to add this import to.'),
      );
    }
  }

  if ($admin) {
    $form['admin'] = array(
      '#type' => 'value',
      '#value' => 1,
    );
  }
  if ($account) {
    $form['account'] = array(
      '#type' => 'value',
      '#value' => $user->uid,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );

  return $form;

}

/**
 * Submit form
 */
function filepicker_import_form_submit($form, &$form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op']: '';
  if ($op == t('Import')) {
    if ( isset($form_state['values']['account']) && isset($form_state['values']['admin'])) {
      $user = user_load($form_state['values']['account']);
    }
    else {
      global $user;
    }

    $destdir = filepicker_get_path(FALSE, (isset($form_state['values']['admin']) ? $user->uid : TRUE));
    $destdirscheme = filepicker_get_path(FALSE, (isset($form_state['values']['admin']) ? $user->uid : TRUE), TRUE);

    if (file_prepare_directory($destdir, FILE_CREATE_DIRECTORY) && $selected = filepicker_import_dir_check()) {
      // clear out the noisy 'created' messages
      drupal_get_messages('status', TRUE);
      // Add DIRECTORY_SEPARATORS here because drupals' functions remove trailing slashes
      $options['destdirscheme'] = $destdirscheme . DIRECTORY_SEPARATOR;
      $options['destdir'] = $destdir . DIRECTORY_SEPARATOR;
      $options['sourcedir'] = filepicker_variable_get('filepicker_import_dir', '');

      $batch = array(
        'title' => t('Importing files'),
        'operations' => array(
          array('filepicker_import_batch', array($form_state, $selected, $options, $user)),
        ),
        'finished' => 'filepicker_import_finished',
        'file' =>  drupal_get_path('module', 'filepicker') . "/filepicker.import.inc",
        'progress_message' => t('Processed @current out of @total.'),
      );
      batch_set($batch);
      // batch_process();
    }
    else {
      drupal_set_message(t('Unable to import.'), 'error');
    }
  }
}

function filepicker_import_batch($form_state, $selected, $options, $account, &$context) {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $max = filepicker_variable_get('filepicker_import_max', 5);
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($selected);
    $context['sandbox']['files'] = $selected;
  }
  // Process files by groups of $max.
  $count = min($max, count($context['sandbox']['files']));
  for ($i = 0; $i < $count; $i++) {
    $filename = array_shift($context['sandbox']['files']);
    // Check the file is OK
    $source_filepath = filepicker_file_check_location($options['sourcedir'] . DIRECTORY_SEPARATOR . $filename, $options['sourcedir']);
    if ($source_filepath &&  filepicker_validate_is_file($source_filepath)) {
      $source = $source_filepath;
      // copy original
      if (filepicker_file_unmanaged_copy($source, FILEPICKER_FILE_SCHEME . $options['destdirscheme'])) {
        $file = basename($source);
        $orig = $options['destdir'] . $file;
        $nextfileid = filepicker_insert_file($user->uid, $file);
        if ($nextfileid) {
          if ( is_array($form_state['values']['grouplist']) && filepicker_variable_get('filepicker_groups_enabled', 1) && filepicker_variable_get('filepicker_groups_in_upload_enabled', 1) ) {
            $query = db_select('filepicker', 'i');
            $query->fields('i', array('file_id') );
            $query->condition('uid', $user->uid)->condition('file_name', $file);
            $record = $query->execute()->fetchObject();
            foreach ($form_state['values']['grouplist'] AS $gid) {
              $record->gid = $gid;
              filepicker_insert_group_file($record);
            }
          }
          // delete
          if (filepicker_variable_get('filepicker_import_delete', 0)) {
            file_unmanaged_delete($source_filepath);
          }
        }
        else {
          file_unmanaged_delete($options['destdir'] . $file);
          $_SESSION['filepicker_import_status'] = t('Error while saving information to database for imported file %im.', array('%im' => $file));
        }
      }
      else {
        $_SESSION['filepicker_import_status'] = t('Could not copy to original: %file', array('%file' => $source_filepath));
      }
    }
    else {
      $_SESSION['filepicker_import_status'] = t('Invalid file: %file', array('%file' => $source_filepath));
    }
    // Update our progress information.
    $context['sandbox']['progress']++;
    $context['results'][] = $source_filepath;
    if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
      $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
    }
    else {
      $context['finished'] = 1;
    }
  }
}

function filepicker_import_finished($success, $results, $operations) {
  if ($success) {
    $message = format_plural(count($results), 'One file processed.', '@count files processed.');
    filepicker_variable_del('filepicker_import_dir');

  }
  else {
    $message = t('Finished with an error.');
  }
  $_SESSION['filepicker_import_status'] = $message;
}

/**
 * Function to display the import folder form
 *
 * @return
 *   Returns the import folder form.
 */
function filepicker_import_dir_form($form, &$form_state) {
  $readstate = t('readable');
  if (filepicker_variable_get('filepicker_import_delete', 0)) {
    $readstate = t('writeable');
  }

  $form['filepicker_import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filepicker Import Folder'),
    '#description' => t('Path of the import folder, relative to the Drupal root or an absolute path.<br />This is where you upload files to, via FTP or rsync.<br />This folder must be !readstate by the webserver.', array('!readstate' => $readstate)),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['filepicker_import']['filepicker_import_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Import Folder'),
    '#size' => 55,
    '#maxlength' => 100,
    '#required' => FALSE,
    '#default_value' => filepicker_variable_get('filepicker_import_dir', ''),
  );
  $form['filepicker_import']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['filepicker_import']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
  );

  return $form;
}

/**
 * Validate form
 */
function filepicker_import_dir_form_validate($form, &$form_state) {
  $importdir = $form_state['values']['filepicker_import_dir'];
  if ($form_state['values']['op'] == t('Save')) {
    if (filepicker_variable_get('filepicker_import_delete', 0)) {
      if (! file_prepare_directory($importdir)) {
        form_set_error('filepicker_import_dir', t('Directory not found or not writable'));
      }
    }
    elseif (! file_exists($importdir)) {
      form_set_error('filepicker_import_dir', t('Directory not found or not readable'));
    }
  }
}

/**
 * Submit form
 */
function filepicker_import_dir_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == t('Save')) {
    filepicker_variable_set('filepicker_import_dir', $form_state['values']['filepicker_import_dir']);
  }
  else {
    filepicker_variable_del('filepicker_import_dir');
  }
}

// D7 has lost file_check_location()
function filepicker_file_check_location($source, $directory = '') {
  $check = realpath($source);
  if ($check) {
    $source = $check;
  }
  else {
    // This file does not yet exist
    $source = realpath(dirname($source)) . '/' . basename($source);
  }
  $directory = realpath($directory);
  if ($directory && strpos($source, $directory) !== 0) {
    return 0;
  }
  return $source;
}
