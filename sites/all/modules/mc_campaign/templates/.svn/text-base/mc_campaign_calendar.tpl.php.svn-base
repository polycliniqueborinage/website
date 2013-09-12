<?php
?>

    <script type="text/javascript">

    function updatecalendar (calendar_id) {
		jQuery('.fc-select-helper').css('background-color', Drupal.settings.calendars[calendar_id].field_agenda_color);
		jQuery('.fc-select-helper .fc-event-inner').css('background-color', Drupal.settings.calendars[calendar_id].field_agenda_color);
		jQuery('.fc-select-helper .fc-event-head').css('background-color', Drupal.settings.calendars[calendar_id].field_agenda_color);
		jQuery('#slot_minutes').val(Drupal.settings.calendars[calendar_id].field_agenda_slotminutes);
	}

    function updatestatus (status) {
        if (status == 'provisional') {
    		jQuery('#slotsingle').show();
    		jQuery('#slotmupliple').show();
    		jQuery('#usermupliple').show();
    		jQuery('#autocomplete-container').hide();
        } else {
    		jQuery('#slotsingle').hide();
    		jQuery('#slotmupliple').hide();
    		jQuery('#usermupliple').hide();
    		jQuery('#autocomplete-container').show();
        }
    }

    function updateslot (slot) {
    	jQuery('#single').attr('checked', false);
    	jQuery('#multiple').attr('checked', true);
    }
    
  	jQuery(document).ready(function() {

		jQuery('#nav ul li .popup').each(function() {
			
    		buildcontent =  jQuery('<div>').addClass('popup');
    		buildcontentul = jQuery('<ul>');
    		// EDIT
    		buildcontent.append(buildcontentul);
    		buildcontentulli = jQuery('<li>');
    		buildcontentul.append(buildcontentulli);
    		buildcontentullia = jQuery('<a>', {
    		    text: 'Edit',
    		    href: jQuery(this).attr('id'),
    		    click: function(){ jQuery('#nav ul li .popup').qtip('destroy');}
    		});
    		buildcontentulli.append(buildcontentullia);
    		// DISPLAY
    		buildcontentulli = jQuery('<li>');
    		buildcontentul.append(buildcontentulli);
    		buildcontentullia = jQuery('<a>', {
    		    text: 'Display only this calendar',
    		    href: jQuery(this).attr('id')
    		});
    		buildcontentulli.append(buildcontentullia);
    		// ENABLE
    		buildcontentulli = jQuery('<li>');
    		buildcontentul.append(buildcontentulli);
    		buildcontentullia = jQuery('<a>', {
    		    text: 'Enable',
    		    href: jQuery(this).attr('id')
    		});
    		buildcontentulli.append(buildcontentullia);
    		// DISABLE
    		buildcontentulli = jQuery('<li>');
    		buildcontentul.append(buildcontentulli);
    		buildcontentullia = jQuery('<a>', {
    		    text: 'Disable',
    		    href: jQuery(this).attr('id')
    		});
    		buildcontentulli.append(buildcontentullia);

    		jQuery(this).qtip({
        		content: {
              		text: buildcontent,
        		},
        		solo: true,
        		hide: {
        			event: 'unfocus'
        		},
        		show: {
        			event: 'click'
        		},
        		events: {
        			show: function(event, api) {
        			  jQuery(event.originalEvent.target).parent().addClass('hover2');
        			},
    				hide: function(event, api) {
        		      jQuery('#nav ul li.hover2').removeClass('hover2');
          			}
        		},
        		position: {
			    	my: 'top left',
			        at: 'bottom center',
			        viewport: jQuery(window),
			        adjust: {
						x: -7
					}
			    },
			    style: {
			          classes: 'ui-autocomplete',
			          widget: false,
			          tip: {
			             corner: false,
			             mimic: false,
			             method: true,
			             width: 9,
			             height: 9,
			             border: 0,
			             offset: 0
			          }
			    }
        	});
			
		})

  	    eventSources = new Array();
		jQuery.each(Drupal.settings.enable_calendars, function(i, item) {
			//console.log(i);
			eventSources.push('/account/calendars/json/' + i);
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
			selectable: Drupal.settings.calendar.field_agenda_selectable,
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
			
			unselect: function(view, jsEvent) {
				jQuery('div.qtip:visible').qtip('destroy');
			},

			select: function(start, end, allDay, jsEvent, view, resource) {

				// Remove old
				jQuery('.ui-tooltip').each(function() {
					jQuery(this).qtip("destroy");
				})

				// Change color
				jQuery('.fc-select-helper').css('background-color', Drupal.settings.calendar.field_agenda_color);
				jQuery('.fc-select-helper .fc-event-inner').css('background-color', Drupal.settings.calendar.field_agenda_color);
				jQuery('.fc-select-helper .fc-event-head').css('background-color', Drupal.settings.calendar.field_agenda_color);

				// Get all calendars
				options_calendar = '';
				jQuery.each(Drupal.settings.calendars, function(i, item) {
				  if (Drupal.settings.calendar.nid == item.nid) {
					  options_calendar += '<option value="' + item.nid + '" selected="selected">' + item.node_title + '</option>';
				  } else {
					  options_calendar += '<option value="' + item.nid + '">' + item.node_title + '</option>';
				  }
				});
				if (options_calendar == '') return;

				// Get all slots
				options_slotminutes = '';
				slot = new Array(5,10,15,20,25,30,45,60,90,120);
				jQuery.each(slot, function(i, item) {
				  if (Drupal.settings.calendar.field_agenda_slotminutes == item) {
				    options_slotminutes += '<option value="' + item + '" selected="selected">' + item + '</option>';
				  } else {
				    options_slotminutes += '<option value="' + item + '">' + item + '</option>';
			      }
				});

				// Dates
				startstring = jQuery.fullCalendar.formatDate(start, 'dd/MM/yyyy HH:mm');
	        	startdatestring = jQuery.fullCalendar.formatDate(start, 'yyyyMMdd');
	        	enddatestring = jQuery.fullCalendar.formatDate(end, 'yyyyMMdd');
				if (startdatestring == enddatestring) {
					endstring = jQuery.fullCalendar.formatDate(end, 'HH:mm');
		        } else {
		        	endstring = jQuery.fullCalendar.formatDate(end, 'dd/MM/yyyy HH:mm');
			    }
	        	
	        	// BUILD
	    		buildcontent =  jQuery('<div>').addClass('create')
	
	    		buildcontentform = jQuery('<div id="tooltip_form">');
	    		buildcontent.append(buildcontentform);
	    		
	    		buildcontentsearch = jQuery('<div id="tooltip_search">');
	    		buildcontent.append(buildcontentsearch);

	    		buildcontentform.append('<div><label class="left">When</label><b>' + startstring + ' - ' + endstring + '</b></div>');
	    		buildcontentform.append('<div><label class="left">What</label><input type="text" id="comment"></div>');
	    		buildcontentform.append('<div><label class="left">Calendar</label><select id="calendar_id" onchange="updatecalendar(this.value)">' + options_calendar + '</select></div>');
	    		buildcontentform.append('<div><label class="left">Status</label><select id="status" onchange="updatestatus(this.value)"><option value="provisional">Provisional</option><option value="requested">Requested</option><option value="confirmed" selected="selected">Confirmed</option><option value="waiting">Waiting</option><option value="miss">Miss it</option><option value="completed">Completed</option></select></div>');

	    		buildcontentsearch.append('<div id="slotsingle" style="display:none"><input id="single" type="radio" name="slot_type"><label for="single">Offer as a single appointment slot</label></div>')
	    		buildcontentsearch.append('<div id="slotmupliple" style="display:none"><input id="multiple" type="radio" name="slot_type" checked="checked"><label for="multiple">Offer as slots of </label><select id="slot_minutes" onchange="updateslot(this.value)">' + options_slotminutes + '</select> minutes</div>');
	    		buildcontentsearch.append('<div id="usermupliple" style="display:none"><label>Number of people allowed: </label><select id="invit_number"><option value="1">1</option></select></div>');
	    		
	    		buildcontentsearch.append('<div id="autocomplete-container" class="clearfix autocomplete-container"><label>Who</label><div class="form-item form-type-autocomplete-deluxe form-item-who"><div id="edit-values" class="autocomplete-deluxe-values form-wrapper"></div><div class="form-item form-type-textfield form-item-textfield"><input type="text" maxlength="128" size="28" value="" name="textfield" style="" id="who" class="autocomplete-deluxe-form form-autocomplete form-text jquery-once-1-processed ui-corner-left ui-autocomplete-input throbbing ui-autocomplete-loading" autocomplete="off" role="textbox" aria-autocomplete="list" aria-haspopup="true"><span tabindex="-1" title="Show all items" class="ui-button ui-widget ui-state-default ui-button-icon-only ui-corner-right ui-button-icon autocomplete-deluxe-button ui-state-focus" role="button"><span class="ui-button-icon-primary ui-icon ui-icon-triangle-1-s"></span><span class="ui-button-text">&nbsp;</span></span></div><div class="autocomplete-deluxe-value-container"><div class="form-item form-type-textfield form-item-value-field"><input type="text" maxlength="128" size="60" value="" name="value_field" id="edit-value-field" class="autocomplete-deluxe-value-field form-text" style="display: none;"></div></div></div></div>');	    		
	    		
	    		buildcontentbutton = jQuery('<div id="tooltip_button">');
	    		buildcontent.append(buildcontentbutton);
	    		
	    		buildcontentsave = jQuery("<a/>").addClass('left button').append('Save').bind('click', function(evenement){

		    		comment = jQuery('#comment').val();
		    		calendar_id = jQuery('#calendar_id').val();
		    		status = jQuery('#status').val();
		    		room_id = jQuery('#room_id').val();
		    		color = jQuery('#color').val();
		    		users = jQuery('#edit-value-field').val();
		    		slot_type = jQuery("input[@name=slot_type]:checked").attr('id');
		    		slot_minutes = jQuery('#slot_minutes').val();
		    		invit_number = jQuery('#invit_number').val();
		    		
	    			jQuery.ajax({
			  			type: "POST",
		      			data: {
			  				start : start.getTime()/1000, 
			  				end : end.getTime()/1000, 
			  				allday : allDay,
			  				color : color,
			  				comment : comment, 
			  				calendar_id : calendar_id,
			  				status : status,
			  				users : users,
			  				room_id : room_id,
			  				slot_type : slot_type,
			  				slot_minutes : slot_minutes,
			  				invit_number : invit_number,
			  			},
			  			dataTypeString : "json",
			  			url: "/account/calendars/add",
			  			success: function(json){
				  			console.log(calendar_id);
				  			console.log(Drupal.settings.enable_calendars);
				  			console.log(Drupal.settings.enable_calendars[calendar_id]);
				  			if (!Drupal.settings.enable_calendars[calendar_id]) {
				  				Drupal.settings.enable_calendars[calendar_id] = true;
						    	jQuery.ajax({url: '/account/calendars/enable/' + calendar_id});
						        jQuery('#calendar').fullCalendar('addEventSource', '/account/calendars/json/' + calendar_id);
						        jQuery('#' + calendar_id).children('.square').addClass("enable");
						        jQuery('#' + calendar_id).children('.square').removeClass("disable");
					  		}
			  				calendar.fullCalendar( 'refetchEvents' );
			  			}
					});
					
					calendar.fullCalendar('unselect');
	    		    
	    		});
	    		buildcontentbutton.append(buildcontentsave);
	
	    		buildcontentcancel = jQuery("<a/>").addClass('right button').append('Cancel').bind('click', function(event){ 
	    			calendar.fullCalendar( 'unselect' );
	    		});
	    		buildcontentbutton.append(buildcontentcancel);

	    		// TODO improve
	    		if (jQuery('.fc-cell-overlay:visible').length) helper = jQuery('.fc-cell-overlay:visible');
	    		if (jQuery('.fc-select-helper').length) helper = jQuery('.fc-select-helper');

	    		helper.qtip({
	        		content: {
	              		text: buildcontent,
	              		title: {
	        				text: 'Add', // Give the tooltip a title using each elements text
	        				button: true
	            		}
	        		},
	        		solo: true,
	        		show: { ready: true, when: false },
	        		hide: false,       		
	        		position: {
				    	my: 'bottom center',
				        at: 'top center',
				        viewport: jQuery(window)
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
				    events: {
				        show: function(event, api) {
				    		// Add autocomplete.
				    		var settings = new Object();
				    		settings.input_id = "who";
				    		settings.min_length = 0;
				    		settings.multiple = true;
				    		settings.type = "ajax";
				    		settings.uri = "/account/reminders/autocomplete";
				    		new Drupal.autocomplete_deluxe(jQuery('#who'), settings);
				        }
				    }
	        	});
				
				
			},
			eventDrop: function( event, dayDelta, minuteDelta, allDay, revertFunc, jsEvent, ui, view ) { 
				if (event.end == null) {
					eventtime = event.start.getTime();
					eventtime = eventtime + 1800000;
					event.end = new Date(eventtime);
				};
				jQuery.ajax({
		  			type: "POST",
		  			data: {
	  					start : event.start.getTime()/1000, 
	  					end : event.end.getTime()/1000, 
	  					allday : event.allDay
		  			},
		  			dataTypeString : "json",
		  			url: "/account/calendars/udpate/" + event.id,
		  			success: function(json){
		  			}
				});
			},
			eventResize: function( event, dayDelta, minuteDelta, revertFunc, jsEvent, ui, view ) { 
				jQuery.ajax({
		  			type: "POST",
		  			data: {
		  				start : event.start.getTime()/1000, 
		  				end : event.end.getTime()/1000, 
		  				allday : event.allDay
		  			},
		  			dataTypeString : "json",
		  			url: "/account/calendars/udpate/" + event.id,
		  			success: function(json){
		  			}
				});
			},
			eventClick: function ( event, jsEvent, view )  {

				jQuery('.ui-tooltip').each(function() {
					jQuery(this).qtip("destroy");
				})

				// Create tip
				options = '';
				// get all calendars
				jQuery.each(Drupal.settings.calendars, function(i, item) {
				  if (event.calendar_id == item.nid) {
					  options += '<option value="' + item.nid + '" selected="selected">' + item.node_title + '</option>';
				  } else {
					  options += '<option value="' + item.nid + '">' + item.node_title + '</option>';
				  }
				});
				
				// get all status
				options_status = '';
				statusA = new Array();
				statusA["provisional"] = 'Provisional';
				statusA["requested"] = 'Requested';
				statusA["confirmed"] = 'Confirmed';
				statusA["waiting"] = 'Waiting';
				statusA["miss"] = 'Miss it';
				statusA["completed"] = 'Completed';
				for(key in statusA) {
					  if (event.status == key) {
						  options_status += '<option value="' + key + '" selected="selected">' + statusA[key] + '</option>';
					  } else if (key != 'provisional') {
						  options_status += '<option value="' + key + '">' + statusA[key] + '</option>';
				      }
				}

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
	    		
	    		//buildcontentsearch = jQuery('<div id="tooltip_search">');
	    		//buildcontent.append(buildcontentsearch);
	    		
	    		buildcontentform.append('<div><label class="left">When</label><b>' + startstring + ' - ' + endstring + '</b></div>');
	    		buildcontentform.append('<div><label class="left">What</label><input type="text" id="comment"></div>');
	    		buildcontentform.append('<div><label class="left">Calendar</label><select id="calendar_id" onchange="updatecalendar(this.value)">' + options + '</select></div>');
	    		buildcontentform.append('<div><label class="left">Status</label><select id="status" onchange="updatestatus(this.value)">' + options_status+ '</select></div>');
	    		
	    		buildcontentbutton = jQuery('<div id="tooltip_button">');
	    		buildcontent.append(buildcontentbutton);
	    		
	    		buildcontentdelete = jQuery("<a/>").addClass('right button').append('Delete').bind('click', function(evenement){ 

	    		    jQuery.ajax({
			  			type: "POST",
			  			data: {id : event.id},
			  			dataTypeString : "json",
			  			url: "/account/calendars/delete/" + event.id,
			  			success: function(json){
			  				calendar.fullCalendar("removeEvents", event.id);
			  			}
					});
					
					calendar.fullCalendar('unselect');
	    		    
	    		});
	    		buildcontentbutton.append(buildcontentdelete);

	    		buildcontentsave = jQuery("<a/>").addClass('left button').append('Save').bind('click', function(evenement) {

		    		calendar_id = jQuery('#calendar_id').val();
		    		status = jQuery('#status').val();
		    		
	    			jQuery.ajax({
			  			type: "POST",
			  			data: {
			  			  calendar_id : calendar_id,
			  			status : status,
				  		},
			  			dataTypeString : "json",
			  			url: "/account/calendars/udpate/" + event.id,
			  			success: function(json){
			  				calendar.fullCalendar( 'refetchEvents' );
			  			}
					});
					
					calendar.fullCalendar('unselect');
	    		    
	    		});
	    		
	    		buildcontentedit = jQuery("<a href='/node/" + event.id + "/edit' />").addClass('left button').append('Edit').bind('click', function(evenement){ 
					jQuery('div.qtip:visible').qtip('destroy');
	    		});;
	    		
	    		buildcontentbutton.append(buildcontentsave);
	    		buildcontentbutton.append(buildcontentedit);

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
				        at: 'top center',
				        viewport: jQuery(window)
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
	        	
			},
			
			dayClick: function(date, allDay, jsEvent, view) {

		    },

		    eventRender: function(event, element) {
		        element.bind('dblclick', function() {
		        	window.location = '#overlay=node/' + event.id + '/edit';
		        });
		     },
		    
			height: calcCalendarHeight()
		});
		
		function calcCalendarHeight() {
			var h = jQuery(window).height() - 300;
			return h;
		}
		
		jQuery(window).resize(function() {
			jQuery('#calendar').fullCalendar('option', 'height', calcCalendarHeight());
		});

		jQuery('#nav ul li .square').click(function() {
			if(jQuery(this).hasClass('enable')) {
		    	jQuery.ajax({url: '/account/calendars/disable/' + jQuery(this).parent().attr('id')});
		        jQuery('#calendar').fullCalendar('removeEventSource', '/account/calendars/json/' + jQuery(this).parent().attr('id'));
		        jQuery(this).addClass("disable");
				jQuery(this).removeClass("enable");
		    } else {
		    	jQuery.ajax({url: '/account/calendars/enable/' + jQuery(this).parent().attr('id')});
		        jQuery('#calendar').fullCalendar('addEventSource', '/account/calendars/json/' + jQuery(this).parent().attr('id'));
		        jQuery(this).addClass("enable");
				jQuery(this).removeClass("disable");
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
   	
   	  <a class="" href="/online/calendars"><?php echo t('Online Booking')?></a>
   	
   	  <a class="button_left" href="/node/add/event"><?php echo t('Create Event')?></a>
   	
   	  <h3>
   	  <?php 
   	    print t('Calendars');
   	    echo "<a class='popup' href='/node/add/agenda'></a></li>";
   	  ?>
   	  </h3>
   	  
   	  <ul>
   		<?php 
   		foreach ($calendars as $calendar) {
   		  echo "<li id='" . $calendar->nid . "'>";
   		  echo "<div style='background-color:" . $calendar->field_agenda_color . "' class='square " . $calendar->class . "'></div><span>";
   		  echo "<a class='view' href='/account/calendars/" . $calendar->nid . "'>";
   		  echo ($calendar->node_title);
   		  echo "</a>";
   		  echo "</span><a class='popup' href='#' id='/node/" . $calendar->nid . "/edit'></a></li>";
   		}
   		?>
   	</ul>
   	
		<h3>
		<?php
		print t('Extrene Calendars');
		echo "<a class='popup' href='/account/calendars/externe'></a></li>";
		?>
		</h3>
   	  
		<ul>
		</ul>   	
   	
	</div>
   	
   	
	<div id='calendar'>
	</div>
