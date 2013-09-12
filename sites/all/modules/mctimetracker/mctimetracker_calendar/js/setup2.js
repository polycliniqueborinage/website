jQuery(document).ready(function() {

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
   
   
   jQuery('#calendar').fullCalendar({
		
		theme : true,
		
		header: {
			left: 'prev,next today',
			center: 'title',
			right: 'month,week, basicWeek, basicDay, agendaWeek, agendaDay '
		},
		
		firstDay:1,
		
		events: "mctimetracker_calendar/getevent",
		
		loading: function(bool) {
			if (bool) jQuery('#loading').show();
			else jQuery('#loading').hide();
		}
		
	});

});
