<?php

/**
 * @file
 * Themimg functions for imagepicker.
 */

/**
 * theme registry
 */
function imagepicker_theme() {

  return array(
    'imagepicker' => array(
      'variables' => array('content' => NULL),
      'template' => 'imagepicker',
    ),
    'imagepicker_iframe' => array(
      'variables' => array(
        'content' => NULL,
        'img'  => NULL,
        'public'  => NULL,
      ),
    ),
    'imagepicker_list' => array(
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
    'imagepicker_stats' => array(
      'variables' => array(
        'header' => array(),
        'rows' => array(),
        'pref' => "",
        'suff' => "",
        'label' => "",
      ),
    ),
    'imagepicker_browser' => array(
      'variables' => array(
        'content' => array(),
        'forms' => array(),
        'message' => "",
        'help' => "",
        'label' => "",
      ),
    ),
    'imagepicker_fullpage' => array(
      'variables' => array(
        'image' => array(),
        'source' => "",
        'link' => 1,
      ),
    ),
    'imagepicker_quota_message' => array(
      'variables' => array(
        'message1' => "",
        'message2' => "",
        'form' => NULL,
        'label' => "",
        'help' => "",
      ),
    ),
    'imagepicker_view' => array(
      'variables' => array(
        'img' => array(),
        'imgpath' => "",
        'info' => array(),
        'exifinfo' => array(),
      ),
    ),
    'imagepicker_image_edit_header' => array(
      'variables' => array(
        'image' => array(),
        'source' => "",
      ),
    ),
    'imagepicker_quota' => array(
      'variables' => array(
        'form' => NULL,
        'message' => "",
        'label' => "",
      ),
    ),
    'imagepicker_user_config' => array(
      'variables' => array(
        'form' => NULL,
        'label' => "",
        'help' => "",
        'message1' => "",
        'message2' => "",
      ),
    ),
    'imagepicker_user_view' => array(
      'variables' => array(
        'view' => "",
        'form1' => NULL,
        'form2' => NULL,
        'form3' => NULL,
      ),
    ),
    'imagepicker_admin_view' => array(
      'variables' => array(
        'view' => "",
        'form1' => NULL,
        'form2' => NULL,
        'form3' => NULL,
      ),
    ),
#    'imagepicker_admin_images' => array(
#      'variables' => array(
#        'view' => "",
#        'form1' => NULL,
#        'form2' => NULL,
#        'form3' => NULL,
#      ),
#    ),
    'imagepicker_insert' => array(
      'variables' => array(
        'img' => NULL,
        'public' => NULL,
        'form1' => NULL,
        'form2' => NULL,
      ),
    ),
    'imagepicker_upload_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_user_image_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_user_config_admin_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_admin_image_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_user_search_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_group_search_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_quota_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_edit_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_groups_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_group_delete_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_image_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_browse_groups_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_browse_public_groups_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_group_images_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_browse_order_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_browse_public_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_browse_search_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_settings_form' => array(
      'render element' => 'form'
    ),
    'imagepicker_copy_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_browse_admin_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_browse_admin' => array(
      'variables' => array(
        'forms' => NULL,
        'pref' => '',
        'suff' => '',
        'label' => '',
      ),
    ),
    'imagepicker_multitask_delete_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_multitask_groups_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_import_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_import_dir_form' => array(
      'render element' => 'form',
    ),
    'imagepicker_admin_orphans_form' => array(
      'render element' => 'form',
    ),

  );
}

// preprocess for imagepicker.tpl.php
function template_preprocess_imagepicker(&$variables) {
  global $language;
  if (module_exists('admin_menu')) {
    admin_menu_suppress();
  }

  $variables['head_title']  = (drupal_get_title() ? strip_tags(drupal_get_title()) : variable_get('site_name', 'Drupal'));
  $variables['styles'] = drupal_get_css();
  $variables['scripts'] = drupal_get_js();
  // fix jquery version thing in the iframe
#  if (module_exists('jquery_update')) {
#    jquery_update_preprocess_page($variables);
#  }
  $tabs = menu_local_tabs();
  $variables['tabs'] = theme('menu_local_tasks', array('primary' => $tabs['#primary']));
  $variables['messages'] = imagepicker_strip_messages(theme('status_messages'));
  $variables['language'] = $language;

}

// set up the iframe
function theme_imagepicker_iframe($variables) {
  $content = $variables['content'];
  $img = (isset($variables['img']) ? $variables['img'] : '');
  $public = (isset($variables['public']) ? $variables['public'] : FALSE);

  drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');
  if ($img) {
    global $user;

    $presetfilelink = '';
    $presetthumblink = '';
    if (module_exists('image') && imagepicker_variable_get('imagepicker_image_enable', 0)) {
      $presetfilelink = imagepicker_get_image_path($img, 'full', ($public ? array('name' => $img->name, 'uid' => $img->uid) : FALSE), TRUE);
      $presetthumblink = imagepicker_get_image_path($img, 'thumb', ($public ? array('name' => $img->name, 'uid' => $img->uid) : FALSE), TRUE);
    }

    $img_title = check_plain($img->img_title);
    $settings = array(
      'imagepicker_iframe' => array(
        'imgpImageAlt' => ($img_title ? addslashes($img_title) : t('Image') ),
        'imgpImageTitle' => ($img_title ? addslashes($img_title) : ''),
        'imgpImageDesc' => preg_replace("/(\n|\r)/", '', nl2br($img->img_description)),
        'imgpFileLink' => imagepicker_get_image_path($img, 'full', ($public ? array('name' => $img->name, 'uid' => $img->uid ) : FALSE) ),
        'imgpPresetFileLink' => $presetfilelink,
        'imgpThumbLink' => imagepicker_get_image_path($img, 'thumb', ($public ? array('name' => $img->name, 'uid' => $img->uid ) : FALSE)),
        'imgpPresetThumbLink' => $presetthumblink,
        'imgpPageLink' => url('imagepicker/image/' . $img->img_id),
        'imgpTemplate' => imagepicker_get_insert_template(),
        'isFCKeditor' => (module_exists('fckeditor') ? 'yes' : 'no' ),
        'isWysiwyg' => (module_exists('wysiwyg') ? 'yes' : 'no' ),
        'use_cssbox' => imagepicker_variable_get('imagepicker_use_cssbox', imagepicker_variable_get('imagepicker_use_cssbox', 0), $user->uid),
        'use_relbox' => imagepicker_variable_get('imagepicker_use_relbox', imagepicker_variable_get('imagepicker_use_relbox', 0), $user->uid),
        'default_align_show' => imagepicker_variable_get('imagepicker_default_align_show', imagepicker_variable_get('imagepicker_default_align_show', 1), $user->uid),
        'default_fleft' => imagepicker_variable_get('imagepicker_default_fleft', imagepicker_variable_get('imagepicker_default_fleft', 'style="float: left"'), $user->uid),
        'default_fright' => imagepicker_variable_get('imagepicker_default_fright', imagepicker_variable_get('imagepicker_default_fright', 'style="float: right"'), $user->uid),
        'lightbox2_enable' => (module_exists('lightbox2') && imagepicker_variable_get('imagepicker_lightbox2_enable', 1)),
        'lightbox2_insert' => imagepicker_variable_get('imagepicker_lightbox2_insert', imagepicker_variable_get('imagepicker_lightbox2_insert', 'lightbox'), $user->uid),
        'colorbox_enable' => (module_exists('colorbox') && imagepicker_variable_get('imagepicker_colorbox_enable', 0)),
        'colorbox_iframe' => (imagepicker_get_colorbox_perms()),
        'node_editbody' => 'edit-body-' . imagepicker_variable_get('imagepicker_node_lang', '') . '-' . imagepicker_variable_get('imagepicker_node_pos', 0) . '-value',
      ),
    );
    drupal_add_js($settings, 'setting');
    drupal_add_js(IMAGEPICKER_PATH . '/imagepicker_iframe.js');
  }
  return $content;
}

function theme_imagepicker_list($variables) {

  $header = $variables['header'];
  $rows =  $variables['rows'];
  $max = $variables['max'];
  $message = $variables['message'];
  $pref = $variables['pref'];
  $suff = $variables['suff'];
  $label = $variables['label'];

  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $build['imagepicker_list'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => $message,
  );
  $build['imagepicker_pager'] = array('#theme' => 'pager');
  $output .= $pref . render($build) . $suff;
  $output .= ($label ? '</fieldset>' : '');
  return $output;

}

function theme_imagepicker_stats($variables) {

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

// theme for thumbnail browser
function theme_imagepicker_browser($variables) {

  $content = $variables['content'];
  $forms = $variables['forms'];
  $message = $variables['message'];
  $help = $variables['help'];
  $label = $variables['label'];

  $rows = $content[0];
  $max = $content[1];
  $cols = $content[2];
  $pref1 = $content[3][0];
  $pref2 = $content[3][1];
  $suff1 = $content[4][0];
  $suff2 = $content[4][1];
  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  if ($rows) {
    // forms
    if ($forms) {
      $output .= (isset($forms['browse_search']) ? $forms['browse_search'] : '');
      $output .= (isset($forms['browse_public']) ? $forms['browse_public'] : '');
      $output .= (isset($forms['browse_public_groups']) ? $forms['browse_public_groups'] : '');
      $output .= (isset($forms['browse_groups']) ? $forms['browse_groups'] : '');
      $output .= (isset($forms['browse_order']) ? $forms['browse_order'] : '');
    }
    // help message
    $output .= ($help ? '<div class="imgp_help">' . $help . '</div>' : '');
    $output .= $pref1;
    $ct = 0;
    foreach ($rows AS $row) {
      $tooltip =  $row['img_name'] . ': ' . $row['img_title'] . ' ' . $row['img_description'];
      $imglink = '<img src="' . $row['imgpath'] . '" alt="' . $row['img_title'] . '" title="' . $tooltip . '" />';
      $output .= $pref2 . l($imglink, $row['imgurl'] . $row['img_id'], array('html' => TRUE)) . $suff2;
      $ct++;
      if ($cols > 0 && $ct >= $cols) {
        $output .= $suff1 . $pref1;
        $ct = 0;
      }
    }
    $output .= $suff1 . theme('pager');
  }
  else {
    $output .= '<div class="messages">' . $message . '</div>';
  }
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_imagepicker_fullpage($variables) {

  $image = $variables['image'];
  $source = $variables['source'];
  $link = $variables['link'];
  $output = '<div id="imgp_page"><div id="imgp_page_img">';
  $output .= '<img src="' . $source . '" alt="' . (isset($image->img_title) ? $image->img_title : '') . '" /></div>';
  $output .= (isset($image->img_description) ? '<div>' . nl2br($image->img_description) . '</div>' : '');
  $output .= ($link ? '<div><a href="#" onclick="history.back()">' . t('Return to page') . '</a></div>' : '');
  $output .= '</div>';
  return $output;
}

function theme_imagepicker_quota_message($variables) {

  $message1 = $variables['message1'];
  $message2 = $variables['message2'];
  $form = $variables['form'];
  $label = $variables['label'];
  $help = $variables['help'];
  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $output .= ($message1 ? '<div class="messages">' . $message1 . '</div>' : '') ;
  $output .= ($help ? '<div class="imgp_help">' . $help . '</div>' : '');
  $output .= $form;
  $output .= ($message2 ? '<div class="messages">' . $message2 . '</div>' : '') ;
  $output .= ($label ? '</fieldset>' : '');

  return $output;
}

function theme_imagepicker_view($variables) {

  $img = $variables['img'];
  $imgpath = $variables['imgpath'];
  $info = $variables['info'];
  $exifinfo = $variables['exifinfo'];
  $output = '<div id="imgp_img_view">';
  $output .= ($img->img_title ? '<div id="imgp_img_view_title">' . $img->img_title . '</div>' : '');
  $output .= '<img id="imgp_img_view_img" src="' . $imgpath . '" alt="' . $img->img_title . '" title="' . $img->img_name . '" />';
  $output .= ($img->img_description ? '<div id="imgp_img_view_desc">' . nl2br($img->img_description) . '</div>' : '');
  $output .= '<div>';
  $output .= t('Width') . ": " . $info['width'] . "&nbsp;&nbsp;&nbsp;";
  $output .= t('Height') . ": " . $info['height'] . "&nbsp;&nbsp;&nbsp;";
  $output .= t('Type') . ": " . $info['extension'] . "&nbsp;&nbsp;&nbsp;";
  $output .= t('Size') . ": " . $info['file_size'];
  $output .= '</div>';

  if ($exifinfo) {
    $output .= '<a id="imgp_trig">' . t('Show/Hide Exif Info') . '</a>';
    $output .= '<div id="imgp_targ" >';
    if (is_array($exifinfo)) {
      // using php function exif_read_data().
      foreach ($exifinfo AS $key => $section) {
        $header = array(
          array('data' => drupal_ucfirst(drupal_strtolower($key)), 'colspan' => 2),
        );
        $rows = array();
        foreach ($section as $name => $val) {
          if ($key == 'COMMENT') {
            $a = explode(':', $val);
            $rows[] = array($a[0] . ':', $a[1]);
          }
          else {
            $rows[] = array($name . ': ', $val);
          }
        }
        $output .= theme('table', array('header' => $header, 'rows' => $rows));
        unset($header);
        unset($rows);
      }
    }
    else {
      // from an external source eg exiftool
      $output .= '<pre>' . check_plain($exifinfo) . '</pre>';
    }
    $output .= '</div>';
  }
  $output .= '</div>';
  return $output;
}

function theme_imagepicker_image_edit_header($variables) {

  $image = $variables['image'];
  $source = $variables['source'];
  $output = '<div class="imgp_help">' . t('Edit image details') . '</div>';
  $output .= '<div id="imgp_img_holder"><img src="' . $source . '" alt="' . check_plain($image->img_title) . '" /></div>';
  return $output;
}

function theme_imagepicker_quota($variables) {

  $form = $variables['quotaform'];
  $message = $variables['message'];
  $label = $variables['label'];
  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $output .= ($message ? '<div class="messages">' . $message . '</div>' : '') ;
  $output .= $form;
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_imagepicker_user_config($variables) {

  $form = $variables['form'];
  $label = $variables['label'];
  $help = $variables['help'];
  $message1 = $variables['message1'];
  $message2 = $variables['message2'];

  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $output .= ($help ? '<div class="imgp_help">' . $help . '</div>' : '');
  $output .= ($message1 ? '<div class="messages">' . $message1 . '</div>' : '') ;
  $output .= $form;
  $output .= ($message2 ? '<div class="messages">' . $message2 . '</div>' : '') ;
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_imagepicker_user_view($variables) {

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

function theme_imagepicker_admin_view($variables) {

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

function theme_imagepicker_insert($variables) {

  $img = $variables['img'];
  $public = $variables['public'];
  $form1 = $variables['form1'];
  $form2 = $variables['form2'];

  $output = "";
  if ($img) {
    $imgpath = imagepicker_get_image_path($img, 'browser', ($public ? array('name' => $img->name , 'uid' => $img->uid) : FALSE ));
    $output .= '<div class="imgp_help">' . t('Choose the settings you want, place the cursor in the Body box above and Insert image.') . '</div>';
    if (isset($img->img_title)) {
      $output .= '<div id="imgp_img_holder_title">' . check_plain($img->img_title) . '</div>';
    }
    if (isset($img->img_description)) {
      $output .= '<div id="imgp_img_holder_description">' . nl2br($img->img_description) . '</div>';
    }
    $output .= '<div id="imgp_img_holder">';
    $output .= '<img id="imgp_img" src="' . $imgpath . '" alt="' . (isset($img->img_title) ? check_plain($img->img_title) : t('Image') ) . '" title="' . $img->img_name . '" />';
    $output .= '</div>';
    $output .= $form1;
    $output .= $form2;
  }
  return $output;
}

function theme_imagepicker_upload_form($variables) {

  $form = $variables['form'];
  $output = '';
  $max_uploads = imagepicker_variable_get('imagepicker_max_uploads', 1);
  $max_filesize = ini_get('upload_max_filesize');

  $form['maxmsg']['#markup'] = '<div class="messages">' . t('Browse your computer for up to !c, Max %max Total', array('!c' => format_plural($max_uploads, '1 Image', '@count Images'), '%max' => $max_filesize) ) . '</div>';
  for ($ct = 1; $ct <= $max_uploads; $ct++) {
    $form['file_upload_' . $ct]['#prefix'] = "<div id='imagepicker_upload_link_wrapper_$ct'>";
    $form['description_' . $ct]['#suffix'] = '</div>';
    if (isset($form['link_' . $ct])) {
      $form['link_' . $ct]['#markup'] = "<div class='imagepicker_upload_link' id='imagepicker_upload_link_$ct' style='display:none'>" . l(t('Upload another'), '', array(
        'attributes' => array(
          'onClick' => "Drupal.imagepicker_upload_link_click($ct); return false;"
        )
      )) . '</div>';
    }
  }

  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_user_image_form($variables) {

  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}
function theme_imagepicker_user_config_admin_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_align']['#prefix'] = '<div class="imgp_img_options">';
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_align']['#suffix'] = '</div>';
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_show']['#prefix'] = '<div class="imgp_img_options">';
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_show']['#suffix'] = '</div>';
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_link']['#prefix'] = '<div class="imgp_img_options">';
  $form['imagepicker_insert_defaults']['imagepicker_insert_defaults_link']['#suffix'] = '</div>';
  $form['imagepicker_watermark']['imagepicker_watermark_image']['#prefix'] = '<div class="container-inline">';
  $form['imagepicker_watermark']['imagepicker_watermark_image_delete']['#suffix'] = '</div>';
  $form['submit']['#prefix'] = '<div class="container-inline">';
  $form['reset']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_admin_image_form($variables) {

  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_user_search_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['imagepicker_currentuser']['#prefix'] = '<div id="imgp_users_form" class="container-inline">';
  $form['imagepicker_user_search_submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_group_search_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['imagepicker_currentgroup']['#prefix'] = '<div id="imgp_groups_form" class="container-inline">';
  $form['imagepicker_group_search_submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_quota_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['imagepicker_quota']['#prefix'] = '<div id="imgp_quota_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_edit_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['title']['#prefix'] = '<div id="imgp_edit_form">';
  $form['description']['#suffix'] = '</div>';
  $form['submit']['#prefix'] = '<div id="imgp_controls">';
  $form['cancel']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_groups_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['groupsave']['group_public_roles']['#prefix'] = '<div id="wrap-group-public-roles">';
  $form['groupsave']['group_public_roles']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_group_delete_form($variables) {

  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_image_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['align']['#prefix'] = '<div class="imgp_img_options">';
  $form['align']['#suffix'] = '</div>';
  $form['show']['#prefix'] = '<div class="imgp_img_options">';
  $form['show']['#suffix'] = '</div>';
  $form['link']['#prefix'] = '<div class="imgp_img_options">';
  $form['link']['#suffix'] = '</div>';
  $form['desc']['#prefix'] = '<div id="imgp_desc_control">';
  $form['desc']['#suffix'] = '</div>';
  $form['insert']['#prefix'] = '<div id="imgp_controls">';
  $form['delete']['#suffix'] = '</div>';
  if (isset($form['presets_show'])) {
    $form['presets_show']['#prefix'] = '<div id="imgp_presets_control"><div id="imgp_presets_show_control">';
    $form['presets_show']['#suffix'] = '</div>';
    $form['presets_link']['#prefix'] = '<div id="imgp_presets_link_control">';
    $form['presets_link']['#suffix'] = '</div></div>';
  }
  if (isset($form['relbox'])) {
    $form['relbox']['#prefix'] = '<div id="imgp_relbox_control"><div id="imgp_relbox_show_control">';
    $form['relbox']['#suffix'] = '</div>';
    $form['linkhide']['#prefix'] = '<div id="imgp_relbox_link_control">';
    $form['linkhide']['#suffix'] = '</div></div>';
  }

  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_browse_groups_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['gid']['#prefix'] = '<div id="imgp_groups_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_browse_public_groups_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['gid']['#prefix'] = '<div id="imgp_groups_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_group_images_form($variables) {

  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_browse_order_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['imagepicker_browser_order']['#prefix'] = '<div id="imgp_order_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_browse_public_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['imagepicker_browse_public']['#prefix'] = '<div id="imgp_browse_form" class="container-inline">';
  $form['submit']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_browse_search_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['imagepicker_browser_search']['#prefix'] = '<div id="imgp_search_form" class="container-inline">';
  $form['reset']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_settings_form($variables) {

  $form = $variables['form'];
  $output = '';

  $jshide = ($form['imagepicker_import']['imagepicker_import_enabled']['#default_value'] ? '' : ' class="js-hide"');
  $form['imagepicker_import']['imagepicker_import_delete']['#prefix'] = '<div id="wrap-imagepicker-import"' . $jshide . '>';
  $form['imagepicker_import']['imagepicker_import_max']['#suffix'] = '</div>';

  if (isset($form['imagepicker_blocks'])) {
    $jshide = ($form['imagepicker_blocks']['imagepicker_galleryblocks_enabled']['#default_value'] ? '' : ' class="js-hide"');
    $form['imagepicker_blocks']['imagepicker_galleryblocks_howmany']['#prefix'] = '<div id="wrap-imagepicker-blocks"' . $jshide . '>';
    $form['imagepicker_blocks']['close']['#suffix'] = '</div>';
  }

  $jshide = ($form['imagepicker_groups']['imagepicker_groups_enabled']['#default_value'] ? '' : ' class="js-hide"');
  $form['imagepicker_groups']['imagepicker_public_enabled']['#prefix'] = '<div id="wrap-imagepicker-groups"' . $jshide . '>';
  $form['imagepicker_groups']['imagepicker_groups_in_upload_enabled']['#suffix'] = '</div>';

  $jshide = ($form['imagepicker_progress']['imagepicker_upload_progress_enabled']['#default_value'] ? '' : ' class="js-hide"');
  $form['imagepicker_progress']['imagepicker_upload_progress_delay']['#prefix'] = '<div id="wrap-imagepicker-upload-progress"' . $jshide . '>';

  if (isset($form['imagepicker_progress']['imagepicker_uploadprogress'])) {
    $form['imagepicker_progress']['imagepicker_uploadprogress']['#suffix'] = '</div>';
  }
  else {
    $form['imagepicker_progress']['imagepicker_upload_progress_message']['#suffix'] = '</div>';
  }

  $jshide = ($form['imagepicker_quotas']['imagepicker_quota_byrole']['#default_value'] ? '' : ' class="js-hide"');
  $form['imagepicker_quotas']['imagepicker_quota_role']['#prefix'] = '<div id="wrap-imagepicker-quota-role"' . $jshide . '>';
  $form['imagepicker_quotas']['imagepicker_quota_role']['#suffix'] = '</div>';

  if (isset($form['imagepicker_watermark'])) {
    $jshide = ($form['imagepicker_watermark']['imagepicker_watermark_enable']['#default_value'] ? '' : ' class="js-hide"');
    $form['imagepicker_watermark']['imagepicker_watermark_image']['#prefix'] = '<div id="wrap-imagepicker-watermark"' . $jshide . '>';
    $form['imagepicker_watermark']['imagepicker_watermark_scale']['#suffix'] = '</div>';
  }
  if (isset($form['imagepicker_settings']['imagepicker_admin_message'])) {
    $form['imagepicker_settings']['imagepicker_admin_message']['#prefix'] = '<div class="messages">';
    $form['imagepicker_settings']['imagepicker_admin_message']['#suffix'] = '</div>';
  }

  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_copy_form($variables) {

  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_browse_admin_form($variables) {

  $form = $variables['form'];
  $output = '';
  $src = $form['options']['src']['#value'];
  $cols = $form['options']['cols']['#value'];
  $public = $form['options']['public']['#value'];
  $form['options']['#prefix'] = '<div class="container-inline">';
  $form['options']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_browse_admin($variables) {

  $forms = $variables['forms'];
  $pref = $variables['pref'];
  $suff = $variables['suff'];
  $label = $variables['label'];
  $output = ($label ? '<fieldset><legend>' . $label . '</legend>' : '');
  $output .= $pref;
  $output .= $forms['browse_search'];
  $output .= $forms['browse_groups'];
  $output .= $forms['browse_public'];
  $output .= $forms['browse_public_groups'];
  $output .= $forms['browse_admin'];
  $output .= $suff;
  $output .= ($label ? '</fieldset>' : '');
  return $output;
}

function theme_imagepicker_multitask_delete_form($variables) {

  $form = $variables['form'];
  $output = '';
  $count = $form['countnids']['#value'];
  $output .= '<p>' . t('You have selected %c to be deleted', array('%c' => format_plural( $count, '1 image', '@count images'))) . '</p>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_multitask_groups_form($variables) {

  $form = $variables['form'];
  $output = '';
  $count = $form['countnids']['#value'];
  $output .= '<p>' . t('You have selected %c to be grouped', array('%c' => format_plural( $count, '1 image', '@count images'))) . '</p>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_import_form($variables) {

  $form = $variables['form'];
  $output = '';
  $markup = $form['total']['#markup'];
  $form['total']['#markup'] = '<p>' . $markup . '</p>';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_import_dir_form($variables) {

  $form = $variables['form'];
  $output = '';
  $output .= drupal_render_children($form);
  return $output;
}

function theme_imagepicker_admin_orphans_form($variables) {

  $form = $variables['form'];
  $output = '';
  $form['msg']['#prefix'] = '<div class="messages">';
  $form['msg']['#suffix'] = '</div>';
  $output .= drupal_render_children($form);
  return $output;
}
