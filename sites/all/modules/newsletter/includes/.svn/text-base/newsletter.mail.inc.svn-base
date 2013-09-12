<?php

/**
 * @file
 *   Contains NewsletterMail and NewsletterCustom that extend NewsletterBasic.
 */

/**
 * Newsletter class that sends automated, non-custom newsletters
 * with dynamic content based on taxonomy terms.
 */
class NewsletterMail extends NewsletterBasic {

  protected $list;
  protected $subscribers;

  protected $template;
  protected $nodes;

  protected $newsletter;

  protected $customSendRate;
  protected $manualSendRate;
  protected $left;


  public function __construct($lid) {
    parent::__construct();

    $this->left = variable_get('newsletter_for_next_cron', array());

    $this->list = db_query('SELECT *
      FROM {newsletter_lists}
      WHERE id = :lid',
      array(':lid' => $lid))
      ->fetchObject();

    $this->subscribers = (array) $this->getSubscribers();

    $this->manualSendRate = (bool) ($this->list->send_rate=='Manual');
    $this->customSendRate = (bool) is_numeric($this->list->send_rate);

    $this->nodes = $this->getNodes();

    $this->template = db_query('SELECT *
      FROM {newsletter_templates}
      WHERE id = :id',
      array(':id' => $this->list->template_id))
      ->fetchObject();

    $this->newsletter = $this->getNewsletterData();

    $this->left[$this->list->id] = array();
  }

  /**
   * Loads up current lists's subscribers.
   */
  protected function getSubscribers() {
    if (!empty($this->left[$this->list->id])) {
      return $this->left[$this->list->id];
    }
    else {
      // Use db_select here instead to avoid
      // selecting created from newsletter_index_subscribers
      $q = db_select('newsletter_subscribers', 'subscribers')->fields('subscribers');
      $q->join('newsletter_index_subscribers', 'sub_index', 'subscribers.id = sub_index.subscriber_id');
      $q->addField('sub_index', 'list_id');
      $q->addField('sub_index', 'tid_subscribed');
      $q->condition('sub_index.list_id', $this->list->id);
      $q->condition('subscribers.confirmed', 1);
      $subscribers = $q->execute()->fetchAll();

      return $subscribers;
    }
  }

