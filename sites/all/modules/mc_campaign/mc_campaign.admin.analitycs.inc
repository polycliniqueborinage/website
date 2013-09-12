<?php


/**
 * Form builder; Campaign Dashboard
 *
 */
function campaign_admin_analitycs($method) {
	
  	$title = t('Analitycs Mail');
  
	drupal_add_css(drupal_get_path('module', 'campaign') . '/css/jquery.jqplot.min.css');
	
	drupal_add_js(drupal_get_path('module', 'campaign') . '/js/jquery.jqplot.min.js');
	
	drupal_add_js(drupal_get_path('module', 'campaign') . '/js/plugins/jqplot.barRenderer.js');
	drupal_add_js(drupal_get_path('module', 'campaign') . '/js/jqplot.categoryAxisRenderer.js');
  	
  	$build['campaign_analitycs'] = array(
    	'#theme' => 'campaign_analitycs',
    	'#title' => $title,
    	'#method' => $method,
  	);

  	$build['campaign_analitycs']['#attached']['js'][] = array('data' => '(function($){$(function() { 

		$(document).ready( function() {
		
			$.jqplot("chartdiv",  [[[1, 2],[3,5.12],[5,13.1],[7,33.6],[9,85.9],[11,219.9]]]);
	

		});

  	})})(jQuery);', 'type' => 'inline');
  
  	$output = drupal_render($build);
  
  	return $output;
  
}