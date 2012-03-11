<div id="loginPane">
<div id="loggedOut">
<?php if ($logged_in){
	$account = user_load($user->uid);
		if (isset($account->field_name['und'])){
		print l($account->field_name['und'][0]['safe_value'], 'user'); /*l('(edit)', 'user/'.$user->uid.'/edit') .*/
		print '<span id="points">'.  l($account->field_points['und'][0]['value'], 'user/'.$user->uid) . '</span>';
		if($account->field_notifications['und'][0]['value'] > 0){
			print '<span id="notifications">' . l($account->field_notifications['und'][0]['value'], 'user/'.$user->uid.'/messages') . '</span>';
		}
	}
	else print l("My profile", 'user') . l('(edit)', 'user/'.$user->uid.'/edit');
?>
&#183; <?php print l('My Content', 'admin/workbench'); ?>
&#183; <?php print l('Logout', 'user/logout'); ?>
<?php } else {?>
<a href="#" id="login">Login</a> &#183; <?php print l('Create account', 'user/register'); ?> &#183; <?php print l('Forgot password', 'user/password'); ?>
<?php } ?>
</div>

</div>

<div id="loginBox">
	<a id="closeBox"><img src="<?php print path_to_theme(); ?>/images/spacer.png" width="15px" height="15px"></a>
	<?php if ($page['login']): ?>
    <?php print render($page['login']); ?>
	<?php endif; ?>
</div>