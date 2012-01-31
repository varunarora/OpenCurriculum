<?php
// $Id: user-profile.tpl.php,v 1.13 2010/01/08 07:04:09 webchick Exp $

/**
 * @file
 * Default theme implementation to present all user profile data.
 *
 * This template is used when viewing a registered member's profile page,
 * e.g., example.com/user/123. 123 being the users ID.
 *
 * Use render($user_profile) to print all profile items, or print a subset
 * such as render($content['field_example']). Always call render($user_profile)
 * at the end in order to print all remaining items. If the item is a category,
 * it will contain all its profile items. By default, $user_profile['summary']
 * is provided which contains data on the user's history. Other data can be
 * included by modules. $user_profile['user_picture'] is available
 * for showing the account picture.
 *
 * Available variables:
 *   - $user_profile: An array of profile items. Use render() to print them.
 *   - Field variables: for each field instance attached to the user a
 *     corresponding variable is defined; e.g., $user->field_example has a
 *     variable $field_example defined. When needing to access a field's raw
 *     values, developers/themers are strongly encouraged to use these
 *     variables. Otherwise they will have to explicitly specify the desired
 *     field language, e.g. $user->field_example['en'], thus overriding any
 *     language negotiation rule that was previously applied.
 *
 * @see user-profile-category.tpl.php
 *   Where the html is handled for the group.
 * @see user-profile-item.tpl.php
 *   Where the html is handled for each item in the group.
 * @see template_preprocess_user_profile()
 */
?>

<?php drupal_set_title($field_name[0]['value']); ?>
<div id="usual1" class="usual"> 
  <ul> 
    <li><h2><a href="#about" class="selected">About</a></h2></li> 
    <?php if (isset($field_body)): ?><li><h2><a href="#mypage"><?php print $field_name[0]['safe_value']; ?>'s page</a></h2></li> <?php endif; ?>
    <li><h2><a href="#activity">Activity</a></h2></li>	
  </ul> 
  <div id="about"> 
<div class="profile"<?php print $attributes; ?>>
<div id="imagePanel">
<?php print render($user_profile['user_picture']); ?><p/>
<div id="profile-points-wrapper"><span id="profile-points"><?php if(isset($field_points[0]['value'])) print $field_points[0]['value']; else print '0'; ?></span><br/>points</div>
</div>
<div id="profileInfo">
<?php hide($user_profile['field_name']); ?>
<?php hide($user_profile['field_interests']);?>
<?php hide($user_profile['field_points']);?>
<?php hide($user_profile['field_twitter']);?>
<?php hide($user_profile['field_feed']);?>
<?php hide($user_profile['summary']);?>
<table id="user_profile">


<?php if (isset($field_bio)): ?>
<tr><td>Biography</td><td><?php print render($user_profile['field_bio']); ?></td></tr>
<?php endif; ?>

<?php if (isset($field_org)): ?>
<tr><td>Organization</td><td><?php print render($user_profile['field_org']); ?></td></tr>
<?php endif; ?>

<?php
$account = user_load(arg(1));
if (isset($account->mail)):
?>

<tr><td>Email</td><td><a href="mailto:<?php print str_replace('@', '(at)', str_replace('.', '(dot)', $account->mail)); ?>"><img src="../sites/default/image.php?email=<?php print $account->mail; ?>" /></a></td></tr>
<?php endif; ?>

<?php if (isset($field_personalweb)): ?>
<tr><td>Website</td><td><?php print render($user_profile['field_personalweb']); ?></td></tr>
<?php endif; ?>

<?php if (isset($field_interests)): ?>
<tr><td>Key Interests</td><td>
	<?php 
	$new_output = "";
	$interests = array();
	
	foreach($field_interests as $interest){
		$interests[] = $interest['taxonomy_term']->name;
	}
	
	$i=1;

	if (isset($interests[0])){
		$new_output =  $interests[0];
	}

	while($i<count($interests)):
		$new_output .=  ', ' .$interests[$i++];
	endwhile;

	print $new_output;
	?>
</td></tr>
<?php endif; ?>

<?php if (isset($group_audience)): ?>
<tr><td>Groups</td><td><?php print render($user_profile['group_audience']); ?></td></tr>
<?php endif; ?>

<?php if (isset($field_twitter)): ?>
<tr><td>Twitter @</td><td id="twitter_handle"><?php print render($user_profile['field_twitter']); ?></td></tr>
<?php endif; ?>

<?php print render($user_profile); ?>
</table>
<p/>
</div>
</div>



</div>

<div id="mypage">
	<div>
		<?php if (isset($field_body)): ?>
			<?php print render($user_profile['field_body']); ?>
		<?php endif; ?>

	</div>
</div>

<div id="activity">
	<!--
			<?php if (isset($field_feed)): ?>
			<?php print render($user_profile['field_feed']); ?>
		<?php endif; ?>-->
			<?php print render($user_profile['summary']); ?>
			
<?php if(isset($field_twitter[0]['value'])) { ?> <div class="tweet"></div> <?php } ?>
</div>

</div>
