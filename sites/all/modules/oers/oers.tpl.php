<div id="resourceButtons"><?php print l('+ Add external resource', 'node/add/oer/', array('query' => array('bookpage' => $node->nid ), 'attributes' => array('class' => 'button')));  ?>
 <?php print l('+ Create new resource', 'node/add/resource/', array('query' => array('bookpage' => $node->nid), 'attributes' => array('class' => 'button')));  ?></div><p/>
<?php $resources = $variables['items']; ?>
<?php foreach($resources as $id => $resource): ?>
<div class="resource">
<?php //debug($resource); ?>
<div class="resource_title">
<h2><?php print l($resource['title'], 'node/'.$resource['id']); ?></h2>
<div class="resourceInfo">Submitted by <?php print l($resource['user']['name'], 'user/'.$resource['user']['uid']); ?> on <?php print $resource['date']; ?> &#183; <img src="<?php print $resource['license']; ?>" />
</div>
</div>
<?php print substr($resource['body'], 0, 240); ?>...
<div class="resource_contents"><p/>
<?php

if (isset($resource['website'])){

if (preg_match('%(?:youtube\.com/(?:user/.+/|(?:v|e(?:mbed)?)/|.*[?&]v=)|youtu\.be/)([^"&?/ ]{11})%i', $resource['website'], $match)) {
    $video_id = $match[1];
    print "<iframe width=\"425\" height=\"349\" src=\"http://www.youtube.com/embed/".$video_id."\" frameborder=\"0\" allowfullscreen></iframe><p/>";
} else {
?>

<a href="<?php print $resource['website'] ?>" /><?php print $resource['website']; }}?></a>
<br/>

<?php print $resource['comments'] ?>
</div>
<a class="showmore">More &#187;</a>
</div>
<?php endforeach; ?>
