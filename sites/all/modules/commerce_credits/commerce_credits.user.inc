<?php

/**
 * @file
 * Contains functions for the user interface of commerce credits module.
 * 
 * This file contains function for building user interface and administering
 * user credits balance.   
 */


/**
 * User balance
 */
function commerce_credits_user_page() {
  global $user;
  return commerce_credits_user_balance($user->uid);

}
/**
 * Display user balance, not editable
 */
function commerce_credits_user_balance($uid) {
  $output = '';
  drupal_set_title(t('Your credits balance'));
  $entity = entity_metadata_wrapper('user', user_load($uid));
  $credits = commerce_credits_entity_credits($entity);
  if (empty($credits)) {
    $output = t('You do not have any credit');
  }
  else {
    // Count the number of credits
    $number = 0;
    foreach ($credits as $credit_number) {
      $number += $credit_number;
    }
    if ($number == 0) {
      $output = t('You do not have any credit');
    }
    else {
      $types = commerce_credits_types();
      $header = array(t('Name'), t('Description'), t('Number'));
      $rows = array();
      foreach ($credits as $credit_type => $credit_number) {
        $type = $types[$credit_type];
        $rows[] = array(
          $type->name,
          $type->description,
          $credit_number
        );
      }
      $output = '<h2>' . t('Credits balance') . '</h2>';
      $output.= theme('table', array('header' => $header, 'rows' => $rows ));
    }
  }
  
  return $output;
}

