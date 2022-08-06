
//
// Title:	        Agon Video BIOS
// Author:        	Dean Belfield
// Created:       	22/03/2022
// Last Updated:	11/07/2022
//
// Modinfo:
// 11/07/2022:		Baud rate tweaked for Agon Light, HW Flow Control temporarily commented out
// 31/07/2022:    Sprite handler complete

#include "fabgl.h"
#include "HardwareSerial.h"

#define	DEBUG		0

#define	ESPSerial Serial2

#define UART_BR		384000
#define UART_NA		-1
#define UART_TX		2
#define UART_RX		34
#define UART_RTS    13 		// The ESP32 RTS pin
#define UART_CTS    14		// The ESP32 CTS pin

#define GPIO_ITRP	17		// VSync Interrupt Pin - for reference only

fabgl::PS2Controller    PS2Controller;
fabgl::VGA16Controller  VGAController;

fabgl::Canvas * Canvas;

int         charX, charY;

Point       p1, p2, p3;
RGB888      fg, bg;

int         count = 0;

/*
 * SPRITE ADD-ON Code
 * Jeroen Venema
 * FabGL accepts only fixed-allocation bitmap/sprite arrays
 * Array max perfectly fits selection window of 256/8bit
 * 
 */
uint8_t numsprites = 0;
uint8_t current_sprite = 0; 
uint8_t current_bitmap = 0;
Bitmap bitmaps[256];
Sprite sprites[256];
/*
 * END SPRITE ADD-ON
 */

#if DEBUG == 1
HardwareSerial DBGSerial(0);
#endif 

void setup() {
  uint16_t n;
  
	disableCore0WDT();			// Disable the watchdog timers
	delay(100); 				// Crashes without this delay
	disableCore1WDT();
	//pinMode(UART_RTS, OUTPUT);	// This should be done by setPins
	//pinMode(UART_CTS, INPUT);
	#if DEBUG == 1
	DBGSerial.begin(115200, SERIAL_8N1, 3, 1);
	#endif 
 	ESPSerial.begin(UART_BR, SERIAL_8N1, UART_RX, UART_TX);
	//ESPSerial.setPins(UART_NA, UART_NA, UART_CTS, UART_RTS);
	//ESPSerial.setHwFlowCtrlMode(HW_FLOWCTRL_CTS_RTS, 64);
 	//ESPSerial.setRxBufferSize(1024);
	//setRTSStatus(true);
 	PS2Controller.begin(PS2Preset::KeyboardPort0, KbdMode::CreateVirtualKeysQueue);
  	VGAController.begin();
  	set_mode(6);   
}

void setRTSStatus(bool value) {
	digitalWrite(UART_RTS, value ? LOW : HIGH);		// Asserts when low
}


void loop() {
	bool cursorVisible = false;
	bool cursorState = false;

	boot_screen();

	while(true) {
    	//cursorVisible = ((count & 0xFFFF) == 0);
    	//if(cursorVisible) {
      //		cursorState = !cursorState;
      //		do_cursor();
    	//}
    	do_keyboard();
    	if(ESPSerial.available() > 0) {
      //		if(cursorState) {
 	    //		cursorState = false;
      // 		do_cursor();
      //		}
      		byte c = ESPSerial.read();
      		vdu(c);
			
    	}
    	count++;
  	}
}

void debug_log(const char *format, ...) {
	#if DEBUG == 1
   	va_list ap;
   	va_start(ap, format);
   	int size = vsnprintf(nullptr, 0, format, ap) + 1;
   	if (size > 0) {
    	va_end(ap);
     	va_start(ap, format);
     	char buf[size + 1];
     	vsnprintf(buf, size, format, ap);
     	DBGSerial.print(buf);
   	}
   	va_end(ap);
	#endif
}

void boot_screen() {
  	cls();
  	print("AGON VPD Version 0.02\n\r");
	ESPSerial.write(27);
}

void cls() {
//	Canvas->swapBuffers();
	Canvas->clear();
	charX = 0;
	charY = 0;
}

