<?php

define('GOOGLE_CHART', 'http://www.google.com/jsapi');

/**
 * Form builder; Campaign Dashboard
 */
function mc_campaign_admin_dashboad() {
	
  // Domain
  $domain = domain_get_domain();
  
  // LOAD JS
  $js_options = array('group' => JS_LIBRARY);
  drupal_add_js(GOOGLE_CHART, $js_options);

  // LOAD CSS  
  //drupal_add_js(drupal_get_path('module', 'mc_campaign') . '/js/jquery.layout-latest.js');
  drupal_add_css(drupal_get_path('module', 'mc_campaign') . '/css/dashboard.css');
  
  //prepare data
  $data['subscriber'] = db_query("SELECT COUNT('sid') FROM {mc_campaign_subscribers} WHERE did = :did",array(':did'=>$domain['domain_id']))->fetchField();
  $data['active_list'] = db_query("SELECT COUNT('lid') FROM {mc_campaign_lists} where active = 1 AND did = :did AND deleted = 0",array(':did'=>$domain['domain_id']))->fetchField();
  $data['sms_sent'] = db_query("SELECT COUNT('nid') FROM {mc_campaign_notifications} WHERE media = :media AND did = :did AND deleted = 0 AND sent > 0", array(':did'=>$domain['domain_id'],':media' => 'sms'))->fetchField();
  $data['mail_sent'] = db_query("SELECT COUNT('nid') FROM {mc_campaign_notifications} WHERE media = :media AND did = :did AND deleted = 0 AND sent > 0", array(':did'=>$domain['domain_id'],':media' => 'mail'))->fetchField();

  // get sms sent for last week
  $results = db_query("SELECT 
    from_unixtime(sent,'%Y-%m-%d') as date, 
    from_unixtime(sent,'%d-%m-%Y') as format_date, 
    COUNT(IF(sent>0,1, NULL)) as sent_count,
    COUNT(IF(compt>2,1, NULL)) as error,
    COUNT(IF(received>0,1, NULL)) as received_count
    FROM {mc_campaign_notifications} 
    WHERE media='sms' AND did = :did AND deleted = 0 and sent > UNIX_TIMESTAMP() - 604800 
    GROUP BY from_unixtime(sent,'%Y-%m-%d') ORDER BY sent", array(':did' => $domain['domain_id']))->fetchAll();
  
  $sms = array();
  $sms[] = array('Date','Sent','Error','Received','Not Received');
  foreach ($results as $result) {
  	$sms[] = array($result->format_date,(int)$result->sent_count,(int)$result->error,(int)$result->received_count,((int)$result->sent_count-(int)$result->received_count));
  }

  // get mail sent for last week
  $results = db_query("SELECT
    from_unixtime(sent,'%Y-%m-%d') as date, 
    from_unixtime(sent,'%d-%m-%Y') as format_date, 
    COUNT(IF(sent>0,1, NULL)) as sent_count,
    COUNT(IF(compt>2,1, NULL)) as error,
    COUNT(IF(received>0,1, NULL)) as received_count,
    count(nid) as created_count 
    FROM {mc_campaign_notifications} 
    WHERE media='mail' AND did = :did AND deleted = 0 and sent > UNIX_TIMESTAMP() - 604800 
    GROUP BY from_unixtime(created,'%Y-%m-%d') ORDER BY created", array(':did' => $domain['domain_id']))->fetchAll();
  $mail = array();
  $mail[] = array('Date','Sent','Error','Received','Not Received');
  foreach ($results as $result) {
  	$mail[] = array($result->format_date,(int)$result->sent_count,(int)$result->error,(int)$result->received_count,((int)$result->sent_count-(int)$result->received_count));
  }

  // get call
  $call = array();
  
  drupal_add_js(array(
    'sms' => $sms,
    'mail' => $mail,
    'call' => $call,
    ), 'setting'
  );  
  
  $title = t('Campaign Dashboard');
  
  $build['dashboard'] = array(
    '#theme' => 'mc_campaign_dashboard',
    '#title' => $title,
    '#data' => $data,
    '#notifications' => '',
  );

  $build['dashboard']['#attached']['js'][] = array('data' => '(function($){$(function() { 


  })})(jQuery);', 'type' => 'inline');
  
  $output = drupal_render($build);
  
  return $output;
  
}








/**
 * Form builder; Campaign People List
 *
 */
