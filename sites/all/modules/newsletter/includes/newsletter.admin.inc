<?php

/**
 * @file
 * Admin page callbacks for the newsletter module.
 */

/**
 * Menu callback; present newsletter statistics.
 */
function newsletter_stats($form, &$form_state) {
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options['compare'] = t('Compare the selected newsletters');
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'compare',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  $header = array(
    'id' => array('data' => t('ID'), 'field' => 'id'),
    'title' => array('data' => t('Newsletter/List Title'), 'field' => 'title'),
    'last_sent' => array('data' => t('Last Sent'), 'field' => 'last_sent', 'sort' => 'desc'),
    'subscribers_sent' => array('data' => t('E-mails Sent'), 'field' => 'subscribers_sent'),
    'clicks' => array('data' => t('Total Clicks'), 'field' => 'clicks'),
    'ctr' => array('data' => t('CTR')),
    'opened' => array('data' => t('Opened(times)'), 'field' => 'opens'),
    'open_rate' => array('data' => t('Open Rate')),
    'send_again' => array('data' => t('To be sent again'), 'field' => 'send_again'),
  );

  $subquery = db_select('newsletter_newsletters', 's');
  $subquery->fields('s', array('title'));
  $subquery->addExpression('MAX(last_sent)', 'maxts');
  $subquery->groupBy('title');

  $query = db_select('newsletter_newsletters', 't')
    ->extend('PagerDefault')
    ->extend('TableSort');
  $query->fields('t');
  $query->join($subquery, 'grp', 'grp.title = t.title AND grp.maxts = t.last_sent');
  $query->leftjoin('newsletter_lists', 'list', 't.title = list.title');
  $query->addField('list', 'send_again', 'send_again');
  $query->condition('t.last_sent', 0, '<>');
  $query->limit(50);
  $query->orderByHeader($header);
  $result = $query->execute();

  $options = array();
  foreach ($result as $row) {
    $send_again_timestamp = strtotime($row->send_again);
    $options[$row->id] = array(
      'id' => (int) $row->id,
      'title' => check_plain($row->title),
      'last_sent' => format_date($row->last_sent, 'short'),
      'subscribers_sent' => (int) $row->subscribers_sent,
      'clicks' => (int) $row->clicks,
      'ctr' => (float) @($row->clicks/$row->subscribers_sent)*100 . '%',
      'opened' => (int) $row->opens,
      'open_rate' => (float) @($row->opens/$row->subscribers_sent)*100 . '%',
      'send_again' => is_numeric($send_again_timestamp) ? format_date($send_again_timestamp, 'short') : '-',
    );
  }

  $form['newsletter'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No newsletters sent yet.'),
  );

  $form['pager'] = array('#theme' => 'pager');
  return $form;
}

function newsletter_stats_submit($form, &$form_state) {
  $operation = $form_state['values']['operation'];
  $newsletters = $form_state['values']['newsletter'];
  if ($newsletters) {
    switch ($operation) {
      case 'compare':
        $args['compare'] = '';
        foreach ($newsletters as $newsletter) {
          if ($newsletter === 0) {
            continue;
          }
          $args['compare'] .= $newsletter . ',';
        }
        $args['compare'] = drupal_encode_path(rtrim($args['compare'], ','));
        drupal_goto('admin/config/media/newsletter/stats/graphic', array(
          'query' => $args,
        ));
        break;
    }
  }
  else {
    drupal_set_message(t('Please Select an action to perform.'), 'warning');
  }
}

/**
 * Menu callback; present newsletter statistics graphs.
 */
function newsletter_stats_graphic() {
  if (!module_exists('libraries')) {
    return drupal_set_message(t('For graphical statistics 
      please install the <a href=@url-lib>Libraries API</a>
      and <a href=@url-jqplot>download the jQplot library</a>.',
    array(
      '@url-lib' => 'http://drupal.org/project/libraries',
      '@url-jqplot' => 'https://bitbucket.org/cleonello/jqplot/downloads/',
    )),
    'warning');
  }
  $path = libraries_get_path('jqplot');
  if (!file_exists($path . '/jquery.jqplot.min.js')) {
    return drupal_set_message(t('jQplot library is not present in your filesystem.
      You need to <a href=@url>download</a> it
      and extract all its files to @path
      folder, so the library is located to
      @path/jquery.jqplot.min.js',
    array(
      '@url' => 'https://bitbucket.org/cleonello/jqplot/downloads/',
      '@path' => $path,
    )),
    'warning');
  }

  //add css and js files for our charts
  $iesupport = array(
    '#type' => 'markup',
    '#markup' => '<!--[if lt IE 9]>
      <script language="javascript"
      type="text/javascript" src="' . $path . '/excanvas.js">
      </script><![endif]--> ',
  );

  drupal_add_html_head($iesupport, 'newsletter');

  drupal_add_js($path . '/jquery.jqplot.min.js');
  drupal_add_js($path . '/plugins/jqplot.highlighter.min.js');
  drupal_add_js($path . '/plugins/jqplot.enhancedLegendRenderer.min.js');
  drupal_add_js($path . '/plugins/jqplot.canvasAxisLabelRenderer.min.js');
  drupal_add_js($path . '/plugins/jqplot.dateAxisRenderer.min.js');
  drupal_add_css($path . '/jquery.jqplot.min.css');
  drupal_add_css(drupal_get_path('module', 'newsletter') . '/css/newsletter.admin.css');

  if (isset($_GET['compare'])) {
    $ids = urldecode($_GET['compare']);
    $ids = explode(',', $ids);

  foreach ($ids as $id) {
      $newsletters[] = db_query('SELECT * FROM {newsletter_newsletters}
        WHERE id = :id
        AND last_sent <> 0',
        array(':id' => $id))
        ->fetchObject();
    }
  }
  else {
    //compare all the existing newsletters; might be slow @TODO check performance
    $newsletters = db_query('SELECT *
      FROM {newsletter_newsletters}
      WHERE last_sent <> 0')
      ->fetchAll();
  }

  $ctr_vars = array();
  $or_vars = array();
  $smaller_timestamp = time();
  foreach ($newsletters as $newsletter) {
    $ctr = @($newsletter->clicks/$newsletter->subscribers_sent) * 100;
    $timestamp_ms = $newsletter->last_sent * 1000;
    @$ctr_vars[$newsletter->title] .= "[$timestamp_ms, $ctr], ";

    $open_rate = @($newsletter->opens/$newsletter->subscribers_sent) * 100;
    @$or_vars[$newsletter->title] .= "[$timestamp_ms, $open_rate], ";

    $smaller_timestamp = ($newsletter->last_sent < $smaller_timestamp)
      ? $newsletter->last_sent
      : $smaller_timestamp;
  }
  $first_day = date('d M,Y');
  if (empty($ctr_vars) && empty($or_vars)) {
    return drupal_set_message(t('No statistics have been gathered yet.'), 'warning');
  }

  $jqplot_js = '(function ($) {$(document).ready(function(){
    $.jqplot.config.enablePlugins = true;';
  $i=1;
  $cvars = '';
  $ovars = '';
  $labels = '';
  foreach ($ctr_vars as $title => $string) {
    $jqplot_js .= "var l$i = [$string];\n";
    $cvars .= "l$i, ";
    $labels .= "{label:'$title'}, ";
    $i++;
  }
  $k=$i;
  foreach ($or_vars as $title => $string) {
    $jqplot_js .= "var l$k = [$string];\n";
    $ovars .= "l$k, ";
    $k++;
  }
  $jqplot_js .= "var first_day = '$first_day';";
  $jqplot_js .= "plot1 = $.jqplot('chart1', [$cvars], {";
  $jqplot_js .= 'legend:{show:true, renderer:$.jqplot.EnhancedLegendRenderer},
                  seriesDefaults: {lineWidth:4},';
  $jqplot_js .= "series:[$labels],";
  $jqplot_js .= 'seriesColors:[ "#4bb2c5", "#c5b47f", "#EAA228", "#579575",
                    "#839557", "#958c12", "#953579", "#4b5de4",
                    "#d8b83f", "#ff5800", "#0085cc"],
                  highlighter: {bringSeriesToFront: true},
                  axes:{
                    xaxis:{
                      renderer: $.jqplot.DateAxisRenderer,
                      tickOptions:{formatString:"%d %b %y"},
                      min: first_day,
                      label:"Date Sent",
                    },
                    yaxis:{
                      pad:1.0,
                      numberTicks:5,
                      autoscale:false,
                      label:"CTR (%)",
                    }
                  }
                });';
  $jqplot_js .= "plot2 = $.jqplot('chart2', [$ovars], {";
  $jqplot_js .= 'legend:{show:true, renderer:$.jqplot.EnhancedLegendRenderer},
                  seriesDefaults: {lineWidth:4},';
  $jqplot_js .= "series:[$labels],";
  $jqplot_js .= 'seriesColors:[ "#4bb2c5", "#c5b47f", "#EAA228", "#579575",
                    "#839557", "#958c12", "#953579", "#4b5de4",
                    "#d8b83f", "#ff5800", "#0085cc"],
                  highlighter: {bringSeriesToFront: true},
                  axes:{
                    xaxis:{
                      renderer: $.jqplot.DateAxisRenderer,
                      tickOptions:{formatString:"%d %b %y"},
                      min: first_day,
                      label:"Date Sent",
                    },
                    yaxis:{
                      pad:1.0,
                      numberTicks:10,
                      autoscale:false,
                      label:"Open Rate (%)"
                    }
                  }
              });
          });})(jQuery);';
  drupal_add_js( $jqplot_js, 'inline');

  return '<div id="chart1"></div><div id="chart2"></div>';
}

