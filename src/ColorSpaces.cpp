#include "ColorSpaces.h"

void processing_RGB(
    const uchar rgbInputImg[],
    int w,
    int h,
    uchar rgbOutputImg[],
    double R,
    double G,
    double B
    ) {
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            rgbOutputImg[y*w*3+x*3 + 0] = R*rgbInputImg[(y*w + x)*3 + 0];
            rgbOutputImg[y*w*3+x*3 + 1] = G*rgbInputImg[(y*w + x)*3 + 1];
            rgbOutputImg[y*w*3+x*3 + 2] = B*rgbInputImg[(y*w + x)*3 + 2];
        }
    }
}

void RGBtoYUV444(
    const uchar rgbImg[],
    int w,
    int h,
    uchar Y_buff[],
    char U_buff[],
    char V_buff[]
    ) {
    uchar R, G, B;
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            R = rgbImg[(y*w + x)*3 + 0];
            G = rgbImg[(y*w + x)*3 + 1];
            B = rgbImg[(y*w + x)*3 + 2];

            Y_buff[y*w + x] =  0.299*R + 0.587*G + 0.114*B;
            U_buff[y*w + x] =  - 0.14713*R - 0.28886*G + 0.436*B;
            V_buff[y*w + x] =  0.615*R - 0.51499*G - 0.10001*B;
        }
    }
}

void YUV444toRGB(
    const uchar Y_buff[],
    const char U_buff[],
    const char V_buff[],
    int w,
    int h,
    uchar rgbImg[]
    ) {
    double R, G, B;
    double Y, U, V;
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            Y = Y_buff[y*w + x];
            U = U_buff[y*w + x];
            V = V_buff[y*w + x];

            R = Y + 1.13983*V;
            G = Y - 0.39465*U - 0.58060*V;
            B = Y + 2.03211*U;

            if(R < 0){
                R = 0;
            }else if(R > 255){
                R = 255;
            }
            if(G < 0){
                G = 0;
            }else if(G > 255){
                G = 255;
            }
            if(B < 0){
                B = 0;
            }else if(B > 255){
                B = 255;
            }

            rgbImg[(y*w + x)*3 + 0] = R;
            rgbImg[(y*w + x)*3 + 1] = G;
            rgbImg[(y*w + x)*3 + 2] = B;

        }
    }
}

void procesing_YUV444(
    uchar Y_buff[],
    char U_buff[],
    char V_buff[],
    int w,
    int h,
    double Y,
    double U,
    double V
    ) {
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            Y_buff[y*w + x] *= Y;
            U_buff[y*w + x] *= U;
            V_buff[y*w + x] *= V;
        }
    }
}

void RGBtoYUV422(
    const uchar rgbImg[],
    int w,
    int h,
    uchar Y_buff[],
    char U_buff[],
    char V_buff[]
    ) {
    uchar R, G, B;
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x += 2){
            U_buff[y*w/2 + x/2] = 0;
            V_buff[y*w/2 + x/2] = 0;

            for(int xb = 0; xb < 2; xb++){
                R = rgbImg[(y*w + x+xb)*3 + 0];
                G = rgbImg[(y*w + x+xb)*3 + 1];
                B = rgbImg[(y*w + x+xb)*3 + 2];

                Y_buff[y*w + x+xb] =  0.299*R + 0.587*G + 0.114*B;

                U_buff[y*w/2 + x/2] +=  (- 0.14713*R - 0.28886*G + 0.436*B)/2;
                V_buff[y*w/2 + x/2] +=  (0.615*R - 0.51499*G - 0.10001*B)/2;
            }
        }
    }
}

void YUV422toRGB(
    const uchar Y_buff[],
    const char U_buff[],
    const char V_buff[],
    int w,
    int h,
    uchar rgbImg[]
    ) {
    double R, G, B;
    double Y, U, V;
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x += 2){
            U = U_buff[y*w/2 + x/2];
            V = V_buff[y*w/2 + x/2];

            for(int xb = 0; xb < 2; xb++){
                Y = Y_buff[y*w + x+xb];

                R = Y + 1.13983*V;
                G = Y -0.39465*U - 0.58060*V;
                B = Y + 2.03211*U;

                if(R < 0){
                    R = 0;
                }else if(R > 255){
                    R = 255;
                }
                if(G < 0){
                    G = 0;
                }else if(G > 255){
                    G = 255;
                }
                if(B < 0){
                    B = 0;
                }else if(B > 255){
                    B = 255;
                }

                rgbImg[(y*w + x+xb)*3 + 0] = R;
                rgbImg[(y*w + x+xb)*3 + 1] = G;
                rgbImg[(y*w + x+xb)*3 + 2] = B;
            }
        }
    }
}

