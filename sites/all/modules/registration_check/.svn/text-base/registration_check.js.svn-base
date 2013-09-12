
(function ($) {

Drupal.behaviors.usernameCheck = {
  attach: function (context) {
    $('#username-check-informer:not(.username-check-processed)', context)
    .each(function() {
      var input = $(this).parents('.form-item').find('input');
      
      Drupal.usernameCheck.username = '';
      input
      .keyup(function () {
        if(input.val() != Drupal.usernameCheck.username) {
          clearTimeout(Drupal.usernameCheck.timer);
          Drupal.usernameCheck.timer = setTimeout(function () {Drupal.usernameCheck.check(input)}, parseFloat(Drupal.settings.usernameCheck.delay)*1000);
        
          if(!$("#username-check-informer").hasClass('username-check-informer-progress')) {
            $("#username-check-informer")
              .removeClass('username-check-informer-accepted')
              .removeClass('username-check-informer-rejected');
          }
            
          $("#username-check-message")
            .hide();
        }
      })
      .blur(function () {
        if(input.val() != Drupal.usernameCheck.username) {
          Drupal.usernameCheck.check(input);
        }
      });    
    })
    .addClass('username-check-processed'); 
  }
};

Drupal.usernameCheck = {};
Drupal.usernameCheck.check = function(input) {
  clearTimeout(Drupal.usernameCheck.timer);
  Drupal.usernameCheck.username = input.val();
  
  $.ajax({
    url: Drupal.settings.usernameCheck.ajaxUrl,
    data: {username: Drupal.usernameCheck.username},
    dataType: 'json',
    beforeSend: function() {
      $("#username-check-informer")
        .removeClass('username-check-informer-accepted')
        .removeClass('username-check-informer-rejected')
        .addClass('username-check-informer-progress');
    },
    success: function(ret){
      if(ret['allowed']){
        $("#username-check-informer")
          .removeClass('username-check-informer-progress')
          .addClass('username-check-informer-accepted');
        
        input
          .removeClass('error');
      }
      else {
        $("#username-check-informer")
          .removeClass('username-check-informer-progress')
          .addClass('username-check-informer-rejected');
        
        $("#username-check-message")
          .addClass('username-check-message-rejected')
          .html(ret['msg'])
          .show();
      }
    }
   });
  
  
  Drupal.behaviors.mailCheck = {
		  attach: function (context) {
		    $('#mail-check-informer:not(.mail-check-processed)', context)
		    .each(function() {
		      var input = $(this).parents('.form-item').find('input');
		      
		      Drupal.mailCheck.mail = '';
		      input
		      .keyup(function () {
		        if(input.val() != Drupal.mailCheck.mail) {
		          clearTimeout(Drupal.mailCheck.timer);
		          Drupal.mailCheck.timer = setTimeout(function () {Drupal.mailCheck.check(input)}, parseFloat(Drupal.settings.mailCheck.delay)*1000);
		        
		          if(!$("#mail-check-informer").hasClass('mail-check-informer-progress')) {
		            $("#mail-check-informer")
		              .removeClass('mail-check-informer-accepted')
		              .removeClass('mail-check-informer-rejected');
		          }
		            
		          $("#mail-check-message")
		            .hide();
		        }
		      })
		      .blur(function () {
		        if(input.val() != Drupal.mailCheck.mail) {
		          Drupal.mailCheck.check(input);
		        }
		      });    
		    })
		    .addClass('mail-check-processed'); 
		  }
		};

		Drupal.mailCheck = {};
		Drupal.mailCheck.check = function(input) {
		  clearTimeout(Drupal.mailCheck.timer);
		  Drupal.mailCheck.mail = input.val();
		  
		  $.ajax({
		    url: Drupal.settings.mailCheck.ajaxUrl,
		    data: {mail: Drupal.mailCheck.mail},
		    dataType: 'json',
		    beforeSend: function() {
		      $("#mail-check-informer")
		        .removeClass('mail-check-informer-accepted')
		        .removeClass('mail-check-informer-rejected')
		        .addClass('mail-check-informer-progress');
		    },
		    success: function(ret){
		      if(ret['allowed']){
		        $("#mail-check-informer")
		          .removeClass('mail-check-informer-progress')
		          .addClass('mail-check-informer-accepted');
		        
		        input
		          .removeClass('error');
		      }
		      else {
		        $("#mail-check-informer")
		          .removeClass('mail-check-informer-progress')
		          .addClass('mail-check-informer-rejected');
		        
		        $("#mail-check-message")
		          .addClass('mail-check-message-rejected')
		          .html(ret['msg'])
		          .show();
		      }
		    }
		   });
		}  
		
		
		Drupal.behaviors.domainCheck = {
				  attach: function (context) {
				    $('#domain-check-informer:not(.domain-check-processed)', context)
				    .each(function() {
				      var input = $(this).parents('.form-item').find('input');
				      
				      Drupal.domainCheck.domain = '';
				      input
				      .keyup(function () {
				        if(input.val() != Drupal.domainCheck.domain) {
				          clearTimeout(Drupal.domainCheck.timer);
				          Drupal.domainCheck.timer = setTimeout(function () {Drupal.domainCheck.check(input)}, parseFloat(Drupal.settings.domainCheck.delay)*1000);
				        
				          if(!$("#domain-check-informer").hasClass('domain-check-informer-progress')) {
				            $("#domain-check-informer")
				              .removeClass('domain-check-informer-accepted')
				              .removeClass('domain-check-informer-rejected');
				          }
				            
				          $("#domain-check-message")
				            .hide();
				        }
				      })
				      .blur(function () {
				        if(input.val() != Drupal.domainCheck.domain) {
				          Drupal.domainCheck.check(input);
				        }
				      });    
				    })
				    .addClass('domain-check-processed'); 
				  }
				};

				Drupal.domainCheck = {};
				Drupal.domainCheck.check = function(input) {
				  clearTimeout(Drupal.domainCheck.timer);
				  Drupal.domainCheck.domain = input.val();
				  
				  $.ajax({
				    url: Drupal.settings.domainCheck.ajaxUrl,
				    data: {domain: Drupal.domainCheck.domain},
				    dataType: 'json',
				    beforeSend: function() {
				      $("#domain-check-informer")
				        .removeClass('domain-check-informer-accepted')
				        .removeClass('domain-check-informer-rejected')
				        .addClass('domain-check-informer-progress');
				    },
				    success: function(ret){
				      if(ret['allowed']){
				        $("#domain-check-informer")
				          .removeClass('domain-check-informer-progress')
				          .addClass('domain-check-informer-accepted');
				        
				        input
				          .removeClass('error');
				      }
				      else {
				        $("#domain-check-informer")
				          .removeClass('domain-check-informer-progress')
				          .addClass('domain-check-informer-rejected');
				        
				        $("#domain-check-message")
				          .addClass('domain-check-message-rejected')
				          .html(ret['msg'])
				          .show();
				      }
				    }
				   });
				}		
  
}



})(jQuery); 
