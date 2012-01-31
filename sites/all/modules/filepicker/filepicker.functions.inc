<?php

/**
 * @file
 * filepicker functions.
 */

/**
 * Menu local task; presents the list and select pages for filepicker
 */
function filepicker_list($file_id=0) {
  if ( $file_id ) {
    return filepicker_file_select($file_id);
  }
  else {
    return theme('filepicker_iframe', array('content' => _filepicker_list_admin()));
  }
}

function filepicker_list_public($file_id=0) {
  if ( $file_id ) {
    return filepicker_file_select($file_id, FALSE, TRUE);
  }
  else {
    return theme('filepicker_iframe', array('content' => _filepicker_list_admin('iframe', FALSE, TRUE)));
  }
}

function filepicker_file_select($file_id, $showgroup=TRUE, $public = FALSE, $account = FALSE) {
  global $base_url;
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $file = _filepicker_get_file($file_id, ($public ? FALSE : TRUE));
  if ($file) {
    $form1 = '';
    if ($user->uid == $file->uid || $public) {
      if (filepicker_variable_get('filepicker_use_full_url', 0)) {
        $filepath = $base_url . DIRECTORY_SEPARATOR . file_directory_path() . DIRECTORY_SEPARATOR . FILEPICKER_FILES_DIR . DIRECTORY_SEPARATOR . $file->uid . DIRECTORY_SEPARATOR . $file->file_name;
      }
      else {
        $filepath = filepicker_get_file_path($file, ($public ? array('uid' => $file->uid) : FALSE ));
      }
      $form1 = render(drupal_get_form('filepicker_file_form', $file, $filepath, $public));
    }

    // groups
    $form2 = '';
    if (_filepicker_has_groups() && $showgroup) {
      $form2 = render(drupal_get_form('filepicker_group_files_form', $file_id));
    }

    $content = theme('filepicker_insert', array('file' => $file, 'public' => $public, 'form1' => $form1, 'form2' => $form2));

  }
  else {
    drupal_set_message(t('File not found.'), 'error');
    $content = '';
  }

  return theme('filepicker_iframe', array('content' => $content, 'file' => $file, 'public' => $public));
}

/**
 * Function to display the file insertion form
 *
 * @param $file_id
 *   The id of the file to be inserted.
 * @param $public
 *   Optional, ensures that public files cannot be edited.
 * @return
 *   Returns the file form.
 */
function filepicker_file_form($form, &$form_state, $file, $filepath, $public = FALSE) {
  global $user;
  $form['insert'] = array(
    '#type' => 'button',
    '#value' => t('Insert file'),
    '#attributes' => array('onclick' => 'filepickerInsert(this); return false;')
  );
  if (! $public) {
    $form['edit'] = array(
      '#type' => 'submit',
      '#value' => t('Edit file'),
      '#submit' => array('filepicker_file_form_edit'),
    );
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete file'),
      '#submit' => array('filepicker_file_form_delete'),
    );
  }
  $form['file_id'] = array(
    '#type' => 'value',
    '#value' => $file->file_id,
  );
  // for js to pick up
  $form['file_title'] = array(
    '#type' => 'hidden',
    '#value' => $file->file_title,
  );
  $form['file_path'] = array(
    '#type' => 'hidden',
    '#value' => $filepath,
  );
  $form['file_name'] = array(
    '#type' => 'hidden',
    '#value' => $file->file_name,
  );

  return $form;
}

/**
 * Submit form functions
 */
function filepicker_file_form_delete($form, &$form_state) {
  filepicker_file_delete($form_state['values']['file_id']);
}

function filepicker_file_form_edit($form, &$form_state) {
  drupal_goto('filepicker/edit/' . $form_state['values']['file_id']);
}

function filepicker_file_delete($file_id, $account=FALSE, $src='iframe') {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  if ($src == 'account') {
    $outpath = 'user/' . $user->uid . '/filepicker/files/list';
  }
  elseif ($src == 'admin') {
    $outpath = FILEPICKER_ADMIN_PATH . '/files/user/' . $user->uid . '/list';
  }
  else {
    $outpath ='filepicker/list';
  }
  _filepicker_file_delete($file_id, $account, $src);
  drupal_goto($outpath);
}

function _filepicker_file_delete($file_id, $account=FALSE, $src='iframe', $suppress = FALSE) {
  if ($account) {
    $user = $account;
    $userdir = $user->uid;
  }
  else {
    global $user;
    $userdir = TRUE;
  }
  $file = _filepicker_get_file($file_id, ($src=='admin' ? FALSE : TRUE), ($account ? $user : FALSE));
  if ($file) {
    $destination = filepicker_get_path(FALSE, $userdir);
    file_unmanaged_delete($destination . $file->file_name);

    if (db_delete('filepicker')->condition('uid', $user->uid)->condition('file_id', $file_id)->execute() ) {
      // groups entries
      db_delete('filepicker_group_files')->condition('file_id', $file_id)->execute();
      if (! $suppress) {
        drupal_set_message(t('File was successfully deleted'));
      }
    }
    else {
      drupal_set_message(t('Error while trying to delete your file from database.'), 'error');
    }
  }
  else {
    drupal_set_message(t('File not found.'), 'error');
  }
  return;
}


// There is not need to inform users, that directory structure has been created
// and show them all paths... So lets strip these messages if there are any.
function filepicker_strip_messages($msg) {
  if ($msg) {
    $dirsep = (DIRECTORY_SEPARATOR == '\\') ? '\\\\' : '\/';
    $pattern = '/<li>.*' . $dirsep . 'filepicker' . $dirsep . '.*<\/li>/i';
    $msg = preg_replace($pattern, '', $msg);
  }
  return $msg;
}

/**
 * list of files with bulk operations
 */
