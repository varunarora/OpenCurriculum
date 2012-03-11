<?php
// $Id: template.php,v 1.12 2010/12/06 19:34:33 webchick Exp $

/**
 * Add body classes if certain regions have content.
 */
function opencurriculum_preprocess_html(&$variables) {

  // Add conditional stylesheets for IE
  drupal_add_css(path_to_theme() . '/css/ie.css', array('group' => CSS_THEME, 'browsers' => array('IE' => 'IE', '!IE' => FALSE), 'preprocess' => FALSE));
  drupal_add_css(path_to_theme() . '/css/ie6.css', array('group' => CSS_THEME, 'browsers' => array('IE' => 'IE 6', '!IE' => FALSE), 'preprocess' => FALSE));

  drupal_add_css(path_to_theme() . '/css/jquery-ui-1.8.6.custom.css');
  drupal_add_css('http://fonts.googleapis.com/css?family=Asap:700'); 
  drupal_add_css(path_to_theme() . '/css/shadowbox.css');
/*  drupal_add_css(path_to_theme() . '/css/mscarousel.css');
*/
  drupal_add_js(path_to_theme() .'/js/shadowbox.js'); 
  drupal_add_js(path_to_theme() .'/js/jquery.tweet.js');
  drupal_add_js(path_to_theme() .'/js/easySlider1.7.js');
  drupal_add_js(path_to_theme() .'/js/init.js');
  drupal_add_js(path_to_theme() .'/js/jquery-ui.min.js');
  drupal_add_js(path_to_theme() .'/js/jquery.tools.min.js');

}

/**
 * Override or insert variables into the page template for HTML output.
 */
function opencurriculum_process_html(&$variables) {
  // Hook into color.module.
  if (module_exists('color')) {
    _color_html_alter($variables);
  }
}

/**
 * Override or insert variables into the page template.
 */
function opencurriculum_process_page(&$variables) {

// Hook into color.module.
  if (module_exists('color')) {
    _color_page_alter($variables);
  }
  // Always print the site name and slogan, but if they are toggled off, we'll
  // just hide them visually.
  $variables['hide_site_name']   = theme_get_setting('toggle_name') ? FALSE : TRUE;
  $variables['hide_site_slogan'] = theme_get_setting('toggle_slogan') ? FALSE : TRUE;
  if ($variables['hide_site_name']) {
    // If toggle_name is FALSE, the site_name will be empty, so we rebuild it.
    $variables['site_name'] = filter_xss_admin(variable_get('site_name', 'Drupal'));
  }
  if ($variables['hide_site_slogan']) {
    // If toggle_site_slogan is FALSE, the site_slogan will be empty, so we rebuild it.
    $variables['site_slogan'] = filter_xss_admin(variable_get('site_slogan', ''));
  }
  // Since the title and the shortcut link are both block level elements,
  // positioning them next to each other is much simpler with a wrapper div.
  if (!empty($variables['title_suffix']['add_or_remove_shortcut']) && $variables['title']) {
    // Add a wrapper div using the title_prefix and title_suffix render elements.
    $variables['title_prefix']['shortcut_wrapper'] = array(
      '#markup' => '<div class="shortcut-wrapper clearfix">',
      '#weight' => 100,
    );
    $variables['title_suffix']['shortcut_wrapper'] = array(
      '#markup' => '</div>',
      '#weight' => -99,
    );
    // Make sure the shortcut link is the first item in title_suffix.
    $variables['title_suffix']['add_or_remove_shortcut']['#weight'] = -100;
  }

}

/**
 * Implements hook_preprocess_maintenance_page().
 */
function opencurriculum_preprocess_maintenance_page(&$variables) {
  if (!$variables['db_is_active']) {
    unset($variables['site_name']);
  }
  drupal_add_css(drupal_get_path('theme', 'bartik') . '/css/maintenance-page.css');
}

/**
 * Override or insert variables into the maintenance page template.
 */
function opencurriculum_process_maintenance_page(&$variables) {
  // Always print the site name and slogan, but if they are toggled off, we'll
  // just hide them visually.
  $variables['hide_site_name']   = theme_get_setting('toggle_name') ? FALSE : TRUE;
  $variables['hide_site_slogan'] = theme_get_setting('toggle_slogan') ? FALSE : TRUE;
  if ($variables['hide_site_name']) {
    // If toggle_name is FALSE, the site_name will be empty, so we rebuild it.
    $variables['site_name'] = filter_xss_admin(variable_get('site_name', 'Drupal'));
  }
  if ($variables['hide_site_slogan']) {
    // If toggle_site_slogan is FALSE, the site_slogan will be empty, so we rebuild it.
    $variables['site_slogan'] = filter_xss_admin(variable_get('site_slogan', ''));
  }
}

