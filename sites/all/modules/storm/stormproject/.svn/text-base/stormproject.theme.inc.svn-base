<?php

/**
 * @file
 */

function theme_stormproject_view($node, $teaser = FALSE, $page = FALSE) {
  drupal_add_css(drupal_get_path('module', 'storm') . '/storm-node.css');
  
  $node = node_prepare($node, $teaser);
  
  $node->content['links'] = array(
    '#prefix' => '<div class="stormlinks"><dl>',
    '#suffix' => '</dl></div>',
    '#weight' => -25,
  );
  
  $node->content['links']['expenses'] = theme('storm_link', 'stormproject', 'stormexpense', $node->nid, 1);
  $node->content['links']['notes'] = theme('storm_link', 'stormproject', 'stormnote', $node->nid, 2);
  $node->content['links']['tasks'] = theme('storm_link', 'stormproject', 'stormtask', $node->nid, 3);
  $node->content['links']['tickets'] = theme('storm_link', 'stormproject', 'stormticket', $node->nid, 4);  
  $node->content['links']['timetrackings'] = theme('storm_link', 'stormproject', 'stormtimetracking', $node->nid, 5);
  
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

  $node->content['group2'] = array(
    '#prefix' => '<div class="stormfields">',
    '#suffix' => '</div>',
    '#weight' => module_exists('content') ? content_extra_field_weight($node->type, 'group2') : -19,
  );

  $node->content['group2']['projectcategory'] = array(
    '#prefix' => '<div class="projectcategory">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Category'), stormattribute_value('Project category', $node->projectcategory)),
    '#weight' => 1,
  );

  $node->content['group2']['projectstatus'] = array(
    '#prefix' => '<div class="projectstatus">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Status'), stormattribute_value('Project status', $node->projectstatus)),
    '#weight' => 2,
  );

  $node->content['group2']['projectpriority'] = array(
    '#prefix' => '<div class="projectpriority">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Priority'), stormattribute_value('Project priority', $node->projectpriority)),
    '#weight' => 3,
  );

  $node->content['group5'] = array(
    '#prefix' => '<div class="stormfields">',
    '#suffix' => '</div>',
    '#weight' => module_exists('content') ? content_extra_field_weight($node->type, 'group5') : -16,
  );
  
  $node->content['group5']['manager'] = array(
    '#prefix' => '<div class="manager">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Project Manager'), l($node->manager_title, 'node/'. $node->manager_nid)),
    '#weight' => 1,
  );
  
  $node->content['group5']['assigned'] = array(
    '#prefix' => '<div class="assigned">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Assigned to'), l($node->assigned_title, 'node/'. $node->assigned_nid)),
    '#weight' => 2,
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