<?php

function mctimetracker_calendar($callback_arg = '',$cid = '') {
	
	$op = isset($_POST['op']) ? $_POST['op'] : $callback_arg;
	
	switch ($op) {
  	
		case 'list':
			
			$build = drupal_get_form('mctimetracker_calendar_list');
			
			break;
    
		case 'add':

			$build = drupal_get_form('mctimetracker_calendar_form_add');
			
			break;
			
		case 'update':

			$build = drupal_get_form('mctimetracker_calendar_form_add',$cid);
			
			break;
		
		default :
		
  			$build = 'default';
	}
	
  	return $build;

}


/**
 * Render a list of entries in the database.
 */
function mctimetracker_calendar_list() {
	
	global $user;
	
	$entry['mctc.uid'] = $user->uid;
	
	$entries = mctimetracker_calendar_entry_load($entry);
	
	$header = array(
		'name' => array('data' => t('Name'), 'field' => 'mctc.name'),
	    'selection' => array('data' => t('Selection'), 'field' => 'mctcs.sid'),
	    'locked' => array('data' => t('Lock'), 'field' => 'mctc.locked'),
	);
    	
    $rows = array();
    	
    foreach ($entries as $entry) {
    	
    	$row['name'] 				= l($entry->name, 'mctimetracker_calendar/update/'.$entry->cid);
    	$row['selection'] 			= ($entry->sid) ? t('Selected') : t('Not Selected');
    	$row['locked'] 				= ($entry->locked) ? t('Locked') : t('Not Locked');

    	// Sanitize the data before handing it off to the theme layer.
      	$rows[] = $row;
    	
    }
    	
	$build['mctimetracker_calendar_table'] = array(
    	'#theme' => 'table',
    	'#header' => $header,
    	'#rows' => $rows,
    	'#empty' => t('No calender available.'),
  	);
  
  	$build['mctimetracker_calendar_pager'] = array('#theme' => 'pager');
  	
  	return $build;

}


function mctimetracker_calendar_form_add($form, &$form_state) {
  
	$form = array();

  	$form['add'] = array(
    	'#type'  => 'fieldset',
    	'#title' => t('Add a new calendar'),
  	);
  	$form['add']['name'] = array(
    	'#type'  => 'textfield',
    	'#title' => t('Name'),
    	'#size'  => 15,
	  	'#required' => TRUE,
  	);
  	
  	$form['add']['description'] = array(
    	'#type'  => 'textarea',
    	'#title' => t('Description'),
  	);
  	
  	$form['submit'] = array(
    	'#type'  => 'submit',
    	'#value' => t('add'),
  	);
  	
	//$form['#validate'][] = 'simplenews_subscription_manager_form_validate';
  	//$form['#submit'][] = 'simplenews_subscription_manager_form_submit';
  	//$form['#redirect'] = ''; //Return to home page after (un)subscribe
  	
  	return $form;

}


function mctimetracker_calendar_form_add_submit($form, &$form_state) {
  
	global $user;

  	// Save the submitted entry.
  	$entry = array(
    	'uid'    		=> $user->uid,
    	'name'    		=> $form_state['values']['name'],
    	'description'   => $form_state['values']['description'],
  		'locked'   		=> 0,
  		'deleted'   	=> 0,
  	);
  	
  	$return = mctimetracker_calendar_entry_insert($entry);
  	
}


function mctimetracker_calendar_form_add_validate($form, &$form_state) {
  
  	
}





























function mctimetracker_calendar_select_calendar() {

	global $user;
	
	$cid 		= $_REQUEST["cid"]; 
	$selected 	= $_REQUEST["selected"]; 
	
	if ($cid) {
		
		if ('true' == $selected) {

			db_delete('mctimetracker_calendar_selection')
  			->condition('uid', $user->uid)
  			->condition('cid', $cid)
  			->execute();
			
		} else {

			db_insert('mctimetracker_calendar_selection')
    	    ->fields(array(
   	       	'uid' 		=> $user->uid,
   	       	'cid' 		=> $cid,
   	     	))
        	->execute();
        	
		}
		
	return true;
	
	}
	
	return false;
    
}



















    
function Strip($value){
    if(get_magic_quotes_gpc() != 0)
      {
        if(is_array($value))  
            if ( array_is_associative($value) )
            {
                foreach( $value as $k=>$v)
                    $tmp_val[$k] = stripslashes($v);
                $value = $tmp_val; 
            }                
            else  
                for($j = 0; $j < sizeof($value); $j++)
                    $value[$j] = stripslashes($value[$j]);
        else
            $value = stripslashes($value);
    }
    return $value;
}

