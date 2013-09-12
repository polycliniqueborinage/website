function loadTemplate(type,id) {
	
  // Retrieve a CAPTCHA:
  jQuery.getJSON(Drupal.settings.basePath + 'admin/campaign/templates/json/' + id,
    function (data) {
	  
      if (type == 'sms') {
          jQuery('#edit-body').val(data.body);
          jQuery('#edit-body').smsCounter('#count');
      }
      if (type == 'mail') {
          jQuery('#edit-mail-body').val(data.body);
      }
      
    }
  );
  return false;
}

