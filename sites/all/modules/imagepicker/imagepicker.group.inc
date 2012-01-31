<?php

/**
 * @file
 * contains the functions for group management
 */

function imagepicker_groups() {
  drupal_add_js(IMAGEPICKER_PATH . '/imagepicker.js');
  $content = '';
  // from iframe
  if (! arg(2)) {
    $content .= imagepicker_groups_list();
    $content .= render(drupal_get_form('imagepicker_groups_form'));
  }
  elseif ( arg(2) == 'edit') {
    $record = imagepicker_get_user_group(arg(3));
    $content .= render(drupal_get_form('imagepicker_groups_form', $record));
  }
  elseif (arg(2) == 'delete') {
    $content .= render(drupal_get_form('imagepicker_group_delete_form', arg(3)));
  }
  elseif ( is_numeric(arg(2)) && arg(2) > 0 ) {
    imagepicker_set_user_group_state(1, arg(2));
    drupal_goto('imagepicker/browse');
  }

  return theme('imagepicker_iframe', array('content' => $content));
}

/**
 * groups
 * from imagepicker groups in my account
 */
function imagepicker_user_groups($mode="", $gid=0) {

  // $mode is edit, delete or empty
  $content = '';
  if (empty($mode)) {
    $content = imagepicker_groups_list('account');
    $content .= render(drupal_get_form('imagepicker_groups_form'));
  }
  elseif ($mode == 'edit') {
    $record = imagepicker_get_user_group($gid);
    $content .= render(drupal_get_form('imagepicker_groups_form', $record));
  }
  elseif ($mode == 'delete') {
    $content .= render(drupal_get_form('imagepicker_group_delete_form', arg(3)));
  }

  return $content;
}

