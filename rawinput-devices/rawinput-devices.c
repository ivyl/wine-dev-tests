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

#include <windows.h>
#include <stdio.h>

int main()
{
	RAWINPUTDEVICELIST devices[16];
	UINT device_count = ARRAYSIZE(devices);
	UINT ret, i;

	ret = GetRawInputDeviceList(devices, &device_count, sizeof(*devices));

	for (i = 0; i < ret; i++)
	{
		char name[128] = {};
		UINT name_size = ARRAYSIZE(name);
		RID_DEVICE_INFO info;
		UINT info_size = sizeof(info);

		info.cbSize = info_size;

		GetRawInputDeviceInfoA(devices[i].hDevice, RIDI_DEVICENAME, name, &name_size);
		GetRawInputDeviceInfoA(devices[i].hDevice, RIDI_DEVICEINFO, &info, &info_size);

		printf("dev: %p\n type: %x\n name: %s\n", devices[i].hDevice, devices[i].dwType, name);
		if (info.dwType == RIM_TYPEHID)
		{
			printf(" vendor: %x\n product: %x\n version: %x\n usage_page: %hx\n page: %hx\n",
				info.hid.dwVendorId, info.hid.dwProductId, info.hid.dwVersionNumber, info.hid.usUsagePage, info.hid.usUsage);
		}
	}


	return 0;
}
