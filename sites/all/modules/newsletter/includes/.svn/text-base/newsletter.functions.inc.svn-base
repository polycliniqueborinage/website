<?php

/**
 * @file
 * Several helper vital functions for newsletter module.
 */


/**
 * Loads up the requested list.
 */
function newsletter_lid_load($lid) {
  return db_query("SELECT * FROM {newsletter_lists} WHERE id = :id",
    array(':id' => $lid))
    ->fetchObject();
}

/**
 * Loads up the requested subscriber.
 */
function newsletter_sid_load($sid) {
  return db_query("SELECT * FROM {newsletter_subscribers} WHERE id = :id",
    array(':id' => $sid))
    ->fetchObject();
}

/**
 * Loads up the requested template.
 */
function newsletter_tid_load($tid) {
  return db_query("SELECT * FROM {newsletter_templates} WHERE id = :id",
    array(':id' => $tid))
    ->fetchObject();
}

/**
 * Loads up the requested template.
 */
function newsletter_nid_load($nid) {
  return db_query("SELECT * FROM {newsletter_newsletters} WHERE id = :id",
    array(':id' => $nid))
    ->fetchObject();
}

/**
 * @return
 *  Languages as an associative array of langcode and language name..
 */
function newsletter_language_list($langcode=NULL) {
  $languages = language_list();
  foreach ($languages as $code => $language) {
    $langs[$code] = $language->name;
  }
  return isset($langcode) ? $langs[$langcode] : $langs;
}

/**
 * Checks whether an email is registered to newsletter list or not.
 * @return
 *  boolean TRUE or FALSE.
 */
function newsletter_is_subscribed($email = NULL) {
  if (!isset($email)) {
    global $user;
    $email = isset($user->mail) ? $user->mail : FALSE;
  }
  if ($email) {
    $is_subscribed = db_query('SELECT email FROM {newsletter_subscribers} WHERE email = :email', array(':email' => $email))->fetchField();
    return $is_subscribed ? TRUE : FALSE;
  }
  else {
    return FALSE;
  }
}

/**
 * Instantiates a new Newsletter class according to http://drupal.org/node/608152.
 *
 * @return
 *   A newsletter object.
 */
function newsletter_create($lid=NULL, $template_id=NULL, $newsletter_id=NULL) {
  if (isset($template_id)) {
    $newsletter = new NewsletterMailCustom($lid, $template_id, $newsletter_id);
  }
  elseif (isset($lid) && !isset($template_id)) {
    $newsletter = new NewsletterMail($lid);
  }
  else {
    $newsletter = new NewsletterBasic();
  }
  return (object) $newsletter;
}

/**
 * Saves a new newsletter subscriber in database.
 *
 * @param $subscriber
 *   An object conatining the following:
 *    email - Required.
 *    uid - Optional.
 *    firstname - Optional.
 *    lastname - Optional.
 *    age_group - Optional.
 *    job - Optional.
 *    country - Optional.
 *    gender - Optional.
 *    ip - Optional.
 *    confirmed - Required.
 * @param $needs_confirm
 *   Whether a email asking for subscription confirmation should be sent.
 * @return
 *   The id of the newly created subscriber will be created since it the object
 *   is passed by reference.boolean FALSE if the insertion fails.
 */
function newsletter_save_new_subscriber(&$subscriber, $needs_confirm = FALSE) {
  if (!valid_email_address($subscriber->email) || newsletter_is_subscribed($subscriber->email))  {
    //return drupal_set_message(t('%email is not a valid email.Please provide a valid email', array('%email' => $subscriber->email)), 'warning');
  }
  $subscriber->id = db_insert('newsletter_subscribers')
    ->fields(array(
      'email' => $subscriber->email,
      'uid' => isset($subscriber->uid) ? $subscriber->uid : NULL,
      'firstname' => isset($subscriber->firstname) ? $subscriber->firstname : NULL,
      'lastname' => isset($subscriber->lastname) ? $subscriber->lastname : NULL,
      'age_group' => isset($subscriber->age_group) ? $subscriber->age_group : NULL,
      'job' => isset($subscriber->job) ? $subscriber->job : NULL,
      'city' => isset($subscriber->city) ? $subscriber->city : NULL,
      'country' => isset($subscriber->country) ? $subscriber->country : NULL,
      'gender' => isset($subscriber->gender) ? $subscriber->gender : NULL,
      'receive_format' => isset($subscriber->receive_format) ? $subscriber->receive_format : NULL,
      'language' => isset($subscriber->language) ? $subscriber->language : NULL,
      'created' => REQUEST_TIME,
      'ip' => isset($subscriber->ip) ? $subscriber->ip : NULL,
      'confirmed' => $subscriber->confirmed,
      'confirmation_timestamp' => $needs_confirm ? 0 : REQUEST_TIME,
      'hash' =>  drupal_hmac_base64(REQUEST_TIME . $subscriber->email, drupal_get_hash_salt() . ip_address()),
    ))
    ->execute();
  if (!$subscriber->id) {
    return FALSE;
  }
  if ($needs_confirm) {
    $sent = newsletter_create()->sendBasic(1, $subscriber->email);
    $sent
      ? drupal_set_message(t("Thank you for your subscription.An e-mail has sent to your address. Please click to the provided link to prove you own this e-mail and confirm subscription. If you can't find our e-mail please check your spam folder."), 'status')
      : drupal_set_message(t('Thank you for your subscription.Your subscription was recorded but the confirmation mail fail to sent probably due to a server failure.'), 'warning');
  }
}

/*
 * Returns an image to be added to newsletter body
 * so we can calculate the open rate.
 *
 * @param $hash
 *   Subscriber's personal hash.
 * @param $newsletter
 *   The newsletter object.
 *
 * @return
 *   An html image string.
 */
function newsletter_add_open_rate_image($hash, $newsletter) {
  $params = array(
    'absolute' => TRUE,
    'query' => array(
      'hash' => $hash,
      'source' => $newsletter->id,
    ),
  );
  $url = url('newsletter/statistics', $params);
  $image = "<img src='$url' width = '1' height = '1' />";
  return $image;
}

/**
 * Sets a watchdog message for the just sent newsletter.
 *
 * @param $id
 *   The newsletter id.
 * @param $title
 *   The subscriber's list title.
 * @param $email
 *   The subscriber's email.
 * @param $result
 *   The result from drupal_mail_system.
 */
function newsletter_set_watchdog($id, $title, $email, $result) {
  watchdog('newsletter', 'Newsletter list with newsletter id @news_id and list name @list_name sent to @sub with status code @status.',
    array(
      '@news_id' => $id,
      '@list_name' => $title,
      '@sub' => $email,
      '@status' => $result,
    ), WATCHDOG_ERROR);
}
