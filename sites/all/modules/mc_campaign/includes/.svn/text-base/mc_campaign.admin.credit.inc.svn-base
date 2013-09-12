<?php

/**
 * listing of all lists
 */
function mc_campaign_credit_list($form, &$form_state) {
	
  // Domain
  $domain = domain_get_domain();
 	
  // Table
  $header = array(
    'cid' => array('data' => t('ID'), 'field' => 'cid'),
    'amount' => array('data' => t('Amount'), 'field' => 'amount'),
    'expiration_date' => array('data' => t('Expiration Date'), 'field' => 'expiration_date'),
    'created' => array('data' => t('Created'), 'field' => 'created','sort' => 'desc'),
    'updated' => array('data' => t('Updated'), 'field' => 'updated'),
  );
  $query = db_select('mc_campaign_credits', 'credit')->extend('PagerDefault')->extend('TableSort');
  $query->fields('credit');
  $result = $query
    ->limit(50)
    ->orderByHeader($header)
    ->condition('did',$domain['domain_id'])
    ->execute();
  
  $options = array();
  
  foreach ($result as $row) {
    $options[$row->cid] = array(
      'cid' => (int) $row->cid,
      'amount' => (int) $row->amount,
      'expiration_date' => format_date($row->expiration_date, 'date_only'),
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
    '#empty' => t('No Credit available.'),
  );

  $form['pager'] = array('#theme' => 'pager');
  return $form;

  
}


function mc_campaign_credit_add($form, &$form_state) {
	
  $form = array();
  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount'),
    '#required' => TRUE,
  );
  $form['expiration_date'] = array(
    '#title' => t('Expiration Date'),
    '#type' => module_exists('date_popup') ? 'date_popup' : 'date_select',
    '#date_timezone' => date_default_timezone(),
    '#date_format' => 'd/m/Y',
    '#date_year_range' => '0:+1',
    '#default_value' => '2020-01-01',
  );

  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Add Credit'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/campaign/credit',
  );
  
  return $form;
  
}
function mc_campaign_credit_add_validate($form, $form_state) {
}
function mc_campaign_credit_add_submit($form, &$form_state) {
	
  $domain = domain_get_domain();
	
  $id = db_insert('mc_campaign_credits')
    ->fields(array(
      'did' => $domain['domain_id'],
      'amount' => (int) $form_state['values']['amount'],
      'expiration_date' => strtotime($form_state['values']['expiration_date']),
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
  ))
  ->execute();

  $id
    ? drupal_set_message(t('Your new credit has been added.'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');

  drupal_goto('admin/campaign/credit');
  
}


