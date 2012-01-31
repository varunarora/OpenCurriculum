
/**
 * @file
 * Javascript functions for multiple uploads
 *
*/

(function($) {

  Drupal.behaviors.filepicker_upload_link = {
    attach:  function(context) {
      var max_uploads = Drupal.settings.filepicker_upload_link.max_uploads;
      for (c = 2; c <= max_uploads; c++) {
        $("#filepicker_upload_link_wrapper_" + c).hide();
      }
      $("#filepicker_upload_link_1").show();

      Drupal.filepicker_upload_link_click = function(ct, context) {
        $("#filepicker_upload_link_wrapper_" + (ct+1)).show();
        $("#filepicker_upload_link_" + (ct+1)).show();
        $("#filepicker_upload_link_" + (ct)).hide();
      }
    }
  };

})(jQuery);
