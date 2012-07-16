<?php
// $Id: neo_dashboard.page.inc,v 1.12.4.2 2010/07/28 16:27:36 beretta627 Exp $

/**
 * @file
 * NeoDash module page file.
 * Builds and renders NeoDash pages.
 */

/**
 * Add the CSS and JS requirements for the page.
 */
function neo_dashboard_add_ui($page = NULL) {
  neo_dashboard_add_tools();
  drupal_add_css(drupal_get_path('module', 'neo_dashboard') . '/neo_dashboard.css');
  drupal_add_js(drupal_get_path('module', 'neo_dashboard') . '/neo_dashboard.js');
  variable_set('jquery_update_compression_type', 'none');
  jquery_ui_add(array('ui.sortable'));
  drupal_add_js(array(
    'dashboardPageAddForm' => drupal_get_form('neo_dashboard_page_add_form'),
  ),'setting');
  drupal_add_css(drupal_get_path('module', 'neo_dashboard') . '/css/smoothness/jquery-ui-1.8.21.custom.css');
  drupal_add_js(drupal_get_path('module', 'neo_dashboard') . '/js/jquery-1.7.2.min.js');
  drupal_add_js(drupal_get_path('module', 'neo_dashboard') . '/js/jquery-ui-1.8.21.custom.min.js');
  global $user;
  $user2 = user_load(arg(1));
  if (!is_null($page)) {
    drupal_add_js(array(
      'currentUser' => $user2->uid,
      'owner' => $user->uid,
      'dashboardPage' => $page->path,
      'dashboardToken' => drupal_get_token('neo_dashboard ' . $page->page_id),
      'dashboardPageEditForm' => drupal_get_form('neo_dashboard_page_edit_form', $page),
    ),'setting');
  }
}

/**
 * Generate a NeoDash page.
 *
 * @param $page
 *   Meta-data about the page to be rendered.
 * @return
 *   A fully-themed HTML page.
 */
function neo_dashboard_page($page = NULL) {
  if (is_null($page)) {
    $page = neo_dashboard_user_page_load('');
  }
  cache_clear_all('neo_dashboard', 'cache', TRUE);
  neo_dashboard_add_ui($page);
  $pages = neo_dashboard_user_page_load();

  $tabelements = array();
  foreach($pages as $key => $item) {
    if(($key != '') && ($key != $page->path)) {
      $widget_page_options .= '<li><a class="neo-dashboard-page-' . $item->page_id . '" href="#">' . check_plain($item->title) . '</a></li>' . "\n";

    }
  }

  array_push($tabelements,$item->page_id);
  $widgets = array_fill(0, 1, '');
  $result = db_query('SELECT widget_id, type, subtype, conf, col, visibility FROM {neo_dashboard_widget} WHERE page_id = %d OR system = 1 ORDER BY weight', $page->page_id);
  while ($widget = db_fetch_object($result)) {
    $content = ctools_content_render($widget->type, $widget->subtype, unserialize($widget->conf));
    $content->widget_id = $widget->widget_id;

    if(!$widget->visibility) {
      $content->visibility = ' style="display: none;"';
    }

    $content->pages = $widget_page_options;

    $widgets[$widget->col] .= theme('neo_dashboard_widget', $content);
  }

  return theme('neo_dashboard_page', neo_dashboard_user_tabs(), $widgets);
}

/**
 * FormsAPI for adding a new page to a NeoDash.
 */
function neo_dashboard_page_add_form($form_state) {
  $form = array(
    '#attributes' => array('class' => 'nav-tab'),
  );

  $form['title'] = array(
    '#prefix' => '<div class="container-inline">',
    '#type' => 'textfield',
    '#size' => 15,
    '#maxlength' => 20,
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#attributes' => array('disabled' => 'disabled'),
    '#suffix' => '</div>',
  );

  return $form;
}

/**
 * FormsAPI submit handler for adding a NeoDash page.
 */
