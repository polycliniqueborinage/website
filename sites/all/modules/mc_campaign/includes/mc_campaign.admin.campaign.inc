<?php

/**
 * Render a list of entries in the database.
 */
function mc_campaign_campaign_list() {
  
  // Domain
  $domain = domain_get_domain();
	
  // Action
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options['enable'] = t('Enable the selected campaigns');
  $options['disable'] = t('Disable the selected campaigns');
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
	
  $header = array(
    'cid' => array('data' => 'ID', 'field' => 'cid'),
    'title' => array('data' => 'Name', 'field' => 'title'),
	'type' => array('data' => 'Type', 'field' => 'type'),
    'media' => array('data' => 'Media', 'field' => 'media'),
	'start_date' => array('data' => 'Start Date', 'field' => 'start_date'),
	'end_date' => array('data' => 'End Date', 'field' => 'end_date'),
    'start_time' => array('data' => 'Start Time', 'field' => 'start_time'),
	'end_time' => array('data' => 'End Time', 'field' => 'end_time'),
	'frequency' => array('data' => 'Frequence', 'field' => 'frequency'),
    'last_execution' => array('data' => 'Last Execution Time', 'field' => 'last_execution'),
    'next_execution' => array('data' => 'Next Execution Time', 'field' => 'next_execution'),
    'active' => array('data' => 'Active', 'field' => 'active','sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('mc_campaign_campaigns', 'campaigns')->extend('PagerDefault')->extend('TableSort');
  $query->fields('campaigns');
  $result = $query
    ->limit(50)
    ->orderByHeader($header)
    ->condition('did',$domain['domain_id'])
    ->execute();
    
  $options = array();
  $destination = drupal_get_destination();
  foreach ($result as $row) {
  	$active['element']['#checked'] = $row->active;
    $active['element']['#attributes']['disabled'] = 'disabled';
    $options[$row->cid] = array(
      'cid' => (int) $row->cid,
      'title' => check_plain($row->title),
      'type' => check_plain($row->type),
      'media' => check_plain($row->media),
      'start_date' => ($row->start_date) ? substr(format_date($row->start_date, 'short'),0,10) : t('N.A.'),
      'end_date' => ($row->start_date) ? substr(format_date($row->end_date, 'short'),0,10) : t('N.A.'),
      'start_time' => ($row->start_time) ? substr(format_date($row->start_time-3600, 'short'),12) : t('N.A.'),
      'end_time' => ($row->end_time) ? substr(format_date($row->end_time-3600, 'short'),12) : t('N.A.'),
      'frequency' => ($row->frequency) ? check_plain($row->frequency/60). ' ' .t('Min') : t('N.A.'),
      'last_execution' => ($row->last_execution) ? format_date($row->last_execution, 'short') : t('N.A.'),
      'next_execution' => ($row->next_execution) ? format_date($row->next_execution, 'short') : t('N.A.'),
      'active' => theme_checkbox($active),
      'created' => format_date($row->created, 'short'),
      'updated' => format_date($row->updated, 'short'),
      'operations' => array(
        'data' => array(
          array(
            '#type' => 'link',
            '#title' => t('View'),
            '#href' => 'admin/campaign/campaign/view/' . $row->cid ,
            '#options' => array('query' => $destination),
          ),
          array(
            '#markup' => ' | ',
          ),
          array(
            '#type' => 'link',
            '#title' => t('Edit'),
            '#href' => 'admin/campaign/campaign/edit/' . $row->cid ,
            '#options' => array('query' => $destination),
          ),
          array(
            '#markup' => ' | ',
          ),
          array(
            '#type' => 'link',
            '#title' => t('Delete'),
            '#href' => 'admin/campaign/campaign/delete/' . $row->cid,
            '#options' => array('query' => $destination),
          ),
        ),
      ),
    );
  }

  $form['campaign'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No templates available.'),
  	'#js_select' => FALSE,
  );

  $form['pager'] = array('#theme' => 'pager');

  return $form;

}

function mc_campaign_campaign_list_submit($form, &$form_state) {
  $operation = $form_state['values']['operation'];
  $campaign = $form_state['values']['campaign'];
  if ($campaign) {
    switch ($operation) {
      case 'enable':
        foreach ($campaign as $cid) {
          $num_updated = db_update('mc_campaign_campaigns')
            ->fields(array('active' => 1))
            ->condition('cid', $cid)
            ->execute();
        }
        break;
      case 'disable':
        foreach ($campaign as $cid) {
          $num_updated = db_update('mc_campaign_campaigns')
            ->fields(array('active' => 0))
            ->condition('cid', $cid)
            ->execute();
        }
        break;
    }
    isset($num_updated)
      ? drupal_set_message(t('Campaign updated successfully'), 'status')
      : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  }
  else {
    drupal_set_message(t('Please Select an action to perform.'), 'warning');
  }
}




































function mc_campaign_campaign_add($form, &$form_state) {

  // Domain
  $domain = domain_get_domain();
	
  drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/mc_loadtemplate.js');
	
  // Initialize a description of the steps.
  if (empty($form_state['step'])) {
    $form_state['step'] = 1;
    $form_state['step_information'] = _mc_campaign_campaign_add();
  }
  $step = &$form_state['step'];
  
  // Call the function named in $form_state['step_information'] to get the
  // form elements to display for this step.
  $form = $form_state['step_information'][$step]['form']($form, $form_state);

  if ($step > 1) {
    $form['prev'] = array(
      '#type' => 'submit',
      '#value' => t('Previous'),
      '#name' => 'prev',
      '#submit' => array('mc_campaign_campaign_add_previous_submit'),
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
      '#submit' => array('mc_campaign_campaign_add_next_submit'),
    );
  }
  
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/campaign/campaign',
    '#weight' => 10,
  );
  

  // Include each validation function defined for the different steps.
  if (function_exists($form_state['step_information'][$step]['form'] . '_validate')) {
    $form['next']['#validate'] = array($form_state['step_information'][$step]['form'] . '_validate');
  }
  
  return $form;
}

function mc_campaign_campaign_edit($form, &$form_state, $cid) {
  
  // Domain
  $domain = domain_get_domain();
	
  drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/mc_loadtemplate.js');
	
  $campaign = db_select('mc_campaign_campaigns','campaigns')
    ->condition('cid',$cid)
    ->fields('campaigns')
    ->execute()
    ->fetchObject();

  $lists = db_select('mc_campaign_campaigns_lists','lists')
    ->condition('cid',$cid)
    ->fields('lists', array('lid'))
    ->execute()
    ->fetchCol();
    
  // Initialize a description of the steps.
  if (empty($form_state['step'])) {
    $form_state['step'] = 1;
    $form_state['step_information'] = _mc_campaign_campaign_add();
    
    // ADD DEFAULT VALUE
    $form_state['step_information'][0]['cid'] = $cid;
  
    // ADD ONLY FIRST TIME
    $form_state['step_information'][1]['stored_values']['title'] = $campaign->title;
    $form_state['step_information'][1]['stored_values']['lists'] = $lists;
    $form_state['step_information'][1]['stored_values']['media'] = $campaign->media;
    $form_state['step_information'][1]['stored_values']['type'] = $campaign->type;

    $form_state['step_information'][1]['stored_values']['frequency'] = $campaign->frequency;
    
    $form_state['step_information'][1]['stored_values']['start_date'] = ($campaign->start_date) ? date('Y-m-d H:i:s', $campaign->start_date) : null;
    $form_state['step_information'][1]['stored_values']['end_date'] = ($campaign->end_date) ? date('Y-m-d H:i:s', $campaign->end_date) : null;
    $form_state['step_information'][1]['stored_values']['start_time'] = ($campaign->start_time) ? $campaign->start_time : null;
    $form_state['step_information'][1]['stored_values']['end_time'] = ($campaign->end_time) ? $campaign->end_time : null;
    $form_state['step_information'][1]['stored_values']['weekday'] = ($campaign->weekday) ? explode(",",$campaign->weekday) : null;
    
    $form_state['step_information'][2]['stored_values']['mtid'] = null;
    $form_state['step_information'][3]['stored_values']['stid'] = null;
    //$form_state['step_information'][3]['stored_values']['ltid'] = null;
    
    $form_state['step_information'][2]['stored_values']['mail_suject'] = ($campaign->mail_suject) ? $campaign->mail_suject: null;
    $form_state['step_information'][2]['stored_values']['mail_from_name'] = ($campaign->mail_from_name) ? $campaign->mail_from_name: null;
    $form_state['step_information'][2]['stored_values']['mail_reply_to'] = ($campaign->mail_reply_to) ? $campaign->mail_reply_to: null;
    $form_state['step_information'][2]['stored_values']['mail_body']['value'] = ($campaign->mail_body) ? $campaign->mail_body: null;
    $form_state['step_information'][2]['stored_values']['mail_body']['format'] = 'filtered_html';

    $form_state['step_information'][3]['stored_values']['body'] = ($campaign->sms_body) ? $campaign->sms_body: null;
    
    $form_state['step_information'][4]['stored_values']['active'] = $campaign->active;
    
  }
  $step = &$form_state['step'];
  
  // Call the function named in $form_state['step_information'] to get the
  // form elements to display for this step.
  $form = $form_state['step_information'][$step]['form']($form, $form_state);

  if ($step > 1) {
    $form['prev'] = array(
      '#type' => 'submit',
      '#value' => t('Previous'),
      '#name' => 'prev',
      '#submit' => array('mc_campaign_campaign_add_previous_submit'),
      '#limit_validation_errors' => array(),
    );
  }
  
  // Show the Next button only if there are more steps defined.
  if ($step < count($form_state['step_information']) - 1) {
    // The Next button should be included on every step
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
      '#name' => 'next',
      '#submit' => array('mc_campaign_campaign_add_next_submit'),
    );
  }
  
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/campaign/campaign',
    '#weight' => 10,
  );

  // Include each validation function defined for the different steps.
  if (function_exists($form_state['step_information'][$step]['form'] . '_validate')) {
    $form['next']['#validate'] = array($form_state['step_information'][$step]['form'] . '_validate');
  }

  return $form;
}

