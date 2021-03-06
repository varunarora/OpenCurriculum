<?php
/**
 * @file
 * field.inc
 *
 * Provides the Finder UI field related functions.
 */

finder_inc('common', 'finder_ui');

/**
 * Finder UI field page.
 *
 * @param $finder
 *  The finder object.
 * @param $feid
 *  The element machine name or 'new'.
 * @param $field_key
 *  The field key.
 * @param $js
 *  The $js param required by ctools.
 * @return
 *  The output.
 */
function finder_ui_field_page($finder, $feid, $field_key, $js) {
  ctools_include('modal');
  ctools_include('ajax');
  ctools_include('object-cache');
  finder_inc('element', 'finder_ui');
  $finder = ctools_object_cache_get('finder', $finder->name);

  $form_state = array(
    'ajax' => $js,
    'finder' => $finder,
    'feid' => $feid,
    'field_key' => $field_key,
  );

  $output = ctools_modal_form_wrapper('finder_ui_field_form', $form_state);

  if ($form_state['executed'] && $js) {
    if ($form_state['triggering_element']['#parents'][0] == 'submit') {
      $finder = ctools_object_cache_get('finder', $finder->name);

      $element = &$finder->elements[$feid];
      $field_key = $form_state['values']['field'];
      $relationship = $form_state['values']['relationship'];

      list($table, $field) = explode('.', $field_key);

      $element->settings['fields'][$field_key] = (object)array(
        'table' => $table,
        'field' => $field,
        'relationship' => $relationship,
      );

      ctools_object_cache_set('finder', $finder->name, $finder);
      $output = drupal_render(finder_ui_field_list($finder, $element->id));
      $commands[] = ajax_command_html('#finder-ui-element-' . $element->id . '-fields-list', $output);
      $commands[] = ajax_command_css('#edit-changed', array('display' => 'block'));
    }
    elseif ($form_state['triggering_element']['#parents'][0] == 'delete') {
      $finder = ctools_object_cache_get('finder', $finder->name);

      $element = &$finder->elements[$feid];
      $field_key = $form_state['values']['field'];

      unset($element->settings['fields'][$field_key]);

      ctools_object_cache_set('finder', $finder->name, $finder);
      $output = drupal_render(finder_ui_field_list($finder, $element->id));
      $commands[] = ajax_command_html('#finder-ui-element-' . $element->id . '-fields-list', $output);
      $commands[] = ajax_command_css('#edit-changed', array('display' => 'block'));
    }
    $commands[] = ctools_modal_command_dismiss();
    print ajax_render($commands);
    exit;
  }

  if ($js) {
    print ajax_render($output);
    exit;
  }

  // Otherwise, just return the output.
  return $output;
}

/**
 * Finder UI field form.
 *
 * @param $form
 *  The form array.
 * @param $form_state
 *  The form state array.
 * @return
 *  The form array.
 */
function finder_ui_field_form($form, &$form_state) {
  $feid = &$form_state['feid'];
  $finder = &$form_state['finder'];
  $field_key = &$form_state['field_key'];
  $element = &$finder->elements[$feid];
  $relationships = $finder->relationships();

  if ($field_key == 'new') {
    $fields = $finder->fields(FALSE, FALSE);

    if ($fields) {

      $field_options = array();
      foreach ($fields as $key => $value) {
        $field_options[$key] = $value['group'] . ': ' . $value['title'] . '<div class="description">' . $value['help'] . '</div>';
      }
      $form['field'] = array(
        '#type' => 'radios',
        '#title' => t('Field'),
        '#options' => $field_options,
        '#description' => t('This is the field that will be searched on (filtered).  This field will also be displayed in choices lists (if applicable), however often the raw values of this field are unacceptable for human readability, you can overcome this with PHP by using a computed field, or using the <em>rewrite choices</em> setting on the finder element.'),
        '#required' => TRUE,
      );
    }
    else {
      $form['no_fields']['#markup'] = t('There are no fields to choose from.');
    }
  }
  else {
    $form['field'] = array(
      '#type' => 'hidden',
      '#default_value' => $field_key,
    );
    $field_item = NULL;
    foreach ($finder->esetting($element, 'fields') as $field) {
      if ($field->name == $field_key) {
        $field_item = $field;
        break;
      }
    }
  }

  if (!empty($relationships)) {
    $form['relationship'] = array(
      '#type' => 'select',
      '#title' => t('Relationship'),
      '#options' => array_merge(array(' - ' . t('Select if applicable') . ' - '), $relationships),
      '#description' => t('If your views display requires a relationship to use this field, specify it here.'),
      '#default_value' => !empty($field->relationship) ? $field->relationship : NULL,
    );
  }

  if ($field_key != 'new' && empty($relationships)) {
    $form['no_config']['#markup'] = t('There is nothing to configure for this field.');
  }

  // Add buttons.
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
  );
  $form['actions']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Remove'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );
  $form['#pre_render'] = array('finder_ui_modal_form_pre_render');

  return $form;
}

/**
 * Finder UI field list.
 *
 * @param $finder
 *  The finder object.
 * @param $feid
 *  The element machine name.
 * @return
 *  The output.
 */
function finder_ui_field_list($finder, $feid) {
  $element = &$finder->elements[$feid];

  $fields = array();
  if ($finder->esetting($element, 'fields')) {
    foreach ($finder->esetting($element, 'fields') as $field) {
      $field_key = $field->table . '.' . $field->field;

      $field_name = '<em>' . t('Missing field') . '</em>';
      foreach ($finder->fields() as $group => $group_fields) {
        if (isset($group_fields[$field_key])) {
          $field_name = $group . ': ' . $group_fields[$field_key];
          break;
        }
      }

      $fields[] = finder_ui_modal_link($field_name, 'admin/structure/finder/list/' . $finder->name . '/field/' . $element->id . '/' . $field_key . '/nojs', t('Edit'), 'ctools-modal-finder-modal-style');
    }
    $output = array(
      '#theme' => 'item_list',
      '#items' => $fields,
    );
  }
  else {
    $output['no_fields'] = array(
      '#prefix' => '<div class="messages error">',
      '#markup' => t('There are no fields configured.'),
      '#suffix' => '</div>',
    );
  }

  return $output;
}