function filepicker_list_admin_form($form, &$form_state, $src="iframe", $account=FALSE, $public=FALSE, $range=1) {

  global $base_url;
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  // paths
  if ($src == 'iframe') {
    $path = 'filepicker';
    $editpath   = "filepicker/edit/";
    $deletepath = "filepicker/delete/";
    $filepath    = 'filepicker/list/';
    if ($public) {
      $filepath    = 'filepicker/list_public/';
    }
    $returnpath = 'i-' . $user->uid;
  }
  elseif ($src == 'admin') {
    $path = FILEPICKER_ADMIN_PATH;
    $editpath   = $path . '/files/user/' . $user->uid . '/edit/';
    $deletepath = $path . '/files/user/' . $user->uid . '/delete/';
    $filepath    = $path . '/files/user/' . $user->uid . '/list/';
    $returnpath = 'a-' . $user->uid;
  }
  else {
    $path = 'user/' . $user->uid . '/filepicker';
    $editpath   = $path . '/files/edit/';
    $deletepath = $path . '/files/delete/';
    $filepath    = $path . '/files/list/';
    $returnpath = 'u-' . $user->uid;
  }

  // if there are groups
  if (filepicker_variable_get('filepicker_groups_enabled', 1)) {
    $gid = 0;
    if (! $public) {
      if (_filepicker_has_groups( ($src == 'admin' ? $user : FALSE) ) && filepicker_has_grouplist(($src == 'admin' ? $user : FALSE))) {
        $gids = filepicker_get_user_group_state(1, ($src == 'admin' ? $user : FALSE));
        $gid = $gids[0];
        if ($account) {
          $gid = filepicker_variable_get('filepicker_currentgroup', 0);
        }
      }
    }
    elseif ($src == 'admin' && _filepicker_has_public_groups($user, TRUE)) {
      $gid = filepicker_variable_get('filepicker_public_currentgroup', 0);
    }
  }

  // tablesort
  if ($src == 'admin') {
    if ($public) {
      $header = array(
        'file_name' => array('data' => t('Name'), 'field' => 'i.file_name'),
        'file_title' => array('data' => t('Link text'), 'field' => 'i.file_title'),
        'file_list' => array('data' => t('User'), 'field' => 'n.name'),
        'file_date' => array('data' => t('Date'), 'field' => 'i.file_date'),
        'file_actions' => array('data' => t('Actions')),
      );
    }
    else {
      $header = array(
        'file_name' => array('data' => t('Name'), 'field' => 'i.file_name'),
        'file_title' => array('data' => t('Link text'), 'field' => 'i.file_title'),
        'file_date' => array('data' => t('Date'), 'field' => 'i.file_date'),
        'file_actions' => array('data' => t('Actions')),
      );
    }
  }
  else {
    if ($public) {
      $header = array(
        'file_name' => array('data' => t('Name'), 'field' => 'i.file_name'),
        'file_title' => array('data' => t('Link text'), 'field' => 'i.file_title'),
        'file_date' => array('data' => t('Date'), 'field' => 'i.file_date'),
      );
    }
    else {
      $header = array(
        'file_name' => array('data' => t('Name'), 'field' => 'i.file_name'),
        'file_title' => array('data' => t('Link text'), 'field' => 'i.file_title'),
        'file_date' => array('data' => t('Date'), 'field' => 'i.file_date'),
        'file_actions' => array('data' => t('Actions')),
      );
    }
  }

  $use_icons = filepicker_variable_get('filepicker_use_icons', 1);
  if ($src == 'admin') {
    $max = filepicker_variable_get('filepicker_rows_per_page', 25);
    $searchs = filepicker_variable_get('filepicker_list_search', '');
  }
  else {
    $max = (filepicker_variable_get('filepicker_rows_per_page', '', $user->uid) ? filepicker_variable_get('filepicker_rows_per_page', '', $user->uid) : filepicker_variable_get('filepicker_rows_per_page', 25));
    $searchs = filepicker_variable_get('filepicker_list_search', '', $user->uid);
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
      $query->fields('i', array('file_id', 'uid', 'file_name', 'file_title', 'file_date'));
      $query->addField('u', 'name');
      $query->join('filepicker', 'i');
      $query->leftjoin('filepicker_group_files', 'g', 'g.file_id = i.file_id');
      $query->leftjoin('filepicker_user_groups', 'iug', 'iug.gid = g.gid');
      $query->condition('u.uid', 'iug.uid')->condition('iug.public', $publicstate);
      if ($gid && $publicstate) {
        $query->condition('iug.gid', $gid);
      }
      if ($range == 1) {
        $label = t('List All Public Files');
      }
      else {
        $label = t('List All Private Files');
      }
    }
    else {
      $query = db_select('filepicker', 'i')
        ->extend('PagerDefault')->limit($max)
        ->extend('TableSort')->orderByHeader($header);
      $query->fields('i', array('file_id', 'uid', 'file_name', 'file_title', 'file_date'));
      $query->join('users', 'u', 'i.uid = u.uid');
      $query->addField('u', 'name');
      $query->condition('u.status', 1);
    }
  }
  else {
    // filter by selected group
    if ($gid) {
      $query = db_select('filepicker', 'i')
        ->extend('PagerDefault')->limit($max)
        ->extend('TableSort')->orderByHeader($header);
      $query->fields('i', array('file_id', 'uid', 'file_name', 'file_title', 'file_date'));
      $query->addField('u', 'name');
      $query->join('filepicker_group_files', 'g', 'i.file_id = g.file_id');
      $query->join('users', 'u', 'i.uid = u.uid');
      $query->condition('u.uid', $user->uid)
        ->condition('u.status', 1)
        ->condition('gid', $gid);
    }
    else {
      if ($public) {
        $query = db_select('users', 'u')
          ->extend('PagerDefault')->limit($max)
          ->extend('TableSort')->orderByHeader($header);
        $query->fields('i', array('file_id', 'uid', 'file_name', 'file_title', 'file_date'));
        $query->addField('u', 'name');
        $query->join('filepicker', 'i');
        $query->leftjoin('filepicker_group_files', 'g', 'g.file_id = i.file_id');
        $query->leftjoin('filepicker_user_groups', 'iug', 'iug.gid = g.gid');
        $query->condition('u.uid', 'iug.uid')->condition('iug.public', 1);
      }
      else {
        $query = db_select('filepicker', 'i')
          ->extend('PagerDefault')->limit($max)
          ->extend('TableSort')->orderByHeader($header);
        $query->fields('i', array('file_id', 'uid', 'file_name', 'file_title', 'file_date'));
        $query->addField('u', 'name');
        $query->join('users', 'u', 'i.uid = u.uid');
        $query->condition('u.uid', $user->uid)
          ->condition('u.status', 1);
      }
    }
  }

  // do search
  $searchsql = '';
  if ($searchs) {
    $searchsql = _filepicker_search_opts($searchs, $account);
  }
  if (is_array($searchsql) ) {
    $query->condition($searchsql[0], $searchsql[1], $searchsql[2]);
  }
  elseif (is_object($searchsql)) {
    $query->condition($searchsql);
  }

  $records = $query->execute();

  if (! $public) {
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
      '#options' => $op_opts,
      '#default_value' => 'delete',
    );
    $form['options']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
    );
  }

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
  $fmt = filepicker_variable_get('filepicker_dateformat_default', 'medium');
  $rowct = 0;
  $options = array();
  // iterate
  foreach ($records AS $row) {
    // file_id file_name  file_title
    $file_name = check_plain($row->file_name);
    $file_id = $row->file_id;
    $filelink = l($file_name, $filepath . $file_id);
    $file_date = format_date($row->file_date, $fmt);

    if ($public && $src == 'admin') {
      $editpath   = $path . '/files/user/' . $row->uid . '/edit/';
      $deletepath = $path . '/files/user/' . $row->uid . '/delete/';
      $listpath   = $path . '/files/user/' . $row->uid . '/list';
      $returnpath = 'a';
      $editlink = ($use_icons ? _filepicker_get_icon('edit', $editpath . $file_id, array('title' => t('Edit'))) : l(t('Edit'), $editpath . $file_id));
      $deletelink = ($use_icons ? _filepicker_get_icon('delete', $deletepath . $file_id, array('title' => t('Delete'))) : l(t('Delete'), $deletepath . $file_id));
      $u = user_load($row->uid);
      $listlink = l($u->name, $listpath);
      $options[$file_id] = array(
        'file_name' => $filelink,
        'file_title' => check_plain($row->file_title),
        'file_list' => $listlink,
        'file_date' => $file_date,
        'file_actions' => "$editlink $deletelink",
      );
      $cols = 6;
    }
    else {
      if ($public) {
        $editlink = "&nbsp;";
        $deletelink = "&nbsp;";
        if ($src == 'account') {
          $filelink = $file_name;
        }
        $options[$file_id] = array(
          'file_name' => $filelink,
          'file_title' => check_plain($row->file_title),
          'file_date' => $file_date,
        );
      }
      else {
        $editlink = ($use_icons ? _filepicker_get_icon('edit', $editpath . $file_id, array('title' => t('Edit'))) : l(t('Edit'), $editpath . $file_id));
        $deletelink = ($use_icons ? _filepicker_get_icon('delete', $deletepath . $file_id, array('title' => t('Delete'))) : l(t('Delete'), $deletepath . $file_id));
        $options[$file_id] = array(
          'file_name' => $filelink,
          'file_title' => check_plain($row->file_title),
          'file_date' => $file_date,
          'file_actions' => "$editlink $deletelink",
        );
      }
      $cols = 5;
    }
    $rowct++;
  } // end of while loop

  $form['options']['returnpath'] = array(
    '#type' => 'value',
    '#value' => $returnpath,
  );

  // tableselect
  $message = t('No files found');
  $form['files'] = array(
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
function filepicker_list_admin_form_submit($form, &$form_state) {
  $operation = $form_state['values']['operation'];
  $path = '';
  if (isset($form_state['values']['path'])) {
    $path = $form_state['values']['path'];
  }
  $returnpath = $form_state['values']['returnpath'];
  $files = array_filter($form_state['values']['files']);
  $file_ids = '';
  if ($files) {
    foreach ($files AS $file_id) {
      $file_ids[] = $file_id;
    }
  }
  if (is_array($file_ids)) {
    $sfile_ids = implode('-', $file_ids);
    drupal_goto($path . "/multitask/$operation/$sfile_ids/$returnpath");
  }
  else {
    $rpath = filepicker_multitask_returnpath($returnpath);
    drupal_set_message(t('No files selected'));
    drupal_goto($rpath);
  }
}

/**
 *
 * Menu callback for filepicker multitask.
 */
function filepicker_multitask($mode = '', $sfile_ids = '', $returnpath = '') {
  $cancelpath = filepicker_multitask_returnpath($returnpath);
  $output = '';
  if ($mode && $sfile_ids) {
    switch ($mode) {
      case 'delete':
        // really delete?, auto themed
        $output .= render(drupal_get_form('filepicker_multitask_delete_form', $sfile_ids, $returnpath));
        $output .= l(t('Cancel'), $cancelpath);
        break;
      case 'groups':
        // select some groups
        $output .= render(drupal_get_form('filepicker_multitask_groups_form', $sfile_ids, $returnpath));
        $output .= l(t('Cancel'), $cancelpath);
        break;
    }
  }
  return $output;
}

function filepicker_multitask_delete_form($form, &$form_state, $sfile_ids, $returnpath) {

  $form['multitask_delete'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bulk delete'),
    '#description' => t('Delete all the selected files.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['multitask_delete']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete All'),
  );
  $form['sfile_ids'] = array('#type' => 'value', '#value' => $sfile_ids);
  $form['returnpath'] = array('#type' => 'value', '#value' => $returnpath);
  $file_ids = explode('-', $sfile_ids);
  $form['countnids'] = array('#type' => 'value', '#value' => count($file_ids));
  $form['#submit'][] = 'filepicker_multitask_delete_form_submit';
  return $form;
}

/**
 * Submit form
 */
function filepicker_multitask_delete_form_submit($form, &$form_state) {
  $sfile_ids = $form_state['values']['sfile_ids'];
  $returnpath = $form_state['values']['returnpath'];

  $file_ids = explode('-', $sfile_ids);
  foreach ($file_ids AS $file_id) {
    _filepicker_file_delete($file_id, FALSE, 'admin');
  }

  drupal_set_message(t('%c deleted', array('%c' => format_plural( count($file_ids), '1 file', '@count files'))));
  $rpath = filepicker_multitask_returnpath($returnpath);
  drupal_goto($rpath);
}

function filepicker_multitask_groups_form($form, &$form_state, $sfile_ids, $returnpath) {

  $file_ids = explode('-', $sfile_ids);
  $countnids = count($file_ids);

  $query = db_select('filepicker', 'i');
  $query->fields('i', array('uid'));
  $query->condition('i.file_id', $file_ids[0]);

  $row = $query->execute()->fetchObject();
  $account = user_load($row->uid);
  $grouplist = filepicker_get_groups($account);

  $form['multitask_groups'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bulk groups management'),
    '#description' => t('Add/Remove all the selected files from groups.'),
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
    '#description' => t('Add or Remove selected files to one or more selected groups.'),
  );
  $form['multitask_groups']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add/Remove All'),
  );

  $form['sfile_ids'] = array('#type' => 'value', '#value' => $sfile_ids);
  $form['returnpath'] = array('#type' => 'value', '#value' => $returnpath);
  $form['countnids'] = array('#type' => 'value', '#value' => $countnids);
  $form['#submit'][] = 'filepicker_multitask_groups_form_submit';
  return $form;
}

