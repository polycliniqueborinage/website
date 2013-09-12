<?php
/**
 * Menu callback; present an administrative subscriber listing.
 */
function mc_campaign_subscriber_list($form, &$form_state) {

  // Domain
  $domain = domain_get_domain();
	
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
    $output = t('Are you sure you want to delete the following newsletter subscribers?');
    $output .= '<ul>';
    foreach ($form_state['values']['subscriber'] as $sid) {
      $subscriber = newsletter_sid_load($sid);
      $output .= !empty($subscriber) ? '<li>' . check_plain($subscriber->email) . '</li>' : '';
    }
    $output .= '</ul>';
    $output .= t('This action cannot be undone.');
    return confirm_form($form, t('Delete the following?'), 'admin/config/system/newsletter/lists/subscribers', filter_xss($output));
  }
  
  // Action
  $lists = db_select('mc_campaign_lists','list')->fields('list')->execute()->fetchAll();
  foreach ($lists as $list) {
  	$options['add_'.$list->lid] = t('Add to list : ').$list->title;
  	$options['delete_'.$list->lid] = t('Delete from list : ').$list->title;
  }
  
  if (isset($options)) {
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
      '#default_value' => 'delete',
    );
    $form['options']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
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
    'created' => array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('mc_campaign_subscribers', 'subscribers')->extend('PagerDefault')->extend('TableSort');
  $query->fields('subscribers');
  $result = $query
    ->limit(50)
    ->orderByHeader($header)
    ->condition('did',$domain['domain_id'])
    ->execute();
  
  $result = $query
    ->fields('subscribers', array(
      'eid',
      'sid',
      'firstname',
      'lastname',
      'email',
      'mobile',
      'fixedline',
      'created',
    ))
    ->limit(30)
    ->orderByHeader($header)
    ->execute();

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
      '#href' => 'admin/campaign/lists/subscribers/view/' . $row->sid,
      '#options' => array('query' => $destination),
    );
    $operation[] = array(
      '#markup' => ' | ',
    );
    $operation[] = array(
      '#type' => 'link',
      '#title' => t('Edit'),
      '#href' => 'admin/campaign/lists/subscribers/edit/' . $row->sid,
      '#options' => array('query' => $destination),
    );
    if ($lists == '') {
      $operation[] = array(
        '#markup' => ' | ',
      );
      $operation[] = array(
        '#type' => 'link',
        '#title' => t('Delete'),
        '#href' => 'admin/campaign/lists/subscribers/deleted/' . $row->sid,
        '#options' => array('query' => $destination),
      );
    }

    $options[$row->sid] = array(
      'firstname' => l(check_plain($row->firstname),'admin/campaign/lists/subscribers/view/' . $row->sid),
      'lastname' => l(check_plain($row->lastname),'admin/campaign/lists/subscribers/view/' . $row->sid),
      'email' => check_plain($row->email),
      'mobile' => check_plain($row->mobile),
      'fixedline' => check_plain($row->fixedline),
      'list_title' => check_plain($lists),
      'created' => format_date($row->created, 'short'),
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


function mc_campaign_admin_list_submit($form, &$form_state) {
  // LIST TO REMOVE
  $lid = $form_state['values']['operation'];
  // Subriber
  $subscribers = $form_state['values']['subscriber'];
  if ($list) {
    switch ($operation) {
      case 'delete':
        if (!isset($form_state['storage']['confirm'])) {
          $form_state['storage']['confirm'] = TRUE;
          $form_state['rebuild'] = TRUE;
          return;
        }
        foreach ($list as $id) {
          $num_deleted = db_delete('newsletter_lists')
            ->condition('id', $id)
            ->execute();
          db_delete('newsletter_index_subscribers')
            ->condition('list_id', $id)
            ->execute();
        }
        break;
    }
    isset($num_deleted)
      ? drupal_set_message(t('Lists deleted successfully'), 'status')
      : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  }
  else {
    drupal_set_message(t('Please Select an action to perform.'), 'warning');
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
  );
  $form['global']['lastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
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
  );
  $form['global']['mobile'] = array(
    '#type' => 'textfield',
    '#title' => t('Mobile number'),
    '#description' => t('Format : +32078123123'),
  );
  $form['global']['fixedline'] = array(
    '#type' => 'textfield',
    '#title' => t('Fixed Landline number'),
    '#description' => t('Format : +3265454532'),
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
  );
  $form['connect']['skype'] = array(
    '#type' => 'textfield',
    '#title' => t('Skype'),
  );
  $form['connect']['twitter'] = array(
    '#type' => 'textfield',
    '#title' => t('Twitter'),
  );
  $form['connect']['facebook'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook'),
  );
  $form['additional'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Info'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['additional']['info1'] = array(
    '#type' => 'textfield',
    '#title' => t('Info 1'),
    '#description' => t('Example : Birthdate, Appointment Time'),
  );
  $form['additional']['info2'] = array(
    '#type' => 'textfield',
    '#title' => t('Info 2'),
    '#description' => t('Example : Birthdate, Appointment Time'),
  );
  $form['additional']['info3'] = array(
    '#type' => 'textfield',
    '#title' => t('Info 3'),
    '#description' => t('Example : Birthdate, Appointment Time'),
  );
  return $form;
}







/**
 * Menu callback; add subscribers.
 */
function mc_campaign_subscriber_add($form, &$form_state, $arg1 = NULL) {

  $lists = db_query("SELECT lid, title FROM {mc_campaign_lists} WHERE type = 'manual'")->fetchAllKeyed();

  $form = mc_campaign_subscriber_form($lists, $arg1);
  
  $form['new'] = array(
    '#markup' => '<h3>' . t('Add a list member') . '</h3>',
    '#weight' => -1,
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
	
  // Create a class	
  $subscriber = new stdClass();

  // Add match field
  $subscriber->eid = NULL;
  $subscriber->email = empty($form_state['input']['email']) ? NULL : $form_state['input']['email'];
  $subscriber->mobile = empty($form_state['input']['mobile']) ? NULL : $form_state['input']['mobile'];
  $subscriber->fixedline = NULL;
  $subscriber->firstname = NULL;
  $subscriber->im = NULL;
  $subscriber->skype = NULL;
  $subscriber->twitter = NULL;
  $subscriber->facebook = NULL;
  
  // Try to match any existing subscriber
  if (mc_campaign_is_subscribed($subscriber)) {
  	drupal_set_message(t('Subscriber already exist'), 'status');
  } else {
  	drupal_set_message(t('Subscriber does not exist'), 'status');
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
  
  $subscriber->sent_time = empty($form_state['input']['sent_time']) ? NULL : $form_state['input']['sent_time'];
  $subscriber->info1 = empty($form_state['input']['info1']) ? NULL : $form_state['input']['info1'];
  $subscriber->info2 = empty($form_state['input']['info2']) ? NULL : $form_state['input']['info2'];
  $subscriber->info3 = empty($form_state['input']['info3']) ? NULL : $form_state['input']['info3'];
  
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
      drupal_set_message(t('Subscriber already linked to this list'), 'status');
      // we save as value may have changed
  	  mc_campaign_update_subscriber_to_list($subscriber);
    } else {
  	  drupal_set_message(t('Subscriber not already linked to this list'), 'status');
  	  mc_campaign_save_subscriber_to_list($subscriber);
    }
  }
  
  //drupal_goto(drupal_get_destination());
  drupal_goto('account/subscribers');
  
}

































/**
 * Menu callbacks; add subscribers (mass).
 */
function mc_campaign_subscriber_add_mass($form, &$form_state, $arg1 = NULL) {

  $lists = db_query('SELECT lid, title FROM {mc_campaign_lists}')->fetchAllKeyed();
	
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
  $num_deleted = db_delete('mc_campaign_subscribers')
    ->condition('sid', $form_state['values']['sid'])
    ->execute();
  isset($num_deleted)
    ? drupal_set_message(t('Subscriber deleted successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
    
  drupal_goto(drupal_get_destination());
}


















/**
 * Menu callback; add subscribers.
 */
function mc_campaign_subscriber_edit($form, &$form_state, $arg1 = NULL) {
	
  $subscriber = db_query("SELECT * FROM {mc_campaign_subscribers} WHERE sid = $arg1")->fetchAll();
	
  $list = db_query("SELECT lid FROM {mc_campaign_lists_subscribers} WHERE sid = $arg1")->fetchAllKeyed();

  dpm($subscriber);
  dpm($list);
  
  $form = mc_campaign_subscriber_form($lists, $arg1);
  
  $form['new'] = array(
    '#markup' => '<h3>' . t('Add a list member') . '</h3>',
    '#weight' => -1,
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
  return $form;
}















function mc_campaign_subscriber_import_email_provider($form, &$form_state) {
	
  $lists['gmail'] = 'Gmail';
  $lists['hotmail'] = 'Live/Hotmail';
  $lists['aol'] = 'AOL';
  $lists['linkedin'] = 'LinkedIn';
  $lists['msn'] = 'MSN';
  $lists['yahoo'] = 'Yahoo!';

  //$lists['facebook'] = 'Facebook';
  
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
    //'#default_value' => 'tt2837082@gmail.com'
  );
  $form['email_provider']['password'] = array(
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

function mc_campaign_subscriber_import_email_provider_validate($form, $form_state) {
	
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
    //dpm($contact_list);
  }
    
}


function mc_campaign_subscriber_import_email_provider_submit($form, &$form_state) {
	
  //dpm($form_state);
  //drupal_goto(drupal_get_destination());
  //drupal_goto('account/subscribers');
  
}



















function mc_campaign_subscriber_import_social_network($form, &$form_state) {
	
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
    //dpm($contact_list);
  }
    
}


function mc_campaign_subscriber_import_social_network_submit($form, &$form_state) {
	
  //dpm($form_state);
  //drupal_goto(drupal_get_destination());
  //drupal_goto('account/subscribers');
  
}































function mc_campaign_subscriber_view($form, &$form_state, $arg1 = NULL) {
	
  // Add CSS	
  drupal_add_css(drupal_get_path('module', 'mc_campaign') . '/css/dashboard.css');
  
  drupal_add_library('system', 'ui.tabs');
  
  drupal_add_js('jQuery(document).ready(function(){
  jQuery("#tabs").tabs(); 
  });
  ', 'inline');
	
  // subscriber detail
  $subscriber = db_select('mc_campaign_subscribers','subscribers')
    ->condition('sid',$arg1)
    ->fields('subscribers')
    ->execute()
    ->fetchObject();
    
  //dpm($subscriber);  
    
  // subscribers detail
  $lists = db_select('mc_campaign_lists_subscribers','lists')
    ->condition('sid',$arg1)
    ->fields('lists')
    ->execute()
    ->fetchAll();

  //dpm($lists);  
    
  $build['dashboard'] = array(
    '#theme' => 'mc_campaign_subscriber_detail',
    '#fields' => $subscriber,
    '#lists' => $lists,
  );

  return $build;
}
