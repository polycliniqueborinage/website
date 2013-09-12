<?php
?>
<h2><?php print $fields->firstname.' '.$fields->lastname ?></h2>

<div class="panel-block gettingstarted">
  <div class="unit size1of2 overflow alignc gs-step">
  	<b><?php echo t('First Name') ?></b> : <?php print $fields->firstname ?><br/>
  	<b><?php echo t('Last Name') ?></b> : <?php print $fields->lastname ?><br/>
  	<b><?php echo t('Created') ?></b> : <?php print format_date($fields->created, 'short') ?><br/>
  	<b><?php echo t('Updated') ?></b> : <?php print format_date($fields->updated, 'short') ?><br/>
  </div>
  <div class="unit size1of2 overflow alignc gs-step">
  	<b><?php echo t('Email') ?></b> : <?php print $fields->email ?><br/>
  	<b><?php echo t('Mobile') ?></b> : <?php print $fields->mobile ?><br/>
  	<b><?php echo t('Fixed Line') ?></b> : <?php print $fields->fixedline ?><br/>
  </div>
  <div class="clear">&nbsp;</div>
</div>
<div class="clear">&nbsp;</div>

