/* example.c */
# include <stdio.h>
# include <cloog/cloog.h>

int main() {
  CloogState *state;
  CloogInput *input;
  CloogOptions *options ;
  struct clast_stmt *root;
  
  /* Setting options and reading program informations. */
  state = cloog_state_malloc();
  options = cloog_options_malloc(state);
  input = cloog_input_read(stdin, options);

  /* Generating and printing the code. */
  root = cloog_clast_create_from_input(input, options);
  clast_pprint(stdout, root, 0, options);

  cloog_clast_free(root);
  cloog_options_free(options) ;
  cloog_state_free(state);
  return 0;
}