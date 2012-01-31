
/**
 * @file
 * Provides javascript for insertion of html from iframe to the body in the parent.
 */

// use jquery browser detection
(function ($) {
  filepicker_browser_detect = function() {
    if ($.browser.msie) {
      return 'msie';
    }
    else if ($.browser.safari) {
      return 'safari';
    }
    else if ($.browser.opera) {
      return 'opera';
    }
    else if ($.browser.mozilla) {
      return 'mozilla';
    }
    else {
      return 'unknown';
    }
  }
})(jQuery);

// collects settings, builds HTML string
function filepickerInsert(button) {
  // Get the form element
  var filepForm = document.getElementById('filepicker-file-form');
  if (filepForm) {
    var filepFilePath = filepForm.file_path.value;
    var filepFileTitle = filepForm.file_title.value;
    var filepFileName = filepForm.file_name.value;
    var filepFileNewWindow = Drupal.settings.filepicker_iframe.file_new_window;
    var isFCKeditor = Drupal.settings.filepicker_iframe.isFCKeditor;
    var isWysiwyg = Drupal.settings.filepicker_iframe.isWysiwyg;
    var colorbox_iframe = Drupal.settings.filepicker_iframe.colorbox_iframe;
    var node_editbody = Drupal.settings.filepicker_iframe.node_editbody;
    //var i;
    var filepInsertion;
    // build a link
    if (filepFilePath) {
      filepInsertion = "<a href='" + filepFilePath + "' "+ (filepFileNewWindow ? 'target="_blank"' : '') + " >" + (filepFileTitle ? filepFileTitle : filepFileName) + "</a>";
    }

    // Get the parent window of filepicker iframe
    var win = window.opener ? window.opener : window.dialogArguments;
    if (!win) {
      if (window.parent) {
        win = window.parent;
      }
      else {
        win = top;
      }
    }

    // track down a wysiwyg editor
    var jobdone = false;
    var inst = false;
    if (win.oFCK_1) {
      inst = win.oFCK_1.InstanceName;
    }
    else if (win.oFCKeditor) {
      inst = win.oFCKeditor.InstanceName;
    }
    else if (isWysiwyg == 'yes' && win.Drupal.wysiwyg) {
      //inst = 'edit-body';
      inst = win.Drupal.wysiwyg.activeId;
    }

    if (inst) {
      if (win.FCKeditorAPI) {
        if (win.FCKeditorAPI.GetInstance(inst)) {
          win.FCKeditorAPI.GetInstance(inst).InsertHtml(filepInsertion);
          jobdone = true;
        }
      }
      // ckeditor 3.?
      if (win.CKEDITOR) {
        if (win.CKEDITOR.instances[inst]) {
          win.CKEDITOR.instances[inst].insertHtml(filepInsertion);
          jobdone = true;
        }
      }
      // tinyMCE v3
      if (win.tinyMCE) {
        if (win.tinyMCE.execInstanceCommand(inst, 'mceInsertContent', false, filepInsertion)){
          jobdone = true;
        }
      }
    }
    // older ckeditor
    if (! jobdone && win.Drupal.ckeditorInstance && win.Drupal.ckeditorInsertHtml) {
      if (win.Drupal.ckeditorInsertHtml(filepInsertion)) {
        jobdone = true;
      }
      //else
      //  return;
    }

    //var isTinyMCE = win.document.getElementById('mce_editor_0'); // buggy
    //var isTinyMCE = win.tinyMCE; // Will be undefined if tinyMCE isn't loaded. This isn't a sure-proof way of knowing if tinyMCE is loaded into a field, but it works.
    // tinyMCE v2
    if (! jobdone && win.tinyMCE && win.tinyMCE.execCommand('mceInsertContent', false, filepInsertion)) {
      jobdone = true;
    }

    if (! jobdone) {
      var nodeBody = win.document.getElementById(node_editbody);
      var commentBody = win.document.getElementById('edit-comment-value');
      var blockBody = win.document.getElementById('edit-body-value');
      if (nodeBody) {
        filepicker_insertAtCursor(nodeBody, filepInsertion);
      }
      else if (commentBody) {
        filepicker_insertAtCursor(commentBody, filepInsertion);
      }
      else if (blockBody) {
        filepicker_insertAtCursor(blockBody, filepInsertion);
      }
    }
    if (! colorbox_iframe) {
      win.location.hash = 'body_hash';
    }
  }
}

// Copy pasted from internet but modified to detect browser
function filepicker_insertAtCursor(myField, myValue) {
  browser = filepicker_browser_detect();
  if (browser == 'msie') {
    if (document.selection) {
      myField.focus();
      //in effect we are creating a text range with zero
      //length at the cursor location and replacing it
      //with myValue
      sel = document.selection.createRange();
      sel.text = myValue;
    }
  }
  else if (browser == 'opera' || browser == 'mozilla' || browser == 'safari' ) {
    if (myField.selectionStart || myField.selectionStart == '0') {
      //Here we get the start and end points of the
      //selection. Then we create substrings up to the
      //start of the selection and from the end point
      //of the selection to the end of the field value.
      //Then we concatenate the first substring, myValue,
      //and the second substring to get the new value.
      var startPos = myField.selectionStart;
      var endPos = myField.selectionEnd;
      myField.value = myField.value.substring(0, startPos)+ myValue + myField.value.substring(endPos, myField.value.length);
    }
  }
  else {
    myField.value += myValue;
  }
}

