#include "common.h"
#include "cmath"

bool Init();
void CleanUp();
void Run();
void Mandelbrot();
void MandelbrotH(); //Histogram coloring
double ScaleNum(double n, double minN, double maxN, double min, double max);
void colors();

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;

int screenWidth = 500;
int screenHeight = 500;
long double zoom = 1;
long double inc = 0;
int maxIters = 500;
int resolution = 2;
long double x = 0;
long double y = 0;
bool wp = false;
bool sp = false;
bool ap = false;
bool dp = false;
double N = 2;

vector<vector<double>> IterationCounts; //Used in commented out Histogram coloring
vector<double> NumIterationsPerPixel; //Used in commented out Histogram coloring
vector<vector<int>> pColors;

bool Init() //basic setup
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main() //basic setup
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp() //basic setup
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));
    colors(); //inputs the color wheel in to vector (rgb)

    while (gameLoop)
    {   
        //MandelbrotH();  //Histogram coloring
        Mandelbrot();
        SDL_RenderPresent(renderer);
        SDL_Event event;
        
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    gameLoop = false;
                    break;
                case SDLK_w:
                    wp = true;
                    break;
                case SDLK_s:
                    sp = true;
                    break;
                case SDLK_a:
                    ap = true;
                    break;
                case SDLK_d:
                    dp = true;
                    break;
                case SDLK_e:
                    inc+= .1;
                    zoom = exp(-1 * inc);
                    //maxIters = exp(inc) + 499 * 2;
                    cout << zoom << endl;
                    break;
                case SDLK_q:
                    inc-= .1;
                    zoom = exp(-1 * inc);
                    //maxIters = exp(inc) + 499 * 2;
                    cout << zoom << endl;
                    break;
                case SDLK_r:
                    maxIters += 100;
                    break;
                case SDLK_f:
                    maxIters -= 100;
                    if(maxIters < 100)
                        maxIters = 100;
                    break;
                case SDLK_SPACE:
                    if(resolution == 2)
                        resolution = 1;
                    else
                        resolution = 2;
                    break;
                default:
                    break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                default:
                    break;
                }
            }
        }
    }
}

