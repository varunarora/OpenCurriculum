<?php

/**
 * @file
 * Imagepicker functions
 */

/**
 * Menu local task; presents the browse and select pages for imagepicker
 */
function imagepicker_browse($img_id=0) {

  if ( $img_id ) {
    return imagepicker_image_select($img_id);
  }
  else {
    return theme('imagepicker_iframe', array('content' => _imagepicker_browse()));
  }
}

function imagepicker_browse_public($img_id=0) {

  if ( $img_id ) {
    return imagepicker_image_select($img_id, FALSE, TRUE);
  }
  else {
    return theme('imagepicker_iframe', array('content' => _imagepicker_browse_public()));
  }
}

function imagepicker_image_select($img_id, $showgroup=TRUE, $public = FALSE, $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $img = _imagepicker_get_img($img_id, ($public ? FALSE : TRUE));
  if ($img) {
    $form1 = '';
    if ($user->uid == $img->uid || $public) {
      $form1 = render(drupal_get_form('imagepicker_image_form', $img_id, $public));
    }

    // groups
    $form2 = '';
    if (_imagepicker_has_groups() && $showgroup) {
      $form2 = render(drupal_get_form('imagepicker_group_images_form', $img->img_id));
    }

    $content = theme('imagepicker_insert', array('img' => $img, 'public' => $public, 'form1' => $form1, 'form2' => $form2));

  }
  else {
    drupal_set_message(t('Image not found.'), 'error');
    $content = '';
  }

  return theme('imagepicker_iframe', array('content' => $content, 'img' => $img, 'public' => $public));
}

/**
 * Function to display the image insertion form
 *
 * @param $img_id
 *   The id of the image to be inserted.
 * @param $public
 *   Optional, ensures that public images cannot be edited.
 * @return
 *   Returns the image form.
 */
function imagepicker_image_form($form, &$form_state, $img_id, $public = FALSE) {

  global $user;
  if (imagepicker_variable_get('imagepicker_default_align_show', 1)) {
    $form['align'] = imagepicker_get_align_opts(imagepicker_variable_get('imagepicker_insert_defaults_align', imagepicker_variable_get('imagepicker_insert_defaults_align', 'none'), $user->uid));
  }
  $form['show'] = imagepicker_get_show_opts(imagepicker_variable_get('imagepicker_insert_defaults_show', imagepicker_variable_get('imagepicker_insert_defaults_show', 'full'), $user->uid));
  $form['link'] = imagepicker_get_link_opts(imagepicker_variable_get('imagepicker_insert_defaults_link', imagepicker_variable_get('imagepicker_insert_defaults_link', 'none'), $user->uid));

  if (module_exists('image') && imagepicker_variable_get('imagepicker_image_enable', 0)) {
    $opts = image_style_options();
    $form['presets_show'] = array(
      '#type' => 'select',
      '#title' => t('Presets for Show'),
      '#options' => $opts,
      '#default_value' => imagepicker_variable_get('imagepicker_image_default_show', ''),
    );
    $form['presets_link'] = array(
      '#type' => 'select',
      '#title' => t('Presets for Link'),
      '#options' => $opts,
      '#default_value' => imagepicker_variable_get('imagepicker_image_default_link', ''),
    );
  }

  $form['desc'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Description'),
    '#description' => t('Insert title and description'),
  );
  // allow override but only if admin allows
  if ((imagepicker_variable_get('imagepicker_use_cssbox', 0)
    && ! imagepicker_variable_get('imagepicker_use_cssbox', 0, $user->uid))
    || (imagepicker_variable_get('imagepicker_use_cssbox', 0)
    && imagepicker_variable_get('imagepicker_use_cssbox', 0, $user->uid) )) {
    $form['cssbox'] = array(
      '#type' => 'textfield',
      '#title' => t('Add additional css'),
      '#size' => 20,
      '#description' => t('You can add additional css here, eg. class="myclass". This will be inserted into the image tag'),
    );
  }

  // rel
  if (module_exists('colorbox') && imagepicker_variable_get('imagepicker_colorbox_enable', 0)) {
    if ((imagepicker_variable_get('imagepicker_use_relbox', 0)
      && ! imagepicker_variable_get('imagepicker_use_relbox', 0, $user->uid))
      || (imagepicker_variable_get('imagepicker_use_relbox', 0)
      && imagepicker_variable_get('imagepicker_use_relbox', 0, $user->uid) )) {
      $form['relbox'] = array(
        '#type' => 'textfield',
        '#title' => t('Add rel attribute'),
        '#size' => 20,
        '#description' => t('You can add a rel tag here, it will be added to the link when colorbox is selected. Useful for creating galleries. No spaces.'),
      );
      $form['linkhide'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide this link'),
      '#description' => t('Use this on subsequent links added to a gallery.'),
      );
    }
  }

  $form['insert'] = array(
    '#type' => 'button',
    '#value' => t('Insert image'),
    '#attributes' => array('onclick' => 'imagepickerInsert(this); return false;')
  );
  if (! $public) {
    $form['edit'] = array(
      '#type' => 'submit',
      '#value' => t('Edit image'),
      '#submit' => array('imagepicker_image_form_edit'),
    );
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete image'),
      '#submit' => array('imagepicker_image_form_delete'),
    );
  }
  $form['img_id'] = array(
    '#type' => 'value',
    '#value' => $img_id,
  );

  return $form;
}
/**
 * Submit form functions
 */
function imagepicker_image_form_delete($form, &$form_state) {

  imagepicker_image_delete($form_state['values']['img_id']);
}

function imagepicker_image_form_edit($form, &$form_state) {

  drupal_goto('imagepicker/edit/' . $form_state['values']['img_id']);
}

function imagepicker_image_delete($img_id, $account=FALSE, $src='iframe') {

  if ($account) {
    $user = $account;
    $userdir = array('uid' => $user->uid);
  }
  else {
    global $user;
    $userdir = TRUE;
  }
  if ($src == 'account') {
    $outpath = 'user/' . $user->uid . '/imagepicker/images/browse';
  }
  elseif ($src == 'admin') {
    $outpath = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $user->uid . '/browse';
  }
  else {
    $outpath ='imagepicker/browse';
  }

  _imagepicker_image_delete($img_id, $account, $src);
  drupal_goto($outpath);
}

function _imagepicker_image_delete($img_id, $account=FALSE, $src='iframe', $suppress = FALSE) {
  if ($account) {
    $user = $account;
    $userdir = array('name' => $user->name, 'uid' => $user->uid);
  }
  else {
    global $user;
    $userdir = TRUE;
  }
  $img = _imagepicker_get_img($img_id, ($src=='admin' ? FALSE : TRUE), ($account ? $user : FALSE));
  if ($img) {
    $destination = imagepicker_get_path(FALSE, $userdir);
    $thumbsdir = $destination . IMAGEPICKER_THUMBS_DIR . DIRECTORY_SEPARATOR;
    $browserdir = $destination . IMAGEPICKER_BROWSER_DIR . DIRECTORY_SEPARATOR;
    $origdir = $destination . IMAGEPICKER_ORIG_DIR . DIRECTORY_SEPARATOR;
    file_unmanaged_delete($destination . $img->img_name);
    file_unmanaged_delete($thumbsdir . $img->img_name);
    file_unmanaged_delete($browserdir . $img->img_name);
    file_unmanaged_delete($origdir . $img->img_name);

    if ( db_delete('imagepicker')->condition('uid', $user->uid)->condition('img_id', $img_id)->execute() ) {
      // groups entries
      db_delete('imagepicker_group_images')->condition('img_id', $img_id)->execute();
      if (! $suppress) {
        drupal_set_message(t('Image was successfully deleted'));
      }
    }
    else {
      drupal_set_message(t('Error while trying to delete your image from database.'), 'error');
    }
  }
  else {
    drupal_set_message(t('Image not found.'), 'error');
  }
  return;
}

/**
 * Menu callback; presents the image page for imagepicker
 * @param numeric $img_id
 * @return string
 */
function imagepicker_image_page($img_id, $preset = FALSE) {

  global $base_url;
  $content = '';

  $query = db_select('imagepicker', 'i');
  $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
  $query->addField('u', 'name');
  $query->range(0, 1);
  $query->join('users', 'u', 'i.uid = u.uid');
  $query->condition('i.img_id', $img_id);
  $img = $query->execute()->fetchObject();
  // $img is now object

  if ($img && is_object($img) && count($img)) {

    drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');
    drupal_set_title($img->img_title);
    // js link
    $account = user_load($img->uid);
    $link = imagepicker_variable_get('imagepicker_default_pagelink', imagepicker_variable_get('imagepicker_default_pagelink', 1), $account->uid);
    if ( variable_get('file_default_scheme', 'public') == 'private' ) {
      if (module_exists('image') && imagepicker_variable_get('imagepicker_image_enable', 0) && $preset) {
        $imgpath = imagepicker_get_image_path($img, 'full', array('uid' => $img->uid), TRUE);
        $imgpath = preg_replace("~__PRESET__~", $preset, $imgpath);
      }
      else {
        $imgpath = imagepicker_get_image_path($img, 'full', array('uid' => $img->uid));
      }
    }
    else {
      if (module_exists('image') && imagepicker_variable_get('imagepicker_image_enable', 0) && $preset) {
        $imgpath = imagepicker_get_image_path($img, 'full', array('uid' => $img->uid), TRUE);
        $imgpath = preg_replace("~__PRESET__~", $preset, $imgpath);
      }
      else {
        $imgpath = imagepicker_get_path(TRUE, $img) . $img->img_name;
      }
    }
    $content = theme('imagepicker_fullpage', array('img' => $img, 'source' => $imgpath , 'link' => $link));
  }
  else {
    drupal_set_message(t('Image not found in page.'), 'error');
  }

  return $content;
}

// There is not need to inform users, that directory structure has been created
// and show them all paths... So lets strip these messages if there are any.
/**
 * @param $msg
 * @return string
 */
function imagepicker_strip_messages($msg) {

  if ($msg) {
    $dirsep = (DIRECTORY_SEPARATOR == '\\') ? '\\\\' : '\/';
    $pattern = '/<li>.*' . $dirsep . 'imagepicker' . $dirsep . '.*<\/li>/i';
    $msg = preg_replace($pattern, '', $msg);
  }
  return $msg;
}

