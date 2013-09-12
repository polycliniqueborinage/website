<?php
/**
 * Menu callback; present an administrative subscriber listing.
 */
function mc_campaign_subscriber_list($form, &$form_state) {

  // FROM GRAPHE	
  $lid = (int) arg(2);
  
  // Roles
  global $user;
  $roles = $user->roles;
      
  // Domain
  $domain = domain_get_domain();
  
  if (isset($roles[7])){
  	drupal_set_message(t('Hey buddy ! <a href="/user/me/edit">Activate your account</a> with the activation code you have received on your mobile.'),'error');
  }
	
  // Remove
  if (isset($form_state['storage']['confirm'])) {
    $form['operation'] = array(
      '#type' => 'hidden',
      '#value' => 'delete',
    );
    $form['subscriber'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['values']['subscriber'],
    );
    $output = t('Are you sure you want to delete the subscribers?');
    $output .= '<br />';
    $output .= t('This action cannot be undone!');
    return confirm_form($form, t('Delete the following?'), 'account/subscribers', $output);
  }
  
  // Action
  $lists = db_select('mc_campaign_lists','list')->condition('type','manual')->condition('did',$domain['domain_id'])->fields('list')->execute()->fetchAll();
  
  $options[''] = t('Select');
  $options['delete'] = t('Delete contact');
  foreach ($lists as $list) {
    $options['add_'.$list->lid] = t('Add to list: ') . $list->title;
    $options['remove_'.$list->lid] = t('Remove from list: ') . $list->title;
  }
  
  if (isset($options)) {
    $form['add']['add'] = array(
    '#markup' => l(t('Add contact'), 'account/subscribers/addsubscribers', array('attributes' => array('class' => 'button_left'))),
    );
  	$form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Options'),
      '#attributes' => array('class' => array('container-inline')),
    );
    $form['options']['operation'] = array(
      '#type' => 'select',
      '#title' => t('Operation'),
      '#title_display' => 'invisible',
      '#options' => $options,
      '#default_value' => '',
    );
    $form['options']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
    );
  } else {
    $form['add']['add'] = array(
    '#markup' => l(t('Create list'), 'account/lists/add', array('attributes' => array('class' => 'button_left'))),
    );
  	$form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Options'),
      '#attributes' => array('class' => array('container-inline')),
    );
  }
  
  // Table
  $header = array(
    'firstname' => array('data' => t('First Name'), 'field' => 'firstname'),
    'lastname' => array('data' => t('Last Name'), 'field' => 'lastname'),
    'email' => array('data' => t('Email'), 'field' => 'email'),
    'mobile' => array('data' => t('Mobile'), 'field' => 'mobile'),
    'fixedline' => array('data' => t('Fixed Line'), 'field' => 'fixedline'),
    'list_title' => array('data' => t('Subscribed to')),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('mc_campaign_subscribers', 's')->extend('PagerDefault')->extend('TableSort');
  $query->leftjoin('mc_campaign_lists_subscribers', 'ls', 'ls.sid = s.sid');
  $query->fields('s');
  $query = $query
    ->limit(50)
    ->orderByHeader($header)
    ->condition('did',$domain['domain_id']);
    
  if ($lid) {
  $query = $query->condition('lid',$lid);
  }
  
  $result = $query->execute();

  $options = array();
  $destination = drupal_get_destination();
  
  foreach ($result as $row) {
  	
    $lists = db_query('SELECT DISTINCT title
      FROM {mc_campaign_lists} lists
      JOIN {mc_campaign_lists_subscribers} lists_subscribers
      ON lists_subscribers.lid = lists.lid
      WHERE lists_subscribers.sid = :id',
      array(':id' => $row->sid ))
      ->fetchCol();
      
    $lists = implode(', ', $lists);
    
    $operation = array();
    $operation[] = array(
      '#type' => 'link',
      '#title' => t('View'),
      '#href' => 'account/subscribers/view/' . $row->sid,
      '#options' => array('query' => $destination),
    );
    $operation[] = array(
      '#markup' => ' | ',
    );
    $operation[] = array(
      '#type' => 'link',
      '#title' => t('Edit'),
      '#href' => 'account/subscribers/edit/' . $row->sid,
      '#options' => array('query' => $destination),
    );
    $operation[] = array(
      '#markup' => ' | ',
    );
    $operation[] = array(
      '#type' => 'link',
      '#title' => t('Delete'),
      '#href' => 'account/subscribers/delete/' . $row->sid,
      '#options' => array('query' => $destination),
    );

    $options[$row->sid] = array(
      'firstname' => l(check_plain($row->firstname),'account/subscribers/view/' . $row->sid),
      'lastname' => l(check_plain($row->lastname),'account/subscribers/view/' . $row->sid),
      'email' => check_plain($row->email),
      'mobile' => check_plain($row->mobile),
      'fixedline' => check_plain($row->fixedline),
      'list_title' => check_plain($lists),
      'operations' => array(
        'data' => $operation,
      ),
    );
  }

  $form['subscriber'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No subscribers yet.'),
  );

  $form['pager'] = array('#theme' => 'pager');
  return $form;
}