/**
 * Validate form
 */
function filepicker_multitask_groups_form_validate($form, &$form_state) {
  $grouplist = array_filter($form_state['values']['grouplist']);
  if (! count($grouplist)) {
    form_set_error('grouplist', t('You did not select any groups.'));
  }
}

/**
 * Submit form
 */
function filepicker_multitask_groups_form_submit($form, &$form_state) {

  $sfile_ids = $form_state['values']['sfile_ids'];
  $file_ids = explode('-', $sfile_ids);
  $returnpath = $form_state['values']['returnpath'];
  $action = $form_state['values']['action'];
  // list of selected groups
  $grouplist = array_filter($form_state['values']['grouplist']);
  $gids = '';
  foreach ($grouplist AS $gid) {
    $gids[] = $gid;
  }

  if (is_array($gids) && count($file_ids)) {
    foreach ($file_ids AS $file_id) {
      foreach ($gids AS $gid) {
        $query = db_select('filepicker_group_files', 'i');
        $query->addExpression('COUNT(i.gid)', 'ct');
        $query->condition('i.gid', $gid);
        $query->condition('i.file_id', $file_id);
        $row = $query->execute()->fetchObject();
        $found = FALSE;
        if ($row->ct > 0) {
          $found = TRUE;
        }
        if ($action == 'add') {
          if (! $found) {
            // add $file_id to $gid
            $object = new stdClass();
            $object->gid = $gid;
            $object->file_id = $file_id;
            filepicker_insert_group_file($object);
          }
        }
        else {
          if ($found) {
            // remove $file_id from $gid
            filepicker_delete_group_file($file_id);
          }
        }
      }
    }
    drupal_set_message(t('%c', array('%c' => format_plural( count($file_ids), '1 file', '@count files'))) . ' ' . ($action == 'add' ? t('added') : t('removed')));
  }

  $rpath = filepicker_multitask_returnpath($returnpath);
  drupal_goto($rpath);
}

function filepicker_multitask_returnpath($returnpath) {
  $rpath = '';
  if ($returnpath == 'a') {
    $rpath = FILEPICKER_ADMIN_PATH . '/files/list_all';
  }
  else {
    $a = explode('-', $returnpath);
    if (is_numeric($a[1])) {
      if ($a[0] == 'a') {
        $rpath = FILEPICKER_ADMIN_PATH . '/files/user/' . $a[1] . '/list';
      }
      elseif ($a[0] == 'u') {
        $rpath = 'user/' . $a[1] . '/filepicker/files/list';
      }
      elseif ($a[0] == 'i') {
        $rpath = 'filepicker/list';
      }
    }
  }
  return $rpath;
}

