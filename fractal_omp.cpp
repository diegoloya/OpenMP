/*
 Fractal code for CS 4380 / CS 5351
 
 Copyright (c) 2016, Texas State University. All rights reserved.
 
 Redistribution in source or binary form, with or without modification,
 is not permitted. Use in source and binary forms, with or without
 modification, is only permitted for academic use in CS 4380 or CS 5351
 at Texas State University.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 Author: Martin Burtscher
 editor: Diego Loya
 */

#include <cstdlib>
#include <sys/time.h>
#include "cs43805351.h"
#include <math.h>


static const double Delta = 0.005491;
static const double xMid = 0.745796;
static const double yMid = 0.105089;

int main(int argc, char *argv[])
{
    
    printf("Fractal v1.5 [OpenMP]\n");
    
    // check command line
//#4 (update usage message)
    if (argc != 4) {fprintf(stderr, "usage: %s frame_width num_frames num_threads\n", argv[0]); exit(-1);}
    int width = atoi(argv[1]);
    if (width < 10) {fprintf(stderr, "error: frame_width must be at least 10\n"); exit(-1);}
    int frames = atoi(argv[2]);
    if (frames < 1) {fprintf(stderr, "error: num_frames must be at least 1\n"); exit(-1);}
//#4
    int thread_count = atoi(argv[3]);
//#5
    if (thread_count<1) {fprintf(stderr, "error: number of threads needs to be at least 1\n"), exit(-1);}
//#6
    printf("Number of threads: %d", thread_count);
    
    
    
    
    
    printf("computing %d frames of %d by %d fractal\n", frames, width, width);
    
    // allocate picture array
    unsigned char* pic = new unsigned char[frames * width * width];
    
    // start time
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // compute frames
    double delta = Delta;
//creates threads and assigns tasks with a default schedule
# pragma omp parallel for num_threads(thread_count) default(none) private(delta) shared(frames, pic, width) //#7&8&9
    for (int frame = 0; frame < frames; frame++) {
       // fixes loop dependency
        delta=Delta*0.99*pow(0.99,frame);
        const double xMin = xMid - delta;
        const double yMin = yMid - delta;
        const double dw = 2.0 * delta / width;
        for (int row = 0; row < width; row++) {
            const double cy = -yMin - row * dw;
            for (int col = 0; col < width; col++) {
                const double cx = -xMin - col * dw;
                double x = cx;
                double y = cy;
                int depth = 256;
                double x2, y2;
                do {
                    x2 = x * x;
                    y2 = y * y;
                    y = 2 * x * y + cy;
                    x = x2 - y2 + cx;
                    depth--;
                } while ((depth > 0) && ((x2 + y2) < 5.0));
                pic[frame * width * width + row * width + col] = (unsigned char)depth;
            }
        }
    }
    
    // end time
    gettimeofday(&end, NULL);
    double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
    printf("compute time: %.4f s\n", runtime);
    
    // verify result by writing frames to BMP files
    if ((width <= 400) && (frames <= 30)) {
        for (int frame = 0; frame < frames; frame++) {
            char name[32];
            sprintf(name, "fractal%d.bmp", frame + 1000);
            writeBMP(width, width, &pic[frame * width * width], name);
        }
    }
    
    delete [] pic;
    return 0;
}


