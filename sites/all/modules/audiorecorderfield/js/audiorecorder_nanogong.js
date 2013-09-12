//TODO: pass langcode and delta to preview func
function submitVoice(field_name, langcode, delta) { 
	var field_id = field_name+"-"+langcode+"-"+delta;
    
    //Remove any error messages
	jQuery("#edit-field-"+field_name+"-"+langcode+"-"+delta+"-ajax-wrapper .error").remove();
	//Disable the button
	jQuery("#nanogong-button-"+field_name+"-"+langcode+"-"+delta).attr('disabled', 'disabled');	
	
	if(jQuery("#nanogong-button-"+field_name+"-"+langcode+"-"+delta).val()=='Upload'){
	//Upload button
		
		// Find the applet object 
		var applet = jQuery("#nanogong-"+field_name+"-"+langcode+"-"+delta).get(0); 
		// Tell the applet to post the voice recording to the backend PHP code 
		var path=Drupal.settings.basePath +"?q=nanogong_file_receive";

		var fid = applet.sendGongRequest("PostToForm", path, "voicefile", "", "temp"); 
        
		if (fid == null || fid == ""){
		jQuery("#nanogong-button-"+field_name+"-"+langcode+"-"+delta).removeAttr("disabled");
        jQuery("#edit-field-"+field_name+"-"+langcode+"-"+delta+"-ajax-wrapper .form-managed-file").prepend('<div class="messages error file-upload-js-error">Failed to submit the voice recording!</div>');

		} 
		else{
			jQuery.ajax({
					type: "GET",
					url: Drupal.settings.basePath +"?q=nanogong_preview",
					data: "fid=" + fid+"&field_id=" + field_id,
					dataType: 'json',
					success: function (data) {
                        NanogongPreview(data, field_id);
                    }
				});
                
			jQuery('input[name="field_'+field_name+'['+langcode+']['+delta+'][fid]"]').val(fid);
		} 
	}
	else{
		//Remove button
		var fid =jQuery("#edit-field-"+field_name+"-"+langcode+"-"+delta+"-fid").val();
		
	jQuery("#edit-field-"+field_name+"-"+langcode+"-"+delta+"-wrapper .filefield-file-info").remove();
	jQuery("#edit-field-"+field_name+"-"+langcode+"-"+delta+"-fid").val("");
	jQuery("#nanogong-button-"+field_name+"-"+langcode+"-"+delta).removeAttr("disabled");
	jQuery("#nanogong-button-"+field_name+"-"+langcode+"-"+delta).val('Upload');
	//Remove and reload the applet
	jQuery("#nanogong-"+field_name+"-"+langcode+"-"+delta+"-wrapper").remove();
	jQuery("#nanogong-button-"+field_name+"-"+langcode+"-"+delta).before('<div class="nanogong-recorder" id="nanogong-'+field_name+"-"+langcode+"-"+delta+'-wrapper"><applet id="nanogong-'+field_name+"-"+langcode+"-"+delta+'" archive="'+Drupal.settings.basePath+Drupal.settings.audiorecorderfield_path+'/recorders/nanogong.jar" code="gong.NanoGong" width="113" height="40"><param name="ShowSaveButton" value="false" /><param name="ShowSpeedButton" value="false" /><param name="MaxDuration" value="1200" /><param name="ShowTime" value="true" /></applet></div>');		
	}
} 

function NanogongPreview(data, field_id){
	//Remove Upload button and add Remove button
	jQuery("#nanogong-button-"+field_id).removeAttr("disabled");
	jQuery("#nanogong-button-"+field_id).val('Remove');
	jQuery("#nanogong-"+field_id+"-wrapper").remove();
	jQuery("#nanogong-button-"+field_id).before(data.nanogong);
}
  
  

