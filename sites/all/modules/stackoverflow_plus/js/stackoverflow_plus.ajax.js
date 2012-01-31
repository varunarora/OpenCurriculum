/**
 * Create a degradeable star rating interface out of a simple form structure.
 */
(function($){ // Create local scope.

Drupal.ajax.prototype.commands.stackoverflow_plusUpdate = function (ajax, response, status) {
  response.selector = $('.fivestar-form-item', ajax.element.form);
  ajax.commands.insert(ajax, response, status);
};

})(jQuery);
