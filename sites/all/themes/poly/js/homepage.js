jQuery(document).ready(function() {
	
	marginleft = '490px';
	if (jQuery('body').width()>=1205) {
		marginleft = '370px';
	}

	jQuery('#slider').anythingSlider({
		autoPlay : true,
		startStopped : false,
		buildStartStop : false,
		buildNavigation : false,
		buildArrows : false,
		buildArrows     : false,
		pauseOnHover : false,
		delay : 5000,
		animationTime : 500,
		onSlideBegin : function(slider) {
		  var current_slide = jQuery('li.activePage').next();
		  if (current_slide.hasClass('cloned')) {
		    current_slide = jQuery('#slider li:first').next();
		  }
		  var current_slide = current_slide.find('.slider-text');
		  current_slide.animate({
		    marginLeft: marginleft
		  }, 1000, 'easeInOutExpo');
		},
		onSlideComplete : function(slider) {
		  var current_slide = jQuery('li.activePage').next();
		  if (current_slide.hasClass('cloned')) {
		    first_slide = jQuery('#slider li:first').next();
			first_slide.find('.slider-text').css({ marginLeft: '1280px' });
		  }
		  current_slide.find('.slider-text').css({ marginLeft: '1280px' });
		}
		
	});
	
	Cufon.replace('#zone-resume h2', { fontFamily: 'Segan Light', hover: true }); 

	Cufon.replace('#slider h2', { fontFamily: 'Segan Light', hover: true });
	
	Cufon.replace('#zone-header h2', { fontFamily: 'Segan Light', hover: true });
	
	Cufon.replace('#block-block-8 h2', { fontFamily: 'Segan Light', hover: true }); 

	Cufon.replace('h1', { fontFamily: 'Segan Light', hover: true }); 
	
	jQuery("#menu").superfish({
	    delay:     300,
	    autoArrows:    false,
	    dropShadows:   false,
	    speed:         'fast'
	});
	
	jQuery("#zone-footer .region").hover(
		function () {
			jQuery(this).addClass('hover');
		}, 
		function () {
			jQuery(this).removeClass('hover');
		}
	);
	
	jQuery('.bar').mosaic({
		animation	:	'slide'		//fade or slide
	});
	
	image_src = jQuery('#zone-image div img').attr("src");
	jQuery('#zone-image').css("background-image", "url(" + image_src + ")"); 
	jQuery('#zone-image div').html("");
	
	jQuery(window).scroll(function () {
		currentScrollTop = jQuery(window).scrollTop();
		if (currentScrollTop < 150) {
			jQuery('#zone-image').css('background-position', '0% ' + parseInt(-currentScrollTop / 1) + 'px');
		}
	});
	
});