function _mc_campaign_campaign_add() {
  return array(
      1 => array(
        'form' => 'mc_campaign_campaign_add_global_info',
      ),
      2 => array(
        'form' => 'mc_campaign_campaign_add_mail_info',
      ),
      3 => array(
        'form' => 'mc_campaign_campaign_add_sms_info',
      ),
      4 => array(
        'form' => 'mc_campaign_campaign_add_resume_info',
      ),
  );
}

function mc_campaign_campaign_add_previous_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
  if ($current_step > 1) {
    if($form_state['step_information'][1]['stored_values']['media'] == "SMS" && $current_step == 3) $current_step--;
    if($form_state['step_information'][1]['stored_values']['media'] == "MAIL" && $current_step == 4) $current_step--;
    $current_step--;
    $form_state['values'] = $form_state['step_information'][$current_step]['stored_values'];
  }
  $form_state['rebuild'] = TRUE;
}

function mc_campaign_campaign_add_next_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
  if ($current_step < count($form_state['step_information'])) {
  	if($form_state['step_information'][1]['stored_values']['media'] == "SMS" && $current_step == 1) $current_step++;
    if ($form_state['step_information'][1]['stored_values']['media'] == "Mail" && $current_step == 2) $current_step++;
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

function mc_campaign_campaign_add_global_info($form, &$form_state) {
	
  $timeoption = array(
    '0' 		=> t('00:00'),
    '3600' 		=> t('01:00'),
    '7200' 		=> t('02:00'),
    '10800'		=> t('03:00'),
    '14400'		=> t('04:00'),
    '18000'		=> t('05:00'),
    '21600'		=> t('06:00'),
    '25200'		=> t('07:00'),
    '28800'		=> t('08:00'),
    '32400'		=> t('09:00'),
    '36000'		=> t('10:00'),
    '39600'		=> t('11:00'),
    '43200'		=> t('12:00'),
    '46800'		=> t('13:00'),
    '50400'		=> t('14:00'),
    '54000'		=> t('15:00'),
    '57600'		=> t('16:00'),
    '61200'		=> t('17:00'),
    '64800'		=> t('18:00'),
    '68400'		=> t('19:00'),
    '72000'		=> t('20:00'),
    '75600'		=> t('21:00'),
    '79200'		=> t('22:00'),
    '82800'		=> t('23:00'),
    '86400'		=> t('24:00'),
  );
  $form = array();
  $form['campaign'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Global Information'),
  );
  $form['campaign']['title'] = array(
    '#type'  => 'textfield',
    '#title' => t('Campaign name'),
    '#required' => TRUE,
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['title'])) ? $form_state['step_information'][1]['stored_values']['title'] : '',
  );
  
  // Domain
  $domain = domain_get_domain();
  
  $lists = db_query('SELECT lid, title FROM {mc_campaign_lists} WHERE did=:did',array(':did'=>$domain['domain_id']))->fetchAllKeyed();
  
  $form['campaign']['lists'] = array(
    '#type' => 'select',
    '#title' => t('Which list would you like to send this campaign to?'),
    '#options' => array_filter($lists, 'check_plain'),
    '#required' => TRUE,
    '#multiple' => TRUE,
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['lists'])) ? $form_state['step_information'][1]['stored_values']['lists'] : null,
  );
  $form['campaign']['media'] = array(
    '#type' => 'radios',
    '#options' => array(
      'Mail' => t('Mail Only'),
      'SMS' => t('SMS Only'),
      'Mail&SMS' => t('Mail and SMS'),
      'SSSOrMail' => t('SMS with Mail backup'),
      'MailOrSMS' => t('Mail with SMS backup'),
    ),
    '#required' => TRUE,
    '#title' => t('Media'),
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['media'])) ? $form_state['step_information'][1]['stored_values']['media'] : 'Mail',
  );
  $form['campaign']['type'] = array(
    '#type' => 'radios',
    '#options' => array(
      'Once' => t('You will only send the campaign once'),
      'Recurrent' => t('You will send this campaign recurrently'),
    ),
    '#required' => TRUE,
    '#title' => t('What kind of campaign do you want to build?'),
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['type'])) ? $form_state['step_information'][1]['stored_values']['type'] : 'Once',
  );
  $form['scheduling'] = array(
    '#type' => 'fieldset',
    '#title' => t('Scheduling'),
    '#states' => array(
      'visible' => array(
        ':input[name="type"]' => array('value' => t('Recurrent')),
      ),
    ),
  );
  $form['scheduling']['frequency'] = array(
    '#type' => 'select',
    '#title' => t('Sent Rate'),
    '#options' => array(
      '1' => t('Each Cron'),
      '900' => t('15 Minutes'),
      '1800' => t('30 Minutes'),
      '3600' => t('1 Hour'),
      '7200' => t('2 Hour'),
      '10800' => t('3 Hours'),
      '21600' => t('6 Hours'),
      '43200' => t('12 Hours'),
      '86400' => t('1 Day'),
    ),
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['frequency'])) ? $form_state['step_information'][1]['stored_values']['frequency'] : null,
  );
  $form['scheduling']['start_date'] = array(
    '#title' => t('Start Date'),
    '#type' => module_exists('date_popup') ? 'date_popup' : 'date_select',
    '#date_timezone' => date_default_timezone(),
    '#date_format' => 'd/m/Y',
    '#date_year_range' => '0:+1',
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['start_date'])) ? $form_state['step_information'][1]['stored_values']['start_date'] : date('Y-m-d', REQUEST_TIME),
  );
  $form['scheduling']['end_date'] = array(
    '#title' => t('End Date'),
    '#type' => module_exists('date_popup') ? 'date_popup' : 'date_select',
    '#date_timezone' => date_default_timezone(),
    '#date_format' => 'd/m/Y',
    '#date_year_range' => '0:+1',
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['end_date'])) ? $form_state['step_information'][1]['stored_values']['end_date'] : '',
  );
  $form['scheduling']['start_time'] = array(
    '#type' => 'select',
    '#title' => t('Start Time'),
    '#options' => $timeoption,
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['start_time'])) ? $form_state['step_information'][1]['stored_values']['start_time'] : '',
  );
  $form['scheduling']['end_time'] = array(
    '#type' => 'select',
    '#title' => t('End Time'),
    '#options' => $timeoption,
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['end_time'])) ? $form_state['step_information'][1]['stored_values']['end_time'] : '',
  );
  $form['scheduling']['weekday'] = array(
    '#type' => 'select',
    '#title' => t('Week Day'),
    '#options' => array(
      1 => t('Monday'),
      2 => t('Tuesday'),
      3 => t('Wednesday'),
      4 => t('Thursay'),
      5 => t('Friday'),
      6 => t('Saturday'),
      7 => t('Sunday'),
    ),
    '#multiple' => TRUE,
    '#required' => TRUE,
    '#default_value' => (isset($form_state['step_information'][1]['stored_values']['weekday'])) ? $form_state['step_information'][1]['stored_values']['weekday'] : '',
  );
  return $form;
}

