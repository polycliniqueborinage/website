<?php

/**
 * @file
 * Bartik's theme implementation to display a single Drupal page.
 *
 * The doctype, html, head and body tags are not in this template. Instead they
 * can be found in the html.tpl.php template normally located in the
 * modules/system folder.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/mctimetracker.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 * - $hide_site_name: TRUE if the site name has been toggled off on the theme
 *   settings page. If hidden, the "element-invisible" class is added to make
 *   the site name visually hidden, but still accessible.
 * - $hide_site_slogan: TRUE if the site slogan has been toggled off on the
 *   theme settings page. If hidden, the "element-invisible" class is added to
 *   make the site slogan visually hidden, but still accessible.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['header']: Items for the header region.
 * - $page['featured']: Items for the featured region.
 * - $page['mainmenu']: Main menu.
 * - $page['highlighted']: Items for the highlighted content region.
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['triptych_first']: Items for the first triptych.
 * - $page['triptych_middle']: Items for the middle triptych.
 * - $page['triptych_last']: Items for the last triptych.
 * - $page['footer_firstcolumn']: Items for the first footer column.
 * - $page['footer_secondcolumn']: Items for the second footer column.
 * - $page['footer_thirdcolumn']: Items for the third footer column.
 * - $page['footer_fourthcolumn']: Items for the fourth footer column.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see mctimetracker_process_page()
 */


?>