function campaign_admin_account() {

  $header = array(
    'username' => array('data' => t('Username'), 'field' => 'u.name'),
    'field_user_first_name' => array('data' => t('First Name')),
    'field_user_last_name' => array('data' => t('Last Name')),
    'mail' => array('data' => t('Mail')),
    'field_user_mobile_phone' => array('data' => t('Mobile')),
    'field_user_accept_mail' => array('data' => t('Mail')),
    'field_user_check_sms' => array('data' => t('SMS')),
    'field_user_accept_letter' => array('data' => t('Letter')),
    'type' => array('data' => t('Type')),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('users', 'u');
  $query->condition('u.uid', 0, '<>');
  user_build_filter_query($query);

  $count_query = clone $query;
  $count_query->addExpression('COUNT(u.uid)');

  $query = $query->extend('PagerDefault')->extend('TableSort');
  $query
    ->fields('u', array('uid', 'name', 'mail', 'created', 'access'))
    ->limit(50)
    ->orderByHeader($header)
    ->setCountQuery($count_query);
  $result = $query->execute();

  $destination = drupal_get_destination();

  $status = array(t('blocked'), t('active'));
  $roles = array_map('check_plain', user_roles(TRUE));
  $accounts = array();
  foreach ($result as $account) {
  	
  	$user_fields = user_load($account->uid);
  	
    $field_user_first_name 		= field_get_items('user', $user_fields, 'field_user_first_name');
    $field_user_last_name 		= field_get_items('user', $user_fields, 'field_user_last_name');
    $field_user_mobile_phone 	= field_get_items('user', $user_fields, 'field_user_mobile_phone');
    $field_user_accept_mail 	= field_get_items('user', $user_fields, 'field_user_accept_mail');
    $field_user_accept_sms 		= field_get_items('user', $user_fields, 'field_user_accept_sms');
    $field_user_accept_letter 	= field_get_items('user', $user_fields, 'field_user_accept_letter');
    
    $options[$account->uid] = array(
    	'username' => theme('username', array('account' => $account)),
    	'field_user_first_name' 	=> $field_user_first_name[0]['value'],
    	'field_user_last_name' 		=> $field_user_first_name[0]['value'],
    	'mail' 						=> $account->mail,
    	'field_user_mobile_phone' 	=> $field_user_mobile_phone[0]['value'],
    	'field_user_accept_mail' 	=> $field_user_accept_mail[0]['value'],
    	'field_user_check_sms'		=> $field_user_accept_sms[0]['value'],
    	'field_user_accept_letter' 	=> $field_user_accept_letter[0]['value'],
    	'type' 						=> $field_user_accept_letter[0]['value'],
      	'operations' 				=> array('data' => array('#type' => 'link', '#title' => t('edit'), '#href' => "user/$account->uid/edit", '#options' => array('query' => $destination))),
    );
  }

  $form['accounts'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No people available.'),
  );
  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}


function user_filter_form() {
  $session = isset($_SESSION['user_overview_filter']) ? $_SESSION['user_overview_filter'] : array();
  $filters = user_filters();

  $i = 0;
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only users where'),
    '#theme' => 'exposed_filters__user',
  );
  foreach ($session as $filter) {
    list($type, $value) = $filter;
    if ($type == 'permission') {
      // Merge arrays of module permissions into one.
      // Slice past the first element '[any]' whose value is not an array.
      $options = call_user_func_array('array_merge', array_slice($filters[$type]['options'], 1));
      $value = $options[$value];
    }
    else {
      $value = $filters[$type]['options'][$value];
    }
    $t_args = array('%property' => $filters[$type]['title'], '%value' => $value);
    if ($i++) {
      $form['filters']['current'][] = array('#markup' => t('and where %property is %value', $t_args));
    }
    else {
      $form['filters']['current'][] = array('#markup' => t('%property is %value', $t_args));
    }
  }

  $form['filters']['status'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('clearfix')),
    '#prefix' => ($i ? '<div class="additional-filters">' . t('and where') . '</div>' : ''),
  );
  $form['filters']['status']['filters'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('filters')),
  );
  foreach ($filters as $key => $filter) {
    $form['filters']['status']['filters'][$key] = array(
      '#type' => 'select',
      '#options' => $filter['options'],
      '#title' => $filter['title'],
      '#default_value' => '[any]',
    );
  }

  $form['filters']['status']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['filters']['status']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => (count($session) ? t('Refine') : t('Filter')),
  );
  if (count($session)) {
    $form['filters']['status']['actions']['undo'] = array(
      '#type' => 'submit',
      '#value' => t('Undo'),
    );
    $form['filters']['status']['actions']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
    );
  }

  drupal_add_library('system', 'drupal.form');

  return $form;
}