function mc_campaign_subscriber_list_submit($form, &$form_state) {
  
  $operation = $form_state['values']['operation'];
  $subscribers = $form_state['values']['subscriber'];
  
  $operation_array = explode('_',$operation);
  $operation = $operation_array[0];
  $operation_value = @$operation_array[1];

  // Domain
  $domain = domain_get_domain();
  
  switch ($operation) {
    case 'delete':
      $num_deleted = 0;
      foreach($subscribers as $key => $value) {
        if ($value) {
          dpm($value);
          $num_deleted += db_delete('mc_campaign_subscribers')
          ->condition('sid', $value)
          ->condition('did', $domain['domain_id'])
          ->execute();
        }
      }
      drupal_set_message(t('@num_deleted Subscriber deleted', array('@num_deleted' => $num_deleted)), 'status');
      break;
      
    case 'add':
      foreach($subscribers as $key => $value) {
        if ($value) {
          $subscriber = new stdClass();
          $subscriber->did = $domain['domain_id'];
          $subscriber->lid = $operation_value;
          $subscriber->sid = $key;
          if (mc_campaign_imc_campaign_is_subscribed_to_list($subscriber)) {
  	        drupal_set_message(t('Subscriber already linked to this list'), 'status');
  	      } else {
  	        drupal_set_message(t('Subscriber not already linked to this list'), 'status');
  	  	    mc_campaign_save_subscriber_to_list($subscriber);
  	  	  };
        }
      }
    break;
    case 'remove':
      foreach($subscribers as $key => $value) {
        if ($value) {
          $subscriber = new stdClass();
          $subscriber->did = $domain['domain_id'];
          $subscriber->lid = $operation_value;
          $subscriber->sid = $key;
          mc_campaign_save_unsubscriber_to_list($subscriber);
        }
      }
    break;
    default:
      drupal_set_message(t('Please select an action to perform.'), 'warning');
  }
  
}














/**
 * 
 */