function _filepicker_list_admin($src="iframe", $account=FALSE, $public=FALSE, $range=1, $label='') {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $content = "";
  $forms = array();
  $forms['list_search'] = '';
  $forms['list_groups'] = '';
  $forms['list_public'] = '';
  $forms['list_public_groups'] = '';
  $forms['list_admin'] = '';

  if ( $src == 'admin' && ! $account && filepicker_variable_get('filepicker_groups_enabled', 1)) {
    $forms['list_public'] = render(drupal_get_form('filepicker_list_public_form'));
  }
  // if there are groups
  if (filepicker_variable_get('filepicker_groups_enabled', 1)) {
    if ( _filepicker_has_public_groups($user, ($src=='admin' ? TRUE : FALSE)) && $src == 'admin' ) {
      // add groups select here
      $forms['list_public_groups'] = render(drupal_get_form('filepicker_list_public_groups_form', $user, ($src=='admin' ? TRUE : FALSE)));
    }
    if (! $public && _filepicker_has_groups( ($src == 'admin' ? $user : FALSE) ) && filepicker_has_grouplist(($src == 'admin' ? $user : FALSE))) {
      // add groups select here
      $forms['list_groups'] = render(drupal_get_form('filepicker_list_groups_form', ($src == 'admin' ? $user : FALSE)));
    }
  }

  if ($src != 'admin' && filepicker_variable_get('filepicker_show_list_search_form', filepicker_variable_get('filepicker_show_list_search_form', 1), $user->uid) ) {
    $forms['list_search'] = render(drupal_get_form('filepicker_list_search_form'));
  }
  elseif (filepicker_variable_get('filepicker_show_list_search_form', 1)) {
    $forms['list_search'] = render(drupal_get_form('filepicker_list_search_form', FALSE, TRUE));
  }

  $forms['list_admin'] = render(drupal_get_form('filepicker_list_admin_form', $src, $account, $public, $range));
  $pref = '<div class="filep_files_list">';
  $suff = '</div>';
  #if ($public && $src == 'admin') {
  if ($public) {
    $content .= render(drupal_get_form('filepicker_list_public_form'));
    if ( filepicker_variable_get('filepicker_groups_enabled', 1) && _filepicker_has_public_groups($user, ($src=='admin' ? TRUE : FALSE))) {
      // add groups select here
      $forms['list_public_groups'] = render(drupal_get_form('filepicker_list_public_groups_form', $user, TRUE));
    }
  }
  return theme('filepicker_list_admin', array('forms' => $forms, 'pref' => $pref, 'suff' => $suff, 'label' => $label));
}

/**
 * @param user object $account
 * @return integer
 */