// Set the video mode
// 
void set_mode(int mode) {
	switch(mode) {
    	case 2:
      		VGAController.setResolution(VGA_320x200_75Hz, 320, 200, false);
      		break;
		case 6:
      		VGAController.setResolution(VGA_640x480_60Hz, 640, 400, false);
      		break;
    	default:
			VGAController.setResolution(VGA_640x480_60Hz);
      		break;
  	}
  	delete Canvas;

  	Canvas = new fabgl::Canvas(&VGAController);
  	fg = Color::BrightWhite;
  	bg = Color::Black;;
  	Canvas->selectFont(&fabgl::FONT_8x8);
  	Canvas->setGlyphOptions(GlyphOptions().FillBackground(true));
  	Canvas->setPenWidth(1);
  	Canvas->setPenColor(fg);
  	Canvas->setBrushColor(bg);	
}

void print(char const * text) {
	for(int i = 0; i < strlen(text); i++) {
    	vdu(text[i]);
  	}
}

void printFmt(const char *format, ...) {
   	va_list ap;
   	va_start(ap, format);
   	int size = vsnprintf(nullptr, 0, format, ap) + 1;
   	if (size > 0) {
    	va_end(ap);
     	va_start(ap, format);
     	char buf[size + 1];
     	vsnprintf(buf, size, format, ap);
     	print(buf);
   	}
   	va_end(ap);
 }

// Handle the keyboard
// 
void do_keyboard() {
  	fabgl::Keyboard* kb = PS2Controller.keyboard();
  	bool isKeyDown;
  	//if(kb->virtualKeyAvailable() > 0) {
    while(kb->virtualKeyAvailable() > 0) {
    	fabgl::VirtualKey vk = kb->getNextVirtualKey(&isKeyDown, 20);
		if(isKeyDown) {
    		int a = kb->virtualKeyToASCII(vk);
    		if(a != -1) {
	      		ESPSerial.write(a);
    		}
		}
  	} 
}

// Render a cursor at the current screen position
//
void do_cursor() {
  	int w = Canvas->getFontInfo()->width;
  	int h = Canvas->getFontInfo()->height;
  	int x = charX; 
  	int y = charY;
  	Canvas->swapRectangle(x, y, x + w - 1, y + h - 1);
}

// Read a byte from the serial port
//
byte readByte() {
  	while(ESPSerial.available() == 0);
  	return ESPSerial.read();
}

// Read a word from the serial port
//
word readWord() {
  	byte l = readByte();
  	byte h = readByte();
  	return (h << 8) | l;
}

/*
 * Jeroen Venema
 * Read a long from the serial port LSB => MSB
 */
uint32_t readLong()
{
  uint32_t temp;
  temp = readByte();  // LSB;
  temp |= (readByte() << 8);
  temp |= (readByte() << 16);
  temp |= (readByte() << 24);
  return temp;
}

void vdu(byte c) {
	switch(c) {
    	case 0x08:  // Cursor Left
      		cursorLeft();
      		break;
    	case 0x09:  // Cursor Right
      		cursorRight();
      		break;
    	case 0x0A:  // Cursor Down
      		cursorDown();
      		break;
    	case 0x0B:  // Cursor Up
      		cursorUp();
      		break;
    	case 0x0C:  // CLS
      		cls();
      		break;
    	case 0x0D:  // CR
      		cursorHome();
//			digitalWrite(GPIO_ITRP, LOW);
//			digitalWrite(GPIO_ITRP, HIGH);
//			digitalWrite(GPIO_ITRP, LOW);
      		break;
    	case 0x12:  // GCOL
      		vdu_gcol();
      		break;
    	case 0x16:  // Mode
      		vdu_mode();
      		break;
    	case 0x17:  // VDU 23
      		vdu_sys();
      		break;
    	case 0x19:  // PLOT
      		vdu_plot();
      		break;
    	case 0x1E:  // Home
      		cursorHome();
      		break;
    	case 0x7F:  // Backspace
      		cursorLeft();
      		Canvas->drawText(charX, charY, " ");
    	  	break;
    	case 32 ... 126:
      		Canvas->drawTextFmt(charX, charY, "%c", c);
      		cursorRight();
      	break;
  	}
}

/*
 * Jeroen Venema
 * Sprite Add-on
 * 
 */

