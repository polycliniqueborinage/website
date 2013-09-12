<?php

/**
 * @file
 * This file is empty by default because the base theme chain (Alpha & Omega) provides
 * all the basic functionality. However, in case you wish to customize the output that Drupal
 * generates through Alpha & Omega this file is a good place to do so.
 * 
 * Alpha comes with a neat solution for keeping this file as clean as possible while the code
 * for your subtheme grows. Please read the README.txt in the /preprocess and /process subfolders
 * for more information on this topic.
 */


function poly_breadcrumb($variables) {
  
  if (count($variables['breadcrumb']) > 0) {
    $txt = "<ul class='breadcrumb-inner'>";
    foreach ($variables['breadcrumb'] as $breadcrumb) {
      $txt .= "<li>" . $breadcrumb . "</li>";
    }
    $txt .= "</ul>";
    return $txt;
  } else {
    return '';
  }
}


function poly_menu_tree__menu_polyclinique($variables) {
  
  $menu_tree = menu_build_tree('menu-polyclinique');
  
  $menu  = "<nav class='navigation'>";

  $menu .= "<ul id='menu'>";
  
  foreach ($menu_tree as $level1) {
    
    $menu .= "<li class='menu_right'><a href='/" . $level1['link']['link_path'] . "' class='drop'>" . $level1['link']['link_title'] . "</a>";

    if (!empty($level1['below'])) {
      
      $class = implode(' ',$level1['link']['localized_options']['attributes']['class']);
      
      $colum = substr($level1['link']['localized_options']['attributes']['class'][0], -1);
      
      $number = sizeof($level1['below']);
      
      $menu .= "<div class='$class'>";
      
      for($i=0; $i < $colum; $i++) {

        if ($i == ($colum - 1))
        $menu .= "<div class='last col_1'>";
        else 
        $menu .= "<div class='col_1'>";
        
        $menu .= "<ul class='simple'>";
        
        for($j = $i; $j < $number; $j+=$colum) {
          $level2 = current(array_slice($level1['below'], $j, 1)); 
          $menu .= "<li><a href='/" . $level2['link']['link_path'] . "'>" . $level2['link']['link_title'] . "</a>";
        }
        
        $menu .= "</ul>";
          
        $menu .= "</div>";
      
      }
      
      $menu .= "</div>";
      
    }
    
    $menu .= "</li>";
    
  }
  
  $menu .= "</ul>";
  
  $menu .= "</nav>";
  
  return $menu;

}


/**
* Add unique class (mlid) to all menu items.
*/
function poly_menu_link(array $variables) {
  
  $element = $variables['element'];
  $sub_menu = '';

  $element['#attributes']['class'][] = 'menu-' . $element['#original_link']['mlid'];
  $element['#attributes']['class'][] = 'menu_right';
  
  if ($element['#below']) {
    $sub_menu = drupal_render($element['#below']);
  }
  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}