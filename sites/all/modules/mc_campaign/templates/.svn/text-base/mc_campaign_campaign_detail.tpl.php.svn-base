<?php
?>
<h2><a href="/admin/campaign/campaign/edit/<?php print $fields->cid ?>"><?php print $fields->title ?></a></h2>

<div class="panel-block gettingstarted">
  <div class="unit size1of2 overflow alignc gs-step">
  	<b><?php echo t('Campaign Name') ?></b> : <?php print $fields->title ?><br/>
  	<b><?php echo t('Created') ?></b> : <?php print format_date($fields->created, 'short') ?><br/>
  	<b><?php echo t('Updated') ?></b> : <?php print format_date($fields->updated, 'short') ?><br/>
  </div>
  <div class="unit size1of2 overflow alignc gs-step">
  	<b><?php echo t('Type') ?></b> : <?php print $fields->type ?><br/>
  	<?php if($fields->type == 'Recurrent') {?>
  	<b><?php echo t('Frequency') ?></b> : <?php print $fields->frequency ?><br/>
  	<b><?php echo t('Next Execution') ?></b> : <?php print format_date($fields->next_execution, 'short') ?><br/>
  	<?php }?>
  </div>
  <div class="clear">&nbsp;</div>
</div>
<div class="clear">&nbsp;</div>

