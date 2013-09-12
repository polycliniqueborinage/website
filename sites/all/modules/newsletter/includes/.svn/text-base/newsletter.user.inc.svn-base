<?php

/**
 * @file
 * Functions and forms for the frontend part of newsletter module.
 */

/**
 * Access callback for user newsletter subscriptions editing.
 */
function newsletter_edit_subsdcriber_access($account) {
  return (($GLOBALS['user']->uid == $account->uid && user_access('subscribe newsletters')) || user_access('administer users')) && $account->uid > 0;
}

/**
 * @return
 *   An array with lists and terms the subscriber has signed up for.
 */
function newsletter_subscribed_lists($subscriber) {
    $lists = db_query('SELECT t.exposed, s.list_id, s.tid_subscribed
      FROM {newsletter_index_subscribers} s
      JOIN {newsletter_lists} l ON s.list_id = l.id
      JOIN {newsletter_templates} t ON t.id = l.template_id
      WHERE subscriber_id = :sid',
      array(
        ':sid' => $subscriber->id,
      ))->fetchAll();
  $subscribed = array('exposed' => array(), 'not_exposed' => array());
  foreach ($lists as $list) {
    if ($list->exposed) {
      $subscribed['exposed'][$list->list_id]= array(
        'value' => 1,
        'terms' => unserialize($list->tid_subscribed),
      );
    }
    else {
      $subscribed['not_exposed'][$list->list_id] = 1;
    }

  }
  return $subscribed;
}

/**
 * Saves/Updates a subscriber's subscribed terms.
 *
 *  @param $lists
 *    An array containing exposed and not exposed lists and the terms
 *    of the exposed ones.
 *  @param $subscriber_id
 *    The subscriber's id.
 */
function newsletter_save_subscriber_terms($lists, $subscriber_id) {
  // delete everything associated to this subscribers index.
  db_delete('newsletter_index_subscribers')
    ->condition('subscriber_id', $subscriber_id)
    ->execute();
  foreach ($lists['not_exposed'] as $lid => $list) {
    if ($list) {
      $inserted = db_insert('newsletter_index_subscribers')
                  ->fields(array(
                    'subscriber_id' => $subscriber_id,
                    'list_id' => $lid,
                  ))
                  ->execute();
    }
  }

  foreach ($lists['exposed'] as $lid => $exposed_list) {
    $is_exposed = FALSE;
    $terms = array();
    foreach ($exposed_list['terms'] as $tid => $value) {
      $terms[] = $tid;
      $is_exposed = (isset($value) && $value) ? TRUE : $is_exposed;
    }
    if ($is_exposed) {
      db_insert('newsletter_index_subscribers')
        ->fields(array(
          'subscriber_id' => $subscriber_id,
          'list_id' => $lid,
        ))
        ->execute();
    }
    if ($is_exposed) {
      $terms=@serialize($terms);
      db_update('newsletter_index_subscribers')
        ->fields(array(
          'tid_subscribed' => $terms,
        ))
        ->condition('list_id', $lid)
        ->condition('subscriber_id', $subscriber_id)
        ->execute();
    }
  }
}

/*
 * Get exposed terms for specific subscriber list.
 *
 * @param $lid
 *   The list id, terms are requested for.
 *
 * @return
 *   An array with terms of the requested list.
 */