/**
 * Override or insert variables into the node template.
 */
function opencurriculum_preprocess_node(&$variables) {
  $variables['submitted'] = t('published by !username on !datetime', array('!username' => $variables['name'], '!datetime' => $variables['date']));
  if ($variables['view_mode'] == 'full' && node_is_page($variables['node'])) {
    $variables['classes_array'][] = 'node-full';
  }
}

function opencurriculum_preprocess_page(&$variables){
  
  if (isset($variables['page']['content']['system_main']['#entity_type'])){
  	// Make the title of the page the name of the user, if we are displaying a user page
  	if ($variables['page']['content']['system_main']['#entity_type'] == 'user'){
  		if (!arg(2) && arg(1) != "register" && isset($variables['page']['content']['system_main']['field_name'])){
  		$variables['title'] = $variables['page']['content']['system_main']['field_name']['#items'][0]['value'];
		}
  	}
  }
  
}

/**
 * Override or insert variables into the block template.
 */
function opencurriculum_preprocess_block(&$variables) {
  // In the header region visually hide block titles.
  if ($variables['block']->region == 'header') {
    $variables['title_attributes_array']['class'][] = 'element-invisible';
  }
}

/**
 * Implements theme_menu_tree().
 */
function opencurriculum_menu_tree($variables) {
  return '<ul class="menu clearfix">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_field__field_type().
 */
function opencurriculum_field__taxonomy_term_reference($variables) {
  $output = '';

  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<h3 class="field-label">' . $variables['label'] . ': </h3>';
  }

  // Render the items.
  $output .= ($variables['element']['#label_display'] == 'inline') ? '<ul class="links inline">' : '<ul class="links">';
  foreach ($variables['items'] as $delta => $item) {
    $output .= '<li class="taxonomy-term-reference-' . $delta . '">' . drupal_render($item) . '</li>';
  }
  $output .= '</ul>';

  // Render the top-level DIV.
  $output = '<div class="' . $variables['classes'] . (!in_array('clearfix', $variables['classes_array']) ? ' clearfix' : '') . '">' . $output . '</div>';

  return $output;
}

function opencurriculum_theme($existing, $type, $theme, $path){

  return array(
   'user_login_blocek' => array(
     'render element' => 'form',
     'template' => 'templates/user-login-block',
),
);

}


function opencurriculum_form_book_node_form_alter(&$form, &$form_state, $form_id){
	$form['actions']['submit']['#value'] = 'Submit';
}

/*
function opencurriculum_form_user_login_block_alter(&$form, &$form_state) {
$form['actions']['submit']['#value'] = 'Sign in';
$form['name']['#title_display'] = 'invisible';
$form['pass']['#title_display'] = 'invisible';
$form['openid_links']['#type'] = 'hidden';
$form['links']['#type'] = 'hidden';
}*/

function opencurriculum_menu_local_task($variables){
  $link = $variables['element']['#link'];
  $link_text = $link['title'];

  
  $path = $variables['element']['#link']['path'];
  
  $id = '';
  
  switch ($path) {
      case 'node/%/edit':
          $id = 'tab-edit';
          break;
      case 'node/%/discussion':
          $id = 'tab-discussion';
          break;		  
      case 'node/%/revisions':
          $id = 'tab-revisions';
          break;
	  case 'node/%/resources':
          $id = 'tab-resources';
          break;
	  case 'node/%/view':
          $id = 'tab-current';
          break;			        
      default:
          
          break;
  }
  
  if (!empty($variables['element']['#active'])) {
    // Add text to indicate active tab for non-visual users.
    $active = '<span class="element-invisible">' . t('(active tab)') . '</span>';

    // If the link does not contain HTML already, check_plain() it now.
    // After we set 'html'=TRUE the link will not be sanitized by l().
    if (empty($link['localized_options']['html'])) {
      $link['title'] = check_plain($link['title']);
    }
    $link['localized_options']['html'] = TRUE;
    $link_text = t('!local-task-title!active', array('!local-task-title' => $link['title'], '!active' => $active));
	
  }

  $link['localized_options']['attributes'] = array('id' => $id);
  
  $classes = array();
  $classes[] = !empty($variables['element']['#active']) ? 'active' : '';
  $classes[] = ($path == 'node/%/revisions' || $path == 'node/%/edit') ? 'tab-right': '';
  

  
  return '<li class="' . implode(' ', $classes) . '" >' . l($link_text, $link['href'], $link['localized_options']) . "</li>\n";
}