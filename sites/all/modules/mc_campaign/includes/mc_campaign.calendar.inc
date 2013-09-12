<?php

/**
 * listing of all lists
 */
function mc_campaign_calendar_reminder($form, &$form_state, $sent = FALSE) {
  
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
    $form['reminders'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['values']['reminders'],
    );
    $output = t('Are you sure you want to delete the following reminders?');
    $output .= '<br/><br/>';
    $output .= t('This action cannot be undone.');
    return confirm_form($form, t('Delete the following?'), 'account/calendars/reminders', filter_xss($output));
  }
  
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options['delete'] = t('Delete the selected reminders');
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
    'media' => array('data' => 'Media', 'field' => 'n.media'),
	'name' => array('data' => 'Name', 'field' => 'contact'),
    'contact' => array('data' => 'Contact', 'field' => 'contact'),
    'message' => array('data' => 'Message', 'field' => 'n.message'),
	'estimate_sent' => array('data' => 'Estimation Time', 'field' => 'n.estimate_sent','sort' => 'desc'),
    'sent' => array('data' => 'Send Time', 'field' => 'n.sent','sort' => 'desc'),
    'received' => array('data' => 'Received Time', 'field' => 'n.received'),
    'status' => array('data' => 'Status', 'field' => 'n.status'),
  );

  if (isset($sent) && $sent) {
    unset ($header['estimate_sent']); 
  } else {
    unset ($header['sent']);
    unset ($header['received']);
  }
  
  $rows = array();
  
  $select = db_select('mc_campaign_notifications','n')->extend('PagerDefault')->extend('TableSort');
  $select->join('mc_campaign_subscribers', 's', 's.sid = n.sid');
  $select->where('n.deleted' . ' = ' . 0);
  if (isset($sent) && $sent) {
    $select->where('n.sent' . ' > ' . 0);
  } else {
    $select->where('n.sent' . ' = ' . 0);
  }
  $select->where('n.did' . ' = ' . $domain['domain_id']);
  $select->where("n.module = 'calendar'");
  
  if (!empty($filter['where'])) {
    $select->where($filter['where'], $filter['args']);
  }
	
  $results = $select
  ->fields('n',array('nid','media','contact','created','estimate_sent','sent','received','compt','message','status'))
  ->fields('s',array('firstname','lastname'))
  ->limit(30)
  ->orderByHeader($header)
  ->execute();
  
  foreach ($results as $entry) {
    $row['media'] 			= $entry->media;
  	$row['name'] 			= $entry->firstname.' '.$entry->lastname;
  	$row['contact'] 		= $entry->contact;
    $row['message'] 		= substr(drupal_html_to_text($entry->message),0,100);
    if (isset($sent) && $sent) {
      $row['sent'] 			= format_date($entry->estimate_sent, 'short');
  	  $row['received'] 		= ($entry->received) ? format_date($entry->received, 'short') : t('Not yet');
    } else {
      $row['estimate_sent'] 	= format_date($entry->estimate_sent, 'short');
    }
    $row['status'] 			= mc_campaign_format_status($entry->status);
    $rows[$entry->nid] = $row;
  }
   	
  $form['reminders'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No reminder yet.'),
  );

  $form['reminder_pager'] = array('#theme' => 'pager');
  
  return $form;
  
}


function mc_campaign_calendar_reminder_submit($form, &$form_state) {
  // Domain
  $domain = domain_get_domain();
  $operation = $form_state['values']['operation'];
  $reminders = $form_state['values']['reminders'];
  if ($reminders) {
    switch ($operation) {
      case 'delete':
        if (!isset($form_state['storage']['confirm'])) {
          $form_state['storage']['confirm'] = TRUE;
          $form_state['rebuild'] = TRUE;
          return;
        }
        foreach ($reminders as $id) {
          $num_deleted = db_delete('mc_campaign_notifications')
            ->condition('nid', $id)
            ->condition('did',$domain['domain_id'])
            ->execute();
        }
        break;
      default:
        drupal_set_message(t('Please select an action'), 'warning');
    }
    isset($num_deleted)
      ? drupal_set_message(t('Reminder(s) deleted successfully'), 'status')
      : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  }
  else {
    drupal_set_message(t('Please select a least one reminder.'), 'warning');
  }
}


function mc_campaign_calendar_delete($nid) {
  $node = node_load($nid);
  $domain = domain_get_domain();
  // TODO : check permission; 
  node_delete($nid);
}


