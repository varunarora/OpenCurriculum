<?php

/**
 * @file
 * Contains the functions pertaining to editing file information
 */

/**
 * Menu callback; fetches the file edit form for filepicker
 */
// iframe
function filepicker_file_edit($file_id) {
  $content = _filepicker_edit_file($file_id, 'iframe');
  return theme('filepicker_iframe', array('content' => $content));
}

// account
function filepicker_user_file_edit($file_id) {
  $content = _filepicker_edit_file($file_id, 'account');
  return $content;
}

function filepicker_edit_form($form, &$form_state, $file, $src='iframe', $account=FALSE) {

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Link text'),
    '#description' => t('Edit link text of your file'),
    '#default_value' => htmlspecialchars_decode($file->file_title),
  );
  if ($account) {
    $form['account'] = array('#type' => 'value', '#value' => $account->uid);
  }
  $form['file_id'] = array('#type' => 'value', '#value' => $file->file_id);
  $form['src'] = array('#type' => 'value', '#value' => $src);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('filepicker_edit_form_process'),
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('filepicker_edit_form_cancel'),
  );
  return $form;
}

/**
 * Submit form functions
 */
function filepicker_edit_form_cancel($form, &$form_state) {
  $file_id = $form_state['values']['file_id'];
  if (isset($form_state['values']['account']) && $form_state['values']['src'] == 'admin') {
    $user = user_load($form_state['values']['account']);
  }
  else {
    global $user;
  }

  if ($form_state['values']['src'] == 'admin') {
    $outpath = FILEPICKER_ADMIN_PATH . '/files/user/' . $user->uid . '/list/' . $file_id;
  }
  elseif ($form_state['values']['src'] == 'account') {
    $outpath = 'user/' . $user->uid . '/filepicker/files/list/' . $file_id;
  }
  else {
    $outpath = 'filepicker/list/' . $file_id;
  }
  drupal_set_message(t('Cancelled.'));
  drupal_goto($outpath);
}


function filepicker_edit_form_process($form, &$form_state) {
  $file_id = $form_state['values']['file_id'];
  $src = $form_state['values']['src'];
  if (isset($form_state['values']['account']) && $src == 'admin') {
    $user = user_load($form_state['values']['account']);
  }
  else {
    global $user;
  }

  if ($src == 'admin') {
    $outpath = FILEPICKER_ADMIN_PATH . '/files/user/' . $user->uid . '/list';
  }
  elseif ($src == 'account') {
    $outpath = 'user/' . $user->uid . '/filepicker/files/list';
  }
  else {
    $outpath = 'filepicker/list';
  }

  $query = db_select('filepicker', 'i');
  $query->fields('i', array('uid', 'file_name'));
  $query->range(0, 1);
  $query->condition('file_id', $file_id);
  $file = $query->execute()->fetchObject();

  if ($file) {
    if ($file->uid == $user->uid) {
      $title = htmlspecialchars($form_state['values']['title']);
      $date = time();
      $num_updated = db_update('filepicker')
        ->fields(array(
          'file_title' => $title,
          'file_date' => $date,
        ))
        ->condition('file_id', $file_id)
        ->execute();
      if ($num_updated) {
        drupal_set_message(t('File was successfully updated.'));
        drupal_goto($outpath);
      }
      else {
        drupal_set_message(t('Error while updating file.'));
      }
    }
    else {
      drupal_set_message(t('This file does not belong to you.'), 'error');
      watchdog('filepicker', 'User uid %d attempted to edit file belonging to user uid %d', array($user->uid, $file->uid), WATCHDOG_WARNING);
    }
  }
  else {
    drupal_set_message(t('File not found.'), 'error');
  }

  drupal_goto($outpath);
}

/**
 * private functions
 */
function _filepicker_edit_file($file_id, $src='iframe', $account=FALSE) {
  $userdir = FALSE;
  if (is_object($account)) {
    $userdir = array('uid' => $account->uid , 'name' => $account->name) ;
  }
  $content = '';
  $file =  _filepicker_get_file($file_id, ($src=='admin'? FALSE: TRUE), $account);
  if ($file) {
    $filesrc = filepicker_get_file_path($file, 'browser', $userdir);
    $content .= theme('filepicker_file_edit_header', array('file' => $file, 'source' => $filesrc));
    $content .= render(drupal_get_form('filepicker_edit_form', $file, $src, $account));
  }
  else {
    drupal_set_message(t('File not found.'), 'error');
    $content = '';
  }

  return $content;
}

