<?php
?>

    <script type="text/javascript">

  	jQuery(document).ready(function() {

  	    eventSources = new Array();
		jQuery.each(Drupal.settings.enable_calendars, function(i, item) {
			eventSources.push('/online/calendars/json/' + i);
		});

  		resources = null;

  		var calendar = jQuery('#calendar').fullCalendar({
  	  		
			header: {
				left: 'prev,next today',
				center: 'title',
				right: Drupal.settings.calendar.field_agenda_views
			},

			columnFormat: {
				month: 'ddd',    // Mon
			    week: 'ddd d/M', // Mon 9/7
			    day: 'dddd d/M'  // Monday 9/7
			},

			axisFormat:'H:mm',

			year: Drupal.settings.year,

			month: Drupal.settings.month - 1,

			date: Drupal.settings.date,

			defaultView: Drupal.settings.calendar.field_agenda_default_views,
			firstDay: Drupal.settings.calendar.field_agenda_first_day,
			editable: Drupal.settings.calendar.field_agenda_editable,
			selectable: false,
			slotMinutes : Drupal.settings.calendar.field_agenda_slotminutes,
			minTime: Drupal.settings.calendar.field_agenda_mintime,
			maxTime: Drupal.settings.calendar.field_agenda_maxtime,
			firstHour: Drupal.settings.calendar.field_agenda_firsthour,
			defaultEventMinutes: Drupal.settings.calendar.field_agenda_slotminutes,
			weekends:true,
			selectHelper: true,
	        unselectCancel : '.ui-autocomplete',
			resources: resources,
			eventSources: eventSources,
			
			eventClick: function ( event, jsEvent, view )  {

				jQuery('.ui-tooltip').each(function() {
					jQuery(this).qtip("destroy");
				})
				
				if(event.status == 'free') {

					// Dates
					startstring = jQuery.fullCalendar.formatDate(event.start, 'dd/MM/yyyy HH:mm');
		        	startdatestring = jQuery.fullCalendar.formatDate(event.start, 'yyyyMMdd');
		        	enddatestring = jQuery.fullCalendar.formatDate(event.end, 'yyyyMMdd');
					if (startdatestring == enddatestring) {
						endstring = jQuery.fullCalendar.formatDate(event.end, 'HH:mm');
			        } else {
			        	endstring = jQuery.fullCalendar.formatDate(event.end, 'dd/MM/yyyy HH:mm');
				    }

		        	buildcontent =  jQuery('<div>').addClass('create');
		
		    		buildcontentform = jQuery('<div id="tooltip_form">');
		    		buildcontent.append(buildcontentform);
		    		
		    		buildcontentsearch = jQuery('<div id="tooltip_search">');
		    		buildcontent.append(buildcontentsearch);
		    		
		    		buildcontentform.append('<div><label class="left">When</label><b>' + startstring + ' - ' + endstring + '</b></div>');
		    		buildcontentform.append('<div>YOU WILL RECEIVED AN EMAIL TO CONFIRM YOUR APPOINTMENT</div>');
		    		buildcontentform.append('<div>IF YOU DO NOT CONFIRM IT, YOUR APPOINTMENT WILL BE DELETED IN 30 MINUTES</div>');
		    		buildcontentform.append('<div><label class="left">Email</label><input type="text" id="mail" value="calcus.david@googlemail.com"></div>');
		    		
		    		buildcontentbutton = jQuery('<div id="tooltip_button">');
		    		buildcontent.append(buildcontentbutton);
		    		
		    		buildcontentsave = jQuery("<a/>").addClass('left button').append('Save').bind('click', function(evenement) {

		    			mail = jQuery('#mail').val();

		    			jQuery.ajax({
				  			type: "POST",
				  			data: {
		    				  mail : mail,
			  				  start : event.start.getTime()/1000, 
			  				  end : event.end.getTime()/1000, 
			  				  allday : event.allDay,
		    				  parent_id : event.parent_id,
		    				  calendar_id : event.calendar_id
					  		},
				  			dataTypeString : "json",
				  			url: "/online/calendars/udpate",
				  			success: function(json){
				  				calendar.fullCalendar( 'refetchEvents' );
				  			}
						});
						
						calendar.fullCalendar('unselect');
		    		    
		    		});
		    		
		    		buildcontentbutton.append(buildcontentsave);

		    		jQuery(this).qtip({
		        		content: {
		              		text: buildcontent,
		              		title: {
		        				text: 'Detail', // Give the tooltip a title using each elements text
		        				button: true
		            		}
		        		},
		        		solo: true,
		        		show: { ready: true, when: false },
		        		hide: false,       		
		        		position: {
					    	my: 'bottom center',
					        at: 'top center'
					    },
					    style: {
					          classes: 'ui-autocomplete',
					          widget: false,
					          tip: {
					             corner: true,
					             mimic: false,
					             method: true,
					             width: 9,
					             height: 9,
					             border: 0,
					             offset: 0
					          }
					    },
		        	});

				}
	        	
			},
			
			dayClick: function(date, allDay, jsEvent, view) {

		    },

		    eventRender: function(event, element) {
			},
		    
			height: calcCalendarHeight()
		});
		
		function calcCalendarHeight() {
			var h = jQuery(window).height();
			return h;
		}
		
		jQuery(window).resize(function() {
			jQuery('#calendar').fullCalendar('option', 'height', calcCalendarHeight());
		});

		jQuery('#nav ul li').click(function() {
			if(jQuery(this).hasClass('enable')) {
		    	jQuery.ajax({url: '/online/calendars/disable/' + jQuery(this).attr('id')});
		        jQuery('#calendar').fullCalendar('removeEventSource', '/online/calendars/json/' + jQuery(this).attr('id'));
				jQuery(this).removeClass("enable");
		    } else {
		    	jQuery.ajax({url: '/online/calendars/enable/' + jQuery(this).attr('id')});
		        jQuery('#calendar').fullCalendar('addEventSource', '/online/calendars/json/' + jQuery(this).attr('id'));
		        jQuery(this).addClass("enable");
			}
	    });

		jQuery("#nav ul li").hover(function () {
			jQuery(this).addClass("hover");
		}, function () {
			 jQuery(this).removeClass("hover");
		});
  		
  	});
      
    </script>
    
   	<div id='nav'>
   	
   	  <a class="" href="/account/calendars"><?php echo t('Calendars')?></a>
   	
   	<h3>
   	<?php 
   	  print t('Calendars');
   	?>
   	</h3>
   	<ul>
   		<?php 
   		foreach ($calendars as $calendar) {
   		  echo "<li class='" . $calendar->class . "' id='" . $calendar->nid . "'>";
   		  echo "<div style='background:" . $calendar->field_agenda_color . "' class='square'></div><span>";
   		  echo "<a class='view' href='/online/calendars/" . $calendar->nid . "'>";
   		  echo ($calendar->node_title);
   		  echo "</a>";
   		  echo "</span><a class='popup' href='/node/" . $calendar->nid . "/edit'></a></li>";
   		}
   		?>
   	</ul>
   	</div>
   	<div id='calendar'>
   	</div>
   	