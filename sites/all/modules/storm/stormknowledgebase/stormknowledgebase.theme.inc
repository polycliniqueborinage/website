<?php

/**
 * @file
 */

function theme_stormknowledgebase_view(array $variables = array()) {
  $node = $variables['node'];
  $teaser = isset($variables['teaser']) ? $variables['teaser'] : FALSE;
  $page = isset($variables['page']) ? $variables['page'] : FALSE;
  
  drupal_add_css(drupal_get_path('module', 'storm') . '/storm-node.css');
  
  //$node = node_prepare($node, $teaser);
  
  /*$node->content['links'] = array(
    '#prefix' => '<div class="stormlinks"><dl>',
    '#suffix' => '</dl></div>',
    '#weight' => $w++,
  );

  $node->content['group1'] = array(
    '#prefix' => '<div class="stormfields">',
    '#suffix' => '</div>',
    '#weight' => module_exists('content') ? content_extra_field_weight($node->type, 'group1') : -20,
  );*/

  /*$node->content['group1']['topic'] = array(
    '#prefix' => '<div class="topic">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Topic'), stormattribute_value('Knowledge base topic', $node->topic)),
    '#weight' => 1,
  );

  $node->content['body_field'] = array(
    '#prefix' => '<div class="stormbody">',
    '#suffix' => '</div>',
    '#value' => theme('storm_view_item', t('Description'), $node->content['body']['#value']),
    '#weight' => module_exists('content') ? content_extra_field_weight($node->type, 'body_field') : -18,
  );
  unset($node->content['body']);*/
  
  return $node;
}

