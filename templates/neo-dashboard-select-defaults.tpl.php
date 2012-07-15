<?php
// $Id: neo-dashboard-select-defaults.tpl.php,v 1.1.2.1 2010/07/28 16:27:36 beretta627 Exp $

/**
 * @file neo-dashboard-select-defaults.tpl.php
 * Default theme implementation to select default widgets from all available blocks.
 *
 */
?>
<?php
  // Add table javascript
  drupal_add_js('misc/tableheader.js');
?>
<p>Select blocks from the "User" column to be added to the default available widgets list on a user's Neo dashboard. Widgets that are removed from this column are <em>not</em> removed from a user's Neo dashboard until explicitly removed by the user.  Alternatively, you may choose to make a widget to be permanent on a user's Neo dashboard by clicking on the relevant block in the "System" column.</p> <table id="select-defaults" class="sticky-enabled">
  <thead>
    <tr>
      <th><?php print t('Block'); ?></th>
      <th><?php print t('System'); ?></th>
      <th><?php print t('User'); ?></th>
    </tr>
  </thead>
  <tbody>
  <?php $row = 0; ?>
  <?php foreach($block_options as $key => $block): ?>
    <tr class="<?php print $row % 2 == 0 ? 'odd' : 'even'; ?>">
      <td><?php print $block['title']; ?></td>
      <td><?php print $block['permanent']; ?></td>
      <td><?php print $block['operation']; ?></td>
    </tr>
    <?php $row++; ?>
  <? endforeach; ?>
  </tbody>
</table>
