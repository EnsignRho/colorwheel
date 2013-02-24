SET STATUS OFF
SET BELL OFF
SET DOHISTORY OFF
SET TALK OFF
SET ENGINEBEHAVIOR 70
SET STATUS BAR ON
SET SAFETY OFF

* Declare our required DLLs
DO declare_colorwheel_dll.prg


* Display the main form
DO FORM frmMain


* Wait until we're done
_vfp.Visible = .f.

READ EVENTS

_vfp.Visible = .t.
SET TALK ON
