<?php

/**
 * @file
 */

function _storminvoice_project_invoices_js($organization_nid, $project_nid) {
  $invoices = array();

  $s = "SELECT n.nid, n.title FROM {node} AS n INNER JOIN {storminvoice} AS sin
  ON n.vid=sin.vid WHERE n.status=1 AND n.type='storminvoice' AND sin.organization_nid=%d AND sin.project_nid=%d
  ORDER BY n.title";

  $s = storminvoice_access_sql($s);
  $s = db_rewrite_sql($s);
  $r = db_query($s, $organization_nid, $project_nid);
  while ($item = db_fetch_object($r)) {
    $nid = $item->nid;
    $invoices[$nid] = $item->title;
  }
  print drupal_to_js($invoices);
  exit();
}

function storminvoice_report($node, $report, $language) {
  print theme('storminvoice_report', $node, $report, $language);
}

/**
 * Page
 */
function storminvoice_send_page($node, $language) {
  $breadcrumb = array();
  $breadcrumb[] = l(t('Storm'), 'storm');
  $breadcrumb[] = l(t('Invoices'), 'storm/invoices');
  $breadcrumb[] = l($node->title, 'node/'.$node->nid);

  drupal_set_breadcrumb($breadcrumb);
  drupal_set_html_head('<meta name="robots" content="noindex, nofollow" />');

  $path =  'node/'. $node->nid;

  return drupal_get_form('storminvoice_send_form', $path, $node, $language);
}

/**
 * Form
 */
function storminvoice_send_form(&$form_state, $path = NULL, $node = NULL, $language) {
  global $base_url, $user;

  $pdffile=variable_get('storminvoice_email_pdf_file_path', '');

  $organization = node_load($node->organization_nid);
  $organization_email = $organization->email;

  $form = array();
  $cid = array();

  $emailtype = 'email';

  $form['message']['instructions'] = array(
    '#type' => 'item',
    '#value' => variable_get('storminvoice_email_pdf_instructions', t('<p>From here you can send PDF file of invoice as attachment to your client</p><p><b>NOTE:</b> You can change subject and cover note of email. If <i>Bcc</i> field is checked you will also get copy of email.</p>')),
  );
  $form['message']['yemail'] = array(
    '#type' => 'textfield',
    '#title' => t('Your Email'),
    '#size' => 58,
    '#maxlength' => 256,
    '#required' => TRUE,
  );
  $form['message']['yname'] = array(
    '#type' => 'textfield',
    '#title' => t('Your Name'),
    '#size' => 58,
    '#maxlength' => 256,
    '#required' => TRUE,
  );
  $form['message']['recipients'] = array(
    '#type' => 'textfield',
    '#title' => t('Send To'),
    '#default_value' => $organization_email,
    '#cols' => 50,
    '#rows' => 1,
    '#description' => t('Enter multiple addresses and separate them with commas.'),
    '#required' => TRUE,
  );

  $form['message']['bcc'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send me a copy'),
    '#default_value' => 1,
    '#description' => t('Leave this box checked if you want to receive a copy as Bcc.'),
  );

  $form['message']['page'] = array(
      '#type' => 'item',
      '#title' => t('You are going to email the following invoice'),
      '#value' => l($node->title, 'storm/invoice/report/'. $node->nid .'/pdf/'. $language),
    );

  $form['message']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Message Subject'),
    '#value' => variable_get('storminvoice_cover_note_subject', ''),
    '#description' => t('Enter subject for email.'),
  );
  $form['message']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Message Body'),
    '#value' => variable_get('storminvoice_cover_note', ''),
    '#description' => 'Cover Note',
    '#cols' => 50,
    '#rows' => 10,
    '#required' => TRUE,
  );

  $form['message']['path'] = array(
    '#type' => 'hidden',
    '#value' => $path,
  );

  $form['message']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send Invoice'),
  );

  if ($user->uid != 0) {
    $form['message']['yemail']['#default_value'] = $user->mail;
    $form['message']['yemail']['#disabled'] = TRUE;
    $form['message']['yemail']['#value'] = $user->mail;
    $form['message']['yname']['#default_value'] = $user->name;
  }

  return $form;
}

function storminvoice_send_form_submit($form, &$form_state) {
  $form_filled = $form_state['clicked_button']['#post'];

  $to = $form_filled['recipients'];
  $language = null;
  $from = $form_state['values']['yname'].'<'.$form_state['values']['yemail'].'>';

  $params = array();

  $params['subject'] = $form_filled['subject'];
  $params['bcc'] = $form_state['values']['bcc'];

  $trenner  = md5(uniqid(time()));
  variable_set('storminvoice_email_pdf_trenner', $trenner);

  $headers['Content-Type'] = "multipart/mixed; boundary=$trenner";
  $params['body']  = "\n--$trenner\n";
  $params['body'] .= "Content-Type: text/plain; charset=UTF-8; format=flowed;"."\n\n"; // sets the mime type
  $params['body'] .= $form_filled['body']."\n";
  $params['body'] .= "\n\n";
  $params['body'] .= "\n\n";

  $node =  $form['#parameters'][3];
  $language = $form['#parameters'][4];
  $filedata = theme('storminvoice_report_pdf', $node, $language, 'email');

  $pdffile = variable_get('storminvoice_email_pdf_file_path', '');
  $pdffile_name = variable_get('storminvoice_email_pdf_file_name', '');
  $pdffile_mime = file_get_mimetype($pdffile);
  $params['body']  .= "--$trenner"."\n";
  $params['body']  .= "Content-Type:$pdffile_mime; name='$pdffile_name'\n";
  $params['body']  .= "Content-Disposition: attachment; filename='$pdffile_name'\n";
  $params['body']  .= "Content-Transfer-Encoding: base64\n\n";
  $params['body']  .= chunk_split(base64_encode($filedata));
  $params['body'] .= "--$trenner--";

  drupal_mail('storminvoice', 'invoice', $to, $language, $params, $from , True);

  drupal_set_message(variable_get('storminvoice_sent', t('You have sent invoice to !email.', array('!email' => $to ))));
}

/**
 * Implementation of hook_mail().
 */
function storminvoice_mail($key, &$message, $params) {

  $trenner = variable_get('storminvoice_email_pdf_trenner', '');
  $message['subject'] = $params['subject'];
  $message['body'][] = $params['body'];
  $message['headers']['MIME-Version'] = '1.0';
  $message['headers']['Content-Type'] = "multipart/mixed; boundary=$trenner";
  if ($params['bcc']){
    $message['headers']['Bcc'] = $message['headers']['From'];
  }
}
