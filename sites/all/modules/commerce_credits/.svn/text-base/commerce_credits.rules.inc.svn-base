<?php


/**
 * @file
 * Rules integration with commerce credits module.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_credits_rules_action_info() {
  $items = array();
  $items['update_credits_on_checkout'] = array(
    'label' => t('Updates credits based on order'),
    'group' => t('Commerce Credits'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'description' => t('The order containing the credits bought.'),
      ),
    ),
   'callbacks' => array(
      'execute' => 'commerce_credits_rules_action_update_on_checkout',
    ),
  );
  $items['transfer_credits'] = array(
    'label' => t('Transfer credits'),
    'group' => t('Commerce Credits'),
    'parameter' => array(
      'entity_from' => array(
        'type' => 'entity',
        'label' => t('Transfer from'),
        'description' => t('Entity to transfer from'),
      ),
      'entity_to' => array(
        'type' => 'entity',
        'label' => t('Transfer to'),
        'description' => t('Entity to transfer to'),
      ),
      'credit_type' => array(
        'type' => 'text',
        'label' => t('Credit type'),
        'description' => t('Credit type to be transfered.'),
        'options list' => 'commerce_credits_types_list',
        'restriction' => 'input'
      ),
      'number' => array(
        'type' => 'integer',
        'label' => t('Number of credits'),
        'description' => t('Number of credits that should be transfered.'),
        'restriction' => 'input'
      )
    ),
    'callbacks' => array(
      'execute' => 'commerce_credits_rules_action_transfer_credits'
    )
  );
  $items['set_credits_expiration_date'] = array(
    'label' => t('Set credits expiration date'),
    'group' => t('Commerce Credits'),
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'description' => t('Entity for which credits expiration date shall be set'),
      ),
      'credit_type' => array(
        'type' => 'text',
        'label' => t('Credit type'),
        'description' => t('Credit type for which expiration date shall be set.'),
        'options list' => 'commerce_credits_types_list',
        'restriction' => 'input'
      ),
    ),
    'callbacks' => array(
      'execute' => 'commerce_credits_rules_action_set_expiration_date'
    )
  );
  
  $items['delete_chosen_credit'] = array(
    'label' => t('Delete chosen credit'),
    'group' => t('Commerce Credits'),
    'parameter' => array(
    ),
    'callbacks' => array(
      'execute' => 'commerce_credits_rules_action_delete_chosen_credit'
    )
  );

  return $items;
}

/**
 * Updates number of credits a user has based on his order
 */
function commerce_credits_rules_action_update_on_checkout($order) {
  $uid = $order->uid;
  $numbers = array();
  $commerce_line_items = $order->commerce_line_items[LANGUAGE_NONE];
  foreach ($commerce_line_items as $l) {
    $id = $l['line_item_id'];
    $line_item = commerce_line_item_load($id);
    if ($line_item->type == 'product') {
      $sku = $line_item->line_item_label;
      $product = commerce_product_load_by_sku($sku);
      if ($product->type == 'commerce_credits') {
        $number = $product->commerce_credits_number[LANGUAGE_NONE][0]['value'];
        $credit_type = $product->commerce_credits_type[LANGUAGE_NONE][0]['value'];
        if (isset($numbers[$credit_type])) {
          $numbers[$credit_type] += $number;
        }
        else {
          $numbers[$credit_type] = $number;
        }
      }
    }
  }
  $user = entity_metadata_wrapper('user', user_load($uid));
  foreach ($numbers as $credit_type => $number) {
    commerce_credits_entity_save($user, $credit_type, $number);
  }
}

function _commerce_credits_get_real_type($type) {
  if ($type == 'from_session') {
    $type = isset($_SESSION[COMMERCE_CREDITS_SESSION_VARIABLE]) ? $_SESSION[COMMERCE_CREDITS_SESSION_VARIABLE] : '';
  }
  return $type;
}

/**
 * Transfers credits from one entity to another
 */
function commerce_credits_rules_action_transfer_credits($from, $to, $type, $number) {
  $type = _commerce_credits_get_real_type($type);
  $number_credits = commerce_credits_entity_credits($from, $type);
  if ($number_credits < $number) {
    throw new Exception("The entity does not have enough credits to be transferred.");
  }
  else {
    // Remove credits from from entity
    commerce_credits_entity_save($from, $type, $number, 'remove');
    // Add credits to to entity
    commerce_credits_entity_save($to, $type, $number);
  }
}

/**
 * Set credits expiration date
 */
function commerce_credits_rules_action_set_expiration_date($entity, $type) {
  $type = _commerce_credits_get_real_type($type);
  commerce_credits_set_expiration_date($entity, $type);
}

function commerce_credits_rules_action_delete_chosen_credit() {
  unset($_SESSION[COMMERCE_CREDITS_SESSION_VARIABLE]);
}

/**
 * Implements hook_rules_condition_info()
 */
