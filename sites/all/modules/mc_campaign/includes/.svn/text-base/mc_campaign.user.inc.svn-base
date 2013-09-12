<?php

/**
 * listing of all lists
 */
function mc_campaign_user_list($form, &$form_state) {
	
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
    $form['list'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['values']['list'],
    );
    $output = t('Are you sure you want to delete the following newsletter lists?');
    $output .= '<ul>';
    foreach ($form_state['values']['list'] as $lid) {
      $list = newsletter_lid_load($lid);
      $output .= !empty($list) ? '<li>' . check_plain($list->title) . '</li>' : '';
    }
    $output .= '</ul>';
    $output .= t('This action cannot be undone.');
    return confirm_form($form, t('Delete the following?'), 'admin/campaign/lists', filter_xss($output));
  }

  // Action
  $form['add']['add'] = array(
    '#markup' => l(t('Add user'), 'account/users/add', array('attributes' => array('class' => 'button_left'))),
  );
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options['delete'] = t('Delete the selected users');
  $options['block'] = t('Block the selected users');
  $options['unblock'] = t('Unblock the selected users');
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
    'username' => array('data' => t('Username'), 'field' => 'name'),
    'mail' => array('data' => t('Mail'), 'field' => 'mail'),
    'status' => array('data' => t('Status'), 'field' => 'status'),
    'member_for' => array('data' => t('Member for'), 'field' => 'created', 'sort' => 'desc'),
    'access' => array('data' => t('Last access'), 'field' => 'access'),
    'operations' => array('data' => t('Operations')),
  );
  
  $query = db_select('users','u')->extend('PagerDefault')->extend('TableSort');
  $query->join('domain_editor', 'd', 'd.uid = u.uid');
  $query->join('users_roles', 'ur', 'ur.uid = u.uid');
  $query->fields('u');
  $query->fields('d',array('domain_id'));
  $query->fields('ur',array('rid'));
  $query = $query
    ->limit(50)
    ->orderByHeader($header)
    ->condition('d.domain_id', $domain['domain_id']);
    
  $result = $query->execute();
    
  $options = array();
  $destination = drupal_get_destination();
  foreach ($result as $row) {
    $operation = array();
    if ($row->rid == 5) {
      $operation[] = array(
        '#type' => 'link',
        '#title' => t('Edit'),
        '#href' => 'account/users/edit/' . $row->uid,
        '#options' => array('query' => $destination),
      );
      $operation[] = array(
        '#markup' => ' | ',
      );
      $operation[] = array(
        '#type' => 'link',
        '#title' => t('Delete'),
        '#href' => 'account/users/delete/' . $row->uid,
        '#options' => array('query' => $destination),
      );
    }
    $status = array(t('blocked'), t('active'));
    $options[$row->uid] = array(
      'username' => theme('username', array('account' => $row)),
      'mail' => $row->mail,
      'status' => $status[$row->status],
      'member_for' => format_interval(REQUEST_TIME - $row->created),
      'access' => $row->access ? t('@time ago', array('@time' => format_interval(REQUEST_TIME - $row->access))) : t('never'),
      'operations' => array(
        'data' => $operation,
      ),
    );  
  }

  $form['list'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    // BUG : need to disable all select as it will select the checkbox inside the table
    '#js_select' => FALSE,
    '#empty' => t('No lists available.'),
  );

  $form['pager'] = array('#theme' => 'pager');
  
  return $form;
  
}