function mc_campaign_subscriber_form($lists, $list = NULL, $subscriber = NULL) {
  $form['global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Info'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['global']['firstname'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => ($subscriber) ? $subscriber->firstname : '',
  );
  $form['global']['lastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => ($subscriber) ? $subscriber->lastname : '',
  );
  $form['global']['list'] = array(
    '#type' => 'select',
    '#title' => t('Select Lists'),
    '#options' => $lists,
    '#required' => TRUE,
    '#multiple' => TRUE,
    '#default_value' => $list,
  );
  $form['global']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Address'),
    '#default_value' => ($subscriber) ? $subscriber->email : '',
  );
  $form['global']['mobile'] = array(
    '#type' => 'textfield',
    '#title' => t('Mobile number'),
    '#description' => t('Do not forget the country prefix ! - Format : +32078123123'),
    '#default_value' => ($subscriber) ? $subscriber->mobile : '',
  );
  $form['global']['fixedline'] = array(
    '#type' => 'textfield',
    '#title' => t('Fixed Landline number'),
    '#description' => t('Do not forget the country prefix ! Format : +3265454532'),
    '#default_value' => ($subscriber) ? $subscriber->fixedline : '',
  );
  $form['connect'] = array(
    '#type' => 'fieldset',
    '#title' => t('Social connexion'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['connect']['im'] = array(
    '#type' => 'textfield',
    '#title' => t('IM'),
    '#default_value' => ($subscriber) ? $subscriber->im : '',
  );
  $form['connect']['skype'] = array(
    '#type' => 'textfield',
    '#title' => t('Skype'),
    '#default_value' => ($subscriber) ? $subscriber->skype : '',
  );
  $form['connect']['twitter'] = array(
    '#type' => 'twitter_username',
    '#instance_settings' => array('validate_existance' =>TRUE),
    '#title' => t('Twitter'),
    '#default_value' => ($subscriber) ? $subscriber->twitter : '',
  );
  $form['connect']['facebook'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook'),
    '#default_value' => ($subscriber) ? $subscriber->facebook : '',
  );
  $form['other'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['other']['birthday'] = array(
    '#type' => 'date_select',
    '#date_timezone' => date_default_timezone(),
    '#date_format' => 'd/m/Y',
    '#date_year_range' => '-120:+0',
    '#title' => t('Birthday'),
    '#default_value' => ($subscriber) ? $subscriber->birthday : '',
  );
  $form['permission'] = array(
    '#type' => 'fieldset',
    '#title' => t('Permissions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['permission']['email_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Email Allowed.'),
    '#default_value' => ($subscriber) ? $subscriber->email_allowed : 1,
  );
  $form['permission']['mobile_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Mobile Allowed.'),
    '#default_value' => ($subscriber) ? $subscriber->mobile_allowed : 1,
  );
  $form['permission']['fixedline_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fixedline Allowed.'),
    '#default_value' => ($subscriber) ? $subscriber->fixedline_allowed : 1,
  );
  return $form;
}





/**
 * Menu callback; add subscribers.
 */
function mc_campaign_subscriber_add($form, &$form_state, $lid = NULL) {

  global $language_content;
  global $user;
  
  // Roles
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('First you need to <a href="/user/me/edit">activate your account</a> with the activation code you have received on your mobile.'),'error');
  } else {
  
    $domain = domain_get_domain();
	
    $lists = db_query("SELECT lid, title FROM {mc_campaign_lists} WHERE did = :domain AND type = 'manual'", array(':domain'=>$domain['domain_id']))->fetchAllKeyed();

    $form = mc_campaign_subscriber_form($lists, $lid);
    
    // Get country code.
    $user = user_load($user->uid);
    $countrycodes = cck_phone_countrycodes($user->field_mobile[LANGUAGE_NONE][0]['country_codes']);
    $form['global']['mobile']['#default_value'] = $countrycodes['code'];
    $form['global']['fixedline']['#default_value'] = $countrycodes['code'];
  
    $form['submitnew'] = array(
      '#type' => 'submit',
      '#value' => t('Add'),
    );
    $form['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'account/subscribers',
    );
  
  }
  
  return $form;
}

function mc_campaign_subscriber_add_validate($form, $form_state) {
  $form_values = $form_state['values'];
  if ($form_values['email'] == '' && $form_values['mobile'] == '') {
    form_set_error('email', t('Enter at least the email address or mobile number'));
    form_set_error('mobile', t('Enter at least the email address or mobile number'));
  }
  if ($form_values['email'] != '' && !valid_email_address($form_values['email'])) {
    form_set_error('email', t('Enter a valid email address'));
  }
}