/**
 * Menu callback; multistep form to create and send a custom newsletter.
 */
function newsletter_create_send($form, &$form_state) {
  $templates = db_query('SELECT id, subject
    FROM {newsletter_templates}
    WHERE basic = 0')
    ->fetchAllKeyed();
  $lists['all'] = t('All subscribers');
  $lists += db_query('SELECT id, title FROM {newsletter_lists}')->fetchAllKeyed();

  if (!empty($form_state['step']) && $form_state['step'] == 2) {
    return newsletter_create_send_form_page_two($form, $form_state);
  }
  $form_state['step'] = 1;
  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#description' => t('A unique newsletter title'),
  );
  $form['template'] = array(
    '#title' => t('Template'),
    '#type' => 'select',
    '#required' => TRUE,
    '#description' => t('Select the template this newsletter will use'),
    '#options' => array_map('check_plain', $templates),
  );
  $form['list'] = array(
   '#title' => t('Subscriber List'),
   '#type' => 'select',
   '#required' => TRUE,
   '#description' => t('Select the subscriber list this newsletter will use'),
   '#options' => array_map('check_plain', $lists),
  );
  $form['preview'] = array(
    '#type' => 'submit',
    '#value' => t('Preview'),
    '#submit' => array('newsletter_create_send_next_submit'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/create-send',
  );
  return $form;
}
function newsletter_create_send_next_submit($form, &$form_state) {
  $form_state['page_values'][1] = $form_state['values'];
  $form_state['step'] = 2;
  $form_state['rebuild'] = TRUE;
}
function newsletter_create_send_form_page_two($form, &$form_state) {

  $data = newsletter_create($form_state['values']['list'], $form_state['values']['template'])->preview();
  $form['newsletter'] = array(
    '#type' => 'item',
    '#description' => t('Please bare in mind that any change here will affect the template directly'),
  );
  $form['newsletter']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#value' => check_plain($data['subject']),
  );
  $form['newsletter']['body'] = array(
    '#type' => 'text_format',
    '#title' => t('Body'),
    '#value' => $data['body'],
    '#format' => $data['body_format'],
  );
  $form['test-email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail to send test mail to'),
    '#description' => t('Leave this empty to use the site e-mail address'),
  );
  $form['send'] = array(
    '#type' => 'submit',
    '#value' => t('Send newsletter now'),
    '#submit' => array('newsletter_create_send_form_page_two_submit'),
  );
  $form['send-test'] = array(
    '#type' => 'submit',
    '#value' => t('Send test e-mail first to address above'),
    '#submit' => array('newsletter_create_send_form_page_two_submit'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/create-send',
  );
  return $form;
}

function newsletter_create_send_form_page_two_submit($form, &$form_state) {
  $changed_subject = $form_state['input']['subject'] != $form_state['values']['subject'];
  $changed_body = $form_state['input']['body']['value'] != $form_state['values']['body']['value'];
  $changed_format = $form_state['input']['body']['format'] != $form_state['values']['body']['format'];
  if ($changed_subject || $changed_body || $changed_format) {
    db_update('newsletter_templates')
      ->fields(array(
        'subject' => $form_state['input']['subject'],
        'body' => $form_state['values']['body']['value'],
        'format' =>  $form_state['values']['body']['format'],
      ))
      ->condition('id', $form_state['page_values'][1]['template'])
      ->execute();
  }
  $title = $form_state['page_values'][1]['title'];
  $template_id = $form_state['page_values'][1]['template'];
  $newsletter_id = newsletter_custom_newsletter_id($title, $template_id);

  $subscribers = ($form_state['page_values'][1]['list'] == 'all')
    ? db_query('SELECT * FROM {newsletter_subscribers}')->fetchAll()
    : db_query('SELECT * FROM {newsletter_subscribers} subscribers
      JOIN {newsletter_index_subscribers} sub_index
      ON subscribers.id = sub_index.subscriber_id
      WHERE sub_index.list_id = :lid',
      array(':lid' => $form_state['page_values'][1]['list']))->fetchAll();
  if ($form_state['clicked_button']['#type'] == t('Send')) {
    newsletter_queue_custom_newsletter($subscribers, $template_id, $newsletter_id);
  }
  else {
    $email = !empty($form_state['input']['test-email']) ? $form_state['input']['test-email'] : NULL;
    newsletter_send_test($template_id, $email);
    $drafts = variable_get('newsletter_custom_drafts', array());
    $drafts[$newsletter_id] = array(
      'subscribers' => $subscribers,
      'template_id' => $template_id,
    );
    variable_set('newsletter_custom_drafts', $drafts);
  }
  return drupal_goto('admin/config/media/newsletter/create-send');
}

/**
 * Menu callback; send a manual newsletter.
 */
function newsletter_manual_send($list) {
  // Dont send.Just queue it for next cron run
  $remaining = variable_get('newsletter_for_next_cron', array());
  $remaining[$list->id] = TRUE;
  variable_set('newsletter_for_next_cron', $remaining);

  drupal_set_message(t('Newsletter queued for the next cron run'));
  return drupal_goto('admin/config/media/newsletter/create-send');
}

/**
 * Menu callback; send a custom newsletter draft.
 */
function newsletter_form_send_draft($form, &$form_state, $newsletter) {
  $form['newsletter_id'] = array(
    '#type' => 'hidden',
    '#value' => $newsletter->id,
  );
  $output = t('Send this newsletter draft now?');
  return confirm_form($form, t('Delete the following?'), 'admin/config/media/newsletter/lists', filter_xss($output));

}

function newsletter_form_send_draft_submit($form, &$form_state) {
  newsletter_send_draft($form_state['values']['newsletter_id']);
}

/**
 * Menu callback; delete a custom newsletter draft.
 */
