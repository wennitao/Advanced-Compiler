# Advanced Compiler
Advanced Compiler Assignment of ACM Class

## Loop Transformation on Polyhedral 

Idea from "A Practical Automatic Polyhedral Parallelizer and Locality Optimizer".

C code -> Polyhedral extraction (clan) -> Dependence analysis (candl) -> **Loop Transformation** -> Code generation (CLooG) -> C code

Polyhedral format: SCoP (Static Control Part) https://github.com/periscop/openscop

Reference: clay https://github.com/periscop/clay

```C
Types:

  array:  [n1, n2, ...]
          array of integer

  bool:   1 | 0 | true | false

  string: "blah"

  list:   {n1, n2, ... | n3, n4, ... | ...}
          list of arrays

  multi:  any object/variable

  beta_loop    The ident corresponds to a loop
  beta_inner   In the interchange, it corresponds to the inner loop
               (or statement)

Variables: For the moment only variables a-z are available.

Available functions:

  void split(array beta, uint depth)
  void reorder(array beta_loop, array neworder)
  void interchange(array beta_inner, uint depth_1, uint depth_2, bool pretty)
  void reverse(ident, uint depth)
  void fuse(array beta_loop)
  void skew(array beta, uint depth, uint depth_other, int coeff)
  void iss(array beta_loop, list inequation { ((output,) params,)) const }
  void stripmine(array beta, uint depth, uint size)
  void unroll(array beta_loop, uint factor)
  void unroll_noepilog(array beta_loop, uint factor)
  void tile(array beta, uint depth, uint depth_outer, uint size)
  void shift(array beta, uint depth, array params, int constant)
  void peel(array beta_loop,  list inequation { (params,) const })
  void context(array vector)
  void dimreorder(array beta, uint #access, array neworder)
  void dimprivatize(array beta, uint #access, uint depth)
  void dimcontract(array beta, uint #access, uint depth)
  int add_array(string name)
  array get_beta_loop(uint n >= 0)
  array get_beta_stmt(uint n >= 0)
  array get_beta_loop_by_name(string iterator)
  int get_array_id(string name)
  void print(multi)
  void replace_array(uint last_id, uint new_id)
  void datacopy(uint id_copy, uint id_orig, array beta_insert,
                bool before, array beta_get_domain)
  void break()
  void block(array beta_stmt1, beta_stmt2)
  void grain(array beta, int depth, int factor)
  void densify(array beta, int depth)
  void reshape(array beta, int depth, int iterator, int amount)
  void collapse(array beta_loop)
  void linearize(array beta_loop, int depth)
  void embed(array beta_stmt)
  void unembed(array beta_stmt)
```

### Example 

```C
#pragma scop
/* Clay
block([0,0], [1,0]);
*/

for (i = 0 ; i <= N ; i++) 
  a[i] = 0;

for (i = 0 ; i <= N ; i++)
  b[i] = 0;

#pragma endscop
```

Use clan to extract polyhedral format from C code: 
```
# [File generated by the OpenScop Library 0.9.2]

<OpenScop>

# =============================================== Global
# Language
C

# Context
CONTEXT
0 3 0 0 0 1

# Parameters are provided
1
<strings>
N
</strings>

# Number of statements
2

# =============================================== Statement 1
# Number of relations describing the statement:
3

# ----------------------------------------------  1.1 Domain
DOMAIN
3 4 1 0 0 1
# e/i|  i |  N |  1  
   1    1    0    0    ## i >= 0
   1   -1    1    0    ## -i+N >= 0
   1    0    1    0    ## N >= 0

# ----------------------------------------------  1.2 Scattering
SCATTERING
3 7 3 1 0 1
# e/i| c1   c2   c3 |  i |  N |  1  
   0   -1    0    0    0    0    0    ## c1 == 0
   0    0   -1    0    1    0    0    ## c2 == i
   0    0    0   -1    0    0    0    ## c3 == 0

# ----------------------------------------------  1.3 Access
WRITE
2 6 2 1 0 1
# e/i| Arr  [1]|  i |  N |  1  
   0   -1    0    0    0    3    ## Arr == a
   0    0   -1    1    0    0    ## [1] == i

# ----------------------------------------------  1.4 Statement Extensions
# Number of Statement Extensions
1
<body>
# Number of original iterators
1
# List of original iterators
i
# Statement body expression
a[i] = 0;
</body>

# =============================================== Statement 2
# Number of relations describing the statement:
3

# ----------------------------------------------  2.1 Domain
DOMAIN
3 4 1 0 0 1
# e/i|  i |  N |  1  
   1    1    0    0    ## i >= 0
   1   -1    1    0    ## -i+N >= 0
   1    0    1    0    ## N >= 0

# ----------------------------------------------  2.2 Scattering
SCATTERING
3 7 3 1 0 1
# e/i| c1   c2   c3 |  i |  N |  1  
   0   -1    0    0    0    0    1    ## c1 == 1
   0    0   -1    0    1    0    0    ## c2 == i
   0    0    0   -1    0    0    0    ## c3 == 0

# ----------------------------------------------  2.3 Access
WRITE
2 6 2 1 0 1
# e/i| Arr  [1]|  i |  N |  1  
   0   -1    0    0    0    4    ## Arr == b
   0    0   -1    1    0    0    ## [1] == i

# ----------------------------------------------  2.4 Statement Extensions
# Number of Statement Extensions
1
<body>
# Number of original iterators
1
# List of original iterators
i
# Statement body expression
b[i] = 0;
</body>

# =============================================== Extensions
<scatnames>
b0 i b1
</scatnames>

<arrays>
# Number of arrays
4
# Mapping array-identifiers/array-names
1 i
2 N
3 a
4 b
</arrays>

<coordinates>
# File name
block.c
# Starting line and column
2 0
# Ending line and column
12 0
# Indentation
0
</coordinates>

<clay>
block([0,0], [1,0]);
</clay>

</OpenScop>
```