void MandelbrotH(){  //with Histogram coloring
    if(wp){
        y -= exp(-1 * inc)/2;
        wp = false;
    }
    else if(sp){
        y += exp(-1 * inc)/2;
        sp = false;
    }
    else if(ap){
        x -= exp(-1 * inc)/2;
        ap = false;
    }
    else if(dp){
        x += exp(-1 * inc)/2;
        dp = false;
    }
    long double minx = x - zoom;
    long double maxx = x + zoom;
    long double miny = y - zoom;
    long double maxy = y + zoom;

    vector<long double> ys;
    
    for(int x = 0; x < screenWidth; x+=resolution){
        for(int y = 0; y < screenHeight; y+=resolution){
            long double zr = ScaleNum(x, 0, screenWidth, minx, maxx);
            long double zi = ScaleNum(y, 0, screenHeight, miny, maxy);
            long double cr = zr;
            long double ci = zi;

            double n = 0;

            while (n < maxIters) {
                long double aa = zr * zr;
                long double bb = zi * zi;
                
                if (aa + bb > 4.0) {
                    break;  // Bail
                }
                
                long double Nab = N * zr * zi;
                zr = aa - bb + cr;
                zi = Nab + ci;
                n++;
            }
            ys.push_back(n);
        }
        IterationCounts.push_back(ys);
        ys.clear();
    }
    for(int i = 0; i <= maxIters; i++){
        NumIterationsPerPixel.push_back(0);
    }
    for(int x = 0; x < screenWidth; x++){
        for(int y = 0; y < screenHeight; y++){
            long double incre = IterationCounts[x][y];
            NumIterationsPerPixel[incre] = NumIterationsPerPixel[incre] + 1;
        }
    }
    double total = 0;
    for(int i = 0; i <= maxIters; i++){
        total += NumIterationsPerPixel[i];
    }
    for(int x = 0; x < screenWidth; x++){
        for(int y = 0; y < screenHeight; y++){
            int iters = IterationCounts[x][y];
            double color = 0;
            for(int i = 0; i <= iters; i++){
                color += NumIterationsPerPixel[i] / total * 255;
            }
            color = ScaleNum(color, 0, 255, 0, pColors.size() - 1);
            
            SDL_Rect pos;
            pos.x = x;
            pos.y = y;
            pos.w = resolution;
            pos.h = resolution;
            SDL_SetRenderDrawColor(renderer, pColors[color][0], pColors[color][2], pColors[color][1], 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
    IterationCounts.clear();
    NumIterationsPerPixel.clear();
}

void Mandelbrot(){   //Without Histogram coloring
    if(wp){
        y -= exp(-1 * inc)/2;
        wp = false;
    }
    else if(sp){
        y += exp(-1 * inc)/2;
        sp = false;
    }
    else if(ap){
        x -= exp(-1 * inc)/2;
        ap = false;
    }
    else if(dp){
        x += exp(-1 * inc)/2;
        dp = false;
    }
    long double minx = x - zoom;
    long double maxx = x + zoom;
    long double miny = y - zoom;
    long double maxy = y + zoom;

    for(int x = 0; x < screenWidth; x+=resolution){
        for(int y = 0; y < screenHeight; y+=resolution){
            long double zr = ScaleNum(x, 0, screenWidth, minx, maxx);
            long double zi = ScaleNum(y, 0, screenHeight, miny, maxy);
            long double cr = zr;
            long double ci = zi;

            int n = 0;

            while (n < maxIters) {
                long double aa = zr * zr;
                long double bb = zi * zi;
                
                if (aa + bb > 4.0) {
                    break;  // Bail
                }
                
                long double Nab = N * zr * zi;
                zr = aa - bb + cr;
                zi = Nab + ci;
                n++;
            }
            double color = sqrt(static_cast<double>(n) / maxIters);
            color = ScaleNum(color, 0, 1, 0, 255);

            SDL_Rect pos;
            pos.x = x;
            pos.y = y;
            pos.w = resolution;
            pos.h = resolution;
            SDL_SetRenderDrawColor(renderer, color, color, color, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
}
void colors(){ //unused color wheel, map n iterations to a color in the vector
    int r = 255;
    int b = 0;
    int g = 0;
    vector<int> color;

    for(b = 0; b < 255; b++){
        color.push_back(r);
        color.push_back(b);
        color.push_back(g);
        pColors.push_back(color);
        color.clear();
    }
    for(r = 255; r > 0; r--){
        color.push_back(r);
        color.push_back(b);
        color.push_back(g);
        pColors.push_back(color);
        color.clear();
    }
    // for(g = 0; g < 255; g++){
    //     color.push_back(r);
    //     color.push_back(b);
    //     color.push_back(g);
    //     pColors.push_back(color);
    //     color.clear();
    // }
    // for(b = 255; b > 0; b--){
    //     color.push_back(r);
    //     color.push_back(b);
    //     color.push_back(g);
    //     pColors.push_back(color);
    //     color.clear();
    // }
    // for(r = 0; r < 255; r++){
    //     color.push_back(r);
    //     color.push_back(b);
    //     color.push_back(g);
    //     pColors.push_back(color);
    //     color.clear();
    // }
    // for(g = 255; g > 0; g--){
    //     color.push_back(r);
    //     color.push_back(b);
    //     color.push_back(g);
    //     pColors.push_back(color);
    //     color.clear();
    // }
    color.push_back(r);
    color.push_back(b);
    color.push_back(g);
    pColors.push_back(color);
    color.clear();
}

double ScaleNum(double n, double minN, double maxN, double min, double max){
    return (((n - minN) / (maxN - minN)) * (max - min)) + min;
}