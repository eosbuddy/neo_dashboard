<?php
// $Id: neo_dashboard.defaults.inc,v 1.9.4.1 2010/07/28 16:27:36 beretta627 Exp $

/**
 * @file
 * Dashboard module defaults file.
 * Stores default content options for neo dashboards.
 */

/** 
 * Create a default neo dashboard for a new user
 */
function neo_dashboard_defaults($uid) {

  $neo_dashboard = variable_get('neo_dashboard_default_layout', '');

  if(is_array($neo_dashboard)) {
    foreach($neo_dashboard as $page) {
      $page_params = array(
        'uid' => $uid,
        'path' => $page['path'],
        'weight' => $page['weight'],
        'title' => $page['title'],
      );
      drupal_write_record('neo_dashboard_page', $page_params);

      $page_id = $page_params['page_id'];

      if(is_array($page['widgets'])) {
        foreach($page['widgets'] as $widget) {
          $widget_params = array(
            'widget_id' => $widget['widget_id'],
            'page_id' => $page_id,
            'type' => $widget['type'],
            'subtype' => $widget['subtype'],
            'conf' => $widget['conf'],
            'col' => $widget['col'],
            'weight' => $widget['weight'],
            'visibility' => $widget['visibility'],
          );
          drupal_write_record('neo_dashboard_widget', $widget_params);
        }
      }
    }
  } else {
    $page = array(
      'uid' => $uid,
      'path' => 'neodash',
      'weight' => 0,
      'title' => 'NeoDash',
    );
    drupal_write_record('neo_dashboard_page', $page);
  }

  //drupal_set_message('A default neo dashboard has been generated.');

}

/** 
 * Set the default NeoDash layout variable 
 */
function neo_dashboard_set_default_layout($uid) {

  $neo_dashboard = array();

  $pages = db_query("SELECT * FROM {neo_dashboard_page} WHERE uid = '%d'", $uid);

  while($page = db_fetch_array($pages)) {
    $page_id = $page['page_id'];

    unset($page['page_id']);
    unset($page['uid']);

    $neo_dashboard[$page_id] = $page;

  }

  if(count($neo_dashboard) > 0) {
    $page_ids = implode(array_keys($neo_dashboard), ',');
    $widgets = db_query("SELECT * FROM {neo_dashboard_widget} WHERE page_id IN (%s)", $page_ids);

    while($widget = db_fetch_array($widgets)) {
      $page_id = $widget['page_id'];

      unset($widget['page_id']);

      $neo_dashboard[$page_id]['widgets'][$widget['widget_id']] = $widget;
    }

    variable_set('neo_dashboard_default_layout', $neo_dashboard);

    drupal_set_message('You have set the default NeoDash layout.');
  } else {
    drupal_set_message('Please setup a NeoDash before you set the default.', 'error');
  }

}