function imagepicker_browse_admin_form($form, &$form_state, $src="iframe", $account=FALSE, $public=FALSE, $range=1) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  // paths
  if ($src == 'iframe') {
    $path = '';
    $editpath   = "imagepicker/edit/";
    $deletepath = "imagepicker/delete/";
    $imgpath    = 'imagepicker/browse/';
  }
  elseif ($src == 'admin') {
    $path = IMAGEPICKER_ADMIN_PATH;
    $editpath   = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $user->uid . '/edit/';
    $deletepath = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $user->uid . '/delete/';
    $imgpath    = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $user->uid . '/browse/';
    $returnpath = 'a-' . $user->uid;
  }
  else {
    $path = 'user/' . $user->uid . '/imagepicker';
    $editpath   = 'user/' . $user->uid . '/imagepicker/images/edit/';
    $deletepath = 'user/' . $user->uid . '/imagepicker/images/delete/';
    $imgpath    = 'user/' . $user->uid . '/imagepicker/images/browse/';
    $returnpath = 'u-' . $user->uid;
  }

  // if there are groups
  if (imagepicker_variable_get('imagepicker_groups_enabled', 1)) {
    $gid = 0;
    if (! $public) {
      if (_imagepicker_has_groups( ($src == 'admin' ? $user : FALSE) ) && imagepicker_has_grouplist(($src == 'admin' ? $user : FALSE))) {
        $gids = imagepicker_get_user_group_state(1, ($src == 'admin' ? $user : FALSE));
        $gid = $gids[0];
        if ($account) {
          $gid = imagepicker_variable_get('imagepicker_currentgroup', 0);
        }
      }
    }
    elseif ($src == 'admin' && _imagepicker_has_public_groups($user, TRUE)) {
      $gid = imagepicker_variable_get('imagepicker_public_currentgroup', 0);
    }
  }

  // table header
  if ($public && $src == 'admin') {
    $header = array(
      'img_name' => array('data' => t('Name'), 'field' => 'i.img_name', 'sort' => 'desc'),
      'img_title' => array('data' => t('Title'), 'field' => 'i.img_title'),
      'img_desc' => t('Description'),
      'img_list' => array('data' => t('User'), 'field' => 'n.name'),
      'img_date' => array('data' => t('Date'), 'field' => 'i.img_date'),
      'img_actions' => array('data' => t('Actions')),
    );
    $cols = 7;
  }
  else {
    $header = array(
      'img_name' => array('data' => t('Name'), 'field' => 'i.img_name', 'sort' => 'desc'),
      'img_title' => array('data' => t('Title'), 'field' => 'i.img_title'),
      'img_desc' => t('Description'),
      'img_date' => array('data' => t('Date'), 'field' => 'i.img_date'),
      'img_actions' => array('data' => t('Actions')),
    );
    $cols = 6;
  }

  $use_icons = imagepicker_variable_get('imagepicker_use_icons', 1);

  if ($src == 'admin') {
    $max = imagepicker_variable_get('imagepicker_rows_per_page', 25);
    $searchs = imagepicker_variable_get('imagepicker_browser_search', '');
  }
  else {
    $max = imagepicker_variable_get('imagepicker_rows_per_page', imagepicker_variable_get('imagepicker_rows_per_page', 25), $user->uid);
    $searchs = imagepicker_variable_get('imagepicker_browser_search', '', $user->uid);
  }
  if ($searchs) {
    $searchs = trim($searchs);
    $searchs = check_plain($searchs);
  }

  if ($public && $src == 'admin') {
    if ($range == 1 ||  $range == 2) {
      $publicstate = ($range == 1 ? 1 : 0);
      $query = db_select('users', 'u')
        ->extend('PagerDefault')->limit($max)
        ->extend('TableSort')->orderByHeader($header);
      $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
      $query->addField('u', 'name');
      $query->join('imagepicker', 'i');
      $query->leftjoin('imagepicker_group_images', 'g', 'g.img_id = i.img_id');
      $query->leftjoin('imagepicker_user_groups', 'iug', 'iug.gid = g.gid');
      $query->condition('u.uid', 'iug.uid')->condition('iug.public', $publicstate);
      if ($gid && $publicstate) {
        $query->condition('iug.gid', $gid);
      }
      if ($range == 1) {
        $label = t('List All Public Images');
      }
      else {
        $label = t('List All Private Images');
      }
    }
    else {
      $query = db_select('imagepicker', 'i')
        ->extend('PagerDefault')->limit($max)
        ->extend('TableSort')->orderByHeader($header);
      $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
      $query->join('users', 'u', 'i.uid = u.uid');
      $query->addField('u', 'name');
      $query->condition('u.status', 1);

    }
  }
  else {
    // filter by selected group
    if ($gid) {
      $query = db_select('imagepicker', 'i')
        ->extend('PagerDefault')->limit($max)
        ->extend('TableSort')->orderByHeader($header);
      $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
      $query->addField('u', 'name');
      $query->join('imagepicker_group_images', 'g', 'i.img_id = g.img_id');
      $query->join('users', 'u', 'i.uid = u.uid');
      $query->condition('u.uid', $user->uid)
        ->condition('u.status', 1)
        ->condition('gid', $gid);
    }
    else {
      $query = db_select('imagepicker', 'i')
        ->extend('PagerDefault')->limit($max)
        ->extend('TableSort')->orderByHeader($header);
      $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
      $query->addField('u', 'name');
      $query->join('users', 'u', 'i.uid = u.uid');
      $query->condition('u.uid', $user->uid)
        ->condition('u.status', 1);
    }
  }
  // do search
  $searchsql = '';
  if ($searchs) {
    $searchsql = _imagepicker_search_opts($searchs, $account);
  }
  if (is_array($searchsql) ) {
    $query->condition($searchsql[0], $searchsql[1], $searchsql[2]);
  }
  elseif (is_object($searchsql)) {
    $query->condition($searchsql);
  }

  $records = $query->execute();

  // bulk ops form
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bulk operations'),
  );
  if ($src == 'admin' && ! $account) {
    $op_opts = array('delete' => t('Delete'));
  }
  else {
    $op_opts = array('delete' => t('Delete'), 'groups' => t('Groups'));
  }
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#options' =>  $op_opts,
    '#default_value' => 'delete',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  $form['options']['src'] = array(
    '#type' => 'value',
    '#value' => $src,
  );
  $form['options']['public'] = array(
    '#type' => 'value',
    '#value' => $public,
  );
  $form['options']['path'] = array(
    '#type' => 'value',
    '#value' => $path,
  );

  $fmt = imagepicker_variable_get('imagepicker_dateformat_default', 'medium');
  $rowct = 0;
  $options = array();
  // iterate
  foreach ($records AS $row) {
    $img_name = check_plain($row->img_name);
    $img_id = $row->img_id;
    $description = check_plain($row->img_description);
    $img_date = format_date($row->img_date, $fmt);
    if (drupal_strlen($description) > IMAGEPICKER_DESC_LEN) {
      $description = drupal_substr($description, 0, IMAGEPICKER_DESC_LEN) . '...';
    }

    if ($public && $src == 'admin') {
      $editpath   = $path . '/images/user/' . $row->uid . '/edit/';
      $deletepath = $path . '/images/user/' . $row->uid . '/delete/';
      $imgpath    = $path . '/images/user/' . $row->uid . '/browse/';
      $listpath   = $path . '/images/user/' . $row->uid . '/browseadmin';
      $returnpath = 'a';
      $editlink = ($use_icons ? _imagepicker_get_icon('edit', $editpath . $img_id, array('title' => t('Edit'))) : l(t('Edit'), $editpath . $img_id));
      $deletelink = ($use_icons ? _imagepicker_get_icon('delete', $deletepath . $img_id, array('title' => t('Delete'))) : l(t('Delete'), $deletepath . $img_id));
      $imglink = l($img_name, $imgpath . $img_id);
      $listlink = l($row->name, $listpath);

      $options[$img_id] = array(
        'img_name' => $imglink,
        'img_title' => check_plain($row->img_title),
        'img_desc' => $description,
        'img_list' => $listlink,
        'img_date' => $img_date,
        'img_actions' => "$editlink $deletelink",
      );
      $cols = 7;
    }
    else {
      $imglink = l($img_name, $imgpath . $img_id);
      $editlink = ($use_icons ? _imagepicker_get_icon('edit', $editpath . $img_id, array('title' => t('Edit'))) : l(t('Edit'), $editpath . $img_id));
      $deletelink = ($use_icons ? _imagepicker_get_icon('delete', $deletepath . $img_id, array('title' => t('Delete'))) : l(t('Delete'), $deletepath . $img_id));

      $options[$img_id] = array(
        'img_name' => $imglink,
        'img_title' => check_plain($row->img_title),
        'img_desc' => $description,
        'img_date' => $img_date,
        'img_actions' => "$editlink $deletelink",
      );
      $cols = 6;
    }
    $rowct++;

  } // end of foreach loop

  $form['options']['cols'] = array(
    '#type' => 'value',
    '#value' => $cols,
  );
  $form['options']['returnpath'] = array(
    '#type' => 'value',
    '#value' => $returnpath,
  );
  $message = '';
  if ($rowct == 0) {
    $ibp = imagepicker_variable_get('imagepicker_browse_public', 0);
    if ($ibp == 1 && $range == 1) {
      $ibpout = "public";
    }
    elseif ($ibp == 2 || $range == 2) {
      $ibpout = "private";
    }
    else {
      $ibpout = "";
    }
    imagepicker_variable_set('imagepicker_browse_public', 0);
    if ($searchs) {
      $message = t('Your search for %searchs found nothing', array('%searchs' => $searchs));
      if ($src == 'admin') {
        imagepicker_browse_search_form_reset_func(TRUE);
      }
      else {
        imagepicker_browse_search_form_reset_func(FALSE);
      }
    }
    else {
      $message = t('There are no !status images', array('!status' => $ibpout));
    }

  }
  // tableselect
  $form['images'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => $message,
  );
  $form['pager'] = array('#markup' => theme('pager', array('tags' => NULL)));

  return $form;
}

/**
 * Submit form
 */
function imagepicker_browse_admin_form_submit($form, &$form_state) {
  $operation = $form_state['values']['operation'];
  $path = '';
  if (isset($form_state['values']['path'])) {
    $path = $form_state['values']['path'];
  }

  $returnpath = $form_state['values']['returnpath'];
  $images = array_filter($form_state['values']['images']);
  $img_ids = '';
  if ($images) {
    foreach ($images AS $img_id) {
      $img_ids[] = $img_id;
    }
  }
  if (is_array($img_ids)) {
    $simg_ids = implode('-', $img_ids);
    drupal_goto($path . "/multitask/$operation/$simg_ids/$returnpath");
  }
  else {
    $rpath = imagepicker_multitask_returnpath($returnpath);
    drupal_set_message(t('No images selected'));
    drupal_goto($rpath);
  }
}

/**
 *
 * Menu callback for imagepicker multitask.
 */
function imagepicker_multitask() {
  if (arg(0) == 'admin') {
    $mode = arg(5);
    $simg_ids = arg(6);
    $returnpath = arg(7);
  }
  elseif (arg(0) == 'user') {
    $mode = arg(4);
    $simg_ids = arg(5);
    $returnpath = arg(6);
  }
  $cancelpath = imagepicker_multitask_returnpath($returnpath);
  $output = '';
  if ($mode && $simg_ids) {
    switch ($mode) {
      case 'delete':
        // really delete?, auto themed
        $output .= render(drupal_get_form('imagepicker_multitask_delete_form', $simg_ids, $returnpath));
        $output .= l(t('Cancel'), $cancelpath);
        break;
      case 'groups':
        // select some groups
        $output .= render(drupal_get_form('imagepicker_multitask_groups_form', $simg_ids, $returnpath));
        $output .= l(t('Cancel'), $cancelpath);
        break;
    }
  }
  return $output;
}

function imagepicker_multitask_delete_form($form, &$form_state, $simg_ids, $returnpath) {

  $form['multitask_delete'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bulk delete'),
    '#description' => t('Delete all the selected images.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['multitask_delete']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete All'),
  );
  $form['simg_ids'] = array('#type' => 'value', '#value' => $simg_ids);
  $form['returnpath'] = array('#type' => 'value', '#value' => $returnpath);
  $img_ids = explode('-', $simg_ids);
  $form['countnids'] = array('#type' => 'value', '#value' => count($img_ids));
  $form['#submit'][] = 'imagepicker_multitask_delete_form_submit';
  return $form;
}

/**
 * Submit form
 */
function imagepicker_multitask_delete_form_submit($form, &$form_state) {
  $simg_ids = $form_state['values']['simg_ids'];
  $returnpath = $form_state['values']['returnpath'];

  $img_ids = explode('-', $simg_ids);
  foreach ($img_ids AS $img_id) {
    _imagepicker_image_delete($img_id, FALSE, 'admin');
  }

  drupal_set_message(t('%c deleted', array('%c' => format_plural( count($img_ids), '1 image', '@count images'))));
  $rpath = imagepicker_multitask_returnpath($returnpath);
  drupal_goto($rpath);
}

function imagepicker_multitask_groups_form($form, &$form_state, $simg_ids, $returnpath) {

  $img_ids = explode('-', $simg_ids);
  $countnids = count($img_ids);

  $query = db_select('imagepicker', 'i');
  $query->fields('i', array('uid'));
  $query->condition('i.img_id', $img_ids[0]);

  $row = $query->execute()->fetchObject();
  $account = user_load($row->uid);
  $grouplist = imagepicker_get_groups($account);

  $form['multitask_groups'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bulk groups management'),
    '#description' => t('Add/Remove all the selected images from groups.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['multitask_groups']['grouplist'] = array(
    '#type' => 'checkboxes',
    '#options' => $grouplist,
    '#title' => t('Your Groups'),
  );
  $form['multitask_groups']['action'] = array(
    '#type' => 'select',
    '#options' => array('add' => t('Add'), 'remove' => t('Remove')),
    '#title' => t('Action'),
    '#description' => t('Add or Remove selected images to one or more selected groups.'),
  );
  $form['multitask_groups']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add/Remove All'),
  );

  $form['simg_ids'] = array('#type' => 'value', '#value' => $simg_ids);
  $form['returnpath'] = array('#type' => 'value', '#value' => $returnpath);
  $form['countnids'] = array('#type' => 'value', '#value' => $countnids);
  $form['#submit'][] = 'imagepicker_multitask_groups_form_submit';
  return $form;
}