function mc_campaign_campaign_add_mail_info($form, &$form_state) {
	  $form = array();
	  $form['mail'] = array(
	    '#type'  => 'fieldset',
	    '#title' => t('Mail Information'),
	  );
	  $form['mail']['mail_suject'] = array(
	    '#type'  => 'textfield',
	    '#title' => t('Message Subject'),
	    '#required' => FALSE,
        '#default_value' => (isset($form_state['step_information'][2]['stored_values']['mail_suject'])) ? $form_state['step_information'][2]['stored_values']['mail_suject'] : '',
	  );
	  $form['mail']['mail_from_name'] = array(
	    '#type'  => 'textfield',
	    '#title' => t('From name'),
	    '#required' => FALSE,
        '#default_value' => (isset($form_state['step_information'][2]['stored_values']['mail_from_name'])) ? $form_state['step_information'][2]['stored_values']['mail_from_name'] : '',
	  );
	  $form['mail']['mail_reply_to'] = array(
	    '#type'  => 'textfield',
	    '#title' => t('Reply-to Email'),
	    '#required' => FALSE,
        '#default_value' => (isset($form_state['step_information'][2]['stored_values']['mail_reply_to'])) ? $form_state['step_information'][2]['stored_values']['mail_reply_to'] : '',
	  );
	  $lists = db_query("SELECT tid, subject FROM {mc_campaign_templates} where media='mail'")->fetchAllKeyed();
	  $form['mail']['lists'] = array(
	    '#type' => 'select',
	    '#title' => t('Template'),
	    '#options' => array_filter($lists, 'check_plain'),
	    '#required' => FALSE,
	    '#multiple' => FALSE,
	    '#empty_value' => '',
        '#default_value' => (isset($form_state['step_information'][2]['stored_values']['mtid'])) ? $form_state['step_information'][2]['stored_values']['mtid'] : null,
	  );
	  $form['mail_body'] = array(
        '#type' => 'text_format',
        '#title' => t('Body'),
        '#default_value' => (isset($form_state['step_information'][2]['stored_values']['mail_body']['value'])) ? $form_state['step_information'][2]['stored_values']['mail_body']['value'] : null,
	  );
      $form['media'] = array(
        '#type' => 'hidden',
      	'#value' => 'mail',
      );
      $form['token_help'] = array(
       '#title' => t('Replacement patterns'),
       '#type' => 'fieldset',
       '#collapsible' => TRUE,
       '#collapsed' => TRUE,
       '#description' => t('Use [repeat] and [/repeat] for recurring items such as nodes'),
     );
     $form['token_help']['help'] = array(
       '#theme' => 'token_tree',
       '#token_types' => array('site', 'node', 'campaign'),
       '#recursion_limit' => 2,
     );
	  
	 return $form;  	
}

