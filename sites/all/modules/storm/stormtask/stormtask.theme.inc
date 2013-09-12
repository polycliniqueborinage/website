<?php

/**
 * @file
 */

function theme_stormtask_tasks($form) {
  if (count($form['tasks'])>0 && stormproject_access('update', $form['project']['#value'])) {
    drupal_add_tabledrag('tasks', 'match', 'parent', 'task-parent-nid', 'task-parent-nid', 'task-nid', TRUE);
    drupal_add_tabledrag('tasks', 'order', 'sibling', 'task-weight'); 
  }

  $header = array();
  $row = array();

  $header = $form['header']['#value'];
  unset($form['header']);

  foreach (element_children($form['tasks']) as $key) {
    $form['tasks'][$key]['task_parent-nid_'. $key]['#attributes']['class'] = 'task-parent-nid';
    $form['tasks'][$key]['task_nid_'. $key]['#attributes']['class'] = 'task-nid';
    $form['tasks'][$key]['task_weight_'. $key]['#attributes']['class'] = 'task-weight';
    $durationunit = $form['tasks'][$key]['task_durationunit_'. $key]['#value'];

    $data = array(
      theme('indentation', $form['tasks'][$key]['task_depth_'. $key]['#value']) . drupal_render($form['tasks'][$key]['task_description_'. $key]),
      drupal_render($form['tasks'][$key]['task_category_'. $key]),
      drupal_render($form['tasks'][$key]['task_status_'. $key]),
      drupal_render($form['tasks'][$key]['task_priority_'. $key]),
      array(
        'data' => sprintf('%.2f', drupal_render($form['tasks'][$key]['task_duration_'. $key])) .' '. substr($durationunit, 0, 1), 
        'style' => 'text-align: right'),
      drupal_render($form['tasks'][$key]['task_parent-nid_'. $key]) . drupal_render($form['tasks'][$key]['task_nid_'. $key]),
      drupal_render($form['tasks'][$key]['task_weight_'. $key]),
      array(
        'data' => drupal_render($form['tasks'][$key]['task_operations_'. $key]),
        'class' => 'storm_list_operations',
      ),
    );
    if (!stormproject_access('update', $form['project']['#value'])) {
      unset($data[5]);
      unset($data[6]);
    }
    $row['data'] = $data;
    if (stormproject_access('update', $form['project']['#value'])) {
      $row['class'] = empty($row['class']) ? 'draggable' : $row['class'] .' draggable';
    }
    $rows[] = $row;
  }
  $o = theme('table', $header, $rows, array('id' => 'tasks'));
  return $o;
}

function theme_stormtask_view($node, $teaser = FALSE, $page = FALSE) {
  drupal_add_css(drupal_get_path('module', 'storm') . '/storm-node.css');
  
  $node = node_prepare($node, $teaser);

  $node->content['links'] = array(
    '#prefix' => '<div class="stormlinks">',
    '#suffix' => '</div>',
    '#weight' => -25,
  );

  $node->content['links']['expenses'] = theme('storm_link', 'stormtask', 'stormexpense', $node->nid, $w++);
  $node->content['links']['notes'] = theme('storm_link', 'stormtask', 'stormnote', $node->nid, $w++);
  $node->content['links']['tickets'] = theme('storm_link', 'stormtask', 'stormticket', $node->nid, $w++);
  $node->content['links']['timetrackings'] = theme('storm_link', 'stormtask', 'stormtimetracking', $node->nid, $w++);
  
  $node->content['group1'] = array(
    '#prefix' => '<div class="stormfields">',
    '#suffix' => '</div>',
    '#weight' => module_exists('content') ? content_extra_field_weight($node->type, 'group1') : -20,
  );

  $node->content['group1']['organization'] = array(
    '#prefix' => '<div class="organization">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Organization'), l($node->organization_title, 'node/'. $node->organization_nid)),
    '#weight' => 1,
  );

  $node->content['group1']['project'] = array(
    '#prefix' => '<div class="project">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Project'), l($node->project_title, 'node/'. $node->project_nid)),
    '#weight' => 2,
  );

  $node->content['group1']['stepno'] = array(
    '#prefix' => '<div class="stepno">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Step no.'), $node->stepno),
    '#weight' => 3,
  );

  $node->content['group2'] = array(
    '#prefix' => '<div class="stormfields">',
    '#suffix' => '</div>',
    '#weight' => module_exists('content') ? content_extra_field_weight($node->type, 'group2') : -19,
  );

  $node->content['group2']['category'] = array(
    '#prefix' => '<div class="category">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Category'), stormattribute_value('Task category', $node->taskcategory)),
    '#weight' => 1,
  );

  $node->content['group2']['status'] = array(
    '#prefix' => '<div class="storm_status">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Status'), stormattribute_value('Task status', $node->taskstatus)),
    '#weight' => 2,
  );
 
  $node->content['group2']['priority'] = array(
    '#prefix' => '<div class="priority">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Priority'), stormattribute_value('Task priority', $node->taskpriority)),
    '#weight' => 3,
  );
  
  $node->content['body_field'] = array(
    '#prefix' => '<div class="stormbody">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Description'), $node->content['body']['#value']),
    '#weight' => module_exists('content') ? content_extra_field_weight($node->type, 'body_field') : -18,
  );
  unset($node->content['body']);

  return $node;
}