function mc_campaign_subscriber_add_submit($form, &$form_state) {
  
  // Domain
  $domain = domain_get_domain();
  
  // convert date
  if (!empty($form_state['input']['birthday']['date'])) {
    list($day, $month, $year) = explode('/', $form_state['input']['birthday']['date']);
    $birthday = strtotime($year . "-" . $month . "-" . $day);
  }

  // Create a class	
  $subscriber = new stdClass();

  // Add match field
  $subscriber->eid = NULL;
  $subscriber->email = empty($form_state['input']['email']) ? NULL : $form_state['input']['email'];
  $subscriber->mobile = empty($form_state['input']['mobile']) ? NULL : $form_state['input']['mobile'];
  $subscriber->did = $domain['domain_id'];
  $subscriber->fixedline = NULL;
  $subscriber->firstname = NULL;
  $subscriber->im = NULL;
  $subscriber->skype = NULL;
  $subscriber->twitter = NULL;
  $subscriber->facebook = NULL;
  $subscriber->birthday = NULL;
  
  // Try to match any existing subscriber
  if (mc_campaign_is_subscribed($subscriber)) {
  	drupal_set_message(t('Subscriber update'), 'status');
  } else {
  	drupal_set_message(t('Subscriber added'), 'status');
  }
  
  $subscriber->firstname = empty($form_state['input']['firstname']) ? $subscriber->firstname : $form_state['input']['firstname'];
  $subscriber->lastname = empty($form_state['input']['lastname']) ? $subscriber->lastname : $form_state['input']['lastname'];
  $subscriber->email = empty($form_state['input']['email']) ? $subscriber->email : $form_state['input']['email'];
  $subscriber->mobile = empty($form_state['input']['mobile']) ? $subscriber->mobile : $form_state['input']['mobile'];
  $subscriber->fixedline = empty($form_state['input']['fixedline']) ? $subscriber->fixedline : $form_state['input']['fixedline'];

  $subscriber->im = empty($form_state['input']['im']) ? $subscriber->im : $form_state['input']['im'];
  $subscriber->skype = empty($form_state['input']['skype']) ? $subscriber->skype : $form_state['input']['skype'];
  $subscriber->facebook = empty($form_state['input']['facebook']) ? $subscriber->facebook : $form_state['input']['facebook'];
  $subscriber->twitter = empty($form_state['input']['twitter']) ? $subscriber->skype : $form_state['input']['twitter'];
  
  $subscriber->birthday = empty($form_state['input']['birthday']['date']) ? $subscriber->birthday : $birthday;
  
  $subscriber->sent_time = empty($form_state['input']['sent_time']) ? NULL : $form_state['input']['sent_time'];
  $subscriber->info1 = empty($form_state['input']['info1']) ? NULL : $form_state['input']['info1'];
  $subscriber->info2 = empty($form_state['input']['info2']) ? NULL : $form_state['input']['info2'];
  $subscriber->info3 = empty($form_state['input']['info3']) ? NULL : $form_state['input']['info3'];

  $subscriber->email_allowed = empty($form_state['input']['email_allowed']) ? 0 : 1;
  $subscriber->mobile_allowed = empty($form_state['input']['mobile_allowed']) ? 0 : 1;
  $subscriber->fixedline_allowed = empty($form_state['input']['fixedline_allowed']) ? 0 : 1;
  
  // Clean
  $subscriber->email = valid_email_address($subscriber->email) ? $subscriber->email : "";
  $subscriber->mobile = mc_clean_mobile_number($subscriber->mobile);
  $subscriber->fixedline = mc_clean_fixedline_number($subscriber->fixedline);
  
  // Save or Update
  if (isset($subscriber->sid)) {
    mc_campaign_update_subscriber($subscriber);
  } else {
  	mc_campaign_save_subscriber($subscriber);
  }
  
  foreach ($form_state['values']['list'] as $lid) {
  	$subscriber->lid = $lid;
    // CHECK IF SUBSCRIBER ALREADY LINKED TO LISTS
    if (mc_campaign_imc_campaign_is_subscribed_to_list($subscriber)) {
      //drupal_set_message(t('Subscriber already linked to this list'), 'status');
      // we save as value may have changed
  	  mc_campaign_update_subscriber_to_list($subscriber);
    } else {
  	  //drupal_set_message(t('Subscriber not already linked to this list'), 'status');
  	  mc_campaign_save_subscriber_to_list($subscriber);
    }
  }
  
  drupal_goto('account/subscribers');
  
}

































/**
 * Menu callbacks; add subscribers (mass).
 */
function mc_campaign_subscriber_add_mass($form, &$form_state, $arg1 = NULL) {

  global $language_content;
  global $user;
  
  // Roles
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('First you need to <a href="/user/me/edit">activate your account</a> with the activation code you have received on your mobile.'),'error');
  } else {
  
    $domain = domain_get_domain();
	
    $lists = db_query("SELECT lid, title FROM {mc_campaign_lists} WHERE did = :domain AND type = 'manual'", array(':domain'=>$domain['domain_id']))->fetchAllKeyed();
    		
    $form['new'] = array(
      '#markup' => '<h3>' . t('Add Subscribers') . '</h3>',
    );
    $form['subscribers'] = array(
      '#type' => 'textarea',
      '#title' => t('Import'),
      '#required' => TRUE,
      '#description' => t('Enter one subscriber per line.To add additional info, use the following format using 0 or FALSE to omit a value: %format', array('%format' => 'email;mobile;firstname;lastname;info1;info2;info3'))."<br/>calcus.david@gmail.com;0478000000;david;calcus;inf1;inf2;inf3",
    );
    $form['list'] = array(
      '#type' => 'select',
      '#title' => t('Select Lists'),
      '#options' => array_filter($lists, 'check_plain'),
      '#required' => TRUE,
      '#multiple' => TRUE,
      '#value' => $arg1,
    );
    $form['submitnew'] = array(
      '#type' => 'submit',
      '#value' => t('Add'),
    );
    $form['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/campaign/list/subscribers',
    );
    
  }
   
  return $form;

}


