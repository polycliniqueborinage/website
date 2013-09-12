<?php


/**
 * @file
 * Admin page callback file for the user module.
 */
function campaign_admin_user($callback_arg = '') {
	
	$op = isset($_POST['op']) ? $_POST['op'] : $callback_arg;

	switch ($op) {
  	
		case 'list':

			$build['campaign_admin_user_filter_form'] = drupal_get_form('campaign_admin_user_filter_form');
			$build['campaign_admin_user_list'] = drupal_get_form('campaign_admin_user_list');

			break;
  	
	}
	
  	return $build;

}


/**
 * Form builder; Campaign People List
 *
 */
function campaign_admin_user_list() {

	$header = array(
    	'username' => array('data' => t('Username'), 'field' => 'u.name'),
    	'roles' => array('data' => t('Roles')),
    	'field_user_first_name' => array('data' => t('First Name')),
    	'field_user_last_name' => array('data' => t('Last Name')),
    	'mail' => array('data' => t('Mail')),
    	'field_user_mobile_phone' => array('data' => t('Mobile')),
    	'field_user_address' => array('data' => t('Address')),
		'field_user_accept_mail' => array('data' => t('Mail')),
    	'field_user_check_sms' => array('data' => t('SMS')),
    	'field_user_accept_letter' => array('data' => t('Letter')),
    	'operations' => array('data' => t('Operations')),
  	);

  	$query = db_select('users', 'u');
  	$query->condition('u.uid', 0, '<>');
  	user_build_filter_query($query);

  	$count_query = clone $query;
  	$count_query->addExpression('COUNT(u.uid)');

  	$query = $query->extend('PagerDefault')->extend('TableSort');
  	$query
    	->fields('u', array('uid', 'name', 'mail'))
    	->limit(50)
    	->orderByHeader($header)
    	->setCountQuery($count_query);
  	$result = $query->execute();

  	$form['options'] = array(
    	'#type' => 'fieldset',
    	'#title' => t('Update options'),
    	'#attributes' => array('class' => array('container-inline')),
  	);
  	$options = array();
  	foreach (module_invoke_all('user_operations') as $operation => $array) {
    	$options[$operation] = $array['label'];
  	}
  	$form['options']['operation'] = array(
    	'#type' => 'select',
    	'#title' => t('Operation'),
    	'#title_display' => 'invisible',
    	'#options' => $options,
    	'#default_value' => 'unblock',
  	);
  	$options = array();
  	$form['options']['submit'] = array(
    	'#type' => 'submit',
    	'#value' => t('Update'),
  	);

  	$destination = drupal_get_destination();

  	$roles = array_map('check_plain', user_roles(TRUE));
  	
  	$accounts = array();
  
  	foreach ($result as $account) {
  		
  		$user_fields = user_load($account->uid);
  	
  		$field_user_first_name 		= field_get_items('user', $user_fields, 'field_user_first_name');
    	$field_user_last_name 		= field_get_items('user', $user_fields, 'field_user_last_name');
    	$field_user_mobile_phone 	= field_get_items('user', $user_fields, 'field_user_mobile_phone');
    	$field_user_address 		= field_get_items('user', $user_fields, 'field_user_address');
    	$field_user_accept_mail 	= field_get_items('user', $user_fields, 'field_user_accept_mail');
    	$field_user_accept_sms 		= field_get_items('user', $user_fields, 'field_user_accept_sms');
    	$field_user_accept_letter 	= field_get_items('user', $user_fields, 'field_user_accept_letter');
    	
    	$field_user_mobile_phone_label = 	cck_phone_countrycodes($field_user_mobile_phone[0]['country_codes']);
    	$field_user_mobile_phone_label = 	$field_user_mobile_phone_label['code'];
    	$field_user_mobile_phone_label .= 	$field_user_mobile_phone[0]['number'];
    	
    	//dpm($field_user_address);
    	
  		$users_roles = array();
    	$roles_result = db_query('SELECT rid FROM {users_roles} WHERE uid = :uid', array(':uid' => $account->uid));
    	foreach ($roles_result as $user_role) {
      		$users_roles[] = $roles[$user_role->rid];
    	}
    	asort($users_roles);

    	$options[$account->uid] = array(
    		'username' => theme('username', array('account' => $account)),
    		'roles' => theme('item_list', array('items' => $users_roles)),
    		'field_user_first_name' 	=> "".$field_user_first_name[0]['value'],
    		'field_user_last_name' 		=> "".$field_user_last_name[0]['value'],
    		'mail' 						=> $account->mail,
    		'field_user_address' 		=> "".$field_user_address[0]['postal_code']." ".$field_user_address[0]['locality']." ".$field_user_address[0]['name_line'],
    		'field_user_mobile_phone' 	=> "".$field_user_mobile_phone_label,
    		'field_user_accept_mail' 	=> "".$field_user_accept_mail[0]['value'],
    		'field_user_check_sms'		=> "".$field_user_accept_sms[0]['value'],
    		'field_user_accept_letter' 	=> "".$field_user_accept_letter[0]['value'],
      		'operations' 				=> array('data' => array('#type' => 'link', '#title' => t('edit'), '#href' => "user/$account->uid/edit", '#options' => array('query' => $destination))),
    	);
  	}

  	$form['accounts'] = array(
    	'#type' => 'tableselect',
    	'#header' => $header,
    	'#options' => $options,
    	'#empty' => t('No user available.'),
	  	'#js_select' => TRUE,
  	);
  	
  	$form['pager'] = array('#markup' => theme('pager'));

  	return $form;
  
}


