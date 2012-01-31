<?php

/**
 * @file
 * my imagepicker in my account
 */
function imagepicker_user_page($uid, $path="", $func="", $id=0) {

  drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');
  drupal_add_js(IMAGEPICKER_PATH . '/imagepicker.js');
  $content = '';
  // path/func/id
  if ($path == 'images') {
    if ( ($func == 'browse' || $func == 'browseadmin') && is_numeric($id) && $id > 0) {
      $content .= imagepicker_user_view($id);
    }
    elseif ($func == 'edit' && is_numeric($id) && $id > 0) {
      module_load_include('inc', 'imagepicker', 'imagepicker.edit');
      $content .= imagepicker_user_image_edit($id);
    }
    elseif ($func == 'browse') {
      $content .= imagepicker_user_browse();
    }
    elseif ($func == 'browse_public') {
      if (is_numeric($id) && $id > 0) {
        $content .= imagepicker_user_view_public($id);
      }
      else {
        $content .= imagepicker_user_browse_public();
      }
    }
    elseif ($func == 'browseadmin') {
      $content .= imagepicker_user_browse_admin();
    }
    elseif ($func == 'delete' && is_numeric($id) && $id > 0) {
      imagepicker_image_delete($id, FALSE, 'account');
    }
  }
  elseif (imagepicker_variable_get('imagepicker_groups_enabled', 1) && $path == 'groups') {
    module_load_include('inc', 'imagepicker', 'imagepicker.group');
    if ($func == 'edit' && is_numeric($id) && $id > 0) {
      $content .= imagepicker_user_groups($func, $id);
    }
    elseif ($func == 'browse' && is_numeric($id) && $id > 0) {
      imagepicker_set_user_group_state(1, $id);
      $content .= imagepicker_user_browse();
    }
    elseif ($func == 'browse') {
      $content .= imagepicker_user_groups();
    }
    elseif ($func == 'delete') {
      $content .= render(drupal_get_form('imagepicker_group_delete_form', $id));
    }
  }
  elseif ($path == 'stats') {
    $content .= imagepicker_group_stats(-1);
  }
  elseif ($path == 'config') {
    $content .= imagepicker_user_config_admin();
  }
  else {
    module_load_include('inc', 'imagepicker', 'imagepicker.upload');
    $content .= imagepicker_user_upload();
  }
  return $content;
}

/**
 * main thumbnails page in my imagepicker
 */
function imagepicker_user_browse() {

  $content = _imagepicker_browse("account");
  return $content;
}

function imagepicker_user_browse_public() {

  $content = _imagepicker_browse_public("account", 1);
  return $content;
}

function imagepicker_user_browse_admin() {

  $content = _imagepicker_browse_admin("account");
  return $content;
}

function imagepicker_user_view($img_id) {

  drupal_add_js(IMAGEPICKER_PATH . '/imagepicker.js');
  $form1 = '';
  $form2 = '';
  $form3 = '';

  $img = _imagepicker_get_img($img_id);
  if ($img) {
    $imgpath = imagepicker_get_image_path($img, 'full');
    $info = image_get_info(imagepicker_get_path(FALSE, TRUE) . $img->img_name);
    $exifinfo = "";
    if ($info['extension'] != 'gif' ) {
      $exifinfo = imagepicker_get_exifinfo(imagepicker_get_path(FALSE, TRUE) . $img->img_name);
    }
    $view = theme('imagepicker_view', array('img' => $img, 'imgpath' => $imgpath, 'info' => $info, 'exifinfo' => $exifinfo));
    $form1 = render(drupal_get_form('imagepicker_user_image_form', $img_id));
    // copy form
    if (imagepicker_variable_get('imagepicker_copy_enabled', 0)) {
      $form2 = render(drupal_get_form('imagepicker_copy_form', $img->img_id, $img->img_name));
    }
    // group form
    global $user;
    if (_imagepicker_has_groups($user)) {
      $form3 = render(drupal_get_form('imagepicker_group_images_form', $img->img_id));
    }
    $content = theme('imagepicker_user_view', array('view' => $view, 'form1' => $form1, 'form2' => $form2, 'form3' => $form3));
  }
  else {
    drupal_set_message(t('Image not found.'), 'error');
    $content = '';
  }
  return $content;
}

