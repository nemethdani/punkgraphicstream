/*
 * Resolution.java
 *
 * Copyright 2008 David Connolly. All rights reserved.
 *
 * This file is part of PunkGraphicStream.
 *
 * PunkGraphicStream is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
package name.connolly.david.pgs;

public enum Resolution {

    NTSC_480, PAL_576, HD_720, HD_1080;

    public int getWidth() {
        switch (this) {
            case NTSC_480:
            case PAL_576:
                return 720;
            case HD_720:
                return 1280;
            case HD_1080:
            default:
                return 1920;
        }
    }

    public int getHeight() {
        switch (this) {
            case NTSC_480:
                return 480;
            case PAL_576:
                return 576;
            case HD_720:
                return 720;
            case HD_1080:
            default:
                return 1080;
        }
    }
}
