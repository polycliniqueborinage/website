<?php

function campaign_admin_letter_list() {
	
	$filter = campaign_admin_letter_list_build_filter_query();
	
  	$build['campaign_admin_letter_filter_form'] = drupal_get_form('campaign_admin_letter_filter_form');
  	
  	//$build['campaign_admin_letter_filter_form_action_form'] = drupal_get_form('campaign_admin_letter_filter_form_action_form');
	
	$header = array(
    	'cid' => array('data' => 'Campaign', 'field' => 'c.name'),
    	'first_name' => array('data' => 'First Name', 'field' => 'fn.field_user_first_name_value'),
    	'last_name' => array('data' => 'Last Name', 'field' => 'ln.field_user_last_name_value'),
    	'created' => array('data' => 'Created', 'field' => 'n.created','sort' => 'desc'),
    	'updated' => array('data' => 'Updated', 'field' => 'n.updated'),
    	'sent' => array('data' => 'Sent', 'field' => 'n.sent'),
    );
  
    $rows = array();

	$select = db_select('mccampaign_notification','n')->extend('PagerDefault')->extend('TableSort');
  	$select->join('users', 'u', 'u.uid = n.uid');
  	$select->join('mccampaign', 'c', 'c.cid = n.cid');
  	$select->join('field_data_field_user_first_name', 'fn', 'fn.entity_id = u.uid');
  	$select->join('field_data_field_user_last_name', 'ln', 'ln.entity_id = u.uid');
	$select->where('n.deleted' . ' = ' . 0);
	$select->where("c.method = 'letter'");
	
	if (!empty($filter['where'])) {
    	$select->where($filter['where'], $filter['args']);
  	}
	
	$results = $select
		->fields('n')
		->fields('fn')
		->fields('ln')
		->fields('c')
		->limit(30)
		->orderByHeader($header)
		->execute();
	
  	foreach ($results as $entry) {
  		
  		$row['cid'] 			= $entry->name;
    	$row['first_name'] 		= $entry->field_user_first_name_value;
    	$row['last_name'] 		= $entry->field_user_last_name_value;
    	$row['created'] 		= format_interval(REQUEST_TIME - $entry->created);
    	$row['updated'] 		= format_interval(REQUEST_TIME - $entry->updated);
    	$row['sent'] 			= format_interval(REQUEST_TIME - $entry->sent);
  		
    	$rows[] = $row;
   	
  	}
   	
  	$build['statistics_table'] = array(
    	'#theme' => 'table',
    	'#header' => $header,
    	'#rows' => $rows,
    	'#empty' => t('No letter available.'),
  	);
  
  	$build['statistics_pager'] = array('#theme' => 'pager');
  
  	return $build;
  	
  
}


function campaign_admin_letter_list_build_filter_query() {
	
  	if (empty($_SESSION['campaign_admin_letter_list_build_filter'])) {
    	return;
  	}

  	$filters = campaign_admin_letter_list_build_filters();

  	// Build query
  	$where = $args = array();
  	
  	foreach ($_SESSION['campaign_admin_letter_list_build_filter'] as $key => $filter) {
    
  		$filter_where = array();
    	foreach ($filter as $value) {
    		$filter_where[] = $filters[$key]['where'];
      		$args[] = $value;
    	}
    	
    	if (!empty($filter_where)) {
      		$where[] = '(' . implode(' OR ', $filter_where) . ')';
    	}
  	}
  	$where = !empty($where) ? implode(' AND ', $where) : '';

  	return array(
    	'where' => $where,
    	'args' => $args,
  	);
}


function campaign_admin_letter_list_build_filters() {
	
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


function campaign_admin_letter_filter_form($form) {
	
  	$filters = campaign_admin_letter_list_build_filters();
  	
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
    	if (!empty($_SESSION['campaign_admin_letter_list_build_filter'][$key])) {
      		$form['filters']['status'][$key]['#default_value'] = $_SESSION['campaign_admin_letter_list_build_filter'][$key];
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
  
  	if (!empty($_SESSION['campaign_admin_letter_list_build_filter'])) {
    	$form['filters']['actions']['reset'] = array(
      		'#type' => 'submit',
      		'#value' => t('Reset')
    	);
  	}

  	return $form;

}


function campaign_admin_letter_filter_form_validate($form, &$form_state) {

	if ($form_state['values']['op'] == t('Filter') && empty($form_state['values']['type']) && empty($form_state['values']['severity'])) {
		form_set_error('type', t('You must select something to filter by.'));
  	}

}


function campaign_admin_letter_filter_form_submit($form, &$form_state) {
  
	$op = $form_state['values']['op'];
  	$filters = campaign_admin_letter_list_build_filters();
  	
  	switch ($op) {
    	case t('Filter'):
      	
    		foreach ($filters as $name => $filter) {
        		if (isset($form_state['values'][$name])) {
          			$_SESSION['campaign_admin_letter_list_build_filter'][$name] = $form_state['values'][$name];
        		}
      		}
      	break;
    	case t('Reset'):
      		$_SESSION['campaign_admin_letter_list_build_filter'] = array();
      	break;
  	}
  	return 'admin/campaign/letter';

}


function campaign_admin_letter_filter_form_action_form($form) {
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


function campaign_admin_letter_filter_form_action_form_submit() {
  	$_SESSION['dblog_overview_filter'] = array();
  	db_delete('watchdog')->execute();
  	drupal_set_message(t('Database log cleared.'));
}
