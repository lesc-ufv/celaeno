# Incidence Matrix

> An _incidence matrix_ representation for a simple graph \\(G(V,E)\\) is a
matrix \\(I\\), where:

\\[ I[u,v] =
  \\begin{cases}
    1\\text{ if edge uv exists} \\\\
    0\\text{ otherwise}
  \\end{cases}
\\]

Consider the two-layered graph shown in Figure 1.

<center>
<img src="./img/incidence.png">
</center>

There is one incidence matrix \\(I_0\\) between levels \\(L_0\\) and \\(L_1\\),
another matrix \\(I_1\\) between levels \\(L_1\\) and \\(L_2\\), and, lastly,
matrix \\(I_2\\) between levels \\(L_2\text{ and }L_3\\). These are
depicted as follows:

\\[ I_0 =
  \\left[
    \\begin{array}[ccc]
    {}1 & 1 & 0 \\\\
     0 & 0 & 1
    \\end{array}
  \\right]
\\]

Where the rows are \\(a\\) and \\(b\\) and the columns,
\\(c,d\text{ and }e\\), respectively.

\\[ I_1 =
  \\left[
    \\begin{array}[ccc]
    {}1 & 0 & 0 \\\\
     0 & 1 & 0 \\\\
     0 & 0 & 1
    \\end{array}
  \\right]
\\]

Where the rows are \\(c,d\text{ and }e\\), and the columns,
\\(f,g\text{ and }h\\), respectively.

\\[ I_2 =
  \\left[
    \\begin{array}[c]
    {}1 \\\\
      1 \\\\
      1 
    \\end{array}
  \\right]
\\]

Lastly, matrix \\(L_2\\) Where the rows are \\(f,g\text{ and }h\\), and the
column, is \\(I\\), respectively.


<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
