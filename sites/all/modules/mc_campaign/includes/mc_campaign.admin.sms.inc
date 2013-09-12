<?php

function mc_campaign_sms_list() {

  // Domain
  $domain = domain_get_domain();
	
  $build['mc_campaign_sms_list_filter_form'] = drupal_get_form('mc_campaign_sms_list_filter_form');
  	
  //$build['mc_campaign_sms_list_form_action_form'] = drupal_get_form('mc_campaign_sms_list_clear_filter_form');
	
  $header = array(
    'cid' => array('data' => 'Campaign', 'field' => 'c.title'),
    'lid' => array('data' => 'List', 'field' => 'l.title'),
    'name' => array('data' => 'Name', 'field' => 's.firstname'),
    'contact' => array('data' => 'Contact', 'field' => 'contact'),
    'message' => array('data' => 'Message', 'field' => 'n.message'),
    'sent' => array('data' => 'Sent Time', 'field' => 'n.sent', 'sort' => 'desc'),
    'status' => array('data' => 'Status', 'field' => 'n.status'),
    'received' => array('data' => 'Reveived', 'field' => 'n.received'),
    'credit' => array('data' => 'Credit', 'field' => 'n.credit'),
  );
	
  $rows = array();

  $select = db_select('mc_campaign_notifications','n')->extend('PagerDefault')->extend('TableSort');
  $select->join('mc_campaign_campaigns', 'c', 'c.cid = n.cid');
  $select->join('mc_campaign_lists', 'l', 'l.lid = n.lid');
  $select->join('mc_campaign_subscribers', 's', 's.sid = n.sid');
  $select->condition('n.deleted', 0);
  $select->condition('n.sent', 0, '>');
  $select->condition('n.media', 'sms');
  $select->condition('n.did',$domain['domain_id']);
  $select->fields('n',array('cid','lid','media','contact','created','estimate_sent','sent','message','received','status','cost','credit'));
  $select->fields('c',array('title'));
  $select->fields('l',array('title'));
  $select->fields('s',array('firstname','lastname'));

  if (!empty($_SESSION['mc_campaign_sms_list_build_filter']['lid']))
  $select->condition('n.lid', $_SESSION['mc_campaign_sms_list_build_filter']['lid'] ,'IN');
  
  if (!empty($_SESSION['mc_campaign_sms_list_build_filter']['cid']))
  $select->condition('n.cid', $_SESSION['mc_campaign_sms_list_build_filter']['cid'] ,'IN');
  
  $results = $select
	->limit(30)
	->orderByHeader($header)
	->execute();
	
  	foreach ($results as $entry) {
  		$row['cid'] 			= $entry->title;
  		$row['lid'] 			= $entry->l_title;
  		$row['name'] 			= $entry->firstname.' '.$entry->lastname;
  		$row['contact'] 		= $entry->contact;
    	$row['message'] 		= substr($entry->message,0,100);
    	$row['sent'] 			= format_date($entry->sent, 'short');
    	$row['status'] 			= format_status($entry->status);
    	$row['received'] 		= ($entry->received == 0) ? t('Not Yet') : format_date($entry->received, 'short');
    	$row['credit'] 			= $entry->credit;
    	$rows[] = $row;
  	}
   	
  	$build['statistics_table'] = array(
    	'#theme' => 'table',
    	'#header' => $header,
    	'#rows' => $rows,
    	'#empty' => t('No notification available.'),
  	);
  
  	$build['statistics_pager'] = array('#theme' => 'pager');
  
  	return $build;
  
}


function mc_campaign_sms_list_filter_form($form) {
	
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter SMS'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($_SESSION['mc_campaign_sms_list_build_filter']),
  );
  
  $lists = db_query('SELECT lid, title FROM {mc_campaign_lists}')->fetchAllKeyed();
  $form['filters']['lid'] = array(
    '#type' => 'select',
    '#title' => t('List'),
    '#options' => array_filter($lists, 'check_plain'),
    '#multiple' => TRUE,
    '#size' => 8,
    '#default_value' => (isset($_SESSION['mc_campaign_sms_list_build_filter']['lid'])) ? $_SESSION['mc_campaign_sms_list_build_filter']['lid'] : null,
  );
  $campaigns = db_query('SELECT cid, title FROM {mc_campaign_campaigns}')->fetchAllKeyed();
  $form['filters']['cid'] = array(
    '#type' => 'select',
    '#title' => t('Campaign'),
    '#options' => array_filter($campaigns, 'check_plain'),
    '#multiple' => TRUE,
    '#size' => 8,
    '#default_value' => (isset($_SESSION['mc_campaign_sms_list_build_filter']['cid'])) ? $_SESSION['mc_campaign_sms_list_build_filter']['cid'] : null,
  );
  
  $form['filters']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['filters']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
  if (!empty($_SESSION['mc_campaign_sms_list_build_filter'])) {
    $form['filters']['actions']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset')
    );
  }

  return $form;
  
}