function neo_dashboard_page_add_form_submit($form, &$form_state) {
  global $user;

  $path = _neo_dashboard_create_path($form_state['values']['title']);

  $page = array(
    'uid' => $user->uid,
    'path' => $path,
    'weight' => db_result(db_query("SELECT max(weight) FROM {neo_dashboard_page} WHERE uid = %d", $user->uid)) + 1,
    'title' => $form_state['values']['title'],
  );
  drupal_write_record('neo_dashboard_page', $page);

  $form_state['redirect'] = 'nd/'. strval($user->uid) . '/'. $path;
}

/*
 * Make a sanitized & unique path.
 */
function _neo_dashboard_create_path($title) {

  $pages = neo_dashboard_user_page_load();
  $used_paths = array_keys($pages);
  $used_paths[] = $pages['']->path;
  $used_paths[] = 'widgets';

  $path = $base_path = preg_replace('/[^a-z0-9-]/', '', drupal_strtolower(str_replace(' ', '-', $title)));
  $n = 0;
  while (in_array($path, $used_paths)) {
    $n += 1;
    $path = $base_path . '-' . $n;
  }

  return $path;

}

/**
 * FormsAPI for editing a NeoDash page.
 */
function neo_dashboard_page_edit_form($form_state, $page) {
  global $user;
  $form = array();

  $form['edit_title'] = array(
    '#prefix' => '<div class="edit container-inline">',
    '#type' => 'textfield',
    '#default_value' => $page->title,
    '#size' => 15,
    '#required' => TRUE,
  );
  $form['page_id'] = array(
    '#type' => 'value',
    '#value' => $page->page_id,
  );
  $form['edit_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#ahah' => array(
      'event' => 'click',
      'path' => 'nd/'. $user->uid . '/' . $page->path .'/rename',
    ),
    '#suffix' => '</div>',
  );
  $form['delete'] = array(
    '#prefix' => '<div class="delete container-inline">',
    '#type' => 'submit',
    '#value' => t('Deleting…'),
    '#suffix' => '<a href="#" class="cancel">' . t('Cancel') . '</a></div>',
  );
  $form['private'] = array(
    '#prefix' => '<div class="private container-inline">',
    '#type' => 'button',
    '#value' => t('Making Private…'),
    '#suffix' => '<a href="#" class="cancel">' . t('Cancel') . '</a></div>',
  );

  return $form;
}

/**
 * Page callback for changing the name of a NeoDash page.
 *
 * @param $page
 *   The page beign edited.
 * @return
 *   A JSON-enabled form.
 */
function neo_dashboard_page_rename($page) {
  drupal_get_form('neo_dashboard_page_edit_form', $page);
  drupal_json(array('status' => TRUE, 'data' => ''));
}

/**
 * FormsAPI submit handler for changing a NeoDash page.
 */
function neo_dashboard_page_edit_form_submit($form, &$form_state) {
  global $user;

  switch ($form_state['clicked_button']['#id']) {
    case 'edit-edit-submit':
      $path = _neo_dashboard_create_path($form_state['values']['edit_title']);

      $page = array(
        'page_id' => $form_state['values']['page_id'],
        'path' => $path,
        'title' => $form_state['values']['edit_title'],
      );
      drupal_write_record('neo_dashboard_page', $page, array('page_id'));
      break;

    case 'edit-delete':
      $weight = 0;

      db_query("DELETE FROM {neo_dashboard_page} WHERE page_id = %d", $form_state['values']['page_id']);
      db_query("DELETE FROM {neo_dashboard_widget} WHERE page_id = %d", $form_state['values']['page_id']);


      //reweight remaining pages
      $user_pages = db_query("SELECT page_id FROM {neo_dashboard_page} WHERE uid = '%d' ORDER BY weight ASC", $user->uid);
      while($user_page = db_fetch_object($user_pages)) {
        db_query("UPDATE {neo_dashboard_page} SET weight = '%d' WHERE page_id = '%d' AND uid = '%d'", $weight, $user_page->page_id, $user->uid);
        $weight++;
      }

      
      $form_state['redirect'] = 'nd/'.$user->uid.'/neodash';
      break;

    case 'edit-private':
      $privacy = db_result(db_query("SELECT privfl FROM {neo_dashboard_page} WHERE uid = '%d' AND page_id = '%d'", $user->uid, $user_page->page_id));
      $privfl  = intval(!$privacy);
      db_query("UPDATE {neo_dashboard_page} SET privfl = '%d' WHERE page_id = '%d' AND uid = '%d'", $privfl, $user_page->page_id, $user->uid);

      $form_state['redirect'] = 'nd/'.$user->uid.'/neodash';
      break;
  }
}

