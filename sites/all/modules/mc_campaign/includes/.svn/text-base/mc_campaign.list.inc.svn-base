<?php

/**
 * listing of all lists
 */
function mc_campaign_list_list($form, &$form_state) {
	
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
    '#markup' => l(t('Create list'), 'account/lists/add', array('attributes' => array('class' => 'button_left'))),
  );
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options['enable'] = t('Enable the selected lists');
  $options['disable'] = t('Disable the selected lists');
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
    'title' => array('data' => t('Title'), 'field' => 'title', 'sort' => 'desc'),
    'subscribers' => array('data' => t('Subscribers')),
    'created' => array('data' => t('Created'), 'field' => 'created'),
    'updated' => array('data' => t('Updated'), 'field' => 'updated'),
    'actions' => array('data' => t('Actions')),
    'operations' => array('data' => t('Operations')),
    'active' => array('data' => t('Active'), 'field' => 'active'),
  );
  $query = db_select('mc_campaign_lists', 'list')->extend('PagerDefault')->extend('TableSort');
  $query->fields('list');
  $result = $query
    ->limit(50)
    ->orderByHeader($header)
    ->condition('did',$domain['domain_id'])
    ->execute();
    
  $options = array();
  $destination = drupal_get_destination();
  foreach ($result as $row) {
  	
  	$subscribers_count = db_query("SELECT COUNT('sid') FROM {mc_campaign_lists_subscribers} WHERE lid = :lid AND deleted = 0", array(
      ':lid' => $row->lid,
    ))->fetchField();
  
  	// add actions
  	if($row->type == 'manual') {
      $actions = array(
        'data' => array(
          array(
            '#type' => 'link',
            '#title' => t('Add people'),
            '#href' => 'account/subscribers/addsubscribers/' . $row->lid ,
            '#options' => array('query' => $destination),
          ),
        ),
      );
  	} else {
  	  $actions= ($row->request_frequency == '-1' ) ? t('No Reload') : t('Next Reload : ').format_date($row->request_next_reload, 'short');
  	  $subscribers_count .= "/".check_plain($row->size);
  	}
  	
  	if($row->type == 'manual') {
      // add operations
      $operations = array(
        'data' => array(
          array(
            '#type' => 'link',
            '#title' => t('View'),
            '#href' => 'account/subscribers/' . $row->lid ,
            '#options' => array('query' => $destination),
          ),
          array(
            '#markup' => ' | ',
          ),
          array(
            '#type' => 'link',
            '#title' => t('Edit'),
            '#href' => 'account/lists/edit/' . $row->lid ,
            '#options' => array('query' => $destination),
          ),
          array(
            '#markup' => ' | ',
          ),
          array(
            '#type' => 'link',
            '#title' => t('Delete'),
            '#href' => 'account/lists/delete/' . $row->lid,
            '#options' => array('query' => $destination),
          ),
        ),
      );
  	} else {
  	  $operations = array(
        'data' => array(
          array(
            '#type' => 'link',
            '#title' => t('View'),
            '#href' => 'account/lists/view/' . $row->lid ,
            '#options' => array('query' => $destination),
          ),
        ),
      );
  	}
    
    $active['element']['#checked'] = $row->active;
    $active['element']['#attributes']['disabled'] = 'disabled';
    $options[$row->lid] = array(
      'title' => l($row->title, 'account/subscribers/'.$row->lid),
      'subscribers' => $subscribers_count,
      'type' => check_plain($row->type),
      'created' => format_date($row->created, 'short'),
      'updated' => format_date($row->updated, 'short'),
      'actions' => $actions,
      'operations' => $operations,
      'active' => theme_checkbox($active),
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

function mc_campaign_list_list_submit($form, &$form_state) {
  $operation = $form_state['values']['operation'];
  $list = $form_state['values']['list'];
  if ($list) {
    switch ($operation) {
      case 'enable':
        foreach ($list as $lid) {
          $num_updated = db_update('mc_campaign_lists')
            ->fields(array('active' => 1))
            ->condition('lid', $lid)
            ->execute();
        }
        break;
      case 'disable':
        foreach ($list as $lid) {
          $num_updated = db_update('mc_campaign_lists')
            ->fields(array('active' => 0))
            ->condition('lid', $lid)
            ->execute();
        }
        break;
      case 'delete':
        if (!isset($form_state['storage']['confirm'])) {
          $form_state['storage']['confirm'] = TRUE;
          $form_state['rebuild'] = TRUE;
          return;
        }
        foreach ($list as $id) {
          $num_deleted = db_update('newsletter_lists')
            ->condition('id', $id)
            ->execute();
          db_delete('newsletter_index_subscribers')
            ->condition('list_id', $id)
            ->execute();
        }
        break;
    }
    isset($num_updated)
      ? drupal_set_message(t('Lists updated successfully'), 'status')
      : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  }
  else {
    drupal_set_message(t('Please Select an action to perform.'), 'warning');
  }
}



function mc_campaign_manual_list_form(){
  $form = array();
  $form['list'] = array(
      '#type' => 'fieldset',
      '#title' => t(''),
  );
  $form['list']['new'] = array(
    '#markup' => '<h2>' . t('Create a new list') . '</h2>',
    '#weight' => -1,
  );
  $form['list']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('List Name'),
    '#required' => TRUE,
  	'#description' => t(' Good example: "Employees, Friends, Patients" - Bad example: "Bla_bla_11_11_2011" '),
  );
  $form['list']['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
  );
  return $form;
}


/**
 * Menu callback; edit a subscribers list.
 */
function mc_campaign_list_edit($form, &$form_state, $lid = NULL) {
	
  // Domain
  $domain = domain_get_domain();
  
  $form = mc_campaign_manual_list_form();

  $form['lid'] = array(
    '#type' => 'hidden',
    '#value' => (int) $lid, 
  );
  
  $form['new'] = array(
    '#markup' => '<h3>' . t('Update your list') . '</h3>',
    '#weight' => -1,
  );

  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Update List'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'account/lists',
  );
  
  $list = db_select('mc_campaign_lists','list')
    ->condition('lid',$lid)
    ->condition('did',$domain['domain_id'])
    ->fields('list')
    ->execute()
    ->fetchObject();
    
  $form['list']['title']['#default_value'] = $list->title;
  $form['list']['description']['#default_value'] = $list->description;
  
  return $form;
      
}
function mc_campaign_list_edit_validate($form, $form_state) {
}
function mc_campaign_list_edit_submit($form, &$form_state) {
	
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









function mc_campaign_list_add($form, &$form_state) {
	
  // Roles
  global $user;
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('First you need to <a href="/user/me/edit">activate your account</a> with the activation code you have received on your mobile.'),'error');
    return NULL;
  }
  $form = mc_campaign_manual_list_form();

  $form['submitnew'] = array(
      '#type' => 'submit',
      '#value' => t('Add List'),
  );
  $form['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'account/lists',
  );
    
  return $form;
  
}

