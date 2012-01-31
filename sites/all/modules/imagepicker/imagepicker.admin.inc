<?php

/**
 * @file
 * admin settings functions
 */

/**
 * Menu callback; presents the settings form for imagepicker
 */
function imagepicker_settings_page($op = NULL) {
  drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');
  $info = '';
  $dir = imagepicker_get_files_directory();
  $orphan_status = imagepicker_check_orphans($dir);
  if ($orphan_status) {
    // we have orphans
    $info .= '<ul class="item-list">';
    foreach ($orphan_status AS $user_uid) {
      $info .= '<li>' . t("Nonexistent user %u has orphan files. Click <b>!h</b> to fix.", array('%u' => $user_uid, '!h' => l( t('Here'), IMAGEPICKER_ADMIN_PATH . '/orphans'))) . '</li>';
    }
    $info .= '</ul>';
  }

  $content = drupal_get_form('imagepicker_settings_form', $info);
  return $content;
}

/**
 * Function to display the imagepicker admin settings form
 * @return
 *   Returns the form.
 */
function imagepicker_settings_form($form, &$form_state, $info) {

  drupal_add_js(IMAGEPICKER_PATH . '/imagepicker.js');
  drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');

  // default settings
  $form['imagepicker_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set imagepicker settings'),
    '#description' => t('Overall administrative settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -40,
  );
  if ($info) {
    $form['imagepicker_settings']['imagepicker_admin_message'] = array(
      '#markup' => $info,
    );
  }
  $node_types = node_type_get_names();
  $form['imagepicker_settings']['imagepicker_node_types_enabled'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Node Types'),
    '#description' => t('Set the node types you want to enable Imagepicker for.'),
    '#default_value' => imagepicker_variable_get('imagepicker_node_types_enabled', array_keys($node_types)),
    '#options' => $node_types,
  );
  if (module_exists('comment')) {
    $form['imagepicker_settings']['imagepicker_comment_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Comments'),
      '#description' => t('Setting this option enables Imagepicker in comments.'),
      '#return_value' => 1,
      '#default_value' => imagepicker_variable_get('imagepicker_comment_enabled', 0),
    );
  }
  $form['imagepicker_settings']['imagepicker_blocks_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Blocks'),
    '#description' => t('Setting this option enables Imagepicker in blocks.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_blocks_enabled', 0),
  );
  $form['imagepicker_settings']['imagepicker_account_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Imagepicker in My Account'),
    '#description' => t('Setting this option enables Imagepicker in My Account.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_account_enabled', 1),
  );
  $form['imagepicker_settings']['imagepicker_user_config_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable User configuration'),
    '#description' => t('Allow users to select their own settings.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_user_config_enable', 1),
  );
  $form['imagepicker_settings']['imagepicker_copy_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable copy feature'),
    '#description' => t('Allow users to copy and optionally resize an image.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_copy_enabled', 0),
  );
  $form['imagepicker_settings']['imagepicker_use_full_url'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use full url'),
    '#description' => t('Setting this option will make imagepicker use a full url to the image being inserted, unsetting it will use a path relative to the Drupal root.<br />This is useful if you are developing a site with a different url than the production site will be on.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_use_full_url', 0),
  );
  $form['imagepicker_settings']['imagepicker_use_cssbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow additional css'),
    '#description' => t('Setting this option will provide a textfield where additional css can be added .'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_use_cssbox', 0),
  );
  // rel
  if (module_exists('colorbox') && imagepicker_variable_get('imagepicker_colorbox_enable', 0)) {
    $form['imagepicker_settings']['imagepicker_use_relbox'] = array(
      '#type' => 'checkbox',
      '#title' => t('Provide a rel textfield'),
      '#description' => t('Setting this option will provide a textfield where a rel attribute can be added. Useful for creating galleries.'),
      '#return_value' => 1,
      '#default_value' => imagepicker_variable_get('imagepicker_use_relbox', 0),
    );
  }
  $form['imagepicker_settings']['imagepicker_use_icons'] = array(
    '#type' => 'checkbox',
    '#title' => t('Icons in tables'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_use_icons', 1),
    '#description' => t('Setting this option enables icons in tables.'),
  );
  $max_uploads = range(0, 6);
  unset($max_uploads[0]);
  $form['imagepicker_settings']['imagepicker_max_uploads'] = array(
    '#type' => 'select',
    '#title' => t('Multiple Uploads'),
    '#description' => t('The maximum number of uploads at a time.'),
    '#default_value' => imagepicker_variable_get('imagepicker_max_uploads', 1),
    '#options' => $max_uploads,
  );

  $form['imagepicker_settings']['imagepicker_invoke_extra'] = array(
    '#type' => 'checkbox',
    '#title' => t('Invoke external imagepicker hook'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_invoke_extra', 0),
    '#description' => t("Invoke the 'imagepicker_upload_extra' external hook. Do not enable this unless you are using this hook."),
  );

  // default options
  $form['imagepicker_defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set imagepicker defaults'),
    '#description' => t('These settings are also available to users if User Configuration is enabled.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -38,
  );
  $form['imagepicker_defaults']['imagepicker_default_thumbnail_size'] = imagepicker_thumbnail_size_elem(imagepicker_variable_get('imagepicker_default_thumbnail_size', 100));
  $form['imagepicker_defaults']['imagepicker_default_scale'] = imagepicker_scale_elem(imagepicker_variable_get('imagepicker_default_scale', ''));
  $form['imagepicker_defaults']['imagepicker_default_pagelink'] = imagepicker_pagelink_elem(imagepicker_variable_get('imagepicker_default_pagelink', 1));
  $form['imagepicker_defaults']['imagepicker_rows_per_page'] = imagepicker_rows_per_page_elem(imagepicker_variable_get('imagepicker_rows_per_page', 25));
  $form['imagepicker_defaults']['imagepicker_dateformat_default'] = array(
    '#type' => 'select',
    '#title' => t('Set Date Format'),
    '#description' => t("Select the Date format to use in tables."),
    '#default_value' => imagepicker_variable_get('imagepicker_dateformat_default', 'medium'),
    '#options' => array('short' => t('Short'), 'medium' => t('Medium'), 'long' => t('Long')),
  );

  // insert settings
  $form['imagepicker_insert_defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set imagepicker insert defaults'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -36,
  );
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_align'] = imagepicker_get_align_opts(imagepicker_variable_get('imagepicker_insert_defaults_align', 'none'));
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_show'] = imagepicker_get_show_opts(imagepicker_variable_get('imagepicker_insert_defaults_show', 'full'));
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_link'] = imagepicker_get_link_opts(imagepicker_variable_get('imagepicker_insert_defaults_link', 'none'));

  // alignment settings
  $form['imagepicker_align'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image alignment settings'),
    '#collapsible' => TRUE,
    '#collapsed' => (imagepicker_variable_get('imagepicker_default_align_show', 1) ? TRUE : FALSE),
    '#weight' => -34,
  );
  $form['imagepicker_align']['imagepicker_default_align_show'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Alignment options'),
    '#description' => t('Setting this option will enable the alignment options on the insert page.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_default_align_show', 1),
  );
  $form['imagepicker_align']['imagepicker_default_fleft'] = imagepicker_fleft_elem(imagepicker_variable_get('imagepicker_default_fleft', 'style="float: left"'));
  $form['imagepicker_align']['imagepicker_default_fright'] = imagepicker_fright_elem(imagepicker_variable_get('imagepicker_default_fright', 'style="float: right"'));

  // browser
  $form['imagepicker_browser'] = array(
    '#type' => 'fieldset',
    '#title' => t('Imagepicker browser settings'),
    '#description' => t('Configure browser.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -32,
  );
  $form['imagepicker_browser']['imagepicker_default_browser_thumbnail_size'] = imagepicker_browser_thumbnail_size_elem(imagepicker_variable_get('imagepicker_default_browser_thumbnail_size', 100));
  $form['imagepicker_browser']['imagepicker_advanced_browser_columns'] = imagepicker_browser_columns_elem(imagepicker_variable_get('imagepicker_advanced_browser_columns', 0));
  $form['imagepicker_browser']['imagepicker_advanced_browser_page'] = imagepicker_browser_page_elem(imagepicker_variable_get('imagepicker_advanced_browser_page', 20));
  $form['imagepicker_browser']['imagepicker_show_browse_order_form'] = imagepicker_show_browse_order_form_elem(imagepicker_variable_get('imagepicker_show_browse_order_form', 1));
  $form['imagepicker_browser']['imagepicker_default_browser_order'] = imagepicker_browser_order_elem(imagepicker_variable_get('imagepicker_default_browser_order', 'img_id DESC'));
  $form['imagepicker_browser']['imagepicker_show_browse_search_form'] = imagepicker_show_browse_search_form_elem(imagepicker_variable_get('imagepicker_show_browse_search_form', 1));

  if (module_exists('lightbox2') || module_exists('colorbox') || module_exists('image')) {
    $form['imagepicker_module'] = array(
      '#type' => 'fieldset',
      '#title' => t('Module integration'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => -30,
    );
    if (module_exists('lightbox2')) {
      $form['imagepicker_module']['imagepicker_lightbox2_enable'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable Lightbox2 in Imagepicker'),
        '#description' => t('Setting this option will enable Lightbox2 in Imagepicker.'),
        '#return_value' => 1,
        '#default_value' => imagepicker_variable_get('imagepicker_lightbox2_enable', 1),
      );
      $form['imagepicker_module']['imagepicker_lightbox2_insert'] = imagepicker_lightbox2_insert_elem(imagepicker_variable_get('imagepicker_lightbox2_insert', 'lightbox'));
    }
    if (module_exists('colorbox')) {
      $form['imagepicker_module']['imagepicker_colorbox_enable'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable Colorbox in Imagepicker'),
        '#description' => t('Setting this option will enable Colorbox in Imagepicker.'),
        '#return_value' => 1,
        '#default_value' => imagepicker_variable_get('imagepicker_colorbox_enable', 1),
      );
    }
    // image module support
    if (module_exists('image')) {
      $form['imagepicker_module']['imagepicker_image_enable'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable Image styles in Imagepicker'),
        '#description' => t('Setting this option will enable Image styles in Imagepicker.'),
        '#return_value' => 1,
        '#default_value' => imagepicker_variable_get('imagepicker_image_enable', 0),
      );
      $opts = image_style_options();
      $form['imagepicker_module']['imagepicker_image_default_show'] = array(
        '#type' => 'select',
        '#title' => t('Default Image Style for show'),
        '#options' => $opts,
        '#default_value' => imagepicker_variable_get('imagepicker_image_default_show', ''),
      );
      $form['imagepicker_module']['imagepicker_image_default_link'] = array(
        '#type' => 'select',
        '#title' => t('Default Image Style for link'),
        '#options' => $opts,
        '#default_value' => imagepicker_variable_get('imagepicker_image_default_link', ''),
      );
    }
  }

  // quota
  $form['imagepicker_quotas'] = array(
    '#type' => 'fieldset',
    '#title' => t('User quotas'),
    '#collapsible' => TRUE,
    '#collapsed' => (imagepicker_variable_get('imagepicker_quota_enable', 1) ? TRUE : FALSE),
    '#weight' => -28,
  );
  $form['imagepicker_quotas']['imagepicker_quota_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Quotas in Imagepicker'),
    '#description' => t('Setting this option will enable Quotas in Imagepicker.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_quota_enable', 1),
  );
  $form['imagepicker_quotas']['imagepicker_quota_byrole'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Quotas by Role'),
    '#description' => t("Setting this option will enable 'Quotas by Role' instead of 'Per User'."),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_quota_byrole', 0),
  );
  $form['imagepicker_quotas']['imagepicker_quota_role'] = array(
    '#type' => 'radios',
    '#title' => t('Available Roles'),
    '#description' => t("Select which Role for 'Quotas by Role'."),
    '#options' => user_roles(TRUE),
    '#default_value' => imagepicker_variable_get('imagepicker_quota_role', 2),
  );
  $form['imagepicker_quotas']['imagepicker_quota_default'] = array(
    '#type' => 'select',
    '#title' => t('Set Quota'),
    '#description' => t("Select the Quota to apply for 'Quotas by Role' or the default for 'Per User'."),
    '#default_value' => imagepicker_variable_get('imagepicker_quota_default', 0),
    '#options' => imagepicker_get_quota_list(),
  );
  if ( imagepicker_image_check_functions()) {
    // watermark settings
    $form['imagepicker_watermark'] = array(
      '#type' => 'fieldset',
      '#title' => t('Image watermark settings'),
      '#collapsible' => TRUE,
      '#collapsed' => ( imagepicker_variable_get('imagepicker_watermark_enable', 0) ? TRUE : FALSE),
      '#weight' => -26,
    );
    $form['imagepicker_watermark']['imagepicker_watermark_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Watermarks in Imagepicker'),
      '#description' => t('Setting this option will enable Watermarks on new images in Imagepicker.'),
      '#return_value' => 1,
      '#default_value' => imagepicker_variable_get('imagepicker_watermark_enable', 0),
    );
    $form['imagepicker_watermark']['imagepicker_watermark_image'] = array(
      '#type' => 'textfield',
      '#title' => t('Default Watermark image'),
      '#description' => t('Path of the image that should be used as the watermark image, relative to the Drupal root.  <br />This should be a truecolor (32 bit) PNG file with an alpha channel, but other formats may work.<br />If this field is used, the watermark will be applied to all new images uploaded to the site.<br />If this field is left empty users with access will have the option to apply their own watermark.'),
      '#size' => 55,
      '#maxlength' => 100,
      '#required' => FALSE,
      '#default_value' => imagepicker_variable_get('imagepicker_watermark_image', ''),
    );
    $form['imagepicker_watermark']['imagepicker_watermark_position'] = array(
      '#type' => 'select',
      '#title' => t('Select default watermark position'),
      '#default_value' => imagepicker_variable_get('imagepicker_watermark_position', 0),
      '#options' => imagepicker_watermark_opts(),
    );
    // watermarks
    $form['imagepicker_watermark']['imagepicker_watermark_scale'] = array(
      '#type' => 'fieldset',
      '#title' => t('Watermark scaling'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => 1,
    );
    $form['imagepicker_watermark']['imagepicker_watermark_scale']['imagepicker_watermark_scaling'] = imagepicker_watermark_scaling_elem(imagepicker_variable_get('imagepicker_watermark_scaling', FALSE));
    $form['imagepicker_watermark']['imagepicker_watermark_scale']['imagepicker_watermark_scaling_percentage'] = imagepicker_watermark_scaling_percentage_elem(imagepicker_variable_get('imagepicker_watermark_scaling_percentage', 50));
    $form['imagepicker_watermark']['imagepicker_watermark_scale']['imagepicker_watermark_min_width'] = imagepicker_watermark_min_width_elem(imagepicker_variable_get('imagepicker_watermark_min_width', 0));
  }
  $form['imagepicker_exif'] = array(
    '#type' => 'fieldset',
    '#title' => t('Imagepicker Exif settings'),
    '#description' => t('Enable exif display.'),
    '#collapsible' => TRUE,
    '#collapsed' => (imagepicker_variable_get('imagepicker_exifinfo_enable', 0) ? TRUE : FALSE),
    '#weight' => -24,
  );
  if (imagepicker_exif_check(TRUE)) {
    $form['imagepicker_exif']['imagepicker_exifinfo_enable'] = array(
     '#type' => 'checkbox',
     '#title' => t('Enable Display of Exif info'),
     '#description' => t('Enable Display of Extended file information using PHP builtin functions.'),
     '#return_value' => 1,
     '#default_value' => imagepicker_variable_get('imagepicker_exifinfo_enable', 0),
    );
  }
  $form['imagepicker_exif']['imagepicker_exifinfo_external'] = array(
    '#type' => 'textfield',
    '#title' => t('Use external program'),
    '#description' => t('Enter an external program to use for exif info instead of builtin functions.<br />For instance exiftool. Requires a full filesystem path.'),
    '#size' => 55,
    '#maxlength' => 100,
    '#required' => FALSE,
    '#default_value' => imagepicker_variable_get('imagepicker_exifinfo_external', ''),
  );
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
    $form['imagepicker_iframe']['imagepicker_advanced_colorbox'] = imagepicker_colorbox_elem(imagepicker_variable_get('imagepicker_advanced_colorbox', 0));
  }

  $form['imagepicker_iframe']['imagepicker_advanced_iframe_width'] = imagepicker_iframe_width_elem(imagepicker_variable_get('imagepicker_advanced_iframe_width', "100%"));
  $form['imagepicker_iframe']['imagepicker_advanced_iframe_height'] = imagepicker_iframe_height_elem(imagepicker_variable_get('imagepicker_advanced_iframe_height', "500"));
  $form['imagepicker_iframe']['imagepicker_advanced_iframe_border'] = imagepicker_iframe_border_elem(imagepicker_variable_get('imagepicker_advanced_iframe_border', "0"));
  $form['imagepicker_iframe']['imagepicker_advanced_collapsed'] = imagepicker_collapsed_elem(imagepicker_variable_get('imagepicker_advanced_collapsed', 0));
  $form['imagepicker_iframe']['imagepicker_use_admin_theme'] = imagepicker_use_admin_theme_elem(imagepicker_variable_get('imagepicker_use_admin_theme', 0));

  // progress bar
  $form['imagepicker_progress'] = array(
    '#type' => 'fieldset',
    '#title' => t('Progress bar'),
    '#description' => t('Enable and configure progress bar.'),
    '#collapsible' => TRUE,
    '#collapsed' => (imagepicker_variable_get('imagepicker_upload_progress_enabled', 1) ? TRUE : FALSE),
    '#weight' => -18,
  );
  $form['imagepicker_progress']['imagepicker_upload_progress_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Progress bar in upload'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_upload_progress_enabled', 1),
  );
  $form['imagepicker_progress']['imagepicker_upload_progress_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Progress bar Delay'),
    '#description' => t('Show the progress bar if processing is longer than this value.'),
    '#size' => 3,
    '#required' => TRUE,
    '#default_value' => imagepicker_variable_get('imagepicker_upload_progress_delay', 3),
    '#field_suffix' => t('seconds'),
  );

  $form['imagepicker_progress']['imagepicker_upload_progress_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Progress bar message'),
    '#description' => t('Message to display while uploading.'),
    '#size' => 60,
    '#default_value' => imagepicker_variable_get('imagepicker_upload_progress_message', t('Processing form... please wait.')),
  );
  $vok = FALSE;
  $extok = FALSE;
  if (version_compare(phpversion(), IMAGEPICKER_UPLOAD_STATUS_MIN_PHP) >= 0 ) {
    $vok = TRUE;
    if (extension_loaded('uploadprogress')) {
      $extok = TRUE;
    }
  }

  if ($vok && $extok) {
    $form['imagepicker_progress']['imagepicker_uploadprogress'] = array(
      '#type' => 'fieldset',
      '#title' => t('Upload Progress Extension'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['imagepicker_progress']['imagepicker_uploadprogress']['imagepicker_uploadprogress_server'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable PECL uploadprogress'),
      '#default_value' => imagepicker_variable_get('imagepicker_uploadprogress_server', ''),
    );
    $form['imagepicker_progress']['imagepicker_uploadprogress']['imagepicker_uploadprogress_interval'] = array(
      '#title' => t('Interval Time'),
      '#description' => t('The interval between each check.'),
      '#type' => 'textfield',
      '#size' => 5,
      '#default_value' => imagepicker_variable_get('imagepicker_uploadprogress_interval', 1),
      '#field_suffix' => t('seconds'),
    );
  }

  // Groups
  $form['imagepicker_groups'] = array(
    '#type' => 'fieldset',
    '#title' => t('Imagepicker group settings'),
    '#description' => t('Enable and configure groups.'),
    '#collapsible' => TRUE,
    '#collapsed' => (imagepicker_variable_get('imagepicker_groups_enabled', 1) ? TRUE : FALSE),
    '#weight' => -16,
  );
  $form['imagepicker_groups']['imagepicker_groups_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Groups'),
    '#description' => t('Allow users to manage groups.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_groups_enabled', 1),
  );
  $form['imagepicker_groups']['imagepicker_public_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Public option'),
    '#description' => t('Allow users to make image groups publicly available.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_public_enabled', 1),
  );
  $form['imagepicker_groups']['imagepicker_publicroles_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Public Role Control option'),
    '#description' => t('Allow users to make publicly available image groups restricted by role.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_publicroles_enabled', 1),
  );
  $form['imagepicker_groups']['imagepicker_groups_in_upload_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Groups in upload form'),
    '#description' => t('Allow users to select a group in the upload form.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_groups_in_upload_enabled', 1),
  );

  // Import
  $form['imagepicker_import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Imagepicker Bulk Import'),
    '#description' => t('Enable bulk import of images.'),
    '#collapsible' => TRUE,
    '#collapsed' => (imagepicker_variable_get('imagepicker_import_enabled', 0) ? TRUE : FALSE),
    '#weight' => -14,
  );
  $form['imagepicker_import']['imagepicker_import_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Bulk import processing'),
    '#description' => t('Import images into imagepicker from an import folder in the filesystem.<br />Requires FTP or SSH access'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_import_enabled', 0),
  );
  $form['imagepicker_import']['imagepicker_import_delete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Bulk import deletion'),
    '#description' => t('Delete uploaded source images after successful import into imagepicker.<br />The image files should be writeable by the web server for this option to work.'),
    '#return_value' => 1,
    '#default_value' => imagepicker_variable_get('imagepicker_import_delete', 0),
  );
  $form['imagepicker_import']['imagepicker_import_max'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Import Batch size'),
    '#description' => t('How many files to process at a time. Must be greater than 0 and less than 100.'),
    '#size' => 3,
    '#required' => TRUE,
    '#default_value' => imagepicker_variable_get('imagepicker_import_max', 5),
  );

  // gallery blocks
  if (module_exists('colorbox')) {

    $groups = imagepicker_get_all_groups();
    $totgroups = 0;
    if ($groups) {
      $totgroups = count($groups);
    }

    $form['imagepicker_blocks'] = array(
      '#type' => 'fieldset',
      '#title' => t('Imagepicker Gallery Blocks'),
      '#collapsible' => TRUE,
      '#collapsed' => (imagepicker_variable_get('imagepicker_galleryblocks_enabled', 0) ? TRUE : FALSE),
      '#weight' => -12,
    );
    $form['imagepicker_blocks']['imagepicker_galleryblocks_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Gallery blocks'),
      '#description' => t('Create one or more blocks containing a link to a grouped image gallery.'),
      '#return_value' => 1,
      '#default_value' => imagepicker_variable_get('imagepicker_galleryblocks_enabled', 0),

    );
    if ($totgroups) {
      $level_options =  range(0, $totgroups);
      unset($level_options[0]);
      $form['imagepicker_blocks']['imagepicker_galleryblocks_howmany'] = array(
        '#type' => 'select',
        '#title' => t('How many gallery blocks do you need?'),
        '#default_value' => imagepicker_variable_get('imagepicker_galleryblocks_howmany', 1),
        '#options' => $level_options,
        '#description' => t('Select how many blocks you need and click on the Save Configuration button to make the form available.'),
      );
      $form['imagepicker_blocks']['message'] = array('#markup' => t('You have %c available.', array('%c' => format_plural($totgroups, '1 group', '@count groups'))));
    }
    else {
      $form['imagepicker_blocks']['imagepicker_galleryblocks_howmany'] = array(
        '#type' => 'value',
        '#value' => 0,
      );
      $form['imagepicker_blocks']['message'] = array('#markup' => t('You need to add some groups.'));
    }
    $form['imagepicker_blocks']['imagepicker_galleryblocks_public'] = array(
      '#type' => 'checkbox',
      '#title' => t('Only allow public groups'),
      '#return_value' => 1,
      '#default_value' => imagepicker_variable_get('imagepicker_galleryblocks_public', 0),
    );

    if ($totgroups) {
      $form['imagepicker_blocks']['imagepicker_galleryblocks_fieldset'] = array(
        '#type' => 'fieldset',
        '#title' => t('Gallery blocks'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#description' => t('Blocks should be configured under Structure > Blocks.'),
      );
      for ($i = 1; $i <= imagepicker_variable_get('imagepicker_galleryblocks_howmany', 1); $i++) {
        $form['imagepicker_blocks']['imagepicker_galleryblocks_fieldset']['imagepicker_galleryblocks_enable_' . $i] = array(
          '#type' => 'checkbox',
          '#title' => t('Enable Gallery block !i', array('!i' => $i)),
          '#return_value' => 1,
          '#default_value' => imagepicker_variable_get('imagepicker_galleryblocks_enable_' . $i, 1),
        );
      }
    }
    $form['imagepicker_blocks']['close'] = array('#markup' => '');
  }

  $form = system_settings_form($form);
  unset($form['#submit']);
  $form['#submit'][] = 'imagepicker_settings_form_submit';
  unset($form['#theme']);
  $form['#theme'] = 'imagepicker_settings_form';
  return $form;
}

