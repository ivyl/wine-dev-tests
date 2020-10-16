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
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "adl_sdk.h"

typedef int(*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
typedef int(*ADL_MAIN_CONTROL_DESTROY)();
typedef int(*ADL_ADAPTER_NUMBEROFADAPTERS_GET)(int*);
typedef int(*ADL_ADAPTER_ADAPTERINFO_GET)(LPAdapterInfo, int);
typedef int(*ADL_DISPLAY_DISPLAYINFO_GET)(int adapter_index, int *num_displays, ADLDisplayInfo **info, int force_detect);
typedef int(*ADL_ADAPTER_CROSSFIRE_CAPS)(int adapter_index, int *preffered, int *num_comb, ADLCrossfireComb** comb);
typedef int(*ADL_ADAPTER_CROSSFIRE_GET)(int adapter_index, ADLCrossfireComb *comb, ADLCrossfireInfo *info);
typedef int(*ADL_ADAPTER_ASICFAMILYTYPE_GET)(int adapter_index, int *asic_type, int *valids);
typedef int(*ADL_ADAPTER_OBSERVEDCLOCKINFO_GET)(int adapter_index, int *core_clock, int *memory_clock);
typedef int(*ADL_ADAPTER_MEMORYINFO_GET)(int adapter_index, ADLMemoryInfo *mem_info);
typedef int(*ADL_GRAPHICS_PLATFORM_GET)(int *platform);

ADL_MAIN_CONTROL_CREATE           ADL_Main_Control_Create = NULL;
ADL_MAIN_CONTROL_DESTROY          ADL_Main_Control_Destroy = NULL;
ADL_ADAPTER_NUMBEROFADAPTERS_GET  ADL_Adapter_NumberOfAdapters_Get = NULL;
ADL_ADAPTER_ADAPTERINFO_GET       ADL_Adapter_AdapterInfo_Get = NULL;
ADL_DISPLAY_DISPLAYINFO_GET       ADL_Display_DisplayInfo_Get = NULL;
ADL_ADAPTER_CROSSFIRE_CAPS        ADL_Adapter_Crossfire_Caps = NULL;
ADL_ADAPTER_CROSSFIRE_GET         ADL_Adapter_Crossfire_Get = NULL;
ADL_ADAPTER_ASICFAMILYTYPE_GET    ADL_Adapter_ASICFamilyType_Get = NULL;
ADL_ADAPTER_OBSERVEDCLOCKINFO_GET ADL_Adapter_ObservedClockInfo_Get = NULL;
ADL_ADAPTER_MEMORYINFO_GET        ADL_Adapter_MemoryInfo_Get = NULL;
ADL_GRAPHICS_PLATFORM_GET         ADL_Graphics_Platform_Get = NULL;

void* __stdcall adl_malloc(int size)
{
	return malloc(size);
}

#define LOAD_FUNCTION(module, name) \
{ \
	name = (typeof(name)) GetProcAddress(module, #name); \
	assert(name != NULL); \
}

#define __token_cat(x, y) x ## y
#define token_cat(x, y) __token_cat(x,y)

#define ADL_IF(call) \
	int token_cat(__ret, __LINE__) = (call); \
	if (token_cat(__ret, __LINE__) != ADL_OK) \
		printf("ERROR: %s failed with %d\n", #call, token_cat(__ret, __LINE__)); \
	else

int main()
{
	int ret;
	int adapters_number, platform;
	HMODULE adl = LoadLibraryA("atiadlxx.dll");
	assert(adl != NULL);

	LOAD_FUNCTION(adl, ADL_Main_Control_Create);
	LOAD_FUNCTION(adl, ADL_Main_Control_Destroy);
	LOAD_FUNCTION(adl, ADL_Adapter_NumberOfAdapters_Get);
	LOAD_FUNCTION(adl, ADL_Adapter_AdapterInfo_Get);
	LOAD_FUNCTION(adl, ADL_Display_DisplayInfo_Get);
	LOAD_FUNCTION(adl, ADL_Adapter_Crossfire_Caps);
	LOAD_FUNCTION(adl, ADL_Adapter_Crossfire_Get);
	LOAD_FUNCTION(adl, ADL_Adapter_ASICFamilyType_Get);
	LOAD_FUNCTION(adl, ADL_Adapter_ObservedClockInfo_Get);
	LOAD_FUNCTION(adl, ADL_Adapter_MemoryInfo_Get);
	LOAD_FUNCTION(adl, ADL_Graphics_Platform_Get);

	assert(ADL_OK == ADL_Main_Control_Create(adl_malloc, 1));
	ADL_IF(ADL_OK == ADL_Graphics_Platform_Get(&platform))
	{
		if (platform == GRAPHICS_PLATFORM_DESKTOP)
			printf("platform: GRAPHICS_PLATFORM_DESKTOP\n");
		else if (platform == GRAPHICS_PLATFORM_MOBILE)
			printf("platform: GRAPHICS_PLATFORM_MOBILE\n");
		else
			printf("platform: UNKNOWN (%d)\n", platform);
	}

	assert(ADL_OK == ADL_Adapter_NumberOfAdapters_Get(&adapters_number));

	printf("adapters number %i\n\n", adapters_number);

	AdapterInfo ai[adapters_number];
	assert(ADL_OK == ADL_Adapter_AdapterInfo_Get(ai, sizeof(ai)));

	for (int i = 0; i < adapters_number; i++)
	{
		int core_clock, memory_clock;
		int asic_type, valids;
		ADLMemoryInfo mem_info;
		int cf_preferred, cf_num_comb;
		ADLCrossfireComb *cf_combs = NULL;
		int num_displays;
		ADLDisplayInfo *display_info = NULL;

		printf("index:        %d\n", ai[i].iAdapterIndex);
		printf("udid:         %s\n", ai[i].strUDID);
		printf("bus:          %d\n", ai[i].iBusNumber);
		printf("dev:          %d\n", ai[i].iDeviceNumber);
		printf("func:         %d\n", ai[i].iFunctionNumber);
		printf("vendor:       %d\n", ai[i].iVendorID);
		printf("name:         %s\n", ai[i].strAdapterName);
		printf("disp:         %s\n", ai[i].strDisplayName);
		printf("present:      %d\n", ai[i].iPresent);

		ADL_IF(ADL_Adapter_ObservedClockInfo_Get(ai[i].iAdapterIndex, &core_clock, &memory_clock))
		{
			printf("core_clock:   %d\n", core_clock);
			printf("memory_clock: %d\n", memory_clock);
		}

		ADL_IF(ADL_Adapter_ASICFamilyType_Get(ai[i].iAdapterIndex, &asic_type, &valids))
		{
			printf("asic_type:    0x%x\n", asic_type);
			printf("valids:       0x%x\n", valids);
		}

		ADL_IF(ADL_Adapter_MemoryInfo_Get(ai[i].iAdapterIndex, &mem_info))
		{
			printf("memory_size:  %d\n", mem_info.iMemorySize);
			printf("memory_type:  %s\n", mem_info.strMemoryType);
			printf("memory_bw:    %d\n", mem_info.iMemoryBandwidth);
		}

		ADL_IF(ADL_Adapter_Crossfire_Caps(ai[i].iAdapterIndex, &cf_preferred, &cf_num_comb, &cf_combs))
		{
			printf("cf_preferred: %d\n", cf_preferred);
			printf("cf_numb_comb: %d\n", cf_num_comb);
			for (int j = 0; j < cf_num_comb; j++)
			{
				printf("  comb:              %d\n", j);
				printf("  num_link_adapters: %d\n", cf_combs[j].iNumLinkAdapter);
				printf("  adapt_link:        { %d, %d, %d }\n", cf_combs[j].iAdaptLink[0], cf_combs[j].iAdaptLink[1], cf_combs[j].iAdaptLink[2]);
			}
		}

		ADL_IF(ADL_Display_DisplayInfo_Get(ai[i].iAdapterIndex, &num_displays, &display_info, 1))
		{
			for (int j = 0; j < num_displays; j++)
			{
				printf("display %d:\n", j);
				printf("  logical_index:  %d\n", display_info[j].displayID.iDisplayLogicalIndex);
				printf("  physical_index: %d\n", display_info[j].displayID.iDisplayPhysicalIndex);
				printf("  log_adap_index: %d\n", display_info[j].displayID.iDisplayLogicalAdapterIndex);
				printf("  phy_adap_index: %d\n", display_info[j].displayID.iDisplayPhysicalAdapterIndex);
				printf("  controller_idx: %d\n", display_info[j].iDisplayControllerIndex);
				printf("  display_name:   %s\n", display_info[j].strDisplayName);
				printf("  manufacturer:   %s\n", display_info[j].strDisplayManufacturerName);
				printf("  display_type:   %d\n", display_info[j].iDisplayType);
				printf("  display_output: %d\n", display_info[j].iDisplayOutputType);
				printf("  connector:      %d\n", display_info[j].iDisplayConnector);
				printf("  info_mask:      0x%x\n", display_info[j].iDisplayInfoMask);
				printf("  info:           0x%x\n", display_info[j].iDisplayInfoValue);
			}
		}

		putchar('\n');
	}

	assert(ADL_OK == ADL_Main_Control_Destroy());

	return 0;
}