void procesing_YUV422(
    uchar Y_buff[],
    char U_buff[],
    char V_buff[],
    int w,
    int h,
    double Y,
    double U,
    double V
    ) {
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x += 2){
            for(int xb = 0; xb < 2; xb++){
                Y_buff[y*w + x+xb] *= Y;
            }
            U_buff[y*w/2 + x/2] *= U;
            V_buff[y*w/2 + x/2] *= V;
        }
    }
}

void RGBtoYUV420(
    const uchar rgbImg[],
    int w,
    int h,
    uchar Y_buff[],
    char U_buff[],
    char V_buff[]
    ) {
    uchar R, G, B;
    double U, V;

    for(int y = 0; y < h; y += 2){
        for(int x = 0; x < w; x += 2){
            U = 0;
            V = 0;
            for(int yb = 0; yb < 2; yb++){
                for(int xb = 0; xb < 2; xb++){
                    R = rgbImg[((y+yb)*w + (x+xb))*3 + 0];
                    G = rgbImg[((y+yb)*w + (x+xb))*3 + 1];
                    B = rgbImg[((y+yb)*w + (x+xb))*3 + 2];

                    Y_buff[(y+yb)*w + (x+xb)] =  0.299*R + 0.587*G + 0.114*B;
                    U += (- 0.14713*R - 0.28886*G + 0.436*B);
                    V += (0.615*R - 0.51499*G - 0.10001*B);
                }
            }
            U_buff[(y/2)*w/2 + x/2] =  U/4;
            V_buff[(y/2)*w/2 + x/2] =  V/4;
        }
    }
}

void YUV420toRGB(
    const uchar Y_buff[],
    const char U_buff[],
    const char V_buff[],
    int w,
    int h,
    uchar rgbImg[]
    ) {
    uchar R, G, B;
    double Y, U, V;

    for(int y = 0; y < h; y += 2){
        for(int x = 0; x < w; x += 2){
            U = U_buff[(y/2)*w/2 + x/2];
            V = V_buff[(y/2)*w/2 + x/2];

            for(int yb = 0; yb < 2; yb++){
                for(int xb = 0; xb < 2; xb++){
                    Y = Y_buff[(y+yb)*w + (x+xb)];

                    R = Y + 1.13983*V;
                    G = Y -0.39465*U - 0.58060*V;
                    B = Y + 2.03211*U;

                    if(R < 0){
                        R = 0;
                    }else if(R > 255){
                        R = 255;
                    }
                    if(G < 0){
                        G = 0;
                    }else if(G > 255){
                        G = 255;
                    }
                    if(B < 0){
                        B = 0;
                    }else if(B > 255){
                        B = 255;
                    }

                    rgbImg[((y+yb)*w + (x+xb))*3 + 0] = R;
                    rgbImg[((y+yb)*w + (x+xb))*3 + 1] = G;
                    rgbImg[((y+yb)*w + (x+xb))*3 + 2] = B;
                }
            }
        }
    }
}

void procesing_YUV420(
    uchar Y_buff[],
    char U_buff[],
    char V_buff[],
    int w,
    int h,
    double Y,
    double U,
    double V
    ) {
    for(int y = 0; y < h; y += 2){
        for(int x = 0; x < w; x += 2){
            for(int yb = 0; yb < 2; yb++){
                for(int xb = 0; xb < 2; xb++){
                    Y_buff[(y+yb)*w + (x+xb)] *= Y;
                }
            }

            U_buff[(y/2)*w/2 + x/2] *= U;
            V_buff[(y/2)*w/2 + x/2] *= V;
        }
    }
}

void decimate_Y(
    uchar Y_buff[],
    int w,
    int h
    ) {
    uchar Y;
    int YY;
    for(int y = 0; y < h; y += 2){
        for(int x = 0; x < w; x += 2){
            YY = 0;
            for(int yb = 0; yb < 2; yb++){
                for(int xb = 0; xb < 2; xb++){
                    YY += Y_buff[(y+yb)*w + (x+xb)];
                }
            }
            Y = YY >> 2;

            for(int yb = 0; yb < 2; yb++){
                for(int xb = 0; xb < 2; xb++){
                    Y_buff[(y+yb)*w + (x+xb)] = Y;
                }
            }
        }
    }
}
