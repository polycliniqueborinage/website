<?php

/**
 * @file
 * Administrative page callbacks for the filemaker module.
 */





/*************************************************************
 * Connections.
 *************************************************************/





/**
 * Callback for admin settings page (admin/settings/filemaker).
 *
 * @return
 *    Themed HTML table of all the FileMaker connections stored in the Drupal database.
 */
function filemaker_admin_settings_page() {

  $link = l(t('Add a connection'), 'admin/settings/filemaker/connection');
  drupal_set_message($link);

  $link = l(t('Add a FileMaker node'), 'node/add/filemaker');
  drupal_set_message($link);

  $link = l(t('Add a script to run at cron'), 'admin/settings/filemaker/cron/script');
  drupal_set_message($link);

  $head = array('fmcid', 'Hostname', 'Database', 'Username', 'Password', 'Actions');
  $rows = array();

  // Get connections from database.
  $query = db_query("SELECT * FROM {filemaker_connection}");
  $i = 0;

  // Loop through connections and add to each one to the $rows array.
  while ($row = db_fetch_array($query)) {
    $rows[$i] = $row;

    // Don't display password.
    $rows[$i]['fmpassword'] = '*********';

    // Actions (links to edit and delete).
    $edit_link = l('Edit', 'admin/settings/filemaker/connection/' . $rows[$i]['fmcid']);
    $delete_link = l('Delete', 'admin/settings/filemaker/connection/' . $rows[$i]['fmcid'] . '/delete');
    $rows[$i]['actions'] = $edit_link . ' | ' . $delete_link;

    $i++;
  }

  return theme_table($head, $rows);
}





/**
 * Page callback for the FileMaker cron scripts page (admin/settings/filemaker/cron-jobs).
 *
 * @return
 *    Form to insert or edit a FileMaker script to be run with Drupal cron.
 */
function filemaker_admin_settings_scripts_page($fmsid = NULL) {

  $link = l('Add a script', 'admin/settings/filemaker/cron/script');
  drupal_set_message($link);

  $head = array('fmsid', 'fmcid', 'Layout', 'Script', 'Parameter', 'Actions');
  $rows = array();

  // Get connections from database.
  $query = db_query("SELECT fmsid, fmcid, layout, name, parameter FROM {filemaker_script}");
  $i = 0;

  // Loop through connections and add to each one to the $rows array.
  while ($row = db_fetch_array($query)) {

    $rows[$i] = $row;

    // Parameters can be long, so we only display a 'yes' or 'no.'
    $rows[$i]['parameter'] = ($rows[$i]['parameter']) ? 'yes' : 'no';

    // Actions (links to edit and delete).
    $edit_link = l('Edit', 'admin/settings/filemaker/cron/script/' . $rows[$i]['fmsid']);
    $delete_link = l('Delete', 'admin/settings/filemaker/cron/script/' . $rows[$i]['fmsid'] . '/delete');
    $rows[$i]['actions'] = $edit_link . ' | ' . $delete_link;

    $i++;
  }

  return theme_table($head, $rows);
}





/**
 * Page callback for the filemaker connection page page, (admin/settings/filemaker/connection).
 *
 * @return
 *    Form to insert or edit connection and themed HTML table of all the layouts the connection being edited has access to.
 */
function filemaker_admin_settings_connection_page($fmcid = NULL) {

  // Is there a connection to edit?
  if ($fmcid) {

    // Get FileMaker object.
    $fm = filemaker_object(NULL, $fmcid);
  
    // Print out table of layouts.
    if ($fm instanceof FileMaker) {
  
      $layouts = $fm->listLayouts();
  
      // Only print out table of connections if we can connect to FileMaker.
      if (!filemaker_is_error($layouts)) {
  
        drupal_set_message(t('Connected to FileMaker.'));
  
        $rows = array();
        foreach ($layouts as $layout) {
          $rows[] = array($layout);
        }
  
        $output .= theme_table(array('Layout'), $rows);
      }
    }
  }

  // Nothing to edit, append form to page to create connection record.
  $output .= drupal_get_form('filemaker_connection_form', $fmcid);

  return $output;
}






/**
 * Form to add or edit a single FileMaker connection.
 */