  /**
   * Loads up current newsletter, or creates a new one.
   *
   * @return
   *   The newsletter object.
   */
  protected function getNewsletterData() {

    $newsletter_send_id = db_query('SELECT MAX(send_id) as send_id
      FROM {newsletter_newsletters}
      WHERE title = :title', array(':title' => $this->list->title))
      ->fetchField();
    $newsletter_current = db_query('SELECT * FROM {newsletter_newsletters}
      WHERE title = :title AND send_id = :send_id',
      array(
        ':title' => $this->list->title,
        ':send_id' => $newsletter_send_id,
      ))->fetchObject();
    $last_sent = @($newsletter_current->last_sent !== '0' || !isset($newsletter_current->last_sent));
    if (empty($this->left[$this->list->id]) && $last_sent) {
      $newsletter_id = db_insert('newsletter_newsletters')
        ->fields(array(
          'title' => $this->list->title,
          'send_id' => ++$newsletter_send_id,
          'template_id' => $this->template->id,
        ))
        ->execute();
      $newsletter = db_query('SELECT *
        FROM {newsletter_newsletters}
        WHERE id = :nid',
        array(':nid' => $newsletter_id))
        ->fetchObject();
    }
    else {
      $newsletter =  $newsletter_current;
    }

    return $newsletter;
  }

  /**
   * Builds a dynamic query
   * that gets the current nodes to be sent with the current newsletter.
   */
  protected function getQuery() {
    $query = db_select('taxonomy_index', 'tax');
    $query->fields('tax', array('nid'));
    $query->join('newsletter_index', 'news', 'tax.tid = news.tid');
    $query->join('newsletter_templates', 'template', 'news.template_id = template.id');
    $query->join('newsletter_lists', 'list', 'template.id = list.template_id');
    $query->join('node', 'node', 'tax.nid = node.nid');
    $query->addField('node', 'created', 'created');
    $query->condition('list.id', $this->list->id);
    $query->where('(list.last_sent < node.created) OR (list.last_sent IS NULL )');
    if (!$this->manualSendRate && !$this->customSendRate) {
      $query->where('list.send_again = CURDATE() OR list.send_again IS NULL');
    }
    if ($this->customSendRate) {
      $query->range(0, (int)$this->list->send_rate);
    }
    $query->orderBy('created', 'DESC');
    $query->distinct();

    return $query;
  }

  /**
   * Get this newsletter nodes if list is not exposed.
   *
   * @return
   *   Array containing node objects.
   */
  protected function getNodes() {
    $check_exposed = db_query('SELECT *
      FROM {newsletter_index_subscribers}
      WHERE list_id = :lid AND tid_subscribed IS NOT NULL',
      array(':lid' => $this->list->id))
      ->fetchAll();
    // if we get even one subscriber with custom terms
    // or left nodes from previous cron run
    // then no need to continue here
    if (!empty($check_exposed) || isset($this->left[$this->list->id][0]->nodes)) {
      return 'exposed';
    }
    $nodes = $this->getQuery()->execute()->fetchAll();
    foreach ($nodes as $node) {
      $newsletter_nodes[] = node_load($node->nid);
    }
    return isset($newsletter_nodes) ? $newsletter_nodes : array();
  }

  /**
   * Get this newsletter nodes if list is exposed.
   *
   * @param $subscriber
   *   The subscriber's object.
   *
   * @return
   *   Array containing node objects.
   */
  protected function getSubscriberNodes($subscriber) {
    if (isset($subscriber->nodes)) {
      return $subscriber->nodes;
    }
    $nodes= array();
    $tids = db_query('SELECT tid_subscribed
      FROM {newsletter_index_subscribers}
      WHERE subscriber_id = :sid AND list_id = :lid',
      array(
        ':sid' => $subscriber->id,
        ':lid' => $this->list->id,
      ))->fetchField();
    $tids = @unserialize($tids);
    if (is_array($tids)) {
      foreach ($tids as $tid) {
        $query = $this->getQuery();
        $query->condition('tax.tid', $tid);
        $nodes = array_merge($nodes, $query->execute()->fetchAll());
      }
    }
    foreach ($nodes as $node) {
      $newsletter_nodes[] = node_load($node->nid);
    }
    return $newsletter_nodes;
  }

  /**
   * Updates newsletter list after it is sent.
   *
   * @param $times_sent
   *   The number of subscribers the newsletter was sent to.
   *
   * @return
   *   Boolean depending on whether the update succeeded or not.
   */
  protected function updateStats($times_sent) {
    switch ($this->list->send_rate) {
      case 'Daily':
        $send_again = date('Y-m-d', strtotime('+1 day'));
        break;
      case 'Weekly':
        $send_again = date('Y-m-d', strtotime('+1 week'));
        break;
      case 'Monthly':
        $send_again = date('Y-m-d', strtotime('+1 month'));
        break;
      default:
        $send_again = NULL;
    }
    $updated_list = db_update('newsletter_lists')
                    ->fields(array(
                      'last_sent' => REQUEST_TIME,
                      'send_again' => $send_again,
                    ))
                    ->condition('id', $this->list->id)
                    ->execute();

    $sent_so_far = db_query('SELECT subscribers_sent
      FROM {newsletter_newsletters} WHERE id = :id',
      array(':id' => $this->newsletter->id))
      ->fetchField();
    $subscribers_sent = isset($sent_so_far)
      ? ($sent_so_far + $times_sent)
      : $times_sent;

    $updated_stats = db_update('newsletter_newsletters')
      ->fields(array(
        'last_sent' => REQUEST_TIME,
        'subscribers_sent' => $subscribers_sent,
      ))
      ->condition('id', $this->newsletter->id)
      ->execute();
    return (isset($updated_list) && isset($updated_stats)) ? TRUE : FALSE;
  }

  /**
   * Checkes whether current custom list reached its send rate.
   */
  public function checkCustom() {
    $query = db_select('taxonomy_index', 'tax');
    $query->join('newsletter_index', 'news', 'tax.tid = news.tid');
    $query->join('newsletter_templates', 'template', 'news.template_id = template.id');
    $query->join('newsletter_lists', 'list', 'template.id = list.template_id');
    $query->join('node', 'node', 'tax.nid = node.nid');
    $query->addExpression('COUNT(node.nid)', 'count');
    $query->condition('list.id', $this->list->id);
    $query->where('(list.last_sent < node.created) OR (list.last_sent IS NULL )');

    $count = $query->execute()->fetchField();

    return (bool) ($count >= $this->list->send_rate);
  }

  /**
   * Replaces tokens and sends the current newsletter.
   */
  public function send() {
    $i=0;
    $mails_to_send = variable_get('newsletter_cron_number', 500);
    $params = array(
      'template' => $this->template,
      'subscriber' => '',
      'list' => $this->list,
      'format' => $this->format,
      'newsletter' => $this->newsletter,
    );

    if (empty($this->subscribers)) {
      return array('No subscribers');
    }

    foreach ($this->subscribers as $subscriber) {
      $language = isset($subscriber->language) ? newsletter_language_list($subscriber->language) : $this->language;
      $nodes = ($this->nodes == 'exposed')
        ? $this->getSubscriberNodes($subscriber)
        : $this->nodes;

      if (empty($nodes)) {
        continue;
      }
      $params['nodes'] = $nodes;
      if ($i>=$mails_to_send) {
        $subscriber->nodes = $nodes;
        $this->left[$this->list->id][] = $subscriber;
        continue;
      }
      $params['subscriber'] = $subscriber;
      $message = drupal_mail('newsletter', 'automated', $subscriber->email, $language, $params, $this->from);
      if (!$message['result']) {
        newsletter_set_watchdog($this->newsletter->id, $this->list->title, $subscriber->email, $result);
      }
      $status[] = $message['result'];

      $i++;
    }

    variable_set('newsletter_for_next_cron', $this->left);

    $this->updateStats($i);

    return isset($status) ? $status : array('No items');
  }
}

/**
 * Newsletter class that sends custom newsletters.
 */
class NewsletterMailCustom extends NewsletterBasic {
  protected $template;
  protected $subscribers;
  protected $newsletter;

