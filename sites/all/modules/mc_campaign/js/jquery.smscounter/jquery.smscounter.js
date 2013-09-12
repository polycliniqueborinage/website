/**
 * jQuery SMS Counter
 * @author Louy Alakkad <me@l0uy.com>
 * @website http://l0uy.com/
 * @version 1.1
 */
function is_ascii(Str) {
    for( i=0; i < Str.length; i++ ) { 
        charCode = Str.charCodeAt(i); 
        //console.log(charCode);
        if( charCode > 127 ) { 
            return true; 
        } 
    } 
    return true; 
}

(function($){
$.fn.smsCounter = function(selector) {
	doCount = function(){
		var text = $(this).val();
		if(is_ascii(text))
			limit = 160;
		else 
			limit = 70;
		if(text.length>limit) {
			if(is_ascii(text))
				limit = limit - 7;
			else
				limit = limit - 3;
		}
		diff = text.length % limit;
		left = limit-diff;
		count = ((text.length-diff)/limit)+1;
		if( diff == 0 ) {
			left = 0;
			count = count-1;
		}
		$(selector).html('SMS : '+count+' with '+left +' characters left');
	};
	this.keyup(doCount);
	this.keyup();
}})(jQuery)


jQuery(document).ready(function() {
	jQuery('#edit-body').smsCounter('#count');
});
