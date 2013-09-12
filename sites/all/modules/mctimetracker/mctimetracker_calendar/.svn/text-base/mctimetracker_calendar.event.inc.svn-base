<?php
// $Id: mc_usermail_check.unique.inc,v 1.5 2009/09/24 12:57:59 duke Exp $
function mctimetracker_calendar_get_event1() {
	
	$start = $_REQUEST["start"]; 
	$end = $_REQUEST['end']; 
	
	$options['timeslotsPerHour'] = 4;
	$options['timeslotHeight'] = 20;
	
  	$select = db_select('mctimetracker_calendar', 'mctc');
  	$select->join('mctimetracker_calendar_event', 'mctce', 'mctce.cid = mctc.cid');
  	$select->leftJoin('mctimetracker_calendar_selection', 'mctcs', 'mctcs.cid = mctc.cid');
  	$select->fields(array('id','nom','prenom'));
  	$select->fields('mctce',array('eid','start','end','title','body'));
  	$select->fields('mctcs',array('sid'));
  	$select->condition('mctcs.sid', 0, '>');
  	
  	$entries = $select->execute()->fetchAll();
  	
  	//dpm($entries);
  	
  	$events = array();

  	if (!empty($entries)) {
  		
    	foreach ($entries as $entry) {
    		
    		$event['id'] 	= $entry->eid;
			$event['start'] = date('c',$entry->start);
			$event['end'] 	= date('c',$entry->end);
    		$event['title'] = 'lunch with Mike';
    		$event['body'] = 'lunch with Mike';
    		$event['readOnly'] = false;
    		$event['color'] = '#FFFFFF';
    		$event['backgroundColor'] = '#FF0000';
    		
			$events[] = $event;
    	}
   	}
  
	$responses['options'] = $options;
	$responses['events'] = $events;
   		
	echo drupal_json_encode($responses);
  
}


function mctimetracker_calendar_get_event2() {
	
	$start = $_REQUEST["start"]; 
	$end = $_REQUEST['end']; 
	
  	$select = db_select('mctimetracker_calendar', 'mctc');
  	$select->join('mctimetracker_calendar_event', 'mctce', 'mctce.cid = mctc.cid');
  	$select->leftJoin('mctimetracker_calendar_selection', 'mctcs', 'mctcs.cid = mctc.cid');
  	$select->fields('mctc',array('cid','name'));
  	$select->fields('mctce',array('eid','start','end','title','body'));
  	$select->fields('mctcs',array('sid'));
  	$select->condition('mctcs.sid', 0, '>');
  	$select->condition('mctce.start', $start, '>');
  	$select->condition('mctce.end', $end, '<');
  	
  	$entries = $select->execute()->fetchAll();
  	
  	//dpm($entries);
  	
  	$events = array();

  	if (!empty($entries)) {
  		
    	foreach ($entries as $entry) {
    		
    		$event['id'] 	= $entry->eid;
			$event['start'] = $entry->start;
			$event['end'] 	= $entry->end;
    		$event['title'] = 'lunch with Mike';
    		$event['body'] = 'lunch with Mike';
    		$event['allDay'] = false;
    		
    		$event['editable'] = false;
    		$event['color'] = '#FFFFFF';
    		$event['backgroundColor'] = '00000';
    		//$event['textColor'] = false;
    		//$event['borderColor'] = false;
    		//$event['className'] = false;
    		
    		
			$events[] = $event;
    	}
   	}
  
	echo drupal_json_encode($events);
	
	
  
}


function mctimetracker_calendar_add_event() {
	
	$entry['uid'] 	= (int)$_REQUEST["uid"]; 
	$entry['cid'] 	= (int)$_REQUEST["cid"]; 
	$entry['start'] = (int)$_REQUEST["start"]; 
	$entry['end'] 	= (int)$_REQUEST["end"]; 
	$entry['title']	= $_REQUEST["title"]; 
	$entry['body'] 	= $_REQUEST["body"]; 
	
	$return_value = db_insert('mctimetracker_calendar_event')->fields($entry)->execute();
                    
	$responses = array();

	$entry['eid'] 	= $return_value;
	
	echo drupal_json_encode($entry);
  
  	exit();
	
}

function mctimetracker_calendar_update_event() {
	
	$eid = $_REQUEST["id"]; 
	
	$entry['start'] = (int)$_REQUEST["start"]; 
	$entry['end'] 	= (int)$_REQUEST["end"]; 
	
	$return_value = db_update('mctimetracker_calendar_event')
              ->fields($entry)
              ->condition('eid', $eid)
              ->execute();

    echo drupal_json_encode($entry);
  
  	exit();
              
}


function mctimetracker_calendar_delete_event() {
	
	$start = $_REQUEST["start"]; 
	$end = $_REQUEST['end']; 
	$title = $_REQUEST["title"]; 
	$body = $_REQUEST['body']; 
	
	db_insert('pm_index')
        ->fields(array(
          'start' 		=> $start,
          'end' 		=> $end,
          'title' 		=> $title,
          'body' 		=> $body
        ))
        ->execute();

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

