<?php
// $Id: campaign.inc,v 1.10 2010/12/22 17:54:53 sun Exp $
function campaign_report_detail() {

	$variables['campaign_report']    = theme('my_theme_function');
	
	return $variables;
		
}
	


function campaign_general() {
	
	//Recent Drafts
	$header = array(
    	t('Campaign Name'),
		t('Define content'),
		t('Define recipients'),
	);
  
  	$rows = null;
  
  	$build['forms'][0] = array(
    	'#theme' => 'table',
    	'#header' => $header,
    	'#rows' => $rows,
    	'#empty' => l(t('Add form'), 'admin/config/campaign/add'),
  	);
  
  	//Recently Sent Campaign Reports
  	$header = array(
    	t('Campaign Name'),
    	t('Sent'),
		t('Recipients'),
		t('Opened'),
		t('Clicked'),
	);

  	$build['forms'][1] = array(
    	'#theme' => 'table',
    	'#header' => $header,
    	'#rows' => $rows,
  	);
  
  	return $build;
		
}

function campaign_report() {
	
  	return 'test';
		
}

function campaign_subscribers() {
	
	//Manage Subscribers
	$header = array(
    	t('List Name'),
		t('Created'),
		t('Update'),
		t('Subscribers'),
		t(''),
	);
  
  	$rows = null;
  
  	$build['forms'][0] = array(
    	'#theme' => 'table',
    	'#header' => $header,
    	'#rows' => $rows,
    	'#empty' => l(t('Create List'), 'admin/config/campaign/subscribers/createlist'),
  	);

  	return $build;
		
}


function campaign_dashboard($first_name = '', $last_name = '', $from_first_name='', $from_last_name='') {
	
  $header = array(
    t('Form'),
    t('Latest Activity'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  
  $rows = null;
  
  $build['forms'][0] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => l(t('Add form'), 'admin/config/content/campaign/add'),
  );
  
  $header = array(
    t('Form'),
    t('Latest Subscriber'),
    array('data' => t('Operations'), 'colspan' => 2),
  );

  $build['forms'][1] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => l(t('Add form'), 'admin/config/content/campaign/add'),
  );
  
  return $build;
		
}
