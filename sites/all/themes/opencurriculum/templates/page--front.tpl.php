<?php

/**
 * @file
 * Bartik's theme implementation to display a single Drupal page.
 *
 * The doctype, html, head and body tags are not in this template. Instead they
 * can be found in the html.tpl.php template normally located in the
 * modules/system folder.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 * - $hide_site_name: TRUE if the site name has been toggled off on the theme
 *   settings page. If hidden, the "element-invisible" class is added to make
 *   the site name visually hidden, but still accessible.
 * - $hide_site_slogan: TRUE if the site slogan has been toggled off on the
 *   theme settings page. If hidden, the "element-invisible" class is added to
 *   make the site slogan visually hidden, but still accessible.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['header']: Items for the header region.
 * - $page['featured']: Items for the featured region.
 * - $page['highlighted']: Items for the highlighted content region.
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['triptych_first']: Items for the first triptych.
 * - $page['triptych_middle']: Items for the middle triptych.
 * - $page['triptych_last']: Items for the last triptych.
 * - $page['footer_firstcolumn']: Items for the first footer column.
 * - $page['footer_secondcolumn']: Items for the second footer column.
 * - $page['footer_thirdcolumn']: Items for the third footer column.
 * - $page['footer_fourthcolumn']: Items for the fourth footer column.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see bartik_process_page()
 */
?>

<script type="text/javascript">
  var uvOptions = {};
  (function() {
    var uv = document.createElement('script'); uv.type = 'text/javascript'; uv.async = true;
    uv.src = ('https:' == document.location.protocol ? 'https://' : 'http://') + 'widget.uservoice.com/aKrd0ayIyn6BsSmA8yegCQ.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(uv, s);
  })();
</script>
<div id="header">
<div id="header-content">
<div id="header-left">
<a href="<?php print $front_page; ?>"><img src="<?php print $logo; ?>" alt="The Open Curriculum Project" /></a><br/>
<span id="one-liner">
        <?php if ($site_slogan): ?>
            <?php print $site_slogan; ?>
        <?php endif; ?>
</span>
</div>

<div id="header-right">
    <?php /*if ($secondary_menu): ?>
        <?php print theme('links__system_secondary_menu', array(
          'links' => $secondary_menu,
          'attributes' => array(
            'id' => 'secondary-menu-links',
            'class' => array('links', 'inline', 'clearfix'),
          ),
          'heading' => array(
            'text' => t('Secondary menu'),
            'level' => 'h2',
            'class' => array('element-invisible'),
          ),
        )); ?>
      <!-- #secondary-menu -->
    <?php endif;*/ ?>
<?php include('login.inc') ?>
  <?php print render($page['search']); ?>
<?php include('gradeblock.inc') ?>
</div>

</div>
</div>
<div id="main">
  <?php if ($messages): ?>
    <div id="messages"><div class="section clearfix">
      <?php print $messages; ?>
    </div></div> <!-- /.section, /#messages -->
  <?php endif; ?>
  <?php if ($main_menu): ?>
      <div id="main-menu" class="navigation">
        <?php print theme('links__system_main_menu', array(
          'links' => $main_menu,
          'attributes' => array(
            'id' => 'main-menu-links',
            'class' => array('links', 'clearfix'),
          ),
          'heading' => array(
            'text' => t('Main menu'),
            'level' => 'h2',
            'class' => array('element-invisible'),
          ),
        )); ?>
      </div> <!-- /#main-menu -->
    <?php endif; ?>
<div id="left-panel">
<?php include('left_menu.inc'); ?>



</div>

<div id="right-panel">
<h1 id="home-title">Free &amp; high-quality K-12 textbooks. <br/><span class="slider-green">Created by you.</span></h1>
<p/>
<div id="project-info" style="overflow: auto">
	<div id="project-brief">OpenCurriculum is a project aimed at creating the first global free K-12 school curriculum created by members of community like you.<p/>We are hope to build a listing of education resources associated with textbook chapters that are sorted, ranked and reviewed by you.<p/>All this, ready for print and mobile &amp; tablet devices!<p style="margin-top: 20px"/>

		<a href="user/register" class="classy-button">Get started now &#187;</a> <a href="about" class="classy-button">Learn more &#187;</a>
		</div>
	<div id="project-image"><img src="<?php print path_to_theme();?>/images/oer_refl.png"></div>
</div>
<!--http://cssglobe.com/post/5780/easy-slider-17-numeric-navigation-jquery-slider -->

<!--
<div id="slider">
	<ul>


	<li><div><span id="slider_title"><h2>An initiative to create the world's first full free and open source global K-12 curriculum</h2>Free and open source breeds open innovation, and that is what we believe in. Every user contributed piece of information, if not explicitly licensed otherwise, is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.</span><img src="<?php print path_to_theme();?>/images/cc_refl.png"></div></li>
	<li><div><span id="slider_title"><h2>Creating the world's largest listing of Open Educational Resources, sorted and ranked</h2>Each chapter, book, and subject can now have OERs related directly to them, which are accessible using several filters, such as rating, sorting, views, stars, license, etc.</span><img src="<?php print path_to_theme();?>/images/oer_refl.png"></div></li>
	<li><h2>Applying the 21st century learning skills to produce meaningful learning content</h2></li>
	<li><div><span id="slider_title"><h2>Supporting ICT for Education projects such as One Laptop Per Child and Digital Study Hall</h2></span><img src="<?php print path_to_theme();?>/images/olpcdsh.png"></div></li>
	</ul>

</div>
-->

<div id="hot"><h2>Hot topics &amp; chapters</h2><p/>
      <?php print render($page['recent_content']); ?>
</div>
<div id="article-feed">
	  <h3 style="margin-top: 0px;">Posts from OC Blog</h3>
      <?php print render($page['content']); ?>
      </div>
</div>

</div>

<?php include('footer.inc') ?>
