<?php

function mc_campaign_format_status($status) {
  switch ($status) {
    case '004':
      $img = "<img src = '/sites/all/themes/mctimetracker/icons/email_open.png' title='Received by recipient' />";
      break;
    case '003':
      $img = "<img src = '/sites/all/themes/mctimetracker/icons/email.png' title='Sent' />";
      break;
    case '002':
      $img = "<img src = '/sites/all/themes/mctimetracker/icons/email.png' title='Sent' />";
      break;
    case '000':
      $img = "<img src = '/sites/all/themes/mctimetracker/icons/email.png' title='Sent' />";
      break;
    default :
      $img = "<img src = '/sites/all/themes/mctimetracker/icons/alarm.png' title='Scheduled' />";
  }
  return $img;
}




function mc_clean_mobile_number($phone) {
  $phone = preg_replace("/^[+][3][2]/", "", $phone);
  $phone = preg_replace("/[^0-9]/", "", $phone);
  $phone = preg_replace("/^[0]/", "", $phone);
  // todo use user default prefix
  // if no country prefix we will use belgian prefix
  if (strlen($phone)==9) $phone = "32".$phone;
  if (strlen($phone)<3) $phone = "";
  return $phone;
}

function mc_clean_fixedline_number($phone) {
  $phone = preg_replace("/^[+][3][2]/", "", $phone);
  $phone = preg_replace("/[^0-9]/", "", $phone);
  $phone = preg_replace("/^[0]/", "", $phone);
  // todo use user default prefix
  // if no country prefix we will use belgian prefix
  if (strlen($phone)==9) $phone = "32".$phone;
  if (strlen($phone)<3) $phone = "";
  return $phone;
}
  
/**
 * Checks whether a subscriber is already registered.
 * @return
 *  boolean TRUE or FALSE.
 */
function mc_campaign_is_subscribed(&$subscriber) {
	
  // FIRST CLEAN MAIL AND MOBILE
  $subscriber->email = valid_email_address($subscriber->email) ? $subscriber->email : '';
  
  // CLEAN MOBILE
  $subscriber->mobile = mc_clean_mobile_number($subscriber->mobile);
  
  // CLEAN FIXEDLINE
  $subscriber->fixedline = mc_clean_fixedline_number($subscriber->fixedline);
  
  // Try to match eid
  // IF EID IS GIVEN DO NOT CHECK EMAIL OR MOBILE NUMBER
  // WHY ? if eid give but subscriber already in the list we may need to update it
  if (isset($subscriber->seid) && $subscriber->seid!='') {
    $result = db_query('SELECT * FROM {mc_campaign_subscribers} WHERE eid = :eid AND did = :did', array(':eid' => $subscriber->seid,':did' => $subscriber->did))->fetchObject();
    if ($result){
    	$subscriber = $result; 
    	return TRUE;
    } 
  }
  
  // Try to match email
  if (isset($subscriber->email) && $subscriber->email!='') {
    $result = db_query('SELECT * FROM {mc_campaign_subscribers} WHERE email = :email AND did = :did', array(':email' => $subscriber->email,':did' => $subscriber->did))->fetchObject();
    if ($result){
    	$subscriber = $result;
    	return TRUE;
    } 
  }
  
  // Try to match mobile
  /*if (isset($subscriber->mobile) && $subscriber->mobile!='') {
    $result = db_query('SELECT * FROM {mc_campaign_subscribers} WHERE mobile = :mobile AND did = :did', array(':mobile' => $subscriber->mobile,':did' => $subscriber->did))->fetchObject();
    if ($result){
    	$subscriber = $result; 
    	return TRUE;
    }
  }*/
  
  return FALSE;
}

/**
 * Saves a new subscriber in database.
 *
 * @param $subscriber
 *   An object conatining the following:
 *    email - Required.
 *    uid - Optional.
 *    firstname - Optional.
 *    lastname - Optional.
 *    mobile - Optional.
 * @return
 *   The id of the newly created subscriber will be created since it the object
 *   is passed by reference.boolean FALSE if the insertion fails.
 */
