<?php

function mc_campaign_notification_list() {
	
  // FROM GRAPHE	
  $media = arg(2);
  $date = arg(3);
  $datebefore = strtotime($date . " -0 day");
  $dateafter = strtotime($date . " +1 day");
  $status = arg(4);
  
  // Roles
  global $user;
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('Hey buddy ! <a href="/user/me/edit">Activate your account</a> with the activation code you have received on your mobile.'),'error');
  }
	
  // Domain
  $domain = domain_get_domain();
	
  //$filter = mc_campaign_notification_list_build_filter_query();
	
  //$build['mc_campaign_notification_filter_form'] = drupal_get_form('mc_campaign_notification_filter_form');
  	
  //$build['campaign_admin_notification_filter_form_action_form'] = drupal_get_form('campaign_admin_notification_filter_form_action_form');
	
  $header = array(
    'cid' => array('data' => 'Campaign', 'field' => 'c.title'),
    'media' => array('data' => 'Method', 'field' => 'n.media'),
    'name' => array('data' => 'Name', 'field' => 's.firstname'),
	'contact' => array('data' => 'Contact', 'field' => 'contact'),
    'message' => array('data' => 'Message', 'field' => 'n.message'),
    'sent' => array('data' => 'Send Time', 'field' => 'n.sent','sort' => 'desc'),
    'received' => array('data' => 'Received Time', 'field' => 'n.received'),
    'status' => array('data' => 'Status', 'field' => 'n.status'),
  );
  
  $rows = array();

  $select = db_select('mc_campaign_notifications','n')->extend('PagerDefault')->extend('TableSort');
  $select->leftjoin('mc_campaign_campaigns', 'c', 'c.cid = n.cid');
  $select->join('mc_campaign_subscribers', 's', 's.sid = n.sid');
  $select->where('n.deleted' . ' = ' . 0);
  $select->where('n.sent' . ' > ' . 0);
  $select->condition('n.did',$domain['domain_id']);
	
  // ARGUMENT FROM PATH
  if ($media) $select->condition('n.media',$media);
  if ($status == 2) $select->condition('n.compt',2,'>');
  if ($status == 3) $select->condition('n.received',0,'>');
  if ($status == 4) $select->condition('n.received',0,'=');
  if ($date) $select->condition('sent',$datebefore,'>');
  if ($date) $select->condition('sent',$dateafter,'<');
  
  // FILTER
  if (!empty($filter['where'])) {
    $select->where($filter['where'], $filter['args']);
  }
	
	$results = $select
		->fields('n',array('media','contact','created','estimate_sent','received','compt','message','status'))
		->fields('c',array('title','cid'))
		->fields('s',array('firstname','lastname'))
		->limit(30)
		->orderByHeader($header)
		->execute();
	
  	foreach ($results as $entry) {
  		$row['cid'] 			= ($entry->cid) ? $entry->title : t('Reminder');
  		$row['media'] 			= $entry->media;
  		$row['name'] 			= $entry->firstname.' '.$entry->lastname;
  		$row['contact'] 		= $entry->contact;
    	$row['message'] 		= substr(drupal_html_to_text($entry->message),0,50);
  		$row['sent'] 			= format_date($entry->estimate_sent, 'short');
  		$row['received'] 		= ($entry->received) ? format_date($entry->received, 'short') : t('Not yet');
  		$row['status'] 			= mc_campaign_format_status($entry->status);
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


function campaign_admin_notification_list_build_filter_query() {
	
  	if (empty($_SESSION['campaign_admin_notification_list_build_filter'])) {
    	return;
  	}

  	$filters = campaign_admin_notification_list_build_filters();

  	// Build query
  	$where = $args = array();
  	
  	foreach ($_SESSION['campaign_admin_notification_list_build_filter'] as $key => $filter) {
    
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


function campaign_admin_notification_list_build_filters() {
	
  $filters = array();


  	return $filters;

}


function mc_campaign_notification_filter_form($form) {
	
  	$filters = campaign_admin_notification_list_build_filters();
  	
  	$form['filters'] = array(
    	'#type' => 'fieldset',
    	'#title' => t('Filter'),
    	'#collapsible' => TRUE,
    	'#collapsed' => FALSE,
  	);
  	
  	$form['filters']['media'] = array(
      '#type' => 'select',
      '#title' => t('How'),
      '#options' => array('sms'=>'SMS','mail'=>'Email','call_mobile'=>'Call Mobile','call_fixedline'=>'Call Fixed Line'),
      '#required' => TRUE,
      '#default_value' => 'sms',
    );

  	$form['filters']['actions'] = array(
    	'#type' => 'actions',
    	'#attributes' => array('class' => array('container-inline')),
  	);
  
  	$form['filters']['actions']['submit'] = array(
    	'#type' => 'submit',
    	'#value' => t('Filter'),
  	);
  
  	if (!empty($_SESSION['campaign_admin_notification_list_build_filter'])) {
    	$form['filters']['actions']['reset'] = array(
      		'#type' => 'submit',
      		'#value' => t('Reset')
    	);
  	}

  	return $form;

}


function campaign_admin_notification_filter_form_validate($form, &$form_state) {

	if ($form_state['values']['op'] == t('Filter') && empty($form_state['values']['type']) && empty($form_state['values']['severity'])) {
		form_set_error('type', t('You must select something to filter by.'));
  	}

}


function campaign_admin_notification_filter_form_submit($form, &$form_state) {
  
	$op = $form_state['values']['op'];
  	$filters = campaign_admin_notification_list_build_filters();
  	
  	switch ($op) {
    	case t('Filter'):
      	
    		foreach ($filters as $name => $filter) {
        		if (isset($form_state['values'][$name])) {
          			$_SESSION['campaign_admin_notification_list_build_filter'][$name] = $form_state['values'][$name];
        		}
      		}
      	break;
    	case t('Reset'):
      		$_SESSION['campaign_admin_notification_list_build_filter'] = array();
      	break;
  	}
  	return 'admin/campaign/notification';

}


function campaign_admin_notification_filter_form_action_form($form) {
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


function campaign_admin_notification_filter_form_action_form_submit() {
  	$_SESSION['dblog_overview_filter'] = array();
  	db_delete('watchdog')->execute();
  	drupal_set_message(t('Database log cleared.'));
}































function mc_campaign_analytics() {
	
  // Add CSS	
  drupal_add_css(drupal_get_path('module', 'mc_campaign') . '/css/dashboard.css');
	
  // Graphe
  if (mc_campaign_list_view_graphic()){
    $build['graphe'] = array(
      '#theme' => 'mc_campaign_graphe',
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
					yAxis: [
					{ // left y axis
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
					}, 
					{ // right y axis
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
					series: [
					{
						name: "Notifications",
						lineWidth: 4,
						marker: {
							radius: 4
						}
					},
					{
						name: "Mails",
						lineWidth: 4,
						marker: {
							radius: 4
						}
					},
					{
						name: "SMS",
						lineWidth: 4,
						marker: {
							radius: 4
						}
					},
					]
				};
				
				jQuery.get("graphe", null, function(data, state, xhr) {
				   options.series[0].data = data.notification;
				   options.series[1].data = data.mail;
				   options.series[2].data = data.sms;
				   chart = new Highcharts.Chart(options);
  				});
  	
  	
	});
   })})(jQuery);', 'type' => 'inline');
  }

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

function mc_campaign_graphe($arg1 = NULL) {
  $serie = array();
  
  $results = db_query("SELECT 
  n.sent as sent,
  from_unixtime(n.sent,'%d-%m-%Y') as format_created, 
  count(n.nid) as count FROM {mc_campaign_notifications} as n 
  WHERE 1=1 GROUP BY from_unixtime(n.sent,'%d-%m-%Y')", array(':lid' => $arg1))->fetchAll();
  $output = array();
  foreach ($results as $result) {
  	 $x = (int) $result->sent * 1000;
  	 $y = (int) $result->count;
 	 $output[] = array($x, $y);
  }
  $serie['notification'] = $output;

  $results = db_query("SELECT 
  n.sent as sent,
  from_unixtime(n.sent,'%d-%m-%Y') as format_created, 
  count(n.nid) as count FROM {mc_campaign_notifications} as n 
  WHERE media = 'mail' AND sent > 0 GROUP BY from_unixtime(n.sent,'%d-%m-%Y')", array(':lid' => $arg1))->fetchAll();
  $output = array();
  foreach ($results as $result) {
  	 $x = (int) $result->sent * 1000;
  	 $y = (int) $result->count;
 	 $output[] = array($x, $y);
  }
  $serie['mail'] = $output;

  $results = db_query("SELECT 
  n.sent as sent,
  from_unixtime(n.sent,'%d-%m-%Y') as format_created, 
  count(n.nid) as count FROM {mc_campaign_notifications} as n 
  WHERE media = 'sms' AND sent > 0 GROUP BY from_unixtime(n.sent,'%d-%m-%Y')", array(':lid' => $arg1))->fetchAll();
  $output = array();
  foreach ($results as $result) {
  	 $x = (int) $result->sent * 1000;
  	 $y = (int) $result->count;
 	 $output[] = array($x, $y);
  }
  $serie['sms'] = $output;
  
  drupal_json_output($serie);
}

