<?php
// $Id: neo_dashboard.admin.inc,v 1.7.4.2 2010/07/28 16:27:36 beretta627 Exp $

/**
 * @file
 * NeoDash module admin file.
 * Stores configuration and administrative elements for Dashboard.
 */

/**
 * Page callback to list views in the system.
 */
function neo_dashboard_overview($arg = NULL) {
  if ($arg != NULL) {
    return drupal_not_found();
  }

  drupal_add_css(drupal_get_path('module', 'neodash') . '/neo_dashboard.css');
  $output = theme('neo_dashboard_list_widget');

  return $output;
}

/**
 * Page callback to list blocks to be selected as default widgets in the system.
 */
function neo_dashboard_select_defaults($arg = NULL) {
  if ($arg != NULL) {
    return drupal_not_found();
  }

  drupal_add_css(drupal_get_path('module', 'neodash') . '/neo_dashboard.css');
  $output = theme('neo_dashboard_select_defaults');

  return $output;
}

/**
 * Page callback to add a default widget.
 */
function neo_dashboard_default_make_available() {

  if(!$_POST['subtype']) {
    return;
  }

  $subtype = $_POST['subtype'];
  list($module, $delta) = explode('-', $subtype, 2);

  $module_block = module_invoke($module, 'block', 'list');

  if ($module_block && $module_block[$delta]) {
    $block = $module_block[$delta];  
  }

  //return drupal_json(array('block info' => print_r($block,TRUE), 'module_block' => print_r($module_block,TRUE)));
  $title = $block['info'];
  $description = '"' . $title . '" block.';

  db_query("INSERT INTO {neo_dashboard_default} (title,description,tags,thumbnail,default_enabled,default_permanent,widget_type,subtype) VALUES ('%s','%s','blocks',0,'1','0','block','%s')", $title, $description, $subtype);

  return drupal_json(array('status' => TRUE, 'label' => 'User added.'));
}

/**
 * Page callback to add a system widget.
 */
function neo_dashboard_system_make_available() {

  if(!$_POST['subtype']) {
    return;
  }

  $subtype = $_POST['subtype'];
  list($module, $delta) = explode('-', $subtype, 2);

  $module_block = module_invoke($module, 'block', 'list');

  if ($module_block && $module_block[$delta]) {
    $block = $module_block[$delta];  
  }

  //return drupal_json(array('block info' => print_r($block,TRUE), 'module_block' => print_r($module_block,TRUE)));
  $title = $block['info'];
  $description = '"' . $title . '" block.';

  db_query("INSERT INTO {neo_dashboard_default} (title,description,tags,thumbnail,default_enabled,default_permanent,widget_type,subtype) VALUES ('%s','%s','blocks',0,'1','1','block','%s')", $title, $description, $subtype);

  return drupal_json(array('status' => TRUE, 'label' => 'System added.'));
}

/**
 * Page callback to remove a default widget.
 */
function neo_dashboard_default_remove() {

  if(!$_POST['subtype']) {
    return;
  }

  $subtype = $_POST['subtype'];

  db_query("DELETE FROM {neo_dashboard_default} WHERE subtype = '%s' LIMIT 1", $subtype);

  return drupal_json(array('status' => TRUE, 'label' => 'Default removed.'));
}

/**
 * Page callback to remove a default widget.
 */
function neo_dashboard_system_remove() {

  if(!$_POST['subtype']) {
    return;
  }

  $subtype = $_POST['subtype'];

  db_query("DELETE FROM {neo_dashboard_default} WHERE subtype = '%s' LIMIT 1", $subtype);

  return drupal_json(array('status' => TRUE, 'label' => 'System removed.'));
}

/**
 * Page callback for Dashboard general settings.
 *
function neo_dashboard_general($arg = NULL) {
  if ($arg != NULL) {
    return drupal_not_found();
  }

  $output = 'General settings page placeholder';

  return $output;
}
*/

/**
 * FormsAPI for module settings page
 */
