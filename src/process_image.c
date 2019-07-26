#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    int xf = (x < 0) ? 0 : ((x > im.w) ? im.w-1 : x);
    int yf = (y < 0) ? 0 : ((y > im.h) ? im.h-1 : y);
    int cf = (c < 0) ? 0 : ((c > im.c) ? im.c-1 : c);
    
    return im.data[xf + yf*im.w + cf*im.w*im.h];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // TODO Fill this in
    if ( !(x < 0 || x > im.w || y < 0 || y > im.h || c < 0 || c > im.c) ){
         im.data[x + y*im.w + c*im.w*im.h] = v;
    }
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    copy.h = im.h;
    copy.w = im.w;
    copy.c = im.c;
    memcpy(copy.data, im.data, im.w*im.h*im.c*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    gray.w = im.w;
    gray.h = im.h;
    gray.c = 1;
    
    /* VERSÃO DIRETA (VETORIAL)
    for (int p=0; p<im.h*im.w; p++){
        gray.data[p] = im.data[p]*.299 + im.data[p + im.w*im.h]*.587 + im.data[p + im.w*im.h*2]*.114;
    }
    */

    // VERSÃO INDIRETA (MATRICIAL)
    for (int x=0; x<im.w; x++){
        for (int y=0; y<im.h; y++){
            float luma = get_pixel(im,x,y,0)*.299 + get_pixel(im,x,y,1)*.587 + get_pixel(im,x,y,2)*.114;
            set_pixel(gray,x,y,0,luma);
        }
    }

    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    /* VERSÃO DIRETA (VETORIAL)
    for (int p=0; p<im.h*im.w; p++){
        im.data[p + c*im.w*im.h] += v;
    }
    */
   // VERSÃO INDIRETA (MATRICIAL)
    for (int x=0; x<im.w; x++){
        for (int y=0; y<im.h; y++){
            set_pixel(im,x,y,c,get_pixel(im,x,y,c)+v);
        }
    }

}

void clamp_image(image im)
{
    // TODO Fill this in
    /* VERSÃO DIRETA (VETORIAL)
    for (int p=0; p<im.h*im.w*im.c; p++){
        im.data[p] = (im.data[p] < 0) ? 0 : ((im.data[p] > 1 ? 1 : im.data[p])) ;
    }
    */

   // VERSÃO INDIRETA (MATRICIAL)
    for (int x=0; x<im.w; x++){
        for (int y=0; y<im.h; y++){
            for (int c=0; c<im.c; c++){
                float p = get_pixel(im,x,y,c);
                float cp = (p < 0) ? 0 : ((p > 1 ? 1 : p)) ;
                set_pixel(im,x,y,c,cp);
            }            
        }
    }

}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in    
    for (int x=0; x<im.w; x++){
        for (int y=0; y<im.h; y++){

            // GET colors' values
            float R = get_pixel(im,x,y,0);
            float G = get_pixel(im,x,y,1);
            float B = get_pixel(im,x,y,2);

            // GET maximum between 3 colors
            float max_cl = (R>G) ? ((R>B) ? 0 : 2) : ((G>B) ? 1 : 2);

            // VALUE: the maximum between 3 channels
            float max_ch = three_way_max(R,G,B);
            float min_ch = three_way_min(R,G,B);
            float V = max_ch;

            // DELTA min-max: might be handy too
            float C = max_ch - min_ch;
            
            // SATURATION: the ratio between the difference min-max and the max
            float S = (V != 0) ? C/V : 0;

            //HUE: The distance in the color hexagon
            float H1 = 0;
            float H = 0;

            if (C != 0){
                if (max_cl == 0){
                H1 = (G - B)/C;
                }else if (max_cl == 1){
                    H1 = (B - R)/C + 2;
                }else if (max_cl == 2){
                    H1 = (R - G)/C + 4;
                }

                if (H1 < 0){
                    H = H1/6 + 1;
                }else{
                    H = H1/6;
                }

                H = (H<0) ? H + 1 : ((H>1) ? 1 : H);                
            }
                                                  
            set_pixel(im,x,y,0,H); // H - Ok
            set_pixel(im,x,y,1,S); // S - Ok
            set_pixel(im,x,y,2,V); // V - Ok

        }
    }
    
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
    // Algoritmo obtido em https://pt.wikipedia.org/wiki/HSV
      for (int x=0; x<im.w; x++){
        for (int y=0; y<im.h; y++){
            
            // GET colors' values
            float H = get_pixel(im,x,y,0);
            float S = get_pixel(im,x,y,1);
            float V = get_pixel(im,x,y,2);

            float R, G, B;
            
            if (S==0){
                R = V;
                G = V;
                B = V;
            }
            else{
                                
                int H1 = ((int)floor(6*H)) % 6;
                float f = 6*H - H1;
                float p = V*(1 - S);
                float q = V*(1 - f*S);
                float t = V*(1 - (1 - f)*S);

                if (H1==0){
                    R = V;
                    G = t;
                    B = p;
                }
                if (H1==1){
                    R = q;
                    G = V;
                    B = p;
                }
                if (H1==2){
                    R = p;
                    G = V;
                    B = t;
                }
                if (H1==3)
                {
                    R = p;
                    G = q;
                    B = V;
                }
                if (H1==4)
                {
                    R = t;
                    G = p;
                    B = V;
                }
                if (H1==5){
                    R = V;
                    G = p;
                    B = q;
                }                
                
            }

            set_pixel(im,x,y,0,R); 
            set_pixel(im,x,y,1,G); 
            set_pixel(im,x,y,2,B); 

        }
    }
}

void scale_image(image im, int c, float v){
    for (int x=0; x<im.w; x++){
        for (int y=0; y<im.h; y++){
            set_pixel(im,x,y,c, get_pixel(im,x,y,c)*v);         
        }
    }
}