function mc_campaign_subscriber_add_mass_submit($form, &$form_state) {
  $entries=$form_state['input']['subscribers'];
  $entries = explode("\n", $entries);
  foreach ($entries as $entry) {
  	
    $subscriber = new stdClass();
    $entry = explode(';', $entry);
    $subscriber->email = (isset($entry[0]) && !empty($entry[0])) ? $entry[0] : NULL;
    $subscriber->mobile = (isset($entry[1]) && !empty($entry[1])) ? $entry[1] : NULL;
    $subscriber->firstname = (isset($entry[2]) && !empty($entry[2])) ? $entry[2] : NULL;
    $subscriber->lastname = (isset($entry[3]) && !empty($entry[3])) ? $entry[3] : NULL;
    $subscriber->info1 =  (isset($entry[4]) && !empty($entry[4])) ? $entry[4] : NULL;
    $subscriber->info2 = (isset($entry[5]) && !empty($entry[5])) ? $entry[5] : NULL;
    $subscriber->info3 = (isset($entry[6]) && !empty($entry[6])) ? $entry[6] : NULL;
    
    // CHECK IF SUBSCRIBER EXIST ALREADY
    if (mc_campaign_is_subscribed($subscriber)) {
  	  drupal_set_message(t('Subscriber already exist'), 'status');
    } else {
  	  mc_campaign_save_subscriber($subscriber);
  	  drupal_set_message(t('Subscriber created successfully'), 'status');
    }
  
    // CHECK IF SUBSCRIBER ALREADY LINKED TO LISTS
    foreach($form_state['input']['list'] as $lid) {
  	  if (mc_campaign_imc_campaign_is_subscribed_to_list($lid,$subscriber->sid)) {
   	   drupal_set_message(t('Subscriber already linked to this list'), 'status');
   	  } else {
  	    mc_campaign_save_subscriber_to_list($lid, $subscriber);	
  	    drupal_set_message(t('Subscriber not already linked to this list'), 'status');
  	  };
    }
    
  }

}















/**
 * Menu callback; delete a subscribers list.
 */