function neo_dashboard_settings_form(&$form_state, $neo_dashboard = 'default') {
	//neo_dashboard_add_tools();
	$form = array('#attributes' => array('enctype' => 'multipart/form-data'));


  if(user_access('set default neo dashboard layout')) {
    $form['default-layout'] = array(
      '#type' => 'fieldset',
      '#title' => t('Default Layout'),
      '#description' => t('Make your <strong>current</strong> NeoDash layout the default layout for new users or for users that have deleted their dashboards. Be sure that your dashboard is appropriately configured before setting the default.'),
    );

    $default_layout = variable_get('neo_dashboard_default_layout','');
    $status = 'UNSET';

    if(is_array($default_layout)) {
      $status = 'SET';
    }

    $form['default-layout']['neo_dashboard_set_default_layout'] = array(
      '#type' => 'submit',
      '#value' => t('Set default layout'),
      '#submit' => array('neo_dashboard_set_default_layout_submit'),
      '#suffix' => 'Currently: <strong>' . $status . '</strong>',
    );
  }
  $form['personalization'] = array(
    '#type' => 'fieldset',
    '#title' => t('Personalization'),
    '#description' => t('Personalization settings for users creating NeoDash\'s.'),
  );

  $form['personalization']['neo_dashboard_personal_dashboards'] = array(
    '#type' => 'radios',
    '#title' => t('Personal NeoDash\'s'),
    '#default_value' => variable_get('neo_dashboard_personal_dashboards', 0),
    '#options' => array(0 => t('Disabled'), 1 => t('Enabled (for users with the "Add Personal NeoDash" permission)')),
    '#description' => t('Controls whether or not users are allowed to create personalized NeoDash\'s.')
  );

  /*
  $form['personalization']['neo_dashboard_personal_content'] = array(
    '#type' => 'radios',
    '#title' => t('Create Widgets'),
    '#default_value' => variable_get('neo_dashboard_personal_content', 0),
    '#options' => array(0 => t('Disabled'), 1 => t('Enabled (for users with the "Add Personal NeoDash" permission)')),
    '#description' => t('Are users allowed to add content to their NeoDash\'s from anywhere in the system?')
  );
  */

  $form['neo_dashboard_widget_content'] = array(
    '#type' => 'fieldset',
    '#title' => t('Widget Types'),
  );

  $content = array(
    'nodes' => 'nodes',
    'blocks' => 'blocks',
    //'views' => 'views',
  );

  //TODO: change default value to reflect selected tags
  $form['neo_dashboard_widget_content']['neo_dashboard_add_content'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Specify the types of content users can add to their personalized NeoDash\'s'),
    '#options' => $content,
    '#default_value' => variable_get('neo_dashboard_add_content', array('nodes' => 'nodes', 'blocks' => 'blocks')),
  );

  // create a directory for storing thumbnails
  if (variable_get('neo_dashboard_thumbs', 0)) {
    $thumb_path = file_create_path(variable_get('neo_dashboard', 'thumbnails'));
    file_check_directory($thumb_path, 1, 'neo_dashboard_thumbnail_path');
  }
  $thumb_support = variable_get('neo_dashboard_thumbs', 0);

  $form['neo_dashboard_thumbnails'] = array(
    '#type' => 'fieldset',
    '#title' => t('Widget Thumbnails'),
  );
  $form['neo_dashboard_thumbnails']['neo_dashboard_thumbs'] = array(
    '#type' => 'radios',
    '#title' => t('Thumbnail support'),
    '#default_value' => $thumb_support,
    '#options' => array(t('Disabled'), t('Enabled')),
  );

  $default_image = drupal_get_path('module', 'neo_dashboard') .'/images/widget-default.png';

  $form['neo_dashboard_thumbnails']['neo_dashboard_thumb_default'] = array(
    '#type' => 'value',
    '#value' => variable_get('neo_dashboard_thumb_default', $default_image)
  );

  $form['neo_dashboard_thumbnails']['neo_dashboard_thumb_fid'] = array(
    '#type' => 'value',
    '#value' => variable_get('neo_dashboard_thumb_fid', 0)
  );

  $file = db_fetch_object(db_query("SELECT * FROM {files} WHERE fid = %d", variable_get('neo_dashboard_thumb_fid', 0)));
  $default_image = '';
  if ($file) {
    $default_image = theme('image', $file->filepath);
  }
  $form['neo_dashboard_thumbnails']['neo_dashboard_thumb_file'] = array(
    '#prefix' => $default_image,
    '#type' => 'file',
    '#title' => t('Default thumbnail'),
    '#description' => t('Upload the default file'),
  );

  // TODO: remove? system_settings_form is adding own submit button
  // $form['submit'] = array(
  //   '#type' => 'submit',
  //   '#value' => t('Submit'),
  // );

  return system_settings_form($form);

}

