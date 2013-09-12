<?php

function mc_campaign_calendar_json($calendar_id) {
  
  // Domain
  $domain = domain_get_domain();
  
  $start = (int) $_GET['start'];
  $end = (int) $_GET['end'];
  
  // Save date in session.
  $_SESSION['current_date'] = date('Y-m-d', $end);

  $result = db_query("SELECT 
  
  node.nid,
  node.title as title,
  field_data_field_event_agenda.field_event_agenda_mc_custom_list,
  field_data_field_event_color.field_event_color_rgb as event_color,
  field_data_field_agenda_color.field_agenda_color_rgb as agenda_color,
  field_data_field_event_status.field_event_status_value as event_status,
  field_data_field_event_date.*,
  field_data_field_agenda_online_booking.field_agenda_online_booking_value,
  field_data_field_event_slot_type.field_event_slot_type_value,
  field_data_field_event_slot_minutes.field_event_slot_minutes_value,
  field_data_field_event_parent.field_event_parent_value
  
  FROM 
  
  {node} node

  LEFT JOIN {domain_access} domain_access ON node.nid = domain_access.nid AND domain_access.realm = 'domain_id'
  LEFT JOIN {field_data_field_event_date} field_data_field_event_date ON node.nid = field_data_field_event_date.entity_id AND (field_data_field_event_date.entity_type = 'node' AND field_data_field_event_date.deleted = '0')
  LEFT JOIN {field_data_field_event_agenda} field_data_field_event_agenda ON node.nid = field_data_field_event_agenda.entity_id AND (field_data_field_event_agenda.entity_type = 'node' AND field_data_field_event_agenda.deleted = '0')
  LEFT JOIN field_data_field_event_color field_data_field_event_color ON ( field_data_field_event_color.entity_id = node.nid AND field_data_field_event_color.deleted = '0' )
  LEFT JOIN field_data_field_agenda_color field_data_field_agenda_color ON ( field_data_field_agenda_color.entity_id = field_data_field_event_agenda.field_event_agenda_mc_custom_list AND field_data_field_agenda_color.deleted = '0' )
  LEFT JOIN field_data_field_agenda_online_booking field_data_field_agenda_online_booking ON ( field_data_field_agenda_online_booking.entity_id = field_data_field_event_agenda.field_event_agenda_mc_custom_list )
  LEFT JOIN field_data_field_event_status field_data_field_event_status ON ( field_data_field_event_status.entity_id = node.nid AND field_data_field_event_status.deleted = '0' )
  LEFT JOIN field_data_field_event_slot_type field_data_field_event_slot_type ON ( field_data_field_event_slot_type.entity_id = node.nid AND field_data_field_event_slot_type.deleted = '0' )
  LEFT JOIN field_data_field_event_slot_minutes field_data_field_event_slot_minutes ON ( field_data_field_event_slot_minutes.entity_id = node.nid AND field_data_field_event_slot_minutes.deleted = '0' )
  LEFT JOIN field_data_field_event_parent field_data_field_event_parent ON ( field_data_field_event_parent.entity_id = node.nid AND field_data_field_event_parent.deleted = '0' )
  
  WHERE 
  (
    ( 
      (domain_access.realm = 'domain_id') 
    )
    AND
    (
      node.status = '1'
      AND node.type IN  ('event')
      AND domain_access.gid IN  (:did)
      AND field_data_field_event_date.field_event_date_value >= :start
      AND field_data_field_event_date.field_event_date_value2 <= :end 
      AND field_data_field_event_agenda.field_event_agenda_mc_custom_list = :calendar_id
      AND field_data_field_agenda_online_booking.field_agenda_online_booking_value != 'private'
      AND ( field_data_field_event_status.field_event_status_value = 'provisional' OR field_data_field_event_parent.field_event_parent_value IS NOT NULL )
    )
  )
  
  ORDER BY field_data_field_event_date.field_event_date_value ASC
  
  ",array(':did'=>$domain['domain_id'], ':start'=>$start, ':end'=>$end, ':calendar_id'=>$calendar_id));
  
  $reponses = array();
  
  foreach ($result as $row) {
    
    if (isset($row->field_event_parent_value)) {
      
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['allDay'] = FALSE;
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['start'] = $row->field_event_date_value;
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['end'] = $row->field_event_date_value2;
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['id'] = 1;
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['title'] = t('FULL');
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['className'] = 'busy';
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['status'] = 'busy';
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['calendar_id'] = $calendar_id;
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['parent_id'] = $row->nid;
      $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['editable'] = FALSE;
	  $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['backgroundColor'] = '#EEE';
	  $reponses[$row->field_event_parent_value . '-' . $row->field_event_date_value]['borderColor'] = '#EEE';
      
    } elseif ($row->event_status = 'provisional' && $row->field_event_slot_type_value == 'multiple') {

      for ($start = $row->field_event_date_value; $start < $row->field_event_date_value2; $start = $start + (60 * $row->field_event_slot_minutes_value)) {
        $temp = (int) $start;
        $reponses[$row->nid . '-' . $start]['allDay'] = FALSE;
        $reponses[$row->nid . '-' . $start]['start'] = "" . $temp;
        $reponses[$row->nid . '-' . $start]['end'] = "" . $temp + (60 * $row->field_event_slot_minutes_value);
        $reponses[$row->nid . '-' . $start]['id'] = 1;
        $reponses[$row->nid . '-' . $start]['title'] = 'SELECT';
        $reponses[$row->nid . '-' . $start]['className'] = 'free';
        $reponses[$row->nid . '-' . $start]['status'] = 'free';
        $reponses[$row->nid . '-' . $start]['calendar_id'] = $calendar_id;
        $reponses[$row->nid . '-' . $start]['parent_id'] = $row->nid;
        $reponses[$row->nid . '-' . $start]['editable'] = FALSE;
	    $reponses[$row->nid . '-' . $start]['backgroundColor'] = $row->agenda_color;
	    $reponses[$row->nid . '-' . $start]['borderColor'] = !empty($row->event_color) ? $row->event_color : '#' . mc_campaign_calendar_hex_darker(drupal_substr($row->agenda_color,1));
      }
      	  
    } elseif ($row->event_status = 'provisional' && $row->field_event_slot_type_value == 'single') {
      
      $reponses[$row->nid . '-' . $row->field_event_date_value]['allDay'] = FALSE;
      $reponses[$row->nid . '-' . $row->field_event_date_value]['start'] = $row->field_event_date_value;
      $reponses[$row->nid . '-' . $row->field_event_date_value]['end'] = $row->field_event_date_value2;
      $reponses[$row->nid . '-' . $row->field_event_date_value]['id'] = 1;
      $reponses[$row->nid . '-' . $row->field_event_date_value]['title'] = 'SELECT';
      $reponses[$row->nid . '-' . $row->field_event_date_value]['className'] = 'free';
      $reponses[$row->nid . '-' . $row->field_event_date_value]['status'] = 'free';
      $reponses[$row->nid . '-' . $row->field_event_date_value]['calendar_id'] = $calendar_id;
      $reponses[$row->nid . '-' . $row->field_event_date_value]['parent_id'] = $row->nid;
      $reponses[$row->nid . '-' . $row->field_event_date_value]['editable'] = FALSE;
	  $reponses[$row->nid . '-' . $row->field_event_date_value]['backgroundColor'] = $row->agenda_color;
	  $reponses[$row->nid . '-' . $row->field_event_date_value]['borderColor'] = !empty($row->event_color) ? $row->event_color : '#' . mc_campaign_calendar_hex_darker(drupal_substr($row->agenda_color,1));
    
    }
  
  }
  
  $responses = array();
  foreach ($reponses as $reponse) {
    array_push($responses, $reponse);
  }
  
  drupal_json_output($responses);
  drupal_exit();
  
}



function mc_campaign_calendar_update() {
  
  $start = (int) $_POST['start'];
  $end = (int) $_POST['end'];
  $allday = $_POST['allday'];
  $mail = $_POST['mail'];
  $parent_id = (int) $_POST['parent_id'];
  $calendar_id = (int) $_POST['calendar_id'];
  $domain = domain_get_domain();
  
  // Get calendar info.
  $calendar = node_load($calendar_id);
  //$calendar->field_agenda_color[LANGUAGE_NONE][O]['rgb'];
  //$calendar->title;
  
  // Get subscriber.
  $subscriber = new stdClass();
  $subscriber->eid = NULL;
  $subscriber->email = $mail;
  $subscriber->mobile = NULL;
  $subscriber->did = $domain['domain_id'];
  
  if ($calendar->field_agenda_online_booking[LANGUAGE_NONE][0][value] == 'public' || mc_campaign_is_subscribed($subscriber)) {
    
    $event = node_load($parent_id);
    $event->is_new = TRUE;
    $event->vid = NULL;
    $event->nid = NULL;
    $event->field_event_parent[LANGUAGE_NONE][0][value] = $parent_id;
    $event->field_event_date[LANGUAGE_NONE][0][value] = $start;
    $event->field_event_date[LANGUAGE_NONE][0][value2] = $end;
    $event->field_event_status[LANGUAGE_NONE][0][value] = 'provisional';
    $event->field_event_subscriber[LANGUAGE_NONE][0][key] = 'sid-' . $subscriber->sid;
    $event->title = t('Booked');
    unset($event->field_event_uid);
    node_save($event);
    
    // Build email
    $params['path'] = $domain['path'];
    $params['delete_path'] = l(t('delete'), $domain['path'] . 'online/calendars/delete/' . $event->field_event_uid[LANGUAGE_NONE][0]['value']);
    $params['confirm_path'] = l(t('confirm'), $domain['path'] . 'online/calendars/confirm/' . $event->field_event_uid[LANGUAGE_NONE][0]['value']);
    $params['startdate'] =  date('d/m/Y', $event->field_event_date[LANGUAGE_NONE][0][value]);
    $params['enddate'] =  date('d/m/Y', $event->field_event_date[LANGUAGE_NONE][0][value2]);
    $params['starttime'] =  date('H:i', $event->field_event_date[LANGUAGE_NONE][0][value]);
    $params['endtime'] =  date('H:i', $event->field_event_date[LANGUAGE_NONE][0][value]);
    $params['message'] =  'event-provisional';
    $params['language'] =  'en';
    
    $subject = t('Confirmation');
    $body = array();
    
    mc_campaign_calendar_mail($subscriber->email, $subject, $body, $params, $attachment);

    drupal_json_output(TRUE);
    
  } else {
    
    drupal_json_output(FALSE);
    
  }
  
  drupal_exit();

}



/**
 * Form builder; Campaign Dashboard
 */
function mc_campaign_calendar_calendar($calendar_id = null) {
  
  $date = (isset($_SESSION['current_date'])) ? $_SESSION['current_date'] : date('Y-m-d');
  $date = explode("-", $date);
  
  $_SESSION['current_calendar'] = NULL;
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
    LEFT JOIN field_data_field_agenda_online_booking field_data_field_agenda_online_booking ON ( field_data_field_agenda_online_booking.entity_id = node.nid )
	WHERE (( (node.status = '1') AND (domain_access.gid IN  (:did)) AND (domain_access.realm = 'domain_id') )AND(( (node.type IN  ('agenda')) )) )
	AND field_data_field_agenda_online_booking.field_agenda_online_booking_value != 'private'
	GROUP BY node.nid
	",array(':did'=>$domain['domain_id']));

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
    $row->field_agenda_selectable = TRUE;
    $row->field_agenda_editable = TRUE;
    $row->field_agenda_color = ($row->field_agenda_color) ? $row->field_agenda_color : 'DDDDDD';
    $row->field_agenda_firsthour = ($row->field_agenda_firsthour) ? (int) $row->field_agenda_firsthour : 0;
    $row->field_agenda_maxtime = isset($row->field_agenda_maxtime) ? (int) $row->field_agenda_maxtime : 24;
    $row->field_agenda_mintime = isset($row->field_agenda_mintime) ? (int) $row->field_agenda_mintime : 6;
    $row->field_agenda_slotminutes = ($row->field_agenda_slotminutes) ? (int) $row->field_agenda_slotminutes : 60;
    $row->class = ($_SESSION['enable_calendar'][$row->nid]) ? 'enable' : '';
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
    '#theme' => 'mc_campaign_online_calendar',
    '#title' => 'titlet',
    '#calendars' => $calendars,
  );

  $output = drupal_render($build);
  
  return $output;
  
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

function mc_campaign_calendar_confirm($uuid) {
  
  $nid = db_query("SELECT entity_id FROM {field_revision_field_event_uid} WHERE field_event_uid_value = :uid", array(':uid' => $uuid))->fetchField();
  $event = node_load($nid);
  $event->field_event_status[LANGUAGE_NONE][0]['value'] = 'requested';
  $calendar = node_load($event->field_event_agenda[LANGUAGE_NONE][0]['mc_custom_list']);
  
  // SEND NOTIFICATION
  if ($calendar->field_agenda_notification_user) {
    
    foreach ($calendar->field_agenda_notification_user[LANGUAGE_NONE] as $key => $value) {
      
      if ($value['requested_notification']) {
        
        // Load user.
        $user = user_load($value['user']);
        
         // Build email
        $params['path'] = $domain['path'];
        $params['delete_path'] = l(t('delete'), $domain['path'] . 'online/calendars/delete/' . $event->field_event_uid[LANGUAGE_NONE][0]['value']);
        $params['confirm_path'] = l(t('confirm'), $domain['path'] . 'online/calendars/confirm/' . $event->field_event_uid[LANGUAGE_NONE][0]['value']);
        $params['startdate'] =  date('d/m/Y', $event->field_event_date[LANGUAGE_NONE][0][value]);
        $params['enddate'] =  date('d/m/Y', $event->field_event_date[LANGUAGE_NONE][0][value2]);
        $params['starttime'] =  date('H:i', $event->field_event_date[LANGUAGE_NONE][0][value]);
        $params['endtime'] =  date('H:i', $event->field_event_date[LANGUAGE_NONE][0][value]);
        $params['message'] =  'event-requested';
        $params['language'] =  ($user->language) ? $user->language : 'en';
        
        $subject = t('Request');
        $body = array();
        
        mc_campaign_calendar_mail($user->mail, $subject, $body, $params, $attachment);
        
      }
    }
  }
  
  return '';
      
}

function mc_campaign_calendar_delete($uuid) {
  
  $nid = db_query("SELECT entity_id FROM {field_revision_field_event_uid} WHERE field_event_uid_value = :uid", array(':uid' => $uuid))->fetchField();
  
  dpm($nid);
  
  node_delete($nid);
  
  return '';
  
}

function mc_campaign_calendar_mail($email, $subject, $body, $params, $attachment) {
  
  $domain = domain_get_domain();
  $module = 'mc_campaign';
  $key = 'onlinecalendar';
  
  // Get user info
  $result = db_query("SELECT uid FROM {domain_editor} WHERE domain_id = :domain_id", array(':domain_id' => $domain['domain_id']))->fetchObject();
  $account = user_load($result->uid);
  
  $language = user_preferred_language($account);
  $params['langcode'] = $language->language;
  
  $params['site_name'] = $account->field_site_name['und'][0]['value'];
  $params['online_mobile'] = $account->field_online_mobile[LANGUAGE_NONE][0]['safe_value'];
  $params['company_facebook'] = $account->field_company_facebook[LANGUAGE_NONE][0]['safe_value'];
  $params['company_twitter'] = $account->field_company_twitter[LANGUAGE_NONE][0]['twitter_username'];
  
  $params['logo'] = $domain['path'] . 'sites/all/themes/mctimetracker/images/html_header-mctimetracker.jpg';
  if (!empty($account->field_logo)){
    $params['logo'] = str_replace('public://', $domain['path'] . 'sites/default/files/',$account->field_logo[LANGUAGE_NONE][0]['uri']);
  }
              
  $message = drupal_mail($module, $key, $email, $language, $params, $from, $send);

  // Fill in
  $message['subject'] = $subject;
  $message['body'] = $body;
  
  // Retrieve the responsible implementation for this message.
  $system = drupal_mail_system($module, $key);
  
  // Format the message body.
  $message = $system->format($message);
  
  $message['result'] = $system->mail($message);

  // Send e-mail.
  if($message['result']) {
  	drupal_set_message(t('HTML Message sent to ' . $email));
  } else {
    drupal_set_message(t('HTML message not sent.'),'error');
  }        

}
