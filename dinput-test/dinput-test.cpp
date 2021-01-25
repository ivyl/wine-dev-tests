/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2020 Arkadiusz Hiler for CodeWeavers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <initguid.h>
#include <windows.h>
#include <dinput.h>
#include <stdio.h>
#include <assert.h>

IDirectInput8 *di8 = nullptr;
LPDIRECTINPUTDEVICE8 joystick = nullptr;

BOOL CALLBACK enum_callback(LPCDIDEVICEINSTANCE instance, LPVOID)
{
	printf("found instance name: %s\n", instance->tszInstanceName);
	printf("      product name:  %s\n", instance->tszProductName);
	assert(!FAILED(di8->CreateDevice(instance->guidInstance, &joystick, nullptr)));
	return DIENUM_STOP;
}

int main()
{
	DIJOYSTATE joystate;
	DIDEVCAPS caps;

	HWND hwnd = CreateWindow("static", "Test", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 10, 10, 200, 200, NULL, NULL, NULL, NULL);

	assert(!FAILED(DirectInput8Create(GetModuleHandleW(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di8, nullptr)));
	di8->EnumDevices(DI8DEVCLASS_GAMECTRL, enum_callback, nullptr, DIEDFL_ATTACHEDONLY);

	assert(joystick != nullptr);

	assert(!FAILED(joystick->SetDataFormat(&c_dfDIJoystick)));
	assert(!FAILED(joystick->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND)));

	DIDEVCAPS capabilities = {};
	capabilities.dwSize = sizeof(DIDEVCAPS);
	assert(!FAILED(joystick->GetCapabilities(&capabilities)));

	DIPROPGUIDANDPATH guidandpath = {};
	guidandpath.diph.dwSize = sizeof(guidandpath);
	guidandpath.diph.dwHeaderSize = sizeof(guidandpath.diph);
	guidandpath.diph.dwObj = 0;
	guidandpath.diph.dwHow = DIPH_DEVICE;

	assert(!FAILED(joystick->GetProperty(DIPROP_GUIDANDPATH, &guidandpath.diph)));

	printf("path == %ls\n", guidandpath.wszPath);

	caps.dwSize = sizeof(caps);
	assert(!FAILED(joystick->GetCapabilities(&caps)));

	printf("axes: %ld, buttons: %ld, POVs: %ld\n\n",
	       caps.dwAxes, caps.dwButtons, caps.dwPOVs);

	joystick->Acquire();
	while (TRUE)
	{
		char buttons[ARRAYSIZE(joystate.rgbButtons)+1] = {};
		char divider[ARRAYSIZE(joystate.rgbButtons)+1] = {};
		char label1[ARRAYSIZE(joystate.rgbButtons)+1] = {};
		char label2[ARRAYSIZE(joystate.rgbButtons)+1] = {};

		for (int i = 0; i < ARRAYSIZE(joystate.rgbButtons); i++)
		{
			divider[i] = '-';
			label1[i] = '0' + (i / 10);
			label2[i] = '0' + (i % 10);

			if (joystate.rgbButtons[i])
			{
				buttons[i] = '1';
			}
			else
			{
				buttons[i] = '0';
			}
		}

		HRESULT hr = joystick->Poll();
		if (FAILED(hr))
		{
			hr = joystick->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = joystick->Acquire();

			assert(hr != DIERR_INVALIDPARAM && hr != DIERR_NOTINITIALIZED);
			if (hr == DIERR_OTHERAPPHASPRIO)
				continue;
		}

		assert(!FAILED(joystick->GetDeviceState(sizeof(joystate), &joystate)));
		printf("lX: %05ld, lY: %05ld, lZ: %05ld, lRx: %05ld, lRy: %05ld, lRz: %05ld, rglSlider0: %05ld, rglSlider1: %05ld\n",
		       joystate.lX,  joystate.lY,  joystate.lZ,
		       joystate.lRx, joystate.lRy, joystate.lRz,
		       joystate.rglSlider[0], joystate.rglSlider[1]);

		printf("pov0: %05ld, pov1: %05ld, pov2: %05ld, pov3: %05ld\n",
			joystate.rgdwPOV[0], joystate.rgdwPOV[1], joystate.rgdwPOV[2], joystate.rgdwPOV[3]);

		printf("buttons: %s\n         %s\n         %s\nstate:   %s\n\n", label1, label2, divider, buttons);


		Sleep(500);
	}

	return 0;
}