/**
 * Form builder; Campaign People List
 *
 */
function campaign_admin_notification() {

  $header = array(
    'eid' => array('data' => t('Notification ID'), 'field' => 'mcn.eid'),
    'uid' => array('data' => t('User ID'), 'field' => 'mcn.uid'),
    'send_expected' => array('data' => t('Time'), 'field' => 'mcn.send_expected'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('mcnotification_event', 'mcn');

  $count_query = clone $query;
  $count_query->addExpression('COUNT(mcn.eid)');

  $query = $query->extend('PagerDefault')->extend('TableSort');
  $query
    ->fields('mcn', array('eid', 'uid', 'type', 'send_expected'))
    ->limit(50)
    ->orderByHeader($header)
    ->setCountQuery($count_query);
  $result = $query->execute();
  
  $destination = drupal_get_destination();

  $notifications = array();
  foreach ($result as $notification) {

  	
    $notifications[$notification->uid] = array(
      'uid' => $notification->eid,
      'uid' => theme('username', array('account' => $notification)),
      'send_expected' =>  t('@time ago', array('@time' => format_interval(REQUEST_TIME - $notification->send_expected))),
      'operations' => array('data' => array('#type' => 'link', '#title' => t('edit'), '#href' => "user/$notification->uid/edit", '#options' => array('query' => $destination))),
    );
    
  }
  
  $form['accounts'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $notifications,
    '#empty' => t('No notification available.'),
  );
  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}









































/**
 * @file
 * Administrative page callbacks for Mollom module.
 */

/**
 * Menu callback; Displays a list of forms configured for Mollom.
 */
function campaign_admin_form_list() {

  $header = array(
    t('Form'),
    t('Protection mode'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  
  $rows = null;
  
  $build['forms'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => l(t('Add form'), 'admin/config/campaign/add'),
  );

  return $build;
}

/**
 * Return registered forms as an array suitable for a 'checkboxes' form element #options property.
 */
function campaign_admin_form_options() {
  // Retrieve all registered forms.
  $form_list = campaign_form_list();

  // Remove already configured form ids.
  $result = db_query('SELECT form_id FROM {campaign_form}')->fetchCol();
  foreach ($result as $form_id) {
    unset($form_list[$form_id]);
  }
  // If all registered forms are configured already, output a message, and
  // redirect the user back to overview.
  if (empty($form_list)) {
    drupal_set_message(t('All available forms are protected already.'));
    drupal_goto('admin/config/content/campaign');
  }

  // Load module information.
  $module_info = system_get_info('module');

  // Transform form information into an associative array suitable for #options.
  $options = array();
  foreach ($form_list as $form_id => $info) {
    // system_get_info() only supports enabled modules. Default to the module's
    // machine name in case it is disabled.
    $module = $info['module'];
    if (!isset($module_info[$module])) {
      $module_info[$module]['name'] = $module;
    }
    $options[$form_id] = t('!module: !form-title', array(
      '!form-title' => $info['title'],
      '!module' => t($module_info[$module]['name']),
    ));
  }
  // Sort form options by title.
  asort($options);

  return $options;
}

/**
 * Form builder; Configure campaign protection for a form.
 */
function campaign_admin_configure_form($form, &$form_state, $campaign_form = NULL) {
  // If no $campaign_form was passed, then we are adding a new form configuration.
  if (!isset($campaign_form)) {
    if (!isset($form_state['storage']['campaign_form'])) {
      $form_state['storage']['step'] = 'select';
    }
    else {
      $form_state['storage']['step'] = 'configure';
      $campaign_form = $form_state['storage']['campaign_form'];
    }
  }
  // When adding a new form configuration, passing form_id via path argument.
  elseif (is_string($campaign_form)) {
    $campaign_form = campaign_form_new($campaign_form);
    $form_state['storage']['step'] = 'configure';
    $form_state['storage']['campaign_form'] = $campaign_form;
  }
  // Otherwise, we are editing an existing form configuration.
  else {
    $form_state['storage']['step'] = 'configure';
    $form_state['storage']['campaign_form'] = $campaign_form;
  }

  $form['#tree'] = TRUE;
  $form['actions'] = array(
    '#type' => 'actions',
  );

  switch ($form_state['storage']['step']) {
    case 'select':
      $form['campaign']['form_id'] = array(
        '#type' => 'select',
        '#title' => t('Form'),
        '#options' => campaign_admin_form_options(),
        '#required' => TRUE,
      );
      $form['actions']['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
        '#submit' => array('campaign_admin_configure_form_next_submit'),
      );
      break;

    case 'configure':
      drupal_set_title(t('Configure %form-title protection', array('%form-title' => $campaign_form['title'])), PASS_THROUGH);

      $form['campaign']['form_id'] = array(
        '#type' => 'value',
        '#value' => $campaign_form['form_id'],
      );

      $modes = array();
      // Textual analysis, if any elements are available.
      if (!empty($campaign_form['elements'])) {
        $modes[campaign_MODE_ANALYSIS] = t('Text analysis');
      }
      // CAPTCHA-only, always available.
      $modes[campaign_MODE_CAPTCHA] = t('CAPTCHA');

      $form['campaign']['mode'] = array(
        '#type' => 'radios',
        '#title' => t('Protection mode'),
        '#options' => $modes,
        '#default_value' => isset($campaign_form['mode']) ? $campaign_form['mode'] : key($modes),
      );

      $all_permissions = array();
      foreach (module_implements('permission') as $module) {
        if ($module_permissions = module_invoke($module, 'permission')) {
          foreach ($module_permissions as &$info) {
            $info += array('module' => $module);
          }
          $all_permissions += $module_permissions;
        }
      }
      // Prepend campaign's global permission to the list.
      array_unshift($campaign_form['bypass access'], 'bypass campaign protection');
      $permissions = array();
      foreach ($campaign_form['bypass access'] as $permission) {
        // @todo D7: Array keys are used as CSS class for the link list item,
        //   but are not sanitized: http://drupal.org/node/98696
        $permissions[drupal_html_class($permission)] = array(
          'title' => $all_permissions[$permission]['title'],
          'href' => 'admin/people/permissions',
          'fragment' => 'module-' . $all_permissions[$permission]['module'],
          'html' => TRUE,
        );
      }
      $form['campaign']['mode']['#description'] = t('The protection is omitted for users having any of the permissions: !permission-list', array(
        '!permission-list' => theme('links', array(
          'links' => $permissions,
          // @todo D7: Something went entirely wrong: system.menus.css makes ANY
          //   ul.links appear as if it would have the .inline CSS class.
          'attributes' => array(),
        )),
      ));

      if (!empty($campaign_form['elements'])) {
        // If not re-configuring an existing protection, make it the default.
        if (!isset($campaign_form['mode'])) {
          $form['campaign']['mode']['#default_value'] = campaign_MODE_ANALYSIS;
        }

        // Textual analysis filters.
        $form['campaign']['checks'] = array(
          '#type' => 'checkboxes',
          '#title' => t('Analyze text for'),
          '#options' => array(
            'spam' => t('Spam'),
            'profanity' => t('Profanity'),
          ),
          '#default_value' => $campaign_form['checks'],
          '#states' => array(
            'visible' => array(
              ':input[name="campaign[mode]"]' => array('value' => (string) campaign_MODE_ANALYSIS),
            ),
          ),
        );

        // Form elements defined by hook_campaign_form_info() use the
        // 'parent][child' syntax, which Form API also uses internally for
        // form_set_error(), and which allows us to recurse into nested fields
        // during processing of submitted form values. However, since we are using
        // those keys also as internal values to configure the fields to use for
        // textual analysis, we need to encode them. Otherwise, a nested field key
        // would result in the following checkbox attribute:
        //   '#name' => 'campaign[enabled_fields][parent][child]'
        // This would lead to a form validation error, because it is a valid key.
        // By encoding them, we prevent this from happening:
        //   '#name' => 'campaign[enabled_fields][parent%5D%5Bchild]'
        $elements = array();
        foreach ($campaign_form['elements'] as $key => $value) {
          $elements[rawurlencode($key)] = $value;
        }
        $enabled_fields = array();
        foreach ($campaign_form['enabled_fields'] as $value) {
          $enabled_fields[] = rawurlencode($value);
        }
        $form['campaign']['enabled_fields'] = array(
          '#type' => 'checkboxes',
          '#title' => t('Text fields to analyze'),
          '#options' => $elements,
          '#default_value' => $enabled_fields,
          '#states' => array(
            'visible' => array(
              ':input[name="campaign[mode]"]' => array('value' => (string) campaign_MODE_ANALYSIS),
            ),
          ),
        );

        $form['campaign']['discard'] = array(
          '#type' => 'radios',
          '#title' => t('When text analysis identifies spam'),
          '#default_value' => $campaign_form['discard'],
          '#options' => array(
            1 => t('Automatically discard the post'),
            0 => t('Retain the post for manual moderation'),
          ),
          '#required' => $campaign_form['mode'] == campaign_MODE_ANALYSIS,
          // Only possible for forms supporting moderation of unpublished posts.
          '#access' => !empty($campaign_form['moderation callback']),
          // Only possible for forms protected via text analysis.
          '#states' => array(
            'visible' => array(
              ':input[name="campaign[mode]"]' => array('value' => (string) campaign_MODE_ANALYSIS),
            ),
          ),
        );
      }

      $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
      );
      break;
  }

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/content/campaign',
  );

  return $form;
}

/**
 * Form submit handler for 'Next' button on campaign form configuration form.
 */
function campaign_admin_configure_form_next_submit($form, &$form_state) {
  $form_id = $form_state['values']['campaign']['form_id'];
  $form_state['redirect'] = $_GET['q'] . '/' . $form_id;
}

/**
 * Form validation handler for campaign_admin_configure_form().
 */
function campaign_admin_configure_form_validate(&$form, &$form_state) {
  // For the 'configure' step, output custom #required form element errors for
  // 'checks' and 'enabled_fields', as their labels do not work with the default
  // #required form error message.
  if ($form_state['storage']['step'] == 'configure') {
    // Make field checkboxes required, if protection mode is textual analysis.
    // @see http://drupal.org/node/875722
    $required = ($form_state['values']['campaign']['mode'] == campaign_MODE_ANALYSIS);
    $form['campaign']['checks']['#required'] = $required;
    $form['campaign']['enabled_fields']['#required'] = $required;
    $form['campaign']['discard']['#required'] = $required;

    if ($required && !array_filter($form_state['values']['campaign']['checks'])) {
      form_error($form['campaign']['checks'], t('At least one text analysis check is required.'));
    }
    if ($required && !array_filter($form_state['values']['campaign']['enabled_fields'])) {
      form_error($form['campaign']['enabled_fields'], t('At least one field is required for text analysis.'));
    }
  }
}

/**
 * Form submit handler for campaign_admin_configure_form().
 */
function campaign_admin_configure_form_submit($form, &$form_state) {
  $campaign_form = $form_state['values']['campaign'];
  // Merge in form information from $form_state.
  $campaign_form += $form_state['storage']['campaign_form'];

  // Only store a list of enabled textual analysis checks.
  $campaign_form['checks'] = array_keys(array_filter($campaign_form['checks']));
  // Prepare selected fields for storage.
  $enabled_fields = array();
  foreach (array_keys(array_filter($campaign_form['enabled_fields'])) as $field) {
    $enabled_fields[] = rawurldecode($field);
  }
  $campaign_form['enabled_fields'] = $enabled_fields;

  $status = campaign_form_save($campaign_form);
  if ($status === SAVED_NEW) {
    drupal_set_message(t('The form protection has been added.'));
  }
  else {
    drupal_set_message(t('The form protection has been updated.'));
  }

  $form_state['redirect'] = 'admin/config/content/campaign';
}

/**
 * Form builder; Remove campaign protection from a form.
 */
function campaign_admin_unprotect_form($form, &$form_state, $campaign_form) {
  $form['#tree'] = TRUE;
  $form['form'] = array(
    '#type' => 'item',
    '#title' => t('Form'),
    '#markup' => $campaign_form['title'],
  );
  $form['campaign']['form_id'] = array(
    '#type' => 'value',
    '#value' => $campaign_form['form_id'],
  );

  return confirm_form($form,
    t('Are you sure you want to unprotect this form?'),
    'admin/config/content/campaign',
    t('campaign will no longer protect this form from spam.')
  );
}

/**
 * Form submit handler for campaign_admin_unprotect_form().
 */
function campaign_admin_unprotect_form_submit($form, &$form_state) {
  campaign_form_delete($form_state['values']['campaign']['form_id']);

  drupal_set_message(t('The form protection has been removed.'));

  $form_state['redirect'] = 'admin/config/content/campaign';
}

/**
 * Form constructor to configure the blacklist.
 *
 * @param $type
 *   The type of blacklist; i.e., 'spam', 'profanity', or 'unwanted'.
 */
function campaign_admin_blacklist_form($form, &$form_state, $type = 'spam') {
  $form['#tree'] = TRUE;

  // Translate internal reason values for rendering and select list in form.
  $contexts = array(
    'everything' => t('All fields'),
    'links' => t('Links'),
    'author' => t('Author name'),
  );
  $matches = array(
    'contains' => t('contains'),
    'exact' => t('exact'),
  );

  $form['blacklist'] = array();
  // Do not retrieve the current blacklist when submitting the form.
  $blacklist = (empty($form_state['input']) ? campaign('campaign.listBlacklistText') : array());
  if (is_array($blacklist)) {
    foreach ($blacklist as $id => $entry) {
      if ($entry['reason'] != $type) {
        continue;
      }
      // #class property is internally used by
      // theme_campaign_admin_blacklist_form().
      $row = array(
        'context' => array(
          '#markup' => check_plain($contexts[$entry['context']]),
          '#class' => 'campaign-blacklist-context value-' . check_plain($entry['context']),
        ),
        'match' => array(
          '#markup' => check_plain($matches[$entry['match']]),
          '#class' => 'campaign-blacklist-match value-' . check_plain($entry['match']),
        ),
        'text' => array(
          '#markup' => check_plain($entry['text']),
          '#class' => 'campaign-blacklist-text',
        ),
      );
      $row['actions']['delete'] = array(
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => 'admin/config/content/campaign/blacklist/delete',
        '#options' => array(
          'query' => array(
            'text' => $entry['text'],
            'context' => $entry['context'],
            'reason' => $entry['reason'],
          ) + drupal_get_destination(),
        ),
      );
      $form['blacklist'][$id] = $row;
    }
  }

  // The value NULL in the following select lists will intentionally trigger a
  // form validation error, since the elements are #required.
  // @see http://drupal.org/node/140783
  $form['entry']['context'] = array(
    '#type' => 'select',
    '#title' => t('Context'),
    '#title_display' => 'invisible',
    '#options' => array(NULL => t('- Any -')) + $contexts,
    '#required' => TRUE,
    '#id' => 'campaign-blacklist-filter-context',
  );
  $form['entry']['match'] = array(
    '#type' => 'select',
    '#title' => t('Match'),
    '#title_display' => 'invisible',
    '#options' => array(NULL => t('- Any -')) + $matches,
    '#required' => TRUE,
    '#id' => 'campaign-blacklist-filter-match',
  );
  $form['entry']['text'] = array(
    '#type' => 'textfield',
    '#title' => t('Text'),
    '#title_display' => 'invisible',
    '#size' => 40,
    '#required' => TRUE,
    '#maxlength' => 64,
    '#id' => 'campaign-blacklist-filter-text',
    '#attributes' => array(
      'autocomplete' => 'off',
    ),
  );
  $form['entry']['reason'] = array(
    '#type' => 'value',
    '#value' => $type,
  );
  $form['entry']['actions'] = array(
    '#type' => 'actions',
    '#tree' => FALSE,
  );
  $form['entry']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  return $form;
}

/**
 * Form submit handler to save a text string to the campaign blacklist.
 */
function campaign_admin_blacklist_form_submit($form, &$form_state) {
  $data = array(
    'text' => $form_state['values']['entry']['text'],
    'context' => $form_state['values']['entry']['context'],
    'match' => $form_state['values']['entry']['match'],
    'reason' => $form_state['values']['entry']['reason'],
  );
  $result = campaign('campaign.addBlacklistText', $data);

  $args = array(
    '@text' => $data['text'],
    '@context' => $data['context'],
    '@match' => $data['match'],
    '@reason' => $data['reason'],
  );
  if ($result === TRUE) {
    drupal_set_message(t('The entry was added to the blacklist.'));
    _campaign_watchdog(array(
      'Added @text (@context, @match) to @reason blacklist.' => $args,
      'Data:<pre>@data</pre>' => array('@data' => $data),
      'Result:<pre>@result</pre>' => array('@result' => $result),
    ));
  }
  else {
    drupal_set_message(t('An error occurred upon trying to add the text to the blacklist.'), 'error');
    _campaign_watchdog(array(
      'Failed to add @text (@context, @match) to @reason blacklist.' => $args,
      'Data:<pre>@data</pre>' => array('@data' => $data),
      'Result:<pre>@result</pre>' => array('@result' => $result),
    ), WATCHDOG_ERROR);
  }
}

/**
 * Formats the blacklist form as table to embed the form.
 */
function theme_campaign_admin_blacklist_form($variables) {
  $form = $variables['form'];
  $header = array(
    t('Context'),
    t('Matches'),
    t('Text'),
    '',
  );
  $rows = array();

  $rows[] = array(
    drupal_render($form['entry']['context']),
    drupal_render($form['entry']['match']),
    drupal_render($form['entry']['text']),
    drupal_render($form['entry']['actions']),
  );

  foreach (element_children($form['blacklist']) as $id) {
    $rows[] = array(
      array(
        'data' => drupal_render($form['blacklist'][$id]['context']),
        'class' => $form['blacklist'][$id]['context']['#class'],
      ),
      array(
        'data' => drupal_render($form['blacklist'][$id]['match']),
        'class' => $form['blacklist'][$id]['match']['#class'],
      ),
      array(
        'data' => drupal_render($form['blacklist'][$id]['text']),
        'class' => $form['blacklist'][$id]['text']['#class'],
      ),
      drupal_render($form['blacklist'][$id]['actions']),
    );
  }

  // This table is never empty due to the form.
  $output  = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'campaign-blacklist')));
  $output .= drupal_render_children($form);

  drupal_add_js(drupal_get_path('module', 'campaign') . '/campaign.admin.js');

  return $output;
}