function campaign_admin_user_filter_form() {
	
	$session = isset($_SESSION['campaign_admin_user_list_filter']) ? $_SESSION['campaign_admin_user_list_filter'] : array();
  
  	$filters = campaign_admin_user_filters();

  	$i = 0;
  
  	$form['filters'] = array(
    	'#type' => 'fieldset',
    	'#title' => t('Show only users where'),
    	'#theme' => 'exposed_filters__user',
  	);
  
  	foreach ($session as $filter) {
    
  		list($type, $value) = $filter;
    
  		if ($type == 'permission') {
      		// Merge arrays of module permissions into one.
      		// Slice past the first element '[any]' whose value is not an array.
      		$options = call_user_func_array('array_merge', array_slice($filters[$type]['options'], 1));
      		$value = $options[$value];
    	}	else {
      		$value = $filters[$type]['options'][$value];
    	}
    	$t_args = array('%property' => $filters[$type]['title'], '%value' => $value);
    	if ($i++) {
      		$form['filters']['current'][] = array('#markup' => t('and where %property is %value', $t_args));
    	} else {
      		$form['filters']['current'][] = array('#markup' => t('%property is %value', $t_args));
    	}
  	}

  	$form['filters']['status'] = array(
    	'#type' => 'container',
    	'#attributes' => array('class' => array('clearfix')),
    	'#prefix' => ($i ? '<div class="additional-filters">' . t('and where') . '</div>' : ''),
  	);
  	
  	$form['filters']['status']['filters'] = array(
    	'#type' => 'container',
    	'#attributes' => array('class' => array('filters')),
  	);
  	
  	foreach ($filters as $key => $filter) {
    	$form['filters']['status']['filters'][$key] = array(
      		'#type' => 'select',
      		'#options' => $filter['options'],
      		'#title' => $filter['title'],
      		'#default_value' => '[any]',
    	);
  	}

  	$form['filters']['status']['actions'] = array(
    	'#type' => 'actions',
    	'#attributes' => array('class' => array('container-inline')),
  	);
  	
  	$form['filters']['status']['actions']['submit'] = array(
    	'#type' => 'submit',
    	'#value' => (count($session) ? t('Refine') : t('Filter')),
  	);
  
  	if (count($session)) {
    	$form['filters']['status']['actions']['undo'] = array(
      		'#type' => 'submit',
      		'#value' => t('Undo'),
    	);
    	$form['filters']['status']['actions']['reset'] = array(
      		'#type' => 'submit',
      		'#value' => t('Reset'),
    	);
  	}

  	drupal_add_library('system', 'drupal.form');

  	return $form;
  
}


/**
 * fork of user_filters
 */
function campaign_admin_user_filters() {
  	// Regular filters
  	$filters = array();
  	$roles = user_roles(TRUE);
  	unset($roles[DRUPAL_AUTHENTICATED_RID]); // Don't list authorized role.
  	if (count($roles)) {
    	$filters['role'] = array(
      		'title' => t('role'),
      		'field' => 'ur.rid',
      		'options' => array(
        		'[any]' => t('any'),
      		) + $roles,
    	);
  	}

  	return $filters;
}


