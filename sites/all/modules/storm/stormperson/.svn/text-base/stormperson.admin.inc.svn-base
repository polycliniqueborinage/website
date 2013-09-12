<?php

/**
 * @file
 */

function _stormperson_organization_people_js($organization_nid=0) {
  $people = array();

  if ($organization_nid) {
    $s = "SELECT n.nid, n.title FROM {node} n INNER JOIN {stormperson} AS spe ON n.vid=spe.vid WHERE n.status=1 AND n.type='stormperson' AND spe.organization_nid=%d ORDER BY n.title";    $s = stormperson_access_sql($s);
    $s = db_rewrite_sql($s);
    $r = db_query($s, $organization_nid);

    while ($item = db_fetch_object($r)) {
      $nid = $item->nid;
      $people[$nid] = $item->title;
    }
  }
  print drupal_to_js($people);
  exit();
}
