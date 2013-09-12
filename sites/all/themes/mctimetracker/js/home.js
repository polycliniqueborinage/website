var animationTimeout=null;

function animateToTab(selectedTab){

	if(jQuery("#"+selectedTab).hasClass("selected")==false){
		jQuery("#feature .tab").removeClass("selected");jQuery("#"+selectedTab).addClass("selected");jQuery(".how-it-works-callout:visible").fadeOut("fast",function(){jQuery("."+selectedTab+"-section").fadeIn("slow");});
	}
	var selectedTabMargin=jQuery("#"+selectedTab).attr('alt');var remainingMargin=960-parseInt(selectedTabMargin);jQuery(".selected-tab").animate({marginLeft:selectedTabMargin,marginRight:remainingMargin},500);}

jQuery(document).ready(function(){
	
	jQuery("#feature .tab").mouseenter(function(){var selectedTab=jQuery(this).attr('id');if(animationTimeout){clearTimeout(animationTimeout);}
	animationTimeout=setTimeout('animateToTab("'+selectedTab+'")',200);});
	jQuery(".tab").click(function(){window.scrollTo(0,jQuery('#feature').position().top);});jQuery(".logo").mouseenter(function(){var selectedNewsLogo=jQuery(this).attr('id');jQuery(".logo").removeClass("selected");jQuery(".news-story p").fadeOut(function(){jQuery(".news-story p").html(jQuery("#"+selectedNewsLogo).attr('alt'));});jQuery(".news-story p").fadeIn();jQuery("#"+selectedNewsLogo).addClass("selected");var margin=0;if(selectedNewsLogo=="gigaom"){margin="115px";}else if(selectedNewsLogo=="tech-crunch"){margin="300px";}else if(selectedNewsLogo=="venture-beat"){margin="490px";}else if(selectedNewsLogo=="nytimes"){margin="510px";}else if(selectedNewsLogo=="wsj"){margin="700px";}
	jQuery(".company-logo-selector img").animate({marginLeft:margin})});
	
	Cufon.replace('.canvas', { fontFamily: 'Note this', hover: true }); 

});