function mc_campaign_campaign_add_sms_info($form, &$form_state) {
  drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.smscounter/jquery.smscounter.js');
  $form = array();
  $form['sms'] = array(
    '#type'  => 'fieldset',
    '#title' => t('SMS Information'),
  );
  $lists = db_query("SELECT tid, subject, body FROM {mc_campaign_templates} where media='sms'")->fetchAllKeyed();
  $form['sms']['lists'] = array(
    '#type' => 'select',
    '#title' => t('Template'),
    '#options' => array_filter($lists, 'check_plain'),
    '#required' => FALSE,
    '#multiple' => FALSE,
    '#empty_value' => '',
    '#attributes' => array('onchange' => "loadTemplate('sms',this.value)"),
    '#default_value' => (isset($form_state['step_information'][3]['stored_values']['stid'])) ? $form_state['step_information'][3]['stored_values']['stid'] : null,
  );
  $form['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#suffix' => "<div id='count'></div><br/>",
    '#default_value' => (isset($form_state['step_information'][3]['stored_values']['body'])) ? $form_state['step_information'][3]['stored_values']['body'] : null,
  );
  $form['token_help'] = array(
    '#title' => t('Replacement patterns'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Use [repeat] and [/repeat] for recurring items such as nodes'),
  );
  $form['token_help']['help'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('site', 'node', 'campaign'),
    '#recursion_limit' => 2,
  );
  return $form;
}

function mc_campaign_campaign_add_resume_info($form, &$form_state) {
  $sms = $mail = null;
  
  // FIRST LIST
  $firstlid = $form_state['step_information'][1]['stored_values']['lists'];
  
  $query = db_select('mc_campaign_subscribers', 'subscribers');
  $query->join('mc_campaign_lists_subscribers', 'ls', 'ls.sid = subscribers.sid');
  $query->fields('subscribers');
  $query->fields('ls');
  $query->condition('lid', $firstlid,'IN');
  $query->condition('deleted', 0);
  $subscriber = $query->execute()->fetchObject();
  
  if ($form_state['step_information'][1]['stored_values']['media'] != 'SMS') {

  	$mail = (isset($form_state['step_information'][2]['stored_values']['mail_body']['value'])) ? $form_state['step_information'][2]['stored_values']['mail_body']['value'] : '';
   //TODO TOKENIZE
    $options = array('clear' => FALSE);
    $mail = token_replace($mail, array());
    $mail = (isset($subscriber->sid)) ? token_replace($mail, array('campaign_subscriber' => $subscriber), $options) : $mail;
  	
  	$form['mail'] = array(
      '#type'  => 'fieldset',
      '#title' => t('Mail Information'),
    );
    $form['mail']['desc'] = array(
      '#markup' => '<h4></h4>'.$mail,
      '#weight' => -1,
    );
  	
  }

  if ($form_state['step_information'][1]['stored_values']['media'] != 'Mail') {
  
  	$sms = (isset($form_state['step_information'][3]['stored_values']['body'])) ? $form_state['step_information'][3]['stored_values']['body'] : '';
    //TODO TOKENIZE
    $options = array('clear' => FALSE);
    $sms = token_replace($sms, array());
    $sms = (isset($subscriber->sid)) ? token_replace($sms, array('campaign_subscriber' => $subscriber), $options) : $sms;

    $form['sms'] = array(
      '#type'  => 'fieldset',
      '#title' => t('SMS Information'),
    );
    $form['sms']['desc'] = array(
      '#markup' => $sms,
    );
    
  }
  
  
  $form['test'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Test Information'),
  );
  if ($mail) {
    $form['test']['test-email'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail to send test mail to'),
      '#required' => FALSE,
      '#default_value' => variable_get('campaign_default_mail', ''),
    );
  }
  if ($sms) {
    $form['test']['test-mobile'] = array(
      '#type' => 'textfield',
      '#title' => t('Mobile to send sms to'),
      '#required' => FALSE,
      '#default_value' => variable_get('campaign_default_mobile', ''),
    );
  }
  
  $form['campaign'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Active'),
  );
  $form['campaign']['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Whether this campaign is active or not.'),
    '#default_value' => (isset($form_state['step_information'][4]['stored_values']['active'])) ? $form_state['step_information'][4]['stored_values']['active'] : 0,
  );
  
  $form['send-test'] = array(
    '#type' => 'submit',
    '#value' => t('Test Campaign with addresses above'),
    '#submit' => array('mc_campaign_campaign_add_sent_test'),
    '#weight' => 8,
  );
  $form['send'] = array(
    '#type' => 'submit',
    '#value' => t('Save Campaign'),
    '#name' => 'save',
    '#submit' => array('mc_campaign_campaign_add_submit'),
    '#weight' => 9,
    //'#limit_validation_errors' => array(),
  );
  
  return $form;
}