function newsletter_form_delete_draft($form, &$form_state, $newsletter) {
  $form['newsletter_id'] = array(
    '#type' => 'hidden',
    '#value' => $newsletter->id,
  );
  $output = t('Are you sure you want to delete this newsletter draft?');
  $output .= t('This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), 'admin/config/media/newsletter/lists', filter_xss($output));
}

function newsletter_form_delete_draft_submit($form, &$form_state) {
  $newsletter_id = $form_state['values']['newsletter_id'];
  $drafts = variable_get('newsletter_custom_drafts', array());
  if (!empty($drafts[$newsletter_id])) {
    unset($drafts[$newsletter_id]);
    variable_set('newsletter_custom_drafts', $drafts);
    // Also delete from database.
    db_delete('newsletter_newsletters')
      ->condition('id', $newsletter_id)
      ->execute();
    drupal_set_message(t('Draft deleted succesfully'));
  }
}

/**
 * Menu callback; delete a sent custom newsletter from database.
 */
function newsletter_form_delete_newsletter($form, &$form_state, $newsletter) {
  $form['newsletter_id'] = array(
    '#type' => 'hidden',
    '#value' => $newsletter->id,
  );
  $output = t('Are you sure you want to delete this newsletter?');
  $output .= '<br />';
  $output .= t('All statistics associated with this newsletter will be deleted too.');
  $output .= '<br />';
  $output .= t('This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), 'admin/config/media/newsletter/lists', filter_xss($output));
}

function newsletter_form_delete_newsletter_submit($form, &$form_state) {
  $newsletter_id = $form_state['values']['newsletter_id'];
  $deleted = db_delete('newsletter_newsletters')
    ->condition('id', $newsletter_id)
    ->execute();
  if ($deleted) {
    drupal_set_message(t('Newsletter deleted succesfully'));
  }
  else {
    drupal_set_message(t('Newsletter failed to delete'), 'error');
  }
}

/**
 * Menu callback; List manual lists and newsletter drafts.
 */
function newsletter_draft_manual_list() {

  $header = array(
    'title' => array('data' => t('Title')),
    'status' => array('data' => t('Status')),
    'template' => array('data' => t('Template')),
    'operations' => array('data' => t('Operations')),
  );
  $query = db_select('newsletter_lists', 'lists');
  $query->join('newsletter_templates', 'templates', 'lists.template_id = templates.id');
  $query->addField('templates', 'subject', 'template_title');
  $query->fields('lists', array('id', 'title'));
  $query->condition('send_rate', 'Manual');

  $manual_lists = $query
    ->execute()
    ->fetchAll();

  $options = array();
  $destination = drupal_get_destination();
  $custom_newsletters = newsletter_load_custom_newsletters();
  $result = array_merge($manual_lists, $custom_newsletters);
  foreach ($result as $key => $row) {

    $operations['data'] = array();

    if (!isset($row->custom)) {
      $status = t('Newsletter List with Manual send rate');
      $send_path = 'admin/config/media/newsletter/create-send/send/';
      $delete_path = 'admin/config/media/newsletter/lists/lists/delete/';
    }
    elseif ($row->custom && !isset($row->sent)) {
      $status = t('Newsletter Draft');
      $send_path = 'admin/config/media/newsletter/create-send/draft/';
      $delete_path = 'admin/config/media/newsletter/create-send/draft/delete/';
    }
    elseif ($row->custom && $row->sent) {
      $status = t('<strong>Sent</strong> Newsletter Draft');
      $delete_path = 'admin/config/media/newsletter/create-send/newsletter/delete/';
    }

    if (!isset($row->sent)) {
      $operations['data'][] = array(
        '#type' => 'link',
        '#title' => t('send'),
        '#href' => $send_path . $row->id,
        '#options' => array('query' => $destination),
      );
      $operations['data'][] = array(
        '#markup' => ' | ',
      );
    }

    $operations['data'][] = array(
      '#type' => 'link',
      '#title' => t('delete'),
      '#href' => $delete_path . $row->id,
      '#options' => array('query' => $destination),
    );

    $options[$key] = array(
      'title' => check_plain($row->title),
      'status' => $status,
      'template' => check_plain($row->template_title),
      'operations' => $operations,
    );
  }

  $table = array(
    'header' => $header,
    'rows' => $options,
    'attributes' => array(),
    'caption' => '',
    'colgroups' => array(),
    'sticky' => TRUE,
    'empty' => t('No custom newsletters or drafts available.'),
  );
  return theme_table($table);
}

/**
 * Menu callback; Form to list subscriber lists on a table and provide certain actions.
 */
function newsletter_list($form, &$form_state) {
  if (isset($form_state['storage']['confirm'])) {
    $form['operation'] = array(
      '#type' => 'hidden',
      '#value' => 'delete',
    );
    $form['list'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['values']['list'],
    );
    $output = t('Are you sure you want to delete the following newsletter lists?');
    $output .= '<ul>';
    foreach ($form_state['values']['list'] as $lid) {
      $list = newsletter_lid_load($lid);
      $output .= !empty($list) ? '<li>' . check_plain($list->title) . '</li>' : '';
    }
    $output .= '</ul>';
    $output .= t('This action cannot be undone.');
    return confirm_form($form, t('Delete the following?'), 'admin/config/media/newsletter/lists', filter_xss($output));
  }

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options['delete'] = t('Delete the selected lists');
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
  $header = array(
    'id' => array('data' => t('ID'), 'field' => 'id'),
    'title' => array('data' => t('Title'), 'field' => 'title'),
    'send_rate' => array('data' => t('Send Rate'), 'field' => 'send_rate'),
    'template' => array('data' => t('Template'), 'field' => 'template'),
    'created' => array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );
  $query = db_select('newsletter_lists', 'lists')->extend('PagerDefault')->extend('TableSort');
  $query->join('newsletter_templates', 'templates', 'lists.template_id = templates.id');
  $query->addField('templates', 'subject', 'template_title');
  $query->fields('lists', array('id', 'title', 'template_id', 'send_rate', 'created'));
  $result = $query
    ->limit(50)
    ->orderByHeader($header)
    ->execute();

  $options = array();
  $destination = drupal_get_destination();
  foreach ($result as $row) {
    $operations = array(
      'data' => array(
        array(
          '#type' => 'link',
          '#title' => t('edit'),
          '#href' => 'admin/config/media/newsletter/lists/lists/edit/' . $row->id ,
          '#options' => array('query' => $destination),
        ),
        array(
          '#markup' => ' | ',
        ),
        array(
          '#type' => 'link',
          '#title' => t('delete'),
          '#href' => 'admin/config/media/newsletter/lists/lists/delete/' . $row->id,
          '#options' => array('query' => $destination),
        ),
      ),
    );

    $options[$row->id] = array(
      'id' => (int) $row->id,
      'title' => check_plain($row->title),
      'send_rate' => check_plain($row->send_rate),
      'template' => check_plain($row->template_title),
      'created' => format_date($row->created, 'short'),
      'operations' => $operations,
    );
  }

  $form['list'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No lists available.'),
  );

  $form['pager'] = array('#theme' => 'pager');
  return $form;
}

function newsletter_list_submit($form, &$form_state) {
  $operation = $form_state['values']['operation'];
  $list = $form_state['values']['list'];
  if ($list) {
    switch ($operation) {
      case 'delete':
        if (!isset($form_state['storage']['confirm'])) {
          $form_state['storage']['confirm'] = TRUE;
          $form_state['rebuild'] = TRUE;
          return;
        }
        foreach ($list as $id) {
          $num_deleted = db_delete('newsletter_lists')
            ->condition('id', $id)
            ->execute();
          db_delete('newsletter_index_subscribers')
            ->condition('list_id', $id)
            ->execute();
        }
        break;
    }
    isset($num_deleted)
      ? drupal_set_message(t('Lists deleted successfully'), 'status')
      : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  }
  else {
    drupal_set_message(t('Please Select an action to perform.'), 'warning');
  }
}

/**
 * Menu callback; add a subscribers list.
 */
function newsletter_list_add($form, &$form_state) {
  $templates = db_query('SELECT id, subject FROM {newsletter_templates} WHERE basic = 0')->fetchAllKeyed();

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
  );
  $form['send_rate'] = array(
    '#type' => 'select',
    '#options' => array(
      'Daily' => t('Daily'),
      'Weekly' => t('Weekly'),
      'Monthly' => t('Monthly'),
      'Manual' => t('Manual'),
      'Custom' => t('Custom'),
    ),
    '#title' => t('Send Rate'),
  );
  $form['send_rate_custom'] = array(
    '#attributes' => array('id' => array('send_rate_custom')),
    '#type' => 'textfield',
    '#title' => t('Custom Send Rate'),
    '#maxlength' => 4,
    '#size' => 4,
    '#description' => t('Only fill this option if you selected Custom Send Rate'),
    '#field_prefix' => t('Once'),
    '#field_suffix' => t('nodes of this list are published.'),
  );
  $form['template_id'] = array(
    '#type' => 'select',
    '#options' => $templates,
    '#title' => t('Template'),
  );
  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Add List'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/lists',
  );
  return $form;
}
function newsletter_list_add_submit($form, &$form_state) {
  $send_rate = $form_state['values']['send_rate']=='Custom'
    ? $form_state['values']['send_rate_custom']
    : $form_state['values']['send_rate'];
  $id = db_insert('newsletter_lists')
    ->fields(array(
      'title' => $form_state['values']['title'],
      'template_id' => $form_state['values']['template_id'],
      'send_rate' => $send_rate,
      'created' => REQUEST_TIME,
    ))
    ->execute();

  $id
    ? drupal_set_message(t('List created successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');

  drupal_goto('admin/config/media/newsletter/lists');
}

/**
 * Menu callback; delete a subscribers list.
 */
function newsletter_list_delete($form, &$form_state, $list) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $list->id,
  );
  $output = t('Are you sure you want to delete the following newsletter list?');

  $output .= '<ul>';
  $output .= '<li>' . check_plain($list->title) . '</li>';
  $output .= '</ul>';

  $output .= t('This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), drupal_get_destination(), filter_xss($output));
}