/**
 * Validate form
 */
function imagepicker_multitask_groups_form_validate($form, &$form_state) {
  $grouplist = array_filter($form_state['values']['grouplist']);
  if (! count($grouplist)) {
    form_set_error('grouplist', t('You did not select any groups.'));
  }
}

/**
 * Submit form
 */
function imagepicker_multitask_groups_form_submit($form, &$form_state) {

  $simg_ids = $form_state['values']['simg_ids'];
  $img_ids = explode('-', $simg_ids);
  $returnpath = $form_state['values']['returnpath'];
  $action = $form_state['values']['action'];
  // list of selected groups
  $grouplist = array_filter($form_state['values']['grouplist']);
  $gids = '';
  foreach ($grouplist AS $gid) {
    $gids[] = $gid;
  }

  if (is_array($gids) && count($img_ids)) {
    foreach ($img_ids AS $img_id) {
      foreach ($gids AS $gid) {
        $query = db_select('imagepicker_group_images', 'i');
        $query->addExpression('COUNT(i.gid)', 'ct');
        $query->condition('i.gid', $gid);
        $query->condition('i.img_id', $img_id);
        $row = $query->execute()->fetchObject();
        $found = FALSE;
        if ($row->ct > 0) {
          $found = TRUE;
        }
        if ($action == 'add') {
          if (! $found) {
            // add $img_id to $gid
            $object = new stdClass();
            $object->gid = $gid;
            $object->img_id = $img_id;
            imagepicker_insert_group_image($object);
          }
        }
        else {
          if ($found) {
            // remove $img_id from $gid
            imagepicker_delete_group_image($img_id);
          }
        }
      }
    }
    drupal_set_message(t('%c', array('%c' => format_plural( count($img_ids), '1 image', '@count images'))) . ' ' . ($action == 'add' ? t('added') : t('removed')));
  }

  $rpath = imagepicker_multitask_returnpath($returnpath);
  drupal_goto($rpath);
}

function imagepicker_multitask_returnpath($returnpath) {
  $rpath = '';
  if ($returnpath == 'a') {
    $rpath = IMAGEPICKER_ADMIN_PATH . '/images/list_all';
  }
  else {
    $a = explode('-', $returnpath);
    if (is_numeric($a[1])) {
      if ($a[0] == 'a') {
        $rpath = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $a[1] . '/browseadmin';
      }
      elseif ($a[0] == 'u') {
        $rpath = 'user/' . $a[1] . '/imagepicker/images/browseadmin';
      }
    }
  }
  return $rpath;
}
/**
 * @param $src
 * @param $account
 * @param $label
 * @return thumbnail browser
 */
function _imagepicker_browse($src="iframe", $account = FALSE, $label="") {

  if ($account) {
    $user = $account;
    $userdir = array( 'uid' => $user->uid);
  }
  else {
    global $user;
    $userdir = FALSE;
  }

  // if there are groups
  $gid = 0;
  if (_imagepicker_has_groups($user)) {
    $gids = imagepicker_get_user_group_state(1, $user);
    $gid = $gids[0];
    if ($account) {
      $gid = imagepicker_variable_get('imagepicker_currentgroup', 0);
    }
  }

  if ($account && $src == 'admin') {
    $default_order = imagepicker_variable_get('imagepicker_default_browser_order', 'img_id DESC');
    $order = imagepicker_variable_get('imagepicker_browser_order', $default_order);
    $searchs = imagepicker_variable_get('imagepicker_browser_search', '');
    $max = imagepicker_variable_get('imagepicker_advanced_browser_page', 25);
  }
  else {
    $default_order = imagepicker_variable_get('imagepicker_default_browser_order', imagepicker_variable_get('imagepicker_default_browser_order', 'img_id DESC'), $user->uid);
    $order = imagepicker_variable_get('imagepicker_browser_order', $default_order, $user->uid);
    $searchs = imagepicker_variable_get('imagepicker_browser_search', '', $user->uid);
    $max = imagepicker_variable_get('imagepicker_advanced_browser_page', imagepicker_variable_get('imagepicker_advanced_browser_page', 25), $user->uid);
  }
  if ($searchs) {
    $searchs = trim($searchs);
    $searchs = check_plain($searchs);
  }

  // filter by selected group
  if ($gid) {
    $query = db_select('imagepicker', 'i')->extend('PagerDefault')->limit($max);
    $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
    $query->join('imagepicker_group_images', 'g', 'i.img_id = g.img_id');
    $query->join('users', 'u', 'i.uid = u.uid');
    $query->condition('u.uid', $user->uid)
      ->condition('u.status', 1)
      ->condition('gid', $gid);
  }
  else {
    $query = db_select('imagepicker', 'i')->extend('PagerDefault')->limit($max);
    $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
    $query->join('users', 'u', 'i.uid = u.uid');
    $query->condition('u.uid', $user->uid)
      ->condition('u.status', 1);
  }
  // do search
  $searchsql = '';
  if ($searchs) {
    $searchsql = _imagepicker_search_opts($searchs, $account);
  }
  if (is_array($searchsql) ) {
    $query->condition($searchsql[0], $searchsql[1], $searchsql[2]);
  }
  elseif (is_object($searchsql)) {
    $query->condition($searchsql);
  }
  // finish
  $a = explode(' ', $order);
  $query->orderBy($a[0], (isset($a[1]) ? $a[1] : 'ASC'));

  $records = $query->execute();
  $content = _imagepicker_thumbs_getrows($records, FALSE, $src);

  $message = "";
  if (! is_array($content) ) {
    if ($searchs) {
      $message = t('Your search for %searchs found nothing', array('%searchs' => $searchs));
      if ($src == 'admin') {
        imagepicker_browse_search_form_reset_func(TRUE);
      }
      else {
        imagepicker_browse_search_form_reset_func(FALSE);
      }
    }
    elseif ($gid) {
      $message = t('You do not have any images in the selected group');
    }
    else {
      $message = t('You do not have any uploaded images');
    }
  }

  $forms = array();
  $forms['browse_search'] = '';
  $forms['browse_groups'] = '';
  $forms['browse_public'] = '';
  $forms['browse_public_groups'] = '';
  $forms['browse_order'] = '';

  if ( $src == 'admin') {
    if (imagepicker_variable_get('imagepicker_show_browse_order_form', 1)) {
      $forms['browse_order'] = render(drupal_get_form('imagepicker_browse_order_form', $user, TRUE));
    }
  }
  elseif (imagepicker_variable_get('imagepicker_show_browse_order_form', imagepicker_variable_get('imagepicker_show_browse_order_form', 1), $user->uid)) {
    $forms['browse_order'] = render(drupal_get_form('imagepicker_browse_order_form', $user, FALSE));
  }

  if ( imagepicker_variable_get('imagepicker_groups_enabled', 1) && _imagepicker_has_groups($user) && imagepicker_has_grouplist($user)) {
    // add groups select here
    $forms['browse_groups'] = render(drupal_get_form('imagepicker_browse_groups_form', ($account ? $user : FALSE)));
  }

  if ( $src == 'admin') {
    if (imagepicker_variable_get('imagepicker_show_browse_search_form', 1)) {
      $forms['browse_search'] = render(drupal_get_form('imagepicker_browse_search_form', $user, TRUE));
    }
  }
  elseif (imagepicker_variable_get('imagepicker_show_browse_search_form', imagepicker_variable_get('imagepicker_show_browse_search_form', 1), $user->uid)) {
    $forms['browse_search'] = render(drupal_get_form('imagepicker_browse_search_form', $user, FALSE));
  }

  if ($src == "account" || $src == "admin") {
    $help = t('Hold the mouse over an image to view Name, Title and Description, Click on it to view.');
  }
  else {
    $help = t('Hold the mouse over an image to view Name, Title and Description, Click on it to use.');
  }

  return theme('imagepicker_browser', array('content' => $content, 'forms' => $forms, 'message' => $message, 'help' => $help, 'label' => $label));
}

function _imagepicker_browse_public($src="iframe", $range=1, $label="") {

  global $user;

  // if there are groups
  $gid = 0;
  if (_imagepicker_has_public_groups($user, ($src=='admin' ? TRUE : FALSE))) {
    if ($src == 'admin') {
      $gid = imagepicker_variable_get('imagepicker_public_currentgroup', 0);
    }
    else {
      // get the current users setting
      $gid = imagepicker_variable_get('imagepicker_public_currentgroup', 0, $user->uid);
    }
  }

  $max = imagepicker_variable_get('imagepicker_advanced_browser_page', imagepicker_variable_get('imagepicker_advanced_browser_page', 25), $user->uid);
  $default_order = imagepicker_variable_get('imagepicker_default_browser_order', imagepicker_variable_get('imagepicker_default_browser_order', 'img_id DESC'), $user->uid);
  $order = imagepicker_variable_get('imagepicker_browser_order', $default_order, $user->uid);

  if ($src == 'admin') {
    $searchs = imagepicker_variable_get('imagepicker_browser_search', '');
  }
  else {
    $searchs = imagepicker_variable_get('imagepicker_browser_search', '', $user->uid);
  }
  if ($searchs) {
    $searchs = trim($searchs);
    $searchs = check_plain($searchs);
  }
  $sarr = '';

  // Build sql for public with role restriction matching
  $rarr = array();
  $rolesql = '';
  $rolesarr = array();
  if ($range == 1 ||  $range == 2) {
    $publicstate = ($range == 1 ? 1 : 0);
    $query = db_select('users', 'u')->extend('PagerDefault')->limit($max);
    $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
    $query->addField('u', 'name');
    $query->join('imagepicker', 'i');
    $query->leftjoin('imagepicker_group_images', 'g', 'g.img_id = i.img_id');
    $query->leftjoin('imagepicker_user_groups', 'iug', 'iug.gid = g.gid');
    $query->condition('u.uid', 'iug.uid');
    $query->condition('iug.public', $publicstate);
    if ($gid && $publicstate) {
      $query->condition('iug.gid', $gid);
    }
    if ($range == 1) {
      $label = t('List All Public Images');
    }
    else {
      $label = t('List All Private Images');
    }
  }
  else {
    $query = db_select('imagepicker', 'i')->extend('PagerDefault')->limit($max);
    $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
    $query->addField('u', 'name');
    $query->join('users', 'u', 'i.uid = u.uid');
    $query->condition('u.status', 1);
  }
  // search
  $searchsql = '';
  if ($searchs) {
    $searchsql = _imagepicker_search_opts($searchs, ($src == 'admin' ? TRUE : FALSE));
  }
  if (is_array($searchsql) ) {
    $query->condition($searchsql[0], $searchsql[1], $searchsql[2]);
  }
  elseif (is_object($searchsql)) {
    $query->condition($searchsql);
  }

  // roles
  $rolesql = '';
  if ($range == 1 && imagepicker_variable_get('imagepicker_publicroles_enabled', 1) &&  $src != 'admin' ) {
    $roles = $user->roles;
    if (count($roles)) {
      $ct = 0;
      foreach ($roles AS $role) {
        $role = '%' . db_like($role) . '%';
        if (! $ct) {
          // first time
          $rolesql = db_or()
            ->condition('iug.avail_roles', 'all', '=')
            ->condition('iug.avail_roles', $role, 'LIKE');
        }
        else {
          $rolesql->condition('iug.avail_roles', $role, 'LIKE');
        }
        $ct++;
      }
    }
  }
  if (is_object($rolesql)) {
    $query->condition($rolesql);
  }

  $a = explode(' ', $order);
  $query->orderBy($a[0], (isset($a[1]) ? $a[1] : 'DESC'));

  $records = $query->execute();
  $content = _imagepicker_thumbs_getrows($records, TRUE, $src);

  $message = "";
  if (! is_array($content) ) {
    $ibp = imagepicker_variable_get('imagepicker_browse_public', 0);
    if ($ibp == 1 || $range == 1) {
      $ibpout = "public";
    }
    elseif ($ibp == 2 || $range == 2) {
      $ibpout = "private";
    }
    else {
      $ibpout = "";
    }
    imagepicker_variable_set('imagepicker_browse_public', 0);
    if ($searchs) {
      $message = t('Your search for %searchs found nothing', array('%searchs' => $searchs));
      if ($src == 'admin') {
        imagepicker_browse_search_form_reset_func(TRUE);
      }
      else {
        imagepicker_browse_search_form_reset_func(FALSE);
      }
    }
    else {
      $message = t('There are no !status images', array('!status' => $ibpout));
    }
  }

  $forms = array();
  if (imagepicker_variable_get('imagepicker_show_browse_order_form', 1, $user->uid)) {
    $forms['browse_order'] = render(drupal_get_form('imagepicker_browse_order_form'));
  }
  elseif (imagepicker_variable_get('imagepicker_show_browse_order_form', 1)) {
    $forms['browse_order'] = render(drupal_get_form('imagepicker_browse_order_form'));
  }

  if ( $src == 'admin' && imagepicker_variable_get('imagepicker_groups_enabled', 1)) {
    $forms['browse_public'] = render(drupal_get_form('imagepicker_browse_public_form'));
  }

  if ( imagepicker_variable_get('imagepicker_groups_enabled', 1) && _imagepicker_has_public_groups($user, ($src=='admin' ? TRUE : FALSE))) {
    // add groups select here
    $forms['browse_public_groups'] = render(drupal_get_form('imagepicker_browse_public_groups_form', $user, ($src=='admin' ? TRUE : FALSE)));
  }

  // search form
  if ( $src == 'admin') {
    if (imagepicker_variable_get('imagepicker_show_browse_search_form', 1)) {
      $forms['browse_search'] = render(drupal_get_form('imagepicker_browse_search_form', $user, TRUE));
    }
  }
  elseif (imagepicker_variable_get('imagepicker_show_browse_search_form', imagepicker_variable_get('imagepicker_show_browse_search_form', 1), $user->uid)) {
    $forms['browse_search'] = render(drupal_get_form('imagepicker_browse_search_form', $user, FALSE));
  }

  if ($src == "account" || $src == 'admin') {
    $help = t('Hold the mouse over an image to view Name, Title and Description, Click on it to view.');
  }
  else {
    $help = t('Hold the mouse over an image to view Name, Title and Description, Click on it to use.');
  }

  return theme('imagepicker_browser', array('content' => $content, 'forms' => $forms, 'message' => $message, 'help' => $help, 'label' => $label));
}

