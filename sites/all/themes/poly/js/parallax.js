(function() {
  jQuery(document).ready(function() {
    var scrollBannerBG;
    scrollBannerBG = function() {
      var bannerHeight, bannerVerticalPos, bgHeight, offset, width;
      width = jQuery(window).width();
      bannerHeight = 560;
      if (width <= 1023) {
        bannerHeight = 500;
      }
      if (width <= 500) {
        bannerHeight = 400;
      }
      bgHeight = bannerHeight * 1.2;
      offset = (jQuery(window).scrollTop() / jQuery(document).height()) * 500;
      bannerVerticalPos = ((bannerHeight - bgHeight) / 2) + offset;
      return jQuery("zone-slider-wrapper").css({
        'background-position': "50% " + bannerVerticalPos + "px"
      });
    };
    if (1 == 1) {
      jQuery(window).bind("scroll", function(e) {
        return scrollBannerBG();
      });
      jQuery(window).bind('resize', function(e) {
        return scrollBannerBG();
      });
    } else {
      window.lsp = 0;
      jQuery(document).bind('touchmove', function(e) {
        scrollBannerBG();
        return window.lsp = jQuery(window).scrollTop();
      });
      window.usp = function() {
        if (jQuery(window).scrollTop() !== window.lsp) {
          return scrollBannerBG();
        }
      };
      setInterval('usp()', 300);
    }
    return scrollBannerBG();
  });
}).call(this);