function newsletter_exposed_terms($lid) {
  $terms = db_query('SELECT tax.tid AS term_id, term.name AS term_name
    FROM {newsletter_lists} list
    INNER JOIN {newsletter_templates} template ON template.id = list.template_id
    INNER JOIN {newsletter_index} tax ON tax.template_id = template.id
    INNER JOIN {taxonomy_term_data} term ON term.tid = tax.tid
    WHERE list.id = :lid', array(':lid' => $lid))
    ->fetchAllKeyed();
  return isset($terms) ? $terms : array();
}

/**
 * @return
 *   An array with terms of an exposed list that a user subscribed.
 */
function newsletter_subscribed_exposed_terms($list_id, $sid) {
    $subscribed = array();
    $terms = db_query('SELECT tid_subscribed
      FROM {newsletter_index_subscribers}
      WHERE list_id = :lid
      AND subscriber_id = :sid',
      array(
        ':lid' => $list_id,
        ':sid' => $sid,
      ))->fetchAll();
    foreach ($terms as $term) {
      if (!is_null($term->tid_subscribed)) {
        $tids = @unserialize($term->tid_subscribed);
        foreach ($tids as $tid) {
          $subscribed[$tid] = 1;
        }
      }
    }
    return $subscribed;

}

/**
 * Loads up the form that is displayed to the newsletter block.
 */
function newsletter_subscribe_form($form, &$form_state) {
  global $user;
  $ajax = array(
            'callback' => 'newsletter_subscribe_ajax',
            'wrapper' => 'error',
            'method' => 'append',
            'effect' => 'fade',
            'progress' => array(
              'type' => 'throbber',
              'message' => NULL,
            ),
            'event' => 'click',
          );
  if (!isset($user->mail) || variable_get('newsletter_show_email_in_block', FALSE)) {
    $form['email'] = array(
      '#type' => 'textfield',
      '#default_value' => 'user@example.com',
      '#size' => 17,
      '#required' => TRUE,
      '#prefix' => '<div id="error"></div>',
    );
  }
  else {
    $form['logged-in'] = array(
      '#type' => 'hidden',
      '#value' => TRUE,
    );
    $form['logged-in']['msg'] = array(
      '#type' => 'markup',
      '#markup' => t('Hello @user.Please consider joining our newsletter lists!', array('@user' => $user->name)),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Subscribe'),
    '#prefix' => '<div id="subscribe" class="button green">',
    '#suffix' => '</div>',
    '#ajax' => $ajax,
  );
  return $form;
}

/**
 * Validates e-mail and sends user to next step which is
 * a colorbox if the module exists.Else simple http redirect.
 */
function newsletter_subscribe_ajax($form, &$form_state) {
  global $base_url;
  if (isset($form_state['values']['logged-in'])) {
    global $user;
    $email = $user->mail;
  }
  elseif (!isset($form_state['values']['logged-in'])) {
    $email = ($form_state['input']['email']!='user@example.com')
    ? $form_state['input']['email']
    : '';
  }

  if (valid_email_address($email) && !newsletter_is_subscribed($email)) {

    return module_exists('colorbox') ?
      "<script type='text/javascript'>
        (function ($) {
          $.fn.colorbox({
            href: '$base_url/colorbox/form/newsletter_subscribe_advanced?destination=&email=$email',
            width: 600,
            height: 650,
            open: true,
            onComplete: function(){Drupal.behaviors.newsletter.subscribeForm();},
          })
        })(jQuery);
      </script>" :
      "<script type='text/javascript'>
        location.href='$base_url/newsletter/subscribe?destination=&email=$email'
      </script>";
  }
  else {
    return t("This e-mail doesn't exist or you have already subscribed");
  }
}

/**
 * The second form step of a newsletter subscription.
 * @TODO get terms of exposed lists.
 */
function newsletter_subscribe_advanced($form, &$form_state) {
  global $user;
  if (arg(0) !== 'user' && !is_numeric(arg(1))) {
    $account = $user;
  }
  else {
    $account = user_load(arg(1));
  }
  $lists = newsletter_get_subscribe_form_lists();

  if (!module_exists('colorbox')) {
    drupal_add_js('(function ($) {$(document).ready(function () {
      Drupal.behaviors.newsletter.subscribeForm();
    });})(jQuery);', 'inline');
  }
  require_once DRUPAL_ROOT . '/includes/locale.inc';

  $countries = country_get_list();
  $show_email = variable_get('newsletter_show_email', TRUE);

  $form['uid'] = array(
    '#type' => 'hidden',
    '#value' => isset($account->uid) ? $account->uid : NULL,
    '#required' => TRUE,
    '#size' => 20,
  );
  if ($show_email) {
    $form['mail'] = array(
      '#attributes' => array('class' => array('container-inline')),
      '#type' => 'fieldset',
      '#title' => t('E-mail'),
    );
  }
  $form['mail']['email'] = array(
    '#type' => $show_email ? 'textfield' : 'hidden',
    '#value' => isset($_GET['email']) ? check_plain($_GET['email']) : @$account->mail,
    '#required' => TRUE,
    '#size' => 20,
  );
  $form['lists'] = array(
    '#type' => 'fieldset',
    '#title' => t('Newsletter Lists'),
  );
  if (!empty($lists['0'])) {
    $form['lists']['not_exposed'] = array(
      '#type' => 'item',
    );

    foreach ($lists['0'] as $id => $title) {
      $form['lists']['not_exposed'][$id] = array(
        '#type' => 'checkbox',
        '#title' => check_plain($title),
      );
    }
  }
  if (!empty($lists['1'])) {

    $form['lists']['exposed'] = array(
      '#type' => 'item',
    );
    foreach ($lists['1'] as $id => $title) {
      $terms = newsletter_exposed_terms($id);

      $form['lists']['exposed'][$id] = array(
        '#type' => 'checkbox',
        '#title' => $title,
      );
      $form['lists']['exposed'][$id . '-terms'] = array(
        '#attributes' => array('class' => array('exposed')),
        '#type' => 'checkboxes',
        '#options' => $terms,
        '#title' => $title . ' ' . t('Configuration'),
        '#description' => t('Select the categories you want to receive newsletters for.Leave empty to subscribe to all.'),
      );
    }
  }
  $personal_info_form = variable_get('newsletter_personal_info_form');
  $is_empty = array_filter(array_map('array_filter', $personal_info_form));
  if (!empty($is_empty)) {
    $form['info'] = array(
      '#type' => 'fieldset',
      '#title' => t('Personal Info'),
    );
  }

  foreach ($personal_info_form as $info_type => $values) {
    if ($values['show']) {
      switch ($info_type) {
        case 'firstname':
          $form['info']['firstname'] = array(
            '#type' => 'textfield',
            '#title' => t('First Name'),
            '#size' => 40,
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
        case 'lastname':
          $form['info']['lastname'] = array(
            '#type' => 'textfield',
            '#title' => t('Last Name'),
            '#size' => 40,
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
        case 'age_group':
          $form['info']['age_group'] = array(
            '#type' => 'select',
            '#title' => t('Age Group'),
            '#options' => array(
              '<18' => '<18',
              '18-24' => '18-24',
              '25-31' => '25-31',
              '32-45' => '32-45',
              '>46' => '>46',
            ),
            '#empty_option' => t('- Select -'),
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
        case 'gender':
          $form['info']['gender'] = array(
            '#type' => 'select',
            '#title' => t('Gender'),
            '#options' => array(
              'male' => t('Male'),
              'female' => t('Female'),
              'other' => t('Other'),
            ),
            '#empty_option' =>  t('- Select -'),
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
        case 'job':
          $form['info']['job'] = array(
            '#type' => 'textfield',
            '#title' => t('Job'),
            '#size' => 40,
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
        case 'country':
          $form['info']['country'] = array(
            '#type' => 'select',
            '#title' => t('Country'),
            '#options' => $countries,
            '#empty_option' =>  t('- Select -'),
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
        case 'city':
          $form['info']['city'] = array(
            '#type' => 'textfield',
            '#title' => t('City'),
            '#size' => 40,
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
        case 'language':
          $form['info']['language'] = array(
            '#type' => 'select',
            '#title' => t('Language'),
            '#options' => newsletter_language_list(),
            '#empty_option' =>  t('- Select -'),
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
        case 'receive_format':
          $form['info']['receive_format'] = array(
            '#type' => 'select',
            '#title' => t('Preferred Format'),
            '#options' => array(
              'html' => t('HTML'),
              'plain' => t('Plain Text'),
            ),
            '#empty_option' =>  t('- Select -'),
            '#required' => $values['require'] ? TRUE : FALSE,
          );
          break;
      }
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Subscribe'),
  );
  return $form;
}

/**
 * Updates subscribers info and potential exposed terms to database.
 * Final subscription step.
 */
function newsletter_subscribe_advanced_submit($form, &$form_state) {

  $needs_confirm = variable_get('newsletter_send_confirm', FALSE);

  $subscriber = new stdClass();
  $subscriber->ip = ip_address();
  $subscriber->email = !empty($form_state['input']['email']) ? $form_state['input']['email'] : $form_state['values']['email'];
  $subscriber->uid = !empty($form_state['input']['uid']) ? $form_state['input']['uid'] : NULL;
  $subscriber->confirmed = $needs_confirm ? 0 : 1;
  $subscriber->age_group = !empty($form_state['input']['age_group']) ? $form_state['input']['age_group'] : NULL;
  $subscriber->firstname = !empty($form_state['input']['firstname']) ? $form_state['input']['firstname'] : NULL;
  $subscriber->lastname = !empty($form_state['input']['lastname']) ? $form_state['input']['lastname'] : NULL;
  $subscriber->job = !empty($form_state['input']['job']) ? $form_state['input']['job'] : NULL;
  $subscriber->gender = !empty($form_state['input']['gender']) ? $form_state['input']['gender'] : NULL;
  $subscriber->country = !empty($form_state['input']['country']) ? $form_state['input']['country'] : NULL;
  $subscriber->city = !empty($form_state['input']['city']) ? $form_state['input']['city'] : NULL;
  $subscriber->receive_format = !empty($form_state['input']['receive_format']) ? $form_state['input']['receive_format'] : NULL;
  $subscriber->language = !empty($form_state['input']['language']) ? $form_state['input']['language'] : NULL;

  newsletter_save_new_subscriber($subscriber, $needs_confirm);
  if (!isset($subscriber->id)) {
    return;
  }
  $lists = array('exposed' => array(), 'not_exposed' => array());
  foreach ($form_state['input'] as $key => $input) {
    if (array_key_exists($key . '-terms', $form_state['input'])) {
      $lists['exposed'][$key]['value'] = $input;
    }
    elseif (strpos($key, '-terms')) {
      $key = (int) str_replace('-terms', '', $key);
      $lists['exposed'][$key]['terms'] = $input;
    }
    elseif (is_numeric($key)) {
      $lists['not_exposed'][$key] = $input;
    }
  }
  newsletter_save_subscriber_terms($lists, $subscriber->id);
  return $subscriber->id
    ? drupal_set_message(t('Thank you for your subscription'), 'status')
    : drupal_set_message(t('There was an error establishing a database connection.Please try again, or contact us if the problem persists.Thank you'), 'error');
}

/**
 * Confirms a subscription to a newsletter list.
 */
function newsletter_confirm() {
  $hash = $_GET['hash'];
  $email = db_query('SELECT email
    FROM {newsletter_subscribers}
    WHERE hash = :hash',
    array(':hash' => $hash))
    ->fetchField();
  if ($email) {
    db_update('newsletter_subscribers')
    ->fields(array(
      'confirmed' => 1,
      'confirmation_timestamp' => REQUEST_TIME,
    ))
    ->condition('hash', $hash)
    ->execute();
    $needs_welcome = variable_get('newsletter_send_welcome', FALSE);
    if ($needs_welcome) {
      $welcome_email = newsletter_create()->sendBasic(2, $email);
    }
  }
  return $email
    ? t('You have successfully confirmed your subscription!')
    : t('The hash you provided is invalid or outdated');
}

/**
 * Unsubscribes a subscriber from newsletter.
 */
function newsletter_unsubscribe() {
  $hash = $_GET['hash'];
  $email = db_query('SELECT email
    FROM {newsletter_subscribers}
    WHERE hash = :hash',
    array(':hash' => $hash))
    ->fetchField();
  if ($email) {
    $needs_unsubscribe = variable_get('newsletter_send_unsubscribe', FALSE);

    if ($needs_unsubscribe) {
      $unsubscribe_email = newsletter_create()->sendBasic(3, $email);
    }

    db_delete('newsletter_subscribers')
      ->condition('hash', $hash)
      ->execute();
  }
  return $email
    ? t('You have successfully unsubscribed from our newsletter')
    : t('The hash you provided is invalid or outdated');
}

/*
 * Updates the opened newsletters.
 */
function newsletter_statistics_open() {
  $subscriber = db_query('SELECT *
    FROM {newsletter_subscribers}
    WHERE hash = :hash',
    array(':hash' => $_GET['hash']))
    ->fetchField();

  if (isset($subscriber)) {
    db_query('UPDATE {newsletter_newsletters}
      SET opens = opens + 1
      WHERE id = :id',
      array(':id' => $_GET['source']));
  }
}

/*
 * Menu callback; Lets users manage their subscriptions.
 */
function newsletter_manage_subscriptions_form($form, &$form_state, $user) {
  $form = array();
  drupal_add_js(drupal_get_path('module', 'newsletter') . '/js/newsletter.js');
  $lists = newsletter_get_subscribe_form_lists();
  drupal_add_js(array( 'exposed' => $lists['1']), 'setting');
  $subscriber = db_query('SELECT * FROM {newsletter_subscribers} WHERE uid = :uid', array(':uid' => $user->uid))->fetchObject();
  if (!$subscriber) {
    drupal_set_message(t('You have not subscribed to any of our newsletters yet'), 'warning');
    return drupal_get_form('newsletter_subscribe_advanced');
  }
  $subscribed = newsletter_subscribed_lists($subscriber);

  drupal_add_js('(function ($) {$(document).ready(function () {
    Drupal.behaviors.newsletter.subscribeForm();
  });})(jQuery);', 'inline');

  $form['subscriber_id'] = array(
    '#type' => 'hidden',
    '#value' => $subscriber->id,
  );
  $form['lists'] = array(
    '#type' => 'fieldset',
    '#title' => t('Newsletter Lists'),
  );
  if (!empty($lists['0'])) {
    $form['lists']['not_exposed'] = array(
      '#type' => 'item',
    );

    foreach ($lists['0'] as $id => $title) {
      $form['lists']['not_exposed'][$id] = array(
        '#type' => 'checkbox',
        '#title' => $title,
        '#value' => isset($subscribed['not_exposed'][$id]),
      );
    }
  }
  if (!empty($lists['1'])) {

    $form['lists']['exposed'] = array(
      '#type' => 'item',
    );
    foreach ($lists['1'] as $id => $title) {
      $terms = newsletter_exposed_terms($id);

      $form['lists']['exposed'][$id] = array(
        '#type' => 'checkbox',
        '#title' => $title,
        '#value' => isset($subscribed['exposed'][$id]['value']),
      );
      $form['lists']['exposed'][$id . '-terms'] = array(
        '#attributes' => array('class' => array('exposed')),
        '#type' => 'checkboxes',
        '#options' => $terms,
        '#default_value' => isset($subscribed['exposed'][$id]['terms']) ? $subscribed['exposed'][$id]['terms'] : array(),
        '#title' => $title . ' ' . t('Configuration'),
        '#description' => t('Select the categories you want to receive newsletters for.Leave empty to subscribe to all.'),
      );
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update your subscriptions'),
  );

  return $form;
}

function newsletter_manage_subscriptions_form_submit($form, &$form_state) {
$lists = array('exposed' => array(), 'not_exposed' => array());
  foreach ($form_state['input'] as $key => $input) {
    if (array_key_exists($key . '-terms', $form_state['input'])) {
      $lists['exposed'][$key]['value'] = $input;
    }
    elseif (strpos($key, '-terms')) {
      $key = (int) str_replace('-terms', '', $key);
      $lists['exposed'][$key]['terms'] = $input;
    }
    elseif (is_numeric($key)) {
      $lists['not_exposed'][$key] = $input;
    }
  }
  $id = $form_state['values']['subscriber_id'];
  newsletter_save_subscriber_terms($lists, $id);
}
