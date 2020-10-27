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
#include <d3d9.h>
#include <stdio.h>

int main()
{
	D3DFORMAT formats[] = { D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5,   D3DFMT_A2R10G10B10 };
	IDirect3D9 *d3d;
	int adapter_count;

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	adapter_count = d3d->GetAdapterCount();

	for (int i = 0; i < adapter_count; i++)
	{
		D3DADAPTER_IDENTIFIER9 ident;
		d3d->GetAdapterIdentifier(i, 0, &ident);

		printf("adapter %d \"%s\":\n", i, ident.Description);

		for (int j = 0; j < ARRAYSIZE(formats); j++) {
			int mode_count = d3d->GetAdapterModeCount(i, formats[j]);
			printf("  format %d mode count %d\n", formats[j], mode_count);
		}
	}
}
