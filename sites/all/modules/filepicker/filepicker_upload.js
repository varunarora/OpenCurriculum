
/**
 * @file
 * Javascript functions for progress bar
 *
*/

(function($) {

  Drupal.behaviors.filepicker_upload = {
    attach: function(context) {

      var name = Drupal.settings.filepicker_upload_progress.name;
      var callback = Drupal.settings.filepicker_upload_progress.callback;
      var interval = Drupal.settings.filepicker_upload_progress.interval;
      var delay = Drupal.settings.filepicker_upload_progress.delay;
      var initmessage = Drupal.settings.filepicker_upload_progress.initmessage;
      var cancelmessage = Drupal.settings.filepicker_upload_progress.cancelmessage;

      // only once
      if ($('#filepicker-sending.processed').size()) {
        return;
      }

      $('#filepicker-upload-form', context).submit(function() {
        $("#filepicker-sending").addClass('progress');
        $("#filepicker-sending").attr('aria-live', 'polite');
        $("#filepicker-sending").addClass('processed');
        $("#filepicker-sending").html('<div class="message">' + initmessage + '</div>'+
            '<div class="bar"><div class="filled"></div></div>'+
            '<div class="percentage"></div>'+
            '<a id="filepicker_upload_progress_cancel_link" href="#">' + cancelmessage + '</a>'+
            '</div>');
        Drupal.filepicker_upload_progress_hide_timeout(delay);
        // are we using PECL uploadprogress
        if (name) {
          progress_key = $("input:hidden[name=" + name + "]").val();
          var i = setInterval(function() {
            $.getJSON(callback + '?key=' + progress_key, function(data) {
              if (data == null) {
                clearInterval(i);
                return;
              }
              $("#filepicker-sending div.message").html(data.message);
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
      Drupal.filepicker_upload_progress_hide = function() {
        $('#filepicker-upload-form').hide();
        $("#filepicker-sending").show();
        $("#filepicker_upload_progress_cancel_link").click(Drupal.filepicker_upload_progress_cancel);
      }
      // set delay
      Drupal.filepicker_upload_progress_hide_timeout = function(delay) {
        setTimeout(Drupal.filepicker_upload_progress_hide, delay*1000);
      }
      // cancel the run
      Drupal.filepicker_upload_progress_cancel = function() {
        $('#filepicker-upload-form').show();
        $("#filepicker-sending").hide();

        // "reload" the form
        window.location = window.location;
      }
    }
  };

})(jQuery);