/**
 * Menu callback for changing the order of a NeoDash page.
 */
function neo_dashboard_page_reorder() {
  foreach (explode(',', $_POST['pages']) as $weight => $page_id) {
    $page = array(
      'page_id' => $page_id,
      'weight' => $weight,
    );
    drupal_write_record('neo_dashboard_page', $page, array('page_id'));
  }
}

/**
 * Menu callback for changing the position of widgets on a page.
 *
 * @param $page
 *   The page being edited.
 */
function neo_dashboard_widget_position() {
  global $user;
  $page_id = db_result(db_query("SELECT page_id FROM neo_dashboard_page WHERE uid = '%d' AND path = '%s'", $user->uid, $_POST['page']));
  foreach ($_POST['widtop'] as $widgetid => $wtops) {
      $wtop = array(
        'top' => intval($wtops),
        'widgetid' => $widgetid+1,
      );
      db_query("UPDATE {neo_dashboard_widget} SET wtop = %d WHERE widget_id = %d AND page_id = %d", intval($wtops), $widgetid+1, $page_id);
  }
  foreach ($_POST['widlef'] as $widgetid => $left) {
      $wleft = array(
        'left' => intval($left),
        'widgetid' => $widgetid+1,
      );
      db_query("UPDATE {neo_dashboard_widget} SET wleft = %d WHERE widget_id = %d AND page_id = %d", intval($left), $widgetid+1, $page_id);
  }
  foreach ($_POST['widpof'] as $widgetid => $posflag) {
      $widpof = array(
        'posflag' => intval($posflag),
        'widgetid' => $widgetid+1,
      );
      db_query("UPDATE {neo_dashboard_widget} SET wpofl = %d WHERE widget_id = %d AND page_id = %d", intval($posflag), $widgetid+1, $page_id);
  }
}
/**
 * Menu callback for getting widget position.
 */
function neo_dashboard_position_widget() {
  global $user;
  $page_id = db_result(db_query("SELECT page_id FROM {neo_dashboard_page} WHERE uid = '%d' AND path = '%s'", $_POST['user'], $_POST['page']));
  $wpos = array();
  $result = db_query("SELECT wtop, wleft, wpofl FROM {neo_dashboard_widget} WHERE page_id = %d OR system = '1'", $page_id);
  while ($data = db_fetch_object($result)) {
      array_push($wpos,$data);
  }
  return drupal_json($wpos);
}

/**
 * Menu callback for changing the size of widgets on a page.
 *
 * @param $page
 *   The page being edited.
 */
function neo_dashboard_widget_resize() {
  global $user;

  $page_id = db_result(db_query("SELECT page_id FROM neo_dashboard_page WHERE uid = '%d' AND path = '%s'", $user->uid, $_POST['page']));
  foreach ($_POST['widwid'] as $widgetid => $width) {
      $wwidth = array(
        'width' => $width,
        'widgetid' => $widgetid+1,
      );
      db_query("UPDATE {neo_dashboard_widget} SET width = %d WHERE widget_id = %d AND page_id = %d", $width, $widgetid+1, $page_id);
  }
  foreach ($_POST['widhei'] as $widgetid => $height) {
      $wheight = array(
        'height' => $height,
        'widgetid' => $widgetid+1,
      );
      db_query("UPDATE {neo_dashboard_widget} SET height = %d WHERE widget_id = %d AND page_id = %d", $height, $widgetid+1, $page_id);
  }
  foreach ($_POST['widref'] as $widgetid => $resflag) {
      $widref = array(
        'resflag' => intval($resflag),
        'widgetid' => $widgetid+1,
      );
      db_query("UPDATE {neo_dashboard_widget} SET wrefl = %d WHERE widget_id = %d AND page_id = %d", intval($resflag), $widgetid+1, $page_id);
  }
}


