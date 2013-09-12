<?php

/**
 * listing of all lists
 */
function mc_campaign_reminder_list($form, &$form_state, $sent = FALSE) {

  // Roles
  global $user;
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('Hey buddy ! <a href="/user/me/edit">Activate your account</a> with the activation code you have received on your mobile.'),'error');
  }
  
  // Domain
  $domain = domain_get_domain();
  
  // Delete
  if (isset($form_state['storage']['confirm'])) {
    $form['operation'] = array(
      '#type' => 'hidden',
      '#value' => 'delete',
    );
    $form['reminders'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['values']['reminders'],
    );
    $output = t('Are you sure you want to delete the following reminders?');
    $output .= '<br/><br/>';
    $output .= t('This action cannot be undone.');
    return confirm_form($form, t('Delete the following?'), 'account/reminders', filter_xss($output));
  }
  
  // Action
  $form['add']['add'] = array(
    '#markup' => l(t('Create reminder'), 'account/reminders/add', array('attributes' => array('class' => 'button_left'))),
  );
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options['delete'] = t('Delete the selected reminders');
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

  // Table
  $header = array(
    'media' => array('data' => 'Media', 'field' => 'n.media'),
	'name' => array('data' => 'Name', 'field' => 'contact'),
    'contact' => array('data' => 'Contact', 'field' => 'contact'),
    'message' => array('data' => 'Message', 'field' => 'n.message'),
	'estimate_sent' => array('data' => 'Estimation Time', 'field' => 'n.estimate_sent','sort' => 'desc'),
    'sent' => array('data' => 'Send Time', 'field' => 'n.sent','sort' => 'desc'),
    'received' => array('data' => 'Received Time', 'field' => 'n.received'),
    'status' => array('data' => 'Status', 'field' => 'n.status'),
  );

  if (isset($sent) && $sent) {
    unset ($header['estimate_sent']); 
  } else {
    unset ($header['sent']);
    unset ($header['received']);
  }
  
  $query = db_select('mc_campaign_notifications','n')->extend('PagerDefault')->extend('TableSort');
  $query->join('mc_campaign_subscribers', 's', 's.sid = n.sid');
  $query->fields('n');
  $query->fields('s');
  $query = $query
    ->limit(50)
    ->orderByHeader($header)
    ->condition('n.did',$domain['domain_id'])
    ->condition('n.deleted', 0)
    ->condition('n.module', 'reminder');
  if ($sent) {
    $query->condition('n.sent', 0, '>');
  } else {
    $query->condition('n.sent', 0, '=');
  }
    
  $result = $query->execute();
    
  $options = array();
  $destination = drupal_get_destination();
    
  foreach ($result as $row) {
    $operation = array();
    $operation[] = array(
      '#type' => 'link',
      '#title' => t('View'),
      '#href' => 'account/subscribers/view/' . $row->nid,
      '#options' => array('query' => $destination),
    );
    $operation[] = array(
      '#markup' => ' | ',
    );
    $operation[] = array(
      '#type' => 'link',
      '#title' => t('Edit'),
      '#href' => 'account/subscribers/edit/' . $row->nid,
      '#options' => array('query' => $destination),
    );
    $operation[] = array(
      '#markup' => ' | ',
    );
    $operation[] = array(
      '#type' => 'link',
      '#title' => t('Delete'),
      '#href' => 'account/subscribers/delete/' . $row->nid,
      '#options' => array('query' => $destination),
    );
    
    $options[$row->nid] = array(
      'media' => $row->media,
      'name' => $row->firstname.' '.$row->lastname,
      'contact' => check_plain($row->contact),
      'message' => check_plain($row->message),
      'estimate_sent' => format_date($row->estimate_sent, 'short'),
      'sent' => format_date($row->estimate_sent, 'sent'),
      'received' => format_date($row->estimate_sent, 'received'),
      'status' => mc_campaign_format_status($row->status),
      'operations' => array(
        'data' => $operation,
      ),
    );  
  }

  $form['reminders'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No reminder yet.'),
  );

  $form['pager'] = array('#theme' => 'pager');
  
  return $form;
  
}