function mc_campaign_save_subscriber(&$subscriber) {
  
  $subscriber->sid = db_insert('mc_campaign_subscribers')
    ->fields(array(
      'did' => $subscriber->did,
      'email' => isset($subscriber->email) ? $subscriber->email : NULL,
      'mobile' => isset($subscriber->mobile) ? $subscriber->mobile : NULL,
      'eid' => isset($subscriber->seid) ? $subscriber->seid : NULL,
      'uid' => isset($subscriber->uid) ? $subscriber->uid : NULL,
      'firstname' => isset($subscriber->firstname) ? $subscriber->firstname : NULL,
      'lastname' => isset($subscriber->lastname) ? $subscriber->lastname : NULL,
      'fixedline' => isset($subscriber->fixedline) ? $subscriber->fixedline : NULL,
      'im' => isset($subscriber->im) ? $subscriber->im : NULL,
      'skype' => isset($subscriber->skype) ? $subscriber->skype : NULL,
      'facebook' => isset($subscriber->facebook) ? $subscriber->facebook : NULL,
      'twitter' => isset($subscriber->twitter) ? $subscriber->twitter : NULL,
      'birthday' => isset($subscriber->birthday) ? $subscriber->birthday : NULL,
      'email_allowed' => isset($subscriber->email_allowed) ? $subscriber->email_allowed : 1,
      'mobile_allowed' => isset($subscriber->mobile_allowed) ? $subscriber->mobile_allowed : 1,
      'fixedline_allowed' => isset($subscriber->fixedline_allowed) ? $subscriber->fixedline_allowed : 1,
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
  ))->execute();
  
}

/**
 * Saves a new subscriber in database.
 *
 * @param $subscriber
 *   An object conatining the following:
 *    email - Required.
 *    uid - Optional.
 *    firstname - Optional.
 *    lastname - Optional.
 *    mobile - Optional.
 * @return
 *   The id of the newly created subscriber will be created since it the object
 *   is passed by reference.boolean FALSE if the insertion fails.
 */
function mc_campaign_update_subscriber(&$subscriber) {
  
  db_update('mc_campaign_subscribers')
    ->fields(array(
      'email' => isset($subscriber->email) ? $subscriber->email : NULL,
      'mobile' => isset($subscriber->mobile) ? $subscriber->mobile : NULL,
      'eid' => isset($subscriber->seid) ? $subscriber->seid : NULL,
      'uid' => isset($subscriber->uid) ? $subscriber->uid : NULL,
      'firstname' => isset($subscriber->firstname) ? $subscriber->firstname : NULL,
      'lastname' => isset($subscriber->lastname) ? $subscriber->lastname : NULL,
      'fixedline' => isset($subscriber->fixedline) ? $subscriber->fixedline : NULL,
      'im' => isset($subscriber->im) ? $subscriber->im : NULL,
      'skype' => isset($subscriber->skype) ? $subscriber->skype : NULL,
      'facebook' => isset($subscriber->facebook) ? $subscriber->facebook : NULL,
      'twitter' => isset($subscriber->twitter) ? $subscriber->twitter : NULL,
      'birthday' => isset($subscriber->birthday) ? $subscriber->birthday : NULL,
      'email_allowed' => isset($subscriber->email_allowed) ? $subscriber->email_allowed : 1,
      'mobile_allowed' => isset($subscriber->mobile_allowed) ? $subscriber->mobile_allowed : 1,
      'fixedline_allowed' => isset($subscriber->fixedline_allowed) ? $subscriber->fixedline_allowed : 1,
      'updated' => REQUEST_TIME,
    ))
    ->condition('sid', $subscriber->sid)
    ->execute();
  
}



