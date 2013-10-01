<?php include "head.php" ; ?>
<html>
<body>
<?php include "topmenu.php" ; ?>
<?php include "imgbar.php" ; ?>

<div id="page">
   <div id="content_full">
      <div class="post hr">

         <h2>Comet parameter: add_Z_user_amino_acid</h2>

         <ul>
         <li>This parameter allows users to define their own custom residue. Just
             encode the letter 'Z' in the input FASTA file and specify its mass here.
         <li>The letter 'Z' has no default mass.  So the mass entered here will
             be its residue mass.
         </ul>

         <p>Example:
         <br><tt>add_Z_user_amino_acid = 100.8</tt>

      </div>
   </div>
   <div style="clear: both;">&nbsp;</div>
</div>

<?php include "footer.php" ; ?>
</html>