function imagepicker_user_view_public($img_id) {

  $img = _imagepicker_get_img($img_id, FALSE);
  if ($img) {
    $imgpath = imagepicker_get_image_path($img, 'full', array('uid' => $img->uid ));
    $info = image_get_info(imagepicker_get_path(FALSE, array('uid' => $img->uid)) . $img->img_name);
    $content = theme('imagepicker_view', array('img' => $img, 'imgpath' => $imgpath, 'info' => $info));
  }
  else {
    drupal_set_message(t('Image not found.'), 'error');
    $content = '';
  }
  return $content;
}

function imagepicker_user_image_form($form, &$form_state, $img_id) {

  $form['edit'] = array(
    '#type' => 'submit',
    '#value' => t('Edit image'),
    '#submit' => array('imagepicker_user_image_form_edit'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete image'),
    '#submit' => array('imagepicker_user_image_form_delete'),
  );
  $form['img_id'] = array(
    '#type' => 'value',
    '#value' => $img_id,
  );
  return $form;
}

function imagepicker_user_image_form_delete($form, &$form_state) {

  imagepicker_image_delete($form_state['values']['img_id'] , FALSE, 'account');
}

function imagepicker_user_image_form_edit($form, &$form_state) {

  global $user;
  drupal_goto('user/' . $user->uid . '/imagepicker/images/edit/' . $form_state['values']['img_id']);
}

/**
 * Menu callback; presents the configuration settings form for imagepicker user.
 */
function imagepicker_user_config_admin() {

  $content = render(drupal_get_form('imagepicker_user_config_admin_form'));
  return theme('imagepicker_user_config', array('form' => $content, 'label' => '', 'help' => t('Imagepicker configuration page.'), 'message1' => '', 'message2' => ''));
}

/**
 * Submit form
 */
function imagepicker_user_config_admin_form($form, &$form_state, $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $form['#attributes']['enctype'] = 'multipart/form-data';

  // default options
  $form['imagepicker_defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set imagepicker defaults'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -38,
  );
  $form['imagepicker_defaults']['imagepicker_default_thumbnail_size'] = imagepicker_thumbnail_size_elem(imagepicker_variable_get('imagepicker_default_thumbnail_size', NULL, $user->uid));
  $form['imagepicker_defaults']['imagepicker_default_scale'] = imagepicker_scale_elem(imagepicker_variable_get('imagepicker_default_scale', NULL, $user->uid));
  $form['imagepicker_defaults']['imagepicker_default_pagelink'] = imagepicker_pagelink_elem(imagepicker_variable_get('imagepicker_default_pagelink', NULL, $user->uid));
  $form['imagepicker_defaults']['imagepicker_rows_per_page'] = imagepicker_rows_per_page_elem(imagepicker_variable_get('imagepicker_rows_per_page', NULL, $user->uid));

  // insert settings
  $form['imagepicker_insert_defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set imagepicker insert defaults'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -36,
  );
  if (imagepicker_variable_get('imagepicker_default_align_show', 1)) {
    $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_align'] = imagepicker_get_align_opts(imagepicker_variable_get('imagepicker_insert_defaults_align', NULL, $user->uid));
  }
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_show'] = imagepicker_get_show_opts(imagepicker_variable_get('imagepicker_insert_defaults_show', NULL, $user->uid));
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_link'] = imagepicker_get_link_opts(imagepicker_variable_get('imagepicker_insert_defaults_link', NULL, $user->uid));

  if (imagepicker_variable_get('imagepicker_default_align_show', 1)) {
    // alignment settings
    $form['imagepicker_align'] = array(
      '#type' => 'fieldset',
      '#title' => t('Image alignment settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => -34,
    );
    $form['imagepicker_align']['imagepicker_default_fleft'] = imagepicker_fleft_elem(imagepicker_variable_get('imagepicker_default_fleft', NULL, $user->uid));
    $form['imagepicker_align']['imagepicker_default_fright'] = imagepicker_fright_elem(imagepicker_variable_get('imagepicker_default_fright', NULL, $user->uid));
  }

  // browser
  $form['imagepicker_browser'] = array(
    '#type' => 'fieldset',
    '#title' => t('Imagepicker browser settings'),
    '#description' => t('Configure browser.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -20,
  );
  $form['imagepicker_browser']['imagepicker_default_browser_thumbnail_size'] = imagepicker_browser_thumbnail_size_elem(imagepicker_variable_get('imagepicker_default_browser_thumbnail_size', NULL, $user->uid));
  $form['imagepicker_browser']['imagepicker_advanced_browser_columns'] = imagepicker_browser_columns_elem(imagepicker_variable_get('imagepicker_advanced_browser_columns', NULL, $user->uid));
  $form['imagepicker_browser']['imagepicker_advanced_browser_page'] = imagepicker_browser_page_elem(imagepicker_variable_get('imagepicker_advanced_browser_page', NULL, $user->uid));
  if (imagepicker_variable_get('imagepicker_show_browse_order_form', 1)) {
    $form['imagepicker_browser']['imagepicker_show_browse_order_form'] = imagepicker_show_browse_order_form_elem(imagepicker_variable_get('imagepicker_show_browse_order_form', NULL, $user->uid));
  }
  $form['imagepicker_browser']['imagepicker_default_browser_order'] = imagepicker_browser_order_elem(imagepicker_variable_get('imagepicker_default_browser_order', NULL, $user->uid));
  if (imagepicker_variable_get('imagepicker_show_browse_search_form', 1)) {
    $form['imagepicker_browser']['imagepicker_show_browse_search_form'] = imagepicker_show_browse_search_form_elem(imagepicker_variable_get('imagepicker_show_browse_search_form', NULL, $user->uid));
  }

  if ( imagepicker_image_check_functions(TRUE)
    && imagepicker_variable_get('imagepicker_watermark_enable', 0)
    && ! imagepicker_variable_get('imagepicker_watermark_image', '')
  ) {

    $desc = "";
    $wfiles = imagepicker_get_watermarks($user);
    $wf = array();
    if ($wfiles) {
      foreach ($wfiles AS $k => $v) {
        $wf[$v] = "$v";
      }
    }
    else {
      $desc = t('You have no uploaded watermark images');
    }
    $form['imagepicker_watermark'] = array(
      '#type' => 'fieldset',
      '#title' => t('Image watermark settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['imagepicker_watermark']['imagepicker_watermark_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable watermark'),
      '#description' => $desc,
      '#default_value' => imagepicker_variable_get('imagepicker_watermark_enable', FALSE, $user->uid),
      '#return_value' => 1,
    );

    if ($wf) {
      $form['imagepicker_watermark']['imagepicker_watermark_image'] = array(
      '#type' => 'select',
      '#default_value' => imagepicker_variable_get('imagepicker_watermark_image', '', $user->uid),
      '#options' => $wf,
      '#title' => t('Select Watermark image'),
      );
      $form['imagepicker_watermark']['imagepicker_watermark_image_delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('imagepicker_user_config_admin_form_delete'),
      );
    }
    $form['imagepicker_watermark']['file_upload'] = array(
      '#type' => 'file',
      '#title' => t('Upload Watermark Image'),
      '#description' => t('Browse your computer for watermark image file'),
      '#required' => FALSE,
      '#value' => 1,
      '#size' => 40,
    );
    $form['imagepicker_watermark']['imagepicker_watermark_position'] = array(
      '#type' => 'select',
      '#default_value' => imagepicker_variable_get('imagepicker_watermark_position', 0, $user->uid),
      '#options' =>  imagepicker_watermark_opts(),
      '#title' => t('Select watermark position'),
    );
    $form['imagepicker_watermark']['imagepicker_watermark_perc'] = array(
      '#type' => 'textfield',
      '#title' => t('JPEG quality'),
      '#size' => 10,
      '#maxlength' => 3,
      '#default_value' => (imagepicker_variable_get('imagepicker_watermark_perc', variable_get('image_jpeg_quality', 75), $user->uid)),
      '#description' => t('Define the image quality for JPEG manipulations. Ranges from 0 to 100. Higher values mean better image quality but bigger files.'),
    );
    $form['imagepicker_watermark']['imagepicker_watermark_use'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use watermark by default'),
      '#description' => t('Check the "Use watermark" checkbox on the upload form by default'),
      '#default_value' => imagepicker_variable_get('imagepicker_watermark_use', FALSE, $user->uid),
      '#return_value' => 1,
    );

    // watermark scaling
    $form['imagepicker_watermark']['imagepicker_user_watermark'] = array(
      '#type' => 'fieldset',
      '#title' => t('Watermark scaling'),
      '#collapsible' => TRUE,
      '#collapsed' => (imagepicker_variable_get('imagepicker_watermark_scaling', FALSE, $user->uid) ? TRUE : FALSE),
    );
    $form['imagepicker_watermark']['imagepicker_user_watermark']['imagepicker_watermark_scaling'] = imagepicker_watermark_scaling_elem(imagepicker_variable_get('imagepicker_watermark_scaling', FALSE, $user->uid));
    $form['imagepicker_watermark']['imagepicker_user_watermark']['imagepicker_watermark_scaling_percentage'] = imagepicker_watermark_scaling_percentage_elem(imagepicker_variable_get('imagepicker_watermark_scaling_percentage', 50, $user->uid));
    $form['imagepicker_watermark']['imagepicker_user_watermark']['imagepicker_watermark_min_width'] = imagepicker_watermark_min_width_elem(imagepicker_variable_get('imagepicker_watermark_min_width', 0, $user->uid));
  }

  if (module_exists('lightbox2') && imagepicker_variable_get('imagepicker_lightbox2_enable', 1)) {
    // Iframe
    $form['imagepicker_lightbox2'] = array(
      '#type' => 'fieldset',
      '#title' => t('Imagepicker Lightbox2 settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => -23,
    );
    $form['imagepicker_lightbox2']['imagepicker_lightbox2_insert'] = imagepicker_lightbox2_insert_elem(imagepicker_variable_get('imagepicker_lightbox2_insert', NULL, $user->uid));
  }

  // Iframe
  $form['imagepicker_iframe'] = array(
    '#type' => 'fieldset',
    '#title' => t('Imagepicker Iframe settings'),
    '#description' => t('You can alter the default styling of the iframe here,<br />useful if the iframe is not fitting in with your theme.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -22,
  );

  if (module_exists('colorbox') && variable_get('colorbox_load', 0)) {
    $form['imagepicker_iframe']['imagepicker_advanced_colorbox'] = imagepicker_colorbox_elem(imagepicker_variable_get('imagepicker_advanced_colorbox', NULL, $user->uid));
  }
  $form['imagepicker_iframe']['imagepicker_advanced_iframe_width'] = imagepicker_iframe_width_elem(imagepicker_variable_get('imagepicker_advanced_iframe_width', NULL, $user->uid));
  $form['imagepicker_iframe']['imagepicker_advanced_iframe_height'] = imagepicker_iframe_height_elem(imagepicker_variable_get('imagepicker_advanced_iframe_height', NULL, $user->uid));
  $form['imagepicker_iframe']['imagepicker_advanced_iframe_border'] = imagepicker_iframe_border_elem(imagepicker_variable_get('imagepicker_advanced_iframe_border', NULL, $user->uid));
  $form['imagepicker_iframe']['imagepicker_advanced_collapsed'] = imagepicker_collapsed_elem(imagepicker_variable_get('imagepicker_advanced_collapsed', NULL, $user->uid));

  if ($account) {
    $form['uid'] = array(
      '#type' => 'value',
      '#value' => $user->uid,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('imagepicker_user_config_form_reset'),
  );
  return $form;
}

function imagepicker_user_config_admin_form_validate($form, &$form_state) {

  if (isset($form_state['values']['uid'])) {
    $user = user_load($form_state['values']['uid']);
  }
  else {
    global $user;
  }
  if (! drupal_strlen($form_state['values']['imagepicker_default_thumbnail_size'])) {
    form_set_error('imagepicker_default_thumbnail_size', t('You must fill in the Thumbnail field'));
  }
  if (! is_numeric($form_state['values']['imagepicker_default_thumbnail_size'])) {
    form_set_error('imagepicker_default_thumbnail_size', t('The Thumbnail field must be a number'));
  }
  if (drupal_strlen($form_state['values']['imagepicker_default_scale']) && ! is_numeric($form_state['values']['imagepicker_default_scale'])) {
    form_set_error('imagepicker_default_scale', t('The Scale field must be a number'));
  }

  if (imagepicker_variable_get('imagepicker_default_align_show', 1)) {
    if (! drupal_strlen($form_state['values']['imagepicker_default_fleft'])) {
      form_set_error('imagepicker_default_fleft', t('You must fill in the Left alignment field'));
    }
    if (! drupal_strlen($form_state['values']['imagepicker_default_fright'])) {
      form_set_error('imagepicker_default_fright', t('You must fill in the Right alignment field'));
    }
  }
  if (! is_numeric($form_state['values']['imagepicker_default_browser_thumbnail_size'])) {
    form_set_error('imagepicker_default_browser_thumbnail_size', t('The Browser Thumbnail field must be a number'));
  }
  if (! is_numeric($form_state['values']['imagepicker_advanced_browser_columns']) || $form_state['values']['imagepicker_advanced_browser_columns'] < 0) {
    form_set_error('imagepicker_advanced_browser_columns', t('The browser columns must be a positive number or zero'));
  }
  if (! is_numeric($form_state['values']['imagepicker_advanced_browser_page']) || $form_state['values']['imagepicker_advanced_browser_page'] < 0) {
    form_set_error('imagepicker_advanced_browser_page', t('The browser page must be a positive number or zero'));
  }
  if (! is_numeric($form_state['values']['imagepicker_rows_per_page']) || $form_state['values']['imagepicker_rows_per_page'] < 1) {
    form_set_error('imagepicker_rows_per_page', t('Rows per page must be a positive number'));
  }
  if (! is_numeric($form_state['values']['imagepicker_advanced_iframe_height'])) {
    form_set_error('imagepicker_advanced_iframe_height', t('The Iframe height must be a number'));
  }
  if (! drupal_strlen($form_state['values']['imagepicker_advanced_iframe_width'])) {
    form_set_error('imagepicker_advanced_iframe_width', t('You must fill in the Iframe width'));
  }
  if (! drupal_strlen($form_state['values']['imagepicker_advanced_iframe_border'])) {
    form_set_error('imagepicker_advanced_iframe_border', t('You must fill in the Iframe border'));
  }
  if (module_exists('lightbox2') && imagepicker_variable_get('imagepicker_lightbox2_enable', 1)) {
    if (! drupal_strlen($form_state['values']['imagepicker_lightbox2_insert'])) {
      form_set_error('imagepicker_lightbox2_insert', t('You must fill in the Lightbox2 insert'));
    }
  }

  if ( isset($form_state['values']['imagepicker_watermark_image']) && $form_state['values']['imagepicker_watermark_image'] ) {
    $wdir = imagepicker_get_watermarks_dir($user);
    $file = $wdir . DIRECTORY_SEPARATOR . $form_state['values']['imagepicker_watermark_image'];
    if (! file_exists($file)) {
      form_set_error('imagepicker_watermark_image', t('This file does not exist or has invalid path: %file.', array('%file' => $file)));
    }
    if ($form_state['values']['imagepicker_watermark_scaling']) {
      if (! is_numeric($form_state['values']['imagepicker_watermark_scaling_percentage']) || $form_state['values']['imagepicker_watermark_scaling_percentage'] <= 0 || $form_state['values']['imagepicker_watermark_scaling_percentage'] >= 100) {
        form_set_error('imagepicker_watermark_scaling_percentage', t('Please enter a number between 1 and 100 for scaling percentage.'));
      }
      elseif ($form_state['values']['imagepicker_watermark_scaling_percentage'] < 10 || ($form_state['values']['imagepicker_watermark_min_width'] < 50 && $form_state['values']['imagepicker_watermark_min_width'] > 0)) {
        // we do not disable scaling for low values, e.g. if the user chooses to scale down to 1%, we just warn
        drupal_set_message(t('Warning: Scaling is set to @percentage%, minimum width is only @min_width px. This may result in very small or almost invisible watermarks. It is recommended to check twice that an adequate minimum width has been set.',
          array('@percentage' => $form_state['values']['imagepicker_watermark_scaling_percentage'], '@min_width' => $form_state['values']['imagepicker_watermark_min_width'])));
      }
      if (intval($form_state['values']['imagepicker_watermark_min_width']) < 0) {
        form_set_error('imagepicker_watermark_min_width', t('Please enter a positive number for minimum width.'));
      }
    }
    if (! is_numeric($form_state['values']['imagepicker_watermark_perc']) || $form_state['values']['imagepicker_watermark_perc'] < 0 || $form_state['values']['imagepicker_watermark_perc'] > 100) {
      form_set_error('imagepicker_watermark_perc', t("JPEG quality value should be an integer between 0 and 100."));
    }
  }

  if (! empty($_FILES['files']['name']['file_upload'])) {
    module_load_include('inc', 'imagepicker', 'imagepicker.upload');
    if (!isset($_FILES['files']['tmp_name']['file_upload']) || !file_exists($_FILES['files']['tmp_name']['file_upload'])) {
      form_set_error('file_upload', t('Error while uploading file.'));
    }
    elseif (!image_get_info($_FILES['files']['tmp_name']['file_upload'])) {
      form_set_error('file_upload', t('Uploaded file is not an image.'));
    }
    elseif (!imagepicker_get_uploaded_file_extension('file_upload')) {
      form_set_error('file_upload', t('Only .jpg, .gif and .png image files are accepted.'));
    }
  }
}

function imagepicker_user_config_admin_form_submit($form, &$form_state) {

  if ( isset($form_state['values']['uid']) ) {
    $user = user_load($form_state['values']['uid']);
  }
  else {
    global $user;
  }
  $file = FALSE;
  if (! empty($_FILES['files']['name']['file_upload'])) {
    module_load_include('inc', 'imagepicker', 'imagepicker.upload');
    $wdir = imagepicker_get_watermarks_dir();
    $wdirscheme = imagepicker_get_watermarks_dir(FALSE, TRUE);
    if (file_prepare_directory($wdir, FILE_CREATE_DIRECTORY)) {
      // clear out the noisy 'created' messages
      drupal_get_messages('status', TRUE);
      $wdir .= DIRECTORY_SEPARATOR;
      $wdirscheme .= DIRECTORY_SEPARATOR;
      $destination = $wdirscheme;
      $file = imagepicker_copy_uploaded_file($destination, 'file_upload');
    }
  }
  // got one
  if ($file) {
    $form_state['values']['imagepicker_watermark_image'] = $file;
  }

  if (isset($form_state['values']['imagepicker_watermark_image'])) {
    imagepicker_variable_set('imagepicker_watermark_enable', $form_state['values']['imagepicker_watermark_enable'], $user->uid);
    imagepicker_variable_set('imagepicker_watermark_image', $form_state['values']['imagepicker_watermark_image'], $user->uid);
    imagepicker_variable_set('imagepicker_watermark_position', $form_state['values']['imagepicker_watermark_position'], $user->uid);
    imagepicker_variable_set('imagepicker_watermark_use', $form_state['values']['imagepicker_watermark_use'], $user->uid);
    imagepicker_variable_set('imagepicker_watermark_perc', $form_state['values']['imagepicker_watermark_perc'], $user->uid);
    if ($form_state['values']['imagepicker_watermark_scaling']) {
      imagepicker_variable_set('imagepicker_watermark_scaling', $form_state['values']['imagepicker_watermark_scaling'], $user->uid);
      imagepicker_variable_set('imagepicker_watermark_scaling_percentage', $form_state['values']['imagepicker_watermark_scaling_percentage'], $user->uid);
      imagepicker_variable_set('imagepicker_watermark_min_width', $form_state['values']['imagepicker_watermark_min_width'], $user->uid);
    }
  }
  else {
    imagepicker_variable_del('imagepicker_watermark_image', $user->uid);
  }
  imagepicker_variable_set('imagepicker_default_thumbnail_size', $form_state['values']['imagepicker_default_thumbnail_size'], $user->uid);
  imagepicker_variable_set('imagepicker_default_scale', $form_state['values']['imagepicker_default_scale'], $user->uid);
  imagepicker_variable_set('imagepicker_rows_per_page', $form_state['values']['imagepicker_rows_per_page'], $user->uid);
  imagepicker_variable_set('imagepicker_default_pagelink', $form_state['values']['imagepicker_default_pagelink'], $user->uid);
  if (imagepicker_variable_get('imagepicker_default_align_show', 1)) {
    imagepicker_variable_set('imagepicker_insert_defaults_align', $form_state['values']['imagepicker_insert_defaults_align'], $user->uid);
  }
  imagepicker_variable_set('imagepicker_insert_defaults_show', $form_state['values']['imagepicker_insert_defaults_show'], $user->uid);
  imagepicker_variable_set('imagepicker_insert_defaults_link', $form_state['values']['imagepicker_insert_defaults_link'], $user->uid);
  if (imagepicker_variable_get('imagepicker_default_align_show', 1)) {
    imagepicker_variable_set('imagepicker_default_fleft', $form_state['values']['imagepicker_default_fleft'], $user->uid);
    imagepicker_variable_set('imagepicker_default_fright', $form_state['values']['imagepicker_default_fright'], $user->uid);
  }
  imagepicker_variable_set('imagepicker_default_browser_thumbnail_size', $form_state['values']['imagepicker_default_browser_thumbnail_size'], $user->uid);
  imagepicker_variable_set('imagepicker_advanced_browser_columns', $form_state['values']['imagepicker_advanced_browser_columns'], $user->uid);
  imagepicker_variable_set('imagepicker_advanced_browser_page', $form_state['values']['imagepicker_advanced_browser_page'], $user->uid);
  if (imagepicker_variable_get('imagepicker_show_browse_order_form', 1)) {
    imagepicker_variable_set('imagepicker_show_browse_order_form', $form_state['values']['imagepicker_show_browse_order_form'], $user->uid);
  }
  imagepicker_variable_set('imagepicker_default_browser_order', $form_state['values']['imagepicker_default_browser_order'], $user->uid);
  if (imagepicker_variable_get('imagepicker_show_browse_search_form', 1)) {
    imagepicker_variable_set('imagepicker_show_browse_search_form', $form_state['values']['imagepicker_show_browse_search_form'], $user->uid);
  }
  if (isset($form_state['values']['imagepicker_advanced_colorbox']) && module_exists('colorbox')) {
    imagepicker_variable_set('imagepicker_advanced_colorbox', $form_state['values']['imagepicker_advanced_colorbox'], $user->uid);
  }
  imagepicker_variable_set('imagepicker_advanced_iframe_width', $form_state['values']['imagepicker_advanced_iframe_width'], $user->uid);
  imagepicker_variable_set('imagepicker_advanced_iframe_height', $form_state['values']['imagepicker_advanced_iframe_height'], $user->uid);
  imagepicker_variable_set('imagepicker_advanced_iframe_border', $form_state['values']['imagepicker_advanced_iframe_border'], $user->uid);
  imagepicker_variable_set('imagepicker_advanced_collapsed', $form_state['values']['imagepicker_advanced_collapsed'], $user->uid);

  if (module_exists('lightbox2') && imagepicker_variable_get('imagepicker_lightbox2_enable', 1)) {
    imagepicker_variable_set('imagepicker_lightbox2_insert', $form_state['values']['imagepicker_lightbox2_insert'], $user->uid);
  }

  drupal_set_message(t('Configuration settings saved'));
}

function imagepicker_user_config_form_reset($form, &$form_state) {

  global $user;
  imagepicker_variable_del('imagepicker_watermark_enable', $user->uid);
  imagepicker_variable_del('imagepicker_watermark_image', $user->uid);
  imagepicker_variable_del('imagepicker_watermark_position', $user->uid);
  imagepicker_variable_del('imagepicker_watermark_use', $user->uid);
  imagepicker_variable_del('imagepicker_watermark_perc', $user->uid);
  imagepicker_variable_del('imagepicker_watermark_scaling', $user->uid);
  imagepicker_variable_del('imagepicker_watermark_scaling_percentage', $user->uid);
  imagepicker_variable_del('imagepicker_watermark_min_width', $user->uid);
  imagepicker_variable_del('imagepicker_default_thumbnail_size', $user->uid);
  imagepicker_variable_del('imagepicker_default_scale', $user->uid);
  imagepicker_variable_del('imagepicker_rows_per_page', $user->uid);
  imagepicker_variable_del('imagepicker_insert_defaults_align', $user->uid);
  imagepicker_variable_del('imagepicker_insert_defaults_show', $user->uid);
  imagepicker_variable_del('imagepicker_insert_defaults_link', $user->uid);
  imagepicker_variable_del('imagepicker_default_fleft', $user->uid);
  imagepicker_variable_del('imagepicker_default_fright', $user->uid);
  imagepicker_variable_del('imagepicker_default_browser_thumbnail_size', $user->uid);
  imagepicker_variable_del('imagepicker_advanced_browser_columns', $user->uid);
  imagepicker_variable_del('imagepicker_advanced_browser_page', $user->uid);
  imagepicker_variable_del('imagepicker_show_browse_order_form', $user->uid);
  imagepicker_variable_del('imagepicker_default_browser_order', $user->uid);
  imagepicker_variable_del('imagepicker_show_browse_search_form', $user->uid);
  imagepicker_variable_del('imagepicker_default_pagelink', $user->uid);
  imagepicker_variable_del('imagepicker_advanced_colorbox', $user->uid);
  imagepicker_variable_del('imagepicker_advanced_iframe_width', $user->uid);
  imagepicker_variable_del('imagepicker_advanced_iframe_height', $user->uid);
  imagepicker_variable_del('imagepicker_advanced_iframe_border', $user->uid);
  imagepicker_variable_del('imagepicker_advanced_collapsed', $user->uid);
  imagepicker_variable_del('imagepicker_lightbox2_insert', $user->uid);

  drupal_set_message(t('Configuration settings reset'));
}

function imagepicker_user_config_admin_form_delete($form, &$form_state) {

  if (isset($form_state['values']['uid'])) {
    $user = user_load($form_state['values']['uid']);
  }
  else {
    global $user;
  }
  if ($form_state['values']['imagepicker_watermark_image']) {
    $file = imagepicker_get_watermarks_dir() . DIRECTORY_SEPARATOR . $form_state['values']['imagepicker_watermark_image'];
    if (file_exists($file)) {
      file_unmanaged_delete($file);
    }
    imagepicker_variable_del('imagepicker_watermark_image', $user->uid);

    drupal_set_message(t('Watermark File deleted'));
  }
}

