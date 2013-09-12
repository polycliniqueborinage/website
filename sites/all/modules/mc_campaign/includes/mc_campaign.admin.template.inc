<?php

/**
 * Menu callback; present an administrative template listing.
 */
function mc_campaign_template_list($form, &$form_state, $media) {
	
  // Domain
  $domain = domain_get_domain();
	
  if (isset($form_state['storage']['confirm'])) {
    $form['operation'] = array(
      '#type' => 'hidden',
      '#value' => 'delete',
    );
    $form['template'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['values']['template'],
    );
    $output = t('Are you sure you want to delete the following templates?');
    $output .= '<ul>';
    foreach ($form_state['values']['template'] as $tid) {
      $template = newsletter_tid_load($tid);
      $output .= !empty($template) ? '<li>' . check_plain($template->subject) . '</li>' : '';
    }
    $output .= '</ul>';
    $output .= t('This action cannot be undone.');
    return confirm_form($form, t('Delete the following?'), 'admin/config/system/newsletter/templates', filter_xss($output));
  }
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );

  $options['delete'] = t('Delete the selected templates');

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
  
  // TABLEAU
  $header = array(
    'tid' => array('data' => t('ID'), 'field' => 'tid'),
    'title' => array('data' => t('Title'), 'field' => 'title'),
    'created' => array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    'udpated' => array('data' => t('Updated'), 'field' => 'udpated'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('mc_campaign_templates', 'templates')->extend('PagerDefault')->extend('TableSort');
  $result = $query
    ->fields('templates')
    ->condition('media', $media)
    ->condition('did',$domain['domain_id'])
    ->limit(50)
    ->orderByHeader($header)
    ->execute();

  $options = array();
  $destination = drupal_get_destination();
  foreach ($result as $row) {
    $options[$row->tid] = array(
      'tid' => (int) $row->tid,
      'title' => check_plain($row->subject),
      'created' => format_date($row->created, 'short'),
      'udpated' => format_date($row->updated, 'short'),
      'operations' => array(
        'data' => array(
          array(
            '#type' => 'link',
            '#title' => t('edit'),
            '#href' => 'admin/campaign/templates/'.$row->media.'/edit/' . $row->tid ,
            '#options' => array('query' => $destination),
          ),
          array(
            '#markup' => ' | ',
          ),
          array(
            '#type' => 'link',
            '#title' => t('delete'),
            '#href' => 'admin/campaign/templates/'.$row->media.'/delete/' . $row->tid,
            '#options' => array('query' => $destination),
          ),
        ),
      ),
    );
  }

  $form['template'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No templates available.'),
  );

  $form['pager'] = array('#theme' => 'pager');

  return $form;
}

function mc_campaign_template_list_submit($form, &$form_state) {
  $template = $form_state['values']['template'];
  $operation = $form_state['values']['operation'];
  switch ($operation) {
      case 'delete':
        if (!isset($form_state['storage']['confirm'])) {
          $form_state['storage']['confirm'] = TRUE;
          $form_state['rebuild'] = TRUE;
          return;
        }
        foreach ($template as $id) {
          $deleted = db_delete('newsletter_templates')
            ->condition('id', $id)
            ->execute();
        }
        is_numeric($deleted)
          ? drupal_set_message(t('Templates deleted successfully'), 'status')
          : drupal_set_message(t('An unknown error occured,please try again'), 'error');
        break;
    }
}




























function mc_campaign_template_form($media) {
  $form = array();
  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#required' => TRUE,
    '#description' => t('This will also used as newsletter subject and title.'),
  );
  $form['token_help'] = array(
    '#title' => t('Replacement patterns'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Use [repeat] and [/repeat] for recurring items such as nodes'),
  );
  $form['token_help']['help'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('site', 'node', 'campaign'),
    '#recursion_limit' => 2,
  );
  switch ($media) {
    case 'mail':
      $form['body'] = array(
        '#type' => 'text_format',
        '#title' => t('Body'),
        '#required' => TRUE,
      );
      $form['media'] = array(
        '#type' => 'hidden',
      	'#value' => 'mail',
      );
      break;
    case 'sms':
      $form['body'] = array(
        '#type' => 'textarea',
        '#title' => t('Body'),
        '#required' => TRUE,
      	'#suffix' => "<div id='count'></div><br/>",
      
      );
      $form['media'] = array(
        '#type' => 'hidden',
      	'#value' => 'sms',
      );
      break;
  }	
  return $form;
}

/**
 * Menu callback; add a newsletter template.
 */
function mc_campaign_template_add($form, &$form_state, $media) {
  $form = mc_campaign_template_form($media);
  switch ($media) {
    case 'mail':
      $form['new'] = array(
        '#markup' => '<h3>' . t('Add new Mail Template') . '</h3>',
        '#weight' => -1,
      );
    break;
    case 'sms':
    // ADD JS SUR FORM
    drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.smscounter/jquery.smscounter.js');
    $form['new'] = array(
        '#markup' => '<h3>' . t('Add new SMS Template') . '</h3>',
        '#weight' => -1, 
    );
    break;
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Template'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/campaign/templates',
  );
  return $form;
}

function mc_campaign_template_add_validate($form, $form_state) {
}


function mc_campaign_template_add_submit($form, &$form_state) {
  $id = db_insert('mc_campaign_templates');
  $domain = domain_get_domain();
  if ($form_state['values']['media'] == 'sms') {
    $id->fields(array(
      'did' => $domain['domain_id'],
      'subject' => $form_state['values']['subject'],
      'body' => $form_state['values']['body'],
      'format' => '',
      'media' => $form_state['values']['media'],
      'updated' => REQUEST_TIME,
      'created' => REQUEST_TIME,
    ));
  } else {
    $id->fields(array(
      'did' => $domain['domain_id'],
      'subject' => $form_state['values']['subject'],
      'body' => $form_state['values']['body']['value'],
      'format' => $form_state['values']['body']['format'],
      'media' => $form_state['values']['media'],
      'updated' => REQUEST_TIME,
      'created' => REQUEST_TIME,
    ));
  }
  $id->execute();
  $id
    ? drupal_set_message(t('Template created successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto('admin/campaign/templates/'.$form_state['values']['media']);
}
/**
 * Menu callback; edit a template.
 */
function mc_campaign_template_edit($form, &$form_state, $media, $tid = NULL) {
  $form = mc_campaign_template_form($media);
  switch ($media) {
    case 'mail':
      $form['new'] = array(
        '#markup' => '<h3>' . t('Update your Mail Template') . '</h3>',
        '#weight' => -1,
      );
    break;
    case 'sms':
    // ADD JS SUR FORM
    drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.smscounter/jquery.smscounter.js');
    $form['new'] = array(
        '#markup' => '<h3>' . t('Update your SMS Template') . '</h3>',
        '#weight' => -1, 
    );
    break;
  }
  $form['tid'] = array(
    '#type' => 'hidden',
    '#value' => (int) $tid, 
  );
  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Update Template'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/campaign/lists/list',
  );
  $template = db_select('mc_campaign_templates','template')
    ->condition('tid',$tid)
    ->fields('template')
    ->execute()
    ->fetchObject();
  $form['subject']['#default_value'] = $template->subject;
  $form['body']['#default_value'] = $template->body;
  $form['type']['#media'] = $template->media;
  return $form;
}

function mc_campaign_template_edit_validate($form, $form_state) {
}

function mc_campaign_template_edit_submit($form, &$form_state) {
  $num_updated = db_update('mc_campaign_templates');
  if ($form_state['values']['media'] == 'sms') {
    $num_updated->fields(array(
      'subject' => $form_state['values']['subject'],
      'body' => $form_state['values']['body'],
      'format' => '',
      'media' => $form_state['values']['media'],
      'updated' => REQUEST_TIME,
    ));
  } else {
    $num_updated->fields(array(
      'subject' => $form_state['values']['subject'],
      'body' => $form_state['values']['body']['value'],
      'format' => $form_state['values']['body']['format'],
      'media' => $form_state['values']['media'],
      'updated' => REQUEST_TIME,
    ));
  }
  $num_updated->condition('tid', $form_state['values']['tid'])->execute();
  $num_updated
    ? drupal_set_message(t('Your list has been updated!'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto('admin/campaign/lists/list');
}









/**
 * Menu callback; delete newsletter templates.
 */
function mc_campaign_template_delete($form, &$form_state, $media, $template) {
  $form['tid'] = array(
    '#type' => 'hidden',
    '#value' => $template,
  );
  $form['media'] = array(
    '#type' => 'hidden',
    '#value' => $media,
  );
  $output = t('Are you sure you want to delete the following template? This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), drupal_get_destination(), filter_xss($output));
}

function mc_campaign_template_delete_submit($form, &$form_state) {
  $num_deleted = db_delete('mc_campaign_templates')
    ->condition('tid', $form_state['values']['tid'])
    ->execute();
  $num_deleted
    ? drupal_set_message(t('Template deleted successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto($_GET['destination']);
}
















function mc_campaign_template_json($tid = NULL) {
  $template = db_select('mc_campaign_templates','template')
    ->condition('tid',$tid)
    ->fields('template')
    ->execute()
    ->fetchObject();
    
   $output['body'] = $template->body;    
    
  drupal_json_output($output);
}


