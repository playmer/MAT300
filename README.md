Joshua T. Fisher
MAT 300

Build Instructions:
  1. Must have a very recent version of Visual Studio 2017 (recommended: 15.5)
  2. Once opened, make sure to select x64, the x86 configuration is not
     supported.
  3. Build as normal. (Select Build, Press Build All)

Controls:
  1. Input Points can be added by moving the horizontal slider at the top 
     of the main window.
  2. Input Points can have their Y value manipulated by using the vertical
     sliders below the control point slider, as well as clicking and dragging
     the points themselves. Though the second might be a little finicky. I'm
     still new at picking.
  3. The graph can be moved around by using the arrow keys.
  4. You can zoom in and out using page up and page down.
  5. The project can be selected using the drop down below the vertical
     sliders.
  6. In the case of Project 1 (this submission), NLI or BB can be selected
     using the radio buttons below the project drop down.
  7. The Options Window can be moved around, resized, or "minimized" as desired.
     The resizing tool is at the bottom right of the Options Window, it's movable
     by clicking anywhere inside it there isn't a button or some such and dragging,
     and it can be "minimized" or unminimized by simply clicking the arrow in the
     top left.
  8. Input points can be reset to y = 1.0 by simply moving the Input Point bar.

Notes/Issues:
  1. BB currently has has issues beyond 21 control points, since 20 is the 
     max for testing, I'll cap the input at 20.
  2. The actual project math is done in Projects.cpp