
/**
 * @file
 * Javascript functions for progress bar anf form enhancement
 *
 * By Bob Hutchinson for imagepicker upload form
*/

(function ($) {

  Drupal.behaviors.imagepicker = {
    attach: function(context) {

      // Attaches the upload behaviour to the imagepicker upload form.
      $('#imagepicker-upload-form', context).submit(Drupal.imagepicker_upload_progress_hide_timeout);

      // exif info toggle
      $('#imgp_trig', context).click(function() {
        $('#imgp_targ').toggle('slow');
      });

      // form enhancement
      $("#edit-imagepicker-quota-byrole", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-imagepicker-quota-role", context).show();
        }
        else {
          $("#wrap-imagepicker-quota-role", context).hide();
        }
      });

      $("#edit-imagepicker-import-enabled", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-imagepicker-import", context).show();
        }
        else {
          $("#wrap-imagepicker-import", context).hide();
        }
      });

      $("#edit-imagepicker-upload-progress-enabled", context).change(function() {
        if ($("#edit-imagepicker-upload-progress-enabled").attr('checked')) {
          $("#wrap-imagepicker-upload-progress", context).show();
        }
        else {
          $("#wrap-imagepicker-upload-progress", context).hide();
        }
      });

      $("#edit-imagepicker-groups-enabled", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-imagepicker-groups", context).show();
        }
        else {
          $("#wrap-imagepicker-groups", context).hide();
        }
      });

      $("#edit-imagepicker-watermark-enable", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-imagepicker-watermark", context).show();
        }
        else {
          $("#wrap-imagepicker-watermark", context).hide();
        }
      });

      $("#edit-imagepicker-galleryblocks-enabled", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-imagepicker-blocks", context).show();
        }
        else {
          $("#wrap-imagepicker-blocks", context).hide();
        }
      });

    }
  };
})(jQuery);
