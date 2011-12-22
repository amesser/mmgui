Multimeter Gui 0.0
==================

Multimeter Gui is graphical tool to capture/read out
digital multimeters from a pc.

Compiling / Installing
----------------------

The following software is required to run mmgui:

- Qt-Toolkit version 4 and above
- Qwt Library
- libvc-gdm70x version 0.2.2 and above

The following addional software is required for
compiling mmgui:

- waf meta build system
- Qt Development Tools (moc and ui mainly)

In order to compile mmgui the following command
shall be run from mmgui source distribution root
directory::

  $ waf configure build

This creates the folder structure 'build' conatining
the application executable 'build/src/mmgui' which
can be run without installing.

If the programm should be installed locally, run the
following as root::

  $ waf install




