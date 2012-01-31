<?php

function oers_views_data(){

  $data['menu_links']['table']['group'] = t('Menu link');

  $data['menu_links']['table']['join']['node'] = array(
    'left_table' => 'book',
    'left_field' => 'mlid',
    'field' => 'mlid',
  );

  $data['menu_links']['plid'] = array(
  'title' => t('Parent'),
  'help' => t('The parent node of the book'),
  'filter' => array(
    'handler' => 'oers_handler_filter_menu_links_plid',
  ),

  );

  return $data;

}
?>
