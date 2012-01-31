<?php

/**
 * @file
 * Contains the functions pertaining to editing image information
 */

/**
 * Menu callback; fetches the image edit form for imagepicker
 */
// iframe
function imagepicker_image_edit($img_id) {

  $content = _imagepicker_edit_img($img_id, 'iframe');
  return theme('imagepicker_iframe', array('content' => $content));
}

// account
function imagepicker_user_image_edit($img_id) {

  $content = _imagepicker_edit_img($img_id, 'account');
  return $content;
}

function imagepicker_edit_form($form, &$form_state, $img, $src='iframe', $account=FALSE) {

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Edit title of your image'),
    '#default_value' => htmlspecialchars_decode($img->img_title),
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 2,
    '#cols' => 80,
    '#description' => t('Edit description of your image'),
    '#default_value' => htmlspecialchars_decode($img->img_description),
  );
  if ($account) {
    $form['account'] = array(
      '#type' => 'value',
      '#value' => $account->uid,
    );
  }
  $form['img_id'] = array(
      '#type' => 'value',
      '#value' => $img->img_id,
    );
  $form['src'] = array(
      '#type' => 'value',
      '#value' => $src,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('imagepicker_edit_form_process'),
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('imagepicker_edit_form_cancel'),
  );
  return $form;
}

/**
 * Submit form functions
 */
function imagepicker_edit_form_cancel($form, &$form_state) {

  $img_id = $form_state['values']['img_id'];
  if (isset($form_state['values']['account']) && $form_state['values']['src'] == 'admin') {
    $user = user_load($form_state['values']['account']);
  }
  else {
    global $user;
  }

  if ($form_state['values']['src'] == 'admin') {
    $outpath = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $user->uid . '/browse/' . $img_id;

  }
  elseif ($form_state['values']['src'] == 'account') {
    $outpath = 'user/' . $user->uid . '/imagepicker/images/browse/' . $img_id;
  }
  else {
    $outpath = 'imagepicker/browse/' . $img_id;
  }
  drupal_set_message(t('Cancelled.'));
  drupal_goto($outpath);
}


function imagepicker_edit_form_process($form, &$form_state) {

  $img_id = $form_state['values']['img_id'];
  $src = $form_state['values']['src'];
  if (isset($form_state['values']['account']) && $src == 'admin') {
    $user = user_load($form_state['values']['account']);
  }
  else {
    global $user;
  }

  if ($src == 'admin') {
    $outpath = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $user->uid . '/browse/' . $img_id;
  }
  elseif ($src == 'account') {
    $outpath = 'user/' . $user->uid . '/imagepicker/images/browse/' . $img_id;
  }
  else {
    $outpath = 'imagepicker/browse/' . $img_id;
  }

  $query = db_select('imagepicker', 'i');
  $query->fields('i', array('uid', 'img_name'));
  $query->range(0, 1);
  $query->condition('img_id', $img_id);
  $img = $query->execute()->fetchObject();

  if ($img) {
    if ($img->uid == $user->uid) {
      $title = htmlspecialchars($form_state['values']['title']);
      $description = htmlspecialchars($form_state['values']['description']);
      $date = time();

      $num_updated = db_update('imagepicker')
        ->fields(array(
          'img_title' => $title,
          'img_description' => $description,
          'img_date' => $date,
        ))
        ->condition('img_id', $img_id)
        ->execute();

      if ($num_updated) {
        drupal_set_message(t('Image was successfully updated.'));
        drupal_goto($outpath);
      }
      else {
        drupal_set_message(t('Error while updating image.'));
      }
    }
    else {
      drupal_set_message(t('This image does not belong to you.'), 'error');
      watchdog('imagepicker', 'User uid %d attempted to edit image belonging to user uid %d', array($user->uid, $img->uid), WATCHDOG_WARNING);
    }
  }
  else {
    drupal_set_message(t('Image not found.'), 'error');
  }

  drupal_goto($outpath);
}

/**
 * private functions
 */
function _imagepicker_edit_img($img_id, $src='iframe', $account=FALSE) {

  $userdir = FALSE;
  if (is_object($account)) {
    $userdir = array('uid' => $account->uid) ;
  }
  $content = '';
  $img =  _imagepicker_get_img($img_id, ($src=='admin'? FALSE: TRUE), $account);
  if ($img) {
    $imgsrc = imagepicker_get_image_path($img, 'browser', $userdir);
    $content .= theme('imagepicker_image_edit_header', array('image' => $img, 'source' => $imgsrc));
    $content .= render(drupal_get_form('imagepicker_edit_form', $img, $src, $account));
  }
  else {
    drupal_set_message(t('Image not found.'), 'error');
    $content = '';
  }

  return $content;
}