  public function __construct($subscribers, $template_id, $newsletter_id = NULL) {
    parent::__construct();
    $this->template = db_query('SELECT *
      FROM {newsletter_templates}
      WHERE id = :id',
      array(':id' => $template_id))
      ->fetchObject();
    $this->subscribers = $subscribers;
    $this->newsletter = db_query('SELECT *
      FROM {newsletter_newsletters}
      WHERE id = :id',
      array(':id' => $newsletter_id))
      ->fetchObject();
  }

  /**
   * Updates current newsletter's number of subscribers sent.
   *
   * @param $times_sent
   *   int The number of subscribers this newsletter was sent this time.
   */
  protected function updateNewsletter($times_sent) {
    $sent_so_far = db_query('SELECT subscribers_sent
      FROM {newsletter_newsletters}
      WHERE id = :id',
      array(':id' => $this->newsletter->id))
      ->fetchField();
    $subscribers_sent = isset($sent_so_far)
      ? ($sent_so_far + $times_sent)
      : $times_sent;

    $updated_stats = db_update('newsletter_newsletters')
      ->fields(array(
        'last_sent' => REQUEST_TIME,
        'subscribers_sent' => $subscribers_sent,
      ))
      ->condition('id', $this->newsletter->id)
      ->execute();
  }

 /**
   * Custom newsletter preview for newsletter administrators.
   *
   * @return
   *   an array containing html formatted newsletter's body and subject
   */
  public function preview() {
    $data['body'] = token_replace($this->template->body);
    $data['body_format'] = $this->template->format;
    $data['subject'] = token_replace($this->template->subject);
    return $data;
  }

  /**
   * Replaces tokens, sends the custom newsletter and updates stats.
   *
   * @return
   *   Array containing the sent mail status and subscribers left for next cron
   */
  public function send() {
    $i=0;
    $mails_to_send = variable_get('newsletter_cron_number', 500);
    $subscribers_left = array();
    $status = array();
    $params = array(
      'template' => $this->template,
      'subscriber' => '',
      'list' => $this->list,
      'format' => $this->format,
      'newsletter' => $this->newsletter,
    );
    foreach ($this->subscribers as $subscriber) {
      $language = isset($subscriber->language) ? newsletter_language_list($subscriber->language) : $this->language;
      if ($i>=$mails_to_send) {
        $subscribers_left[] = $subscriber;
        continue;
      }
      $params['subscriber'] = $subscriber;
      $message = drupal_mail('newsletter', 'custom', $subscriber->email, $language, $params, $this->from);

      if (!$message['result']) {
        newsletter_set_watchdog($this->newsletter->id, $this->newsletter->title, $subscriber->email, $result);
      }
      $status[] = $message['result'];

      $i++;
    }
    $this->updateNewsletter($i);
    return array(
      'status' => $status,
      'subscribers_left' => $subscribers_left,
    );
  }
}