function commerce_credits_rules_condition_info() {
  $items = array();
  $items['entity_has_credits'] = array(
    'label' => t('Entity has credits'),
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'description' => t('Specify the entity for which to evaluate the condition.'),
        'restriction' => 'selector'
      ),
      'credit_type' => array(
        'type' => 'text',
        'label' => t('Credit type'),
        'description' => t('Which credit type this entity should have for the condition to be true.'),
        'options list' => 'commerce_credits_types_list',
        'restriction' => 'input'
      ),
      'credit_number' => array(
        'type' => 'integer',
        'label' => t('Number of credits'),
        'description' => t('Number of credits the entity should have for this condition to be true.'),
        'restriction' => 'input'
      )
    ),
    'group' => t('Commerce Credits'),
    'callbacks' => array(
      'execute' => 'commerce_credits_rules_condition_entity_has_credits'
    )
  );
  $items['entity_has_credits_of_group'] = array(
    'label' => t('Entity has credits of group'),
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'description' => t('Specify the entity for which to evaluate the condition.'),
        'restriction' => 'selector'
      ),
      'credit_group' => array(
        'type' => 'text',
        'label' => t('Credit group'),
        'description' => t('Which credit group this entity should have for the condition to be true.'),
        'options list' => 'commerce_credits_groups_list',
        'restriction' => 'input'
      ),
      'credit_number' => array(
        'type' => 'integer',
        'label' => t('Number of credits'),
        'description' => t('Number of credits the entity should have for this condition to be true.'),
        'restriction' => 'input'
      )
    ),
    'group' => t('Commerce Credits'),
    'callbacks' => array(
      'execute' => 'commerce_credits_rules_condition_entity_has_credits_of_group'
    )
  );
  $items['credit_is_of_type'] = array(
    'label' => t('Credit is of type'),
    'parameter' => array(
      'credit_type_to_check' => array(
        'type' => 'text',
        'label' => t('Credit to check'),
        'description' => t('Specify the credit for which to evaluate the condition.'),
        'restriction' => 'selector'
      ),
      'credit_type' => array(
        'type' => 'text',
        'label' => t('Credit type'),
        'description' => t('Which credit type this credit should have for the condition to be true.'),
        'options list' => 'commerce_credits_types_list',
        'restriction' => 'input'
      ),
    ),
    'group' => t('Commerce Credits'),
    'callbacks' => array(
      'execute' => 'commerce_credits_rules_condition_credit_is_of_type'
    )
  );
  $items['credit_is_of_group'] = array(
    'label' => t('Credit is of group'),
    'parameter' => array(
      'credit_type_to_check' => array(
        'type' => 'text',
        'label' => t('Credit to check'),
        'description' => t('Specify the credit for which to evaluate the condition.'),
        'restriction' => 'selector'
      ),
      'credit_group' => array(
        'type' => 'text',
        'label' => t('Credit group'),
        'description' => t('Which credit group this credit should belong to for the condition to be true.'),
        'options list' => 'commerce_credits_groups_list',
        'restriction' => 'input'
      ),
    ),
    'group' => t('Commerce Credits'),
    'callbacks' => array(
      'execute' => 'commerce_credits_rules_condition_credit_is_of_group'
    )
  );
  $items['credit_was_chosen'] = array(
    'label' => t('Credit was chosen'),
    'parameter' => array(
    ),
    'group' => t('Commerce Credits'),
    'callbacks' => array(
      'execute' => 'commerce_credits_rules_condition_credit_was_chosen'
    )
  );
  return $items;
}

function commerce_credits_rules_condition_entity_has_credits($entity, $credit_type, $credit_number) {
  $credit_type = _commerce_credits_get_real_type($credit_type);
  $number_credits = commerce_credits_entity_credits($entity, $credit_type);
  if ($number_credits >= $credit_number) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

function commerce_credits_rules_condition_entity_has_credits_of_group($entity, $credit_group, $credit_number) {
  $credits = commerce_credits_entity_credits($entity);
  if (empty($credits)) {
    return FALSE;
  }
  else {
    $types = commerce_credits_types();
    $return = FALSE;
    $total_number = 0;
    foreach ($credits as $credit_type => $number) {
      $type = $types[$credit_type];
      if ($type->group_label == $credit_group) {
        $total_number += $number;
      }
    }
    if ($total_number >= $credit_number) {
      $return = TRUE;
    }
    return $return;
  }
}

function commerce_credits_rules_condition_credit_is_of_type($credit_type_to_check, $credit_type) {
  $credit_type = _commerce_credits_get_real_type($credit_type);
  return ($credit_type_to_check == $credit_type);
}

function commerce_credits_rules_condition_credit_is_of_group($credit_type_to_check, $credit_group) {
  $types = commerce_credits_types();
  $type = $types[$credit_type_to_check];
  return ($type->group_label == $credit_group);
}

function commerce_credits_rules_condition_credit_was_chosen() {
  return isset($_SESSION[COMMERCE_CREDITS_SESSION_VARIABLE]);
}

/**
 * Implements hook_rules_event_info()
 */
function commerce_credits_rules_event_info() {
  $items = array();
  $items['when_credits_expire'] = array(
    'label' => t('When credits expire'),
    'variables' => array(
      'entity' => array('type' => 'entity', 'label' => t('associated entity')),
      'credit_type' => array('type' => 'text', 'label' => t('credit type'))
    ),
    'group' => t('Commerce Credit'),
    'module' => 'commerce_credits'
  );
  $items['after_credit_choice'] = array(
    'label' => t('After credit choice'),
    'variables' => array(
      'credit_type' => array('type' => 'text', 'label' => t('credit type'))
    ),
    'group' => t('Commerce Credit'),
    'module' => 'commerce_credits'
  );
  return $items;
}


