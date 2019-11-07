#include "PPU.h"



PPU::PPU(Timer * timer)
{
}


PPU::~PPU()
{
}

void PPU::step()
{
	modeClock += timer->tCycles;

	switch (mode)
	{
		// OAM read mode, scanline active
	case 2:
		if (modeClock >= 80)
		{
			// Enter scanline mode 3
			modeClock = 0;
			mode = 3;
		}
		break;

		// VRAM read mode, scanline active
		// Treat end of mode 3 as end of scanline
	case 3:
		if (modeClock >= 172)
		{
			// Enter hblank
			modeClock = 0;
			mode = 0;

			// Write a scanline to the framebuffer
			renderscan();
		}
		break;

		// Hblank
		// After the last hblank, push the screen data to canvas
	case 0:
		if (modeClock >= 204)
		{
			modeClock = 0;
			line++;

			if (line == 143)
			{
				// Enter vblank
				mode = 1;
				//GPU._canvas.putImageData(GPU._scrn, 0, 0);
			}
			else
			{
				mode = 2;
			}
		}
		break;

		// Vblank (10 lines)
	case 1:
		if (modeClock >= 456)
		{
			modeClock = 0;
			line++;

			if (line > 153)
			{
				// Restart scanning modes
				mode = 2;
				line = 0;
			}
		}
		break;
	}

}
