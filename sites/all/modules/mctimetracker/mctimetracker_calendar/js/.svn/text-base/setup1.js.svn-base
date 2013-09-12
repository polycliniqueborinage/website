jQuery(document).ready(function() {

	var currentcalendarid = null;
	
	var $calendar = jQuery('#calendar');

	jQuery("#dashboard").layout({
		applyDefaultStyles: true,
		west__fxName:			"drop",
		west__size:				290,
		west__minSize:			290
	});
	
   jQuery( "#datepicker" ).datepicker({
	   	onSelect: function(dateText, inst) {
   			$calendar.weekCalendar('gotoWeek', new Date(dateText));
   		}	   
   });

   jQuery( "#mycalendar .calendar" ).click(function(){
	    
	   var cid = jQuery(this).attr("id");
	   
	   var selected = jQuery(this).hasClass('selected')
	   if (selected) {
			jQuery(this).removeClass('selected');
	   } else {
			jQuery(this).addClass('selected');
	   }
	    
	   jQuery.ajax({
		   type: "POST",
		   async:false,
	       url: "mctimetracker_calendar/selectcalendar",
	       global: true,
	       data : 'cid='+cid+'&selected='+selected,
	       success: function (data) {
	    			$calendar.weekCalendar('refresh');
	           },
	           complete: function() {
	           },
	           dataType: 'json'
	    });
   }); 
   
   jQuery( "#calendar_edit_container" ).dialog({
       modal: true,
       title: "Add Calendar",
       close: function() {
       },
       buttons: {
          save : function() {
          },
          "delete" : function() {
          },
          cancel : function() {
             $dialogContent.dialog("close");
          }
       }
    }).show();
   
   
   var id = 10;
   
   $calendar.weekCalendar({
	   
      displayOddEven:true,
      
      timeslotsPerHour : 4,
      
      allowCalEventOverlap : true,
      
      overlapEventsSeparate: true,
      
      firstDayOfWeek : 1,
      
      businessHours :{start: 8, end: 18, limitDisplay: false },
      
      daysToShow : 7,
      
      switchDisplay: {'1 day': 1, 'work week': 5, 'full week': 7, 'month': 31},
      
      title: function(daysToShow) {
			return daysToShow == 1 ? '%date%' : '%start% - %end%';
      },
      
      height : function($calendar) {
    	  
         return 550;
         
      },
      
      eventRender : function(calEvent, $event) {
         if (calEvent.end.getTime() < new Date().getTime()) {
            $event.css("backgroundColor", "#aaa");
            $event.find(".wc-time").css({
               "backgroundColor" : calEvent.backgroundColor,
               "border" : "1px solid #888"
            });
         } else {
        	 $event.css("backgroundColor", calEvent.backgroundColor);
        	 $event.find(".wc-time").css({
                 "backgroundColor" : calEvent.backgroundColor,
                 "border" : "1px solid #888"
              });
         }
      },
      
      draggable : function(calEvent, $event) {
    	  
         return calEvent.readOnly != true;
      
      },
      
      resizable : function(calEvent, $event) {
      
    	  return calEvent.readOnly != true;
      
      },
      
      eventNew : function(calEvent, $event) {
    	  
         var $dialogContent = jQuery("#event_edit_container");
         
         var uidField = $dialogContent.find("input[name='uid']");
         var cidField = $dialogContent.find("select[name='cid']").val(calEvent.cid);
         var startField = $dialogContent.find("select[name='start']").val(calEvent.start);
         var endField = $dialogContent.find("select[name='end']").val(calEvent.end);
         var iidField = $dialogContent.find("input[name='iid']").val(calEvent.iid);;
         
         $dialogContent.dialog({
            modal: true,
            title: "New Calendar Event",
            close: function() {
               $dialogContent.dialog("destroy");
               $dialogContent.hide();
               jQuery('#calendar').weekCalendar("removeUnsavedEvents");
            },
            buttons: {
            	
               save : function() {
            	
                  calEvent.uid = uidField.val();
                  calEvent.cid = cidField.val();
                  calEvent.start = new Date(startField.val()).getTime()/1000;
                  calEvent.end = new Date(endField.val()).getTime()/1000;
                  calEvent.title = 'title';
                  calEvent.body = 'body';
                  
                  jQuery.ajax({
                      type: "POST",
                      url: "mctimetracker_calendar/addevent",
                      data: calEvent,
                      global: true,
                      success: function (data) {
                	  		$calendar.weekCalendar('refresh');
                      },
                      complete: function() {
                      },
                      dataType: 'json'
                  });

                  $dialogContent.dialog("close");
               },
               cancel : function() {
                  $dialogContent.dialog("close");
               }
            }
         }).show();
         
         $dialogContent.find(".date_holder").text($calendar.weekCalendar("formatDate", calEvent.start));
         
         setupStartAndEndTimeFields(startField, endField, calEvent, $calendar.weekCalendar("getTimeslotTimes", calEvent.start));

      },
     
      eventDrop : function(calEvent, $event) {
    	  
          calEvent.start = calEvent.start.getTime()/1000;
          calEvent.end = calEvent.end.getTime()/1000;
    	  
    	  jQuery.ajax({
              type: "POST",
              url: "mctimetracker_calendar/updateevent",
              data: calEvent,
              global: true,
              success: function (data) {
        	  		$calendar.weekCalendar('refresh');
              },
              complete: function() {
              },
              dataType: 'json'
          });
    	  
      },
      
      eventResize : function(calEvent, $event) {
    	  
          calEvent.start = calEvent.start.getTime()/1000;
          calEvent.end = calEvent.end.getTime()/1000;
    	  
    	  jQuery.ajax({
              type: "POST",
              url: "mctimetracker_calendar/updateevent",
              data: calEvent,
              global: true,
              success: function (data) {
        	  		$calendar.weekCalendar('refresh');
              },
              complete: function() {
              },
              dataType: 'json'
          });
    	  
      },
      
      eventClick : function(calEvent, $event) {

         if (calEvent.readOnly) {
            return;
         }

         var $dialogContent = jQuery("#event_edit_container");
         
         var startField = $dialogContent.find("select[name='start']").val(calEvent.start);
         var endField = $dialogContent.find("select[name='end']").val(calEvent.end);
         var cidField = $dialogContent.find("select[name='cid']").val(calEvent.end);
         var titleField = $dialogContent.find("input[name='title']").val(calEvent.title);
         var bodyField = $dialogContent.find("textarea[name='body']");
         
         $dialogContent.dialog({
            modal: true,
            title: "Edit - " + calEvent.title,
            close: function() {
               $dialogContent.dialog("destroy");
               $dialogContent.hide();
               jQuery('#calendar').weekCalendar("removeUnsavedEvents");
            },
            buttons: {
               save : function() {

                  calEvent.start = new Date(startField.val());
                  calEvent.end = new Date(endField.val());
                  calEvent.cid = new Date(cidField.val());
                  calEvent.title = titleField.val();
                  calEvent.body = bodyField.val();

                  $calendar.weekCalendar("updateEvent", calEvent);
                  $dialogContent.dialog("close");
               },
               "delete" : function() {
                  $calendar.weekCalendar("removeEvent", calEvent.id);
                  $dialogContent.dialog("close");
               },
               cancel : function() {
                  $dialogContent.dialog("close");
               }
            }
         }).show();

         var startField = $dialogContent.find("select[name='start']").val(calEvent.start);
         var endField = $dialogContent.find("select[name='end']").val(calEvent.end);
         $dialogContent.find(".date_holder").text($calendar.weekCalendar("formatDate", calEvent.start));
         setupStartAndEndTimeFields(startField, endField, calEvent, $calendar.weekCalendar("getTimeslotTimes", calEvent.start));
         jQuery(window).resize().resize(); //fixes a bug in modal overlay size ??

      },
      eventMouseover : function(calEvent, $event) {
      },
      eventMouseout : function(calEvent, $event) {
      },
      noEvents : function() {

      },
      
      data : function(start, end, callback) {
          callback(getEventData());
       }
      
     /* data:"mctimetracker_calendar/getevent"*/
       
   
   });

   function resetForm($dialogContent) {
      $dialogContent.find("input").val("");
      $dialogContent.find("textarea").val("");
   }

   function getEventData() {
	   
	   var events = null;
	   var events = null;
	   var events = null;
	   
	   jQuery.ajax({
           type: "POST",
           async:false,
           url: "mctimetracker_calendar/getevent1",
           global: true,
           success: function (data) {
		     events =  data;
           },
           complete: function() {
           },
           dataType: 'json'
       });
	   
	   return events;
   }


   /*
    * Sets up the start and end time fields in the calendar event
    * form for editing based on the calendar event being edited
    */
   function setupStartAndEndTimeFields($startTimeField, $endTimeField, calEvent, timeslotTimes) {

      $startTimeField.empty();
      $endTimeField.empty();

      for (var i = 0; i < timeslotTimes.length; i++) {
         var startTime = timeslotTimes[i].start;
         var endTime = timeslotTimes[i].end;
         var startSelected = "";
         if (startTime.getTime() === calEvent.start.getTime()) {
            startSelected = "selected=\"selected\"";
         }
         var endSelected = "";
         if (endTime.getTime() === calEvent.end.getTime()) {
            endSelected = "selected=\"selected\"";
         }
         $startTimeField.append("<option value=\"" + startTime + "\" " + startSelected + ">" + timeslotTimes[i].startFormatted + "</option>");
         $endTimeField.append("<option value=\"" + endTime + "\" " + endSelected + ">" + timeslotTimes[i].endFormatted + "</option>");

         $timestampsOfOptions.start[timeslotTimes[i].startFormatted] = startTime.getTime();
         $timestampsOfOptions.end[timeslotTimes[i].endFormatted] = endTime.getTime();

      }
      $endTimeOptions = $endTimeField.find("option");
      $startTimeField.trigger("change");
   }

   var $endTimeField = jQuery("select[name='end']");
   var $endTimeOptions = $endTimeField.find("option");
   var $timestampsOfOptions = {start:[],end:[]};

   //reduces the end time options to be only after the start time options.
   jQuery("select[name='start']").change(function() {
      var startTime = $timestampsOfOptions.start[jQuery(this).find(":selected").text()];
      var currentEndTime = $endTimeField.find("option:selected").val();
      $endTimeField.html(
            $endTimeOptions.filter(function() {
               return startTime < $timestampsOfOptions.end[jQuery(this).text()];
            })
            );

      var endTimeSelected = false;
      $endTimeField.find("option").each(function() {
         if (jQuery(this).val() === currentEndTime) {
            jQuery(this).attr("selected", "selected");
            endTimeSelected = true;
            return false;
         }
      });

      if (!endTimeSelected) {
         //automatically select an end date 2 slots away.
         $endTimeField.find("option:eq(1)").attr("selected", "selected");
      }

   });

});