function mc_campaign_subscriber_delete($form, &$form_state, $sid) {
  $form['sid'] = array(
    '#type' => 'hidden',
    '#value' => $sid,
  );
  $output = t('Are you sure you want to delete the subscriber ? This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), drupal_get_destination(), filter_xss($output));
}
function mc_campaign_subscriber_delete_submit($form, &$form_state) {
  $domain = domain_get_domain();
  
  $num_deleted = db_delete('mc_campaign_subscribers')
    ->condition('sid', $form_state['values']['sid'])
    ->condition('did', $domain['domain_id'])
    ->execute();
    
  isset($num_deleted)
    ? drupal_set_message(t('Subscriber deleted successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
    
  drupal_goto(drupal_get_destination());
}


















/**
 * Menu callback; add subscribers.
 */
function mc_campaign_subscriber_edit($form, &$form_state, $sid = NULL) {
  
  global $user;
	
  $domain = domain_get_domain();
	
  $subscriber = db_query("SELECT * FROM {mc_campaign_subscribers} WHERE sid = :sid",array(':sid'=>$sid))->fetchObject();
	
  $lists = db_query("SELECT lid, title FROM {mc_campaign_lists} WHERE did = :domain AND type = 'manual'",array(':domain'=>$domain['domain_id']))->fetchAllKeyed();
  
  $list = db_query("SELECT lid, lid FROM {mc_campaign_lists_subscribers} WHERE sid = :sid",array(':sid'=>$sid))->fetchAllKeyed();
  
  $form = mc_campaign_subscriber_form($lists, $list, $subscriber);
  
  // Get country code.
  $user = user_load($user->uid);
  $countrycodes = cck_phone_countrycodes($user->field_mobile[LANGUAGE_NONE][0]['country_codes']);
  if($form['global']['mobile']['#default_value'] == '') $form['global']['mobile']['#default_value'] =  $countrycodes['code'];
  if($form['global']['fixedline']['#default_value'] == '') $form['global']['fixedline']['#default_value'] = $countrycodes['code'];
  
  $form['sid'] = array(
    '#type' => 'hidden',
    '#default_value' => ($subscriber) ? $subscriber->sid : '',
  );
  $form['new'] = array(
    '#markup' => '<h3>' . t('Edit a list member') . '</h3>',
    '#weight' => -1,
  );
  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'account/subscribers',
  );
  return $form;
}

function mc_campaign_subscriber_edit_submit($form, &$form_state) {
	
  $subscriber = new stdClass();

  $subscriber->sid = $form_state['values']['sid'];
  $subscriber->firstname = $form_state['values']['firstname'];
  $subscriber->lastname = $form_state['values']['lastname'];
  $subscriber->email = $form_state['values']['email'];
  $subscriber->mobile = $form_state['values']['mobile'];
  $subscriber->fixedline = $form_state['values']['fixedline'];
  
  $subscriber->im = $form_state['values']['im'];
  $subscriber->skype = $form_state['values']['skype'];
  $subscriber->facebook = $form_state['values']['facebook'];
  $subscriber->twitter = $form_state['values']['twitter'];
  
  $subscriber->email_allowed = empty($form_state['input']['email_allowed']) ? 0 : 1;
  $subscriber->mobile_allowed = empty($form_state['input']['mobile_allowed']) ? 0 : 1;
  $subscriber->fixedline_allowed = empty($form_state['input']['fixedline_allowed']) ? 0 : 1;
  
  // Clean
  $subscriber->email = valid_email_address($subscriber->email) ? $subscriber->email : "";
  $subscriber->mobile = mc_clean_mobile_number($subscriber->mobile);
  $subscriber->fixedline = mc_clean_fixedline_number($subscriber->fixedline);
  
  // Domain
  $domain = domain_get_domain();
  $subscriber->did = $domain['domain_id'];
  
  mc_campaign_update_subscriber($subscriber);
  
  drupal_goto(drupal_get_destination());
  
}





function _mc_campaign_subscriber_import_email_provider_steps() {
  return array(
      1 => array(
        'form' => 'mc_campaign_subscriber_import_email_provider_step1',
      ),
      2 => array(
        'form' => 'mc_campaign_subscriber_import_email_provider_step2',
      ),
    );
}

function mc_campaign_subscriber_import_email_provider_previous_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
  if ($current_step > 1) {
    $current_step--;
    $form_state['values'] = $form_state['step_information'][$current_step]['stored_values'];
  }
  $form_state['rebuild'] = TRUE;
}

function mc_campaign_subscriber_import_email_provider_next_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];

  if ($current_step < count($form_state['step_information'])) {
    $current_step++;
    if (!empty($form_state['step_information'][$current_step]['stored_values'])) {
      $form_state['values'] = $form_state['step_information'][$current_step]['stored_values'];
    }
    else {
      $form_state['values'] = array();
    }
    $form_state['rebuild'] = TRUE;  // Force rebuild with next step.
    return;
  }
}

function mc_campaign_subscriber_import_email_provider($form, &$form_state) {
  
  global $user;
  
  // Roles
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('First you need to <a href="/user/me/edit">activate your account</a> with the activation code you have received on your mobile.'),'error');
    return NULL;
  }
  
  // Initialize a description of the steps for the wizard.
  if (empty($form_state['step'])) {
    $form_state['step'] = 1;

    // This array contains the function to be called at each step to get the
    // relevant form elements. It will also store state information for each
    // step.
    $form_state['step_information'] = _mc_campaign_subscriber_import_email_provider_steps();
  }
  $step = &$form_state['step'];
  
  // Call the function named in $form_state['step_information'] to get the
  // form elements to display for this step.
  $form = $form_state['step_information'][$step]['form']($form, $form_state);

  // Show the 'previous' button if appropriate. Note that #submit is set to
  // a special submit handler, and that we use #limit_validation_errors to
  // skip all complaints about validation when using the back button. The
  // values entered will be discarded, but they will not be validated, which
  // would be annoying in a "back" button.
  if ($step > 1) {
    $form['prev'] = array(
      '#type' => 'submit',
      '#value' => t('Previous'),
      '#name' => 'prev',
      '#submit' => array('mc_campaign_subscriber_import_email_provider_previous_submit'),
      '#limit_validation_errors' => array(),
    );
  }

  // Show the Next button only if there are more steps defined.
  if ($step < count($form_state['step_information'])) {
    // The Next button should be included on every step
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
      '#name' => 'next',
      '#submit' => array('mc_campaign_subscriber_import_email_provider_next_submit'),
    );
    $form['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'account/subscribers',
    );    
  }
  else {
    // Just in case there are no more steps, we use the default submit handler
    // of the form wizard. Call this button Finish, Submit, or whatever you
    // want to show. When this button is clicked, the
    // form_example_wizard_submit handler will be called.
    $form['finish'] = array(
      '#type' => 'submit',
      '#value' => t('Import'),
    );
  }

  // Include each validation function defined for the different steps.
  if (function_exists($form_state['step_information'][$step]['form'] . '_validate')) {
    $form['next']['#validate'] = array($form_state['step_information'][$step]['form'] . '_validate');
  }

  return $form;
  
}