function filemaker_connection_form(&$form_state, $fmcid) {

  $form = array();

  // Record being inserted or updated?
  if ($fmcid) {
    
    // Get data to fill in fields.
    $sql = "SELECT * FROM {filemaker_connection} WHERE fmcid = %d";
    $fmc = db_fetch_object(db_query($sql, $fmcid));
    
    // Store fmcid to access in submit handler.
    $form_state['storage']['fmcid'] = $fmcid;
  }

  // Insert validation, submit function names, and redirect location.
  $form['#submit'][] = 'filemaker_connection_submit';
  $form['#redirect'] = 'admin/settings/filemaker';

  // Create connection fieldset.
  $form['connection'] = array(
    '#title' => t('FileMaker Connection'),
    '#type' => 'fieldset',
  );

  // Hostname field.
  $form['connection']['fmhostname'] = array(
    '#title' => t('Host Name'),
    '#type' => 'textfield',
    '#default_value' => $fmc->fmhostname,
  );

  // Database field.
  $form['connection']['fmdatabase'] = array(
    '#title' => t('Database Name'),
    '#type' => 'textfield',
    '#default_value' => $fmc->fmdatabase,
  );

  // Username field.
  $form['connection']['fmusername'] = array(
    '#title' => t('Username'),
    '#type' => 'textfield',
    '#default_value' => $fmc->fmusername,
  );

  // Password field.
  $form['connection']['fmpassword'] = array(
    '#title' => t('Password'),
    '#type' => 'password',
    '#default_value' => $fmc->fmpassword,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Connection'),
  );

  return $form;
}





/**
 * Submit handler for filemaker_connection_form.
 */
function filemaker_connection_submit(&$form, &$form_state) {

  $fmcid = $form_state['storage']['fmcid'];

  // Update connection?
  if ($fmcid) {

    $sql = "UPDATE {filemaker_connection} SET fmhostname = '%s', fmdatabase = '%s', fmusername = '%s', fmpassword = '%s' WHERE fmcid = '%d'";
    db_query($sql, 
      $form_state['values']['fmhostname'],
      $form_state['values']['fmdatabase'],
      $form_state['values']['fmusername'],
      $form_state['values']['fmpassword'],
      $fmcid
    );

    drupal_set_message(t('Connection updated.'));
  }

  // Insert connection.
  else {

    $sql = "INSERT INTO {filemaker_connection} (fmcid, fmhostname, fmdatabase, fmusername, fmpassword) VALUES (NULL, '%s', '%s', '%s', '%s')";
    db_query($sql, 
      $form_state['values']['fmhostname'],
      $form_state['values']['fmdatabase'],
      $form_state['values']['fmusername'],
      $form_state['values']['fmpassword']
    );

    drupal_set_message(t('Connection Created.'));
  }
}





/**
 * Deletes a single FileMaker connection.
 */
function filemaker_delete_connection($fmcid) {

  // Delete record.
  $sql = "DELETE FROM {filemaker_connection} WHERE fmcid = %d";
  $result = db_query($sql, $fmcid);

  drupal_set_message(t('FileMaker connection deleted.'));
  watchdog('filemaker', 'FileMaker connection deleted.', array(), WATCHDOG_INFO);
}  





/*************************************************************
 * Scripts.
 *************************************************************/





/**
 * Form to add or edit a single FileMaker connection.
 */
function filemaker_script_form(&$form_state, $fmsid = NULL) {

  // If we are editing a script, we can skip choosing a connection (step 1).
  if ($fmsid) {
    $form_state['storage']['step'] = 2;
    $form_state['storage']['fmsid'] = $fmsid;
    
    // Get connection befor forwarding to script details (step 2).
    $sql = "SELECT fmcid FROM {filemaker_script} WHERE fmsid = %d";
    $fmcid = db_result(db_query($sql, $fmsid));

    $form_state['storage']['step_1_values']['fmcid'] = $fmcid;
  }

  // Which step are we on?
  switch ($form_state['storage']['step']) {

    case NULL;
      $form = filemaker_script_form_1($form_state, $fmsid);
      break;

    case 2;
      $form = filemaker_script_form_2($form_state, $fmsid);
      break;
  }

  return $form;
}





/**
 * Step 1 of our script form, for choosing a connection, which is required for step 2.
 */