/**
 * Form builder; Builds the confirmation form for deleting a blacklist item.
 *
 * @param $key
 *   The blacklist entry text to remove, base64-encoded.
 * @param $context
 *   The context of the blacklist entry.
 * @param $reason
 *   The reason of the blacklist entry.
 *
 * @ingroup forms
 * @see campaign_admin_blacklist_delete_submit()
 */
function campaign_admin_blacklist_delete($form, &$form_state) {
  $form['text'] = array(
    '#type' => 'value',
    '#value' => $_GET['text'],
  );
  $form['context'] = array(
    '#type' => 'value',
    '#value' => $_GET['context'],
  );
  $form['reason'] = array(
    '#type' => 'value',
    '#value' => $_GET['reason'],
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete %text from the blacklist?', array('%text' => $_GET['text'])),
    'admin/config/content/campaign/blacklist',
    t('This action cannot be undone.'),
    t('Delete'), t('Cancel')
  );
}

/**
 * Form submit handler to delete an entry from the blacklist.
 */
function campaign_admin_blacklist_delete_submit($form, &$form_state) {
  $data = array(
    'text' => $form_state['values']['text'],
    'context' => $form_state['values']['context'],
    'reason' => $form_state['values']['reason'],
  );
  $result = campaign('campaign.removeBlacklistText', $data);

  $args = array(
    '@text' => $data['text'],
    '@context' => $data['context'],
    '@reason' => $data['reason'],
  );
  if ($result === TRUE) {
    drupal_set_message(t('The entry was removed from the blacklist.'));
    _campaign_watchdog(array(
      'Removed @text (@context) from @reason blacklist.' => $args,
      'Data:<pre>@data</pre>' => array('@data' => $data),
      'Result:<pre>@result</pre>' => array('@result' => $result),
    ));
  }
  else {
    drupal_set_message(t('An error occurred upon trying to remove the item from the blacklist.'), 'error');
    _campaign_watchdog(array(
      'Failed to removed @text (%context) from @reason blacklist.' => $args,
      'Data:<pre>@data</pre>' => array('@data' => $data),
      'Result:<pre>@result</pre>' => array('@result' => $result),
    ), WATCHDOG_ERROR);
  }

  $form_state['redirect'] = 'admin/config/content/campaign/blacklist';
}