/**
 * Menu callback for getting NeoDash privacy.
 */
function neo_dashboard_privacy() { 
    global $user; 
    $privfl = array(); 
    $page_id = db_result(db_query("SELECT page_id FROM {neo_dashboard_page} WHERE uid = '%d' AND path = '%s'", $user->uid, $_POST['page'])); 
    $result = db_result(db_query("SELECT privfl FROM {neo_dashboard_page} WHERE uid = '%d' AND page_id = '%d'", $user->uid, $page_id)); 
    array_push($privfl,$result); 
    return drupal_json($privfl);
}
/**
 * Menu callback for changing NeoDash privacy.
 */
function neo_dashboard_change_privacy() { 
    global $user; 
    if ($user->uid == arg(1)) {
        $page_id = db_result(db_query("SELECT page_id FROM {neo_dashboard_page} WHERE uid = '%d' AND path = '%s'", $user->uid, $_POST['page'])); 
        $privacy = db_result(db_query("SELECT privfl FROM {neo_dashboard_page} WHERE uid = '%d' AND page_id = '%d'", $user->uid, $page_id)); 
        db_query("UPDATE {neo_dashboard_page} SET privfl = %d WHERE uid = %d AND page_id = %d", intval(!$privacy), $user->uid, $page_id);
        $result = 'TRUE';
    } else {
        $result = 'FALSE';
    }
    return drupal_json($result);
}
/**
 * Menu callback for getting widget width.
 */
function neo_dashboard_widget_width() {
  global $user;
  $page_id = db_result(db_query("SELECT page_id FROM {neo_dashboard_page} WHERE uid = '%d' AND path = '%s'", $_POST['user'], $_POST['page']));
  $size = array();
  $result = db_query("SELECT height, width, wrefl FROM {neo_dashboard_widget} WHERE page_id = %d OR system = 1", $page_id);
  while ($data = db_fetch_object($result)) {
      array_push($size,$data);
  }
  return drupal_json($size);
}

/**
 * Menu callback for getting widget height.
 */
function neo_dashboard_widget_height() {
  global $user;
  $widgetid = $_POST['widget_id'];
  $page_id = db_result(db_query("SELECT page_id FROM {neo_dashboard_page} WHERE uid = '%d' AND path = '%s'", $_POST['user'], $_POST['page']));
  $height = array();
  $result = db_result(db_query("SELECT height FROM {neo_dashboard_widget} WHERE page_id = %d AND widget_id = %d", $page_id, $widgetid));
  array_push($height,$result);
  return drupal_json($height);
}

/**
 * Menu callback for adding a widget to page.
 */
