
/**
 * @file
 * Javascript functions for progress bar
 *
*/

(function($) {

  Drupal.behaviors.imagepicker_upload = {
    attach: function(context) {

      var name = Drupal.settings.imagepicker_upload_progress.name;
      var callback = Drupal.settings.imagepicker_upload_progress.callback;
      var interval = Drupal.settings.imagepicker_upload_progress.interval;
      var delay = Drupal.settings.imagepicker_upload_progress.delay;
      var initmessage = Drupal.settings.imagepicker_upload_progress.initmessage;
      var cancelmessage = Drupal.settings.imagepicker_upload_progress.cancelmessage;

      // only once
      if ($('#imagepicker-sending.processed').size()) {
        return;
      }

      $('#imagepicker-upload-form', context).submit(function() {
        $("#imagepicker-sending").addClass('progress');
        $("#imagepicker-sending").attr('aria-live', 'polite');
        $("#imagepicker-sending").addClass('processed');
        $("#imagepicker-sending").html('<div class="message">' + initmessage + '</div>'+
            '<div class="bar"><div class="filled"></div></div>'+
            '<div class="percentage"></div>'+
            '<a id="imagepicker_upload_progress_cancel_link" href="#">' + cancelmessage + '</a>'+
            '</div>');
        Drupal.imagepicker_upload_progress_hide_timeout(delay);
        // are we using PECL uploadprogress
        if (name) {
          progress_key = $("input:hidden[name=" + name + "]").val();
          var i = setInterval(function() {
            $.getJSON(callback + '?key=' + progress_key, function(data) {
              if (data == null) {
                clearInterval(i);
                return;
              }
              $("#imagepicker-sending div.message").html(data.message);
              var percentage = data.percentage;
              if (percentage >= 0 && percentage <= 100) {
                $('div.filled').css('width', percentage +'%');
                $('div.percentage').html(percentage +'%');
              }
            });
          }, interval*1000);
        }
      });

      // Hide the form and show the busy div
      Drupal.imagepicker_upload_progress_hide = function() {
        $('#imagepicker-upload-form').hide();
        $("#imagepicker-sending").show();
        $("#imagepicker_upload_progress_cancel_link").click(Drupal.imagepicker_upload_progress_cancel);
      }
      // set delay
      Drupal.imagepicker_upload_progress_hide_timeout = function(delay) {
        setTimeout(Drupal.imagepicker_upload_progress_hide, delay*1000);
      }
      // cancel the run
      Drupal.imagepicker_upload_progress_cancel = function() {
        $('#imagepicker-upload-form').show();
        $("#imagepicker-sending").hide();

        // "reload" the form
        window.location = window.location;
      }
    }
  };

})(jQuery);