function _imagepicker_thumbs_getrows($result, $public = FALSE, $src = 'account') {

  global $user;
  $ct = 0;
  $imgct = 0;
  $rows = array();
  if ($result) {
    foreach ($result AS $img) {
      if ($public || $src == 'admin') {
        $rows[$imgct]['userdir'] = array('uid' => $img->uid);
      }
      else {
        $rows[$imgct]['userdir'] = array( 'uid' => $user->uid);
      }
      $rows[$imgct]['imgpath'] = imagepicker_get_image_path($img, 'browser', $rows[$imgct]['userdir']);
      if ($rows[$imgct]['imgpath']) {
        if ($public) {
          // paths
          if ($src == 'account') {
            $rows[$imgct]['imgurl'] = 'user/' . $user->uid . '/imagepicker/images/browse_public/';
          }
          elseif ($src == 'admin') {
            $rows[$imgct]['imgurl'] = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $img->uid . '/browse/';
          }
          else {
            $rows[$imgct]['imgurl'] = 'imagepicker/browse_public/';
          }
        }
        else {
          // paths
          if ($src == 'account') {
            $rows[$imgct]['imgurl'] = 'user/' . $user->uid . '/imagepicker/images/browse/';
          }
          elseif ($src == 'admin') {
            $rows[$imgct]['imgurl'] = IMAGEPICKER_ADMIN_PATH . '/images/user/' . $img->uid . '/browse/';
          }
          else {
            $rows[$imgct]['imgurl'] = 'imagepicker/browse/';
          }
        }

        $rows[$imgct]['img_name'] = $img->img_name;
        $rows[$imgct]['img_title'] = $img->img_title;
        $rows[$imgct]['img_description'] = $img->img_description;
        $rows[$imgct]['img_id'] = $img->img_id;
        $imgct++;
      }
    }
  }
  if (! $imgct) {
    return;
  }

  if ($src == 'account') {
    $page =imagepicker_variable_get('imagepicker_advanced_browser_page', imagepicker_variable_get('imagepicker_advanced_browser_page', 25), $user->uid);
    $cols =imagepicker_variable_get('imagepicker_advanced_browser_columns', imagepicker_variable_get('imagepicker_advanced_browser_columns', 0), $user->uid);
  }
  else {
    $page = imagepicker_variable_get('imagepicker_advanced_browser_page', 25);
    $cols = imagepicker_variable_get('imagepicker_advanced_browser_columns', 0);
  }
  return array($rows, $page, $cols, array('<div class="clear-block">', '<div class="imgp_holder">'), array('</div>', '</div>'));
}

/**
 * @param $src
 * @param $account
 * @param $public
 * @param $range
 * @param $label
 * @return themed admin table
 */
function _imagepicker_browse_admin($src="iframe", $account=FALSE, $public=FALSE, $range=1, $label='') {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $content = "";
  // if there are groups
  $forms = array();
  $forms['browse_search'] = '';
  $forms['browse_groups'] = '';
  $forms['browse_public'] = '';
  $forms['browse_public_groups'] = '';
  $forms['browse_admin'] = '';
  if (imagepicker_variable_get('imagepicker_groups_enabled', 1)) {
    $gid = 0;
    if (! $public) {
      if (_imagepicker_has_groups( ($src == 'admin' ? $user : FALSE) ) && imagepicker_has_grouplist(($src == 'admin' ? $user : FALSE))) {
        // add groups select here
        $forms['browse_groups'] = render(drupal_get_form('imagepicker_browse_groups_form', ($src == 'admin' ? $user : FALSE)));
      }
    }
  }

  $searchform = '';
  if ($src != 'admin' && imagepicker_variable_get('imagepicker_show_browse_search_form', imagepicker_variable_get('imagepicker_show_browse_search_form', 1), $user->uid) ) {
    $forms['browse_search'] = render(drupal_get_form('imagepicker_browse_search_form'));
  }
  elseif (imagepicker_variable_get('imagepicker_show_browse_search_form', 1)) {
    $forms['browse_search'] = render(drupal_get_form('imagepicker_browse_search_form', FALSE, TRUE));
  }

  $forms['browse_admin'] = render(drupal_get_form('imagepicker_browse_admin_form', $src, $account, $public, $range));
  $pref = '<div class="imgp_imgs_list">';
  $suff = '</div>';
  if ($public && $src == 'admin') {
    $forms['browse_public'] = render(drupal_get_form('imagepicker_browse_public_form'));
    if ( imagepicker_variable_get('imagepicker_groups_enabled', 1) && _imagepicker_has_public_groups($user, ($src=='admin' ? TRUE : FALSE))) {
      $forms['browse_public_groups'] = render(drupal_get_form('imagepicker_browse_public_groups_form', $user, TRUE));
    }
  }
  return theme('imagepicker_browse_admin', array('forms' => $forms, 'pref' => $pref, 'suff' => $suff, 'label' => $label));
}
/**
 * @param user object $account
 * @return integer
 */