function neo_dashboard_set_default_layout_submit($form, &$form_state) {
  global $user;
  include_once './' . drupal_get_path('module', 'neo_dashboard') . '/neo_dashboard.defaults.inc';

  neo_dashboard_set_default_layout($user->uid);
}

/**
 * Validate form & handle upload of new file
 */
function neo_dashboard_settings_form_validate($form, &$form_state) {

  $values = $form_state['values'];
  if ((is_integer($values['neo_dashboard_add_content']['nodes'])) && (is_integer($form_state['values']['neo_dashboard_add_content']['blocks'])))  {
    form_set_error('neo_dashboard_add_content', t('You must select at least one type of content for users to add to their NeoDash\'s.'));
  }
  $validators = array('file_validate_is_image' => array());
  if ($file = file_save_upload('neo_dashboard_thumb_file', $validators, file_directory_path())) {
    // If there is old file delete it
    $old_file = db_fetch_object(db_query("SELECT * FROM {files} WHERE fid = %d", $form_state['values']['neo_dashboard_thumb_fid']));
    if ($old_file) {
      file_delete($old_file->filepath);
      db_query("DELETE FROM {files} WHERE fid = %d", $old_file->fid);
    }

    $form_state['values']['neo_dashboard_thumb_fid'] = $file->fid;
    $form_state['values']['neo_dashboard_thumb_default'] = $file->filepath;
  }
}

/**
 * Preprocess the list widget theme
 */
