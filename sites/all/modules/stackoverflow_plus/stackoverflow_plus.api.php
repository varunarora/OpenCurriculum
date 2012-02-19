<?php

/**
 * @file
 * Provides API documentation for the stackoverflow_plus module.
 */

 /**
 * Implementation of stackoverflow_plus_fivestar_access().
 *
 * This hook is called before every vote is cast through Fivestar. It allows
 * modules to allow or deny voting on any type of entity, such as nodes, users, or
 * comments.
 *
 * @param $entity_type
 *   Type entity.
 * @param $id
 *   Identifier within the type.
 * @param $tag
 *   The VotingAPI tag string.
 * @param $uid
 *   The user ID trying to cast the vote.
 *
 * @return boolean or NULL
 *   Returns TRUE if voting is supported on this object.
 *   Returns NULL if voting is not supported on this object by this module.
 *   If needing to absolutely deny all voting on this object, regardless
 *   of permissions defined in other modules, return FALSE. Note if all
 *   modules return NULL, stating no preference, then access will be denied.
 *
 * @see fivestar_validate_target()
 * @see fivestar_fivestar_access()
 */
function hook_stackoverflow_plus_access($entity_type, $id, $tag, $uid) {
  if ($uid == 1) {
    // We are never going to allow the admin user case a stackoverflow_plus vote.
    return FALSE;
  }
}
 