function mc_campaign_reminder_list_submit($form, &$form_state) {
  // Domain
  $domain = domain_get_domain();
  $operation = $form_state['values']['operation'];
  $reminders = $form_state['values']['reminders'];
  if ($reminders) {
    switch ($operation) {
      case 'delete':
        if (!isset($form_state['storage']['confirm'])) {
          $form_state['storage']['confirm'] = TRUE;
          $form_state['rebuild'] = TRUE;
          return;
        }
        foreach ($reminders as $id) {
          $num_deleted = db_delete('mc_campaign_notifications')
            ->condition('nid', $id)
            ->condition('did',$domain['domain_id'])
            ->execute();
        }
        break;
      default:
        drupal_set_message(t('Please select an action'), 'warning');
    }
    isset($num_deleted)
      ? drupal_set_message(t('Reminder(s) deleted successfully'), 'status')
      : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  }
  else {
    drupal_set_message(t('Please select a least one reminder.'), 'warning');
  }
}










/**
 * Custom taxonomy callback, which also accepts an empty string search.
 */
function mc_campaign_reminder_autocomplete($form, &$form_state, $search) {
  
  // Domain
  $domain = domain_get_domain();
  $options= array();

  // Get autocomplete contact
  $query = db_select('mc_campaign_subscribers','subscribers')->fields('subscribers');
  $results = $query
  ->condition(db_or()
  ->condition('firstname', '%' . db_like($search) . '%', 'LIKE')
  ->condition('lastname', '%' . db_like($search) . '%', 'LIKE')
  )
  ->condition('did',$domain['domain_id'])
  ->range(0,10)
  ->execute()
  ->fetchAll();

  foreach ($results as $result) {
    $options['sid-' . $result->sid] = t('Contact: ') . $result->firstname . ' ' . $result->lastname;
  }

  // Get autocomplete list
  $query = db_select('mc_campaign_lists', 'lists')->fields('lists');
  $results = $query
  ->condition('did',$domain['domain_id'])
  ->condition('title', '%' . db_like($search) . '%', 'LIKE')
  ->range(0,10)
  ->execute()
  ->fetchAll();
    
  foreach ($results as $result) {
    $options['lid-' . $result->lid] = t('List: ') . $result->title;
  }
  
  //$options= array();
  //$options[1] = array('value'=>1,'label'=>'david1');
  //$options[2] = array('value'=>2,'label'=>'david2');
  //array_push($options,array('value'=>1,'label'=>'david1'));
  //array_push($options,array('value'=>2,'label'=>'david2'));
  drupal_json_output($options);
  
  exit();
  
  return '';

}

/**
 * Menu callback; add a newsletter template.
 */