function template_preprocess_neo_dashboard_list_widget(&$vars) {
  drupal_add_js(drupal_get_path('module', 'neo_dashboard') . '/neo_dashboard.admin.js');
  $widgets = neo_dashboard_get_all_defaults();

  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  ctools_modal_add_js();

  $form_state = array(
    'widgets' => $widgets,
    'input' => $_POST,
    'method' => 'post',
    'rerender' => TRUE,
    'no_redirect' => TRUE,
  );

  //TODO move back to CORE FAPI
  $vars['widget_filter'] = drupal_build_form('neo_dashboard_list_widget_form', $form_state);
  $sorts = array();

  foreach ($widgets as $widget) {
    if ($form_state['values']['tag'] != 'all') {
      if ($form_state['values']['tag'] == 'none') {
        if (!empty($widget->tags)) {
          continue;
        }
      }
      else if ($form_state['values']['tag'] != $widget->tags) {
        continue;
      }
    }
    if ($form_state['values']['type'] != 'all' && $form_state['values']['type'] != $widget->widget_type) {
      continue;
    }

    // TODO: get all the right fields in here
    $item = new stdclass();
    $item->title = $widget->title;
    $item->type = $widget->widget_type;
    $item->subtype = $widget->subtype;
    $item->thumbnail = '';
    $item->description = check_plain($widget->description);
    $item->default_enabled = $widget->default_enabled;

    $item->ops = array();
    //$item->ops[] = l(t('Edit'), "admin/settings/neodash/widget/$widget->id/edit");
    $item->ops[] = ctools_modal_text_button('Edit', "admin/settings/neodash/widget/$widget->id/edit", $alt, $class);
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

  if ($form_state['values']['sort'] == 'desc') {
    arsort($sorts);
  } else {
    asort($sorts);
  }

  $i = array();
  foreach ($sorts as $id => $title) {
    $i[] = $items[$id];
  }

  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  ctools_modal_add_js();

  $vars['widget_add'] = ctools_modal_text_button("Create Default Widget", 'admin/settings/neodash/add/modal', $alt, $class);

  $vars['widgets'] = $i;
}

/**
 * Preprocess the list widget theme
 */
function template_preprocess_neo_dashboard_select_defaults(&$vars) {
  drupal_add_js(drupal_get_path('module', 'neo_dashboard') . '/neo_dashboard.admin.js');

  //TODO@ alphabetize this list of blocks
  $blocks = _block_rehash();
  $subtypes = array();
  $permanent = array();

  $defaults_query = db_query("SELECT subtype FROM {neo_dashboard_default}");

  while($results = db_fetch_object($defaults_query)) {
     $subtypes[] = $results->subtype;
  }

  $permanent_query = db_query("SELECT subtype FROM {neo_dashboard_default} WHERE default_permanent='1'");

  while($permres = db_fetch_object($permanent_query)) {
     $permanent[] = $permres->subtype;
  }
  $block_options = array();

  foreach($blocks as $key => $block) {
    $subtype = $block['module'] . '-' . $block['delta'];
    $block_options[$key]['subtype'] = $block['module'] . '-' . $block['delta'];
    $block_options[$key]['title'] = $block['info'];
    /*if(in_array($subtype, $subtypes)) {
    } else {
    }*/
    if(in_array($subtype, $subtypes)) {
      if (in_array($subtype,$permanent)) {
          $block_options[$key]['permanent'] = '<a href="#" class="remove-permanent" id="remove-permanent-' . $subtype . '">' . t('Remove') . '</a>'; 
      } else {
          $block_options[$key]['operation'] = '<a href="#" class="remove-default" id="remove-default-' . $subtype . '">' . t('Remove') . '</a>'; 
      }
    } else {
      $block_options[$key]['permanent'] = '<a href="#" class="make-permanent-default" id="make-permanent-default-' . $subtype . '">' . t('Make Available') . '</a>'; 
      $block_options[$key]['operation'] = '<a href="#" class="make-available-default" id="make-available-default-' . $subtype . '">' . t('Make Available') . '</a>'; 
    }

  }

  $vars['blocks'] = $blocks;
  $vars['block_options'] = $block_options;

}

/**
 * generate a list of default widgets
 */
function neo_dashboard_get_all_defaults() {

  // select distinct from views table
  // iterates through and displays
  $widget_type = variable_get('neo_dashboard_add_content', array('nodes' => 'nodes', 'blocks' => 'blocks'));

  if(is_string($widget_type['blocks']) && is_integer($widget_type['nodes'])) {
    $where = " WHERE widget_type = 'block'";
  } else if(is_integer($widget_type['blocks']) && is_string($widget_type['nodes'])) {
    $where = " WHERE widget_type = 'node'";
  }

  $output = '';
  $query = db_query("SELECT * FROM {neo_dashboard_default}" . $where);

  $widgets = array();

  while ($results = db_fetch_object($query)) {
  	$widgets[] = $results;
  }

  return $widgets;
}

/**
 * Provide a form for sorting and filtering the list of widgets within the administrator. Provides more options than the public version.
 */
function neo_dashboard_list_widget_form(&$form_state) {
 if (!variable_get('clean_url', FALSE)) {
    $form['q'] = array(
      '#type' => 'hidden',
      '#value' => $_GET['q'],
    );
  }

  $all = array('all' => t('<All>'));
  $none = array('none' => t('<None>'));

  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Storage'),
    '#options' => array(
      'all' => t('<All>'),
      'block' => t('Blocks'),
      'node' => t('Nodes'),
    ),
    '#default_value' => 'all',
  );

  $tags = array();

  $extras = array();
  foreach ($form_state['widgets'] as $name => $widget) {
  	if (!empty($widget->tags)) {
      $tags[$widget->tags] = $widget->tags;
    }
  }

  asort($tags);

  $form['tag'] = array(
    '#type' => 'select',
    '#title' => t('Tag'),
    '#options' => array_merge($all, $none, $tags),
    '#default_value' => 'all',
  );

  $form['order'] = array(
    '#type' => 'select',
    '#title' => t('Sort by'),
    '#options' => array(
      'title' => t('Title'),
      'tag' => t('Tag'),
      'widget_type' => t('Type'),
      'desc' => t('Description'),
    ),
    '#default_value' => 'name',
  );

  $form['sort'] = array(
    '#type' => 'select',
    '#title' => t('Order'),
    '#options' => array(
      'asc' => t('Up'),
      'desc' => t('Down'),
    ),
    '#default_value' => 'asc',
  );

  $form['submit'] = array(
    '#name' => '', // so it won't in the $_GET args
    '#type' => 'submit',
    '#id' => 'edit-widget-apply',
    '#value' => t('Apply'),
  );

  if (!empty($_SESSION['neo dashboard']['#admin'])) {
    $form['reset'] = array(
      '#type' => 'submit',
      '#id' => 'edit-widget-reset',
      '#value' => t('Reset'),
    );
  }

  $form['#theme'] = array('neo_dashboard_list_widget_form_form');
  return $form;
}