function mc_campaign_campaign_add_submit($form, &$form_state) {
	
  $domain = domain_get_domain();
	
  if (isset($form_state['step_information'][0]['cid'])) {
  	
    $id = db_update('mc_campaign_campaigns')
      ->fields(array(
        'title' => $form_state['step_information'][1]['stored_values']['title'],
        'type' => $form_state['step_information'][1]['stored_values']['type'],
        'media' => $form_state['step_information'][1]['stored_values']['media'],
      
        'frequency' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? $form_state['step_information'][1]['stored_values']['frequency'] : null,
        'start_date' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? strtotime($form_state['step_information'][1]['stored_values']['start_date']) : null,
        'end_date' =>($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? strtotime($form_state['step_information'][1]['stored_values']['end_date']) : null,
        'start_time' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? $form_state['step_information'][1]['stored_values']['start_time'] : null,
        'end_time' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? $form_state['step_information'][1]['stored_values']['end_time'] : null,
        'weekday' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? implode(",",$form_state['step_information'][1]['stored_values']['weekday']) : null,
              
        'stid' => null,
        'ltid' => null,
        'mtid' => null,
        
        'mail_body' => (isset($form_state['step_information'][2]['stored_values']['mail_body']['value'])) ? $form_state['step_information'][2]['stored_values']['mail_body']['value'] : '',
        'mail_suject' => (isset($form_state['step_information'][2]['stored_values']['mail_suject'])) ? $form_state['step_information'][2]['stored_values']['mail_suject'] : '',
        'mail_from_name' => (isset($form_state['step_information'][2]['stored_values']['mail_from_name'])) ? $form_state['step_information'][2]['stored_values']['mail_from_name'] : '',
        'mail_reply_to' => (isset($form_state['step_information'][2]['stored_values']['mail_reply_to'])) ? $form_state['step_information'][2]['stored_values']['mail_reply_to'] : '',
            
        'sms_body' => (isset($form_state['step_information'][3]['stored_values']['body'])) ? $form_state['step_information'][3]['stored_values']['body'] : '',
        
        'updated' => REQUEST_TIME,
        
        'next_execution' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? 0 : null,
        'last_execution' => null,
      
        'active' => $form_state['values']['active'],
      ))
      ->condition('cid', $form_state['step_information'][0]['cid'])
      ->execute();
      
    // RESET ONCE
    // TODO ??
    if ($form_state['step_information'][1]['stored_values']['type'] == 'Once') {
      $id = db_update('mc_campaign_campaigns')
      ->fields(array(
        'last_execution' => null,
      ))
      ->condition('cid', $form_state['step_information'][0]['cid'])
      ->execute();	
    }
    
    // JOIN LIST
    $result = db_delete('mc_campaign_campaigns_lists')
    ->condition('cid', $form_state['step_information'][0]['cid'])
    ->execute();
        
    foreach($form_state['step_information'][1]['stored_values']['lists'] as $lid) {
  	  $result = db_insert('mc_campaign_campaigns_lists')
        ->fields(array(
          'cid' => $form_state['step_information'][0]['cid'],
          'lid' => $lid,
          'created' => REQUEST_TIME,
      ))
      ->execute();
    }

  } else {
  	
    $id = db_insert('mc_campaign_campaigns')
    ->fields(array(
    
  	  'did' => $domain['domain_id'],
        
      'title' => $form_state['step_information'][1]['stored_values']['title'],
      'type' => $form_state['step_information'][1]['stored_values']['type'],
      'media' => $form_state['step_information'][1]['stored_values']['media'],

      'frequency' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? $form_state['step_information'][1]['stored_values']['frequency'] : null,
      'start_date' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? strtotime($form_state['step_information'][1]['stored_values']['start_date']) : null,
      'end_date' =>($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? strtotime($form_state['step_information'][1]['stored_values']['end_date']) : null,
      'start_time' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? $form_state['step_information'][1]['stored_values']['start_time'] : null,
      'end_time' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? $form_state['step_information'][1]['stored_values']['end_time'] : null,
      'weekday' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? implode(",",$form_state['step_information'][1]['stored_values']['weekday']) : null,
    
      'mtid' => null,
      'stid' => null,
      'ltid' => null,

      'mail_body' => (isset($form_state['step_information'][2]['stored_values']['mail_body']['value'])) ? $form_state['step_information'][2]['stored_values']['mail_body']['value'] : '',
      'mail_suject' => (isset($form_state['step_information'][2]['stored_values']['mail_suject'])) ? $form_state['step_information'][2]['stored_values']['mail_suject'] : '',
      'mail_from_name' => (isset($form_state['step_information'][2]['stored_values']['mail_from_name'])) ? $form_state['step_information'][2]['stored_values']['mail_from_name'] : '',
      'mail_reply_to' => (isset($form_state['step_information'][2]['stored_values']['mail_reply_to'])) ? $form_state['step_information'][2]['stored_values']['mail_reply_to'] : '',

      'sms_body' => (isset($form_state['step_information'][3]['stored_values']['body'])) ? $form_state['step_information'][3]['stored_values']['body'] : '',

      'next_execution' => ($form_state['step_information'][1]['stored_values']['type'] != 'Once') ? 0 : null,
      'last_execution' => null,
    
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
      'active' => $form_state['values']['active'],
    ))
    ->execute();
  
    foreach($form_state['step_information'][1]['stored_values']['lists'] as $lid) {
  	$result = db_insert('mc_campaign_campaigns_lists')
      ->fields(array(
      'cid' => $id,
      'lid' => $lid,
      'created' => REQUEST_TIME,
    ))
    ->execute();
    }

    $id
    ? drupal_set_message(t('Your new campaign was created !'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  	
  }

  drupal_goto('admin/campaign/campaign');
  
}


function mc_campaign_campaign_add_sent_test($form, &$form_state) {
	
  $domain = domain_get_domain();
	
  $firstlid = reset($form_state['step_information'][1]['stored_values']['lists']);

  $query = db_select('mc_campaign_subscribers', 'subscribers');
  $query->join('mc_campaign_lists_subscribers', 'ls', 'ls.sid = subscribers.sid');
  $query->fields('subscribers');
  $query->fields('ls');
  $query->condition('lid', $firstlid);
  $query->condition('deleted', 0);
  $subscriber = $query->execute()->fetchObject();
  
  if ($form_state['step_information'][1]['stored_values']['media'] != 'SMS') {

  	$mail_body = (isset($form_state['step_information'][2]['stored_values']['mail_body']['value'])) ? $form_state['step_information'][2]['stored_values']['mail_body']['value'] : '';
   //TODO TOKENIZE
    $options = array('clear' => FALSE);
    $mail_body = token_replace($mail_body, array());
    $mail_body = token_replace($mail_body, array('campaign_subscriber' => $subscriber), $options);
    
    $message = drupal_mail('mc_campaign', 'mc_campaign_key', $form_state['values']['test-email'], language_default(), array(), 'no-reply@polycliniqueborinage.com', FALSE);
    $message['body'] = $mail_body;
    $message['subject'] = $form_state['step_information'][2]['stored_values']['mail_suject'];
    $message['body_format'] = 'plain_text';
    
    // Retrieve the responsible implementation for this message.
    $system = drupal_mail_system('mc_campaign', 'test');

    // Format the message body.
    $message = $system->format($message);

    // Send e-mail.
    $message['result'] = $system->mail($message);
    
    //$system = drupal_mail_system('mc_campaign', 'mc_campaign_key');
    //$status = $system->mail($message);
    //if ($status) drupal_set_message(t('Test mail sent succesfully')); else drupal_set_message(t('Test mail failed to send'));
  	
  }

  if ($form_state['step_information'][1]['stored_values']['media'] != 'Mail') {
  
  	$sms = (isset($form_state['step_information'][3]['stored_values']['body'])) ? $form_state['step_information'][3]['stored_values']['body'] : '';
    //TODO TOKENIZE
    $options = array('clear' => FALSE);
    $sms = token_replace($sms, array());
    $sms = token_replace($sms, array('campaign_subscriber' => $subscriber), $options);

    // SEND SMS
    $result = sms_clickatell_command('auth');
    if ($result['status'] == TRUE) {
  	  $data['sid'] 	= $result['sid'];
  	  $data['message'] = $sms;
  	  $data['number']	 = $form_state['values']['test-mobile'];
  	  $result = sms_clickatell_command('sendmsg', $data);
	  if ($result['status'] == TRUE) {
        mc_campaign_insert_cost($domain['domain_id'], -10 * $result['cost'], 'Sent Test SMS');
        drupal_set_message(t('SMS send to ').$form_state['values']['test-mobile'], 'status');
	  }
    }
    
  }
  
  $current_step = &$form_state['step'];
  $form_state['rebuild'] = TRUE;  // Force rebuild with next step.
  
}
























/**
 * Menu callback; delete a subscribers list.
 */
function mc_campaign_campaign_delete($form, &$form_state, $cid) {
	
  $form['cid'] = array(
    '#type' => 'hidden',
    '#value' => $cid,
  );
  $output = t('Are you sure you want to delete the campaign? This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), drupal_get_destination(), filter_xss($output));
  
}

function mc_campaign_campaign_delete_submit($form, &$form_state) {
	
  $num_deleted = db_delete('mc_campaign_campaigns')
    ->condition('cid', $form_state['values']['cid'])
    ->execute();

  $result = db_delete('mc_campaign_campaigns_lists')
    ->condition('cid', $form_state['values']['cid'])
    ->execute();
  
  $result = db_delete('mc_campaign_notifications')
    ->condition('cid', $form_state['values']['cid'])
    ->execute();
    
  isset($num_deleted)
    ? drupal_set_message(t('Campaign deleted successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
    
  drupal_goto(drupal_get_destination());
}

























function mc_campaign_campaign_view($form, &$form_state, $arg1 = NULL) {
	
  // Add CSS	
  drupal_add_css(drupal_get_path('module', 'mc_campaign') . '/css/dashboard.css');
	
  // List detail
  $campaign = db_select('mc_campaign_campaigns','campaign')
    ->condition('cid',$arg1)
    ->fields('campaign')
    ->execute()
    ->fetchObject();
  
  // sms
  $campaign->mail = db_query("SELECT COUNT('nid') FROM {mc_campaign_notifications} WHERE cid = :cid AND deleted = 0", array(
      ':cid' => $arg1,
  ))->fetchField();
  // mail
  $campaign->sms = db_query("SELECT COUNT('nid') FROM {mc_campaign_notifications} WHERE cid = :cid AND deleted = 0", array(
      ':cid' => $arg1,
  ))->fetchField();
  
  $build['dashboard'] = array(
    '#theme' => 'mc_campaign_campaign_detail',
    '#fields' => $campaign,
  );
  
  // Graphe
  if (mc_campaign_campaign_view_graphic()){
    $build['graphe'] = array(
      '#theme' => 'mc_campaign_graphe',
      '#fields' => $campaign,
  );
  $build['graphe']['#attached']['js'][] = array('data' => '(function($){$(function() { 
	var chart;
  	$(document).ready( function() {
  	
  	
				var options = {
					chart: {
						renderTo: "container"
					},
					title: {
						text: ""
					},
					xAxis: {
						type: "datetime",
						tickInterval: 7 * 24 * 3600 * 1000, // one week
						tickWidth: 0,
						gridLineWidth: 1,
						labels: {
							align: "left",
							x: 3,
							y: -3 
						}
					},
					yAxis: [{ // left y axis
						title: {
							text: null
						},
						labels: {
							align: "left",
							x: 0,
							y: 16,
							formatter: function() {
								return Highcharts.numberFormat(this.value, 0);
							}
						},
						showFirstLabel: false
					}, { // right y axis
						linkedTo: 0,
						gridLineWidth: 0,
						opposite: true,
						title: {
							text: null
						},
						labels: {
							align: "right",
							x: 0,
							y: 16,
							formatter: function() {
								return Highcharts.numberFormat(this.value, 0);
							}
						},
						showFirstLabel: false
					}],
					legend: {
						align: "left",
						verticalAlign: "top",
						y: -10,
						floating: true,
						borderWidth: 0
					},
					tooltip: {
						shared: true,
						crosshairs: true
					},
					plotOptions: {
						series: {
							cursor: "pointer",
							point: {
								events: {
									click: function() {
									    console.log(this);
									}
								}
							},
							marker: {
								lineWidth: 1
							}
						}
					},
					
					series: [{
						name: "Suscribers",
						lineWidth: 4,
						marker: {
							radius: 4
						}
					}]
				};
				
				jQuery.get("graphe/'.$arg1.'", null, function(data, state, xhr) {
				   options.series[0].data = data;
				   chart = new Highcharts.Chart(options);
  				});
  	
  	
	});
   })})(jQuery);', 'type' => 'inline');
  }


  return $build;
}

function mc_campaign_campaign_view_graphic() {
  if (!module_exists('libraries')) {
    return drupal_set_message(t('For graphical statistics 
      please install the <a href=@url-lib>Libraries API</a>
      and <a href=@url-jqplot>download the jQplot library</a>.',
    array(
      '@url-lib' => 'http://drupal.org/project/libraries',
      '@url-jqplot' => 'https://bitbucket.org/cleonello/jqplot/downloads/',
    )),
    'warning');
  }
  $path = libraries_get_path('highcharts');
  if (!file_exists($path . '/js/highcharts.js')) {
    drupal_set_message(t('HighCharts.js library is not present in your filesystem.'),'warning');
    return FALSE;
  } else {
    drupal_add_js($path . '/js/highcharts.js');
    return TRUE;
  }
}

function mc_campaign_campaign_graphe($arg1 = NULL) {
	
  $results = db_query("SELECT lcreated as created, from_unixtime(created,'%d-%m-%Y') as format_created, count(nid) as count 
  FROM {mc_campaign_notifications} WHERE cid = :cid GROUP BY from_unixtime(created,'%Y-%m-%d')", array(':cid' => $arg1))->fetchAll();
  $mail = array();
  foreach ($results as $result) {
  	 $x = (int) $result->created * 1000;
  	 $y = (int) $result->count;
 	 $mail[] = array($x, $y);
  }
  $results = db_query("SELECT lcreated as created, from_unixtime(created,'%d-%m-%Y') as format_created, count(nid) as count 
  FROM {mc_campaign_notifications} WHERE cid = :cid GROUP BY from_unixtime(created,'%Y-%m-%d')", array(':cid' => $arg1))->fetchAll();
  $sms = array();
  foreach ($results as $result) {
  	 $x = (int) $result->created * 1000;
  	 $y = (int) $result->count;
 	 $sms[] = array($x, $y);
  }
  $output['sms'] = $sms;
  drupal_json_output($output);
}


