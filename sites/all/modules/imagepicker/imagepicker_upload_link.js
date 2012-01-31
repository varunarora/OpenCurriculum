
/**
 * @file
 * Javascript functions for multiple uploads
 *
*/

(function($) {

  Drupal.behaviors.imagepicker_upload_link = {
    attach:  function(context) {
      var max_uploads = Drupal.settings.imagepicker_upload_link.max_uploads;
      for (c = 2; c <= max_uploads; c++) {
        $("#imagepicker_upload_link_wrapper_" + c).hide();
      }
      $("#imagepicker_upload_link_1").show();

      Drupal.imagepicker_upload_link_click = function(ct, context) {
        $("#imagepicker_upload_link_wrapper_" + (ct+1)).show();
        $("#imagepicker_upload_link_" + (ct+1)).show();
        $("#imagepicker_upload_link_" + (ct)).hide();
      }
    }
  };

})(jQuery);
