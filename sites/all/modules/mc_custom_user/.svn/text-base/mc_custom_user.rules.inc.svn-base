<?php

/**
 * Implements hook_rules_action_info().
 *
 * @ingroup rules
 */
function mc_custom_user_rules_action_info() {
  $defaults = array(
   'parameter' => array(
      'account' => array(
        'type' => 'user',
        'label' => t('User'),
        'description' => t('The user whose domain should be set up.'),
        'save' => TRUE,
      ),
    ),
    'group' => t('User'),
  );
  $items['user_create_domain_action'] = $defaults + array(
    'label' => t('Set Up A domain'),
    'base' => 'user_create_domain_action',
  );
  $items['user_authenticate_action'] = $defaults + array(
    'label' => t('Activate Authentification'),
    'base' => 'user_authenticate_action',
  );
  return $items;
}

/**
 * Action: Adds roles to a particular user.
 */
function user_create_domain_action($account) {
	
  if ($account->uid || !empty($account->is_new)) {
  	
      // user_save() inserts roles after invoking hook_user_insert() anyway, so
      // we skip saving to avoid errors due saving them twice.

      // Domain
      $domain = domain_get_domain();
      $domain = $domain['subdomain'];
      if( ($x_pos = strpos($domain, '.')) !== FALSE )
	  $domain = substr($domain, $x_pos + 1);
	    
	  // Random SMS code
	  $code =  rand(111111, 999999);

	  // Number
	  $number = cck_phone_countrycodes($account->field_mobile['und'][0]['country_codes']);
	  $number = $number['code'];
	  $number .= $account->field_mobile['und'][0]['number'];
		  
	  // Send Activation SMS
	  $result = sms_clickatell_command('auth');
	  if ($result['status'] == TRUE) {
	  	$data['sid'] = $result['sid'];
		$data['number'] = $number;
		$data['message'] = 'MC Time Tracker Mobile Confirmation Code: '.$code;
		$sms = sms_clickatell_command('sendmsg', $data);
		drupal_set_message(t('A confirmation code has been sent to your mobile. Your will need this code in order to complete the activation process.'));
	  }
		  
	  // Create a new domain
	  $record = array(
	    'sitename' => $account->field_site_name['und'][0]['value'],
		'subdomain' => $account->field_domain['und'][0]['value'] . '.mctimetracker.com',
		'valid' => 1,
		'is_default' => 0,
		'weight' => 1,
		'scheme' => 'http',
	  );
	  $domain = domain_save($record, $record);
		  
	  // Add user to new domain
	  $edit['domain_user'][$domain['domain_id']] = array('status'=>1);
	  domain_user_save($edit, $account, '');
    	
	  // Save Activation Code
	  //Get the id of your field
	  $field_name = 'field_activation_code_hidden';
	  $info = field_info_field($field_name);
	  $fields = array($info['id']);
      // Fill in Entity
      $account->field_activation_code_hidden['und'][0]['value'] = $code;
  	  //Execute the storage function
      field_sql_storage_field_storage_write('user', $account, 'update', $fields);
      
	  drupal_set_message(t("Don't want to wait ? Just click on the following <a href='http://" . $domain['subdomain'] . "/user/login'>link to login.</a>"));
      
      return FALSE;
      
  }
  else {
  	
    return FALSE;
  }
  
}

/**
 * Action: Adds roles to a particular user.
 */
function user_authenticate_action($account) {
	
  if ($account->uid) {
  	
    // Domain
    $domain = domain_get_domain();
  	
    // Add 100 credits
	mc_campaign_insert_cost($domain['domain_id'], 100 , 'Free Credits');
	
    // Create personal agenda
	$node = new stdClass();
    $node->type = 'agenda';
    node_object_prepare($node);
    $node->title    = t('My calendar');
    $node->language = LANGUAGE_NONE;
    node_save($node);
    
    // Rebuild menu
    //menu_rebuild();
  	
    return FALSE;
      
  } else {
  	
    return FALSE;
  
  }
  
}