function mc_campaign_sms_list_filter_form_validate($form, &$form_state) {
  if ($form_state['values']['op'] == t('Filter') && empty($form_state['values']['lid']) && empty($form_state['values']['cid'])) {
    form_set_error('type', t('You must select something to filter by.'));
  }
}


function mc_campaign_sms_list_filter_form_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  switch ($op) {
    case t('Filter'):
      if (isset($form_state['values']['lid']))  $_SESSION['mc_campaign_sms_list_build_filter']['lid'] = $form_state['values']['lid'];
      if (isset($form_state['values']['cid']))  $_SESSION['mc_campaign_sms_list_build_filter']['cid'] = $form_state['values']['cid'];
      break;
    case t('Reset'):
      $_SESSION['mc_campaign_sms_list_build_filter'] = array();
      break;
  }
  return 'admin/campaign/sms';
}







































function format_status($status) {
  switch($status) {
  	case '001' : 
    return 'Message unknown';
    break;		
  	case '002' : 
    return 'Message queued';
    break;		
  	case '003' : 
    return 'Delivered to gateway';
    break;		
    case '004' : 
    return 'Received by recipient';
    break;		
    case '005' : 
    return 'Error with message';
    break;		
    case '006' : 
    return 'User cancelled message delivery';
    break;		
    case '007' : 
    return 'Error delivering message';
    break;		
    case '008' : 
    return 'OK';
    break;		
    case '009' : 
    return 'Routing error';
    break;		
    case '010' : 
    return 'Message expired';
    break;		
    case '011' : 
    return 'Message queued for later delivery';
    break;		
    case '012' : 
    return 'Out of credit';
    break;
    default:
    return 'Unknow';		
  }	
}
	




function campaign_admin_sms_list_build_filters() {
	
  	$filters = array();

  	foreach (mc_campaign_admin_campaign_entry_load() as $type) {
    	$types[$type->cid] = $type->name;
  	}

  	if (!empty($types)) {
    	$filters['type'] = array(
      		'title' => t('Type'),
      		'where' => "c.cid = ?",
      		'options' => $types,
    	);
  	}

  	return $filters;

}


function campaign_admin_sms_filter_form($form) {
	
  	$filters = campaign_admin_sms_list_build_filters();
  	
  	$form['filters'] = array(
    	'#type' => 'fieldset',
    	'#title' => t('Filter'),
    	'#collapsible' => TRUE,
    	'#collapsed' => empty($session),
  	);
  
  	foreach ($filters as $key => $filter) {
    	$form['filters']['status'][$key] = array(
      		'#title' => $filter['title'],
      		'#type' => 'select',
      		'#multiple' => TRUE,
      		'#size' => 8,
      		'#options' => $filter['options'],
    	);
    	if (!empty($_SESSION['campaign_admin_sms_list_build_filter'][$key])) {
      		$form['filters']['status'][$key]['#default_value'] = $_SESSION['campaign_admin_sms_list_build_filter'][$key];
    	}
  	}

  	$form['filters']['actions'] = array(
    	'#type' => 'actions',
    	'#attributes' => array('class' => array('container-inline')),
  	);
  
  	$form['filters']['actions']['submit'] = array(
    	'#type' => 'submit',
    	'#value' => t('Filter'),
  	);
  
  	if (!empty($_SESSION['campaign_admin_sms_list_build_filter'])) {
    	$form['filters']['actions']['reset'] = array(
      		'#type' => 'submit',
      		'#value' => t('Reset')
    	);
  	}

  	return $form;

}


function campaign_admin_sms_filter_form_validate($form, &$form_state) {

	if ($form_state['values']['op'] == t('Filter') && empty($form_state['values']['type']) && empty($form_state['values']['severity'])) {
		form_set_error('type', t('You must select something to filter by.'));
  	}

}


function campaign_admin_sms_filter_form_submit($form, &$form_state) {
  
	$op = $form_state['values']['op'];
  	$filters = campaign_admin_sms_list_build_filters();
  	
  	switch ($op) {
    	case t('Filter'):
      	
    		foreach ($filters as $name => $filter) {
        		if (isset($form_state['values'][$name])) {
          			$_SESSION['campaign_admin_sms_list_build_filter'][$name] = $form_state['values'][$name];
        		}
      		}
      	break;
    	case t('Reset'):
      		$_SESSION['campaign_admin_sms_list_build_filter'] = array();
      	break;
  	}
  	return 'admin/campaign/sms';

}


function campaign_admin_sms_filter_form_action_form($form) {
  $form['dblog_clear'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clear log messages'),
    '#description' => t('This will permanently remove the log messages from the database.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['dblog_clear']['clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear log messages'),
    '#submit' => array('dblog_clear_log_submit'),
  );

  return $form;
}


function campaign_admin_sms_filter_form_action_form_submit() {
  	$_SESSION['dblog_overview_filter'] = array();
  	db_delete('watchdog')->execute();
  	drupal_set_message(t('Database log cleared.'));
}