/**
 * generate a widget column
 *
 * @return unknown
 */
function theme_neo_dashboard_admin_widget() {
  return '';
}


function theme_neo_dashboard_list_widget_form_form($form) {
  // Don't render these:
  unset($form['form_id']);
  unset($form['form_build_id']);
  unset($form['form_token']);
  return drupal_render($form);
}

function neo_dashboard_wizard_modal() {
  $step = arg(5);
	ctools_include('wizard');
  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  $form_info = array(
    // think of this ID like a "sub" hook
    'id' => 'dashboard_generator',
    'js' => array('misc/autocomplete.js', 'misc/textarea.js', 'misc/collapse.js'),
    'path' => "admin/settings/neodash/add/modal/%step",
    'show back' => TRUE,
    'show cancel' => TRUE,
    'next text' => 'Proceed to next step',
    'next callback' =>  'neo_dashboard_generator_add_subtask_next',
    'finish callback' => 'neo_dashboard_generator_add_subtask_finish',
    'cancel callback' => 'neo_dashboard_generator_add_subtask_cancel',
    'order' => array(
      'information' => t('Step 1: Widget information'),
      'selection' => t('Step 2: Widget selection'),
    ),
    'forms' => array(
      'information' => array(
         // see docs in advanced help for more options here such as file path
        'form id' => 'neo_dashboard_generator_form_information'
      ),
      'selection' => array(
        'form id' => 'neo_dashboard_generator_form_selection'
      ),
    ),
  );

  // Load widget for editing
  if ($step == 'edit') {
    $neo_dashboard = db_fetch_object(db_query("SELECT * FROM {neo_dashboard_default} WHERE id = %d", arg(4)));
    $step = 'information';

    // Fix autocomplete field default value to look like autocomplete output
    switch ($neo_dashboard->widget_type) {
      case 'block':
        list($module, $delta) = explode('-', $neo_dashboard->subtype, 2);
        $module_block = module_invoke($module, 'block', 'list');
        if ($module_block && $module_block[$delta]) {
          $block = $module_block[$delta];
          $neo_dashboard->subtype = $block['info'] ." [module:". $module ." delta:". $delta ."]";
        }
        break;
      case 'node':
        list($type, $nid) = explode('-', $neo_dashboard->subtype);
        $node = node_load($nid);
        $neo_dashboard->subtype = $node->title ." [nid:$nid]";
        break;
    }
    // Update cache
    neo_dashboard_generator_set_page_cache($neo_dashboard);
  }
  else {
    // **  always load the stored values
    $neo_dashboard = neo_dashboard_generator_get_page_cache('');
  }
  // if there is none set up the default storage object
  if (!$neo_dashboard) {
    // set form to first step -- we have no data
    $step = current(array_keys($form_info['order']));
    // initialize your storage object
    $neo_dashboard = new stdClass();
    // ** set the storage object so its ready for whatever comes next
    neo_dashboard_generator_set_page_cache($neo_dashboard);
  }

  $form_state = array(
    'cache name' => '',
    'cstorage' => $neo_dashboard,
    'ajax' => TRUE,
    'modal' => TRUE,
    'commands' => array(),
  );

  ctools_wizard_multistep_form($form_info, $step, $form_state);
}

/*-------------------------- PART 2 JUST CLASSIC FORMAPI  ---------------------- */

/**
 * All forms within this wizard will take $form, and $form_state by reference
 * note that the form doesn't have a return value.
 */
function neo_dashboard_generator_form_information(&$form, &$form_state) {
  $widget = &$form_state['cstorage'];
  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#title' => 'Widget title',
    '#default_value' => $widget->title,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#title' => 'Widget description',
    '#default_value' => $widget->description,
  );
  if(variable_get('neo_dashboard_thumbs',0)) {
    $form['thumbnail'] = array(
      '#type' => 'file',
      '#required' => 0,
      '#title' => 'Widget thumbnail',
      '#default_value' => $widget->thumbnail,
    );
  }
  $form['tags'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#title' => 'Widget tags',
    '#default_value' => $widget->tags,
    '#autocomplete_path' => 'admin/settings/neodash/ajax/autocomplete/tag',
  );
  $form['widget_type'] = array(
    '#title' => 'Widget type',
    '#type' => 'radios',
    '#required' => 1,
    '#default_value' => $widget->widget_type,
    '#options' => array(
      'block' => 'Block',
      'node' => 'Node',
      //'view' => 'View',
    )
  );
  $form_state['no buttons'] = TRUE;
}

