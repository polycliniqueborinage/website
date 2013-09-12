// $Id: campaign.js,v 1.9 2010/08/04 03:58:46 dries Exp $
(function ($) {

/**
 * Open campaign privacy policy link in a new window.
 *
 * Required for valid XHTML Strict markup.
 */
Drupal.behaviors.campaignPrivacy = {
  attach: function (context) {
    $('.campaign-privacy a', context).click(function () {
      this.target = '_blank';
    });
  }
};

/**
 * Attach click event handlers for CAPTCHA links.
 */
Drupal.behaviors.campaignCaptcha = {
  attach: function (context, settings) {
    // @todo Pass the local settings we get from Drupal.attachBehaviors(), or
    //   inline the click event handlers, or turn them into methods of this
    //   behavior object.
    $('a.campaign-switch-captcha', context).click(getcampaignCaptcha);
  }
};

/**
 * Fetch a campaign CAPTCHA and output the image or audio into the form.
 */
function getcampaignCaptcha() {
  // Get the current requested CAPTCHA type from the clicked link.
  var newCaptchaType = $(this).hasClass('campaign-audio-captcha') ? 'audio' : 'image';

  var context = $(this).parents('form');

  // Extract the campaign session id and form build id from the form.
  var campaignSessionId = $('input.campaign-session-id', context).val();
  var formBuildId = $('input[name="form_build_id"]', context).val();

  // Retrieve a CAPTCHA:
  $.getJSON(Drupal.settings.basePath + 'campaign/captcha/' + newCaptchaType + '/' + formBuildId + '/' + campaignSessionId,
    function (data) {
      if (!(data && data.content)) {
        return;
      }
      // Inject new CAPTCHA.
      $('.campaign-captcha-content', context).parent().html(data.content);
      // Update session id.
      $('input.campaign-session-id', context).val(data.session_id);
      // Add an onclick-event handler for the new link.
      Drupal.attachBehaviors(context);
      // Focus on the CATPCHA input.
      $('input[name="campaign[captcha]"]', context).focus();
    }
  );
  return false;
}

})(jQuery);