function mc_campaign_calendar_disable($calendar_id = null) {
  if ($calendar_id) {
    unset($_SESSION['enable_calendar'][$calendar_id]);
  }
  drupal_json_output($_SESSION['enable_calendar']);
  drupal_exit();
}


function mc_campaign_calendar_enable($calendar_id = null) {
  // Save enable calendar in session.
  if ($calendar_id) {
    if (is_array($_SESSION['enable_calendar'])) {
      $_SESSION['enable_calendar'] += array($calendar_id => TRUE);
    } else {
      $_SESSION['enable_calendar'] = array($calendar_id => TRUE);
    }
  }
  drupal_json_output($_SESSION['enable_calendar']);
  drupal_exit();
}


function mc_campaign_calendar_json($calendar_id) {
  
  // Domain
  $domain = domain_get_domain();
  
  $start = (int) $_GET['start'];
  $end = (int) $_GET['end'];
  
  // Save date in session.
  $_SESSION['current_date'] = date('Y-m-d', $start);

  $result = db_query("SELECT 
  
  node.nid,
  node.title as title,
  field_data_field_event_agenda.field_event_agenda_mc_custom_list,
  field_data_field_event_color.field_event_color_rgb as event_color,
  field_data_field_agenda_color.field_agenda_color_rgb as agenda_color,
  field_data_field_event_status.field_event_status_value as event_status,
  field_data_field_event_date.*,
  field_data_field_event_slot_minutes.field_event_slot_minutes_value as event_slot_minutes,
  field_data_field_event_slot_type.field_event_slot_type_value as event_slot_type,
  field_data_field_event_subscriber.field_event_subscriber_key as event_subscriber,
  field_data_field_event_reminder.field_event_reminder_media as event_reminder
  
  FROM 
  {node} node
  
  LEFT JOIN {domain_access} domain_access ON node.nid = domain_access.nid AND domain_access.realm = 'domain_id'
  LEFT JOIN {field_data_field_event_date} field_data_field_event_date ON node.nid = field_data_field_event_date.entity_id AND (field_data_field_event_date.entity_type = 'node' AND field_data_field_event_date.deleted = '0')
  LEFT JOIN {field_data_field_event_agenda} field_data_field_event_agenda ON node.nid = field_data_field_event_agenda.entity_id AND (field_data_field_event_agenda.entity_type = 'node' AND field_data_field_event_agenda.deleted = '0')
  LEFT JOIN field_data_field_event_color field_data_field_event_color ON ( field_data_field_event_color.entity_id = node.nid AND field_data_field_event_color.deleted = '0' )
  LEFT JOIN field_data_field_agenda_color field_data_field_agenda_color ON ( field_data_field_agenda_color.entity_id = field_data_field_event_agenda.field_event_agenda_mc_custom_list AND field_data_field_agenda_color.deleted = '0' )
  LEFT JOIN field_data_field_event_status field_data_field_event_status ON ( field_data_field_event_status.entity_id = node.nid AND field_data_field_event_status.deleted = '0' )
  LEFT JOIN field_data_field_event_slot_minutes field_data_field_event_slot_minutes ON ( field_data_field_event_slot_minutes.entity_id = node.nid AND field_data_field_event_slot_minutes.deleted = '0' )
  LEFT JOIN field_data_field_event_slot_type field_data_field_event_slot_type ON ( field_data_field_event_slot_type.entity_id = node.nid AND field_data_field_event_slot_type.deleted = '0' )
  LEFT JOIN field_data_field_event_subscriber field_data_field_event_subscriber ON ( field_data_field_event_subscriber.entity_id = node.nid AND field_data_field_event_subscriber.deleted = '0' )
  LEFT JOIN field_data_field_event_reminder field_data_field_event_reminder ON ( field_data_field_event_reminder.entity_id = node.nid AND field_data_field_event_reminder.deleted = '0' )
  
  WHERE 
  (
    (
      (domain_access.realm = 'domain_id') 
    )
    AND
    (
      node.status = '1'
      AND (node.type IN  ('event')) 
      AND (domain_access.gid IN  (:did)) 
      AND (field_data_field_event_date.field_event_date_value >= :start) 
      AND (field_data_field_event_date.field_event_date_value2 <= :end) 
      AND (field_data_field_event_agenda.field_event_agenda_mc_custom_list = :calendar_id) 
    )
  )
  ",array(':did'=>$domain['domain_id'], ':start'=>$start, ':end'=>$end, ':calendar_id'=>$calendar_id));
  
  $reponses = array();
  
  foreach ($result as $row) {
    
    if(date('H:i',$row->field_event_date_value) == '00:00' && date('H:i',$row->field_event_date_value2) == '00:00'){
	  $reponses[$row->nid]['allDay'] = TRUE;
      $reponses[$row->nid]['start'] = $row->field_event_date_value;
      $reponses[$row->nid]['end'] = $row->field_event_date_value2 - 86400;
	} else {
	  $reponses[$row->nid]['allDay'] = FALSE;
      $reponses[$row->nid]['start'] = $row->field_event_date_value;
      $reponses[$row->nid]['end'] = $row->field_event_date_value2;
	}
    $reponses[$row->nid]['id'] = $row->nid;
    $reponses[$row->nid]['title'] = ($row->title) ? $row->title : '';
    if ($row->event_status == 'provisional' && $row->event_slot_type == 'multiple') $reponses[$row->nid]['title'] .= ' (' .$row->event_slot_minutes . ' ' . t('min slot') .')';
	$reponses[$row->nid]['className'] = $row->event_status;
	$reponses[$row->nid]['status'] = $row->event_status;
	$reponses[$row->nid]['reminder'] = ($row->event_status == 'confirmed' && isset($row->event_subscriber) && isset($row->event_reminder)) ? 'reminder' : '';
	$reponses[$row->nid]['subscribers_class'] = (isset($row->event_subscriber)) ? 'subscribers' : '';
	//$reponse['subscribers'] = 2;
	$reponses[$row->nid]['calendar_id'] = $calendar_id;
	$reponses[$row->nid]['editable'] = TRUE;
	$reponses[$row->nid]['backgroundColor'] = $row->agenda_color;
	$reponses[$row->nid]['borderColor'] = !empty($row->event_color) ? $row->event_color : '#' . mc_campaign_calendar_hex_darker(drupal_substr($row->agenda_color,1));
  
  }
  
  $responsesclean = array();
  foreach ($reponses as $reponse) {
    array_push($responsesclean, $reponse);
  }
  
  drupal_json_output($responsesclean);
  
  die();
  
}



function mc_campaign_calendar_update($nid) {
  
  $start = (int) $_POST['start'];
  $end = (int) $_POST['end'];
  $allday = $_POST['allday'];

  $calendar_id = (int) $_POST['calendar_id'];
  $status = $_POST['status'];
  
  // Domain
  $domain = domain_get_domain();
  
  $node = node_load($nid);
  
  if (!empty($calendar_id)) {
    $node->field_event_agenda[LANGUAGE_NONE][0]['mc_custom_list'] = $calendar_id;
    $node->field_event_status[LANGUAGE_NONE][0][value] = $status;
  }
  
  if (!empty($start)) {
    if ($allday == 'true') {
      $start = strtotime(date('Y-m-d',$start));
      $end = strtotime(date('Y-m-d',$end)) + 86400;
      $node->field_event_date[LANGUAGE_NONE][0][value] = $start;
      $node->field_event_date[LANGUAGE_NONE][0][value2] = $end;
    } else {
      $node->field_event_date[LANGUAGE_NONE][0][value] = $start;
      $node->field_event_date[LANGUAGE_NONE][0][value2] = $end;
    }
  }
  
  node_save($node);  
  
  drupal_json_output($node->nid);
  
}



function mc_campaign_calendar_add() {

  $start = (int) $_POST['start'];
  $end = (int) $_POST['end'];
  $allday = $_POST['allday'];
  $calendar_id = (int) $_POST['calendar_id'];
  //$room_id = (int) $_POST['room_id'];
  $comment = $_POST['comment'];
  //$color = $_POST['color'];
  $status = $_POST['status'];
  $users = $_POST['users'];
  $slot_type = $_POST['slot_type'];
  $slot_minutes = (int) $_POST['slot_minutes'];
  $invit_number = 1;
  
  $calendar = node_load($calendar_id);
  
  $node = new stdClass();
  $node->type = 'event';
  node_object_prepare($node);
  
  // check permission
  if ($calendar->nid) {
    
    $node->is_new = TRUE;
    $node->title = ($comment !='') ? $comment : t('no title');
    $node->language = LANGUAGE_NONE;
    $node->field_event_agenda[LANGUAGE_NONE][0]['mc_custom_list'] = $calendar_id;
    $node->field_event_status[LANGUAGE_NONE][0][value] = $status;
    if ($allday == 'true') {
      $start = strtotime(date('Y-m-d',$start));
      $end = strtotime(date('Y-m-d',$end)) + 86400;
      $node->field_event_date[LANGUAGE_NONE][0][value] = $start;
      $node->field_event_date[LANGUAGE_NONE][0][value2] = $end;
    } else {
      $node->field_event_date[LANGUAGE_NONE][0][value] = $start;
      $node->field_event_date[LANGUAGE_NONE][0][value2] = $end;
    }
    if (isset($users) && $users != '') {
      $node->field_event_subscriber[LANGUAGE_NONE][0]['key'] = $users;
    }

    $node->field_event_slot_type[LANGUAGE_NONE][0][value] = $slot_type;
    $node->field_event_slot_minutes[LANGUAGE_NONE][0][value] = $slot_minutes;
    
    $node->field_event_date[LANGUAGE_NONE][0][show_todate] = 1;
    $node->field_event_date[LANGUAGE_NONE][0][show_repeat_settings] = 0;
    $node->field_event_reminder = $calendar->field_agenda_reminder;
    // DO NOT SAVE COLOR ! SO WILL BE CHANGED IF CALENDAR COLOR CHANGE
    $node->field_event_color[LANGUAGE_NONE][0]['rgb'] = '';
    $node = node_save($node);
  
  }
  
  drupal_json_output($node);

}


/**
 * Form builder; Campaign Dashboard
 */
function mc_campaign_calendar_calendar($calendar_id = null) {
  
  //unset($_SESSION['current_date']);
  $date = (isset($_SESSION['current_date'])) ? $_SESSION['current_date'] : date('Y-m-d');
  $date = explode("-", $date);
  
  if ($calendar_id != NULL) {
    $_SESSION['current_calendar'] = $calendar_id;
  }
  
  // Roles
  global $user;
  $roles = $user->roles;
  
  if (isset($roles[7])){
  	drupal_set_message(t('Hey buddy ! <a href="/user/me/edit">Activate your account</a> with the activation code you have received on your mobile.'),'error');
  }
  
  // Domain
  $domain = domain_get_domain();
  
  // LOAD JS
  drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jqtip/jquery.qtip.js');
  drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/fullcalendar/fullcalendar.js');
  drupal_add_js(drupal_get_path('module', 'autocomplete_deluxe') . '/autocomplete_deluxe.js');
    
  // LOAD CSS
  drupal_add_css(drupal_get_path('module', 'mc_campaign') . '/js/jqtip/jquery.qtip.css');
  drupal_add_css(drupal_get_path('module', 'mc_campaign') . '/css/fullcalendar.css');
  drupal_add_css(drupal_get_path('module', 'autocomplete_deluxe') . '/autocomplete_deluxe.css');

  //prepare data
  if (user_access('view any calendar')) {

    $result = db_query("SELECT 
      node.title AS node_title, 
      node.nid AS nid,
      field_data_field_agenda_color.field_agenda_color_rgb AS field_agenda_color,
      GROUP_CONCAT(DISTINCT field_data_field_agenda_reminder.field_agenda_reminder_media) AS field_reminder_media,
      GROUP_CONCAT(DISTINCT field_data_field_agenda_reminder.field_agenda_reminder_unit) AS field_reminder_unit,
      GROUP_CONCAT(DISTINCT field_data_field_agenda_reminder.field_agenda_reminder_number) AS field_reminder_number,
      field_data_field_agenda_firsthour.field_agenda_firsthour_value AS field_agenda_firsthour,
      field_data_field_agenda_maxtime.field_agenda_maxtime_value AS field_agenda_maxtime,
      field_data_field_agenda_mintime.field_agenda_mintime_value AS field_agenda_mintime,
      field_data_field_agenda_slotminutes.field_agenda_slotminutes_value AS field_agenda_slotminutes,
      GROUP_CONCAT(DISTINCT field_data_field_agenda_views.field_agenda_views_value) AS field_agenda_views,
      field_data_field_agenda_default_views.field_agenda_default_views_value AS field_agenda_default_views
      FROM 
  	{node} node
  	LEFT JOIN {field_data_field_agenda_reminder} field_data_field_agenda_reminder ON node.nid = field_data_field_agenda_reminder.entity_id AND (field_data_field_agenda_reminder.entity_type = 'node' AND field_data_field_agenda_reminder.deleted = '0')
  	LEFT JOIN {field_data_field_agenda_color} field_data_field_agenda_color ON node.nid = field_data_field_agenda_color.entity_id AND (field_data_field_agenda_color.entity_type = 'node' AND field_data_field_agenda_color.deleted = '0')
  	LEFT JOIN {field_data_field_agenda_firsthour} field_data_field_agenda_firsthour ON node.nid = field_data_field_agenda_firsthour.entity_id AND (field_data_field_agenda_firsthour.entity_type = 'node' AND field_data_field_agenda_firsthour.deleted = '0')
  	LEFT JOIN {field_data_field_agenda_maxtime} field_data_field_agenda_maxtime ON node.nid = field_data_field_agenda_maxtime.entity_id AND (field_data_field_agenda_maxtime.entity_type = 'node' AND field_data_field_agenda_maxtime.deleted = '0')
  	LEFT JOIN {field_data_field_agenda_mintime} field_data_field_agenda_mintime ON node.nid = field_data_field_agenda_mintime.entity_id AND (field_data_field_agenda_mintime.entity_type = 'node' AND field_data_field_agenda_mintime.deleted = '0')
  	LEFT JOIN {field_data_field_agenda_slotminutes} field_data_field_agenda_slotminutes ON node.nid = field_data_field_agenda_slotminutes.entity_id AND (field_data_field_agenda_slotminutes.entity_type = 'node' AND field_data_field_agenda_slotminutes.deleted = '0')
  	LEFT JOIN {field_data_field_agenda_views} field_data_field_agenda_views ON node.nid = field_data_field_agenda_views.entity_id AND (field_data_field_agenda_views.entity_type = 'node' AND field_data_field_agenda_views.deleted = '0')
  	LEFT JOIN {field_data_field_agenda_default_views} field_data_field_agenda_default_views ON node.nid = field_data_field_agenda_default_views.entity_id AND (field_data_field_agenda_default_views.entity_type = 'node' AND field_data_field_agenda_default_views.deleted = '0')
  	LEFT JOIN {domain_access} domain_access ON node.nid = domain_access.nid AND domain_access.realm = 'domain_id'
  	WHERE (( (node.status = '1') AND (domain_access.gid IN  (:did)) AND (domain_access.realm = 'domain_id') )AND(( (node.type IN  ('agenda')) )))
  	GROUP BY node.nid
  	",array(':did'=>$domain['domain_id']));

  } else {
    
    $result = db_query("SELECT 
    node.title AS node_title, 
    node.nid AS nid,
    field_data_field_agenda_color.field_agenda_color_rgb AS field_agenda_color,
    GROUP_CONCAT(DISTINCT field_data_field_agenda_reminder.field_agenda_reminder_media) AS field_reminder_media,
    GROUP_CONCAT(DISTINCT field_data_field_agenda_reminder.field_agenda_reminder_unit) AS field_reminder_unit,
    GROUP_CONCAT(DISTINCT field_data_field_agenda_reminder.field_agenda_reminder_number) AS field_reminder_number,
    field_data_field_agenda_firsthour.field_agenda_firsthour_value AS field_agenda_firsthour,
    field_data_field_agenda_maxtime.field_agenda_maxtime_value AS field_agenda_maxtime,
    field_data_field_agenda_mintime.field_agenda_mintime_value AS field_agenda_mintime,
    field_data_field_agenda_slotminutes.field_agenda_slotminutes_value AS field_agenda_slotminutes,
    GROUP_CONCAT(DISTINCT field_data_field_agenda_views.field_agenda_views_value) AS field_agenda_views,
    field_data_field_agenda_default_views.field_agenda_default_views_value AS field_agenda_default_views
    FROM 
	{node} node
	LEFT JOIN {field_data_field_agenda_reminder} field_data_field_agenda_reminder ON node.nid = field_data_field_agenda_reminder.entity_id AND (field_data_field_agenda_reminder.entity_type = 'node' AND field_data_field_agenda_reminder.deleted = '0')
	LEFT JOIN {field_data_field_agenda_color} field_data_field_agenda_color ON node.nid = field_data_field_agenda_color.entity_id AND (field_data_field_agenda_color.entity_type = 'node' AND field_data_field_agenda_color.deleted = '0')
	LEFT JOIN {field_data_field_agenda_firsthour} field_data_field_agenda_firsthour ON node.nid = field_data_field_agenda_firsthour.entity_id AND (field_data_field_agenda_firsthour.entity_type = 'node' AND field_data_field_agenda_firsthour.deleted = '0')
	LEFT JOIN {field_data_field_agenda_maxtime} field_data_field_agenda_maxtime ON node.nid = field_data_field_agenda_maxtime.entity_id AND (field_data_field_agenda_maxtime.entity_type = 'node' AND field_data_field_agenda_maxtime.deleted = '0')
	LEFT JOIN {field_data_field_agenda_mintime} field_data_field_agenda_mintime ON node.nid = field_data_field_agenda_mintime.entity_id AND (field_data_field_agenda_mintime.entity_type = 'node' AND field_data_field_agenda_mintime.deleted = '0')
	LEFT JOIN {field_data_field_agenda_slotminutes} field_data_field_agenda_slotminutes ON node.nid = field_data_field_agenda_slotminutes.entity_id AND (field_data_field_agenda_slotminutes.entity_type = 'node' AND field_data_field_agenda_slotminutes.deleted = '0')
	LEFT JOIN {field_data_field_agenda_views} field_data_field_agenda_views ON node.nid = field_data_field_agenda_views.entity_id AND (field_data_field_agenda_views.entity_type = 'node' AND field_data_field_agenda_views.deleted = '0')
	LEFT JOIN {field_data_field_agenda_default_views} field_data_field_agenda_default_views ON node.nid = field_data_field_agenda_default_views.entity_id AND (field_data_field_agenda_default_views.entity_type = 'node' AND field_data_field_agenda_default_views.deleted = '0')
	LEFT JOIN {domain_access} domain_access ON node.nid = domain_access.nid AND domain_access.realm = 'domain_id'
	LEFT JOIN {field_data_field_agenda_notification_user} field_data_field_agenda_notification_user ON node.nid = field_data_field_agenda_notification_user.entity_id AND field_data_field_agenda_notification_user.deleted = '0'
	
	WHERE node.status = '1'
	AND domain_access.gid IN (:did)
	AND domain_access.realm = 'domain_id'
	AND node.type IN  ('agenda')
	AND field_data_field_agenda_notification_user.field_agenda_notification_user_mc_custom_list = :uid 
	GROUP BY node.nid
	",array(':did' => $domain['domain_id'], ':uid' => $user->uid));
    
  }

  $calendars = array();
  foreach ($result as $row) {
    
    // if not calendar pick the first one.
    if (empty($_SESSION['current_calendar'])) {
      $_SESSION['current_calendar'] = $row->nid;
    }
    
    // Get active active calendar.
    $calendar_id = $_SESSION['current_calendar'];
    
    // Enable active calendar.
    if (is_array($_SESSION['enable_calendar'])) {
      $_SESSION['enable_calendar'] += array($calendar_id => TRUE);
    } else {
      $_SESSION['enable_calendar'] = array($calendar_id => TRUE);
    }
    
    $row->field_agenda_first_day = 1;
    $row->field_agenda_views = $row->field_agenda_views . ',onlineBooking';
    $row->field_agenda_selectable = TRUE;
    $row->field_agenda_editable = TRUE;
    $row->field_agenda_color = ($row->field_agenda_color) ? $row->field_agenda_color : 'DDDDDD';
    $row->field_agenda_firsthour = ($row->field_agenda_firsthour) ? (int) $row->field_agenda_firsthour : 0;
    $row->field_agenda_maxtime = isset($row->field_agenda_maxtime) ? (int) $row->field_agenda_maxtime : 24;
    $row->field_agenda_mintime = isset($row->field_agenda_mintime) ? (int) $row->field_agenda_mintime : 6;
    $row->field_agenda_slotminutes = ($row->field_agenda_slotminutes) ? (int) $row->field_agenda_slotminutes : 60;
    $row->class = ($_SESSION['enable_calendar'][$row->nid]) ? 'enable' : 'disable';
    $row->class .= ($_SESSION['current_calendar'] == $row->nid) ? ' active' : '';
    $calendars[$row->nid] = $row;
    
  }
  
  if (isset($calendars[$calendar_id])) {
    drupal_add_js(array(
      'calendars' => $calendars,
      'enable_calendars' => $_SESSION['enable_calendar'],
      'calendar' => $calendars[$calendar_id],
      'year' => $date[0],
      'month' => $date[1],
      'date' => $date[2],
      ), 'setting'
    );  
  }
  
  $title = t('Campaign Dashboard');
  
  $build['calendar'] = array(
    '#theme' => 'mc_campaign_calendar',
    '#title' => 'titlett',
    '#calendars' => $calendars,
  );

  $output = drupal_render($build);
  
  return $output;
  
}

/**
 * listing of all lists
 */
function mc_campaign_calendar_list($form, &$form_state) {
  
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
    $form['reminders'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['values']['reminders'],
    );
    $output = t('Are you sure you want to delete the following reminders?');
    $output .= '<br/><br/>';
    $output .= t('This action cannot be undone.');
    return confirm_form($form, t('Delete the following?'), 'account/calendars/reminders', filter_xss($output));
  }
  
  $form['add']['add'] = array(
    '#markup' => l(t('Add calendar'), 'node/add/agenda', array('attributes' => array('class' => 'button_left'))),
  );
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options['delete'] = t('Delete the selected calendar');
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
    'operations' => array('data' => t('Operations')),
  );
  
  $result = db_query("
  SELECT node.title AS node_title, node.nid AS nid, node.language AS node_language, node.created AS node_created
  FROM {node} node
  LEFT JOIN {domain_access} domain_access ON node.nid = domain_access.nid AND domain_access.realm = 'domain_id'
  WHERE (( (node.status = '1') AND (node.type IN  ('agenda')) AND (domain_access.gid = :did) AND (domain_access.realm = 'domain_id')))
  ",array(':did'=>$domain['domain_id']))->fetchAll();
  
  $destination = drupal_get_destination();
  
  $options = NULL;
  foreach ($result as $row) {
    $operation = array();
    $operation[] = array(
        '#type' => 'link',
        '#title' => t('Edit'),
        '#href' => 'node/' . $row->nid . '/edit',
        '#options' => array('query' => $destination),
    );
    $options[$row->nid] = array(
      'title' => l(check_plain($row->node_title), 'account/calendars/'.$row->nid),
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


function mc_campaign_calendar_externe($form, &$form_state) {
  
  // Roles
  global $user;
  $roles = $user->roles;
      
  if (isset($roles[7])){
  	drupal_set_message(t('Hey buddy ! <a href="/user/me/edit">Activate your account</a> with the activation code you have received on your mobile.'),'error');
  }
	
  // Domain
  $domain = domain_get_domain();
 	
  // Table
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'title', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );
  
  $result = db_query("
  SELECT node.title AS node_title, node.nid AS nid, node.language AS node_language, node.created AS node_created
  FROM {node} node
  LEFT JOIN {domain_access} domain_access ON node.nid = domain_access.nid AND domain_access.realm = 'domain_id'
  WHERE (( (node.status = '1') AND (node.type IN  ('agenda')) AND (domain_access.gid = :did) AND (domain_access.realm = 'domain_id')))
  ",array(':did'=>$domain['domain_id']))->fetchAll();
  
  $destination = drupal_get_destination();
  
  $options = NULL;
  foreach ($result as $row) {
    $operation = array();
    $operation[] = array(
        '#type' => 'link',
        '#title' => t('Edit'),
        '#href' => 'node/' . $row->nid . '/edit',
        '#options' => array('query' => $destination),
    );
    $options[$row->nid] = array(
      'title' => l(check_plain($row->node_title), 'account/calendars/'.$row->nid),
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


function mc_campaign_calendar_hex_darker($hex,$factor = 10) {
  $new_hex = '';
        
        $base['R'] = hexdec($hex{0}.$hex{1});
        $base['G'] = hexdec($hex{2}.$hex{3});
        $base['B'] = hexdec($hex{4}.$hex{5});
        
        foreach ($base as $k => $v)
                {
                $amount = $v / 100;
                $amount = round($amount * $factor);
                $new_decimal = $v - $amount;
        
                $new_hex_component = dechex($new_decimal);
                if(strlen($new_hex_component) < 2)
                        { $new_hex_component = "0".$new_hex_component; }
                $new_hex .= $new_hex_component;
                }
                
        return $new_hex;        
}
