<?php

/**
 * @file
 */

function _stormticket_task_tickets_js($organization_nid=0, $project_nid=0, $task_nid=0) {
  $tickets = array();

  $s = "SELECT n.nid, n.title FROM {node} n INNER JOIN {stormticket} sti
  ON n.vid=sti.vid WHERE n.status=1 AND n.type='stormticket' AND sti.organization_nid=%d AND sti.project_nid=%d
  AND sti.task_nid=%d ORDER BY n.title";

  $s = stormticket_access_sql($s);
  $s = db_rewrite_sql($s);
  $r = db_query($s, $organization_nid, $project_nid, $task_nid);
  while ($item = db_fetch_object($r)) {
    $nid = $item->nid;
    $tickets[$nid] = $item->title;
  }
  print drupal_to_js($tickets);
  exit();
}
