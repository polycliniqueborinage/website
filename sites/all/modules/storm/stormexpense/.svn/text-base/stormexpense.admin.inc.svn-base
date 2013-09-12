<?php

/**
 * @file
 */

function stormexpense_list_form_report() {
  $form = array();
  $form['#prefix'] = '<div id="stormexpense_list_form_report" class="storm_list_form_report">';
  $form['report'] = array(
    '#type' => 'select',
    '#title' => t('Reports'),
    '#options' => theme('stormexpense_list_form_report_reports'),
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

function stormexpense_list_form_report_submit($form, &$form_state) {
  $report = $form_state['values']['report'];
  $language = $form_state['values']['language'];
  drupal_goto('storm/expenses/report/'. $report .'/'. $language);
}

function stormexpense_list_report($report, $language) {
  $timetrackings = array();

  $sql = $_SESSION['stormexpense_list_filter']['sql'];
  $args = $_SESSION['stormexpense_list_filter']['args'];
  $r = db_query($sql, $args);
  while ($e = db_fetch_object($r)) {
    $expense = node_load($e->nid);
    $expenses[] = $expense;
  }

  print theme('stormexpense_list_report', $report, $language, $expenses);
}

function stormexpense_autocomplete($string = '') {
  // Autocomplete for providers textbox: shows any active organizations in Storm system which have been selected as providers.
  $matches = array();
  if ($string) {
    $s = "SELECT n.title FROM {node} AS n INNER JOIN {stormorganization} AS sor ON n.vid=sor.vid WHERE n.status=1 AND n.type='stormorganization' AND sor.isprovider=1 AND sor.isactive=1";
    $where = array("LOWER(title) LIKE LOWER('%s%%')");
    $s = stormorganization_access_sql($s, $where);
    $s = db_rewrite_sql($s);
    $result = db_query_range($s, $string, 0, 10);
    while ($pro = db_fetch_object($result)) {
      $matches[$pro->title] = check_plain($pro->title);
    }
  }

  drupal_json($matches);
}
