<?php

/**
 * 
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @return multitype:string The
 */
function mc_campaign_quick_sms($form, &$form_state) {
  $form = array();
  drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.smscounter/jquery.smscounter.js');
  $form['new'] = array(
    '#markup' => '<h3>' . t('Quick SMS') . '</h3>',
    '#weight' => -1, 
  );
  $form['mobiles'] = array(
    '#type' => 'textarea',
    '#title' => t('Mobiles'),
    '#required' => TRUE,
    '#description' => t('Enter one mobile per line. Format : 447548589864'),
  );
  $form['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#required' => TRUE,
    '#suffix' => "<div id='count'></div><br/>",
  );
  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Sent message'),
  );
  return $form;
}

function mc_campaign_quick_sms_validate($form, $form_state) {
  $error = false;
  $entries = $form_state['values']['mobiles'];
  $entries = explode("\n", $entries);
  foreach ($entries as $entry) {
  	$entry = trim($entry);
  	$cleannumber = mc_clean_phone_number($entry);
  	if ($cleannumber === $entry) {
  	} else {
  		$error = true;
  	}
  }
  if ($error) form_set_error('mobiles', t('Check your numbers'));
}


  

function mc_campaign_quick_sms_submit($form, &$form_state) {
	
  $cost = 0;	
	
  $entries = $form_state['values']['mobiles'];
  $entries = explode("\n", $entries);
  $result = sms_clickatell_command('auth');
  
  if (isset($result['status']) && $result['status'] == TRUE) {
  	
  	$data['sid'] = $result['sid'];
  	$data['message'] = $form_state['values']['body'];
  	
    //$result = sms_clickatell_command('getbalance', $data);
    //$credit= round(preg_replace("/[^0-9\.]/", "", $result['data']),2);
    //$sms= round(preg_replace("/[^0-9\.]/", "", $result['data']) / 1.5);
    //$euro= round(((int) preg_replace("/[^0-9\.]/", "", $result['data'])) /15);
    //drupal_set_message("Initial Credit: ".$credit." = ".$sms." SMS (".$euro." Euro)", 'status');
    
    foreach ($entries as $entry) {
  
  	  $data['number'] = $entry;
  	  $data['options']['reference'] = '';
  	  
  	  $result = sms_clickatell_command('sendmsg', $data);
  	  
	  if ($result['status'] == TRUE) {
	  	$cost = $cost - $result['cost'];
	  	drupal_set_message(t('Message send to ').$entry, 'status');
	  }
	  
    }
    
    // UPDATE CREDIT
    //mc_campaign_insert_cost($cost);
    
    //$result = sms_clickatell_command('getbalance', $data);
    //$credit= round(preg_replace("/[^0-9\.]/", "", $result['data']),2);
    //$sms= round(preg_replace("/[^0-9\.]/", "", $result['data']) / 1.5);
    //$euro= round(((int) preg_replace("/[^0-9\.]/", "", $result['data'])) /15);
    //drupal_set_message("Initial Credit: ".$credit." = ".$sms." SMS (".$euro." Euro)", 'status');
        
  }

}