function mc_campaign_reminder_add($form, &$form_state) {
	
  global $language_content;
  global $user;
  
  // Roles
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('First you need to <a href="/user/me/edit">activate your account</a> with the activation code you have received on your mobile.'),'error');
    return NULL;
  }

  drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.smscounter/jquery.smscounter.js');
  
  switch($language_content->language) {
    case 'nl':
      drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.datejs/nl-NL.js');
      break;
    case 'fr':
      drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.datejs/fr-FR.js');
      break;
    default:
      drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.datejs/en-GB.js');
  }
  drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.datejs/date.js');
    
  drupal_add_library('system','ui.datepicker');
    drupal_add_js('jQuery(document).ready(function(){jQuery( ".pickadate" ).datepicker({
      dateFormat: "dd/mm/yy",
      autoSize: true
    });
  });', 'inline');     
    
  $form = array();
  
  $form['manage'] = array(
    '#type' => 'fieldset',
    '#title' => t(''),
  );
    
  $form['manage']['who'] = array(
    '#type' => 'autocomplete_deluxe',
    '#autocomplete_deluxe_path' => 'autocomplete',
    '#multiple' => TRUE,
    '#title' => t('Who'),
    '#required' => FALSE,
  );
  
  $form['manage']['datetime'] = array(
    '#type' => 'textfield',
    '#attributes' => array('autocomplete'=>'off'),
    '#title' => t('When (date and time)'),
    '#required' => FALSE,
    '#description' => t('now ; today, 15:15 ; tomorrow, 8am ; next week, 12:00 ; 11/10/2012 11:13 ; next monday'),
    '#suffix' => "<div id='datetimestring'></div><br/>",
  );
  $form['manage']['datetimehidden'] = array(
    '#type' => 'textfield',
    '#attributes' => array('class' => array('hide')),
    '#default_value' => '',
  );
  // TODO ADD 'call_mobile'=>'Call Mobile'
  $form['manage']['media'] = array(
    '#type' => 'select',
    '#title' => t('How'),
    '#options' => array('sms' => t('SMS'), 'mail' => t('Email'), 'call_fixedline' => t('Call Fixed Line')),
    '#required' => TRUE,
    '#default_value' => 'sms',
  );
  $form['sms'] = array(
      '#type' => 'fieldset',
      '#title' => t(''),
      '#states' => array(
        'visible' => array(
          ':input[name="media"]' => array('value' => 'sms'),
        ),
      ),
  );
  $form['sms']['body'] = array(
      '#type' => 'textarea',
      '#title' => t('What'),
      '#required' => FALSE,
      '#suffix' => "<div id='count'></div><br/>",
  );
  $form['mail'] = array(
      '#type' => 'fieldset',
      '#title' => t(''),
      '#states' => array(
        'visible' => array(
          ':input[name="media"]' => array('value' => 'mail'),
        ),
      ),
  );
  $form['mail']['mail_body'] = array(
        '#type' => 'text_format',
        '#title' => t('Body'),
        '#default_value' => null,
  );
	
  $form['call_mobile'] = array(
      '#type' => 'fieldset',
      '#title' => t(''),
      '#states' => array(
        'visible' => array(
          ':input[name="media"]' => array('value' => 'call_mobile'),
        ),
      ),
  );
  $form['call_mobile']['call_mobile_body'] = array(
      '#type' => 'textarea',
      '#title' => t('What'),
      '#required' => FALSE,
      '#suffix' => "<div id='count_mobile'></div><br/>",
  );
    
  $form['call_fixedline'] = array(
      '#type' => 'fieldset',
      '#title' => t(''),
      '#states' => array(
        'visible' => array(
          ':input[name="media"]' => array('value' => 'call_fixedline'),
        ),
      ),
  );
  $form['call_fixedline']['call_fixedline_body'] = array(
      '#type' => 'textarea',
      '#title' => t('What'),
      '#required' => FALSE,
      '#suffix' => "<div id='count_fixedline'></div><br/>",
  );
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Create'),
  );
  $form['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'account/reminders',
  );

  return $form;
}


function mc_campaign_reminder_add_validate($form, $form_state) {
  if ($form_state['values']['datetimehidden'] == '') {
    form_set_error('datetime', t('Enter the date ad time you want to send the message'));
  }
}


function mc_campaign_reminder_add_submit($form, &$form_state) {
  
  //dpm($form_state['values']);
	
  $cid = 0;
  $lid = 0;
  $id = 0;
  $module = 'reminder';
  
  // GET DOMAIN
  $domain = domain_get_domain();
  $did = $domain['domain_id'];
  
  // GET Estimate Time
  $estimate_sent = strtotime($form_state['values']['datetimehidden']);
  
  // GET UUID
  $uuid = mc_campaign_action_uuid_generate_php();
  
  // GET MESSAGE AND MEDIA
  $media = $form_state['values']['media'];
  switch($media) {
    case 'sms':
      $message = $form_state['values']['body'];
  	  break;
    case 'mail':
      $message = $form_state['values']['mail_body']['value'];
  	  break;
    case 'call_mobile':
      $message = $form_state['values']['call_mobile_body'];
  	  break;
    case 'call_fixedline':
      $message = $form_state['values']['call_fixedline_body'];
  	  break;
  }
  
  // GET SID AND LID AND CONTACT
  //dpm($form_state['values']);
  $whos = explode(",",$form_state['values']['who']);
  
  foreach($whos as $who){
  	
  	$type = substr($who, 0, 3);
  	$who = substr($who, 4);
  	
  	// IF CONTACT
  	if ($type == 'sid') {
  		$sid = $who;
  		$subscriber = mc_campaign_get_subscriber($sid);
  		switch($media) {
    	  case 'call_mobile':
  		  case 'sms':
    	  	$contact = $subscriber->mobile;
    	  	break;
  		  case 'call_fixedline':
    	  	$contact = $subscriber->fixedline;
    	  	break;
    	  case 'mail':
    	  	$contact = $subscriber->email;
    	  	break;
  		}
  		if ($contact !='') {
  		  drupal_set_message(t('Reminder created'), 'status');
  	  	  campaign_action_add_message($did,$id,$uuid,$cid,$lid,$sid,$module,$media,$contact,$message,$estimate_sent);
  		} else {
  		  drupal_set_message(t('Contact detail missing'),'error');
  		}
  	} else {
  		// GET CONTACTS
  		//campaign_action_add_message($did,$id,$uuid,$cid,$lid,$sid,$module,$media,$contact,$message,$estimate_sent);
  	}
  	
  }
  
  //$config['sms_tropo_ssl'] = TRUE;
  
  //$data['network'] = 'voice';
  //$data['channel'] = 'text';
  //$data['number'] = '447548589864';
  //$data['message'] = 'message';
  
  //sms_tropo_command('create', $data, $config);

  //drupal_goto('account/reminders');
}