function newsletter_list_delete_submit($form, &$form_state) {
  $num_deleted = db_delete('newsletter_lists')
    ->condition('id', $form_state['values']['id'])
    ->execute();
  db_delete('newsletter_index_subscribers')
    ->condition('list_id', $form_state['values']['id'])
    ->execute();
  isset($num_deleted)
    ? drupal_set_message(t('List deleted successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto(drupal_get_destination());
}

/**
 * Menu callback; edit a subscribers list.
 */
function newsletter_list_edit($form, &$form_state, $list) {
  $templates = db_query('SELECT id, subject
    FROM {newsletter_templates}
    WHERE basic = 0')->fetchAllKeyed();

  $form['newsletter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit newsletter list.'),
  );
  $form['newsletter']['id'] = array(
    '#type' => 'hidden',
    '#value' => (int) $list->id,
  );
  $form['newsletter']['title'] = array(
    '#type' => 'textfield',
    '#value' => check_plain($list->title),
    '#title' => 'Name',
  );
  $form['newsletter']['template'] = array(
    '#type' => 'select',
    '#options' => array_map('check_plain', $templates),
    '#value' => (int) $list->template_id,
    '#title' => 'Template',
  );
  $form['newsletter']['send_rate'] = array(
    '#type' => 'select',
    '#options' => array(
      'Custom' => t('Custom'),
      'Manual' => t('Manual'),
      'Daily' => t('Daily'),
      'Weekly' => t('Weekly'),
      'Monthly' => t('Monthly'),
    ),
    '#value' => check_plain($list->send_rate),
    '#title' => t('Send Rate'),
  );
  $form['newsletter']['send_rate_custom'] = array(
    '#attributes' => array('id' => array('send_rate_custom')),
    '#type' => 'textfield',
    '#title' => t('Custom Send Rate'),
    '#value' => is_numeric($list->send_rate) ? $list->send_rate : NULL,
    '#maxlength' => 4,
    '#size' => 4,
    '#description' => t('Only fill this option if you selected Custom Send Rate'),
    '#field_prefix' => t('Once'),
    '#field_suffix' => t('nodes of this list are published.'),
  );

  $form['newsletter']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  $form['newsletter']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/lists',
  );
  return $form;
}

function newsletter_list_edit_submit($form, &$form_state) {
  $send_rate = $form_state['input']['send_rate']=='Custom'
    ? $form_state['input']['send_rate_custom']
    : $form_state['input']['send_rate'];
  $num_updated = db_update('newsletter_lists')
    ->fields(array(
      'title' => $form_state['input']['title'],
      'template_id' => $form_state['input']['template'],
      'send_rate' => $send_rate,
    ))
    ->condition('id', $form_state['values']['id'])
    ->execute();
  isset($num_updated)
    ? drupal_set_message(t('List updated successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto(drupal_get_destination());
}

/**
 * Menu callback; add subscribers.
 */
function newsletter_subscriber_add($form, &$form_state) {

  $lists = db_query('SELECT id, title FROM {newsletter_lists}')->fetchAllKeyed();
  require_once DRUPAL_ROOT . '/includes/locale.inc';
  $countries = country_get_list();

  $form['new'] = array(
    '#markup' => '<h3>' . t('Add Subscriber') . '</h3>',
  );
  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#required' => TRUE,
  );
  $form['list'] = array(
    '#type' => 'select',
    '#title' => t('Select Lists'),
    '#options' => array_map('check_plain', $lists),
    '#required' => TRUE,
    '#multiple' => TRUE,
  );
  $form['additional'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Info'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['additional']['firstname'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
  );
  $form['additional']['lastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
  );
  $form['additional']['age_group'] = array(
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
  );
  $form['additional']['job'] = array(
    '#type' => 'textfield',
    '#title' => t('Job'),
  );
  $form['additional']['gender'] = array(
    '#type' => 'select',
    '#title' => t('Gender'),
    '#options' => array(
      'male' => t('Male'),
      'female' => t('Female'),
      'other' => t('Other'),
    ),
    '#empty_option' =>  t('- Select -'),
  );
  $form['additional']['country'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#options' => $countries,
    '#empty_option' =>  t('- Select -'),
  );
  $form['additional']['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
  );
  $form['additional']['language'] = array(
    '#type' => 'select',
    '#title' => t('Language'),
    '#options' => newsletter_language_list(),
    '#empty_option' =>  t('- Select -'),
  );
  $form['additional']['receive_format'] = array(
    '#type' => 'select',
    '#title' => t('Preferred Format'),
    '#options' => array(
      'html' => t('HTML'),
      'plain' => t('Plain Text'),
    ),
    '#empty_option' =>  t('- Select -'),
  );
  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/lists/subscribers',
  );
  return $form;
}
function newsletter_subscriber_add_submit($form, &$form_state) {
  $subscriber = new stdClass();
  $subscriber->email = $form_state['input']['email'];
  $subscriber->firstname = empty($form_state['input']['firstname']) ? NULL : $form_state['input']['firstname'];
  $subscriber->lastname = empty($form_state['input']['lastname']) ? NULL : $form_state['input']['lastname'];
  $subscriber->age_group = empty($form_state['input']['age_group']) ? NULL : $form_state['input']['age_group'];
  $subscriber->job = empty($form_state['input']['job']) ? NULL : $form_state['input']['job'];
  $subscriber->country = empty($form_state['input']['country']) ? NULL : $form_state['input']['country'];
  $subscriber->city = empty($form_state['input']['city']) ? NULL : $form_state['input']['city'];
  $subscriber->gender = empty($form_state['input']['gender']) ? NULL : $form_state['input']['gender'];
  $subscriber->receive_format = empty($form_state['input']['receive_format']) ? NULL : $form_state['input']['receive_format'];
  $subscriber->language = empty($form_state['input']['language']) ? NULL : $form_state['input']['language'];
  $subscriber->confirmed = 1;
  newsletter_save_new_subscriber($subscriber);

  if (!isset($subscriber->id)) {
    return;
  }
  foreach ($form_state['input']['list'] as $list) {
      $indexes[] = db_insert('newsletter_index_subscribers')
        ->fields(array(
          'list_id' => $list,
          'subscriber_id' => $subscriber->id,
          'created' => REQUEST_TIME,
        ))
        ->execute();
  }
  ($subscriber->id && $indexes)
    ? drupal_set_message(t('Subscriber created successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto('admin/config/media/newsletter/lists/subscribers');
}

/**
 * Menu callbacks; add subscribers (mass).
 */
function newsletter_subscriber_add_mass($form, &$form_state) {

  $lists = db_query('SELECT id, title FROM {newsletter_lists}')->fetchAllKeyed();

  $form['new'] = array(
    '#markup' => '<h3>' . t('Add Subscribers') . '</h3>',
  );
  $form['subscribers'] = array(
    '#type' => 'textarea',
    '#title' => t('Emails'),
    '#required' => TRUE,
    '#description' => t('Enter one e-mail address per line.To add additional info, use the following format using 0 or FALSE to omit a value: %format', array('%format' => 'email;firstname;lastname;age_group;job;country;city;gender;newsletter-format;language')),
  );
  $form['list'] = array(
    '#type' => 'select',
    '#title' => t('Select Lists'),
    '#options' => array_map('check_plain', $lists),
    '#required' => TRUE,
    '#multiple' => TRUE,
  );
  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/lists/subscribers',
  );
  return $form;
}

function newsletter_subscriber_add_mass_submit($form, &$form_state) {
  $entries=$form_state['input']['subscribers'];
  $entries = explode("\n", $entries);
  foreach ($entries as $entry) {
    $subscriber = new stdClass();
    $entry = explode(';', $entry);
    $entry = array_map('trim', $entry);
    $subscriber->email = (isset($entry[0]) && !empty($entry[0])) ? $entry[0] : NULL;
    $subscriber->firstname = (isset($entry[1]) && !empty($entry[1])) ? $entry[1] : NULL;
    $subscriber->lastname = (isset($entry[2]) && !empty($entry[2])) ? $entry[2] : NULL;
    $subscriber->age_group = (isset($entry[3]) && !empty($entry[3])) ? $entry[3] : NULL;
    $subscriber->job =  (isset($entry[4]) && !empty($entry[4])) ? $entry[4] : NULL;
    $subscriber->country = (isset($entry[5]) && !empty($entry[5])) ? $entry[5] : NULL;
    $subscriber->city = (isset($entry[6]) && !empty($entry[6])) ? $entry[6] : NULL;
    $subscriber->gender = (isset($entry[7]) && !empty($entry[7])) ? $entry[7] : NULL;
    $subscriber->receive_format = (isset($entry[8]) && !empty($entry[8])) ? $entry[8] : NULL;
    $subscriber->gender = (isset($entry[9]) && !empty($entry[9])) ? $entry[9] : NULL;
    $subscriber->confirmed = 1;
    if (!isset($subscriber->email)) {
      continue;
    }
    newsletter_save_new_subscriber($subscriber);
    if (!isset($subscriber->id)) {
      continue;
    }
    foreach ($form_state['input']['list'] as $list) {
        $indexes = db_insert('newsletter_index_subscribers')
                ->fields(array(
                  'list_id' => $list,
                  'subscriber_id' => $subscriber->id,
                  'created' => REQUEST_TIME,
                ))
                ->execute();
    }
  }
  drupal_goto('admin/config/media/newsletter/lists/subscribers');
}

/**
 * Menu callback; present an administrative subscriber listing.
 */
function newsletter_subscriber_list($form, &$form_state) {
  if (isset($form_state['storage']['confirm'])) {
    $form['operation'] = array(
      '#type' => 'hidden',
      '#value' => 'delete',
    );
    $form['subscriber'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['values']['subscriber'],
    );
    $output = t('Are you sure you want to delete the following newsletter subscribers?');
    $output .= '<ul>';
    foreach ($form_state['values']['subscriber'] as $sid) {
      $subscriber = newsletter_sid_load($sid);
      $output .= !empty($subscriber) ? '<li>' . check_plain($subscriber->email) . '</li>' : '';
    }
    $output .= '</ul>';
    $output .= t('This action cannot be undone.');
    return confirm_form($form, t('Delete the following?'), 'admin/config/media/newsletter/lists/subscribers', filter_xss($output));
  }
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#attributes' => array('class' => array('container-inline')),
  );

  $options['delete'] = t('Delete the selected subscribers');

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

  $header = array(
    'id' => array('data' => t('ID'), 'field' => 'id'),
    'email' => array('data' => t('Email'), 'field' => 'email'),
    'info' => array('data' => t('Personal Info')),
    'list_title' => array('data' => t('Subscribed to')),
    'created' => array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    'confirmed' => array('data' => t('Confirmed')),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('newsletter_subscribers', 'subscribers')->extend('PagerDefault')->extend('TableSort');
  $query->join('newsletter_index_subscribers', 'subs_index', 'subscribers.id = subs_index.subscriber_id');
  $query->join('newsletter_lists', 'lists', 'subs_index.list_id = lists.id');
  $query->addField('lists', 'title', 'list_title');
  $result = $query
    ->fields('subscribers')
    ->limit(50)
    ->orderByHeader($header)
    ->execute();

  $options = array();
  $destination = drupal_get_destination();
  foreach ($result as $row) {

    $info = array('data' => array());
    $info['data'][] = isset($row->firstname) ? array('#markup' => t('Firstname:') . check_plain($row->firstname) . '<br />') : NULL;
    $info['data'][] = isset($row->lastname) ? array('#markup' => t('Lastname:') . check_plain($row->lastname) . '<br />') : NULL;
    $info['data'][] = isset($row->age_group) ? array('#markup' => t('Age Group:') . check_plain($row->age_group) . '<br />') : NULL;
    $info['data'][] = isset($row->job) ? array('#markup' => t('Job:') . check_plain($row->job) . '<br />') : NULL;
    $info['data'][] = isset($row->country) ? array('#markup' => t('Country:') . check_plain($row->country) . '<br />') : NULL;
    $info['data'][] = isset($row->city) ? array('#markup' => t('City:') . check_plain($row->city) . '<br />') : NULL;
    $info['data'][] = isset($row->gender) ? array('#markup' => t('Gender:') . check_plain($row->gender) . '<br />') : NULL;
    $info['data'][] = isset($row->receive_format) ? array('#markup' => t('Preferred Format:') . check_plain($row->receive_format) . '<br />') : NULL;
    $info['data'][] = isset($row->language) ? array('#markup' => t('Language:') . check_plain($row->language) . '<br />') : NULL;

    $lists = db_query('SELECT title
      FROM {newsletter_lists} list
      JOIN {newsletter_index_subscribers} sub_index
      ON sub_index.list_id = list.id
      WHERE sub_index.subscriber_id = :id',
      array(':id' => $row->id ))
      ->fetchCol();
    $lists = implode(', ', $lists);

    $options[$row->id] = array(
      'id' => (int) $row->id,
      'email' => check_plain($row->email),
      'info' => $info,
      'list_title' => check_plain($lists),
      'created' => format_date($row->created, 'short'),
      'confirmed' => $row->confirmed ? t('Yes, on') . '<br/>' . format_date($row->confirmation_timestamp, 'short') : t('No'),
      'operations' => array(
        'data' => array(
          array(
            '#type' => 'link',
            '#title' => t('edit'),
            '#href' => 'admin/config/media/newsletter/lists/subscribers/edit/' . $row->id,
            '#options' => array('query' => $destination),
          ),
          array(
            '#markup' => ' | '
          ),
          array(
            '#type' => 'link',
            '#title' => t('delete'),
            '#href' => 'admin/config/media/newsletter/lists/subscribers/delete/' . $row->id,
            '#options' => array('query' => $destination),
          ),
        ),
      ),
    );
  }

  $form['subscriber'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No subscribers yet.'),
  );

  $form['pager'] = array('#theme' => 'pager');
  return $form;
}
function newsletter_subscriber_list_submit($form, &$form_state) {
  $operation = $form_state['values']['operation'];
  $subscriber = $form_state['values']['subscriber'];

  if ($subscriber) {
    switch ($operation) {
      case 'delete':
        if (!isset($form_state['storage']['confirm'])) {
          $form_state['storage']['confirm'] = TRUE;
          $form_state['rebuild'] = TRUE;
          return;
        }
        foreach ($subscriber as $id) {
          $deleted = db_delete('newsletter_subscribers')
            ->condition('id', $id)
            ->execute();
        }
        is_numeric($deleted)
          ? drupal_set_message(t('Subscriber deleted successfully'), 'status')
          : drupal_set_message(t('An unknown error occured,please try again'), 'error');
        break;
    }
  }
  else {
    drupal_set_message(t('Please Select an action to perform.'), 'warning');
  }
}

/**
 * Menu callback; edit subscribers.
 */
function newsletter_subscriber_edit($form, &$form_state, $subscriber) {
  $lists = db_query('SELECT id, title FROM {newsletter_lists}')->fetchAllKeyed();
  require_once DRUPAL_ROOT . '/includes/locale.inc';
  $countries = country_get_list();
  $subscriber_lists_id = db_query('SELECT id
    FROM {newsletter_lists} list
    JOIN {newsletter_index_subscribers} sub_index
    ON sub_index.list_id = list.id WHERE sub_index.subscriber_id = :id',
    array(':id' => $subscriber->id ))
    ->fetchCol();

  $form['edit'] = array(
    '#markup' => '<h3>' . t('Edit Subscriber') . '</h3>',
  );
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => (int) $subscriber->id,
  );
  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#required' => TRUE,
    '#value' => check_plain($subscriber->email),
  );
  $form['list'] = array(
    '#type' => 'select',
    '#title' => t('Select Lists'),
    '#options' => array_map('check_plain', $lists),
    '#required' => TRUE,
    '#multiple' => TRUE,
    '#default_value' => $subscriber_lists_id,
  );
  $form['additional'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Info'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['additional']['firstname'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#value' => check_plain($subscriber->firstname),
  );
  $form['additional']['lastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#value' => check_plain($subscriber->lastname),
  );
  $form['additional']['age_group'] = array(
    '#type' => 'select',
    '#title' => t('Age Group'),
    '#options' => array(
      '<18' => '<18',
      '18-24' => '18-24',
      '25-31' => '25-31',
      '32-45' => '32-45',
      '>46' => '>46',
    ),
    '#value' => check_plain($subscriber->age_group),
  );
  $form['additional']['job'] = array(
    '#type' => 'textfield',
    '#title' => t('Job'),
    '#value' => check_plain($subscriber->job),
  );
  $form['additional']['country'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#options' => $countries,
    '#value' => check_plain($subscriber->country),
  );
  $form['additional']['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#value' => check_plain($subscriber->gender),
  );
  $form['additional']['gender'] = array(
    '#type' => 'textfield',
    '#title' => t('Gender'),
    '#value' => check_plain($subscriber->gender),
  );
  $form['additional']['language'] = array(
    '#type' => 'select',
    '#title' => t('Language'),
    '#options' => newsletter_language_list(),
    '#value' => check_plain($subscriber->language),
  );
  $form['additional']['receive_format'] = array(
    '#type' => 'select',
    '#title' => t('Preferred Format'),
    '#options' => array(
      'html' => t('HTML'),
      'plain' => t('Plain Text'),
    ),
    '#value' => check_plain($subscriber->receive_format),
  );
  $form['submitnew'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/lists/subscribers',
  );
  return $form;
}

