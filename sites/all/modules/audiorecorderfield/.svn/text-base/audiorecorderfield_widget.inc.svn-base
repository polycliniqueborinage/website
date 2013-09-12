<?php
// $Id:

/**
 * @file
 * audiorecorderfield widget hooks and callbacks.
 */

 
 /**
 * Implements hook_field_load().
 */
function audiorecorderfield_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  file_field_load($entity_type, $entities, $field, $instances, $langcode, $items, $age);
}

/**
 * Implements hook_field_prepare_view().
 */
function audiorecorderfield_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  //TODO: Check this:
  //  Remove files specified to not be displayed.
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      if (!file_field_displayed($item, $field)) {
        unset($items[$id][$delta]);
      }
      // Ensure consecutive deltas.
      $items[$id] = array_values($items[$id]);
    }
  }
}

/**
 * Implements hook_field_is_empty().
 */
function audiorecorderfield_field_is_empty($item, $field) {
  return file_field_is_empty($item, $field);
}

/**
 * Implements hook_field_widget_info().
 */
function audiorecorderfield_field_widget_info() {
  return array(  
    'audiorecorderfield_widget' => array(
      'label' => t('Audio Recorder'),
      'field types' => array('file'),
      'settings' => array(
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
 
function audiorecorderfield_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  //TODO: Add display_field setting to field because file_field_widget_form() assumes it is set.
  $field['settings']['display_field'] = 0;
  $elements = file_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  $settings = $instance['settings'];

  foreach (element_children($elements) as $delta) {
    // Add all extra functionality provided by the audiorecorder widget.
      $elements[$delta]['#process'] = array('audiorecorderfield_widget_process');
  }

  if ($field['cardinality'] == 1) {
    // If there's only one field, return it as delta 0.
    if (empty($elements[0]['#default_value']['fid'])) {
      $elements[0]['#description'] = theme('file_upload_help', array('description' => $instance['description'], 'upload_validators' => $elements[0]['#upload_validators']));
    }
  }
  else {
    $elements['#file_upload_description'] = theme('file_upload_help', array('upload_validators' => $elements[0]['#upload_validators']));
  }

  return $elements;
}

/*
 * NanoGong recorder applet
 */
function audiorecorderfield_recorder_nanogong($element) {
    // The widget is being presented, so apply the JavaScript.
    drupal_add_js(drupal_get_path('module', 'audiorecorderfield') . '/js/audiorecorder_nanogong.js');
   
    $field_id=str_replace('field_', '', $element['#parents'][0]);
    //Change "_" into "-"
    $field_id=str_replace('_', '-', $field_id);
    $langcode = $element['#parents'][1];
    $delta = $element['#parents'][2];
    $field_name = $field_id."-". $langcode."-".$delta;
    
    //TODO: Load options here (width, height,MaxDuration...)
    /*$width = isset($widget['player_width'])?$widget['recorder_width']:300;
    $height = isset($widget['player_height'])? $widget['recorder_height']:100;
    */
    
    $item = $element['#value'];

    $audio_url='';
    //Nanogong configuration http://gong.ust.hk/nanogong/info_config.html
    $config = '<param name="ShowSaveButton" value="false" />'; //Disable Save button
    $config .= '<param name="ShowTime" value="true" /> '; //Show Time
    $config .= '<param name="ShowSpeedButton" value="false" /> '; //Disable speed button
    $config .= '<param name="MaxDuration" value="1200" /> '; //Set MaxDuration to maximum (20 minutes)
    
    if (!empty($item['uri'])) {
        $audiofile=file_create_url($item['uri']);
        $info = pathinfo($audiofile);
        $button_op='Remove';
        $class_suffix='player';
        
        $op = $info['extension'];
        if ($op=='wav') {
          //Load existing file in Nanogong
          $audio_url = '<param name="SoundFileURL" value="' . $audiofile . '"/>';
          $config .= '<param name="ShowRecordButton" value="false" /> '; //Disable Record button  
          $width='82px';
        }
        else {
          $output = '<div id="nanogong-' . $field_name . '-wrapper">' . audiofield_get_player($audiofile, $op) . '</div>';
        }
    }
    else {
        $op='wav';
        $button_op='Upload';
        $class_suffix='recorder';
        $width='113px';
    }
    
    //Display Nanogong applet only if there is no file or uploaded file is wav
    if ($op=='wav') {
        //Add css
        drupal_add_css(drupal_get_path('module', 'audiorecorderfield') . '/css/audiorecorderfield.css');
        //Nanogong Applet
        $output = '<div class="nanogong-' . $class_suffix . '" id="nanogong-' . $field_name . '-wrapper"><applet id="nanogong-' . $field_name . '" archive="' . base_path() . drupal_get_path('module', 'audiorecorderfield') . '/recorders/nanogong.jar" 
            code="gong.NanoGong" width="' . $width . '" height="40">
            ' . $config . '
            ' . $audio_url . '
            </applet></div>';
    }
    //Upload/Remove button
    $output .= '<div><input id="nanogong-button-' . $field_name . '" type="button" class="form-submit" value="' . $button_op . '" onclick="submitVoice(\'' . $field_id . '\',\'' . $langcode . '\',\'' . $delta . '\')" /></div>';

    $output.='<input type="hidden" value="0" name="field_'.$field_id.'['.$langcode.']['.$delta.'][fid]">';
    return $output;
}

/**
 * An element #process callback for the audiorecorderfield_widget field type.
 *
 * Expands the audiorecorderfield_widget type to additional fields.
 */
function audiorecorderfield_widget_process($element, &$form_state, $form) {

  // If not a recognized field instance, do not process.
  if (!isset($element['#field_name']) || !($instance = field_widget_instance($element, $form_state))) {
    return;
  }
  //TODO: Add support for more recording applets (ListenUp,...)
  //TODO: Add hook_audiofield_recorders to allow other modules to add more recorder applets.
  $element['applet'] = array(
     '#type' =>'item',
    '#markup' => audiorecorderfield_recorder_nanogong($element),
    //'#theme' => 'audiorecorderfield_widget',
  );  
 
  return $element;

}

/**
 * Theme the output of the audio recorder field.
 */
function theme_audiorecorderfield_widget($variables) {
  $element = $variables['element'];
  
  $output = audiorecorderfield_recorder_nanogong($element);
  return '<div class="filefield-source filefield-source-recorder clear-block">' . $output . '</div>';
}