/**
 * Page callback for views tag autocomplete
 */
function neo_dashboard_autocomplete_tag($string = '') {
  $matches = array();
  // get matches from default views:
  $widgets = neo_dashboard_get_all_defaults();
  foreach ($widgets as $widget) {
    if (!empty($widget->tags) && strpos($widget->tags, $string) === 0) {
      $matches[$widget->tags] = $widget->tags;
    }
  }

  if ($string) {
    $result = db_query_range("SELECT DISTINCT tags FROM {neo_dashboard_default} WHERE LOWER(tags) LIKE LOWER('%s%%')", $string, 0, 10 - count($matches));
    while ($widget = db_fetch_object($result)) {
      $matches[$widget->tags] = check_plain($widget->tags);
    }
  }

  drupal_json($matches);
}

/**
 * Note that this validate callback operates exactly like it does in the regular form api
 */
function neo_dashboard_generator_form_information_validate(&$from, &$form_state) {
}

/**
 * KEY CONCEPT: generally, you will never save data here -- you will simply add values to the
 * yet to be saved ctools_cache object.
 *
 * Saving happens at the end, within the $form_info['finish callback'];
 */
function neo_dashboard_generator_form_information_submit(&$from, &$form_state) {
  $submitted = $form_state['values'];
  $save_values = array('title', 'description', 'thumbnail', 'widget_type', 'tags');
  foreach($save_values as $value) {
    // set it in cstorage, the voodoo in part 3 will take care of the rest magically
    $form_state['cstorage']->$value = $submitted[$value];
  }
}

/**
 * Menu callback; Retrieve a pipe delimited string of autocomplete suggestions for existing users
 */
function neo_dashboard_autocomplete_node($string = '') {
	$sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE n.title LIKE '%%%s%%'");
  $result = db_query($sql, $string);
  $references = array();
  while ($nodes = db_fetch_object($result)) {
    $references[$nodes->nid] = array(
      'title' => $nodes->title,
      'rendered' => check_plain($nodes->title),
    );
  }
  $matches = array();
  if (!empty($references)) {
    foreach ($references as $id => $row) {
      // Add a class wrapper for a few required CSS overrides.
      $matches[$row['title'] ." [nid:$id]"] = '<div class="reference-autocomplete">'. $row['rendered'] . '</div>';
    }
  }
  drupal_json($matches);
}

function neo_dashboard_autocomplete_block($string = '') {
  $matches = array();
  foreach (module_list() as $module) {
    $module_blocks = module_invoke($module, 'block', 'list');
    if ($module_blocks) {
      foreach ($module_blocks as $delta => $block) {
        // If it's a new block, add identifiers.
        $block['module'] = $module;
        $block['delta']  = $delta;
        // Add to the list of blocks we return.
        // TODO make this a preg
        if (strpos(strtolower($block['info']), strtolower($string)) !== FALSE) {
          $matches[$block['info'] ." [module:". $block['module'] . ' delta:' . $block['delta'] ."]"] = '<div class="reference-autocomplete">'. $block['info'] . '</div>';
        }
      }
    }
  }
  drupal_json($matches);
}

function neo_dashboard_generator_form_selection(&$form, &$form_state) {
  $widget = &$form_state['cstorage'];
  //TODO make autocomplete and add if logic for the path
  if ($widget->widget_type == 'node') {
  	$path = 'admin/settings/neodash/ajax/autocomplete/node';
  }
  else if ($widget->widget_type == 'block') {
  	$path = 'admin/settings/neodash/ajax/autocomplete/block';
  }
  $form['subtype'] = array(
    '#type' => 'textfield',
    '#title' => 'Widget',
    '#description' => 'Select the block or node to display as a widget using the field above. Select content by title. Partial matches allowed.',
    '#required' => 1,
    '#default_value' => $widget->subtype,
    '#autocomplete_path' => $path,
  );
  $form_state['no buttons'] = TRUE;
}


/**
 * Same idea as previous steps submit
 */
function neo_dashboard_generator_form_selection_submit(&$form, &$form_state) {
  $submitted = $form_state['values'];
  $form_state['cstorage']->subtype = $submitted['subtype'];
  //TODO build conf if necessary
  //TODO what are we going to do with defaults here
}