function filemaker_script_form_1(&$form_state, $fmsid) {

  $form = array();

  $form['#submit'][] = 'filemaker_cron_script_form_submit';

  // If there are FileMaker connections to select from, show a select field and submit.
  $options = filemaker_connections_array();
  if ($options) {
    $form['fmcid'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => t('Connection to FileMaker database'),
      '#required' => TRUE,
      '#default_value' => '', // isset($node->fmcid) ? $node->fmcid : '',
      '#weight' => 5,
    );

    // Submit button.
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Use connection and select FileMaker script',
      '#weight' => 50,
    );
  }

  // No connections in the database, which means we have to create them before we can do anything.
  else {
    $link = l(t('Add a FileMaker Connection'), 'admin/settings/filemaker/connection');
    $form['nofmcid'] = array(
      '#type' => 'markup',
      '#title' => t('No FileMaker database connections.'),
      '#value' => $link,
      '#weight' => 5,
    );
  }

  return $form;
}





/**
 * Step 2 of our script form, for selecting script details.
 */
function filemaker_script_form_2(&$form_state, $fmsid) {

  // Updating?
  if ($fmsid) {
    $sql = "SELECT * FROM {filemaker_script} WHERE fmsid = %d";
    $script = db_fetch_object(db_query($sql, $fmsid));
  }

  // Need a FileMaker object for layout and script options.
  $fmcid = $form_state['storage']['step_1_values']['fmcid'];
  $fm = filemaker_object(NULL, $fmcid);
  // Least expensive way I can come up with to figure out if we are connected.
  $connected = $fm->getProperty('database');

  $form = array();
  $form['#submit'][] = 'filemaker_cron_script_form_submit';
  $form['#redirect'][] = 'admin/settings/filemaker/cron';


  // This fieldset will be hidden until a connection is selcted.
  $attributes = array('id' => 'script-details');
  $form['details'] = array(
    '#title' => t('Script Details'),
    '#type' => 'fieldset',
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#attributes' => $attributes,
  );

  // After the 2nd step is submitted, the form will be rebuilt and
  // $fmcid isn't there. We will get errors if we can't connect.
  if ($connected) {
    // Layout.
    $options = filemaker_layouts_array($fm);
    $form['details']['layout'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => t('FileMaker layout'),
      '#required' => TRUE,
      '#default_value' => isset($script->layout) ? $script->layout : '',
    );

    // Script.
    $options = filemaker_scripts_array($fm);
    $form['details']['name'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => t('FileMaker script'),
      '#required' => TRUE,
      '#default_value' => isset($script->name) ? $script->name : '',
    );
  }

  // Script parameter.
  $form['details']['parameter'] = array(
    '#title' => t('Optional script parameter'),
    '#type' => 'textfield',
    '#required' =>  FALSE,
    '#default_value' => isset($script->parameter) ? $script->parameter : '',
  );
 

  // Submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save FileMaker script',
    '#weight' => 50,
  );

  return $form;
}





/**
 * Submit handler for filemaker_script_form().
 */
function filemaker_cron_script_form_submit(&$form, &$form_state) {

  // Step one? Go to step two form.
  if (!$form_state['storage']['step']) {
    $form_state['storage']['step'] = 2;
    $form_state['storage']['step_1_values'] = $form_state['values'];
  }

  // Step two form. Process results.
  else {

    // Updating?
    if ($form_state['storage']['fmsid']) {
      $sql = "UPDATE {filemaker_script} SET layout = '%s', name = '%s', parameter = '%s' WHERE fmsid = %d";
      db_query($sql,
        $form_state['values']['layout'],
        $form_state['values']['name'],
        $form_state['values']['parameter'],
        $form_state['storage']['fmsid']
      );
    }

    // Inserting.
    else {
      $sql = "INSERT INTO {filemaker_script} (fmcid, layout, name, parameter) VALUES (%d, '%s', '%s', '%s')";
      db_query($sql, 
        $form_state['storage']['step_1_values']['fmcid'],
        $form_state['values']['layout'],
        $form_state['values']['name'],
        $form_state['values']['parameter']
      );
    }

    unset($form_state['storage']);
  }
}





/**
 * Returns an array of FileMaker layouts, for the $options of a select field.
 */
function filemaker_scripts_array($fm) {
  
  $script_options = $fm->listScripts();
  filemaker_is_error($options);

  if (!$script_options) return;

  $options = array();

  foreach ($script_options as $script) {
    $options[$script] = $script;
  }

  return $options;
}





/**
 * Deletes a single FileMaker script.
 */
function filemaker_delete_script($fmsid) {

  // Delete record.
  $sql = "DELETE FROM {filemaker_script} WHERE fmsid = %d";
  $result = db_query($sql, $fmsid);

  drupal_set_message(t('FileMaker script deleted.'));
  watchdog('filemaker', 'FileMaker script deleted.', array(), WATCHDOG_INFO);
}
