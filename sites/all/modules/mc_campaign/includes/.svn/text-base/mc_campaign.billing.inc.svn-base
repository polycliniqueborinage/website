<?php

/**
 * listing of all lists
 */
function mc_campaign_credit_list($form, &$form_state) {
	
  // Domain
  $domain = domain_get_domain();
 	
  // Table
  $header = array(
    'amount' => array('data' => t('Amount'), 'field' => 'amount'),
    'description' => array('data' => t('Description'), 'field' => 'description'),
    'created' => array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    'updated' => array('data' => t('Updated'), 'field' => 'updated'),
  );
  
  $query = db_select('mc_campaign_credits', 'credits')->extend('PagerDefault')->extend('TableSort');
  $query->fields('credits');
  $result = $query
    ->limit(10)
    ->orderByHeader($header)
    ->condition('did',$domain['domain_id'])
    ->execute();
    
  $options = array();
  
  foreach ($result as $row) {
    $options[$row->cid] = array(
      'amount' => (int) $row->amount,
      'description' => $row->description,
      'created' => format_date($row->created, 'short'),
      'updated' => format_date($row->updated, 'short'),
    );
  }

  $form['list'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    // BUG : need to disable all select as it will select the checkbox inside the table
    '#js_select' => FALSE,
    '#empty' => t('No credits available.'),
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


function mc_campaign_list_form(){
  $form = array();
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('List Name'),
    '#required' => TRUE,
  	'#description' => t(' Good example: "Employees, Friends, Patients" - Bad example: "Bla_bla_11_11_2011" '),
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
  );
  $options = array();
  $options['manual'] = t('Manual');
  $options['request'] = t('Request');
  if (module_exists('filemaker') == TRUE) $options['filemaker'] = t('Filemaker');
  if (module_exists('facebook') == TRUE) $options['facebook'] = t('Facebook');
  $form['type'] = array(
    '#type' => 'radios',
    '#options' => $options,
    '#title' => t('What kind of list do you want to build?'),
  	'#default_value' => "manual",
  );
  // SQL
  $form['request'] = array(
    '#type' => 'fieldset',
    '#title' => t('Request'),
    '#states' => array(
      'visible' => array(
        ':input[name="type"]' => array('value' => t('request')),
      ),
    ),
  );
  $form['request']['database'] = array(
    '#type' => 'select',
    '#title' => t('Data Base'),
    '#options' => array(
      'default' => t('Drupal'),
      'poly' => t('Poly'),
    ),
  );
  $form['request']['sql'] = array(
    '#type' => 'textarea',
    '#title' => t('SQL'),
    '#description' => t('Need to return : sid (key for subscriber), firstname, lastname, email, mobile, fixedline, im, skype, twitter, facebook.<br/>Need to return : lid (key for list), sent_time, eid, info1, info2, info3...')."<br/>".t('Need to be ordered if you need to limit your query'),
  );
  $form['request']['frequency'] = array(
    '#type' => 'select',
    '#title' => t('Frequency'),
    '#options' => array(
      '-1' => t('Never'),
      '60' => t('Each Cron'),
      '3600' => t('1 Hour'),
      '7200' => t('2 Hours'),
      '10800' => t('3 Hours'),
      '21600' => t('6 Hours'),
      '43200' => t('12 Hours'),
      '86400' => t('1 Day'),
      '604800' => t('1 Week'),
      '2592000' => t('1 Month'),
    ),
  );
  $form['request']['limit'] = array(
    '#type' => 'select',
    '#title' => t('Limit'),
    '#options' => array(
      '999999' => t('No Limit'),
      '0' => t('0 (Handy to test the query)'),
      '1' => t('1'),
      '10' => t('10'),
      '20' => t('20'),
      '50' => t('50'),
  	  '100' => t('100'),
  	  '200' => t('200'),
    ),
    '#description' => t('How many subscibers to add by reload'),
  );
  $form['request']['next_reload'] = array(
    '#title' => t('Next Reload'),
    '#type' => module_exists('date_popup') ? 'date_popup' : 'date_select',
    '#date_timezone' => date_default_timezone(),
    '#date_format' => 'd/m/Y H:i',
    '#date_year_range' => '0:+1',
    '#default_value' => date('Y-m-d H:i:s', REQUEST_TIME),
  );
  $form['request']['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Whether this list is active or not.'),
    '#default_value' => 1,
  );
  // FILEMAKER
  if(module_exists('filemaker') == TRUE) {
  	$form['filemaker'] = array(
      '#type' => 'fieldset',
      '#title' => t('Filemaker'),
      '#states' => array(
        'visible' => array(
          ':input[name="type"]' => array('value' => t('filemaker')),
        ),
      ),
    );
  }
  // FACEBOOK
  if(module_exists('facebook') == TRUE) {
  	$form['facebook'] = array(
      '#type' => 'fieldset',
      '#title' => t('Facebook'),
      '#states' => array(
        'visible' => array(
          ':input[name="type"]' => array('value' => t('facebook')),
        ),
      ),
    );
  }
  return $form;
}

function mc_campaign_manual_list_form(){
  $form = array();
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('List Name'),
    '#required' => TRUE,
  	'#description' => t(' Good example: "Employees, Friends, Patients" - Bad example: "Bla_bla_11_11_2011" '),
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
  );
  return $form;
}


/**
 * Menu callback; edit a subscribers list.
 */
