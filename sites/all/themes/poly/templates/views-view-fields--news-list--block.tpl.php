<?php
/**
 * @file views-view-fields.tpl.php
 * Default simple view template to all the fields as a row.
 *
 * - $view: The view in use.
 * - $fields: an array of $field objects. Each one contains:
 *   - $field->content: The output of the field.
 *   - $field->raw: The raw data for the field, if it exists. This is NOT output safe.
 *   - $field->class: The safe class id to use.
 *   - $field->handler: The Views field handler object controlling this field. Do not use
 *     var_export to dump this object, as it can't handle the recursion.
 *   - $field->inline: Whether or not the field should be inline.
 *   - $field->inline_html: either div or span based on the above flag.
 *   - $field->wrapper_prefix: A complete wrapper containing the inline_html to use.
 *   - $field->wrapper_suffix: The closing tag for the wrapper.
 *   - $field->separator: an optional separator that may appear before a field.
 *   - $field->label: The wrap label text to use.
 *   - $field->label_html: The full HTML of the label to use including
 *     configured element type.
 * - $row: The raw result object from the query, with all data it fetched.
 *
 * @ingroup views_templates
 */
$date = $fields['field_news_date']->content;
$date = str_replace($fields['field_news_date']->wrapper_prefix, "", $date);
$date = str_replace($fields['field_news_date']->wrapper_suffix, "", $date);

$year = date('Y',$date);
$month = date('m',$date);
$day = date('d',$date);

// TODO ADD ANCHOR
// <a name="tips">Useful Tips Section</a> 
?>

<div class="hentry">
	<abbr title="2011-12-20T14:51:07+00:00" class="postdate published">
		<span class="month m-<?php print $month;?>">Dec</span>
		<span class="day d-<?php print $day;?>">20</span>
        <span class="year y-<?php print $year;?>">2011</span>
	</abbr>
	<div class="post">
	<h3><?php echo $fields['title']->content;?></h3>
	<div class="entry">
	<p>
	<?php echo $fields['body']->content;?>
	</p>
	</div>
	</div>
</div>