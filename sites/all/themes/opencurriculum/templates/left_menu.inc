<div id="left_nav">
<a href="<?php print $front_page; ?>etextbooks" ><strong>eTextbooks</strong></a>
  <?php if ($page['featured']): ?>
      <?php print render($page['featured']); ?>
  <?php endif; ?>
  
<a href="<?php print $front_page; ?>forum" id="forums">Forums</a>
<a href="#" id="subjectBrowse">Browse by subject <img src="<?php print $base_path.path_to_theme(); ?>/images/arrow_below.png" height="8px" width="15px" /></a>
<div id="subjectList">
	&rsaquo; <?php print l("Mathematics", "mathematics"); ?><br/>
	&rsaquo; <?php print l("Science", "science"); ?><br/>
<!--	&rsaquo; <?php print l("Computer Science", "computer-science"); ?><br/>-->
	&rsaquo; <?php print l("English", "english"); ?><br/>
	&rsaquo; <?php print l("Geography", "geography"); ?><br/>
	&rsaquo; <?php print l("Government & Civics", "government-civics"); ?><br/>
	&rsaquo; <?php print l("History", "history"); ?><br/>

</div>
<a href="<?php print $front_page; ?>licenses" id="license">Browse resouces by license</a>
<!--<a href="<?php print $front_page; ?>faqs" id="faqs">FAQs</a>-->

<div id="oers">

	<a href="<?php print $front_page; ?>community" id="blog">Community &amp; Blog</a>
	<a href="<?php print $front_page; ?>board" id="ocb">OpenCurriculum Board</a>
	<!--<h2>Open Educational Resource (OER) networks</h2><h3>Coming soon!</h3>-->
	<!--
    <?php if ($page['oers']): ?>
        <?php print render($page['oers']); ?>
    <?php endif; ?>-->
</div>

</div>

<h2>eTextbooks Licence</h2><p/>
All the eTextbook content on this website is licensed under the <a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/">Creative Commons Attribution-ShareAlike 3.0 Unported License</a> with allows you to copy, mix and redistribute the content for free
<p/>
<img alt="Creative Commons License" src="http://i.creativecommons.org/l/by-sa/3.0/88x31.png" />