/**
 * Executes a command using the Clickatell API
 *
 * data array fields:
 * <ul>
 *   <li>number - MSISDN of message recipient. Purely numeric and must begin with intl prefix, eg. 4477121231234.</li>
 *   <li>message - Message text. Max 459 chars (3x SMS). Use %n for newline.</li>
 *   <li>options - Array of additional options, as below.</li>
 * </ul>
 *
 * data['options'] array fields:
 * <ul>
 *   <li>sender - Optional: Sender ID may be an MSISDN (max 16 chars) or an alphanumeric string (max 11 chars). See note about Approved Sender IDs in the header of this file. Clickatell param: 'from'</li>
 *   <li>reference - Optional: Reference tag to apply to message. Will appear on any receipt. No spaces. Clickatell param: 'cliMsgId'</li>
 *   <li>delaymins - Optional: Delay message sending by N minutes. Clickatell param: 'deliv_time'</li>
 *   <li>expiremins - Optional: The message send will abort if not sent within N minutes. Clickatell param: 'validity'</li>
 *   <li>priority - Optional: Queue priority to apply to the message. Can be 1, 2 or 3, where 1 is high priority. Clickatell param: 'queue'</li>
 *   <li>expectreply - Optional: Route the message properly so that the user can reply. Clickatell param: 'mo'</li>
 * </ul>
 *
 * @param str $command One of 'auth', 'sendmsg' or 'getbalance'.
 * @param array $data All data required to perform the command.
 * @param array $config Gateway configuration parameters.
 * @return bool Whether the command succeeded or not.
 */
function sms_tropo_command($command = 'create', $data = array(), $config = NULL) {
	
  dpm($config);
  
  dpm($data);
  
  if ($config['sms_tropo_ssl']) {
    $scheme = 'https';
  }
  else {
    $scheme = 'http';
  }
  
  dpm($scheme);
	
  switch ($command) {
  	
    case 'create':
    	
      $message = drupal_encode_path($data['message']);
	  
	  if (isset($data['network'])) {
	    $data['number'] = $data['number'] . '&network=' . $data['network'];
	  }
	  // Build query string
	  $query = '&token=' . '09fa699136bea84c81fc95dc6e209d43727d731443280817f7592894533418a28768462cf6a0c2f5d23536f6'; // use the appropriate (text or voice) token
	  $query .= '&to=' . $data['number'] . '&channel=' . $data['channel'] . '&message=' . $message;
      break;
  }
  
  // Run the command
  $url = $scheme . '://api.tropo.com/1.0/sessions?action=' . $command . $query;
  dpm($url);
  $http_result = drupal_http_request($url);
  watchdog('sms_tropo', "DRUPAL_HTTP_REQUEST RESULT: " . print_r($http_result,TRUE));
  // Check for HTTP errors
  dpm($http_result);
  
  return '';

}

/**
 * Converts a string to USC-2 encoding if neccessary.
 */
function sms_tropo_unicode($message) {
  if (function_exists('iconv')) {
    $latin = @iconv('UTF-8', 'ISO-8859-1', $message);
    if (strcmp($latin, $message)) {
      $arr = unpack('H*hex', @iconv('UTF-8', 'UCS-2BE', $message));
      return strtoupper($arr['hex']) . '&unicode=1';
    }
  }
  return FALSE;
}