function mc_campaign_subscriber_import_email_provider_step1($form, &$form_state) {
	
  $lists['gmail'] = 'Gmail';
  $lists['hotmail'] = 'Live/Hotmail';
  $lists['aol'] = 'AOL';
  $lists['linkedin'] = 'LinkedIn';
  $lists['msn'] = 'MSN';
  $lists['yahoo'] = 'Yahoo!';
  
  $form['email_provider'] = array(
      '#type' => 'fieldset',
      '#title' => t('Email Provider'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
  );
  $form['email_provider']['list'] = array(
      '#type' => 'select',
      '#title' => t('Select Lists'),
      '#options' => $lists,
      '#required' => TRUE,
    );
    $form['email_provider']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Your email address'),
      '#size' => 25,
      '#attributes' => array('autocomplete' => 'off'),
      '#required' => TRUE,
      '#default_value' => '',
    );
    $form['email_provider']['password'] = array(
      '#type' => 'password', //password
      '#title' => t('Password'),
      '#size' => 25,
      '#description' => t('Will not be saved.'),
      '#required' => TRUE,
      '#attributes' => array('autocomplete' => 'off'),
    );
  
  return $form;
  
}

function mc_campaign_subscriber_import_email_provider_step1_validate($form, &$form_state) {
	
  if (!$form_state['values']['email'] || !valid_email_address($form_state['values']['email'])) {
    form_set_error('email', t('The email address %email is not valid.', array('%email' => $form_state['values']['email'])));
    return;
  }
  
  if (isset($form_state['values']['list']) && isset($form_state['values']['email']) && isset($form_state['values']['password'])) {
    $inviter = _openinviter_engine_authenticate(
      $errors,
      $form_state['values']['list'],
      $form_state['values']['email'],
      $form_state['values']['password']
    );
  
    if (!empty($errors)) {
      $success = FALSE;
      foreach ($errors as $error) {
        form_set_error('email', $error);
      }
    } else {
      $success = TRUE;
      $form_state['contact_list'] = _openinviter_engine_contacts_get($inviter);
    }
  }
    
}


function mc_campaign_subscriber_import_email_provider_step2($form, &$form_state) {

  // Domain
  $domain = domain_get_domain();
  
  $lists = db_query("SELECT lid, title FROM {mc_campaign_lists} WHERE did = :domain AND type = 'manual'", array(':domain'=>$domain['domain_id']))->fetchAllKeyed();
  		
  $form['list'] = array(
    '#type' => 'select',
    '#title' => t('Select Lists'),
    '#options' => array_filter($lists, 'check_plain'),
    '#required' => TRUE,
    '#multiple' => TRUE,
  );
  
  $header = array(
    'email' => array('data' => t('Mail'), 'field' => 'email', 'sort' => 'desc'),
    'name' => array('data' => t('Name')),
  );
  
  $form['contact_list_check'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $form_state['contact_list'],
    // BUG : need to disable all select as it will select the checkbox inside the table
    '#js_select' => TRUE,
    '#empty' => t('No lists available.'),
  );

  $form['pager'] = array('#theme' => 'pager');  
  
  return $form;
  
}

function mc_campaign_subscriber_import_email_provider_validate($form, &$form_state) {
  
  
}



