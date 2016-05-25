# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
### BEGIN LICENSE
# This file is in the public domain
### END LICENSE

import gettext
from gettext import gettext as _
gettext.textdomain('fibclock')

from gi.repository import Gtk # pylint: disable=E0611
import logging
logger = logging.getLogger('fibclock')

from fibclock_lib import Window
from fibclock.AboutFibclockDialog import AboutFibclockDialog
from fibclock.PreferencesFibclockDialog import PreferencesFibclockDialog

# See fibclock_lib.Window.py for more details about how this class works
class FibclockWindow(Window):
    __gtype_name__ = "FibclockWindow"
    
    def finish_initializing(self, builder): # pylint: disable=E1002
        """Set up the main window"""
        super(FibclockWindow, self).finish_initializing(builder)

        self.AboutDialog = AboutFibclockDialog
        self.PreferencesDialog = PreferencesFibclockDialog

        # Code for other initialization actions should be added here.

