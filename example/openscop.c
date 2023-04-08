/* example.c */
# include <stdio.h>
# include <osl/osl.h>

int main() {
  osl_scop_p scop;

  // Read the OpenScop file.
  scop = osl_scop_read(stdin);

  // Dump the content of the scop data structure.
  osl_scop_dump(stdout, scop);

  // Save the planet.
  osl_scop_free(scop);

  return 0;
}