After block transformation: 
```
# [File generated by the OpenScop Library 0.9.2]

<OpenScop>

# =============================================== Global
# Language
C

# Context
CONTEXT
0 3 0 0 0 1

# Parameters are provided
1
<strings>
N
</strings>

# Number of statements
1

# =============================================== Statement 1
# Number of relations describing the statement:
4

# ----------------------------------------------  1.1 Domain
DOMAIN
3 4 1 0 0 1
# e/i|  i |  N |  1  
   1    1    0    0    ## i >= 0
   1   -1    1    0    ## -i+N >= 0
   1    0    1    0    ## N >= 0

# ----------------------------------------------  1.2 Scattering
SCATTERING
3 7 3 1 0 1
# e/i| c1   c2   c3 |  i |  N |  1  
   0   -1    0    0    0    0    0    ## c1 == 0
   0    0   -1    0    1    0    0    ## c2 == i
   0    0    0   -1    0    0    0    ## c3 == 0

# ----------------------------------------------  1.3 Access
WRITE
2 6 2 1 0 1
# e/i| Arr  [1]|  i |  N |  1  
   0   -1    0    0    0    3    ## Arr == a
   0    0   -1    1    0    0    ## [1] == i

WRITE
2 6 2 1 0 1
# e/i| Arr  [1]|  i |  N |  1  
   0   -1    0    0    0    4    ## Arr == b
   0    0   -1    1    0    0    ## [1] == i

# ----------------------------------------------  1.4 Statement Extensions
# Number of Statement Extensions
1
<body>
# Number of original iterators
1
# List of original iterators
i
# Statement body expression
{a[i] = 0;b[i] = 0;}
</body>

# =============================================== Extensions
<scatnames>
b0 i b1
</scatnames>

<arrays>
# Number of arrays
4
# Mapping array-identifiers/array-names
1 i
2 N
3 a
4 b
</arrays>

<coordinates>
# File name
block.c
# Starting line and column
2 0
# Ending line and column
12 0
# Indentation
0
</coordinates>

</OpenScop>


```

Use CLooG to generate C code: 
```C
#pragma scop

if (N >= 0) {
  for (i=0;i<=N;i++) {
    {a[i] = 0;b[i] = 0;}
  }
}
#pragma endscop
```

## Code Example

See `example/poc.c`. 

It is an example of a source (to polyhedra) to source compiler. 

```bash
gcc -DCLOOG_INT_GMP poc.c -lcloog-isl -lclan -losl -o poc
./poc scop.c
```

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

Data Dependence Analyzer in the Polyhedral Model

https://github.com/periscop/candl

(Probably you don't need this.)

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
./configure
make
make check
make install
```

Try to compile `example/poc.c` by `gcc -DCLOOG_INT_GMP poc.c -lcloog-isl -lclan -losl -o poc` to test you have installed cloog successfully.
Then `./poc fuse.c`, you will see SCoP and the generated C code of `fuse.c`.