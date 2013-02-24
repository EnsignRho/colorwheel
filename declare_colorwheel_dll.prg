* DECLARE_COLORWHEEL_DLL.PRG
* Indicate what form should be used to draw the color image, and at what coordinates
DECLARE				colorwheel_set_subclass_form IN colorwheel.dll ;
						INTEGER	tnHwnd, ;
						INTEGER	tnX, ;
						INTEGER	tnY, ;
						INTEGER	tnWidth, ;
						INTEGER tnHeight

* Un-subclass upon exit
DECLARE				colorwheel_un_subclass_form IN colorwheel.dll

* Indicate the new / updated color image that should be drawn
DECLARE				colorwheel_set_rgb_grayscale_adjustments IN colorwheel.dll ;
						INTEGER	tnRed, ;
						INTEGER	tnGrn, ;
						INTEGER	tnBlu, ;
						INTEGER	tnGray, ;
						INTEGER	tnPastel, ;
						INTEGER	tnLinear, ;
						INTEGER tnRotation

* Returns the RGB() value for the color at X,Y within the color image
DECLARE INTEGER		colorwheel_get_rgb_at_xy IN colorwheel.dll ;
						INTEGER	@tnX, ;
						INTEGER	@tnY

* Sets the RGB() value the user is using
DECLARE INTEGER		colorwheel_set_rgb IN colorwheel.dll ;
						INTEGER	tnRgb