function mc_campaign_list_add_validate($form, $form_state) {
}

function mc_campaign_list_add_submit($form, &$form_state) {

  $domain = domain_get_domain();
	
  $id = db_insert('mc_campaign_lists')
    ->fields(array(
      'did' => $domain['domain_id'],
      'title' => $form_state['values']['title'],
      'description' => $form_state['values']['description'],
      'type' => 'manual',
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
      'active' => 1,
  ))
  ->execute();

  $id
    ? drupal_set_message(t('Your new list was created!'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');

  drupal_goto('account/lists');
  
}




/**
 * Menu callback; delete a subscribers list.
 */
function mc_campaign_list_delete($form, &$form_state, $list) {
	
  $form['lid'] = array(
    '#type' => 'hidden',
    '#value' => $list,
  );
  $output = t('Are you sure you want to delete the list? This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), drupal_get_destination(), filter_xss($output));
  
}


function mc_campaign_list_delete_submit($form, &$form_state) {
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


function mc_campaign_list_view($form, &$form_state, $lid = NULL) {
	
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

function mc_campaign_list_view_graphic() {
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

function mc_campaign_list_graphe($arg1 = NULL) {
  $results = db_query("SELECT ls.created as created, from_unixtime(ls.created,'%d-%m-%Y') as format_created, count(ls.lid) as count FROM {mc_campaign_subscribers} as s LEFT JOIN {mc_campaign_lists_subscribers} as ls on ls.sid = s.sid WHERE ls.lid = :lid GROUP BY from_unixtime(ls.created,'%Y-%m-%d')", array(':lid' => $arg1))->fetchAll();
  $output = array();
  foreach ($results as $result) {
  	 $x = (int) $result->created * 1000;
  	 $y = (int) $result->count;
 	 $output[] = array($x, $y);
  }
  drupal_json_output($output);
}




