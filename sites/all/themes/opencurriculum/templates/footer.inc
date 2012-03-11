<div id="footer">
<div id="footer-content">
	<div id="footer-about">
		<strong>About</strong>
		<ul>
			<li><?php print l("About us", 'about'); ?></li>
			<li><?php print l("How it works", 'how-it-works'); ?></li>
			<li><?php print l("Goals", 'about/goals'); ?></li>
			<li><?php print l("Principles", 'about/core-principles'); ?></li>
			<li><?php print l("History", 'about/history'); ?></li>
			<li><?php print l("Team", 'about/team'); ?></li>
			<li><?php print l("Technology Roadmap", 'about/roadmap/technology'); ?></li>
			<li><?php print l("Education Roadmap", 'about/roadmap/education'); ?></li>
			<li><?php print l("Collaboration Roadmap", 'about/roadmap/collaborations'); ?></li>
			<li><?php print l("Terms & Conditions", 'terms-conditions'); ?></li>
		</ul>
	</div>
	
	<div id="footer-etextbooks">
		<strong>E-Textbooks</strong>
		<ul>
			<li><?php print l("Grade 1", 'etextbooks/1'); ?></li>
			<li><?php print l("Grade 2", 'etextbooks/2'); ?></li>
			<li><?php print l("Grade 3", 'etextbooks/3'); ?></li>
			<li><?php print l("Grade 4", 'etextbooks/4'); ?></li>
			<li><?php print l("Grade 5", 'etextbooks/5'); ?></li>
			<li><?php print l("Grade 6", 'etextbooks/6'); ?></li>
			<li><?php print l("Grade 7", 'etextbooks/7'); ?></li>
			<li><?php print l("Grade 8", 'etextbooks/8'); ?></li>
			<li><?php print l("Grade 9", 'etextbooks/9'); ?></li>
			<li><?php print l("Grade 10", 'etextbooks/10'); ?></li>
			<li><?php print l("Grade 11", 'etextbooks/11'); ?></li>
			<li><?php print l("Grade 12", 'etextbooks/12'); ?></li>
		</ul>
	</div>
	
	<div id="footer-licenses">
		<strong>Licenses</strong>
		<ul>
			<? // Currently all manually done, need to automate in future ?>
			<li><?php print l("CC BY", 'licenses/cc-by'); ?></li>
			<li><?php print l("CC BY-ND", 'licenses/cc-by-nd'); ?></li>
			<li><?php print l("CC BY-NC", 'licenses/cc-by-nc'); ?></li>
			<li><?php print l("CC BY-NC-ND", 'licenses/cc-by-nc-nd'); ?></li>
			<li><?php print l("CC BY-NC-SA", 'licenses/cc-by-nc-sa'); ?></li>
			<li><?php print l("CC BY-NC-ND", 'licenses/cc-by-sa'); ?></li>
		</ul>
	</div>
	
	<div id="footer-community">
		<strong>Community</strong>
		<ul>
			<li><?php print l("OC Board", 'board'); ?></li>
			<li><?php print l("Forums", 'forum'); ?></li>
			<li><?php print l("Get involved", 'get-involved'); ?></li>
			<li><?php print l("FAQs", 'faqs'); ?></li>
		</ul>
	</div>
	
	<div id="footer-subjects">
		<strong>Subjects</strong>
		<? // Currently all manually done, need to automate in future ?>
		<ul>
			<li><?php print l("Mathematics", 'mathematics'); ?></li>
			<li><?php print l("Science", 'science'); ?></li>
			<!--<li><?php print l("Computer Science", 'computer-science'); ?></li>-->
			<li><?php print l("English", "english"); ?></li>
			<li><?php print l("Geography", "geography"); ?></li>
			<li><?php print l("Government & Civics", "government-civics"); ?></li>
			<li><?php print l("History", "history"); ?></li>
		</ul>
	</div>
	
	<div id="footer-contact">
		<strong>Get in touch</strong>
		<ul>
			<li><?php print l("Contact us", 'contact'); ?></li>
			<li><a href="http://www.twitter.com/opencurriculum" target="_blank">Follow us on Twitter</a></li>
			<li><a href="https://plus.google.com/b/110490420045595560778/" target="_blank">Add us to your circle</a></li>
		</ul>
	</div>
	
	
<div style="clear: both;"> </div>
      <?php //print $feed_icons; ?>


    <?php if ($page['footer']): ?>
        <?php // print render($page['footer']); ?>
    <?php endif; ?>


</div>
</div>