/**
 * Validate settings form
 */
function imagepicker_settings_form_validate($form, &$form_state) {

  global $user;
  if (! drupal_strlen($form_state['values']['imagepicker_default_thumbnail_size'])) {
    form_set_error('imagepicker_default_thumbnail_size', t('You must fill in the Thumbnail field'));
  }
  if (! is_numeric($form_state['values']['imagepicker_default_thumbnail_size'])) {
    form_set_error('imagepicker_default_thumbnail_size', t('The Thumbnail field must be a number'));
  }
  if (! drupal_strlen($form_state['values']['imagepicker_default_browser_thumbnail_size'])) {
    form_set_error('imagepicker_default_browser_thumbnail_size', t('You must fill in the Browser Thumbnail field'));
  }
  if (! is_numeric($form_state['values']['imagepicker_default_browser_thumbnail_size'])) {
    form_set_error('imagepicker_default_browser_thumbnail_size', t('The Browser Thumbnail field must be a number'));
  }
  if (drupal_strlen($form_state['values']['imagepicker_default_scale']) && ! is_numeric($form_state['values']['imagepicker_default_scale'])) {
    form_set_error('imagepicker_default_scale', t('The Scale field must be a number'));
  }
  if (! is_numeric($form_state['values']['imagepicker_rows_per_page']) || $form_state['values']['imagepicker_rows_per_page'] < 1 ) {
    form_set_error('imagepicker_rows_per_page', t('The rows per page field must be greater than one.'));
  }
  if (! drupal_strlen($form_state['values']['imagepicker_default_fleft'])) {
    form_set_error('imagepicker_default_fleft', t('You must fill in the Left alignment field'));
  }
  if (! drupal_strlen($form_state['values']['imagepicker_default_fright'])) {
    form_set_error('imagepicker_default_fright', t('You must fill in the Right alignment field'));
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
  if (! is_numeric($form_state['values']['imagepicker_advanced_browser_columns']) || $form_state['values']['imagepicker_advanced_browser_columns'] < 0) {
    form_set_error('imagepicker_advanced_browser_columns', t('The browser columns must be a positive number or zero'));
  }
  if (! is_numeric($form_state['values']['imagepicker_advanced_browser_page']) || $form_state['values']['imagepicker_advanced_browser_page'] < 0) {
    form_set_error('imagepicker_advanced_browser_page', t('The browser page must be a positive number or zero'));
  }
  if (! is_numeric($form_state['values']['imagepicker_upload_progress_delay']) || $form_state['values']['imagepicker_upload_progress_delay'] < 0) {
    form_set_error('imagepicker_upload_progress_delay', t('The progress delay must be a positive number or zero'));
  }
  if (isset($form_state['values']['imagepicker_uploadprogress_interval'])) {
    if ( ! is_numeric($form_state['values']['imagepicker_uploadprogress_interval']) || $form_state['values']['imagepicker_uploadprogress_interval'] < 1) {
      form_set_error('imagepicker_uploadprogress_interval', t('The progress interval must be a positive number'));
    }
  }
  if (! drupal_strlen($form_state['values']['imagepicker_insert_defaults_align'])) {
    form_set_error('imagepicker_insert_defaults_align', t('You must fill in the default align field'));
  }
  if (! drupal_strlen($form_state['values']['imagepicker_insert_defaults_show'])) {
    form_set_error('imagepicker_insert_defaults_show', t('You must fill in the default show field'));
  }
  if (! drupal_strlen($form_state['values']['imagepicker_insert_defaults_link'])) {
    form_set_error('imagepicker_insert_defaults_link', t('You must fill in the default link field'));
  }

  // watermark settings
  if (imagepicker_image_check_functions()) {
    if (isset($form_state['values']['imagepicker_watermark_enable']) && $form_state['values']['imagepicker_watermark_enable']) {
      if (! empty($form_state['values']['imagepicker_watermark_image']) && ! file_exists($form_state['values']['imagepicker_watermark_image'])) {
        form_set_error('imagepicker_watermark_image', t('This file does not exist or has invalid path: %file.', array('%file' => $form_state['values']['imagepicker_watermark_image'])));
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
    }
  }

  if (! empty($form_state['values']['imagepicker_exifinfo_external']) && ! is_executable($form_state['values']['imagepicker_exifinfo_external'])) {
    form_set_error('imagepicker_exifinfo_external', t('file does not exist or is not executable'));
  }

  if (isset($form_state['values']['imagepicker_lightbox2_enable']) && $form_state['values']['imagepicker_lightbox2_enable']) {
    if (! drupal_strlen($form_state['values']['imagepicker_lightbox2_insert'])) {
      form_set_error('imagepicker_lightbox2_insert', t('You must fill in the Lightbox2 insert'));
    }
  }
  if (! is_numeric($form_state['values']['imagepicker_import_max']) || ($form_state['values']['imagepicker_import_max'] < 1 || $form_state['values']['imagepicker_import_max'] > 100 )) {
    form_set_error('imagepicker_import_max', t('The Import batch size must be must be a positive number.'));
  }

  if ($form_state['values']['imagepicker_show_browse_order_form'] != 1) {
    imagepicker_variable_del('imagepicker_browser_order', $user->uid);
  }

  if ($form_state['values']['imagepicker_groups_enabled'] != 1) {
    $form_state['values']['imagepicker_public_enabled'] = 0;
    $form_state['values']['imagepicker_publicroles_enabled'] = 0;
    $form_state['values']['imagepicker_groups_in_upload_enabled'] = 0;
  }
  if ($form_state['values']['imagepicker_quota_enable'] != 1) {
    $form_state['values']['imagepicker_quota_byrole'] = 0;
  }
  // gallery blocks
  if (! isset($form_state['values']['imagepicker_galleryblocks_enabled'])) {
    imagepicker_variable_del('imagepicker_galleryblocks_enabled');
    imagepicker_variable_del('imagepicker_galleryblocks_howmany');
  }
  if (! module_exists('image')) {
    imagepicker_variable_del('imagepicker_image_enable');
    imagepicker_variable_del('imagepicker_image_default_link');
    imagepicker_variable_del('imagepicker_image_default_show');
  }
}

function imagepicker_settings_form_submit($form, &$form_state) {

  imagepicker_variable_set('imagepicker_node_types_enabled', $form_state['values']['imagepicker_node_types_enabled']);
  if (module_exists('comment')) {
    imagepicker_variable_set('imagepicker_comment_enabled', $form_state['values']['imagepicker_comment_enabled']);
  }
  imagepicker_variable_set('imagepicker_blocks_enabled', $form_state['values']['imagepicker_blocks_enabled']);
  imagepicker_variable_set('imagepicker_account_enabled', $form_state['values']['imagepicker_account_enabled']);
  imagepicker_variable_set('imagepicker_user_config_enable', $form_state['values']['imagepicker_user_config_enable']);
  imagepicker_variable_set('imagepicker_copy_enabled', $form_state['values']['imagepicker_copy_enabled']);
  imagepicker_variable_set('imagepicker_use_full_url', $form_state['values']['imagepicker_use_full_url']);
  imagepicker_variable_set('imagepicker_use_cssbox', $form_state['values']['imagepicker_use_cssbox']);
  imagepicker_variable_set('imagepicker_use_icons', $form_state['values']['imagepicker_use_icons']);
  imagepicker_variable_set('imagepicker_max_uploads', $form_state['values']['imagepicker_max_uploads']);
  imagepicker_variable_set('imagepicker_invoke_extra', $form_state['values']['imagepicker_invoke_extra']);
  imagepicker_variable_set('imagepicker_default_thumbnail_size', $form_state['values']['imagepicker_default_thumbnail_size']);
  imagepicker_variable_set('imagepicker_default_scale', $form_state['values']['imagepicker_default_scale']);
  imagepicker_variable_set('imagepicker_default_pagelink', $form_state['values']['imagepicker_default_pagelink']);
  imagepicker_variable_set('imagepicker_rows_per_page', $form_state['values']['imagepicker_rows_per_page']);
  imagepicker_variable_set('imagepicker_dateformat_default', $form_state['values']['imagepicker_dateformat_default']);
  imagepicker_variable_set('imagepicker_insert_defaults_align', $form_state['values']['imagepicker_insert_defaults_align']);
  imagepicker_variable_set('imagepicker_insert_defaults_show', $form_state['values']['imagepicker_insert_defaults_show']);
  imagepicker_variable_set('imagepicker_insert_defaults_link', $form_state['values']['imagepicker_insert_defaults_link']);
  imagepicker_variable_set('imagepicker_default_align_show', $form_state['values']['imagepicker_default_align_show']);
  imagepicker_variable_set('imagepicker_default_fleft', $form_state['values']['imagepicker_default_fleft']);
  imagepicker_variable_set('imagepicker_default_fright', $form_state['values']['imagepicker_default_fright']);
  imagepicker_variable_set('imagepicker_default_browser_thumbnail_size', $form_state['values']['imagepicker_default_browser_thumbnail_size']);
  imagepicker_variable_set('imagepicker_advanced_browser_columns', $form_state['values']['imagepicker_advanced_browser_columns']);
  imagepicker_variable_set('imagepicker_advanced_browser_page', $form_state['values']['imagepicker_advanced_browser_page']);
  imagepicker_variable_set('imagepicker_show_browse_order_form', $form_state['values']['imagepicker_show_browse_order_form']);
  imagepicker_variable_set('imagepicker_default_browser_order', $form_state['values']['imagepicker_default_browser_order']);
  imagepicker_variable_set('imagepicker_show_browse_search_form', $form_state['values']['imagepicker_show_browse_search_form']);

  if (isset($form_state['values']['imagepicker_lightbox2_enable'])) {
    imagepicker_variable_set('imagepicker_lightbox2_enable', $form_state['values']['imagepicker_lightbox2_enable']);
    if (imagepicker_variable_get('imagepicker_lightbox2_enable', 0)) {
      imagepicker_variable_set('imagepicker_lightbox2_insert', $form_state['values']['imagepicker_lightbox2_insert']);
    }
  }

  if (isset($form_state['values']['imagepicker_colorbox_enable'])) {
    imagepicker_variable_set('imagepicker_colorbox_enable', $form_state['values']['imagepicker_colorbox_enable']);
    if (variable_get('colorbox_load', 0)) {
      imagepicker_variable_set('imagepicker_advanced_colorbox', $form_state['values']['imagepicker_advanced_colorbox']);
    }
    imagepicker_variable_set('imagepicker_use_relbox', $form_state['values']['imagepicker_use_relbox']);
  }

  if (isset($form_state['values']['imagepicker_image_enable'])) {
    imagepicker_variable_set('imagepicker_image_enable', $form_state['values']['imagepicker_image_enable']);
    imagepicker_variable_set('imagepicker_image_default_link', $form_state['values']['imagepicker_image_default_link']);
    imagepicker_variable_set('imagepicker_image_default_show', $form_state['values']['imagepicker_image_default_show']);
  }

  imagepicker_variable_set('imagepicker_quota_enable', $form_state['values']['imagepicker_quota_enable']);
  imagepicker_variable_set('imagepicker_quota_byrole', $form_state['values']['imagepicker_quota_byrole']);
  imagepicker_variable_set('imagepicker_quota_role', $form_state['values']['imagepicker_quota_role']);
  imagepicker_variable_set('imagepicker_quota_default', $form_state['values']['imagepicker_quota_default']);
  if (isset($form_state['values']['imagepicker_watermark_enable'])) {
    imagepicker_variable_set('imagepicker_watermark_enable', $form_state['values']['imagepicker_watermark_enable']);
    imagepicker_variable_set('imagepicker_watermark_image', $form_state['values']['imagepicker_watermark_image']);
    imagepicker_variable_set('imagepicker_watermark_position', $form_state['values']['imagepicker_watermark_position']);
    imagepicker_variable_set('imagepicker_watermark_scaling', $form_state['values']['imagepicker_watermark_scaling']);
    imagepicker_variable_set('imagepicker_watermark_scaling_percentage', $form_state['values']['imagepicker_watermark_scaling_percentage']);
    imagepicker_variable_set('imagepicker_watermark_min_width', $form_state['values']['imagepicker_watermark_min_width']);
  }
  if (isset($form_state['values']['imagepicker_exifinfo_enable'])) {
    imagepicker_variable_set('imagepicker_exifinfo_enable', $form_state['values']['imagepicker_exifinfo_enable']);
  }
  imagepicker_variable_set('imagepicker_exifinfo_external', $form_state['values']['imagepicker_exifinfo_external']);
  imagepicker_variable_set('imagepicker_advanced_iframe_width', $form_state['values']['imagepicker_advanced_iframe_width']);
  imagepicker_variable_set('imagepicker_advanced_iframe_height', $form_state['values']['imagepicker_advanced_iframe_height']);
  imagepicker_variable_set('imagepicker_advanced_iframe_border', $form_state['values']['imagepicker_advanced_iframe_border']);
  imagepicker_variable_set('imagepicker_advanced_collapsed', $form_state['values']['imagepicker_advanced_collapsed']);
  imagepicker_variable_set('imagepicker_use_admin_theme', $form_state['values']['imagepicker_use_admin_theme']);
  imagepicker_variable_set('imagepicker_upload_progress_enabled', $form_state['values']['imagepicker_upload_progress_enabled']);
  imagepicker_variable_set('imagepicker_upload_progress_delay', $form_state['values']['imagepicker_upload_progress_delay']);
  imagepicker_variable_set('imagepicker_upload_progress_message', $form_state['values']['imagepicker_upload_progress_message']);
  if (isset($form_state['values']['imagepicker_uploadprogress_server'])) {
    imagepicker_variable_set('imagepicker_uploadprogress_server', $form_state['values']['imagepicker_uploadprogress_server']);
    imagepicker_variable_set('imagepicker_uploadprogress_interval', $form_state['values']['imagepicker_uploadprogress_interval']);
  }
  imagepicker_variable_set('imagepicker_groups_enabled', $form_state['values']['imagepicker_groups_enabled']);
  imagepicker_variable_set('imagepicker_public_enabled', $form_state['values']['imagepicker_public_enabled']);
  imagepicker_variable_set('imagepicker_publicroles_enabled', $form_state['values']['imagepicker_publicroles_enabled']);
  imagepicker_variable_set('imagepicker_groups_in_upload_enabled', $form_state['values']['imagepicker_groups_in_upload_enabled']);
  imagepicker_variable_set('imagepicker_import_enabled', $form_state['values']['imagepicker_import_enabled']);
  imagepicker_variable_set('imagepicker_import_delete', $form_state['values']['imagepicker_import_delete']);
  imagepicker_variable_set('imagepicker_import_max', $form_state['values']['imagepicker_import_max']);

  if (isset($form_state['values']['imagepicker_galleryblocks_enabled'])) {
    // gallery blocks
    imagepicker_variable_set('imagepicker_galleryblocks_enabled', $form_state['values']['imagepicker_galleryblocks_enabled']);
    imagepicker_variable_set('imagepicker_galleryblocks_howmany', $form_state['values']['imagepicker_galleryblocks_howmany']);
    imagepicker_variable_set('imagepicker_galleryblocks_public', $form_state['values']['imagepicker_galleryblocks_public']);
    $howmany = imagepicker_variable_get('imagepicker_galleryblocks_howmany', 1);
    if ($howmany) {
      for ($i = 1; $i <= $howmany; $i++) {
        if (isset($form_state['values']['imagepicker_galleryblocks_enable_' . $i])) {
          imagepicker_variable_set('imagepicker_galleryblocks_enable_' . $i, $form_state['values']['imagepicker_galleryblocks_enable_' . $i]);
        }
      }
    }
  }
}

/**
 * Administration functions
 */
function imagepicker_admin_users_list($dir, $label) {

  $max = imagepicker_variable_get('imagepicker_rows_per_page', 25);
  $header = array(
    array('data' => t('Name'), 'field' => 'u.name'),
    array('data' => t('Images'), 'field' => 'ct'),
    (imagepicker_variable_get('imagepicker_groups_enabled', 1) ? t('Groups') : ''),
    t('Quotas'),
    t('Used'),
  );
  $rowcount = 0;
  $rows = NULL;
  $countquery = db_select('users', 'u');
  $countquery->addExpression('COUNT(DISTINCT u.uid)', 'ct');
  $countquery->join('imagepicker', 'i', 'i.uid=u.uid');
  $countquery->condition('u.status', 1);

  $query = db_select('users', 'u')
    ->extend('PagerDefault')->limit($max)
    ->extend('TableSort')->orderByHeader($header);
  $query->setCountQuery($countquery);
  $query->distinct()
    ->fields('u', array('uid', 'name'))
    ->addExpression('COUNT( i.img_id)', 'ct');
  $query->join('imagepicker', 'i', 'i.uid=u.uid');
  $query->condition('u.status', 1)
    ->groupBy('u.uid')
    ->groupBy('u.name');
  $records = $query->execute();

  foreach ($records AS $row) {
    $account = user_load($row->uid);
    #$allcount = _imagepicker_user_has_img($account);
    $allcount = $row->ct;
    $groupcount = _imagepicker_user_has_groups('all', $account);
    $usedbytesprint = _imagepicker_bkmg(imagepicker_get_all_bytes($account)+1);
    $quota = 0;
    if (imagepicker_variable_get('imagepicker_quota_enable', 1)) {
      if (imagepicker_variable_get('imagepicker_quota_byrole', 0)) {
        $roleid = imagepicker_variable_get('imagepicker_quota_role', 2);
        if (imagepicker_user_has_role($roleid, $account) && $account->uid > 1) {
          $quota = imagepicker_variable_get('imagepicker_quota_default', 0);
        }
      }
      else {
        $quota = imagepicker_variable_get('imagepicker_quota_default', imagepicker_variable_get('imagepicker_quota_default', 0), $account->uid);
      }
    }

    $row_data = array(
      check_plain($row->name),
      ($allcount ? l(format_plural($allcount, '1 image', '@count images'), IMAGEPICKER_ADMIN_PATH . '/images/user/' . $row->uid . '/browse' ) : t('No images')),
      (imagepicker_variable_get('imagepicker_groups_enabled', 1) ? ($groupcount ? (l(format_plural($groupcount, '1 group', '@count groups'), IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid . '/browse' )) : t('No groups')) :  t('n/a')),
      (imagepicker_variable_get('imagepicker_quota_enable', 1) ? l(imagepicker_get_quota_list($quota+1), IMAGEPICKER_ADMIN_PATH . '/images/user/' . $row->uid . '/quota') : t('n/a')),
      $usedbytesprint,
    );
    $rows[] = $row_data;
    $rowcount++;
  }

  $message = t('No users found.');
  $content = theme('imagepicker_list', array('header' => $header, 'rows' => $rows, 'max' => $max, 'message' => $message, 'pref' => '', 'suff' => '', 'label' => $label));
  return $content;
}

function imagepicker_admin_groups_list($label="") {

  $max = imagepicker_variable_get('imagepicker_advanced_browser_page', 25);
  $use_icons = imagepicker_variable_get('imagepicker_use_icons', 1);
  $rows = NULL;
  $header = array(
    array('data' => t('Group Name'), 'field' => 'g.group_name'),
    array('data' => t('User Name'), 'field' => 'u.name'),
    array('data' => t('Status'), 'field' => 'g.public'),
    array('data' => t('Images'), 'field' => 'ct'),
    array('data' => t('Actions'), 'colspan' => 2),
  );
  $countquery = db_select('imagepicker_user_groups', 'g');
  $countquery->addExpression('COUNT(g.gid)', 'ct');

  $query = db_select('imagepicker_user_groups', 'g')
    ->extend('PagerDefault')->limit($max)
    ->extend('TableSort')->orderByHeader($header);
  $query->setCountQuery($countquery);
  $query->fields('g', array('gid', 'uid', 'group_name', 'public'))
    ->addField('u', 'name');
  $query->addExpression('COUNT(i.img_id)', 'ct');
  $query->leftjoin('imagepicker_group_images', 'i', 'g.gid = i.gid');
  $query->leftjoin('users', 'u', 'g.uid = u.uid');
  $query->groupBy('g.gid');
  $records = $query->execute();

  foreach ($records AS $row) {
    // get image count
    $ict = $row->ct;
    $editpath = IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid . '/edit/' . $row->gid;
    $deletepath = IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid . '/delete/' . $row->gid;
    $row_data = array(
      $row->group_name,
      l($row->name, IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid),
      ($row->public ? t('Public') : t('Private')),
      ($ict ? l(format_plural($ict, '1 image', '@count images'), IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $row->uid . '/browse/' . $row->gid) : t('No images')),
      ($use_icons ? _imagepicker_get_icon('edit', $editpath, array('title' => t('Edit'))) : l(t('Edit'), $editpath)),
      ($use_icons ?  _imagepicker_get_icon('delete', $deletepath, array('title' => t('Delete'))) : l(t('Delete'), $deletepath)),
    );
    $rows[] = $row_data;
  }
  $pref = '';
  $suff = '';
  return theme('imagepicker_list', array('header' => $header, 'rows' => $rows, 'max' => $max, 'message' => t('No groups found.'), 'pref' => $pref, 'suff' => $suff, 'label' => $label));
}

function imagepicker_admin_view($img_id, $account = FALSE) {

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

  drupal_add_js(IMAGEPICKER_PATH . '/imagepicker.js');
  $img = _imagepicker_get_img($img_id, FALSE, $user);
  if ($img) {
    $imgpath = imagepicker_get_image_path($img, 'full', $userdir);
    $info = image_get_info(imagepicker_get_path(FALSE, $userdir) . $img->img_name);
    $exifinfo = "";
    if ($info['extension'] != 'gif' ) {
      $exifinfo = imagepicker_get_exifinfo(imagepicker_get_path(FALSE, $userdir) . $img->img_name);
    }
    $view = theme('imagepicker_view', array('img' => $img, 'imgpath' => $imgpath, 'info' => $info, 'exifinfo' => $exifinfo));
    $form1 = render(drupal_get_form('imagepicker_admin_image_form', $img_id, $user));
    // copy form
    if (imagepicker_variable_get('imagepicker_copy_enabled', 0)) {
      $form2 = render(drupal_get_form('imagepicker_copy_form', $img->img_id, $img->img_name, $user));
    }
    // group form
    if (_imagepicker_has_groups($user)) {
      $form3 = render(drupal_get_form('imagepicker_group_images_form', $img->img_id, $user));
    }
    $content = theme('imagepicker_admin_view', array('view' => $view, 'form1' => $form1, 'form2' => $form2, 'form3' => $form3));
  }
  else {
    drupal_set_message(t('Image not found.'), 'error');
    $content = '';
  }
  return $content;
}

/**
 * Function to display the image edit/delete form
 * @return
 *   Returns the form.
 */
function imagepicker_admin_image_form($form, &$form_state, $img_id, $account = FALSE) {

  if ($account) {
    $form['account'] = array(
      '#type' => 'value',
      '#value' => $account->uid,
    );
  }
  $form['img_id'] = array(
    '#type' => 'value',
    '#value' => $img_id,
  );
  $form['edit'] = array(
    '#type' => 'submit',
    '#value' => t('Edit image'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete image'),
  );
  return $form;
}

function imagepicker_admin_image_form_submit($form, &$form_state) {

  if ($form_state['values']['op'] == t('Delete image')) {
    $account = user_load($form_state['values']['account']);
    imagepicker_image_delete($form_state['values']['img_id'], $account, 'admin');
  }
  elseif ($form_state['values']['op'] == t('Edit image')) {
    drupal_goto(IMAGEPICKER_ADMIN_PATH . '/images/user/' . $form_state['values']['account'] . '/edit/' . $form_state['values']['img_id']);
  }
}

/**
 * Function to display the imagepicker user search form
 * @return
 *   Returns the form.
 */
function imagepicker_user_search_form($form, &$form_state, $dir) {

  $form['imagepicker_currentuser'] = array(
    '#type' => 'textfield',
    '#title' => t('Users'),
    '#autocomplete_path' => IMAGEPICKER_ADMIN_PATH . '/' . $dir . '/autocomplete',
    '#default_value' => '',
    '#maxlength' => 30,
    '#size' => 25,
  );
  $form['dir'] = array(
    '#type' => 'value',
    '#value' => $dir,
  );
  $form['imagepicker_user_search_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );
  return $form;
}

function imagepicker_user_search_form_submit($form, &$form_state) {

  if ($form_state['values']['imagepicker_currentuser']) {
    $currentuser = $form_state['values']['imagepicker_currentuser'];
    if ($form_state['values']['dir'] == 'import') {
      $query = db_select('users', 'u');
      $query->fields('u', array('uid'));
    }
    else {
      $query = db_select('users', 'u');
      $query->fields('u', array('uid'));
      $query->join('imagepicker', 'i', 'u.uid = i.uid');
    }
    $query->condition('u.name', $currentuser);
    $row = $query->execute()->fetchAssoc();
    if ($row['uid']) {
      imagepicker_variable_set('imagepicker_currentuser', $row['uid']);
      drupal_goto(IMAGEPICKER_ADMIN_PATH . '/' . $form_state['values']['dir'] . '/user/' . $row['uid']);
    }
    else {
      imagepicker_variable_del('imagepicker_currentuser');
    }
  }
  else {
    imagepicker_variable_del('imagepicker_currentuser');
  }
}

function imagepicker_check_orphans($dir) {
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
        $query = db_select('imagepicker');
        $query->addExpression('COUNT(img_id)', 'ct');
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

function imagepicker_admin_orphans() {
  // show a form if applicable

  $content = drupal_get_form('imagepicker_admin_orphans_form');
  return $content;

}

function imagepicker_admin_orphans_form($form_state) {

  // show a dropdown of nonexistent user ids
  // two choices:
  // reallocate to another user
  // delete the images

  $dir = imagepicker_get_files_directory();
  $orphanids = imagepicker_check_orphans($dir);
  if ($orphanids) {
    foreach ($orphanids AS $v) {
      $opt[$v] = $v;
    }
    $form['orphanids'] = array(
      '#type' => 'select',
      '#options' => $opt,
      '#title' => t('Nonexistent user ids'),
      '#description' => t('These ids no longer exist but still have images. Choose what to do below'),
    );
    $form['reallocate_to_user'] = array(
      '#type' => 'textfield',
      '#title' => t('Reallocate to'),
      '#description' => t('Select a user who will own these images.'),
      '#autocomplete_path' => IMAGEPICKER_ADMIN_PATH . '/orphans/autocomplete',
      '#default_value' => '',
      '#maxlength' => 30,
      '#size' => 25,
    );
    $form['orphans_delete'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete the images'),
      '#return_value' => 1,
    );
    $form['msg'] = array(
      '#markup' => t("Any content using these images will be broken unless removed or edited."),
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

function imagepicker_admin_orphans_form_submit($form, &$form_state) {

  $result = imagepicker_admin_orphans_do($form_state['values']['orphanids'], $form_state['values']['orphans_delete'], $form_state['values']['reallocate_to_user']);
  if (is_array($result)) {
    $message = $result[0];
    $status = $result[1];
    drupal_set_message($message, ($status ? 'status' : 'error'));
  }
  drupal_goto(IMAGEPICKER_ADMIN_PATH);

}

function imagepicker_admin_orphans_do($orphanid, $orphans_delete=FALSE, $reallocate_to_user='') {

  $OK = FALSE;
  $message = '';
  $return = '';
  // check that the incoming user id is nonexistent
  $dir = imagepicker_get_files_directory();
  $orphanids = imagepicker_check_orphans($dir);
  // sanity check
  if (in_array($orphanid, $orphanids)) {
    // delete images
    if ($orphans_delete == 1) {
      // create 'phony' user object
      $user_obj = new stdClass;
      $user_obj->uid = $orphanid;
      // get all image ids belonging to orphanids
      $ct = 0;
      $result = db_query('SELECT img_id FROM {imagepicker} WHERE uid=:uid', array(':uid' => $user_obj->uid));
      while ($row = $result->fetchAssoc()) {
        _imagepicker_image_delete($row['img_id'], $user_obj, 'admin', TRUE);
        imagepicker_delete_group_image($row['img_id']);
        $ct++;
      }
      $OK = imagepicker_delete_olduser_dirs($user_obj->uid);
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
        $result2 = db_query('SELECT img_id FROM {imagepicker} WHERE uid=:uid', array(':uid' => $orphanid));
        while ($row2 = $result2->fetchAssoc()) {
          $newf = imagepicker_reallocate_image($row2['img_id'], $orphanid, $new_uid);
          if ($newf) {
            // move this entry in imagepicker to $new_uid
            $img_name = basename($newf);
            db_update('imagepicker')
              ->fields(array('uid' => $new_uid, 'img_name' => $img_name))
              ->condition('img_id', $row2['img_id'])
              ->execute();
            $ct++;
          }
        }
        // give the new user the groups if any
        db_update('imagepicker_user_groups')
          ->fields(array('uid' => $new_uid))
          ->condition('uid', $orphanid)
          ->execute();
        $OK = imagepicker_delete_olduser_dirs($orphanid);
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

function imagepicker_reallocate_image($img_id, $old_uid, $new_uid) {

  $OK = FALSE;
  // $old_uid might not exist in users table but still have images
  $old_user_obj = new stdClass;
  $old_user_obj->uid = $old_uid;
  $img = _imagepicker_get_img($img_id, FALSE, $old_user_obj);
  if ($img) {
    $old_userdir = array('uid' => $old_user_obj->uid);
    $source = imagepicker_get_path(FALSE, $old_userdir);
    $source_thumbsdir = $source . IMAGEPICKER_THUMBS_DIR . DIRECTORY_SEPARATOR . $img->img_name;
    $source_browserdir = $source . IMAGEPICKER_BROWSER_DIR . DIRECTORY_SEPARATOR . $img->img_name;
    $source_origdir = $source . IMAGEPICKER_ORIG_DIR . DIRECTORY_SEPARATOR . $img->img_name;
    $source = $source . $img->img_name;

    $new_user_obj = user_load($new_uid);
    $new_userdir = array('uid' => $new_user_obj->uid);
    $dest = imagepicker_get_path(FALSE, $new_userdir);
    $dest_thumbsdir = $dest . IMAGEPICKER_THUMBS_DIR . DIRECTORY_SEPARATOR;
    $dest_browserdir = $dest . IMAGEPICKER_BROWSER_DIR . DIRECTORY_SEPARATOR;
    $dest_origdir = $dest . IMAGEPICKER_ORIG_DIR . DIRECTORY_SEPARATOR;
    $OK = imagepicker_file_unmanaged_move($source_thumbsdir, $dest_thumbsdir);
    $OK = imagepicker_file_unmanaged_move($source_browserdir, $dest_browserdir);
    $OK = imagepicker_file_unmanaged_move($source_origdir, $dest_origdir);
    $OK = imagepicker_file_unmanaged_move($source, $dest);
  }

  return $OK;
}

function imagepicker_delete_olduser_dirs($old_uid) {

  $OK = FALSE;
  // $old_uid might not exist in users table but still have images
  $old_userdir = array('uid' => $old_uid);
  $source = imagepicker_get_path(FALSE, $old_userdir);
  $source_thumbsdir = $source . IMAGEPICKER_THUMBS_DIR;
  $source_browserdir = $source . IMAGEPICKER_BROWSER_DIR;
  $source_origdir = $source . IMAGEPICKER_ORIG_DIR;
  $OK = drupal_rmdir($source_origdir);
  $OK = drupal_rmdir($source_browserdir);
  $OK = drupal_rmdir($source_thumbsdir);
  $OK = drupal_rmdir($source);

  return $OK;
}

function imagepicker_admin_groups() {

  drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');
  drupal_add_js(IMAGEPICKER_PATH . '/imagepicker.js');
  module_load_include('inc', 'imagepicker', 'imagepicker.group');
  $content = '';
  $action = '';
  $rows = NULL;
  $gid = FALSE;
  if (! arg(5) ) {
    imagepicker_variable_del('imagepicker_currentuser');
    imagepicker_variable_set('imagepicker_currentgroup', 0);
    $action = 'users';
  }
  $uid = imagepicker_variable_get('imagepicker_currentuser', '');
  if ( arg(5) == 'user' && is_numeric(arg(6))) {
    $uid = arg(6);
  }
  elseif (arg(5) == 'stats' || arg(5) == 'users') {
    $action = arg(5);
  }
  if ( (arg(7) == 'browse' || arg(7) == 'edit' || arg(7) == 'delete') && is_numeric(arg(8)) ) {
    $gidaction = arg(7);
    $gid = arg(8);
    if (arg(7) == 'browse') {
      imagepicker_variable_set('imagepicker_currentgroup', $gid);
      drupal_goto(IMAGEPICKER_ADMIN_PATH . '/images/user/' . $uid . '/browse');
    }
  }
  $name = "";
  if ($uid) {
    $account = user_load($uid);
    $name = $account->name;
  }
  if (imagepicker_variable_get('imagepicker_groups_enabled', 1)) {
    $content .= render(drupal_get_form('imagepicker_group_search_form', 'groups'));

    $content .= '<p>' . t('Managing groups for !n', array('!n' => ($name ? l($name, "user/$uid") : t('All')))) . '</p>';
    if ($uid) {
      // show groups for $uid
      if ($gid) {
        if ($gidaction == 'edit') {
          $record = imagepicker_get_user_group($gid);
          $content .= render(drupal_get_form('imagepicker_groups_form', $record));
        }
        elseif ($gidaction == 'delete') {
          $content .= render(drupal_get_form('imagepicker_group_delete_form', $gid));
        }
      }
      elseif (arg(6) == 'stats') {
        $content .= imagepicker_group_stats($account, t('Statistics for %name', array('%name' => $name)));
      }
      else {
        $content .= imagepicker_groups_list('admin', $account, t('Groups for %name', array('%name' => $name)));
        $content .= render(drupal_get_form('imagepicker_groups_form', 0, $account));
      }
    }
    else {
      if ($action == 'stats') {
        $content .= imagepicker_group_stats(FALSE, t('Statistics'));
      }
      else {
        $content .= imagepicker_admin_groups_list(t('Groups'));
      }
    }
  }
  return $content;
}

function imagepicker_admin_images() {

  drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');
  $action = FALSE;
  $imgaction = FALSE;
  $img_id = FALSE;
  $content = '';
  if (! arg(5) ) {
    imagepicker_variable_del('imagepicker_currentuser');
    imagepicker_variable_set('imagepicker_currentgroup', 0);
    $action = 'users';
  }
  $uid = imagepicker_variable_get('imagepicker_currentuser', '');
  if ( arg(5) == 'user' && is_numeric(arg(6))) {
    $uid = arg(6);
  }
  elseif (arg(5) == 'browse_public' || arg(5) == 'stats' || arg(5) == 'users' || arg(5) == 'list_public') {
    $action = arg(5);
    imagepicker_variable_del('imagepicker_currentuser');
    imagepicker_variable_set('imagepicker_currentgroup', 0);
    $uid = imagepicker_variable_get('imagepicker_currentuser', '');
  }
  if ( arg(7) == 'browse_public'
    || arg(7) == 'browse'
    || arg(7) == 'edit'
    || arg(7) == 'delete'
    || arg(7) == 'browseadmin'
    || arg(7) == 'upload'
    || arg(7) == 'stats'
    || arg(7) == 'groups'
    || arg(7) == 'quota') {
    $imgaction = arg(7);
  }
  if ( is_numeric(arg(8))) {
    $img_id = arg(8);
  }
  if (arg(7) == 'upload' ) {
    module_load_include('inc', 'imagepicker', 'imagepicker.upload');
    if (imagepicker_variable_get('imagepicker_upload_progress_enabled', 1)) {
      $content .= imagepicker_upload_progress_get_script(imagepicker_variable_get('imagepicker_upload_progress_delay', 0));
    }
  }
  $name = "";
  if ($uid) {
    $account = user_load($uid);
    $name = $account->name;
  }
  $content .= render(drupal_get_form('imagepicker_user_search_form', 'images'));
  $content .= '<p>' . t('Managing images for !n', array('!n' => ($name ? l($name, "user/$uid") : t('All')))) . '</p>';
  if ($uid) {
    if ($img_id) {
      if ($imgaction == 'edit' ) {
        module_load_include('inc', 'imagepicker', 'imagepicker.edit');
        $img =  _imagepicker_get_img($img_id, FALSE, $account);
        $imgsrc = imagepicker_get_image_path($img, 'browser', array('uid' => $account->uid , 'name' => $account->name) );
        $content .= theme('imagepicker_image_edit_header', array('image' => $img, 'source' => $imgsrc));
        $content .= render(drupal_get_form('imagepicker_edit_form', $img, 'admin', $account));
      }
      elseif ($imgaction == 'delete') {
        imagepicker_image_delete($img_id, $account, 'admin');
      }
      else {
        $content .= imagepicker_admin_view($img_id, $account);
      }
    }
    elseif ($imgaction == 'upload') {
      $content .= imagepicker_quota_ok('admin', $account, t('Upload file for %name', array('%name' => $name)));
    }
    elseif ($imgaction == 'stats') {
      $content .= imagepicker_group_stats($account, t('Statistics for %name', array('%name' => $name)));
    }
    elseif ($imgaction == 'browseadmin') {
      $content .= _imagepicker_browse_admin('admin', $account, FALSE, 1, t('List images for %name', array('%name' => $name)));
    }
    elseif ($imgaction == 'quota') {
      $usedbytes = imagepicker_get_all_bytes($account)+1;
      $usedbytesprint = _imagepicker_bkmg($usedbytes);
      $quota = 0;
      if (imagepicker_variable_get('imagepicker_quota_enable', 1)) {
        if (imagepicker_variable_get('imagepicker_quota_byrole', 0)) {
          $roleid = imagepicker_variable_get('imagepicker_quota_role', 2);
          if (imagepicker_user_has_role($roleid, $account) && $account->uid > 1) {
            $quota = imagepicker_variable_get('imagepicker_quota_default', 0);
          }
        }
        else {
          $quota = imagepicker_variable_get('imagepicker_quota_default', imagepicker_variable_get('imagepicker_quota_default', 0), $account->uid);
        }
      }
      $label = t('Quota for %name', array('%name' => $name));
      $imgtot = _imagepicker_user_has_img($account);
      $pl = format_plural($imgtot, '1 image', '@count images');
      if ($quota) {
        $pused = round($usedbytes / ($quota*1000000) * 100, 2);
        $message = t('User %name has used %pused percent or %usedbytesprint of quota %quotaprint and has %pl.', array('%name' => $name, '%pused' => $pused, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl, '%quotaprint' => imagepicker_get_quota_list($quota+1)));
      }
      else {
        $message = t('User %name has used %usedbytesprint and has %pl.', array('%name' => $name, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
      }
      if (! imagepicker_variable_get('imagepicker_quota_byrole', 0) ) {
        $form = render(drupal_get_form('imagepicker_quota_form', $account));
      }
      else {
        $form = "";
      }
      $content .= theme('imagepicker_quota', array('quotaform' => $form, 'message' => $message, 'label' => $label));
    }
    elseif ($imgaction == 'groups') {
      drupal_goto(IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $uid);
    }
    else {
      $content .= _imagepicker_browse('admin', $account, t('Browse images for %name', array('%name' => $name)));
    }
  }
  else {
    if ($action == 'stats') {
      $content .= imagepicker_group_stats(FALSE, t('Statistics'));
    }
    elseif ($action == 'browse_public') {
      $content .= _imagepicker_browse_public('admin', imagepicker_variable_get('imagepicker_browse_public', 0), t('Browse All Images'));
    }
    elseif ($action == 'list_public') {
      $content .= _imagepicker_browse_admin('admin', FALSE, TRUE, imagepicker_variable_get('imagepicker_browse_public', 0), t('List All Images'));
    }
    else {
      $label = t('Users');
      $content .= imagepicker_admin_users_list('images', $label);
    }
  }
  return $content;
}

/**
 * Function to display the imagepicker group search form
 * @return
 *   Returns the form.
 */
function imagepicker_group_search_form($form, &$form_state, $dir = 'groups') {

  $form['imagepicker_currentgroup'] = array(
    '#type' => 'textfield',
    '#title' => t('Groups'),
    '#autocomplete_path' => IMAGEPICKER_ADMIN_PATH . '/groups/autocomplete',
    '#default_value' => '',
    '#maxlength' => 30,
    '#size' => 25,
  );
  $form['dir'] = array(
    '#type' => 'value',
    '#value' => $dir,
  );
  $form['imagepicker_group_search_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );
  return $form;
}

function imagepicker_group_search_form_submit($form, &$form_state) {

  if ($form_state['values']['imagepicker_currentgroup']) {
    $currentgroup = $form_state['values']['imagepicker_currentgroup'];
    $query = db_select('imagepicker_user_groups', 'iug');
    $query->fields('iug', array('gid', 'uid'));
    $query->condition('iug.group_name', $currentgroup);
    $row = $query->execute()->fetchAssoc();
    if ($row['gid'] ) {
      imagepicker_variable_set('imagepicker_currentgroup', $row['gid']);
      drupal_goto(IMAGEPICKER_ADMIN_PATH . '/' . $form_state['values']['dir'] . '/user/' . $row['uid'] . '/edit/' . $row['gid']);
    }
    else {
      imagepicker_variable_set('imagepicker_currentgroup', 0);
    }
  }
  else {
    imagepicker_variable_set('imagepicker_currentgroup', 0);
  }
}

/**
 * Function to display the imagepicker quota form
 * @return
 *   Returns the form.
 */
function imagepicker_quota_form($form, &$form_state, $account=FALSE) {

  $list = imagepicker_get_quota_list();
  $default = imagepicker_variable_get('imagepicker_quota_default', 0);
  $quota = $default;
  $title = t('Set default quota');
  if ($account) {
    $title = t('Set user quota');
    $quota = (imagepicker_variable_get('imagepicker_quota_default', 0, $account->uid) ? imagepicker_variable_get('imagepicker_quota_default', 0, $account->uid) : 0);
    if (! $quota) {
      $quota = $default;
    }
  }
  $form['imagepicker_quota'] = array(
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

function imagepicker_quota_form_submit($form, &$form_state) {

    if ( $form_state['values']['account'] > 0 ) {
      $account = user_load($form_state['values']['account']);
      imagepicker_variable_set('imagepicker_quota_default', $form_state['values']['imagepicker_quota'], $account->uid);
    }
    else {
      imagepicker_variable_set('imagepicker_quota_default', $form_state['values']['imagepicker_quota']);
    }
}

function imagepicker_admin_import() {

  drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');
  module_load_include('inc', 'imagepicker', 'imagepicker.import');
  if (! arg(4) ) {
    imagepicker_variable_del('imagepicker_currentuser');
  }
  $uid = imagepicker_variable_get('imagepicker_currentuser', '');
  if ( arg(4) == 'user' && is_numeric(arg(5))) {
    $uid = arg(5);
  }
  $content = "";
  if (! $importdir = imagepicker_variable_get('imagepicker_import_dir', '')) {
    drupal_set_message(t('You need to set the import folder'));
  }
  $content = render(drupal_get_form('imagepicker_import_dir_form'));
  if ($importdir) {
    if ($import_files = imagepicker_import_dir_check()) {
      $content .= render(drupal_get_form('imagepicker_user_search_form', 'import'));
      if ($uid) {
        $account = user_load($uid);
        $content .= render(drupal_get_form('imagepicker_import_form', count($import_files), $account, TRUE));
      }
      else {
        drupal_set_message(t('Please select a user. You have %total files to be imported', array('%total' => count($import_files)) ));
      }
    }
  }

  if ( isset($_SESSION['imagepicker_import_status'])) {
    drupal_get_messages('status', TRUE);
    drupal_set_message($_SESSION['imagepicker_import_status']);
    unset($_SESSION['imagepicker_import_status']);
  }
  return $content;
}

/**
 * Retrieve a pipe delimited string of autocomplete suggestions for existing users
 */
function imagepicker_user_autocomplete($string = '') {

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
      $query->join('imagepicker', 'i', 'u.uid = i.uid');
      $query->where("LOWER(u.name) LIKE LOWER(:st)", array(':st' => $string . '%') );
      $query->condition('u.status', 1);
      $query->range(0, 10);
    }
    $result = $query->execute();
    foreach ($result as $account) {
      if ((arg(4) == 'import') OR (arg(4) == 'orphans')) {
        $user = user_load($account->uid);
        if (user_access('use imagepicker', $user)) {
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
function imagepicker_group_search_autocomplete($string = '') {

  $matches = array();
  if ($string) {
    $query = db_select('imagepicker_user_groups', 'iug');
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