/**
 * Form builder; Global campaign settings form.
 *
 * This form does not validate campaign API keys, since the fallback method still
 * needs to be able to be reconfigured in case campaign services are down.
 * campaign.verifyKey would invalidate the keys and throw an error; hence,
 * _campaign_fallback() would invoke form_set_error(), effectively preventing this
 * form from submitting.
 */
function campaign_admin_settings($form, &$form_state) {
  // Output a positive status message, since users keep on asking whether
  // campaign should work or not. Re-check on every regular visit of this form to
  // verify the module's configuration.
  if (empty($form_state['input'])) {
    $status = _campaign_status();
    // If there is any configuration error, then campaign_init() will have output
    // it already.
    if ($status === TRUE) {
      drupal_set_message(t('campaign servers verified your keys. The services are operating correctly.'));
    }
  }

  $form['access-keys'] = array(
    '#type' => 'fieldset',
    '#title' => t('Access keys'),
    '#description' => t('To use campaign, you need a public and private key. To obtain your keys, <a href="@campaign-login-url">register and login on campaign.com</a>, and <a href="@campaign-manager-add-url">create a subscription</a> for your site. Once you created a subscription, copy your private and public access keys from the <a href="@campaign-manager-url">site manager</a> into the form fields below, and you are ready to go.', array(
      '@campaign-login-url' => 'http://campaign.com/user',
      '@campaign-manager-add-url' => 'http://campaign.com/site-manager/add',
      '@campaign-manager-url' => 'http://campaign.com/site-manager',
    )),
    '#collapsible' => TRUE,
    // Only show key configuration fields if they are not configured or invalid.
    '#collapsed' => !isset($status) ? FALSE : $status === TRUE,
  );
  // Keys are not #required to allow to install this module and configure it
  // later.
  $form['access-keys']['campaign_public_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Public key'),
    '#default_value' => variable_get('campaign_public_key', ''),
    '#description' => t('Used to uniquely identify you.'),
  );
  $form['access-keys']['campaign_private_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Private key'),
    '#default_value' => variable_get('campaign_private_key', ''),
    '#description' => t('Used to prevent someone else from hijacking your requests. Similar to a password, it should never be shared with anyone.'),
  );

  $form['campaign_fallback'] = array(
    '#type' => 'radios',
    '#title' => t('When campaign is down or unreachable'),
    // Default to treating everything as inappropriate.
    '#default_value' => variable_get('campaign_fallback', campaign_FALLBACK_BLOCK),
    '#options' => array(
      campaign_FALLBACK_BLOCK => t('Block all form submissions'),
      campaign_FALLBACK_ACCEPT => t('Accept all form submissions'),
    ),
    '#description' => t('In case the campaign services are unreachable, no text analysis can be performed and no CAPTCHAs can be generated. Subscribers to <a href="@pricing-url">campaign Plus</a> receive access to <a href="@sla-url">campaign\'s high-availability backend infrastructure</a>, not available to free users, reducing potential downtime.', array(
      '@pricing-url' => 'http://campaign.com/pricing',
      '@sla-url' => 'http://campaign.com/standard-service-level-agreement',
    )),
  );

  $form['campaign_privacy_link'] = array(
    '#type' => 'checkbox',
    '#title' => t("Link to campaign's privacy policy on forms protected by textual analysis"),
    '#return_value' => 1,
    '#default_value' => variable_get('campaign_privacy_link', 1),
    '#description' => t('Displays a link to the recommended <a href="@privacy-policy-url">privacy policy on campaign.com</a> on all forms that are protected via <a href="@help-url">textual analysis</a>. When disabling this option, you are required to inform visitors about data privacy through other means, as stated in the <a href="@terms-of-service-url">terms of service</a>.', array(
      '@privacy-policy-url' => 'http://campaign.com/web-service-privacy-policy',
      '@help-url' => url('admin/help/campaign'),
      '@terms-of-service-url' => 'http://campaign.com/terms-of-service',
    )),
  );

  $form['campaign_testing_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable testing mode'),
    '#default_value' => variable_get('campaign_testing_mode', 0),
    '#description' => t('Submitting "ham", "unsure", or "spam" on a protected form will trigger the corresponding behavior, and similarly, word verifications will only respond to "correct" and "incorrect", instead of the actual characters asked for. This option should be disabled in production environments.'),
  );

  return system_settings_form($form);
}

/**
 * Menu callback; Displays the administrative reports page.
 */
function campaign_reports_page($form, &$form_state) {
  $embed_attributes = array(
    'src' => 'http://campaign.com/statistics.swf?key=' . check_plain(variable_get('campaign_public_key', '')),
    'quality' => 'high',
    'width' => '100%',
    'height' => '430',
    'name' => 'campaign',
    'align' => 'middle',
    'play' => 'true',
    'loop' => 'false',
    'allowScriptAccess' => 'sameDomain',
    'type' => 'application/x-shockwave-flash',
    'pluginspage' => 'http://www.adobe.com/go/getflashplayer',
    'wmode' => 'transparent',
  );
  $form['chart'] = array(
    '#type' => 'item',
    '#title' => t('Statistics'),
    '#markup' => '<embed' . drupal_attributes($embed_attributes) . '></embed>',
  );

  return $form;
}