function neo_dashboard_widget_add() {

  // Get the widget_id sent by ajax post request
  $widget_id = $_POST['widget_id'];
  $result = array();
  if ($widget_id) {

    // Get default NeoDash page id for logged user
    $page_id = neo_dashboard_get_default_page_id();
    $page_name = check_plain(db_result(db_query("SELECT title FROM {neo_dashboard_page} WHERE page_id = '%d'", $page_id)));

    // Get default widget data
    $res = db_query('SELECT * FROM {neo_dashboard_default} WHERE id = %d', $widget_id);
    while ($default_widget = db_fetch_object($res)) {
      $result['widget_type'] = $default_widget->widget_type;
      $result['subtype'] = $default_widget->subtype;
      $result['conf'] = $default_widget->conf;
      $result['system'] = $default_widget->default_permanent;
    }

    // Get remaining data for insert
    $type = $result['widget_type'];
    $subtype = $result['subtype'];
    $conf = $result['conf'];
    $col = 0;
    $weight = 0;
    $system=$result['system'];

    // Replace existing row with the new one
    db_query("DELETE FROM {neo_dashboard_widget} WHERE widget_id = %d AND page_id = %d", $widget_id, $page_id);
    db_query("INSERT INTO {neo_dashboard_widget} (widget_id, page_id, system, type, subtype, conf, col, weight)
    VALUES (%d, %d, %d, '%s', '%s', '%s', %d, %d)", $widget_id, $page_id, $system, $type, $subtype, $conf, $col, $weight);

    $result['success'] = TRUE;
    $result['label'] = t('Added to ' . $page_name);
    cache_clear_all('neo_dashboard', 'cache', TRUE);
  }
  else {
    $result['success'] = FALSE;
  }
  return drupal_json($result);
}

/**
 * Menu callback for adding a system widget to all pages.
 */
function neo_dashboard_system_widget_add() {

  // Get the widget_id sent by ajax post request
  $widget_id = $_POST['widget_id'];
  $result = array();
  if ($widget_id) {

    // Get default NeoDash page id for logged user
    //$page_id = neo_dashboard_get_default_page_id();
    $page_id = 0;
    //$page_name = check_plain(db_result(db_query("SELECT title FROM {neo_dashboard_page} WHERE page_id = '%d'", $page_id)));
    $page_name = 'system';

    // Get default widget data
    $res = db_query('SELECT * FROM {neo_dashboard_default} WHERE id = %d AND default_permanent = "1"', $widget_id);
    while ($default_widget = db_fetch_object($res)) {
      $result['widget_type'] = $default_widget->widget_type;
      $result['subtype'] = $default_widget->subtype;
      $result['conf'] = $default_widget->conf;
    }

    // Get remaining data for insert
    $type = $result['widget_type'];
    $subtype = $result['subtype'];
    $conf = $result['conf'];
    $col = 0;
    $weight = 0;

    // Replace existing row with the new one
    db_query("DELETE FROM {neo_dashboard_widget} WHERE widget_id = %d AND page_id = %d", $widget_id, $page_id);
    db_query("INSERT INTO {neo_dashboard_widget} (widget_id, page_id, type, subtype, conf, col, weight)
    VALUES (%d, %d, '%s', '%s', '%s', %d, %d)", $widget_id, $page_id, $type, $subtype, $conf, $col, $weight);

    $result['success'] = TRUE;
    $result['label'] = t('Added to ' . $page_name);
    cache_clear_all('neo_dashboard', 'cache', TRUE);
  }
  else {
    $result['success'] = FALSE;
  }
  return drupal_json($result);
}

/**
 * Menu callback for minimizing a widget.
 */
function neo_dashboard_widget_minimize() {
  global $user;

  $widget = db_fetch_object(db_query("SELECT {neo_dashboard_page}.page_id, {neo_dashboard_widget}.visibility FROM {neo_dashboard_page} LEFT JOIN {neo_dashboard_widget} ON {neo_dashboard_page}.page_id = {neo_dashboard_widget}.page_id WHERE uid = %d AND widget_id = %d", $user->uid, $_POST['widget_id']));
  if ($widget->visibility == 1) {
    db_query("UPDATE {neo_dashboard_widget} SET visibility = 0 WHERE widget_id = %d AND page_id = %d", $_POST['widget_id'], $widget->page_id);
  } else {
    db_query("UPDATE {neo_dashboard_widget} SET visibility = 1 WHERE widget_id = %d AND page_id = %d", $_POST['widget_id'], $widget->page_id);
  }
}

/**
 * Menu callback for changing a user's widget page.
 */
function neo_dashboard_widget_change_widget_page() {
  global $user;
  $old_id = db_result(db_query("SELECT dw.page_id FROM {neo_dashboard_widget} dw LEFT JOIN {neo_dashboard_page} dp ON dp.page_id = dw.page_id WHERE dw.widget_id = '%d' AND dp.uid = '%d'", $_POST['widget_id'], $user->uid)); 

  //update the weights of currently added widgets
  $current_widgets_query = db_query('SELECT widget_id, weight FROM {neo_dashboard_widget} WHERE page_id = %d AND col = 0', $_POST['page_id']);
  while($widget = db_fetch_object($current_widgets_query)) {
    $weight = $widget->weight + 1;
    db_query("UPDATE {neo_dashboard_widget} SET weight = %d WHERE widget_id = %d AND page_id = %d", $weight, $widget->widget_id, $_POST['page_id']);
  }

  db_query("UPDATE {neo_dashboard_widget} SET page_id = %d, col = 0, weight = 0 WHERE widget_id = %d AND page_id = %d", $_POST['page_id'], $_POST['widget_id'], $old_id);
}

/**
 * Menu callback for deleting a widget.
 */
function neo_dashboard_widget_remove() {
  global $user;
  $page_id = db_result(db_query("SELECT {neo_dashboard_page}.page_id FROM {neo_dashboard_page} LEFT JOIN {neo_dashboard_widget} ON {neo_dashboard_page}.page_id = {neo_dashboard_widget}.page_id WHERE uid = %d AND widget_id = %d", $user->uid, $_POST['widget_id']));
  if ($page_id) {
    db_query("DELETE FROM {neo_dashboard_widget} WHERE widget_id = %d AND page_id = %d", $_POST['widget_id'], $page_id);
  }
}

/**
 * Generate a list of widgets for people to add to their personal NeoDash\'s.
 * @param unknown_type $page
 * @return unknown_type
 */
function neo_dashboard_widgets_page($page) {

  drupal_rebuild_theme_registry();
  global $user;
  $user->uid == arg(1);
  neo_dashboard_add_ui($page);
  $output = theme('neo_dashboard_widget_browser');

  return $output;
}

/**
 * Generate a list of system widgets to the User personal NeoDash\'s.
 * @param unknown_type $page
 * @return unknown_type
 */
function neo_dashboard_system_widgets_page($page) {

  drupal_rebuild_theme_registry();
  global $user;
  neo_dashboard_add_ui($page);
  $output = theme('neo_dashboard_system_widget_browser');

  return $output;
}

/**
 * Get default NeoDash page id for logged user. the default page is the first in the tab list.
 */
function neo_dashboard_get_default_page_id() {
  global $user;
  if (!empty($user->uid)) {
    $result = db_query('SELECT page_id FROM {neo_dashboard_page} WHERE uid = %d ORDER BY weight ASC', $user->uid);
    $row = db_fetch_object($result);
    if ($row->page_id) {
      return $row->page_id;
    }
  }
  return FALSE;
}

/**
 * Generate a list of public widgets
 */
function neo_dashboard_get_public_defaults() {

  // selects a list of all widgets
  $output = '';
  $query = db_query("SELECT * FROM {neo_dashboard_default} WHERE default_permanent != '1'");
  $widgets = array();

  while ($results = db_fetch_object($query)) {
    $widgets[] = $results;
  }
  return $widgets;
}

/**
 * Generate a list of system widgets
 */
function neo_dashboard_get_system_defaults() {

  // selects a list of all system widgets
  $output = '';
  $query = db_query("SELECT * FROM {neo_dashboard_default} WHERE default_permanent = '1'");
  $widgets = array();

  while ($results = db_fetch_object($query)) {
    $widgets[] = $results;
  }
  return $widgets;
}

/**
 * Preprocessor for the browse widgets page.
 * @param array $vars Variables
 */
function template_preprocess_neo_dashboard_display_widget(&$vars) {

  $widget = $vars['widget'];

  // controls whether or not we see thumbnails
  if (variable_get('neo_dashboard_thumbs', 0)) {
    if ($widget->thumbnail !== '') {
      $img = base_path() . $widget->thumbnail;
    } else {
      $img = base_path() . variable_get('neo_dashboard_thumb_default', '');
    }
    $vars['thumbnail'] = "<img src='" . $img . "' >";
  }

  $widget_list = neo_dashboard_load_all_widgets();

  //is_object prevents mistaking a page_id with a widget_id
  if (isset($widget_list[$widget->id]) && is_object($widget_list[$widget->id])) {
    // Add the "Add to NeoDash" link to template
    $vars['add_to_neo_dashboard'] = t('Already on User NeoDash.');
  }
  else {
    // Add the "Add to NeoDash" link to template
    $vars['add_to_neo_dashboard'] = '<a href="#" class="add-widget" id="add-widget-'. $widget->id .'">'. t('Add to User NeoDash') .'</a>';
  }
}

/**
 * Preprocessor for the browse widgets page.
 * @param array $vars Variables
 */
function template_preprocess_neo_dashboard_widget_browser(&$vars) {

  global $user;
  $user2 = user_load(arg(1));
  //$widgets = neo_dashboard_get_public_defaults();
  $widgets_public = neo_dashboard_get_public_defaults();
  $widgets_system = neo_dashboard_get_system_defaults();
  if (($user->uid == 1) && ($user2->uid == 1)) {
     $widgets = $widgets_public + $widgets_system;
  } else {
     $widgets = $widgets_public;
  }

  $form_state = array(
    'widgets' => $widgets,
    'input' => $_POST,
    'method' => 'post',
    'rerender' => TRUE,
    'no_redirect' => TRUE,
  );

  //TODO move back to CORE FAPI
  //TODO remove all filters except tags, display tags as checkboxes
  if(count($widgets) > 0){
    $vars['widget_filter'] = drupal_build_form('neo_dashboard_select_widget_form', $form_state);
  } else {
    $vars['widget_filter'] = '';
  }
  //prevent issue w/ dupe links from d.o_crosssite
  if(variable_get('theme_default', '') != 'bluecheese' && $user->theme != 'bluecheese') {
    $vars['links'] = theme('links', neo_dashboard_user_tabs());
  }
  
  $sorts = array();

  foreach ($widgets as $widget) {
    //TODO: change this logic to accomodate the checkboxes
    $check = false;

    foreach ($form_state['values']['tags'] as $tag) {
      if ($widget->tags === $tag || $form_state['values']['default'] === 'true') {
        $check = true;
      }
    }
    if ($check !== true) {
      continue;
    }

    // TODO: get all the right fields in here
    $item = new stdclass();
    $item->id = $widget->id;
    $item->title = check_plain($widget->title);
    $item->type = $widget->widget_type;
    $item->subtype = $widget->subtype;
    $item->thumbnail = '';
    $item->description = check_plain($widget->description);
    $item->default_enabled = $widget->default_enabled;

    $item->ops = array();
    $item->ops[] = l(t('Edit'), "admin/settings/neodash/widget/$widget->id/edit");
    $item->ops[] = l(t('Delete'), "admin/settings/neodash/widget/$widget->id/delete");
    $item->ops = implode(' | ', $item->ops);

    if (!empty($widget->tags)) {
      $item->tag = check_plain($widget->tags);
    }

    $item->classes = empty($widget->default_enabled) ? 'widget-enabled' : 'widget-disabled';
    $items[] = $item;

    $sort = '';
    switch ($form_state['values']['order']) {
      case 'title':
      default:
        $sort = strtolower($widget->title);
        break;
      case 'type':
        $sort .= strtolower($widget->type);
        break;
      case 'tag':
        $sort .= strtolower($widget->tags);
        break;
      case 'desc':
        $sort .= strtolower($widget->description);
        break;
    }
    $sorts[] = $sort;
  }

  $i = array();
  foreach ($sorts as $id => $title) {
    $i[] = $items[$id];
  }

  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  ctools_modal_add_js();

  $vars['widgets'] = array();

  $vars['widgets'] = $i;

}
/**
 * Preprocessor for the browse widgets page.
 * @param array $vars Variables
 */
function template_preprocess_neo_dashboard_system_widget_browser(&$vars) {

  global $user;
  //$widgets = neo_dashboard_get_public_defaults();
  $widgets = neo_dashboard_get_system_defaults();

  $form_state = array(
    'widgets' => $widgets,
    'input' => $_POST,
    'method' => 'post',
    'rerender' => TRUE,
    'no_redirect' => TRUE,
  );

  //TODO move back to CORE FAPI
  //TODO remove all filters except tags, display tags as checkboxes
  if(count($widgets) > 0){
    $vars['widget_filter'] = drupal_build_form('neo_dashboard_select_widget_form', $form_state);
  } else {
    $vars['widget_filter'] = '';
  }
  //prevent issue w/ dupe links from d.o_crosssite
  if(variable_get('theme_default', '') != 'bluecheese' && $user->theme != 'bluecheese') {
    $vars['links'] = theme('links', neo_dashboard_user_tabs());
  }
  
  $sorts = array();

  foreach ($widgets as $widget) {
    //TODO: change this logic to accomodate the checkboxes
    $check = false;

    foreach ($form_state['values']['tags'] as $tag) {
      if ($widget->tags === $tag || $form_state['values']['default'] === 'true') {
        $check = true;
      }
    }
    if ($check !== true) {
      continue;
    }

    // TODO: get all the right fields in here
    $item = new stdclass();
    $item->id = $widget->id;
    $item->title = check_plain($widget->title);
    $item->type = $widget->widget_type;
    $item->subtype = $widget->subtype;
    $item->thumbnail = '';
    $item->description = check_plain($widget->description);
    $item->default_enabled = $widget->default_enabled;

    $item->ops = array();
    $item->ops[] = l(t('Edit'), "admin/settings/neodash/widget/$widget->id/edit");
    $item->ops[] = l(t('Delete'), "admin/settings/neodash/widget/$widget->id/delete");
    $item->ops = implode(' | ', $item->ops);

    if (!empty($widget->tags)) {
      $item->tag = check_plain($widget->tags);
    }

    $item->classes = empty($widget->default_enabled) ? 'widget-enabled' : 'widget-disabled';
    $items[] = $item;

    $sort = '';
    switch ($form_state['values']['order']) {
      case 'title':
      default:
        $sort = strtolower($widget->title);
        break;
      case 'type':
        $sort .= strtolower($widget->type);
        break;
      case 'tag':
        $sort .= strtolower($widget->tags);
        break;
      case 'desc':
        $sort .= strtolower($widget->description);
        break;
    }
    $sorts[] = $sort;
  }

  $i = array();
  foreach ($sorts as $id => $title) {
    $i[] = $items[$id];
  }

  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  ctools_modal_add_js();

  $vars['widgets'] = array();

  $vars['widgets'] = $i;

}

/**
 * Provide a form for filtering the public list of widgets.
 */
function neo_dashboard_select_widget_form(&$form_state) {

  // check for clean urls
  if (!variable_get('clean_url', FALSE)) {
    $form['q'] = array(
      '#type' => 'hidden',
      '#value' => $_GET['q'],
    );
  }

  if (sizeof($form_state['input']) == 0) {
      $form['default'] = array(
        '#type' => 'hidden',
        '#value' => 'true',
      );
    }

  $tags = array();
  foreach ($form_state['widgets'] as $name => $widget) {
    if (!empty($widget->tags)) {
      $tags[$widget->tags] = check_plain($widget->tags);
    }
  }
  asort($tags);

  if ($form_state['input']['tags']) {
    $defaults = $form_state['input']['tags'];
  } else {
    $defaults = array_merge($tags);
  }

  $form_state['values']['tags'] = $defaults;

  //TODO: change default value to reflect selected tags
  $form['tags'] = array(
    '#type' => 'checkboxes',
    '#prefix' => '<h3 class="form-title">'. t('Categories') .'</h3>',
    '#options' => array_merge($tags),
    '#default_value' => $defaults,
  );

  $form['submit'] = array(
    '#name' => '', // so it won't in the $_GET args
    '#type' => 'submit',
    '#id' => 'edit-widget-apply',
    '#value' => t('Apply'),
  );
  //TODO change the form theme to something more appropriate
  // $form['#theme'] = array('neo_dashboard_list_widget_form_form');
  return $form;
}
function neo_dashboard_access_browser() {
   return true;
}