function mc_campaign_list_edit($form, &$form_state, $lid = NULL) {
	
  $form = mc_campaign_list_form();

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
    '#href' => 'admin/campaign/lists/list',
  );
  
  $list = db_select('mc_campaign_lists','list')
    ->condition('lid',$lid)
    ->fields('list')
    ->execute()
    ->fetchObject();
    
  $form['title']['#default_value'] = $list->title;
  $form['description']['#default_value'] = $list->description;
  $form['type']['#default_value'] = $list->type;
  $form['request']['database']['#default_value'] = $list->request_database;
  $form['request']['sql']['#default_value'] = $list->request_sql;
  $form['request']['frequency']['#default_value'] = $list->request_frequency;
  $form['request']['limit']['#default_value'] = $list->request_limit;
  $form['request']['next_reload']['#default_value'] = ($list->request_next_reload) ? date('Y-m-d H:i:s', $list->request_next_reload) : '';
  $form['request']['active']['#default_value'] = $list->active;
  
  return $form;
      
}
function mc_campaign_list_edit_validate($form, $form_state) {
  /*$next_reload = strtotime($form_state['values']['next_reload']);
  if ($form_state['values']['type'] == 'request' && $next_reload < REQUEST_TIME) {
      form_set_error('unpublish_on', t("The next reload time must be in the future"));
  }*/
}
function mc_campaign_list_edit_submit($form, &$form_state) {
	
  $num_updated = db_update('mc_campaign_lists')
    ->fields(array(
      'title' => $form_state['values']['title'],
      'description' => $form_state['values']['description'],
      'type' => $form_state['values']['type'],
      'request_database' => ($form_state['values']['type'] == 'request') ? $form_state['values']['database'] : null,
      'request_sql' => ($form_state['values']['type'] == 'request') ? $form_state['values']['sql'] : null,
      'request_frequency' => ($form_state['values']['type'] == 'request') ? $form_state['values']['frequency'] : null,
      'request_limit' => ($form_state['values']['type'] == 'request') ? $form_state['values']['limit'] : null,
      'request_next_reload' => ($form_state['values']['type'] == 'request') ? strtotime($form_state['values']['next_reload']) : null,
      'active' => $form_state['values']['active'],
      'updated' => REQUEST_TIME,
    ))
    ->condition('lid', $form_state['values']['lid'])->execute();
  
  $num_updated
    ? drupal_set_message(t('Your list has been updated!'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');

  drupal_goto('admin/campaign/lists/list');
  
}


function mc_campaign_list_add($form, &$form_state) {
	
  $form = mc_campaign_list_form();

  $form['new'] = array(
    '#markup' => '<h3>' . t('Set up your new list') . '</h3>',
    '#weight' => -1,
  );

  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Add List'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/campaign/lists/list',
  );
  
  return $form;
  
}
function mc_campaign_list_add_validate($form, $form_state) {
  $next_reload = strtotime($form_state['values']['next_reload']);
  if ($form_state['values']['type'] == 'request' && $next_reload < REQUEST_TIME) {
      form_set_error('unpublish_on', t("The next reload time must be in the future"));
  }
}
function mc_campaign_list_add_submit($form, &$form_state) {

  $domain = domain_get_domain();
	
  $id = db_insert('mc_campaign_lists')
    ->fields(array(
      'did' => $domain['domain_id'],
      'title' => $form_state['values']['title'],
      'description' => $form_state['values']['description'],
      'type' => $form_state['values']['type'],
      'request_database' => ($form_state['values']['type'] == 'request') ? $form_state['values']['database'] : null,
      'request_sql' => ($form_state['values']['type'] == 'request') ? $form_state['values']['sql'] : null,
      'request_frequency' => ($form_state['values']['type'] == 'request') ? $form_state['values']['frequency'] : null,
      'request_limit' => ($form_state['values']['type'] == 'request') ? $form_state['values']['limit'] : null,
      'request_next_reload' => ($form_state['values']['type'] == 'request') ? strtotime($form_state['values']['next_reload']) : null,
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
      'active' => $form_state['values']['active'],
  ))
  ->execute();

  $id
    ? drupal_set_message(t('Your new list was created! Your new list by default will store email, first name, and last name, but you can add more fields if you like.'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');

  drupal_goto('admin/campaign/lists/list');
  
}






function mc_campaign_list_add_manual($form, &$form_state) {
	
  $form = mc_campaign_manual_list_form();

  $form['new'] = array(
    '#markup' => '<h3>' . t('Set up your new list') . '</h3>',
    '#weight' => -1,
  );

  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Add List'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/campaign/lists/list',
  );
  
  return $form;
  
}
function mc_campaign_list_add_manual_validate($form, $form_state) {
}
function mc_campaign_list_add_manual_submit($form, &$form_state) {

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
    ? drupal_set_message(t('Your new list was created! Your new list by default will store email, first name, and last name, but you can add more fields if you like.'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');

  drupal_goto('admin/campaign/lists/list');
  
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


function mc_campaign_list_view($form, &$form_state, $arg1 = NULL) {
	
  // Add CSS	
  drupal_add_css(drupal_get_path('module', 'mc_campaign') . '/css/dashboard.css');
	
  // List detail
  $list = db_select('mc_campaign_lists','list')
    ->condition('lid',$arg1)
    ->fields('list')
    ->execute()
    ->fetchObject();
    
  $list->subscribers = db_query("SELECT COUNT('sid') FROM {mc_campaign_lists_subscribers} WHERE lid = :lid AND deleted = 0", array(
      ':lid' => $arg1,
  ))->fetchField();
    
    
  $build['dashboard'] = array(
    '#theme' => 'mc_campaign_list_detail',
    '#fields' => $list,
  );
  
  // Graphe
  if (mc_campaign_list_view_graphic()){
    $build['graphe'] = array(
      '#theme' => 'mc_campaign_graphe',
      '#fields' => $list,
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
    ->condition('lists_subscribers.lid', $arg1 ,'=')
    ->limit(100)
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




