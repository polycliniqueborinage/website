<?php

/**
 * listing of all lists
 */
function mc_campaign_calendar_list($form, &$form_state) {
	
  // Domain
  $domain = domain_get_domain();
 	
  // Table
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'title', 'sort' => 'desc'),
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
            '#href' => 'admin/campaign/lists/subscribers/add/' . $row->lid ,
            '#options' => array('query' => $destination),
          ),
          array(
            '#markup' => ' | ',
          ),
          array(
            '#type' => 'link',
            '#title' => t('Import people'),
            '#href' => 'admin/campaign/lists/subscribers/add-mass/' . $row->lid,
            '#options' => array('query' => $destination),
          ),
        ),
      );
  	} else {
  	  $actions= ($row->request_frequency == '-1' ) ? t('No Reload') : t('Next Reload : ').format_date($row->request_next_reload, 'short');
  	  $subscribers_count .= "/".check_plain($row->size);
  	}
  	// add operations
    $operations = array(
      'data' => array(
        array(
          '#type' => 'link',
          '#title' => t('View'),
          '#href' => 'admin/campaign/lists/list/view/' . $row->lid ,
          '#options' => array('query' => $destination),
        ),
        array(
          '#markup' => ' | ',
        ),
        array(
          '#type' => 'link',
          '#title' => t('Edit'),
          '#href' => 'admin/campaign/lists/list/edit/' . $row->lid ,
          '#options' => array('query' => $destination),
        ),
        array(
          '#markup' => ' | ',
        ),
        array(
          '#type' => 'link',
          '#title' => t('Delete'),
          '#href' => 'admin/campaign/lists/list/delete/' . $row->lid,
          '#options' => array('query' => $destination),
        ),
      ),
    );
    $active['element']['#checked'] = $row->active;
    $active['element']['#attributes']['disabled'] = 'disabled';
    $options[$row->lid] = array(
      'title' => l(check_plain($row->title), 'admin/campaign/lists/list/view/'.$row->lid),
      'subscribers' => $subscribers_count,
      'type' => check_plain($row->type),
      'created' => format_date($row->created, 'short'),
      'updated' => format_date($row->updated, 'short'),
      'active' => theme_checkbox($active),
      'request_last_reload' => isset($row->request_last_reload) ? format_date($row->request_last_reload, 'short') : t('Not Yet'),
      'actions' => $actions,
      'operations' => $operations,
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



