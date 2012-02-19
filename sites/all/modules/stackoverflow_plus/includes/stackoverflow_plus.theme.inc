<?php

/**
 * @file
 * Provides the theming functions for stackoverflow_plus
 */

/**
 * Show a preview of a widget using a custom CSS file.
 */
function theme_stackoverflow_plus_preview_widget($variables) {
  $path = drupal_get_path('module', 'stackoverflow_plus');
  $form = array(
    '#post' => array(),
    '#programmed' => FALSE,
    '#tree' => FALSE,
    '#parents' => array(),
    '#array_parents' => array(),
    '#required' => FALSE,
    '#attributes' => array(),
    '#title_display' => 'before',
  );
  $form_state = form_state_defaults();
  $form_state['values'] = array();
  $form_state['process_input'] = array();
  $form_state['complete form'] = array();

  $form['vote'] = array(
    '#type' => 'stackoverflow_plus',
    '#stars' => 5,
    '#auto_submit' => FALSE,
    '#allow_clear' => TRUE,
    '#widget' => array(
      'name' => isset($variables['name']) ? $variables['name'] : 'default',
      'css' => isset($variables['css']) && $variables['css'] != 'default' ? $variables['css'] : FALSE,
    ),
  );
  
  // Attach necessary JS settings
  $settings = array(
    'titleUser' => t('Your rating') .': ',
    'titleAverage' => t('Average') .': ',
    'feedbackSavingVote' => t('Saving your vote...'),
    'feedbackVoteSaved' => t('Your vote has been saved.'),
    'feedbackDeletingVote' => t('Deleting your vote...'),
    'feedbackVoteDeleted' => t('Your vote has been deleted.'),
  );

  drupal_add_js(array('stackoverflow_plus' => $settings), 'setting');

  $form = form_builder('stackoverflow_plus_preview', $form, $form_state);

  $output = '<div class="stackoverflow_plus-star-preview stackoverflow_plus-' . $form['vote']['#widget']['name'] . '">';
  $output .= drupal_render_children($form);
  $output .= '</div>';

  return $output;
}

function theme_stackoverflow_plus_preview($variables) {
  extract($variables, EXTR_SKIP);
  $values = array(
    'average' => 50,
    'user' => 80,
    'count' => 20,
  );
  $settings = array(
    'stars' => $stars,
    'allow_clear' => $unvote,
    'style' => $style,
    'text' => $text,
    'title' => $title,
    'autosubmit' => FALSE,
    'tag' => 'vote',
  );

  $form = drupal_get_form('stackoverflow_plus_custom_widget', $values, $settings);
  $form = drupal_render($form);
  // This regex is sadly necessary because having duplicate form_tokens or
  // form_id elements can cause the content type form to choke. Forms inside of
  // forms is also frowned upon, so this removes the wrapping form tag as well.
  $form = str_replace(array('<form', '</form>'), array('<div', '</div>'), $form);
  $form = preg_replace('/( method=".*?")|( action=".*?")|(<input.*?name="(form_token|form_id|destination|form_build_id)".*?\/>)/', '', $form);
  return $form;
}

function theme_stackoverflow_plus_preview_wrapper($variables) {
  return '<div class="stackoverflow-plus-preview stackoverflow-plus-preview-'. $variables['type'] .'">'. $variables['content'] .'</div>';
}

/**
 * Theme function for 'default' stackoverflow_plus field formatter.
 */
function theme_stackoverflow_plus_formatter_default($variables) {
  $element = $variables['element'];
  if (empty($element['#instance_settings']['stars'])) {
    $element['#instance_settings']['stars'] = 5;
  }

  // Add CSS and JS
  $path = drupal_get_path('module', 'stackoverflow_plus');
  drupal_add_js($path . '/js/stackoverflow_plus.js');
  drupal_add_css($path . '/css/stackoverflow_plus.css');

  $variables = array(
    'rating' => $element['#rating'],
    'stars' => $element['#instance_settings']['stars'],
    'widget' => $element['#widget'],
  );
  $star_display = theme('stackoverflow_plus_static', $variables);
  return theme('stackoverflow_plus_static_element', array('description' => $element['#description'], 'star_display' => $star_display));
}

/**
 * Theme function for 'rating' stackoverflow_plus field formatter.
 */
function theme_stackoverflow_plus_formatter_rating($variables) {
  $element = $variables['element'];

  if (empty($element['#item']['average'])) {
    $element['#item']['average'] = 0;
  }
  // Get number of stars.
  $stars = (empty($element['#instance_settings']['stars'])) ? 5 : $element['#instance_settings']['stars'];
  $average = $element['#item']['average'];
  // Rating is X out of Y stars.
  $rating = round(($average/100) * $stars, 1); 
  $output = $rating . '/' . $stars;

  return $output;
}

/**
 * Theme function for 'percentage' stackoverflow_plus field formatter.
 */
function theme_stackoverflow_plus_formatter_percentage($variables) {
  $element = $variables['element'];

  if (empty($element['#item']['average'])) {
    $element['#item']['average'] = 0;
  }

  return round($element['#item']['average'], 1) . '%';
}

/**
 * Theme the stackoverflow_plus form element by adding necessary css and javascript.
 */
function theme_stackoverflow_plus($variables) {
  $element = $variables['element'];

  return theme('form_element', array('element' => $element));
}