function newsletter_subscriber_edit_submit($form, &$form_state) {
  $num_updated = db_update('newsletter_subscribers')
    ->fields(array(
      'email' => $form_state['input']['email'],
      'age_group' => !empty($form_state['input']['age_group']) ? $form_state['input']['age_group'] : NULL,
      'firstname' => !empty($form_state['input']['firstname']) ? $form_state['input']['firstname'] : NULL,
      'lastname' => !empty($form_state['input']['lastname']) ? $form_state['input']['lastname'] : NULL,
      'job' => !empty($form_state['input']['job']) ? $form_state['input']['job'] : NULL,
      'gender' => !empty($form_state['input']['gender']) ? $form_state['input']['gender'] : NULL,
      'country' => !empty($form_state['input']['country']) ? $form_state['input']['country'] : NULL,
      'city' => !empty($form_state['input']['city']) ? $form_state['input']['city'] : NULL,
      'receive_format' => !empty($form_state['input']['receive_format']) ? $form_state['input']['receive_format'] : NULL,
      'language' => !empty($form_state['input']['language']) ? $form_state['input']['language'] : NULL,
    ))
    ->condition('id', $form_state['values']['id'])
    ->execute();

  $deleted = db_delete('newsletter_index_subscribers')
    ->condition('subscriber_id',  $form_state['values']['id'])
    ->execute();


  foreach ($form_state['input']['list'] as $list_id) {
    $term = db_insert('newsletter_index_subscribers')
      ->fields(array(
        'subscriber_id' => $form_state['values']['id'],
        'list_id' => $list_id,
        'created' => REQUEST_TIME,
      ))
      ->execute();
  }

  return ($num_updated || $deleted)
    ? drupal_set_message(t('Subscriber updated successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
}

/**
 * Menu callback; delete subscribers.
 */
function newsletter_subscriber_delete($form, &$form_state, $subscriber) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $subscriber->id,
  );
  $output = t('Are you sure you want to delete the following newsletter subscriber?');

  $output .= '<ul>';
  $output .= '<li>' . check_plain($subscriber->email) . '</li>';
  $output .= '</ul>';

  $output .= t('This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), drupal_get_destination(), filter_xss($output));
}

function newsletter_subscriber_delete_submit($form, &$form_state) {
  $num_deleted = db_delete('newsletter_subscribers')
    ->condition('id', $form_state['values']['id'])
    ->execute();
  db_delete('newsletter_index_subscribers')
    ->condition('subscriber_id', $form_state['values']['id'])
    ->execute();
  $num_deleted
    ? drupal_set_message(t('Subscriber deleted successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto(drupal_get_destination());
}

/**
 * Menu callback; present an administrative template listing.
 */
function newsletter_template_list($form, &$form_state) {
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
    return confirm_form($form, t('Delete the following?'), 'admin/config/media/newsletter/templates', filter_xss($output));
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
  $header = array(
    'id' => array('data' => t('ID'), 'field' => 'id'),
    'title' => array('data' => t('Title'), 'field' => 'title'),
    'exposed' => array('data' => t('Exposed'), 'field' => 'exposed'),
    'created' => array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('newsletter_templates', 'templates')->extend('PagerDefault')->extend('TableSort');
  $result = $query
    ->fields('templates', array('id', 'subject', 'exposed', 'created'))
    ->condition('basic', 0)
    ->limit(50)
    ->orderByHeader($header)
    ->execute();

  $options = array();
  $destination = drupal_get_destination();
  foreach ($result as $row) {
    $options[$row->id] = array(
      'id' => (int) $row->id,
      'title' => check_plain($row->subject),
      'exposed' => $row->exposed ? t('Yes') : t('No'),
      'created' => format_date($row->created, 'short'),
      'operations' => array(
        'data' => array(
          array(
            '#type' => 'link',
            '#title' => t('edit'),
            '#href' => 'admin/config/media/newsletter/templates/edit/' . $row->id ,
            '#options' => array('query' => $destination),
          ),
          array(
            '#markup' => ' | ',
          ),
          array(
            '#type' => 'link',
            '#title' => t('delete'),
            '#href' => 'admin/config/media/newsletter/templates/delete/' . $row->id,
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

function newsletter_template_list_submit($form, &$form_state) {
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

/**
 * Menu callback; add a newsletter template.
 */
function newsletter_template_add($form, &$form_state) {
  $tree = newsletter_get_tree();
  $vids = newsletter_get_vids();

  $form['new'] = array(
    '#markup' => '<h3>' . t('Add new Template') . '</h3>',
  );
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
    '#token_types' => array('site', 'node', 'newsletter', 'term'),
    '#recursion_limit' => 2,
  );
  $form['body'] = array(
    '#type' => 'text_format',
    '#title' => t('Body'),
    '#required' => TRUE,
  );
  if (isset($tree)) {
    $form['terms'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Terms'),
    );
    $i=0;
    foreach ($tree as $vocabulary_name => $terms) {
      $form['terms'][$vocabulary_name] = array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#title' => check_plain($vocabulary_name),
      );
      $form['terms'][$vocabulary_name]['term_id_' . $vids[$i]] = array(
        '#type' => 'checkboxes',
        '#options' => array_map('check_plain', $terms),
      );
      $i++;
    }
  }
  else {
    drupal_set_message(t('No terms added yet.You need to add taxonomy terms, in order to have dynamic newsletter content'), 'warning');
  }
  $form['exposed'] = array(
    '#type' => 'select',
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#title' => t('Exposed'),
    '#description' => t('Should subscribers be able to choose from the checked terms below?'),
  );
  $form['vids'] = array(
    '#type' => 'hidden',
    '#value' => $vids,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Template'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/templates/mail',
  );

  return $form;
}
function newsletter_template_add_submit($form, &$form_state) {
  $vids = $form_state['values']['vids'];
  $id = db_insert('newsletter_templates')
    ->fields(array(
      'subject' => $form_state['values']['subject'],
      'body' => $form_state['values']['body']['value'],
      'format' => $form_state['values']['body']['format'],
      'exposed' => $form_state['values']['exposed'],
      'created' => REQUEST_TIME,
    ))
    ->execute();
  $id
    ? drupal_set_message(t('Template created successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');

  foreach ($vids as $vid) {
    foreach ($form_state['values']['term_id_' . $vid] as $tid => $checked) {
      if ($checked!=0) {
        $term = db_insert('newsletter_index')
          ->fields(array(
            'template_id' => $id,
            'tid' => $tid,
            'created' => REQUEST_TIME,
          ))
          ->execute();
      }
    }
  }
  drupal_goto('admin/config/media/newsletter/templates/mail');
}

/**
 * Menu callback; delete newsletter templates.
 */
function newsletter_template_delete($form, &$form_state, $template) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $template->id,
  );
  $output = t('Are you sure you want to delete the following template?');

  $output .= '<ul>';
  $output .= '<li>' . check_plain($template->subject) . '</li>';
  $output .= '</ul>';

  $output .= t('This action cannot be undone.');
  return confirm_form($form, t('Delete the following?'), drupal_get_destination(), filter_xss($output));
}

function newsletter_template_delete_submit($form, &$form_state) {
  $num_deleted = db_delete('newsletter_templates')
    ->condition('id', $form_state['values']['id'])
    ->execute();
  db_delete('newsletter_index')
    ->condition('template_id', $form_state['values']['id'])
    ->execute();
  $num_deleted
    ? drupal_set_message(t('Template deleted successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto($_GET['destination']);
}

/**
 * Menu callbacks; edit basic or non-basic newsletter templates.
 */
function newsletter_basic_template_edit( $form, &$form_state, $tid=NULL) {
  $template = db_query("SELECT *
    FROM {newsletter_templates}
    WHERE id = :id", array(':id' => $tid))
    ->fetchObject();

  $form['newsletter-template'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit newsletter list.'),
  );
  $form['newsletter-template']['id'] = array(
    '#type' => 'hidden',
    '#value' => (int) $template->id,
  );
  $form['newsletter-template']['subject'] = array(
    '#type' => 'textfield',
    '#value' => check_plain($template->subject),
    '#title' => t('Subject'),
    '#description' => t('This will also used as newsletter title.'),
  );
  $form['newsletter-template']['token_help'] = array(
    '#title' => t('Replacement patterns'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['newsletter-template']['token_help']['help'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('site', 'newsletter'),
    '#recursion_limit' => 2,
  );
  $form['newsletter-template']['body'] = array(
    '#type' => 'text_format',
    '#format' => $template->format,
    '#value' => $template->body,
    '#title' => t('Body'),
  );
  $form['newsletter-template']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  $form['newsletter-template']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/templates',
  );
  return $form;
}

function newsletter_basic_template_edit_submit($form, &$form_state) {
  $num_updated = db_update('newsletter_templates')
    ->fields(array(
      'subject' => $form_state['input']['subject'],
      'body' => $form_state['input']['body']['value'],
      'format' => $form_state['input']['body']['format'],
      'exposed' => isset($form_state['input']['exposed']) ? $form_state['input']['exposed'] : 0,
    ))
    ->condition('id', $form_state['values']['id'])
    ->execute();

  isset($num_updated)
    ? drupal_set_message(t('Template updated successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto('admin/config/media/newsletter/templates');
}

function newsletter_template_edit($form, &$form_state, $template) {

  $tree = newsletter_get_tree();
  $vids = newsletter_get_vids();
  $checked = newsletter_template_checked_terms($template->id);

  $form['newsletter-template'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit newsletter list.'),
  );
  $form['newsletter-template']['vids'] = array(
    '#type' => 'hidden',
    '#value' => $vids,
  );
  $form['newsletter-template']['id'] = array(
    '#type' => 'hidden',
    '#value' => (int) $template->id,
  );
  $form['newsletter-template']['subject'] = array(
    '#type' => 'textfield',
    '#value' => check_plain($template->subject),
    '#title' => t('Subject'),
    '#description' => t('This will also used as newsletter title.'),
  );
  $form['newsletter-template']['token_help'] = array(
    '#title' => t('Replacement patterns'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Use [repeat] and [/repeat] for recurring items such as nodes'),
  );
  $form['newsletter-template']['token_help']['help'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('site', 'node', 'newsletter', 'term'),
    '#recursion_limit' => 2,
  );
  $form['newsletter-template']['body'] = array(
    '#type' => 'text_format',
    '#value' => $template->body,
    '#format' => $template->format,
    '#title' => 'Body',
  );
  if (!empty($tree)) {
    $form['newsletter-template']['terms'] = array(
      '#type' => 'fieldset',
      '#collapsed' => FALSE,
      '#collapsible' => TRUE,
      '#title' => t('Terms'),
    );
    $i=0;
    foreach ($tree as $vocabulary_name => $terms) {
      $form['newsletter-template']['terms'][$vocabulary_name] = array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#title' => check_plain($vocabulary_name),
      );
      $form['newsletter-template']['terms'][$vocabulary_name]['term_id_' . $vids[$i]] = array(
        '#type' => 'checkboxes',
        '#options' => array_map('check_plain', $terms),
        '#value' => $checked,
      );
      $i++;
    }
  }
  $form['newsletter-template']['exposed'] = array(
    '#type' => 'checkbox',
    '#value' => $template->exposed,
    '#title' => t('Exposed'),
  );

  $form['newsletter-template']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  $form['newsletter-template']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/newsletter/templates',
  );
  return $form;
}

function newsletter_template_edit_submit($form, &$form_state) {
  $num_updated = db_update('newsletter_templates')
    ->fields(array(
      'subject' => $form_state['input']['subject'],
      'body' => $form_state['input']['body']['value'],
      'format' => $form_state['input']['body']['format'],
      'exposed' => isset($form_state['input']['exposed']) ? $form_state['input']['exposed'] : 0,
    ))
    ->condition('id', $form_state['values']['id'])
    ->execute();
  db_delete('newsletter_index')
    ->condition('template_id',  $form_state['values']['id'])
    ->execute();
  if (is_array($form_state['values']['vids'])) {
    foreach ($form_state['values']['vids'] as $vid) {
      foreach ($form_state['input']['term_id_' . $vid] as $tid => $checked) {
        if ($checked != 0) {
          $term = db_insert('newsletter_index')
            ->fields(array(
              'template_id' => $form_state['values']['id'],
              'tid' => $tid,
              'created' => REQUEST_TIME,
            ))
            ->execute();
        }
      }
    }
  }
  isset($num_updated)
    ? drupal_set_message(t('Template updated successfully'), 'status')
    : drupal_set_message(t('An unknown error occured,please try again'), 'error');
  drupal_goto('admin/config/media/newsletter/templates');
}

/**
 * Menu callback; present newsletter configuration.
 */
function newsletter_configure($form, &$form_state) {
  $personal_info_form = variable_get('newsletter_personal_info_form');
  $personal_info_options = array(
      'show' => t('show'),
      'require' => t('require'),
  );
  $form['test'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'fieldset',
    '#title' => t('Send a test Newsletter'),
  );
  $form['test']['to'] = array(
    '#type' => 'textfield',
    '#title' => t('To'),
    '#value' => variable_get('site_mail'),
  );
  $form['test']['send_test'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Settings'),
  );
  $form['settings']['from'] = array(
    '#type' => 'textfield',
    '#title' => t('From'),
    '#size' => 60,
    '#maxlength' => 128,
    '#value' => variable_get('newsletter_from') ? variable_get('newsletter_from') : variable_get('site_mail'),
    '#description' => t('Suggested format: User &lt;user@example.com&gt;'),
  );
  $form['settings']['format'] = array(
    '#type' => 'select',
    '#title' => t('Format'),
    '#options' => array(
      'plain' => t('Plain Text'),
      'html' => t('HTML'),
    ),
    '#default_value' => variable_get('newsletter_format', 'html'),
  );
  $form['settings']['track_open'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track open rate'),
    '#default_value' => variable_get('newsletter_track_open_rate', FALSE),
    '#description' => t('Enable it, only if you really want the email to be tracked,
      since the method used might cause some email clients to mark your emails as spam'),
  );
  $form['settings']['nopercron'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Mails to Send per Cron Run'),
    '#default_value' => variable_get('newsletter_cron_number', 500),
    '#description' => t('The more newsletter lists the smaller this number should be.'),
  );
  $form['settings']['send_confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send confirmation email'),
    '#default_value' => variable_get('newsletter_send_confirm'),
    '#description' => t('Attention: If you disable confirmation e-mails, users will be able to subscribe others without their permission.'),
  );
  $form['settings']['send_welcome'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send welcome email'),
    '#default_value' => variable_get('newsletter_send_welcome'),
  );
  $form['settings']['send_unsubscribe'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send unsubscribe email, after a subscriber decides to unsubscribe'),
    '#default_value' => variable_get('newsletter_send_unsubscribe'),
  );
  $form['settings']['subscribe_form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Subscribe Form'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['settings']['subscribe_form']['show_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Always show e-mail in the subscribe form.'),
    '#value' => variable_get('newsletter_show_email', TRUE),
  );
  $form['settings']['subscribe_form']['show_email_in_block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Always show e-mail in the subscribe form block.'),
    '#value' => variable_get('newsletter_show_email_in_block', FALSE),
  );
  $form['settings']['subscribe_form']['always_show_subscribe_block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Always show subscribe block,even if user is subscribed.'),
    '#value' => variable_get('newsletter_always_show_subscribe_block', FALSE),
  );
  $form['settings']['subscribe_form']['personal_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Personal Info Form'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['settings']['subscribe_form']['personal_info']['firstname'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('First Name (textfield)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['firstname'],
  );
  $form['settings']['subscribe_form']['personal_info']['lastname'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('Last Name (textfield)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['lastname'],
  );
  $form['settings']['subscribe_form']['personal_info']['gender'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('Gender (select)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['gender'],
  );
  $form['settings']['subscribe_form']['personal_info']['age_group'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('Age Group (select)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['age_group'],
  );
  $form['settings']['subscribe_form']['personal_info']['job'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('Job (textfield)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['job'],
  );
  $form['settings']['subscribe_form']['personal_info']['city'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('City (select)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['city'],
  );
  $form['settings']['subscribe_form']['personal_info']['country'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('Country (textfield)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['country'],
  );
  $form['settings']['subscribe_form']['personal_info']['receive_format'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('Preferred Format (select)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['receive_format'],
  );
  $form['settings']['subscribe_form']['personal_info']['language'] = array(
    '#attributes' => array('class' => array('container-inline')),
    '#type' => 'checkboxes',
    '#title' => t('Language (select)'),
    '#options' => $personal_info_options,
    '#value' => $personal_info_form['language'],
  );
  $form['settings']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  return $form;
}

function newsletter_configure_submit($form, &$form_state) {
  if ($form_state['input']['op'] == t('Update')) {
    $personal_info_form = array(
      'firstname' => $form_state['input']['firstname'],
      'lastname' => $form_state['input']['lastname'],
      'gender' => $form_state['input']['gender'],
      'age_group' => $form_state['input']['age_group'],
      'job' => $form_state['input']['job'],
      'city' => $form_state['input']['city'],
      'country' => $form_state['input']['country'],
      'receive_format' => $form_state['input']['receive_format'],
      'language' => $form_state['input']['language'],
    );
    variable_set('newsletter_from', $form_state['input']['from']);
    variable_set('newsletter_format', $form_state['input']['format']);
    variable_set('newsletter_track_open_rate', $form_state['input']['track_open']);
    variable_set('newsletter_cron_number', $form_state['input']['nopercron']);
    variable_set('newsletter_send_confirm', $form_state['input']['send_confirm']);
    variable_set('newsletter_send_welcome', $form_state['input']['send_welcome']);
    variable_set('newsletter_send_unsubscribe', $form_state['input']['send_unsubscribe']);
    variable_set('newsletter_show_email', isset($form_state['input']['show_email']));
    variable_set('newsletter_show_email_in_block', isset($form_state['input']['show_email_in_block']));
    variable_set('newsletter_always_show_subscribe_block', isset($form_state['input']['always_show_subscribe_block']));
    variable_set('newsletter_personal_info_form', $personal_info_form);

    drupal_set_message(t('Settings saved successfully'), 'status');
  }
  elseif ($form_state['input']['op'] == t('Send')) {
    $to = $form_state['input']['to'];

    $status = newsletter_create()->sendTest($to);
    if ($status) {
      drupal_set_message(t('Test mail sent successfully!'), 'status');
    }
    else {
      drupal_set_message(t('Your test mail failed to be delivered.Please check your server logs for more information.', 'error'));
    }
  }
}

/**
 * Queues a custom newsletter for cron run.
 *
 * @param $subscribers
 *   An array containing the subscribers this newsletter must be delivered.
 * @param $template_id
 *   The id of the newsletter template to be used.
 * @param $newsletter_id
 *   The id of the newsletter.
 */
function newsletter_queue_custom_newsletter($subscribers, $template_id, $newsletter_id) {
    $custom_queue = variable_get('newsletter_custom_for_next_cron', array());
    $custom_queue[] = array(
      'subscribers' => $subscribers,
      'template_id' => $template_id,
      'newsletter_id' => $newsletter_id,
    );
    variable_set('newsletter_custom_for_next_cron', $custom_queue);
    drupal_set_message(t('Newsletter queued for the next cron run'));
}


/**
 * Sends a test newsletter to site administrator.
 *
 * @param $template_id
 *   The newsletter template id..
 */
function newsletter_send_test($template_id, $email = NULL) {
  if (!isset($email)) {
    $email = variable_get('site_mail');
  }
  $message = drupal_mail('newsletter', $template_id, $email, language_default(), array(), $email, FALSE);
  $message = array_merge($message, newsletter_create(array(), $template_id)->preview());
  $message['format'] = variable_get('newsletter_format', 'html');
  $system = drupal_mail_system('newsletter', $template_id);
  $message = $system->format($message);
  $status = $system->mail($message);
  return $status ? drupal_set_message(t('Test mail sent succesfully')) : drupal_set_message(t('Test mail failed to send'));
}

/**
 * Load draft newsletters as object containing all needed information.
 *
 * @param $sent
 *  Whether to load the sent custom Newlsetters.
 * @return
 *  an array with newsletter draft objects with the following properties:
 *  id: the draft key in the drupal variable system.
 *  title: the newsletter title.
 *  template_title: the template title this draft uses.
 *  subscribers: the array with subscriber objects that this newsletter will be
 *  sent to
 *  draft: boolean TRUE.
 *
 */
function newsletter_load_custom_newsletters($sent = TRUE) {
  $draft_newsletters = variable_get('newsletter_custom_drafts', array());
  $custom_newsletters = array();
  foreach ($draft_newsletters as $newsletter_id => $newsletter) {
    $draft = new stdClass();
    $draft->id = $newsletter_id;
    $draft->title = newsletter_nid_load($newsletter_id)->title;
    $draft->template_title = newsletter_tid_load($newsletter['template_id'])->subject;
    $draft->custom = TRUE;
    $custom_newsletters[] = $draft;
  }
  if ($sent) {
    $query = db_select('newsletter_newsletters', 'newsletters');
    $query->join('newsletter_templates', 'templates', 'newsletters.template_id = templates.id');
    $query->addField('templates', 'subject', 'template_title');
    $query->fields('newsletters', array('id', 'title'));
    $query->condition('custom', 1);

    $manual_lists = $query->execute();
    foreach ($manual_lists as $manual) {
      $manual->custom = TRUE;
      if (array_search($manual, $custom_newsletters) === FALSE) {
        $manual->sent = TRUE;
        $custom_newsletters[] = $manual;
      }
    }
  }
  return $custom_newsletters;
}

/**
 * Send a custom newsletter draft.
 *
 * @param $newsletter_id
 *  The draft's key in Drupal's variable system, which is the newsletter's id.
 */
function newsletter_send_draft($newsletter_id) {
  $drafts = variable_get('newsletter_custom_drafts', array());
  if (isset($drafts[$newsletter_id]) && !empty($drafts[$newsletter_id])) {
    newsletter_queue_custom_newsletter($drafts[$newsletter_id]['subscribers'], $drafts[$newsletter_id]['template_id'], $newsletter_id);
    unset($drafts[$newsletter_id]);
    variable_set('newsletter_custom_drafts', $drafts);
  }
}


/**
 * @return
 *   An array with available vocabularies that have terms.
 */
function newsletter_get_vids() {
  $vocabularies = taxonomy_get_vocabularies();
  foreach ($vocabularies as $vocabulary) {
    //Make sure this vocabulary has terms
    $count = db_query('SELECT COUNT(tid)
      FROM {taxonomy_term_data}
      WHERE vid = :vid',
      array(':vid' => $vocabulary->vid))
      ->fetchField();
    if ($count>0)
      $vids[] = (int) $vocabulary->vid;
  }
  return isset($vids) ? $vids : NULL;
}

/**
 * @return
 *   An 3 level associated array of vocabulary names - term ids and term names.
 */
function newsletter_get_tree() {
  $vocabularies = taxonomy_get_vocabularies();
  foreach ($vocabularies as $vocabulary) {
    $terms = taxonomy_get_tree($vocabulary->vid);
    foreach ($terms as $term) {
      $tree[check_plain($vocabulary->name)][$term->tid] = check_plain($term->name);
    }
  }
  return isset($tree) ? $tree : NULL;
}

/**
 * @return
 *   An array with enabled newsletter terms.
 */
function newsletter_template_checked_terms($template_id) {
  $checked = db_query('SELECT *
    FROM {newsletter_index}
    WHERE template_id = :templateid',
    array(':templateid' => $template_id))
    ->fetchAll();
  foreach ($checked as $check) {
    $checked_tids[$check->tid] = 1;
  }
  return isset($checked_tids) ? $checked_tids : NULL;
}

/**
 * Actually creates the custom newsletter.
 *
 * @param $title
 *   The custom newsletter's title.
 *
 * @return
 *   The id of the requested newsletter title as integer.
 */
function newsletter_custom_newsletter_id($title, $template_id) {
  $max_send_id = (int) db_query('SELECT MAX(send_id) as send_id
    FROM {newsletter_newsletters}
    WHERE title = :title',
    array(':title' => $title))
    ->fetchField();
  if (!empty($max_send_id)) {
    $newsletter_id = db_insert('newsletter_newsletters')
      ->fields(array(
        'title' => $title,
        'send_id' => ++$max_send_id,
        'custom' => 1,
        'template_id' => $template_id,
      ))
      ->execute();
  }
  else {
    $newsletter_id = db_insert('newsletter_newsletters')
      ->fields(array(
        'title' => $title,
        'custom' => 1,
        'template_id' => $template_id,
      ))
      ->execute();
  }
  return (int) $newsletter_id;
}

