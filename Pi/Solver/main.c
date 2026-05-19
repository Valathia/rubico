#include "cube.h"
#include "solver.h"
#include "solution.h"
#include "config.h"

int main(int argc, char** argv) {
    Cube cube_arr[2];
    Solution sol = {.length=0};

    if(argc < 2) {
        printf("Usage: %s \"your string\"\n", argv[0]);
        return 1;
    }

    #if TEST
        const char* scramble = argv[1];
        const char* cube_string = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
        parse_cube(cube_arr,cube_string);
        #ifdef SOLVE
            print_cube(&cube_arr[0]);
        #endif 
        apply_alg(cube_arr,NULL,scramble);
    #else 
        const char* cube_string = argv[1];
        parse_cube(cube_arr,cube_string);
    #endif
    
    #ifdef SOLVE
        print_cube(&cube_arr[0]);
    #endif 
    valid_cube_config(&cube_arr[0]);
    solve(cube_arr,&sol);  
    
    return EXIT_SUCCESS;
}