/**
 * Theme the straight HTML version of the stackoverflow_plus select list. This is used
 * to remove the wrapping 'form-item' div from the select list.
 */
function theme_stackoverflow_plus_select($variables) {
  $element = $variables['element'];
  element_set_attributes($element, array('id', 'name', 'size'));
  _form_set_class($element, array('form-select'));
  return '<select' . drupal_attributes($element['#attributes']) . '>' . form_select_options($element) . '</select><h1 class="currentCount">' . $element['#default_value']. '</h1>';
}
/**
 * Display a plain HTML view-only version of the widget with a specified rating.
 *
 * @param $rating
 *   The desired rating to display out of 100 (i.e. 80 is 4 out of 5 stars).
 * @param $stars
 *   The total number of stars this rating is out of.
 * @param $tag
 *   Allows multiple ratings per node.
 * @return
 *   A themed HTML string representing the star widget.
 */ 
function theme_stackoverflow_plus_static($variables) {
  $rating  = $variables['rating'];
  $stars = $variables['stars'];
  $tag = $variables['tag'];
  $widget = $variables['widget'];
  if($widget['name'] != 'default') {
    drupal_add_css($widget['css']);
  }
  
  $output = '<div class="stackoverflow_plus-' . $widget['name'] . '">';
  $output .= '<div class="stackoverflow_plus-widget-static stackoverflow_plus-widget-static-'. $tag .' stackoverflow_plus-widget-static-'. $stars . ' clearfix">';
  if (empty($stars)) {
    $stars = 5;
  }
  $numeric_rating = $rating/(100/$stars);
  for ($n=1; $n <= $stars; $n++) {
    $star_value = ceil((100/$stars) * $n);
    $prev_star_value = ceil((100/$stars) * ($n-1));
    $zebra = ($n % 2 == 0) ? 'even' : 'odd';
    $first = $n == 1 ? ' star-first' : '';
    $last = $n == $stars ? ' star-blast' : '';
    $output .= '<div class="star star-'. $n .' star-'. $zebra . $first . $last .'">';
    if ($rating < $star_value && $rating > $prev_star_value) {
      $percent = (($rating - $prev_star_value) / ($star_value - $prev_star_value)) * 100;
      $output .= '<span class="on" style="width: '. $percent .'%">';
    }
    elseif ($rating >= $star_value) {
      $output .= '<span class="on">';
    }
    else {
      $output .= '<span class="off">';
    }
    if ($n == 1)$output .= $numeric_rating;
    $output .= '</span></div>';
  }
  
  $output .= '<h1>0</h1></div></div>';
  return $output;
}

/**
 * Display the text associated with a static star display.
 *
 * Note that passing in explicit data types is extremely important when using
 * this function. A NULL value will exclude the value entirely from display,
 * while a 0 value indicates that the text should be shown but it has no value
 * yet.
 *
 * All ratings are from 0 to 100.
 *
 * @param $user_rating
 *   The current user's rating.
 * @param $average
 *   The average rating.
 * @param $votes
 *   The total number of votes.
 * @param $stars
 *   The number of stars being displayed.
 * @return
 *   A themed HTML string representing the star widget.
 */
function theme_stackoverflow_plus_summary($variables) {
  extract($variables, EXTR_SKIP);
  $output = '';
  $div_class = '';
  if (isset($user_rating)) {
    $div_class = isset($votes) ? 'user-count' : 'user';
    $user_stars = round(($user_rating * $stars) / 100, 1);
    $output .= '<span class="user-rating">'. t('Your rating: <span>!stars</span>', array('!stars' => $user_rating ? $user_stars : t('None'))) .'</span>';
  }
  if (isset($user_rating) && isset($average_rating)) {
    $output .= ' ';
  }
  if (isset($average_rating)) {
    $div_class = isset($votes) ? 'average-count' : 'average';
    $average_stars = round(($average_rating * $stars) / 100, 1);
    $output .= '<span class="average-rating">'. t('Average: <span>!stars</span>', array('!stars' => $average_stars)) .'</span>';
  }
  if (isset($user_rating) && isset($average_rating)) {
    $div_class = 'combo';
  }

  if (isset($votes) && !(isset($user_rating) || isset($average_rating))) {
    $output .= ' <span class="total-votes">'. format_plural($votes, '<span>@count</span> vote', '<span>@count</span> votes') .'</span>';
    $div_class = 'count';
  }
  elseif (isset($votes)) {
    $output .= ' <span class="total-votes">('. format_plural($votes, '<span>@count</span> vote', '<span>@count</span> votes') .')</span>';
  }

  if ($votes === 0) {
    $output = '<span class="empty">'. t('No votes yet') .'</span>';
  }

  $output = '<div class="stackoverflow_plus-summary stackoverflow_plus-summary-'. $div_class . '">'. $output .'</div>';
  return $output;
}

/**
 * Display a static stackoverflow_plus value as stars with a title and description.
 */
function theme_stackoverflow_plus_static_element($variables) {
  $output = '';
  $output .= '<div class="stackoverflow_plus-static-form-item">';
  $element = array(
    '#type' => 'item',
    '#title' => $variables['title'],
    '#description' => $variables['description'],
    '#children' => $variables['star_display'],
  );

  $output .= theme('form_element', array('element' => $element));
  $output .= '</div>';
  return $output;
}