function mc_campaign_get_subscriber($sid) {

  $domain = domain_get_domain();
  
  return db_select('mc_campaign_subscribers','s')
    ->fields('s')
    ->condition('sid', $sid)
    ->condition('s.did',$domain['domain_id'])
    ->execute()->fetch();
  
}

function mc_campaign_get_list($lid) {

  $domain = domain_get_domain();
  
  return db_select('mc_campaign_lists','l')
    ->fields('l')
    ->condition('lid', $lid)
    ->condition('l.did',$domain['domain_id'])
    ->execute()->fetch();
  
}

function mc_campaign_get_subscriber_from_list($lid) {

  $domain = domain_get_domain();

  $query = db_select('mc_campaign_subscribers','s');
  $query->join('mc_campaign_lists_subscribers', 'ls', 'ls.sid = s.sid');
  $query->fields('s');
  return $query->condition('ls.lid', $lid)
    ->condition('ls.deleted', 0)
    ->condition('s.did',$domain['domain_id'])
    ->execute()->fetchAll();
  
}







/**
 * Checks whether a user is registered to a list or not and remove semaphore if exist
 * @return
 *  boolean TRUE or FALSE.
 */
function mc_campaign_imc_campaign_is_subscribed_to_list(&$subscriber) {
	
  // Try to match eid
  // IF EID IS GIVEN DO NOT CHECK FURTHER
  // CHECK EID AND LID
  if (isset($subscriber->leid) && $subscriber->leid!='') {
    $is_subscribed = db_query('SELECT id FROM {mc_campaign_lists_subscribers} WHERE eid = :eid AND lid = :lid', array(':eid' => $subscriber->leid,':lid' => $subscriber->lid))->fetchField();
    if ($is_subscribed){
    	$subscriber->id = $is_subscribed;
    	return TRUE;
    } else {
    	return FALSE;
    }
  }
  
  $is_subscribed = db_query('SELECT id FROM {mc_campaign_lists_subscribers} WHERE lid = :lid AND sid = :sid AND deleted = 0', 
  array(':lid' => $subscriber->lid,':sid' => $subscriber->sid)
  )->fetchField();
  
  if ($is_subscribed) {
  	$subscriber->id = $is_subscribed;
  	return TRUE;
  } else {
  	return FALSE;
  } 
}


function mc_campaign_save_subscriber_to_list($subscriber) {
  
  db_insert('mc_campaign_lists_subscribers')
    ->fields(array(
      'lid' => $subscriber->lid,
      'sid' => $subscriber->sid,
	  'eid' => (isset($subscriber->leid)) ? $subscriber->leid : null,  
	  'sent_time' => (isset($subscriber->sent_time)) ? $subscriber->sent_time : null,
	  'info1' => (isset($subscriber->info1)) ? $subscriber->info1 : null, 
	  'info2' => (isset($subscriber->info2)) ? $subscriber->info2 : null,
	  'info3' => (isset($subscriber->info3)) ? $subscriber->info3 : null,
      'info4' => (isset($subscriber->info4)) ? $subscriber->info4 : null,
	  'info5' => (isset($subscriber->info5)) ? $subscriber->info5 : null,
	  'info6' => (isset($subscriber->info6)) ? $subscriber->info6 : null,
      'info7' => (isset($subscriber->info7)) ? $subscriber->info7 : null,
      'info8' => (isset($subscriber->info8)) ? $subscriber->info8 : null, 
	  'info9' => (isset($subscriber->info0)) ? $subscriber->info9 : null,
	  'info10' => (isset($subscriber->info10)) ? $subscriber->info10 : null,
      'semaphore' => 0,
      'deleted' => 0,
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
  ))->execute();
  
}