void vdu_sprite(void)
{
    uint32_t color;
    void *dataptr;
    int16_t x,y;
    int16_t width,height;
    uint16_t n,temp;
    bool refresh = false;
    
    byte cmd = readByte();
    switch(cmd)
    {
      case 0: // select bitmap
        current_bitmap = readByte();
        debug_log("vdu - bitmap %d selected\n\r", current_bitmap);
        break;

      case 1: // Send bitmap data
      case 2: // Define bitmap in single color
        width = readWord();
        height = readWord();
        
        // clear out any old data first
        free(bitmaps[current_bitmap].data);
        // allocate new heap data
        dataptr = (void *)heap_caps_malloc(sizeof(uint32_t)*width*height, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        bitmaps[current_bitmap].data = (uint8_t *)dataptr;
        
        if(dataptr != NULL)
        {                  
          if(cmd == 1)
          {
            // read data to the new databuffer
            for(n = 0; n < width*height; n++) ((uint32_t *)bitmaps[current_bitmap].data)[n] = readLong();  
            debug_log("vdu - bitmap %d - data received - width %d, height %d\n\r", current_bitmap, width, height);
          }
          if(cmd == 2)
          {
            color = readLong();
            // Define single color
            for(n = 0; n < width*height; n++) ((uint32_t *)dataptr)[n] = color;            
            debug_log("vdu - bitmap %d - set to solid color - width %d, height %d\n\r", current_bitmap, width, height);            
          }
          // Create bitmap structure
          bitmaps[current_bitmap] = Bitmap(width,height,dataptr,PixelFormat::RGBA8888);
          bitmaps[current_bitmap].dataAllocated = false;
        }
        else
        {
          for(n = 0; n < width*height; n++) readLong(); // discard incoming data
          debug_log("vdu - bitmap %d - data discarded, no memory available - width %d, height %d\n\r", current_bitmap, width, height);
        }
        break;

      case 3: // Draw bitmap to screen (x,y)
        x = readWord();
        y = readWord();

        if(bitmaps[current_bitmap].data) Canvas->drawBitmap(x,y,&bitmaps[current_bitmap]);
   
        debug_log("vdu - bitmap %d draw command\n\r", current_bitmap);
        break;

      /*
       * Sprites
       * 
       * Sprite creation order:
       * 1) Create bitmap(s) for sprite, or re-use bitmaps already created
       * 2) Select the correct sprite ID (0-255). The GDU only accepts sequential sprite sets, starting from ID 0. All sprites must be adjacent to 0
       * 3) Clear out any frames from previous program definitions
       * 4) Add one or more frames to each sprite
       * 5) Activate sprite to the GDU
       * 6) Show sprites on screen / move them around as needed
       */
       
      case 4: // select sprite
        current_sprite = readByte();
        debug_log("vdu - sprite %d selected\n\r", current_sprite);
        break;

      case 5: // clear frames
        sprites[current_sprite].clearBitmaps();
        debug_log("vdu - sprite %d - all frames cleared\n\r", current_sprite);
        break;
        
      case 6: // add frame to sprite
        n = readByte();

        sprites[current_sprite].addBitmap(&bitmaps[n]);
        sprites[current_sprite].visible = false;
       
        debug_log("vdu - sprite %d - bitmap %d added as frame %d\n\r", current_sprite, n, sprites[current_sprite].framesCount-1);
        break;

      case 7: // Active sprites to GDU
        /*
         * Sprites 0-(numsprites-1) will be activated on-screen
         * Make sure all sprites have at least one frame attached to them
         */
        numsprites = readByte();
        VGAController.setSprites(sprites, numsprites);
        debug_log("vdu - %d sprites activated\n\r", numsprites);
        break;

      case 8: // set next frame on sprite
        sprites[current_sprite].nextFrame();
        refresh = true;
        debug_log("vdu - sprite %d next frame\n\r", current_sprite);
        break;

      case 9: // set previous frame on sprite
        n = sprites[current_sprite].currentFrame;
        
        if(n) sprites[current_sprite].currentFrame = n-1; // previous frame
        else sprites[current_sprite].currentFrame = sprites[current_sprite].framesCount - 1;  // last frame

        refresh = true;
        debug_log("vdu - sprite %d previous frame\n\r", current_sprite);
        break;

      case 10: // set current frame id on sprite
        n = readByte();

        if(n < sprites[current_sprite].framesCount) sprites[current_sprite].currentFrame = n;

        refresh = true;
        debug_log("vdu - sprite %d set to frame %d\n\r", current_sprite,n);
        break;

      case 11: // show sprite
        sprites[current_sprite].visible = 1;

        refresh = true;
        debug_log("vdu - sprite %d show cmd\n\r", current_sprite);
        break;

      case 12: // hide sprite
        sprites[current_sprite].visible = 0;

        refresh = true;
        debug_log("vdu - sprite %d hide cmd\n\r", current_sprite);
        break;

      case 13: // move sprite to coordinate on screen
        x = readWord();
        y = readWord();
        
        sprites[current_sprite].moveTo(x,y); 

        refresh = true;
        debug_log("vdu - sprite %d - move to (%d,%d)\n\r", current_sprite, x, y);
        break;

      case 14: // move sprite by offset to current coordinate on screen
        x = readWord();
        y = readWord();
        
        sprites[current_sprite].x += x;
        sprites[current_sprite].y += y;

        refresh = true;
        debug_log("vdu - sprite %d - move by offset (%d,%d)\n\r", current_sprite, x, y);
        break;
    }
    if(numsprites && refresh) // only refresh if needed and number of sprites is activated to the GDU
    {
      debug_log("vdu - perform sprite refresh\n\r");
      VGAController.refreshSprites();
    }
}

// Handle the cursor
//
void cursorLeft() {
	charX -= Canvas->getFontInfo()->width;
  	if(charX < 0) {
    	charX = 0;
  	}
}

void cursorRight() {
  	charX += Canvas->getFontInfo()->width;
  	if(charX >= Canvas->getWidth()) {
    	cursorDown();
    	cursorHome();
  	}
}

void cursorDown() {
	int h = Canvas->getFontInfo()->height;
  	charY += h;
	  if(charY >= Canvas->getHeight()) {
		charY -= h;
		Canvas->scroll(0, -h);
	  }
}

void cursorUp() {
  	charY -= Canvas->getFontInfo()->height;
  	if(charY < 0) {
		charY = 0;
  	}
}
void cursorHome() {
  	charX = 0;
}

// Handle MODE
//
void vdu_mode() {
  	byte mode = readByte();
	debug_log("vdu_mode: %d\n\r", mode);
  	set_mode(mode);
}

// Handle GCOL
// 
void vdu_gcol() {
  	byte mode = readByte();
  	fg.R = readByte();
  	fg.G = readByte();
  	fg.B = readByte();
	debug_log("vdu_gcol: %d,%d,%d,%d\n\r", mode, fg.R, fg.G, fg.B);
 	Canvas->setPenColor(fg);
}

// Handle PLOT
//
void vdu_plot() {
  	byte mode = readByte();
  	p3 = p2;
  	p2 = p1;
  	p1.X = readWord();
  	p1.Y = readWord();
	debug_log("vdu_plot: %d,%d,%d\n\r", mode, p1.X, p1.Y);
  	switch(mode) {
    	case 0x04: 
      		Canvas->moveTo(p1.X, p1.Y);
      		break;
    	case 0x05: // Line
      		Canvas->lineTo(p1.X, p1.Y);
      		break;
		case 0x40 ... 0x47: // Point
			vdu_plot_point(mode);
			break;
    	case 0x50 ... 0x57: // Triangle
      		vdu_plot_triangle(mode);
      		break;
    	case 0x90 ... 0x97: // Circle
      		vdu_plot_circle(mode);
      		break;
  	}
}

void vdu_plot_point(byte mode) {
	Canvas->setPixel(p1.X, p1.Y);
}

void vdu_plot_triangle(byte mode) {
  	Point p[3] = { p3, p2, p1 };
  	Canvas->setBrushColor(fg);
  	Canvas->fillPath(p, 3);
  	Canvas->setBrushColor(bg);
}

void vdu_plot_circle(byte mode) {
  	int a, b, r;
  	switch(mode) {
    	case 0x90 ... 0x93: // Circle
      		r = 2 * (p1.X + p1.Y);
      		Canvas->drawEllipse(p2.X, p2.Y, r, r);
      		break;
    	case 0x94 ... 0x97: // Circle
      		a = p2.X - p1.X;
      		b = p2.Y - p1.Y;
      		r = 2 * sqrt(a * a + b * b);
      		Canvas->drawEllipse(p2.X, p2.Y, r, r);
      		break;
  	}
}

// Handle SYS
//
void vdu_sys() {
  	byte mode = readByte();
	  //debug_log("vdu_sys: %d\n\r", mode);
  	switch(mode) {
	    case 0:
      		vdu_sys_esp();
          break;
      /*
       * Jeroen Venema
       */
      case 27:
          vdu_sprite();
      		break;
  	}
}


void vdu_sys_esp() {
    byte mode = readByte();
    switch(mode) {
    }
}