function mc_campaign_user_list_submit($form, &$form_state) {

  // Domain
  $domain = domain_get_domain();
  
  $operation = $form_state['values']['operation'];
  $list = $form_state['values']['list'];
  if ($operation) {
    switch ($operation) {
      case 'unblock':
        foreach ($list as $uid => $value) {
          if ($value != 0) {
            $num_updated = db_update('users')
            ->fields(array('status' => 1))
            ->condition('uid', $uid)
            ->execute();
          }
        }  
        break;
      case 'block':
        foreach ($list as $uid => $value) {
          if ($value != 0) {
            $user = user_load($uid);
            if (isset($user->roles[5]) && isset($user->domain_user[$domain['domain_id']])) {
              $num_updated = db_update('users')
                ->fields(array('status' => 0))
                ->condition('uid', $uid)
                ->execute();
            }
          }
        }  
        break;
      case 'delete':
        foreach ($list as $uid => $value) {
          if ($value != 0) {
            $user = user_load($uid);
            if (isset($user->roles[5]) && isset($user->domain_user[$domain['domain_id']])) {
              $num_updated = db_delete('users')
                ->condition('uid', $uid)
                ->execute();
            }
          }
        }
        break;
    }
    isset($num_updated)
      ? drupal_set_message(t('Users updated successfully'), 'status')
      : drupal_set_message(t('Please select at least one user'), 'error');
  }
  else {
    drupal_set_message(t('Please select an action to perform.'), 'warning');
  }
}



/**
 * Menu callback; edit a subscribers list.
 */
