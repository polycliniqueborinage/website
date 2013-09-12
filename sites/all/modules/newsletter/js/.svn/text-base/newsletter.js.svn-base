(function ($) {
  Drupal.behaviors.newsletter = {
    attach: function (context, settings) {
      $("input[name='email']").click(function () {
          if ($("input[name='email']").val() == 'user@example.com') {
            $("input[name='email']").val('');
          }
      });
      $("input[name='email']").blur(function () {
        if ($("input[name='email']").val() == '') {
          $("input[name='email']").val('user@example.com');
        }
      });
    },

    subscribeForm: function(data) {
      $.each(Drupal.settings.exposed, function(i, e) {
        if (!$('#edit-' + i).attr('checked')) {
          $('.form-item-' + i + '-terms').css('display', 'none');
        }

        $('#edit-' + i).click(function () {
          if($('.form-item-' + i + '-terms').css('display') == 'none') {
            $('.form-item-' + i + '-terms').css('display', 'block');
          }
          else {
            $('.form-item-' + i + '-terms').css('display', 'none');
          }
        });
      });

      $('#newsletter-subscribe-advanced').submit(function() {
        if(!$('#newsletter-subscribe-advanced input[type="checkbox"]').is(':checked')){
          alert(Drupal.t("Please check at least one newsletter list."));
          return false;
        }
      });
    },
  };
})(jQuery);
