<?php

/**
 * @file
 *  Administrative functions for Commerce Credits.
 */

/**
 * Administration form for credit types.
 *
 * Administrators can use this form to add, delete, and
 * update the credit types.
 */
function commerce_credits_admin_credit_types_form($form, &$form_state) {
  $form['types'] = array(
    '#tree' => TRUE,
  );

  // List existing types.
  $types = commerce_credits_types();
  foreach ($types as $type) {
    $form['types'][$type->name]['type'] = array(
      '#type' => 'value',
      '#value' => $type,
    );
    $form['types'][$type->name]['group'] = array(
      '#type' => 'value',
      '#value' => $type->group_label
    );
    $form['types'][$type->name]['group_label'] = array(
      '#markup' => check_plain($type->group_label)
    );
    $form['types'][$type->name]['name'] = array(
      '#type' => 'value',
      '#value' => $type->name,
    );
    $form['types'][$type->name]['label'] = array(
      '#markup' => check_plain($type->name),
    );
    $form['types'][$type->name]['description'] = array(
      '#type' => 'textfield',
      '#default_value' => $type->description,
    );
    $form['types'][$type->name]['duration'] = array(
      '#type' => 'textfield',
      '#default_value' => $type->duration,
    );
    $form['types'][$type->name]['delete'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete'),
      '#title_display' => 'invisible',
      '#default_value' => FALSE,
    );
  }

  // Provide fields to create a new type.
  $new_type['group'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 255,
    '#title' => t('New type'),
    '#description' => t('Enter the group of the new credit type.')
  );
  $new_type['name'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 255,
    '#title' => t('&nbsp;'),
    '#description' => t('Enter name of the new credit type.')
  );
  $new_type['description'] = array(
    '#type' => 'textfield',
    '#maxlength' => 255,
    '#title' => '&nbsp;',
    '#description' => 'Enter a description of the new credit type.'
  );
  $new_type['duration'] = array(
    '#type' => 'textfield',
    '#maxlength' => 255,
    '#title' => '&nbsp;',
    '#description' => 'Enter the duration in minutes of this credit type'
  );
  $form['types'][] = $new_type;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Transforms the states administration form into a reorderable table.
 */
function theme_commerce_credits_admin_credit_types_form($variables) {
  $form = $variables['form'];

  //drupal_add_tabledrag('workbench-moderation-states', 'order', 'sibling', 'workbench-moderation-state-weight');

  $header = array(
    t('Group'),
    t('Name'),
    t('Description'),
    t('Duration'),
    array('data' => t('Delete'), 'class' => array('checkbox')),
  );

  $rows = array();
  foreach (element_children($form['types']) as $key) {
    $element = &$form['types'][$key];

    $row = array(
      'data' => array(),
      'class' => array(),
    );
    $row['data']['group'] = drupal_render($element['group']) . drupal_render($element['group_label']);
    $row['data']['name'] = drupal_render($element['name']) . drupal_render($element['label']);
    $row['data']['description'] = drupal_render($element['description']);
    $row['data']['duration'] = drupal_render($element['duration']);
    $row['data']['delete'] = drupal_render($element['delete']);

    $rows[] = $row;
  }

  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'commerce-credits-types')));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Form submit handler for moderation state administration.
 */
function commerce_credits_admin_credit_types_form_submit($form, &$form_state) {
  foreach ($form_state['values']['types'] as $key => $info) {
    if (!empty($info['delete'])) {
      commerce_credits_type_delete($info['type']);
      drupal_set_message(t('%type has been deleted.', array('%type' => $key)));
    }
    elseif (!empty($info['name'])) {
      $type = (object) array(
        'group_label' => $info['group'],
        'name' => $info['name'],
        'description' => $info['description'],
        'duration' => $info['duration'],
      );
      commerce_credits_type_save($type);
    }
  }

  drupal_set_message(t('Your settings have been saved.'));

  // Warn users that changes here may need require reconfiguring Views that use Workbench.
  /*$views_message = t('Depending on the changes you have made it may be necessary to <a href="@views" target="_blank">reconfigure Views</a> that leverage Workbench Moderation such as <a href="@workbench_moderation" target="_blank">workbench_moderation</a>', array('@views' => url('admin/structure/views'), '@workbench_moderation' => url('admin/structure/views/edit/workbench_moderation')));
  drupal_set_message($views_message, $type = 'warning', $repeat = FALSE);*/
}