function mc_campaign_user_edit($form, &$form_state, $lid = NULL) {
	
  // Domain
  $domain = domain_get_domain();
  
  user_account_form($form, $form_state);

  $form['global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Info'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['global']['account'] = $form['account'];
  
  unset( $form['account']);
  
  return $form;
      
}

function mc_campaign_user_edit_validate($form, $form_state) {
}

function mc_campaign_user_edit_submit($form, &$form_state) {
	
  // Domain
  $domain = domain_get_domain();
  
  $num_updated = db_update('mc_campaign_lists')
    ->fields(array(
      'title' => $form_state['values']['title'],
      'description' => $form_state['values']['description'],
      'updated' => REQUEST_TIME,
    ))
    ->condition('lid', $form_state['values']['lid'])
    ->condition('did',$domain['domain_id'])
    ->execute();
  
  $num_updated
    ? drupal_set_message(t('Your list has been updated!'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');

  drupal_goto('account/lists');
  
}









function mc_campaign_user_add($form, &$form_state) {
	
  // Roles
  global $user;
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('First you need to <a href="/user/me/edit">activate your account</a> with the activation code you have received on your mobile.'),'error');
    return NULL;
  }
  
  // Start with the default user account fields.
  user_account_form($form, $form_state);
  
  $form['global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Info'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['global']['account'] = $form['account'];
  
  unset( $form['account']);
  
  $form['submitnew'] = array(
      '#type' => 'submit',
      '#value' => t('Add User'),
  );
  $form['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'account/users',
  );
  
  $form['#validate'][0] = 'mc_campaign_user_add_validate';
  unset($form['#validate'][1]);
    
  return $form;
  
}

function mc_campaign_user_add_validate($form, $form_state) {
  if (isset($form_state['values']['name'])) {
    if ($error = user_validate_name($form_state['values']['name'])) {
      form_set_error('name', $error);
    }
    elseif ((bool) db_select('users')->fields('users', array('uid'))->condition('name', db_like($form_state['values']['name']), 'LIKE')->range(0, 1)->execute()->fetchField()) {
      form_set_error('name', t('The name %name is already taken.', array('%name' => $form_state['values']['name'])));
    }
  }

  if ($error = user_validate_mail($form_state['values']['mail'])) {
    form_set_error('mail', $error);
  }
  elseif ((bool) db_select('users')->fields('users', array('uid'))->condition('mail', db_like($form_state['values']['mail']), 'LIKE')->range(0, 1)->execute()->fetchField()) {
    // Format error message dependent on whether the user is logged in or not.
    form_set_error('mail', t('The e-mail address %email is already taken.', array('%email' => $form_state['values']['mail'])));
  }
}

function mc_campaign_user_add_submit($form, &$form_state) {

  $domain = domain_get_domain();
  $domain['domain_user'][$domain['domain_id']]= TRUE;
  
  $form_state['values']['init'] = $form_state['values']['mail'];
  $form_state['values']['timezone'] = 'Europe/London';
  $form_state['values']['signature_format'] = 'filtered_html';
  $form_state['values']['signature'] = '';
  
  $account = user_save(NULL, $form_state['values']);
  
  domain_user_save($domain, $account, '');
  
  drupal_goto('account/users');
  
}




/**
 * Menu callback; delete a subscribers list.
 */
function mc_campaign_user_delete($form, &$form_state, $list) {
	
  $form['lid'] = array(
    '#type' => 'hidden',
    '#value' => $list,
  );
  $output = t('Are you sure you want to delete the list? This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), drupal_get_destination(), filter_xss($output));
  
}


function mc_campaign_user_delete_submit($form, &$form_state) {
  $num_deleted = db_delete('mc_campaign_lists')
    ->condition('lid', $form_state['values']['lid'])
    ->execute();
    
  $num_updated = db_update('mc_campaign_lists_subscribers')
    ->fields(array(
      'deleted' => 1,
      'updated' => REQUEST_TIME,
    ))
    ->condition('lid', $form_state['values']['lid'])
    ->execute();
        
  isset($num_deleted)
    ? drupal_set_message(t('List deleted successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
    
  if (isset($num_updated)) drupal_set_message($num_updated . t(' subscribers unlinked from this list'), 'status');
    
  drupal_goto(drupal_get_destination());
}


function mc_campaign_user_view($form, &$form_state, $lid = NULL) {
	
  // Domain
  $domain = domain_get_domain();
  
  // Table
  $header = array(
    'id' => array('data' => t('ID'), 'field' => 'id'),
    'firstname' => array('data' => t('First Name'), 'field' => 'firstname'),
    'lastname' => array('data' => t('Last Name'), 'field' => 'lastname'),
    'eid' => array('data' => t('EID'), 'field' => 'eid'),
    'email' => array('data' => t('Email'), 'field' => 'email'),
    'mobile' => array('data' => t('Mobile'), 'field' => 'mobile'),
    'info1' => array('data' => t('Info1'), 'field' => 'info1'),
    'info2' => array('data' => t('Info2'), 'field' => 'info2'),
    'info3' => array('data' => t('Info3'), 'field' => 'info3'),
    'deleted' => array('data' => t('Status'), 'field' => 'deleted'),
    'created' => array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    'updated' => array('data' => t('Updated'), 'field' => 'updated'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('mc_campaign_lists_subscribers', 'lists_subscribers')->extend('PagerDefault')->extend('TableSort');
  $query->leftJoin('mc_campaign_subscribers', 'subscribers', 'subscribers.sid = lists_subscribers.sid');
  $query->condition('did',$domain['domain_id']);
    
  $result = $query
    ->fields('lists_subscribers', array(
      'created',
      'updated',
      'deleted',
      'info1',
      'info2',
      'info3',
      'id',
      'eid',
    ))
    ->fields('subscribers', array(
      'sid',
      'firstname',
      'lastname',
      'email',
      'mobile',
    ))
    ->condition('lists_subscribers.lid', $lid ,'=')
    ->limit(50)
    ->orderByHeader($header)
    ->execute();

  $options = array();
  $destination = drupal_get_destination();
  
  foreach ($result as $row) {
    $options[$row->id] = array(
      'id' => (int) $row->id,
      'firstname' => l(check_plain($row->firstname), 'admin/campaign/lists/subscribers/view/' . $row->sid),
      'lastname' => l(check_plain($row->lastname), 'admin/campaign/lists/subscribers/view/' . $row->sid),
      'eid' => (int) $row->eid,
      'email' => check_plain($row->email),
      'mobile' => check_plain($row->mobile),
      'info1' => check_plain($row->info1),
      'info2' => check_plain($row->info2),
      'info3' => check_plain($row->info3),
      'deleted' => ($row->deleted == 1) ? t('Disable') : t('Enable'),
      'created' => format_date($row->created, 'short'),
      'updated' => format_date($row->updated, 'short'),
      'operations' => array(
        'data' => array(
          array(
            '#type' => 'link',
            '#title' => t('View'),
            '#href' => 'admin/campaign/lists/subscribers/view/' . $row->sid,
            '#options' => array('query' => $destination),
          ),	
        ),
      ),
    );
  }
  
  $build['subscriber_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No subscribers yet.'),
  );
  $build['subscriber_pager'] = array('#theme' => 'pager');

  return $build;
}





