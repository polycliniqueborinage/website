<?php
?>
<h2><a href="/admin/campaign/lists/list/edit/<?php print $fields->lid ?>"><?php print $fields->title ?></a></h2>

<div class="panel-block gettingstarted">
  <div class="unit size1of2 overflow alignc gs-step">
  	<b><?php echo t('List Name') ?></b> : <?php print $fields->title ?><br/>
  	<b><?php echo t('Description') ?></b> : <?php print $fields->description ?><br/>
  	<b><?php echo t('Subscribers') ?></b> : <?php print $fields->subscribers ?><br/>
  	<b><?php echo t('Created') ?></b> : <?php print format_date($fields->created, 'short') ?><br/>
  	<b><?php echo t('Updated') ?></b> : <?php print format_date($fields->updated, 'short') ?><br/>
  </div>
  <div class="unit size1of2 overflow alignc gs-step">
  	<b><?php echo t('Type') ?></b> : <?php print $fields->type ?><br/>
  	<?php if($fields->type == 'request') {?>
  	<b><?php echo t('Frequency') ?></b> : <?php if ($fields->request_frequency != '-1') print $fields->request_frequency; else print t('Never'); ?><br/>
  	<b><?php echo t('Limit') ?></b> : <?php print $fields->request_limit ?><br/>
  	<b><?php echo t('Max Size') ?></b> : <?php print $fields->size ?><br/>
  	<b><?php echo t('Next Reload') ?></b> : <?php if ($fields->request_frequency != '-1') print format_date($fields->request_next_reload, 'short'); else print t('Never'); ?><br/>
  	<?php }?>
  </div>
  <div class="clear">&nbsp;</div>
</div>
<div class="clear">&nbsp;</div>

