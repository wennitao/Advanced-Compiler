# Advanced Compiler

Advanced Compiler Assignment of ACM Class

## Loop Transformation on Polyhedral

Idea from "A Practical Automatic Polyhedral Parallelizer and Locality Optimizer".

C code → Polyhedral extraction (clan) → Dependence analysis (candl) → **Loop Transformation** → Code generation (CLooG) → C code

Polyhedral format: SCoP (Static Control Part)
https://github.com/periscop/openscop

Reference: clay 
https://github.com/periscop/clay

## Implementation

```C
/**
 * split function:
 * Split the loop into two parts at the depth-th level from the statement
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * depth
 * return status
 */
int split(osl_scop_p scop, std::vector<int> statementID, unsigned int depth);

/**
 * reorder function:
 * Reorders the statements in the loop
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * neworder: the new order of the statements
 * return status
 */
int reorder(osl_scop_p scop, std::vector<int> statementID, std::vector<int> neworder) ;

/**
 * interchange function:
 * On each statement which belongs to the node, the loops that match the
 * depth_1-th and the depth_2 are interchanged
 * given the inner loop
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * depth_1, depth_2: >= 1
 * pretty: 1 or 0 : whether update the scatnames
 * return status
 */
int interchange(osl_scop_p scop,
                std::vector<int> statementID,
                unsigned int depth_1, unsigned int depth_2,
                int pretty) ;

/**
 * fuse function:
 * Fuse loop with the first loop after
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * return status
 */
int fuse(osl_scop_p scop, std::vector<int> statementID) ;

/**
 * skew function
 * Transform the iteration domain so that the loop at depth depends on the
 * loop iterator at depth_other: in all occurrences, the loop iterator i
 * of the former loop is replaced by (i + coeff*j) where j is the loop iterator
 * of the latter loop.  Adjusts the loop boundaries accordingly.
 * Skewing the loop by its own iterator, i.e. depth == depth_outer, is invalid
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * depth: 1-based depth of the output loop to modify
 * depth_other: 1-based depth of the loop iterator to add
 * coeff: the coefficient to multiply the dimension by
 * return status
 */
int skew(osl_scop_p scop,
         std::vector<int> statementID,
         unsigned int depth,
         unsigned int depth_other,
         int coeff) ;

/**
 * tile function:
 * Do tiling on the loop at depth with size, the outer loop is at depth_outer
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * depth: tiling on loop at depth
 * depth_outer: outer loop depth
 * size: tiling size
 * return status
*/
int tile(osl_scop_p scop,
         std::vector<int> statementID, unsigned int depth, unsigned int depth_outer,
         unsigned int size) ;

// BONUS
/** unroll function
 * Unroll a loop
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * factor: unroll factor
 * return status
 */
int unroll(osl_scop_p scop, std::vector<int> statementID, unsigned int factor) ;
```

### Example

```C
#pragma scop
/* Clay
   fuse([1]);
*/
a = 0;
for(i = 0 ; i <= N ; i++) {
  b[i] = 0;
  c[i] = 0;
  d[i] = 0;
}
for(i = 0 ; i <= N ; i++) {
  t[i] = 0;
  s[i] = 0;
}
f = 0;
#pragma endscop
```

Use clan to extract polyhedral format from C code.
See `example/fuse.c.orig.scop`.

After loop fusion, see `example/fuse.c.clay.scop`.

Use CLooG to generate C code:

```C
a = 0;
for (i=0;i<=N;i++) {
  b[i] = 0;
  c[i] = 0;
  d[i] = 0;
  t[i] = 0;
  s[i] = 0;
}
f = 0;
```

## Code Structure

See `example/poc.cpp`. 

It is an example of a source (to polyhedra) to source compiler. 

```bash
g++ -DCLOOG_INT_GMP poc.cpp -lcloog-isl -lclan -losl -o poc
./poc scop.c
```

You can find detailed documentation of these libraries in `doc/`, including data structure and function descriptions.

## Environment 

**Requirements**: `autoconf`, `automake`, `libtool`, `GMP`, `bison >= 2.4`

If you are a Mac user, you may need to run to set default library and include path.

```bash
/bin/launchctl setenv LIBRARY_PATH /usr/local/lib
/bin/launchctl setenv CPATH /usr/local/include
```

(I don't know what makes me to successfully compile it on my Mac.)

### Clan

Chunky Loop Analyzer: A Polyhedral Representation Extraction Tool for High Level Programs

https://github.com/periscop/clan

```bash
git clone https://github.com/periscop/clan.git
cd clan
./autogen.sh
./configure
make
make check
make install
```

If your make does not make doc, you can install texinfo by 
`sudo apt install texinfo` and go to `doc` subfolder and `makeinfo --html --no-split -o clan.html clan.texi`.

Try to compile `example/clan.c` by `gcc clan.c -losl -lclan -o clan` to test you have installed clan successfully.
Then `more fuse.c | ./clan` to see the corresponding SCoP of `fuse.c`.

### Candl

**You can finish this assignment without checking data dependence.**

Data Dependence Analyzer in the Polyhedral Model

https://github.com/periscop/candl

```bash
git clone https://github.com/periscop/candl.git
cd candl
./get_submodules.sh
./autogen.sh
./configure --with-piplib=bundled --with-scoplib=bundled
make
make install
```

### CLooG

The CLooG Code Generator in the Polytope Model

https://github.com/periscop/cloog

```bash
git clone https://github.com/periscop/cloog.git
cd cloog
./get_submodules.sh
./autogen.sh
./configure --with-osl=system
make
make check
make install
```

Try to compile `example/poc.c` by `gcc -DCLOOG_INT_GMP poc.c -lcloog-isl -lclan -losl -o poc` to test you have installed cloog successfully.
Then `./poc fuse.c`, you will see SCoP and the generated C code of `fuse.c`.