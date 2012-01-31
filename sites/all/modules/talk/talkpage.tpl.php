<?php // $Id: talkpage.tpl.php,v 1.2.4.2 2009/07/11 19:15:49 cwgordon7 Exp $ ?>
<?php
/**
 * @file
 * Overridable template file for talk page.
 *
 * Available variables:
 *   $node represents the node whose comments we're displaying.
 *   $comments represents the rendered comments.
 *   $comment_link represents an "add new comment" link.
 *   $add_comments is TRUE if the user has permission to add comments.
 *   $redisplay is TRUE if the "add new comment" link should be redisplayed at the bottom of the page.
 *   $title represents the title of the talk page. Defaults to "Talk".
 */
?>

<?php if ($comment_count == 0): ?>
    <div class="talk-page-empty-message">
      <?php print t('There are currently no comments on this page.'); ?>
    </div>
  <?php endif; ?>
  

<?php print $comments; ?>