function _imagepicker_user_has_img($account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker', 'i');
  $query->addExpression('COUNT(i.img_id)', 'ct');
  $query->condition('i.uid', $user->uid);
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

/**
 * groups
 */
function _imagepicker_user_has_groups($public = 'all', $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker_user_groups', 'g');
  $query->addExpression('COUNT(g.gid)', 'gidct');
  $query->condition('g.uid', $user->uid);
  if ($public == 'yes') {
    $query->condition('g.public', 1);
  }
  elseif ($public == 'no') {
    $query->condition('g.public', 0);
  }
  $row = $query->execute()->fetchObject();
  return $row->gidct;
}

function _imagepicker_user_has_grouped_img($public = 'all', $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker_group_images', 'i');
  $query->addExpression('COUNT(DISTINCT i.img_id)', 'ct');
  $query->join('imagepicker_user_groups', 'g', 'i.gid = g.gid');
  $query->condition('g.uid', $user->uid);
  if ($public == 'yes') {
    $query->condition('g.public', 1);
  }
  elseif ($public == 'no') {
    $query->condition('g.public', 0);
  }
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

function imagepicker_has_groups($account=FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker_user_groups', 'g');
  $query->addExpression('COUNT(g.gid)', 'gidct');
  $query->condition('g.uid', $user->uid);
  $row = $query->execute()->fetchObject();
  return $row->gidct;
}

// get all the groups for the current user;
function imagepicker_get_groups($account=FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker_user_groups', 'g');
  $query->fields('g', array('gid', 'uid', 'group_name', 'group_description', 'state', 'public', 'avail_roles') );
  $query->condition('g.uid', $user->uid);
  $rows = $query->execute();
  $count = 0;
  foreach ($rows AS $row) {
    $data[$row->gid] = $row->group_name . ($row->public ? ' - ' . t('Public') : ' - ' . t('Private'));
    $count++;
  }
  if ($count) {
    return $data;
  }
  return FALSE;
}


// get get the gid of the selected group
function imagepicker_get_user_group_state($state=1, $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker_user_groups', 'g');
  $query->fields('g', array('gid'));
  $query->condition('uid', $user->uid)
    ->condition('state', $state);
  $rows = $query->execute();
  $ct = 0;
  foreach ($rows AS $row) {
    $data[] = $row->gid;
    $ct++;
  }
  if ($ct) {
    return $data;
  }
  return FALSE;

}

function imagepicker_set_user_group_state($state, $gid, $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  db_update('imagepicker_user_groups')
    ->fields(array('state' => 0))
    ->condition('uid', $user->uid)
    ->condition('state', 1)
    ->execute();

  db_update('imagepicker_user_groups')
    ->fields(array('state' => $state))
    ->condition('gid', $gid)
    ->execute();
}

// for dropdown
function imagepicker_get_grouplist($account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $grouplist = array('0' => 'All');
  $query = db_select('imagepicker_user_groups', 'g');
  $query->fields('g', array('gid', 'group_name', 'public'));
  $query->distinct();
  $query->join('imagepicker_group_images', 'i', 'g.gid = i.gid');
  $query->condition('g.uid', $user->uid);
  $rows = $query->execute();
  foreach ($rows AS $row) {
    $grouplist[$row->gid] = $row->group_name . (user_access('use public imagepicker') && imagepicker_variable_get('imagepicker_public_enabled', 1) ? ($row->public ? ' - ' . t('Public') : ' - ' . t('Private')) : '');
  }
  return $grouplist;
}

// for dropdown
function imagepicker_has_grouplist($account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker_user_groups', 'g');
  $query->addExpression('COUNT(DISTINCT g.gid)', 'ct');
  $query->join('imagepicker_group_images', 'i', 'g.gid = i.gid');
  $query->condition('g.uid', $user->uid);
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

// for public dropdown
function imagepicker_get_public_grouplist($account = FALSE, $admin = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $grouplist = array('0' => 'All');
  $tmp = '';
  $query = db_select('imagepicker_user_groups', 'g');
  $query->fields('g', array('gid', 'group_name', 'avail_roles'));
  $query->condition('g.public', 1);
  $rows = $query->execute();
  $rowct = 0;
  foreach ($rows AS $row) {
    $tmp[$rowct]['gid'] = $row->gid;
    $tmp[$rowct]['group_name'] = $row->group_name;
    $tmp[$rowct]['avail_roles'] = $row->avail_roles;
    $rowct++;
  }

  // filter for role
  if (is_array($tmp)) {
    for ($ct = 0; $ct < $rowct; $ct++) {
      if (! $admin) {
        if ($tmp[$ct]['avail_roles'] != 'all' && imagepicker_variable_get('imagepicker_publicroles_enabled', 0)) {
          $role = $tmp[$ct]['avail_roles'];
          if (imagepicker_user_has_role($role, $user)) {
            $grouplist[$tmp[$ct]['gid']] = $tmp[$ct]['group_name'];
          }
        }
        else {
          $grouplist[$tmp[$ct]['gid']] = $tmp[$ct]['group_name'];
        }
      }
      else {
        $grouplist[$tmp[$ct]['gid']] = $tmp[$ct]['group_name'];
      }
    }
    return $grouplist;
  }
  return FALSE;
}

function _imagepicker_has_public_groups($account=FALSE, $admin = FALSE) {

  if (! imagepicker_variable_get('imagepicker_groups_enabled', 0)) {
    return 0;
  }
  if ($admin && imagepicker_variable_get('imagepicker_browse_public', 0) != 1) {
    return 0;
  }
  $list = imagepicker_get_public_grouplist($account, $admin);
  if (is_array($list)) {
    return count($list);
  }
  return 0;
}

function _imagepicker_has_groups($account=FALSE) {

  if (! imagepicker_variable_get('imagepicker_groups_enabled', 0)) {
    return 0;
  }
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker_user_groups', 'iug');
  $query->addExpression('COUNT(iug.gid)', 'ct');
  $query->condition('iug.uid', $user->uid);
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

// get enabled groups that have images. usually just one
function imagepicker_get_enabled_group($account = FALSE) {

  if (! imagepicker_variable_get('imagepicker_groups_enabled', 0)) {
    return FALSE;
  }
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('imagepicker_user_groups', 'g');
  $query->fields('g', array('gid', 'group_name'));
  $query->distinct();
  $query->join('imagepicker_group_images', 'i', 'g.gid = i.gid');
  $query->condition('g.uid', $user->uid);
  $query->condition('g.state', 1);
  $rows = $query->execute();
  $ct = 0;
  foreach ($rows AS $row) {
    $data[] = $row->gid;
    $ct++;
  }
  if ($ct) {
    return $data;
  }
  return FALSE;
}

function imagepicker_group_exists($gid) {
  $result = db_query("SELECT gid FROM {imagepicker_user_groups} WHERE gid = :gid", array(':gid' => $gid));
  if ($row = $result->fetchAssoc()) {
    if ($row['gid']) {
      return TRUE;
    }
  }
  return FALSE;
}

function imagepicker_browse_groups_form($form, &$form_state, $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  // all the groups for the current user which have images attached
  $grouplist = imagepicker_get_grouplist($user);
  if ($account) {
    $enabledlist = imagepicker_variable_get('imagepicker_currentgroup', 0);
    // check it
    if (! imagepicker_group_exists($enabledlist)) {
      imagepicker_variable_del("imagepicker_currentgroup");
      $enabledlist = '';
    }
  }
  else {
    $enabledlist = imagepicker_get_enabled_group($user);
    // check it
    if (! imagepicker_group_exists($enabledlist)) {
      imagepicker_variable_del("imagepicker_currentgroup", $user->uid);
      $enabledlist = '';
    }
  }

  $form['gid'] = array(
    '#type' => 'select',
    '#default_value' => $enabledlist,
    '#options' => $grouplist,
    '#title' => t('Group'),
  );
  if ($account) {
    $form['uid'] = array(
      '#type' => 'value',
      '#value' => $user->uid,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );

  return $form;
}

/**
 * Submit browse groups form
 */
function imagepicker_browse_groups_form_submit($form, &$form_state) {

  // need to get the users gids
  $account = FALSE;
  if (isset($form_state['values']['uid'])) {
    $account = user_load($form_state['values']['uid']);
    if (isset($form_state['values']['gid'])) {
    imagepicker_variable_set('imagepicker_currentgroup', $form_state['values']['gid']);
    }
    else {
    imagepicker_variable_set('imagepicker_currentgroup', 0);
    }
  }
  else {
    $gids = imagepicker_get_groups($account);
    $gids = array_keys($gids);
    foreach ($gids AS $gid) {
      $state = 0;
      if ( $gid == $form_state['values']['gid'] ) {
        $state = 1;
      }
      db_update('imagepicker_user_groups')
        ->fields(array('state' => $state ))
        ->condition('gid', $gid)
        ->execute();
    }
  }
}

/**
 * Submit public browse groups form
 *
 * @param $account
 *   Optional,
 * @param $admin
 *   Optional, sets where the form is coming from
 * @return
 *   Returns the form.
 */
function imagepicker_browse_public_groups_form($form, &$form_state, $account = FALSE, $admin = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  // all the groups for the current user which have images attached
  $grouplist = imagepicker_get_public_grouplist($user, $admin);
  if ($admin) {
    $enabledlist = imagepicker_variable_get('imagepicker_public_currentgroup', 0);
    // check it
    if (! imagepicker_group_exists($enabledlist)) {
      imagepicker_variable_del("imagepicker_public_currentgroup");
      $enabledlist = '';
    }
  }
  else {
    $enabledlist = imagepicker_variable_get('imagepicker_public_currentgroup', 0, $user->uid);
    // check it
    if (! imagepicker_group_exists($enabledlist)) {
      imagepicker_variable_del("imagepicker_public_currentgroup", $user->uid);
      $enabledlist = '';
    }
  }

  $form['gid'] = array(
    '#type' => 'select',
    '#default_value' => $enabledlist,
    '#options' => $grouplist,
    '#title' => t('Public Group'),
  );
  if ($admin) {
    $form['admin'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );

  return $form;
}

/**
 * Submit public browse groups form
 */
function imagepicker_browse_public_groups_form_submit($form, &$form_state) {

  if ( isset($form_state['values']['admin'])) {
    if ($form_state['values']['gid'] > 0) {
    imagepicker_variable_set('imagepicker_public_currentgroup', $form_state['values']['gid']);
    }
    else {
    imagepicker_variable_del('imagepicker_public_currentgroup');
    }
  }
  else {
    global $user;
    if ($form_state['values']['gid'] > 0) {
      imagepicker_variable_set('imagepicker_public_currentgroup', $form_state['values']['gid'], $user->uid);
    }
    else {
      imagepicker_variable_del('imagepicker_public_currentgroup', $user->uid);
    }
  }
}

/**
 * Insert a form into the edit image page to allow the image to be associated with a group.
 *
 * @param $img_id
 *   The id of the image to be inserted.
 * @param $account
 *   Optional, allows the administrator to edit user settings.
 * @return
 *   Returns the group image form.
 */
function imagepicker_group_images_form($form, &$form_state, $img_id, $account = FALSE) {

  $grouplist = imagepicker_get_groups($account);
  $enabledlist = imagepicker_get_image_groups($img_id);

  $form['group_images'] = array(
    '#type' => 'fieldset',
    '#title' => t('Groups'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['group_images']['grouplist'] = array(
    '#type' => 'checkboxes',
    '#default_value' => $enabledlist,
    '#options' => $grouplist,
    '#title' => t('Your Groups'),
  );
  $form['group_images']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save group settings'),
    );
  $form['img_id'] = array(
    '#type' => 'value',
    '#value' => $img_id,
  );
  return $form;
}

/**
 * Submit group images form
 */
function imagepicker_group_images_form_submit($form, &$form_state) {

  // have to delete all the entries for this image and rebuild with the new ones;
  $img_id = $form_state['values']['img_id'];
  imagepicker_delete_group_image($img_id);
  $grouplist = $form_state['values']['grouplist'];
  $inserted = FALSE;
  foreach ($grouplist AS $gid) {
    if ($gid > 0) {
      $record->gid = $gid;
      $record->img_id = $img_id;
      imagepicker_insert_group_image($record);
      $inserted = TRUE;
    }
  }
  if (! $inserted) {
    $gid = imagepicker_get_user_group_state();
    if ($gid) {
      $ids = imagepicker_get_images_by_group($gid);
      if (! $ids) {
        global $user;
        db_update('imagepicker_user_groups')
          ->fields(array('state' => 0))
          ->condition('uid', $user->uid)
          ->condition('state', 1)
          ->execute();
      }
    }
  }
}

function imagepicker_get_image_groups($img_id) {

  $data = array();
  $query = db_select('imagepicker_group_images', 'i');
  $query->fields('i', array('gid'));
  $query->condition('i.img_id', $img_id);
  $rows = $query->execute();
  foreach ($rows AS $row) {
    $data[] = $row->gid;
  }
  return $data;
}

function imagepicker_get_images_by_group($gid) {

  $query = db_select('imagepicker_group_images', 'i');
  $query->fields('i', array('img_id'));
  $query->condition('i.gid', $gid);
  $ct = 0;
  $rows = $query->execute();
  foreach ($rows AS $row) {
    $data[] = $row->img_id;
    $ct++;
  }
  if ($ct) {
    return $data;
  }
  return FALSE;
}

function imagepicker_delete_group_image($img_id) {

  db_delete('imagepicker_group_images')
    ->condition('img_id', $img_id)
    ->execute();
}

function imagepicker_insert_group_image($record) {

  if ($record->gid && $record->img_id) {
    db_insert('imagepicker_group_images')
      ->fields( array('gid' => $record->gid, 'img_id' => $record->img_id))
      ->execute();
  }
}

/**
 * Function to display the browser order form
 *
 * @param $account
 *   Optional user account object.
 * @param $admin
 *   Optional admin flag.
 * @return
 *   Returns the browser order form.
 */
function imagepicker_browse_order_form($form, &$form_state, $account = FALSE, $admin = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $default_order = imagepicker_variable_get('imagepicker_default_browser_order', 'img_id DESC');
  if ($admin) {
    $order = imagepicker_variable_get('imagepicker_browser_order', $default_order);
    $form['admin'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
  }
  else {
    $order = imagepicker_variable_get('imagepicker_browser_order', $default_order, $user->uid);
  }
  $orderlist = array(
    1 => t('Newest first'),
    2 => t('Newest last'),
    3 => t('Edited first'),
    4 => t('Edited last'),
    5 => t('By name'),
  );
  $orderlistnum = array(
    'img_id DESC' => 1,
    'img_id ASC' => 2,
    'img_date DESC' => 3,
    'img_date ASC' => 4,
    'img_name' => 5,
  );

  $form['imagepicker_browser_order'] = array(
    '#type' => 'select',
    '#default_value' => $orderlistnum[$order],
    '#options' => $orderlist,
    '#title' => t('Order'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );

  return $form;
}

/**
 * Submit form
 */
function imagepicker_browse_order_form_submit($form, &$form_state) {

  $orderlist = array(
    1 => 'img_id DESC',
    2 => 'img_id ASC',
    3 => 'img_date DESC',
    4 => 'img_date ASC',
    5 => 'img_name',
  );
  $order = $orderlist[$form_state['values']['imagepicker_browser_order']];
  if (isset($form_state['values']['admin'])) {
    imagepicker_variable_set('imagepicker_browser_order', $order);
  }
  else {
    global $user;
    imagepicker_variable_set('imagepicker_browser_order', $order, $user->uid);
  }
}

function _imagepicker_get_img($img_id, $checkuser=TRUE, $account = FALSE) {

  if (is_object($account)) {
    $user = $account;
  }
  else {
    global $user;
  }

  $query = db_select('imagepicker', 'i');
  $query->fields('i', array('img_id', 'uid', 'img_name', 'img_title', 'img_description', 'img_date'));
  $query->range(0, 1);
  $query->condition('img_id', $img_id);
  $img = $query->execute()->fetchObject();

  if ( count($img) ) {
    if (($img->uid != $user->uid) && $checkuser) {
      drupal_set_message(t('This image does not belong to you.'), 'error');
      watchdog('imagepicker', 'User uid %d attempted to edit image belonging to user uid %d', array($user->uid, $img->uid), WATCHDOG_WARNING);
      return FALSE;
    }
    // get user name
    $query = db_select('users', 'u');
    $query->fields('u', array('name'));
    $query->condition('uid', $img->uid);
    $name = $query->execute()->fetchObject();
    $img->name = $name;
    return $img;
  }
  return FALSE;
}

/**
 * statistics
 */
function imagepicker_group_stats($account = FALSE, $label = '') {

  // stats
  $groupusercount = FALSE;
  $usercount = FALSE;
  $bytecount = 0;
  if ($account) {
    if (is_int($account) && $account == -1) {
      $allcount = _imagepicker_user_has_img();
      $totcount = _imagepicker_user_has_grouped_img('all');
      $publiccount = _imagepicker_user_has_grouped_img('yes');
      $nopubliccount = _imagepicker_user_has_grouped_img('no');
      $groupcount = _imagepicker_user_has_groups('all');
      $publicgroupcount = _imagepicker_user_has_groups('yes');
      $nopublicgroupcount = _imagepicker_user_has_groups('no');
      $bytecount = imagepicker_get_all_bytes(-1);
    }
    else {
      $allcount = _imagepicker_user_has_img($account);
      $totcount = _imagepicker_user_has_grouped_img('all', $account);
      $publiccount = _imagepicker_user_has_grouped_img('yes', $account);
      $nopubliccount = _imagepicker_user_has_grouped_img('no', $account);
      $groupcount = _imagepicker_user_has_groups('all', $account);
      $publicgroupcount = _imagepicker_user_has_groups('yes', $account);
      $nopublicgroupcount = _imagepicker_user_has_groups('no', $account);
      $bytecount = imagepicker_get_all_bytes($account);
    }
  }
  else {
    // imagepicker users
    $query = db_select('imagepicker_user_groups');
    $query->addExpression('COUNT(DISTINCT uid)', 'ct');
    $row = $query->execute()->fetchAssoc();
    $groupusercount = $row['ct'];
    $query = db_select('imagepicker');
    $query->addExpression('COUNT(DISTINCT uid)', 'ct');
    $row = $query->execute()->fetchAssoc();
    $usercount = $row['ct'];
    // all groups
    $query = db_select('imagepicker_user_groups');
    $query->addExpression('COUNT(gid)', 'ct');
    $row = $query->execute()->fetchAssoc();
    $groupcount = $row['ct'];
    // public groups
    $query = db_select('imagepicker_user_groups');
    $query->addExpression('COUNT(gid)', 'ct');
    $query->condition('public', 1);
    $row = $query->execute()->fetchAssoc();
    $publicgroupcount = $row['ct'];
    // private groups
    $query = db_select('imagepicker_user_groups');
    $query->addExpression('COUNT(gid)', 'ct');
    $query->condition('public', 0);
    $row = $query->execute()->fetchAssoc();
    $nopublicgroupcount = $row['ct'];
    // all images
    $query = db_select('imagepicker');
    $query->addExpression('COUNT(img_id)', 'ct');
    $row = $query->execute()->fetchAssoc();
    $allcount = $row['ct'];
    // grouped images
    $query = db_select('imagepicker_group_images', 'gi');
    $query->addExpression('COUNT(DISTINCT gi.img_id)', 'ct');
    $query->join('imagepicker_user_groups', 'g', 'g.gid = gi.gid');
    $row = $query->execute()->fetchAssoc();
    $totcount = $row['ct'];
    // public images
    $query = db_select('users', 'u');
    $query->addExpression('COUNT(i.img_id)', 'ct');
    $query->join('imagepicker', 'i');
    $query->leftjoin('imagepicker_group_images', 'g', 'g.img_id = i.img_id');
    $query->leftjoin('imagepicker_user_groups', 'iug', 'iug.gid = g.gid');
    $query->condition('u.uid', 'iug.uid')->condition('iug.public', 1);
    $row = $query->execute()->fetchAssoc();
    $publiccount = $row['ct'];
    // private images
    $query = db_select('users', 'u');
    $query->addExpression('COUNT(i.img_id)', 'ct');
    $query->join('imagepicker', 'i');
    $query->leftjoin('imagepicker_group_images', 'g', 'g.img_id = i.img_id');
    $query->leftjoin('imagepicker_user_groups', 'iug', 'iug.gid = g.gid');
    $query->condition('u.uid', 'iug.uid')->condition('iug.public', 0);
    $row = $query->execute()->fetchAssoc();
    $nopubliccount = $row['ct'];
    $bytecount = imagepicker_get_all_bytes();
  }
  $bytecount = _imagepicker_bkmg($bytecount);

  $header = array(
    array('data' =>  t('Group statistics'), 'colspan' => 2),
    array('data' =>  t('Image statistics'), 'colspan' => 2),
  );

  if ( user_access('use public imagepicker') && imagepicker_variable_get('imagepicker_public_enabled', 1) ) {
    $rows = array(
      array(t('Groups') . ": ", $groupcount, t('Images') . ": ", $allcount),
      array(t('Public groups') . ": ", $publicgroupcount, t('Public images') . ": ", $publiccount),
      array(t('Private groups') . ": ", $nopublicgroupcount, t('Private images') . ": ", $nopubliccount),
      array( '', '', t('Grouped images') . ": ", $totcount),
      array( '', '', t('Ungrouped images') . ": ", $allcount-$totcount),
      array(($groupusercount ? t('Group Users') . ": " : ''), ($groupusercount ? $groupusercount : ''), t('Space used') . ": ", $bytecount),
    );
    if (!$account) {
      $rows = array_merge($rows, array(array(($usercount ? t('All Users') . ": " : ''), ($usercount ? $usercount : ''), '', '')));
    }
  }
  else {
    $rows = array(
      array(t('Groups') . ": ", $groupcount, t('Images') . ": ", $allcount),
      array( '', '', t('Grouped images') . ": ", $totcount),
      array( '', '', t('Ungrouped images') . ": ", $allcount-$totcount),
      array(($groupusercount ? t('Group Users') . ": " : ''), ($groupusercount ? $groupusercount : ''), t('Space used') . ": ", $bytecount),
    );
    if (!$account) {
      $rows = array_merge($rows, array(array(($usercount ? t('All Users') . ": " : ''), ($usercount ? $usercount : ''), '', '')));
    }
  }
  return theme('imagepicker_stats', array('header' => $header, 'rows' => $rows, 'message' => t('No Stats found'), 'pref' => '<div class="imgp_groups_info">', 'suff' => '</div>', 'label' => $label));
}

/**
 * Function to display the public status selection form
 *
 * @return
 *   Returns the form.
 */
function imagepicker_browse_public_form($form, &$form_state) {

  $list = array(
    0 => t('All'),
    1 => t('Public'),
    2 => t('Private'),
  );
  $form['imagepicker_browse_public'] = array(
    '#type' => 'select',
    '#default_value' => imagepicker_variable_get('imagepicker_browse_public', 0),
    '#options' => $list,
    '#title' => t('Show'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );
  return $form;
}

/**
 * Submit form
 */
function imagepicker_browse_public_form_submit($form, &$form_state) {

  imagepicker_variable_set('imagepicker_browse_public', $form_state['values']['imagepicker_browse_public']);
}

function _imagepicker_get_bytes($img_id, $account = FALSE) {

  if ($account) {
    $user = $account;
    $userdir = array('uid' => $user->uid);
  }
  else {
    global $user;
    $userdir = FALSE;
  }

  $tot = 0;
  $img = _imagepicker_get_img($img_id, FALSE, $user);
  if ($img) {
    $path = imagepicker_get_path(FALSE, $userdir);
    $fullinfo = image_get_info($path . $img->img_name);
    $thumbsinfo = image_get_info($path . IMAGEPICKER_THUMBS_DIR . '/' . $img->img_name);
    $browserinfo = image_get_info($path . IMAGEPICKER_BROWSER_DIR . '/' . $img->img_name);
    if (file_exists($path . IMAGEPICKER_ORIG_DIR . '/' . $img->img_name)) {
      $originfo = image_get_info($path . IMAGEPICKER_ORIG_DIR . '/' . $img->img_name);
    }
    else {
      $originfo = array('file_size' => 0);
    }
    $tot = $fullinfo['file_size'] + $thumbsinfo['file_size'] + $browserinfo['file_size'] + $originfo['file_size'];
  }
  else {
    drupal_set_message(t('Image not found.'), 'error');
  }
  return $tot;
}


function imagepicker_get_all_bytes($account = FALSE) {

  $tot = 0;
  $olduid = 0;
  $sql = 'SELECT img_id, uid FROM {imagepicker} ';
  $crit = array();
  if (is_object($account) ) {
    $user = $account;
    $sql .= 'WHERE uid=:uid';
    $crit = array(':uid' => $user->uid);
  }
  elseif ($account == -1) {
    global $user;
    $sql .= 'WHERE uid=:uid';
    $crit = array(':uid' => $user->uid);
  }
  $result = db_query($sql, $crit);
  while ($row = $result->fetchAssoc()) {
    if (! $account && $olduid <> $row['uid'] ) {
      $user = user_load($row['uid']);
    }
    $tot += _imagepicker_get_bytes($row['img_id'], $user);
    $olduid = $row['uid'];
  }
  return $tot;
}

function _imagepicker_bkmg($number) {

  $inc = 1000;
  $count = 1000;
  $symarr = array('K', 'M', 'G', 'T');
  $sym = 'B';
  while ($number > $count) {
    $count = $count * $inc;
    $sym = array_shift($symarr);

  }
  if ($number < $inc ) {
    if ($number > 0) {
      return ($number-1 . ' ' . $sym);
    }
    return ("0 $sym");
  }
  return (round(($number/$count)*$inc, 2) . ' ' . $sym);
}

function imagepicker_get_quota_list($key = 'x') {

  $list = array(
    0 => t('Unlimited'),
    1 => t('1 Meg'),
    5 => t('5 Meg'),
    10 => t('10 Meg'),
    25 => t('25 Meg'),
    50 => t('50 Meg'),
    100 => t('100 Meg'),
    250 => t('250 Meg'),
    500 => t('500 Meg'),
    750 => t('750 Meg'),
    1000 => t('1 Gig'),
    2000 => t('2 Gig'),
    5000 => t('5 Gig'),
  );

  if ($key == 'x') {
    return $list;
  }

  $value = $list[$key-1];
  return $value;
}

/**
 * Checks quotas
 *
 * @param $src
 *   Tells the function where the request is coming from
 * @param $account
 *   Optional user account object.
 * @param $label
 *   Optional label to pass through to the theme
 * @param $help
 *   Optional help to pass through to the theme
 * @return
 *   Returns the quota message along with the upload form, all themed
 */
function imagepicker_quota_ok($src, $account = FALSE, $label="", $help="") {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $ret = _imagepicker_quota_check($src, $account);
  $quota_ok = $ret[0];
  $message1 = $ret[1];
  $message2 = $ret[2];
  $form = '';

  if ($quota_ok) {
    if ($account) {
      if ($src == 'admin') {
        $form = render(drupal_get_form('imagepicker_upload_form', $user, TRUE));
      }
      elseif ($src == 'user') {
        $form = render(drupal_get_form('imagepicker_upload_form', $user));
      }
    }
    else {
      $form = render(drupal_get_form('imagepicker_upload_form'));
    }
  }
  return theme('imagepicker_quota_message', array('message1' => $message1, 'message2' => $message2, 'form' => $form, 'label' => $label, 'help' => $help));
}

function _imagepicker_quota_check($src, $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $usedbytes = imagepicker_get_all_bytes($user)+1;
  $usedbytesprint = _imagepicker_bkmg($usedbytes);
  $quota = 0;
  $quota_enabled = imagepicker_variable_get('imagepicker_quota_enable', 1);
  if ($quota_enabled) {
    if (imagepicker_variable_get('imagepicker_quota_byrole', 0)) {
      $roleid = imagepicker_variable_get('imagepicker_quota_role', 2);
      if (imagepicker_user_has_role($roleid, $user) && $user->uid > 1) {
        $quota = imagepicker_variable_get('imagepicker_quota_default', 0);
      }
    }
    else {
      $quota = imagepicker_variable_get('imagepicker_quota_default', imagepicker_variable_get('imagepicker_quota_default', 0), $user->uid);
    }
    if ($quota > 0) {
      $quota_ok = (( ($quota*1000000) >  $usedbytes) ? TRUE : FALSE );
    }
    else {
      $quota_ok = TRUE;
    }
  }
  else {
    $quota_ok = TRUE;
  }
  $imgtot = _imagepicker_user_has_img($user);
  $pl = format_plural($imgtot, '1 image', '@count images');
  $message1 = '';
  $message2 = '';
  if ($quota_enabled) {
    if ($quota > 0) {
      $pused = round($usedbytes / ($quota*1000000) * 100, 2);
      $quotaprint = imagepicker_get_quota_list($quota+1);
      if ($src == 'admin') {
        $message1 = t('The quota for %name is %quotaprint and has used %pused percent, or %usedbytesprint in %pl', array('%name' => $user->name, '%quotaprint' => $quotaprint, '%pused' => $pused, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
      }
      else {
        $message1 = t('Your quota is %quotaprint and you have used %pused percent, or %usedbytesprint in %pl', array('%quotaprint' => $quotaprint, '%pused' => $pused, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
      }
    }
    else {
      $quotaprint = imagepicker_get_quota_list($quota+1);
      if ($src == 'admin') {
        $message1 = t('The quota for %name is %quotaprint and has used %usedbytesprint in %pl', array('%name' => $user->name, '%quotaprint' => $quotaprint, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
      }
      else {
        $message1 = t('Your quota is %quotaprint and you have used %usedbytesprint in %pl', array('%quotaprint' => $quotaprint, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
      }
    }
  }
  else {
    if ($src == 'admin') {
      $message1 = t('%name has used %usedbytesprint in %pl', array('%name' => $user->name, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
    }
    else {
      $message1 = t('You have used %usedbytesprint in %pl', array('%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
    }
  }
  if (! $quota_ok ) {
    if ($src == 'admin') {
      $message2 = t('%name has used all of quota, please delete some files to make some room.', array('%name' => $user->name));
    }
    else {
      $message2 = t('You have used all of your quota, please delete some files to make some room.');
    }
  }
  return array($quota_ok, $message1, $message2);
}
/**
 * Function to display the image search form
 *
 * @param $account
 *   Optional user account object.
 * @param $admin
 *   Optional admin flag.
 * @return
 *   Returns the search form.
 */
function imagepicker_browse_search_form($form, &$form_state, $account = FALSE, $admin = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $sarr = '';
  if ($admin) {
    $search = imagepicker_variable_get('imagepicker_browser_search', '');
    $sarr = imagepicker_variable_get('imagepicker_browser_search_opts', '');
    $form['admin'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
  }
  else {
    $search = imagepicker_variable_get('imagepicker_browser_search', '', $user->uid);
    $sarr = imagepicker_variable_get('imagepicker_browser_search_opts', '', $user->uid);
  }

  $form['imagepicker_browser_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Search'),
    '#size' => 10,
    '#default_value' => $search,
  );
  $form['imagepicker_browser_search_by_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('By Name'),
    '#default_value' => ( is_array($sarr) ? $sarr['name'] : 0),
  );
  $form['imagepicker_browser_search_by_desc'] = array(
    '#type' => 'checkbox',
    '#title' => t('By Description'),
    '#default_value' => ( is_array($sarr) ? $sarr['desc'] : 0),
  );
  $form['imagepicker_browser_search_by_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('By Title'),
    '#default_value' => ( is_array($sarr) ? $sarr['title'] : 0),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('imagepicker_browse_search_form_reset'),
  );

  return $form;
}

/**
 * Submit form
 */
function imagepicker_browse_search_form_submit($form, &$form_state) {

  global $user;
  if ($form_state['values']['imagepicker_browser_search'] != '') {
    $arr = array(
        'name' => $form_state['values']['imagepicker_browser_search_by_name'],
        'desc' => $form_state['values']['imagepicker_browser_search_by_desc'],
        'title' => $form_state['values']['imagepicker_browser_search_by_title']);
    $search = $form_state['values']['imagepicker_browser_search'];
    if (isset($form_state['values']['admin'])) {
      imagepicker_variable_set('imagepicker_browser_search', check_plain($search));
      imagepicker_variable_set('imagepicker_browser_search_opts', $arr);
    }
    else {
      imagepicker_variable_set('imagepicker_browser_search', check_plain($search), $user->uid);
      imagepicker_variable_set('imagepicker_browser_search_opts', $arr, $user->uid);
    }
  }
  else {
    if (isset($form_state['values']['admin'])) {
      imagepicker_variable_del('imagepicker_browser_search');
      imagepicker_variable_del('imagepicker_browser_search_opts');
    }
    else {
      imagepicker_variable_del('imagepicker_browser_search', $user->uid);
      imagepicker_variable_del('imagepicker_browser_search_opts', $user->uid);
    }
  }
}

/**
 * Submit Reset button.
 */
function imagepicker_browse_search_form_reset($form, &$form_state) {

  imagepicker_browse_search_form_reset_func( isset($form_state['values']['admin']) ? $form_state['values']['admin'] : FALSE);
}

function imagepicker_browse_search_form_reset_func($admin = FALSE) {

  global $user;
  if ($admin) {
    imagepicker_variable_del('imagepicker_browser_search');
    imagepicker_variable_del('imagepicker_browser_search_opts');
  }
  else {
    imagepicker_variable_del('imagepicker_browser_search', $user->uid);
    imagepicker_variable_del('imagepicker_browser_search_opts', $user->uid);
  }
}

function _imagepicker_search_opts($searchs, $account) {

  if ($account) {
    $search_opts = imagepicker_variable_get('imagepicker_browser_search_opts', array());
  }
  else {
    global $user;
    $search_opts = imagepicker_variable_get('imagepicker_browser_search_opts', imagepicker_variable_get('imagepicker_browser_search_opts', array()), $user->uid);
  }
  if (! isset($search_opts['name']) ) {
    $search_opts['name'] = FALSE;
  }
  if (! isset($search_opts['desc']) ) {
    $search_opts['desc'] = FALSE;
  }
  if (! isset($search_opts['title']) ) {
    $search_opts['title'] = FALSE;
  }

  $os = $searchs;
  $searchs = '%' . db_like($searchs) . '%';
  if (     $search_opts['name']
      && ! $search_opts['desc']
      && ! $search_opts['title']) {
    $conditions = array('img_name', $searchs, 'LIKE');
  }
  elseif ( $search_opts['name']  && $search_opts['desc'] && ! $search_opts['title']) {
    $conditions = db_or()
      ->condition('img_name', $searchs, 'LIKE')
      ->condition('img_description', $searchs, 'LIKE');
  }
  elseif ( $search_opts['name'] && ! $search_opts['desc'] && $search_opts['title']) {
    $conditions = db_or()
      ->condition('img_name', $searchs, 'LIKE')
      ->condition('img_title', $searchs, 'LIKE');
  }
  elseif (! $search_opts['name'] && $search_opts['desc'] && $search_opts['title']) {
    $conditions = db_or()
      ->condition('img_description', $searchs, 'LIKE')
      ->condition('img_title', $searchs, 'LIKE');
  }
  elseif (! $search_opts['name'] && ! $search_opts['desc'] && $search_opts['title']) {
    $conditions = array('img_title', $searchs, 'LIKE');
  }
  elseif (! $search_opts['name'] && $search_opts['desc'] && ! $search_opts['title']) {
    $conditions = array('img_description', $searchs, 'LIKE');
  }
  else {
    $conditions = db_or()
      ->condition('img_name', $searchs, 'LIKE')
      ->condition('img_description', $searchs, 'LIKE')
      ->condition('img_title', $searchs, 'LIKE');
  }
  return ($conditions);
}

/**
 * Adapted from watermark module.
 *
 * @return
 *   Status of function availability.
 */
function imagepicker_image_check_functions($silent = FALSE) {

  $errors = 0;
  $function_list = array();
  $function_list[] = 'imagecopy';
  $function_list[] = 'imagecopyresampled';
  $function_list[] = 'imagedestroy';
  $types = array('gif', 'jpeg', 'png', 'wbmp');
  foreach ($types as $type) {
    $function_list[] = 'image' . $type;
    $function_list[] = 'imagecreatefrom' . $type;
  }
  foreach ($function_list as $function) {
    if (!function_exists($function)) {
      if (! $silent) {
        drupal_set_message(t('Function %func does not exist. Advanced image manipulation cannot be done. Please make sure that you are running PHP %ver or higher, or that you (or your hosting provider) enable the GD library in your PHP installation.', array('%func' => $function, '%ver' => IMAGEPICKER_UPLOAD_STATUS_MIN_PHP)) , 'warning');
      }
      $errors++;
    }
  }
  if ($errors) {
    return FALSE;
  }
  return TRUE;
}

function imagepicker_exif_check($silent = FALSE) {
  if (! function_exists('exif_imagetype')) {
    if (! $silent) {
      drupal_set_message(t('Function exif_imagetype does not exist.'));
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * options for dropdown.
 *
 * @return
 *   Array for position select box.
 */
function imagepicker_watermark_opts() {

  return array(
    0 => t('Middle'),
    1 => t('Middle Right'),
    2 => t('Middle Left'),
    3 => t('Top Middle'),
    4 => t('Top Left'),
    5 => t('Top Right'),
    6 => t('Bottom Middle'),
    7 => t('Bottom Right'),
    8 => t('Bottom Left'),
  );
}

function imagepicker_get_watermarks($account=FALSE) {

  $wdir = imagepicker_get_watermarks_dir($account, FALSE);
  $wdirlist = file_scan_directory($wdir, "/.*/");
  $wfiles = array();
  foreach ($wdirlist AS $k => $v) {
    $wfiles[] = $wdirlist[$k]->filename;
  }
  if (count($wfiles)) {
    return $wfiles;
  }
  return FALSE;
}

function imagepicker_get_watermarks_dir($account=FALSE, $scheme = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  if ($scheme) {
    $destdir = imagepicker_get_path(FALSE, array('name' => $user->name, 'uid' => $user->uid ), TRUE);
  }
  else {
    $destdir = imagepicker_get_path(FALSE, array('name' => $user->name, 'uid' => $user->uid ));
  }
  return $destdir . IMAGEPICKER_WATERMARK_DIR;
}

/**
 * Function to generate the copy form
 *
 * @param $img_id
 *   Required image id
 * @param $img_name
 *   Required image name
 * @param $account
 *   Optional $user object
 * @param $admin
 *   Optional admin status
 * @return
 *   The form array
 *
 */
function imagepicker_copy_form($form, &$form_state, $img_id, $img_name, $account = FALSE, $admin = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  if ($admin) {
    $form['admin'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
  }
  if ($account) {
    $form['uid'] = array(
      '#type' => 'value',
      '#value' => $user->uid,
    );
  }

  $form['img_id'] = array(
    '#type' => 'value',
    '#value' => $img_id,
  );

  $form['copy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Copy'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => $img_name,
  );
  $form['copy']['imagepicker_copy'] = array(
    '#type' => 'textfield',
    '#title' => t('Copy to'),
    '#size' => 25,
    '#default_value' => '',
  );
  $form['copy']['scale'] = array(
    '#type' => 'textfield',
    '#title' => t('Scale image'),
    '#size' => 10,
    '#default_value' => imagepicker_variable_get('imagepicker_default_scale', ''),
    '#description' => t('Scale image to this size in pixels if not left empty'),
  );

  if ( imagepicker_image_check_functions(TRUE) && imagepicker_variable_get('imagepicker_watermark_enable', 0)) {
    if (imagepicker_variable_get('imagepicker_watermark_enable', 0, $user->uid)
    && ! imagepicker_variable_get('imagepicker_watermark_image', '')
    && imagepicker_variable_get('imagepicker_watermark_image', '', $user->uid)) {
      $form['copy']['watermark'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use watermark'),
        '#description' => t('Use watermark on this image.'),
        '#default_value' => imagepicker_variable_get('imagepicker_watermark_use', FALSE, $user->uid),
      );
    }
    elseif (imagepicker_variable_get('imagepicker_watermark_image', '')) {
      $form['watermark'] = array(
        '#type' => 'value',
        '#value' => 1,
      );
    }
  }
  $form['copy']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Copy image'),
  );
  return $form;

}

/**
 * Function to validate the copy form
 *
 */
function imagepicker_copy_form_validate($form, &$form_state) {

  if (! drupal_strlen($form_state['values']['imagepicker_copy'])) {
    form_set_error('imagepicker_copy', t('You must supply a name'));
  }
  elseif (! preg_match('/^[a-zA-Z0-9_ .-]+$/', $form_state['values']['imagepicker_copy'])) {
    form_set_error('imagepicker_copy', t('You must supply a valid name'));
  }

  if (isset($form_state['values']['uid'])) {
    $user = user_load($form_state['values']['uid']);
  }
  else {
    global $user;
  }

  $img_id = $form_state['values']['img_id'];
  $img = _imagepicker_get_img($img_id, TRUE, $user);
  preg_match('/\.([a-zA-Z]+)$/', $img->img_name , $m);
  $oldext = $m[1];
  preg_match('/\.([a-zA-Z]+)$/', $form_state['values']['imagepicker_copy'] , $m);
  $newext = $m[1];
  if ( $oldext != $newext) {
    form_set_error('imagepicker_copy', t('You must use the same extension'));
  }
  if ( $img->img_name == $form_state['values']['imagepicker_copy'] ) {
    form_set_error('imagepicker_copy', t('You cannot copy a file onto itself'));
  }

  if (!preg_match('/^[0-9]{0,3}$/', $form_state['values']['scale'])) {
    form_set_error($name, t('Scale value should be an integer between 1 and 999 or leave it empty if you don\'t want to scale your image.'));
  }
}

/**
 * Function to submit the copy form
 *
 */
function imagepicker_copy_form_submit($form, &$form_state) {

  module_load_include('inc', 'imagepicker', 'imagepicker.imagefuncs');
  if (isset($form_state['values']['uid'])) {
    $user = user_load($form_state['values']['uid']);
  }
  else {
    global $user;
  }

  $img_id = $form_state['values']['img_id'];
  $newname = $form_state['values']['imagepicker_copy'];
  $img = _imagepicker_get_img($img_id, TRUE, $user);
  if ($img) {
    $destdir = imagepicker_get_path(FALSE, (isset($form_state['values']['admin']) ? array('name' => $user->name, 'uid' => $user->uid ) : TRUE));
    $thumbsdir = $destdir . IMAGEPICKER_THUMBS_DIR;
    $browserdir = $destdir . IMAGEPICKER_BROWSER_DIR;
    $origdir = $destdir . IMAGEPICKER_ORIG_DIR;
    // relative paths
    $destdirscheme = imagepicker_get_path(FALSE, (isset($form_state['values']['admin']) ? array('name' => $user->name, 'uid' => $user->uid ) : TRUE), TRUE);
    $thumbsdirscheme = $destdirscheme . IMAGEPICKER_THUMBS_DIR . DIRECTORY_SEPARATOR;
    $browserdirscheme = $destdirscheme . IMAGEPICKER_BROWSER_DIR . DIRECTORY_SEPARATOR;
    $origdirscheme = $destdirscheme . IMAGEPICKER_ORIG_DIR . DIRECTORY_SEPARATOR;

    if (file_prepare_directory($destdir, FILE_CREATE_DIRECTORY)
      && file_prepare_directory($thumbsdir, FILE_CREATE_DIRECTORY)
      && file_prepare_directory($browserdir, FILE_CREATE_DIRECTORY)
      && file_prepare_directory($origdir, FILE_CREATE_DIRECTORY)
      ) {
      // clear out the noisy 'created' messages
      drupal_get_messages('status', TRUE);
      // Add DIRECTORY_SEPARATORS here because drupals' functions remove trailing slashes
      $destdir = $destdir . DIRECTORY_SEPARATOR;
      $thumbsdir = $thumbsdir . DIRECTORY_SEPARATOR;
      $browserdir  = $browserdir . DIRECTORY_SEPARATOR;
      $origdir  = $origdir . DIRECTORY_SEPARATOR;
      $doinsert = TRUE;
      if (file_exists($destdir . $newname)) {
        $doinsert = FALSE;
      }

      $scaleto = $form_state['values']['scale'] ? $form_state['values']['scale'] : FALSE;
      $dest = $destdirscheme . $newname;
      if ($scaleto) {
        // as origdir is quite new...
        $source = $origdir . $img->img_name;
        if (! file_exists($source) ) {
          $source = $destdir . $img->img_name;
        }
        $imagescaled = imagepicker_scale_image($source, IMAGEPICKER_FILE_SCHEME . $dest, $scaleto);

        // if watermark is enabled just apply to destdir image, not orig or the thumbs
        if ( $form_state['values']['watermark'] ) {
          if (! imagepicker_watermark_process($dest, $user)) {
            drupal_set_message(t('Error while watermarking an uploaded image.'), 'error');
          }
        }
      }
      else {
        // no scaling, copy direct from $destdir to $destdir
        $source = $destdir . $img->img_name;
        imagepicker_file_unmanaged_copy($source, IMAGEPICKER_FILE_SCHEME . $dest, FILE_EXISTS_REPLACE);
      }

      $source = $thumbsdir . $img->img_name;
      $dest = $thumbsdirscheme . $newname;
      imagepicker_file_unmanaged_copy($source, IMAGEPICKER_FILE_SCHEME . $dest, FILE_EXISTS_REPLACE);

      $source = $browserdir . $img->img_name;
      $dest = $browserdirscheme . $newname;
      imagepicker_file_unmanaged_copy($source, IMAGEPICKER_FILE_SCHEME . $dest, FILE_EXISTS_REPLACE);

      $source = $origdir . $img->img_name;
      $dest = $origdirscheme . $newname;
      imagepicker_file_unmanaged_copy($source, IMAGEPICKER_FILE_SCHEME . $dest, FILE_EXISTS_REPLACE);

      if ($doinsert) {
        $nextimgid = imagepicker_insert_image($user->uid, $newname, $img->img_title, $img->img_description);
        if ($nextimgid) {
          $record->img_id = $nextimgid;
          $gids = imagepicker_get_image_groups($img_id);
          if (count($gids) ) {
            foreach ($gids AS $gid) {
              $record->gid = $gid;
              imagepicker_insert_group_image($record);
            }
          }
          drupal_set_message(t('Copy done.'));
        }
        else {
          // rollback
          file_unmanaged_delete($thumbsdir . $newname);
          file_unmanaged_delete($browserdir . $newname);
          file_unmanaged_delete($origdir . $newname);
          file_unmanaged_delete($destdir . $newname);
          // warn
          drupal_set_message(t('Copy failed.'), 'warning');
        }
      }
    }
  }
}

/**
 * Function to insert the image data into db
 *
 * @param $uid
 *   Required user id
 * @param $img_name
 *   Required image name
 * @param $img_title
 *   Optional image title
 * @param $img_description
 *   Optional image description
 * @return
 *   Returns the current img id
 */
function imagepicker_insert_image($uid, $img_name, $img_title="", $img_description="") {

  if (! $uid || ! $img_name) {
    return FALSE;
  }

  $nextimgid = db_insert('imagepicker')
    ->fields(
      array(
        'uid' => $uid,
        'img_name' => $img_name,
        'img_title' => $img_title,
        'img_description' => $img_description,
        'img_date' => time(),
      )
    )->execute();
  return $nextimgid;
}

/**
 * Function to fetch exif info
 *
 * @param $image
 *   Required image with full path
 * @return
 *   Returns the exif info
 */
function imagepicker_get_exifinfo($image) {
  $exif = '';
  if (file_exists($image)) {
    if (imagepicker_exif_check(TRUE) && imagepicker_variable_get('imagepicker_exifinfo_enable', 0)) {
      $exif = exif_read_data($image, 0, TRUE);
    }
    else {
      $extprog = imagepicker_variable_get('imagepicker_exifinfo_external', '');
      if ($extprog) {
        $exif = shell_exec("$extprog $image");
      }
    }
  }
  return $exif;
}

/**
 * Helper function to check if a user has a specific role
 */
function imagepicker_user_has_role($role, $user=NULL) {

  if ($user == NULL) {
    global $user;
  }
  // first check if $role is numeric or string
  if (is_numeric($role) ) {
    if (is_array($user->roles) && in_array($role, array_keys($user->roles))) {
      return TRUE;
    }
  }
  else {
    if (is_array($user->roles) && in_array($role, array_values($user->roles))) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Function to get an icon
 * Derived from invoice module
 *
 * @param string $name
 *   Name of icon without extension.
 * @param string $url
 *   URL to link the icon to.
 * @param array $attributes
 *   Any optional HTML attributes.
 * @param string $extension
 *   The file extension.
 * @return
 *   The icon string.
 */
function _imagepicker_get_icon($name, $url = NULL, $attributes = array(), $extension = 'png') {

  if (empty($attributes['alt'])) {
    $attributes['alt'] = $attributes['title'];
  }

  $img_addition = '';
  foreach ($attributes as $key => $value) {
    $img_addition .= ' ' . $key . '="' . $value . '"';
  }

  $icon = '<img src="' . base_path() . drupal_get_path('module', 'imagepicker') . '/images/' . $name . '.' . $extension . '"' . $img_addition . ' />';
  if (!empty($url)) {
    $icon = l($icon, $url, array('html' => TRUE));
  }
  return $icon;
}
// all iframe links pass through here
function imagepicker_box() {
  $a1 = FALSE;
  $a2 = 0;
  $a3 = FALSE;

  if (arg(1)) {
    $a1 = arg(1);
  }
  if (arg(2) && is_numeric(arg(2))) {
    $a2 = arg(2);
  }
  if (arg(3) && $a2) {
    $a3 = arg(3);
  }

  if ($a1 == 'browse') {
    $content = imagepicker_browse($a2);
  }
  elseif ($a1 == 'browse_public') {
    $content = imagepicker_browse_public($a2);
  }
  elseif ($a1 == 'groups') {
    $content = imagepicker_groups();
  }
  elseif ($a1 == 'edit') {
    $content = imagepicker_image_edit($a2);
  }
  elseif ($a1 == 'image') {
    $content = imagepicker_image_page($a2, $a3);
  }
  elseif ($a1 == 'upload_postlet') {
    $content = imagepicker_postlet_page();
  }
  else {
    $content = imagepicker_upload();
  }

  drupal_add_css(IMAGEPICKER_PATH . '/imagepicker.css');

  $output = theme('imagepicker', array('content' => $content));
  print $output;
  exit;

}

/**
 * callback for uploadprogress information function.
 */
function imagepicker_uploadprogress_callback($progress_key='') {

  if (! $progress_key) {
    $progress_key = $_GET['key'];
  }

  if (imagepicker_variable_get('imagepicker_uploadprogress_server', '') && function_exists('uploadprogress_get_info') && $progress_key) {
    $status = uploadprogress_get_info($progress_key);
    if ($status['bytes_total']) {
      $status['status'] = 1;
      $status['percentage'] = round($status['bytes_uploaded']/$status['bytes_total'] * 100, 0);
      $eta = sprintf("%02d:%02d", $status['est_sec'] / 60, $status['est_sec'] % 60 );
      $speed = _imagepicker_bkmg($status['speed_average']);
      $bytes_total = _imagepicker_bkmg($status['bytes_total']);
      $status['message'] = t('Filesize: !bytes_total. !eta left at !speed/sec.', array('!eta' => $eta, '!speed' => $speed, '!bytes_total' => $bytes_total));
    }
    else {
      $status['status'] = 1;
      $status['percentage'] = -1;
      $status['message'] = imagepicker_variable_get('imagepicker_upload_progress_message', t('Processing form... please wait.'));
    }
    echo json_encode($status);
  }

  exit;
}

function imagepicker_file_unmanaged_copy($s, $d, $r = FILE_EXISTS_RENAME) {
  // file_unmanaged_copy does not check for trailing slashes
  $d = rtrim($d, DIRECTORY_SEPARATOR);
  $n = file_unmanaged_copy($s, $d, $r);
  if ($n) {
    // Fix bug in drupal's file_copy function which uses '/' instead of
    // DIRECTORY_SEPARATOR for making directories. This causes problems on
    // Windows machines. still true in D7
    $n2 = preg_replace("#^" . IMAGEPICKER_FILE_SCHEME . "#", '', $n);
    $n2 = str_replace('/', DIRECTORY_SEPARATOR, $n2);
    $n = IMAGEPICKER_FILE_SCHEME . $n2;
  }
  return $n;
}

function imagepicker_file_unmanaged_move($s, $d, $r = FILE_EXISTS_RENAME) {
  // file_unmanaged_move does not check for trailing slashes
  $d = rtrim($d, DIRECTORY_SEPARATOR);
  $n = file_unmanaged_move($s, $d, $r);
  if ($n) {
    // Fix bug in drupal's file_copy function which uses '/' instead of
    // DIRECTORY_SEPARATOR for making directories. This causes problems on
    // Windows machines. still true in D7
    $n2 = preg_replace("#^" . IMAGEPICKER_FILE_SCHEME . "#", '', $n);
    $n2 = str_replace('/', DIRECTORY_SEPARATOR, $n2);
    $n = IMAGEPICKER_FILE_SCHEME . $n2;
  }
  return $n;
}

function imagepicker_get_insert_template() {
  $file = IMAGEPICKER_PATH . DIRECTORY_SEPARATOR . IMAGEPICKER_INSERT_TEMPLATE;
  $t = "<div class='imgp_title'>__TITLE__</div>__INSERT__<div class='imgp_desc'>__DESC__</div>";
  $template = '';
  if ( file_exists($file) ) {
    $template = file_get_contents($file);
  }
  if ($template) {
    return $template;
  }
  return $t;
}