<div id="page-wrapper"><div id="page">

  <div id="header" class="<?php print $secondary_menu ? 'with-secondary-menu': 'without-secondary-menu'; ?>"><div class="section clearfix">

    <?php if ($logo): ?>
      <a href="<?php print $front_page; ?>" title="<?php print t('Online Calendar SMS Reminder'); ?>" rel="home" id="logo">
        <img src="<?php print $logo; ?>" alt="<?php print t('Online Calendar SMS Reminder'); ?>" />
      </a>
    <?php endif; ?>

    <?php if ($site_name || $site_slogan): ?>
      <div id="name-and-slogan"<?php if ($hide_site_name && $hide_site_slogan) { print ' class="element-invisible"'; } ?>>

        <?php if ($site_name): ?>
          <?php if ($title): ?>
            <div id="site-name"<?php if ($hide_site_name) { print ' class="element-invisible"'; } ?>>
              <strong>
                <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a>
              </strong>
            </div>
          <?php else: /* Use h1 when the content title is empty */ ?>
            <h1 id="site-name"<?php if ($hide_site_name) { print ' class="element-invisible"'; } ?>>
              <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a>
            </h1>
          <?php endif; ?>
        <?php endif; ?>

        <?php if ($site_slogan): ?>
          <div id="site-slogan"<?php if ($hide_site_slogan) { print ' class="element-invisible"'; } ?>>
            <?php print $site_slogan; ?>
          </div>
        <?php endif; ?>

      </div> <!-- /#name-and-slogan -->
    <?php endif; ?>

    <?php print render($page['header']); ?>


    <?php if ($main_menu): ?>
      <div id="main-menu" class="navigation">
        <?php print render($page['mainmenu']); ?>
      </div> <!-- /#main-menu -->
    <?php endif; ?>

  </div></div> <!-- /.section, /#header -->

  <?php if ($messages): ?>
    <div id="messages"><div class="section clearfix">
      <?php print $messages; ?>
    </div></div> <!-- /.section, /#messages -->
  <?php endif; ?>

  <?php if ($page['featured']): ?>
    <div id="featured"><div class="section clearfix">
      <?php print render($page['featured']); ?>
    </div></div> <!-- /.section, /#featured -->
  <?php endif; ?>

  <div id='home-page' class="clearfix">
  
  <?php   
  $domain = domain_get_domain();
  if ($domain['domain_id'] == 1) {
  ?>
  
  <div id="banner">
  
    <div class="inner-content">
      
      <div class="slider">
						
	    <div class="slider-wrapper theme-default">
          <div id="slider" class="nivoSlider">
                <img src="/sites/all/themes/mctimetracker/images/agenda1.jpg" alt="Online Calendar" />
                <img src="/sites/all/themes/mctimetracker/images/agenda2.jpg" alt="Notifications Statistics" />
                <img src="/sites/all/themes/mctimetracker/images/agenda3.jpg" alt="Online Reminder" />
          </div>
        </div>
      
      </div>
		
	  <div class="banner-content"> 
	  
	  	<?php print render($page['homepage_intro']); ?>
		
	  </div>
		
	</div>
	
  </div>


		<div class="absolute-pusher">
			<div id="feature">
				<div class="yui3-u-1">
					<div class="inner-content">
						<div class="yui3-g">

							<!-- agenda -->
							<div class="yui3-u-1-4">
								<div class="tab" id="agenda" alt="83px">
									<h4 class="body-text"><?php print t('ONLINE')?><br><span class="big"><?php print t('CALENDARS')?></span></h4>
								</div>
							</div>

							<!-- sms -->
							<div class="yui3-u-1-4">
								<div class="tab" id="sms" alt="322px">
									<h4 class="body-text"><?php print t('ONLINE')?><br><span class="big"><?php print t('REMINDERS')?></span></h4>
								</div>
							</div>

							<!-- client -->
							<div class="yui3-u-1-4">
								<div class="tab" id="call" alt="556px">
									<h4 class="body-text"><?php print t('MAKE')?><br><span class="big"><?php print t('CALLS')?></span></h4>
								</div>
							</div>

							<!-- phone-numbers -->
							<div class="yui3-u-1-4">
								<div class="tab" id="booking" alt="800px">
									<h4 class="body-text"><?php print t('BOOK')?><br><span class="big"><?php print t('ONLINE')?></span></h4>
								</div>
							</div>
							
						</div>
					</div>
				</div>
			</div>
		</div>
		<div class="yui3-u-1">
			<div class="selected-tab client-selected"><img src="/sites/all/themes/mctimetracker/images/nav-selected-arrow.jpg"></div>
		</div>
		<div class="yui3-u-1">
			<div class="how-it-works-callout agenda-section">
				<div class="callout-text">
					<?php print render($page['homepage_calendar']); ?>
				</div>
				<div class="callout-img">
					<img src="/sites/all/themes/mctimetracker/images/agenda_slider.jpg" class="diagram">
				</div>
			</div>
		</div>
		<div class="yui3-u-1">
			<div class="how-it-works-callout sms-section">
				<div class="callout-text">
					<?php print render($page['homepage_reminder']); ?>
				</div>
				<div class="callout-img">
					<div class="link"><a href="http://bit.ly/gFuFMA" target="_blank"></a></div>
					<img src="/sites/all/themes/mctimetracker/images/sms_slider.jpg" class="diagram">
				</div>
			</div>
		</div>
		<div class="yui3-u-1">
			<div class="how-it-works-callout call-section">
				<div class="callout-text">
					<?php print render($page['homepage_call']); ?>
				</div>
				<div class="callout-img">
					<img src="/sites/all/themes/mctimetracker/images/call_slider.jpg" class="diagram">
				</div>
			</div>
		</div>
		<div class="yui3-u-1">
			<div class="how-it-works-callout booking-section">
				<div class="callout-text">
					<?php print render($page['homepage_online']); ?>
				</div>
				<div class="callout-img">
					<img src="/sites/all/themes/mctimetracker/images/booking_slider.jpg" class="diagram">
				</div>
			</div>
		</div>


		<div id="why-content" class="yui3-u-1">
			<div class="yui3-u-2-3">
				<?php print render($page['homepage_why']); ?>
				<!-- END -->
			</div><!-- yui3-u-1 -->
			<div class="yui3-u-1-3">
				<h2 class="canvas"><?php print t('Ready to start ?')?></h1>
				<p class="by-line">
				<a href="user/register" class="try">
					<span class="title"><?php print t('Try MC Time Tracker')?></span>
		    		<span class="credit"><?php print t('100 FREE CREDITS')?></span>
		 		 </a>
				</p>
			</div><!-- yui3-u-1 -->
		</div>
		
		
		<div id="what-content" class="yui3-u-1">
			<?php print render($page['homepage_what']); ?>
		</div>


  <?php   
  } else {
  ?>
  
  <div id="main" class="clearfix">
  <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
  </div>

  <?php   
  }
  ?>
  
  
  </div> <!-- /#main, /#main-wrapper -->

  <?php if ($page['triptych_first'] || $page['triptych_middle'] || $page['triptych_last']): ?>
    <div id="triptych-wrapper"><div id="triptych" class="clearfix">
      <?php print render($page['triptych_first']); ?>
      <?php print render($page['triptych_middle']); ?>
      <?php print render($page['triptych_last']); ?>
    </div></div> <!-- /#triptych, /#triptych-wrapper -->
  <?php endif; ?>

  <div id="footer-wrapper"><div class="section">

    <?php if ($page['footer_firstcolumn'] || $page['footer_secondcolumn'] || $page['footer_thirdcolumn'] || $page['footer_fourthcolumn']): ?>
      <div id="footer-columns" class="clearfix">
        <?php print render($page['footer_firstcolumn']); ?>
        <?php print render($page['footer_secondcolumn']); ?>
        <?php print render($page['footer_thirdcolumn']); ?>
        <?php print render($page['footer_fourthcolumn']); ?>
      </div> <!-- /#footer-columns -->
    <?php endif; ?>

    <?php if ($page['footer']): ?>
      <div id="footer" class="clearfix">
        <?php print render($page['footer']); ?>
      </div> <!-- /#footer -->
    <?php endif; ?>

  </div></div> <!-- /.section, /#footer-wrapper -->

</div></div> <!-- /#page, /#page-wrapper -->


    <script type="text/javascript">
    jQuery(window).load(function() {
    	jQuery('#slider').nivoSlider({
			directionNav:false,
			effect: 'random',
	        slices:15,
	        boxCols:8,
	        boxRows:4,
	        animSpeed:500,
	        pauseTime:3000,
	        startSlide:0,
	        directionNav:false,
	        directionNavHide:false,
	        controlNav:false,
	        controlNavThumbs:false,
	        controlNavThumbsFromRel:false,
	        keyboardNav:false,
	        pauseOnHover:false,
	        manualAdvance:false
    	});
    });
    </script>
    
    
    <script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-27044754-1']);
  _gaq.push(['_setDomainName', 'mctimetracker.com']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

     </script>
