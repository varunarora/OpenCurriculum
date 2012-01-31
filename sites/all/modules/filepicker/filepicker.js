/**
 * @file
 * Javascript functions for admin form enhancement
 *
*/

( function ($) {
  Drupal.behaviors.filepicker = {
    attach: function(context) {

      $("#edit-filepicker-quota-byrole", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-filepicker-quota-role", context).show();
        }
        else {
          $("#wrap-filepicker-quota-role", context).hide();
        }
      });

      $("#edit-filepicker-import-enabled", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-filepicker-import", context).show();
        }
        else {
          $("#wrap-filepicker-import", context).hide();
        }
      });

      $("#edit-filepicker-upload-progress-enabled", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-filepicker-upload-progress", context).show();
        }
        else {
          $("#wrap-filepicker-upload-progress", context).hide();
        }
      });

      $("#edit-filepicker-groups-enabled", context).change(function() {
        if ($(this).attr('checked')) {
          $("#wrap-filepicker-groups", context).show();
        }
        else {
          $("#wrap-filepicker-groups", context).hide();
        }
      });
    }
  };
})(jQuery);
