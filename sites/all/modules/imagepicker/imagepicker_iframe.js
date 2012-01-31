
/**
 * @file
 * Provides javascript for insertion of html from iframe to the body in the parent.
 */

  // use jquery browser detection
(function ($) {
  imagepicker_browser_detect = function() {
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
function imagepickerInsert(button) {
  // Get the form element
  var imgpForm = document.getElementById('imagepicker-image-form');
  if (imgpForm) {
    var imgpShow = 'thumb';
    var imgpLink = 'file';
    var imgpAlign = 'none';
    var imgpImagePath;
    var imgpImageElement;
    var imgpImageStyle;
    var imgpImageCss = 'class="imgp_img"';
    var imgpLinkRel = '';
    var imgpLinkHide = '';
    var imgpInsertion;
    var imgpImageAlt = Drupal.settings.imagepicker_iframe.imgpImageAlt;
    var imgpImageTitle = Drupal.settings.imagepicker_iframe.imgpImageTitle;
    var imgpImageDesc = Drupal.settings.imagepicker_iframe.imgpImageDesc;
    var imgpFileLink = Drupal.settings.imagepicker_iframe.imgpFileLink;
    var imgpPresetFileLink = Drupal.settings.imagepicker_iframe.imgpPresetFileLink;
    var imgpThumbLink = Drupal.settings.imagepicker_iframe.imgpThumbLink;
    var imgpPresetThumbLink = Drupal.settings.imagepicker_iframe.imgpPresetThumbLink;
    var imgpPageLink = Drupal.settings.imagepicker_iframe.imgpPageLink;
    var imgpTemplate = Drupal.settings.imagepicker_iframe.imgpTemplate;
    var isFCKeditor = Drupal.settings.imagepicker_iframe.isFCKeditor;
    var isWysiwyg = Drupal.settings.imagepicker_iframe.isWysiwyg;
    var use_cssbox = Drupal.settings.imagepicker_iframe.use_cssbox;
    var use_relbox = Drupal.settings.imagepicker_iframe.use_relbox;
    var default_align_show = Drupal.settings.imagepicker_iframe.default_align_show;
    var lightbox2_insert = Drupal.settings.imagepicker_iframe.lightbox2_insert;
    var fleft = Drupal.settings.imagepicker_iframe.default_fleft;
    var fright = Drupal.settings.imagepicker_iframe.default_fright;
    var colorbox_iframe = Drupal.settings.imagepicker_iframe.colorbox_iframe;
    var node_editbody = Drupal.settings.imagepicker_iframe.node_editbody;
    var i;

    // Get show value
    for (i = 0; i < imgpForm.show.length; i++) {
      if(imgpForm.show[i].checked) {
        imgpShow = imgpForm.show[i].value;
      }
    }
    // Get link value
    for (i = 0; i < imgpForm.link.length; i++) {
      if(imgpForm.link[i].checked) {
        imgpLink = imgpForm.link[i].value;
      }
    }
    // cssbox
    if (use_cssbox) {
      // get css value
      if(imgpForm.cssbox.value) {
        imgpImageCss = imgpForm.cssbox.value;
      }
    }
    // relbox
    if (use_relbox) {
      // get rel value
      if (imgpForm.relbox.value) {
        imgpLinkRel = "rel='" + imgpForm.relbox.value + "'";
        if (imgpForm.linkhide.checked) {
          imgpLinkHide = "js-hide";
        }
      }
    }
    // alignment settings
    if (default_align_show) {
      // Get align value
      for (i = 0; i < imgpForm.align.length; i++) {
        if(imgpForm.align[i].checked) {
          imgpAlign = imgpForm.align[i].value;
        }
      }
      // Create a style for image holder
      switch (imgpAlign) {
        case 'fleft':
          imgpImageStyle = fleft;
          break;
        case 'fright':
          imgpImageStyle = fright;
          break;
        case 'none':
        default:
          imgpImageStyle = '';
          break;
      }
    }
    else {
      imgpImageStyle = '';
    }
    // imagecache
    var presetFileLink = false;
    if (imgpForm.presets_show) {
      if (imgpForm.presets_show.value && imgpPresetFileLink) {
        presetFileLink = imgpPresetFileLink.replace('__PRESET__', imgpForm.presets_show.value);
      }
    }
    var presetThumbLink = false;
    if (imgpForm.presets_show) {
      if (imgpForm.presets_show.value && imgpPresetThumbLink) {
        presetThumbLink = imgpPresetThumbLink.replace('__PRESET__', imgpForm.presets_show.value);
      }
    }

    switch (imgpShow) {
      case 'full':
        if (presetFileLink) {
          imgpImagePath = presetFileLink;
        }
        else {
          imgpImagePath = imgpFileLink;
        }
        break;
      case 'title':
        imgpImagePath = '';
        break;
      case 'thumb':
      default:
        if (presetThumbLink) {
          imgpImagePath = presetThumbLink;
        }
        else {
          imgpImagePath = imgpThumbLink;
        }
        break;
    }

    // Create an image or span (containing title) HTML string
    if (imgpImagePath) {
      imgpImageElement = "<img src='" + imgpImagePath + "' alt='" + imgpImageAlt + "' " + imgpImageStyle + " " + imgpImageCss + " />";
    }
    else {
      imgpImageElement = "<span>" + imgpImageTitle + "</span>";
    }

    // imagecache
    if (imgpForm.presets_link) {
      if (imgpForm.presets_link.value) {
        imgpFileLink = imgpPresetFileLink.replace('__PRESET__', imgpForm.presets_link.value);
        imgpPageLink = imgpPageLink + '/' + imgpForm.presets_link.value;
      }
    }

    // Create a link HTML string
    switch (imgpLink) {
      case 'none':
        imgpInsertion = imgpImageElement;
        break;
      case 'page':
        imgpInsertion = "<a href='" + imgpPageLink + "' title='" + imgpImageTitle + "' >" + imgpImageElement + "</a>";
        break;
      case 'lightbox':
        imgpInsertion = "<a href='" + imgpFileLink + "' title='" + imgpImageTitle + "' rel= '" + lightbox2_insert + "' >" + imgpImageElement + "</a>";
        break;
      case 'colorbox':
        imgpInsertion = "<a href='" + imgpFileLink + "' title='" + imgpImageTitle + "' class='colorbox " + imgpLinkHide + "' " + imgpLinkRel + ">" + imgpImageElement + "</a>";
        break;
      case 'file':
      default:
        imgpInsertion = "<a href='" + imgpFileLink + "' title='" + imgpImageTitle + "' target='_blank' >" + imgpImageElement + "</a>";
        break;
    }
    // wrap title and description if requested
    if (imgpForm.desc.checked) {
      imgpTemplate = imgpTemplate.replace("__TITLE__", imgpImageTitle);
      imgpTemplate = imgpTemplate.replace("__INSERT__", imgpInsertion);
      imgpInsertion = imgpTemplate.replace("__DESC__", imgpImageDesc);
    }

    // Get the parent window of imagepicker iframe
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
          win.FCKeditorAPI.GetInstance(inst).InsertHtml(imgpInsertion);
          jobdone = true;
        }
      }
      // ckeditor 3.?
      if (win.CKEDITOR) {
        if (win.CKEDITOR.instances[inst]) {
          win.CKEDITOR.instances[inst].insertHtml(imgpInsertion);
          jobdone = true;
        }
      }
      // tinyMCE v3
      if (win.tinyMCE) {
        if (win.tinyMCE.execInstanceCommand(inst, 'mceInsertContent', false, imgpInsertion)){
          jobdone = true;
        }
      }
    }
    // older ckeditor
    if (! jobdone && win.Drupal.ckeditorInstance && win.Drupal.ckeditorInsertHtml) {
      if (win.Drupal.ckeditorInsertHtml(imgpInsertion)) {
        jobdone = true;
      }
      //else
      //  return;
    }

    //var isTinyMCE = win.document.getElementById('mce_editor_0'); // buggy
    //var isTinyMCE = win.tinyMCE; // Will be undefined if tinyMCE isn't loaded. This isn't a sure-proof way of knowing if tinyMCE is loaded into a field, but it works.
    if (! jobdone && win.tinyMCE && win.tinyMCE.execCommand('mceInsertContent', false, imgpInsertion)) {
      jobdone = true;
    }

    if (! jobdone) {
      var nodeBody = win.document.getElementById(node_editbody);
      var commentBody = win.document.getElementById('edit-comment-value');
      var blockBody = win.document.getElementById('edit-body-value');
      if (nodeBody) {
        imagepicker_insertAtCursor(nodeBody, imgpInsertion);
      }
      else if (commentBody) {
        imagepicker_insertAtCursor(commentBody, imgpInsertion);
      }
      else if (blockBody) {
        imagepicker_insertAtCursor(blockBody, imgpInsertion);
      }
    }
    if (! colorbox_iframe) {
      win.location.hash = 'body_hash';
    }
  }
}

// Copy pasted from internet but modified to detect browser
function imagepicker_insertAtCursor(myField, myValue) {
  browser = imagepicker_browser_detect();
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

