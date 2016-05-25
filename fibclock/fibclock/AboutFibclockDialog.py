# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
### BEGIN LICENSE
# This file is in the public domain
### END LICENSE

import gettext
from gettext import gettext as _
gettext.textdomain('fibclock')

import logging
logger = logging.getLogger('fibclock')

from fibclock_lib.AboutDialog import AboutDialog

# See fibclock_lib.AboutDialog.py for more details about how this class works.
class AboutFibclockDialog(AboutDialog):
    __gtype_name__ = "AboutFibclockDialog"
    
    def finish_initializing(self, builder): # pylint: disable=E1002
        """Set up the about dialog"""
        super(AboutFibclockDialog, self).finish_initializing(builder)

        # Code for other initialization actions should be added here.