/*----PART 3 CTOOLS CALLBACKS -- these usually don't have to be very unique  ---------------------- */

/**
 * Callback generated when the add page process is finished.
 * this is where you'd normally save.
 */
function neo_dashboard_generator_add_subtask_finish(&$form_state) {
  $widget = &$form_state['cstorage'];
  // Clear the cache
  if ($widget->widget_type == 'block') {
    $widget->conf = "";
    preg_match('/^(.*)\[module\:(.*)\s*delta\:(.*)\]$/', $widget->subtype, $matches);
    if (!empty($matches)) {
      list(, $title, $module, $delta) = $matches;
      $widget->subtype = trim($module) . '-' . $delta;
    }
  }
  else {
    preg_match('/^(?:\s*|(.*) )?\[\s*nid\s*:\s*(\d+)\s*\]$/', $widget->subtype, $matches);
    if (!empty($matches)) {
      list(, $title, $nid) = $matches;
      $widget->subtype = 'node-'. $nid;
      $widget->conf = serialize(array('nid' => $nid));
    }
  }
  // Not current used so set to true
  $widget->default_enabled = TRUE;

  // Set update when appropriate (id exists)
  $update = array();
  if ($widget->id) {
    $update[] = 'id';
  }

  drupal_write_record("neo_dashboard_default", $widget, $update);
  neo_dashboard_generator_clear_page_cache($form_state['cache name']);

  //update outdated widgets on user pages

  if(!empty($widget->id) && !empty($widget->subtype)) {
    db_query("UPDATE {neo_dashboard_widget} SET subtype = '%s' WHERE widget_id = '%d'", $widget->subtype, $widget->id);
  }
}

/**
 * Callback for the proceed step
 *
 */
function neo_dashboard_generator_add_subtask_next(&$form_state) {
  $neo_dashboard = &$form_state['cstorage'];
  ctools_include('object-cache');
  $cache = ctools_object_cache_set('neo_dashboard_generator', $form_state['cache name'], $neo_dashboard);
}

/**
 * Callback generated when the 'cancel' button is clicked.
 *
 * All we do here is clear the cache.
 */
function neo_dashboard_generator_add_subtask_cancel(&$form_state) {
  neo_dashboard_generator_clear_page_cache($form_state['cache name']);
}

/*----PART 4 CTOOLS FORM STORAGE HANDLERS -- these usually don't have to be very unique  ---------------------- */

/**
 * Store changes to a task handler in the object cache.
 */
function neo_dashboard_generator_set_page_cache($neo_dashboard) {
  ctools_include('object-cache');
  ctools_object_cache_set('neo_dashboard_generator', $form_state['cache name'], $neo_dashboard);
}

/**
 * Remove an item from the object cache.
 */
function neo_dashboard_generator_clear_page_cache($name) {
  ctools_include('object-cache');
  ctools_object_cache_clear('neo_dashboard_generator', $name);
}

/**
 * Get the cached changes to a given task handler.
 */
function neo_dashboard_generator_get_page_cache($name) {
  ctools_include('object-cache');
  $cache = ctools_object_cache_get('neo_dashboard_generator', $name);
  return $cache;
}

/**
 * Menu callback - ask for confirmation of default widget deletion
 */
function neo_dashboard_default_widget_delete_confirm(&$form_state, $widget_id) {
  $form['widget_id'] = array(
    '#type' => 'value',
    '#value' => $widget_id,
  );

  return confirm_form($form,
    // @TODO: get the widget title here instead of $widget_id
    t('Are you sure you want to delete default widget %title?', array('%title' => $widget_id)),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/settings/neodash/widgets',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute default widget deletion
 */
function neo_dashboard_default_widget_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $widget_id = $form_state['values']['widget_id'];
    if (is_numeric($widget_id)) {
      // Delete all widget instances
      // @TODO: widget_id will be replaced with widget_default_id
      db_query("DELETE FROM {neo_dashboard_widget} WHERE widget_id = %d", $widget_id);
      // Delete default widget
      db_query("DELETE FROM {neo_dashboard_default} WHERE id = %d", $widget_id);
      drupal_set_message(t('Default widget %title deleted.', array('%title' => $widget_id)));
    }
  }
  $form_state['redirect'] = 'admin/settings/neodash/widgets';
}