function mc_campaign_update_subscriber_to_list($subscriber) {
  
  //REMOVE SEMAPHORE
  db_update('mc_campaign_lists_subscribers')
        ->fields(array(
        'semaphore' => 0,
        'sent_time' => (isset($subscriber->sent_time)) ? $subscriber->sent_time : null,
	    'info1' => (isset($subscriber->info1)) ? $subscriber->info1 : null, 
	    'info2' => (isset($subscriber->info2)) ? $subscriber->info2 : null,
	    'info3' => (isset($subscriber->info3)) ? $subscriber->info3 : null,
        'info4' => (isset($subscriber->info4)) ? $subscriber->info4 : null,
	    'info5' => (isset($subscriber->info5)) ? $subscriber->info5 : null,
	    'info6' => (isset($subscriber->info6)) ? $subscriber->info6 : null,
        'info7' => (isset($subscriber->info7)) ? $subscriber->info7 : null,
        'info8' => (isset($subscriber->info8)) ? $subscriber->info8 : null, 
	    'info9' => (isset($subscriber->info0)) ? $subscriber->info9 : null,
	    'info10' => (isset($subscriber->info10)) ? $subscriber->info10 : null,
        'updated' => REQUEST_TIME,
    ))
    ->condition('id', $subscriber->id)
    ->execute();
  
}


function mc_campaign_save_unsubscriber_to_list($subscriber) {
  
  // TODO : secure
  //dpm($subscriber);
  
  db_delete('mc_campaign_lists_subscribers')
    //->leftJoin('mc_campaign_subscribers', 'subscribers.sid = lists_subscribers.sid')
    ->condition('sid', $subscriber->sid)
    ->condition('lid', $subscriber->lid)
    //->condition('did', $subscriber->did)
    ->execute();
  
}



































function mc_campaign_action_notification_entry_insert($entry) {
	drupal_set_message('mc_campaign_action_notification_entry_insert');
	$return_value = NULL;
  	try {
    	$return_value = db_insert('mcmc_campaign_notification')
                    ->fields($entry)
                    ->execute();
  	}
  	catch (Exception $e) {
    	drupal_set_message(t('db_insert failed. Message = %message, query= %query',array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
  	}
  	return $return_value;
}


function mc_campaign_action_notification_entry_update($entry) {

	drupal_set_message('mc_campaign_action_notification_entry_update');

	$return_value = NULL;
	try {
    	$count = db_update('mcmc_campaign_notification')
              ->fields($entry)
              ->condition('cid', $entry['cid'])
              ->condition('id', $entry['id'])
              ->execute();
  	}
  	catch (Exception $e) {
    	drupal_set_message(t('db_update failed. Message = %message, query= %query',array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
  	}
  	return $return_value;
}


function mc_campaign_admin_mc_campaign_entry_load($entry = array()) {
  
	$select = db_select('mccampaign');
  	$select->fields('mccampaign');
  	$select->condition('deleted', 0);
  	
  	// Add each field and value as a condition to this query.
  	foreach ($entry as $field => $value) {
    	$select->condition($field, $value);
  	}
  	// Return the result in object format.
  	return $select->execute()->fetchAll();

}

/**
 * CAMPAIGN
 * @param $entry
 */

function mc_campaign_admin_mc_campaign_entry_insert($entry) {
	$return_value = NULL;
  	try {
    	$return_value = db_insert('mccampaign')
                    ->fields($entry)
                    ->execute();
  	}
  	catch (Exception $e) {
    	drupal_set_message(t('db_insert failed. Message = %message, query= %query',array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
  	}
  	return $return_value;
}


function mc_campaign_admin_mc_campaign_entry_update($entry) {
	try {
    $count = db_update('mccampaign')
              ->fields($entry)
              ->condition('cid', $entry['cid'])
              ->execute();
  	}
  	catch (Exception $e) {
    	drupal_set_message(t('db_update failed. Message = %message, query= %query',array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
  	}
	return $count;
}


function mc_campaign_admin_mc_campaign_entry_delete($entry) {
	db_delete('dbtng_example')
    ->condition('pid', $entry['pid'])
    ->execute();
}