<?php

function mctimetracker_calendar_full() {
	
  global $user;
  
  $calendars = _mctimetracker_calendar_get($user->uid);
  dpm($calendars);
  
  $calendarsdropdow = _mctimetracker_calendar_get($user->uid);
  dpm($calendarsdropdow);
  
  $build['calendar'] = array(
    '#theme' 		=> 'mctimetracker_calendar_full',
    '#mycalendar' 	=> theme('item_list', array('items' => $calendars['span'], 'title' => t('My Calendar'))),
  	'#addcalendar' 	=> "<div id='calendars_my_links'><span id='calendars_my_links_add' class='link'>Add</span> <span id='calendars_my_links_param' class='link'>Param</span></div>",
  );
  
  $output = drupal_render($build);
  
  return $output;
  
}

