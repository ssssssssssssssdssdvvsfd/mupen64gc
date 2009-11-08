#include "InputStatusBar.h"
#include "GuiResources.h"
#include "GraphicsGX.h"
#include "IPLFont.h"
#include "Image.h"
#include "FocusManager.h"
#include <math.h>
#include <gccore.h>
#include "../main/wii64config.h"

extern "C" {
#include "../gc_input/controller.h"
}

namespace menu {

InputStatusBar::InputStatusBar(float x, float y)
		: x(x),
		  y(y)
/*		: active(false),
		  selected(false),
		  normalImage(0),
		  focusImage(0),
		  selectedImage(0),
		  selectedFocusImage(0),
		  buttonText(label),
		  buttonStyle(style),
		  labelMode(LABEL_CENTER),
		  labelScissor(0),
		  StartTime(0),
		  x(x),
		  y(y),
		  width(width),
		  height(height),
		  clickedFunc(0),
		  returnFunc(0)*/
{
						//Focus color			Inactive color		  Active color			Selected color		  Label color
//	GXColor colors[5] = {{255, 100, 100, 255}, {255, 255, 255,  70}, {255, 255, 255, 130}, {255, 255, 255, 255}, {255, 255, 255, 255}};

}

InputStatusBar::~InputStatusBar()
{
}

void InputStatusBar::drawComponent(Graphics& gfx)
{
	WPADData* wpad;
	GXColor activeColor = (GXColor) {255, 255, 255, 255};
	GXColor inactiveColor = (GXColor) {128, 128, 128, 128};
	char statusText[20];
	//update controller availability
	//build text
	//draw text with appropriate color
	for (int i = 0; i < 4; i++)
	{
		switch (padType[i])
		{
		case PADTYPE_GAMECUBE:
			//update available
			controller_GC.available[(int)padAssign[i]] = (gc_connected & (1<<padAssign[i])) ? 1 : 0;
			if (controller_GC.available[(int)padAssign[i]])
				IplFont::getInstance().drawInit(activeColor);
			else
				IplFont::getInstance().drawInit(inactiveColor);
			sprintf (statusText, "Pad%d: GC%d", i, padAssign[i]);

			//build text
			//draw with appropriate color
			break;
#ifdef HW_RVL
		case PADTYPE_WII:
			wpad = WPAD_Data((int)padAssign[i]);
			controller_Classic.available[(int)padAssign[i]] = (wpad->err == WPAD_ERR_NONE && wpad->exp.type == WPAD_EXP_CLASSIC) ? 1 : 0;
			controller_WiimoteNunchuk.available[(int)padAssign[i]] = (wpad->err == WPAD_ERR_NONE && wpad->exp.type == WPAD_EXP_NUNCHUK) ? 1 : 0;
			if (controller_Classic.available[(int)padAssign[i]])
			{
				assign_controller(i, &controller_Classic, (int)padAssign[i]);
				IplFont::getInstance().drawInit(activeColor);
				sprintf (statusText, "Pad%d: CC%d", i, padAssign[i]);
			}
			else if (controller_WiimoteNunchuk.available[(int)padAssign[i]])
			{
				assign_controller(i, &controller_WiimoteNunchuk, (int)padAssign[i]);
				IplFont::getInstance().drawInit(activeColor);
				sprintf (statusText, "Pad%d: WM+N%d", i, padAssign[i]);
			}
			else
			{
				IplFont::getInstance().drawInit(inactiveColor);
				sprintf (statusText, "Pad%d: Wii%d", i, padAssign[i]);
			}

			break;
#endif
		case PADTYPE_NONE:
			IplFont::getInstance().drawInit(inactiveColor);
				sprintf (statusText, "Pad%d: None", i);
			break;
		}
		IplFont::getInstance().drawString((int) 540, (int) 150+30*i, statusText, 1.0, true);
	}


/*
//	printf("Button drawComponent\n");

	gfx.setColor(inactiveColor);

	//activate relevant texture
	if(active)
	{
		//draw normalImage with/without gray mask and with alpha test on
		//printf("Button Active\n");
		gfx.setColor(activeColor);
	}
	if(getFocus())
	{
		//draw focus indicator (extra border for button?)
		//printf("Button in Focus\n");
		gfx.setColor(focusColor);
	}
	//draw buttonLabel?

	gfx.enableBlending(true);
//	gfx.setTEV(GX_PASSCLR);
	gfx.setTEV(GX_MODULATE);

//	gfx.setColor(focusColor);
	gfx.setDepth(-10.0f);
	gfx.newModelView();
	gfx.loadModelView();
	gfx.loadOrthographic();

	switch (buttonStyle)
	{
	case BUTTON_DEFAULT:
//	gfx.fillRect(x, y, width, height);
		normalImage->activateImage(GX_TEXMAP0);
		gfx.drawImage(0, x, y, width/2, height/2, 0.0, width/16.0, 0.0, height/16.0);
		gfx.drawImage(0, x+width/2, y, width/2, height/2, width/16.0, 0.0, 0.0, height/16.0);
		gfx.drawImage(0, x, y+height/2, width/2, height/2, 0.0, width/16.0, height/16.0, 0.0);
		gfx.drawImage(0, x+width/2, y+height/2, width/2, height/2, width/16.0, 0.0, height/16.0, 0.0);
//	gfx.drawImage(0, x, y, width, height, 0.0, 1.0, 0.0, 1.0);

		if (selected)
		{
			gfx.setColor(selectedColor);
			if(selectedImage) selectedImage->activateImage(GX_TEXMAP0);
			gfx.drawImage(0, x, y, width/2, height/2, 0.0, width/16.0, 0.0, height/16.0);
			gfx.drawImage(0, x+width/2, y, width/2, height/2, width/16.0, 0.0, 0.0, height/16.0);
			gfx.drawImage(0, x, y+height/2, width/2, height/2, 0.0, width/16.0, height/16.0, 0.0);
			gfx.drawImage(0, x+width/2, y+height/2, width/2, height/2, width/16.0, 0.0, height/16.0, 0.0);
		}
		break;
	case BUTTON_STYLEA_NORMAL:
		if (getFocus())	focusImage->activateImage(GX_TEXMAP0);
		else			normalImage->activateImage(GX_TEXMAP0);
		gfx.drawImage(0, x, y, width/2, height, 0.0, width/8.0, 0.0, 1.0);
		gfx.drawImage(0, x+width/2, y, width/2, height, width/8.0, 0.0, 0.0, 1.0);
		break;
	case BUTTON_STYLEA_SELECT:
		if (selected)
		{
			if (getFocus())	selectedFocusImage->activateImage(GX_TEXMAP0);
			else			selectedImage->activateImage(GX_TEXMAP0);
		}
		else
		{
			if (getFocus())	focusImage->activateImage(GX_TEXMAP0);
			else			normalImage->activateImage(GX_TEXMAP0);
		}
		gfx.drawImage(0, x, y, width/2, height, 0.0, width/8.0, 0.0, 1.0);
		gfx.drawImage(0, x+width/2, y, width/2, height, width/8.0, 0.0, 0.0, 1.0);
		break;
	}

	if (buttonText)
	{
		int strWidth, strHeight;
		unsigned long CurrentTime;
		float scrollWidth, time_sec, scrollOffset;
		gfx.enableScissor(x + labelScissor, y, width - 2*labelScissor, height);
		if(active)	IplFont::getInstance().drawInit(labelColor);
		else		IplFont::getInstance().drawInit(inactiveColor);
		switch (labelMode)
		{
			case LABEL_CENTER:
				IplFont::getInstance().drawString((int) (x+width/2), (int) (y+height/2), *buttonText, 1.0, true);
				break;
			case LABEL_LEFT:
				strWidth = IplFont::getInstance().getStringWidth(*buttonText, 1.0);
				strHeight = IplFont::getInstance().getStringHeight(*buttonText, 1.0);
				IplFont::getInstance().drawString((int) (x+labelScissor), (int) (y+(height-strHeight)/2), *buttonText, 1.0, false);
				break;
			case LABEL_SCROLL:
				strHeight = IplFont::getInstance().getStringHeight(*buttonText, 1.0);
				scrollWidth = IplFont::getInstance().getStringWidth(*buttonText, 1.0)-width+2*labelScissor;
				scrollWidth = scrollWidth < 0.0f ? 0.0 : scrollWidth;
				CurrentTime = ticks_to_microsecs(gettick());
				time_sec = (float)(CurrentTime - StartTime)/1000000.0f;
				if (time_sec > SCROLL_PERIOD) StartTime = ticks_to_microsecs(gettick());
				scrollOffset = fabsf(fmodf(time_sec,SCROLL_PERIOD)-SCROLL_PERIOD/2)/(SCROLL_PERIOD/2);
				IplFont::getInstance().drawString((int) (x+labelScissor-(int)(scrollOffset*scrollWidth)), (int) (y+(height-strHeight)/2), *buttonText, 1.0, false);
				break;
			case LABEL_SCROLLONFOCUS:
				if(getFocus())
				{
					strHeight = IplFont::getInstance().getStringHeight(*buttonText, 1.0);
					scrollWidth = IplFont::getInstance().getStringWidth(*buttonText, 1.0)-width+2*labelScissor;
					scrollWidth = scrollWidth < 0.0f ? 0.0 : scrollWidth;
					CurrentTime = ticks_to_microsecs(gettick());
					time_sec = (float)(CurrentTime - StartTime)/1000000.0f;
					if (time_sec > SCROLL_PERIOD) StartTime = ticks_to_microsecs(gettick());
					scrollOffset = fabsf(fmodf(time_sec,SCROLL_PERIOD)-SCROLL_PERIOD/2)/(SCROLL_PERIOD/2);
					IplFont::getInstance().drawString((int) (x+labelScissor-(int)(scrollOffset*scrollWidth)), (int) (y+(height-strHeight)/2), *buttonText, 1.0, false);
				}
				else
				{
				strWidth = IplFont::getInstance().getStringWidth(*buttonText, 1.0);
				strHeight = IplFont::getInstance().getStringHeight(*buttonText, 1.0);
				IplFont::getInstance().drawString((int) (x+labelScissor), (int) (y+(height-strHeight)/2), *buttonText, 1.0, false);
				}
				break;
		}
		gfx.disableScissor();
	}
*/
}

void InputStatusBar::updateTime(float deltaTime)
{
	//Overload in Component class
	//Add interpolator class & update here?
}

Component* InputStatusBar::updateFocus(int direction, int buttonsPressed)
{
	return NULL;
}

} //namespace menu 