function array_is_associative ($array) {
    if ( is_array($array) && ! empty($array) )
    {
        for ( $iterator = count($array) - 1; $iterator; $iterator-- )
        {
            if ( ! array_key_exists($iterator, $array) ) { return true; }
        }
        return ! array_key_exists(0, $array);
    }
    return false;
}

function constructWhere($searchString){

    $qwery = "";
    //['eq','ne','lt','le','gt','ge','bw','bn','in','ni','ew','en','cn','nc']
    $qopers = array(
                  'eq'=>" = ",
                  'ne'=>" <> ",
                  'lt'=>" < ",
                  'le'=>" <= ",
                  'gt'=>" > ",
                  'ge'=>" >= ",
                  'bw'=>" LIKE ",
                  'bn'=>" NOT LIKE ",
                  'in'=>" IN ",
                  'ni'=>" NOT IN ",
                  'ew'=>" LIKE ",
                  'en'=>" NOT LIKE ",
                  'cn'=>" LIKE " ,
                  'nc'=>" NOT LIKE " );
    
    if ($searchString) {
    
        $searchJson = json_decode($searchString,true);
        
        if(is_array($searchJson)){
        
            // init
            $qwery = " AND ( ";
        
            // AND , OR
            $groupe_operation = $searchJson['groupOp'];
            
            // All the rule
            $rules = $searchJson['rules'];
            
            $i = 0;
            
            foreach($rules as $key=>$val) {
            
                $field         = $val['field'];
                $operation     = $val['op'];
                $value         = $val['data'];
                
                switch ($field) {
				    case 'uid':
				        $field = "u.uid";
				        break;
				    case 'patient':
				        $field = "concat(ct1.field_personal_first_name_value,' ',ct1.field_personal_last_name_value)";
				        break;
				    case 'group':
				        $field = "r.rid";
				        break;
				    case 'status':
				        $field = "(IF( ccu.completeness = 100 && ct2.field_hc_status_value = 'in_progress' ,'completed',ct2.field_hc_status_value))";
				        break;
				    case 'np_approved':
				        $field = "ct3.field_consent_agency_app_value";
				        break;    
                }
                
                if($operation) {
                
                    //$value = ToSql($field,$op,$v);
                    if ($i!=0) $qwery .= " " .$groupe_operation." ";
                    
                    switch ($operation) {
                        // in need other thing
                        case 'in' :
                        case 'ni' :
                            $qwery .= $field.$qopers[$operation]." (".$value.")";
                            break;
                        case 'bw' :
                        case 'bn' :
                            $qwery .= $field.$qopers[$operation]." '".$value."%'";
                            break;
                        case 'ew' :
                        case 'en' :
                            $qwery .= $field.$qopers[$operation]." '%".$value."'";
                            break;
                        case 'cn' :
                        case 'nc' :
                            $qwery .= $field.$qopers[$operation]." '%".$value."%'";
                            break;
                        case 'eq' :
                            if ($value == '') {
                                $qwery .= " (".$field.$qopers[$operation]." '".$value."' OR ".$field." IS NULL".")";
                                
                            } else {
                                $qwery .= $field.$qopers[$operation]." '".$value."'";
                            }
                            break;
                        default:
                            $qwery .= $field.$qopers[$operation]."'".$value."'";
                    }
                    
                    $i++;
                }
                
            }
            
            // end
            $qwery .= " ) ";
        }
    }
    return $qwery;
}


function ToSql ($field, $oper, $val) {
    // we need here more advanced checking using the type of the field - i.e. integer, string, float
    switch ($field) {
        case 'id':
            return intval($val);
            break;
        case 'amount':
        case 'tax':
        case 'total':
            return floatval($val);
            break;
        default :
            //mysql_real_escape_string is better
            if($oper=='bw' || $oper=='bn') return "'" . addslashes($val) . "%'";
            else if ($oper=='ew' || $oper=='en') return "'%" . addcslashes($val) . "'";
            else if ($oper=='cn' || $oper=='nc') return "'%" . addslashes($val) . "%'";
            else return "'" . addslashes($val) . "'";
    }
}

