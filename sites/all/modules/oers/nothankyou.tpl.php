<?php print $variables['items']; 
print drupal_render(drupal_get_form('user_login')); ?>

<?php print l('No, thank you. I do not want to create an account', $_GET['destination']); ?>
