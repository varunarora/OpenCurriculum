<?php

/**
 * @file
 * upload functions
 */

/**
 * Menu callback; presents the upload form for filepicker
 */
// iframe
function filepicker_upload() {
  filepicker_variable_del('filepicker_advanced_browser_pagestart');
  if (filepicker_variable_get('filepicker_upload_progress_enabled', 1)) {
    $content = filepicker_upload_progress_get_script(filepicker_variable_get('filepicker_upload_progress_delay', 0));
  }
  $content .= filepicker_quota_ok('iframe', FALSE, '', t('Upload files. You can give them a title and description'));
  return theme('filepicker_iframe', array('content' => $content));
}
// account
function filepicker_user_upload() {
  global $user;
  filepicker_variable_del('filepicker_advanced_browser_pagestart');
  if (filepicker_variable_get('filepicker_upload_progress_enabled', 1)) {
    $content = filepicker_upload_progress_get_script(filepicker_variable_get('filepicker_upload_progress_delay', 0));
  }
  $content .= filepicker_quota_ok('user', $user, '', t('Upload files. You can give them a Link text'));
  return $content;
}

function filepicker_upload_form($form, &$form_state, $account=FALSE, $admin = FALSE) {
  // multiple uploads
  $max_uploads = filepicker_variable_get('filepicker_max_uploads', 1);
  $settings = array('filepicker_upload_link' => array('max_uploads' => $max_uploads));
  drupal_add_js(FILEPICKER_PATH . '/filepicker_upload_link.js');
  drupal_add_js($settings, 'setting');

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $form['#attributes']['enctype'] = 'multipart/form-data';
  if (filepicker_variable_get('filepicker_upload_progress_enabled', 1) && filepicker_variable_get('filepicker_uploadprogress_server', '')) {
    $form[FILEPICKER_UPLOAD_ID] = array(
      '#type' => 'hidden',
      '#value' => uniqid(),
      '#weight' => -50,
    );
  }
  $form['maxmsg'] = array(
    '#markup' => '',
  );
  for ($ct = 1; $ct <= $max_uploads; $ct++) {
    $form['file_upload_' . $ct] = array(
      '#type' => 'file',
      '#title' => ($max_uploads == 1 ? t('File') : t('File %ct', array('%ct' => $ct))),
      '#required' => ($ct == 1 ? TRUE : FALSE),
      '#value' => 1,
      '#size' => 50,
    );
    $form['title_' . $ct] = array(
      '#type' => 'textfield',
      '#title' => ($max_uploads == 1 ? t('Link text') : t('Link text for File %ct', array('%ct' => $ct))),
      '#description' => t('The text to use for a link. The default is the filename'),
      '#size' => 50,
    );
    if ($ct < $max_uploads) {
      $form['link_' . $ct] = array(
        '#markup' => '',
      );
    }
  }

  // groups
  if (filepicker_variable_get('filepicker_groups_enabled', 1) && filepicker_variable_get('filepicker_groups_in_upload_enabled', 1)) {
    $grouplist = filepicker_get_groups( ($admin ? $user : FALSE));
    if ($grouplist) {
      $form['grouplist'] = array(
        '#type' => 'checkboxes',
        '#options' => $grouplist,
        '#title' => t('Your Groups'),
        '#description' => t('Select a group to add your file to.'),
      );
    }
  }

  if ($account) {
    $form['account'] = array(
      '#type' => 'value',
      '#value' => $user->uid,
    );
  }
  if ($admin) {
    $form['admin'] = array(
      '#type' => 'value',
      '#value' => 1,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Upload'),
  );
  if (filepicker_variable_get('filepicker_upload_progress_enabled', 1)) {
    $form['#suffix'] = filepicker_upload_progress_busy();
  }
  return $form;
}

/**
 * Validate form
 */
function filepicker_upload_form_validate($form, &$form_state) {

  $max_uploads = variable_get('filepicker_max_uploads', 1);
  for ($ct = 1; $ct <= $max_uploads; $ct++) {
    foreach ($form_state['values'] as $name => $value) {
      if ($ct == 1 && $name == 'file_upload_' . $ct) {
        if (empty($_FILES['files']['name'][$name])) {
          form_set_error($name, t('File upload field is required.'));
        }
        elseif (!isset($_FILES['files']['tmp_name'][$name]) || !file_exists($_FILES['files']['tmp_name'][$name])) {
          form_set_error($name, t('Error while uploading file.'));
        }
        elseif (!filepicker_get_uploaded_file_extension($name)) {
          form_set_error($name, t('Illegal file.'));
        }
      }
      else {
        if ($name == 'file_upload_' . $ct && !empty($_FILES['files']['name'][$name])) {
          if (!isset($_FILES['files']['tmp_name'][$name]) || !file_exists($_FILES['files']['tmp_name'][$name])) {
            form_set_error($name, t('Error while uploading file.'));
          }
          elseif (!filepicker_get_uploaded_file_extension($name)) {
            form_set_error($name, t('Illegal file.'));
          }
        }
      }
    }
  }
}

/**
 * Submit form
 */
function filepicker_upload_form_submit($form, &$form_state) {
  $success = 0;
  $errormsg = '';
  if ($form_state['values']['op'] == t('Upload')) {
    if ( isset($form_state['values']['account']) && isset($form_state['values']['admin'])) {
      $user = user_load($form_state['values']['account']);
    }
    else {
      global $user;
    }

    $destdir = filepicker_get_path(FALSE, (isset($form_state['values']['admin']) ? array('uid' => $user->uid) : TRUE));
    $destdirscheme = filepicker_get_path(FALSE, (isset($form_state['values']['admin']) ? array('uid' => $user->uid) : TRUE), TRUE);

    if (file_prepare_directory($destdir, FILE_CREATE_DIRECTORY)) {
      // clear out the noisy 'created' messages
      drupal_get_messages('status', TRUE);
      $max_uploads = filepicker_variable_get('filepicker_max_uploads', 1);
      for ($ct = 1; $ct <= $max_uploads; $ct++) {
        $destination = $destdirscheme;
        foreach ($form_state['values'] as $name => $value) {
          if ($name == 'file_upload_' . $ct && !empty($_FILES['files']['name'][$name])) {
            $filemoved = filepicker_copy_uploaded_file($destination, $name);
            if ($filemoved) {
              $file = $filemoved;
              $title = htmlspecialchars($form_state['values']['title_' . $ct]);
              $nextfileid = filepicker_insert_file($user->uid, $file, $title);
              if ($nextfileid) {
                if ( isset($form_state['values']['grouplist']) && is_array($form_state['values']['grouplist']) && filepicker_variable_get('filepicker_groups_enabled', 1) && filepicker_variable_get('filepicker_groups_in_upload_enabled', 1) ) {
                  $query = db_select('filepicker', 'i');
                  $query->fields('i', array('file_id') );
                  $query->condition('uid', $user->uid)->condition('file_name', $file);
                  $record = $query->execute()->fetchObject();
                  foreach ($form_state['values']['grouplist'] AS $gid) {
                    $record->gid = $gid;
                    filepicker_insert_group_file($record);
                  }
                }
                // external functions
                #module_invoke_all('filepicker_upload_extra', $nextfileid);
                $success++;
              }
              else {
                file_unmanaged_delete(FILEPICKER_FILE_SCHEME . $destination . $file);
                $errormsg = t('Error while saving information to database for uploaded file.');
              }
            }
            else {
              $errormsg = t('Error while moving uploaded file %ct to its destination.', array('%ct' => $ct));
            }
          }
        }
      }
    }
    else {
      $errormsg = t('Unable to create a directory structure for your files.');
    }
  }
  if ($success) {
    drupal_set_message(t('%c successfully uploaded.', array('%c' => format_plural($success, '1 file', '@count files'))));
    if (isset($form_state['values']['admin'])) {
      $outpath = FILEPICKER_ADMIN_PATH . '/files/user/' . $user->uid . '/list';
    }
    elseif (isset($form_state['values']['account'])) {
      $outpath = 'user/' . $user->uid . '/filepicker/files/list';
    }
    else {
      $outpath = 'filepicker/list' . ($success == 1 ? '/' . $nextfileid : '');
    }
    drupal_goto($outpath);
  }
  else {
    if (! $errormsg) {
      $max_filesize = ini_get('upload_max_filesize');
      $errormsg = t('No files uploaded. This may be because the files totalled more than %c', array('%c' => $max_filesize));
    }
    drupal_set_message($errormsg, 'error');
  }
}

function filepicker_copy_uploaded_file($destination, $name) {

  if ($name && isset($_FILES['files']['tmp_name'][$name]) ) {
    $source = $_FILES['files']['tmp_name'][$name];

    $newsrc = filepicker_file_unmanaged_copy($source, FILEPICKER_FILE_SCHEME . $destination);
    if ($newsrc) {
      $file = filepicker_get_uploaded_file_name($destination, $name);
      $destination .= $file;
      $file_moved = filepicker_file_unmanaged_move($newsrc, FILEPICKER_FILE_SCHEME . $destination);
      // might be renamed if a file of same name already exists
      $file_moved = drupal_substr($file_moved, strrpos($file_moved, DIRECTORY_SEPARATOR)+1);
      return $file_moved;
    }
  }
  return FALSE;
}

function filepicker_get_uploaded_file_extension($name) {
  $filename = $_FILES['files']['name'][$name];
  return filepicker_validate_is_file($filename);
}

function filepicker_get_uploaded_file_name($destination, $name) {
  $fileext = filepicker_get_uploaded_file_extension($name);

  if (FALSE !== strpos($_FILES['files']['name'][$name], '.')) {
    $filename = drupal_substr($_FILES['files']['name'][$name], 0, strrpos($_FILES['files']['name'][$name], '.'));
  }
  else {
    $filename = $_FILES['files']['name'][$name];
  }

  $file = $filename . '.' . $fileext;
  $i = 0;
  while (file_exists($destination . $file)) {
    $i++;
    $file = $filename . '_' . $i . '.' . $fileext;
  }
  return $file;
}

function filepicker_upload_progress_get_script($delay) {

  $message = filepicker_variable_get('filepicker_upload_progress_message', t('Processing form... please wait.'));
  if (filepicker_variable_get('filepicker_uploadprogress_server', '')) {
    // Immediately disable devel shutdown functions so that it doesn't botch our
    // JSON output.
    $GLOBALS['devel_shutdown'] = FALSE;
    $settings = array(
      'filepicker_upload_progress' => array(
        'callback' => 'http://' . $_SERVER['HTTP_HOST'] . url('filepicker/progress'),
        'interval' => filepicker_variable_get('filepicker_uploadprogress_interval', 1),
        'name' => FILEPICKER_UPLOAD_ID,
        'delay' => $delay,
        'initmessage' => $message,
        'cancelmessage' => t('Cancel'),
      ),
    );
  }
  else {
    $settings = array(
      'filepicker_upload_progress' => array(
        'callback' => '',
        'interval' => '',
        'name' => '',
        'delay' => $delay,
        'initmessage' => $message,
        'cancelmessage' => t('Cancel'),
      ),
    );
  }

  drupal_add_js(FILEPICKER_PATH . '/filepicker_upload.js');
  drupal_add_js($settings, 'setting');
}

function filepicker_upload_progress_busy() {
  return '<div id="filepicker-sending" style="display: none;"></div>';
}

