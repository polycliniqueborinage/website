<?php

/**
 * @file
 * Administration functions for language_cookie.module.
 */

/**
 * The URL language provider configuration form.
 */
function language_cookie_admin_form($form, &$form_state) {
  $form['language_cookie_param'] = array(
    '#title' => t('Cookie parameter'),
    '#type' => 'textfield',
    '#default_value' => variable_get('language_cookie_param', 'language'),
    '#description' => t('Name of the cookie parameter used to determine the desired language.'),
  );

  $form['language_cookie_time'] = array(
    '#title' => t('Cookie duration'),
    '#type' => 'textfield',
    '#default_value' => variable_get('language_cookie_time', 31536000),
    '#description' => t('The time the cookie expires. This is the number of seconds from the current time.'),
  );

  $form_state['redirect'] = 'admin/config/regional/language/configure';
  return system_settings_form($form);
}