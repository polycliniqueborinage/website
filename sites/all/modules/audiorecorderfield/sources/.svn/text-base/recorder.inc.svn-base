<?php
// $Id$

/**
 * @file
 * A FileField extension to allow uploading audio files by recording them in Nanogong
 */

/**
 * Implementation of hook_filefield_source_info().
 */
function filefield_source_recorder_info() {
  $source['recorder'] = array(
    'name' => t('Audio Recorder'),
    'label' => t('Audio Recorder'),
    'description' => t('Record audio using Nanogong audio recorder.'),
    'process' => 'filefield_source_recorder_process',
    //'value' => 'filefield_source_recorder_value',
    'weight' => 3,
  );
  return $source;
}

/**
 * Implementation of hook_theme().
 */
function filefield_source_recorder_theme() {
  return array(
    'filefield_source_recorder_element' => array(
      'arguments' => array('element' => NULL),
      'file' => 'sources/recorder.inc',
    ),
  );
}

/**
 * A #process callback to extend the filefield_widget element type.
 */
function filefield_source_recorder_process($element, &$form_state, $form) {
  $element['filefield_recorder'] = array(
    '#theme' => 'filefield_source_recorder_element',
    '#weight' => 100.5,
    '#access' => empty($element['fid']['#value']),
    '#description' => t('Record audio using Nanogong audio recorder.'),
  );    

  $element['filefield_recorder']['recorder'] = array(
    '#name' => implode('_', $element['#array_parents']) . '_attach',
    '#type' => 'submit',
    '#value' => t('Upload'),
    '#validate' => array(),
    '#submit' => array('filefield_sources_field_submit'),
    '#limit_validation_errors' => array($element['#parents']),
    '#ajax' => array(
      'path' => 'file/ajax/' . implode('/', $element['#array_parents']) . '/' . $form['form_build_id']['#value'],
      'wrapper' => $element['#id'] . '-ajax-wrapper',
       'method' => 'replace',
       'effect' => 'fade',
    ),
  );
  return $element;
}


/**
 * Theme the output of the audio recorder field.
 */
function theme_filefield_source_recorder_element($variables) {
 $element = $variables['element'];

  $output = audiorecorderfield_recorder_nanogong($element);
  $element['#type'] = 'item';
  
  return '<div class="filefield-source filefield-source-recorder clear-block">' . $output . theme('form_element', array('element'=>$element)) . '</div>';
}