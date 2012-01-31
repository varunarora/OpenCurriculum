<?php

/**
 * @file
 * template for imagepicker iframe
 */

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML+RDFa 1.0//EN"
  "http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" version="XHTML+RDFa 1.0" dir="ltr"
  xmlns:content="http://purl.org/rss/1.0/modules/content/"
  xmlns:dc="http://purl.org/dc/terms/"
  xmlns:foaf="http://xmlns.com/foaf/0.1/"
  xmlns:og="http://ogp.me/ns#"
  xmlns:rdfs="http://www.w3.org/2000/01/rdf-schema#"
  xmlns:sioc="http://rdfs.org/sioc/ns#"
  xmlns:sioct="http://rdfs.org/sioc/types#"
  xmlns:skos="http://www.w3.org/2004/02/skos/core#"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema#">
<head profile="http://www.w3.org/1999/xhtml/vocab">
<!-- imagepicker.tpl.php -->
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

  <title><?php echo $head_title ?></title>
  <?php echo $styles ?>
  <?php echo $scripts ?>
  <script type="text/javascript"><?php /* Needed to avoid Flash of Unstyle Content in IE */ ?> </script>
</head>
<body id="imagepicker" class="html not-front logged-in no-sidebars overlay" >
  <div>
    <div class="tabs">
      <?php echo $tabs; ?>
    </div>
    <div id="imgp">
      <?php echo $messages ?>
      <?php echo $content; ?>
    </div>
  </div>
</body>
</html>
