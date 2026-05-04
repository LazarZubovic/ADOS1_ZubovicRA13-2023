#include "JPEG.h"
#include "NxNDCT.h"
#include <math.h>

#include "JPEGBitStreamWriter.h"


#define DEBUG(x) do{ qDebug() << #x << " = " << x;}while(0)



// quantization tables from JPEG Standard, Annex K
uint8_t QuantLuminance[8*8] =
    { 16, 11, 10, 16, 24, 40, 51, 61,
      12, 12, 14, 19, 26, 58, 60, 55,
      14, 13, 16, 24, 40, 57, 69, 56,
      14, 17, 22, 29, 51, 87, 80, 62,
      18, 22, 37, 56, 68,109,103, 77,
      24, 35, 55, 64, 81,104,113, 92,
      49, 64, 78, 87,103,121,120,101,
      72, 92, 95, 98,112,100,103, 99 };
uint8_t QuantChrominance[8*8] =
    { 17, 18, 24, 47, 99, 99, 99, 99,
      18, 21, 26, 66, 99, 99, 99, 99,
      24, 26, 56, 99, 99, 99, 99, 99,
      47, 66, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99, 99, 99, 99, 99 };

static char quantizationMatrix[64] =
{
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

struct imageProperties{
    int width;
    int height;
    int16_t* coeffs;
};


void DCTUandV(const char input[], int16_t output[], int N, double* DCTKernel)
{
    double* temp = new double[N*N];
    double* DCTCoefficients = new double[N*N];

    double sum;
    for (int i = 0; i <= N - 1; i++)
    {
        for (int j = 0; j <= N - 1; j++)
        {
            sum = 0;
            for (int k = 0; k <= N - 1; k++)
            {
                sum = sum + DCTKernel[i*N+k] * (input[k*N+j]);
            }
            temp[i*N + j] = sum;
        }
    }

    for (int i = 0; i <= N - 1; i++)
    {
        for (int j = 0; j <= N - 1; j++)
        {
            sum = 0;
            for (int k = 0; k <= N - 1; k++)
            {
                sum = sum + temp[i*N+k] * DCTKernel[j*N+k];
            }
            DCTCoefficients[i*N+j] = sum;
        }
    }

    for(int i = 0; i < N*N; i++)
    {
        output[i] = floor(DCTCoefficients[i]+0.5);
    }

    delete[] temp;
    delete[] DCTCoefficients;

    return;
}

uint8_t quantQuality(uint8_t quant, uint8_t quality) {
    // Convert to an internal JPEG quality factor, formula taken from libjpeg
    int16_t q = quality < 50 ? 5000 / quality : 200 - quality * 2;
    return clamp((quant * q + 50) / 100, 1, 255);
}

static void doZigZag(int16_t block[], uint8_t quantizationBlock[], int N, int DCTorQuantization)
{
    // TO DO
    int currDiagonalWidth = 1;
    int currNum = 0;
    int row, col;
    //gornji deo
    while(currDiagonalWidth < N){
        for(int i = 0; i < currDiagonalWidth; i++){
            if(currDiagonalWidth % 2 == 1){
                row = currDiagonalWidth - i - 1;
                col = i;
            } else {
                row = i;
                col = currDiagonalWidth - i - 1;
            }

            if(DCTorQuantization == 0)
                quantizationBlock[currNum++] = (uint8_t)block[row*N + col];
            else
                ((int16_t*)quantizationBlock)[currNum++] = block[row*N + col];
        }
        currDiagonalWidth++;
    }

    //donji deo
    while(currDiagonalWidth > 0){
        for(int i = currDiagonalWidth; i > 0; i--){
            if(currDiagonalWidth % 2 == 1){
                row = N - currDiagonalWidth + i - 1;
                col = N - i;
            } else {
                row = N - i;
                col = N - currDiagonalWidth + i - 1;
            }

            if(DCTorQuantization == 0)
                quantizationBlock[currNum++] = (uint8_t)block[row*N + col];
            else
                ((int16_t*)quantizationBlock)[currNum++] = block[row*N + col];
        }
        currDiagonalWidth--;
    }
}

/* perform DCT */
imageProperties performDCT(char input[], int xSize, int ySize, int N, uint8_t quality, bool quantType)
{
    // TO DO
    double* DCTKernel = new double[N*N];
    GenerateDCTmatrix(DCTKernel, N);

    int16_t* coeffs = new int16_t[xSize * ySize];

    uchar inBlock[64];
    int16_t dctBlock[64];

    for(int y = 0; y < ySize/N; y++){
        for(int x = 0; x < xSize/N; x++){

            for(int j=0; j<N; j++)
                for(int i=0; i<N; i++)
                {
                    if(quantType) // Y komponenta
                        inBlock[j*N+i] = (char)(input[(y*N+j)*xSize + (x*N+i)] - 128);
                    else
                        inBlock[j*N+i] = input[(y*N+j)*xSize + (x*N+i)];
                }

            if(quantType)
                DCT((char*)inBlock, dctBlock, N, DCTKernel);
            else
                DCTUandV((char*)inBlock, dctBlock, N, DCTKernel);

            for(int i = 0; i < N*N; i++){
                uint8_t Q;
                if(quantType)
                    Q = quantQuality(QuantLuminance[i], quality);
                else
                    Q = quantQuality(QuantChrominance[i], quality);

                dctBlock[i] = (int16_t)round((double)dctBlock[i] / Q);
            }

            for(int j=0; j<N; j++){
                for(int i=0; i<N; i++){
                    coeffs[(y*N+j)*xSize + (x*N+i)] =
                        dctBlock[j*N+i];
                }
            }
        }
    }

    imageProperties img;
    img.width = xSize;
    img.height = ySize;
    img.coeffs = coeffs;

    delete[] DCTKernel;

    return img;
}

//JPEGBitStreamWriter streamer("example.jpg");
void performJPEGEncoding(uchar Y_buff[], char U_buff[], char V_buff[], int xSize, int ySize, int quality)
{
    DEBUG(quality);

    auto s = new JPEGBitStreamWriter("example.jpg");

    s->writeHeader();

    uint8_t qL[64], qC[64];
    int16_t tmpL[64], tmpC[64];

    for(int i=0;i<64;i++){
        tmpL[i] = quantQuality(QuantLuminance[i], quality);
        tmpC[i] = quantQuality(QuantChrominance[i], quality);
    }

    doZigZag(tmpL, qL, 8, 0);
    doZigZag(tmpC, qC, 8, 0);

    s->writeQuantizationTables(qL, qC);

    s->writeImageInfo(xSize, ySize);
    s->writeHuffmanTables();

    char* Y_signed = new char[xSize * ySize];

    for(int i = 0; i < xSize*ySize; i++)
        Y_signed[i] = (char)(Y_buff[i]);

    auto Y = performDCT(Y_signed, xSize, ySize, 8, quality, true);

    delete[] Y_signed;
    auto U = performDCT(U_buff, xSize/2, ySize/2, 8, quality, false);
    auto V = performDCT(V_buff, xSize/2, ySize/2, 8, quality, false);

    int16_t block[64];
    int16_t zz[64];

    for(int y = 0; y < ySize/16; y++){
        for(int x = 0; x < xSize/16; x++){

            for(int by=0; by<2; by++){
                for(int bx=0; bx<2; bx++){

                    for(int j=0;j<8;j++){
                        for(int i=0;i<8;i++){
                            block[j*8+i] =
                                Y.coeffs[(y*16 + by*8 + j)*xSize + (x*16 + bx*8 + i)];
                        }
                    }

                    doZigZag(block, (uint8_t*)zz, 8, 1);
                    s->writeBlockY(zz);
                }
            }

            for(int j=0;j<8;j++){
                for(int i=0;i<8;i++){
                    block[j*8+i] =
                        U.coeffs[(y*8 + j)*(xSize/2) + (x*8 + i)];
                }
            }

            doZigZag(block, (uint8_t*)zz, 8, 1);
            s->writeBlockU(zz);

            for(int j=0;j<8;j++){
                for(int i=0;i<8;i++){
                    block[j*8+i] =
                        V.coeffs[(y*8 + j)*(xSize/2) + (x*8 + i)];
                }
            }

            doZigZag(block, (uint8_t*)zz, 8, 1);
            s->writeBlockV(zz);
        }
    }

    s->finishStream();
}