function mc_campaign_subscriber_import_email_provider_submit($form, &$form_state) {
	
  $contact_list_check = $form_state['values']['contact_list_check'];
  $contact_list = $form_state['contact_list'];
  // Domain
  $domain = domain_get_domain();
  
  foreach ($contact_list_check as $key => $value) {
    
    if ($value != 0) {
      $contact = $contact_list[$key];
      
      $subscriber = new stdClass();

      $subscriber->eid = NULL;
      $subscriber->email = $contact['email'];
      $subscriber->mobile = NULL;
      $subscriber->fixedline = NULL;
      $subscriber->firstname = $contact['name'];
      $subscriber->im = NULL;
      $subscriber->skype = NULL;
      $subscriber->twitter = NULL;
      $subscriber->facebook = NULL;
  
      $subscriber->did = $domain['domain_id'];
      
      // CHECK IF SUBSCRIBER EXIST ALREADY
      // DO NOT UPDATE AS VALUE IN MUST BE MORE ACCURATE
      if (mc_campaign_is_subscribed($subscriber)) {
        drupal_set_message(t('Subscriber already exist'), 'status');
      } else {
        mc_campaign_save_subscriber($subscriber);
    	drupal_set_message(t('Subscriber created successfully'), 'status');
      }
    
      // CHECK IF SUBSCRIBER ALREADY LINKED TO LISTS
      foreach($form_state['values']['list'] as $lid) {
        $subscriber->lid = $lid;
   	    if (mc_campaign_imc_campaign_is_subscribed_to_list($subscriber)) {
     	  drupal_set_message(t('Subscriber already linked to this list'), 'status');
     	} else {
    	  mc_campaign_save_subscriber_to_list($subscriber);	
    	  drupal_set_message(t('Subscriber not already linked to this list'), 'status');
    	};
      }
    
    }
    
  }
  
  drupal_goto('account/subscribers');
  
}



















function mc_campaign_subscriber_import_social_network($form, &$form_state) {
	
  global $user;
  
  // Roles
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('First you need to <a href="/user/me/edit">activate your account</a> with the activation code you have received on your mobile.'),'error');
    return NULL;
  }
  
  $lists['facebook'] = 'Facebook';
  $lists['twitter'] = 'Twitter';
  $lists['myspace'] = 'MySpace';
  
  $form['social_network'] = array(
    '#type' => 'fieldset',
    '#title' => t('Social Network'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['social_network']['list'] = array(
    '#type' => 'select',
    '#title' => t('Select Lists'),
    '#options' => $lists,
    '#required' => TRUE,
  );
  $form['social_network']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Your email address'),
    '#size' => 25,
    '#attributes' => array('autocomplete' => 'off'),
    '#required' => TRUE,
    //'#default_value' => 'tt2837082@gmail.com'
  );
  $form['social_network']['password'] = array(
    '#type' => 'password', //password
    '#title' => t('Password'),
    '#size' => 25,
    '#description' => t('Will not be saved.'),
    '#required' => TRUE,
    '#attributes' => array('autocomplete' => 'off'),
    //'#default_value' => 'azerty2007-89'
  );
  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'account/subscribers',
  );
  
  return $form;
  
}

function mc_campaign_subscriber_import_social_network_validate($form, $form_state) {
	
  if (!$form_state['values']['email'] || !valid_email_address($form_state['values']['email'])) {
    form_set_error('email', t('The email address %email is not valid.', array('%email' => $form_state['values']['email'])));
    return;
  }
  
  $errors = array();
  
  $inviter = _openinviter_engine_authenticate(
    $errors,
    $form_state['values']['list'],
    $form_state['values']['email'],
    $form_state['values']['password']
  );
  
  if (!empty($errors)) {
    $success = FALSE;
    foreach ($errors as $error) {
      form_set_error('email', $error);
    }
  }
  else {
    $success = TRUE;
    $contact_list = _openinviter_engine_contacts_get($inviter);
    dpm($contact_list);
  }
    
}


function mc_campaign_subscriber_import_social_network_submit($form, &$form_state) {
	
  //dpm($form_state);
  //drupal_goto(drupal_get_destination());
  //drupal_goto('account/subscribers');
  
}































function mc_campaign_subscriber_view($form, &$form_state, $sid = NULL) {
	
  // Add CSS	
  drupal_add_css(drupal_get_path('module', 'mc_campaign') . '/css/dashboard.css');
  
  $sid = (int) $sid;

  // Domain
  $domain = domain_get_domain();
  
  // subscriber detail
  $subscriber = db_select('mc_campaign_subscribers','subscribers')
    ->condition('sid',$sid)
    ->condition('did',$domain['domain_id'])
    ->fields('subscribers')
    ->execute()
    ->fetchObject();
    
  // subscribers detail
  $lists = db_select('mc_campaign_lists_subscribers','lists')
    ->condition('sid',$sid)
    ->fields('lists')
    ->execute()
    ->fetchAll();

  $build['dashboard'] = array(
    '#theme' => 'mc_campaign_subscriber_detail',
    '#fields' => $subscriber,
    '#lists' => $lists,
  );

  return $build;
}
