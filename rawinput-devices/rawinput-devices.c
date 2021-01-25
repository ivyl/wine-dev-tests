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
#include <hidsdi.h>

void check_caps(HANDLE device_file)
{
	wchar_t product_string[0x100];
	PHIDP_PREPARSED_DATA preparsed_data;
	HIDP_CAPS hidp_caps;
	PHIDP_BUTTON_CAPS button_caps;
	NTSTATUS status;

	if (device_file == INVALID_HANDLE_VALUE)
	{
		printf("INVALID_HANDLE_VALUE, LastError = %lx\n", GetLastError());
		return;
	}

	if (!HidD_GetProductString(device_file, product_string, sizeof(product_string)))
	{
		printf("failed to get product string, LastError = %lx\n", GetLastError());
		return;
	}
	printf("  HidD_GetProductString: %ls\n", product_string);

	if (!HidD_GetPreparsedData(device_file, &preparsed_data))
	{
		printf("failed to get preparsed data, LastError = %lx\n", GetLastError());
		return;
	}

	status = HidP_GetCaps(preparsed_data, &hidp_caps);
	if (status != HIDP_STATUS_SUCCESS) {
		printf("HidP_GetCaps failed with %lx, LastError = %lx\n", status, GetLastError());
		return;
	}

	printf("  caps.Usage = %hu\n", hidp_caps.Usage);
	printf("  caps.UsagePage = %hu\n", hidp_caps.UsagePage);
	printf("  caps.InputReportByteLength = %hu\n", hidp_caps.InputReportByteLength);
	printf("  caps.OutputReportByteLength = %hu\n", hidp_caps.OutputReportByteLength);
	printf("  caps.FeatureReportByteLength = %hu\n", hidp_caps.FeatureReportByteLength);
	printf("  caps.NumberLinkCollectionNodes = %hu\n", hidp_caps.NumberLinkCollectionNodes);
	printf("  caps.NumberInputButtonCaps = %hu\n", hidp_caps.NumberInputButtonCaps);
	printf("  caps.NumberInputValueCaps = %hu\n", hidp_caps.NumberInputValueCaps);
	printf("  caps.NumberInputDataIndices = %hu\n", hidp_caps.NumberInputDataIndices);
	printf("  caps.NumberOutputButtonCaps = %hu\n", hidp_caps.NumberOutputButtonCaps);
	printf("  caps.NumberOutputValueCaps = %hu\n", hidp_caps.NumberOutputValueCaps);
	printf("  caps.NumberOutputDataIndices = %hu\n", hidp_caps.NumberOutputDataIndices);
	printf("  caps.NumberFeatureButtonCaps = %hu\n", hidp_caps.NumberFeatureButtonCaps);
	printf("  caps.NumberFeatureValueCaps = %hu\n", hidp_caps.NumberFeatureValueCaps);
	printf("  caps.NumberFeatureDataIndices = %hu\n", hidp_caps.NumberFeatureDataIndices);

	USHORT num_button_caps = hidp_caps.NumberInputButtonCaps;
	button_caps = calloc(num_button_caps, sizeof(HIDP_BUTTON_CAPS));
	status = HidP_GetButtonCaps(HidP_Input, button_caps, &num_button_caps, preparsed_data);
	if (status != HIDP_STATUS_SUCCESS) {
		printf("HidP_GetButtonCaps failed with %lx, LastError = %lx\n", status, GetLastError());
		return;
	}

	for (int i = 0; i < num_button_caps; i++)
	{
		printf("  button_cap #%i\n", i);
		printf("  button_caps.UsagePage = %hu\n", button_caps[i].UsagePage);
		printf("  button_caps.ReportID = %hu\n", button_caps[i].ReportID);
		printf("  button_caps.IsAlias = %hu\n", button_caps[i].IsAlias);
		printf("  button_caps.BitField = %hu\n", button_caps[i].BitField);
		printf("  button_caps.LinkCollection = %hu\n", button_caps[i].LinkCollection);
		printf("  button_caps.LinkUsage = %hu\n", button_caps[i].LinkUsage);
		printf("  button_caps.LinkUsagePage = %hu\n", button_caps[i].LinkUsagePage);

		printf("  button_caps.IsRange = %hu\n", button_caps[i].IsRange);
		printf("  button_caps.IsStringRange = %hu\n", button_caps[i].IsStringRange);
		printf("  button_caps.IsDesignatorRange = %hu\n", button_caps[i].IsDesignatorRange);
		printf("  button_caps.IsAbsolute = %hu\n", button_caps[i].IsAbsolute);

		if (button_caps[i].IsRange)
		{
			printf("  button_caps.Range.UsageMin = %hu\n", button_caps[i].Range.UsageMin);
			printf("  button_caps.Range.UsageMax = %hu\n", button_caps[i].Range.UsageMax);
			printf("  button_caps.Range.StringMin = %hu\n", button_caps[i].Range.StringMin);
			printf("  button_caps.Range.StringMax = %hu\n", button_caps[i].Range.StringMax);
			printf("  button_caps.Range.DesignatorMin = %hu\n", button_caps[i].Range.DesignatorMin);
			printf("  button_caps.Range.DesignatorMax = %hu\n", button_caps[i].Range.DesignatorMax);
			printf("  button_caps.Range.DataIndexMin = %hu\n", button_caps[i].Range.DataIndexMin);
			printf("  button_caps.Range.DataIndexMax = %hu\n", button_caps[i].Range.DataIndexMax);
		}
		else
		{
			printf("  button_caps.NotRange.Usage = %hu\n", button_caps[i].NotRange.Usage);
			printf("  button_caps.NotRange.StringIndex = %hu\n", button_caps[i].NotRange.StringIndex);
			printf("  button_caps.NotRange.DesignatorIndex = %hu\n", button_caps[i].NotRange.DesignatorIndex);
			printf("  button_caps.NotRange.DataIndex = %hu\n", button_caps[i].NotRange.DataIndex);
		}
	}

	char buffer[65];
	if (!HidD_GetInputReport(device_file, (void*)buffer, sizeof(buffer)))
	{
		printf("last error: %x\n", GetLastError());
	}
}

int main()
{
	RAWINPUTDEVICELIST devices[16];
	UINT device_count = ARRAYSIZE(devices);
	UINT ret, i;

	ret = GetRawInputDeviceList(devices, &device_count, sizeof(*devices));

	for (i = 0; i < ret; i++)
	{
		wchar_t name[128] = {};
		UINT name_size = ARRAYSIZE(name);
		RID_DEVICE_INFO info;
		UINT info_size = sizeof(info);

		info.cbSize = info_size;

		GetRawInputDeviceInfoW(devices[i].hDevice, RIDI_DEVICENAME, name, &name_size);
		GetRawInputDeviceInfoA(devices[i].hDevice, RIDI_DEVICEINFO, &info, &info_size);

		printf("dev: %p\n type: %x\n name: %ls\n", devices[i].hDevice, devices[i].dwType, name);
		if (info.dwType == RIM_TYPEHID)
		{
			printf(" vendor: %x\n product: %x\n version: %x\n usage_page: %hx\n page: %hx\n",
				info.hid.dwVendorId, info.hid.dwProductId, info.hid.dwVersionNumber, info.hid.usUsagePage, info.hid.usUsage);
			if (info.hid.usUsage == 5)
			{
				HANDLE file = CreateFileW(name, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
				check_caps(file);
			}
		}
	}


	return 0;
}