function imagepicker_groups_form($form, &$form_state, $record=FALSE, $account=FALSE) {

  $form['groupsave'] = array(
    '#type' => 'fieldset',
    '#title' => ( $record ?  t('Edit group') : t('Add group')),
    '#description' => t('Give your group a brief name and optionally put any additional information in the group description box'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['groupsave']['group_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Group name'),
    '#size' => 20,
    '#default_value' => ($record ? $record->group_name : ''),
    '#required' => TRUE
  );
  $form['groupsave']['group_description'] = array(
    '#type' => 'textfield',
    '#title' => t('group description'),
    '#size' => 50,
    '#maxlength' => 50,
    '#default_value' => ($record ? $record->group_description : ''),
    '#description' => t('Maximum 50 characters'),
    '#required' => FALSE
  );
  if (user_access('use public imagepicker') && user_access('create public imagepicker groups') && imagepicker_variable_get('imagepicker_public_enabled', 1)) {
    $form['groupsave']['group_public'] = array(
      '#type' => 'checkbox',
      '#title' => t('Public'),
      '#return_value' => 1,
      '#default_value' => (isset($record->public) ? $record->public : ''),
      '#description' => t('Make this group publicly available'),
    );
    // Create role restrictions
    if (imagepicker_variable_get('imagepicker_publicroles_enabled', 1)) {
      $form['groupsave']['group_public_roles'] = array(
        '#type' => 'fieldset',
        '#title' => ( isset($record->gid) ?  t('Edit group roles') : t('Add public group roles')),
        '#description' => t('Add roles for public access to your group. (Don\'t check anything to make it available to all roles.)'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      // Output a list of available roles as checkboxes
      $form['groupsave']['group_public_roles']['roles']['#tree'] = TRUE;
      if (isset($record->avail_roles)) {
        if ($record->avail_roles != 'all') $availroles = explode(':', $record->avail_roles);
      }
      foreach (user_roles(TRUE) as $rid => $role) {
        if (imagepicker_role_has_permission($rid)) {
          if (isset($availroles) && is_array($availroles) && in_array($role, array_values($availroles))) $checked = TRUE;
          else $checked = FALSE;
          $form['groupsave']['group_public_roles']['roles'][$rid] = array(
            '#type' => 'checkbox',
            '#title' => $role,
            '#return_value' => $role,
            '#default_value' => ($checked ? 1 : ''),
          );
        }
      }
    }
  }

  if ($account) {
    $form['groupsave']['account'] = array(
      '#type' => 'value',
      '#value' => $account->uid,
    );
  }
  if (isset($record->gid)) {
    $form['groupsave']['gid'] = array(
      '#type' => 'value',
      '#value' => $record->gid,
    );
  }
  $form['groupsave']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save group'),
  );

  return $form;
}

function imagepicker_groups_form_submit($form, &$form_state) {

  global $user;
  $record['group_name'] = $form_state['values']['group_name'];
  $record['group_description'] = $form_state['values']['group_description'];
  $record['public'] = $form_state['values']['group_public'];

  // Check role restrictions submitted
  if (isset($form_state['values']['roles'])) {
    $found = 0;
    foreach ($form_state['values']['roles'] as $rid => $role) {
      if (!$role) {
        unset($form_state['values']['roles'][$rid]);
      }
      else {
        $found++;
      }
    }
    if ($found) {
      $record['avail_roles'] = implode(':', $form_state['values']['roles']);
    }
    else {
      $record['avail_roles'] = 'all';
    }
  }
  else {
    $record['avail_roles'] = 'all';
  }

  if ( isset($form_state['values']['account'] )) {
    $record['uid'] = $form_state['values']['account'];
  }
  else {
    $record['uid'] = $user->uid;
  }

  if (isset($form_state['values']['gid'])) {
    $record['gid'] = $form_state['values']['gid'];
    imagepicker_update_user_group($record);
  }
  else {
    imagepicker_insert_user_group($record);
  }
}

/**
 * imagepicker_user_groups functions
 */
function imagepicker_insert_user_group($record) {

  if (db_insert('imagepicker_user_groups')
    ->fields(array(
      'uid' => $record['uid'],
      'group_name' => $record['group_name'],
      'group_description' => $record['group_description'],
      'public' => $record['public'],
       'avail_roles' => (isset($record['avail_roles']) ? $record['avail_roles'] : 'all'),
    ))
    ->execute()
  ) {
    drupal_set_message(t('Group was successfully inserted'));
  }
  else {
    drupal_set_message(t('Error while trying to insert your group.'));
  }
}

function imagepicker_update_user_group($record) {

  if (db_update('imagepicker_user_groups')
    ->fields(array(
      'group_name' => $record['group_name'],
      'group_description' => $record['group_description'],
      'public' => $record['public'],
       'avail_roles' => (isset($record['avail_roles']) ? $record['avail_roles'] : 'all'),
    ))
    ->condition('gid', $record['gid'])
    ->execute()
  ) {
    drupal_set_message(t('Group was successfully updated'));
  }
  else {
    drupal_set_message(t('Error while trying to update your group.'));
  }
}

function imagepicker_delete_user_group($gid) {

  if ( db_delete('imagepicker_user_groups')->condition('gid', $gid)->execute()
    && db_delete('imagepicker_group_images')->condition('gid', $gid)->execute() ) {
    drupal_set_message(t('Group was successfully deleted'));
    $pgid = imagepicker_variable_get('imagepicker_public_currentgroup', 0);
    if ($pgid == $gid) {
      imagepicker_variable_del("imagepicker_public_currentgroup");
    }
    $pgid = imagepicker_variable_get('imagepicker_currentgroup', 0);
    if ($pgid == $gid) {
      imagepicker_variable_del("imagepicker_currentgroup");
    }
  }
  else {
    drupal_set_message(t('Error while trying to delete group.'));
  }
}

function imagepicker_group_edit($gid) {

  $record = imagepicker_get_user_group($gid);
  $content = render(drupal_get_form('imagepicker_groups_form', $record));
  return $content;
}

/**
 * group delete form
 */
function imagepicker_group_delete_form($form, &$form_state, $gid) {

  if ($gid) {
    $record = imagepicker_get_user_group($gid);
    if ($record) {
      $form['groupname'] = array(
        '#value' => '<p>' . $record->group_name . '</p>',
      );
      $form['mode'] = array(
        '#type' => 'value',
        '#value' => 'reallydelete',
      );
      $form['gid'] = array(
        '#type' => 'value',
        '#value' => $gid,
      );
      $form['delete'] = array(
        '#type' => 'submit',
        '#value' => t('Really Delete record'),
      );
      return $form;
    }
  }
}

/**
 * Submit group delete form
 */
function imagepicker_group_delete_form_submit($form, &$form_state) {

  if ($form_state['values']['gid']) {
    imagepicker_delete_user_group($form_state['values']['gid']);
  }
}

function imagepicker_role_has_permission($rid) {

  $query = db_select('role_permission', 'p');
  $query->addExpression('COUNT(rid)', 'ct');
  $query->condition('rid', $rid)->condition('permission', '%use public imagepicker%', 'LIKE');
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

// build a table
function imagepicker_groups_list($src=FALSE, $account=FALSE, $label="") {

  if ($account) {
    $user = $account;
  }
  else {
    global $user;
  }

  if ($src) {
    if ($src == 'admin') {
      $editpath = IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $user->uid . '/edit/';
      $deletepath = IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $user->uid . '/delete/';
      $browsepath = IMAGEPICKER_ADMIN_PATH . '/groups/user/' . $user->uid . '/browse/';
    }
    else {
      $editpath = 'user/' . $user->uid . '/imagepicker/groups/edit/';
      $deletepath = 'user/' . $user->uid . '/imagepicker/groups/delete/';
      $browsepath = 'user/' . $user->uid . '/imagepicker/groups/browse/';
    }
  }
  else {
    $editpath = "imagepicker/groups/edit/";
    $deletepath = "imagepicker/groups/delete/";
    $browsepath = 'imagepicker/groups/';
  }

  $content = "";
  $max = imagepicker_variable_get('imagepicker_advanced_browser_page', 25);
  $use_icons = imagepicker_variable_get('imagepicker_use_icons', 1);

  if (user_access('use public imagepicker') && imagepicker_variable_get('imagepicker_public_enabled', 1)) {
    $header = array(
      array('data' => t('Group name'), 'field' => 'g.group_name'),
      array('data' => t('Description'), 'field' => 'g.group_description'),
      array('data' => t('Images'), 'field' => 'ct'),
      t('State'),
      array('data' => t('Public'), 'field' => 'g.public'),
      array('data' => t('Actions'), 'colspan' => 2),
    );
  }
  else {
    $header = array(
      array('data' => t('Group name'), 'field' => 'g.group_name'),
      array('data' => t('Description'), 'field' => 'g.group_description'),
      array('data' => t('Images'), 'field' => 'ct'),
      t('State'),
      array('data' => t('Actions'), 'colspan' => 2),
    );
  }

  $countquery = db_select('imagepicker_user_groups', 'g');
  $countquery->addExpression('COUNT(g.gid)', 'ct');
  $countquery->join('users', 'u', 'u.uid = g.uid');
  $countquery->condition('g.uid', $user->uid)
    ->condition('u.status', 1);

  $query = db_select('imagepicker_user_groups', 'g')
    ->extend('PagerDefault')->limit($max)
    ->extend('TableSort')->orderByHeader($header);
  $query->setCountQuery($countquery);
  $query->fields('g', array('gid', 'group_name', 'group_description', 'public'))
    ->addExpression('COUNT(i.img_id)', 'ct');
  $query->leftjoin('imagepicker_group_images', 'i', 'g.gid = i.gid');
  $query->leftjoin('users', 'u', 'g.uid = u.uid');
  $query->condition('g.uid', $user->uid)
    ->condition('u.status', 1)
    ->groupBy('g.gid')
    ->groupBy('g.group_name')
    ->groupBy('g.group_description')
    ->groupBy('g.public');
  $result = $query->execute();

  $rows = array();
  $enabledlist = imagepicker_get_enabled_group($account);
  foreach ($result AS $row) {
    $rowct = $row->ct;
    if (user_access('use public imagepicker') && imagepicker_variable_get('imagepicker_public_enabled', 1)) {
      $row_data = array(
        check_plain($row->group_name),
        check_plain($row->group_description),
        ($rowct && $browsepath ?  l(format_plural($rowct, '1 image', '@count images'), $browsepath . $row->gid) : t('No images')),
        (($enabledlist && in_array($row->gid, $enabledlist)) ? t('selected') : ''),
        ($row->public ? t('Yes') : t('No')),
        ($use_icons ? _imagepicker_get_icon('edit', $editpath . $row->gid, array('title' => t('Edit'))) : l(t('Edit'), $editpath . $row->gid)),
        ($use_icons ? _imagepicker_get_icon('delete', $deletepath . $row->gid, array('title' => t('Delete'))) : l(t('Delete'), $deletepath . $row->gid)),
      );
    }
    else {
      $row_data = array(
        check_plain($row->group_name),
        check_plain($row->group_description),
        ($rowct && $browsepath ?  l(format_plural($rowct, '1 image', '@count images'), $browsepath . $row->gid) : t('No images')),
        (($enabledlist && in_array($row->gid, $enabledlist)) ? t('selected') : ''),
        ($use_icons ? _imagepicker_get_icon('edit', $editpath . $row->gid, array('title' => t('Edit'))) : l(t('Edit'), $editpath . $row->gid)),
        ($use_icons ? _imagepicker_get_icon('delete', $deletepath . $row->gid, array('title' => t('Delete'))) : l(t('Delete'), $deletepath . $row->gid)),
      );
    }
    $rows[] = $row_data;
  }
  return theme('imagepicker_list', array('header' => $header, 'rows' => $rows, 'max' => $max, 'message' => t('No groups found.'), 'pref' => '<div class="imgp_groups_list">', 'suff' => '</div>', 'label' => $label));
}

function imagepicker_get_group_images_count($gid) {

  $query = db_select('imagepicker_group_images', 'g');
  $query->addExpression('COUNT(gid)', 'ct');
  $query->condition('gid', $gid);
  $row = $query->execute()->fetchObject();
  return $row->ct;
}

