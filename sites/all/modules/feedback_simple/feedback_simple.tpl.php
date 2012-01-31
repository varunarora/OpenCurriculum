<?php
/**
 * @file
 * Template file.
 *
 * @param $link: link path.
 * @param $align: left or right.
 * @param $class: classes to apply to anchor tag.
 * @param $top: distance from the top; include unit.
 * @param $alt: alt text.
 * @param $image: image path.
 * @param $height: image height; include unit.
 * @param $width: image width; include unit.
 * @param $enabled: true or false.
 */
?>
<?php if ($enabled): ?>
<div id='feedback_simple'>
  <a
    href='<?php print $link ?>'
    class='feedback_simple-<?php print $align ?> <?php print implode(' ', $class) ?>'
    style='top: <?php print $top ?>; height: <?php print $height ?>; width: <?php print $width ?>;'>
      <img
        alt='<?php print $alt ?>'
        src='<?php print $image ?>'
        height='<?php print $height ?>'
        width='<?php print $width ?>' />
  </a>
</div>
<?php endif; ?>
