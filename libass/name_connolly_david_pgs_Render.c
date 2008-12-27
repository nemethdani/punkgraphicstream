/*
 * ass_jni.c
 *
 * Copyright 2008 David Connolly. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
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

#include "name_connolly_david_pgs_Render.h"
#include "ass.h"
#include "stdlib.h"
#include <string.h>

int loaded = 0;

ass_library_t* ass_library;
ass_renderer_t* ass_renderer;
ass_track_t* ass_track;

JNIEXPORT void JNICALL Java_name_connolly_david_pgs_Render_openSubtitle
(JNIEnv * env, jobject obj, jstring filename) 
{
	ass_library = ass_library_init();
	
	if (!ass_library) {
		// FIXME: throw exception
		printf("not ass_library");
		return ;
	}
	
	ass_renderer = ass_renderer_init(ass_library);
	
	if (!ass_renderer) {
		printf("not ass_renderer");
		// FIXME: throw exception
	}
	
	ass_set_fonts_dir(ass_library, "/Users/slarti/Development/blu-ray subtitles/code/");
	
	ass_set_font_scale(ass_renderer, 1.);
	ass_set_fonts(ass_renderer, NULL, "Arial");
	ass_set_frame_size(ass_renderer, 1920, 1080);
	
	//(ass_library_t* library, char* fname, char* codepage);
	ass_track = ass_read_file(ass_library, "/Users/slarti/Development/blu-ray subtitles/code/subs.ass", "UTF-8");
	
	if (!ass_track) {
		printf("not ass_renderer");
		// FIXME: throw exception
	}
	
	return;
}

JNIEXPORT void JNICALL Java_name_connolly_david_pgs_Render_closeSubtitle
(JNIEnv * env, jobject obj) 
{
	return;
}

JNIEXPORT jint JNICALL Java_name_connolly_david_pgs_Render_getEventCount
(JNIEnv * env, jobject obj)
{	
	return ass_track->n_events;
}

JNIEXPORT jobject JNICALL Java_name_connolly_david_pgs_Render_getEventDuration
(JNIEnv * env, jobject obj, jint event)
{
	jclass durationClass;
	jmethodID cid;
	jobject result;
	durationClass = (*env)->FindClass(env, "name/connolly/david/pgs/SubtitleEvent");
	
	if (durationClass == NULL) {
		return NULL; /* exception thrown */
	}
	/* Get the method ID for the String(char[]) constructor */
	cid = (*env)->GetMethodID(env, durationClass,
							  "<init>", "(JJ)V");
	if (cid == NULL) {
		return NULL; /* exception thrown */
	}
	
	result = (*env)->NewObject(env, durationClass, cid, ass_track->events[event].Start, ass_track->events[event].Duration);
	
	(*env)->DeleteLocalRef(env, durationClass);
	return result;
}

JNIEXPORT void JNICALL Java_name_connolly_david_pgs_Render_render
(JNIEnv * env, jobject obj, jobject image, jlong timecode)
{
	jclass cls = (*env)->GetObjectClass(env, image);
	jmethodID getRGB = (*env)->GetMethodID(env, cls, "getRGB", "(II)I");
	jmethodID setRGB = (*env)->GetMethodID(env, cls, "setRGB", "(III)V");
	
	if (getRGB == NULL) {
		printf("setRGB() not found\n");
		return; /* method not found */
	}
	
	if (setRGB == NULL) {
		printf("setRGB() not found\n");
		return; /* method not found */
	}

	ass_image_t *p_img = ass_render_frame(ass_renderer,
										  ass_track, (long long)(timecode), NULL);
	
	if (!p_img) {
		printf("no image %d\n", timecode);
	}
	
	while (p_img != NULL)
	{ 
		const int r = (p_img->color >> 24)&0xff; 
		const int g = (p_img->color >> 16)&0xff; 
		const int b = (p_img->color >>  8)&0xff; 
		const int a = (p_img->color) & 0xFF;
		
		int x, y; 
		
		for( y = 0; y < p_img->h; y++ ) 
		{ 
			for( x = 0; x < p_img->w; x++ ) 
			{ 
				const int alpha = p_img->bitmap[y*p_img->stride+x]; 
				const int an = (255 - a) * alpha / 255; 
				
				// TYPE_INT_ARGB
				//const int argb = (an << 24) + rgb;
				
				const int old_argb = (*env)->CallIntMethod(env, image, getRGB, x + p_img->dst_x, y + p_img->dst_y);
				int new_argb = 0;
				const int old_a = (old_argb >> 24) & 0xFF;
				const int old_r = (old_argb >> 16) & 0xFF;
				const int old_g = (old_argb >>  8) & 0xFF;
				const int old_b =  old_argb & 0xFF;
				new_argb += (255 - ( 255 - old_a ) * ( 255 - an ) / 255) << 24;
				new_argb += ((( old_r * (255-an) + r * an ) / 255) & 0xFF) << 16;
                new_argb += ((( old_g * (255-an) + g * an ) / 255) & 0xFF) << 8;
                new_argb += ((( old_b * (255-an) + b * an ) / 255) & 0xFF);
				
				(*env)->CallVoidMethod(env, image, setRGB, x + p_img->dst_x, y + p_img->dst_y, new_argb);
			}
		}

		p_img = p_img->next;
	}
}