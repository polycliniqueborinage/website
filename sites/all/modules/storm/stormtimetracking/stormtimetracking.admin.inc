<?php

/**
 * @file
 */

function stormtimetracking_list_form_report() {
  $form = array();
  $form['#prefix'] = '<div id="stormtimetracking_list_form_report" class="storm_list_form_report">';
  $form['report'] = array(
    '#type' => 'select',
    '#title' => t('Reports'),
    '#options' => theme('stormtimetracking_list_form_report_reports'),
  );

  if (module_exists('locale')) {
    $language_list = language_list();
    $languages = array();
    foreach ($language_list as $key => $lang) {
      $languages[$key] = $lang->name;
    }
    $form['language'] = array(
      '#type' => 'select',
      '#title' => t('Language'),
      '#options' => $languages,
    );
  }
  else {
    $form['language'] = array(
      '#type' => 'hidden',
      '#default_value' => 'en',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Report'),
  );
  $form['#suffix'] = '</div>';
  return $form;
}

function stormtimetracking_list_form_report_submit($form, &$form_state) {
  $report = $form_state['values']['report'];
  $language = $form_state['values']['language'];
  drupal_goto('storm/timetrackings/report/'. $report .'/'. $language);
}

function stormtimetracking_list_report($report, $language) {
  $timetrackings = array();

  $s = $_SESSION['stormtimetracking_list_filter']['sql'];
  $args = $_SESSION['stormtimetracking_list_filter']['args'];

  $r = db_query($s, $args);
  while ($t = db_fetch_object($r)) {
    $timetracking = node_load($t->nid);
    $timetrackings[] = $timetracking;
  }

  print theme('stormtimetracking_list_report', $report, $language, $timetrackings);
}
