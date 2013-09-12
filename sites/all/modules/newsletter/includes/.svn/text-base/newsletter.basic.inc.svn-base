<?php

/**
 * @file
 *   Contains NewsletterBasic class and
 *   newsletter's module implementation of drupal mail system.
 */

/**
 * Basic Newsletter class.
 */
class NewsletterBasic {

  protected $from;
  protected $format;
  protected $language;

  public function __construct() {
    $this->from = variable_get('newsletter_from');
    $this->format = variable_get('newsletter_format', 'html');
    $this->language = language_default();
  }

  /**
   * Sends a test newsletter to an email-address.
   * Called from administration Settings tab.
   *
   * @param $to
   *   The e-mail to send the test newsletter to.
   */
  public function sendTest($to) {
    $message = drupal_mail('newsletter', 'test' , $to , $this->language , array('format' => $this->format) , $this->from);
    return $message['result'];
  }

  /**
   * Sends email to a subscriber using a basic template (confirmation/welcome/unsubscribe emails).
   *
   * @param $id
   *   The the basic template id of this email.
   * @param $to
   *   The e-mail to send the newsletter confirmation to.
   */
  public function sendBasic($id, $to) {
    $template = db_query('SELECT subject, body, format
      FROM {newsletter_templates}
      WHERE id = :id',
      array(':id' => $id))
      ->fetchObject();
    $subscriber = db_query('SELECT *
      FROM {newsletter_subscribers}
      WHERE email = :email',
      array(':email' => $to))
      ->fetchObject();
    $params = array(
      'template' => $template,
      'subscriber' => $subscriber,
      'format' => $this->format,
    );
    $language = isset($subscriber->language) ? newsletter_language_list($subscriber->language) : $this->language;
    $message = drupal_mail('newsletter', 'basic', $to, $language , $params , $this->from);
    return $message['result'];
  }
}

/**
 * Modify the drupal mail system to send HTML emails.
 */
class NewsletterMailSystem implements MailSystemInterface {
  public function format(array $message) {
    // Format the body according to text format
    // or strip all the tags if email format is plain text
    if (!empty($message['body'])) {
      $message['body'] = ($message['format'] == 'html')
        ? check_markup($message['body'], $message['body_format'])
        : strip_tags($message['body']);
    }
    if (!empty($message['subject'])) {
      $message['subject'] = ($message['format'] == 'html') ? check_plain($message['subject']) : strip_tags($message['subject']);
    }
    return $message;
  }
  public function mail(array $message) {
    if (isset($message['headers']['Return-Path']) && !ini_get('safe_mode')) {
      $return_path_set = strpos(ini_get('sendmail_path'), ' -f');
      if (!$return_path_set) {
        $message['Return-Path'] = $message['headers']['Return-Path'];
        unset($message['headers']['Return-Path']);
      }
    }
    if ($message['format']==='html') {
      $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    }
    $mimeheaders = array();
    foreach ($message['headers'] as $name => $value) {
      $mimeheaders[] = $name . ': ' . mime_header_encode($value);
    }
    $line_endings = variable_get('mail_line_endings', MAIL_LINE_ENDINGS);
    $mail_subject = mime_header_encode($message['subject']);
    $mail_body = $message['body'];
    $mail_headers = join("\n", $mimeheaders);
    if (isset($message['Return-Path']) && !ini_get('safe_mode')) {
      $mail_result = mail($message['to'], $mail_subject, $mail_body, $mail_headers, '-f ' . $message['Return-Path'] );
    }
    else {
      $mail_result = mail($message['to'], $mail_subject, $mail_body, $mail_headers );
    }
    // Integration with maillog
    if (module_exists('maillog') && variable_get('maillog_log', TRUE)) {
      $record = new stdClass;

      $record->header_message_id = isset($message['headers']['Message-ID']) ? $message['headers']['Message-ID'] : NULL;
      $record->subject = $message['subject'];
      $record->body = $message['body'];
      $record->header_from = isset($message['from']) ? $message['from'] : NULL;
      $record->header_to = isset($message['to']) ? $message['to'] : NULL;
      $record->header_reply_to = isset($message['headers']['Reply-To']) ? $message['headers']['Reply-To'] : '';
      $record->header_all = serialize($message['headers']);
      $record->sent_date = time();

      drupal_write_record('maillog', $record);
    }
    return $mail_result;
  }
}
