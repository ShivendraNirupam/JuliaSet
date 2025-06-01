# Julia Set 

This project visualizes the **Julia Set**, a beautiful and complex fractal derived from iterative functions in the complex plane.

It is a C++ program using SDL2 to render the set by mapping pixels to complex numbers and calculating how quickly each point escapes to infinity under the iteration:

\[
z_{n+1} = z_n^2 + c
\]

---

##  Mathematical Theory

### The Iteration Function

Each pixel represents a complex number \( z_0 = x + iy \). The iteration follows:

\[
f_c(z) = z^2 + c
\]

- \( z \in \mathbb{C} \): the starting point for each pixel
- \( c \in \mathbb{C} \): a constant that defines the shape of the fractal

The point escapes if \( |z_n| > R \), where \( R \) is a fixed **escape radius**, commonly \( R = 2 \).

### Escape-Time Algorithm

1. Map each pixel \((px, py)\) to a complex number \( z \) using linear interpolation.
2. Iterate: \( z = z^2 + c \)
3. Count how many iterations before \( |z| > R \)
4. Color the pixel based on this count (the **escape time**)

---

### Coordinate Mapping

Pixels are mapped to the complex plane using a linear transformation:

\[
x = \text{map}(px, 0, \text{width}, -r, r)
\]
\[
y = \text{map}(py, 0, \text{height}, -r \cdot \frac{H}{W}, r \cdot \frac{H}{W})
\]

This ensures a correct aspect ratio when rendering complex numbers.

---

### Coloring Formula

Smooth coloring is applied based on the normalized iteration count \( t \):

\[
t = \frac{\text{iteration count}}{\text{max iterations}}
\]

Then:

- \( R = 9(1 - t)t^3 \cdot 255 \)
- \( G = 15(1 - t)^2t^2 \cdot 255 \)
- \( B = 8.5(1 - t)^3t \cdot 255 \)

This gives a visually pleasing gradient that reflects the "speed" of escape.

---

##  Dynamic Exploration

The complex constant \( c \) can be modified interactively:

- Press **Spacebar** and move the mouse to update \( c \)
- Click to lock the value

Each new \( c \) reveals a different **Julia set**, a new pattern in the universe of complex dynamics.

---

##  Summary Table

| Symbol | Description |
|--------|-------------|
| \( z \) | Complex number corresponding to a pixel |
| \( c \) | Constant defining the Julia set |
| \( R \) | Escape radius (default: 2.0) |
| \( N \) | Maximum number of iterations |
| \( t \) | Normalized escape iteration |
| \( z_{n+1} \) | Next value in the iteration: \( z_n^2 + c \) |

---

## ⚙️ Compilation

Ensure SDL2 is installed. Compile and run with:

```bash
g++ juliaset.cpp -o main $(sdl2-config --cflags --libs)
./main