function _filepicker_user_has_file($account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker', 'i');
  $query->addExpression('COUNT(i.file_id)', 'ct');
  $query->condition('i.uid', $user->uid);
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

/**
 * groups
 */
function _filepicker_user_has_groups($public = 'all', $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker_user_groups', 'g');
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


function _filepicker_user_has_grouped_file($public = 'all', $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker_group_files', 'i');
  $query->addExpression('COUNT(DISTINCT i.file_id)', 'ct');
  $query->join('filepicker_user_groups', 'g', 'i.gid = g.gid');
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


/**
 * count of groups per user
 */
function filepicker_has_groups($account=FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker_user_groups', 'g');
  $query->addExpression('COUNT(g.gid)', 'gidct');
  $query->condition('g.uid', $user->uid);
  $row = $query->execute()->fetchObject();
  return $row->gidct;
}

// get all the groups for the current user;
function filepicker_get_groups($account=FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker_user_groups', 'g');
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

/**
 * get all for a group
 */
function filepicker_get_user_group($gid) {

  $query = db_select('filepicker_user_groups', 'g');
  $query->fields('g', array('gid', 'uid', 'group_name', 'group_description', 'state', 'public', 'avail_roles') );
  $query->condition('g.gid', $gid);
  $row = $query->execute()->fetchObject();
  return $row;
}

/**
 * get the gid of the selected group
 */
function filepicker_get_user_group_state($state=1, $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker_user_groups', 'g');
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

/**
 * set group state
 */
function filepicker_set_user_group_state($state, $gid, $account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  db_update('filepicker_user_groups')
    ->fields(array('state' => 0))
    ->condition('uid', $user->uid)
    ->condition('state', 1)
    ->execute();

  db_update('filepicker_user_groups')
    ->fields(array('state' => $state))
    ->condition('gid', $gid)
    ->execute();
}

/**
 * build groups select
 */
function filepicker_get_grouplist($account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $grouplist = array('0' => 'All');

  $query = db_select('filepicker_user_groups', 'g');
  $query->fields('g', array('gid', 'group_name', 'public'));
  $query->distinct();
  $query->join('filepicker_group_files', 'i', 'g.gid = i.gid');
  $query->condition('g.uid', $user->uid);
  $rows = $query->execute();

  foreach ($rows AS $row) {
    $grouplist[$row->gid] = $row->group_name . (user_access('use public filepicker') && filepicker_variable_get('filepicker_public_enabled', 1) ? ($row->public ? ' - ' . t('Public') : ' - ' . t('Private')) : '');
  }
  return $grouplist;
}

/**
 * get how many groups
 */
function filepicker_has_grouplist($account = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $query = db_select('filepicker_user_groups', 'g');
  $query->addExpression('COUNT(DISTINCT g.gid)', 'ct');
  $query->join('filepicker_group_files', 'i', 'g.gid = i.gid');
  $query->condition('g.uid', $user->uid);
  $row = $query->execute()->fetchObject();

  return $row->ct;
}

/**
 * for public dropdown
 */
function filepicker_get_public_grouplist($account = FALSE, $admin = FALSE) {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $grouplist = array('0' => 'All');
  $tmp = '';

  $rows = db_select('filepicker_user_groups', 'g')
    ->fields('g', array('gid', 'group_name', 'avail_roles'))
    ->condition('g.public', 1)
    ->execute();

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
        if ($tmp[$ct]['avail_roles'] != 'all' && filepicker_variable_get('imagepicker_publicroles_enabled', 0)) {
          $role = $tmp[$ct]['avail_roles'];
          if (filepicker_user_has_role($role, $user)) {
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

/**
 * how many public groups
 */
function _filepicker_has_public_groups($account=FALSE, $admin = FALSE) {

  if (! filepicker_variable_get('filepicker_groups_enabled', 0)) {
    return 0;
  }
  if ($admin && filepicker_variable_get('filepicker_list_public', 0) != 1) {
    return 0;
  }
  $list = filepicker_get_public_grouplist($account, $admin);
  if (is_array($list)) {
    return count($list);
  }
  return 0;
}

/**
 * how many groups
 */
function _filepicker_has_groups($account=FALSE) {

  if (! filepicker_variable_get('filepicker_groups_enabled', 0)) {
    return 0;
  }
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker_user_groups', 'iug');
  $query->addExpression('COUNT(iug.gid)', 'ct');
  $query->condition('iug.uid', $user->uid);
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

/**
 * get enabled groups that have files. usually just one
 */
function filepicker_get_enabled_group($account = FALSE) {

  if (! filepicker_variable_get('filepicker_groups_enabled', 0)) {
    return FALSE;
  }
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker_user_groups', 'g');
  $query->fields('g', array('gid', 'group_name'));
  $query->distinct();
  $query->join('filepicker_group_files', 'i', 'g.gid = i.gid');
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

function filepicker_group_exists($gid) {
  $result = db_query("SELECT gid FROM {filepicker_user_groups} WHERE gid = :gid", array(':gid' => $gid));
  if ($row = $result->fetchAssoc()) {
    if ($row['gid']) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * all the groups for the current user which have files attached
 */
function filepicker_list_groups_form($form, &$form_state, $account = FALSE) {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $grouplist = filepicker_get_grouplist($user);
  if ($account) {
    $enabledlist = filepicker_variable_get('filepicker_currentgroup', 0);
    // check it
    if (! filepicker_group_exists($enabledlist)) {
      filepicker_variable_del("filepicker_currentgroup");
      $enabledlist = '';
    }
  }
  else {
    $enabledlist = filepicker_get_enabled_group($user);
    // check it
    if (! filepicker_group_exists($enabledlist)) {
      filepicker_variable_del("filepicker_currentgroup", $user->uid);
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
 * Submit list groups form
 */
function filepicker_list_groups_form_submit($form, &$form_state) {
  // need to get the users gids
  $account = FALSE;
  if ( isset($form_state['values']['uid'])) {
    $account = user_load($form_state['values']['uid']);
    if ($form_state['values']['gid']) {
    filepicker_variable_set('filepicker_currentgroup', $form_state['values']['gid']);
    }
    else {
    filepicker_variable_set('filepicker_currentgroup', 0);
    }
  }
  else {
    $gids = filepicker_get_groups($account);
    $gids = array_keys($gids);
    foreach ($gids AS $gid) {
      $state = 0;
      if ( $gid == $form_state['values']['gid'] ) {
        $state = 1;
      }
      db_update('filepicker_user_groups')
        ->fields(array('state' => $state ))
        ->condition('gid', $gid)
        ->execute();
    }
  }
}

/**
 * Submit public list groups form
 *
 * @param $account
 *   Optional,
 * @param $admin
 *   Optional, sets where the form is coming from
 * @return
 *   Returns the form.
 */
function filepicker_list_public_groups_form($form, &$form_state, $account = FALSE, $admin = FALSE) {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  // all the groups for the current user which have files attached
  $grouplist = filepicker_get_public_grouplist($user, $admin);
  if ($admin) {
    $enabledlist = filepicker_variable_get('filepicker_public_currentgroup', 0);
    // check it
    if (! filepicker_group_exists($enabledlist)) {
      filepicker_variable_del("filepicker_public_currentgroup");
      $enabledlist = '';
    }
  }
  else {
    $enabledlist = filepicker_variable_get('filepicker_public_currentgroup', 0, $user->uid);
    // check it
    if (! filepicker_group_exists($enabledlist)) {
      filepicker_variable_del("filepicker_public_currentgroup", $user->uid);
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
 * Submit public list groups form
 */
function filepicker_list_public_groups_form_submit($form, &$form_state) {

  if ( isset($form_state['values']['admin'])) {
    if ($form_state['values']['gid'] > 0) {
    filepicker_variable_set('filepicker_public_currentgroup', $form_state['values']['gid']);
    }
    else {
    filepicker_variable_del('filepicker_public_currentgroup');
    }
  }
  else {
    global $user;
    if ($form_state['values']['gid'] > 0) {
      filepicker_variable_set('filepicker_public_currentgroup', $form_state['values']['gid'], $user->uid);
    }
    else {
      filepicker_variable_del('filepicker_public_currentgroup', $user->uid);
    }
  }
}

/**
 * Insert a form into the edit file page to allow the file to be associated with a group.
 *
 * @param $file_id
 *   The id of the file to be inserted.
 * @param $account
 *   Optional, allows the administrator to edit user settings.
 * @return
 *   Returns the group file form.
 */
function filepicker_group_files_form($form, &$form_state, $file_id, $account = FALSE) {

  $grouplist = filepicker_get_groups($account);
  $enabledlist = filepicker_get_file_groups($file_id);

  $form['group_files'] = array(
    '#type' => 'fieldset',
    '#title' => t('Groups'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['group_files']['grouplist'] = array(
    '#type' => 'checkboxes',
    '#default_value' => $enabledlist,
    '#options' => $grouplist,
    '#title' => t('Your Groups'),
  );
  $form['group_files']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save group settings'),
    );
  $form['file_id'] = array(
    '#type' => 'value',
    '#value' => $file_id,
  );
  return $form;
}

/**
 * Submit group files form
 */
function filepicker_group_files_form_submit($form, &$form_state) {

  // have to delete all the entries for this file and rebuild with the new ones;
  $file_id = $form_state['values']['file_id'];
  filepicker_delete_group_file($file_id);
  $grouplist = $form_state['values']['grouplist'];
  $inserted = FALSE;
  foreach ($grouplist AS $gid) {
    if ($gid > 0) {
      $record->gid = $gid;
      $record->file_id = $file_id;
      filepicker_insert_group_file($record);
      $inserted = TRUE;
    }
  }
  if (! $inserted) {
    $gid = filepicker_get_user_group_state();
    if ($gid) {
      $ids = filepicker_get_files_by_group($gid);
      if (! $ids) {
        global $user;
        db_update('filepicker_user_groups')
          ->fields(array('state' => 0))
          ->condition('uid', $user->uid)
          ->condition('state', 1)
          ->execute();
      }
    }
  }
}

function filepicker_get_file_groups($file_id) {

  $data = array();
  $rows = db_select('filepicker_group_files', 'i')
    ->fields('i', array('gid'))
    ->condition('i.file_id', $file_id)
    ->execute();

  foreach ($rows AS $row) {
    $data[] = $row->gid;
  }
  return $data;
}

function filepicker_get_files_by_group($gid) {
  $rows = db_select('filepicker_group_files', 'i')
    ->fields('i', 'file_id')
    ->condition('i.gid', $gid)
    ->execute();

  $ct = 0;
  foreach ($rows AS $row) {
    $data[] = $row->file_id;
    $ct++;
  }
  if ($ct) {
    return $data;
  }
  return FALSE;
}

function filepicker_delete_group_file($file_id) {

  db_delete('filepicker_group_files')
    ->condition('file_id', $file_id)
    ->execute();
}

function filepicker_insert_group_file($record) {

  if ($record->gid && $record->file_id) {
    db_insert('filepicker_group_files')
      ->fields( array('gid' => $record->gid, 'file_id' => $record->file_id))
      ->execute();
  }
}

function _filepicker_get_file($file_id, $checkuser=TRUE, $account = FALSE) {

  if (is_object($account)) {
    $user = $account;
  }
  else {
    global $user;
  }

  $file = db_select('filepicker', 'i')
    ->fields('i', array('file_id', 'uid', 'file_name', 'file_title', 'file_date'))
    ->range(0, 1)
    ->condition('file_id', $file_id)
    ->execute()
    ->fetchObject();

  if ( count($file) ) {
    if (($file->uid != $user->uid) && $checkuser) {
      drupal_set_message(t('This file does not belong to you.'), 'error');
      watchdog('filepicker', 'User uid %d attempted to edit file belonging to user uid %d', array($user->uid, $file->uid), WATCHDOG_WARNING);
      return FALSE;
    }
    // get user name
    $name = db_select('users', 'u')
      ->fields('u', array('name'))
      ->condition('uid', $file->uid)
      ->execute()
      ->fetchObject();

    $file->name = $name;
    return $file;
  }
  return FALSE;
}

/**
 * statistics
 */
function filepicker_group_stats($account = FALSE, $label = '') {

  // stats
  $groupusercount = FALSE;
  $usercount = FALSE;
  $bytecount = 0;
  if ($account) {
    if (is_int($account) && $account == -1) {
      $allcount = _filepicker_user_has_file();
      $totcount = _filepicker_user_has_grouped_file('all');
      $publiccount = _filepicker_user_has_grouped_file('yes');
      $nopubliccount = _filepicker_user_has_grouped_file('no');
      $groupcount = _filepicker_user_has_groups('all');
      $publicgroupcount = _filepicker_user_has_groups('yes');
      $nopublicgroupcount = _filepicker_user_has_groups('no');
      $bytecount = filepicker_get_all_bytes(-1);
    }
    else {
      $allcount = _filepicker_user_has_file($account);
      $totcount = _filepicker_user_has_grouped_file('all', $account);
      $publiccount = _filepicker_user_has_grouped_file('yes', $account);
      $nopubliccount = _filepicker_user_has_grouped_file('no', $account);
      $groupcount = _filepicker_user_has_groups('all', $account);
      $publicgroupcount = _filepicker_user_has_groups('yes', $account);
      $nopublicgroupcount = _filepicker_user_has_groups('no', $account);
      $bytecount = filepicker_get_all_bytes($account);
    }
  }
  else {
    // filepicker users
    $query = db_select('filepicker_user_groups');
    $query->addExpression('COUNT(DISTINCT uid)', 'ct');
    $row = $query->execute()->fetchAssoc();
    $groupusercount = $row['ct'];
    $query = db_select('filepicker');
    $query->addExpression('COUNT(DISTINCT uid)', 'ct');
    $row = $query->execute()->fetchAssoc();
    $usercount = $row['ct'];
    // all groups
    $query = db_select('filepicker_user_groups');
    $query->addExpression('COUNT(gid)', 'ct');
    $row = $query->execute()->fetchAssoc();
    $groupcount = $row['ct'];
    // public groups
    $query = db_select('filepicker_user_groups');
    $query->addExpression('COUNT(gid)', 'ct');
    $query->condition('public', 1);
    $row = $query->execute()->fetchAssoc();
    $publicgroupcount = $row['ct'];
    // private groups
    $query = db_select('filepicker_user_groups');
    $query->addExpression('COUNT(gid)', 'ct');
    $query->condition('public', 0);
    $row = $query->execute()->fetchAssoc();
    $nopublicgroupcount = $row['ct'];
    // all files
    $query = db_select('filepicker');
    $query->addExpression('COUNT(file_id)', 'ct');
    $row = $query->execute()->fetchAssoc();
    $allcount = $row['ct'];
    // grouped files
    $query = db_select('filepicker_group_files', 'gi');
    $query->addExpression('COUNT(DISTINCT gi.file_id)', 'ct');
    $query->join('filepicker_user_groups', 'g', 'g.gid = gi.gid');
    $row = $query->execute()->fetchAssoc();
    $totcount = $row['ct'];
    // public files
    $query = db_select('users', 'u');
    $query->addExpression('COUNT(i.file_id)', 'ct');
    $query->join('filepicker', 'i');
    $query->leftjoin('filepicker_group_files', 'g', 'g.file_id = i.file_id');
    $query->leftjoin('filepicker_user_groups', 'iug', 'iug.gid = g.gid');
    $query->condition('u.uid', 'iug.uid')->condition('iug.public', 1);
    $row = $query->execute()->fetchAssoc();
    $publiccount = $row['ct'];
    // private files
    $query = db_select('users', 'u');
    $query->addExpression('COUNT(i.file_id)', 'ct');
    $query->join('filepicker', 'i');
    $query->leftjoin('filepicker_group_files', 'g', 'g.file_id = i.file_id');
    $query->leftjoin('filepicker_user_groups', 'iug', 'iug.gid = g.gid');
    $query->condition('u.uid', 'iug.uid')->condition('iug.public', 0);
    $row = $query->execute()->fetchAssoc();
    $nopubliccount = $row['ct'];
    $bytecount = filepicker_get_all_bytes();
  }
  $bytecount = _filepicker_bkmg($bytecount);

  $header = array(
    array('data' =>  t('Group statistics'), 'colspan' => 2),
    array('data' =>  t('File statistics'), 'colspan' => 2),
  );

  if ( user_access('use public filepicker') && filepicker_variable_get('filepicker_public_enabled', 1) ) {
    $rows = array(
      array(t('Groups') . ": ", $groupcount, t('Files') . ": ", $allcount),
      array(t('Public groups') . ": ", $publicgroupcount, t('Public files') . ": ", $publiccount),
      array(t('Private groups') . ": ", $nopublicgroupcount, t('Private files') . ": ", $nopubliccount),
      array( '', '', t('Grouped files') . ": ", $totcount),
      array( '', '', t('Ungrouped files') . ": ", $allcount-$totcount),
      array(($groupusercount ? t('Group Users') . ": " : ''), ($groupusercount ? $groupusercount : ''), t('Space used') . ": ", $bytecount),
    );
    if (!$account) {
      $rows = array_merge($rows, array(array(($usercount ? t('All Users') . ": " : ''), ($usercount ? $usercount : ''), '', '')));
    }

  }
  else {
    $rows = array(
      array(t('Groups') . ": ", $groupcount, t('Files') . ": ", $allcount),
      array( '', '', t('Grouped files') . ": ", $totcount),
      array( '', '', t('Ungrouped files') . ": ", $allcount-$totcount),
      array(($groupusercount ? t('Group Users') . ": " : ''), ($groupusercount ? $groupusercount : ''), t('Space used') . ": ", $bytecount),
    );
    if (!$account) {
      $rows = array_merge($rows, array(array(($usercount ? t('All Users') . ": " : ''), ($usercount ? $usercount : ''), '', '')));
    }
  }
  return theme('filepicker_stats', array('header' => $header, 'rows' => $rows, 'message' => t('No Stats found'), 'pref' => '<div class="filep_groups_info">', 'suff' => '</div>', 'label' => $label));
}

/**
 * Function to display the public status selection form
 *
 * @return
 *   Returns the form.
 */
function filepicker_list_public_form($form, &$form_state) {
  $list = array(
    0 => t('All'),
    1 => t('Public'),
    2 => t('Private'),
  );
  $form['filepicker_list_public'] = array(
    '#type' => 'select',
    '#default_value' => filepicker_variable_get('filepicker_list_public', 0),
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
function filepicker_list_public_form_submit($form, &$form_state) {
  filepicker_variable_set('filepicker_list_public', $form_state['values']['filepicker_list_public']);
}

function _filepicker_get_bytes($file_id, $account = FALSE) {
  if ($account) {
    $user = $account;
    $userdir = array('uid' => $user->uid);
  }
  else {
    global $user;
    $userdir = FALSE;
  }

  $tot = 0;
  $file = _filepicker_get_file($file_id, FALSE, $user);
  if ($file) {
    $path = filepicker_get_path(FALSE, $userdir);
    if (file_exists($path . '/' . $file->file_name)) {
      $tot = filesize($path . '/' . $file->file_name);
    }
  }
  else {
    drupal_set_message(t('File not found.'), 'error');
  }
  return $tot;
}

function filepicker_get_all_bytes($account = FALSE) {

  $tot = 0;
  $olduid = 0;
  if (is_object($account) ) {
    $user = $account;
  }
  else {
    global $user;
  }
  $query = db_select('filepicker', 'i');
  $query->fields('i', array('file_id', 'uid'));
  $query->condition('uid', $user->uid);
  $rows = $query->execute();
  foreach ($rows AS $row) {
    if (! $account && $olduid <> $row->uid) {
      $user = user_load($row->uid);
    }
    $tot += _filepicker_get_bytes($row->file_id, $user);
    $olduid = $row->uid;
  }
  return $tot;
}

function _filepicker_bkmg($number) {
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

function filepicker_get_quota_list($key = 'x') {
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
function filepicker_quota_ok($src, $account = FALSE, $label="", $help="") {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  $ret = _filepicker_quota_check($src, $account);
  $quota_ok = $ret[0];
  $message1 = $ret[1];
  $message2 = $ret[2];
  $form = '';

  if ($quota_ok) {
    if ($account) {
      if ($src == 'admin') {
        $form = render(drupal_get_form('filepicker_upload_form', $user, TRUE));
      }
      elseif ($src == 'user') {
        $form = render(drupal_get_form('filepicker_upload_form', $user));
      }
    }
    else {
      $form = render(drupal_get_form('filepicker_upload_form'));
    }
  }
  return theme('filepicker_quota_message', array('message1' => $message1, 'message2' => $message2, 'form' => $form, 'label' => $label, 'help' => $help));
}

function _filepicker_quota_check($src, $account = FALSE) {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $usedbytes = filepicker_get_all_bytes($user)+1;
  $usedbytesprint = _filepicker_bkmg($usedbytes);
  $quota = 0;
  $quota_enabled = filepicker_variable_get('filepicker_quota_enable', 1);
  if ($quota_enabled) {
    if (filepicker_variable_get('filepicker_quota_byrole', 0)) {
      $roleid = filepicker_variable_get('filepicker_quota_role', 2);
      if (filepicker_user_has_role($roleid, $user) && $user->uid > 1) {
        $quota = filepicker_variable_get('filepicker_quota_default', 0);
      }
    }
    else {
      $quota = (filepicker_variable_get('filepicker_quota_default', 0, $user->uid) ? filepicker_variable_get('filepicker_quota_default', 0, $user->uid) : filepicker_variable_get('filepicker_quota_default', 0));
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
  $filetot = _filepicker_user_has_file($user);
  $pl = format_plural($filetot, '1 file', '@count files');
  $message1 = '';
  $message2 = '';
  if ($quota_enabled) {
    if ($quota > 0) {
      $pused = round($usedbytes / ($quota*1000000) * 100, 2);
      $quotaprint = filepicker_get_quota_list($quota+1);
      if ($src == 'admin') {
        $message1 = t('The quota for %name is %quotaprint and has used %pused percent, or %usedbytesprint in %pl', array('%name' => $user->name, '%quotaprint' => $quotaprint, '%pused' => $pused, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
      }
      else {
        $message1 = t('Your quota is %quotaprint and you have used %pused percent, or %usedbytesprint in %pl', array('%quotaprint' => $quotaprint, '%pused' => $pused, '%usedbytesprint' => $usedbytesprint, '%pl' => $pl));
      }
    }
    else {
      $quotaprint = filepicker_get_quota_list($quota+1);
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
 * Function to display the file search form
 *
 * @param $account
 *   Optional user account object.
 * @param $admin
 *   Optional admin flag.
 * @return
 *   Returns the search form.
 */
function filepicker_list_search_form($form, &$form_state, $account = FALSE, $admin = FALSE) {
  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }
  $sarr = '';
  if ($admin) {
    $search = filepicker_variable_get('filepicker_list_search', '');
    $sarr = filepicker_variable_get('filepicker_list_search_opts', '');
    $form['admin'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
  }
  else {
    $search = filepicker_variable_get('filepicker_list_search', '', $user->uid);
    $sarr = filepicker_variable_get('filepicker_list_search_opts', '', $user->uid);
  }

  $form['filepicker_list_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Search'),
    '#size' => 10,
    '#default_value' => $search,
  );
  $form['filepicker_list_search_by_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('By Name'),
    '#default_value' => ( is_array($sarr) ? $sarr['name'] : 0),
  );
  $form['filepicker_list_search_by_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('By Link text'),
    '#default_value' => ( is_array($sarr) ? $sarr['title'] : 0),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('filepicker_list_search_form_reset'),
  );

  return $form;
}

/**
 * Submit form
 */
function filepicker_list_search_form_submit($form, &$form_state) {
  global $user;
  if ($form_state['values']['filepicker_list_search'] != '') {
    $arr = array(
        'name' => $form_state['values']['filepicker_list_search_by_name'],
        'title' => $form_state['values']['filepicker_list_search_by_title']);
    $search = $form_state['values']['filepicker_list_search'];
    if (isset($form_state['values']['admin'])) {
      filepicker_variable_set('filepicker_list_search', $search);
      filepicker_variable_set('filepicker_list_search_opts', $arr);
    }
    else {
      filepicker_variable_set('filepicker_list_search', $search, $user->uid);
      filepicker_variable_set('filepicker_list_search_opts', $arr, $user->uid);
    }
  }
  else {
    if (isset($form_state['values']['admin'])) {
      filepicker_variable_del('filepicker_list_search');
      filepicker_variable_del('filepicker_list_search_opts');
    }
    else {
      filepicker_variable_del('filepicker_list_search', $user->uid);
      filepicker_variable_del('filepicker_list_search_opts', $user->uid);
    }
  }
}

/**
 * Submit Reset button.
 */
function filepicker_list_search_form_reset($form, &$form_state) {
  filepicker_list_search_form_reset_func( (isset($form_state['values']['admin']) ? TRUE : FALSE));
}

function filepicker_list_search_form_reset_func($admin = FALSE) {
  global $user;
    if ($admin) {
      filepicker_variable_del('filepicker_list_search');
      filepicker_variable_del('filepicker_list_search_opts');
    }
    else {
      filepicker_variable_del('filepicker_list_search', $user->uid);
      filepicker_variable_del('filepicker_list_search_opts', $user->uid);
    }
}

function _filepicker_search_opts($searchs, $account) {

  if ($account) {
    $search_opts = filepicker_variable_get('filepicker_list_search_opts', array());
  }
  else {
    global $user;
    $search_opts = filepicker_variable_get('filepicker_list_search_opts', filepicker_variable_get('filepicker_list_search_opts', array()), $user->uid);
  }
  if (! isset($search_opts['name']) ) {
    $search_opts['name'] = FALSE;
  }
  if (! isset($search_opts['title']) ) {
    $search_opts['title'] = FALSE;
  }

  $os = $searchs;
  $searchs = '%' . db_like($searchs) . '%';
  if ($search_opts['name'] && ! $search_opts['title']) {
    $conditions = array('file_name', $searchs, 'LIKE');
  }
  elseif ($search_opts['name'] && $search_opts['title']) {
    $conditions = db_or()
      ->condition('file_name', $searchs, 'LIKE')
      ->condition('file_title', $searchs, 'LIKE');
  }
  elseif (! $search_opts['name'] && $search_opts['title']) {
    $conditions = array('file_title', $searchs, 'LIKE');
  }
  else {
    $conditions = db_or()
      ->condition('file_name', $searchs, 'LIKE')
      ->condition('file_title', $searchs, 'LIKE');
  }
  return ($conditions);
}

/**
 * Function to insert the file data into db
 *
 * @param $uid
 *   Required user id
 * @param $file_name
 *   Required file name
 * @param $file_title
 *   Optional file title
 * @return
 *   Returns the current file id
 */
function filepicker_insert_file($uid, $file_name, $file_title="") {

  if (! $uid || ! $file_name) {
    return FALSE;
  }

  $nextfileid = db_insert('filepicker')
    ->fields(
      array(
        'uid' => $uid,
        'file_name' => $file_name,
        'file_title' => $file_title,
        'file_date' => time(),
      )
    )->execute();
  return $nextfileid;
}

/**
 * Helper function to check if a user has a specific role
 */
function filepicker_user_has_role($role, $user=NULL) {
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
 * @param $name
 *   Name of icon without extension.
 * @param $url
 *   URL to link the icon to.
 * @param $attributes
 *   Any optional HTML attributes.
 * @param $extension
 *   The file extension.
 * @return
 *   The icon string.
 */
function _filepicker_get_icon($name, $url = NULL, $attributes = array(), $extension = 'png') {
  if (empty($attributes['alt'])) {
    $attributes['alt'] = $attributes['title'];
  }

  $file_addition = '';
  foreach ($attributes as $key => $value) {
    $file_addition .= ' ' . $key . '="' . $value . '"';
  }

  $icon = '<img src="' . base_path() . drupal_get_path('module', 'filepicker') . '/images/' . $name . '.' . $extension . '"' . $file_addition . ' />';
  if (!empty($url)) {
    $icon = l($icon, $url, array('html' => TRUE));
  }
  return $icon;
}

function filepicker_group_files_count($gid) {
  $query = db_select('filepicker_group_files', 'g');
  $query->addExpression('COUNT(g.file_id)', 'ct');
  $query->condition('g.gid', $gid);
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

function filepicker_get_info($file) {

  $filepath = filepicker_get_path(FALSE, array('uid' => $file->uid));
  $info['file_size'] = filesize($filepath . $file->file_name);
  return $info;
}

function filepicker_validate_is_file($filename) {
  if (filepicker_variable_get('filepicker_upload_mimetypes', 0)) {
    $mimetype = file_get_mimetype($filename);
    if ($mimetype == 'application/octet-stream') {
      return '';
    }
  }
  if (preg_match("!.*\.(.*)$!i", $filename, $matches)) {
    if ($matches[1]) {
      $filext = drupal_strtolower($matches[1]);
      $extensions = explode(' ', filepicker_variable_get('filepicker_upload_extensions', 'jpg jpeg gif png txt doc xls pdf ppt pps odt ods odp'));
      if (in_array($filext, $extensions)) {
        return $matches[1];
      }
    }
  }
  return '';
}

/**
 * all iframe links pass through here
 */
function filepicker_box() {
  $a1 = FALSE;
  $a2 = 0;

  if (arg(1)) {
    $a1 = arg(1);
  }
  if (arg(2) && is_numeric(arg(2))) {
    $a2 = arg(2);
  }

  if ($a1 == 'list') {
    $content = filepicker_list($a2);
  }
  elseif ($a1 == 'list_public') {
    $content = filepicker_list_public($a2);
  }
  elseif ($a1 == 'groups') {
    $content = filepicker_groups();
  }
  elseif ($a1 == 'edit') {
    $content = filepicker_file_edit($a2);
  }
  elseif ($a1 == 'multitask') {
    $content = filepicker_multitask(arg(2), arg(3), arg(4));
  }
  else {
    module_load_include('inc', 'filepicker', 'filepicker.upload');
    $content = filepicker_upload();
  }

  drupal_add_css(FILEPICKER_PATH . '/filepicker.css');

  $output = theme('filepicker', array('content' => $content));
  print $output;
  exit;

}

/**
 * callback for uploadprogress information function.
 */
function filepicker_uploadprogress_callback($progress_key='') {

  if (! $progress_key) {
    $progress_key = $_GET['key'];
  }

  if (filepicker_variable_get('filepicker_uploadprogress_server', '') && function_exists('uploadprogress_get_info') && $progress_key) {
    $status = uploadprogress_get_info($progress_key);
    if ($status['bytes_total']) {
      $status['status'] = 1;
      $status['percentage'] = round($status['bytes_uploaded']/$status['bytes_total'] * 100, 0);
      $eta = sprintf("%02d:%02d", $status['est_sec'] / 60, $status['est_sec'] % 60 );
      $speed = _filepicker_bkmg($status['speed_average']);
      $bytes_total = _filepicker_bkmg($status['bytes_total']);
      $status['message'] = t('Filesize: !bytes_total. !eta left at !speed/sec.', array('!eta' => $eta, '!speed' => $speed, '!bytes_total' => $bytes_total));
    }
    else {
      $status['status'] = 1;
      $status['percentage'] = -1;
      $status['message'] = filepicker_variable_get('filepicker_upload_progress_message', t('Processing form... please wait.'));
    }
    echo json_encode($status);

  }

  exit;
}

function filepicker_file_unmanaged_copy($s, $d, $r = FILE_EXISTS_RENAME) {
  // file_unmanaged_copy does not check for trailing slashes
  $d = rtrim($d, DIRECTORY_SEPARATOR);
  $n = file_unmanaged_copy($s, $d, $r);
  if ($n) {
    // Fix bug in drupal's file_copy function which uses '/' instead of
    // DIRECTORY_SEPARATOR for making directories. This causes problems on
    // Windows machines. still true in D7
    $n2 = preg_replace("#^" . FILEPICKER_FILE_SCHEME . "#", '', $n);
    $n2 = str_replace('/', DIRECTORY_SEPARATOR, $n2);
    $n = FILEPICKER_FILE_SCHEME . $n2;
  }
  return $n;
}

function filepicker_file_unmanaged_move($s, $d, $r = FILE_EXISTS_RENAME) {
  $d = rtrim($d, DIRECTORY_SEPARATOR);
  $n = file_unmanaged_move($s, $d, $r);
  if ($n) {
    // Fix bug in drupal's file_copy function which uses '/' instead of
    // DIRECTORY_SEPARATOR for making directories. This causes problems on
    // Windows machines. still true in D7
    $n2 = preg_replace("#^" . FILEPICKER_FILE_SCHEME . "#", '', $n);
    $n2 = str_replace('/', DIRECTORY_SEPARATOR, $n2);
    $n = FILEPICKER_FILE_SCHEME . $n2;
  }
  return $n;
}
